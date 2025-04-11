#include <string>
#include "mock_small_communication.h"
#include <memory>
#include "exchange/communications/small_communication.h"
#include "exchange/transmits/big_transmit.h"
#include "exchange/communications/big_communication.h"
#include <iostream>

std::shared_ptr<MockSmallCommunication> MockSmallCommunication::create() {
    auto s = new MockSmallCommunication();
    return std::shared_ptr<MockSmallCommunication>{s};
}

OperationResult MockSmallCommunication::getNotified(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}