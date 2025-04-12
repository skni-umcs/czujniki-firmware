#include <utils/operation_result.h>
#include <iostream>
#include <LoRa_E220.h>
#include "utils/string_utils.h"
#include <FastCRC.h>
#include <sstream>
#include <utils/address_handler.h>
#include <message/message_decode_utils.h>
#include <memory>
#include "mock_transmit.h"

std::shared_ptr<MockTransmit> MockTransmit::create() {
    auto m = new MockTransmit();
    return std::shared_ptr<MockTransmit>{m};
}


OperationResult MockTransmit::send(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}

OperationResult MockTransmit::send(std::string content, moduleAddress destinationNode) {
    return OperationResult::SUCCESS;
}

OperationResult MockTransmit::receive(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}