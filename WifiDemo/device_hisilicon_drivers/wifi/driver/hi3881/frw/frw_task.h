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

#ifndef __FRW_TASK_H__
#define __FRW_TASK_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  枚举定义
**************************************************************************** */
/* 线程的运行状态 */
typedef enum {
    FRW_TASK_STATE_IRQ_UNBIND = 0, /* 线程处于暂停状态 */
    FRW_TASK_STATE_IRQ_BIND,       /* 线程处于运行状态 */

    FRW_TASK_BUTT
} frw_task_state_enum;
typedef hi_u8 frw_task_state_enum_uint8;

/* ****************************************************************************
  4 STRUCT定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
typedef struct {
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
    hi_u32             taskid;                                /* task变量 */
    oal_wait_queue_head_stru frw_wq;                          /* waitqueue */
    hi_void (*event_handler_func)(hi_void *_pst_bind_cpu);    /* kthread主程序 */

#elif (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_TASKLET)
    oal_tasklet_stru   event_tasklet;                        /* tasklet变量 */
    hi_void (*event_handler_func)(hi_u32);                   /* tasklet处理程序 */
#endif
} frw_task_stru;

#else /* (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */

typedef struct {
    oal_kthread_stru        *pst_event_kthread;
    oal_wait_queue_head_stru frw_wq;
    hi_u8                    uc_task_state;
    hi_u8                    auc_resv[3];   /* resv 3 byte */
    hi_u32                   ul_total_loop_cnt;
    hi_u32                   ul_total_event_cnt;
    hi_u32                   ul_max_empty_count;
    hi_void (*p_event_handler_func)(hi_void *_pst_bind_cpu);
} frw_task_stru;
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_u32 frw_task_init(hi_void);
hi_void frw_task_exit(hi_void);
hi_void frw_task_event_handler_register(hi_void (*func)(hi_void));
hi_void frw_task_sched(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of frw_task.h */
