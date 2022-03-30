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

#ifndef __IOT_SNTP_H
#define __IOT_SNTP_H

#include <time.h>

/**
 * @brief Additional introduction for the key members in  struct tm
 * struct timeval maybe defines as following
 * int tm_sec;      seconds (0 - 60)
 * int tm_min;      minutes (0 - 59)
 * int tm_hour;     hours (0 - 23)
 * int tm_mday;     day of month (1 - 31)
 * int tm_mon;      month of year (0 - 11)
 * int tm_year;     year - 1900
 * int tm_wday;     day of week (Sunday = 0)
 * int tm_yday;     day of year (0 - 365)
 * int tm_isdst;    is summer time in effect?
 * char *tm_zone;   abbreviation of timezone name
 * long tm_gmtoff;  offset from UTC in seconds
 *
 */

/**
 * @brief Get the ntp time from ntp server (local RTC TIME);Could only be used after the
 *        Network is ok and the tcpip is initialized ok
 *
 * @param localTimeZone indicates the local timezone, and could be -11 to 11, 0 means the UTC+0,
 *                      positive means east while negative means west, 8 means UTC+8, like China
 * @param rtcTime is a pointer to storage the rtc time if the return value is 0, and must not
 *                be NULL
 * @return Returns 0 success while -1 failed; The content in rtcTime Only be valid when returns 0
 *
 */
int SntpGetRtcTime(int localTimeZone, struct tm *rtcTime);


#endif /* __IOT_SNTP_H */
