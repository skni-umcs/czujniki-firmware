#include "timer.h"
#include <iostream>
#include "timerUpdate.h"

Timer::Timer() {}

std::shared_ptr<Timer> Timer::create() {
    auto timerPtr = std::shared_ptr<Timer>(new Timer());
    TimerUpdate::addSubscriber(timerPtr);
    return timerPtr;
}

void timerTask(void* timerObjectRawPointer) {
    std::shared_ptr<Timer>* timerPtr = static_cast<std::shared_ptr<Timer>*>(timerObjectRawPointer);
    while(1) {
        if(timerPtr->get()->getExecuteFunction() != nullptr && !timerPtr->get()->getRecentlyUpdated()) {
            timerPtr->get()->getExecuteFunction()();
        }
        timerPtr->get()->setRecentlyUpdated(false);
        vTaskDelay(timerPtr->get()->getPeriod());
    }
}

void Timer::changeTimerTask() {
    if(this->currentTask != NULL) {
        vTaskDelete(this->currentTask);
        this->currentTask = NULL;
    }

    auto* taskPtr = new std::shared_ptr<Timer>(shared_from_this());

    const int bytesNeeded = 25600; //temporary value thats working
    const char* taskName = "timerTask";
    void* taskArgument = static_cast<void*>(taskPtr);
    const int taskPriority = 1;
    TaskHandle_t* const taskHandle = &this->currentTask;
    xTaskCreate(timerTask, taskName, bytesNeeded, taskArgument, taskPriority, taskHandle);
}

void Timer::updateTime(int lastDate, int period) {
    this->recentlyUpdated = true;
    this->lastDate = lastDate;
    this->period = period;
    this->changeTimerTask();
}

void Timer::onTimerUpdate() {
    this->recentlyUpdated = true;
}

executeFunctionType Timer::getExecuteFunction() {
    return this->executeFunction;
}

void Timer::setExecuteFunction(executeFunctionType executeFunction) {
    this->executeFunction = executeFunction;
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