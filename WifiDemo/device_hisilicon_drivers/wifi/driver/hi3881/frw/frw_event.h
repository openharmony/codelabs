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

#ifndef __FRW_EVENT_MAIN_H__
#define __FRW_EVENT_MAIN_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "frw_event_sched.h"
#include "oal_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
*****************************************************************************/
#define HI_EVENT_DEVICE_READY             BIT0
#define HI_EVENT_OPEN_BCPU_DONE           BIT1
#define HI_EVENT_CLOSE_BCPU_DONE          BIT2
#define HI_EVENT_HALT_BCPU_DONE           BIT3
#define HI_EVENT_WAKEUP_DONE              BIT4
#define HI_EVENT_SLEEP_REQUEST_ACK        BIT5
#define HI_EVENT_SLEEP_REQUEST_DONE       BIT6
#define HI_EVENT_SLEEPWORK_DONE           BIT7
#define HI_EVENT_RESUME_DONE              BIT8
#define HI_EVENT_SDIO_DRIVE_READY         BIT9
#define HI_EVENT_D2H_READY_EVENT          BIT10
#define HI_EVENT_WOW_ENABLE_READY         BIT11
#define HI_EVENT_CLOSE_DONE               BIT12
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_HMAC)
#define HI_EVENT_FRW_TASK                 BIT13
#endif

#ifdef _PRE_OAL_FEATURE_TASK_NEST_LOCK
extern oal_task_lock_stru g_frw_event_task_lock;
#define frw_event_task_lock() do {                                           \
        oal_smp_task_lock(&g_frw_event_task_lock); \
    } while (0)
#define frw_event_task_unlock() do {                                             \
        oal_smp_task_unlock(&g_frw_event_task_lock); \
    } while (0)
#else
#define frw_event_task_lock() do {                      \
    } while (0)
#define frw_event_task_unlock() do {                        \
    } while (0)
#endif
/* ****************************************************************************
  2 枚举定义
**************************************************************************** */
/* ****************************************************************************
  枚举名  : frw_event_deploy_enum_uint8
  协议表格:
  枚举说明: 事件部署类型
**************************************************************************** */
typedef enum {
    FRW_EVENT_DEPLOY_NON_IPC = 0, /* 非核间通信 */
    FRW_EVENT_DEPLOY_IPC,         /* 核间通讯 */

    FRW_EVENT_DEPLOY_BUTT
} frw_event_deploy_enum;
typedef hi_u8 frw_event_deploy_enum_uint8;

typedef enum {
    FRW_TIMEOUT_TIMER_EVENT, /* 以FRW_TIMER_DEFAULT_TIME为周期的定时器事件子类型 */

    FRW_TIMEOUT_SUB_TYPE_BUTT
} hmac_timeout_event_sub_type_enum;
typedef hi_u8 hmac_timeout_event_sub_type_enum_uint8;

/* ****************************************************************************
  枚举名  : frw_event_type_enum_uint8
  协议表格:
  枚举说明: 事件类型
**************************************************************************** */
typedef enum {
    FRW_EVENT_TYPE_HIGH_PRIO = 0,        /* 高优先级事件队列 */
    FRW_EVENT_TYPE_HOST_CRX,             /* 接收Host侧发来的配置事件 */
    FRW_EVENT_TYPE_HOST_DRX,             /* 接收Host侧发来的数据事件 */
    FRW_EVENT_TYPE_HOST_CTX,             /* 发向HOST侧的配置事件 */
    FRW_EVENT_TYPE_DMAC_TO_HMAC_CFG = 4, /* DMAC发往HMAC的配置事件 */
    FRW_EVENT_TYPE_WLAN_CRX,             /* 接收Wlan侧发来的管理/控制帧事件 */
    FRW_EVENT_TYPE_WLAN_DRX,             /* 接收Wlan侧发来的数据帧事件 */
    FRW_EVENT_TYPE_WLAN_CTX,             /* 管理/控制帧发送至Wlan侧事件 */
    FRW_EVENT_TYPE_WLAN_DTX,             /* 数据帧发送至Wlan侧事件 */
    FRW_EVENT_TYPE_WLAN_TX_COMP = 9,     /* 发送完成事件 */
    FRW_EVENT_TYPE_TBTT,                 /* TBTT中断事件 */
    FRW_EVENT_TYPE_TIMEOUT,              /* FRW普通超时事件 */
    FRW_EVENT_TYPE_DMAC_MISC = 12,       /* DMAC杂散事件 */
    FRW_EVENT_TYPE_HCC = 13,             /* HCC 事件 */
    /* 添加新的事件类型 */
    FRW_EVENT_TYPE_BUTT
} frw_event_type_enum;
typedef hi_u8 frw_event_type_enum_uint8;

