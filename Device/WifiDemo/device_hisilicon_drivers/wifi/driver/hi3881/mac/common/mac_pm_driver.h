/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __PM_DRIVER_H__
#define __PM_DRIVER_H__

#include "hi_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
    宏定义
**************************************************************************** */
#define PM_SWITCH_MIN_TIMESLOT                  33                               /* 33ms */
#define PM_SWITCH_MAX_TIMESLOT                  4000                             /* 4000ms */
#define PM_SWITCH_ON                            1
#define PM_SWITCH_OFF                           0
#define STA_NUM                                 0x2UL
#define PM_SWITCH_AUTO_FLAG                     1

/* ****************************************************************************
    枚举，结构定义
**************************************************************************** */
enum wlan_pm_mode {
    WLAN_PM_WORK,
    WLAN_PM_LIGHT_SLEEP,
    WLAN_PM_DEEP_SLEEP
};

/* ****************************************************************************
    对外函数
**************************************************************************** */
hi_u8 *mac_get_pm_wlan_need_stop_ba(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
