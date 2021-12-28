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

#ifndef __FRW_TIMER_H__
#define __FRW_TIMER_H__

/* ***************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "frw_event.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
typedef hi_u32 (*frw_timeout_func)(hi_void *);

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
    hi_u32            handle;
    hi_void          *timeout_arg;      /* 超时处理函数入参 */
    frw_timeout_func  func;             /* 超时处理函数 */
    hi_u32            time_stamp;      /* 定时器启动时间 */
    hi_u32            timeout;         /* 过多长时间定时器超时 */
    hi_u32            timer_id;        /* 定时器唯一标识 */
    hi_u8             is_deleting;     /* 是否需要删除 */
    hi_u8             is_registerd;    /* 定时器是否已经注册 */
    hi_u8             is_periodic;     /* 定时器是否为周期的 */
    hi_u8             is_enabled :4;   /* 定时器是否使能 */
    hi_u8             is_running :4;
    oal_timer_list_stru      timer;
    hi_list           entry;           /* 定时器链表索引 */
}frw_timeout_stru;

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_void frw_timer_init(hi_void);
#ifdef _PRE_HDF_LINUX
hi_void frw_timer_timeout_proc_event(oal_timer_list_stru *arg);
#else
hi_void frw_timer_timeout_proc_event(unsigned long arg);
#endif
hi_void frw_timer_create_timer(frw_timeout_stru *timeout,
                               frw_timeout_func  timeout_func,
                               hi_u32 timeoutval,
                               hi_void *timeout_arg,
                               hi_u8 is_periodic);
hi_void frw_timer_immediate_destroy_timer(frw_timeout_stru *timeout);
hi_void frw_timer_restart_timer(frw_timeout_stru *timeout,
                                hi_u32 timeoutval,
                                hi_u8  is_periodic);
hi_void frw_timer_stop_timer(frw_timeout_stru *timeout);
hi_void frw_timer_delete_all_timer(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of frw_timer.h */
