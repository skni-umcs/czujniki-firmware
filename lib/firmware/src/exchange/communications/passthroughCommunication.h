#ifndef PASSTHROUGHCOMMUNICATION_H
#define PASSTHROUGHCOMMUNICATION_H

#include "jsonCommunication.h"
#include <set>
#include "time/timer.h"

class PassthroughCommunication : public JsonCommunication {
    
    std::set<std::shared_ptr<LoraMessage>> messageSet;
    std::shared_ptr<Timer> timer;
    
    public:
        static std::shared_ptr<PassthroughCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
        void handleQueueMessages();
        std::set<std::shared_ptr<LoraMessage>> getSameMessages(std::shared_ptr<LoraMessage> message);
        OperationResult removeMessage(std::set<std::shared_ptr<LoraMessage>>& rebroadcastedMessages, std::shared_ptr<LoraMessage> message);
};

#endif