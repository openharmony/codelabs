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

/* ****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "oam_ext_if.h"
#include "wlan_mib.h"
#include "frw_timer.h"
#include "wal_main.h"
#include "mac_device.h"
#include "mac_vap.h"
#include "mac_ie.h"
#include "mac_regdomain.h"
#include "mac_frame.h"
#include "dmac_ext_if.h"
#include "hmac_device.h"
#include "hmac_ext_if.h"
#include "hmac_fsm.h"
#include "hmac_vap.h"
#include "hmac_rx_data.h"
#include "hmac_config.h"
#include "hmac_chan_mgmt.h"
#include "hmac_mgmt_bss_comm.h"
#include "hmac_mgmt_sta.h"
#include "hmac_scan.h"
#include "hmac_mgmt_ap.h"
#include "hmac_11i.h"
#include "hmac_encap_frame_ap.h"
#include "hmac_sme_sta.h"
#include "hmac_config.h"
#ifdef _PRE_WLAN_FEATURE_WOW
#include "hmac_wow.h"
#endif
#include "wal_event_msg.h"
#include "wal_customize.h"
#ifdef _PRE_WLAN_FEATURE_P2P
#include "hmac_p2p.h"
#endif
#include "hmac_tx_data.h"
#ifdef _PRE_WLAN_FEATURE_ANY
#include "hmac_any.h"
#endif
#include "hmac_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
/* WLAN_DTX 事件子类型表 */
frw_event_sub_table_item_stru g_ast_hmac_wlan_dtx_event_sub_table[DMAC_TX_WLAN_DTX_BUTT];
/* HMAC模块 WLAN_DRX事件处理函数注册结构定义 */
frw_event_sub_table_item_stru g_ast_hmac_wlan_drx_event_sub_table[DMAC_WLAN_DRX_EVENT_SUB_TYPE_BUTT];
/* HMAC模块 WLAN_CRX事件处理函数注册结构定义 */
frw_event_sub_table_item_stru g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_SUB_TYPE_BUTT];
/* HMAC模块 MISC杂散事件处理函数注册结构定义 */
frw_event_sub_table_item_stru g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_BUTT];
/* HMAC模块 发向HOST侧的配置事件处理函数注册结构定义 */
frw_event_sub_table_item_stru g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_SYN_BUTT];
/* HOST CRX子表 */
frw_event_sub_table_item_stru g_ast_wal_host_crx_table[WAL_HOST_CRX_SUBTYPE_BUTT];
#endif

/* ================ cfgid操作全局变量 =============================================== */
static const wal_wid_op_stru g_ast_board_wid_op[] = {
/* cfgid                   是否复位mac  保留一字节   get函数              set函数 */
#if defined(_PRE_WLAN_FEATURE_SIGMA) || defined(_PRE_DEBUG_MODE)
    {WLAN_CFGID_ADDBA_REQ,         HI_FALSE,  {0},   HI_NULL,              hmac_config_addba_req},
#endif
    {WLAN_CFGID_SET_DSCR,          HI_FALSE,  {0},   HI_NULL,              hmac_config_set_dscr_param},
    {WLAN_CFGID_VAP_INFO,          HI_FALSE,  {0},   HI_NULL,              hmac_config_vap_info},
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_AMPDU_START,       HI_FALSE,  {0},   HI_NULL,              hmac_config_ampdu_start},
    {WLAN_CFGID_AMSDU_AMPDU_SWITCH, HI_FALSE,  {0},  HI_NULL,              hmac_config_amsdu_ampdu_switch},
    {WLAN_CFGID_DELBA_REQ,         HI_FALSE,  {0},   HI_NULL,              hmac_config_delba_req},
#endif
    {WLAN_CFGID_USER_INFO,         HI_FALSE,  {0},   HI_NULL,              hmac_config_user_info},

#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_BSS_TYPE,          HI_TRUE,   {0},   hmac_config_get_bss_type,   hmac_config_set_bss_type},
#endif
    {WLAN_CFGID_ADD_VAP,           HI_FALSE,  {0},   HI_NULL,              hmac_config_add_vap},
    {WLAN_CFGID_START_VAP,         HI_FALSE,  {0},   HI_NULL,              hmac_config_start_vap},
    {WLAN_CFGID_DEL_VAP,           HI_FALSE,  {0},   HI_NULL,              hmac_config_del_vap},
    {WLAN_CFGID_DOWN_VAP,          HI_FALSE,  {0},   HI_NULL,              hmac_config_down_vap},
    {WLAN_CFGID_MODE,              HI_FALSE,  {0},   hmac_config_get_mode,       hmac_config_set_mode},
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_BANDWIDTH,         HI_FALSE,  {0},   HI_NULL,              hmac_config_set_mode},
#endif
    {WLAN_CFGID_CURRENT_CHANEL,    HI_FALSE,  {0},   hmac_config_get_freq,       hmac_config_set_freq},
    {WLAN_CFGID_STATION_ID,        HI_TRUE,   {0},   HI_NULL,              hmac_config_set_mac_addr},
    {WLAN_CFGID_SSID,              HI_FALSE,  {0},   hmac_config_get_ssid,       hmac_config_set_ssid},
    {WLAN_CFGID_SHORTGI,           HI_FALSE,  {0},   hmac_config_get_shortgi20,  hmac_config_set_shortgi20},
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_SHORT_PREAMBLE,    HI_FALSE,  {0},   hmac_config_get_shpreamble, hmac_config_set_shpreamble},
    {WLAN_CFGID_PROT_MODE,         HI_FALSE,  {0},   hmac_config_get_prot_mode,  hmac_config_set_prot_mode},
    {WLAN_CFGID_AUTH_MODE,         HI_FALSE,  {0},   hmac_config_get_auth_mode,  hmac_config_set_auth_mode},
    {WLAN_CFGID_BEACON_INTERVAL,   HI_FALSE,  {0},   hmac_config_get_bintval,    hmac_config_set_bintval},
#endif
    {WLAN_CFGID_TX_POWER,          HI_FALSE,  {0},   hmac_config_get_txpower,    hmac_config_set_txpower},

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_DTIM_PERIOD,       HI_FALSE,  {0},   hmac_config_get_dtimperiod, hmac_config_set_dtimperiod},
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_EDCA_TABLE_CWMIN,          HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_CWMAX,          HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_AIFSN,          HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_TXOP_LIMIT,     HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_MSDU_LIFETIME,  HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_MANDATORY,      HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_CWMIN,         HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_CWMAX,         HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_AIFSN,         HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_TXOP_LIMIT,    HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_MSDU_LIFETIME, HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_MANDATORY,     HI_FALSE,  {0},   hmac_config_get_wmm_params,    hmac_config_set_wmm_params},
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    {WLAN_CFGID_BTCOEX_ENABLE,          HI_FALSE,  {0},    HI_NULL,            hmac_config_set_btcoex_en},
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_REPORT_VAP_INFO,        HI_FALSE,  {0},    HI_NULL,            hmac_config_report_vap_info},
#endif
#endif
    {WLAN_CFGID_CLEAN_SCAN_RESULT,      HI_FALSE,  {0},   HI_NULL,             hmac_scan_clean_result},
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
    {WLAN_CFGID_SET_REKEY,              HI_FALSE,  {0},   HI_NULL,             hmac_config_set_rekey_info},
