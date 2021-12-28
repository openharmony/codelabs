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

#ifndef __HCC_TASK_H__
#define __HCC_TASK_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hcc_host.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
typedef struct {
    oal_kthread_stru            *hcc_kthread;
    oal_wait_queue_head_stru     hcc_wq;
    hi_u8                        task_state;
    hi_u8                        auc_resv[3];   /* resv 3 bytes */
} hcc_task_stru;
#endif

hi_u32 hcc_task_init(hcc_handler_stru *hcc_handler);
hi_void hcc_task_exit(hi_void);

hi_void hcc_task_sched(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of frw_task.h */
