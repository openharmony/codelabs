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

#ifndef __OAL_CFG80211_H__
#define __OAL_CFG80211_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
#ifndef HAVE_PCLINT_CHECK
#include <net/cfg80211.h>
#endif
#include <net/genetlink.h>
#include <net/cfg80211.h>
#include <linux/nl80211.h>
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "oal_ext_if.h"
#include "los_typedef.h"
#endif
#include "oal_net.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 结构体定义
**************************************************************************** */
/* 驱动sta上报内核的关联结果 */
typedef struct {
    hi_u8  auc_bssid[WLAN_MAC_ADDR_LEN]; /* sta关联的ap mac地址 */
    hi_u16 us_status_code;              /* ieee协议规定的16位状态码 */

    hi_u8 *puc_rsp_ie; /* asoc_req_ie  */
    hi_u8 *puc_req_ie;

    hi_u32 req_ie_len; /* asoc_req_ie len */
    hi_u32 rsp_ie_len;

    hi_u16 us_connect_status;
    hi_u16 us_freq;
} oal_connet_result_stru;

typedef struct cfg80211_scan_info oal_cfg80211_scan_info_stru;

/* ****************************************************************************
  3 函数声明
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_void oal_cfg80211_ready_on_channel(
#if (LINUX_VERSION_CODE >= kernel_version(3, 10, 44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    oal_wireless_dev *wdev,
#else
    oal_net_device_stru *net_dev,
#endif
    hi_u64 ull_cookie, oal_ieee80211_channel_stru *chan, hi_u32 duration, oal_gfp_enum_uint8 en_gfp);
hi_void oal_cfg80211_mgmt_tx_status(
#if (LINUX_VERSION_CODE >= kernel_version(3, 10, 44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    oal_wireless_dev *wdev,
#else
    oal_net_device_stru *pst_net_dev,
#endif
    hi_u64 cookie, const hi_u8 *buf, size_t len, hi_u8 ack, oal_gfp_enum_uint8 gfp);
hi_void oal_cfg80211_sched_scan_result(oal_wiphy_stru *pst_wiphy);
hi_void oal_kobject_uevent_env_sta_join(oal_net_device_stru *net_device, const hi_u8 *mac_addr);
hi_void oal_kobject_uevent_env_sta_leave(oal_net_device_stru *net_device, const hi_u8 *mac_addr);
hi_void oal_cfg80211_mic_failure(oal_net_device_stru *net_device, const hi_u8 *mac_addr,
    enum nl80211_key_type key_type, hi_s32 key_id, const hi_u8 *tsc, oal_gfp_enum_uint8 en_gfp);
hi_u32 oal_cfg80211_rx_mgmt(oal_net_device_stru *pst_dev, hi_s32 l_freq, const hi_u8 *puc_buf, hi_u32 ul_len,
    oal_gfp_enum_uint8 en_gfp);
oal_cfg80211_bss_stru *oal_cfg80211_inform_bss_frame(oal_wiphy_stru *pst_wiphy,
    oal_ieee80211_channel_stru *pst_ieee80211_channel, oal_ieee80211_mgmt_stru *pst_mgmt, hi_u32 ul_len,
    hi_s32 l_signal, oal_gfp_enum_uint8 en_ftp);
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

#ifndef _PRE_HDF_LINUX
hi_void oal_cfg80211_put_bss(oal_wiphy_stru *pst_wiphy, oal_cfg80211_bss_stru *pst_cfg80211_bss);
#endif
hi_void oal_cfg80211_scan_done(oal_cfg80211_scan_request_stru *pst_cfg80211_scan_request, hi_s8 c_aborted);
hi_u32 oal_cfg80211_new_sta(oal_net_device_stru *net_device, const hi_u8 *mac_addr, hi_u8 addr_len,
    oal_station_info_stru *station_info, oal_gfp_enum_uint8 en_gfp);
hi_u32 oal_cfg80211_del_sta(oal_net_device_stru *net_device, const hi_u8 *mac_addr, hi_u8 addr_len,
    oal_gfp_enum_uint8 en_gfp);
hi_u32 oal_cfg80211_connect_result(oal_net_device_stru *net_device, const oal_connet_result_stru *connect_result);
hi_u32 oal_cfg80211_disconnected(oal_net_device_stru *pst_net_device, hi_u16 us_reason, const hi_u8 *puc_ie,
    hi_u32 ul_ie_len, hi_bool locally_generated);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_main */
