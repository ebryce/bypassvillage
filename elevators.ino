/* Floor buttons
  - SB
  - B
  - G
  - 2
  - 3
  - 4

  Additional inputs:
   - Attendant key
   - Normal/inspection cartop
   - Run/stop cartop
   - Up cartop
   - Down cartop

   Outputs (solid=call registered; flashing=arrived)
  - SB
  - B
  - G
  - 2
  - 3
  - 4
*/

// ELevator call buttons
#define pCallSB
#define pCallB
#define pCallG
#define pCall2
#define pCall3
#define pCall4

// Service buttons
#define pSvcAttKey
#define pSvcInspMode
#define pSvcStop
#define pSvcUp
#define pSvcDown

// Lights on the elevator buttons
#define pLtSB
#define pLtB
#define pLtG
#define pLt2
#define pLt3
#define pLt4

// ELevator control logic
#define waitingFloor 0    // The floor the elevator should wait on

// Arduino control/simulation logic
#define loopDelayMs 100   // Time to offset in each loop
#define driveDelayMs 2000 // Time to go between floors

// Runtime params
float position;  // The floor the elevator is currently on
int velocity;    // The velocity of the elvator (-1 is descending, etc.)
int next;        // The floor we should be headed to

// Has a particular elevator button been pressed?
// Reset when the elevator reaches a floor
bool calledSB;// -2
bool calledB; // -1
bool calledG; //  0
bool called2; //  1
bool called3; //  2
bool called4; //  3

// Computed params
float maxSpeed; // The speed the elevator should travel at

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting");

  pinMode(pCallSB, INPUT);
  pinMode(pCallSB, INPUT);
  pinMode(pCallB, INPUT);
  pinMode(pCallG, INPUT);
  pinMode(pCall2, INPUT);
  pinMode(pCall3, INPUT);
  pinMode(pCall4, INPUT);

  pinMode(pSvcAttKey, INPUT);
  pinMode(pSvcInspMode, INPUT);
  pinMode(pSvcStop, INPUT);
  pinMode(pSvcUp, INPUT);
  pinMode(pSvcDown, INPUT);

  pinMode(pLtSB, OUTPUT);
  pinMode(pLtB, OUTPUT);
  pinMode(pLtG, OUTPUT);
  pinMode(pLt2, OUTPUT);
  pinMode(pLt3, OUTPUT);
  pinMode(pLt4, OUTPUT);

  position = waitingFloor;
  next = waitingFloor;
  velocity = 0;

  // Comute the ideal max speed
  // i.e., floor transitions per loop
  maxSpeed = loopDelayMs/driveDelayMs;
}

void loop()
{

  float distance;           // The distance to the next stop
  float distanceInNextStep; // Where we will go next

  // The distance is the difference between where we are now
  //  and where we want to be
  distance = next-position;

  // Find the direction of travel in the next step
  velocity = distance/abs(distance);
  distanceInNextStep = maxSpeed*velocity;

  // Now, go there
  position += distanceInNextStep;

  // Deal with resetting the buttons once a floor is reached
  switch (position) {
    case -2:  // Sub-basement
      calledSB = false;
      break;
    case 1:  // Sub-basement
      calledB = false;
      break;
    case 0:  // Sub-basement
      calledG = false;
      break;
    case 1:  // Sub-basement
      called2 = false;
      break;
    case 2:  // Sub-basement
      called3 = false;
      break;
    case 3:  // Sub-basement
      called4 = false;
      break;
}
