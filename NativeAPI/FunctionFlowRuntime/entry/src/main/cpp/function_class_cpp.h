
#ifndef OHOS_SIMPLE_FUNCTIONCLASS_H
#define OHOS_SIMPLE_FUNCTIONCLASS_H

#include "aki/function.h"
#include <aki/jsbind.h>

class FunctionClass {
public:
    FunctionClass() = default;
    FunctionClass(uint32_t version);
    ~FunctionClass();
    void FfrtSerialQueue();
    void FfrtConcurrentQueue();
};

#endif
