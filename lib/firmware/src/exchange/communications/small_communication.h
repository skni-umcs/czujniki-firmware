#ifndef SMALLCOMMUNICATION_H
#define SMALLCOMMUNICATION_H

#include <vector>
#include <exchange/transmits/small_transmit.h>
#include <exchange/communications/communication.h>
#include <memory>
#include "utils/message.h"

class SmallCommunication : public Communication<SmallTransmit, SmallCommunication> {

};

#endif