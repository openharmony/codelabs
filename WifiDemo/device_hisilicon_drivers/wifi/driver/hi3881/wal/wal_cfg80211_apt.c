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
#include "wal_cfg80211_apt.h"
#include "net_adpater.h"
#include "hmac_ext_if.h"
#include "wal_event_msg.h"
#include "wal_wpa_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 外部函数声明
**************************************************************************** */
/* ****************************************************************************
  3 全局变量定义
**************************************************************************** */
hisi_upload_frame_cb     g_upload_frame_func = HI_NULL;
hisi_send_event_cb       g_send_event_func   = HI_NULL;

/* ****************************************************************************
  4 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 提供给用户的回调函数注册接口，回调函数用于驱动向wpa抛事件
 输入参数  : [1]func
 输出参数  : 无
 返 回 值  : 无
**************************************************************************** */
hi_s32 hisi_register_send_event_cb(hisi_send_event_cb func)
{
    if ((g_send_event_func != NULL) && (func != NULL)) {
        g_send_event_func = func;
        return HI_ERR_S_FAILURE;
    }
    g_send_event_func = func;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 上报new station事件接口
 输入参数  : oal_net_device_stru *pst_dev, const hi_u8 *mac_addr,
             oal_station_info_stru *sinfo, oal_gfp_enum_uint8 gfp
 输出参数  : 无
 返 回 值  : 无
**************************************************************************** */
hi_u32 cfg80211_new_sta(const oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len,
    const oal_station_info_stru *sinfo)
{
    hisi_new_sta_info_stru new_sta_info;

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::g_send_event_func is null.}\r\n");
        return HI_FAIL;
    }

    /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(&new_sta_info, sizeof(hisi_new_sta_info_stru), 0, sizeof(hisi_new_sta_info_stru));

    if (sinfo->assoc_req_ies_len == 0) {
        oam_error_log2(0, OAM_SF_ANY, "{cfg80211_new_sta::assoc_req_ies or assoc_req_ies_len null error %p,%d.}\r\n",
            (uintptr_t)sinfo->assoc_req_ies, sinfo->assoc_req_ies_len);
        return HI_FAIL;
    }

    new_sta_info.ielen = sinfo->assoc_req_ies_len;
    new_sta_info.reassoc = 0;
    if (memcpy_s(new_sta_info.macaddr, ETH_ADDR_LEN, mac_addr, addr_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::mem safe function err!}");
        return HI_FAIL;
    }

    new_sta_info.ie = (hi_u8 *)oal_memalloc(sinfo->assoc_req_ies_len);
    if (new_sta_info.ie == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::pst_new_sta_info->ie malloc error!}");
        return HI_FAIL;
    }
    if (memcpy_s(new_sta_info.ie, sinfo->assoc_req_ies_len, sinfo->assoc_req_ies, sinfo->assoc_req_ies_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::mem safe function err!}");
        oal_free(new_sta_info.ie);
        return HI_FAIL;
    }

    hi_u32 ret = (hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_NEW_STA, (hi_u8 *)&new_sta_info,
        (hi_u32)sizeof(hisi_new_sta_info_stru));
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::g_send_event_func fail.}\r\n");
        oal_free(new_sta_info.ie);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 上报new station事件接口
 输入参数  : oal_net_device_stru *pst_dev, const hi_u8 *puc_mac_addr,
            oal_gfp_enum_uint8 en_gfp
 输出参数  : 无
 返 回 值  : 无
**************************************************************************** */
hi_u32 cfg80211_del_sta(oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len)
{
    hi_u32 l_ret;

    /* 入参检查 */
    if ((netdev == HI_NULL) || (mac_addr == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{cfg80211_del_sta::pst_dev or puc_mac_addr ptr NULL %p,%p.}\r\n",
            (uintptr_t)netdev, (uintptr_t)mac_addr);
        return HI_FAIL;
    }

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_del_sta::g_send_event_func is null.}\r\n");
        return HI_FAIL;
    }

    l_ret = (hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_DEL_STA, (hi_u8 *)mac_addr, addr_len);
    return l_ret;
}

