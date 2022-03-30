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

#ifndef __IOT_SCHEDULE_H__
#define __IOT_SCHEDULE_H__

int IOT_ScheduleInit(void);

int IOT_ScheduleAdd(const char *id, int *day, int size, int startTime, int durTime, int cmd, int value);

int IOT_ScheduleUpdate(const char *id, int *day, int size, int startTime, int durTime, int cmd, int value);

int IOT_ScheduleDelete(const char *id, int *day, int size, int startTime, int durTime, int cmd, int value);

int IOT_ScheduleIsUpdate(unsigned int weekday, int curtime);

void IOT_ScheduleSetUpdate(int update);

int IOT_ScheduleGetStartTime(void);

int IOT_ScheduleGetDurationTime(void);

int IOT_ScheduleGetCommand(int *cmd, int *value);

void IOT_ScheduleDeinit(void);

#endif  /* __IOT_SCHEDULE_H__ */