#endif
#ifdef _PRE_WLAN_RF_110X_CALI_DPD
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_START_DPD,              HI_FALSE,  {0},    HI_NULL,            hmac_config_start_dpd},
#endif
#endif
    {WLAN_CFGID_SET_PM_SWITCH,          HI_FALSE,  {0},    HI_NULL,            hmac_config_set_pm_switch},
    {WLAN_CFGID_KICK_USER,              HI_FALSE,  {0},    HI_NULL,            hmac_config_kick_user},

    {WLAN_CFGID_COUNTRY,                HI_FALSE,  {0},    hmac_config_get_country,  hmac_config_set_country},
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_TID,                    HI_FALSE,  {0},    hmac_config_get_tid,      HI_NULL},
#endif
    {WLAN_CFGID_RX_FCS_INFO,            HI_FALSE,  {0},    HI_NULL,            hmac_config_rx_fcs_info},
    {WLAN_CFGID_SET_RATE,               HI_FALSE,  {0},    HI_NULL,            hmac_config_set_rate},
    {WLAN_CFGID_SET_MCS,                HI_FALSE,  {0},    HI_NULL,            hmac_config_set_mcs},
#ifdef _PRE_WLAN_FEATURE_CFG_ALG
    {WLAN_CFGID_ALG,                    HI_FALSE,  {0},    HI_NULL,            hmac_config_alg},
#endif
#if defined (_PRE_WLAN_FEATURE_HIPRIV) || defined (_PRE_WLAN_FEATURE_SIGMA)
    {WLAN_CFGID_FRAG_THRESHOLD_REG,     HI_FALSE,  {0},    HI_NULL,            hmac_config_frag_threshold},
    {WLAN_CFGID_AMPDU_TX_ON,            HI_FALSE,  {0},    HI_NULL,            hmac_config_set_ampdu_tx_on},
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_LIST_CHAN,              HI_FALSE,  {0},    HI_NULL,            hmac_config_list_channel},
    {WLAN_CFGID_SEND_BAR,               HI_FALSE,  {0},    HI_NULL,            hmac_config_send_bar},
#endif
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_AMPDU_END,              HI_FALSE,  {0},    HI_NULL,            hmac_config_ampdu_end},
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_SET_AGGR_NUM,           HI_FALSE,  {0},    HI_NULL,            hmac_config_set_ampdu_aggr_num},
#endif
    {WLAN_CFGID_SET_STBC_CAP,           HI_FALSE,  {0},    HI_NULL,            hmac_config_set_stbc_cap},
    {WLAN_CFGID_SET_BW,                 HI_FALSE,  {0},    hmac_config_get_bw,      hmac_config_set_bw},
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    {WLAN_CFGID_SET_ALWAYS_TX,          HI_FALSE,  {0},    HI_NULL,            hmac_config_always_tx},
#endif
    {WLAN_CFGID_SET_ALWAYS_RX,          HI_FALSE,  {0},    HI_NULL,            hmac_config_always_rx},
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_REG_INFO,               HI_FALSE,  {0},    HI_NULL,            hmac_config_reg_info},
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_WOW
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_HOST_SLEEP_EN,          HI_FALSE,  {0},    HI_NULL,            hmac_config_host_sleep_switch},
    {WLAN_CFGID_SET_WOW,                HI_FALSE,  {0},    HI_NULL,            hmac_config_set_wow},
    {WLAN_CFGID_WOW_ACTIVATE_EN,        HI_FALSE,  {0},    HI_NULL,            hmac_config_wow_activate_switch},
    {WLAN_CFGID_SET_WOW_PATTERN,        HI_FALSE,  {0},    HI_NULL,            hmac_config_set_wow_pattern},
#endif
    {WLAN_CFGID_SET_WOW_PARAM,          HI_FALSE,  {0},    HI_NULL,            hmac_config_wow_set_param},
#endif /* end of _PRE_WLAN_FEATURE_WOW */

#ifdef _PRE_WLAN_FEATURE_PROMIS
    {WLAN_CFGID_MONITOR_EN,            HI_FALSE,  {0},    HI_NULL,            hmac_config_set_monitor_switch},
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_REG_WRITE,               HI_FALSE,  {0},    HI_NULL,           hmac_config_reg_write},
#endif
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_TXBF_SWITCH,            HI_FALSE,  {0},    HI_NULL,            hmac_config_txbf_switch},
#endif
    {WLAN_CFGID_SCAN_ABORT,               HI_FALSE,  {0},    HI_NULL,      hmac_config_scan_abort},
/* 以下为内核cfg80211配置的命令 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_CFG80211_START_SCHED_SCAN, HI_FALSE,  {0},    HI_NULL,      hmac_start_sched_scan},
    {WLAN_CFGID_CFG80211_STOP_SCHED_SCAN, HI_FALSE,  {0},    HI_NULL,      hmac_stop_sched_scan},
#endif
    {WLAN_CFGID_CFG80211_START_SCAN,      HI_FALSE,  {0},    HI_NULL,      hmac_process_scan_req},
    {WLAN_CFGID_CFG80211_START_CONNECT,   HI_FALSE,  {0},    HI_NULL,      hmac_config_connect},
    {WLAN_CFGID_CFG80211_SET_CHANNEL,     HI_FALSE,  {0},    HI_NULL,      hmac_config_set_channel},
    {WLAN_CFGID_CFG80211_CONFIG_BEACON,   HI_FALSE,  {0},    HI_NULL,      hmac_config_set_beacon},

    {WLAN_CFGID_ADD_KEY,           HI_FALSE,  {0},   HI_NULL,              hmac_config_11i_add_key},
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_GET_KEY,           HI_FALSE,  {0},   HI_NULL,              hmac_config_11i_get_key},
#endif
    {WLAN_CFGID_REMOVE_KEY,        HI_FALSE,  {0},   HI_NULL,              hmac_config_11i_remove_key},
    {WLAN_CFGID_ALG_PARAM,         HI_FALSE,  {0},   HI_NULL,              hmac_config_alg_param},

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP_DEBUG
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_EDCA_OPT_SWITCH_STA,  HI_FALSE,  {0},   HI_NULL,           hmac_config_set_edca_opt_switch_sta},
    {WLAN_CFGID_EDCA_OPT_SWITCH_AP,   HI_FALSE,  {0},   HI_NULL,           hmac_config_set_edca_opt_switch_ap},
#endif
    {WLAN_CFGID_EDCA_OPT_CYCLE_AP,    HI_FALSE,  {0},   HI_NULL,           hmac_config_set_edca_opt_cycle_ap},
#endif

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_SET_FLOWCTL_PARAM,    HI_FALSE,  {0},   HI_NULL,           hmac_config_set_flowctl_param},
    {WLAN_CFGID_GET_FLOWCTL_STAT,     HI_FALSE,  {0},   HI_NULL,           hmac_config_get_flowctl_stat},
#endif
#endif
#ifdef _PRE_DEBUG_MODE
    /* START:开源APP 程序下发的私有命令 */
    {WLAN_CFGID_GET_ASSOC_REQ_IE,  HI_FALSE,  {0},   hmac_config_get_assoc_req_ie,   HI_NULL},
