#include <aki/jsbind.h>
#include "function_class_cpp.h"
#include "function_class.h"
#include "compute_class.h"

int add(int a, int b) 
{
    return a + b; 
}

// Step 2 注册 FFI 特性
JSBIND_GLOBAL() 
{
    JSBIND_FUNCTION(add);
    JSBIND_FUNCTION(ProcessFfrtQueue);
    JSBIND_FUNCTION(ComputeFfrtQueue);
}

JSBIND_CLASS(FunctionClass) {
    JSBIND_CONSTRUCTOR<int>();
    JSBIND_PMETHOD(FfrtSerialQueue);
    JSBIND_PMETHOD(FfrtConcurrentQueue);
}

// Step 1 注册 AKI 插件
JSBIND_ADDON(entry) // 注册 AKI 插件名: 即为编译*.so名称，规则与NAPI一致
