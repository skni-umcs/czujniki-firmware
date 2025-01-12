#include <string>
#include "passthroughCommunication.h"
#include <memory>
#include <utils/otherUtils.h>

const int SNR_WAIT_MULTIPLIER = 2;

std::shared_ptr<PassthroughCommunication> PassthroughCommunication::create() {
    auto s = new PassthroughCommunication();
    return std::shared_ptr<PassthroughCommunication>{s};
}

void PassthroughCommunication::handleQueueMessages() {
    if (messageSet.empty()) {
        return;
    }
}

OperationResult rebroadcastMessage(std::shared_ptr<LoraMessage> message) {
    Serial.println("rebro");
    return OperationResult::SUCCESS;
}

OperationResult messageAlreadyRebroadcasted() {
    Serial.println("already rebro");
    return OperationResult::SUCCESS;
}

std::set<std::shared_ptr<LoraMessage>> PassthroughCommunication::getRebroadcastedMessages(std::shared_ptr<LoraMessage> message) {
    std::set<std::shared_ptr<LoraMessage>> result;

    for (auto setMessage : messageSet) {
        if(message->isSameMessage(setMessage)) {
            result.emplace(setMessage);
        }
    }

    return result;
}

OperationResult PassthroughCommunication::removeMessage(std::set<std::shared_ptr<LoraMessage>>& rebroadcastedMessages, std::shared_ptr<LoraMessage> message) {
    Serial.println("removing message");
    messageSet.erase(message);
    for (auto rebroadcastedMessage : rebroadcastedMessages) {
        messageSet.erase(rebroadcastedMessage);
    }
    return OperationResult::SUCCESS;
}

OperationResult PassthroughCommunication::getNotified(std::shared_ptr<Message> message) {
    if (message->type() != MessageType::LoraMessage) {
        Serial.println("Passthrough got non-lora message, discarding");
        return OperationResult::ERROR;
    }
    std::shared_ptr<LoraMessage> loraMessage = std::static_pointer_cast<LoraMessage>(message);

    messageSet.emplace(loraMessage);
    Serial.println("czekam");
    delay(loraMessage->getSnr()*SNR_WAIT_MULTIPLIER);

    if (!setContains(messageSet, loraMessage)) {
        return messageAlreadyRebroadcasted();
    }

    std::set<std::shared_ptr<LoraMessage>> rebroadcastedMessages = getRebroadcastedMessages(loraMessage);

    if (rebroadcastedMessages.empty()) {
        rebroadcastMessage(loraMessage);
    }
    else {
        removeMessage(rebroadcastedMessages, loraMessage);
    }
    return OperationResult::SUCCESS;
}