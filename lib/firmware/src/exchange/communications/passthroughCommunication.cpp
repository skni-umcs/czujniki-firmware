#include <string>
#include "passthroughCommunication.h"
#include <memory>
#include <utils/otherUtils.h>

const int SNR_WAIT_MULTIPLIER = 2;

std::shared_ptr<PassthroughCommunication> PassthroughCommunication::create() {
    auto s = new PassthroughCommunication();
    return std::shared_ptr<PassthroughCommunication>{s};
}

OperationResult PassthroughCommunication::rebroadcastMessage(std::shared_ptr<LoraMessage> message) {
    Serial.println("siema z pasfru");
    Serial.println(message->getHopLimit());
    message->decrementHopLimit();
    if(message->getHopLimit() > 0) {
        transmit(message);
    }
    return OperationResult::SUCCESS;
}

OperationResult messageAlreadyRebroadcasted() {
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

OperationResult PassthroughCommunication::removeMessage(std::set<std::shared_ptr<LoraMessage>>& rebroadcastedMessages, std::shared_ptr<LoraMessage> message) {
    Serial.println("removing message");
    messageSet.erase(message);
    for (auto rebroadcastedMessage : rebroadcastedMessages) {
        messageSet.erase(rebroadcastedMessage);
    }
    return OperationResult::SUCCESS;
}

OperationResult PassthroughCommunication::getNotified(std::shared_ptr<Message> message) {
    Serial.println("siema z getnotified");
    Serial.println(message->getHopLimit());
    if (message->type() != MessageType::LoraMessage) {
        Serial.println("Passthrough got non-lora message, discarding");
        return OperationResult::ERROR;
    }
    std::shared_ptr<LoraMessage> loraMessage = std::static_pointer_cast<LoraMessage>(message);

    messageSet.emplace(loraMessage);
    int passDelay = loraMessage->getSnr()*SNR_WAIT_MULTIPLIER;
    Serial.printf("czekam %i\n", passDelay);
    delay(passDelay);

    if (!setContains(messageSet, loraMessage)) {
        return messageAlreadyRebroadcasted();
    }

    std::set<std::shared_ptr<LoraMessage>> sameMessages = getSameMessages(loraMessage);

    if (sameMessages.empty()) {
        rebroadcastMessage(loraMessage);
    }
    removeMessage(sameMessages, loraMessage);
    return OperationResult::SUCCESS;
}