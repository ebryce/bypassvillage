#include <Hx711.h>
Hx711 scale1(A2, A3);
Hx711 scale2(A4, A5);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Scale 1 : \t");
  Serial.print(scale1.getGram(), 1);
  Serial.println(" g");
  Serial.print("Scale 2 : \t");
  Serial.print(scale2.getGram(), 1);
  Serial.println(" g");
  delay(200);
}

