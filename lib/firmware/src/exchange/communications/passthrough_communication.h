#ifndef PASSTHROUGHCOMMUNICATION_H
#define PASSTHROUGHCOMMUNICATION_H

#include "small_communication.h"
#include <set>
#include "time/timer.h"

class PassthroughCommunication : public SmallCommunication {
    
    std::set<std::shared_ptr<LoraMessage>> messageSet;
    std::shared_ptr<Timer> timer; //TODO: check if its used anywhere
    
    public:
        static std::shared_ptr<PassthroughCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
        std::set<std::shared_ptr<LoraMessage>> getSameMessages(std::shared_ptr<LoraMessage> message);
        OperationResult removeSameMessages(std::set<std::shared_ptr<LoraMessage>>& rebroadcastedMessages, std::shared_ptr<LoraMessage> message);
        OperationResult rebroadcast(std::shared_ptr<LoraMessage> message);
        bool shouldRebroadcast(std::shared_ptr<LoraMessage> message);
};

#endif