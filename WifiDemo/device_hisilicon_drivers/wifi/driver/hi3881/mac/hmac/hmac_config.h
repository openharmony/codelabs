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

#ifndef __HMAC_CONFIG_H__
#define __HMAC_CONFIG_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "mac_vap.h"
#include "mac_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
typedef hi_u32 (*wal_config_get_func)(mac_vap_stru *pst_mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
typedef hi_u32 (*wal_config_set_func)(mac_vap_stru *pst_mac_vap, hi_u16 us_len, const hi_u8 *puc_param);

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* HOST CRX事件子类型 */
typedef enum {
    WAL_HOST_CRX_SUBTYPE_CFG,

    WAL_HOST_CRX_SUBTYPE_BUTT
} wal_host_crx_subtype_enum;
typedef hi_u8 wal_host_crx_subtype_enum_uint8;

/* HOST DRX事件子类型 */
typedef enum {
    WAL_HOST_DRX_SUBTYPE_TX,

    WAL_HOST_DRX_SUBTYPE_BUTT
} wal_host_drx_subtype_enum;
typedef hi_u8 wal_host_drx_subtype_enum_uint8;

/* ****************************************************************************
  4 STRUCT定义
**************************************************************************** */
/* WID对应的操作 */
typedef struct {
    wlan_cfgid_enum_uint16  cfgid;              /* wid枚举 */
    hi_u8                   reset;              /* 是否复位 */
    hi_u8                   auc_resv[1];
    wal_config_get_func     get_func;            /* get函数 */
    wal_config_set_func     set_func;            /* set函数 */
} wal_wid_op_stru;

/* hmac_vap结构中，一部分成员的大小，这些成员在linux和windows下的定义可能不同 */
typedef struct {
    hi_u32 hmac_vap_cfg_priv_stru_size;
    hi_u32 frw_timeout_stru_size;
    hi_u32 mac_key_mgmt_stru_size;
    hi_u32 mac_pmkid_cache_stru_size;
    hi_u32 mac_curr_rateset_stru_size;
    hi_u32 hmac_vap_stru_size;
} hmac_vap_member_size_stru;

#define REG_INFO_MAX_NUM 20
typedef struct {
    hi_u32 reg_info_num;
    hi_u32 flag;
    hi_u32 val[REG_INFO_MAX_NUM];
} hmac_reg_info_receive_event;

#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct {
    hi_u8 auc_mac_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 bcn_prio;
    hi_u8 is_mbr;
    hi_u8 accept_sta;
    hi_u8 link_num;
    hi_u8 rssi;
    hi_u8 rsv;
} hmac_report_new_peer_candidate_stru;
#endif

typedef struct {
    hi_u32 send_bps;  // bit per second
    hi_u32 avg_retry; // 千分制
} hamc_config_report_tx_params_stru;

/* ****************************************************************************
  5 函数声明
**************************************************************************** */
hi_u32 hmac_config_sync_cmd_common(const mac_vap_stru *mac_vap, wlan_cfgid_enum_uint16 cfg_id, hi_u16 us_len,
    const hi_u8 *puc_param);
