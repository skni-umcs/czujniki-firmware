#ifndef STORAGETYPES_H
#define STORAGETYPES_H

#include <string>
#include <vector>
#include <Arduino.h>
#include <memory>
#include "operation_result.h"

#define INVALID_ADDRESS -1
#define INVALID_CRC -1;
#define INVALID_STRING "";
#define INVALID_VECTOR {};

typedef short moduleAddress;
const moduleAddress SERVER_ADDRESS = 0;

enum class MessageType {
    LoraMessage,
    GeneratedMessage,
    TextMessage
};

#endif