#endif
    {WLAN_CFGID_SET_RTS_THRESHHOLD, HI_FALSE,  {0},   HI_NULL,             hmac_config_rts_threshold},
    /* END:开源APP 程序下发的私有命令 */
    {WLAN_CFGID_DEFAULT_KEY,        HI_FALSE,  {0},   HI_NULL,            hmac_config_11i_set_default_key},

    {WLAN_CFGID_WMM_SWITCH,         HI_FALSE,  {0},   HI_NULL,            hmac_config_open_wmm},
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_SET_MIB,            HI_FALSE,  {0},   HI_NULL,            hmac_config_set_mib},
    {WLAN_CFGID_GET_MIB,            HI_FALSE,  {0},   HI_NULL,            hmac_config_get_mib},
    {WLAN_CFGID_SET_RANDOM_MAC_ADDR_SCAN, HI_FALSE,  {0},  HI_NULL,       hmac_config_set_random_mac_addr_scan},
#endif
#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_REGDOMAIN_PWR,            HI_FALSE,  {0},  HI_NULL,            hmac_config_set_regdomain_pwr},
    {WLAN_CFGID_START_JOIN,               HI_FALSE,  {0},  HI_NULL,            hmac_config_start_join},
    {WLAN_CFGID_START_DEAUTH,             HI_FALSE,  {0},  HI_NULL,            hmac_config_send_deauth},
    {WLAN_CFGID_ADD_USER,                 HI_FALSE,  {0},  HI_NULL,            hmac_config_add_user},
    {WLAN_CFGID_SET_AUTO_PROTECTION,      HI_FALSE,  {0},  HI_NULL,            hmac_config_set_auto_protection},
    {WLAN_CFIGD_BGSCAN_ENABLE,            HI_FALSE,  {0},  HI_NULL,            hmac_config_bgscan_enable},
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_REDUCE_SAR,               HI_FALSE,  {0},  HI_NULL,            hmac_config_set_regdomain_pwr},
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_HIDE_SSID,                HI_FALSE,  {0},  HI_NULL,            hmac_config_hide_ssid},
    {WLAN_CFGID_START_SCAN,               HI_FALSE,  {0},  HI_NULL,            hmac_sta_initiate_scan},
#endif
    {WLAN_CFGID_DYNC_TXPOWER,             HI_FALSE,  {0},  HI_NULL,            hmac_config_dync_txpower},
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_VAP_PKT_STAT,             HI_FALSE,  {0},  HI_NULL,            hmac_config_vap_pkt_stat},
#endif
    {WLAN_CFGID_GET_ALL_REG_VALUE,        HI_FALSE,  {0},  HI_NULL,            hmac_config_get_all_reg_value},
    {WLAN_CFGID_REPORT_AMPDU_STAT,        HI_FALSE,  {0},  HI_NULL,            hmac_config_report_ampdu_stat},
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_SCAN_TEST,                HI_FALSE,  {0},  HI_NULL,            hmac_config_scan_test},
#endif
    {WLAN_CFGID_BTCOEX_INFO,              HI_FALSE,  {0},  HI_NULL,            hmac_config_print_btcoex_status},
    {WLAN_CFGID_GET_MPDU_NUM,             HI_FALSE,  {0},  HI_NULL,            hmac_config_get_mpdu_num},
#endif

    {WLAN_CFGID_CFG80211_MGMT_TX,   HI_FALSE,  {0},    HI_NULL,           hmac_wpas_mgmt_tx},
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_QUERY_STATION_STATS,   HI_FALSE,  {0},    HI_NULL,           hmac_config_query_station_info},
#endif
#ifdef _PRE_WLAN_FEATURE_CSI
    {WLAN_CFGID_CSI_SWITCH,             HI_FALSE,  {0},    HI_NULL,       hmac_config_csi_set_switch},
    {WLAN_CFGID_CSI_SET_CONFIG,         HI_FALSE,  {0},    HI_NULL,       hmac_config_csi_set_config},
#endif

#ifdef _PRE_WLAN_FEATURE_STA_PM
    {WLAN_CFGID_SET_PS_MODE,           HI_FALSE,  {0},   HI_NULL,         hmac_config_set_sta_pm_mode},
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_SET_PSM_PARAM,         HI_FALSE,  {0},   HI_NULL,          hmac_config_set_pm_param},
#endif
    {WLAN_CFGID_SET_STA_PM_ON,          HI_FALSE,  {0},   HI_NULL,         hmac_config_set_sta_pm_on},
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_SET_PSM_OFFSET,        HI_FALSE,  {0},   HI_NULL,          hmac_config_set_psm_offset},
    {WLAN_CFGID_SET_STA_HW_PS_MODE,    HI_FALSE,  {0},   HI_NULL,          hmac_config_set_sta_hw_ps_mode},
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
    {WLAN_CFGID_SET_UAPSD_PARA,        HI_FALSE, {0},      HI_NULL,       hmac_config_set_uapsd_para},
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    {WLAN_CFGID_SEND_P2P_ACTION,                   HI_FALSE,  {0},   HI_NULL,   hmac_p2p_send_action},
    {WLAN_CFGID_SET_WPS_P2P_IE,                    HI_FALSE,  {0},   HI_NULL,   hmac_p2p_set_wps_p2p_ie},
    {WLAN_CFGID_CFG80211_REMAIN_ON_CHANNEL,        HI_FALSE,  {0},   HI_NULL,   hmac_p2p_config_remain_on_channel},
    {WLAN_CFGID_CFG80211_CANCEL_REMAIN_ON_CHANNEL, HI_FALSE,  {0},   HI_NULL,   hmac_p2p_cancel_remain_on_channel},
    {WLAN_CFGID_SET_P2P_PS_OPS,                    HI_FALSE,  {0},   HI_NULL,   hmac_p2p_set_ps_ops},
    {WLAN_CFGID_SET_P2P_PS_NOA,                    HI_FALSE,  {0},   HI_NULL,   hmac_p2p_set_ps_noa},
    {WLAN_CFGID_SET_P2P_STATUS,                    HI_FALSE,  {0},   HI_NULL,   hmac_p2p_set_status},
#endif
#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
    {WLAN_CFGID_ARP_OFFLOAD_SETTING,     HI_FALSE,  {0},    HI_NULL,  hmac_config_arp_offload_setting},
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_ARP_OFFLOAD_SHOW_INFO,  HI_FALSE,  {0},    HI_NULL,   hmac_config_arp_offload_show_info},
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_DHCP_OFFLOAD
    {WLAN_CFGID_DHCP_OFFLOAD_SETTING,    HI_FALSE,  {0},    HI_NULL,   hmac_config_dhcp_offload_setting},
