#include <FastCRC.h>
#include <sstream>
#include <HardwareSerial.h>
#include "storageTypes.h"
#include "packetUtils.h"
#include <set>

FastCRC32 CRC32;

const char PACKET_BORDER = '~';
const char MAIN_JSON_BORDER = '^';
const char NODE_BORDER = '$';

const unsigned char DESTINATION_INDEX = 0;
const unsigned char SENDER_INDEX = 1;

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

uint32_t getCrc(std::string string) {
	return CRC32.crc32(
		reinterpret_cast<const uint8_t*>(string.c_str()), 
		string.size()
	);
}

std::string createPacket(Message message) {
	std::string validatedPart = 
		NODE_BORDER+toHexString(message.sender)+NODE_BORDER+toHexString(message.destination)+
		MAIN_JSON_BORDER+message.content+MAIN_JSON_BORDER;
	
	uint32_t crc = getCrc(validatedPart);

	std::stringstream hexStream;
	hexStream << std::hex << crc;
	Serial.println(hexStream.str().c_str());

	return PACKET_BORDER+ 
	validatedPart+
	toHexString(crc)+
	PACKET_BORDER;
}

std::string getNthLastAdressTableElement(std::string packet, unsigned char n) {
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	if (jsonStart == std::string::npos) {
		Serial.printf("Invalid packet, no message: %s\n", packet.c_str());
		return "";
	}
	int nodeAddressStart = jsonStart;
	for(int i = 0;i<=n;++i) {
		jsonStart = nodeAddressStart;
		int indexBeforeCurrentAddress = nodeAddressStart-1;
		nodeAddressStart = packet.find_last_of(NODE_BORDER, indexBeforeCurrentAddress);
	}
	if (nodeAddressStart == std::string::npos) {
		Serial.printf("Invalid packet, no destination: %s", packet.c_str());
		return "";
	}
	int charsBetweenAddressStartAndJSON = jsonStart-nodeAddressStart-1;
	std::string nodeSubstr = packet.substr(nodeAddressStart+1, charsBetweenAddressStartAndJSON);
	return nodeSubstr.c_str();
}

moduleAddress stringToAddress(std::string string) {
	if (string.empty()) {
		return INVALID_ADDRESS;
	}
	else {
		int hexadecimalBase = 16;
		auto trackInvalidPosition = nullptr;
		return stoi(string, trackInvalidPosition, hexadecimalBase);
	}
}

std::string getValidatedPart(std::string packet) {
	int leftBorder = packet.find(PACKET_BORDER);
	if (leftBorder == std::string::npos) {
		Serial.printf("Invalid packet, no left border %s\n", packet.c_str());
		return "";
	}
	int rightBorder = packet.find_last_of(MAIN_JSON_BORDER);
	if (rightBorder == std::string::npos) {
		Serial.printf("Invalid packet, no JSON borders %s\n", packet.c_str());
		return "";
	}
	int partStart = leftBorder+1;
	int partLength = rightBorder-leftBorder;
	return packet.substr(partStart, partLength);
}

uint32_t getPacketCrc(std::string packet) {
	int jsonEnd = packet.find_last_of(MAIN_JSON_BORDER);
	if (jsonEnd == std::string::npos) {
		Serial.printf("Invalid packet, no crc %s\n", packet.c_str());
		return 0;
	}
	int packetEnd = packet.find_last_of(PACKET_BORDER);
	if (packetEnd == std::string::npos) {
		Serial.printf("Invalid packet, no end border %s\n", packet.c_str());
		return 0;
	}
	int crcStart = jsonEnd+1;
	int crcLength = packetEnd-jsonEnd-1;
	std::string crcSubstring = packet.substr(crcStart, crcLength);
	auto errorDestination = nullptr;
	int hexadecimalBase = 16;
	return stoi(crcSubstring, errorDestination, hexadecimalBase);
}

moduleAddress getNthLastAddress(std::string packet, unsigned char n) {
	std::set<unsigned char> startAddresses{0,1};
	if (startAddresses.find(n) != startAddresses.end()) {
		return stringToAddress(getNthLastAdressTableElement(packet, n));
	}
	else {
		unsigned char firstMapped = 3;
		unsigned char jump = (n-2)*2;
		unsigned char mappedPosition = firstMapped+jump;
		return stringToAddress(getNthLastAdressTableElement(packet, mappedPosition));
	}
}

std::string getPacketContent(std::string packet) {
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	int jsonEnd = packet.find_last_of(MAIN_JSON_BORDER);
	if (jsonStart == std::string::npos || jsonEnd == std::string::npos) {
		Serial.printf("Invalid packet, no message: %s\n",packet.c_str());
		return "";
	}
	int jsonChars = jsonEnd-jsonStart-1;
	return packet.substr(jsonStart+1, jsonChars);
}

Message getPacketMessage(std::string packet) {
	Serial.printf("getting message from: %s\n", packet.c_str());
	Message result = Message();
	result.destination = getNthLastAddress(packet, DESTINATION_INDEX);
	result.sender = getNthLastAddress(packet, SENDER_INDEX);
	result.content = getPacketContent(packet);
	return result;
}

bool isCrcCorrect(std::string packet) {
	Message message = getPacketMessage(packet);

	uint32_t oldCrc = getPacketCrc(packet);
	uint32_t newCrc = getCrc(getValidatedPart(packet));

	return oldCrc == newCrc;
}

bool isBorderCorrect(std::string packet) {
	return packet.find(PACKET_BORDER) == 0 && packet.find_last_of(PACKET_BORDER) == packet.size()-1;
}

bool isPacketCorrect(std::string packet) {
	return isBorderCorrect(packet) && isCrcCorrect(packet);
}