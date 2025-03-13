#include "Adafruit_MAX31865.h"

Adafruit_MAX31865 thermo = Adafruit_MAX31865(18, 19, 22, 23);

#define RREF 430.0
#define RNOMINAL 100.0

void setup() {
  Serial.begin(9600);
  thermo.begin(MAX31865_3WIRE);  // 2WIRE, 3WIRE, 4WIRE 
}  

void loop() {
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x");
    Serial.println(fault, HEX);
    thermo.clearFault();
  } else {
    float temp = thermo.temperature(RNOMINAL, RREF);
    Serial.print("Temp:");
    Serial.println(temp);
  }
  delay(1000);
}