#endif
#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    {WLAN_CFGID_CFG_VAP_H2D,  HI_FALSE,    {0},    HI_NULL,            hmac_config_cfg_vap_h2d},
    {WLAN_CFGID_HOST_DEV_INIT,  HI_FALSE,  {0},    HI_NULL,            hmac_config_host_dev_init},
    {WLAN_CFGID_HOST_DEV_EXIT,  HI_FALSE,  {0},    HI_NULL,            hmac_config_host_dev_exit}, /* liuxiaoqi */
#endif
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_SET_CHN_EST_CTRL,          HI_FALSE,  {0},  HI_NULL,        hmac_config_set_chn_est_ctrl},
#endif
    {WLAN_CFGID_SET_PM_CFG_PARAM,          HI_FALSE,  {0},  HI_NULL,        hmac_config_set_pm_cfg_param},
    {WLAN_CFGID_SET_CUS_RF,                HI_FALSE,  {0},  HI_NULL,        hmac_config_set_cus_rf},
    {WLAN_CFGID_SET_CUS_DTS_CALI,          HI_FALSE,  {0},  HI_NULL,        hmac_config_set_cus_dts_cali},
    {WLAN_CFGID_SET_CUS_NVRAM_PARAM,       HI_FALSE,  {0},  HI_NULL,        hmac_config_set_cus_nvram_params},
    {WLAN_CFGID_SET_CUS_FCC_TX_PWR,        HI_FALSE,  {0},  HI_NULL,        hmac_config_set_cus_fcc_tx_params},
#ifdef _PRE_XTAL_FREQUENCY_COMPESATION_ENABLE
    {WLAN_CFGID_SET_FREQ_COMP,             HI_FALSE,  {0},  HI_NULL,        hmac_config_set_freq_comp},
#endif
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_DESTROY_VAP,               HI_FALSE,  {0},  HI_NULL,        hmac_config_vap_destroy},
#endif
    {WLAN_CFGID_SET_RETRY_LIMIT,           HI_FALSE,  {0},  HI_NULL,        hmac_config_set_retry_limit},
#ifdef _PRE_WLAN_FEATURE_MESH
    {WLAN_CFGID_SET_MESH_STA,              HI_FALSE,  {0},  HI_NULL,        hmac_config_set_mesh_user},
    {WLAN_CFGID_SEND_MESH_ACTION,          HI_FALSE,  {0},  HI_NULL,        hmac_config_send_mesh_action},
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_SET_REPORT_TIMES_LIMIT,    HI_FALSE,  {0},  HI_NULL,        hmac_config_set_report_times_limit},
    {WLAN_CFGID_SET_REPORT_CNT_LIMIT,      HI_FALSE,  {0},  HI_NULL,        hmac_config_set_report_cnt_limit},
#endif
    {WLAN_CFGID_MESHID,                    HI_FALSE,  {0},  hmac_config_get_meshid, hmac_config_set_meshid},
    {WLAN_CFGID_ACCEPT_PEER,               HI_FALSE,  {0},  HI_NULL,        hmac_config_set_accept_peer},
    {WLAN_CFGID_SET_BEACON_PRIORITY,       HI_FALSE,  {0},  HI_NULL,        hmac_config_set_beacon_priority},
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_SET_MBR_EN,                HI_FALSE,  {0},  HI_NULL,        hmac_config_set_en_mbr},
#endif
    {WLAN_CFGID_SET_MNID,                  HI_FALSE,  {0},  HI_NULL,        hmac_config_set_mnid},
    {WLAN_CFGID_ADD_MESH_USER,             HI_FALSE,  {0},  HI_NULL,        hmac_config_add_mesh_user},
    {WLAN_CFGID_SET_NEW_PEER_CONFIG_EN,    HI_FALSE,  {0},  HI_NULL,        hmac_config_new_peer_candidate_en},
    {WLAN_CFGID_SET_MESH_ACCEPT_STA,       HI_FALSE,  {0},  HI_NULL,        hmac_config_set_mesh_accept_sta},
    {WLAN_CFGID_SET_MESH_USER_GTK,         HI_FALSE,  {0},  HI_NULL,        hmac_config_set_mesh_user_gtk},
    {WLAN_CFGID_GET_MESH_NODE_INFO,        HI_FALSE,  {0},  hmac_config_get_mesh_nodeinfo, HI_NULL},
#endif

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
    {WLAN_CFGID_TX_CLASSIFY_LAN_TO_WLAN_SWITCH,     HI_FALSE,  {0},   HI_NULL,      hmac_config_set_tx_classify_switch},
#endif
    {WLAN_CFGID_RSSI,                HI_FALSE,  {0},    HI_NULL,            hmac_config_query_rssi},
    {WLAN_CFGID_CUSTOM_PKT,          HI_FALSE,  {0},   HI_NULL,             hmac_send_custom_pkt},
#ifdef _PRE_WLAN_FEATURE_ANY
    {WLAN_CFGID_ANY_SEND_PEER_DATA,  HI_FALSE,  {0},    HI_NULL,            hmac_any_send_action},
    {WLAN_CFGID_ANY_ADD_PEER_INFO,  HI_FALSE,  {0},    HI_NULL,             hmac_any_add_peer},
    {WLAN_CFGID_ANY_DEL_PEER_INFO,  HI_FALSE,  {0},    HI_NULL,             hmac_any_del_peer},
    {WLAN_CFGID_ANY_INIT,  HI_FALSE,  {0},    HI_NULL,                      hmac_any_init},
    {WLAN_CFGID_ANY_DEINIT,  HI_FALSE,  {0},    HI_NULL,                    hmac_any_deinit},
    {WLAN_CFGID_ANY_SCAN,  HI_FALSE,  {0},    HI_NULL,                      hmac_any_start_scan},
    {WLAN_CFGID_ANY_FETCH_PEER_INFO,  HI_FALSE,  {0},  HI_NULL,             hmac_any_query_peer_by_index},
#endif
#ifdef FEATURE_DAQ
    {WLAN_CFGID_DATA_ACQ_START,   HI_FALSE,  {0},    HI_NULL,        hmac_config_data_acq_start},
    {WLAN_CFGID_DATA_ACQ_STATUS,  HI_FALSE,  {0},    HI_NULL,        hmac_config_data_acq_status},
    {WLAN_CFGID_DATA_ACQ_REPORT,  HI_FALSE,  {0},    HI_NULL,        hmac_config_data_acq_result},
#endif
#ifdef _PRE_WLAN_FEATURE_BW_HIEX
    {WLAN_CFGID_SET_SELFCTS,   HI_FALSE,  {0},    HI_NULL,        hmac_config_hiex_set_selfcts},
