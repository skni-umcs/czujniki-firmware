#ifndef MESSAGE_H
#define MESSAGE_H

class Message {
    public:
        virtual bool isAddressInvalid() = 0;
        virtual std::string getMessage() = 0;

}

#endif