typedef enum {
    OAM_DUMP_TYPE = 4,
    OAM_BACK_TRACE_TYPE,
    OAM_LOG_TYPE,
    OAM_PM_TYPE,
    OAM_HEATBAET_TYPE,
    OAM_SET_SOFT_RST,
} oam_even_type;

/* ****************************************************************************
  枚举名  : frw_event_type_enum_uint8
  协议表格:
  枚举说明: 事件分段号，取值[0, 1]
**************************************************************************** */
typedef enum {
    FRW_EVENT_PIPELINE_STAGE_0 = 0,
    FRW_EVENT_PIPELINE_STAGE_1,

    FRW_EVENT_PIPELINE_STAGE_BUTT
} frw_event_pipeline_enum;
typedef hi_u8 frw_event_pipeline_enum_uint8;

typedef oal_mem_stru frw_event_mem_stru; /* 事件结构体内存块转定义 */

/* ****************************************************************************
  3 宏定义
**************************************************************************** */
/* 事件表的最大个数 */
/* 一种类型的事件对应两个表项，所以事件表的大小为事件类型的2倍 */
#define FRW_EVENT_TABLE_MAX_ITEMS (FRW_EVENT_TYPE_BUTT * 2)

/* 事件头长度 */
#define FRW_EVENT_HDR_LEN            sizeof(frw_event_hdr_stru)
#define FRW_RX_EVENT_TRACK_NUM       256
#define FRW_EVENT_TRACK_NUM          128

#define frw_field_setup(_p, _m, _v) ((_p)->_m = _v)

/* 事件头修改宏(修改事件头中的pipeline和subtype) */
#define frw_event_hdr_modify_pipeline_and_subtype(_pst_event_hdr, _uc_sub_type) \
    do {                                                                        \
        frw_field_setup((_pst_event_hdr), pipeline, 1);                         \
        frw_field_setup((_pst_event_hdr), sub_type, (_uc_sub_type));            \
    } while (0)

/* 事件头初始化宏 */
#define frw_event_hdr_init(_pst_event_hdr, _en_type, _uc_sub_type, _us_length, _en_pipeline, _uc_vap_id) \
    do {                                                                                                 \
        frw_field_setup((_pst_event_hdr), us_length, ((_us_length) + FRW_EVENT_HDR_LEN));                \
        frw_field_setup((_pst_event_hdr), type, (_en_type));                                             \
        frw_field_setup((_pst_event_hdr), sub_type, (_uc_sub_type));                                     \
        frw_field_setup((_pst_event_hdr), pipeline, (_en_pipeline));                                     \
        frw_field_setup((_pst_event_hdr), vap_id, (_uc_vap_id));                                         \
    } while (0)

#define frw_get_event_stru(event_mem) ((frw_event_stru *)(event_mem)->puc_data)
#define frw_get_event_hdr(event_mem) ((frw_event_hdr_stru *)(&((frw_event_stru *)(event_mem)->puc_data)->event_hdr))
#define frw_get_event_payload(event_mem) ((hi_u8 *)((frw_event_stru *)(event_mem)->puc_data)->auc_event_data)

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
/* ****************************************************************************
  结构名  : frw_event_hdr_stru
  结构说明: 事件头结构体,
  备注    : uc_length的值为(payload长度 + 事件头长度 - 2)
**************************************************************************** */
typedef struct {
    frw_event_type_enum_uint8     type    : 4;    /* 事件类型 */
    hi_u8                         vap_id  : 4;    /* VAP ID */
    hi_u8                         sub_type: 6;    /* 事件子类型 */
    frw_event_pipeline_enum_uint8 pipeline: 2;    /* 事件分段号 */
    hi_u16                        us_length;      /* 事件总体长度 */
} frw_event_hdr_stru;

