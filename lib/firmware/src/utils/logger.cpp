#include "logger.h"

std::shared_ptr<Logger> Logger::instance;

std::shared_ptr<Logger> Logger::getInstance() {
    if(!instance) {
        instance = std::shared_ptr<Logger>{new Logger()};
    }
    return instance;
}

OperationResult Logger::log(...) {
    return OperationResult::SUCCESS;
}

OperationResult Logger::logf(const char* str, ...) {
    va_list ptr;
    va_start(ptr, str);

    char token[1000];

    int tokenIndex = 0;
    // for(int i = 0;str[i] != '\0'; i++) {
    //     token[tokenIndex++] = str[i];

    //     if(str[i+1] == '%' || str[i+1] == '\0') {
    //         token[tokenIndex] = '\0';
    //         tokenIndex = 0;
    //     }
    // }
    instance->wifi.get()->send(str, 0);
    return OperationResult::SUCCESS;
}