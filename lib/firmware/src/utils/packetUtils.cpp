#include <FastCRC.h>
#include <sstream>
#include <HardwareSerial.h>
#include "storageTypes.h"

FastCRC32 CRC32;

const char PACKET_BORDER = '~';
const char MAIN_JSON_BORDER = '^';
const char NODE_BORDER = '$';

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
	int JSONStart = packet.find(MAIN_JSON_BORDER);
	int JSONEnd = packet.find_last_of(MAIN_JSON_BORDER);
	if (JSONStart == std::string::npos || JSONEnd == std::string::npos) {
		Serial.printf("Invalid packet, no message: %s\n",packet);
		return "";
	}
	int JSONChars = JSONEnd-JSONStart-1;
	return packet.substr(JSONStart+1, JSONChars);
}