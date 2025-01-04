#include <FastCRC.h>
#include <sstream>
#include <HardwareSerial.h>
#include "types/loraTypes.h"
#include "packetUtils.h"

FastCRC32 CRC32;

const char PACKET_BORDER = '~';
const char MAIN_JSON_BORDER = '^';
const char NODE_BORDER = '$';

PacketMessage::PacketMessage(TransmissionCode type, std::string message) {
    this->type = type;
    this->message = message;
}

std::string transmissionCodeFromEnum(TransmissionCode transmissionCode) {
    return std::string(1, (char)transmissionCode);
}

std::string PacketMessage::getJson() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    std::string serializedJson;

    root[transmissionCodeFromEnum(TransmissionCode::MESSAGE_TYPE)] = transmissionCodeFromEnum(type);
    root[transmissionCodeFromEnum(TransmissionCode::MESSAGE)] = message;

    serializeJson(doc, serializedJson);

    return serializedJson;
}

template<typename T>
std::string toHexString(T address) {
	std::stringstream hexStream;
	hexStream << std::hex << address;
	return hexStream.str();
}

std::string createPacket(std::string message, moduleAddress senderNode, moduleAddress destinationNode) {
	std::string validatedPart = 
		toHexString(senderNode)+NODE_BORDER+toHexString(destinationNode)+
		MAIN_JSON_BORDER+message+MAIN_JSON_BORDER;
	
	uint32_t crc = CRC32.crc32(
		reinterpret_cast<const uint8_t*>(validatedPart.c_str()), 
		validatedPart.size()
	);

	std::stringstream hexStream;
	hexStream << std::hex << crc;
	Serial.println(hexStream.str().c_str());

	return PACKET_BORDER+ 
	validatedPart+
	toHexString(crc)+
	PACKET_BORDER;
}

moduleAddress getDestinationAddress(std::string packet) {
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	if (jsonStart == std::string::npos) {
		Serial.printf("Invalid packet, no message: %s\n", packet);
		return INVALID_ADDRESS;
	}
	int nodeAddressStart = packet.find_last_of(NODE_BORDER);
	if (nodeAddressStart == std::string::npos) {
		Serial.printf("Invalid packet, no destination: %s", packet);
		return INVALID_ADDRESS;
	}
	int charsBetweenAddressStartAndJSON = jsonStart-nodeAddressStart-1;
	std::string nodeSubstr = packet.substr(nodeAddressStart+1, charsBetweenAddressStartAndJSON);
	return atoi(nodeSubstr.c_str());
}

std::string getPacketMessage(std::string packet) {
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	int jsonEnd = packet.find_last_of(MAIN_JSON_BORDER);
	if (jsonStart == std::string::npos || jsonEnd == std::string::npos) {
		Serial.printf("Invalid packet, no message: %s\n",packet);
		return "";
	}
	int jsonChars = jsonEnd-jsonStart-1;
	return packet.substr(jsonStart+1, jsonChars);
}