hi_u32 hmac_config_frag_threshold(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_user_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_wow(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_start_sched_scan(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_stop_sched_scan(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_set_country(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_rts_threshold(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_mib(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_mib(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_set_thruput_bypass(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_sta_pm_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_pm_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_enable_arp_offload(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_config_show_arpoffload_info(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_auto_freq_enable(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_linkloss_threshold(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);

hi_u32 hmac_config_set_pm_cfg_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_cus_rf(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_cus_dts_cali(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_cus_nvram_params(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);

hi_u32 hmac_config_set_sta_pm_on(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_add_vap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_del_vap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_start_vap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_down_vap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_mode(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_freq(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_freq(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_mac_addr(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_concurrent(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_config_get_concurrent(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_ssid(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_ssid(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_shortgi20(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_shortgi20(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_config_set_shpreamble(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_shpreamble(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_prot_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_prot_mode(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_auth_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_auth_mode(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_bintval(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_bintval(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
#endif
hi_u32 hmac_config_get_txpower(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_txpower(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_dtimperiod(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_dtimperiod(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_config_set_wmm_params(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_vap_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_random_mac_addr_scan(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_start_dpd(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_ampdu_start(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif

hi_u32 hmac_config_addba_req(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_delba_req(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_list_sta(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_config_kick_user(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_ampdu_tx_on(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_ampdu_tx_on_from_dmac(mac_vap_stru *mac_vap, hi_u8 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_send_bar(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_amsdu_ampdu_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_get_country(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_country_for_dfs(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_config_get_tid(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_ALG_CFG
hi_u32 hmac_config_alg(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_set_forty_mhz_intolerant(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_rx_fcs_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
hi_u32 hmac_config_list_channel(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif

hi_u32 hmac_config_user_info_syn(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user);
hi_u32 hmac_config_set_dscr_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_rate(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_mcs(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_bw(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_bw(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_always_tx(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_CSI
hi_u32 hmac_config_csi_set_config(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_csi_set_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_csi_data_report_event(frw_event_mem_stru *event_mem);
#endif
hi_u32 hmac_config_always_rx(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_reg_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_host_sleep_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_wow_pattern(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_wow_activate_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_set_monitor_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_reg_write(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_scan_abort(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_connect(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_channel(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_beacon(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_alg_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_edca_opt_switch_sta(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_edca_opt_switch_ap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_set_edca_opt_cycle_ap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);

hi_u32 hmac_config_set_wps_ie(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);

#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_ampdu_aggr_num(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_freq_adjust(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_stbc_cap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_config_query_station_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_set_uapsd_para(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_bcast_pkt(mac_vap_stru *mac_vap, hi_u32 payload_len);
oal_netbuf_stru *hmac_config_create_al_tx_packet(hi_u32 size, mac_rf_payload_enum_uint8 payload_flag, hi_u8 init_flag);
hi_u32 _hmac_config_always_tx(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_app_ie_to_vap(mac_vap_stru *mac_vap, oal_app_ie_stru *app_ie, en_app_ie_type_uint8 type);
hi_u32 hmac_event_config_syn(frw_event_mem_stru *event_mem);
hi_u32 hmac_protection_info_sync_event(frw_event_mem_stru *event_mem);

#ifdef _PRE_WLAN_FEATURE_ANY
hi_u32 hmac_any_send_action(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_any_init(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_any_deinit(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_any_start_scan(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_any_query_peer_by_mac(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param);
hi_u32 hmac_any_query_peer_by_index(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_config_get_wmm_params(mac_vap_stru *mac_vap, hi_u16 *us_len, hi_u8 *puc_param);
#endif
hi_void hmac_event_fsm_table_register(hi_void);

hi_u32 hmac_config_open_wmm(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_wow_set_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);

#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
hi_u32 hmac_config_arp_offload_setting(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_arp_offload_show_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_DHCP_OFFLOAD
hi_u32 hmac_config_dhcp_offload_setting(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_set_retry_limit(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 hmac_config_set_mesh_user(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_send_mesh_action(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_report_times_limit(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_report_cnt_limit(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_get_meshid(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
hi_u32 hmac_config_set_meshid(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_beacon_priority(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_mnid(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_en_mbr(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_add_mesh_user(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *puc_param);
hi_u32 hmac_config_new_peer_candidate_en(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_mesh_accept_sta(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_mesh_user_gtk(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_accept_peer(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_new_peer_candidate_event(const mac_vap_stru *mac_vap, oal_netbuf_stru *netbuf, hi_u32 payload_len);
hi_u32 hmac_config_get_mesh_nodeinfo(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
hi_u32 hmac_config_set_rekey_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_config_query_rssi(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_send_custom_pkt(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
oal_net_dev_tx_enum hmac_bridge_vap_xmit(oal_net_device_stru *netdev, oal_netbuf_stru *netbuf);
hi_u32 hmac_from_dmac_rx_control_handle(frw_event_mem_stru *event_mem);
hi_u32 hmac_from_dmac_rx_data_handle(frw_event_mem_stru *event_mem, oal_netbuf_stru *netbuf, hi_u16 netbuf_num);
#ifdef _PRE_WLAN_FEATURE_BTCOEX
hi_u32 hmac_config_set_btcoex_en(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_btcoex_rx_delba_trigger(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
#endif
#ifdef FEATURE_DAQ
hi_u32 hmac_config_data_acq_start(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_data_acq_status(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_get_data_acq_status(mac_vap_stru *mac_vap, hi_u8 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_data_acq_result(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_get_data_acq_result(mac_vap_stru *mac_vap, hi_u8 us_len, const hi_u8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_BW_HIEX
hi_u32 hmac_config_hiex_set_selfcts(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 wal_config_process_pkt(frw_event_mem_stru *event_mem);
hi_u32 hmac_protection_update_from_user(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_pm_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_proc_dev_sleep_req(const frw_event_mem_stru *event_mem);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_psm_offset(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *param);
hi_u32 hmac_config_set_sta_hw_ps_mode(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *param);
#endif
hi_u32 hmac_config_start_vap_event(const mac_vap_stru *mac_vap, hi_u8 mgmt_rate_init_flag);
hi_u32 hmac_set_mode_event(const mac_vap_stru *mac_vap);
hi_u32 hmac_config_user_cap_syn(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user);
hi_u32 hmac_init_user_security_port(const mac_vap_stru *mac_vap, mac_user_stru *mac_user);
hi_u32 hmac_config_vap_state_syn(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_bool hmac_get_wlan_pm_switch(hi_void);
hi_void hmac_set_wlan_pm_switch(hi_bool wlan_pm_switch);
hi_u32 hmac_get_rx_fcs_info(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param);
hi_u32 hmac_proc_query_rssi_response(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
hi_u32 hmac_config_sta_update_rates(mac_vap_stru *mac_vap, const mac_cfg_mode_param_stru *cfg_mode);
hi_u32 hmac_cfg_vap_send_event(const mac_device_stru *device);
#ifdef _PRE_WLAN_RF_110X_CALI_DPD
hi_u32 hmac_dpd_cali_data_recv(const frw_event_mem_stru *event_mem);
#endif
#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
hi_u32 hmac_config_set_tx_classify_switch(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif

hi_u32 hmac_set_psm_timeout(hi_void *puc_para);
hi_u32 hmac_config_set_cal_band_power(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_cal_rate_power(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_cal_freq(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_efuse_mac(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_efuse_mac(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_dataefuse(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_cal_data(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_cus_fcc_tx_params(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_cca_th(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_user_set_asoc_state(const mac_vap_stru *mac_vap, mac_user_stru *mac_user,
    mac_user_asoc_state_enum_uint8 value);
hi_u32 hmac_config_set_tx_pwr_offset(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_set_freq_comp(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
hi_u32 hmac_config_set_obss_scan_param(const mac_vap_stru *mac_vap);
hi_u32 hmac_config_user_rate_info_syn(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user);
hi_u32 hmac_config_user_asoc_state_syn(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user);
hi_u32 hmac_config_sta_vap_info_syn(const mac_vap_stru *mac_vap);
hi_u32 hmac_syn_info_event(frw_event_mem_stru *event_mem);
hi_u32 hmac_voice_aggr_event(frw_event_mem_stru *event_mem);
hi_u32 hmac_config_cfg_vap_h2d(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_get_thruput_info(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
hi_u32 hmac_config_get_efuse_mac_addr(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_report_mac_from_efuse(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_report_vap_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_wifitest_get_reg_info(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
hi_u32 hmac_report_mfg_test(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param);
hi_u32 hmac_report_dbg_cal_data_from_dev(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param);
#endif
#endif
#endif
#if defined(_PRE_WLAN_FEATURE_HIPRIV) && defined(_PRE_WLAN_FEATURE_INTRF_MODE)
hi_u32 hmac_config_set_intrf_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
#endif
hi_u32 hmac_send_rx_filter_event(const mac_vap_stru *mac_vap, hi_u32 rx_filter_val);

hi_u32 hmac_config_notify_get_tx_params(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_u32 hmac_config_report_tx_params(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param);
hi_u32 hmac_config_minimize_boot_current(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_CONFIG_H__ */
