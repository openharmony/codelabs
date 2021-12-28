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
#ifndef __PLAT_PM_H__
#define __PLAT_PM_H__

/* ****************************************************************************
  1 Include other Head file
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/mutex.h>
#include <linux/kernel.h>
#include "plat_pm_wlan.h"
#include "plat_board_adapt.h"
#endif
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <linux/wakelock.h>
#include "plat_pm_wlan.h"
#include "plat_board_adapt.h"
#endif
#include "oal_task.h"

/* ****************************************************************************
  2 Define macro
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define PM_SUSPEND_PREPARE  0x0003 /* Going to suspend the system */
#define PM_POST_SUSPEND     0x0004 /* Suspend finished */
#define MMC_PM_KEEP_POWER   (1 << 0)    /* preserve card power during suspend */
#define PM_SUSPEND_PREPARE  0x0003 /* Going to suspend the system */
#define PM_POST_SUSPEND     0x0004 /* Suspend finished */
#define MMC_PM_KEEP_POWER   (1 << 0)    /* preserve card power during suspend */

#define ASYNCB_SUSPENDED    30 /* Serial port is suspended */
#endif

#define SUCCESS                        0
#define FAILURE                        1

#define WAIT_DEVACK_MSEC               100
#define WAIT_DEVACK_TIMEOUT_MSEC       200

#define WAIT_WKUPDEV_MSEC              3400

#define SDIO_REINIT_RETRY              5

enum WLAN_DEV_STATUS_ENUM {
    HOST_DISALLOW_TO_SLEEP = 0,
    HOST_ALLOW_TO_SLEEP = 1,
    HOST_EXCEPTION = 2,
    DEV_SHUTDOWN = 3,
};

enum WLAN_HOST_STATUS_ENUM {
    HOST_NOT_SLEEP = 0,
    HOST_SLEEPED = 1,
};

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* BFGX系统上电加载异常类型 */
enum BFGX_POWER_ON_EXCEPTION_ENUM {
    BFGX_POWER_FAILED                               = -1,
    BFGX_POWER_SUCCESS                              = 0,

    BFGX_POWER_PULL_POWER_GPIO_FAIL                 = 1,
    BFGX_POWER_TTY_OPEN_FAIL                        = 2,
    BFGX_POWER_TTY_FLOW_ENABLE_FAIL                 = 3,

    BFGX_POWER_WIFI_DERESET_BCPU_FAIL               = 4,
    BFGX_POWER_WIFI_ON_BOOT_UP_FAIL                 = 5,

    BFGX_POWER_WIFI_OFF_BOOT_UP_FAIL                = 6,
    BFGX_POWER_DOWNLOAD_FIRMWARE_FAIL               = 7,

    BFGX_POWER_WAKEUP_FAIL                          = 8,
    BFGX_POWER_OPEN_CMD_FAIL                        = 9,

    BFGX_POWER_ENUM_BUTT,
};

/* wifi系统上电加载异常类型 */
enum WIFI_POWER_ON_EXCEPTION_ENUM {
    WIFI_POWER_FAIL                                 = -1,
    WIFI_POWER_SUCCESS                              = 0,
    WIFI_POWER_PULL_POWER_GPIO_FAIL                 = 1,

    WIFI_POWER_BFGX_OFF_BOOT_UP_FAIL                = 2,
    WIFI_POWER_BFGX_OFF_FIRMWARE_DOWNLOAD_FAIL      = 3,

    WIFI_POWER_BFGX_ON_BOOT_UP_FAIL                 = 4,
    WIFI_POWER_BFGX_DERESET_WCPU_FAIL               = 5,
    WIFI_POWER_BFGX_ON_FIRMWARE_DOWNLOAD_FAIL       = 6,

    WIFI_POWER_ENUM_BUTT,
};

#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* wifi系统上电加载异常类型 */
enum WIFI_POWER_ON_EXCEPTION_ENUM {
    WIFI_POWER_FAIL                                 = -1,
    WIFI_POWER_SUCCESS                              = 0,
    WIFI_POWER_BFGX_OFF_BOOT_UP_FAIL                = 1,
    WIFI_POWER_BFGX_OFF_FIRMWARE_DOWNLOAD_FAIL      = 2,

    WIFI_POWER_ENUM_BUTT,
};
#endif

typedef enum {
    POWER_STATE_SHUTDOWN = 0,
    POWER_STATE_OPEN     = 1,
    POWER_STATE_BUTT     = 2,
} power_state_enum;

/* ****************************************************************************
  3 STRUCT DEFINE
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

struct notifier_block;

typedef int (*notifier_fn_t)(struct notifier_block *nb, unsigned long action, void *data);

struct notifier_block {
    notifier_fn_t notifier_call;
    struct notifier_block *next;
    int priority;
};
#endif

/* ****************************************************************************
  4 EXTERN VARIABLE
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
extern unsigned int g_ul_wlan_resume_state;
extern unsigned int g_ul_wlan_resume_wifi_init_flag;
#endif

/* ****************************************************************************
  5 EXTERN FUNCTION
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
extern hi_s32 wlan_power_on(void);
extern int firmware_download_function(hi_u32 which_cfg);
#endif

#endif