#endif
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    {WLAN_CFGID_CAL_BAND_POWER,  HI_FALSE,  {0},  HI_NULL,        hmac_config_set_cal_band_power},
    {WLAN_CFGID_CAL_RATE_POWER,  HI_FALSE,  {0},  HI_NULL,        hmac_config_set_cal_rate_power},
    {WLAN_CFGID_CAL_FREQ,        HI_FALSE,  {0},  HI_NULL,        hmac_config_set_cal_freq},
    {WLAN_CFGID_SET_EFUSE_MAC,   HI_FALSE,  {0},  HI_NULL,        hmac_config_set_efuse_mac},
    {WLAN_CFGID_GET_EFUSE_MAC,   HI_FALSE,  {0},  HI_NULL,        hmac_config_get_efuse_mac},
    {WLAN_CFGID_SET_DATAEFUSE,   HI_FALSE,  {0},  HI_NULL,        hmac_config_set_dataefuse},
#endif
    {WLAN_CFGID_SET_CCA_TH,      HI_FALSE,  {0},  HI_NULL,        hmac_config_set_cca_th},
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    {WLAN_CFGID_GET_CAL_DATA,          HI_FALSE,  {0},  HI_NULL,  hmac_config_get_cal_data},
#endif
    {WLAN_CFGID_SET_TX_PWR_OFFSET,     HI_FALSE,  {0},  HI_NULL,  hmac_config_set_tx_pwr_offset},
    {WLAN_CFGID_GET_EFUSE_MAC_ADDR,    HI_FALSE,  {0},  HI_NULL,  hmac_config_get_efuse_mac_addr},
#if defined(_PRE_WLAN_FEATURE_HIPRIV) && defined(_PRE_WLAN_FEATURE_INTRF_MODE)
    {WLAN_CFGID_INTRF_MODE_ON,         HI_FALSE,  {0},  HI_NULL,  hmac_config_set_intrf_mode},
#endif
    {WLAN_CFGID_NOTIFY_GET_TX_PARAMS,     HI_FALSE,   {0},    HI_NULL,    hmac_config_notify_get_tx_params},
    {WLAN_CFGID_MINIMIZE_BOOT_CURRET,   HI_FALSE,  {0},  HI_NULL, hmac_config_minimize_boot_current},
    {WLAN_CFGID_BUTT,                   HI_FALSE,  {0},    0,     0},
};

/* ================ hmac配置同步事件 ============================================= */
static const hmac_config_syn_stru g_ast_hmac_config_syn[] = {
/* 同步ID                                 保留2个字节            函数操作 */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {WLAN_CFGID_QUERY_STATION_STATS,        {0, 0}, hmac_proc_query_response_event},
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    {WLAN_CFGID_THRUPUT_INFO,               {0, 0}, hmac_get_thruput_info},
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_BTCOEX_RX_DELBA_TRIGGER,    {0, 0}, hmac_btcoex_rx_delba_trigger},
#endif
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    {WLAN_CFGID_REG_INFO,                   {0, 0}, hmac_wifitest_get_reg_info},
#endif
#endif
    {WLAN_CFGID_CFG80211_MGMT_TX_STATUS,    {0, 0}, hmac_mgmt_tx_event_status},
#ifdef _PRE_WLAN_FEATURE_MESH
    {WLAN_CFGID_UNICAST_DATA_TX_INFO,       {0, 0}, hmac_unicast_data_tx_event_info},
#endif
#ifdef _PRE_WLAN_FEATURE_ANY
    {WLAN_CFGID_ANY_TX_STATUS,              {0, 0}, hmac_any_tx_event_status},
    {WLAN_CFGID_ANY_FETCH_PEER_INFO,        {0, 0}, hmac_any_get_peer_info},
#endif
#ifdef FEATURE_DAQ
    {WLAN_CFGID_DATA_ACQ_STATUS,            {0, 0}, hmac_get_data_acq_status},
    {WLAN_CFGID_DATA_ACQ_REPORT,            {0, 0}, hmac_get_data_acq_result},
#endif
    {WLAN_CFGID_RX_FCS_INFO,                {0, 0}, hmac_get_rx_fcs_info},
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    {WLAN_CFGID_MFG_TEST,                   {0, 0}, hmac_report_mfg_test},
    {WLAN_CFGID_REPORT_CAL_DATA,            {0, 0}, hmac_report_dbg_cal_data_from_dev},
#endif
    {WLAN_CFGID_GET_MAC_FROM_EFUSE,         {0, 0}, hmac_report_mac_from_efuse},
    {WLAN_CFGID_RSSI,                       {0, 0}, hmac_proc_query_rssi_response},
#if defined (_PRE_WLAN_FEATURE_HIPRIV) || defined (_PRE_WLAN_FEATURE_SIGMA)
    {WLAN_CFGID_AMPDU_TX_ON,                {0, 0}, hmac_config_set_ampdu_tx_on_from_dmac},
#endif
    {WLAN_CFGID_REPORT_TX_PARAMS,           {0, 0}, hmac_config_report_tx_params},
    {WLAN_CFGID_BUTT,                       {0, 0}, HI_NULL},
};

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
hi_void hmac_event_fsm_table_register(hi_void)
{
    /* Part1: 以下是Hmac收的事件 */
    /* 注册WLAN_DTX事件子表 */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_DTX, FRW_EVENT_PIPELINE_STAGE_0, g_ast_hmac_wlan_dtx_event_sub_table);

    /* 注册WLAN_DRX事件子表 */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_DRX, FRW_EVENT_PIPELINE_STAGE_1, g_ast_hmac_wlan_drx_event_sub_table);

    /* 注册HMAC模块WLAN_CRX事件子表 */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_CRX, FRW_EVENT_PIPELINE_STAGE_1, g_ast_hmac_wlan_crx_event_sub_table);

    /* 注册DMAC模块MISC事件字表 */
    frw_event_table_register(FRW_EVENT_TYPE_DMAC_MISC, FRW_EVENT_PIPELINE_STAGE_1,
        g_ast_hmac_wlan_misc_event_sub_table);

    /* 注册统计结果查询事件子表 */
    frw_event_table_register(FRW_EVENT_TYPE_DMAC_TO_HMAC_CFG, FRW_EVENT_PIPELINE_STAGE_1,
        g_ast_hmac_wlan_ctx_event_sub_table);

    /* Part2: 以下是Hmac收Wal 的事件 */
    frw_event_table_register(FRW_EVENT_TYPE_HOST_CRX, FRW_EVENT_PIPELINE_STAGE_0, g_ast_wal_host_crx_table);
}

