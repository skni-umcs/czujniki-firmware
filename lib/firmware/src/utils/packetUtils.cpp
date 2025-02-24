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

TransmissionCode enumFromTransmissionCode(std::string code) {
	if (code.length() != 1) {
        return TransmissionCode::ERROR_CODE;
    }
    return static_cast<TransmissionCode>(code[0]);
}

std::string PacketMessage::getJson() {
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

PacketMessage PacketMessage::fromJson(std::string jsonString) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        Serial.printf("Failed to deserialize json %s", jsonString.c_str());
		return PacketMessage(TransmissionCode::ERROR_CODE, "");
    }

    JsonObject root = doc.as<JsonObject>();

    TransmissionCode type = enumFromTransmissionCode(root[transmissionCodeFromEnum(TransmissionCode::MESSAGE_TYPE)].as<std::string>());
    std::string message = root[transmissionCodeFromEnum(TransmissionCode::MESSAGE)].as<std::string>();
	unsigned long jsonificationEpoch = root[transmissionCodeFromEnum(TransmissionCode::TIMESTAMP)].as<unsigned long>();

	Serial.println(transmissionCodeFromEnum(type).c_str());
	Serial.println(message.c_str());
	Serial.println(jsonificationEpoch);

	PacketMessage result = PacketMessage(type, message);
	result.jsonificationEpoch = jsonificationEpoch;

    return result;
}

TransmissionCode PacketMessage::getType() {
	return type;
}

std::string PacketMessage::getMessage() {
	return message;
}

unsigned long PacketMessage::getJsonificationEpoch() {
	return jsonificationEpoch;
}

uint32_t getCrc(std::string string) {
	return CRC32.crc32(
		reinterpret_cast<const uint8_t*>(string.c_str()), 
		string.size()
	);
}

moduleAddress stringToAddress(std::string string) {
	std::cout << string << std::endl;
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
	size_t size = elements.size();
	if(elements.empty() || index < 0 || index > elements.size()-1) {
		Serial.printf("Can't get address table element: %i\n", n);
		return INVALID_STRING;
	}
	return elements.at(index);
}

moduleAddress nthLastAddress(std::vector<std::string> addressTableElements, unsigned char n) {
	std::cout << "biore nty last adres " << (int)n << std::endl;
	std::set<unsigned char> startAddresses{0,1};
	if (setContains(startAddresses, n)) {
		std::cout << " set containowal " << std::endl;
		std::string adres = nthLastAddressTableElement(addressTableElements, n);
		std::cout << " a jego adres to " << adres << std::endl;
		return stringToAddress(adres);
	}
	else {
		unsigned char firstMapped = 3;
		unsigned char jump = (n-2)*2;
		unsigned char mappedPosition = firstMapped+jump;
		std::cout << "a teraz adres" << std::endl;
		std::string a = nthLastAddressTableElement(addressTableElements, mappedPosition);
		std::cout << " adres to " << a << std::endl;
		return stringToAddress(a);
	}
}

std::string nthLastRssi(std::vector<std::string> addressTableElements, unsigned char n) {
	std::cout << "ente ostatnie rssi nr " << (int)n << " to byl index mam nadzieje ze sie podobal" << std::endl;
	unsigned char firstMapped = 2;
	unsigned char jump = n*2;
	unsigned char mappedPosition = firstMapped+jump;
	return nthLastAddressTableElement(addressTableElements, mappedPosition);
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
	return strtoul(crcSubstring.c_str(), errorDestination, HEX);
}

std::vector<moduleAddress> getSenders(std::vector<std::string> addressTable) {
	std::cout << " getsenders" << std::endl;
	if(addressTable.size() <= 1) {
		Serial.println("Incorrect addressTable, no original sender found!");
		return INVALID_VECTOR;
	}
	int originalSenderAndDestinationCount = 2;
	int otherElements = addressTable.size()-originalSenderAndDestinationCount;
	int otherSendersCount = (otherElements)/2;
	std::cout << " egfege? " << std::endl;
	std::vector<moduleAddress> result = {nthLastAddress(addressTable, ORIGINAL_SENDER_INDEX)};
	std::cout << " hello? " << std::endl;
	for(int i = 0;i<otherSendersCount;++i) {
		result.push_back(nthLastAddress(addressTable, i+originalSenderAndDestinationCount));
	}
	return result;
}

std::vector<std::string> getRssi(std::vector<std::string> addressTable) {
	std::cout << "BLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLEBLE" << std::endl;
	if(addressTable.size() <= 3) {
		std::cout << "nie mo rssi" << std::endl;
		Serial.println("No rssi found in packet");
		return INVALID_VECTOR;
	}
	int originalSenderAndDestinationCount = 2;
	int otherElements = addressTable.size()-originalSenderAndDestinationCount;
	int otherSendersCount = (otherElements)/2;

	std::vector<std::string> result = {};
	for(int i = 0;i<otherSendersCount;++i) {
		std::cout << " biore " << i << " te ostatnie rssi" << std::endl;
		std::string e = nthLastRssi(addressTable, i);
		std::cout << " to jest " << e << std::endl;
		result.push_back(e);
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

	std::string pattern = PACKET_BORDER+validatedPart.str()+jsonPart.str()+crcPart+PACKET_BORDER;
	char result = ms.Match(pattern.c_str());
	delete[] buf;
	return result > 0;
}

bool isPacketCorrect(std::string packet) {
	return isRegexCorrect(packet) && isCrcCorrect(packet);
}