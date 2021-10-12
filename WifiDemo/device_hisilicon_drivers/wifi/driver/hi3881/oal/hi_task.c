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
#include <hi_types_base.h>
#include <los_task.h>
#include <los_task_pri.h>
#include "hi_config.h"
#include "hi_task.h"

extern hi_u32 ms2systick(HI_IN hi_u32 ms, HI_IN hi_bool include0);

hi_u32 hi_task_create(hi_u32 *taskid, const hi_task_attr *attr, hi_void *(*task_route)(hi_void *), hi_void *arg)
{
    TSK_INIT_PARAM_S my_task = { 0, };
    hi_u32 ret;

    /* 内核接口对输入参数有判断，此处省略 */
    if (taskid == HI_NULL || task_route == HI_NULL) {
        return HI_ERR_TASK_INVALID_PARAM;
    }

    if (attr != NULL) {
        my_task.pcName      = attr->task_name;
        my_task.uwStackSize = attr->stack_size;
        my_task.usTaskPrio  = attr->task_prio;
    } else {
        my_task.pcName      = HI_DEFAULT_TSKNAME;
        my_task.uwStackSize = HI_DEFAULT_STACKSIZE;
        my_task.usTaskPrio  = HI_DEFAULT_TSKPRIO;
    }
    /* user task priority is limit bteween 1 and 30. */
    if (my_task.usTaskPrio == OS_TASK_PRIORITY_HIGHEST || my_task.usTaskPrio == OS_TASK_PRIORITY_LOWEST) {
        my_task.usTaskPrio = 20; /* normal, usTaskPrio should be [20-30]. */
    }

    if (my_task.uwStackSize == 0) {
        my_task.uwStackSize = HI_DEFAULT_STACKSIZE;
    }

    my_task.uwResved = LOS_TASK_STATUS_DETACHED;
    my_task.pfnTaskEntry = (TSK_ENTRY_FUNC)task_route;
    my_task.auwArgs[0] = (hi_u32)(uintptr_t)arg;
#ifdef LOSCFG_KERNEL_SMP
    my_task.usCpuAffiMask = CPUID_TO_AFFI_MASK(0);
#endif
    ret = LOS_TaskCreate(taskid, &my_task);
    if (ret != LOS_OK) {
        return HI_ERR_TASK_CREATE_FAIL;
    }

    return HI_ERR_SUCCESS;
}

hi_u32 hi_task_delete(hi_u32 taskid)
{
    hi_u32 ret;

    ret = LOS_TaskDelete(taskid);
    if (ret != LOS_OK) {
        return HI_ERR_TASK_DELETE_FAIL;
    }

    return HI_ERR_SUCCESS;
}

hi_u32 hi_task_suspend(hi_u32 taskid)
{
    hi_u32 ret;

    ret = LOS_TaskSuspend(taskid);
    if (ret != LOS_OK) {
        return HI_ERR_TASK_SUPPEND_FAIL;
    }

    return HI_ERR_SUCCESS;
}

hi_u32 hi_task_resume(hi_u32 taskid)
{
    hi_u32 ret;

    ret = LOS_TaskResume(taskid);
    if (ret != LOS_OK) {
        return HI_ERR_TASK_RESUME_FAIL;
    }

    return HI_ERR_SUCCESS;
}

hi_u32 hi_task_get_priority(hi_u32 taskid, hi_u32 *priority)
{
    hi_u16 pri;

    if (priority == HI_NULL) {
        return HI_ERR_TASK_INVALID_PARAM;
    }

    pri = LOS_TaskPriGet(taskid);
    if (pri == (hi_u16)OS_INVALID) {
        return HI_ERR_TASK_GET_PRI_FAIL;
    }

    *priority = pri;
    return HI_ERR_SUCCESS;
}

hi_u32 hi_task_set_priority(hi_u32 taskid, hi_u32 priority)
{
    hi_u32 ret;

    ret = LOS_TaskPriSet(taskid, (hi_u16)priority);
    if (ret != LOS_OK) {
        return HI_ERR_TASK_SET_PRI_FAIL;
    }

    return HI_ERR_SUCCESS;
}

hi_u32 hi_task_get_current_id(hi_void)
{
    hi_u32 tmp_tid;

    tmp_tid = LOS_CurTaskIDGet();
    if (tmp_tid == LOS_ERRNO_TSK_ID_INVALID) {
        return HI_INVALID_TASK_ID;
    }

    return tmp_tid;
}

hi_void hi_task_lock(hi_void)
{
    LOS_TaskLock();
}

hi_void hi_task_unlock(hi_void)
{
    LOS_TaskUnlock();
}

hi_u32 hi_sleep(hi_u32 ms)
{
    hi_u32 ret;

    hi_u32 tick = ms2systick(ms, HI_FALSE);

    ret = LOS_TaskDelay(tick);
    if (ret != LOS_OK) {
        return HI_ERR_TASK_DELAY_FAIL;
    }

    return HI_ERR_SUCCESS;
}

ROM_TEXT_PATCH_SECTION hi_void hi_task_join(hi_u32 taskid, hi_u32 wait_interval)
{
    if (taskid == hi_task_get_current_id()) {
        return;
    }

    if (wait_interval < HI_MILLISECOND_PER_TICK) {
        wait_interval = HI_MILLISECOND_PER_TICK;
    }

    hi_task_info info = { 0 };
    while (!(info.status & OS_TASK_STATUS_UNUSED)) {
        if (hi_task_get_info(taskid, &info) != HI_ERR_SUCCESS) {
            break;
        }

        if (hi_sleep(wait_interval) != HI_ERR_SUCCESS) {
            break;
        }
    }
}
