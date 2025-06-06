#ifndef BIGCOMMUNICATION_H
#define BIGCOMMUNICATION_H

#include <vector>
#include <exchange/communications/communication.h>
#include "message/message.h"

class BigTransmit;

class BigCommunication : public Communication<BigTransmit, BigCommunication> {
    protected:
        BigCommunication() {};
};

#endif