/* ****************************************************************************
 功能描述  : 上报管理帧数据接口
 输入参数  : struct wireless_dev *wdev, hi_s32 freq, hi_s32 sig_mbm,
             const hi_u8 *buf, size_t len, oal_gfp_enum_uint8 gfp
 输出参数  : 无
 返 回 值  : bool
**************************************************************************** */
hi_u8 cfg80211_rx_mgmt(const oal_net_device_stru *netdev, hi_s32 freq, hi_s32 l_sig_mbm, const hi_u8 *buffer,
    size_t len)
{
    hisi_rx_mgmt_stru rx_mgmt;
    oal_wireless_dev *wdev = (oal_wireless_dev *)GET_NET_DEV_CFG80211_WIRELESS(netdev);
    hi_u32 l_ret;

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::g_send_event_func is null.}\r\n");
        return HI_FALSE;
    }

    if (memset_s(&rx_mgmt, sizeof(hisi_rx_mgmt_stru), 0, sizeof(hisi_rx_mgmt_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::mem safe function err!}");
        return HI_FALSE;
    }
    /* 入参检查 */
    if ((wdev == HI_NULL) || (buffer == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::pst_wdev or buf ptr NULL %p,%p.}\r\n", (uintptr_t)wdev,
            (uintptr_t)buffer);
        return HI_FALSE;
    }

    rx_mgmt.buf = HI_NULL;
    rx_mgmt.len = len;
    rx_mgmt.freq = (hi_u32)freq;
    rx_mgmt.sig_mbm = l_sig_mbm;

    if (len != 0) {
        rx_mgmt.buf = oal_memalloc(len);
        if (rx_mgmt.buf == HI_NULL) {
            oam_error_log1(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::pst_rx_mgmt->buf malloc error %p.}",
                (uintptr_t)rx_mgmt.buf);
            return HI_FALSE;
        }
        if (memcpy_s(rx_mgmt.buf, len, buffer, len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::mem safe function err!}");
            oal_free(rx_mgmt.buf);
            return HI_FALSE;
        }
    }

    l_ret = (hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_RX_MGMT, (hi_u8 *) &rx_mgmt,
                                      sizeof(hisi_rx_mgmt_stru));
    if (l_ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::g_send_event_func fail.}\r\n");
        if (rx_mgmt.buf != HI_NULL) {
            oal_free(rx_mgmt.buf);
        }
        return HI_FALSE;
    }

    return HI_TRUE;
}

/* ****************************************************************************
 函 数 名  : cfg80211_mgmt_tx_status
 功能描述  : 上报发送状态接口
 输入参数  : struct wireless_dev *wdev, hi_u64 cookie,
             const hi_u8 *buf, size_t len, bool ack, oal_gfp_enum_uint8 gfp
 输出参数  : 无
 返 回 值  : bool
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u8 cfg80211_mgmt_tx_status(struct wireless_dev *wdev, const hi_u8 *buffer, size_t len, hi_u8 ack)
{
    hisi_tx_status_stru tx_status;
    hi_u32 l_ret;

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::g_send_event_func is null.}\r\n");
        return HI_FALSE;
    }

    if (memset_s(&tx_status, sizeof(hisi_tx_status_stru), 0, sizeof(hisi_tx_status_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::mem safe function err!}");
        return HI_FALSE;
    }
    /* 入参检查 */
    if ((wdev == HI_NULL) || (buffer == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::pst_wdev or puc_buf ptr NULL %p,%p.}\r\n",
            (uintptr_t)wdev, (uintptr_t)buffer);
        return HI_FALSE;
    }

    tx_status.buf = HI_NULL;
    tx_status.len = len;
    tx_status.ack = ack;

    if (len != 0) {
        tx_status.buf = oal_memalloc(len);
        if (tx_status.buf == HI_NULL) {
            oam_error_log1(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::pst_tx_status->buf malloc error %p.}",
                (uintptr_t)tx_status.buf);
            return HI_FALSE;
        }
        if (memcpy_s(tx_status.buf, len, buffer, len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::mem safe function err!}");
            oal_free(tx_status.buf);
            return HI_FALSE;
        }
    }

    l_ret = (hi_u32)g_send_event_func(wdev->netdev->name, HISI_ELOOP_EVENT_TX_STATUS, (hi_u8 *)&tx_status,
        sizeof(hisi_tx_status_stru));
    if (l_ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::g_send_event_func fail.}\r\n");
        if (tx_status.buf != HI_NULL) {
            oal_free(tx_status.buf);
        }
        return HI_FALSE;
    }

    return HI_TRUE;
}

