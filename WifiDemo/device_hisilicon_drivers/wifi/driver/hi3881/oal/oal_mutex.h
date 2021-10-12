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
#ifndef __OAL_MUTEX_H__
#define __OAL_MUTEX_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/mutex.h>
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <pthread.h>
#endif
#include "hi_types_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct mutex oal_mutex_stru;
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
typedef pthread_mutex_t oal_mutex_stru;
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
#define    OAL_MUTEX_INIT(mutex)        mutex_init(mutex)
#define    OAL_MUTEX_DESTROY(mutex)     mutex_destroy(mutex)
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define    OAL_MUTEX_INIT(mutex)        pthread_mutex_init(mutex, NULL)
#define    OAL_MUTEX_DESTROY(mutex)     pthread_mutex_destroy(mutex)
#endif

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
static inline hi_void oal_mutex_lock(oal_mutex_stru *lock)
{
    mutex_lock(lock);
}

static inline hi_s32 oal_mutex_trylock(oal_mutex_stru *lock)
{
    return mutex_trylock(lock);
}

static inline hi_void oal_mutex_unlock(oal_mutex_stru *lock)
{
    mutex_unlock(lock);
}
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
static inline hi_void oal_mutex_lock(oal_mutex_stru *lock)
{
    hi_s32 ret;
    ret = pthread_mutex_lock(lock);
    if (ret != ENOERR) {
    }
}

static inline hi_s32 oal_mutex_trylock(oal_mutex_stru *lock)
{
    return pthread_mutex_trylock(lock);
}

static inline hi_void oal_mutex_unlock(oal_mutex_stru *lock)
{
    hi_s32 ret;
    ret = pthread_mutex_unlock(lock);
    if (ret != ENOERR) {
    }
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_mutex.h */
