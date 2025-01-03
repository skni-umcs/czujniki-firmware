#ifndef PACKETUTILS_H
#define PACKETUTILS_H

#include <string>
#include "storageTypes.h"

template<typename T>
std::string toHexString(T address);
std::string createPacket(std::string message, moduleAddress senderNode, moduleAddress destinationNode);
moduleAddress getDestinationAddress(std::string packet);
std::string getPacketMessage(std::string packet);

#endif