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

#ifndef __EXCEPTION_RST_H__
#define __EXCEPTION_RST_H__

/* ****************************************************************************
  1 Include other Head file
**************************************************************************** */
#include "oal_spinlock.h"
#include "oal_workqueue.h"

/* ****************************************************************************
  2 Define macro
**************************************************************************** */
#define WIFI_DUMP_TIMEOUT 1000 /* wifi dump bcpu数据等待时间 */

#define PLAT_EXCEPTION_RESET_IDLE  0      /* plat 没有在处理异常 */
#define PLAT_EXCEPTION_RESET_BUSY  1      /* plat 正在处理异常 */

#define EXCEPTION_SUCCESS          0
#define EXCEPTION_FAIL             1

/* plat cfg cmd */
#define PLAT_CFG_IOC_MAGIC                     'z'
#define PLAT_DFR_CFG_CMD                       _IOW(PLAT_CFG_IOC_MAGIC, PLAT_DFR_CFG, int)
#define PLAT_BEATTIMER_TIMEOUT_RESET_CFG_CMD   _IOW(PLAT_CFG_IOC_MAGIC, PLAT_BEATTIMER_TIMEOUT_RESET_CFG, int)

#define PLAT_EXCEPTION_ENABLE                  1
#define PLAT_EXCEPTION_DISABLE                 0

/* ****************************************************************************
  3 STRUCT DEFINE
**************************************************************************** */
enum EXCEPTION_TYPE_ENUM {
    DEVICE_PANIC      = 0, /* arm exception */
    TRANS_FAIL        = 1, /* read or write failed */
    HOST_EXCP         = 2, /* sdio host controller exception */
    EXCEPTION_TYPE_BUTT,
};

typedef struct wifi_dump_mem_info_stru {
    unsigned long mem_addr;
    hi_u32 size;
    hi_u8 *file_name;
} wifi_dump_mem_info_stru;

#define NOT_DUMP_MEM 0

struct st_exception_info {
    hi_u32 exception_reset_enable;
    hi_u32 excetion_type;

    hi_u32 wifi_exception_cnt;

    /* when recv error data from device, dump device mem */
    hi_u32 dump_mem_flag;

    oal_work_stru       excp_worker;
    oal_spin_lock_stru  excp_lock;

    void (*wifi_dfr_func)(void);
};

/* ****************************************************************************
  4 EXTERN VARIABLE
**************************************************************************** */
extern wifi_dump_mem_info_stru g_apst_panic_dump_mem[];
extern wifi_dump_mem_info_stru g_apst_trans_fail_dump_mem[];

/* ****************************************************************************
  5 全局变量定义
**************************************************************************** */
enum PLAT_CFG {
    PLAT_DFR_CFG = 0,
    PLAT_BEATTIMER_TIMEOUT_RESET_CFG,
    PLAT_TRIGGER_EXCEPTION,
    PLAT_POWER_RESET,
    PLAT_CHANNEL_DISABLE,

    PLAT_CFG_BUFF,
};

/* ****************************************************************************
  6 EXTERN FUNCTION
**************************************************************************** */
void plat_dfr_cfg_set(unsigned long arg);
hi_s32 get_exception_info_reference(struct st_exception_info **exception_data);
hi_s32 plat_exception_handler(hi_u32 exception_type);
hi_s32 wifi_exception_mem_dump(wifi_dump_mem_info_stru *mem_dump_info);
hi_s32 plat_exception_reset_init(void);
hi_s32 plat_exception_reset_exit(void);
hi_s32 wifi_system_reset(void);
void oal_exception_submit(hi_s32 excep_type);
void oal_wakeup_exception(void);
hi_s32 oal_device_panic_callback(void *data);
hi_s32 oal_trigger_exception(hi_s32 is_sync);
hi_s32 oal_exception_is_busy(void);
void oal_try_to_dump_device_mem(hi_s32 is_sync);
hi_void oal_set_dev_panic(hi_void);
hi_void oal_clear_dev_panic(hi_void);
hi_u8 oal_dev_is_panic(hi_void);
hi_void oal_frw_exception_report(hi_void);
#endif
