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

#ifndef __HMAC_MGMT_BSS_COMM_H__
#define __HMAC_MGMT_BSS_COMM_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "mac_frame.h"
#include "dmac_ext_if.h"
#include "hmac_vap.h"
#include "hmac_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OAL_IE_HDR_LEN 2

typedef struct {
    hi_u8 tidno;
    hi_u8 resvd[3]; /* 3 BYTE保留字段 */
    hi_u16 *frame_len;
} hmac_addba_req_info;

/* ****************************************************************************
  5 函数声明
**************************************************************************** */
hi_u16 hmac_mgmt_encap_addba_req(hmac_vap_stru *hmac_vap, hi_u8 *puc_data, dmac_ba_tx_stru *tx_ba, hi_u8 tid);
hi_u16 hmac_mgmt_encap_addba_rsp(hmac_vap_stru *hmac_vap, hi_u8 *puc_data, hmac_ba_rx_stru *addba_rsp, hi_u8 tid,
    hi_u8 status);
hi_u16 hmac_mgmt_encap_delba(hmac_vap_stru *hmac_vap, hi_u8 *puc_data, const mac_action_mgmt_args_stru *action_args);
hi_u32 hmac_mgmt_rx_addba_req(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, const hi_u8 *puc_payload);
hi_u32 hmac_mgmt_rx_addba_rsp(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, const hi_u8 *puc_payload);
hi_u32 hmac_mgmt_rx_delba(const hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, const hi_u8 *puc_payload);
hi_u32 hmac_mgmt_tx_addba_req(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user,
    const mac_action_mgmt_args_stru *action_args);
hi_u32 hmac_mgmt_tx_addba_rsp(hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user, hmac_ba_rx_stru *ba_rx_info,
    hi_u8 tid, hi_u8 status);
hi_u32 hmac_mgmt_tx_delba(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user,
    const mac_action_mgmt_args_stru *action_args);
hi_u32 hmac_mgmt_tx_addba_timeout(hi_void *arg);
hi_u32 hmac_mgmt_tx_ampdu_start(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, mac_priv_req_args_stru *priv_req);
hi_u32 hmac_mgmt_tx_ampdu_end(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, mac_priv_req_args_stru *priv_req);

#ifdef _PRE_WLAN_FEATURE_PMF
hi_u32 hmac_sa_query_interval_timeout(hi_void *arg);
hi_void hmac_send_sa_query_rsp(mac_vap_stru *mac_vap, hi_u8 *hdr, hi_u8 is_protected);
hi_u32 hmac_start_sa_query(mac_vap_stru *mac_vap, hmac_user_stru *hmac_user, hi_u8 is_protected);
hi_u32 hmac_pmf_check_err_code(const mac_user_stru *user_base_info, hi_u8 is_protected, const hi_u8 *puc_mac_hdr);
#endif
hi_u32 hmac_tx_mgmt_send_event(const mac_vap_stru *mac_vap, oal_netbuf_stru *mgmt_frame, hi_u32 us_frame_len);
hi_void hmac_mgmt_update_assoc_user_qos(hi_u8 *puc_payload, hi_u16 us_msg_len, hi_u16 us_info_elem_offset,
    const hmac_user_stru *hmac_user);
hi_void hmac_set_user_protocol_mode(const mac_vap_stru *mac_vap, hmac_user_stru *hmac_user);
hi_u32 hmac_mgmt_reset_psm(const mac_vap_stru *mac_vap, hi_u8 user_id);
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
hi_u32 hmac_ie_proc_opmode_field(mac_vap_stru *mac_vap, mac_user_stru *mac_user, mac_opmode_notify_stru *opmode_notify,
    hi_u8 mgmt_frm_type);
#endif
hi_u32 hmac_keepalive_set_interval(mac_vap_stru *mac_vap, hi_u16 us_keepalive_interval);
hi_u32 hmac_keepalive_set_limit(mac_vap_stru *mac_vap, hi_u32 us_keepalive_limit);

#ifdef _PRE_WLAN_FEATURE_PMF
hi_void hmac_rx_sa_query_req(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf, hi_u8 is_protected);
hi_void hmac_rx_sa_query_rsp(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf, hi_u8 is_protected);
#endif
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
hi_u32 hmac_mgmt_rx_opmode_notify_frame(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf);
#endif
hi_void hmac_send_mgmt_to_host(const hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf, hi_u16 us_len, hi_s32 l_freq);
hi_void hmac_rx_mgmt_send_to_host(const hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf);
#ifdef _PRE_WLAN_FEATURE_ANY
hi_u32 hmac_any_tx_event_status(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
hi_u32 hmac_any_get_peer_info(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_mgmt_tx_event_status(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
hi_void hmac_rx_ba_session_decr(hmac_vap_stru *hmac_vap);
hi_void hmac_tx_ba_session_decr(hmac_vap_stru *hmac_vap);
#else
hi_void hmac_rx_ba_session_decr(hmac_device_stru *hmac_dev);
hi_void hmac_tx_ba_session_decr(hmac_device_stru *hmac_dev);
#endif
hi_void hmac_mgmt_rx_action_ba(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, const hi_u8 *puc_data);
hi_void hmac_vap_set_user_avail_rates(mac_vap_stru *mac_vap, hmac_user_stru *hmac_user);
hi_u32 hmac_proc_ht_cap_ie(mac_vap_stru *mac_vap, mac_user_stru *mac_user, const hi_u8 *puc_ht_cap_ie);
hi_void hmac_mgmt_send_deauth_frame(mac_vap_stru *mac_vap, const hi_u8 *da_mac_addr, hi_u8 addr_len, hi_u16 err_code);
hi_void hmac_mgmt_send_disassoc_frame(mac_vap_stru *mac_vap, const hi_u8 *da_mac_addr, hi_u16 err_code,
    hi_u8 is_protected);
hi_u32 hmac_wpas_mgmt_tx(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u8 hmac_get_auc_avail_protocol_mode(wlan_protocol_enum_uint8 vap_protocol, wlan_protocol_enum_uint8 user_protocol);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
hi_u8 hmac_get_g_ul_print_wakeup_mgmt(hi_void);
hi_void hmac_set_g_ul_print_wakeup_mgmt(hi_u8 print_wakeup_mgmt);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_MGMT_BSS_COMM_H__ */
