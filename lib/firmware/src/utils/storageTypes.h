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
    unsigned char hopLimit;
    public:
        Message();
        Message(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi, unsigned char hopLimit);
        virtual std::vector<moduleAddress> getSenders();
        virtual moduleAddress getDestination();
        virtual std::string getContent();
        virtual moduleAddress getOriginalSender();
        virtual std::string createAddressTable();
        virtual std::string createAddressTableWithoutHop();
        virtual std::string createOwnAddressTable();
        virtual std::string createPacket(bool addSelf = false);
        virtual std::string createPacketForSending();
};

class LoraMessage : public Message {
    byte currentRssiByte;
    int snr;

    public:
    LoraMessage(std::string packet, byte rssi, int snr);
    LoraMessage(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi, unsigned char hopLimit, byte currentRssiByte, int snr);
    std::string createOwnAddressTable() override;
};

class GeneratedMessage : public Message {
    public:
    GeneratedMessage(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi, unsigned char hopLimit);
};

#endif