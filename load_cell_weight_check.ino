#include "HX711.h"

#define DOUT  22  // Change to your actual GPIO pin
#define SCK   23  // Change to your actual GPIO pin

HX711 scale;
float calibration_factor = 2521;  // Replace this with your calculated value

void setup() {
  Serial.begin(115200);
  scale.begin(DOUT, SCK);
  scale.set_scale(calibration_factor);
  scale.tare();  // Reset to zero

  Serial.println("Scale Ready.");
}

void loop() {
  Serial.print("Weight (kg): ");
  Serial.println(scale.get_units(5)+1, 2); // Take average of 5 readings
  delay(1000);
}
