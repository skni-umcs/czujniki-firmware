#include "storageTypes.h"
#include "packetUtils.h"

Message::Message(std::string packet) {
    this->packet = packet;
    std::vector<std::string> addressTable = allAddressTableElements(packet);
	this->senders = ::getSenders(addressTable);
	this->destination =	nthLastAddress(addressTable, DESTINATION_INDEX);
	this->content =	getPacketContent(packet);
}

Message::Message(std::vector<moduleAddress> senders, moduleAddress destination, std::string content) {
    this->senders = senders;
    this->destination = destination;
    this->content = content;
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

LoraMessage::LoraMessage(std::string packet, byte rssi) : Message(packet) {
    this->rssi = rssi;
}

    