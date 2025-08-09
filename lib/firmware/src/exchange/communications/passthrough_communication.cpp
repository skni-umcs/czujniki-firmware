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

std::vector<std::shared_ptr<LoraMessage>> PassthroughCommunication::getSameMessages(std::shared_ptr<LoraMessage> message) {
    std::vector<std::shared_ptr<LoraMessage>> result;

    for (auto setMessage : messageSet) {
        if(message != setMessage && message->isSameMessage(setMessage)) {
            result.emplace(result.begin(), setMessage);
        }
    }

    return result;
}

OperationResult PassthroughCommunication::removeSameMessages(std::vector<std::shared_ptr<LoraMessage>>& rebroadcastedMessages, std::shared_ptr<LoraMessage> message) {
    message->setShouldTransmit(false);
    vectorErase(messageSet, message);
    for (auto rebroadcastedMessage : rebroadcastedMessages) {
        rebroadcastedMessage->setShouldTransmit(false);
        vectorErase(messageSet, rebroadcastedMessage);
    }
    return OperationResult::SUCCESS;
}

bool PassthroughCommunication::shouldRebroadcast(std::shared_ptr<LoraMessage> message) {
    return message->getHopLimit() > 0 && 
    !message->isCurrentModuleSenderPresent() && 
    message->getDestination() != AddressHandler::getInstance()->readAddress();
}

OperationResult PassthroughCommunication::rebroadcastAfterWait(std::shared_ptr<LoraMessage> loraMessage) {
    Logger::logf("rebroadcastAfterWait for %s", loraMessage->getPacket().c_str());
    if (!vectorContains(messageSet, loraMessage)) {
        return alreadyRebroadcasted();
    }
    std::shared_ptr<LoraMessage> lastLoraMessage = messageSet.back();
    Logger::logf("rebroadcastAfterWait removing message %s", lastLoraMessage->getPacket().c_str());
    int debugSizeBefore = messageSet.size();
    messageSet.pop_back();
    int debugSizeAfter = messageSet.size();
    Logger::logf("Popped up, messageSet decreased from %d to %d", debugSizeBefore, debugSizeAfter);
    std::vector<std::shared_ptr<LoraMessage>> sameMessages = getSameMessages(loraMessage);

    if (sameMessages.empty()) {
        rebroadcast(loraMessage);
    }
    removeSameMessages(sameMessages, loraMessage);
    return OperationResult::SUCCESS;
}

OperationResult PassthroughCommunication::processNewMessage() {
    std::shared_ptr<LoraMessage> loraMessage = messageSet.back();
    Logger::logf("PASSTHROUGH process new message %s", loraMessage->getPacket().c_str());
    int passDelay = (int)((double)(loraMessage->getSnr()-MINIMAL_SNR)*SNR_WAIT_MULTIPLIER);
    if(passDelay < 1) {
        passDelay = 1;
    }
    Logger::logf("PASSTHROUGH WAIT %i ms\n", passDelay);


    sendWaiter.get()->setExecuteFunction([this, loraMessage]() {
        Logger::log("PASSTHROUGH execute afterwait");
        this->rebroadcastAfterWait(loraMessage);
        this->ponderAfterWait(true);
    });
    sendWaiter.get()->updateTime(passDelay/15); //TODO: FIX /15
    sendWaiter.get()->changeTimerTask();
    return OperationResult::SUCCESS;
}

OperationResult PassthroughCommunication::ponderAfterWait(bool isLoop) {
    Logger::logf("ponderAfterWait %d %d %d", isLoop, isOldLoopActive, messageSet.empty());
    if (!isOldLoopActive || isLoop) {
        if (!messageSet.empty()) {
            isOldLoopActive = true;
            processNewMessage();
        }
        else {
            Logger::logf("PASSTHROUGH set empty for isLoop: %d", isLoop);
            isOldLoopActive = false;
            sendWaiter->updateTime(1000000000);
            sendWaiter->changeTimerTask();
        }
    }
    return OperationResult::SUCCESS;
}

OperationResult PassthroughCommunication::updateSetFromNewMessage(std::shared_ptr<Message> message) {
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
            //oldMessage->setShouldTransmit(false); //TODO: uncomment
            toErase.push_back(oldMessage);
        }
    }
    Logger::logf("PASSTHROUGH ERASE %d messages", toErase.size());
    for (const auto& key : toErase) {
        vectorErase(messageSet, key);
    }
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
        messageSet.emplace(messageSet.begin(), loraMessage);
        int size = messageSet.size();
        Logger::logf("Emplaced message in passthrough, size: %d", size);
        ponderAfterWait(false);
        return OperationResult::SUCCESS;
    }
    return OperationResult::OPERATION_IGNORED;

}

int PassthroughCommunication::getMessageSetLength() {
    return messageSet.size();
}

bool PassthroughCommunication::getIsSendWaiting() {
    return isOldLoopActive;
}