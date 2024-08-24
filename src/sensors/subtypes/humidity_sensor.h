#include "sensor.h"

class HumiditySensor : public Sensor {
    public:
        std::string getSensorDataJson() override;
        void setupSensor(uint32_t* delayMS) override;
};