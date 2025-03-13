#include "HX711.h"

#define DOUT  22  // HX711 Data Pin
#define SCK   23  // HX711 Clock Pin

HX711 scale;
float calibration_factor = 2521;  // Replace with your correct value
const int numReadings = 5;  // Reduce for faster response

void setup() {
  Serial.begin(115200);
  scale.begin(DOUT, SCK);
  scale.set_scale(calibration_factor);
  scale.tare();  // Reset to zero
  Serial.println("Scale Ready.");
}

float getFastWeight() {
  float sum = 0;
  
  for (int i = 0; i < numReadings; i++) {
    sum += scale.get_units(1);  // Get one reading
  }

  return sum / numReadings;  // Return average
}

void loop() {
  float weight = getFastWeight();

  Serial.print("Fast Weight (kg): ");
  Serial.println(weight, 2);

  delay(500);  // Shorter delay for faster readings
}
