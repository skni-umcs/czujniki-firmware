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
std::string createPacket(std::string message, moduleAddress senderNode, moduleAddress destinationNode);
moduleAddress getDestinationAddress(std::string packet);
std::string getPacketMessage(std::string packet);

#endif