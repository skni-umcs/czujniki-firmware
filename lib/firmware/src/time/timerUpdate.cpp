#include "timerUpdate.h"
#include <ESP32Time.h>

ESP32Time rtc(3600);  // offset in seconds GMT+1

void TimerUpdate::setTime(int time) {
    rtc.setTime(time);
    TimerUpdate::notifySubscribers();
}

void TimerUpdate::notifySubscribers() {
    for(auto &subscriber : TimerUpdate::subscribers) {
        subscriber.get()->onTimerUpdate();
    }
}

void TimerUpdate::addSubscriber(std::weak_ptr<Timer> timer) {
    TimerUpdate::subscribers.push_back(timer.lock());
}
