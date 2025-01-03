#include <utils/operationResult.h>
#include "loraTransmit.h"
#include <iostream>
#include <LoRa_E220.h>
#include "utils/stringUtils.h"
#include <FastCRC.h>
#include <sstream>
#include <utils/addressHandler.h>

void printParameters(struct Configuration configuration);

// With FIXED RECEIVER configuration
#define DESTINATION_ADDL 2
// If you want use RSSI uncomment
//#define ENABLE_RSSI true

const int DEFAULT_LORA_POLL_MS = 600;

void LoraTransmit::setup() {
	Serial.println("Setupping LoraTransmit");
	timer.get()->setExecuteFunction([this]() {this->poll();});
    timer.get()->updateTime(10,DEFAULT_LORA_POLL_MS);

	// Startup all pins and UART
	e220ttl.begin();

    ResponseStructContainer c = ResponseStructContainer();
	c = e220ttl.getConfiguration();
	// It's important get configuration pointer before all other operation
	Configuration configuration = *(Configuration*) c.data;
	configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION; // Enable repeater mode
	Serial.println(c.status.getResponseDescription());
	Serial.println(c.status.code);

	printParameters(configuration);
	c.close();
}

std::shared_ptr<LoraTransmit> LoraTransmit::create() {
    auto loraTransmit = new LoraTransmit();
    loraTransmit->setup();
    return std::shared_ptr<LoraTransmit>{loraTransmit};
}

FastCRC32 CRC32;

const char PACKET_BORDER = '~';
const char MAIN_JSON_BORDER = '^';
const char NODE_BORDER = '$';

template<typename T>
std::string toHexString(T address) {
	std::stringstream hexStream;
	hexStream << std::hex << address;
	return hexStream.str();
}

std::string createPacket(std::string message, moduleAddress senderNode, moduleAddress destinationNode) {
	std::string validatedPart = 
		toHexString(senderNode)+NODE_BORDER+toHexString(destinationNode)+
		MAIN_JSON_BORDER+message+MAIN_JSON_BORDER;
	
	uint32_t crc = CRC32.crc32(
		reinterpret_cast<const uint8_t*>(validatedPart.c_str()), 
		message.size()
	);

	return PACKET_BORDER+
	validatedPart+
	toHexString(crc)+
	PACKET_BORDER;
}

moduleAddress getDestinationAddress(std::string packet) {
	int jsonStart = packet.find(MAIN_JSON_BORDER);
	if (jsonStart == std::string::npos) {
		Serial.printf("Invalid packet, no message: %s\n", packet);
		return -1;
	}
	int nodeAddressStart = packet.find_last_of(NODE_BORDER);
	if (nodeAddressStart == std::string::npos) {
		Serial.printf("Invalid packet, no destination: %s", packet);
		return -1;
	}
	int charsBetweenAddressStartAndJSON = jsonStart-nodeAddressStart-1;
	std::string nodeSubstr = packet.substr(nodeAddressStart, charsBetweenAddressStartAndJSON);
	return atoi(nodeSubstr.c_str());
}

std::string getPacketMessage(std::string packet) {
	int JSONStart = packet.find(MAIN_JSON_BORDER);
	int JSONEnd = packet.find_last_of(MAIN_JSON_BORDER);
	if (JSONStart == std::string::npos || JSONEnd == std::string::npos) {
		Serial.printf("Invalid packet, no message: %s\n",packet);
		return "";
	}
	int JSONChars = JSONEnd-JSONStart-1;
	return packet.substr(JSONStart, JSONChars);
}

OperationResult LoraTransmit::send(std::string message, moduleAddress destinationNode) {
	Serial.println("Hi, I'm going to send message!");

	// Send message
	//ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, "Hello, world?");
	std::string packet = createPacket(message, AddressHandler::getInstance().get()->readAddress(), destinationNode);
	ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, packet.c_str());
	// Check If there is some problem of succesfully send
	Serial.println(rs.getResponseDescription());
    return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::poll() {
	if (e220ttl.available()>1) {
		Serial.println("Message received!");

		// read the String message
		#ifdef ENABLE_RSSI
			ResponseContainer rc = e220ttl.receiveMessageRSSI();
		#else
			ResponseContainer rc = e220ttl.receiveMessage();
		#endif
			// Is something goes wrong print error
			if (rc.status.code!=1){
				Serial.println(rc.status.getResponseDescription());
				return OperationResult::ERROR;
			}else{
				// Print the data received
				Serial.println(rc.status.getResponseDescription());
				Serial.println(rc.data);

				Serial.println("dest:");
				Serial.println(getDestinationAddress(fromWString(rc.data)));
				receive(getPacketMessage(fromWString(rc.data)));
		#ifdef ENABLE_RSSI
				Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
		#endif
		}
	}
	return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::receive(std::string message) {
	notifySubscribers(message);
    return OperationResult::SUCCESS;
}

void printParameters(struct Configuration configuration) {
	Serial.println("----------------------------------------");

	Serial.print(F("HEAD : "));  Serial.print(configuration.COMMAND, HEX);Serial.print(" ");Serial.print(configuration.STARTING_ADDRESS, HEX);Serial.print(" ");Serial.println(configuration.LENGHT, HEX);
	Serial.println(F(" "));
	Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, HEX);
	Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, HEX);
	Serial.println(F(" "));
	Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
	Serial.println(F(" "));
	Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
	Serial.print(F("SpeedUARTDatte     : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRateDescription());
	Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRateDescription());
	Serial.println(F(" "));
	Serial.print(F("OptionSubPacketSett: "));  Serial.print(configuration.OPTION.subPacketSetting, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getSubPacketSetting());
	Serial.print(F("OptionTranPower    : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());
	Serial.print(F("OptionRSSIAmbientNo: "));  Serial.print(configuration.OPTION.RSSIAmbientNoise, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getRSSIAmbientNoiseEnable());
	Serial.println(F(" "));
	Serial.print(F("TransModeWORPeriod : "));  Serial.print(configuration.TRANSMISSION_MODE.WORPeriod, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getWORPeriodByParamsDescription());
	Serial.print(F("TransModeEnableLBT : "));  Serial.print(configuration.TRANSMISSION_MODE.enableLBT, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getLBTEnableByteDescription());
	Serial.print(F("TransModeEnableRSSI: "));  Serial.print(configuration.TRANSMISSION_MODE.enableRSSI, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getRSSIEnableByteDescription());
	Serial.print(F("TransModeFixedTrans: "));  Serial.print(configuration.TRANSMISSION_MODE.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getFixedTransmissionDescription());

	Serial.println("----------------------------------------");
}