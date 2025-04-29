
#include "spiffs_utils.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "logger.h"

#define NO_NETWORKS {}

std::map<String, String> retrieveNetworks() {
  if (!SPIFFS.begin(true)) {
    Logger::log("An error occurred while mounting SPIFFS");
    return NO_NETWORKS;
  }

  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Logger::log("Failed to open config file");
    return NO_NETWORKS;
  }
  
  size_t size = configFile.size();
  if (size > 1024) {
    Logger::log("Config file size is too large");
    return NO_NETWORKS;
  }
  
  std::unique_ptr<char[]> buf(new char[size + 1]);
  configFile.readBytes(buf.get(), size);
  buf[size] = '\0';

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Logger::log("Failed to parse config file: ");
    Logger::log(error.c_str());
    return NO_NETWORKS;
  }

  std::map<String, String> result;
  JsonObject obj = doc.as<JsonObject>();
  for (JsonPair kv : obj) {
    const char* ssid = kv.key().c_str();
    const char* password = kv.value().as<const char*>();
    result.insert({ssid, password});
  }
  return result;
}