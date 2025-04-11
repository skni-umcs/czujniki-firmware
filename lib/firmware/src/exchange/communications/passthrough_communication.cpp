#include <string>
#include "passthrough_communication.h"
#include <memory>
#include <utils/other_utils.h>
#include "utils/address_handler.h"

const double SNR_WAIT_MULTIPLIER = 5;
const int MINIMAL_SNR = -80;

std::shared_ptr<PassthroughCommunication> PassthroughCommunication::create() {
    auto s = new PassthroughCommunication();
    return std::shared_ptr<PassthroughCommunication>{s};
}

OperationResult PassthroughCommunication::rebroadcast(std::shared_ptr<LoraMessage> message) {
    message->decrementHopLimit();
    transmit(message);
    return OperationResult::SUCCESS;
}

OperationResult alreadyRebroadcasted() {
    Serial.println("already rebro");
    return OperationResult::SUCCESS;
}

std::set<std::shared_ptr<LoraMessage>> PassthroughCommunication::getSameMessages(std::shared_ptr<LoraMessage> message) {
    std::set<std::shared_ptr<LoraMessage>> result;

    for (auto setMessage : messageSet) {
        if(message != setMessage && message->isSameMessage(setMessage)) {
            result.emplace(setMessage);
        }
    }

    return result;
}

OperationResult PassthroughCommunication::removeSameMessages(std::set<std::shared_ptr<LoraMessage>>& rebroadcastedMessages, std::shared_ptr<LoraMessage> message) {
    messageSet.erase(message);
    for (auto rebroadcastedMessage : rebroadcastedMessages) {
        messageSet.erase(rebroadcastedMessage);
    }
    return OperationResult::SUCCESS;
}

bool PassthroughCommunication::shouldRebroadcast(std::shared_ptr<LoraMessage> message) {
    return message->getHopLimit() > 0 && 
    !message->isCurrentModuleSenderPresent() && 
    message->getDestination() != AddressHandler::getInstance()->readAddress();
}

OperationResult PassthroughCommunication::getNotified(std::shared_ptr<Message> message) {
    if(!message->getIsPacketCorrect()) {
        return OperationResult::OPERATION_IGNORED;
    }
    if (message->type() != MessageType::LoraMessage) {
        Serial.println("Passthrough got non-lora message, discarding");
        return OperationResult::ERROR;
    }
    std::shared_ptr<LoraMessage> loraMessage = std::static_pointer_cast<LoraMessage>(message);

    if (shouldRebroadcast(loraMessage)) {
        messageSet.emplace(loraMessage);
        int passDelay = (int)((double)(loraMessage->getSnr()-MINIMAL_SNR)*SNR_WAIT_MULTIPLIER);
        if(passDelay < 1) {
            passDelay = 1;
        }
        Serial.printf("czekam %i\n", passDelay);
        delay(passDelay);

        if (!setContains(messageSet, loraMessage)) {
            return alreadyRebroadcasted();
        }

        std::set<std::shared_ptr<LoraMessage>> sameMessages = getSameMessages(loraMessage);

        if (sameMessages.empty()) {
            rebroadcast(loraMessage);
        }
        removeSameMessages(sameMessages, loraMessage);
        return OperationResult::SUCCESS;
    }
    return OperationResult::OPERATION_IGNORED;

}