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

#ifndef __OAL_LINUX_WAKE_LOCK_H__
#define __OAL_LINUX_WAKE_LOCK_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_mutex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
typedef struct _oal_wakelock_stru_ {
#ifdef CONFIG_HAS_WAKELOCK
    struct wake_lock st_wakelock; /* wakelock锁 */
    oal_spin_lock_stru lock;      /* wakelock锁操作spinlock锁 */
#endif
    unsigned long lock_count;  /* 持有wakelock锁的次数 */
    unsigned long locked_addr; /* the locked address */
} oal_wakelock_stru;

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
/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
static inline hi_void oal_wake_lock_init(oal_wakelock_stru *pst_wakelock, char *name)
{
#ifdef CONFIG_HAS_WAKELOCK
    memset_s((hi_u8 *)pst_wakelock, sizeof(oal_wakelock_stru), 0, sizeof(oal_wakelock_stru));

    wake_lock_init(&pst_wakelock->st_wakelock, WAKE_LOCK_SUSPEND, name ? name : "wake_lock_null");
    oal_spin_lock_init(&pst_wakelock->lock);
    pst_wakelock->lock_count = 0;
    pst_wakelock->locked_addr = 0;
#else
    hi_unref_param(pst_wakelock);
    hi_unref_param(name);
#endif
}

static inline hi_void oal_wake_lock_exit(oal_wakelock_stru *pst_wakelock)
{
#ifdef CONFIG_HAS_WAKELOCK
    wake_lock_destroy(&pst_wakelock->st_wakelock);
#else
    hi_unref_param(pst_wakelock);
#endif
}

static inline void oal_wake_lock(oal_wakelock_stru *pst_wakelock)
{
#ifdef CONFIG_HAS_WAKELOCK
    unsigned long ul_flags;

    oal_spin_lock_irq_save(&pst_wakelock->lock, &ul_flags);
    if (!pst_wakelock->lock_count) {
        wake_lock(&pst_wakelock->st_wakelock);
        pst_wakelock->locked_addr = (unsigned long)_RET_IP_;
    }
    pst_wakelock->lock_count++;
    oal_spin_unlock_irq_restore(&pst_wakelock->lock, &ul_flags);
#else
    hi_unref_param(pst_wakelock);
#endif
}

static inline void oal_wake_unlock(oal_wakelock_stru *pst_wakelock)
{
#ifdef CONFIG_HAS_WAKELOCK
    unsigned long ul_flags;

    oal_spin_lock_irq_save(&pst_wakelock->lock, &ul_flags);
    if (pst_wakelock->lock_count) {
        pst_wakelock->lock_count--;
        if (!pst_wakelock->lock_count) {
            wake_unlock(&pst_wakelock->st_wakelock);
            pst_wakelock->locked_addr = (unsigned long)0x0;
        }
    }
    oal_spin_unlock_irq_restore(&pst_wakelock->lock, &ul_flags);
#else
    hi_unref_param(pst_wakelock);
#endif
}

static inline unsigned long oal_wakelock_active(oal_wakelock_stru *pst_wakelock)
{
#ifdef CONFIG_HAS_WAKELOCK
    return (unsigned long)wake_lock_active(&pst_wakelock->st_wakelock);
#else
    hi_unref_param(pst_wakelock);
    return 0;
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_wakelock.h */
