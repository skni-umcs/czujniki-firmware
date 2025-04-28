#ifndef LOGGER_H
#define LOGGER_H

#include "operation_result.h"
#include <memory>
#include "exchange/transmits/wifi_transmit.h"

class Logger {
    static std::shared_ptr<Logger> instance;
    std::shared_ptr<WifiTransmit> wifi;

    public:
        static std::shared_ptr<Logger> getInstance();
        static OperationResult log(const char* str, ...);
};

#endif