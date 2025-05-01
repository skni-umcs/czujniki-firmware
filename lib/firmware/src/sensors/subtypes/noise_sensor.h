#ifndef NOISE_H
#define NOISE_H

#include "sensor.h"
#include "exchange/transmits/lora_transmit.h"

//TODO: rename to lora sensor
class NoiseSensor : public Sensor {
    std::shared_ptr<LoraTransmit> transmit;
    
    public:
        NoiseSensor(std::shared_ptr<LoraTransmit> transmit);
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
