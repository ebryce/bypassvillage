// Fob reader pinout
#define pBEP 6  // YELLOW
#define pGRN 4  // ORANGE
#define pRED 5  // BROWN
#define pHLD -1 // BLUE
#define pIO1 -1 // RED/GREEN
#define pIO2 -1 // TAN
#define pIO3 -1 // PINK
#define pIO4 -1 // GRAY
#define pTMP 7  // VIOLET
#define pCLK 3  // WHT
#define iCLK 1  // WHT 3 -> 1
#define pDTA 12 // GREEN

#define pDRS 13 // Door state senor
#define pWLC 11 // Wheelchair button
#define pMAG 2  // Magstrike control

#define msLoop 100            // ms per control loop

#define msWheelchairLim 20000 // ms until wheelchair button is irrelevant
#define msFobHold 500         // ms until we should not cluck again
#define msFobLim 5000         // ms until the Fob reader is irrelevant
#define msUnlockCooldown 1000 // ms until we are allowed to unlock after a lock event

int msWheelchair; // ms left until wheelchair resets
int msFob;        // ms until fob resets
int msSinceLock;  // ms since the door was locked

int lockState;    // 1=door is locked         0=door is unlocked
int doorState;    // 1=door is open           0=door is closed
int fireState;    // 1=building is on fire    0=building is extinguished
int tamperState;  // 1=device is opened       0=device is sealed

int noteBeep; 

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  
  pinMode(pBEP, OUTPUT);  // Beep
  pinMode(pGRN, OUTPUT);  // Green LED
  pinMode(pRED, OUTPUT);  // Red LED
  pinMode(pMAG, OUTPUT);  // Magstrike relay control
  
  pinMode(pTMP, INPUT);   // Tamper evident sensor
  pinMode(pCLK, INPUT);   // Weigand clock
  pinMode(pDTA, INPUT);   // Weigand data
  pinMode(pIO1, INPUT);   // Prox card data
  pinMode(pIO2, INPUT);   // Prox card date
  pinMode(pIO3, INPUT);   // Prox card data
  pinMode(pIO4, INPUT);   // Prox card date
  
  pinMode(pDRS, INPUT);   // Door state
  pinMode(pWLC, INPUT);   // Door state

  digitalWrite(pGRN, HIGH);
  digitalWrite(pRED, HIGH);
  digitalWrite(pBEP, HIGH);

  // Set default states
  fireState = 0;    // No fire
  doorState = 0;    // Door closed
  tamperState = 0;  // No tamper
  lockState = 0;    // Locked
  msFob = 0;        // Fob not active
  msWheelchair = 0; // Wheelchair button not active

  noteBeep = 0;
  
  attachInterrupt(iCLK, unlockFob, FALLING);  // On Fob activation, cluck
}

void blinkenlight(int n=1)
{
  digitalWrite(pRED, HIGH);
  for (int i = 0; i < n; i++)
  {
    // Red + No Beep
    digitalWrite(pGRN, LOW);
    digitalWrite(pBEP, LOW);
    delay(500);
    // Orange + Beep
    digitalWrite(pGRN, HIGH);
    digitalWrite(pBEP, HIGH);
    delay(500);
  }
}

void unlockFob()
{
  if (msSinceLock == 0)
  { // Avoid transient clock registration
    if (msFob < msFobLim) // Avoid duplication
    {
      Serial.println("Fob triggered");
      msFob = msFobLim;
    }
  }
}

void loop() {
  noteBeep=0;

  // Keep track of the time since the last lock event
  msSinceLock -= msLoop;
  if (msSinceLock < 0) {msSinceLock=0;}

  // Keep track of time on the wheelchair button
  if (digitalRead(pWLC)==0)
  { // If the wheelchair button is being pressed
    if (msWheelchair == 0)
      { Serial.println("Wheelchair button pressed"); }
    msWheelchair = msWheelchairLim;
  }
  
  if ((msWheelchair <= msLoop) & (msWheelchair > 0)) { Serial.println("Wheelchair unlock expiring"); }
  msWheelchair -= msLoop;
  if (msWheelchair < 0) {msWheelchair=0;}

  // Keep track of time on the fob
  if ((msFob <= msLoop) & (msFob > 0)){ Serial.println("Fob unlock expiring"); }
  msFob -= msLoop;
  if (msFob < 0) {msFob=0;}

  // Check if the door should be locked
  if (( (msWheelchair > 0) | (msFob > 0) | (fireState > 0) ) 
       & (tamperState==0) // Uncomment to forbid unlocking while tampered
       //& (doorState==0)  // Uncomment to forbid unlocking while door is open
      )
  { // Unlocked
    if (lockState==0) {Serial.println("Unlocking"); noteBeep=1;}  // On transition
    lockState = 1;
    digitalWrite(pGRN, LOW);  // Green on
    digitalWrite(pRED, HIGH); // Red off
    digitalWrite(pBEP, HIGH); // No beep
    digitalWrite(pMAG, LOW); // Unlock
  }  
  else
  { // Locked
    if (lockState==1)
      {Serial.println("Locking"); noteBeep=1; msSinceLock=msUnlockCooldown;
    }  // On transition
    lockState = 0;
    digitalWrite(pGRN, HIGH); // Green off
    digitalWrite(pRED, LOW);  // Red on
    digitalWrite(pBEP, HIGH); // No beep
    digitalWrite(pMAG, HIGH);  // Lock
  }  

  // Check whether the door is open
  // n.b., the switch is 0 when door is open, 1 when door is closed
  // but this program's logic is inverted (1 when open, 0 when closed)
  if (digitalRead(pDRS)==1)
  {
    if (doorState==1){ Serial.println("Door closing");noteBeep = 1; }
    doorState=0;
  } 
  else
  {
    if (doorState==0){
      if (lockState==0) { Serial.println("Door opening but is locked"); }
      else { Serial.println("Door opening and is unlocked"); }
      noteBeep = 1;
    }
    doorState=1;
  }

  if (doorState > lockState)
  { // If the door is forced open
    digitalWrite(pGRN, LOW); // Green on
    digitalWrite(pRED, LOW); // Red on (orange)
    digitalWrite(pBEP, LOW); // Beep continuously
  }

  if(digitalRead(pTMP)==0)
  { // Again, we swap normally open vs normally closed
    // If the backplate has been removed
    if (tamperState==0) {Serial.println("Backplate removed");noteBeep = 1;}
    tamperState = 1;
    digitalWrite(pGRN, LOW); // Green on
    digitalWrite(pRED, LOW); // Red on (orange)
    //digitalWrite(pBEP, LOW); // Beep continuously
  }
  else
  {
    // If the backplate is in place
    if (tamperState==1) {Serial.println("Backplate replaced");noteBeep = 1;}
    tamperState = 0;
  }

  if (noteBeep==1) { digitalWrite(pBEP, LOW); }

  delay(msLoop);
}

// Fob reader
// Contact censor
// Fire alarm
// Wheelchair exit
// Magstrike
