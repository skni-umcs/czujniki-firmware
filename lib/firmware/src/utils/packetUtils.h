#ifndef PACKETUTILS_H
#define PACKETUTILS_H

#include <string>
#include "storageTypes.h"
#include <ArduinoJson.h>

enum class MessageType : char {
    RESET = 'r'
};

class PacketMessage {
    MessageType type;
    std::string message;

    public:
        PacketMessage(MessageType type, std::string message);
        std::string getJson();
};

template<typename T>
std::string toHexString(T address);
std::string createPacket(std::string message, moduleAddress senderNode, moduleAddress destinationNode);
moduleAddress getDestinationAddress(std::string packet);
std::string getPacketMessage(std::string packet);

#endif