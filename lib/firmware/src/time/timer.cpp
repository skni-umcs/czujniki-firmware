#include "timer.h"
#include <iostream>



void printStuff () {
    std::cout << "elo z południańca";
}

void timerTask(void* timerObjectRawPointer) {
    Timer* timer = (Timer*)timerObjectRawPointer;
    while(1) {
        std::cout << "timerTask iteration" << timer->executeFunction << timer->recentlyUpdated << std::endl;
        if(timer->executeFunction != nullptr && !timer->recentlyUpdated) {
            timer->executeFunction();
        }
        timer->recentlyUpdated = false;
        vTaskDelay(2000);
    }
}

void Timer::updateTime(int lastDate, int period) {
    this->recentlyUpdated = true;
    this->lastDate = lastDate;
    this->period = period;
    this->executeFunction = printStuff;
    // if(this->currentTask != NULL) {
    //     vTaskDelete(this->currentTask);
    // }
    const int bytesNeeded = 2560; //temporary value thats working
    const char* taskName = "timerTask";
    void* taskArgument = (void*)this;
    const int taskPriority = 1;
    TaskHandle_t* const taskHandle = NULL;
    xTaskCreate(timerTask, taskName, bytesNeeded, taskArgument, taskPriority, taskHandle);
}

void Timer::onTimerUpdate() {
    this->recentlyUpdated = true;
}