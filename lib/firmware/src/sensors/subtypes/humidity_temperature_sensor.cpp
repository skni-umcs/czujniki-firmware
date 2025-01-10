#include "humidity_temperature_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <string>
#include <sstream>

#define DHTPIN 40     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified humidityTemperatureDht(DHTPIN, DHTTYPE);

void HumidityTemperatureSensor::setupSensor(uint32_t* delayMS) {
    humidityTemperatureDht.begin();
    sensor_t sensor;
    humidityTemperatureDht.temperature().getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));
    // Print humidity sensor details.
    humidityTemperatureDht.humidity().getSensor(&sensor);
    Serial.println(F("Humidity Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
    Serial.println(F("------------------------------------"));

    // Set delay between sensor readings based on sensor details.
    *delayMS = sensor.min_delay / 1000;

}

std::string HumidityTemperatureSensor::getSensorDataJson() {
    // Get temperature event and print its value.
    sensors_event_t event;
    std::stringstream result;
    humidityTemperatureDht.temperature().getEvent(&event);
    result << "humidity: " << event.relative_humidity << " temperature: " << event.temperature;
    return result.str();
}