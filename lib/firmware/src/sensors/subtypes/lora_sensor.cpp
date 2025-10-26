#include "lora_sensor.h"

#include <Wire.h>
#include <utils/logger.h>

#include <sstream>

const std::string NOISE_CODE = "n";
const std::string QUEUE_CODE = "q";
const std::string TRANSMITS_CODE = "r";
const std::string COLLISIONS_CODE = "x";

LoraSensor::LoraSensor(std::shared_ptr<LoraTransmit> transmit) {
  this->transmit = transmit;
}

OperationResult LoraSensor::setupSensor() {
  if (transmit == nullptr) {
    Logger::log("Can't read lora data, no lora transmit");
    return OperationResult::NOT_FOUND;
  }
  transmit->updateNoise();
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> LoraSensor::getSensorData() {
  std::map<std::string, std::string> resultMap;
  if (transmit == nullptr) {
    Logger::log("Can't read lora data, no lora transmit");
    return resultMap;
  }
  int noise = transmit->getNoise();
  int collisionPercentage = transmit->getCollisionRate();


  resultMap.insert(std::make_pair(NOISE_CODE, std::to_string(noise)));
  resultMap.insert(std::make_pair(
      QUEUE_CODE, std::to_string(transmit->getWaitingMessagesCount())));
  resultMap.insert(std::make_pair(
      TRANSMITS_CODE, std::to_string(transmit->getTransmitCount())));
  resultMap.insert(std::make_pair(
      COLLISIONS_CODE, std::to_string(collisionPercentage)));
  return resultMap;
}
