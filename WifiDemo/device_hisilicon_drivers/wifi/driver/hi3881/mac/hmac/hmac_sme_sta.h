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

#ifndef __HMAC_SME_STA_H__
#define __HMAC_SME_STA_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hi_types.h"
#include "oal_err_wifi.h"
#include "hmac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define MAX_AUTH_CNT 3
#define MAX_ASOC_CNT 5
#ifdef _PRE_WLAN_FEATURE_PMF
#define MAX_ASOC_REJECT_CNT 10
#endif
#define WLAN_11B_SUPPORT_RATE_NUM 4
#define WLAN_11B_SUPPORT_RATE_1M 0x82
#define WLAN_11B_SUPPORT_RATE_2M 0x84
#define WLAN_11B_SUPPORT_RATE_5M 0x8b
#define WLAN_11B_SUPPORT_RATE_11M 0x96
#define WLAN_BGSCAN_CHANNEL_INTERVAL 6
typedef hi_void (*hmac_sme_handle_rsp_func)(hmac_vap_stru *hmac_vap, const hi_u8 *puc_msg);

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* 上报给SME结果 类型定义 */
typedef enum {
    HMAC_SME_SCAN_RSP,
    HMAC_SME_JOIN_RSP,
    HMAC_SME_AUTH_RSP,
    HMAC_SME_ASOC_RSP,

    HMAC_SME_RSP_BUTT
} hmac_sme_rsp_enum;
typedef hi_u8 hmac_sme_rsp_enum_uint8;

/* ****************************************************************************
  4 函数声明
**************************************************************************** */
hi_void hmac_send_rsp_to_sme_sta(hmac_vap_stru *hmac_vap, hmac_sme_rsp_enum_uint8 type, const hi_u8 *puc_msg);
hi_void hmac_handle_scan_rsp_sta(hmac_vap_stru *hmac_vap, const hi_u8 *puc_msg);
hi_void hmac_handle_join_rsp_sta(hmac_vap_stru *hmac_vap, const hi_u8 *puc_msg);
hi_void hmac_handle_auth_rsp_sta(hmac_vap_stru *hmac_vap, const hi_u8 *puc_msg);
hi_void hmac_handle_asoc_rsp_sta(hmac_vap_stru *hmac_vap, const hi_u8 *puc_msg);
#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_sta_initiate_scan(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_start_sched_scan(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_stop_sched_scan(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_process_scan_req(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_sta_initiate_join(mac_vap_stru *mac_vap, mac_bss_dscr_stru *bss_dscr);
hi_void hmac_report_assoc_state_sta(const hmac_vap_stru *hmac_vap, const hi_u8 *mac_addr, hi_u8 assoc);
hi_void hmac_handle_conn_fail(const mac_vap_stru *mac_vap);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_SME_STA_H__ */
