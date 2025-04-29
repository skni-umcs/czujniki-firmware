#ifndef LOGGER_H
#define LOGGER_H

#include "operation_result.h"
#include <memory>
#include "exchange/transmits/wifi_transmit.h"
#include <string>
#include <sstream>
#include <utility>

class Logger {
    static std::shared_ptr<Logger> instance;
    std::shared_ptr<WifiTransmit> wifi = nullptr;

    static void appendArguments(std::ostringstream& stream) {
    }

    template<typename T, typename... Rest>
    static void appendArguments(std::ostringstream& stream, T first, Rest... rest) {
        stream << first;
        appendArguments(stream, rest...);
    }

    public:
        static std::shared_ptr<Logger> getInstance();

        template<typename... Args>
        static OperationResult log(Args ... args) {
            if(getWifi()) {
                std::ostringstream oss;
                appendArguments(oss, args...);
                std::string message = oss.str();
                getWifi()->send(message, 0);
            }
            return OperationResult::SUCCESS;
        }
        static OperationResult logf(const char* str, ...);
        static std::shared_ptr<WifiTransmit> getWifi();
        static OperationResult setWifi(std::shared_ptr<WifiTransmit> wifi);
};

#endif