#include "message_content.h"
#include <sstream>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include "time/time_constants.h"
#include <utils/logger.h>

MessageContent::MessageContent(TransmissionCode type, std::string message) {
    this->type = type;
    this->details = message;
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

std::string MessageContent::getJson() {
	jsonificationEpoch = rtc.getEpoch();
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    std::string serializedJson;

    root[transmissionCodeFromEnum(TransmissionCode::MESSAGE_TYPE)] = transmissionCodeFromEnum(type);
    root[transmissionCodeFromEnum(TransmissionCode::DETAILS)] = details;
	root[transmissionCodeFromEnum(TransmissionCode::TIMESTAMP)] = jsonificationEpoch;

    serializeJson(doc, serializedJson);

    return serializedJson;
}

MessageContent MessageContent::fromJson(std::string jsonString) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        Logger::logf("Failed to deserialize json %s", jsonString.c_str());
		return MessageContent(TransmissionCode::ERROR_CODE, "");
    }

    JsonObject root = doc.as<JsonObject>();

    TransmissionCode type = enumFromTransmissionCode(root[transmissionCodeFromEnum(TransmissionCode::MESSAGE_TYPE)].as<std::string>());
    std::string message = root[transmissionCodeFromEnum(TransmissionCode::DETAILS)].as<std::string>();
	unsigned long jsonificationEpoch = root[transmissionCodeFromEnum(TransmissionCode::TIMESTAMP)].as<unsigned long>();

	MessageContent result = MessageContent(type, message);
	result.jsonificationEpoch = jsonificationEpoch;

    return result;
}

TransmissionCode MessageContent::getType() {
	return type;
}

std::string MessageContent::getDetails() {
	return details;
}

unsigned long MessageContent::getJsonificationEpoch() {
	return jsonificationEpoch;
}