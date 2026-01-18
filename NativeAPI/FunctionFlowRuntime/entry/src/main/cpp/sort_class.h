//
// Created on 2026/1/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef FUNCTIONFLOWRUNTIME_SORT_CLASS_H
#define FUNCTIONFLOWRUNTIME_SORT_CLASS_H

#include "aki/function.h"
#include <aki/jsbind.h>

class SortClass {
public:
    SortClass() = default;
    SortClass(uint32_t version);
    ~SortClass();
    int FfrtConcurrentQueue();
};

#endif //FUNCTIONFLOWRUNTIME_SORT_CLASS_H
