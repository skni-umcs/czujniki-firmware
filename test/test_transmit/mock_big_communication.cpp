#include <string>
#include "mock_big_communication.h"
#include <memory>
#include "exchange/communications/small_communication.h"
#include "exchange/transmits/big_transmit.h"
#include "exchange/communications/big_communication.h"
#include <iostream>

int sensorTransmits = 0;

std::shared_ptr<MockBigCommunication> MockBigCommunication::create() {
    auto s = new MockBigCommunication();
    return std::shared_ptr<MockBigCommunication>{s};
}

OperationResult MockBigCommunication::getNotified(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}