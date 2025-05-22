#include <utils/operation_result.h>
#include "test_transmit.h"
#include <iostream>

OperationResult TestTransmit::send(std::shared_ptr<Message> message) {
    std::cout << "lora sending\n" << message->getContent() << "\n" << std::flush;
    return OperationResult::SUCCESS;
}

OperationResult TestTransmit::receive(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}
