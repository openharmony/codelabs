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

#ifndef __WAL_CFG80211_APT_H__
#define __WAL_CFG80211_APT_H__

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#ifndef HAVE_PCLINT_CHECK
#include "hi_wifi_driver_wpa_if.h"
#endif
#include "oam_ext_if.h"
#include "oal_ext_if.h"
#include "wal_event.h"
#include "oal_cfg80211.h"

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
typedef hi_s32 (*hisi_upload_frame_cb)(hi_void *recv_buf, hi_s32 frame_len, hi_s8 rssi);

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 cfg80211_new_sta(const oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len,
    const oal_station_info_stru *sinfo);
hi_u32 cfg80211_del_sta(oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len);
hi_u8 cfg80211_mgmt_tx_status(struct wireless_dev *wdev, const hi_u8 *puc_buf, size_t len, hi_u8 ack);
hi_void cfg80211_inform_bss_frame(const oal_net_device_stru *netdev, oal_wiphy_stru *wiphy,
    const oal_ieee80211_channel_stru *ieee80211_channel, const wal_scanned_bss_info_stru *scanned_bss_info);
hi_void cfg80211_connect_result(const oal_net_device_stru *netdev, const oal_connet_result_stru *connet_result);
hi_u32 cfg80211_disconnected(const oal_net_device_stru *netdev, hi_u16 us_reason, const hi_u8 *puc_ie, hi_u32 ie_len);
hi_u32 cfg80211_timeout_disconnected(const oal_net_device_stru *netdev);
hi_void cfg80211_scan_done(const oal_net_device_stru *netdev, hisi_scan_status_enum status);
hi_u8 cfg80211_rx_mgmt(const oal_net_device_stru *netdev, hi_s32 freq, hi_s32 l_sig_mbm, const hi_u8 *puc_buf,
    size_t len);
hi_u32 cfg80211_csa_channel_switch(const oal_net_device_stru *netdev, hi_s32 freq);
hi_u32 cfg80211_remain_on_channel(const oal_net_device_stru *netdev, hi_u32 freq, hi_u32 duration);
hi_u32 cfg80211_cancel_remain_on_channel(const oal_net_device_stru *netdev, hi_u32 freq);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 cfg80211_rx_mgmt_ext(const oal_net_device_stru *netdev, hi_s32 freq, const hi_u8 *puc_buf, hi_u32 len);
hi_void cfg80211_rx_exception(const oal_net_device_stru *netdev, hi_u8 *puc_data, hi_u32 data_len);
hi_u32 cfg80211_mic_failure(const oal_net_device_stru *netdev, const hmac_mic_event_stru *mic_event,
    const hi_u8 *puc_tsc, oal_gfp_enum_uint8 gfp);
hi_void cfg80211_kobject_uevent_env_sta_join(oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len);
hi_void cfg80211_kobject_uevent_env_sta_leave(oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len);
#endif

#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 cfg80211_mesh_close(const oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len, hi_u16 us_reason);
hi_u32 cfg80211_mesh_new_peer_candidate(const oal_net_device_stru *netdev,
    const hmac_report_new_peer_candidate_stru *puc_new_peer);
#endif

#ifdef _PRE_WLAN_FEATURE_PROMIS
hi_u32 hisi_wlan_register_upload_frame_cb(hisi_upload_frame_cb func);
hi_u32 hwal_send_others_bss_data(const oal_netbuf_stru *netbuf);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */
#endif /* __WAL_CFG80211_APT_H__ */
