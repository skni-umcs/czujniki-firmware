#ifndef BME280_H
#define BME280_H

#include "sensor.h"
#include <Adafruit_BME280.h>

class BME280Sensor : public Sensor {
    Adafruit_BME280 bme;
    public:
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
