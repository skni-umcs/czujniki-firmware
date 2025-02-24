#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "noise_sensor.h"
#include <sstream>

void NoiseSensor::setupSensor(uint32_t* delayMS) {
  if(transmit == nullptr) {
    Serial.println("Can't read noise, no lora transmit");
    return;
  }
  transmit->updateNoise();
}

std::string NoiseSensor::getSensorData() {

  if(transmit == nullptr) {
    Serial.println("Can't read noise, no lora transmit");
    return NO_NOISE_MESSAGE;
  }
  transmit->updateNoise();

  int noise = transmit->getNoise();

  std::stringstream result;
  result << "noise: " << noise;
  return result.str();
}

void NoiseSensor::setLoraTransmit(std::shared_ptr<LoraTransmit> transmit) {
  this->transmit = transmit;
}