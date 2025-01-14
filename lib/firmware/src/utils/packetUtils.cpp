#include <sstream>
#include <HardwareSerial.h>
#include "storageTypes.h"
#include "packetUtils.h"
#include <set>
#include <iostream>
#include <regex>
#include "otherUtils.h"
#include "time/timeConstants.h"
#include <Regexp.h>

FastCRC32 CRC32;

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

	time_t now;
	time(&now);

    root[transmissionCodeFromEnum(TransmissionCode::MESSAGE_TYPE)] = transmissionCodeFromEnum(type);
    root[transmissionCodeFromEnum(TransmissionCode::MESSAGE)] = message;
	root[transmissionCodeFromEnum(TransmissionCode::TIMESTAMP)] = now;

    serializeJson(doc, serializedJson);

    return serializedJson;
}

uint32_t getCrc(std::string string) {
	return CRC32.crc32(
		reinterpret_cast<const uint8_t*>(string.c_str()), 
		string.size()
	);
}

moduleAddress stringToAddress(std::string string) {
	if (string.empty()) {
		return INVALID_ADDRESS;
	}
	else {
        int hexadecimalBase = 16;
        char* invalidPosition = 0;
       	moduleAddress address = INVALID_ADDRESS;
        address = strtol(string.c_str(), &invalidPosition, hexadecimalBase);
        if (*invalidPosition != '\0') {
            return INVALID_ADDRESS;
        }
        return address;
	}
}

std::vector<std::string> allAddressTableElements(std::string packet) {
	int addressesStart = packet.find(NODE_BORDER);
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	if (addressesStart == std::string::npos || jsonStart == std::string::npos || addressesStart >= jsonStart) {
		Serial.printf("Invalid packet, no message: %s\n", packet.c_str());
		return INVALID_VECTOR;
	}
	std::vector<std::string> result;

	int addressesSize = jsonStart-addressesStart-1;
	std::string substr = packet.substr(addressesStart+1, addressesSize);

	std::stringstream s(substr);
	std::string temp;
	while(getline(s, temp, NODE_BORDER)) {
		result.push_back(temp);
	}

	return result;
}

std::string nthLastAddressTableElement(std::vector<std::string> elements, unsigned char n) {
	size_t index = elements.size()-n-1;
	if(elements.empty() || index < 0 || index > elements.size()-1) {
		Serial.printf("Can't get address table element: %i\n", n);
		return INVALID_STRING;
	}
	return elements.at(index);
}

moduleAddress nthLastAddress(std::vector<std::string> addressTableElements, unsigned char n) {
	std::set<unsigned char> startAddresses{0,1};
	if (setContains(startAddresses, n)) {
		return stringToAddress(nthLastAddressTableElement(addressTableElements, n));
	}
	else {
		unsigned char firstMapped = 3;
		unsigned char jump = (n-2)*2;
		unsigned char mappedPosition = firstMapped+jump;
		return stringToAddress(nthLastAddressTableElement(addressTableElements, mappedPosition));
	}
}

std::string getValidatedPart(std::string packet) {
	int leftBorder = packet.find(PACKET_BORDER);
	if (leftBorder == std::string::npos) {
		Serial.printf("Invalid packet, no left border %s\n", packet.c_str());
		return INVALID_STRING;
	}
	int rightBorder = packet.find_last_of(MAIN_JSON_BORDER);
	if (rightBorder == std::string::npos) {
		Serial.printf("Invalid packet, no JSON borders %s\n", packet.c_str());
		return INVALID_STRING;
	}
	int partStart = leftBorder+1;
	int partLength = rightBorder-leftBorder;
	return packet.substr(partStart, partLength);
}

uint32_t getPacketCrc(std::string packet) {
	int jsonEnd = packet.find_last_of(MAIN_JSON_BORDER);
	if (jsonEnd == std::string::npos) {
		Serial.printf("Invalid packet, no crc %s\n", packet.c_str());
		return INVALID_CRC;
	}
	int packetEnd = packet.find_last_of(PACKET_BORDER);
	if (packetEnd == std::string::npos) {
		Serial.printf("Invalid packet, no end border %s\n", packet.c_str());
		return INVALID_CRC;
	}
	int crcStart = jsonEnd+1;
	int crcLength = packetEnd-jsonEnd-1;
	std::string crcSubstring = packet.substr(crcStart, crcLength);
	auto errorDestination = nullptr;
	int hexadecimalBase = 16;
	return strtol(crcSubstring.c_str(), errorDestination, hexadecimalBase);
}

std::vector<moduleAddress> getSenders(std::vector<std::string> addressTable) {
	if(addressTable.size() <= 1) {
		Serial.println("Incorrect addressTable, no original sender found!");
		return INVALID_VECTOR;
	}
	int originalSenderAndDestinationCount = 2;
	int otherElements = addressTable.size()-originalSenderAndDestinationCount;
	int otherSendersCount = (otherElements)/2;

	std::vector<moduleAddress> result = {nthLastAddress(addressTable, ORIGINAL_SENDER_INDEX)};
	for(int i = 0;i<otherSendersCount;++i) {
		result.push_back(nthLastAddress(addressTable, i+originalSenderAndDestinationCount));
	}
	return result;
}

unsigned char getHopLimit(std::vector<std::string> addressTable) {
	if(addressTable.size() == 0) {
		return 0;
	}
	char* invalidPosition = 0;
	char hopLimit = 0;
	hopLimit = strtol(addressTable.at(0).c_str(), &invalidPosition, HEX);
	if (*invalidPosition != '\0') {
		return 0;
	}
	return hopLimit;
}

std::string getPacketContent(std::string packet) {
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	int jsonEnd = packet.find_last_of(MAIN_JSON_BORDER);
	if (jsonStart == std::string::npos || jsonEnd == std::string::npos || jsonEnd <= jsonStart) {
		Serial.printf("Invalid packet, no message: %s\n",packet.c_str());
		return INVALID_STRING;
	}
	int jsonChars = jsonEnd-jsonStart-1;
	return packet.substr(jsonStart+1, jsonChars);
}

bool isCrcCorrect(std::string packet) {
	uint32_t oldCrc = getPacketCrc(packet);
	uint32_t newCrc = getCrc(getValidatedPart(packet));

	return oldCrc == newCrc;
}

bool isRegexCorrect(std::string packet) {
	std::stringstream validatedPart;
	validatedPart << NODE_BORDER << ".+";
	std::stringstream jsonPart;
	jsonPart << "[\\" << MAIN_JSON_BORDER << "].+[\\" << MAIN_JSON_BORDER << "]";
	std::string crcPart = ".+";

	size_t packetSize = packet.size();
	char* buf = new char[packetSize+1];
	memcpy(buf, packet.c_str(), packetSize);
	buf[packetSize] = '\0';
	MatchState ms;
	ms.Target(buf);

	std::cout << "elo z regexa" << std::endl;
	std::cout << buf << " to był buf mam nadzieje ze sie podobal" << std::endl;

	std::string pattern = PACKET_BORDER+validatedPart.str()+jsonPart.str()+crcPart+PACKET_BORDER;
	char result = ms.Match(pattern.c_str());
	std::cout << pattern << "to buyl aptern" << std::endl;
	delete[] buf;
	return result > 0;
}

bool isPacketCorrect(std::string packet) {
	return isRegexCorrect(packet) && isCrcCorrect(packet);
}