/* ****************************************************************************
  结构名  : frw_event_stru
  结构说明: 事件结构体
**************************************************************************** */
typedef struct {
    frw_event_hdr_stru    event_hdr;           /* 事件头 */
    hi_u8                 auc_event_data[];    /* 4:事件数据数组大小 */
} frw_event_stru;

/* ****************************************************************************
  结构名  : frw_event_sub_table_item_stru
  结构说明: 事件子表结构体
**************************************************************************** */
typedef struct {
    hi_u32 (*func)(frw_event_mem_stru *); /* (type, subtype, pipeline)类型的事件对应的处理函数 */
    hi_u32 (*p_tx_adapt_func)(frw_event_mem_stru *);
    frw_event_mem_stru *(*p_rx_adapt_func)(frw_event_mem_stru *);
} frw_event_sub_table_item_stru;

/* ****************************************************************************
  结构名  : frw_event_table_item_stru
  结构说明: 事件表结构体
**************************************************************************** */
typedef struct {
    const frw_event_sub_table_item_stru *sub_table; /* 指向子表的指针 */
} frw_event_table_item_stru;

typedef struct {
    hi_u32 event_cnt;
    hi_u32 aul_event_time[FRW_EVENT_TRACK_NUM];
    hi_u16 us_event_type[FRW_EVENT_TRACK_NUM];
    hi_u16 us_event_sub_type[FRW_EVENT_TRACK_NUM];
} frw_event_track_time_stru;

/* ****************************************************************************
  结构名  : frw_event_cfg_stru
  结构说明: 事件队列配置信息结构体
**************************************************************************** */
typedef struct {
    hi_u8                       weight;        /* 队列权重 */
    hi_u8                       max_events;    /* 队列所能容纳的最大事件个数 */
    frw_sched_policy_enum_uint8 policy;        /* 队列所属调度策略(高优先级、普通优先级) */
    hi_u8                       auc_resv;
} frw_event_cfg_stru;

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 frw_event_init(hi_void);
hi_void frw_event_exit(hi_void);
hi_u32 frw_event_queue_enqueue(frw_event_queue_stru *event_queue, frw_event_mem_stru *event_mem);
frw_event_mem_stru *frw_event_queue_dequeue(frw_event_queue_stru *event_queue);
hi_u32 frw_event_vap_flush_event(hi_u8 vap_id, frw_event_type_enum_uint8 event_type, hi_u8 drop);
hi_u32 frw_event_lookup_process_entry(frw_event_mem_stru *event_mem, const frw_event_hdr_stru *event_hrd);
frw_event_mem_stru *frw_event_alloc(hi_u16 us_payload_length);
hi_u32 frw_event_free(frw_event_mem_stru *event_mem);
hi_u32 frw_event_dispatch_event(frw_event_mem_stru *event_mem);
hi_u32 frw_event_post_event(frw_event_mem_stru *event_mem);
hi_void frw_event_table_register(frw_event_type_enum_uint8 type, frw_event_pipeline_enum_uint8 pipeline,
    const frw_event_sub_table_item_stru *sub_table);
hi_u32 frw_event_flush_event_queue(frw_event_type_enum_uint8 event_type);
hi_void frw_event_process_all_event(hi_void);
hi_u8 frw_is_vap_event_queue_empty(hi_u8 vap_id, hi_u8 event_type);
const frw_event_sub_table_item_stru *frw_get_event_sub_table(hi_u8 type, hi_u8 pipeline);
hi_u8 frw_task_thread_condition_check(hi_void);
#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
hi_void hcc_host_update_vi_flowctl_param(hi_u32 be_cwmin, hi_u32 vi_cwmin);
#endif
hi_u32 get_app_event_id(hi_void);
hi_void frw_event_sub_rx_adapt_table_init(frw_event_sub_table_item_stru *pst_sub_table, hi_u32 ul_table_nums,
    frw_event_mem_stru *(*p_rx_adapt_func)(frw_event_mem_stru *));

/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of frw_event_main.h */
