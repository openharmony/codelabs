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
#include "oal_net.h"
#include "oal_cfg80211.h"
#include "hdf_wifi_event.h"
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "wal_cfg80211_apt.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
static oal_kobj_uevent_env_stru g_env;
#endif

/* ****************************************************************************
 功能描述  : 上报调度扫描结果
**************************************************************************** */
hi_void oal_cfg80211_sched_scan_result(oal_wiphy_stru *pst_wiphy)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    return cfg80211_sched_scan_results(pst_wiphy);
#else
    hi_unref_param(pst_wiphy);
#endif
}

/* ****************************************************************************
 功能描述  : 上报linux 内核已经处于指定信道
**************************************************************************** */
hi_void oal_cfg80211_ready_on_channel(
#if (LINUX_VERSION_CODE >= kernel_version(3, 10, 44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    oal_wireless_dev *wdev,
#else
    oal_net_device_stru *net_dev,
#endif
    hi_u64 cookie, oal_ieee80211_channel_stru *chan, hi_u32 duration, oal_gfp_enum_uint8 en_gfp)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
#if (LINUX_VERSION_CODE >= kernel_version(3, 10, 44))
    cfg80211_ready_on_channel(wdev, cookie, chan, duration, en_gfp);
#else
    enum nl80211_channel_type en_channel_type;
    en_channel_type = NL80211_CHAN_HT20;
    cfg80211_ready_on_channel(pst_net_dev, cookie, chan, en_channel_type, duration, en_gfp);
#endif
#else
    hi_unref_param(wdev);
    hi_unref_param(cookie);
    hi_unref_param(chan);
    hi_unref_param(duration);
    hi_unref_param(en_gfp);
#endif
}

hi_void oal_kobject_uevent_env_sta_join(oal_net_device_stru *net_device, const hi_u8 *mac_addr)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    memset_s(&g_env, sizeof(g_env), 0, sizeof(g_env));
    add_uevent_var(&g_env, "SOFTAP=STA_JOIN wlan0 wlan0 %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1],
        mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]); /* mac addr 0:1:2:3:4:5 */
#if (LINUX_VERSION_CODE >= kernel_version(4, 1, 0))
    kobject_uevent_env(&(net_device->dev.kobj), KOBJ_CHANGE, g_env.envp);
#else
    kobject_uevent_env(&(net_device->dev.kobj), KOBJ_CHANGE, (char **)&g_env);
#endif
#else
    hi_unref_param(net_device);
    hi_unref_param(mac_addr);
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */
}

hi_void oal_kobject_uevent_env_sta_leave(oal_net_device_stru *net_device, const hi_u8 *mac_addr)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    memset_s(&g_env, sizeof(g_env), 0, sizeof(g_env));
    add_uevent_var(&g_env, "SOFTAP=STA_LEAVE wlan0 wlan0 %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1],
        mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]); /* mac addr 0:1:2:3:4:5 */
    kobject_uevent_env(&(net_device->dev.kobj), KOBJ_CHANGE, g_env.envp);
#else
    hi_unref_param(net_device);
    hi_unref_param(mac_addr);
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */
}

hi_void oal_cfg80211_mgmt_tx_status(
#if (LINUX_VERSION_CODE >= kernel_version(3, 10, 44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    oal_wireless_dev *wdev,
#else
    oal_net_device_stru *pst_net_dev,
#endif
    hi_u64 cookie, const hi_u8 *buf, size_t len, hi_u8 ack, oal_gfp_enum_uint8 gfp)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    hi_unref_param(cookie);
    hi_unref_param(gfp);
    HdfWifiEventMgmtTxStatus(wdev->netdev, buf, len, ack);
#elif (LINUX_VERSION_CODE >= kernel_version(3, 10, 44))
    cfg80211_mgmt_tx_status(wdev, cookie, buf, len, ack, gfp);
#else /* linux vs 3.4.5 */
    cfg80211_mgmt_tx_status(pst_net_dev, cookie, buf, len, ack, gfp);
#endif
}

/* ****************************************************************************
 功能描述  : 上报扫描完成结果
**************************************************************************** */
hi_void oal_cfg80211_scan_done(oal_cfg80211_scan_request_stru *pst_cfg80211_scan_request, hi_s8 c_aborted)
{
    (void)pst_cfg80211_scan_request;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
#if (LINUX_VERSION_CODE >= kernel_version(4, 8, 0))
    oal_cfg80211_scan_info_stru scan_info = {
        .aborted = c_aborted
    };
    cfg80211_scan_done(pst_cfg80211_scan_request, &scan_info);
#endif
#else /* (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX) */
    (void)c_aborted;
#endif
}

