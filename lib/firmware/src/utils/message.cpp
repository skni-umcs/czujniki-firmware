#include "storageTypes.h"
#include "packetUtils.h"
#include <sstream>

Message::Message(std::string packet) {
    this->packet = packet;
    this->addressTable = allAddressTableElements(packet);
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

std::string Message::createPacket() {
	std::string validatedPart = 
		NODE_BORDER+toHexString(getOriginalSender())+NODE_BORDER+toHexString(getDestination())+
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

LoraMessage::LoraMessage(std::string packet, byte currentRssi) : Message(packet) {
    this->currentRssi = currentRssi;
}

LoraMessage::LoraMessage(
    std::vector<moduleAddress> senders, 
    moduleAddress destination, 
    std::string content,
    byte currentRssi,
    std::vector<std::string> rssi,
    int hopLimit
) : Message(senders, destination, content) {
    this->currentRssi = currentRssi;
    this->rssi = rssi;
    this->hopLimit = hopLimit;
}