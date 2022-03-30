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
#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <stdio.h>
#include <string.h>

#include <cmsis_os2.h>

#define LOG_E(fmt, arg...)    printf("[ERROR][%s|%d]" fmt, __func__, __LINE__, ##arg)
#define LOG_D(fmt, arg...)    printf("[DEBUG][%s|%d]" fmt, __func__, __LINE__, ##arg)
#define LOG_I(fmt, arg...)    printf("[INFO ][%s|%d]" fmt, __func__, __LINE__, ##arg)

#define WINDBG(fmt, args...)    printf("[WINDER_DEBUG][%s|%d]" fmt, __func__, __LINE__, ##args)
#define WINERR(fmt, args...)    printf("[WINDER_ERROR][%s|%d]" fmt, __func__, __LINE__, ##args)
#define WINPRN(fmt, args...)    printf("[WINDER_INFO ][%s|%d]" fmt, __func__, __LINE__, ##args)

#define KEY_ERR(fmt, args...)   printf("[KEY_ERROR][%s|%d]" fmt, __func__, __LINE__, ##args)
#define KEY_DBG(fmt, args...)   printf("[KEY_DEBUG][%s|%d]" fmt, __func__, __LINE__, ##args)
#define KEY_PRT(fmt, args...)   printf("[KEY_INFO ][%s|%d]" fmt, __func__, __LINE__, ##args)

#define NET_ERR(fmt, args...)   printf("[NETCFG_ERROR][%s|%d]" fmt, __func__, __LINE__, ##args)
#define NET_DBG(fmt, args...)   printf("[NETCFG_DEBUG][%s|%d]" fmt, __func__, __LINE__, ##args)
#define NET_PRT(fmt, args...)   printf("[NETCFG_INFO ][%s|%d]" fmt, __func__, __LINE__, ##args)

#define OLED_ERR(fmt, arg...)    printf("[OLED_ERROR][%s|%d]" fmt, __func__, __LINE__, ##arg)
#define OLED_DBG(fmt, arg...)    printf("[OLED_DEBUG][%s|%d]" fmt, __func__, __LINE__, ##arg)
#define OLED_PRT(fmt, arg...)    printf("[OLED_INFO ][%s|%d]" fmt, __func__, __LINE__, ##arg)

#define PWM_ERR(fmt, arg...)    printf("[PWM_ERROR][%s|%d]" fmt, __func__, __LINE__, ##arg)
#define PWM_DBG(fmt, arg...)    printf("[PWM_DEBUG][%s|%d]" fmt, __func__, __LINE__, ##arg)
#define PWM_PRT(fmt, arg...)    printf("[PWM_INFO ][%s|%d]" fmt, __func__, __LINE__, ##arg)


#endif  /* __DEFINES_H__ */
