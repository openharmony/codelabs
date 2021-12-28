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
#include "wal_scan.h"
#include "wal_cfg80211.h"
#include "wal_main.h"
#include "wal_event.h"
#include "hmac_ext_if.h"
#include "frw_timer.h"
#include "wal_cfg80211_apt.h"
#include "hdf_wifi_event.h"
#include "hi_task.h"
#include "hi_wifi_driver_wpa_if.h"
#include "osal_mem.h"

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifndef _PRE_HDF_LINUX
#include <net/cfg80211.h>
#endif
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifndef U64
#define U64 UINT64
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
hi_void wal_free_scan_mgmt_resource(hmac_scan_stru *scan_mgmt)
{
    if (scan_mgmt->request->ssids != HI_NULL) {
        OsalMemFree(scan_mgmt->request->ssids);
        scan_mgmt->request->ssids = HI_NULL;
    }
    if (scan_mgmt->request->ie != HI_NULL) {
        OsalMemFree(scan_mgmt->request->ie);
        scan_mgmt->request->ie = HI_NULL;
    }
    OsalMemFree(scan_mgmt->request);
    scan_mgmt->request = HI_NULL;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_inform_bss_frame
 功能描述  : 逐个上报ssid消息给内核
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void wal_inform_bss_frame(const oal_net_device_stru *netdev, wal_scanned_bss_info_stru *scanned_bss_info,
    hi_void *data)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_cfg80211_bss_stru        *cfg80211_bss = HI_NULL;
#endif
    oal_wiphy_stru               *wiphy = HI_NULL;
    oal_ieee80211_channel_stru   *ieee80211_channel = HI_NULL;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0))
    struct timespec ts;
#endif

    if ((scanned_bss_info == HI_NULL) || (data == HI_NULL)) {
        oam_error_log2(0, OAM_SF_SCAN,
            "{wal_inform_bss_frame::input param pointer is null, pst_scanned_bss_info[%p], p_data[%p]!}",
            (uintptr_t)scanned_bss_info, (uintptr_t)data);
        return;
    }

    wiphy = (oal_wiphy_stru *)data;

    ieee80211_channel = oal_ieee80211_get_channel(wiphy, (hi_s32)scanned_bss_info->s_freq);
    if (ieee80211_channel == HI_NULL) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_inform_bss_frame::get channel failed, wrong s_freq[%d]}",
            (hi_s32)scanned_bss_info->s_freq);
        return;
    }

    scanned_bss_info->l_signal = scanned_bss_info->l_signal * 100; /* 100 扩大100倍 */

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0))
    /* 由于驱动缓存扫描结果，导致cts认证2次扫描的bss的timestamp时间一致(后一次没有扫描到) */
    get_monotonic_boottime(&ts);
    scanned_bss_info->mgmt->u.probe_resp.timestamp = ((hi_u64)ts.tv_sec * 1000000) /* 1000000 时间戳单位转换为s */
        + ts.tv_nsec / 1000;                                                       /* 1000 时间戳单位转换为s */
#endif
    /* 扫描维测 */
    oam_info_log3(0, OAM_SF_SCAN, "{wal_inform_bss_frame::bssid:0x%x:XX:XX:XX:0x%x:0x%x}",
        scanned_bss_info->mgmt->bssid[0], scanned_bss_info->mgmt->bssid[4], scanned_bss_info->mgmt->bssid[5]); /* 4 5 */

    /* 逐个上报内核bss 信息 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    struct WlanChannel channel = {
        .channelId = ieee80211_channel->hw_value,
        .centerFreq = ieee80211_channel->center_freq,
        .flags = ieee80211_channel->flags,
    };
    HdfWifiEventInformBssFrame(netdev, &channel, (struct ScannedBssInfo *)scanned_bss_info);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    cfg80211_bss = oal_cfg80211_inform_bss_frame(wiphy, ieee80211_channel, scanned_bss_info->mgmt,
        scanned_bss_info->mgmt_len, scanned_bss_info->l_signal, GFP_ATOMIC);
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    if (cfg80211_bss != NULL) {
        /* cfg80211_put_bss(struct wiphy *wiphy, struct cfg80211_bss *pub) */
        oal_cfg80211_put_bss(wiphy, cfg80211_bss);
    }
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /* liteos has no cfg80211_put_bss */
#endif

    return;
}

