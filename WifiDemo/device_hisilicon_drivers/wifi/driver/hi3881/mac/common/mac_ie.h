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

#ifndef __MAC_IE_H__
#define __MAC_IE_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "mac_frame.h"
#include "wlan_mib.h"
#include "wlan_types.h"
#include "mac_user.h"
#include "mac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
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
/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 inline函数定义
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
/* ****************************************************************************
 功能描述  : 判断帧类型是否为(重)关联请求/响应
 输入参数  : uc_mgmt_frm_type: 帧类型
 返 回 值  : 是HI_TRUE/否HI_FALSE
 修改历史      :
  1.日    期   : 2014年7月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u32 mac_check_is_assoc_frame(hi_u8 mgmt_frm_type)
{
    if ((mgmt_frm_type == WLAN_FC0_SUBTYPE_ASSOC_RSP) || (mgmt_frm_type == WLAN_FC0_SUBTYPE_REASSOC_REQ) ||
        (mgmt_frm_type == WLAN_FC0_SUBTYPE_REASSOC_RSP) || (mgmt_frm_type == WLAN_FC0_SUBTYPE_ASSOC_REQ)) {
        return HI_TRUE;
    }
    return HI_FALSE;
}
#endif

/* ****************************************************************************
  11 函数声明
**************************************************************************** */
mac_sec_ch_off_enum_uint8 mac_get_sco_from_bandwidth(wlan_channel_bandwidth_enum_uint8 bandwidth);

wlan_channel_bandwidth_enum_uint8 mac_get_bandwith_from_center_freq_seg0(hi_u8 channel, hi_u8 chan_center_freq);

hi_void mac_ie_get_vht_rx_mcs_map(mac_rx_max_mcs_map_stru *mac_rx_mcs_sta, mac_rx_max_mcs_map_stru *mac_rx_mcs_ap);

wlan_mib_mimo_power_save_enum_uint8 mac_ie_proc_sm_power_save_field(hi_u8 smps);
hi_u8 mac_ie_proc_ht_green_field(const mac_user_stru *mac_user_sta, mac_vap_stru *mac_vap, hi_u8 ht_green_field,
    hi_bool prev_asoc_ht);
hi_u8 mac_ie_proc_ht_supported_channel_width(const mac_user_stru *mac_user_sta, mac_vap_stru *mac_vap,
    hi_u8 supported_channel_width, hi_bool prev_asoc_ht);
hi_u8 mac_ie_proc_lsig_txop_protection_support(const mac_user_stru *mac_user_sta, mac_vap_stru *mac_vap,
    hi_u8 lsig_txop_protection_support, hi_bool prev_asoc_ht);
hi_u8 mac_ie_get_chan_num(hi_u8 *puc_frame_body, hi_u16 us_frame_len, hi_u16 us_offset, hi_u8 curr_chan);
hi_u32 mac_set_second_channel_offset_ie(wlan_channel_bandwidth_enum_uint8 bw, hi_u8 *pauc_buffer,
    hi_u8 *puc_output_len);
hi_u8 mac_ie_check_p2p_action(const hi_u8 *puc_payload);
hi_u32 mac_config_set_mib(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_void mac_ie_proc_ht_sta(const mac_vap_stru *mac_vap, const hi_u8 *puc_payload, const hi_u16 *pus_index,
    mac_user_stru *mac_user_ap, hi_u16 *pus_ht_cap_info, hi_u16 *pus_amsdu_max);
hi_u32 mac_proc_ht_opern_ie(mac_vap_stru *mac_vap, const hi_u8 *puc_payload, mac_user_stru *mac_user);
hi_void mac_ie_txbf_set_ht_hdl(mac_user_ht_hdl_stru *ht_hdl, hi_u32 info_elem);
wlan_channel_bandwidth_enum_uint8 mac_get_bandwidth_from_sco(mac_sec_ch_off_enum_uint8 sec_chan_offset);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __MAC_IE_H__ */
