#include <utils/operation_result.h>
#include "lora_transmit.h"
#include <LoRa_E220.h>
#include "utils/string_utils.h"
#include <FastCRC.h>
#include <sstream>
#include <utils/address_handler.h>
#include "message/message_decode_utils.h"
#include <utils/logger.h>

void printParameters(struct Configuration configuration);

#define MAX_QUEUE 20

// With FIXED RECEIVER configuration
#define DESTINATION_ADDL 2
// If you want use RSSI uncomment
#define ENABLE_RSSI true

const int DEFAULT_LORA_POLL_MS = 100;
const int DEFAULT_NOISE_UPDATE_MS = 5*60000;
const int MAX_MESSAGE_ADVANCE_MS = 60*60000;
const int CHANNEL = 39;
const unsigned char HOP_DISCARD_LIMIT = 0;

OperationResult LoraTransmit::updateNoise() {
	Logger::log("Noise update");
	unsigned short RSSIAmbient = e220ttl.readRSSIAmbient();

	if (RSSIAmbient == RSSI_AMBIENT_ERROR_RESPONSE) {
		Logger::log("Invalid noise");
		return OperationResult::ERROR;
	}

	byte localNoise = RSSIAmbient >> 8;
	noiseRaw = localNoise;
	Logger::logf("Noise (raw): %i\n",noiseRaw);

	return OperationResult::SUCCESS;
}

int LoraTransmit::getSnr(int readRssi) {
	int RssidB = -((256)-readRssi);
	int noisedB = -((256)-noiseRaw);
	if(noisedB == 0) {
		Logger::log("Error, realNoise is 0");
		return -1;
	}
	Logger::log("rssi rzeczy");
	Logger::log(readRssi);
	Logger::log(RssidB);
	Logger::log(noisedB);
	return RssidB-noisedB;
}

int LoraTransmit::getNoise() {
	return noiseRaw;
}

