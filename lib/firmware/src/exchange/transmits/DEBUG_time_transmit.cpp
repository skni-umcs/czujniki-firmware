#include <utils/operation_result.h>
#include "DEBUG_time_transmit.h"
#include <iostream>
#include <LoRa_E220.h>
#include "utils/string_utils.h"
#include <FastCRC.h>
#include <sstream>
#include <utils/address_handler.h>
#include "message/message_decode_utils.h"
#include <memory>
#include <utils/logger.h>

void DEBUG_timeTransmit::setup() {
	Logger::log("Setupping DEBUG_timeTransmit");
}

std::shared_ptr<DEBUG_timeTransmit> DEBUG_timeTransmit::create() {
    auto loraTransmit = new DEBUG_timeTransmit();

    loraTransmit->setup();
    return std::shared_ptr<DEBUG_timeTransmit>{loraTransmit};
}

OperationResult DEBUG_timeTransmit::send(std::shared_ptr<Message> message) {
	Logger::log("Hi, I'm going to send DEBUG_timeTransmit message from ready message message!");
	Logger::log(message->createPacketForSending().c_str());
    return OperationResult::SUCCESS;
}

OperationResult DEBUG_timeTransmit::poll() {
	return OperationResult::SUCCESS;
}

OperationResult DEBUG_timeTransmit::receive(std::shared_ptr<Message> message) {
	Logger::log("I receive DEBUG_timeTransmit message message!");
	Logger::log(message->createPacket().c_str());
	notifySubscribers(message);
    return OperationResult::SUCCESS;
}
