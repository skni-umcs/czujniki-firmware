#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>
#include "storage_types.h"
#include <sstream>

template<typename T>
std::string toHexString(T address) {
	std::stringstream hexStream;
	hexStream << std::hex << address;
	return hexStream.str();
}

class Message {
    protected:
    std::string packet;
    std::vector<std::string> addressTable;
    std::vector<moduleAddress> senders;
    moduleAddress destination = INVALID_ADDRESS;
    std::string content;
    std::vector<std::string> rssi;
    unsigned char hopLimit;
    bool isPacketCorrect = true;
    public:
        Message();
        Message(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi, unsigned char hopLimit);
        virtual std::vector<moduleAddress> getSenders();
        virtual std::vector<std::string> getRssi();
        virtual bool isCurrentDestination();
        virtual bool isSenderPresent(moduleAddress sender);
        virtual bool isCurrentModuleSenderPresent();
        virtual moduleAddress getDestination();
        virtual std::string getContent();
        virtual moduleAddress getOriginalSender();
        virtual unsigned char getHopLimit();
        virtual bool getIsPacketCorrect();
        virtual std::string getPacket();
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
    byte getCurrentRssiByte();
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

class TextMessage : public Message {
    public:
    TextMessage(std::string content);
    MessageType type() const override {
        return MessageType::TextMessage;
    }
};

#endif