void LoraTransmit::setup() {
	Logger::log("Setupping LoraTransmit");

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

std::shared_ptr<LoraTransmit> LoraTransmit::create(std::shared_ptr<WifiTransmit> DEBUG_wifi) {
    auto loraTransmit = new LoraTransmit();
	loraTransmit->DEBUG_wifi = DEBUG_wifi;

    loraTransmit->setup();

    loraTransmit->pollTimer.get()->setExecuteFunction([loraTransmit]() {
       loraTransmit->poll();
    });
    loraTransmit->pollTimer.get()->updateTime(DEFAULT_LORA_POLL_MS);

	loraTransmit->noiseUpdateTimer.get()->setExecuteFunction([loraTransmit]() {
		loraTransmit->updateNoise();
	});
	loraTransmit->noiseUpdateTimer.get()->updateTime(DEFAULT_NOISE_UPDATE_MS);

	loraTransmit->sendWaiter.get()->setExecuteFunction([loraTransmit]() {
		loraTransmit->advanceMessages();
	});
	loraTransmit->sendWaiter.get()->updateTime(1000);
	loraTransmit->sendWaiter.get()->changeTimerTask();
    return std::shared_ptr<LoraTransmit>{loraTransmit};
}

OperationResult LoraTransmit::physicalSend(std::shared_ptr<Message> message) {
	DEBUG_wifi->send(message);
	std::string packet = message.get()->createPacketForSending();
	Logger::logf("SEND %s\n", packet.c_str());
	transmitCount++;
	ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(CHANNEL, packet.c_str());
	return OperationResult::SUCCESS;
}

double calculateToA(
    int size,
    int sf,
    int bw,
    std::string codingRate = "4/5",
    std::string lowDrOptimize = "auto",
    bool explicitHeader = true,
    int preambleLength = 8
) {
    // Wszystkie czasy w milisekundach
    double tSym = (pow(2, sf) / (bw * 1000.0)) * 1000.0;
    double tPreamble = (preambleLength + 4.25) * tSym;
    
    // H = 0 gdy nagłówek jest włączony, H = 1 gdy nagłówek nie występuje
    int h = explicitHeader ? 0 : 1;
    
    // DE = 1 gdy optymalizacja niskiej przepływności jest włączona, DE = 0 gdy wyłączona
    // W trybie 'auto' tylko dla SF11 i SF12 przy paśmie 125kHz
    int de = 0;
    if ((lowDrOptimize == "auto" && bw == 125 && sf >= 11) || lowDrOptimize == "true") {
        de = 1;
    }
    
    // CR to współczynnik kodowania od 1 do 4
    int cr = codingRate[2] - '0' - 4;
    
    int payloadSymbNb = 8 + 
        std::max(
            static_cast<int>(
                ceil((8.0 * size - 4.0 * sf + 28.0 + 16.0 - 20.0 * h) / (4.0 * (sf - 2.0 * de)))
            ) * (cr + 4),
            0
        );
        
    double tPayload = payloadSymbNb * tSym;
    return tPreamble + tPayload;
}

int airTime(std::shared_ptr<Message> message) {
	int length = message.get()->createPacketForSending().size();
	int waitTime = calculateToA(length, 7, 125)*99;
	Logger::logf("WAIT TIME: %d\n", waitTime);
	return waitTime;
}

OperationResult LoraTransmit::advanceMessages() {
	if(messages.size() > 0) {
		std::shared_ptr<Message> message = messages.front();
		messages.pop_front();
		physicalSend(message);
		canTransmit = false;
		sendWaiter.get()->updateTime(airTime(message));
	}
	else {
		canTransmit = true;
		sendWaiter.get()->updateTime(1000);
	}
	return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::scheduleMessage(std::shared_ptr<Message> message) {
	if(getWaitingMessagesCount() <= MAX_QUEUE) {
		Logger::logf("SCHEDULE SEND %s\n", message->createPacketForSending().c_str());
		messages.push_back(message);
	}
	/*if(canTransmit) {
		advanceMessages();
	}*/
	return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::send(std::shared_ptr<Message> message) {
	scheduleMessage(message);
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
				Logger::log(rc.status.getResponseDescription());
				return OperationResult::ERROR;
			}else{
				byte rssi = rc.rssi;
				int snr = getSnr((int)rssi);
				auto loraMessage = std::shared_ptr<LoraMessage>(new LoraMessage(fromWString(rc.data), rssi, snr));
				receive(loraMessage);
		#ifdef ENABLE_RSSI
				//Logger::log("RSSI: "); Logger::log(rc.rssi, DEC);
		#endif
		}
	}
	return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::receive(std::shared_ptr<Message> message) {
	Logger::logf("RECEIVE %s\n", message->getPacket().c_str());
	notifySubscribers(message);
    return OperationResult::SUCCESS;
}

bool LoraTransmit::getCanTransmit() {
	return canTransmit;
}

int LoraTransmit::getWaitingMessagesCount() {
	return messages.size();
}

void printParameters(struct Configuration configuration) {
	Logger::log("----------------------------------------");

	Logger::log(F("HEAD : "));  Logger::log(configuration.COMMAND, HEX);Logger::log(" ");Logger::log(configuration.STARTING_ADDRESS, HEX);Logger::log(" ");Logger::log(configuration.LENGHT, HEX);
	Logger::log(F(" "));
	Logger::log(F("AddH : "));  Logger::log(configuration.ADDH, HEX);
	Logger::log(F("AddL : "));  Logger::log(configuration.ADDL, HEX);
	Logger::log(F(" "));
	Logger::log(F("Chan : "));  Logger::log(configuration.CHAN, DEC); Logger::log(" -> "); Logger::log(configuration.getChannelDescription());
	Logger::log(F(" "));
	Logger::log(F("SpeedParityBit     : "));  Logger::log(configuration.SPED.uartParity, BIN);Logger::log(" -> "); Logger::log(configuration.SPED.getUARTParityDescription());
	Logger::log(F("SpeedUARTDatte     : "));  Logger::log(configuration.SPED.uartBaudRate, BIN);Logger::log(" -> "); Logger::log(configuration.SPED.getUARTBaudRateDescription());
	Logger::log(F("SpeedAirDataRate   : "));  Logger::log(configuration.SPED.airDataRate, BIN);Logger::log(" -> "); Logger::log(configuration.SPED.getAirDataRateDescription());
	Logger::log(F(" "));
	Logger::log(F("OptionSubPacketSett: "));  Logger::log(configuration.OPTION.subPacketSetting, BIN);Logger::log(" -> "); Logger::log(configuration.OPTION.getSubPacketSetting());
	Logger::log(F("OptionTranPower    : "));  Logger::log(configuration.OPTION.transmissionPower, BIN);Logger::log(" -> "); Logger::log(configuration.OPTION.getTransmissionPowerDescription());
	Logger::log(F("OptionRSSIAmbientNo: "));  Logger::log(configuration.OPTION.RSSIAmbientNoise, BIN);Logger::log(" -> "); Logger::log(configuration.OPTION.getRSSIAmbientNoiseEnable());
	Logger::log(F(" "));
	Logger::log(F("TransModeWORPeriod : "));  Logger::log(configuration.TRANSMISSION_MODE.WORPeriod, BIN);Logger::log(" -> "); Logger::log(configuration.TRANSMISSION_MODE.getWORPeriodByParamsDescription());
	Logger::log(F("TransModeEnableLBT : "));  Logger::log(configuration.TRANSMISSION_MODE.enableLBT, BIN);Logger::log(" -> "); Logger::log(configuration.TRANSMISSION_MODE.getLBTEnableByteDescription());
	Logger::log(F("TransModeEnableRSSI: "));  Logger::log(configuration.TRANSMISSION_MODE.enableRSSI, BIN);Logger::log(" -> "); Logger::log(configuration.TRANSMISSION_MODE.getRSSIEnableByteDescription());
	Logger::log(F("TransModeFixedTrans: "));  Logger::log(configuration.TRANSMISSION_MODE.fixedTransmission, BIN);Logger::log(" -> "); Logger::log(configuration.TRANSMISSION_MODE.getFixedTransmissionDescription());

	Logger::log("----------------------------------------");
}

int LoraTransmit::getTransmitCount() {
	return transmitCount;
}