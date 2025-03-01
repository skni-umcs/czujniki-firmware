#ifndef SERVICECOMMUNICATION_H
#define SERVICECOMMUNICATION_H

#include <vector>
#include "jsonCommunication.h"
#include <exchange/transmits/jsonTransmit.h>

#define DIDNT_ASK -1

class ServiceCommunication : public JsonCommunication {
    
    unsigned long lastAskTime = DIDNT_ASK;

    protected:
        OperationResult setAskForTimeTask();
    public:
        static std::shared_ptr<ServiceCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
        void sendResetReason();
        OperationResult askForTime();
        OperationResult updateTime(unsigned long serverTime);
        unsigned long getLastAskTime();
        OperationResult setLastAskTime(unsigned long lastAskTime);

};

#endif