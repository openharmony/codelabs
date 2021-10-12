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
/* 1 头文件包含 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "hi_task.h"
#endif
#include "frw_event.h"
#include "frw_task.h"
#include "frw_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define FRW_THREAD_NAME_MAX_SIZE 20
#endif
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
#define FRW_TASK_PRIO 4 /* 驱动task的优先级,建议比上层高 */
hi_char g_frw_thread_name[] = { "hisi_frw" };
#endif
/* *****************************************************************************
    事件处理全局变量
****************************************************************************** */
frw_task_stru g_ast_event_task;
/* ****************************************************************************
    线程退出标志全局变量
****************************************************************************** */
hi_u8 g_frw_exit = HI_FALSE;
hi_u8 g_frw_stop = HI_FALSE;
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
/* ****************************************************************************S
 功能描述  : frw 内核线程主程序
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_void *frw_task_thread(hi_void *bind_cpu)
{
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_HMAC)
    hi_u32 event;
#else
    hi_s32 ret;
#endif
    hi_unref_param(bind_cpu);
    /* 死循环直至事件调度完 */
    for (;;) {
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_HMAC)
        while (!frw_task_thread_condition_check()) {
            hi_event_wait(get_app_event_id(), HI_EVENT_FRW_TASK, &event, HI_SYS_WAIT_FOREVER,
                HI_EVENT_WAITMODE_AND | HI_EVENT_WAITMODE_CLR);
        }
#else
        /* state为TASK_INTERRUPTIBLE，condition不成立则线程阻塞，直到被唤醒进入waitqueue */
        ret = hi_wait_event_interruptible(g_ast_event_task.frw_wq, frw_task_thread_condition_check() == HI_TRUE);
        if (oal_unlikely(ret == -ERESTARTSYS)) {
            hi_diag_log_msg_i0(0, "wifi task was interrupted by a signal\n");
            break;
        }
#endif
        frw_event_process_all_event();
    }
    return HI_NULL; /* 任务管理函数，for (;;) 死循环后需要return，lin_t e527告警屏蔽 */
}
#endif
#endif /* #if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD) */
/* ****************************************************************************
 功能描述  : kthread初始化接口
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 frw_task_init(void)
{
    /* 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(&g_ast_event_task, sizeof(g_ast_event_task), 0, sizeof(g_ast_event_task));

    hi_wait_queue_init_head(&g_ast_event_task.frw_wq);

#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
    hi_u32 ret, taskid;
    hi_task_attr attr = { 0 };

    attr.stack_size = FRW_TASK_SIZE;
    attr.task_prio = FRW_TASK_PRIO;
    attr.task_name = g_frw_thread_name;
    attr.task_policy = 1; /* SCHED_FIFO */
    attr.task_cpuid = 0;
    ret = hi_task_create(&taskid, &attr, frw_task_thread, 0);
    if (ret != HI_SUCCESS) {
        return HI_FAIL;
    }
    g_ast_event_task.taskid = taskid;
#elif (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_TASKLET)
    oal_task_kill(&g_ast_event_task.event_tasklet);
#endif
    return HI_SUCCESS;
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 线程退出函数
 修改历史      :
  1.日    期   : 2015年4月9日
**************************************************************************** */
hi_void frw_task_exit(hi_void)
{
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
    if (g_ast_event_task.taskid) {
        hi_task_delete(g_ast_event_task.taskid);
        g_ast_event_task.taskid = 0;
    }
#elif (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_TASKLET)
    oal_task_kill(&g_ast_event_task.event_tasklet);
#endif
}
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */

/* ****************************************************************************
 功能描述  : 供外部模块注册tasklet处理函数中执行的函数
 输入参数  : p_func: 需要被执行的函数

 修改历史      :
  1.日    期   : 2015年4月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void frw_task_event_handler_register(hi_void (*func)(hi_void))
{
    if (oal_unlikely(func == HI_NULL)) {
        hi_diag_log_msg_i0(0, "{frw_task_event_handler_register:: p_func is null ptr}");
        return;
    }

#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_TASKLET)
    g_ast_event_task.event_handler_func = func;
#endif
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 唤醒event时间处理线程，与wake_event_interruptible对应
 修改历史      :
  1.日    期   : 2015年4月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void frw_task_sched(hi_void)
{
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_HMAC)
    (hi_void)hi_event_send(get_app_event_id(), HI_EVENT_FRW_TASK);
#else
    hi_wait_queue_wake_up_interrupt(&g_ast_event_task.frw_wq);
#endif
#elif (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_TASKLET)
    if (oal_task_is_scheduled(&g_ast_event_task.event_tasklet)) {
        return;
    }
    oal_task_sched(&g_ast_event_task.event_tasklet);
#endif
}
#endif

#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_TASKLET)
/* ****************************************************************************
 功能描述  : 将tasklet调度执行，被IPI中断调度执行
**************************************************************************** */
hi_void frw_remote_task_receive(hi_void *info)
{
    oal_tasklet_stru *task = (oal_tasklet_stru *)info;
    oal_task_sched(task);
}

