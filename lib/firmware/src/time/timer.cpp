#include "timer.h"
#include <iostream>



void printStuff () {
    std::cout << "elo z południańca" << std::endl;
}

void timerTask(void* timerObjectRawPointer) {
    Timer* timer = (Timer*)timerObjectRawPointer;
    while(1) {
        if(timer->getExecuteFunction() != nullptr && !timer->getRecentlyUpdated()) {
            timer->getExecuteFunction()();
        }
        timer->setRecentlyUpdated(false);
        vTaskDelay(timer->getPeriod());
    }
}

void Timer::changeTimerTask() {
    if(this->currentTask != NULL) {
        vTaskDelete(this->currentTask);
        this->currentTask = NULL;
    }
    const int bytesNeeded = 2560; //temporary value thats working
    const char* taskName = "timerTask";
    void* taskArgument = (void*)this;
    const int taskPriority = 1;
    TaskHandle_t* const taskHandle = &this->currentTask;
    xTaskCreate(timerTask, taskName, bytesNeeded, taskArgument, taskPriority, taskHandle);
}

void Timer::updateTime(int lastDate, int period) {
    this->recentlyUpdated = true;
    this->lastDate = lastDate;
    this->period = period;
    this->executeFunction = printStuff;
    this->changeTimerTask();
}

void Timer::onTimerUpdate() {
    this->recentlyUpdated = true;
}

executeFunctionType Timer::getExecuteFunction() {
    return this->executeFunction;
}

bool Timer::getRecentlyUpdated() {
    return this->recentlyUpdated;
}

void Timer::setRecentlyUpdated(bool recentlyUpdated) {
    this->recentlyUpdated = recentlyUpdated;
}

int Timer::getPeriod() {
    return this->period;
}