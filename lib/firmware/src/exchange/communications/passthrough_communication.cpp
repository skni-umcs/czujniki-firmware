#include <string>
#include "passthrough_communication.h"
#include <memory>
#include <utils/other_utils.h>
#include "utils/address_handler.h"
#include <utils/logger.h>
#include <exchange/transmits/lora_transmit.h>

#define MAX_LORA_QUEUE_PASSTHROUGH 5

const double SNR_WAIT_MULTIPLIER = 500;
const int MINIMAL_SNR = -80;
const int MAXMIMUM_PASSTHROUGH_MESSAGES = 50;

std::shared_ptr<PassthroughCommunication> PassthroughCommunication::create() {
    auto passthroughCommunication = new PassthroughCommunication();
    return std::shared_ptr<PassthroughCommunication>{passthroughCommunication};
}

OperationResult PassthroughCommunication::rebroadcast(std::shared_ptr<LoraMessage> message) {
    message->decrementHopLimit();
    //TODO: scheduling transmit is not enough because you should wait as long as possible to make sure the message wasnt received from another node
    transmit(message);
    return OperationResult::SUCCESS;
}

//TODO: remove
OperationResult PassthroughCommunication::transmit(std::string message, moduleAddress destinationNode) {
    for(auto const& destination : transmitTo) {
        if(destination->type() == TransmitType::LoraTransmit) {
            std::shared_ptr<LoraTransmit> loraTransmit = std::static_pointer_cast<LoraTransmit>(destination);
            if(loraTransmit->getWaitingMessagesCount() <= MAX_LORA_QUEUE_PASSTHROUGH) {
                destination->send(GeneratedMessage::fromText(message, destinationNode));
            }
        }
        else {
            Logger::log("Passthrough only accepts lora transmit!");
        }
    }
    return OperationResult::SUCCESS;
}

OperationResult PassthroughCommunication::transmit(std::shared_ptr<Message> message) {
    for(auto const& destination : transmitTo) {
        if(destination->type() == TransmitType::LoraTransmit) {
            std::shared_ptr<LoraTransmit> loraTransmit = std::static_pointer_cast<LoraTransmit>(destination);
            if(loraTransmit->getWaitingMessagesCount() <= MAX_LORA_QUEUE_PASSTHROUGH) {
                destination->send(message);
            }
        }
        else {
            Logger::log("Passthrough only accepts lora transmit!");
        }
    }
    return OperationResult::SUCCESS;
}

OperationResult alreadyRebroadcasted() {
    Logger::log("already rebro");
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
    message->setShouldTransmit(false);
    messageSet.erase(message);
    for (auto rebroadcastedMessage : rebroadcastedMessages) {
        rebroadcastedMessage->setShouldTransmit(false);
        messageSet.erase(rebroadcastedMessage);
    }
    return OperationResult::SUCCESS;
}

bool PassthroughCommunication::shouldRebroadcast(std::shared_ptr<LoraMessage> message) {
    return message->getHopLimit() > 0 && 
    !message->isCurrentModuleSenderPresent() && 
    message->getDestination() != AddressHandler::getInstance()->readAddress();
}

OperationResult PassthroughCommunication::afterWait(std::shared_ptr<LoraMessage> loraMessage) {
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

OperationResult PassthroughCommunication::updateSetFromNewMessage(std::shared_ptr<Message> message) {
    //vTaskSuspendAll();
    std::vector<std::shared_ptr<LoraMessage>> toErase;
    for (const auto& oldMessage : messageSet) {
        if (oldMessage->getWasTransmitted()) {
            toErase.push_back(oldMessage);
        }
        else if(oldMessage->isSameMessage(message)) {
            oldMessage->setShouldTransmit(false);
            toErase.push_back(oldMessage);
        }
        else if(messageSet.size()-toErase.size() > MAXMIMUM_PASSTHROUGH_MESSAGES) {
            toErase.push_back(oldMessage);
        }
    }
    for (const auto& key : toErase) {
        messageSet.erase(key);
    }
    //xTaskResumeAll();
    return OperationResult::SUCCESS;
}

OperationResult PassthroughCommunication::getNotified(std::shared_ptr<Message> message) {
    if(!message->getIsPacketCorrect()) {
        return OperationResult::OPERATION_IGNORED;
    }
    if (message->type() != MessageType::LoraMessage) {
        Logger::log("Passthrough got non-lora message, discarding");
        return OperationResult::ERROR;
    }
    updateSetFromNewMessage(message);
    std::shared_ptr<LoraMessage> loraMessage = std::static_pointer_cast<LoraMessage>(message);

    Logger::logf("REBROADCAST CONDITION: %d", shouldRebroadcast(loraMessage));
    if (shouldRebroadcast(loraMessage)) {
        messageSet.emplace(loraMessage);
        int passDelay = (int)((double)(loraMessage->getSnr()-MINIMAL_SNR)*SNR_WAIT_MULTIPLIER);
        if(passDelay < 1) {
            passDelay = 1;
        }
        Logger::logf("PASSTHROUGH WAIT %i s\n", passDelay);

        //TODO: rewrite to receive task and send task
        auto waiter = Waiter::create();
        waiter.get()->setExecuteFunction([this, loraMessage, waiter]() {
            this->afterWait(loraMessage);
            this->waiters.erase(waiter);
        });
        waiter.get()->updateTime(passDelay);
        waiter.get()->changeTimerTask();
        waiters.emplace(waiter);

        return OperationResult::SUCCESS;
    }
    return OperationResult::OPERATION_IGNORED;

}

int PassthroughCommunication::getMessageSetLength() {
    return messageSet.size();
}