#ifndef PACKETUTILS_H
#define PACKETUTILS_H

#include <string>
#include "storageTypes.h"
#include <ArduinoJson.h>

enum class TransmissionCode : char {
    RESET = 'r',
    MESSAGE_TYPE = 't',
    MESSAGE = 'm'
};

class PacketMessage {
    TransmissionCode type;
    std::string message;

    public:
        PacketMessage(TransmissionCode type, std::string message);
        std::string getJson();
};

template<typename T>
std::string toHexString(T address);
std::string transmissionCodeFromEnum(TransmissionCode transmissionCode);
std::string createPacket(Message message);
moduleAddress getDestinationAddress(std::string packet);
std::vector<std::string> getAllAddressTableElements(std::string packet);
std::string getNthLastAddressTableElement(std::vector<std::string> addressTableElements, unsigned char n);
moduleAddress getNthLastAddress(std::vector<std::string> addressTableElements, unsigned char n);
std::vector<moduleAddress> getSenders(std::vector<std::string> addressTable);
Message getPacketMessage(std::string packet);
std::string getValidatedPart(std::string packet);
bool isCrcCorrect(std::string packet);
bool isPacketCorrect(std::string packet);

#endif