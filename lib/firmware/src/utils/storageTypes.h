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
    public:
        virtual std::vector<moduleAddress> getSenders();
        virtual moduleAddress getDestination();
        virtual std::string getContent();
        virtual moduleAddress getOriginalSender();
        virtual std::string createAddressTable();
        virtual std::string createOwnAddressTable();
        virtual std::string createPacket(bool addSelf = false);
        virtual std::string createPacketForSending();
};

class LoraMessage : public Message {
    std::string packet;
    std::vector<std::string> addressTable;
    std::vector<moduleAddress> senders;
    moduleAddress destination;
    std::string content;
    std::vector<std::string> rssi;
    byte currentRssi;
    int hopLimit;

    public:
    LoraMessage(std::string packet, byte rssi);
    LoraMessage(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi, byte currentRssi, int hopLimit);
    std::vector<moduleAddress> getSenders() override;
    moduleAddress getDestination() override;
    std::string getContent() override;
    moduleAddress getOriginalSender() override;
    std::string createAddressTable() override;
    std::string createOwnAddressTable() override;
    std::string createPacket(bool addSelf = false) override;
    std::string createPacketForSending() override;
};

#endif