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

#ifndef __OAL_LINUX_WORKQUEUE_H__
#define __OAL_LINUX_WORKQUEUE_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#endif
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include "hi_types.h"
#include "oal_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
typedef struct workqueue_struct          oal_workqueue_stru;
typedef struct work_struct               oal_work_stru;
typedef struct delayed_work              oal_delayed_work;

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
extern struct workqueue_struct *g_pstSystemWq;
#endif

/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 宏定义
**************************************************************************** */
#define OAL_INIT_WORK(_p_work, _p_func)            INIT_WORK(_p_work, _p_func)
#define OAL_INIT_DELAYED_WORK(_work, _func)         INIT_DELAYED_WORK(_work, _func)
#define OAL_CREATE_SINGLETHREAD_WORKQUEUE(_name)   create_singlethread_workqueue(_name)
#define oal_create_workqueue(name)                 create_workqueue(name)

hi_u32 oal_workqueue_init(hi_void);
hi_u32 oal_workqueue_exit(hi_void);
hi_u32 oal_workqueue_schedule(oal_work_stru *pst_work);
hi_u32 oal_workqueue_delay_schedule(oal_delayed_work *pst_work, unsigned long delay);
/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 创建一个单线程的工作队列
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline oal_workqueue_stru *oal_create_singlethread_workqueue(hi_char *pc_workqueue_name)
{
    return create_singlethread_workqueue(pc_workqueue_name);
}

/* ****************************************************************************
 功能描述  : 销毁工作队列
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_destroy_workqueue(oal_workqueue_stru *pst_workqueue)
{
    destroy_workqueue(pst_workqueue);
}

/* ****************************************************************************
 功能描述  : 添加一个任务到工作队列
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_u32 oal_queue_work(oal_workqueue_stru *pst_workqueue, oal_work_stru *pst_work)
{
    return queue_work(pst_workqueue, pst_work);
}

/* *
 * queue_delayed_work - queue work on a workqueue after delay
 * @wq: workqueue to use
 * @dwork: delayable work to queue
 * @delay: number of jiffies to wait before queueing
 *
 * Equivalent to queue_delayed_work_on() but tries to use the local CPU.
 */
static inline hi_u32 oal_queue_delayed_work(oal_workqueue_stru *pst_workqueue, oal_delayed_work *pst_work,
    unsigned long delay)
{
    return queue_delayed_work(pst_workqueue, pst_work, delay);
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* *
 * queue_delayed_work_on - queue work on specific CPU after delay
 * @cpu: CPU number to execute work on
 * @wq: workqueue to use
 * @dwork: work to queue
 * @delay: number of jiffies to wait before queueing
 *
 * Returns %false if @work was already on a queue, %true otherwise.  If
 * @delay is zero and @dwork is idle, it will be scheduled for immediate
 *  */
static inline hi_u32 oal_queue_delayed_work_on(hi_u32 cpu, oal_workqueue_stru *pst_workqueue,
    oal_delayed_work *pst_work, unsigned long delay)
{
    return queue_delayed_work_on(cpu, pst_workqueue, pst_work, delay);
}
#endif

/* ****************************************************************************
 功能描述  : queue work on system wq after delay
 输入参数  :  @dwork: delayable work to queue
              @delay: number of jiffies to wait before queueing
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_u32 oal_queue_delayed_system_work(oal_delayed_work *pst_work, unsigned long delay)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#if (LINUX_VERSION_CODE >= kernel_version(3, 4, 35))
    return queue_delayed_work(system_wq, pst_work, delay);
#else
    return 1;
#endif

#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (queue_delayed_work(g_pstSystemWq, pst_work, delay)) {
        return OAL_SUCC;
    }
    return OAL_EFAIL;
#endif
}

#define oal_work_is_busy(work)              work_busy(work)
#define oal_cancel_delayed_work_sync(dwork) cancel_delayed_work_sync(dwork)
#define oal_cancel_work_sync(work)          cancel_work_sync(work)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_workqueue.h */
