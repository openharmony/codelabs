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
#ifndef __WAL_EVENT_H__
#define __WAL_EVENT_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "wal_main.h"
#include "hmac_ext_if.h"
#include "hi_wifi_api.h"
#ifdef _PRE_WLAN_FEATURE_CSI
#include "hi_wifi_csi_api.h"
#endif
#ifdef _PRE_WLAN_FEATURE_ANY
#include "hi_any_api.h"
#endif

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
/* 驱动sta上报内核的扫描结果 */
typedef struct {
    hi_s32               l_signal;      /* 信号强度 */

    hi_s16               s_freq;        /* bss所在信道的中心频率 */
    hi_u8               auc_arry[2];   /* 2 数组元素个数 */

    hi_u32                mgmt_len;   /* 管理帧长度 */
    oal_ieee80211_mgmt_stru  *mgmt;      /* 管理帧起始地址 */
} wal_scanned_bss_info_stru;


/* 驱动sta上报内核的去关联结果 */
typedef struct {
    hi_u16  us_reason_code;         /* 去关联 reason code */
    hi_u8   auc_resv[2];            /* 2 保留字节 */

    hi_u8  *pus_disconn_ie;        /* 去关联关联帧 ie */
    hi_u32  us_disconn_ie_len;      /* 去关联关联帧 ie 长度 */
} oal_disconnect_result_stru;

#ifdef _PRE_WLAN_FEATURE_MESH
/* 驱动MESH上报WPA去关联 */
typedef struct {
    hi_u16  us_reason_code;         /* 去关联 reason code */
    hi_u8   auc_resv[2];            /* 2 保留字节 */

    hi_u8  *pus_disconn_ie;        /* 去关联关联帧 ie */
    hi_u32  us_disconn_ie_len;      /* 去关联关联帧 ie 长度 */
} oal_mesh_close_peer_stru;
#endif
/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 wal_scan_comp_proc_sta(frw_event_mem_stru *event_mem);
hi_u32 wal_asoc_comp_proc_sta(frw_event_mem_stru *event_mem);
hi_u32 wal_disasoc_comp_proc_sta(frw_event_mem_stru *event_mem);
hi_u32 wal_connect_new_sta_proc_ap(frw_event_mem_stru *event_mem);
hi_u32 wal_disconnect_sta_proc_ap(frw_event_mem_stru *event_mem);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 wal_mic_failure_proc(frw_event_mem_stru *event_mem);
#endif
hi_u32 wal_send_mgmt_to_host(frw_event_mem_stru *event_mem);
#ifdef _PRE_WLAN_FEATURE_P2P
hi_u32 wal_p2p_listen_timeout(frw_event_mem_stru *event_mem);
#endif
#ifdef _PRE_WLAN_FEATURE_ANY
hi_u32 wal_any_process_rx_data(frw_event_mem_stru *event_mem);
hi_u32 wal_any_process_tx_complete(frw_event_mem_stru *event_mem);
hi_u32 wal_any_process_scan_result(frw_event_mem_stru *event_mem);
hi_u32 wal_any_process_peer_info(frw_event_mem_stru *event_mem);
hi_u32 wal_any_wait_query_result(hmac_vap_stru *hmac_vap, hi_wifi_any_peer_info *peer);
hi_s32 wal_any_global_config(wlan_cfgid_enum_uint16 wid, oal_net_device_stru *netdev);
hi_void wal_any_set_callback(hi_wifi_any_send_complete_cb send_cb, hi_wifi_any_recv_cb recv_cb);
hi_void wal_any_set_scan_callback(hi_wifi_any_scan_result_cb cb);
#endif
hi_void wal_set_tpc_mode(oal_net_device_stru *netdev, hi_u32 mode);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 wal_mesh_close_peer_inform(frw_event_mem_stru *event_mem);
hi_u32 wal_mesh_new_peer_candidate(frw_event_mem_stru *event_mem);
#endif

#ifdef _PRE_WLAN_FEATURE_CSI
hi_void wal_csi_register_data_report_cb(hi_wifi_csi_data_cb func);
hi_u32 wal_csi_data_report(frw_event_mem_stru *event_mem);
hi_s32 wal_csi_switch(hi_u8 switch_flag);
hi_s32 wal_csi_set_config(const hi_char *ifname, hi_u32 report_min_interval, const hi_wifi_csi_entry *entry_list,
    hi_s32 entry_num);
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
hi_u32 wal_p2p_action_tx_status(frw_event_mem_stru *event_mem);
#endif
hi_u32 wal_get_efuse_mac_from_dev(frw_event_mem_stru *event_mem);
#ifdef FEATURE_DAQ
hi_u32 wal_data_acq_status(frw_event_mem_stru *event_mem);
hi_u32 wal_data_acq_result(frw_event_mem_stru *event_mem);
#endif
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
hi_u32 wal_channel_switch_report(frw_event_mem_stru *event_mem);
#endif
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
hi_u32 wal_get_dbg_cal_data_from_dev(frw_event_mem_stru *event_mem);
#endif

hi_u32 wal_report_tx_params(frw_event_mem_stru *event_mem);
hi_void wal_register_tx_params_callback(hi_wifi_report_tx_params_callback func);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wal_event.h */
