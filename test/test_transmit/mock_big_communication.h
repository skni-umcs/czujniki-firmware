#ifndef MOCKBIGCOMMUNICATION_H
#define MOCKBIGCOMMUNICATION_H

#include <vector>
#include "exchange/communications/small_communication.h"
#include "exchange/communications/big_communication.h"

class MockBigCommunication : public BigCommunication {
    
    public:
        static std::shared_ptr<MockBigCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
};

#endif