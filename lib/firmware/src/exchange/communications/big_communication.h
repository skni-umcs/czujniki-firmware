#ifndef BIGCOMMUNICATION_H
#define BIGCOMMUNICATION_H

#include <vector>
#include <exchange/communications/communication.h>
#include "utils/storage_types.h"

class BigTransmit;

class BigCommunication : public Communication<BigTransmit, BigCommunication> {
};

#endif