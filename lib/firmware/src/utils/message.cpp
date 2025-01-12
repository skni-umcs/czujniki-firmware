#include "storageTypes.h"
#include "packetUtils.h"
#include <sstream>
#include "addressHandler.h"
#include <iostream>

Message::Message() {}

Message::Message(
    std::vector<moduleAddress> senders, 
    moduleAddress destination, 
    std::string content,
    std::vector<std::string> rssi,
    unsigned char hopLimit
) {
    this->senders = senders;
    this->destination = destination;
    this->content = content;
    this->rssi = rssi;
    this->hopLimit = hopLimit;
}

LoraMessage::LoraMessage(std::string packet, byte currentRssiByte, int snr) {
    this->packet = packet;
    this->addressTable = allAddressTableElements(packet);
	this->senders = ::getSenders(addressTable);
	this->destination =	nthLastAddress(addressTable, DESTINATION_INDEX);
	this->content =	getPacketContent(packet);
    this->currentRssiByte = currentRssiByte;
    this->snr = snr;
}

LoraMessage::LoraMessage(
    std::vector<moduleAddress> senders, 
    moduleAddress destination, 
    std::string content,
    std::vector<std::string> rssi,
    unsigned char hopLimit,
    byte currentRssi,
    int snr
) : Message(senders, destination, content, rssi, hopLimit) {
    this->currentRssiByte = currentRssi;
    this->snr = snr;
}

GeneratedMessage::GeneratedMessage(
    std::vector<moduleAddress> senders, 
    moduleAddress destination, 
    std::string content,
    std::vector<std::string> rssi,
    unsigned char hopLimit
) : Message(senders, destination, content, rssi, hopLimit) {

}

std::vector<moduleAddress> Message::getSenders() {
    return senders;
}

moduleAddress Message::getDestination() {
    return destination;
}

std::string Message::getContent() {
    return content;
}

moduleAddress Message::getOriginalSender() {
    if(senders.size() <= 0) {
        return INVALID_ADDRESS;
    }
    return senders.at(0);
}

bool Message::isSameMessage(std::shared_ptr<Message> message) {
    return message->getOriginalSender() == getOriginalSender() &&
    message->getDestination() == getDestination() &&
    message->getContent() == getContent(); 
}

std::string Message::createAddressTableWithoutHop() {
    std::string result = "";
    for(int i = 0;i < senders.size()-1; ++i) {
        int rssiIndex = rssi.size()-1-i;
        int senderIndex = senders.size()-1-i;
        result += NODE_BORDER+toHexString(senders.at(senderIndex));
        if(rssiIndex < 0 || rssiIndex >= rssi.size()) {
            result += NODE_BORDER;
        }
        else {
            result += NODE_BORDER+rssi.at(rssiIndex);
        }
    }
    result += NODE_BORDER+toHexString(getOriginalSender())+NODE_BORDER+toHexString(getDestination());
    return result;
}

std::string Message::createAddressTable() {
    std::string hopLimitString = NODE_BORDER+toHexString((int)hopLimit);
    return hopLimitString+createAddressTableWithoutHop();
}

std::string Message::createOwnAddressTable() {
    return NODE_BORDER+toHexString((int)hopLimit)+NODE_BORDER+
    toHexString(AddressHandler::getInstance().get()->readAddress())+NODE_BORDER+
    createAddressTable();
}

std::string Message::createPacket(bool addSelf) {
    std::string addressTable = addSelf ? createOwnAddressTable() : createAddressTable();
	std::string validatedPart = 
		addressTable+
		MAIN_JSON_BORDER+getContent()+MAIN_JSON_BORDER;
	
	uint32_t crc = getCrc(validatedPart);

	std::stringstream hexStream;
	hexStream << std::hex << crc;
	Serial.println(hexStream.str().c_str());

	return PACKET_BORDER+ 
	validatedPart+
	toHexString(crc)+
	PACKET_BORDER;
}

std::string Message::createPacketForSending() {
    return createPacket(true);
}

std::string LoraMessage::createOwnAddressTable() {
    return NODE_BORDER+toHexString((int)hopLimit)+NODE_BORDER+
    toHexString(AddressHandler::getInstance().get()->readAddress())+NODE_BORDER+
    toHexString((int)currentRssiByte)+
    Message::createAddressTableWithoutHop();
}

int LoraMessage::getSnr() {
    return snr;
}