/* ****************************************************************************
 函 数 名  : wal_inform_all_bss
 功能描述  : 上报所有的bss到内核
 输入参数  : oal_wiphy_stru  *pst_wiphy,
             hmac_bss_mgmt_stru  *pst_bss_mgmt,
             hi_u8   uc_vap_id
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年7月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_inform_all_bss(const oal_net_device_stru *netdev, oal_wiphy_stru *wiphy, hmac_bss_mgmt_stru *bss_mgmt,
    hi_u8 vap_id)
{
    hi_list                   *entry = HI_NULL;
    wal_scanned_bss_info_stru  scanned_bss_info;
    hi_u32                     bss_num_not_in_regdomain = 0;

#ifdef _PRE_WLAN_FEATURE_MESH
    mac_vap_stru *mac_vap = mac_vap_get_vap_stru(vap_id);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_inform_all_bss::mac_vap_get_vap_stru failed}");
        return;
    }
#else
    hi_unref_param(vap_id);
#endif

    /* 获取锁 */
    oal_spin_lock(&(bss_mgmt->st_lock));

    /* 遍历扫描到的bss信息 */
    hi_list_for_each(entry, &(bss_mgmt->bss_list_head)) {
        hmac_scanned_bss_info *scanned_bss = hi_list_entry(entry, hmac_scanned_bss_info, dlist_head);
        hi_u8                  chan        = scanned_bss->bss_dscr_info.channel.chan_number;
        enum ieee80211_band    band        = (enum ieee80211_band)scanned_bss->bss_dscr_info.channel.band;

        /* 判断信道是不是在管制域内，如果不在，则不上报内核 */
        if (mac_is_channel_num_valid(band, chan) != HI_SUCCESS) {
            oam_warning_log2(vap_id, OAM_SF_SCAN, "{wal_inform_all_bss::chan=%d,band=%d not in regdomain}", chan, band);
            bss_num_not_in_regdomain++;
            continue;
        }

#ifdef _PRE_WLAN_FEATURE_ANY
        /* 支持ANY的设备进行扫描的时候，会把支持ANY的其他STA设备也扫描到,此时这些STA不应该上报内核，只上报AP */
        if ((scanned_bss->bss_dscr_info.supp_any == HI_TRUE) && (scanned_bss->bss_dscr_info.is_any_sta == HI_TRUE)) {
            continue;
        }
#endif

        /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
        memset_s(&scanned_bss_info, sizeof(wal_scanned_bss_info_stru), 0, sizeof(wal_scanned_bss_info_stru));
        scanned_bss_info.l_signal = scanned_bss->bss_dscr_info.rssi;

        /* 填bss所在信道的中心频率 */
        scanned_bss_info.s_freq = (hi_s16)oal_ieee80211_channel_to_frequency(chan, band);

        /* 填管理帧指针和长度 */
        scanned_bss_info.mgmt = (oal_ieee80211_mgmt_stru *)(scanned_bss->bss_dscr_info.auc_mgmt_buff);
        scanned_bss_info.mgmt_len = scanned_bss->bss_dscr_info.mgmt_len;

        /* 获取上报的扫描结果的管理帧的帧头 */
        mac_ieee80211_frame_stru *frame_hdr = (mac_ieee80211_frame_stru *)scanned_bss->bss_dscr_info.auc_mgmt_buff;

        /* 如果扫描请求接收到的帧类型有beacon类型，统一修改为probe rsp类型上报，
           为了解决上报内核的扫描结果beacon帧不够敏感的问题，此问题，在01出现过 */
        frame_hdr->frame_control.sub_type =
            (frame_hdr->frame_control.sub_type == WLAN_BEACON) ? WLAN_PROBE_RSP : frame_hdr->frame_control.sub_type;

        /* 上报扫描结果给内核 */
        /* 如果是mesh ap发起的扫描，只上报mesh ap */
#ifdef _PRE_WLAN_FEATURE_MESH
        if (((mac_vap->vap_mode == WLAN_VAP_MODE_MESH) && (scanned_bss->bss_dscr_info.is_hisi_mesh == HI_TRUE)) ||
            (mac_vap->vap_mode != WLAN_VAP_MODE_MESH)) {
            wal_inform_bss_frame(netdev, &scanned_bss_info, wiphy);
        }
#else
        wal_inform_bss_frame(netdev, &scanned_bss_info, wiphy);
#endif
    }

    /* 解除锁 */
    oal_spin_unlock(&(bss_mgmt->st_lock));

    oam_warning_log2(vap_id, OAM_SF_SCAN, "{wal_inform_all_bss::%d bss not in regdomain,inform kernal bss num=%d}",
        bss_num_not_in_regdomain, (bss_mgmt->bss_num - bss_num_not_in_regdomain));
}

