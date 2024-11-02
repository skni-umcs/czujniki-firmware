#ifndef TIMER_UPDATE_H
#define TIMER_UPDATE_H

#include <string>
#include "timer.h"
#include <vector>
#include <memory>

class TimerUpdate
{
    static TimerUpdate timerUpdate;
    static std::vector<std::shared_ptr<Timer>> subscribers;

    public:
        static void notifySubscribers();
        static void addSubscriber(std::weak_ptr<Timer> t);
        static void setTime(int time);
};

#endif