// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "sensors/humidity_sensor.h"

uint32_t delayMS;

HumiditySensor s;

void setup() {
  Serial.begin(9600);
  s = HumiditySensor();
  s.setupSensor(&delayMS);
}

void loop() {
  // Delay between measurements.
  delay(delayMS);
  Serial.print(s.getSensorDataJson().c_str());
}