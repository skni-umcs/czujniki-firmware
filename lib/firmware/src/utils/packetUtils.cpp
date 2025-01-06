#include <FastCRC.h>
#include <sstream>
#include <HardwareSerial.h>
#include "storageTypes.h"
#include "packetUtils.h"
#include <set>
#include <iostream>
#include <regex>

FastCRC32 CRC32;

const char PACKET_BORDER = '~';
const char MAIN_JSON_BORDER = '^';
const char NODE_BORDER = '$';

const unsigned char DESTINATION_INDEX = 0;
const unsigned char ORIGINAL_SENDER_INDEX = 1;

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
		NODE_BORDER+toHexString(message.getOriginalSender())+NODE_BORDER+toHexString(message.getDestination())+
		MAIN_JSON_BORDER+message.getContent()+MAIN_JSON_BORDER;
	
	uint32_t crc = getCrc(validatedPart);

	std::stringstream hexStream;
	hexStream << std::hex << crc;
	Serial.println(hexStream.str().c_str());

	return PACKET_BORDER+ 
	validatedPart+
	toHexString(crc)+
	PACKET_BORDER;
}

std::vector<std::string> getAllAddressTableElements(std::string packet) {
	int addressesStart = packet.find(NODE_BORDER);
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	if (addressesStart == std::string::npos || jsonStart == std::string::npos || addressesStart >= jsonStart) {
		Serial.printf("Invalid packet, no message: %s\n", packet.c_str());
		return {};
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

std::string getNthLastAddressTableElement(std::vector<std::string> elements, unsigned char n) {
	size_t index = elements.size()-n-1;
	std::cout << "HALO HSALO CZY NAS SLYSZCIE";
	std::cout << elements.size() << " zabija " << index;
	for(auto t : elements) {
		std::cout << "elemet: " << t << std::endl;
	}
	std::cout << elements.size() << " zabija " << index;
	if(elements.empty() || index < 0 || index > elements.size()-1) {
		Serial.printf("Can't get address table element: %i\n", n);
		return "";
	}
	std::cout << elements.size() << " zabija " << index;
	return elements.at(index);
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
        std::cout << invalidPosition << std::endl << string;
        if (*invalidPosition != '\0') {
            return INVALID_ADDRESS;
        }
        return address;
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
const uint32_t INVALID_CRC = -1;
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
	return stoi(crcSubstring, errorDestination, hexadecimalBase);
}

template<typename T>
bool setContains(std::set<T> set, T element) {
	return set.find(element) != set.end();
}

moduleAddress getNthLastAddress(std::vector<std::string> addressTableElements, unsigned char n) {
	std::set<unsigned char> startAddresses{0,1};
	if (setContains(startAddresses, n)) {
		return stringToAddress(getNthLastAddressTableElement(addressTableElements, n));
	}
	else {
		unsigned char firstMapped = 3;
		unsigned char jump = (n-2)*2;
		unsigned char mappedPosition = firstMapped+jump;
		return stringToAddress(getNthLastAddressTableElement(addressTableElements, mappedPosition));
	}
}

std::vector<moduleAddress> getSenders(std::vector<std::string> addressTable) {
	std::cout << "to je koniec?";
	if(addressTable.size() <= 1) {
		Serial.println("Incorrect addressTable, no original sender found!");
		return {};
	}
	std::cout << "nie?";
	int originalSenderAndDestinationCount = 2;
	int otherElements = addressTable.size()-originalSenderAndDestinationCount;
	int otherSendersCount = (otherElements)/2;

	std::vector<moduleAddress> result = {getNthLastAddress(addressTable, ORIGINAL_SENDER_INDEX)};
	for(int i = 0;i<otherSendersCount;++i) {
		result.push_back(getNthLastAddress(addressTable, i+originalSenderAndDestinationCount));
	}
	for(auto t : result) {
		std::cout << t << std::endl;
	}
	std::cout << "dozylem?" << result.size() << std::endl;
	return result;
}

std::string getPacketContent(std::string packet) {
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	int jsonEnd = packet.find_last_of(MAIN_JSON_BORDER);
	if (jsonStart == std::string::npos || jsonEnd == std::string::npos || jsonEnd <= jsonStart) {
		Serial.printf("Invalid packet, no message: %s\n",packet.c_str());
		return "";
	}
	int jsonChars = jsonEnd-jsonStart-1;
	return packet.substr(jsonStart+1, jsonChars);
}

Message getPacketMessage(std::string packet) {
	Serial.printf("getting message from: %s\n", packet.c_str());
	std::vector<std::string> addressTable = getAllAddressTableElements(packet);
	std::cout << packet << " the adresowanie: " << addressTable.size() << std::endl;
	auto senders = getSenders(addressTable);
	Message result = Message(
		senders,
		getNthLastAddress(addressTable, DESTINATION_INDEX),
		getPacketContent(packet)
	);
	std::cout << "postadresowanie" << std::flush;
	return result;
}

bool isCrcCorrect(std::string packet) {
	uint32_t oldCrc = getPacketCrc(packet);
	uint32_t newCrc = getCrc(getValidatedPart(packet));

	std::cout << "validated part " << getValidatedPart(packet) << std::endl;
	std::cout << "crc comp " << oldCrc << " " << newCrc << " it is " << (oldCrc == newCrc) << std::endl;

	return oldCrc == newCrc;
}

bool isBorderCorrect(std::string packet) {
	return packet.find(PACKET_BORDER) == 0 && packet.find_last_of(PACKET_BORDER) == packet.size()-1;
}

bool isRegexCorrect(std::string packet) {
	std::stringstream validatedPart;
	validatedPart << "(\\" << NODE_BORDER << "[^\\" << NODE_BORDER << "]*)*";
	std::stringstream jsonPart;
	jsonPart << "\\" << MAIN_JSON_BORDER << ".*\\" << MAIN_JSON_BORDER;
	std::string crcPart = ".*";

	std::string pattern = PACKET_BORDER+validatedPart.str()+jsonPart.str()+crcPart+PACKET_BORDER;
	std::regex re(pattern);
	std::smatch foundValue;
	return std::regex_match(packet, foundValue, re);
}

bool isPacketCorrect(std::string packet) {
	return isRegexCorrect(packet) && isCrcCorrect(packet);
}