#ifndef NOISE_H
#define NOISE_H

#include "sensor.h"
#include "exchange/transmits/loraTransmit.h"

class NoiseSensor : public Sensor {
    std::shared_ptr<LoraTransmit> transmit;
    
    public:
        std::map<std::string, std::string> getSensorData() override;
        void setupSensor(uint32_t* delayMS) override;
        void setLoraTransmit(std::shared_ptr<LoraTransmit> transmit);
};

#endif
