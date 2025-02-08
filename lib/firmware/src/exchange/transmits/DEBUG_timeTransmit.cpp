#include <utils/operationResult.h>
#include "DEBUG_timeTransmit.h"
#include <iostream>
#include <LoRa_E220.h>
#include "utils/stringUtils.h"
#include <FastCRC.h>
#include <sstream>
#include <utils/addressHandler.h>
#include <utils/packetUtils.h>

void DEBUG_timeTransmit::setup() {
	Serial.println("Setupping DEBUG_timeTransmit");
}

std::shared_ptr<DEBUG_timeTransmit> DEBUG_timeTransmit::create() {
    auto loraTransmit = new DEBUG_timeTransmit();

    loraTransmit->setup();
    return std::shared_ptr<DEBUG_timeTransmit>{loraTransmit};
}

OperationResult DEBUG_timeTransmit::send(std::shared_ptr<Message> message) {
	Serial.println("Hi, I'm going to send DEBUG_timeTransmit message from ready message!");
    return OperationResult::SUCCESS;
}

OperationResult DEBUG_timeTransmit::send(std::string content, moduleAddress destinationNode) {
	Serial.println("Hi, I'm going to send DEBUG_timeTransmit message!");
    return OperationResult::SUCCESS;
}

OperationResult DEBUG_timeTransmit::poll() {
	return OperationResult::SUCCESS;
}

OperationResult DEBUG_timeTransmit::receive(std::shared_ptr<Message> message) {
	notifySubscribers(message);
    return OperationResult::SUCCESS;
}
