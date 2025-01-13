#include "timerUpdate.h"
#include <ESP32Time.h>
#include "timeConstants.h"

std::vector<std::shared_ptr<Timer>> TimerUpdate::subscribers;

void TimerUpdate::setTime(int time) {
    rtc.setTime(time);
    TimerUpdate::notifySubscribers();
}

void TimerUpdate::notifySubscribers() {
    for(auto &subscriber : TimerUpdate::subscribers) {
        subscriber.get()->onTimerUpdate();
    }
}

void TimerUpdate::addSubscriber(std::shared_ptr<Timer> timer) {
    TimerUpdate::subscribers.push_back(timer);
}
