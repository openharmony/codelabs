# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(FunctionFlowRuntime)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

set(AKI_ROOT_PATH ${NATIVERENDER_ROOT_PATH}/../../../oh_modules/@ohos/aki) # 设置AKI根路径
set(CMAKE_MODULE_PATH ${AKI_ROOT_PATH})
find_package(Aki REQUIRED)

set(MODULES_PATH ${NATIVERENDER_ROOT_PATH}/../../../oh_modules)
include_directories(${MODULES_PATH}/@ppd/ffrt/include)

add_library(entry SHARED napi_init.cpp FunctionClass.cpp BankQueueSystem.cpp common_ffrt.c compute_class.c function_class.c)
target_link_libraries(entry PUBLIC libace_napi.z.so)
target_link_libraries(entry PUBLIC libhilog_ndk.z.so Aki::libjsbind)
target_link_libraries(entry PUBLIC libffrt.z.so)