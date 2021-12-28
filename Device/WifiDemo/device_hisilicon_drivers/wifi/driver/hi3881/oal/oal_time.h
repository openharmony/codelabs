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

#ifndef __OAL_TIME_H__
#define __OAL_TIME_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/rtc.h>
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <los_sys.h>
#include <linux/kernel.h>
#include "hi_types.h"
#include <linux/hrtimer.h>
#include <linux/rtc.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* 32位寄存器最大长度 */
#define OAL_TIME_US_MAX_LEN (0xFFFFFFFF - 1)

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* 获取毫秒级时间戳 */
#define hi_get_milli_seconds() jiffies_to_msecs(jiffies)
#endif
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define hi_get_milli_seconds() LOS_Tick2MS(OAL_TIME_JIFFY)
#endif

/* 获取高精度毫秒时间戳,精度1ms */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
#define OAL_TIME_GET_HIGH_PRECISION_MS() oal_get_time_stamp_from_timeval()
#endif

#define OAL_ENABLE_CYCLE_COUNT()
#define OAL_DISABLE_CYCLE_COUNT()
#define OAL_GET_CYCLE_COUNT() 0

/* 寄存器反转模块运行时间计算 */
#define OAL_TIME_CALC_RUNTIME(_ul_start, _ul_end) \
    ((((OAL_TIME_US_MAX_LEN) / HZ) * 1000) + ((OAL_TIME_US_MAX_LEN) % HZ) * (1000 / HZ) - (_ul_start) + (_ul_end))

#define OAL_TIME_HZ HZ

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define OAL_TIME_JIFFY jiffies
#define OAL_MSECS_TO_JIFFIES(_msecs) msecs_to_jiffies(_msecs)

#define OAL_JIFFIES_TO_MSECS(_jiffies) jiffies_to_msecs(_jiffies)
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define OAL_TIME_JIFFY LOS_TickCountGet()
#define OAL_MSECS_TO_JIFFIES(_msecs) LOS_MS2Tick(_msecs)
#define OAL_JIFFIES_TO_MSECS(_jiffies) LOS_Tick2MS(_jiffies)
#endif

/* 获取从_ul_start到_ul_end的时间差 */
#define OAL_TIME_GET_RUNTIME(_ul_start, _ul_end) \
    (((_ul_start) > (_ul_end)) ? (OAL_TIME_CALC_RUNTIME((_ul_start), (_ul_end))) : ((_ul_end) - (_ul_start)))

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
  7 STRUCT定义
**************************************************************************** */
typedef struct {
    signed long i_sec;
    signed long i_usec;
} oal_time_us_stru;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef ktime_t oal_time_t_stru;
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
typedef union ktime oal_time_t_stru;

#ifndef ktime_t
#define ktime_t union ktime
#endif

#endif
typedef struct timeval oal_timeval_stru;
typedef struct rtc_time oal_rtctime_stru;

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

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
/* ****************************************************************************
 功能描述  : 获取微妙精度级的时间戳
 输入参数  : pst_usec: 时间结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_time_get_stamp_us(oal_time_us_stru *pst_usec)
{
    struct timespec ts;
    getnstimeofday(&ts);
    pst_usec->i_sec     = ts.tv_sec;
    pst_usec->i_usec    = ts.tv_nsec / 1000; /* 1us 是 1000ns */
}
#endif
/* ****************************************************************************
 功能描述  : 调用内核函数获取当前时间戳
 输入参数  : hi_void
 输出参数  :
 返 回 值  :
**************************************************************************** */
static inline oal_time_t_stru oal_ktime_get(hi_void)
{
    return ktime_get();
}

/* ****************************************************************************
 功能描述  : 调用内核函数获取时间差值
 输入参数  :
 输出参数  :
 返 回 值  :
**************************************************************************** */
static inline oal_time_t_stru oal_ktime_sub(const oal_time_t_stru lhs, const oal_time_t_stru rhs)
{
    return ktime_sub(lhs, rhs);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
/* ****************************************************************************
 功能描述  : 获取时间精度
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_u64 oal_get_time_stamp_from_timeval(hi_void)
{
    struct timeval tv;
    hi_u64 curr_time;

    do_gettimeofday(&tv);
    curr_time = tv.tv_usec;
    do_div(curr_time, 1000);                  /* div 1000 */
    curr_time = curr_time + tv.tv_sec * 1000; /* mul 1000 */

    return curr_time;
}

static inline hi_void oal_do_gettimeofday(oal_timeval_stru *tv)
{
    do_gettimeofday(tv);
}

static inline hi_void oal_rtc_time_to_tm(unsigned long time, oal_rtctime_stru *tm)
{
    rtc_time_to_tm(time, tm);
}
#endif

