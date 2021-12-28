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

#ifndef __MAC_MIB_H__
#define __MAC_MIB_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "wlan_mib.h"
#include "oam_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* ****************************************************************************
  内联函数定义
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 设置wep MIB信息
**************************************************************************** */
static inline hi_void mac_mib_set_wep(const mac_vap_stru *mac_vap, hi_u8 key_id)
{
    mac_vap->mib_info->ast_wlan_mib_wep_dflt_key[key_id].auc_dot11_wep_default_key_value[WLAN_WEP_SIZE_OFFSET] =
        40; /* 初始化wep相关MIB信息为40 */
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11_rsna_config_group_cipher 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_rsnacfggroupcipher(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11_rsna_config_group_cipher 的值
**************************************************************************** */
static inline hi_void mac_mib_set_rsnacfggroupcipher(const mac_vap_stru *mac_vap, hi_u8 group_cipher)
{
    mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher = group_cipher;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11_wep_default_key_id 的值
**************************************************************************** */
static inline hi_void mac_mib_set_wep_default_keyid(const mac_vap_stru *mac_vap, hi_u8 default_key_id)
{
    mac_vap->mib_info->wlan_mib_privacy.dot11_wep_default_key_id = default_key_id;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11_wep_default_key_id 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_wep_default_keyid(const mac_vap_stru *mac_vap)
{
    return (mac_vap->mib_info->wlan_mib_privacy.dot11_wep_default_key_id);
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11_wep_default_key_id 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_wep_type(const mac_vap_stru *mac_vap)
{
    return (mac_vap->mib_info->wlan_mib_privacy.dot11_wep_default_key_id);
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11_rsna_config_group_cipher 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_privacyinvoked(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_privacy.dot11_privacy_invoked;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11_privacy_invoked 的值
**************************************************************************** */
static inline hi_void mac_mib_set_privacyinvoked(const mac_vap_stru *mac_vap, hi_u8 privacyinvoked)
{
    mac_vap->mib_info->wlan_mib_privacy.dot11_privacy_invoked = privacyinvoked;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11_rsna_activated 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_rsnaactivated(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11_rsna_activated 的值
**************************************************************************** */
static inline hi_void mac_mib_set_rsnaactivated(const mac_vap_stru *mac_vap, hi_u8 rsnaactivated)
{
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated = rsnaactivated;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11StationID 的值
**************************************************************************** */
static inline hi_u8 *mac_mib_get_station_id(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11OBSSScanPassiveDwell 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_obssscan_passive_dwell(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_passive_dwell;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11OBSSScanPassiveDwell 的值
**************************************************************************** */
static inline hi_void mac_mib_set_obssscan_passive_dwell(const mac_vap_stru *mac_vap, hi_u32 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_passive_dwell = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11OBSSScanActiveDwell 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_obssscan_active_dwell(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_active_dwell;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11OBSSScanActiveDwell 的值
**************************************************************************** */
static inline hi_void mac_mib_set_obssscan_active_dwell(const mac_vap_stru *mac_vap, hi_u32 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_active_dwell = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11BSSWidthTriggerScanInterval 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_bsswidth_trigger_scan_interval(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot11_bss_width_trigger_scan_interval;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11BSSWidthTriggerScanInterval 的值
**************************************************************************** */
static inline hi_void mac_mib_set_bsswidth_trigger_scan_interval(const mac_vap_stru *mac_vap, hi_u32 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_bss_width_trigger_scan_interval = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11OBSSScanPassiveTotalPerChannel 的值
 修改历史      :
  1.日    期   : 2014年2月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u32 mac_mib_get_obssscan_passive_total_per_channel(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_passive_total_per_channel;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11OBSSScanPassiveTotalPerChannel 的值
**************************************************************************** */
static inline hi_void mac_mib_set_obssscan_passive_total_per_channel(const mac_vap_stru *mac_vap, hi_u32 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_passive_total_per_channel = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11OBSSScanActiveTotalPerChannel 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_obssscan_active_total_per_channel(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_active_total_per_channel;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11OBSSScanActiveTotalPerChannel 的值
**************************************************************************** */
static inline hi_void mac_mib_set_obssscan_active_total_per_channel(const mac_vap_stru *mac_vap, hi_u32 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_active_total_per_channel = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11BSSWidthChannelTransitionDelayFactor 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_bsswidth_channel_transition_delay_factor(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot11_bss_width_channel_transition_delay_factor;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11BSSWidthChannelTransitionDelayFactor 的值
**************************************************************************** */
static inline hi_void mac_mib_set_bsswidth_channel_transition_delay_factor(const mac_vap_stru *mac_vap, hi_u32 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_bss_width_channel_transition_delay_factor = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11OBSSScanActivityThreshold 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_obssscan_activity_threshold(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_activity_threshold;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11OBSSScanActivityThreshold 的值
**************************************************************************** */
static inline hi_void mac_mib_set_obssscan_activity_threshold(const mac_vap_stru *mac_vap, hi_u32 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_activity_threshold = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11HighThroughputOptionImplemented 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_high_throughput_option_implemented(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_sta_config.dot11_high_throughput_option_implemented;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11HighThroughputOptionImplemented 的值
**************************************************************************** */
static inline hi_void mac_mib_set_high_throughput_option_implemented(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_sta_config.dot11_high_throughput_option_implemented = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11FortyMHzOperationImplemented 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_forty_mhz_operation_implemented(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->phy_ht.dot112_g_forty_m_hz_operation_implemented;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11FortyMHzOperationImplemented 的值
**************************************************************************** */
static inline hi_void mac_mib_set_forty_mhz_operation_implemented(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->phy_ht.dot112_g_forty_m_hz_operation_implemented = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11SpectrumManagementImplemented 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_spectrum_management_implemented(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_implemented;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11SpectrumManagementImplemented 的值
**************************************************************************** */
static inline hi_void mac_mib_set_spectrum_management_implemented(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_implemented = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11FortyMHzIntolerant 的值
 修改历史      :
  1.日    期   : 2014年2月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_mib_get_forty_mhz_intolerant(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot11_forty_m_hz_intolerant;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11FortyMHzIntolerant 的值
**************************************************************************** */
static inline hi_void mac_mib_set_forty_mhz_intolerant(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_forty_m_hz_intolerant = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot112040BSSCoexistenceManagementSupport 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_2040bss_coexistence_management_support(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_operation.dot112040_bss_coexistence_management_support;
}
/* ****************************************************************************
功能描述  : 获取MIB项 dot11RSNAActivated 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_dot11_rsna_activated(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11RSNAActivated 的值
**************************************************************************** */
static inline hi_void mac_mib_set_dot11_rsnaactivated(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated = val;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11RSNAMFPC 的值
**************************************************************************** */
static inline hi_void mac_mib_set_dot11_rsnamfpc(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpc = val;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11RSNAMFPR的值
**************************************************************************** */
static inline hi_void mac_mib_set_dot11_rsnamfpr(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpr = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11AssociationSAQueryMaximumTimeout 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_dot11_association_saquery_maximum_timeout(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_sta_config.dot11_association_sa_query_maximum_timeout;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 dot11AssociationSAQueryRetryTimeout 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_dot11_association_saquery_retry_timeout(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_sta_config.dot11_association_sa_query_retry_timeout;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot112040BSSCoexistenceManagementSupport 的值
**************************************************************************** */
static inline hi_void mac_mib_set_2040bss_coexistence_management_support(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_operation.dot112040_bss_coexistence_management_support = val;
}

/* ****************************************************************************
 功能描述  : 获取MIB项 ul_dot11DTIMPeriod 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_dot11dtimperiod(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_sta_config.dot11_dtim_period;
}

/* ****************************************************************************
 功能描述  : 初始化支持2040共存
**************************************************************************** */
static inline hi_void mac_mib_init_2040(const mac_vap_stru *mac_vap)
{
    mac_mib_set_forty_mhz_intolerant(mac_vap, HI_FALSE);
    mac_mib_set_spectrum_management_implemented(mac_vap, HI_TRUE);
    mac_mib_set_2040bss_coexistence_management_support(mac_vap, HI_FALSE);
}

/* ****************************************************************************
 功能描述  : 设置MIB项 ul_dot11DTIMPeriod 的值
**************************************************************************** */
static inline hi_void mac_mib_set_dot11dtimperiod(const mac_vap_stru *mac_vap, hi_u32 val)
{
    if (val != 0) {
        mac_vap->mib_info->wlan_mib_sta_config.dot11_dtim_period = val;
    }
}

/* ****************************************************************************
 功能描述  : 获取MIB项 ul_dot11DTIMPeriod 的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_powermanagementmode(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_sta_config.dot11_power_management_mode;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 ul_dot11DTIMPeriod 的值
**************************************************************************** */
static inline hi_void mac_mib_set_powermanagementmode(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_sta_config.dot11_power_management_mode = val;
}

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
/* ****************************************************************************
 功能描述  : 获取MIB项 dot11OperatingModeNotificationImplemented 的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_operating_mode_notification_implemented(mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_sta_config.dot11_operating_mode_notification_implemented;
}

/* ****************************************************************************
 功能描述  : 设置MIB项 dot11OperatingModeNotificationImplemented 的值
**************************************************************************** */
static inline hi_void mac_mib_set_operating_mode_notification_implemented(mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_sta_config.dot11_operating_mode_notification_implemented = val;
}
#endif

/* ****************************************************************************
 功能描述  : 获取LsigTxopFullProtectionActivated值
**************************************************************************** */
static inline hi_u8 mac_mib_get_lsig_txop_full_protection_activated(const mac_vap_stru *mac_vap)
{
    return (mac_vap->mib_info->wlan_mib_operation.dot11_lsigtxop_full_protection_activated);
}

/* ****************************************************************************
 功能描述  : 设置LsigTxopFullProtectionActivated值
**************************************************************************** */
static inline hi_void mac_mib_set_lsig_txop_full_protection_activated(const mac_vap_stru *mac_vap,
    hi_u8 lsig_txop_full_protection_activated)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_lsigtxop_full_protection_activated =
        lsig_txop_full_protection_activated;
}

/* ****************************************************************************
 功能描述  : 获取NonGFEntitiesPresent值
**************************************************************************** */
static inline hi_u8 mac_mib_get_non_gfentities_present(const mac_vap_stru *mac_vap)
{
    return (mac_vap->mib_info->wlan_mib_operation.dot11_non_gf_entities_present);
}

/* ****************************************************************************
 功能描述  : 设置NonGFEntitiesPresent值
**************************************************************************** */
static inline hi_void mac_mib_set_non_gfentities_present(const mac_vap_stru *mac_vap, hi_u8 non_gf_entities_present)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_non_gf_entities_present = non_gf_entities_present;
}

/* ****************************************************************************
 功能描述  : 获取RIFSMode值
**************************************************************************** */
static inline hi_u8 mac_mib_get_rifs_mode(const mac_vap_stru *mac_vap)
{
    return (mac_vap->mib_info->wlan_mib_operation.dot11_rifs_mode);
}

/* ****************************************************************************
 功能描述  : 设置RIFSMode值
**************************************************************************** */
static inline hi_void mac_mib_set_rifs_mode(const mac_vap_stru *mac_vap, hi_u8 rifs_mode)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_rifs_mode = rifs_mode;
}

/* ****************************************************************************
 功能描述  : 获取htProtection值
**************************************************************************** */
static inline wlan_mib_ht_protection_enum_uint8 mac_mib_get_ht_protection(const mac_vap_stru *mac_vap)
{
    return (mac_vap->mib_info->wlan_mib_operation.dot11_ht_protection);
}

/* ****************************************************************************
 功能描述  : 设置htProtection值
**************************************************************************** */
static inline hi_void mac_mib_set_ht_protection(const mac_vap_stru *mac_vap,
    wlan_mib_ht_protection_enum_uint8 ht_protection)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_ht_protection = ht_protection;
}

/* ****************************************************************************
 功能描述  : 获取ShortPreambleOptionImplemented值
**************************************************************************** */
static inline wlan_11b_mib_preamble_enum_uint8 mac_mib_get_short_preamble_option_implemented(
    const mac_vap_stru *mac_vap)
{
    return (mac_vap->mib_info->phy_hrdsss.dot11_short_preamble_option_implemented);
}

/* ****************************************************************************
 功能描述  : 设置ShortPreambleOptionImplemented值
**************************************************************************** */
static inline hi_void mac_mib_set_short_preamble_option_implemented(const mac_vap_stru *mac_vap,
    wlan_11b_mib_preamble_enum_uint8 preamble)
{
    mac_vap->mib_info->phy_hrdsss.dot11_short_preamble_option_implemented = preamble;
}

/* ****************************************************************************
 功能描述  : 设置en_dot11SpectrumManagementRequired值
**************************************************************************** */
static inline hi_void mac_mib_set_spectrum_management_required(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_required = val;
}

/* ****************************************************************************
 功能描述  : 获取en_dot11ShortGIOptionInFortyImplemented值
**************************************************************************** */
static inline hi_u8 mac_mib_get_shortgi_option_in_forty_implemented(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->phy_ht.dot112_g_short_gi_option_in_forty_implemented;
}

/* ****************************************************************************
 功能描述  : 设置en_dot11ShortGIOptionInFortyImplemented值
**************************************************************************** */
static inline hi_void mac_mib_set_shortgi_option_in_forty_implemented(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->phy_ht.dot112_g_short_gi_option_in_forty_implemented = val;
}

/* ****************************************************************************
 功能描述  : 设置分片门限值
**************************************************************************** */
static inline hi_void mac_mib_set_frag_threshold(const mac_vap_stru *mac_vap, hi_u32 frag_threshold)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_fragmentation_threshold = frag_threshold;
}

/* ****************************************************************************
 功能描述  : 设置RTS门限值
**************************************************************************** */
static inline hi_void mac_mib_set_rts_threshold(const mac_vap_stru *mac_vap, hi_u32 rts_threshold)
{
    mac_vap->mib_info->wlan_mib_operation.dot11_rts_threshold = rts_threshold;
}

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* ****************************************************************************
 功能描述  : 设置Mesh Privacy值
**************************************************************************** */
static inline hi_void mac_mib_set_mesh_security(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_security_activated = val;
}

/* ****************************************************************************
 功能描述  : 设置Mesh Auth Protocol值
**************************************************************************** */
static inline hi_void mac_mib_set_mesh_auth_protocol(const mac_vap_stru *mac_vap, hi_u8 auth_protocol)
{
    mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_active_authentication_protocol = auth_protocol;
}

/* ****************************************************************************
 功能描述  : 清除Mesh Auth Protocol值
**************************************************************************** */
static inline hi_void mac_mib_clear_mesh_auth_protocol(const mac_vap_stru *mac_vap)
{
    mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_active_authentication_protocol = 0;
}

/* ****************************************************************************
 功能描述  : 设置Accpeting Peer值
**************************************************************************** */
static inline hi_void mac_mib_set_accepting_peer(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_accepting_additional_peerings = val;
}

/* ****************************************************************************
 功能描述  : 获取Accpeting Peer值
**************************************************************************** */
static inline hi_u8 mac_mib_get_accepting_peer(const mac_vap_stru *mac_vap)
{
    return mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_accepting_additional_peerings;
}

/* ****************************************************************************
 功能描述  : 设置MBCA值
**************************************************************************** */
static inline hi_void mac_mib_set_mbca_en(const mac_vap_stru *mac_vap, hi_u8 val)
{
    mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mbca_activated = val;
}

/* ****************************************************************************
 功能描述  : 获取MBCA值
**************************************************************************** */
static inline hi_void mac_mib_get_mbca_en(const mac_vap_stru *mac_vap, hi_u8 *val)
{
    *val = mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mbca_activated;
}
#endif

/* ****************************************************************************
 功能描述  : 设置StationID值，即mac地址
**************************************************************************** */
static inline hi_void mac_mib_set_station_id(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    mac_cfg_staion_id_param_stru *param = HI_NULL;

    hi_unref_param(len);
    param = (mac_cfg_staion_id_param_stru *)puc_param;
    if (memcpy_s(mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN,
                 param->auc_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
        return;
    }
}

/* ****************************************************************************
 功能描述  : 设置bss type mib值
**************************************************************************** */
static inline hi_void mac_mib_set_bss_type(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hi_s32 l_value;
    hi_unref_param(len);
    l_value = *((hi_s32 *)puc_param);
    mac_vap->mib_info->wlan_mib_sta_config.dot11_desired_bss_type = (hi_u8)l_value;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 设置beacon interval的值
**************************************************************************** */
static inline hi_void mac_mib_set_beacon_period(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hi_u32 value;
    hi_unref_param(len);
    value = *((hi_u32 *)puc_param);
    mac_vap->mib_info->wlan_mib_sta_config.dot11_beacon_period = (hi_u32)value;
}

/* ****************************************************************************
 功能描述  : 设置dtim period的值
**************************************************************************** */
static inline hi_void mac_mib_set_dtim_period(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hi_s32 l_value;
    hi_unref_param(len);
    l_value = *((hi_s32 *)puc_param);
    mac_vap->mib_info->wlan_mib_sta_config.dot11_dtim_period = (hi_u32)l_value;
}

/* ****************************************************************************
 功能描述  : 设置short preamble MIB值
**************************************************************************** */
static inline hi_void mac_mib_set_shpreamble(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hi_s32 l_value;
    hi_unref_param(len);
    l_value = *((hi_s32 *)puc_param);
    if (l_value != 0) {
        mac_mib_set_short_preamble_option_implemented(mac_vap, HI_TRUE);
    } else {
        mac_mib_set_short_preamble_option_implemented(mac_vap, HI_FALSE);
    }
}
#endif

/* ****************************************************************************
 功能描述  : 获取默认密钥的大小
**************************************************************************** */
static inline hi_u8 mac_mib_get_wep_default_keysize(const mac_vap_stru *mac_vap)
{
    wlan_mib_dot11_wep_default_keys_entry_stru *pwlan_mib_wep_dflt_key =
        mac_vap->mib_info->ast_wlan_mib_wep_dflt_key;
    return (
        pwlan_mib_wep_dflt_key[mac_mib_get_wep_type(mac_vap)].auc_dot11_wep_default_key_value[WLAN_WEP_SIZE_OFFSET]);
}

/* ****************************************************************************
 功能描述  : 获取指定序列号的wep key的值
**************************************************************************** */
static inline hi_u8 mac_mib_get_wep_keysize(const mac_vap_stru *mac_vap, hi_u8 idx)
{
    return (mac_vap->mib_info->ast_wlan_mib_wep_dflt_key[idx].auc_dot11_wep_default_key_value[WLAN_WEP_SIZE_OFFSET]);
}

/* ****************************************************************************
 功能描述  : 清除 RSN认证套件
**************************************************************************** */
static inline hi_void mac_mib_clear_rsna_auth_suite(const mac_vap_stru *mac_vap)
{
    hi_u8 index;

    for (index = 0; index < WLAN_AUTHENTICATION_SUITES; index++) {
        mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[index].dot11_rsna_config_authentication_suite_activated =
            HI_FALSE;
        mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[index].dot11_rsna_config_authentication_suite_implemented =
            0xff;
    }
}

/* ****************************************************************************
 功能描述  : 设置RSN认证套件
**************************************************************************** */
static inline hi_void mac_mib_set_rsnaconfig_authentication_suite_implemented(const mac_vap_stru *mac_vap, hi_u8 inp,
    hi_u8 idx)
{
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[idx].dot11_rsna_config_authentication_suite_activated = HI_TRUE;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[idx].dot11_rsna_config_authentication_suite_implemented = inp;
}

/* ****************************************************************************
 功能描述  : 设置RSN认证套件
**************************************************************************** */
static inline hi_void mac_mib_set_rsnaclear_wpa_pairwise_cipher_implemented(const mac_vap_stru *mac_vap)
{
    hi_u8 index;
    wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru *wlan_mib_rsna_cfg_wpa_pairwise_cipher =
        mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher;

    for (index = 0; index < WLAN_PAIRWISE_CIPHER_SUITES; index++) {
        wlan_mib_rsna_cfg_wpa_pairwise_cipher[index].dot11_rsna_config_pairwise_cipher_implemented = 0xFF;
        wlan_mib_rsna_cfg_wpa_pairwise_cipher[index].dot11_rsna_config_pairwise_cipher_activated = HI_FALSE;
    }
}

/* ****************************************************************************
 功能描述  : 设置RSN WPA2认证套件
**************************************************************************** */
static inline hi_void mac_mib_set_rsnaclear_wpa2_pairwise_cipher_implemented(const mac_vap_stru *mac_vap)
{
    hi_u8 index;
    wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru *wlan_mib_rsna_cfg_wpa2_pairwise_cipher =
        mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher;

    for (index = 0; index < WLAN_PAIRWISE_CIPHER_SUITES; index++) {
        wlan_mib_rsna_cfg_wpa2_pairwise_cipher[index].dot11_rsna_config_pairwise_cipher_implemented = 0xFF;
        wlan_mib_rsna_cfg_wpa2_pairwise_cipher[index].dot11_rsna_config_pairwise_cipher_activated = HI_FALSE;
    }
}

/* ****************************************************************************
 功能描述  : 读取bss type mib值
**************************************************************************** */
static inline hi_u32 mac_mib_get_bss_type(const mac_vap_stru *mac_vap, hi_u8 *puc_len, const hi_u8 *puc_param)
{
    *((hi_s32 *)puc_param) = mac_vap->mib_info->wlan_mib_sta_config.dot11_desired_bss_type;
    *puc_len = sizeof(hi_s32);
    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 读取beacon interval的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_beacon_period(const mac_vap_stru *mac_vap, hi_u8 *puc_len, const hi_u8 *puc_param)
{
    *((hi_u32 *)puc_param) = mac_vap->mib_info->wlan_mib_sta_config.dot11_beacon_period;
    *puc_len = sizeof(hi_u32);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 读取dtim period的值
**************************************************************************** */
static inline hi_u32 mac_mib_get_dtim_period(const mac_vap_stru *mac_vap, hi_u8 *puc_len, const hi_u8 *puc_param)
{
    *((hi_u32 *)puc_param) = mac_vap->mib_info->wlan_mib_sta_config.dot11_dtim_period;
    *puc_len = sizeof(hi_u32);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取short preamble MIB值
**************************************************************************** */
static inline hi_u32 mac_mib_get_shpreamble(const mac_vap_stru *mac_vap, hi_u8 *puc_len, const hi_u8 *puc_param)
{
    hi_s32 l_value;
    l_value = mac_mib_get_short_preamble_option_implemented(mac_vap);
    *((hi_s32 *)puc_param) = l_value;
    *puc_len = sizeof(l_value);
    return HI_SUCCESS;
}
#endif

static inline hi_u8 mac_is_wep_enabled(const mac_vap_stru *mac_vap)
{
    if (!mac_mib_get_privacyinvoked(mac_vap) || mac_mib_get_rsnaactivated(mac_vap)) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

/* ****************************************************************************
 功能描述  : 获取wep type的值
**************************************************************************** */
static inline wlan_ciper_protocol_type_enum_uint8 mac_get_wep_type(const mac_vap_stru *mac_vap, hi_u8 key_id)
{
    if (mac_mib_get_wep_keysize(mac_vap, key_id) == 104) { /* size位104 */
        return WLAN_80211_CIPHER_SUITE_WEP_104;
    } else {
        return WLAN_80211_CIPHER_SUITE_WEP_40;
    }
}

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_u32 mac_mib_set_meshid(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
hi_u32 mac_mib_get_meshid(const mac_vap_stru *mac_vap, hi_u8 *puc_len, const hi_u8 *puc_param);
hi_void mac_mib_get_wpa_pairwise_cipher_suite(const mac_vap_stru *mac_vap, hi_u8 *puc_num);
hi_void mac_mib_get_wpa2_pairwise_cipher_suite(const mac_vap_stru *mac_vap, hi_u8 *puc_num);
hi_void mac_mib_get_authentication_suite(const mac_vap_stru *mac_vap, hi_u8 *puc_num);
hi_void mac_mib_get_wpa2_pairwise_cipher_suite_value(const mac_vap_stru *mac_vap, hi_u8 *puc_pairwise_value,
    hi_u8 pairwise_len);
hi_void mac_mib_get_wpa_pairwise_cipher_suite_value(const mac_vap_stru *mac_vap, hi_u8 *puc_pairwise_value,
    hi_u8 pairwise_len);
hi_void mac_mib_set_rsna_auth_suite(const mac_vap_stru *mac_vap, hi_u8 auth_value);
hi_u32 mac_mib_set_ssid(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
hi_void mac_mib_set_rsnaconfig_wpa_pairwise_cipher_implemented(const mac_vap_stru *mac_vap, hi_u8 pairwise_value);
hi_void mac_mib_set_rsnaconfig_wpa2_pairwise_cipher_implemented(const mac_vap_stru *mac_vap, hi_u8 pairwise_value);
hi_u32 mac_mib_get_ssid(const mac_vap_stru *mac_vap, hi_u8 *puc_len, const hi_u8 *puc_param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __MAC_MIB_H__ */
