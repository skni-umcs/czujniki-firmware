#ifndef BME680_H
#define BME680_H

#include "sensor.h"
#include <Adafruit_BME680.h>

const std::string GAS_CODE = "g";
#define GAS_PRECISION 1;

class BME680Sensor : public Sensor {
    Adafruit_BME680 bme = Adafruit_BME680(&Wire);
    public:
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
