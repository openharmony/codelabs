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

#ifndef __SCHEDULE_LIST_H__
#define __SCHEDULE_LIST_H__

#include "schedule_store.h"

#define SCH_DEBUG
#ifdef  SCH_DEBUG
#define SCH_DBG(fmt, args...)   printf("[DEBUG][%s|%d]" fmt, __func__, __LINE__, ##args)
#define SCH_ERR(fmt, args...)   printf("[ERROR][%s|%d]" fmt, __func__, __LINE__, ##args)
#else
#define SCH_DBG(fmt, args...)   do{}while(0)
#define SCH_ERR(fmt, args...)   do{}while(0)
#endif

typedef void* SCHEDULE_LIST;

SCHEDULE_LIST ScheduleListCreate(ScheduleInfo *info);

int ScheduleListAppend(SCHEDULE_LIST mHandle, ScheduleInfo *info);

int ScheduleListUpdate(SCHEDULE_LIST mHandle, ScheduleInfo *info);

int ScheduleListDelete(SCHEDULE_LIST mHandle, ScheduleInfo *info);

int ScheduleListGetSize(SCHEDULE_LIST mHandle);

int ScheduleListGet(SCHEDULE_LIST mHandle, int idx, ScheduleInfo *info);

void ScheduleListDestroy(SCHEDULE_LIST mHandle);

#endif  /* __SCHEDULE_LIST_H__ */
