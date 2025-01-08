#include "storageTypes.h"
#include "packetUtils.h"
#include <sstream>
#include "addressHandler.h"

Message::Message(std::string packet) {
    this->packet = packet;
    this->addressTable = allAddressTableElements(packet);
	this->senders = ::getSenders(addressTable);
	this->destination =	nthLastAddress(addressTable, DESTINATION_INDEX);
	this->content =	getPacketContent(packet);
}

Message::Message(std::vector<moduleAddress> senders, moduleAddress destination, std::string content, std::vector<std::string> rssi) {
    this->senders = senders;
    this->destination = destination;
    this->content = content;
    this->rssi = rssi;
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

std::string Message::createAddressTable() {
    std::string result = ""; 
    for(int i = 0;i < senders.size()-1; ++i) {
        int rssiIndex = rssi.size()-1-(i-1);
        int senderIndex = senders.size()-1-(i);
        result += NODE_BORDER+toHexString(senders.at(senderIndex));
        if(rssiIndex < 0 || rssiIndex >= rssi.size()) {
            result += NODE_BORDER;
        }
        else {
            result += rssi.at(rssiIndex)+NODE_BORDER;
        }
    }
    result += NODE_BORDER+toHexString(getOriginalSender())+NODE_BORDER+toHexString(getDestination());
    return result;
}

std::string Message::createOwnAddressTable() {
    return "baseclass";//toHexString(AddressHandler::getInstance().get()->readAddress())+NODE_BORDER+NODE_BORDER+createAddressTable();
}

std::string Message::createPacket(bool addSelf) {
    std::string addressTable = addSelf ? this->createOwnAddressTable() : this->createAddressTable();
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

LoraMessage::LoraMessage(std::string packet, byte currentRssi) : Message(packet) {
    this->currentRssi = currentRssi;
}

LoraMessage::LoraMessage(
    std::vector<moduleAddress> senders, 
    moduleAddress destination, 
    std::string content,
    std::vector<std::string> rssi,
    byte currentRssi,
    int hopLimit
) : Message(senders, destination, content, rssi) {
    this->currentRssi = currentRssi;
    this->hopLimit = hopLimit;
}

std::string LoraMessage::createAddressTable() {
    return NODE_BORDER+toHexString(hopLimit)+Message::createAddressTable();
}

std::string LoraMessage::createOwnAddressTable() {
    return "childclass";/*NODE_BORDER+toHexString(hopLimit)+NODE_BORDER+
    toHexString(AddressHandler::getInstance().get()->readAddress())+NODE_BORDER+
    toHexString(currentRssi)+NODE_BORDER+
    createAddressTable();*/
}