/* ****************************************************************************
 函 数 名  : wal_free_scan_resource
 功能描述  : 释放申请的信道信息资源
 输入参数  : mac_cfg80211_scan_param_stru *
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void wal_free_scan_resource(mac_cfg80211_scan_param_stru *scan_param)
{
    if (scan_param->pul_channels_2_g != HI_NULL) {
        oal_free(scan_param->pul_channels_2_g);
        scan_param->pul_channels_2_g = HI_NULL;
    }
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (scan_param->puc_ie != HI_NULL) {
        oal_free((hi_u8 *)(scan_param->puc_ie));
        scan_param->puc_ie = HI_NULL;
    }
#endif
    oal_mem_free(scan_param);
}

/* ****************************************************************************
 函 数 名  : wal_set_scan_channel
 功能描述  : 提取内核下发扫描信道相关参数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_set_scan_channel(const oal_cfg80211_scan_request_stru *request,
    mac_cfg80211_scan_param_stru *scan_param)
{
    hi_u32 loop;
    hi_u32 num_chan_2g = 0;

    if (request->n_channels == 0) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_get_scan_channel_num::channel number is 0 in scan request, is wrong!}");
        return HI_FAIL;
    }

    scan_param->pul_channels_2_g = (hi_u32 *)oal_memalloc(request->n_channels * sizeof(hi_u32));
    if (oal_unlikely(scan_param->pul_channels_2_g == HI_NULL)) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_scan_work_func::memory is too low, fail to alloc for 2.4G channel!}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    for (loop = 0; loop < request->n_channels; loop++) {
        hi_u16  us_center_freq;
        hi_u32  chn;

        us_center_freq = request->channels[loop]->center_freq;

        /* 根据中心频率，计算信道号 */
        chn = (hi_u32)oal_ieee80211_frequency_to_channel((hi_s32)us_center_freq);

        if (us_center_freq <= WAL_MAX_FREQ_2G) {
            scan_param->pul_channels_2_g[num_chan_2g++] = chn;
        }
    }

    scan_param->num_channels_2_g = (hi_u8)num_chan_2g;

    if (num_chan_2g == 0) {
        oal_free(scan_param->pul_channels_2_g);
        scan_param->pul_channels_2_g = HI_NULL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置扫描的SSID

 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void wal_set_scan_ssid(const oal_cfg80211_scan_request_stru *request,
    mac_cfg80211_scan_param_stru *scan_param)
{
    hi_u32 loop;
    hi_u32 index = 0;
    hi_u32 ssid_num;

    scan_param->l_ssid_num = 0;
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (request->prefix_ssid_scan_flag == HI_TRUE) {
        scan_param->l_ssid_num = request->n_ssids;
        return;
    }
#endif

    /* 取内核下发的ssid的个数 */
    ssid_num = (request->n_ssids > WLAN_SCAN_REQ_MAX_BSS) ? WLAN_SCAN_REQ_MAX_BSS : request->n_ssids;

    /* 将用户下发的ssid信息拷贝到对应的结构体中 */
    if ((ssid_num == 0) || (request->ssids == HI_NULL)) {
        return;
    }

    for (loop = 0; loop < ssid_num; loop++) {
        if (scan_param->ssids[index].ssid_len > OAL_IEEE80211_MAX_SSID_LEN) {
            oam_warning_log2(0, OAM_SF_SCAN, "{wal_set_scan_ssid::ssid length [%d] is larger than %d, skip it.}",
                scan_param->ssids[loop].ssid_len, OAL_IEEE80211_MAX_SSID_LEN);
            continue;
        }
        scan_param->ssids[index].ssid_len = request->ssids[loop].ssid_len;
        if (memcpy_s(scan_param->ssids[index].auc_ssid, OAL_IEEE80211_MAX_SSID_LEN, request->ssids[loop].ssid,
            scan_param->ssids[loop].ssid_len) != EOK) {
            oam_warning_log0(0, OAM_SF_SCAN, "{wal_set_scan_ssid::fail to copy ssid to scan_param, skip it!}");
            continue;
        }
        index++;
    }

    scan_param->l_ssid_num = index;
}

/* ****************************************************************************
 功能描述  : 等待扫描完成超时处理函数

 修改历史      :
  1.日    期   : 2015年5月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_wait_for_scan_timeout_fn(hi_void *arg)
{
    hmac_vap_stru                  *hmac_vap = (hmac_vap_stru *)arg;
    mac_vap_stru                   *mac_vap  = hmac_vap->base_vap;
    hmac_device_stru               *hmac_dev = HI_NULL;
    mac_device_stru                *mac_dev = HI_NULL;
    hmac_bss_mgmt_stru             *bss_mgmt = HI_NULL;
    hmac_scan_stru                 *scan_mgmt = HI_NULL;
    oal_wiphy_stru                 *wiphy = HI_NULL;
    oal_net_device_stru            *netdev = HI_NULL;

    oam_warning_log0(mac_vap->vap_id, OAM_SF_SCAN, "{wal_wait_for_scan_timeout_fn:: 5 seconds scan timeout proc.}");

    /* 根据当前扫描的类型和当前vap的状态，决定切换vap的状态，扫描异常保护中，上报内核扫描状态为扫描完成 */
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) && (mac_vap->vap_state == MAC_VAP_STATE_STA_WAIT_SCAN)) {
        /* 改变vap状态到SCAN_COMP */
        mac_vap_state_change(mac_vap, MAC_VAP_STATE_STA_SCAN_COMP);
    }

    /* 获取hmac device */
    hmac_dev   = hmac_get_device_stru();
    mac_dev = mac_res_get_dev();
    scan_mgmt  = &(hmac_dev->scan_mgmt);
    wiphy      = mac_dev->wiphy;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device(mac_vap->vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_mesh_close_peer_inform::get net device ptr is null!}\r\n");
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
        oal_spin_lock(&(scan_mgmt->st_scan_request_spinlock));
        wal_free_scan_mgmt_resource(scan_mgmt);
        oal_spin_unlock(&(scan_mgmt->st_scan_request_spinlock));
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        scan_mgmt->request = HI_NULL;
#endif
        scan_mgmt->complete = HI_TRUE;
        scan_mgmt->is_scanning = HI_FALSE;
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取扫描结果的管理结构地址 */
    bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt);

    /* 对于内核下发的扫描request资源加锁 */
    oal_spin_lock(&(scan_mgmt->st_scan_request_spinlock));

    if (scan_mgmt->request != HI_NULL) {
        /* 上报扫描到的所有的bss */
        wal_inform_all_bss(netdev, wiphy, bss_mgmt, mac_vap->vap_id);

        /* 通知 kernel scan 已经结束 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
        HdfWifiEventScanDone(netdev, (WifiScanStatus)HISI_SCAN_TIMEOUT);
        wal_free_scan_mgmt_resource(scan_mgmt);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        oal_cfg80211_scan_done(scan_mgmt->request, 0);
        scan_mgmt->request = HI_NULL;
#endif
        scan_mgmt->complete = HI_TRUE;
        scan_mgmt->is_scanning = HI_FALSE;
    }

    /* 通知完内核，释放资源后解锁 */
    oal_spin_unlock(&(scan_mgmt->st_scan_request_spinlock));

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_process_timer_for_scan
 功能描述  : 关闭扫描结果老化定时器，启动扫描定时器做扫描超时保护处理

 修改历史      :
  1.日    期   : 2015年5月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void wal_process_timer_for_scan(hi_u8 vap_id)
{
    hmac_vap_stru *hmac_vap = HI_NULL;

    /* 获取hmac vap */
    hmac_vap = hmac_vap_get_vap_stru(vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(vap_id, OAM_SF_SCAN, "{wal_process_timer_for_scan::pst_hmac_vap is null!}");
        return;
    }
    /* 关闭扫描结果老化定时器 */
    if (hmac_vap->scan_timeout.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(hmac_vap->scanresult_clean_timeout));
    }
    /* 启动扫描保护定时器，在指定时间没有上报扫描结果，主动上报扫描完成 */
    frw_timer_create_timer(&(hmac_vap->scan_timeout), wal_wait_for_scan_timeout_fn, WAL_MAX_SCAN_TIME_PER_SCAN_REQ,
        hmac_vap, HI_FALSE);

    return;
}

