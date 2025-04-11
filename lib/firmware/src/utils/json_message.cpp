#include "json_message.h"
#include <sstream>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include "time/time_constants.h"

JsonMessage::JsonMessage(TransmissionCode type, std::string message) {
    this->type = type;
    this->message = message;
}

std::string transmissionCodeFromEnum(TransmissionCode transmissionCode) {
    return std::string(1, (char)transmissionCode);
}

TransmissionCode enumFromTransmissionCode(std::string code) {
	if (code.length() != 1) {
        return TransmissionCode::ERROR_CODE;
    }
    return static_cast<TransmissionCode>(code[0]);
}

std::string JsonMessage::getJson() {
	jsonificationEpoch = rtc.getEpoch();
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    std::string serializedJson;

    root[transmissionCodeFromEnum(TransmissionCode::MESSAGE_TYPE)] = transmissionCodeFromEnum(type);
    root[transmissionCodeFromEnum(TransmissionCode::MESSAGE)] = message;
	root[transmissionCodeFromEnum(TransmissionCode::TIMESTAMP)] = jsonificationEpoch;

    serializeJson(doc, serializedJson);

    return serializedJson;
}

JsonMessage JsonMessage::fromJson(std::string jsonString) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        Serial.printf("Failed to deserialize json %s", jsonString.c_str());
		return JsonMessage(TransmissionCode::ERROR_CODE, "");
    }

    JsonObject root = doc.as<JsonObject>();

    TransmissionCode type = enumFromTransmissionCode(root[transmissionCodeFromEnum(TransmissionCode::MESSAGE_TYPE)].as<std::string>());
    std::string message = root[transmissionCodeFromEnum(TransmissionCode::MESSAGE)].as<std::string>();
	unsigned long jsonificationEpoch = root[transmissionCodeFromEnum(TransmissionCode::TIMESTAMP)].as<unsigned long>();

	JsonMessage result = JsonMessage(type, message);
	result.jsonificationEpoch = jsonificationEpoch;

    return result;
}

TransmissionCode JsonMessage::getType() {
	return type;
}

std::string JsonMessage::getMessage() {
	return message;
}

unsigned long JsonMessage::getJsonificationEpoch() {
	return jsonificationEpoch;
}