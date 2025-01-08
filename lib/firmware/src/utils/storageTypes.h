#ifndef STORAGETYPES_H
#define STORAGETYPES_H

#include <string>
#include <vector>
#include <Arduino.h>

#define INVALID_ADDRESS -1
#define INVALID_CRC -1;
#define INVALID_STRING "";
#define INVALID_VECTOR {};

typedef short moduleAddress;
const moduleAddress SERVER_ADDRESS = 0;

class Message {
    protected:
    std::string packet;
    std::vector<std::string> addressTable;
    std::vector<moduleAddress> senders;
    moduleAddress destination;
    std::string content;
    std::vector<std::string> rssi;

    public:
        Message(std::string packet);
        Message(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi);
        std::vector<moduleAddress> getSenders();
        moduleAddress getDestination();
        std::string getContent();
        moduleAddress getOriginalSender();
        std::string createAddressTable();
        std::string createOwnAddressTable();
        std::string createPacket(bool addSelf = false);
        std::string createPacketForSending();
};

class LoraMessage : public Message {
    byte currentRssi;
    int hopLimit;
    public:
    LoraMessage(std::string packet, byte rssi);
    LoraMessage(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi, byte currentRssi, int hopLimit);
    std::string createAddressTable();
    std::string createOwnAddressTable();
};

#endif