/* ****************************************************************************
 功能描述  : 使用IPI中断，调度目标core上的tasklet执行处理事件
**************************************************************************** */
hi_void frw_task_sched_on_cpu(oal_tasklet_stru *task)
{
    oal_smp_call_function_single(0, frw_remote_task_receive, (hi_void *)task, 0);
}
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_s32 frw_task_thread(hi_void *ul_bind_cpu)
{
    hi_s32 ret;
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
    hi_u32 ul_empty_count = 0;
#endif
    g_frw_exit = HI_FALSE; /* 线程初始化时为此全局变量赋值 */
    g_frw_stop = HI_FALSE; /* 线程初始化时为此全局变量赋值 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    allow_signal(SIGTERM);
#endif
    for (;;) {
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
        hi_u32 ul_event_count;
#endif
        if (oal_kthread_should_stop()) {
            break;
        }

        /* state为TASK_INTERRUPTIBLE，condition不成立则线程阻塞，直到被唤醒进入waitqueue */
        /*lint -e730*/
#ifdef _PRE_FRW_EVENT_PROCESS_TRACE_DEBUG
        frw_event_last_pc_trace(__FUNCTION__, __LINE__, (hi_u32)(unsigned long)ul_bind_cpu);
#endif
        ret = hi_wait_event_interruptible(g_ast_event_task.frw_wq,
            (HI_TRUE == frw_task_thread_condition_check() || g_frw_exit));
        /*lint +e730*/
        if (oal_unlikely(ret == -ERESTARTSYS)) {
            oal_io_print1("wifi task %s was interrupted by a signal\n", oal_get_current_task_name());
            break;
        }
        if (g_frw_exit == HI_TRUE) {
            break;
        }
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
        g_ast_event_task.ul_total_loop_cnt++;

        ul_event_count = g_ast_event_task.ul_total_event_cnt;
#endif
        frw_event_process_all_event();
#if (_PRE_FRW_FEATURE_PROCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCESS_ENTITY_THREAD)
        if (ul_event_count == g_ast_event_task.ul_total_event_cnt) {
            /* 空转 */
            ul_empty_count++;
            if (ul_empty_count == 10000) { /* empty count 10000 */
            }
        } else {
            if (ul_empty_count > g_ast_event_task.ul_max_empty_count) {
                g_ast_event_task.ul_max_empty_count = ul_empty_count;
            }
            ul_empty_count = 0;
        }
#endif
#ifdef _PRE_FRW_EVENT_PROCESS_TRACE_DEBUG
        frw_event_last_pc_trace(__FUNCTION__, __LINE__, (hi_u32)(unsigned long)ul_bind_cpu);
#endif
#if (!defined(CONFIG_PREEMPT) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION))
        cond_resched();
#endif
    }
    g_frw_stop = HI_TRUE;
    return 0;
}

hi_u32 frw_task_init()
{
    oal_kthread_stru *kthread = HI_NULL;
    oal_kthread_param_stru st_thread_param = { 0 };

    memset_s(&g_ast_event_task, sizeof(g_ast_event_task), 0, sizeof(g_ast_event_task));

    hi_wait_queue_init_head(&g_ast_event_task.frw_wq);

    st_thread_param.l_cpuid      = 0;
    st_thread_param.l_policy     = OAL_SCHED_RR;
    st_thread_param.l_prio       = FRW_TASK_PRIO;
    st_thread_param.ul_stacksize = FRW_TASK_SIZE;

    kthread = oal_kthread_create(g_frw_thread_name, frw_task_thread,
        (hi_void *)(st_thread_param.l_cpuid), &st_thread_param);
    if (IS_ERR_OR_NULL(kthread)) {
        return HI_FAIL;
    }

    g_ast_event_task.pst_event_kthread = kthread;
    g_ast_event_task.uc_task_state = FRW_TASK_STATE_IRQ_UNBIND;
    return HI_SUCCESS;
}

hi_void frw_task_exit(hi_void)
{
    int times = 0;
    if (g_ast_event_task.pst_event_kthread != HI_NULL) {
        g_frw_exit = HI_TRUE;
        frw_task_sched();
        while (!g_frw_stop && times < 10000) {
            usleep_range(50, 100); /* 50 100 均为延时参数，最小延时50，最大100 */
            times++;
        }
    }
}
hi_void frw_task_sched(hi_void)
{
    hi_wait_queue_wake_up_interrupt(&g_ast_event_task.frw_wq);
}
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
