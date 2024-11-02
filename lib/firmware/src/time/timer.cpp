#include "timer.h"
#include <iostream>

void printStuff () {
    std::cout << "elo z południańca";
}

void Timer::updateTime(int lastDate, int period) {
    this->recentlyUpdated = true;
    this->lastDate = lastDate;
    this->period = period;
    this->executeFunction = printStuff;
}

void Timer::onTaskDelay() {
    if(this->executeFunction != nullptr) {
        this->executeFunction();
    }
}

void Timer::onTimerUpdate() {
    this->recentlyUpdated = true;
}