/* ****************************************************************************
 功能描述  : 处理配置查询请求
 输入参数  : event_hdr   : 事件头
             puc_req_msg     : 请求消息
             us_req_msg_len  : 请求消息长度
 输出参数  : puc_rsp_msg     : 返回消息
             pus_rsp_msg_len : 返回消息长度
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2012年11月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 wal_config_process_query(mac_vap_stru *mac_vap, hi_u8 *req, hi_u16 req_len, hi_u8 *rsp, hi_u8 *rsp_len)
{
    hi_u16 us_req_idx = 0;      /* 请求消息索引 */
    hi_u16 us_rsp_idx = 0;      /* 返回消息索引 */
    hi_u16 us_len     = 0;      /* WID对应返回值的长度 */
    hi_u16 us_cfgid;

    /* 查询消息格式如下:                                                     */
    /* +-------------------------------------------------------------------+ */
    /* | WID0          | WID1         | WID2         | ................... | */
    /* +-------------------------------------------------------------------+ */
    /* |     2 Bytes   |    2 Bytes   |    2 Bytes   | ................... | */
    /* +-------------------------------------------------------------------+ */
    /* 返回消息格式如下:                                                     */
    /* +-------------------------------------------------------------------+ */
    /* | WID0      | WID0 Length | WID0 Value  | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes   | 2 Byte      | WID Length  | ......................... | */
    /* +-------------------------------------------------------------------+ */
    while (us_req_idx < req_len) {
        /* 从查询消息中得到一个WID值   */
        wal_msg_query_stru *query_msg = (wal_msg_query_stru *)(&req[us_req_idx]);
        us_req_idx += WAL_MSG_WID_LENGTH; /* 指向下一个WID */

        /* 获取返回消息内存 */
        wal_msg_write_stru *rsp_msg = (wal_msg_write_stru *)(&rsp[us_rsp_idx]);

        /* 寻找cfgid 对应的get函数 */
        for (us_cfgid = 0; WLAN_CFGID_BUTT != g_ast_board_wid_op[us_cfgid].cfgid; us_cfgid++) {
            if (query_msg->wid == g_ast_board_wid_op[us_cfgid].cfgid) {
                break;
            }
        }

        /* 异常情况，cfgid不在操作函数表中 */
        if (g_ast_board_wid_op[us_cfgid].cfgid == WLAN_CFGID_BUTT) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY,
                             "{wal_config_process_query::cfgid not invalid %d!}\r\n", query_msg->wid);
            continue;
        }

        /* 异常情况，cfgid对应的get函数为空 */
        if (g_ast_board_wid_op[us_cfgid].get_func == HI_NULL) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY,
                "{wal_config_process_query::g_ast_board_wid_op get_func ptr is null, wid is %d!}\r\n", query_msg->wid);
            continue;
        }

        hi_u32 ret = g_ast_board_wid_op[us_cfgid].get_func(mac_vap, &us_len, rsp_msg->auc_value);
        if (ret != HI_SUCCESS) {
            oam_warning_log3(mac_vap->vap_id, OAM_SF_ANY,
                "{wal_config_process_query::g_ast_board_wid_op[cfg_id=%d] func return fail.wid and ret value is:%d,%d}",
                us_cfgid, query_msg->wid, ret);
            continue;
        }

        rsp_msg->wid = query_msg->wid; /* 设置返回消息的WID */
        rsp_msg->us_len = us_len;

        us_rsp_idx += us_len + WAL_MSG_WRITE_MSG_HDR_LENGTH; /* 消息体的长度 再加上消息头的长度 */

        /* 消息Response 接口容易让调用者使用超过消息数组空间长度，
           这里需要加判断，检查长度和狗牌，后续需要整改 */
        if (oal_unlikely(us_rsp_idx + sizeof(wal_msg_hdr_stru) >= HMAC_RSP_MSG_MAX_LEN)) {
            oam_error_log3(mac_vap->vap_id, OAM_SF_ANY, "{wal_config_process_query::cfgid=%d,msg_len:%u over limit:%u}",
                us_cfgid, us_rsp_idx + sizeof(wal_msg_hdr_stru), HMAC_RSP_MSG_MAX_LEN);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
            oal_print_hex_dump((hi_u8 *)rsp, HMAC_RSP_MSG_MAX_LEN, 32, "puc_rsp_msg: "); /* 32:组大小 */
#endif
            break;
        }
    }

    *rsp_len = (hi_u8)us_rsp_idx;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理配置设置请求
 输入参数  : event_hdr: 事件头
             puc_req_msg  : 请求消息
             us_msg_len   : 请求消息长度
 输出参数  : HI_SUCCESS或其它错误码
             puc_rsp_msg     : 返回消息
             pus_rsp_msg_len : 返回消息长度
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2012年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 wal_config_process_write(mac_vap_stru *mac_vap, hi_u8 *puc_req_msg, hi_u16 us_msg_len,
    hi_u8 *puc_rsp_msg, hi_u8 *puc_rsp_msg_len)
{
    hi_u16                  us_req_idx = 0;
    hi_u16                  us_rsp_idx = 0;
    hi_u16                  us_cfgid;
    wal_msg_write_stru     *write_msg = HI_NULL;
    wal_msg_write_rsp_stru *rsp_msg = HI_NULL;
    hi_u32                  ret;
    if (oal_unlikely((mac_vap == HI_NULL) || (puc_req_msg == HI_NULL))) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_config_process_write::pst_mac_vap/puc_req_msg null ptr error %p, %p!}\r\n",
            (uintptr_t)mac_vap, (uintptr_t)puc_req_msg);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 设置消息的格式如下:                                                   */
    /* +-------------------------------------------------------------------+ */
    /* | WID0      | WID0 Length | WID0 Value  | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes   | 2 Byte      | WID Length  | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* 返回消息的格式如下:                                                   */
    /* +-------------------------------------------------------------------+ */
    /* | WID0     | resv    | WID0 错误码 |  WID1   | resv | WID1错误码 |  | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes  | 2 Bytes | 4 Byte      | 2 Bytes | 2 B  |  4 Bytes   |  | */
    /* +-------------------------------------------------------------------+ */
    while (us_req_idx < us_msg_len) {
        /* 获取一个设置WID消息   */
        write_msg = (wal_msg_write_stru *)(&puc_req_msg[us_req_idx]);
        /* 获取返回消息内存 */
        rsp_msg = (wal_msg_write_rsp_stru *)(&puc_rsp_msg[us_rsp_idx]);

        us_req_idx += write_msg->us_len + WAL_MSG_WRITE_MSG_HDR_LENGTH; /* 指向下一个WID设置消息 */

        /* 寻找cfgid 对应的write函数 */
        for (us_cfgid = 0; g_ast_board_wid_op[us_cfgid].cfgid != WLAN_CFGID_BUTT; us_cfgid++) {
            if (write_msg->wid == g_ast_board_wid_op[us_cfgid].cfgid) {
                break;
            }
        }

        /* 异常情况，cfgid不在操作函数表中 */
        if (g_ast_board_wid_op[us_cfgid].cfgid == WLAN_CFGID_BUTT) {
            continue;
        }
        ret = g_ast_board_wid_op[us_cfgid].set_func(mac_vap, write_msg->us_len, write_msg->auc_value);
        /* 将返回错误码设置到rsp消息中 */
        rsp_msg->wid = write_msg->wid;
        rsp_msg->err_code = ret;
        us_rsp_idx += sizeof(wal_msg_write_rsp_stru);

        /* 消息Response 接口容易让调用者使用超过消息数组空间长度，
           这里需要加判断，检查长度和狗牌，后续需要整改 */
        if (oal_unlikely(us_rsp_idx + sizeof(wal_msg_hdr_stru) > HMAC_RSP_MSG_MAX_LEN)) {
            oam_error_log3(mac_vap->vap_id, OAM_SF_ANY,
                "{wal_config_process_write::us_cfgid:%d response msg len:%u over limit:%u}", us_cfgid,
                us_rsp_idx + sizeof(wal_msg_hdr_stru), HMAC_RSP_MSG_MAX_LEN);
        }

        if (ret != HI_SUCCESS) {
            oam_warning_log3(mac_vap->vap_id, OAM_SF_ANY,
                "{config_process_write::g_ast_board_wid_op[cfg_id=%d] func return failed. wid ret value is %d,%d!}\r\n",
                us_cfgid, write_msg->wid, ret);
        }
    }

    *puc_rsp_msg_len = (hi_u8)us_rsp_idx;

    return HI_SUCCESS;
}

