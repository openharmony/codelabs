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

#ifndef __HMAC_MGMT_AP_H__
#define __HMAC_MGMT_AP_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "mac_frame.h"
#include "hmac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define is_wep_cipher(uc_cipher) \
    ((WLAN_80211_CIPHER_SUITE_WEP_40 == (uc_cipher)) || (WLAN_80211_CIPHER_SUITE_WEP_104 == (uc_cipher)))

typedef struct hmac_sup_rates_info_stru {
    hi_u32 msg_len;

    hi_u16 us_offset;
    hi_u16 us_rsvd;

    hi_u16 *pus_msg_idx;
} hmac_sup_rates_info_stru;

typedef struct hmac_update_sta_user_info_stru {
    hi_u8 *puc_mac_hdr;
    hi_u8 *puc_payload;
    hi_u32 msg_len;
} hmac_update_sta_user_info_stru;

typedef struct hmac_sta_check_info {
    hi_u16 cap;
    hi_u16 msg_idx;
    hi_u16 offset;
    hi_u8 rsvd[2]; /* 2 byte保留字段 */
} hmac_sta_check_info;

typedef struct hmac_ap_asoc_info_stru {
    hi_u8 *sta_mac_addr;
    hmac_user_stru *hmac_user;
    mac_status_code_enum_uint16 *status_code;
} hmac_ap_asoc_info_stru;

typedef struct hmac_ap_asoc_req_stru {
    const hmac_vap_stru *hmac_vap;
    hi_u8 mgmt_frm_type;
    hi_u8 *puc_mac_hdr;
    hi_u8 *puc_payload;
    hi_u32 payload_len;
} hmac_ap_asoc_req_stru;

/* ****************************************************************************
  3 函数声明
**************************************************************************** */
hi_u32 hmac_ap_up_rx_mgmt(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *mgmt_rx_event);
hi_u32 hmac_ap_wait_start_rx_mgmt(hmac_vap_stru *hmac_vap, hi_void *param);
hi_u32 hmac_mgmt_timeout_ap(hi_void *param);
hi_void hmac_handle_disconnect_rsp_ap(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 hmac_mesh_update_sta_sup_rates(hi_u8 *puc_sup_rates, hi_u8 sup_rates_len, hmac_user_stru *hmac_user,
    mac_status_code_enum_uint16 *pen_status_code);
hi_u8 hmac_mesh_check_pcip_policy(hi_u8 policy);
hi_u8 hmac_mesh_check_auth_policy(hi_u8 policy);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_MGMT_AP_H__ */
