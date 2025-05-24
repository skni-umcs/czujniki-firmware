#ifndef SERVICECOMMUNICATION_H
#define SERVICECOMMUNICATION_H

#include <vector>
#include "small_communication.h"
#include <exchange/transmits/small_transmit.h>
#include "time/timer.h"
#include "time/time_constants.h"

#define DIDNT_ASK -1

static int DEFAULT_ASK_TIMEOUT_MS = 10000;
static int DEFAULT_TIME_SYNC_PERIOD_MS = FULL_WEEK_MS;

class ServiceCommunication : public SmallCommunication {
    
    unsigned long lastAskTime = DIDNT_ASK;
    std::shared_ptr<Timer> askTimeTimeoutTimer = Timer::create();
    std::shared_ptr<Timer> timeSyncTimer = Timer::create();
    int askTimeoutMs = DEFAULT_ASK_TIMEOUT_MS;
    int timeSyncPeriodMs = DEFAULT_TIME_SYNC_PERIOD_MS;

    protected:
        OperationResult setAskForTimeTask();
        ServiceCommunication() {};

    public:
        static std::shared_ptr<ServiceCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
        void sendResetReason();
        OperationResult askForTime();
        OperationResult updateTime(unsigned long serverTime);
        unsigned long getLastAskTime();
        OperationResult setLastAskTime(unsigned long lastAskTime);
        OperationResult setAskTimeoutMs(int askTimeoutMs);
        OperationResult setTimeSyncPeriodMs(int timeSyncPeriodMs);
        int getAskTimeoutMs();
        int getTimeSyncPeriodMs();

};

#endif