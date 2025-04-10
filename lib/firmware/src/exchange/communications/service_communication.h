#ifndef SERVICECOMMUNICATION_H
#define SERVICECOMMUNICATION_H

#include <vector>
#include "small_communication.h"
#include <exchange/transmits/json_transmit.h>
#include "time/timer.h"

#define DIDNT_ASK -1

class ServiceCommunication : public SmallCommunication {
    
    unsigned long lastAskTime = DIDNT_ASK;
    std::shared_ptr<Timer> askTimeTimeoutTimer = Timer::create();;

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