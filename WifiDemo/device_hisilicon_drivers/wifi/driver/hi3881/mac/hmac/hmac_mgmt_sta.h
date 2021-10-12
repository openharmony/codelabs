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

#ifndef __HMAC_MGMT_STA_H__
#define __HMAC_MGMT_STA_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "hmac_mgmt_bss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define HMAC_WMM_QOS_PARAMS_HDR_LEN        8
#define HMAC_WMM_QOSINFO_AND_RESV_LEN      2
#define HMAC_WMM_AC_PARAMS_RECORD_LEN      4

#define DMAC_MAX_SW_RETRIES         5   /* 数据报文重传次数 */
#define DMAC_MGMT_MAX_SW_RETRIES    3   /* 管理报文重传次数 */

/* ****************************************************************************
  3 STRUCT定义
**************************************************************************** */
/* 加入请求参数 */
typedef struct {
    mac_bss_dscr_stru bss_dscr; /* 要加入的bss网络 */
    hi_u16 us_join_timeout;     /* 加入超时 */
    hi_u16 us_probe_delay;
} hmac_join_req_stru;

/* 认证结果 */
typedef struct {
    hi_u8  peer_sta_addr[WLAN_MAC_ADDR_LEN];
    hi_u16 us_status_code;  /* 认证结果 */
} hmac_auth_rsp_stru;

typedef struct hmac_edca_params_info_stru {
    hi_u8 *puc_payload;

    hi_u16 us_msg_len;
    hi_u16 us_info_elem_offset;
} hmac_edca_params_info_stru;

typedef struct hmac_check_ht_sta_info_stru {
    hi_u8 *puc_payload;
    hi_u16 us_offset;
    hi_u16 us_rx_len;
} hmac_check_ht_sta_info_stru;

/* ****************************************************************************
  4 UNION定义
**************************************************************************** */
/* ****************************************************************************
  5 函数声明
**************************************************************************** */
hi_u32 hmac_sta_wait_join(hmac_vap_stru *hmac_vap, hmac_join_req_stru *join_req);
hi_u32 hmac_sta_wait_auth(hmac_vap_stru *hmac_vap, hi_u16 auth_timeout);
hi_u32 hmac_sta_wait_asoc(hmac_vap_stru *hmac_vap, hi_u16 us_assoc_timeout);
hi_u32 hmac_sta_wait_asoc_rx(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *crx_event);
hi_u32 hmac_sta_wait_auth_seq2_rx(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *crx_event);
hi_u32 hmac_sta_wait_auth_seq4_rx(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *crx_event);
hi_u32 hmac_sta_auth_timeout(hmac_vap_stru *hmac_vap);
hi_u32 hmac_sta_up_rx_mgmt(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *crx_event);
hi_u32 hmac_sta_wait_asoc_timeout(hmac_vap_stru *hmac_vap);
hi_void hmac_sta_disassoc_rsp(const hmac_vap_stru *hmac_vap, hi_u16 us_disasoc_reason_code,
                              hi_u16 us_dmac_reason_code);
wlan_channel_bandwidth_enum_uint8 hmac_sta_get_band(wlan_bw_cap_enum_uint8 dev_cap,
    wlan_channel_bandwidth_enum_uint8 bss_cap);
hi_u32 hmac_sta_get_user_protocol(mac_bss_dscr_stru *bss_dscr, wlan_protocol_enum_uint8 *protocol_mode);

hi_void hmac_sta_update_wmm_info(hmac_vap_stru *hmac_vap, mac_user_stru *mac_user, hi_u8 *puc_wmm_ie);
hi_u32 hmac_sta_up_update_edca_params_machw(const hmac_vap_stru *hmac_vap, mac_wmm_set_param_type_enum_uint8 type);
hi_void hmac_send_2040_coext_mgmt_frame_sta(mac_vap_stru *mac_vap);
hi_u32 hmac_sta_set_txopps_partial_aid(mac_vap_stru *mac_vap);
hi_void hmac_sta_up_update_edca_params(const hmac_edca_params_info_stru *edca_params_info,
    const hmac_vap_stru *hmac_vap, hi_u8 frame_sub_type, const hmac_user_stru *hmac_user);
hi_u32 hmac_ie_proc_obss_scan_ie(const mac_vap_stru *mac_vap, const hi_u8 *puc_payload);
hi_u8 hmac_is_support_11grate(const hi_u8 *puc_rates, hi_u8 rate_num);
hi_u8 hmac_is_support_11brate(const hi_u8 *puc_rates, hi_u8 rate_num);
hi_u32 hmac_set_retry_time_en(const mac_vap_stru *mac_vap, hi_u8 retry_time, hi_u8 retry_frame_type);
hi_u32 hmac_set_retry_time_close(const mac_vap_stru *mac_vap);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_MGMT_STA_H__ */
