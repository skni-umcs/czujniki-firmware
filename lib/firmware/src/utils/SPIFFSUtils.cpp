
#include "SPIFFSUtils.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

std::map<String, String> getNetworks() {
      
  // Initialize SPIFFS and mount it
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return {};
  }
  
  // Open the configuration file
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return {};
  }
  
  // Get the file size and read its contents into a buffer
  size_t size = configFile.size();
  if (size > 1024) { // Adjust size if necessary
    Serial.println("Config file size is too large");
    return {};
  }
  
  std::unique_ptr<char[]> buf(new char[size + 1]);
  configFile.readBytes(buf.get(), size);
  buf[size] = '\0'; // Null-terminate the string

  // Parse the JSON file
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.print("Failed to parse config file: ");
    Serial.println(error.c_str());
    return {};
  }
  

  std::map<String, String> result;
  // Access the JSON as an object and iterate over its key-value pairs.
  JsonObject obj = doc.as<JsonObject>();
  for (JsonPair kv : obj) {
    const char* ssid = kv.key().c_str();
    const char* password = kv.value().as<const char*>();
    result.insert({ssid, password});
    Serial.printf("SSID: %s, PASSWORD: %s\n", ssid, password);
    
    // Insert your logic to handle each SSID/password pair here.
    // For example, you might attempt to connect to each WiFi network.
  }
  return result;
}