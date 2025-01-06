#include "storageTypes.h"

Message::Message(std::vector<moduleAddress> senders, moduleAddress destination, std::string content) {
    this->senders = senders;
    this->destination = destination;
    this->content = content;
}

std::vector<moduleAddress> Message::getSenders() {
    return senders;
}

moduleAddress Message::getDestination() {
    return destination;
}

std::string Message::getContent() {
    return content;
}

moduleAddress Message::getOriginalSender() {
    if(senders.size() <= 0) {
        return INVALID_ADDRESS;
    }
    return senders.at(0);
}