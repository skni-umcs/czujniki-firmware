#include <Wire.h>
#include "lora_sensor.h"
#include <sstream>
#include <utils/logger.h>

const std::string NOISE_CODE = "n";
const std::string QUEUE_CODE = "q";
const std::string TRANSMITS_CODE = "r";

LoraSensor::LoraSensor(std::shared_ptr<LoraTransmit> transmit) {
  this->transmit = transmit;
}

OperationResult LoraSensor::setupSensor() {
  if(transmit == nullptr) {
    Logger::log("Can't read lora data, no lora transmit");
    return OperationResult::NOT_FOUND;
  }
  transmit->updateNoise();
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> LoraSensor::getSensorData() {
  std::map<std::string, std::string> resultMap;
  if(transmit == nullptr) {
    Logger::log("Can't read lora data, no lora transmit");
    return resultMap;
  }
  int noise = transmit->getNoise();
  
  resultMap.insert(std::make_pair(NOISE_CODE, std::to_string(noise))); 
  resultMap.insert(std::make_pair(QUEUE_CODE, std::to_string(transmit->getWaitingMessagesCount())));
  resultMap.insert(std::make_pair(TRANSMITS_CODE, std::to_string(transmit->getTransmitCount())));

  return resultMap;
}
