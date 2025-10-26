#ifndef OPERATIONRESULT_H
#define OPERATIONRESULT_H

enum class OperationResult {
    SUCCESS,
    ERROR,
    DEFERRED,
    NOT_FOUND,
    OPERATION_IGNORED
};

#endif