# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(FunctionFlowRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lc++_shared")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_library(entry SHARED napi_init.cpp function_class.cpp bank_queue_system.cpp common_ffrt.c compute_class.c function_class.c)
target_link_libraries(entry PUBLIC libace_napi.z.so)
target_link_libraries(entry PUBLIC libhilog_ndk.z.so Aki::libjsbind)
target_link_libraries(entry PUBLIC libffrt.z.so)