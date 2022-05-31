# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# the minimum version of CMake.指明了对cmake的最低(高)版本的要求
cmake_minimum_required(VERSION 3.4.1)
#配置项目信息
project(XComponent)
#指定编程语言
set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
#设置头文件的搜索目录
include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include
                    )
# 添加名为xxx的库
add_library(hello SHARED hello.cpp)
#构建此可执行文件需要链接的库
target_link_libraries(hello PUBLIC libace_napi.z.so libc++.a)