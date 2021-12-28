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

#ifndef __OAL_LINUX_TIMER_H__
#define __OAL_LINUX_TIMER_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include <linux/timer.h>
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <los_swtmr.h>
#include "oal_time.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
typedef struct timer_list oal_timer_list_stru;

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
#ifdef _PRE_HDF_LINUX
typedef void (*oal_timer_func)(struct timer_list *);
#else
typedef void (*oal_timer_func)(unsigned long);
#endif

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

/* ****************************************************************************
 功能描述  : 初始化定时器
 输入参数  : pst_timer: 定时器结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_timer_init(oal_timer_list_stru *pst_timer, unsigned long ul_delay, oal_timer_func p_func,
    unsigned long ui_arg)
{
#ifdef _PRE_HDF_LINUX
    timer_setup(pst_timer, p_func, 0);
    hi_unref_param(ui_arg);
#else
    init_timer(pst_timer);
    pst_timer->function = p_func;
    pst_timer->data = ui_arg;
#endif
    pst_timer->expires = jiffies + msecs_to_jiffies(ul_delay);
}

/* ****************************************************************************
 功能描述  : 删除定时器
 输入参数  : pst_timer: 定时器结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_s32 oal_timer_delete(oal_timer_list_stru *pst_timer)
{
    return del_timer(pst_timer);
}

/* ****************************************************************************
 功能描述  : 同步删除定时器，用于多核
 输入参数  : pst_timer: 定时器结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_s32 oal_timer_delete_sync(oal_timer_list_stru *pst_timer)
{
    return del_timer_sync(pst_timer);
}

/* ****************************************************************************
 功能描述  : 激活定时器
 输入参数  : pst_timer: 定时器结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_timer_add(oal_timer_list_stru *pst_timer)
{
    add_timer(pst_timer);
}

/* ****************************************************************************
 功能描述  : 重启定时器
 输入参数  : pst_timer: 结构体指针
             ui_expires: 重启的溢出事件
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_s32 oal_timer_start(oal_timer_list_stru *pst_timer, unsigned long ui_delay)
{
    return mod_timer(pst_timer, (jiffies + msecs_to_jiffies(ui_delay)));
}

/* ****************************************************************************
 功能描述  : 指定cpu,重启定时器,调用时timer要处于非激活状态否者会死机
 输入参数  : pst_timer: 结构体指针
             ui_expires: 重启的溢出事件
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_timer_start_on(oal_timer_list_stru *pst_timer, unsigned long ui_delay, hi_s32 cpu)
{
    pst_timer->expires = jiffies + msecs_to_jiffies(ui_delay);
    add_timer_on(pst_timer, cpu);
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 初始化定时器
 输入参数  : pst_timer: 定时器结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_timer_init(oal_timer_list_stru *pst_timer, hi_u32 ul_delay, oal_timer_func p_func,
    hi_u64 ui_arg)
{
    init_timer(pst_timer);
    pst_timer->expires = OAL_MSECS_TO_JIFFIES(ul_delay);
    pst_timer->function = p_func;
    pst_timer->data = ui_arg;
}

/* ****************************************************************************
 功能描述  : 删除定时器
 输入参数  : pst_timer: 定时器结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_s32 oal_timer_delete(oal_timer_list_stru *pst_timer)
{
    return del_timer(pst_timer);
}

/* ****************************************************************************
 功能描述  : 同步删除定时器，用于多核
 输入参数  : pst_timer: 定时器结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_s32 oal_timer_delete_sync(oal_timer_list_stru *pst_timer)
{
    return del_timer_sync(pst_timer);
}

/* ****************************************************************************
 功能描述  : 激活定时器
 输入参数  : pst_timer: 定时器结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_timer_add(oal_timer_list_stru *pst_timer)
{
    add_timer(pst_timer);
}

/* ****************************************************************************
 功能描述  : 重启定时器
 输入参数  : pst_timer: 结构体指针
             ui_expires: 重启的溢出事件
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_s32 oal_timer_start(oal_timer_list_stru *pst_timer, hi_u64 ui_delay)
{
    if (pst_timer->flag == TIMER_UNVALID) {
        pst_timer->expires = OAL_MSECS_TO_JIFFIES((hi_u32)ui_delay);
        add_timer(pst_timer);
        return 0;
    } else {
        return mod_timer(pst_timer, OAL_MSECS_TO_JIFFIES((hi_u32)ui_delay));
    }
}

/* ****************************************************************************
 功能描述  : 指定cpu,重启定时器,调用时timer要处于非激活状态否者会死机
 输入参数  : pst_timer: 结构体指针
             ui_expires: 重启的溢出事件
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_timer_start_on(oal_timer_list_stru *pst_timer, hi_u64 ui_delay, hi_s32 cpu)
{
    hi_unref_param(cpu);
    pst_timer->expires = OAL_MSECS_TO_JIFFIES(ui_delay);
    add_timer(pst_timer);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_timer.h */
