#ifndef PACKETUTILS_H
#define PACKETUTILS_H

#include <string>
#include "storageTypes.h"
#include <ArduinoJson.h>
#include <FastCRC.h>
#include <sstream>

const char PACKET_BORDER = '~';
const char MAIN_JSON_BORDER = '^';
const char NODE_BORDER = '$';

const unsigned char DESTINATION_INDEX = 0;
const unsigned char ORIGINAL_SENDER_INDEX = 1;

enum class TransmissionCode : char {
    //error
    ERROR_CODE = ' ',

    //JSON codes
    MESSAGE_TYPE = 't',
    MESSAGE = 'm',
    TIMESTAMP = 'c',

    //message types
    RESET = 'r',
    SENSOR_READING = 'p',
    TIME_SYNCHRONIZATION = 'o'
};

class PacketMessage {
    TransmissionCode type;
    std::string message;
    unsigned long jsonificationEpoch;

    public:
        PacketMessage(TransmissionCode type, std::string message);
        std::string getJson();
        static PacketMessage fromJson(std::string jsonString);
        TransmissionCode getType();
        std::string getMessage();
        unsigned long getJsonificationEpoch();
};

template<typename T>
std::string toHexString(T address) {
	std::stringstream hexStream;
	hexStream << std::hex << address;
	return hexStream.str();
}

static std::string transmissionCodeFromEnum(TransmissionCode transmissionCode);
uint32_t getCrc(std::string string);
moduleAddress getDestinationAddress(std::string packet);
std::vector<std::string> allAddressTableElements(std::string packet);
std::string nthLastAddressTableElement(std::vector<std::string> addressTableElements, unsigned char n);
moduleAddress nthLastAddress(std::vector<std::string> addressTableElements, unsigned char n);
std::vector<moduleAddress> getSenders(std::vector<std::string> addressTable);
std::vector<std::string> getRssi(std::vector<std::string> addressTable);
unsigned char getHopLimit(std::vector<std::string> addressTable);
std::string getPacketContent(std::string packet);
std::string getValidatedPart(std::string packet);
bool isCrcCorrect(std::string packet);
bool isPacketCorrect(std::string packet);

#endif