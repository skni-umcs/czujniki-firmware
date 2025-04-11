#ifndef MOCSMALLKCOMMUNICATION_H
#define MOCSMALLKCOMMUNICATION_H

#include <vector>
#include "exchange/communications/small_communication.h"
#include "exchange/communications/big_communication.h"

class MockSmallCommunication : public SmallCommunication {
    
    public:
        static std::shared_ptr<MockSmallCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
};

#endif