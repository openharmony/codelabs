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

#ifndef __OAL_LINUX_TASK_H__
#define __OAL_LINUX_TASK_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/mutex.h>
#endif
#include "oal_workqueue.h"
#include "oal_spinlock.h"
#include "oal_wait.h"
#include "oal_atomic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
typedef struct _oal_task_lock_stru_ {
    oal_wait_queue_head_stru    wq;
    struct task_struct         *claimer;    /* task that has host claimed */
    oal_spin_lock_stru          lock;       /* lock for claim and bus ops */
    unsigned long               claim_addr;
    hi_u32                      claimed;
    hi_s32                      claim_cnt;
} oal_task_lock_stru;

typedef struct tasklet_struct oal_tasklet_stru;
typedef hi_void (*oal_defer_func)(unsigned long);

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 宏定义
**************************************************************************** */
/* tasklet声明 */
#define OAL_DECLARE_TASK DECLARE_TASKLET

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 任务初始化。初始化完成后，任务处于挂起状态。
 输入参数  : pst_task: 任务结构体指针
             func: 任务处理函数入口地址
             p_args: 需进行处理的函数的入参地址
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_task_init(oal_tasklet_stru *pst_task, oal_defer_func p_func, hi_void *p_args)
{
    tasklet_init(pst_task, p_func, (uintptr_t)p_args);
}

/* ****************************************************************************
 功能描述  : 退出task运行
 输入参数  : pst_task: 任务结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_task_kill(oal_tasklet_stru *pst_task)
{
    return tasklet_kill(pst_task);
}

/* ****************************************************************************
 功能描述  : 任务调度，令任务处于准备就绪状态；当任务执行完后，又回到挂起状
             态。
 输入参数  : pst_task: 任务结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_task_sched(oal_tasklet_stru *pst_task)
{
    tasklet_schedule(pst_task);
}

/* ****************************************************************************
 功能描述  : 检测tasklet是否等待执行
 输入参数  : pst_task: 任务结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline unsigned long oal_task_is_scheduled(oal_tasklet_stru *pst_task)
{
    return oal_bit_atomic_test(TASKLET_STATE_SCHED, (unsigned long *)&pst_task->state);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_task.h */
