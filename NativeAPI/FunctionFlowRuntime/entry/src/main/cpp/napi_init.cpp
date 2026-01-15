/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <aki/jsbind.h>
#include "function_class_cpp.h"
#include "function_class.h"
#include "compute_class.h"

int Add(int a, int b) 
{
    return a + b;
}

// Step 2 注册 FFI 特性
JSBIND_GLOBAL()
{
    JSBIND_FUNCTION(Add);
    JSBIND_FUNCTION(ProcessFfrtQueue);
    JSBIND_FUNCTION(ComputeFfrtQueue);
}

JSBIND_CLASS(FunctionClass) {
    JSBIND_CONSTRUCTOR<int>();
    JSBIND_METHOD(FfrtSerialQueue);
    JSBIND_METHOD(FfrtConcurrentQueue);
}

// Step 1 注册 AKI 插件
JSBIND_ADDON(entry) // 注册 AKI 插件名: 即为编译*.so名称，规则与NAPI一致
