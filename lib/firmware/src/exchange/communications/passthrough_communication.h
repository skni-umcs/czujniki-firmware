#ifndef PASSTHROUGHCOMMUNICATION_H
#define PASSTHROUGHCOMMUNICATION_H

#include "small_communication.h"
#include <set>
#include "time/waiter.h"

class PassthroughCommunication : public SmallCommunication {

    //TODO: replace with a priority queue
    std::set<std::shared_ptr<LoraMessage>> messageSet;
    std::set<std::shared_ptr<Waiter>> waiters;

    protected:
        PassthroughCommunication() {};

    public:
        static std::shared_ptr<PassthroughCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
        std::set<std::shared_ptr<LoraMessage>> getSameMessages(std::shared_ptr<LoraMessage> message);
        OperationResult updateSetFromNewMessage(std::shared_ptr<Message> message);
        OperationResult removeSameMessages(std::set<std::shared_ptr<LoraMessage>>& rebroadcastedMessages, std::shared_ptr<LoraMessage> message);
        OperationResult rebroadcast(std::shared_ptr<LoraMessage> message);
        OperationResult transmit(std::string message, moduleAddress destinationNode) override;
        OperationResult transmit(std::shared_ptr<Message> message) override;
        bool shouldRebroadcast(std::shared_ptr<LoraMessage> message);
        OperationResult afterWait(std::shared_ptr<LoraMessage> loraMessage);
        int getMessageSetLength();
};

#endif