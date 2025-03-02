#ifndef NOISE_H
#define NOISE_H

#include "sensor.h"
#include "exchange/transmits/loraTransmit.h"

class NoiseSensor : public Sensor {
    std::shared_ptr<LoraTransmit> transmit;
    
    public:
        NoiseSensor(std::shared_ptr<LoraTransmit> transmit);
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
