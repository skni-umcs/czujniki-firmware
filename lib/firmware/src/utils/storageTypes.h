#ifndef STORAGETYPES_H
#define STORAGETYPES_H

#include <string>
#include <vector>
#include <Arduino.h>
#include <memory>
#include "operationResult.h"

#define INVALID_ADDRESS -1
#define INVALID_CRC -1;
#define INVALID_STRING "";
#define INVALID_VECTOR {};

typedef short moduleAddress;
const moduleAddress SERVER_ADDRESS = 0;

enum class MessageType {
    LoraMessage,
    GeneratedMessage
};

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
        virtual bool isSenderPresent(moduleAddress sender);
        virtual bool isCurrentModuleSenderPresent();
        virtual moduleAddress getDestination();
        virtual std::string getContent();
        virtual moduleAddress getOriginalSender();
        virtual unsigned char getHopLimit();
        virtual OperationResult decrementHopLimit();
        virtual std::string createAddressTable();
        virtual std::string createAddressTableWithoutHop();
        virtual std::string createOwnAddressTable();
        virtual std::string createPacket(bool addSelf = false);
        virtual std::string createPacketForSending();
        virtual bool isSameMessage(std::shared_ptr<Message> message);
        virtual MessageType type() const = 0;
};

class LoraMessage : public Message {
    byte currentRssiByte;
    int snr;

    public:
    LoraMessage(std::string packet, byte rssi, int snr);
    LoraMessage(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi, unsigned char hopLimit, byte currentRssiByte, int snr);
    std::string createOwnAddressTable() override;
    int getSnr();
    MessageType type() const override {
        return MessageType::LoraMessage;
    }
};

class GeneratedMessage : public Message {
    public:
    GeneratedMessage(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi, unsigned char hopLimit);
    MessageType type() const override {
        return MessageType::GeneratedMessage;
    }
};

#endif