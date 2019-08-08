/* Calibration sketch for HX711 */
 
#include "HX711.h"  // Library needed to communicate with HX711 https://github.com/bogde/HX711
 
#define DOUT  6  // Arduino pin 6 connect to HX711 DOUT
#define CLK  5  //  Arduino pin 5 connect to HX711 CLK
 
HX711 scale(DOUT, CLK);  // Init of library

void setup() {
  Serial.begin(9600);
  scale.set_scale();  // Start scale
  scale.tare();       // Reset scale to zero
}

void loop() {
  float current_weight=scale.get_units(20);  // get average of 20 scale readings
  float scale_factor=(current_weight/0.145);  // divide the result by a known weight
  Serial.println(scale_factor);  // Print the scale factor to use
}
