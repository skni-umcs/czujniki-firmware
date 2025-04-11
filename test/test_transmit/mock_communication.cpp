#include <string>
#include "mock_communication.h"
#include <memory>
#include "exchange/communications/small_communication.h"
#include "exchange/transmits/big_transmit.h"
#include "exchange/communications/big_communication.h"
#include <iostream>

int sensorTransmits = 0;

std::shared_ptr<MockCommunication> MockCommunication::create() {
    auto s = new MockCommunication();
    std::cout << "EEEEEEEEEEEEEEEEEEEEEEEEEEEE" << std::flush;
    return std::shared_ptr<MockCommunication>{s};
}

OperationResult MockCommunication::getNotified(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}