/* ****************************************************************************
 函 数 名  : cfg80211_inform_bss_frame
 功能描述  : 扫描结果上报接口
 输入参数  :

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void cfg80211_inform_bss_frame(const oal_net_device_stru *netdev, oal_wiphy_stru *wiphy,
    const oal_ieee80211_channel_stru *ieee80211_channel, const wal_scanned_bss_info_stru *scanned_bss_info)
{
    hisi_scan_result_stru scan_result = { 0 };

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::g_send_event_func is null.}\r\n");
        return;
    }

    if ((wiphy == HI_NULL) || (ieee80211_channel == HI_NULL) || (scanned_bss_info->mgmt == HI_NULL)) {
        oam_error_log3(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::wiphy or ieee80211_channel or mgmt null %p,%p,%p}",
            (uintptr_t)wiphy, (uintptr_t)ieee80211_channel, (uintptr_t)(scanned_bss_info->mgmt));
        return;
    }

    hi_u32 ie_len = scanned_bss_info->mgmt_len - (hi_u32)oal_offset_of(oal_ieee80211_mgmt_stru, u.probe_resp.variable);
    hi_u32 beacon_len = scanned_bss_info->mgmt_len - (hi_u32)oal_offset_of(oal_ieee80211_mgmt_stru, u.beacon.variable);

    scan_result.variable = oal_memalloc(ie_len + beacon_len);
    if (scan_result.variable == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::mallocErr.ieLen%d,beaconLen%d}", ie_len, beacon_len);
        return;
    }

    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    memset_s(scan_result.variable, ie_len + beacon_len, 0, ie_len + beacon_len);

    scan_result.ie_len        = ie_len;
    scan_result.beacon_ie_len = beacon_len;
    scan_result.beacon_int  = (hi_s16)scanned_bss_info->mgmt->u.probe_resp.beacon_int;
    scan_result.caps        = (hi_s16) scanned_bss_info->mgmt->u.probe_resp.capab_info;
    scan_result.level       = scanned_bss_info->l_signal;
    scan_result.freq        = ieee80211_channel->center_freq;
    scan_result.flags       = (hi_s32)ieee80211_channel->flags;

    if (memcpy_s(scan_result.bssid, ETH_ADDR_LEN, scanned_bss_info->mgmt->bssid, ETH_ALEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::mem safe function err!}");
        oal_free(scan_result.variable);
        return;
    }
    if (memcpy_s(scan_result.variable, ie_len, scanned_bss_info->mgmt->u.probe_resp.variable, ie_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::mem safe function err!}");
        oal_free(scan_result.variable);
        return;
    }
    if (memcpy_s(scan_result.variable + ie_len, beacon_len,
        scanned_bss_info->mgmt->u.beacon.variable, beacon_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::mem safe function err!}");
        oal_free(scan_result.variable);
        return;
    }

    if ((hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_SCAN_RESULT, (hi_u8 *)&scan_result,
        sizeof(hisi_scan_result_stru)) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::g_send_event_func fail.}");
        oal_free(scan_result.variable);
    }
}

hi_void cfg80211_connect_result_return(const oal_net_device_stru *netdev, const oal_connet_result_stru *connet_result,
    hisi_connect_result_stru *connect_result)
{
    hi_unref_param(connet_result);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (connet_result->us_connect_status == 0) {
        /* 如果上报关联成功则关闭PM */
        hi_u32 pm_flag = 0;
        hwal_ioctl_set_pm_on(netdev->name, &pm_flag);
    }
#endif
    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::g_send_event_func is null.}\r\n");
        return;
    }

    if ((hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_CONNECT_RESULT, (hi_u8 *)connect_result,
        sizeof(hisi_connect_result_stru)) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::g_send_event_func fail.}\r\n");
        if (connect_result->req_ie != HI_NULL) {
            oal_free(connect_result->req_ie);
            connect_result->req_ie = HI_NULL;
        }
        if (connect_result->resp_ie != HI_NULL) {
            oal_free(connect_result->resp_ie);
            connect_result->resp_ie = HI_NULL;
        }
    }

    return;
}