/* ****************************************************************************
 功能描述  : STA上报给关联结果结构体
**************************************************************************** */
hi_u32 oal_cfg80211_connect_result(oal_net_device_stru *net_device, const oal_connet_result_stru *connect_result)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    cfg80211_connect_result(net_device, connect_result->auc_bssid, connect_result->puc_req_ie,
                            connect_result->req_ie_len, connect_result->puc_rsp_ie, connect_result->rsp_ie_len,
                            connect_result->us_status_code, GFP_ATOMIC);
#else
    
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : STA上报给内核去关联结果
**************************************************************************** */
hi_u32 oal_cfg80211_disconnected(oal_net_device_stru *pst_net_device, hi_u16 us_reason, const hi_u8 *puc_ie,
    hi_u32 ul_ie_len, hi_bool locally_generated)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    cfg80211_disconnected(pst_net_device, us_reason, puc_ie, ul_ie_len, locally_generated, GFP_ATOMIC);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    HdfWifiEventDisconnected(pst_net_device, us_reason, puc_ie, ul_ie_len);
    hi_unref_param(locally_generated);
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : AP上报去关联某个STA情况
**************************************************************************** */
hi_u32 oal_cfg80211_del_sta(oal_net_device_stru *net_device, const hi_u8 *mac_addr, hi_u8 addr_len,
    oal_gfp_enum_uint8 en_gfp)
{
    hi_unref_param(addr_len);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    cfg80211_del_sta(net_device, mac_addr, en_gfp);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    HdfWifiEventDelSta(net_device, mac_addr, WLAN_MAC_ADDR_LEN);
    hi_unref_param(en_gfp);
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : AP上报新关联某个STA情况
**************************************************************************** */
hi_u32 oal_cfg80211_new_sta(oal_net_device_stru *net_device, const hi_u8 *mac_addr, hi_u8 addr_len,
    oal_station_info_stru *station_info, oal_gfp_enum_uint8 en_gfp)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    cfg80211_new_sta(net_device, mac_addr, station_info, en_gfp);
    hi_unref_param(addr_len);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    struct StationInfo info = { 0 };
    info.assocReqIes = station_info->assoc_req_ies;
    info.assocReqIesLen = station_info->assoc_req_ies_len;
    HdfWifiEventNewSta(net_device, mac_addr, WLAN_MAC_ADDR_LEN, &info);
    hi_unref_param(en_gfp);
    hi_unref_param(addr_len);
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 上报mic攻击
**************************************************************************** */
hi_void oal_cfg80211_mic_failure(oal_net_device_stru *net_device, const hi_u8 *mac_addr,
    enum nl80211_key_type key_type, hi_s32 key_id, const hi_u8 *tsc, oal_gfp_enum_uint8 en_gfp)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    cfg80211_michael_mic_failure(net_device, mac_addr, key_type, key_id, tsc, en_gfp);
#else
    hi_unref_param(net_device);
    hi_unref_param(mac_addr);
    hi_unref_param(key_type);
    hi_unref_param(key_id);
    hi_unref_param(tsc);
    hi_unref_param(en_gfp);
#endif
}

/* ****************************************************************************
 功能描述  : 上报接收到的管理帧
**************************************************************************** */
hi_u32 oal_cfg80211_rx_mgmt(oal_net_device_stru *pst_dev, hi_s32 l_freq, const hi_u8 *puc_buf, hi_u32 ul_len,
    oal_gfp_enum_uint8 en_gfp)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    oal_wireless_dev *pst_wdev = HI_NULL;
    pst_wdev = pst_dev->ieee80211Ptr;

    cfg80211_rx_mgmt(pst_wdev, l_freq, 0, puc_buf, ul_len, en_gfp);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    HdfWifiEventRxMgmt(pst_dev, l_freq, 0, puc_buf, (size_t)ul_len);
    hi_unref_param(en_gfp);
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
上报内核bss信息
**************************************************************************** */
oal_cfg80211_bss_stru *oal_cfg80211_inform_bss_frame(oal_wiphy_stru *pst_wiphy,
    oal_ieee80211_channel_stru *pst_ieee80211_channel, oal_ieee80211_mgmt_stru *pst_mgmt, hi_u32 ul_len,
    hi_s32 l_signal, oal_gfp_enum_uint8 en_ftp)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    return cfg80211_inform_bss_frame(pst_wiphy, pst_ieee80211_channel, pst_mgmt, ul_len, l_signal, en_ftp);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    hi_unref_param(pst_wiphy);
    hi_unref_param(pst_ieee80211_channel);
    hi_unref_param(pst_mgmt);
    hi_unref_param(ul_len);
    hi_unref_param(l_signal);
    hi_unref_param(en_ftp);
    return NULL;
#endif
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
hi_void oal_cfg80211_put_bss(oal_wiphy_stru *pst_wiphy, oal_cfg80211_bss_stru *pst_cfg80211_bss)
{
    cfg80211_put_bss(pst_wiphy, pst_cfg80211_bss);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
