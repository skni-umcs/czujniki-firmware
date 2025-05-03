#ifndef NOISE_H
#define NOISE_H

#include "sensor.h"
#include "exchange/transmits/lora_transmit.h"

class LoraSensor : public Sensor {
    std::shared_ptr<LoraTransmit> transmit;
    
    public:
        LoraSensor(std::shared_ptr<LoraTransmit> transmit);
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