/* ****************************************************************************
 函 数 名  : cfg80211_connect_result
 功能描述  : 关联结果上报接口
 输入参数  :

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void cfg80211_connect_result(const oal_net_device_stru *netdev, const oal_connet_result_stru *connet_result)
{
    hisi_connect_result_stru connect_result = { 0 };

    if (g_send_event_func == HI_NULL || netdev == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{cfg80211_connect_result::g_send_event_func or dev null %p}", (uintptr_t)netdev);
        return;
    }

    if (memcpy_s(connect_result.bssid, ETH_ADDR_LEN, connet_result->auc_bssid, ETH_ALEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::mem safe function err!}");
        return;
    }

    if ((connet_result->puc_req_ie != HI_NULL) && (connet_result->req_ie_len != 0)) {
        connect_result.req_ie = oal_memalloc(connet_result->req_ie_len);
        connect_result.req_ie_len = connet_result->req_ie_len;
        if (connect_result.req_ie == HI_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::pst_connect_result->req_ie zalloc error}\r\n");
            return;
        }
        if (memcpy_s(connect_result.req_ie, connet_result->req_ie_len, connet_result->puc_req_ie,
            connet_result->req_ie_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::mem safe function err!}");
            oal_free(connect_result.req_ie);
            return;
        }
    }

    if ((connet_result->puc_rsp_ie != HI_NULL) && (connet_result->rsp_ie_len != 0)) {
        connect_result.resp_ie = oal_memalloc(connet_result->rsp_ie_len);
        connect_result.resp_ie_len = connet_result->rsp_ie_len;
        if (connect_result.resp_ie == HI_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::pst_connect_result->resp_ie zalloc error}\n");
            oal_free(connect_result.req_ie);
            return;
        }
        if (memcpy_s(connect_result.resp_ie, connet_result->rsp_ie_len, connet_result->puc_rsp_ie,
            connet_result->rsp_ie_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::mem safe function err!}");
            oal_free(connect_result.req_ie);
            oal_free(connect_result.resp_ie);
            return;
        }
    }

    connect_result.status = connet_result->us_connect_status;
    connect_result.freq = connet_result->us_freq;

    cfg80211_connect_result_return(netdev, connet_result, &connect_result);

    return;
}

/* ****************************************************************************
 函 数 名  : cfg80211_disconnected
 功能描述  : 去关联上报接口
 输入参数  :

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 cfg80211_disconnected(const oal_net_device_stru *netdev, hi_u16 us_reason, const hi_u8 *ie, hi_u32 ie_len)
{
    hisi_disconnect_stru disconnect;
    hi_u32 l_ret;

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_disconnected::g_send_event_func is null.}");
        return HI_FAIL;
    }

    /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(&disconnect, sizeof(hisi_disconnect_stru), 0, sizeof(hisi_disconnect_stru));

    if ((ie != HI_NULL) && (ie_len != 0)) {
        disconnect.ie = oal_memalloc(ie_len);
        disconnect.ie_len = ie_len;
        if (disconnect.ie == HI_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_disconnected::malloc error.}");
            return HI_FAIL;
        }
        if (memcpy_s(disconnect.ie, ie_len, ie, ie_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_disconnected::mem safe function err!}");
            oal_free(disconnect.ie);
            return HI_FAIL;
        }
    }
    disconnect.reason = us_reason;

    l_ret = (hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_DISCONNECT, (hi_u8 *)&disconnect,
        sizeof(hisi_disconnect_stru));
    if (l_ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{cfg80211_disconnected::g_send_event_func fail, ret:%u.}", l_ret);
        if (disconnect.ie != HI_NULL) {
            oal_free(disconnect.ie);
        }
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 延时上报机制中超时上报去关联给WPA
**************************************************************************** */
hi_u32 cfg80211_timeout_disconnected(const oal_net_device_stru *netdev)
{
    hi_u32 l_ret;

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_timeout_disconnected::g_send_event_func is null.}");
        return HI_FAIL;
    }

    l_ret = (hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_TIMEOUT_DISCONN, HI_NULL, 0);
    if (l_ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_timeout_disconnected::g_send_event_func fail.}");
    }

    return l_ret;
}

