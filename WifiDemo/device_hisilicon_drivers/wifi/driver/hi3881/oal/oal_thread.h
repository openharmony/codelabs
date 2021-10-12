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

#ifndef __OAL_LINUX_THREAD_H__
#define __OAL_LINUX_THREAD_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <asm/atomic.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <asm/param.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0))
#include <uapi/linux/sched/types.h>
#endif
#ifndef HAVE_PCLINT_CHECK
#include <linux/kthread.h>
#endif
#endif
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <los_task.h>
#endif
#include "oal_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct task_struct oal_kthread_stru;
#endif

typedef struct _kthread_param_ {
    hi_u32 ul_stacksize;
    hi_s32 l_prio;
    hi_s32 l_policy;
    hi_s32 l_cpuid;
    hi_s32 l_nice;
} oal_kthread_param_stru;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define LOS_TASK_CB         LosTaskCB
#define pcTaskName          taskName
#define uwTaskID            taskID
#define pstRunTask          runTask
#define g_pstTaskCBArray    g_taskCBArray
#define g_stLosTask         g_losTask
typedef LOS_TASK_CB oal_kthread_stru;
#endif

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
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

#define oal_kthread_should_stop   kthread_should_stop
#define oal_schedule              schedule

#define OAL_CURRENT current
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define OAL_CURRENT oal_get_current()
#endif

#define OAL_SCHED_FIFO      1
#define OAL_SCHED_RR        2
#define NOT_BIND_CPU        (-1)

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
typedef int (*oal_thread_func)(void *);

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline oal_kthread_stru *oal_kthread_create(char *pc_thread_name, oal_thread_func pf_threadfn, void *p_data,
    const oal_kthread_param_stru *thread_param)
{
    hi_s32 uwRet;
    oal_kthread_stru *kthread = NULL;
    struct sched_param st_sched_param;
    OAL_BUG_ON(!thread_param);

    kthread = kthread_create(pf_threadfn, p_data, "%s", pc_thread_name);
    if (IS_ERR_OR_NULL(kthread)) {
        oal_io_print1("failed to run theread:%s\n", pc_thread_name);
        return NULL;
    }

    st_sched_param.sched_priority = thread_param->l_prio;
    uwRet = sched_setscheduler(kthread, thread_param->l_policy, &st_sched_param);
    if (oal_unlikely(uwRet)) {
        oal_io_print3("%s sched_setscheduler failed! ret =%d, prio=%d\n", pc_thread_name, uwRet, thread_param->l_prio);
    }

    if (thread_param->l_cpuid >= 0) { /* cpuid为负数时无效 */
        kthread_bind(kthread, thread_param->l_cpuid);
    } else {
        oal_io_print0("did not bind cpu...\n");
    }
    wake_up_process(kthread);
    return kthread;
}

static inline hi_u32 oal_set_thread_property(oal_kthread_stru *pst_thread, oal_kthread_param_stru *thread_param)
{
    struct sched_param st_sched_param;
    OAL_BUG_ON(!pst_thread);
    OAL_BUG_ON(!thread_param);

    st_sched_param.sched_priority = thread_param->l_prio;
    oal_io_print1("set thread scheduler policy %d\n", thread_param->l_policy);

    if (sched_setscheduler(pst_thread, thread_param->l_policy, &st_sched_param)) {
        oal_io_print1("[Error]set scheduler failed! %d\n", thread_param->l_policy);
        return -OAL_EFAIL;
    }

    if (thread_param->l_policy != SCHED_FIFO && thread_param->l_policy != SCHED_RR) {
        oal_io_print1("set thread scheduler nice %d\n", thread_param->l_nice);
        set_user_nice(pst_thread, thread_param->l_nice);
    }
    return HI_SUCCESS;
}

static inline hi_void oal_kthread_stop(oal_kthread_stru *pst_thread)
{
    OAL_BUG_ON(!pst_thread);

    send_sig(SIGTERM, pst_thread, 1);
    kthread_stop(pst_thread);
}

static inline char *oal_get_current_task_name(hi_void)
{
    return current->comm;
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
static inline oal_kthread_stru *oal_kthread_create(char *pc_thread_name, oal_thread_func pf_threadfn, void *p_data,
    const oal_kthread_param_stru *thread_param)
{
    hi_u32 ret;
    TSK_INIT_PARAM_S st_sd_task;
    hi_u32 ul_taskid;
    oal_kthread_stru *kthread = NULL;
    if (thread_param == HI_NULL) {
        oal_io_print0("oal_kthread_create thread_param null\n");
        return NULL;
    }

    /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(&st_sd_task, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    st_sd_task.pfnTaskEntry  = (TSK_ENTRY_FUNC)pf_threadfn;
    st_sd_task.auwArgs[0]    = (uintptr_t)p_data;
    st_sd_task.uwStackSize   = thread_param->ul_stacksize;
    st_sd_task.pcName        = pc_thread_name;
    st_sd_task.usTaskPrio    = (hi_u16)thread_param->l_prio;
    st_sd_task.uwResved      = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    st_sd_task.usCpuAffiMask = CPUID_TO_AFFI_MASK(0);
#endif
    ret = LOS_TaskCreate(&ul_taskid, &st_sd_task);
    if (ret != 0) {
        oal_io_print1("Failed to create %s thread\n", (uintptr_t)pc_thread_name);
        return NULL;
    }
    if (g_pstTaskCBArray == HI_NULL) {
        oal_io_print0("oal_kthread_create g_pstTaskCBArray null\n");
        return NULL;
    }

    kthread = (oal_kthread_stru *)&g_pstTaskCBArray[ul_taskid];
    return kthread;
}


static inline hi_void oal_kthread_stop(const oal_kthread_stru *kthread)
{
    hi_u32 ret;
    if (oal_unlikely(kthread == NULL)) {
        oal_io_print0("thread can't stop\n");
        return;
    }
    oal_io_print1("%s thread stop\n", (uintptr_t)kthread->pcTaskName);
    ret = LOS_TaskDelete(kthread->uwTaskID);
    if (ret != 0) {
        oal_io_print0("LOS_TaskDelete fail!\n");
    }
}

static inline hi_s32 oal_kthread_should_stop(hi_void)
{
    return 0;
}

static inline hi_u32 oal_set_thread_property(const oal_kthread_stru *thread, const oal_kthread_param_stru *thread_param)
{
    return LOS_TaskPriSet(thread->uwTaskID, thread_param->l_prio);
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
