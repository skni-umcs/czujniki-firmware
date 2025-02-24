#ifndef NOISE_H
#define NOISE_H

#include "sensor.h"
#include "exchange/transmits/loraTransmit.h"

class NoiseSensor : public Sensor {
    std::shared_ptr<LoraTransmit> transmit;

    const std::string NO_NOISE_MESSAGE = "NoNoise";
    
    public:
        std::string getSensorData() override;
        void setupSensor(uint32_t* delayMS) override;
        void setLoraTransmit(std::shared_ptr<LoraTransmit> transmit);
};

#endif