/* ****************************************************************************
 函 数 名  : wal_start_scan_req
 功能描述  : 解析内核下发扫描命令相关参数，启动扫描
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_start_scan_req(oal_net_device_stru *netdev, hmac_scan_stru *scan_mgmt)
{
    mac_cfg80211_scan_param_stru *scan_param = HI_NULL;
    hi_u32                        ret;
    mac_vap_stru                 *mac_vap = oal_net_dev_priv(netdev);
    hi_u8                         vap_id = mac_vap->vap_id;
    oal_cfg80211_scan_request_stru *request = scan_mgmt->request;

    scan_param =
        (mac_cfg80211_scan_param_stru *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, sizeof(mac_cfg80211_scan_param_stru));
    if (scan_param == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_start_scan_req::memory is too low, fail to alloc scan param memory!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    memset_s(scan_param, sizeof(mac_cfg80211_scan_param_stru), 0, sizeof(mac_cfg80211_scan_param_stru));

    /* 解析内核下发的扫描信道列表 */
    if (wal_set_scan_channel(request, scan_param) != HI_SUCCESS) {
        wal_free_scan_resource(scan_param);
        return HI_FAIL;
    }

    /* 解析内核下发的ssid */
    wal_set_scan_ssid(request, scan_param);

    /* 解析内核下发的ie,        Mesh ID携带在IE段中 */
    if ((request->ie_len > 0) && (request->ie != HI_NULL)) {
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        scan_param->puc_ie = (hi_u8 *)malloc(request->ie_len);
        if (oal_unlikely(scan_param->puc_ie == HI_NULL)) {
            oam_error_log0(0, OAM_SF_SCAN, "{wal_start_scan_req::memory is too low, fail to alloc for scan ie!}");
            wal_free_scan_resource(scan_param);
            return HI_FAIL;
        }

        if (memcpy_s((hi_void *)(scan_param->puc_ie), request->ie_len, request->ie, request->ie_len) != EOK) {
            oam_error_log0(0, OAM_SF_SCAN, "{wal_start_scan_req::fail to copy scan ie, return!}");
            wal_free_scan_resource(scan_param);
            return HI_FAIL;
        }
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        scan_param->puc_ie = request->ie;
#endif

        scan_param->ie_len = request->ie_len;
    } else {
        scan_param->puc_ie = HI_NULL;
        scan_param->ie_len = 0;
    }

    scan_param->scan_type = OAL_ACTIVE_SCAN; /* active scan */

    /* P2P WLAN/P2P 特性情况下，根据扫描的ssid 判断是否为p2p device 发起的扫描，
     * ssid 为"DIRECT-"则认为是p2p device 发起的扫描解析下发扫描的device 是否为p2p device(p2p0)
     */
    if (is_p2p_scan_req(request)) {
        scan_param->is_p2p0_scan = HI_TRUE;
    }

    /* 在事件前防止异步调度完成扫描后,发生同步问题 */
    scan_mgmt->complete = HI_FALSE;

    /* 抛事件，通知驱动启动扫描 */
    ret = wal_cfg80211_start_req(netdev, &scan_param, sizeof(uintptr_t), WLAN_CFGID_CFG80211_START_SCAN, HI_TRUE);
    if (ret != HI_SUCCESS) {
        wal_free_scan_resource(scan_param);
        oal_cfg80211_scan_done(scan_mgmt->request, 0);
        scan_mgmt->complete = HI_TRUE;
        return HI_FAIL;
    }
    wal_free_scan_resource(scan_param);
    /* 关闭扫描结果老化定时器,启动扫描超时定时器 */
    wal_process_timer_for_scan(vap_id);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_send_scan_abort_msg
 功能描述  : 终止扫描
 输入参数  : oal_net_device_stru   *pst_net_dev
 输出参数  : 无
 返 回 值  : hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年7月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_send_scan_abort_msg(oal_net_device_stru *netdev)
{
    wal_msg_write_stru              write_msg;
    hi_u32                          pedding_data = 0;       /* 填充数据，不使用，只是为了复用接口 */
    hi_u32                          ret;
    wal_msg_stru                    *rsp_msg = HI_NULL;

    /* 拋事件通知device侧终止扫描 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SCAN_ABORT, sizeof(pedding_data));

    if (memcpy_s(write_msg.auc_value, sizeof(pedding_data), (hi_s8 *)&pedding_data, sizeof(pedding_data)) != EOK) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_send_scan_abort_msg::mem safe function err!}");
        return HI_FAIL;
    }

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(pedding_data),
        (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (HI_SUCCESS != wal_check_and_release_msg_resp(rsp_msg)) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_send_scan_abort_msg::wal_check_and_release_msg_resp fail.}");
    }

    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_send_scan_abort_msg::fail to stop scan, error[%u]}", ret);
    }

    return ret;
}

/* ************** **************************************************************
 函 数 名  : wal_force_scan_complete
 功能描述  : 通知扫描完成
 输入参数  : oal_net_device_stru   *pst_net_dev,
             hi_bool          en_is_aborted
 输出参数  : 无
 返 回 值  : hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年7月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_force_scan_complete(oal_net_device_stru *netdev)
{
    mac_device_stru *mac_dev = mac_res_get_dev();

    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_force_scan_complete::Cannot find mac_vap by net_dev!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取hmac device */
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    /* stop的vap和正在扫描的vap不相同则直接返回 */
    if (mac_vap->vap_id != hmac_dev->scan_mgmt.scan_record_mgmt.vap_id) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_SCAN,
            "{wal_force_scan_complete::stop_vap[%d] is different scan_vap[%d]!}", mac_vap->vap_id,
            hmac_dev->scan_mgmt.scan_record_mgmt.vap_id);
        return HI_SUCCESS;
    }

    hmac_scan_stru *scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 结束扫描的时候设置标志为非ANY扫描 */
    scan_mgmt->scan_record_mgmt.is_any_scan = HI_FALSE;

    /* 如果是来自内部的扫描 */
    if (scan_mgmt->request == HI_NULL) {
        /* 判断是否存在内部扫描，如果存在，也需要停止 */
        if ((hmac_dev->scan_mgmt.is_scanning == HI_TRUE) &&
            (mac_vap->vap_id == hmac_dev->scan_mgmt.scan_record_mgmt.vap_id)) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_SCAN, "{wal_force_scan_complete::maybe internal scan,stop scan}");
            /* 终止扫描 */
            wal_send_scan_abort_msg(netdev);
        }

        return HI_SUCCESS;
    }

    /* 获取hmac vap */
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN, "{wal_force_scan_complete::hmac_vap is null, vap_id[%d]!}",
            mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 删除等待扫描超时定时器 */
    if (hmac_vap->scan_timeout.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(hmac_vap->scan_timeout));
    }

    /* 如果是上层下发的扫描请求，则通知内核扫描结束，内部扫描不需通知 */
    if (scan_mgmt->request != HI_NULL) {
        /* 对于内核下发的扫描request资源加锁 */
        oal_spin_lock(&(scan_mgmt->st_scan_request_spinlock));

        /* 上报内核扫描结果 */
        wal_inform_all_bss(netdev, mac_dev->wiphy, &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt), mac_vap->vap_id);

        /* 通知内核扫描终止 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
        HdfWifiEventScanDone(netdev, (WifiScanStatus)HISI_SCAN_SUCCESS);
        wal_free_scan_mgmt_resource(scan_mgmt);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        oal_cfg80211_scan_done(scan_mgmt->request, HI_TRUE);
        scan_mgmt->request = HI_NULL;
#endif
        scan_mgmt->complete = HI_TRUE;

        /* 通知完内核，释放资源后解锁 */
        oal_spin_unlock(&(scan_mgmt->st_scan_request_spinlock));

        /* 下发device终止扫描 */
        wal_send_scan_abort_msg(netdev);

        oam_info_log1(mac_vap->vap_id, OAM_SF_SCAN, "{wal_force_scan_complete::force to stop scan of vap_id[%d]}",
            mac_vap->vap_id);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