/* ****************************************************************************
 函 数 名  : cfg80211_scan_done
 功能描述  : 扫描完成上报接口
 输入参数  :

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void cfg80211_scan_done(const oal_net_device_stru *netdev, hisi_scan_status_enum status)
{
    hisi_driver_scan_status_stru scan_ctl_status;

    scan_ctl_status.scan_status = status;
    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_scan_done::g_send_event_func is null.}");
        return;
    }
    g_send_event_func(netdev->name, HISI_ELOOP_EVENT_SCAN_DONE, (hi_u8 *)&scan_ctl_status,
        sizeof(hisi_driver_scan_status_stru));
}
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : cfg80211_rx_exception
 功能描述  : DFX上报接口(私有接口)
 输入参数  :

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年2月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void cfg80211_rx_exception(const oal_net_device_stru *netdev, hi_u8 *puc_data, hi_u32 data_len)
{
    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_rx_exception::g_send_event_func is null.}\r\n");
        return;
    }
    if ((puc_data == HI_NULL) || (data_len > 32)) { /* 32:数据长度 */
        oam_error_log1(0, OAM_SF_ANY, "{cfg80211_rx_exception::puc_data is null or ul_data_len[%d] too long.}\r\n",
            data_len);
        return;
    }
    g_send_event_func(netdev->name, HISI_ELOOP_EVENT_RX_DFX, puc_data, data_len);
}

/* ****************************************************************************
 函 数 名  : cfg80211_mic_failure
 功能描述  : 上报mic攻击
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年12月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2014年5月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 cfg80211_mic_failure(const oal_net_device_stru *netdev, const hmac_mic_event_stru *mic_event,
    const hi_u8 *puc_tsc, oal_gfp_enum_uint8 gfp)
{
    hi_u32 ret;
    hisi_michael_mic_failure_stru mic_failure;

    cfg80211_michael_mic_failure(netdev, mic_event->auc_user_mac, mic_event->key_type, mic_event->l_key_id, puc_tsc,
        gfp);

    mic_failure.key_type = mic_event->key_type;
    mic_failure.l_key_id = mic_event->l_key_id;
    if (memcpy_s(mic_failure.user_mac_addr, WLAN_MAC_ADDR_LEN, mic_event->auc_user_mac, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mic_failure::mem safe function err!}");
        return HI_FAIL;
    }

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mic_failure::g_send_event_func is null.}\r\n");
        return HI_FAIL;
    }

    ret = (hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_MICHAEL_MIC_FAILURE, (hi_u8 *)&mic_failure,
        sizeof(hisi_michael_mic_failure_stru));
    return ret;
}
#endif
/* ****************************************************************************
 功能描述  : STA根据CSA切换信道事件上报
**************************************************************************** */
hi_u32 cfg80211_csa_channel_switch(const oal_net_device_stru *netdev, hi_s32 freq)
{
    hi_u32 ret;
    hisi_ch_switch_stru channel_switch;

    channel_switch.freq = freq;

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_report_accept_peer::g_send_event_func is null.}");
        return HI_FAIL;
    }

    ret = (hi_u32)g_send_event_func(netdev->name, HISI_ELOOP_EVENT_CHANNEL_SWITCH, (hi_u8 *)&channel_switch,
        (hi_u32)sizeof(hisi_ch_switch_stru));
    return ret;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : Mesh上报需要发送Mesh Peering Close Frame
 被调函数  :
  1.日    期   : 2019年1月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 cfg80211_mesh_close(const oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len, hi_u16 us_reason)
{
    hisi_mesh_close_peer_stru mesh_close_peer;

    if (memset_s(&mesh_close_peer, sizeof(hisi_mesh_close_peer_stru), 0, sizeof(hisi_mesh_close_peer_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_close::mem safe function err!}");
        return HI_FAIL;
    }

    if (memcpy_s(mesh_close_peer.macaddr, ETH_ADDR_LEN, mac_addr, addr_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_close::mem safe function err!}");
        return HI_FAIL;
    }

    mesh_close_peer.us_reason = us_reason;

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_close::g_send_event_func is null.}\r\n");
        return HI_FAIL;
    }

    g_send_event_func(netdev->name, HISI_ELOOP_EVENT_MESH_CLOSE, (hi_u8 *)&mesh_close_peer,
        sizeof(hisi_mesh_close_peer_stru));
    return HI_SUCCESS;
}

