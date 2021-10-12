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

#ifndef __OAL_LINUX_WAIT_H__
#define __OAL_LINUX_WAIT_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include <linux/wait.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifdef _PRE_HDF_LINUX
typedef wait_queue_entry_t oal_wait_queue_stru;
#else
typedef wait_queue_t oal_wait_queue_stru;
#endif
#endif
typedef wait_queue_head_t oal_wait_queue_head_stru;

/* ****************************************************************************
  3 宏定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define hi_wait_queue_wake_up_interrupt(_pst_wq) wake_up_interruptible(_pst_wq)
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
extern void __wake_up_interruptible(wait_queue_head_t *wait);
#define hi_wait_queue_wake_up_interrupt(p_wait) __wake_up_interruptible(p_wait)
#endif

#define hi_wait_queue_wake_up(_pst_wq) wake_up(_pst_wq)

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define hi_interruptible_sleep_on(_pst_wq) interruptible_sleep_on(_pst_wq)
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
extern void __init_waitqueue_head(wait_queue_head_t *wait);
#endif
#define hi_wait_queue_init_head(_pst_wq) init_waitqueue_head(_pst_wq)

#define hi_wait_event_interruptible_timeout(_st_wq, _condition, _timeout) \
    wait_event_interruptible_timeout(_st_wq, _condition, _timeout)

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define hi_wait_event_timeout(_st_wq, _condition, _timeout) wait_event_timeout(_st_wq, _condition, _timeout)
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define hi_wait_event_timeout(_st_wq, _condition, _timeout) \
    wait_event_interruptible_timeout(_st_wq, _condition, _timeout)
#endif

#define hi_wait_event_interruptible(_st_wq, _condition) wait_event_interruptible(_st_wq, _condition)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_wait.h */
