#include "logger.h"

std::shared_ptr<Logger> Logger::instance;

std::shared_ptr<Logger> Logger::getInstance() {
    if(!instance) {
        instance = std::shared_ptr<Logger>{new Logger()};
    }
    return instance;
}

std::shared_ptr<WifiTransmit> Logger::getWifi() {
    return getInstance()->wifi;
}

OperationResult Logger::setWifi(std::shared_ptr<WifiTransmit> wifi) {
    getInstance()->wifi = wifi;
    return OperationResult::SUCCESS;
}