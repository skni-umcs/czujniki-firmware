#ifndef STORAGETYPES_H
#define STORAGETYPES_H

#define INVALID_ADDRESS -1

typedef unsigned short moduleAddress;
const moduleAddress SERVER_ADDRESS = 0;

struct Message {
    moduleAddress sender;
    moduleAddress destination;
    std::string content;
};

#endif