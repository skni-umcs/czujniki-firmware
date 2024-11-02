#include "timer.h"
#include <iostream>



void printStuff () {
    std::cout << "elo z południańca";
}

void timerTask(void* timerObjectRawPointer) {
    Timer* timer = (Timer*)timerObjectRawPointer;
    while(1) {
        if(timer->executeFunction != nullptr && !timer->recentlyUpdated) {
            timer->executeFunction();
        }
        vTaskDelay(600);
    }
}

void zadanie1(void *arg)
{
    while (1) {
        printf("%s", "jeden");
        vTaskDelay(500);
        printf("%s", "dwa");
        vTaskDelay(500);
        std::flush(std::cout);
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
    void* taskArgument = NULL;
    const int taskPriority = 1;
    TaskHandle_t* const taskHandle = NULL;
    xTaskCreate(zadanie1, taskName, bytesNeeded, taskArgument, taskPriority, taskHandle);
}

void Timer::onTimerUpdate() {
    this->recentlyUpdated = true;
}