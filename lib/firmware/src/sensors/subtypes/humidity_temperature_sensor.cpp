#include "humidity_temperature_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <string>
#include <sstream>

#define DHT_POWER_PIN 3

#define DHTPIN 40     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT dht(DHTPIN, DHTTYPE);

void HumidityTemperatureSensor::setupSensor(uint32_t* delayMS) {
    pinMode(DHT_POWER_PIN, OUTPUT);
    digitalWrite(DHT_POWER_PIN, LOW);
}

std::string HumidityTemperatureSensor::getSensorData() {
    digitalWrite(DHT_POWER_PIN, HIGH);
    delay(1000);
    // Get temperature event and print its value.
    dht.begin();
    delay(2000);
    sensors_event_t event;
    std::stringstream result;
    result << "humidity: " << dht.readHumidity() << " temperature: " << dht.readTemperature();
    digitalWrite(DHT_POWER_PIN, LOW);
    return result.str();
}