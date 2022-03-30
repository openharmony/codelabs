/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef __WINDER_H__
#define __WINDER_H__


#include <cmsis_os2.h>

#define LAMP_TASK_STACK_SIZE      (1024*4)
#define LAMP_TASK_PRIO            (25)

#define BUF_SIZE        64

#define POWER_XPOS      0
#define POWER_YPOS      0
#define NETSTA_XPOS     0
#define NETSTA_YPOS     1

#define MESSAGE_LEN     6
#define MSG_VAL_LEN     2
typedef enum {
    MESSAGE_POWER_OFF = 1,
    MESSAGE_POWER_OFF_S = 11,
}MESSAGE_TYPE;
#endif  /* __WINDER_H__ */