/* ****************************************************************************
功能描述  : mesh非扫描状态上报mesh beacon帧
输入参数  :oal_net_device_stru *pst_net_device, hi_u8 *puc_mac_addr,
         hi_u8 uc_bcn_prio, hi_u8 en_is_mbr, oal_gfp_enum_uint8 en_gfp
返 回 值  :hi_u32
修改历史      :
 1.日    期   : 2019年3月21日
   作    者   : HiSilicon
   修改内容   : 新生成函数
**************************************************************************** */
hi_u32 cfg80211_mesh_new_peer_candidate(const oal_net_device_stru *netdev,
    const hmac_report_new_peer_candidate_stru *puc_new_peer)
{
    hisi_mesh_new_peer_candidate_stru new_mesh_peer;

    if (memset_s(&new_mesh_peer, sizeof(hisi_mesh_new_peer_candidate_stru), 0,
        sizeof(hisi_mesh_new_peer_candidate_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_new_peer_candidate::mem safe function err!}");
        return HI_FAIL;
    }

    new_mesh_peer.bcn_prio = puc_new_peer->bcn_prio;
    new_mesh_peer.is_mbr = puc_new_peer->is_mbr;
    new_mesh_peer.rssi = (hi_s8)(-(puc_new_peer->rssi));
    if (memcpy_s(new_mesh_peer.auc_peer_addr, ETH_ADDR_LEN, puc_new_peer->auc_mac_addr, ETH_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_new_peer_candidate::mem safe function err!}");
        return HI_FAIL;
    }

    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_new_peer_candidate::g_send_event_func is null.}\r\n");
        return HI_FAIL;
    }

    g_send_event_func(netdev->name, HISI_ELOOP_EVENT_NEW_PEER_CANDIDATE, (hi_u8 *)&new_mesh_peer,
        sizeof(hisi_mesh_new_peer_candidate_stru));
    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
/* ****************************************************************************
 功能描述  : 保持在指定信道上报
 修改历史      :
  1.日    期   : 2019年4月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 cfg80211_remain_on_channel(const oal_net_device_stru *netdev, hi_u32 freq, hi_u32 duration)
{
    hisi_on_channel_stru remain_on_channel;
    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_remain_on_channel::g_send_event_func is null.}\r\n");
        return HI_FAIL;
    }
    remain_on_channel.freq = (hi_s32)freq;
    remain_on_channel.duration = (hi_s32)duration;

    g_send_event_func(netdev->name, HISI_ELOOP_EVENT_REMAIN_ON_CHANNEL, (hi_u8 *)&remain_on_channel,
        sizeof(hisi_on_channel_stru));
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 取消保持在指定信道事件上报
 修改历史      :
  1.日    期   : 2019年4月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 cfg80211_cancel_remain_on_channel(const oal_net_device_stru *netdev, hi_u32 freq)
{
    hisi_on_channel_stru remain_on_channel;
    if (g_send_event_func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_report_accept_peer::g_send_event_func is null.}\r\n");
        return HI_FAIL;
    }
    remain_on_channel.freq = (hi_s32)freq;

    g_send_event_func(netdev->name, HISI_ELOOP_EVENT_CANCEL_REMAIN_ON_CHANNEL, (hi_u8 *)&remain_on_channel,
        sizeof(hisi_on_channel_stru));
    return HI_SUCCESS;
}
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : cfg80211_rx_mgmt_ext
 功能描述  : 上报接收到的管理帧
 输入参数  : [1]dev
             [2]freq
             [3]buf
             [4]len
 输出参数  : HI_SUCCESS 上报成功，其它错误码 上报失败
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 cfg80211_rx_mgmt_ext(const oal_net_device_stru *netdev, hi_s32 freq, const hi_u8 *buf, hi_u32 len)
{
    hi_u8 ret;

    ret = cfg80211_rx_mgmt(netdev, freq, 0, buf, len);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    ret = HI_TRUE;
#endif
    if (ret == HI_TRUE) {
        return HI_SUCCESS;
    } else {
        return HI_FAIL;
    }
}

hi_void cfg80211_kobject_uevent_env_sta_join(oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len)
{
    hi_unref_param(addr_len);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (memset_s(&env, sizeof(env), 0, sizeof(env)) != EOK) {
        return;
    }
    add_uevent_var(&env, "SOFTAP=STA_JOIN wlan0 wlan0 %02x:%02x:%02x:%02x:%02x:%02x",
                   mac_addr[0], mac_addr[1], mac_addr[2],  /* 0 1 2 数组的位数 */
                   mac_addr[3], mac_addr[4], mac_addr[5]); /* 3 4 5 数组的位数 */
#if (LINUX_VERSION_CODE >= kernel_version(4, 1, 0))
    kobject_uevent_env(&(netdev->dev.kobj), KOBJ_CHANGE, env.envp);
