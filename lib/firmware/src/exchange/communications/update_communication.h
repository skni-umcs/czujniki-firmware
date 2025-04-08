#ifndef UPDATECOMMUNICATION_H
#define UPDATECOMMUNICATION_H

#include "big_communication.h"
#include <set>
#include "time/timer.h"

class UpdateCommunication : public BigCommunication {
    
    public:
        static std::shared_ptr<UpdateCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
        OperationResult update();
};

#endif