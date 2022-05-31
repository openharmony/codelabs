/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DEMO_COMMON_H
#define DEMO_COMMON_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include <cstring>
#include <string>
#include <securec.h>

#define SAMPLE_INFO(format, args...)                                                              \
    do {                                                                                          \
        fprintf(stderr, "\033[1;32m [INFO](%s:%d):\t\033[0m" format, __func__, __LINE__, ##args); \
        printf("\n");                                                                             \
    } while (0)

#define SAMPLE_ERROR(format, args...)                                                                         \
    do {                                                                                                      \
        fprintf(stderr, "\033[1;31m [ERROR]RTSP SERVER(%s:%d):\t\033[0m" format, __func__, __LINE__, ##args); \
        printf("\n");                                                                                         \
    } while (0)

#define LISTEN_BACK_LOG 10
#define CLIENT_IP_MAX 40
#define SERVER_PORT 8556
#define BUF_MAX_SIZE (1024 * 1024)

#endif // DEMO_COMMON_H