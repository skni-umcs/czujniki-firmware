#include <utils/operationResult.h>
#include "loraTransmit.h"
#include <iostream>
#include <LoRa_E220.h>
#include "utils/stringUtils.h"
#include <FastCRC.h>
#include <sstream>
#include <utils/addressHandler.h>
#include <utils/packetUtils.h>

void printParameters(struct Configuration configuration);

// With FIXED RECEIVER configuration
#define DESTINATION_ADDL 2
// If you want use RSSI uncomment
#define ENABLE_RSSI true

const int DEFAULT_LORA_POLL_MS = 100;
const int DEFAULT_NOISE_UPDATE_MS = 5*60000;
const int CHANNEL = 39;
const unsigned char HOP_START_LIMIT = 100;
const unsigned char HOP_DISCARD_LIMIT = 0;

OperationResult LoraTransmit::updateNoise() {
	Serial.println("Noise update");
	unsigned short RSSIAmbient = e220ttl.readRSSIAmbient();
	byte localNoise = RSSIAmbient >> 8;

	if (localNoise == -1) {
		Serial.println("Invalid noise");
		return OperationResult::ERROR;
	}
	noiseRaw = localNoise;
	Serial.printf("Noise (raw): %i\n",noiseRaw);

	return OperationResult::SUCCESS;
}

int LoraTransmit::getSnr(int readRssi) {
	int RssidB = -((256)-readRssi);
	int noisedB = -((256)-noiseRaw);
	if(noisedB == 0) {
		Serial.println("Error, realNoise is 0");
		return -1;
	}
	Serial.println("rssi rzeczy");
	Serial.println(readRssi);
	Serial.println(RssidB);
	Serial.println(noisedB);
	return RssidB-noisedB;
}

int LoraTransmit::getNoise() {
	return noiseRaw;
}

void LoraTransmit::setup() {
	Serial.println("Setupping LoraTransmit");

	// Startup all pins and UART
	e220ttl.begin();

	moduleAddress address = AddressHandler::getInstance()->readAddress();

    ResponseStructContainer c = ResponseStructContainer();
	c = e220ttl.getConfiguration();
	// It's important get configuration pointer before all other operation
	Configuration configuration = *(Configuration*) c.data;
	configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION; // Enable repeater mode
	configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_ENABLED; // Need to send special command
	configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED; // Enable RSSI info
	configuration.OPTION.transmissionPower = POWER_10;
	configuration.CHAN = CHANNEL;
	configuration.ADDL = address & 0x00ff;
	configuration.ADDH = address & 0xff00;
	ResponseStatus rs = e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);

	printParameters(configuration);

	c.close();

	updateNoise();
}

std::shared_ptr<LoraTransmit> LoraTransmit::create() {
    auto loraTransmit = new LoraTransmit();

    loraTransmit->pollTimer.get()->setExecuteFunction([loraTransmit]() {
       loraTransmit->poll();
    });
    loraTransmit->pollTimer.get()->updateTime(DEFAULT_LORA_POLL_MS);

	loraTransmit->noiseUpdateTimer.get()->setExecuteFunction([loraTransmit]() {
		loraTransmit->updateNoise();
	});
	loraTransmit->noiseUpdateTimer.get()->updateTime(DEFAULT_NOISE_UPDATE_MS);

    loraTransmit->setup();
    return std::shared_ptr<LoraTransmit>{loraTransmit};
}

OperationResult LoraTransmit::send(std::shared_ptr<Message> message) {
	std::string packet = message.get()->createPacketForSending();
	Serial.printf("SEND (ready) %s\n", packet.c_str());
	ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(CHANNEL, packet.c_str());
    return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::send(std::string content, moduleAddress destinationNode) {
	auto senders = std::vector<moduleAddress>();
	auto rssi = std::vector<std::string>();
	std::shared_ptr<GeneratedMessage> message = std::shared_ptr<GeneratedMessage>(new GeneratedMessage(
		senders,
		destinationNode,
		content,
		rssi,
		HOP_START_LIMIT
	));
	send(message);
    return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::poll() {
	if (e220ttl.available()>1) {

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
				byte rssi = rc.rssi;
				int snr = getSnr((int)rssi);
				auto loraMessage = std::shared_ptr<LoraMessage>(new LoraMessage(fromWString(rc.data), rssi, snr));
				receive(loraMessage);
		#ifdef ENABLE_RSSI
				//Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
		#endif
		}
	}
	return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::receive(std::shared_ptr<Message> message) {
	Serial.printf("RECEIVE %s\n", message->getPacket().c_str());
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