/* ****************************************************************************
 功能描述  : 判断ul_time是否比当前时间早
             若早，表示超时时间已过；若不早，表明还未超时
 输入参数  : unsigned long ui_time
 输出参数  : 无
 返 回 值  : static inline hi_u32
**************************************************************************** */
static inline hi_u32 oal_time_is_before(unsigned long ui_time)
{
    return (hi_u32)time_is_before_jiffies(ui_time);
}

/* ****************************************************************************
 功能描述  : 判断时间戳ul_time_a是否在ul_time_b之后:
 输入参数  : unsigned long ui_time
 输出参数  : 无
 返 回 值  : Return: 1 ul_time_a在ul_time_b之后; 否则 Return: 0
**************************************************************************** */
static inline hi_u32 oal_time_after(hi_u64 ul_time_a, hi_u64 ul_time_b)
{
    return (hi_u32)time_after((unsigned long)ul_time_a, (unsigned long)ul_time_b);
}

static inline unsigned long oal_ktime_to_us(const oal_time_t_stru kt)
{
    return ktime_to_us(kt);
}

static inline hi_u32 oal_time_before_eq(hi_u32 ul_time_a, hi_u32 ul_time_b)
{
    return (hi_u32)time_before_eq((unsigned long)ul_time_a, (unsigned long)ul_time_b);
}

static inline hi_u32 oal_time_before(hi_u32 ul_time_a, hi_u32 ul_time_b)
{
    return (hi_u32)time_before((unsigned long)ul_time_a, (unsigned long)ul_time_b);
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 获取微妙精度级的时间戳
 输入参数  : pst_usec: 时间结构体指针
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_void oal_time_get_stamp_us(oal_time_us_stru *pst_usec)
{
    oal_timeval_stru tv;
    do_gettimeofday(&tv);
    pst_usec->i_sec = tv.tv_sec;
    pst_usec->i_usec = tv.tv_usec;
}

/* ****************************************************************************
 功能描述  : 调用内核函数获取当前时间戳
 输入参数  : hi_void
 输出参数  :
 返 回 值  :
**************************************************************************** */
static inline oal_time_t_stru oal_ktime_get(hi_void)
{
    oal_time_t_stru time;
    time.tv64 = (hi_s64)LOS_TickCountGet();
    return time;
}

/* ****************************************************************************
 功能描述  : 调用内核函数获取时间差值
 输入参数  :
 输出参数  :
 返 回 值  :
**************************************************************************** */
static inline oal_time_t_stru oal_ktime_sub(const oal_time_t_stru lhs, const oal_time_t_stru rhs)
{
    oal_time_t_stru res;
    res.tv64 = lhs.tv64 - rhs.tv64;
    return res;
}

/* ****************************************************************************
 功能描述  : 获取时间精度
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_u64 oal_get_time_stamp_from_timeval(hi_void)
{
    oal_timeval_stru tv;
    do_gettimeofday(&tv);
    return ((hi_u64)tv.tv_usec / 1000 + (hi_u64)tv.tv_sec * 1000); /* div/mul 1000 */
}

static inline hi_void oal_do_gettimeofday(oal_timeval_stru *tv)
{
    do_gettimeofday(tv);
}

/* ****************************************************************************
 功能描述  : 判断时间戳ul_time_a是否在ul_time_b之后:
 输入参数  : hi_u64 ui_time
 输出参数  : 无
 返 回 值  : Return: 1 ul_time_a在ul_time_b之后; 否则 Return: 0
**************************************************************************** */
static inline hi_u32 oal_time_after(hi_u64 ul_time_a, hi_u64 ul_time_b)
{
    return (hi_u32)((hi_s64)((hi_s64)(ul_time_b) - (ul_time_a)) < 0);
}

/* ****************************************************************************
 功能描述  : 判断ul_time是否比当前时间早
             若早，表示超时时间已过；若不早，表明还未超时
 输入参数  :
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_u32 oal_time_is_before(hi_u64 ui_time)
{
    return oal_time_after(OAL_TIME_JIFFY, ui_time);
}

static inline hi_u64 oal_ktime_to_us(const oal_time_t_stru kt)
{
    return (OAL_JIFFIES_TO_MSECS((hi_u32)kt.tv64) * 1000); /* mul 1000 */
}

static inline hi_u64 oal_ktime_to_ms(const oal_time_t_stru kt)
{
    return (OAL_JIFFIES_TO_MSECS(kt.tv64));
}

static inline hi_u32 oal_time_before_eq(hi_u32 ul_time_a, hi_u32 ul_time_b)
{
    return (hi_u32)((hi_s64)((ul_time_a) - (ul_time_b)) <= 0);
}

static inline hi_u32 oal_time_before(hi_u32 ul_time_a, hi_u32 ul_time_b)
{
    return (hi_u32)((hi_s64)((hi_s64)(ul_time_a) - (ul_time_b)) < 0);
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_time.h */
