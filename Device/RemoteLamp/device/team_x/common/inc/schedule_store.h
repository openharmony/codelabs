/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SCHEDULE_STORE_H__
#define __SCHEDULE_STORE_H__

#define SCHEDULE_ID_LENGTH  40

typedef struct {
    unsigned char cmd;
    unsigned int value;
}ScheduleCommand;

typedef struct {
    unsigned char week;
    unsigned int starttime;
    unsigned int duration;
    unsigned char id[SCHEDULE_ID_LENGTH];

    ScheduleCommand scheduleCmd;
} ScheduleInfo;

typedef enum {
    SCHEDULE_OPTION_ADD = 0,
    SCHEDULE_OPTION_UPDATE,
    SCHEDULE_OPTION_DELETE
} SCHEDULE_OPTION;

int ScheduleStoreUpdate(ScheduleInfo *info, SCHEDULE_OPTION option);

int ScheduleStoreGetTotal(void);

int ScheduleStoreGetInfo(ScheduleInfo *info, int idx);

void ScheduleStoreDelete(void);

#endif  /* __SCHEDULE_STORE_H__ */