#else
    kobject_uevent_env(&(netdev->dev.kobj), KOBJ_CHANGE, (hi_s8 **)&env);
#endif
#else
    hi_unref_param(netdev);
    hi_unref_param(mac_addr);
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */
}

hi_void cfg80211_kobject_uevent_env_sta_leave(oal_net_device_stru *netdev, const hi_u8 *mac_addr, hi_u8 addr_len)
{
    hi_unref_param(addr_len);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (memset_s(&env, sizeof(env), 0, sizeof(env)) != EOK) {
        return;
    }
    add_uevent_var(&env, "SOFTAP=STA_LEAVE wlan0 wlan0 %02x:%02x:%02x:%02x:%02x:%02x",
                   mac_addr[0], mac_addr[1], mac_addr[2],  /* 0 1 2 数组的位数 */
                   mac_addr[3], mac_addr[4], mac_addr[5]); /* 3 4 5 数组的位数 */
#if (LINUX_VERSION_CODE >= kernel_version(4, 1, 0))
    kobject_uevent_env(&(netdev->dev.kobj), KOBJ_CHANGE, env.envp);
#else
    kobject_uevent_env(&(netdev->dev.kobj), KOBJ_CHANGE, (hi_s8 **)&env);
#endif
#else
    hi_unref_param(netdev);
    hi_unref_param(mac_addr);
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */
}
#endif

#ifdef _PRE_WLAN_FEATURE_PROMIS
/* ****************************************************************************
 函 数 名  : hwal_send_others_bss_data
 功能描述  : 上报其他BSS网络数据包
 输入参数  : pst_netbuf 数据包的netbuf指针、数据包的长度
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年7月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hwal_send_others_bss_data(const oal_netbuf_stru *netbuf)
{
    hi_u32 *recv_buf = HI_NULL;
    hi_s32 frame_len;
    hi_s8 rssi;
    mac_ieee80211_frame_stru *frame_hdr = HI_NULL;
    hi_u8 filter;

    if (netbuf == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hwal_send_others_bss_data::pst_netbuf is null");
        return HI_ERR_CODE_PTR_NULL;
    }
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    if (rx_ctrl == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hwal_send_others_bss_data::rx_ctrl is null");
        return HI_ERR_CODE_PTR_NULL;
    }
    recv_buf = rx_ctrl->pul_mac_hdr_start_addr;
    frame_len = (hi_s32)rx_ctrl->us_frame_len;
    rssi = rx_ctrl->rssi_dbm;

    frame_hdr = (mac_ieee80211_frame_stru *)recv_buf;

    /* mdata_en/udata_en/mmngt_en/umngt_en */
    filter = hwal_get_promis_filter();
    if ((filter == 0x3) || (filter == 0x2) || (filter == 0x1)) { /* 数据帧 0011 0x3 0x2 0x1 */
        if (frame_hdr->frame_control.type != WLAN_DATA_BASICTYPE) {
            return HI_SUCCESS;
        }
    }
    if ((filter == 0xC) || (filter == 0x8) || (filter == 0x4)) { /* 管理帧 1100 0xC 0x8 0x4 */
        if (frame_hdr->frame_control.type != WLAN_MANAGEMENT) {
            return HI_SUCCESS;
        }
    }
    if ((filter == 0x2) || (filter == 0x8)) { /* 单播 管理帧+数据帧 0x2 0x8 */
        if (ether_is_multicast(frame_hdr->auc_address1)) {
            return HI_SUCCESS;
        }
    }
    if ((filter == 0x1) || (filter == 0x4)) { /* 多播+广播 管理帧+数据帧 0x1 0x4 */
        if (!ether_is_multicast(frame_hdr->auc_address1)) {
            return HI_SUCCESS;
        }
    }

    if (g_upload_frame_func != HI_NULL) {
        g_upload_frame_func(recv_buf, frame_len, rssi);
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hisi_wlan_register_upload_frame_cb
 功能描述  : 供用户调用，注册处理驱动上报的数据包函数
 输入参数  : func 函数指针
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年1月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hisi_wlan_register_upload_frame_cb(hisi_upload_frame_cb func)
{
    hi_task_lock();
    g_upload_frame_func = func;
    hi_task_unlock();
    return HI_SUCCESS;
}
#endif // #ifdef _PRE_WLAN_FEATURE_PROMIS

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
