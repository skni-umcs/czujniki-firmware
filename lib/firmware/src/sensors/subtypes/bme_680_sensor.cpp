#include "bme_680_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <string>
#include <sstream>
#include <iostream>
#include <Adafruit_BME680.h>
#include "bme_constants.h"
#include <utils/logger.h>

#if defined(esp32firebeetle)
  #define I2C_SDA 21
  #define I2C_SCL 22
#else
  #define I2C_SDA 8
  #define I2C_SCL 9
#endif

OperationResult BME680Sensor::setupSensor() {
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!bme.begin()) {
    Logger::log("Could not find a valid BME680 sensor!");
    return OperationResult::ERROR;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> BME680Sensor::getSensorData() {
  sensors_event_t event;
  std::stringstream result;

  int humidity = bme.readHumidity()*HUMIDITY_PRECISION;
  int temperature = bme.readTemperature()*TEMPERATURE_PRECISION;
  int pressure = bme.readPressure()/PRESSURE_CUT;
  uint32_t gas = bme.readGas()*GAS_PRECISION;

  std::map<std::string, std::string> resultMap;
  resultMap.insert(std::make_pair(HUMIDITY_CODE, std::to_string(humidity)));
  resultMap.insert(std::make_pair(TEMPERATURE_CODE, std::to_string(temperature)));
  resultMap.insert(std::make_pair(PRESSURE_CODE, std::to_string(pressure)));
  resultMap.insert(std::make_pair(GAS_CODE, std::to_string(gas)));

  return resultMap;
}
