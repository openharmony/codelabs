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

#ifndef __FRW_EVENT_SCHED_H__
#define __FRW_EVENT_SCHED_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* 获取事件队列中已经缓存的事件个数 */
#define frw_event_queue_get_pending_events_num(_pst_event_queue) oal_queue_get_length(&(_pst_event_queue)->queue)

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
  枚举名  : frw_sched_policy_enum_uint8
  协议表格:
  枚举说明: 事件队列对应的调度策略
**************************************************************************** */
typedef enum {
    FRW_SCHED_POLICY_HI     = 0,   /* 高优先级队列 */
    FRW_SCHED_POLICY_NORMAL = 1,   /* 普通优先级队列 */
    FRW_SCHED_HCC_ASSEM     = 2,   /* 特殊事件，用于HCC聚合 */

    FRW_SCHED_POLICY_BUTT
} frw_sched_policy_enum;
typedef hi_u8 frw_sched_policy_enum_uint8;

/* ****************************************************************************
  枚举名  : frw_event_queue_state_enum_uint8
  协议表格:
  枚举说明: 事件队列状态
**************************************************************************** */
typedef enum {
    FRW_EVENT_QUEUE_STATE_INACTIVE = 0, /* 事件队列不活跃(不可被调度) */
    FRW_EVENT_QUEUE_STATE_ACTIVE,       /* 事件队列活跃(可被调度) */

    FRW_EVENT_QUEUE_STATE_BUTT
} frw_event_queue_state_enum;
typedef hi_u8 frw_event_queue_state_enum_uint8;

/* ****************************************************************************
  枚举名  : frw_vap_state_enum_uint8
  协议表格:
  枚举说明: 事件部署类型
**************************************************************************** */
typedef enum {
    FRW_VAP_STATE_RESUME = 0, /* VAP恢复 */
    FRW_VAP_STATE_PAUSE,      /* VAP暂停 */

    FRW_VAP_STATE_BUTT
} frw_vap_state_enum;
typedef hi_u8 frw_vap_state_enum_uint8;

/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
/* ****************************************************************************
  结构名  : frw_event_sched_queue_stru
  结构说明: 可调度队列
**************************************************************************** */
typedef struct {
    oal_spin_lock_stru st_lock;
    hi_u32             total_weight_cnt;    /* 可调度队列上的总权重计数器 */
    hi_list            head;                /* 可调度队列的事件链表头 */
} frw_event_sched_queue_stru;

/* ****************************************************************************
  结构名  : frw_event_queue_stru
  结构说明: 事件队列结构体
**************************************************************************** */
typedef struct tag_frw_event_queue_stru {
    oal_spin_lock_stru               st_lock;
    oal_queue_stru                   queue;       /* 队列 */
    frw_event_queue_state_enum_uint8 state;       /* 队列状态 */
    frw_sched_policy_enum_uint8      policy;      /* 队列调度策略(高优先级、普通优先级) */

    hi_u8                            weight;      /* WRR权重配置值 */
    hi_u8                            weight_cnt;  /* WRR权重计数器 */

    frw_vap_state_enum_uint8         vap_state;   /* VAP的状态值，0为恢复，1为暂停 */
    hi_u8                            auc_resv[3]; /* 3:保留数组大小 */
    hi_list                          list;
}frw_event_queue_stru;

/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 frw_event_sched_init(frw_event_sched_queue_stru *sched_queue);
hi_u32 frw_event_queue_init(frw_event_queue_stru *event_queue, hi_u8 weight, frw_sched_policy_enum_uint8 policy,
    frw_event_queue_state_enum_uint8 state, hi_u8 max_events);
hi_void frw_event_queue_destroy(frw_event_queue_stru *event_queue);
hi_void *frw_event_sched_pick_next_event_queue_wrr(frw_event_sched_queue_stru *sched_queue);
hi_void frw_event_sched_deactivate_queue(frw_event_sched_queue_stru *sched_queue, frw_event_queue_stru *event_queue);
hi_u32 frw_event_sched_activate_queue_no_lock(frw_event_sched_queue_stru *sched_queue,
                                              frw_event_queue_stru *event_queue);
hi_void* frw_event_schedule(frw_event_sched_queue_stru *sched_queue);

/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of frw_event_sched.h */
