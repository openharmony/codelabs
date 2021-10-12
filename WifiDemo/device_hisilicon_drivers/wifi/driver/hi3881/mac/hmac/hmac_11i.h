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

#ifndef __HMAC_11I_H__
#define __HMAC_11I_H__

/* ****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "mac_resource.h"
#include "hmac_vap.h"
#include "hmac_user.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define DMAC_WPA_802_11I  BIT0 /* 安全加密: bss_info中记录AP能力标识，WPA or WPA2 */
#define DMAC_RSNA_802_11I BIT1

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* 芯片密钥类型定义 */
typedef enum {
    HAL_KEY_TYPE_TX_GTK              = 0,       /* Hi1131:HAL_KEY_TYPE_TX_IGTK */
    HAL_KEY_TYPE_PTK                 = 1,
    HAL_KEY_TYPE_RX_GTK              = 2,
    HAL_KEY_TYPE_RX_GTK2             = 3,       /* 02使用 */
    HAL_KEY_TYPE_BUTT
} hal_cipher_key_type_enum;
typedef hi_u8 hal_cipher_key_type_enum_uint8;

typedef struct hmac_cap_supplicant_info_stru {
    hi_u8 *puc_grp_policy_match;
    hi_u8 *puc_pcip_policy_match;
    hi_u8 *puc_auth_policy_match;
} hmac_cap_supplicant_info_stru;

/* ****************************************************************************
  4 函数声明
**************************************************************************** */
hi_u32 hmac_init_security(mac_vap_stru *mac_vap, hi_u8 *mac_addr, hi_u16 addr_len);

hi_u8 hmac_check_pcip_wpa_policy(const wlan_mib_ieee802dot11_stru *mib_info, hi_u8 policy);
hi_u8 hmac_check_pcip_wpa2_policy(const wlan_mib_ieee802dot11_stru *mib_info, hi_u8 policy);
hi_u32 hmac_check_pcip_policy(const wlan_mib_ieee802dot11_stru *mib_info, hi_u8 policy, hi_u8 is_80211i_mode);
hi_u32 hmac_get_security_oui(hi_u8 is_80211i_mode, hi_u8 *auc_oui);
hi_u8 hmac_get_pcip_policy_auth(const hi_u8 *puc_frame, hi_u8 *puc_len);
hi_u8 hmac_get_auth_policy_auth(const hi_u8 *puc_frame, hi_u8 *len);
hi_u32 hmac_check_join_req_parewise_cipher_supplicant(const wlan_mib_ieee802dot11_stru *mib_info,
    hi_u8 *puc_pcip_policy_match, hi_u8 is_802_11i_mode, const hi_u8 *puc_pcip_policy);
hi_u8 hmac_check_bss_cap_info(hi_u16 us_cap_info, mac_vap_stru *mac_vap);
hi_u32 hmac_update_current_join_req_parms_11i(mac_vap_stru *mac_vap, const mac_bss_80211i_info_stru *is_11i);
hi_void hmac_update_pcip_policy_prot_supplicant(mac_vap_stru *mac_vap, hi_u8 pcip_policy_match);
hi_u32 hmac_check_rsn_capability(const mac_vap_stru *mac_vap, const hi_u8 *puc_rsn_ie,
    mac_status_code_enum_uint16 *pen_status_code);
hi_u32 hmac_en_mic(const hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf, hi_u8 *puc_iv_len);
hi_u32 hmac_de_mic(const hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf);
hi_u32 hmac_rx_tkip_mic_failure_process(frw_event_mem_stru *event_mem);
hi_u32 hmac_11i_ether_type_filter(const hmac_vap_stru *hmac_vap, const hi_u8 *mac_addr, hi_u16 us_ether_type);
hi_u32 hmac_config_11i_add_key(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_config_11i_get_key(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_11i_remove_key(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_11i_set_default_key(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_11i_add_wep_entry(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_check_capability_mac_phy_supplicant(mac_vap_stru *mac_vap, mac_bss_dscr_stru *bss_dscr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
