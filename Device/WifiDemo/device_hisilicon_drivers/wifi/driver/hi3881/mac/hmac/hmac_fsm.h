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

#ifndef __HMAC_FSM_H__
#define __HMAC_FSM_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hmac_main.h"
#include "hmac_vap.h"
#include "hmac_rx_filter.h"
#include "hmac_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
typedef hi_u32 (*hmac_fsm_func)(hmac_vap_stru *hmac_vap, hi_void *p_param);

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* 状态机输入类型枚举 */
typedef enum {
    /* AP STA共有的输入类型 */
    HMAC_FSM_INPUT_RX_MGMT,
    HMAC_FSM_INPUT_RX_DATA,
    HMAC_FSM_INPUT_TX_DATA,
    HMAC_FSM_INPUT_TIMER0_OUT,
    HMAC_FSM_INPUT_MISC, /* TBTT 异常 等其他输入类型 */

    /* AP 独有的输入类型 */
    HMAC_FSM_INPUT_START_REQ,

    /* STA独有的输入类型 */
    HMAC_FSM_INPUT_SCAN_REQ,
    HMAC_FSM_INPUT_JOIN_REQ,
    HMAC_FSM_INPUT_AUTH_REQ,
    HMAC_FSM_INPUT_ASOC_REQ,
    HMAC_FSM_INPUT_LISTEN_REQ, /*  监听请求 */
    HMAC_FSM_INPUT_LISTEN_TIMEOUT,
    HMAC_FSM_INPUT_SCHED_SCAN_REQ, /* PNO调度扫描请求 */
    HMAC_FSM_INPUT_TYPE_BUTT
} hmac_fsm_input_type_enum;
typedef hi_u8 hmac_fsm_input_type_enum_uint8;

#define HMAC_FSM_AP_INPUT_TYPE_BUTT     (HMAC_FSM_INPUT_RX_MGMT + 1)
#define HMAC_FSM_STA_INPUT_TYPE_BUTT    HMAC_FSM_INPUT_TYPE_BUTT
#define HMAC_SWITCH_STA_PSM_PERIOD      120000  /* staut打开低功耗超时的时间参考1101 120s */

/* MISC输入类型的子类型定义 */
typedef enum {
    HMAC_MISC_TBTT,
    HMAC_MISC_ERROR,
    HMAC_MISC_RADAR,

    HMAC_MISC_BUTT
} hmac_misc_input_enum;
typedef hi_u8 hmac_misc_input_enum_uint8;

/* ****************************************************************************
  4 STRUCT定义
**************************************************************************** */
/* MISC输入类型，入参结构体定义 */
typedef struct {
    hmac_misc_input_enum_uint8 type;
    hi_u8 auc_resv[3]; /* 3 保留字节 */
    hi_void *data;
} hmac_misc_input_stru;

/* ****************************************************************************
  5 函数声明
**************************************************************************** */
hi_u32 hmac_fsm_handle_scan_req(const mac_vap_stru *mac_vap, mac_scan_req_stru *scan_params);
hi_void hmac_fsm_change_state(hmac_vap_stru *hmac_vap, mac_vap_state_enum_uint8 vap_state);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_FSM_H__ */
