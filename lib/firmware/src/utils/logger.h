#ifndef LOGGER_H
#define LOGGER_H

#include "operation_result.h"
#include <memory>
#include "exchange/transmits/wifi_transmit.h"
#include <string>
#include <sstream>
#include <utility>
#include <cstdio>
#include "time/time_constants.h"
#include <iomanip>

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
            std::ostringstream oss;

            int h   = rtc.getHour();
            int m   = rtc.getMinute();
            int s   = rtc.getSecond();
            int D   = rtc.getDay();
            int M   = rtc.getMonth()+1;
            int Y   = rtc.getYear();
            
            oss << '['
                << std::setfill('0') << std::setw(2) << h << ':'
                << std::setfill('0') << std::setw(2) << m << ':'
                << std::setfill('0') << std::setw(2) << s << ' '
                << std::setfill('0') << std::setw(2) << D << '.'
                << std::setfill('0') << std::setw(2) << M << '.'
                << Y
                << "] ";        

            appendArguments(oss, args...);
            std::string message = oss.str();
            if(getWifi()) {
                getWifi()->send(std::shared_ptr<TextMessage>(new TextMessage(message)));
            }
            Serial.println(message.c_str());
            return OperationResult::SUCCESS;
        }

        template<typename... Args>
        static OperationResult logf(const char* str, Args ... args) {
            int size = std::snprintf(nullptr, 0, str, args...);
            if(size < 0) {
                return OperationResult::ERROR;
            }
            std::string message;
            message.resize(size);
            std::snprintf(&message[0], size+1, str, args...);

            std::ostringstream oss;

            int h   = rtc.getHour();
            int m   = rtc.getMinute();
            int s   = rtc.getSecond();
            int D   = rtc.getDay();
            int M   = rtc.getMonth()+1;
            int Y   = rtc.getYear();
            
            oss << '['
                << std::setfill('0') << std::setw(2) << h << ':'
                << std::setfill('0') << std::setw(2) << m << ':'
                << std::setfill('0') << std::setw(2) << s << ' '
                << std::setfill('0') << std::setw(2) << D << '.'
                << std::setfill('0') << std::setw(2) << M << '.'
                << Y
                << "] ";
            if(getWifi()) {
                getWifi()->send(std::shared_ptr<TextMessage>(new TextMessage(oss.str()+message)));
            }
            oss << message.c_str();
            Serial.println(oss.str().c_str());
            return OperationResult::SUCCESS;
        }
        static std::shared_ptr<WifiTransmit> getWifi();
        static OperationResult setWifi(std::shared_ptr<WifiTransmit> wifi);
};

#endif