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

#ifndef __OAL_LINUX_SEMAPHORE_H__
#define __OAL_LINUX_SEMAPHORE_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/semaphore.h>
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <los_sem_pri.h>
#include <los_sem.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct semaphore oal_semaphore_stru;
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define SEM_CB_S LosSemCB
#define usSemID semID
typedef SEM_CB_S oal_semaphore_stru;
#endif

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_void oal_sema_init(oal_semaphore_stru *sem, hi_s32 val)
{
    sema_init(sem, val);
}


static inline hi_void oal_up(oal_semaphore_stru *sem)
{
    up(sem);
}

static inline hi_void oal_down(oal_semaphore_stru *sem)
{
    down(sem);
}

static inline hi_s32 oal_down_timeout(oal_semaphore_stru *sem, hi_s32 timeout)
{
    return down_timeout(sem, timeout);
}

static inline hi_s32 oal_down_interruptible(oal_semaphore_stru *sem)
{
    return down_interruptible(sem);
}

static inline hi_s32 oal_down_trylock(oal_semaphore_stru *sem)
{
    return down_trylock(sem);
}

static inline hi_s32 oal_sema_destroy(const oal_semaphore_stru *pst_sem)
{
    return 0;
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
static inline hi_void oal_sema_init(oal_semaphore_stru *pst_sem, hi_u16 us_val)
{
    hi_u32 ul_semhandle;
    if (pst_sem == HI_NULL) {
        oal_io_print1("[%s]pst_sem is null!\n", (uintptr_t) __func__);
        return;
    }

    if (LOS_OK != LOS_SemCreate(us_val, &ul_semhandle)) {
        oal_io_print0("LOS_SemCreate failed!\n");
        return;
    }
    *pst_sem = *(GET_SEM(ul_semhandle));
}

static inline hi_void oal_up(const oal_semaphore_stru *pst_sem)
{
    if (pst_sem == HI_NULL) {
        oal_io_print1("[%s]pst_sem is null!\n", (uintptr_t) __func__);
        return;
    }

    if (0 != LOS_SemPost(pst_sem->usSemID)) {
        oal_io_print0("LOS_SemPost failed!\n");
        return;
    }
}

static inline hi_void oal_down(const oal_semaphore_stru *pst_sem)
{
    if (pst_sem == HI_NULL) {
        oal_io_print1("[%s]pst_sem is null!\n", __func__);
        return;
    }
    if (0 != LOS_SemPend(pst_sem->usSemID, 0xFFFFFFFF)) {
        oal_io_print0("LOS_SemPend failed!\n");
        return;
    }
}

static inline hi_s32 oal_down_timeout(const oal_semaphore_stru *pst_sem, hi_u32 ul_timeout)
{
    hi_u32 ul_reval;
    if (pst_sem == HI_NULL) {
        oal_io_print1("[%s]pst_sem is null!\n", (uintptr_t) __func__);
        return -1;
    }

    ul_reval = LOS_SemPend(pst_sem->usSemID, ul_timeout);
    if (ul_reval != 0) {
        return -1;
    }
    return 0;
}

static inline hi_s32 oal_down_interruptible(const oal_semaphore_stru *pst_sem)
{
    hi_u32 ul_reval;
    if (pst_sem == HI_NULL) {
        oal_io_print1("[%s]pst_sem is null!\n", __func__);
        return -1;
    }
    ul_reval = LOS_SemPend(pst_sem->usSemID, 0xFFFFFFFF);
    if (ul_reval != 0) {
        return -1;
    }
    return 0;
}

static inline hi_s32 oal_down_trylock(const oal_semaphore_stru *pst_sem)
{
    hi_u32 ul_reval;
    if (pst_sem == HI_NULL) {
        oal_io_print1("[%s]pst_sem is null!\n", __func__);
        return -1;
    }

    ul_reval = LOS_SemPend(pst_sem->usSemID, 0);
    if (ul_reval != 0) {
        return -1;
    }
    return 0;
}

static inline hi_s32 oal_sema_destroy(const oal_semaphore_stru *pst_sem)
{
    hi_u32 ul_reval;
    if (pst_sem == HI_NULL) {
        oal_io_print1("[%s]pst_sem is null!\n", __func__);
        return -1;
    }

    ul_reval = LOS_SemDelete(pst_sem->usSemID);
    if (ul_reval != 0) {
        return -1;
    }
    return 0;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_completion.h */
