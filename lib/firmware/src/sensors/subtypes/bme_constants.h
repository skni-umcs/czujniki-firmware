#ifndef BMECONSTANTS_H
#define BMECONSTANTS_H

#include <string>

const std::string TEMPERATURE_CODE = "t";
const std::string PRESSURE_CODE = "p";
const std::string HUMIDITY_CODE = "h";

#define HUMIDITY_PRECISION 1;
#define TEMPERATURE_PRECISION 100;
#define PRESSURE_CUT 10;
#define BME_POWER_PIN 3

#endif
