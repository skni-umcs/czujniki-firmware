#include "storageTypes.h"
#include "packetUtils.h"
#include <sstream>
#include "addressHandler.h"

LoraMessage::LoraMessage(std::string packet, byte currentRssi) {
    this->packet = packet;
    this->addressTable = allAddressTableElements(packet);
	this->senders = ::getSenders(addressTable);
	this->destination =	nthLastAddress(addressTable, DESTINATION_INDEX);
	this->content =	getPacketContent(packet);
    this->currentRssi = currentRssi;
}

LoraMessage::LoraMessage(
    std::vector<moduleAddress> senders, 
    moduleAddress destination, 
    std::string content,
    std::vector<std::string> rssi,
    byte currentRssi,
    int hopLimit
) {
    this->senders = senders;
    this->destination = destination;
    this->content = content;
    this->rssi = rssi;
    this->currentRssi = currentRssi;
    this->hopLimit = hopLimit;
}

std::vector<moduleAddress> LoraMessage::getSenders() {
    return senders;
}

moduleAddress LoraMessage::getDestination() {
    return destination;
}

std::string LoraMessage::getContent() {
    return content;
}

moduleAddress LoraMessage::getOriginalSender() {
    if(senders.size() <= 0) {
        return INVALID_ADDRESS;
    }
    return senders.at(0);
}

std::string LoraMessage::createAddressTable() {
    std::string result = NODE_BORDER+toHexString(hopLimit); 
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

std::string LoraMessage::createPacket(bool addSelf) {
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

std::string LoraMessage::createPacketForSending() {
    return createPacket(true);
}

std::string LoraMessage::createOwnAddressTable() {
    return NODE_BORDER+toHexString(hopLimit)+NODE_BORDER+
    toHexString(AddressHandler::getInstance().get()->readAddress())+NODE_BORDER+
    toHexString((int)currentRssi)+
    createAddressTable();
}