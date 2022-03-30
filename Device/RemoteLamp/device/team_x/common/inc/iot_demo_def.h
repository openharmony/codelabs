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

#ifndef __IOT_DEMO_DEF_H__
#define __IOT_DEMO_DEF_H__

#define CN_BOARD_SWITCH_ON    1
#define CN_BOARD_SWITCH_OFF   0
#define CN_HOURS_IN_DAY         24
#define CN_MINUTES_IN_HOUR      60
#define CN_SECONDS_IN_MINUTE    60
#define CN_SECONDS_IN_HOUR      (CN_MINUTES_IN_HOUR * CN_SECONDS_IN_MINUTE)
#define CN_MINISECONDS_IN_SECOND 1000
#define CN_MINUTES_IN_DAY (CN_HOURS_IN_DAY * CN_MINUTES_IN_HOUR)
#define CN_SECONS_IN_DAY (CN_SECONDS_IN_MINUTE * CN_MINUTES_IN_DAY)
#define BASE_YEAR_OF_TIME_CALC           1900

#define AFTER_NETCFG_ACTION   2
#define BUFF_SIZE  256

typedef enum {

    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERR,
    LOG_LEVEL_EXCEPTION,
    LOG_LEVEL_EXIT,
}LogLevelType;

//we defines a log arise function
#define RAISE_LOG 1

#ifdef RAISE_LOG
#define RaiseLog(level,fmt, ...) \
    do \
    { \
        printf("[%s][%u][%d] " fmt "\r\n", \
        __FUNCTION__,__LINE__,level, ##__VA_ARGS__); \
    } while (0)
#else
#define RaiseLog(level,fmt, ...)
#endif


#endif /* __IOT_DEMO_DEF_H__ */