hi_u32 wal_set_msg_response(hi_u32 request_address, hi_u8 rsp_toal_len, const hi_u8 *rsp_msg)
{
    hi_u8 *rsp_msg_tmp = HI_NULL;

    if (request_address) {
        rsp_msg_tmp = oal_memalloc(rsp_toal_len);
        if (rsp_msg_tmp == NULL) {
            oam_error_log1(0, OAM_SF_ANY, "{wal_set_msg_response:: msg alloc %u failed!", rsp_toal_len);

            if (wal_set_msg_response_by_addr(request_address, NULL, HI_ERR_CODE_PTR_NULL, rsp_toal_len) != HI_SUCCESS) {
                oam_error_log0(0, OAM_SF_ANY, "{wal_set_msg_response::didn't found the request msg!");
            }
        } else {
            if (memcpy_s(rsp_msg_tmp, rsp_toal_len, rsp_msg, rsp_toal_len) != EOK) {
                oal_free(rsp_msg_tmp);
                oam_error_log0(0, OAM_SF_CFG, "wal_set_msg_response::ac_rsp_msg memcpy_s fail.");
                return HI_FAIL;
            }

            if (wal_set_msg_response_by_addr(request_address, rsp_msg_tmp, HI_SUCCESS, rsp_toal_len) != HI_SUCCESS) {
                oam_error_log0(0, OAM_SF_ANY, "{wal_set_msg_response::didn't found the request msg!");
                oal_free(rsp_msg_tmp);
            }
        }
    }

    /* 唤醒WAL等待的进程 */
    wal_cfg_msg_task_sched();
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 配置包事件处理
 输入参数  : pst_event: 事件
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2012年11月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 wal_config_process_pkt(frw_event_mem_stru *event_mem)
{
    hi_u32 ret;
    hi_u8  ac_rsp_msg[HMAC_RSP_MSG_MAX_LEN] = {0};
    hi_u8 rsp_len = 0;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_config_process_pkt::event_mem null ptr error!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    frw_event_stru     *event     = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_stru *event_hdr = &(event->event_hdr);
    wal_msg_stru       *msg       = (wal_msg_stru *)(frw_get_event_payload(event_mem) + sizeof(wal_msg_rep_hdr));
    mac_vap_stru       *mac_vap   = mac_vap_get_vap_stru(event_hdr->vap_id);
    hi_u32              request_address = ((wal_msg_rep_hdr*)event->auc_event_data)->request_address;

    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_warning_log0(event_hdr->vap_id, OAM_SF_ANY, "{wal_config_process_pkt::hmac_get_vap_by_id return err code}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 取返回消息 */
    wal_msg_stru *rsp_msg = (wal_msg_stru *)ac_rsp_msg;

    /* 取配置消息的长度 */
    /* event->auc_event_data, 可变数组 */
    hi_u16 us_msg_len = msg->msg_hdr.us_msg_len;

    switch (msg->msg_hdr.msg_type) {
        case WAL_MSG_TYPE_QUERY:
            ret = wal_config_process_query(mac_vap, msg->auc_msg_data, us_msg_len, rsp_msg->auc_msg_data, &rsp_len);
            if (ret != HI_SUCCESS) {
                oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_config_process_pkt::query Err=%d}", ret);
                return ret;
            }
            break;

        case WAL_MSG_TYPE_WRITE:
            ret = wal_config_process_write(mac_vap, msg->auc_msg_data, us_msg_len, rsp_msg->auc_msg_data, &rsp_len);
            if (ret != HI_SUCCESS) {
                oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_config_process_pkt::write Err=%d}", ret);
                return ret;
            }
            break;

        default:
            oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_config_process_pkt::msg_type Err=%d}",
                msg->msg_hdr.msg_type);
            return HI_ERR_CODE_INVALID_CONFIG;
    }

    /* response 长度要包含头长 */
    hi_u8 rsp_toal_len = rsp_len + sizeof(wal_msg_hdr_stru);
    if (oal_unlikely(rsp_toal_len > HMAC_RSP_MSG_MAX_LEN)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_config_process_pkt::invalid response len %u}", rsp_toal_len);
        return HI_FAIL;
    }

    /* 填充返回消息头 */
    rsp_msg->msg_hdr.msg_type   = WAL_MSG_TYPE_RESPONSE;
    rsp_msg->msg_hdr.msg_sn     = msg->msg_hdr.msg_sn;
    rsp_msg->msg_hdr.us_msg_len = rsp_len;

    return wal_set_msg_response(request_address, rsp_toal_len, ac_rsp_msg);
}

/* ****************************************************************************
 功能描述  : hmac配置同步事件处理入口
 输入参数  : event_mem: 事件内存结构体
 修改历史      :
  1.日    期   : 2013年4月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_event_config_syn(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    frw_event_hdr_stru *event_hdr = HI_NULL;
    dmac_to_hmac_cfg_msg_stru *dmac2hmac_msg = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u32 ret;
    hi_u16 us_cfgid;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_event_config_syn::event_mem null.}");

        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取事件 */
    event = (frw_event_stru *)event_mem->puc_data;
    event_hdr = &(event->event_hdr);
    dmac2hmac_msg = (dmac_to_hmac_cfg_msg_stru *)event->auc_event_data;

    oam_info_log1(event_hdr->vap_id, OAM_SF_CFG, "{hmac_event_config_syn::a dmac config syn event occur, cfg_id=%d.}",
        dmac2hmac_msg->syn_id);
    /* 获取dmac vap */
    mac_vap = mac_vap_get_vap_stru(event_hdr->vap_id);
    if (mac_vap == HI_NULL) {
        oam_error_log0(event_hdr->vap_id, OAM_SF_CFG, "{hmac_event_config_syn::pst_mac_vap null.}");

        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获得cfg id对应的操作函数 */
    for (us_cfgid = 0; WLAN_CFGID_BUTT != g_ast_hmac_config_syn[us_cfgid].cfgid; us_cfgid++) {
        if (g_ast_hmac_config_syn[us_cfgid].cfgid == dmac2hmac_msg->syn_id) {
            break;
        }
    }

    /* 异常情况，cfgid在g_ast_dmac_config_syn中不存在 */
    if (WLAN_CFGID_BUTT == g_ast_hmac_config_syn[us_cfgid].cfgid) {
        oam_warning_log1(event_hdr->vap_id, OAM_SF_CFG, "{hmac_event_config_syn::invalid en_cfgid[%d].",
            dmac2hmac_msg->syn_id);
        return HI_ERR_CODE_INVALID_CONFIG;
    }

    /* 执行操作函数 */
    ret = g_ast_hmac_config_syn[us_cfgid].set_func(mac_vap, (hi_u8)(dmac2hmac_msg->us_len),
        (hi_u8 *)dmac2hmac_msg->auc_msg_body);
    if (ret != HI_SUCCESS) {
        oam_warning_log2(event_hdr->vap_id, OAM_SF_CFG,
            "{hmac_event_config_syn::p_set_func failed, ul_ret=%d en_syn_id=%d.", ret, dmac2hmac_msg->syn_id);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : hmac接收dmac控制事件
**************************************************************************** */
hi_u32 hmac_from_dmac_rx_control_handle(frw_event_mem_stru *event_mem)
{
    frw_event_hdr_stru *event_hdr = HI_NULL;
    const frw_event_sub_table_item_stru *sub_table_item = HI_NULL;

    event_hdr = frw_get_event_hdr(event_mem);
    sub_table_item = frw_get_event_sub_table(event_hdr->type, FRW_EVENT_PIPELINE_STAGE_1);
    if (sub_table_item == NULL) {
        return HI_FAIL;
    }

    return sub_table_item[event_hdr->sub_type].func(event_mem);
}

/* ****************************************************************************
 功能描述  : hmac接收dmac数据事件
**************************************************************************** */
hi_u32 hmac_from_dmac_rx_data_handle(frw_event_mem_stru *event_mem, oal_netbuf_stru *netbuf, hi_u16 netbuf_num)
{
    frw_event_stru           *event          = frw_get_event_stru(event_mem);
    frw_event_hdr_stru       *event_hdr      = frw_get_event_hdr(event_mem);
    dmac_wlan_crx_event_stru *wlan_crx_event = (dmac_wlan_crx_event_stru *)(event->auc_event_data);
    dmac_tx_event_stru *crx_event = (dmac_tx_event_stru *)(event->auc_event_data);
    dmac_wlan_drx_event_stru *wlan_rx_event = (dmac_wlan_drx_event_stru *)(event->auc_event_data);
    const frw_event_sub_table_item_stru *sub_table_item = HI_NULL;

    if (event_hdr->type == FRW_EVENT_TYPE_WLAN_CRX) {
        switch (event_hdr->sub_type) {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
            case DMAC_WLAN_CRX_INIT:
#endif
#ifdef _PRE_WLAN_FEATURE_PROMIS
            case DMAC_WLAN_CRX_EVENT_PROMIS:
#endif
            case DMAC_WLAN_CRX_RX:
                wlan_crx_event->netbuf = (oal_dev_netbuf_stru *)netbuf;
                break;
            case DMAC_WLAN_CRX_NEW_PEER_REPORT:
            case DMAC_WLAN_CRX_SCAN_RESULT:
                crx_event->netbuf = netbuf;
                break;
            default:
                hi_diag_log_msg_e1(0, "hmac_from_dmac_rx_data_handle:: invalid CRX sub-type:%d.", event_hdr->sub_type);
                return HI_FAIL;
        }

        sub_table_item = g_ast_hmac_wlan_crx_event_sub_table;
    } else if (event_hdr->type == FRW_EVENT_TYPE_WLAN_DRX) {
        switch (event_hdr->sub_type) {
            case DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_AP:
            case DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_STA:
                wlan_rx_event->netbuf = (oal_dev_netbuf_stru *)netbuf;
                wlan_rx_event->us_netbuf_num = netbuf_num;
                break;
            default:
                hi_diag_log_msg_e1(0, "hmac_from_dmac_rx_data_handle:: invalid DRX sub-type:%d.", event_hdr->sub_type);
                return HI_FAIL;
        }
        sub_table_item = g_ast_hmac_wlan_drx_event_sub_table;
    }

    if (sub_table_item == HI_NULL) {
        oam_warning_log0(0, 0, "hmac_from_dmac_rx_data_handle:pst_sub_table_item is null!");
        return HI_FAIL;
    }
    return sub_table_item[event_hdr->sub_type].func(event_mem);
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : hmac_proc_query_response_event
 功能描述  : hmac接收dmac抛回来的查询应答事件
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_proc_query_response_event(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    dmac_query_station_info_response_event *query_station_reponse_event = HI_NULL;

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_query_response::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    query_station_reponse_event = (dmac_query_station_info_response_event *)(puc_param);
    if (query_station_reponse_event->query_event == OAL_QUERY_STATION_INFO_EVENT) {
        hmac_vap->station_info.signal     =  (hi_s8)query_station_reponse_event->signal;
        hmac_vap->station_info.rx_packets =  query_station_reponse_event->rx_packets;
        hmac_vap->station_info.tx_packets =  query_station_reponse_event->tx_packets;
        hmac_vap->station_info.rx_bytes   =  query_station_reponse_event->rx_bytes;
        hmac_vap->station_info.tx_bytes   =  query_station_reponse_event->tx_bytes;
        hmac_vap->station_info.tx_retries =  query_station_reponse_event->tx_retries;
        hmac_vap->station_info.rx_dropped_misc =
            hmac_vap->station_info.rx_dropped_misc + query_station_reponse_event->rx_dropped_misc;
        hmac_vap->station_info.tx_failed       =  query_station_reponse_event->tx_failed;
        hmac_vap->station_info.txrate          =  query_station_reponse_event->txrate;
    }

    /* 唤醒wal_sdt_recv_reg_cmd等待的进程 */
    hmac_vap->query_wait_q_flag = HI_TRUE;
    hi_wait_queue_wake_up_interrupt(&(hmac_vap->query_wait_q));

    return HI_SUCCESS;
}

#endif
#endif

/* ****************************************************************************
 功能描述  : hmac 抛事件到 wal
**************************************************************************** */
hi_u32 hmac_send_event_to_host(const mac_vap_stru *mac_vap, const hi_u8 *param, hi_u16 len, hi_u8 sub_type)
{
    frw_event_mem_stru      *event_mem = HI_NULL;
    frw_event_stru          *event = HI_NULL;
    hi_u32                   ret;

    /* 抛事件到WAL */
    event_mem = frw_event_alloc(len);
    if ((event_mem == HI_NULL) || (event_mem->puc_data == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_send_event_to_host::event_mem null.}");
        return HI_FAIL;
    }

    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;

    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CTX, sub_type, len, FRW_EVENT_PIPELINE_STAGE_0,
        mac_vap->vap_id);
    if (param != HI_NULL) {
        if (memcpy_s(event->auc_event_data, len, param, len) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "hmac_send_event_to_host::pst_peer_param memcpy fail.");
            frw_event_free(event_mem);
            return HI_FAIL;
        }
    }
    /* 分发事件 */
    ret = frw_event_dispatch_event(event_mem);
    frw_event_free(event_mem);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
