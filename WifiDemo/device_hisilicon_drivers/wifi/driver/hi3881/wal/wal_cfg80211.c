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
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "hmac_ext_if.h"
#include "wal_cfg80211.h"
#include "wal_scan.h"
#include "wal_main.h"
#include "wal_regdb.h"
#include "wal_ioctl.h"
#include "wal_hipriv.h"
#include "net_adpater.h"
#include "wal_customize.h"
#include "mac_ie.h"
#include "wal_event_msg.h"
#include "wal_cfg80211_apt.h"
#include "hdf_wifi_event.h"
#include "plat_pm_wlan.h"
#include "hi_config.h"
#include "net_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern hi_u32 hi_get_tick(hi_void);

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#define WIFI_G_RATES           (g_wifi_rates + 0)
#define WIFI_G_RATES_SIZE      12
#define WIFI_ROC_TIMEOUT       200

/* 设备支持的速率 */
static oal_ieee80211_rate g_wifi_rates[] = {
    ratetab_ent(10,  0x1,   0),
    ratetab_ent(20,  0x2,   0),
    ratetab_ent(55,  0x4,   0),
    ratetab_ent(110, 0x8,   0),
    ratetab_ent(60,  0x10,  0),
    ratetab_ent(90,  0x20,  0),
    ratetab_ent(120, 0x40,  0),
    ratetab_ent(180, 0x80,  0),
    ratetab_ent(240, 0x100, 0),
    ratetab_ent(360, 0x200, 0),
    ratetab_ent(480, 0x400, 0),
    ratetab_ent(540, 0x800, 0),
};

/* 2.4G 频段 */
oal_ieee80211_channel g_wifi_2ghz_channels[] = {
    chan2g(1, 2412, 0),
    chan2g(2, 2417, 0),
    chan2g(3, 2422, 0),
    chan2g(4, 2427, 0),
    chan2g(5, 2432, 0),
    chan2g(6, 2437, 0),
    chan2g(7, 2442, 0),
    chan2g(8, 2447, 0),
    chan2g(9, 2452, 0),
    chan2g(10, 2457, 0),
    chan2g(11, 2462, 0),
    chan2g(12, 2467, 0),
    chan2g(13, 2472, 0),
    chan2g(14, 2484, 0),
};

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* 设备支持的加密套件 */
static const hi_u32 g_wifi_cipher_suites[] = {
    WLAN_CIPHER_SUITE_WEP40,
    WLAN_CIPHER_SUITE_WEP104,
    WLAN_CIPHER_SUITE_TKIP,
    WLAN_CIPHER_SUITE_CCMP,
    WLAN_CIPHER_SUITE_AES_CMAC,
    WLAN_CIPHER_SUITE_SMS4,
};
#endif

/* 2.4G 频段信息 */
static oal_ieee80211_supported_band g_wifi_band_2ghz = {
    .channels   = g_wifi_2ghz_channels,
    .n_channels = sizeof(g_wifi_2ghz_channels) / sizeof(oal_ieee80211_channel),
    .bitrates   = WIFI_G_RATES,
    .n_bitrates = WIFI_G_RATES_SIZE,
    .ht_cap = {
        .ht_supported = HI_TRUE,
        .cap = IEEE80211_HT_CAP_SUP_WIDTH_20_40 | IEEE80211_HT_CAP_SGI_20 | IEEE80211_HT_CAP_SGI_40,
    },
};

#ifdef _PRE_WLAN_FEATURE_P2P
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_workqueue_stru *g_del_virtual_inf_workqueue = HI_NULL;

static oal_ieee80211_iface_limit g_sta_p2p_limits[] = {
    {
        .max = 2,
        .types = bit(NL80211_IFTYPE_STATION),
    },
    /* 1131添加一个AP类型接口 */
    {
        .max = 1,
        .types = bit(NL80211_IFTYPE_AP),
    },
    {
        .max = 2,
        .types = bit(NL80211_IFTYPE_P2P_GO) | BIT(NL80211_IFTYPE_P2P_CLIENT),
    },
#ifdef _PRE_WLAN_FEATURE_MESH
    {
        .max = 1,
        .types = bit(NL80211_IFTYPE_MESH_POINT),
    },
#endif
};

static oal_ieee80211_iface_combination
g_sta_p2p_iface_combinations[] = {
    {
        .num_different_channels = 2,
        .max_interfaces = 3,
        .limits = g_sta_p2p_limits,
        .n_limits = hi_array_size(g_sta_p2p_limits),
    },
};

/* There isn't a lot of sense in it, but you can transmit anything you like */
static const struct ieee80211_txrx_stypes
g_wal_cfg80211_default_mgmt_stypes[NUM_NL80211_IFTYPES] = {
    [NL80211_IFTYPE_ADHOC] = {
        .tx = 0xffff,
        .rx = bit(IEEE80211_STYPE_ACTION >> 4)
    },
    [NL80211_IFTYPE_STATION] = {
        .tx = 0xffff,
        .rx = bit(IEEE80211_STYPE_ACTION >> 4) |
        bit(IEEE80211_STYPE_PROBE_REQ >> 4)
    },
    [NL80211_IFTYPE_AP] = {
        .tx = 0xffff,
        .rx = bit(IEEE80211_STYPE_ASSOC_REQ >> 4) |
        bit(IEEE80211_STYPE_REASSOC_REQ >> 4) |
        bit(IEEE80211_STYPE_PROBE_REQ >> 4) |
        bit(IEEE80211_STYPE_DISASSOC >> 4) |
        bit(IEEE80211_STYPE_AUTH >> 4) |
        bit(IEEE80211_STYPE_DEAUTH >> 4) |
        bit(IEEE80211_STYPE_ACTION >> 4)
    },
    [NL80211_IFTYPE_AP_VLAN] = {
        /* copy AP */
        .tx = 0xffff,
        .rx = bit(IEEE80211_STYPE_ASSOC_REQ >> 4) |
        bit(IEEE80211_STYPE_REASSOC_REQ >> 4) |
        bit(IEEE80211_STYPE_PROBE_REQ >> 4) |
        bit(IEEE80211_STYPE_DISASSOC >> 4) |
        bit(IEEE80211_STYPE_AUTH >> 4) |
        bit(IEEE80211_STYPE_DEAUTH >> 4) |
        bit(IEEE80211_STYPE_ACTION >> 4)
    },
#if defined(_PRE_WLAN_FEATURE_P2P)
    [NL80211_IFTYPE_P2P_CLIENT] = {
        .tx = 0xffff,
        .rx = bit(IEEE80211_STYPE_ACTION >> 4) |
        bit(IEEE80211_STYPE_PROBE_REQ >> 4)
    },
    [NL80211_IFTYPE_P2P_GO] = {
        .tx = 0xffff,
        .rx = bit(IEEE80211_STYPE_ASSOC_REQ >> 4) |
        bit(IEEE80211_STYPE_REASSOC_REQ >> 4) |
        bit(IEEE80211_STYPE_PROBE_REQ >> 4) |
        bit(IEEE80211_STYPE_DISASSOC >> 4) |
        bit(IEEE80211_STYPE_AUTH >> 4) |
        bit(IEEE80211_STYPE_DEAUTH >> 4) |
        bit(IEEE80211_STYPE_ACTION >> 4)
    },
    [NL80211_IFTYPE_P2P_DEVICE] = {
        .tx = 0xffff,
        .rx = bit(IEEE80211_STYPE_ACTION >> 4) |
        bit(IEEE80211_STYPE_PROBE_REQ >> 4)
    },
#endif /* WL_CFG80211_P2P_DEV_IF */
#ifdef _PRE_WLAN_FEATURE_MESH
    [NL80211_IFTYPE_MESH_POINT] = {
        .tx = 0xffff,
        .rx = bit(IEEE80211_STYPE_ASSOC_REQ >> 4) |
        bit(IEEE80211_STYPE_REASSOC_REQ >> 4) |
        bit(IEEE80211_STYPE_PROBE_REQ >> 4) |
        bit(IEEE80211_STYPE_DISASSOC >> 4) |
        bit(IEEE80211_STYPE_AUTH >> 4) |
        bit(IEEE80211_STYPE_DEAUTH >> 4) |
        bit(IEEE80211_STYPE_ACTION >> 4)
    },
#endif
};
#endif
#endif

hi_u8               g_cookie_array_bitmap = 0;   /* 每个bit 表示cookie array 中是否使用，1 - 已使用；0 - 未使用 */
cookie_arry_stru    g_cookie_array[WAL_COOKIE_ARRAY_SIZE];

/* insmod 设置vap mode */
int g_mode = WAL_WIFI_MODE_STA; /* wifi 默认STA_AP 共存模式 */
module_param(g_mode, int, 0644);
/* insmod 设置vap bandwith */
int g_bw = WAL_WIFI_BW_LEGACY_20M;
module_param(g_bw, int, 0644);
/* insmod 设置vap protocol */
int g_proto = WAL_PHY_MODE_11N;
module_param(g_proto, int, 0644);

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
oal_ieee80211_channel *wal_get_g_wifi_2ghz_channels(hi_void)
{
    return g_wifi_2ghz_channels;
}

#ifdef _PRE_WLAN_FEATURE_P2P
/* ****************************************************************************
 函 数 名  : wal_is_p2p_group_exist
 功能描述  : 检查是否存在P2P group
 输入参数  : mac_device_stru *pst_mac_device
 输出参数  : 无
 返 回 值  : static hi_u32 HI_TRUE    存在P2P group
                           HI_FALSE   不存在P2P group
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年9月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_is_p2p_group_exist(mac_device_stru *mac_dev)
{
    if (hmac_p2p_check_vap_num(mac_dev, WLAN_P2P_GO_MODE) != HI_SUCCESS ||
        hmac_p2p_check_vap_num(mac_dev, WLAN_P2P_CL_MODE) != HI_SUCCESS) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}
/* ****************************************************************************
 函 数 名  : wal_del_p2p_group
 功能描述  : 删除P2P group
 输入参数  : mac_device_stru *pst_mac_device
 输出参数  : 无
 返 回 值  : static hi_void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年9月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_del_p2p_group(const mac_device_stru *mac_dev)
{
    mac_vap_stru        *mac_vap  = HI_NULL;
    hmac_vap_stru       *hmac_vap = HI_NULL;
    oal_net_device_stru *netdev   = HI_NULL;

    for (hi_u8 vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (oal_unlikely(mac_vap == HI_NULL)) {
            oam_warning_log1(0, OAM_SF_P2P, "{wal_del_p2p_group::mac vap Err! vapId = %u}",
                mac_dev->auc_vap_id[vap_idx]);
            continue;
        }

        hmac_vap = hmac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (oal_unlikely(hmac_vap == HI_NULL)) {
            oam_warning_log1(0, OAM_SF_P2P, "{wal_del_p2p_group::get hmac vap resource failed! vap id is %u}",
                mac_dev->auc_vap_id[vap_idx]);
            continue;
        }

        netdev = hmac_vap->net_device;
        if (oal_unlikely(netdev == HI_NULL)) {
            oam_warning_log1(0, OAM_SF_P2P, "{wal_del_p2p_group::netdev Err! vap id = %u}",
                mac_dev->auc_vap_id[vap_idx]);
            continue;
        }

        if (is_p2p_go(mac_vap) || is_p2p_cl(mac_vap)) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
            mac_cfg_del_vap_param_stru del_vap_param;

            /* 规则6.6：禁止使用内存操作类危险函数 例外(1)对固定长度的数组进行初始化 */
            memset_s(&del_vap_param, sizeof(del_vap_param), 0, sizeof(del_vap_param));

            del_vap_param.net_dev = netdev;
            del_vap_param.vap_mode = mac_vap->vap_mode;
            del_vap_param.p2p_mode = mac_get_p2p_mode(mac_vap);
#endif
            oam_warning_log2(mac_vap->vap_id, OAM_SF_P2P, "{wal_del_p2p_group:: vap mode[%d], p2p mode[%d]}\r\n",
                mac_vap->vap_mode, mac_get_p2p_mode(mac_vap));
            /* 删除已经存在的P2P group */
            wal_force_scan_complete(netdev);
            wal_stop_vap(netdev);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
            if (wal_cfg80211_del_vap(&del_vap_param) == HI_SUCCESS) {
                wal_cfg80211_unregister_netdev(netdev);
            }
#else
            if (wal_deinit_wlan_vap(netdev) == HI_SUCCESS) {
                /* 去注册netdev */
                oal_net_unregister_netdev(netdev);
                oal_net_free_netdev(netdev);
            }
#endif
        }
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : wal_set_p2p_status
 功能描述  :设置p2p 为对应状态
 输入参数  : oal_net_device_stru *net_dev, wlan_p2p_status_enum_uint32 en_status
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_set_p2p_status(oal_net_device_stru *netdev, wlan_p2p_status_enum_uint32 status)
{
    hi_u32 ret;
    wal_msg_write_stru write_msg;

    /* 填写消息头 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_P2P_STATUS, sizeof(wlan_p2p_status_enum_uint32));
    /* 填写消息体 */
    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value),
                 &status, sizeof(wlan_p2p_status_enum_uint32)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_set_p2p_status::mem safe function err!}");
        return HI_FAIL;
    }
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(wlan_p2p_status_enum_uint32), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_set_p2p_status::return err code [%u]!}\r\n", ret);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}
#endif

hi_u32 wal_cfg80211_add_virtual_intf_p2p_proc(mac_device_stru *mac_device)
{
    /* 添加net_device 前先判断当前是否正在删除net_device 状态，
        如果正在删除net_device，则等待删除完成，再添加 */
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    if (hmac_dev->p2p_intf_status & bit(P2P_STATUS_IF_DELETING)) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf:Released lock, wait till IF_DEL is complete}");
        hi_s32 l_timeout = hi_wait_event_timeout(hmac_dev->netif_change_event,
            ((hmac_dev->p2p_intf_status & bit(P2P_STATUS_IF_DELETING)) == HI_FALSE),
            WAL_MAX_WAIT_TIME / HI_MILLISECOND_PER_TICK);
        if (l_timeout > 0) {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::IF DEL is success!}\r\n");
        } else {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::timeount < 0, return -EAGAIN!}\r\n");
            return HI_FAIL;
        }
    }

    /* 检查wifi 驱动中，P2P group 是否已经创建，如果P2P group 已经创建，
        则将该P2P group 删除，并且重新创建P2P group */
    if (wal_is_p2p_group_exist(mac_device) == HI_TRUE) {
        oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf::found exist p2p group, delet it first!}");
        if (wal_del_p2p_group(mac_device) != HI_SUCCESS) {
            return HI_FAIL;
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_remain_on_channel
 功能描述  : 保持在指定信道
 输入参数  : [1]wiphy
             [2]wdev
             [3]chan
             [4]duration
             [5]pull_cookie
 输出参数  : 无
 返 回 值  : static hi_s32
**************************************************************************** */
hi_u32 wal_p2p_stop_roc(mac_vap_stru *mac_vap, oal_net_device_stru *netdev)
{
#ifdef _PRE_WLAN_FEATURE_P2P
    if (mac_vap->vap_state == MAC_VAP_STATE_STA_LISTEN) {
        hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
        if (hmac_vap == HI_NULL) {
            oam_error_log0(0, OAM_SF_P2P, "{wal_p2p_stop_roc:: pst_hmac_vap null!}\r\n");
            return HI_FAIL;
        }
        hmac_vap->en_wait_roc_end = HI_TRUE;
        OAL_INIT_COMPLETION(&(hmac_vap->st_roc_end_ready));
        wal_force_scan_complete(netdev);
        if (oal_wait_for_completion_timeout(&(hmac_vap->st_roc_end_ready),
            (hi_u32)OAL_MSECS_TO_JIFFIES(WIFI_ROC_TIMEOUT)) == 0) {
            oam_error_log0(0, OAM_SF_P2P, "{wal_p2p_stop_roc::cancel old roc timeout!}");
            return HI_FAIL;
        }
    }
#endif
    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 wal_drv_remain_on_channel(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, oal_ieee80211_channel *chan,
    hi_u32 duration, hi_u64 *pull_cookie, wlan_ieee80211_roc_type_uint8 en_roc_type)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_drv_remain_on_channel(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, oal_ieee80211_channel *chan,
    hi_u32 duration, hi_u64 *pull_cookie, wlan_ieee80211_roc_type_uint8 en_roc_type)
#endif
{
    /* 1.1 入参检查 */
    if ((wiphy == HI_NULL) || (wdev == HI_NULL) || (chan == HI_NULL) || (pull_cookie == HI_NULL)) {
        oam_error_log0(0, OAM_SF_P2P, "{wal_drv_remain_on_channel::wiphy or wdev or chan or pull_cookie is NULL}");
        goto fail;
    }

    oal_net_device_stru *netdev = wdev->netdev;
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_P2P, "{wal_drv_remain_on_channel::pst_netdev ptr is NULL!}\r\n");
        goto fail;
    }

    mac_device_stru *mac_device     = (mac_device_stru *)mac_res_get_dev();
    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
#ifdef _PRE_WLAN_FEATURE_WAPI
    if (hmac_user_is_wapi_connected() == HI_TRUE) {
        oam_warning_log0(0, OAM_SF_CFG, "{stop p2p remaining under wapi!}");
        goto fail;
    }
#endif /* #ifdef _PRE_WLAN_FEATURE_WAPI */
    if (mac_vap->vap_state == MAC_VAP_STATE_STA_LISTEN) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_P2P, "{wal_drv_remain_on_channel::new roc type[%d],cancel old roc!}",
            en_roc_type);
        if (wal_p2p_stop_roc(mac_vap, netdev) != HI_SUCCESS) {
            oam_warning_log0(0, OAM_SF_CFG, "{wal_p2p_stop_roc fail!}");
            goto fail;
        }
    }

    /* 2.1 消息参数准备 */
    hi_u16 us_center_freq = chan->center_freq;
    hi_s32 l_channel = (hi_s32)oal_ieee80211_frequency_to_channel((hi_s32)us_center_freq);
    mac_remain_on_channel_param_stru remain_on_channel = {0};
    remain_on_channel.uc_listen_channel = (hi_u8)l_channel;
    remain_on_channel.listen_duration = duration;
    remain_on_channel.st_listen_channel = *chan;
    remain_on_channel.listen_channel_type = WLAN_BAND_WIDTH_20M;

    if ((hi_u8)chan->band == IEEE80211_BAND_2GHZ) {
        remain_on_channel.band = WLAN_BAND_2G;
    } else {
        oam_warning_log1(0, OAM_SF_P2P, "{wal_drv_remain_on_channel::wrong band type[%d]!}\r\n", chan->band);
        goto fail;
    }
    if (en_roc_type == IEEE80211_ROC_TYPE_NORMAL) {
        *pull_cookie = ++mac_device->p2p_info.ull_last_roc_id;
        if (*pull_cookie == 0) {
            *pull_cookie = ++mac_device->p2p_info.ull_last_roc_id;
        }

    /* 保存cookie 值，下发给HMAC 和DMAC */
    remain_on_channel.ull_cookie = mac_device->p2p_info.ull_last_roc_id;
    }
    /* 抛事件给驱动 */
    hi_u32 ret = wal_cfg80211_start_req(netdev, &remain_on_channel, sizeof(mac_remain_on_channel_param_stru),
        WLAN_CFGID_CFG80211_REMAIN_ON_CHANNEL, HI_TRUE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_drv_remain_on_channel::wal_send_cfg_event return err code:[%d]}", ret);
        goto fail;
    }

if (en_roc_type == IEEE80211_ROC_TYPE_NORMAL) {
    /* 上报暂停在指定信道成功 */
#if (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX) && !defined(_PRE_HDF_LINUX)
    cfg80211_ready_on_channel(wdev, *pull_cookie, chan, duration, GFP_KERNEL);
#else
    ret = HdfWifiEventRemainOnChannel(netdev, chan->center_freq, duration);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_drv_remain_on_channel::cfg80211_remain_on_channel failed[%u]}\r\n",
            ret);
        goto fail;
    }
#endif
}
    oam_warning_log4(0, OAM_SF_P2P,
        "{wal_drv_remain_on_channel::SUCC! l_channel = %d, ul_duration = %d, cookie 0x%x, band = %u!}\r\n",
        l_channel, duration, *pull_cookie, remain_on_channel.band);

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_remain_on_channel(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, oal_ieee80211_channel *chan,
    hi_u32 duration, hi_u64 *pull_cookie)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_remain_on_channel(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, oal_ieee80211_channel *chan,
    hi_u32 duration, hi_u64 *pull_cookie)
#endif
{
    return wal_drv_remain_on_channel(wiphy, wdev, chan, duration, pull_cookie, IEEE80211_ROC_TYPE_NORMAL);
}
/* ****************************************************************************
 函 数 名  : wal_cfg80211_cancel_remain_on_channel
 功能描述  : 停止保持在指定信道
 输入参数  : [1]wiphy
             [2]wdev
             [3]ull_cookie
 输出参数  : 无
 返 回 值  : static hi_s32
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_cancel_remain_on_channel(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, hi_u64 ull_cookie)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_cancel_remain_on_channel(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, hi_u64 ull_cookie)
#endif
{
    oal_net_device_stru             *netdev                   = HI_NULL;
    mac_remain_on_channel_param_stru cancel_remain_on_channel = {0};
    hi_u32                           ret;

    hi_unref_param(wiphy);
    hi_unref_param(ull_cookie);
    netdev = wdev->netdev;

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &cancel_remain_on_channel, sizeof(mac_remain_on_channel_param_stru),
        WLAN_CFGID_CFG80211_CANCEL_REMAIN_ON_CHANNEL, HI_TRUE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_P2P,
            "{wal_cfg80211_cancel_remain_on_channel::wal_send_cfg_event return err code:[%u]!}", ret);
        goto fail;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}
#endif /* #ifdef _PRE_WLAN_FEATURE_P2P */

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : wal_cfg80211_register_netdev
 功能描述  : 内核注册指定类型的net_device,用于需要解mutex lock的应用
 输入参数  : mac_device_stru *pst_hmac_device
             oal_net_device_stru *pst_net_dev
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年7月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#ifndef _PRE_HDF_LINUX
hi_u32 wal_cfg80211_register_netdev(oal_net_device_stru *netdev)
{
    hi_u8 rollback_lock = HI_FALSE;

    /*  DTS2015022603795 nl80211 netlink pre diot 中会获取rntl_lock互斥锁，注册net_device 会获取rntl_lock互斥锁，造成了死锁 */
    if (rtnl_is_locked()) {
        rtnl_unlock();
        rollback_lock = HI_TRUE;
    }

    /* 内核注册net_device, 只返回0 */
    hi_u32 ret = (hi_u32)oal_net_register_netdev(netdev);
    if (rollback_lock) {
        rtnl_lock();
    }
    return ret;
}

hi_void wal_cfg80211_unregister_netdev(oal_net_device_stru *netdev)
{
    hi_u8 rollback_lock = HI_FALSE;

    if (rtnl_is_locked()) {
        rtnl_unlock();
        rollback_lock = HI_TRUE;
    }
    /* 去注册netdev */
    oal_net_unregister_netdev(netdev);

    if (rollback_lock) {
        rtnl_lock();
    }
}
#endif

/* ****************************************************************************
 函 数 名  : wal_find_wmm_uapsd
 功能描述  : 查找内核下发的beacon_info中的wmm ie中wmm uapsd是否使能
 输入参数  : hi_u8 *puc_frame_body, hi_s32 l_len

 输出参数  : 无
 返 回 值  : uapsd使能，返回HI_TRUE，否则，返回HI_FALSE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年8月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_find_wmm_uapsd(hi_u8 *puc_frame_body, hi_s32 l_len)
{
    hi_s32    l_index = 0;
    hi_u8     auc_oui[MAC_OUI_LEN];

    auc_oui[0] = (hi_u8)MAC_WLAN_OUI_MICRO0;
    auc_oui[1] = (hi_u8)MAC_WLAN_OUI_MICRO1;
    auc_oui[2] = (hi_u8)MAC_WLAN_OUI_MICRO2; /* 2: 数组第3位 */
    /* 判断 WMM UAPSD 是否使能 */
    while (l_index < l_len) {
        if ((puc_frame_body[l_index] == MAC_EID_WMM) &&
            (memcmp(puc_frame_body + l_index + 2, auc_oui, MAC_OUI_LEN) == 0) && /* 2：偏移位 */
            (puc_frame_body[l_index + 2 + MAC_OUI_LEN] == MAC_OUITYPE_WMM) && /* 2：偏移位 */
            (puc_frame_body[l_index + MAC_WMM_QOS_INFO_POS] & BIT7)) {
            return HI_TRUE;
        } else {
            l_index += (MAC_IE_HDR_LEN + puc_frame_body[l_index + 1]);
        }
    }

    return HI_FALSE;
}

hi_u32 wal_cfg80211_open_wmm(oal_net_device_stru *netdev, hi_u16 us_len, hi_u8 *puc_param)
{
    mac_vap_stru *mac_vap = HI_NULL;
    wal_msg_write_stru write_msg;
    hi_u32 ret;
    mac_vap = oal_net_dev_priv(netdev);
    if (oal_unlikely(mac_vap == HI_NULL || puc_param == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_open_wmm::pst_mac_vap/puc_param is NULL!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 针对配置vap做保护 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_CONFIG) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{wal_cfg80211_open_wmm::this is config vap! can't get info.}");
        return HI_FAIL;
    }

    /* 填写事件头 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_WMM_SWITCH, sizeof(hi_u8));
    /* 填写消息体 */
    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), puc_param, sizeof(hi_u8)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_open_wmm::mem safe function err!}");
        return HI_FAIL;
    }

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u8),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_open_wmm:return err code %d!}\r\n", ret);
        return HI_FAIL;
    }

    return ret;
}

/* ****************************************************************************
 函 数 名  : wal_parse_wmm_ie
 功能描述  : 解析内核传递过来beacon信息中的Wmm信息元素
 输入参数  : oal_beacon_parameters *pst_beacon_info

 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年7月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_parse_wmm_ie(oal_net_device_stru *netdev, mac_vap_stru *mac_vap, oal_beacon_parameters *beacon_info)
{
    hi_u8               *puc_wmm_ie = HI_NULL;
    hi_u16               us_len = sizeof(hi_u8);
    hi_u8                wmm = HI_TRUE;
    hi_u32               ret = HI_SUCCESS;

    hi_u8                uapsd;
    wal_msg_write_stru   write_msg;

    /*  查找wmm_ie  */
    puc_wmm_ie = mac_find_vendor_ie(MAC_WLAN_OUI_MICROSOFT, MAC_WLAN_OUI_TYPE_MICROSOFT_WMM, beacon_info->tail,
        beacon_info->tail_len);
    if (puc_wmm_ie == HI_NULL) {
        /* wmm ie未找到，则说明wmm 关 */
        wmm = HI_FALSE;
    } else { /*  找到wmm ie，顺便判断下uapsd是否使能 */
        if (HI_FALSE == wal_find_wmm_uapsd(beacon_info->tail, beacon_info->tail_len)) {
            /* 对应UAPSD 关 */
            uapsd = HI_FALSE;
            oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{wal_parse_wmm_ie::uapsd is disabled!!}");
        }

        /* 填写 msg 消息头 */
        wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_UAPSD_EN, sizeof(hi_u32));
        /* 填写 msg 消息体 */
        uapsd = HI_FALSE;

        if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), &uapsd, sizeof(hi_u32)) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "{wal_parse_wmm_ie::mem safe function err!}");
            return HI_FAIL;
        }

        /* 发送消息 */
        ret = (hi_u32)wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u32),
            (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            ret = HI_FAIL;
            oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{wal_parse_wmm_ie::uapsd switch set failed[%u].}", ret);
        }
    }

    /*  wmm 开启/关闭 标记  */
    ret = wal_cfg80211_open_wmm(netdev, us_len, &wmm);
    if (ret != HI_SUCCESS) {
        ret = HI_FAIL;
        oam_warning_log0(0, OAM_SF_TX, "{wal_parse_wmm_ie::can not open wmm!}\r\n");
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 从已保存信息里获取ptk,gtk等密钥
 输入参数  : [1]wiphy
             [2]netdev
             [3]p_cfg80211_get_key_info
             [4]cookie
             [5]callback
 输出参数  : hi_u32
 返 回 值  : 0:成功,其他:失败
**************************************************************************** */
static hi_s32 wal_cfg80211_get_key(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index, bool pairwise,
    const hi_u8 *puc_mac_addr, hi_void *cookie, hi_void (*callback)(hi_void *cookie, oal_key_params_stru *))
{
    wal_msg_write_stru    write_msg;
    mac_getkey_param_stru payload  = {0};
    hi_u8                 mac_addr[WLAN_MAC_ADDR_LEN];
    wal_msg_stru         *rsp_msg = HI_NULL;

    /* 1.1 入参检查 */
    if ((wiphy == HI_NULL) || (netdev == HI_NULL) || (cookie == HI_NULL) || (callback == HI_NULL)) {
        oam_error_log4(0, OAM_SF_ANY,
            "{wal_cfg80211_get_key::Param ERR, wiphy, netdev, cookie, callback %d, %d, %d, %d}", wiphy, netdev, cookie,
            callback);
        goto fail;
    }

    /* 2.1 消息参数准备 */
    payload.netdev    = netdev;
    payload.key_index = key_index;

    if (puc_mac_addr != HI_NULL) {
        /* 不能使用内核下发的mac指针，可能被释放，需要拷贝到本地再使用 */
        if (memcpy_s(mac_addr, WLAN_MAC_ADDR_LEN, puc_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_get_key::mem safe function err!}");
            goto fail;
        }
        payload.puc_mac_addr = mac_addr;
    } else {
        payload.puc_mac_addr = HI_NULL;
    }

    payload.pairwise = pairwise;
    payload.cookie = cookie;
    payload.callback = callback;

    oam_info_log2(0, OAM_SF_ANY, "{wal_cfg80211_get_key::key_idx:%d,en_pairwise:%d}", key_index, payload.pairwise);
    if (puc_mac_addr != HI_NULL) {
        oam_info_log3(0, OAM_SF_ANY, "{wal_cfg80211_get_key::MAC ADDR: XX:XX:XX:%02X:%02X:%02X!}\r\n", puc_mac_addr[3],
            puc_mac_addr[4], puc_mac_addr[5]); /* mac addr 0:1:2:3:4:5 */
    } else {
        oam_info_log0(0, OAM_SF_ANY, "{wal_cfg80211_get_key::MAC ADDR IS NULL!}\r\n");
    }
    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    /* 3.1 填写 msg 消息头 */
    write_msg.wid = WLAN_CFGID_GET_KEY;
    write_msg.us_len = sizeof(mac_getkey_param_stru);

    /* 3.2 填写 msg 消息体 */
    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), &payload, sizeof(mac_getkey_param_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_get_key::mem safe function err!}");
        goto fail;
    }

    /* 由于消息中使用了局部变量指针，因此需要将发送该函数设置为同步，否则hmac处理时会使用野指针 */
    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_getkey_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_get_key::return err code [%u]!}", ret);
        goto fail;
    }

    if (wal_check_and_release_msg_resp(rsp_msg) != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_get_key::wal_check_and_release_msg_resp failed.}");
        goto fail;
    }

    return HI_SUCCESS;
fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 功能描述  : 使配置的密钥生效.PMF 特性使用，配置管理密钥
 输入参数  : [1]wiphy
             [2]netdev
             [3]key_index
 输出参数  : hi_u32
 返 回 值  : 0:成功,其他:失败
**************************************************************************** */
hi_s32 wal_cfg80211_set_default_mgmt_key(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index)
{
    /* 设置管理密钥 */
    return -HI_FAIL;
}

/* ****************************************************************************
 功能描述  : 设置wiphy设备的 参数，RTS 门限阈值，分片门限阈值
 输入参数  : oal_wiphy_stru *pst_wiphy
             hi_u32 ul_changed
 返 回 值  : static hi_s32
 修改历史      :
  1.日    期   : 2013年10月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_s32 wal_cfg80211_set_wiphy_params(oal_wiphy_stru *wiphy, hi_u32 changed)
{
    /* 通过HOSTAPD 设置RTS 门限，分片门限 采用接口wal_ioctl_set_frag， wal_ioctl_set_rts */
    oam_warning_log0(0, OAM_SF_CFG,
        "{wal_cfg80211_set_wiphy_params::should not call this function. call wal_ioctl_set_frag/wal_ioctl_set_rts!}\r\n");
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 修改bss参数信息
 输入参数  : oal_wiphy_stru        *pst_wiphy
             oal_net_device_stru   *pst_netdev
             oal_bss_parameters    *pst_bss_params
 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#ifndef _PRE_HDF_LINUX
static hi_s32 wal_cfg80211_change_bss(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev,
    oal_bss_parameters *bss_params)
{
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 打印上层下发的调度扫描请求信息
 修改历史      :
  1.日    期   : 2015年6月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void wal_cfg80211_print_sched_scan_req_info(oal_cfg80211_sched_scan_request_stru *request)
{
    hi_char    ac_tmp_buff[200]; /* 200 buffer元素个数 */
    hi_s32     l_loop = 0;

    /* 打印基本参数 */
/* HI1131C modify begin */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oam_warning_log3(0, OAM_SF_SCAN, "wal_cfg80211_print_sched_scan_req_info::channels[%d], flags[%d], rssi_thold[%d]",
        request->n_channels, request->flags, request->min_rssi_thold);
#endif
    /* HI1131C modify end */
    /* 打印ssid集合的信息 */
    for (l_loop = 0; l_loop < request->n_match_sets; l_loop++) {
        if (memset_s(ac_tmp_buff, sizeof(ac_tmp_buff), 0, sizeof(ac_tmp_buff)) != EOK) {
            continue;
        }
        if (snprintf_s(ac_tmp_buff, sizeof(ac_tmp_buff), sizeof(ac_tmp_buff) - 1,
            "mactch_sets[%d] info, ssid_len[%d], ssid: %s.\n", l_loop, request->match_sets[l_loop].ssid.ssid_len,
            request->match_sets[l_loop].ssid.ssid) == -1) {
            oam_error_log0(0, OAM_SF_CFG, "wal_cfg80211_print_sched_scan_req_info:: l_loop snprintf_s fail.");
            continue;
        }
    }

    for (l_loop = 0; l_loop < request->n_ssids; l_loop++) {
        if (memset_s(ac_tmp_buff, sizeof(ac_tmp_buff), 0, sizeof(ac_tmp_buff)) != EOK) {
            continue;
        }
        if (snprintf_s(ac_tmp_buff, sizeof(ac_tmp_buff), sizeof(ac_tmp_buff) - 1,
            "ssids[%d] info, ssid_len[%d], ssid: %s.\n", l_loop, request->ssids[l_loop].ssid_len,
            request->ssids[l_loop].ssid) == -1) {
            oam_error_log0(0, OAM_SF_CFG, "wal_cfg80211_print_sched_scan_req_info:: snprintf_s failed.");
            continue;
        }
    }

    return;
}

/* ****************************************************************************
 功能描述  : 调度扫描启动
 输入参数  : oal_wiphy_stru                         *pst_wiphy
             oal_net_device_stru                    *pst_netdev
             oal_cfg80211_sched_scan_request_stru   *pst_request
 修改历史      :
  1.日    期   : 2015年6月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#ifndef _PRE_HDF_LINUX
static hi_s32 wal_cfg80211_sched_scan_start(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev,
    oal_cfg80211_sched_scan_request_stru *request)
{
    oal_cfg80211_ssid_stru *ssid_tmp = HI_NULL;
    mac_pno_scan_stru pno_scan_info;

    /* 参数合法性检查 */
    if ((wiphy == HI_NULL) || (netdev == HI_NULL) || (request == HI_NULL)) {
        oam_error_log3(0, OAM_SF_CFG,
            "{wal_cfg80211_sched_scan_start::input param pointer is NULL, pst_wiphy[%p], pst_netdev[%p], "
            "pst_request[%p]}",
            wiphy, netdev, request);
        goto fail;
    }

    /* 通过net_device 找到对应的mac_device_stru 结构 */
    mac_vap_stru     *mac_vap   = oal_net_dev_priv(netdev);
    hmac_device_stru *hmac_dev  = hmac_get_device_stru();
    hmac_scan_stru   *scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 如果当前设备处于扫描状态，不启动调度扫描 */
    if (scan_mgmt->request != HI_NULL) {
        oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_sched_scan_start:: device is busy, don't start sched scan!}");
        goto fail;
    }

    /* 检测内核下发的需要匹配的ssid集合的个数是否合法 */
    if (request->n_match_sets <= 0) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN, "{wal_cfg80211_sched_scan_start::match_sets = %d!}",
            request->n_match_sets);
        goto fail;
    }

    /* 初始化pno扫描的结构体信息
       安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(&pno_scan_info, sizeof(mac_pno_scan_stru), 0, sizeof(mac_pno_scan_stru));

    /* 将内核下发的匹配的ssid集合复制到本地 */
    for (hi_s32 l_loop = 0; l_loop < request->n_match_sets; l_loop++) {
        ssid_tmp = (oal_cfg80211_ssid_stru *)&(request->match_sets[l_loop].ssid);
        if (memcpy_s(pno_scan_info.ac_match_ssid_set[l_loop], WLAN_SSID_MAX_LEN,
                     ssid_tmp->ssid, ssid_tmp->ssid_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_sched_scan_start::mem safe function err!}");
            continue;
        }
        pno_scan_info.ac_match_ssid_set[l_loop][ssid_tmp->ssid_len] = '\0';
        pno_scan_info.l_ssid_count++;
    }

    /* 其它参数赋值 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    pno_scan_info.l_rssi_thold = request->min_rssi_thold;
#endif

    pno_scan_info.pno_scan_interval = PNO_SCHED_SCAN_INTERVAL;        /* 驱动自己定义为30s */
    pno_scan_info.pno_scan_repeat   = MAX_PNO_REPEAT_TIMES;

    /* 保存当前的PNO调度扫描请求指针 */
    scan_mgmt->sched_scan_req = request;
    scan_mgmt->sched_scan_complete = HI_FALSE;

    /* 维测打印上层下发的调度扫描请求参数信息 */
    wal_cfg80211_print_sched_scan_req_info(request);

    /* 下发pno扫描请求到hmac */
    hi_u32 ret = wal_cfg80211_start_sched_scan(netdev, &pno_scan_info);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_cfg80211_sched_scan_start::wal_cfg80211_start_sched_scan err[%u]}", ret);
        goto fail;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}
#endif

/* ****************************************************************************
 函 数 名  : wal_cfg80211_add_station
 功能描述  : 增加用户
 输入参数  : oal_wiphy_stru *pst_wiphy
             oal_net_device *pst_dev
             hi_u8 *puc_mac         用户mac 地址
             oal_station_parameters_stru *pst_sta_parms 用户参数
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#ifndef _PRE_HDF_LINUX
static hi_s32 wal_cfg80211_add_station(oal_wiphy_stru *wiphy, oal_net_device_stru *dev, const hi_u8 *puc_mac,
    oal_station_parameters_stru *pst_sta_parms)
{
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_cfg80211_change_station
 功能描述  : 删除用户
 输入参数  : oal_wiphy_stru *pst_wiphy
             oal_net_device *pst_dev
             hi_u8 *puc_mac         用户mac 地址
             oal_station_parameters_stru *pst_sta_parms 用户参数
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#ifndef _PRE_HDF_LINUX
static hi_s32 wal_cfg80211_change_station(oal_wiphy_stru *wiphy, oal_net_device_stru *dev, const hi_u8 *puc_mac,
    oal_station_parameters_stru *sta_parms)
{
    return HI_SUCCESS;
}
#endif

#define QUERY_STATION_INFO_TIME (5 * HZ)
/* ****************************************************************************
 函 数 名  : wal_cfg80211_fill_station_info
 功能描述  : station_info结构赋值
 输入参数  : oal_station_info_stru  *pst_sta_info,
             oal_station_info_stru *pst_stats
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void wal_cfg80211_fill_station_info(oal_station_info_stru *sta_info, oal_station_info_stru *stats)
{
    sta_info->filled |= STATION_INFO_SIGNAL;

    sta_info->signal = stats->signal;

    sta_info->filled |= STATION_INFO_RX_PACKETS;
    sta_info->filled |= STATION_INFO_TX_PACKETS;

    sta_info->rx_packets = stats->rx_packets;
    sta_info->tx_packets = stats->tx_packets;

    sta_info->filled   |= STATION_INFO_RX_BYTES;
    sta_info->filled   |= STATION_INFO_TX_BYTES;
    sta_info->rx_bytes  = stats->rx_bytes;
    sta_info->tx_bytes  = stats->tx_bytes;

    sta_info->filled |= STATION_INFO_TX_RETRIES;
    sta_info->filled |= STATION_INFO_TX_FAILED;
    sta_info->filled |= STATION_INFO_RX_DROP_MISC;

    sta_info->tx_retries       = stats->tx_retries;
    sta_info->tx_failed        = stats->tx_failed;
    sta_info->rx_dropped_misc  = stats->rx_dropped_misc;

    sta_info->filled |= STATION_INFO_TX_BITRATE;
    sta_info->txrate.legacy = (hi_u16)(stats->txrate.legacy * 10); /* 内核中单位为100kbps */
    sta_info->txrate.flags  = stats->txrate.flags;
    sta_info->txrate.mcs    = stats->txrate.mcs;
    sta_info->txrate.nss    = stats->txrate.nss;

    oam_info_log4(0, OAM_SF_CFG, "{wal_cfg80211_fill_station_info::legacy[%d],mcs[%d],flags[%d],nss[%d].}",
        sta_info->txrate.legacy / 10, sta_info->txrate.mcs, /* 10: 单位转换 */
        sta_info->txrate.flags, sta_info->txrate.nss);
}

/* ****************************************************************************
 功能描述  : update rssi once a second
 修改历史      :
  1.日    期   : 2015年8月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 wal_cfg80211_get_station_filter(mac_vap_stru *mac_vap, hi_u8 *mac_addr)
{
    hmac_user_stru *hmac_user = HI_NULL;
    hi_u32      current_time = hi_get_milli_seconds();
    hi_u32      runtime;

    hmac_user = mac_vap_get_hmac_user_by_addr(mac_vap, mac_addr, WLAN_MAC_ADDR_LEN);
    if (hmac_user == HI_NULL) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{wal_cfg80211_get_station_filter::user %d is null.}");
        return HI_FALSE;
    }

    if (current_time >= hmac_user->rssi_last_timestamp) {
        runtime = current_time - hmac_user->rssi_last_timestamp;
    } else {
        runtime = (hi_u32)(HI_U32_MAX - (hmac_user->rssi_last_timestamp - current_time) / HI_MILLISECOND_PER_TICK) *
            HI_MILLISECOND_PER_TICK;
    }

    if (runtime < WAL_GET_STATION_THRESHOLD) {
        return HI_FALSE;
    }

    hmac_user->rssi_last_timestamp = current_time;
    return HI_TRUE;
}

hi_u32 wal_cfg80211_send_query_station_event(oal_net_device_stru *netdev, mac_vap_stru *mac_vap,
    dmac_query_request_event *query_request, oal_station_info_stru *sta_info)
{
    wal_msg_write_stru write_msg;
    write_msg.wid = WLAN_CFGID_QUERY_STATION_STATS;
    write_msg.us_len = sizeof(dmac_query_request_event);

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_send_query_station_event::hmac_vap_get_vap_stru fail.vap_id[%u]}",
            mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 3.2 填写 msg 消息体 */
    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value),
        query_request, sizeof(dmac_query_request_event)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_get_station::mem safe function err!}");
        return HI_FAIL;
    }

    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(dmac_query_request_event), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_get_station::wal_send_cfg_event Err=%d}", ret);
        return ret;
    }

    /* info, boolean argument to function */
    hi_s32 i_leftime = hi_wait_event_timeout(hmac_vap->query_wait_q, (hmac_vap->query_wait_q_flag == HI_TRUE),
        QUERY_STATION_INFO_TIME);
    if (i_leftime == 0) {
        /* 超时还没有上报扫描结束 */
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_get_station::query info wait for %ld ms timeout!}",
            ((QUERY_STATION_INFO_TIME * 1000) / HZ)); /* 1000: 时间转换为ms */
        return HI_FAIL;
    } else if (i_leftime < 0) {
        /* 定时器内部错误 */
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_get_station::query info wait for %ld ms error!}",
            ((QUERY_STATION_INFO_TIME * 1000) / HZ)); /* 1000: 时间转换为ms */
        return HI_FAIL;
    } else {
        /* 正常结束  */
        wal_cfg80211_fill_station_info(sta_info, &hmac_vap->station_info);
        oam_info_log1(0, OAM_SF_CFG, "{wal_cfg80211_get_station::rssi %d.}", hmac_vap->station_info.signal);
        return HI_SUCCESS;
    }
}

/* ****************************************************************************
 功能描述  : 获取station信息
 输入参数  : [1]wiphy,
             [2]dev,
             [3]puc_mac,
             [4]sta_info
 返 回 值  : static hi_s32

**************************************************************************** */
hi_s32 wal_cfg80211_get_station(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, const hi_u8 *mac_addr,
    oal_station_info_stru *sta_info)
{
    dmac_query_request_event query_request;

    if ((wiphy == HI_NULL) || (netdev == HI_NULL) || (mac_addr == HI_NULL) || (sta_info == HI_NULL)) {
        oam_error_log4(0, OAM_SF_ANY, "{wal_cfg80211_get_station::wiphy[0x%p],dev[0x%p],mac[0x%p],sta_info[0x%p]}",
            wiphy, netdev, mac_addr, sta_info);
        goto fail;
    }

    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_get_station::oal_net_dev_priv, return null!}");
        goto fail;
    }

    query_request.query_event = OAL_QUERY_STATION_INFO_EVENT;
    if (memcpy_s(query_request.auc_query_sta_addr, WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_get_station::mem safe function err!}");
        goto fail;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_get_station:vap_get_vap_stru Err.vap_id=%u}", mac_vap->vap_id);
        goto fail;
    }

    /* 固定时间内最多更新一次RSSI */
    if (wal_cfg80211_get_station_filter(hmac_vap->base_vap, (hi_u8 *)mac_addr) == HI_FALSE) {
        wal_cfg80211_fill_station_info(sta_info, &hmac_vap->station_info);
        return HI_SUCCESS;
    }

    hmac_vap->query_wait_q_flag = HI_FALSE;

    /* *******************************************************************************
        抛事件到wal层处理 ，对于低功耗需要做额外处理，不能像下层抛事件，直接起定时器
        低功耗会在接收beacon帧的时候主动上报信息。
    ******************************************************************************* */
    if (wal_cfg80211_send_query_station_event(netdev, mac_vap, &query_request, sta_info) != HI_SUCCESS) {
        goto fail;
    }
    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_dump_station
 功能描述  : 删除用户
 输入参数  : oal_wiphy_stru *pst_wiphy
             oal_net_device *pst_dev
             hi_u8 *puc_mac         用户mac 地址
             oal_station_parameters_stru *pst_sta_parms 用户参数
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_s32 wal_cfg80211_dump_station(oal_wiphy_stru *wiphy, oal_net_device_stru *dev, hi_s32 int_index,
    hi_u8 *mac_addr, oal_station_info_stru *sta_info)
{
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_mgmt_tx_cancel_wait
 功能描述  : 取消发送管理帧等待
 输入参数  : [1]wiphy
             [2]wdev
             [3]ull_cookie
 输出参数  : 无
 返 回 值  : static hi_s32
**************************************************************************** */
static hi_s32 wal_cfg80211_mgmt_tx_cancel_wait(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, hi_u64 ull_cookie)
{
    return -HI_FAIL;
}

hi_u32 wal_cfg80211_get_vap_p2p_mode(nl80211_iftype_uint8 type, wlan_p2p_mode_enum_uint8 *p2p_mode,
    wlan_vap_mode_enum_uint8 *vap_mode)
{
    switch (type) {
        case NL80211_IFTYPE_P2P_DEVICE: /* P2P DEVICE在前面就返回，不应该走到这里 */
            oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf:: p2p0 need create before this!}");
            return HI_FAIL;
        case NL80211_IFTYPE_P2P_CLIENT:
            *vap_mode = WLAN_VAP_MODE_BSS_STA;
            *p2p_mode = WLAN_P2P_CL_MODE;
            break;
        case NL80211_IFTYPE_STATION:
            *vap_mode = WLAN_VAP_MODE_BSS_STA;
            *p2p_mode = WLAN_LEGACY_VAP_MODE;
            break;
        case NL80211_IFTYPE_P2P_GO:
            *vap_mode = WLAN_VAP_MODE_BSS_AP;
            *p2p_mode = WLAN_P2P_GO_MODE;
            break;
        case NL80211_IFTYPE_AP:
            *vap_mode = WLAN_VAP_MODE_BSS_AP;
            *p2p_mode = WLAN_LEGACY_VAP_MODE;
            break;
#ifdef _PRE_WLAN_FEATURE_MESH
        case NL80211_IFTYPE_MESH_POINT:
            *vap_mode = WLAN_VAP_MODE_MESH;
            *p2p_mode = WLAN_LEGACY_VAP_MODE;
            break;
#endif
        default:
            oam_error_log1(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf::Unsupported interface type[%d]!}", type);
            return HI_FAIL;
    }

    return HI_SUCCESS;
}

hi_u32 wal_cfg80211_add_virtual_intf_get_netdev(const hi_char *puc_name, oal_net_device_stru **netdev)
{
    hi_char auc_name[OAL_IF_NAME_SIZE] = {0};

    oal_net_device_stru *netdev_cfg = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev_cfg == HI_NULL) {
        oam_error_log0(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::pst_cfg_net_dev null!}");
        return HI_FAIL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev_cfg);
#endif
    if (memcpy_s(auc_name, OAL_IF_NAME_SIZE, puc_name, strlen(puc_name)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf::mem safe function err!}");
        return HI_CONTINUE;
    }

#if defined(_PRE_WLAN_FEATURE_FLOWCTL)
    /* 此函数第一个入参代表私有长度，此处不涉及为0 */
    *netdev = oal_net_alloc_netdev_mqs(auc_name);
#elif defined(_PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL)
    /* 此函数第一个入参代表私有长度，此处不涉及为0 */
    *netdev = oal_net_alloc_netdev_mqs(auc_name);
#else
    /* 此函数第一个入参代表私有长度，此处不涉及为0 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    *netdev = oal_net_alloc_netdev(auc_name);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    *netdev = oal_net_alloc_netdev(0, auc_name, oal_ether_setup);
#endif
#endif
    if (oal_unlikely((*netdev) == HI_NULL)) {
        oam_error_log0(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::pst_net_dev null ptr error!}");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

#ifndef _PRE_HDF_LINUX
hi_u32 wal_cfg80211_add_virtual_intf_set_wireless_dev(oal_wireless_dev *wdev, mac_device_stru *mac_device,
    oal_net_device_stru *netdev, nl80211_iftype_uint8 type)
{
    /* 对netdevice进行赋值 */
    /* 对新创建的net_device 初始化对应参数 */
#if (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    netdev->wireless_handlers = wal_get_g_iw_handler_def();
#endif
    netdev->netdev_ops = wal_get_net_dev_ops();

#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    netdev->ethtool_ops = &g_wal_ethtool_ops;
#endif

    oal_netdevice_destructor(netdev)     = oal_net_free_netdev;
    oal_netdevice_ifalias(netdev)        = HI_NULL;
    oal_netdevice_watchdog_timeo(netdev) = 5; /* 固定设置为 5 */
    oal_netdevice_wdev(netdev)           = wdev;
    oal_netdevice_qdisc(netdev, HI_NULL);

    wdev->iftype = type;
    wdev->wiphy  = mac_device->wiphy;
    wdev->netdev = netdev;    /* 给wdev 中的net_device 赋值 */
    oal_netdevice_flags(netdev) &= ~OAL_IFF_RUNNING;    /* 将net device的flag设为down */

    if (wal_cfg80211_register_netdev(netdev) != HI_SUCCESS) {
        /* 注册不成功，释放资源 */
        oal_mem_free(wdev);
        oal_net_free_netdev(netdev);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

oal_wireless_dev *wal_cfg80211_add_virtual_intf_send_event(oal_net_device_stru *netdev, oal_wireless_dev *wdev,
    wlan_p2p_mode_enum_uint8 p2p_mode, wlan_vap_mode_enum_uint8 vap_mode)
{
    wal_msg_write_stru   write_msg;
    wal_msg_stru        *rsp_msg    = HI_NULL;
    oal_net_device_stru *netdev_cfg = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev_cfg == HI_NULL) {
        oam_error_log0(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::pst_cfg_net_dev null!}");
        goto ERR_STEP;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev_cfg);
#endif
    /* 填写消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ADD_VAP, sizeof(mac_cfg_add_vap_param_stru));

    mac_cfg_add_vap_param_stru *add_vap_param = (mac_cfg_add_vap_param_stru *)(write_msg.auc_value);
    add_vap_param->net_dev  = netdev;
    add_vap_param->vap_mode = vap_mode;
    add_vap_param->cfg_vap_indx = WLAN_CFG_VAP_ID;
#ifdef _PRE_WLAN_FEATURE_P2P
    add_vap_param->p2p_mode = p2p_mode;
#endif

    /* 发送消息 */
    hi_u32 ret = wal_send_cfg_event(netdev_cfg, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_add_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::wal_send_cfg_event return err code=%u}", ret);
        goto ERR_STEP;
    }

    /* 读取返回的错误码 */
    if (wal_check_and_release_msg_resp(rsp_msg) != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::check_and_release_msg_resp fail:err_code}");
        goto ERR_STEP;
    }

    /* 设置netdevice的MAC地址，MAC地址在HMAC层被初始化到MIB中 */
    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::oal_net_dev_priv(pst_net_dev) is null ptr.}");
        goto ERR_STEP;
    }

    /* 设置VAP UP */
    wal_netdev_open(netdev);

    oam_warning_log1(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf:succ.vap_id[%d]}", mac_vap->vap_id);

    return wdev;

/* 异常处理 */
ERR_STEP: // 操作失败之后，进行内存释放等操作，属于例外，lint_t e801告警屏蔽
    wal_cfg80211_unregister_netdev(netdev);
    /* 先去注册，后释放 */
    oal_mem_free(wdev);
    return ERR_PTR(-EAGAIN);
}
#endif

/* ****************************************************************************
 函 数 名  : wal_cfg80211_add_virtual_intf
 功能描述  : 添加指定类型的net_device
 输入参数  : [1]wiphy
             [2]puc_name
             [3]type
             [4]pul_flags
             [5]params
 输出参数  : 无
 返 回 值  : static oal_wireless_dev
**************************************************************************** */
#ifndef _PRE_HDF_LINUX
static oal_wireless_dev *wal_cfg80211_add_virtual_intf(oal_wiphy_stru *wiphy, const hi_char *puc_name,
    nl80211_iftype_uint8 name_assign_type, enum_nl80211_iftype type, hi_u32 *pul_flags, oal_vif_params_stru *params)
{
    oal_wireless_dev *wdev = HI_NULL;
    wlan_p2p_mode_enum_uint8 p2p_mode;
    wlan_vap_mode_enum_uint8 vap_mode;

    hi_unref_param(pul_flags);

    /* 1.1 入参检查 */
    if ((wiphy == HI_NULL) || (puc_name == HI_NULL) || (params == HI_NULL)) {
        oam_error_log3(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf:: ptr is null,wiphy %p,name %p,params %p!}",
            (uintptr_t)wiphy, (uintptr_t)puc_name, (uintptr_t)params);
        return ERR_PTR(-EINVAL);
    }

    /* 入参检查无异常后赋值，并调用OAL统一接口 */
    mac_wiphy_priv_stru *wiphy_priv = oal_wiphy_priv(wiphy);
    mac_device_stru     *mac_device = wiphy_priv->mac_device;

    oam_warning_log1(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf::en_type[%d]!}", type);
#ifdef _PRE_WLAN_FEATURE_P2P
    hi_u8 p2p0_vap_idx = mac_device->p2p_info.p2p0_vap_idx;
    hmac_vap_stru *p2p0_hmac_vap = (hmac_vap_stru *)hmac_vap_get_vap_stru(p2p0_vap_idx);
    if (p2p0_hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf::p2p0_hmac_vap[id=%d] null!}", p2p0_vap_idx);
        return ERR_PTR(-ENODEV);
    }
#endif

    /* 如果创建的net device已经存在，直接返回 */
    oal_net_device_stru *netdev = oal_get_netdev_by_name(puc_name);
    if (netdev != HI_NULL) {
        /* 调用oal_dev_get_by_name后，必须调用oal_dev_put使net_dev的引用计数减一 */
        oal_dev_put(netdev);
        oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::the net_device is already exist!}");
        wdev = GET_NET_DEV_CFG80211_WIRELESS(netdev);
        return wdev;
    }

    if (wal_cfg80211_get_vap_p2p_mode(type, &p2p_mode, &vap_mode) != HI_SUCCESS) {
        return ERR_PTR(-EINVAL);
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    if (wal_cfg80211_add_virtual_intf_p2p_proc(mac_device) != HI_SUCCESS) {
        return ERR_PTR(-EAGAIN);
    }
#endif

    hi_u32 ret = wal_cfg80211_add_virtual_intf_get_netdev(puc_name, &netdev);
    if (ret == HI_FAIL) {
        return ERR_PTR(-ENOMEM);
    } else if (ret == HI_CONTINUE) {
        return HI_NULL;
    }

    wdev = (oal_wireless_dev *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, sizeof(oal_wireless_dev));
    if (oal_unlikely(wdev == HI_NULL)) {
        oam_error_log0(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{wal_cfg80211_add_virtual_intf::alloc mem, pst_wdev is null ptr}");
        /* 异常处理，释放内存 */
        oal_net_free_netdev(netdev);
        return ERR_PTR(-ENOMEM);
    }

    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    memset_s(wdev, sizeof(oal_wireless_dev), 0, sizeof(oal_wireless_dev));

    if (memcpy_s((hi_u8 *)oal_netdevice_mac_addr(netdev), WLAN_MAC_ADDR_LEN,
        (hi_u8 *)oal_netdevice_mac_addr(p2p0_hmac_vap->net_device), WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_add_virtual_intf::mem safe function err!}");
        oal_mem_free(wdev);
        return ERR_PTR(-ENOMEM);
    }
    if (p2p_mode == WLAN_P2P_CL_MODE) {
        ((hi_u8 *)oal_netdevice_mac_addr(netdev))[0] |= 0x02;
    }
    if (wal_cfg80211_add_virtual_intf_set_wireless_dev(wdev, mac_device, netdev, type) != HI_SUCCESS) {
        return ERR_PTR(-EBUSY);
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    return wal_cfg80211_add_virtual_intf_send_event(netdev, wdev, p2p_mode, vap_mode);
}
#endif

hi_u32 wal_cfg80211_del_p2p_proc(wal_msg_write_stru *write_msg, oal_net_device_stru *netdev, mac_vap_stru *mac_vap)
{
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 p2p_mode =
        wal_wireless_iftype_to_mac_p2p_mode(GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype);
    if (p2p_mode == WLAN_P2P_BUTT) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_virtual_intf::get p2p mode err}");
        return HI_ERR_CODE_PTR_NULL;
    }

    ((mac_cfg_del_vap_param_stru *)write_msg->auc_value)->p2p_mode = mac_get_p2p_mode(mac_vap);

    hi_u32 ret = wal_set_p2p_status(netdev, P2P_STATUS_IF_DELETING);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_del_virtual_intf::wal_set_p2p_status return %u}.", ret);
        return ret;
    }
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_del_virtual_intf
 功能描述  : 删除对应VAP
 输入参数  : oal_wiphy_stru            *pst_wiphy
             oal_wireless_dev     *pst_wdev
             hi_s32                  l_ifindex
 输出参数  : 无
 返 回 值  : hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 wal_cfg80211_del_virtual_intf(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev)
{
    wal_msg_stru       *rsp_msg = HI_NULL;
    wal_msg_write_stru  write_msg;

    if (oal_unlikely((wiphy == HI_NULL) || (wdev == HI_NULL))) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_del_virtual_intf::pst_wiphy or pst_wdev null ptr error %p, %p}",
            (uintptr_t)wiphy, (uintptr_t)wdev);
        return -HI_ERR_CODE_PTR_NULL;
    }

    oal_net_device_stru *netdev  = wdev->netdev;
    mac_vap_stru        *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_del_virtual_intf::mac_vap is null by netdev, mode[%d]}",
            GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype);
        return -HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_del_virtual_intf::get_vap_stru fail.vap_id[%u]}", mac_vap->vap_id);
        return -HI_ERR_CODE_PTR_NULL;
    }

    oal_net_tx_stop_all_queues();
    wal_netdev_stop(netdev);

    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    /* 初始化删除vap 参数 */
    ((mac_cfg_del_vap_param_stru *)write_msg.auc_value)->net_dev = netdev;

    hi_u32 ret = wal_cfg80211_del_p2p_proc(&write_msg, netdev, mac_vap);
    if (ret != HI_SUCCESS) {
        return -HI_FAIL;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DEL_VAP, sizeof(mac_cfg_del_vap_param_stru));

    /* 启动linux work 删除net_device */
#ifdef _PRE_WLAN_FEATURE_P2P
    hmac_vap->del_net_device = netdev;
#endif

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_del_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);

    if (wal_check_and_release_msg_resp(rsp_msg) != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_virtual_intf::wal_check_and_release_msg_resp fail}");
    }

    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_del_virtual_intf::return err code %d}", ret);
        return -HI_FAIL;
    }

    return HI_SUCCESS;
}

/* P2P 补充缺失的CFG80211接口 */
hi_void wal_cfg80211_mgmt_frame_register(struct wiphy *wiphy, struct wireless_dev *wdev, hi_u16 frame_type, bool reg)
{
    return;
}

hi_s32 wal_cfg80211_set_bitrate_mask(struct wiphy *wiphy, oal_net_device_stru *netdev, const hi_u8 *peer,
    const struct cfg80211_bitrate_mask *mask)
{
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_start_p2p_device
 功能描述  : 启动P2P_DEV
 输入参数  : oal_wiphy_stru       *pst_wiphy
             oal_wireless_dev   *pst_wdev
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_s32 wal_cfg80211_start_p2p_device(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev)
{
    return -HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_stop_p2p_device
 功能描述  : 停止P2P_DEV
 输入参数  : oal_wiphy_stru       *pst_wiphy
             oal_wireless_dev   *pst_wdev
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void wal_cfg80211_stop_p2p_device(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev)
{
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_set_power_mgmt
 功能描述  : 开关低功耗
 输入参数  : oal_wiphy_stru       *pst_wiphy
             oal_wireless_dev   *pst_wdev
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年07月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_s32 wal_cfg80211_set_power_mgmt(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, bool enabled,
    hi_s32 timeout)
{
    wal_msg_write_stru           write_msg;
    mac_cfg_ps_open_stru        *sta_pm_open = HI_NULL;
    hi_u32                       ret;
    mac_vap_stru                *mac_vap = HI_NULL;

    /* host低功耗没有开,此时不开device的低功耗 */
    if (!hmac_get_wlan_pm_switch()) {
        return HI_SUCCESS;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_STA_PM_ON, sizeof(mac_cfg_ps_open_stru));
    mac_vap = oal_net_dev_priv(netdev);
    if (oal_unlikely(mac_vap == NULL)) {
        oam_warning_log0(0, OAM_SF_PWR, "{wal_cfg80211_set_power_mgmt::get mac vap failed.}");
        return HI_SUCCESS;
    }

    /* P2P dev不下发 */
    if (is_p2p_dev(mac_vap)) {
        oam_warning_log0(0, OAM_SF_PWR, "wal_cfg80211_set_power_mgmt:vap is p2p dev return");
        return HI_SUCCESS;
    }

    oam_warning_log3(0, OAM_SF_PWR, "{wal_cfg80211_set_power_mgmt::vap mode[%d]p2p mode[%d]set pm:[%d]}",
        mac_vap->vap_mode, mac_vap->p2p_mode, enabled);

    sta_pm_open = (mac_cfg_ps_open_stru *)(write_msg.auc_value);
    /* MAC_STA_PM_SWITCH_ON / MAC_STA_PM_SWITCH_OFF */
    sta_pm_open->pm_enable = enabled;
    sta_pm_open->pm_ctrl_type = MAC_STA_PM_CTRL_TYPE_HOST;

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_ps_open_stru),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_power_mgmt::fail to send pm cfg msg, error[%u]}", ret);
        return -HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_start_sched_scan
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_start_sched_scan(oal_net_device_stru *netdev, mac_pno_scan_stru *pno_scan_info)
{
    mac_pno_scan_stru      *pno_scan_params = HI_NULL;
    hi_u32                  ret;

    /* 申请pno调度扫描参数，此处申请hmac层释放 */
    pno_scan_params = (mac_pno_scan_stru *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, sizeof(mac_pno_scan_stru));
    if (pno_scan_params == HI_NULL) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_cfg80211_start_sched_scan::alloc pno scan param memory failed!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (memcpy_s(pno_scan_params, sizeof(mac_pno_scan_stru), pno_scan_info, sizeof(mac_pno_scan_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_start_sched_scan::mem safe function err!}");
        oal_mem_free(pno_scan_params);
        return HI_FAIL;
    }

    ret = (hi_u32)wal_cfg80211_start_req(netdev, &pno_scan_params, sizeof(mac_pno_scan_stru),
        WLAN_CFGID_CFG80211_START_SCHED_SCAN, HI_TRUE);
    if (ret != HI_SUCCESS) {
        oal_mem_free(pno_scan_params);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_sched_scan_stop
 功能描述  : 调度扫描关闭
 输入参数  : oal_wiphy_stru                       *pst_wiphy
             oal_net_device_stru                  *pst_netdev
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_s32 wal_cfg80211_sched_scan_stop(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev)
{
    hmac_device_stru               *hmac_dev = HI_NULL;
    mac_device_stru                *mac_device = HI_NULL;
    hmac_scan_stru                 *scan_mgmt = HI_NULL;
    wal_msg_write_stru              write_msg;
    hi_u32                          pedding_data = 0;       /* 填充数据，不使用，只是为了复用接口 */
    hi_u32                          ret;

    /* 参数合法性检查 */
    if ((wiphy == HI_NULL) || (netdev == HI_NULL)) {
        oam_error_log2(0, OAM_SF_CFG,
            "{wal_cfg80211_sched_scan_stop::input param pointer is null, pst_wiphy[%p], pst_netdev[%p]!}",
            (uintptr_t)wiphy, (uintptr_t)netdev);
        goto fail;
    }

    /* 通过net_device 找到对应的mac_device_stru 结构 */
    hmac_dev = (hmac_device_stru *)hmac_get_device_stru();
    scan_mgmt = &(hmac_dev->scan_mgmt);

    oam_warning_log2(0, OAM_SF_SCAN, "{wal_cfg80211_sched_scan_stop::sched scan req[%p],sched scan complete[%d]}",
        (uintptr_t)scan_mgmt->sched_scan_req, scan_mgmt->sched_scan_complete);

    if ((scan_mgmt->sched_scan_req != HI_NULL) && (scan_mgmt->sched_scan_complete != HI_TRUE)) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        /* 如果正常扫描请求未执行，则上报调度扫描结果 */
        if (scan_mgmt->request == HI_NULL) {
            mac_device = mac_res_get_dev();
            oal_cfg80211_sched_scan_result(mac_device->wiphy);
        }
#endif
        scan_mgmt->sched_scan_req = HI_NULL;
        scan_mgmt->sched_scan_complete = HI_TRUE;

        /* 拋事件通知device侧停止PNO调度扫描 */
        wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_CFG80211_STOP_SCHED_SCAN, sizeof(pedding_data));
        if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value),
            (hi_s8 *)&pedding_data, sizeof(pedding_data)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_sched_scan_stop::mem safe function err!}");
            goto fail;
        }

        ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(pedding_data),
            (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
        if (ret != HI_SUCCESS) {
            oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_sched_scan_stop::fail to stop pno sched scan, error[%u]}",
                ret);
            goto fail;
        }
    }

    return HI_SUCCESS;
fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_stop_ap
 功能描述  : 停止AP
 输入参数  : oal_wiphy_stru        *pst_wiphy
             oal_net_device_stru *pst_netdev
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 wal_cfg80211_stop_ap(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev)
{
    wal_msg_write_stru write_msg;

    hi_unref_param(wiphy);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* 参数合法性检查 */
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::pst_netdev is null!}");
        goto fail;
    }
#endif
    /* 获取vap id */
    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::can't get mac vap from netdevice priv data!}");
        goto fail;
    }

    /* 判断是否为非ap模式 */
    if ((mac_vap->vap_mode != WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        && (mac_vap->vap_mode != WLAN_VAP_MODE_MESH)
#endif
    ) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_stop_ap::vap is not in ap mode!}");
        goto fail;
    }

    /* 如果netdev不是running状态，则不需要down */
    if ((oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::vap is already down!}\r\n");
        return HI_SUCCESS;
    }

    /* ****************************************************************************
        发送消息，停用ap
    **************************************************************************** */
    /* 填写消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DOWN_VAP, sizeof(mac_cfg_start_vap_param_stru));

#ifdef _PRE_WLAN_FEATURE_P2P
    oal_wireless_dev *wdev = GET_NET_DEV_CFG80211_WIRELESS(netdev);
    wlan_p2p_mode_enum_uint8 p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(wdev->iftype);
    if (WLAN_P2P_BUTT == p2p_mode) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::wal_wireless_iftype_to_mac_p2p_mode return BUFF}\r\n");
        goto fail;
    }
    oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::en_p2p_mode=%u}\r\n", p2p_mode);

    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->p2p_mode = p2p_mode;
#endif

    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->net_dev = netdev;

    /* 发送消息 */
    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_start_vap_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_stop_ap::failed to stop ap, error[%u]}", ret);
        goto fail;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

hi_void wal_cfg80211_set_iftype(const mac_cfg_add_vap_param_stru *add_vap_param, oal_wireless_dev *wdev)
{
    if (add_vap_param->vap_mode == WLAN_VAP_MODE_BSS_AP) {
        wdev->iftype = NL80211_IFTYPE_AP;
    } else if (add_vap_param->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        wdev->iftype = NL80211_IFTYPE_STATION;
#ifdef _PRE_WLAN_FEATURE_MESH
    } else if (add_vap_param->vap_mode == WLAN_VAP_MODE_MESH) {
        wdev->iftype = NL80211_IFTYPE_MESH_POINT;
#endif
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    if (add_vap_param->p2p_mode == WLAN_P2P_DEV_MODE) {
        wdev->iftype = NL80211_IFTYPE_P2P_DEVICE;
    } else if (add_vap_param->p2p_mode == WLAN_P2P_CL_MODE) {
        wdev->iftype = NL80211_IFTYPE_P2P_CLIENT;
    } else if (add_vap_param->p2p_mode == WLAN_P2P_GO_MODE) {
        wdev->iftype = NL80211_IFTYPE_P2P_GO;
    }
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_add_vap
 功能描述  : CFG80211 接口添加网络设备
 输入参数  : mac_cfg_add_vap_param_stru *pst_add_vap_param
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#ifndef _PRE_HDF_LINUX
hi_u32 wal_cfg80211_add_vap(const mac_cfg_add_vap_param_stru *add_vap_param)
{
    wal_msg_write_stru     write_msg;
    wal_msg_stru          *rsp_msg    = HI_NULL;
    oal_net_device_stru   *netdev     = add_vap_param->net_dev;
    oal_wireless_dev      *wdev       = GET_NET_DEV_CFG80211_WIRELESS(netdev);
    oal_net_device_stru   *netdev_cfg = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);

    if (netdev_cfg == HI_NULL) {
        oal_mem_free(wdev);
        oal_net_free_netdev(netdev);
        oam_warning_log0(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{wal_cfg80211_add_vap::pst_cfg_net_dev is null!}");
        return HI_FAIL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev_cfg);
#endif
    wal_cfg80211_set_iftype(add_vap_param, wdev);

    oal_netdevice_flags(netdev) &= ~OAL_IFF_RUNNING; /* 将net device的flag设为down */

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    /* 填写消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ADD_VAP, sizeof(mac_cfg_add_vap_param_stru));
    ((mac_cfg_add_vap_param_stru *)write_msg.auc_value)->net_dev      = netdev;
    ((mac_cfg_add_vap_param_stru *)write_msg.auc_value)->vap_mode     = add_vap_param->vap_mode;
    ((mac_cfg_add_vap_param_stru *)write_msg.auc_value)->cfg_vap_indx = WLAN_CFG_VAP_ID;
#ifdef _PRE_WLAN_FEATURE_P2P
    ((mac_cfg_add_vap_param_stru *)write_msg.auc_value)->p2p_mode     = add_vap_param->p2p_mode;
#endif

    /* 发送消息 */
    hi_u32 ret = wal_send_cfg_event(netdev_cfg, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_add_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oal_mem_free(wdev);
        oal_net_free_netdev(netdev);
        oam_warning_log1(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{wal_cfg80211_add_vap::return err code [%u]}", ret);
        return ret;
    }

    /* 读取返回的错误码 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{wal_cfg80211_add_vap::hmac add vap Err=%u}", ret);
        /* 异常处理，释放内存 */
        oal_mem_free(wdev);
        oal_net_free_netdev(netdev);
        return ret;
    }

    wal_set_mac_to_mib(add_vap_param->net_dev); /* by lixu tmp */

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_cfg80211_del_vap
 功能描述  : CFG80211 接口删除网络设备
 输入参数  : mac_cfg_del_vap_param_stru *pst_del_vap_param
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_del_vap(const mac_cfg_del_vap_param_stru *del_vap_param)
{
    hi_u32                       ret;
    wal_msg_write_stru           write_msg;
    wal_msg_stru                *rsp_msg = HI_NULL;
    oal_net_device_stru         *netdev = HI_NULL;

    if (oal_unlikely(del_vap_param == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_vap::pst_del_vap_param null ptr !}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    netdev = del_vap_param->net_dev;
    /* 设备在up状态不允许删除，必须先down */
    if (oal_unlikely((OAL_IFF_RUNNING & oal_netdevice_flags(netdev)) != 0)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_del_vap::device is busy, please down it first %d!}\r\n",
            oal_netdevice_flags(netdev));
        return HI_ERR_CODE_CONFIG_BUSY;
    }

    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    /* 初始化删除vap 参数 */
    ((mac_cfg_del_vap_param_stru *)write_msg.auc_value)->net_dev = netdev;
#ifdef _PRE_WLAN_FEATURE_P2P
    ((mac_cfg_del_vap_param_stru *)write_msg.auc_value)->p2p_mode = del_vap_param->p2p_mode;
#endif
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DEL_VAP, sizeof(mac_cfg_del_vap_param_stru));

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_del_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_del_vap::return err code [%u]!}\r\n", ret);
        return ret;
    }

    if (HI_SUCCESS != wal_check_and_release_msg_resp(rsp_msg)) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_vap::wal_check_and_release_msg_resp fail!}");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_start_connect
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_start_connect(oal_net_device_stru *netdev,
    const mac_cfg80211_connect_param_stru *mac_cfg80211_connect_param)
{
    return wal_cfg80211_start_req(netdev, mac_cfg80211_connect_param, sizeof(mac_cfg80211_connect_param_stru),
        WLAN_CFGID_CFG80211_START_CONNECT, HI_TRUE);
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_start_disconnect
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_start_disconnect(oal_net_device_stru *netdev, const mac_cfg_kick_user_param_stru *disconnect_param)
{
    /* 注意 由于消息未真正处理就直接返回，导致WPA_SUPPLICANT继续下发消息，在驱动侧等到处理时被异常唤醒，
       导致后续下发的消息误以为操作失败，目前将去关联事件修改为等待消息处理结束后再上报，
       最后一个入参由HI_FALSE改为HI_TRUE */
    return wal_cfg80211_start_req(netdev, disconnect_param, sizeof(mac_cfg_kick_user_param_stru), WLAN_CFGID_KICK_USER,
        HI_TRUE);
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_unregister_netdev
 功能描述  : 内核去注册指定类型的net_device,用于需要解mutex lock的应用
 输入参数  : mac_device_stru *pst_hmac_device
             oal_net_device_stru *pst_net_dev
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年7月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_cfg80211_unregister_netdev(oal_net_device_stru *netdev)
{
    /* 去注册netdev */
    oal_net_unregister_netdev(netdev);
    oal_net_free_netdev(netdev);
}

#if (LINUX_VERSION_CODE >= kernel_version(4, 1, 0))
static hi_void wal_cfg80211_abort_scan(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev)
{
    oal_net_device_stru *netdev = HI_NULL;

    /* 1.1 入参检查 */
    if ((wiphy == HI_NULL) || (wdev == HI_NULL)) {
        oam_error_log2(0, OAM_SF_CFG, "{wal_cfg80211_abort_scan::wiphy or wdev is null, %p, %p!}", wiphy, wdev);
        return;
    }
    netdev = wdev->netdev;
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_abort_scan::netdev is null!}\r\n");
        return;
    }
    oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_abort_scan::enter!}\r\n");
    wal_force_scan_complete(netdev);
    return;
}
#endif
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

/* ****************************************************************************
 函 数 名  : wal_parse_rsn_ie
 功能描述  : 解析beacon帧中的 RSN 信息元素
 输入参数  : [1]puc_ie
             [2]beacon_param
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 wal_parse_rsn_ie(const hi_u8 *puc_ie, mac_beacon_param_stru *beacon_param)
{
    hi_u16 us_temp, auth_temp;

    /* *********************************************************************** */
    /*                  RSN Element Format                                   */
    /* --------------------------------------------------------------------- */
    /* |Element ID | Length | Version | Group Cipher Suite | Pairwise Cipher */
    /* --------------------------------------------------------------------- */
    /* |     1     |    1   |    2    |         4          |       2         */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* Suite Count| Pairwise Cipher Suite List | AKM Suite Count | AKM Suite List */
    /* ---------------------------------------------------------------------  */
    /*            |         4*m                |     2           |   4*n      */
    /* ---------------------------------------------------------------------  */
    /* ---------------------------------------------------------------------  */
    /* |RSN Capabilities|PMKID Count|PMKID List|Group Management Cipher Suite */
    /* ---------------------------------------------------------------------  */
    /* |        2       |    2      |   16 *s  |               4           |  */
    /* ---------------------------------------------------------------------  */
    /*                                                                        */
    /* ************************************************************************ */
    hi_u8 auc_oui[MAC_OUI_LEN] = {MAC_WLAN_OUI_RSN0, MAC_WLAN_OUI_RSN1, MAC_WLAN_OUI_RSN2};
    hi_u8 index = 2; /* 2: 忽略 RSN IE 和 IE 长度 */

    /* 长度若小于2，取后面的值都会异常 */
    if (puc_ie[1] < MAC_MIN_RSN_LEN) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_parse_rsn_ie::invalid RSN IE len[%d]!}\r\n", puc_ie[1]);
        return HI_FAIL;
    }

    /* 获取RSN 版本号 */
    if (hi_makeu16(puc_ie[index], puc_ie[index + 1]) != MAC_RSN_IE_VERSION) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_parse_rsn_ie::RSN version illegal!}\r\n");
        return HI_FAIL;
    }

    index += 2; /* 2: 忽略 RSN 版本号长度 */

    /* 获取组播密钥套件 */
    if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_parse_rsn_ie::RSN group OUI illegal!}\r\n");
        return HI_FAIL;
    }
    beacon_param->group_crypto = puc_ie[index + MAC_OUI_LEN];

    index += 4; /* 4: 忽略组播密钥套件长度 */

    /* 获取成对密钥套件 */
    hi_u16 us_pcip_num = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    if (us_pcip_num > MAC_PAIRWISE_CIPHER_SUITES_NUM) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_parse_rsn_ie::pairwise chiper num illegal!}\r\n", us_pcip_num);
        return HI_FAIL;
    }

    /* 将加密套件初始化为0xff */ /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(beacon_param->auc_pairwise_crypto_wpa2, MAC_PAIRWISE_CIPHER_SUITES_NUM, 0xff,
        MAC_PAIRWISE_CIPHER_SUITES_NUM);

    index += 2; /* 2: 获取加密套件 */
    for (us_temp = 0; us_temp < us_pcip_num; us_temp++) {
        if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_parse_rsn_ie::RSN paerwise OUI illegal!}\r\n");
            return HI_FAIL;
        }
        beacon_param->auc_pairwise_crypto_wpa2[us_temp] = puc_ie[index + MAC_OUI_LEN];

        index += 4; /* 4: 套件长度 */
    }

    /* 获取认证套件计数 */
    hi_u16 us_auth_num = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    us_auth_num = oal_min(us_auth_num, WLAN_AUTHENTICATION_SUITES);
    index += 2; /* 2: 获取认证类型 */

    /* 将认证套件初始化为0xff */ /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(beacon_param->auc_auth_type, us_auth_num, 0xff, us_auth_num);

    /* 获取认证类型 */
    for (auth_temp = 0; auth_temp < us_auth_num; auth_temp++) {
        if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_parse_rsn_ie::RSN auth OUI illegal!}\r\n");
            return HI_FAIL;
        }
        beacon_param->auc_auth_type[auth_temp] = puc_ie[index + MAC_OUI_LEN];
        index += 4; /* 4: 类型长度 */
    }

    /* 获取RSN 能力信息 */
    beacon_param->us_rsn_capability = hi_makeu16(puc_ie[index], puc_ie[index + 1]);

    /* 设置加密模式 */
    beacon_param->crypto_mode |= WLAN_WPA2_BIT;

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_parse_wpa_ie
 功能描述  : 解析beacon帧中的 WPA 信息元素
 输入参数  : [1]puc_ie
             [2]beacon_param
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 wal_parse_wpa_ie(const hi_u8 *puc_ie, mac_beacon_param_stru *beacon_param)
{
    hi_u8  auc_oui[MAC_OUI_LEN] = {MAC_WLAN_OUI_MICRO0, MAC_WLAN_OUI_MICRO1, MAC_WLAN_OUI_MICRO2};
    hi_u16 us_temp, auth_temp;

    /* ************************************************************************ */
    /*                  WPA Element Format                                    */
    /* ---------------------------------------------------------------------  */
    /* |Element ID | Length |    WPA OUI    |  Version |  Group Cipher Suite  */
    /* ---------------------------------------------------------------------  */
    /* |     1     |   1    |        4      |     2    |         4            */
    /* ---------------------------------------------------------------------  */
    /* ---------------------------------------------------------------------  */
    /* Pairwise Cipher |  Pairwise Cipher   |                 |               */
    /* Suite Count     |    Suite List      | AKM Suite Count |AKM Suite List */
    /* ---------------------------------------------------------------------  */
    /*        2        |          4*m       |         2       |     4*n       */
    /* ---------------------------------------------------------------------  */
    /*                                                                        */
    /* ************************************************************************ */
    hi_u8 index = 2 + 4; /* 2 4: 忽略 WPA IE(1 字节) ，IE 长度(1 字节) ，WPA OUI(4 字节) */

    hi_u16 us_ver = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    /* 对比WPA 版本信息 */
    if (us_ver != MAC_WPA_IE_VERSION) {
        oam_error_log0(0, OAM_SF_WPA, "{wal_parse_wpa_ie::WPA version illegal!}\r\n");
        return HI_FAIL;
    }

    index += 2; /* 2: 忽略 版本号 长度 */

    hi_u8 *puc_pcip_policy = beacon_param->auc_pairwise_crypto_wpa;
    hi_u8 *puc_auth_policy = beacon_param->auc_auth_type;

    /* 获取组播密钥套件 */
    if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
        oam_error_log0(0, OAM_SF_WPA, "{wal_parse_wpa_ie::WPA group OUI illegal!}\r\n");
        return HI_FAIL;
    }
    beacon_param->group_crypto = puc_ie[index + MAC_OUI_LEN];

    index += 4; /* 4: 忽略组播密钥套件长度 */

    /* 获取成对密钥套件 */
    hi_u16 us_pcip_num = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    if (us_pcip_num > MAC_PAIRWISE_CIPHER_SUITES_NUM) {
        oam_error_log1(0, OAM_SF_WPA, "{wal_parse_wpa_ie::pairwise chiper num illegal %d!}\r\n", us_pcip_num);
        return HI_FAIL;
    }

    /* 将加密套件初始化为0xff */ /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化 */
    memset_s(beacon_param->auc_pairwise_crypto_wpa, MAC_PAIRWISE_CIPHER_SUITES_NUM, 0xff,
        MAC_PAIRWISE_CIPHER_SUITES_NUM);

    index += 2; /* 2: 获取套件 */
    for (us_temp = 0; us_temp < us_pcip_num; us_temp++) {
        if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
            oam_error_log0(0, OAM_SF_WPA, "{wal_parse_wpa_ie::WPA pairwise OUI illegal!}\r\n");
            return HI_FAIL;
        }
        puc_pcip_policy[us_temp] = puc_ie[index + MAC_OUI_LEN];
        index += 4; /* 4: 套件长度 */
    }

    /* 获取认证套件计数 */
    hi_u16 us_auth_num = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    us_auth_num = oal_min(us_auth_num, MAC_AUTHENTICATION_SUITE_NUM);
    index += 2; /* 2 获取认证类型 */

    /* 将认证套件初始化为0xff */
    if (memset_s(puc_auth_policy, us_auth_num, 0xff, us_auth_num) != EOK) {
        return HI_FAIL;
    }
    /* 获取认证类型 */
    for (auth_temp = 0; auth_temp < us_auth_num; auth_temp++) {
        if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
            oam_error_log0(0, OAM_SF_WPA, "{wal_parse_wpa_ie::WPA auth OUI illegal!}\r\n");
            return HI_FAIL;
        }
        puc_auth_policy[auth_temp] = puc_ie[index + MAC_OUI_LEN];
        index += 4; /* 4: 类型长度 */
    }

    /* 设置加密模式 */
    beacon_param->crypto_mode |= WLAN_WPA_BIT;

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_parse_wpa_wpa2_ie
 功能描述  : 解析内核传递过来的beacon信息中的WPA/WPA2 信息元素
 输入参数  : oal_beacon_parameters *pst_beacon_info
             mac_beacon_param_stru *pst_beacon_param
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年12月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_parse_wpa_wpa2_ie(const oal_beacon_parameters *beacon_info, mac_beacon_param_stru *beacon_param)
{
    const hi_u8         *puc_rsn_ie = HI_NULL;
    hi_u8               *puc_wpa_ie = HI_NULL;
    hi_u32               ret;
    oal_ieee80211_mgmt  *mgmt   = HI_NULL;
    hi_u16               us_capability_info;

    /* 判断是否加密 */
    mgmt = (oal_ieee80211_mgmt *)beacon_info->head;
    us_capability_info = mgmt->u.beacon.capab_info;
    beacon_param->privacy = HI_FALSE;
    if (WLAN_WITP_CAPABILITY_PRIVACY & us_capability_info) {
        beacon_param->privacy = HI_TRUE;

        /* 查找 RSN 信息元素 */
        puc_rsn_ie = mac_find_ie(MAC_EID_RSN, beacon_info->tail, beacon_info->tail_len);
        if (puc_rsn_ie != HI_NULL) {
            /* 根据RSN 信息元素解析出认证类型 */
            ret = wal_parse_rsn_ie(puc_rsn_ie, beacon_param);
            if (ret != HI_SUCCESS) {
                oam_warning_log0(0, OAM_SF_WPA, "{wal_parse_wpa_wpa2_ie::Failed to parse RSN ie!}\r\n");
                return HI_FAIL;
            }
        }

        /* 查找 WPA 信息元素，并解析出认证类型 */
        puc_wpa_ie = mac_find_vendor_ie(MAC_WLAN_OUI_MICROSOFT, MAC_OUITYPE_WPA, beacon_info->tail,
            (hi_s32)beacon_info->tail_len);
        if (puc_wpa_ie != HI_NULL) {
            ret = wal_parse_wpa_ie(puc_wpa_ie, beacon_param);
            if (ret != HI_SUCCESS) {
                oam_warning_log0(0, OAM_SF_WPA, "{wal_parse_wpa_wpa2_ie::Failed to parse WPA ie!}\r\n");
                return HI_FAIL;
            }
        }
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : 解析内核传递过来beacon信息中的mesh configuration信息元素
 输入参数  : oal_beacon_parameters *pst_beacon_info
 返 回 值  : hi_u32
 修改历史      :
  1.日    期   : 2019年3月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_parse_mesh_conf_ie(const oal_beacon_parameters *beacon_info, mac_beacon_param_stru *beacon_param)
{
    const hi_u8 *puc_mesh_conf_ie = HI_NULL;
    hi_u8 index = 0;

    /* 查找 mesh conf信息元素 */
    puc_mesh_conf_ie = mac_find_ie(MAC_EID_MESH_CONF, beacon_info->tail, beacon_info->tail_len);
    if (puc_mesh_conf_ie != HI_NULL) {
        /* 根据Mesh Conf信息元素解析出mesh配置项 */
        /* 长度若小于2，取后面的值都会异常 */
        if (puc_mesh_conf_ie[1] < MAC_MIN_MESH_CONF_LEN) {
            oam_warning_log1(0, OAM_SF_ANY, "{wal_parse_mesh_conf_ie::invalid mesh conf IE len[%d]!}\r\n",
                puc_mesh_conf_ie[1]);
            return HI_FAIL;
        }

        index += 6; /* 6: 忽略 Mesh conf IE 和 IE 长度 ,选路算法及用塞控制 */
        beacon_param->mesh_auth_protocol = puc_mesh_conf_ie[index++];
        beacon_param->mesh_formation_info = puc_mesh_conf_ie[index++];
        beacon_param->mesh_capability = puc_mesh_conf_ie[index];
        oam_warning_log3(0, OAM_SF_ANY,
            "{wal_parse_mesh_conf_ie::auth_protocol = %d, formation_info = %d, capability = %d!}",
            beacon_param->mesh_auth_protocol, beacon_param->mesh_formation_info, beacon_param->mesh_capability);
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_parse_mesh_conf_ie::mesh vap can't find mesh conf ie!}");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_check_support_basic_rate_6M
 功能描述  : 判断指数速率集和扩展速率集中是否包含6M速率作为基本速率
 输入参数  : [1]puc_supported_rates_ie
             [2]supported_rates_num
             [3]puc_extended_supported_rates_ie
             [4]extended_supported_rates_num
 输出参数  : 无
 返 回 值  : static hi_bool : HI_TRUE    支持
                              HI_FALSE   不支持
**************************************************************************** */
static hi_bool wal_check_support_basic_rate_6m(const hi_u8 *puc_supported_rates_ie, hi_u8 supported_rates_num,
    const hi_u8 *puc_extended_supported_rates_ie, hi_u8 extended_supported_rates_num)
{
    hi_u8 loop;
    hi_bool support = HI_FALSE;
    for (loop = 0; loop < supported_rates_num; loop++) {
        if (puc_supported_rates_ie == HI_NULL) {
            break;
        }
        if (puc_supported_rates_ie[2 + loop] == 0x8c) { /* 2：偏移位 */
            support = HI_TRUE;
        }
    }

    for (loop = 0; loop < extended_supported_rates_num; loop++) {
        if (puc_extended_supported_rates_ie == HI_NULL) {
            break;
        }
        if (puc_extended_supported_rates_ie[2 + loop] == 0x8c) { /* 2：偏移位 */
            support = HI_TRUE;
        }
    }

    return support;
}

/* ****************************************************************************
 函 数 名  : wal_parse_protocol_mode
 功能描述  : 解析协议模式
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年6月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_parse_protocol_mode(wlan_channel_band_enum_uint8 band, const oal_beacon_parameters *beacon_info,
    const hi_u8 *puc_ht_ie, const hi_u8 *puc_vht_ie, wlan_protocol_enum_uint8 *pen_protocol)
{
    hi_u8   *puc_supported_rates_ie             = HI_NULL;
    hi_u8   *puc_extended_supported_rates_ie    = HI_NULL;
    hi_u8    supported_rates_num             = 0;
    hi_u8    extended_supported_rates_num    = 0;
    hi_u16   us_offset;

    if (puc_vht_ie != HI_NULL) {
        /* 设置AP 为11ac 模式 */
        *pen_protocol = WLAN_VHT_MODE;
        return HI_SUCCESS;
    }
    if (puc_ht_ie != HI_NULL) {
        /* 设置AP 为11n 模式 */
        *pen_protocol = WLAN_HT_MODE;
        return HI_SUCCESS;
    }

    if (WLAN_BAND_2G == band) {
        us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
        if (beacon_info->head_len > us_offset) {
            puc_supported_rates_ie = mac_find_ie(MAC_EID_RATES, beacon_info->head + 24 + us_offset,
                beacon_info->head_len - us_offset); /* mac header长度24 */
            if (puc_supported_rates_ie != HI_NULL) {
                supported_rates_num = puc_supported_rates_ie[1];
            }
        }
        puc_extended_supported_rates_ie = mac_find_ie(MAC_EID_XRATES, beacon_info->tail, beacon_info->tail_len);
        if (puc_extended_supported_rates_ie != HI_NULL) {
            extended_supported_rates_num = puc_extended_supported_rates_ie[1];
        }

        if (supported_rates_num + extended_supported_rates_num == 4) { /* 判断总IE长度是否为4，选择模式 */
            *pen_protocol = WLAN_LEGACY_11B_MODE;
            return HI_SUCCESS;
        }
        if (supported_rates_num + extended_supported_rates_num == 8) { /* 判断总IE长度是否为8，选择模式 */
            *pen_protocol = WLAN_LEGACY_11G_MODE;
            return HI_SUCCESS;
        }
        if (supported_rates_num + extended_supported_rates_num == 12) { /* 判断总IE长度是否为12，选择模式 */
            /* 根据基本速率区分为 11gmix1 还是 11gmix2 */
            /* 如果基本速率集支持 6M , 则判断为 11gmix2 */
            *pen_protocol = WLAN_MIXED_ONE_11G_MODE;
            if (wal_check_support_basic_rate_6m(puc_supported_rates_ie, supported_rates_num,
                puc_extended_supported_rates_ie, extended_supported_rates_num) == HI_TRUE) {
                *pen_protocol = WLAN_MIXED_TWO_11G_MODE;
            }
            return HI_SUCCESS;
        }
    }

    /* 其他情况，认为配置不合理 */
    *pen_protocol = WLAN_PROTOCOL_BUTT;

    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_parse_ht_vht_ie
 功能描述  : 解析内核传递过来的beacon信息中的ht_vht 信息元素
 输入参数  : oal_beacon_parameters *pst_beacon_info
             mac_beacon_param_stru *pst_beacon_param
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_parse_ht_vht_ie(const mac_vap_stru *mac_vap, const oal_beacon_parameters *beacon_info,
    mac_beacon_param_stru *beacon_param)
{
    hi_u8 *puc_ht_ie  = mac_find_ie(MAC_EID_HT_CAP, beacon_info->tail, beacon_info->tail_len);
    hi_u8 *puc_vht_ie = mac_find_ie(MAC_EID_VHT_CAP, beacon_info->tail, beacon_info->tail_len);

    /* 解析协议模式 */
    hi_u32 ret = wal_parse_protocol_mode(mac_vap->channel.band, beacon_info, puc_ht_ie,
        puc_vht_ie, &beacon_param->protocol);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_parse_ht_vht_ie::return err code!}\r\n", ret);
        return ret;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    /* 定制化实现P2P GO 2.4G下默认支持11ac 协议模式 */
    if (is_p2p_go(mac_vap) && (WLAN_BAND_2G == mac_vap->channel.band)) {
        beacon_param->protocol = ((HI_TRUE == mac_vap->cap_flag.ac2g) ? WLAN_VHT_MODE : WLAN_HT_MODE);
    }
#endif /* _PRE_WLAN_FEATURE_P2P */

    /* 解析short gi能力 */
    if (puc_ht_ie == HI_NULL) {
        return HI_SUCCESS;
    }

    /* 使用ht cap ie中数据域的2个字节 */
    if (puc_ht_ie[1] < sizeof(mac_frame_ht_cap_stru)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_parse_ht_vht_ie::invalid htcap ie len %d}\n", puc_ht_ie[1]);
        return HI_SUCCESS;
    }

    mac_frame_ht_cap_stru *ht_cap = (mac_frame_ht_cap_stru *)(puc_ht_ie + MAC_IE_HDR_LEN);

    beacon_param->shortgi_20 = (hi_u8)ht_cap->short_gi_20mhz;
    beacon_param->shortgi_40 = 0;

    if ((mac_vap->channel.en_bandwidth > WLAN_BAND_WIDTH_20M) &&
        (mac_vap->channel.en_bandwidth != WLAN_BAND_WIDTH_BUTT)) {
        beacon_param->shortgi_40 = (hi_u8)ht_cap->short_gi_40mhz;
    }

    beacon_param->smps_mode = (hi_u8)ht_cap->sm_power_save;

    if (puc_vht_ie == HI_NULL) {
        return HI_SUCCESS;
    }

    /* 使用vht cap ie中数据域的4个字节 */
    if (puc_vht_ie[1] < sizeof(mac_vht_cap_info_stru)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_parse_ht_vht_ie::invalid ht cap ie len[%d]!}\r\n",
            puc_vht_ie[1]);
        return HI_SUCCESS;
    }

    mac_vht_cap_info_stru *vht_cap = (mac_vht_cap_info_stru *)(puc_vht_ie + MAC_IE_HDR_LEN);

    beacon_param->shortgi_80 = 0;

    if ((mac_vap->channel.en_bandwidth > WLAN_BAND_WIDTH_40MINUS) &&
        (mac_vap->channel.en_bandwidth != WLAN_BAND_WIDTH_BUTT)) {
        beacon_param->shortgi_80 = vht_cap->short_gi_80mhz;
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_QUICK_START
hi_u32 g_l_scan_enable = HI_FALSE;
hi_u32 hisi_quick_set_scan_enable(hi_s32 l_enable_flag)
{
    return g_l_scan_enable = l_enable_flag;
}
hi_u32 hisi_quick_get_scan_enable(hi_void)
{
    return g_l_scan_enable;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_cfg80211_scan
 功能描述  : 内核调用启动扫描的接口函数
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
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
hi_u32 wal_cfg80211_scan(oal_wiphy_stru *wiphy, oal_cfg80211_scan_request_stru *request)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_scan(oal_wiphy_stru *wiphy, oal_cfg80211_scan_request_stru *request)
#endif
{
    hmac_device_stru           *hmac_dev = HI_NULL;
    mac_vap_stru               *mac_vap = HI_NULL;
    hmac_scan_stru             *scan_mgmt = HI_NULL;
    oal_net_device_stru        *netdev = HI_NULL;

    if ((wiphy == HI_NULL) || (request == HI_NULL)) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan::parameter is wrong, return fail!}");
        goto fail;
    }

    oam_info_log2(0, OAM_SF_SCAN, "{wal_cfg80211_scan::request to scan, channel:%d, ssid:%d}", request->n_channels,
        request->n_ssids);

    netdev = (request->wdev == HI_NULL ? HI_NULL : request->wdev->netdev);
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan::acquire netdev fail, return fail!}");
        goto fail;
    }

    /* 通过net_device 找到对应的mac_vap_stru 结构 */
    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan::cann't acquire mac_vap from netdev, return fail!}");
        goto fail;
    }

#ifdef _PRE_WLAN_FEATURE_WAPI
    if (is_p2p_scan_req(request) && (HI_TRUE == hmac_user_is_wapi_connected())) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan::cann't execute p2p scan under wapi mode, return!}");
        goto fail;
    }
#endif /* #ifdef _PRE_WLAN_FEATURE_WAPI */

    hmac_dev = hmac_get_device_stru();
    scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 如果扫描未完成，则直接返回 */
    if (scan_mgmt->complete == HI_FALSE) {
        oam_warning_log0(0, OAM_SF_SCAN,
            "{wal_cfg80211_scan::the last scan is still running, refuse this scan request.}");
        goto fail;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* 如果当前调度扫描在运行，先暂停调度扫描 */
    if (scan_mgmt->sched_scan_req != HI_NULL) {
        wal_cfg80211_sched_scan_stop(wiphy, netdev);
    }
#endif

    /* 保存当前下发的扫描请求到本地 */
    scan_mgmt->request = request;

    /* 进入扫描 */
    if (HI_SUCCESS != wal_start_scan_req(netdev, scan_mgmt)) {
        scan_mgmt->request = HI_NULL;
        goto fail;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_set_wep_key
 功能描述  : 配置wep加密信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年11月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  1.日    期   : 2014年01月24日
    作    者   : HiSilicon
    修改内容   : 修改函数， 从内核拷贝wep 加密信息到驱动

**************************************************************************** */
static hi_u32 wal_set_wep_key(mac_cfg80211_connect_param_stru *connect_param,
    const oal_cfg80211_connect_params_stru *sme)
{
    connect_param->puc_wep_key         = sme->key;
    connect_param->wep_key_len         = sme->key_len;
    connect_param->wep_key_index       = sme->key_idx;
    connect_param->crypto.cipher_group = (hi_u8)sme->crypto.cipher_group;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理使能PMF STAUT下发n_akm_suites==0的RSN特殊情况
 输入参数  : mac_cfg80211_connect_param_stru   *pst_connect_param
             oal_cfg80211_connect_params_stru    *pst_sme
 返 回 值  : static hi_u32
**************************************************************************** */
static hi_u32 wal_set_crypto_pmf(mac_cfg80211_connect_param_stru *connect, const oal_cfg80211_connect_params_stru *sme,
    const hi_u8 *puc_ie)
{
    hi_u8 loop2, loop3;

    /* 设置WPA/WPA2 加密信息 */
    connect->crypto.control_port = (hi_u8)sme->crypto.control_port;
    connect->crypto.wpa_versions = (hi_u8)sme->crypto.wpa_versions;

    /* puc_ie[1] 为IE字段的长度 */
    if (MAC_RSN_VERSION_LEN + MAC_OUI_LEN + MAC_OUITYPE_WPA + MAC_RSN_CIPHER_COUNT_LEN < puc_ie[1]) {
        return HI_FAIL;
    }

    /* 获取group cipher type */
    connect->crypto.cipher_group = puc_ie[MAC_IE_HDR_LEN + MAC_RSN_VERSION_LEN + MAC_OUI_LEN];

    /* 获取pairwise cipher cout */
    hi_u32 offset = MAC_IE_HDR_LEN + MAC_RSN_VERSION_LEN + MAC_OUI_LEN + MAC_OUITYPE_WPA;
    connect->crypto.n_ciphers_pairwise = puc_ie[offset];
    connect->crypto.n_ciphers_pairwise += (hi_u8)(puc_ie[offset + 1] << 8); /* 左移8位 */
    if (connect->crypto.n_ciphers_pairwise > OAL_NL80211_MAX_NR_CIPHER_SUITES) {
        oam_warning_log1(0, 0, "{wal_set_crypto_pmf:invalid ciphers len:%d!}", connect->crypto.n_ciphers_pairwise);
        return HI_FAIL;
    }
    /* 获取pairwise cipher type */
    offset += MAC_RSN_CIPHER_COUNT_LEN;
    if (connect->crypto.n_ciphers_pairwise) {
        for (loop2 = 0; loop2 < connect->crypto.n_ciphers_pairwise; loop2++) {
            connect->crypto.ciphers_pairwise[loop2] = (hi_u8)puc_ie[offset + MAC_OUI_LEN];
            offset += (MAC_OUITYPE_WPA + MAC_OUI_LEN);
            if (offset - MAC_IE_HDR_LEN < puc_ie[1]) {
                return HI_FAIL;
            }
        }
    }

    if (offset + MAC_RSN_CIPHER_COUNT_LEN - MAC_IE_HDR_LEN < puc_ie[1]) {
        return HI_FAIL;
    }

    /* 获取AKM cout */
    connect->crypto.n_akm_suites = puc_ie[offset];
    connect->crypto.n_akm_suites += (hi_u8)(puc_ie[offset + 1] << 8); /* 左移8位 */
    if (connect->crypto.n_akm_suites > OAL_NL80211_MAX_NR_AKM_SUITES) {
        oam_warning_log1(0, 0, "{wal_set_crypto_pmf:invalid akm len:%d!}", connect->crypto.n_akm_suites);
        return HI_FAIL;
    }
    /* 获取AKM type */
    offset += MAC_RSN_CIPHER_COUNT_LEN;
    if (connect->crypto.n_akm_suites) {
        for (loop3 = 0; loop3 < connect->crypto.n_akm_suites; loop3++) {
            connect->crypto.akm_suites[loop3] = (hi_u8)puc_ie[offset + MAC_OUI_LEN];
            offset += (MAC_OUITYPE_WPA + MAC_OUI_LEN);
            if (offset - MAC_IE_HDR_LEN < puc_ie[1]) {
                return HI_FAIL;
            }
        }
    }

    return HI_SUCCESS;
}


/* ****************************************************************************
 功能描述  : 设置STA connect 加密信息
 输入参数  : mac_cfg80211_connect_param_stru   *pst_connect_param
             oal_cfg80211_connect_params_stru    *pst_sme
 返 回 值  : static hi_u32
**************************************************************************** */
hi_u32 wal_set_crypto_info(mac_cfg80211_connect_param_stru *connect_param, const oal_cfg80211_connect_params_stru *sme)
{
    hi_u8 *puc_ie = mac_find_ie(MAC_EID_RSN, (hi_u8 *)sme->ie, sme->ie_len);
    hi_u8 loop, loop1;

    if ((sme->key_len != 0) && (sme->crypto.n_akm_suites == 0)) {
        /* 设置wep加密信息 */
        return wal_set_wep_key(connect_param, sme);
    } else if (sme->crypto.n_akm_suites != 0) {
        if ((sme->crypto.n_akm_suites > OAL_NL80211_MAX_NR_AKM_SUITES) ||
            (sme->crypto.n_ciphers_pairwise > OAL_NL80211_MAX_NR_CIPHER_SUITES)) {
            oam_warning_log0(0, OAM_SF_CFG, "{wal_set_crypto_info:invalid suites len!}");
            return HI_FAIL;
        }
        /* 设置WPA/WPA2 加密信息 */
        connect_param->crypto.wpa_versions       = (hi_u8)sme->crypto.wpa_versions;
        connect_param->crypto.cipher_group       = (hi_u8)sme->crypto.cipher_group;
        connect_param->crypto.n_ciphers_pairwise = (hi_u8)sme->crypto.n_ciphers_pairwise;
        connect_param->crypto.n_akm_suites       = (hi_u8)sme->crypto.n_akm_suites;
        connect_param->crypto.control_port       = (hi_u8)sme->crypto.control_port;

        for (loop = 0; loop < connect_param->crypto.n_ciphers_pairwise; loop++) {
            connect_param->crypto.ciphers_pairwise[loop] = (hi_u8)sme->crypto.ciphers_pairwise[loop];
        }

        for (loop1 = 0; loop1 < connect_param->crypto.n_akm_suites; loop1++) {
            connect_param->crypto.akm_suites[loop1] = (hi_u8)sme->crypto.akm_suites[loop1];
        }

        return HI_SUCCESS;
    } else if (puc_ie != HI_NULL) {
        /* 处理使能PMF STAUT下发n_akm_suites==0的RSN特殊情况 */
        return wal_set_crypto_pmf(connect_param, sme, puc_ie);
    } else if (mac_find_vendor_ie(MAC_WLAN_OUI_MICROSOFT, MAC_WLAN_OUI_TYPE_MICROSOFT_WPS, (hi_u8 *)sme->ie,
        (hi_s32)(sme->ie_len))) {
        oam_warning_log0(0, OAM_SF_CFG, "{wal_set_crypto_info:connect use wps method!}");

        return HI_SUCCESS;
    }

    return HI_FAIL;
}

#ifdef _PRE_WLAN_FEATURE_P2P
/* ****************************************************************************
 功能描述  : 判断是否为P2P DEVICE .如果是P2P device，则不允许关联。
 输入参数  : oal_net_device_stru *pst_net_device
 返 回 值  : hi_u8 HI_TRUE:P2P DEVICE 设备，
                                 HI_FALSE:非P2P DEVICE 设备
 修改历史      :
  1.日    期   : 2019年5月20日
    作    者   : HiSilicon
    修改内容   : 通过iftype识别p2p dev
**************************************************************************** */
static hi_u8 wal_is_p2p_device(const oal_net_device_stru *netdev)
{
    oal_wireless_dev *wdev = HI_NULL;

    /* 获取mac device */
    wdev = GET_NET_DEV_CFG80211_WIRELESS(netdev);
    return (wdev->iftype == NL80211_IFTYPE_P2P_DEVICE);
}
#endif

hi_u32 wal_cfg80211_start_connect_or_req(oal_net_device_stru *netdev,
    const mac_cfg80211_connect_param_stru *mac_cfg80211_connect_param)
{
    hi_u32 ret;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    ret = wal_cfg80211_start_connect(netdev, mac_cfg80211_connect_param);
#else
    ret = wal_cfg80211_start_req(netdev, mac_cfg80211_connect_param, sizeof(mac_cfg80211_connect_param_stru),
        WLAN_CFGID_CFG80211_START_CONNECT, HI_TRUE);
#endif
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_start_connect_or_req::wal_cfg80211_start_connect fail %u}", ret);
    }

    return ret;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_connect
 功能描述  : 解析内核下发的关联命令，sta启动关联
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2013年10月24日
    作    者   : HiSilicon
    修改内容   : 增加加密认证相关的处理

**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 wal_cfg80211_connect(oal_wiphy_stru *wiphy, oal_net_device_stru *net_device,
    oal_cfg80211_connect_params_stru *sme)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_connect(oal_wiphy_stru *wiphy, oal_net_device_stru *net_device,
    oal_cfg80211_connect_params_stru *sme)
#endif
{
    mac_cfg80211_connect_param_stru mac_cfg80211_connect_param = { 0 };

    if ((wiphy == HI_NULL) || (net_device == HI_NULL) || (sme == HI_NULL)) {
        oam_error_log3(0, OAM_SF_ANY, "{wal_cfg80211_connect::connect failed,wiphy=%p,netdev=%p,sme=%p}",
            (uintptr_t)wiphy, (uintptr_t)net_device, (uintptr_t)sme);
        goto fail;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    if (wal_is_p2p_device(net_device)) {
        oam_warning_log0(0, OAM_SF_ANY, "wal_cfg80211_connect:connect stop, p2p device should not connect.");
        goto fail;
    }
#endif

    /* iw接口下发的关联请求，有可能无信道信息，此时会访问空指针，并且此处获取信道号，在入网过程中，并没有用到 */
    /* 解析内核下发的 ssid */
    mac_cfg80211_connect_param.puc_ssid = (hi_u8 *)sme->ssid;
    mac_cfg80211_connect_param.ssid_len = (hi_u8)sme->ssid_len;

    /* 解析内核下发的 bssid */
    mac_cfg80211_connect_param.puc_bssid = (hi_u8 *)sme->bssid;

    /* 解析内核下发的安全相关参数 */
    /* 设置认证类型 */
    mac_cfg80211_connect_param.auth_type = sme->auth_type;

    /* 设置加密能力 */
    mac_cfg80211_connect_param.privacy = sme->privacy;

    /* 获取内核下发的pmf是使能的结果 */
    mac_cfg80211_connect_param.mfp = sme->mfp;

    oam_warning_log4(0, OAM_SF_ANY, "{wal_cfg80211_connect::start new conn,ssid_len=%d,auth_type=%d,privacy=%d,mfp=%d}",
        sme->ssid_len, sme->auth_type, sme->privacy, sme->mfp);

    /* 设置加密参数 */
#ifdef _PRE_WLAN_FEATURE_WAPI
    if (sme->crypto.wpa_versions == WITP_WAPI_VERSION) {
        oam_warning_log0(0, OAM_SF_ANY, "wal_cfg80211_connect::crypt ver is wapi!");
        mac_cfg80211_connect_param.wapi = HI_TRUE;
    } else {
        mac_cfg80211_connect_param.wapi = HI_FALSE;
    }
#endif

    if (sme->privacy) {
        hi_u32 ret = wal_set_crypto_info(&mac_cfg80211_connect_param, sme);
        if (ret != HI_SUCCESS) {
            oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_connect::connect failed, wal_set_wep_key fail:%d!}\r\n", ret);
            goto fail;
        }
    }

    /* 设置关联P2P/WPS ie */
    mac_cfg80211_connect_param.puc_ie = (hi_u8 *)sme->ie;
    mac_cfg80211_connect_param.ie_len = (hi_u32)(sme->ie_len);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) && \
    ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION))
    wlan_pm_set_timeout(WLAN_SLEEP_LONG_CHECK_CNT);
#endif

    if (wal_cfg80211_start_connect_or_req(net_device, &mac_cfg80211_connect_param) != HI_SUCCESS) {
        goto fail;
    }
    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_disconnect
 功能描述  :
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
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 wal_cfg80211_disconnect(oal_wiphy_stru *wiphy, oal_net_device_stru *net_device, hi_u16 us_reason_code)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_disconnect(oal_wiphy_stru *wiphy, oal_net_device_stru *net_device, hi_u16 us_reason_code)
#endif
{
    mac_cfg_kick_user_param_stru    mac_cfg_kick_user_param;
    hi_u32                          ret;
    mac_user_stru                   *mac_user = HI_NULL;
    mac_vap_stru                    *mac_vap = HI_NULL;
    hi_unref_param(wiphy);

    if (net_device == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_disconnect::input netdev pointer is null!}\r\n");
        goto fail;
    }

    /* 解析内核下发的connect参数 */
    if (memset_s(&mac_cfg_kick_user_param, sizeof(mac_cfg_kick_user_param_stru), 0,
        sizeof(mac_cfg_kick_user_param_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_disconnect::mem safe function err!}");
        goto fail;
    }

    /* 解析内核下发的去关联原因  */
    mac_cfg_kick_user_param.us_reason_code = us_reason_code;

    /* 填写和sta关联的ap mac 地址 */
    mac_vap = oal_net_dev_priv(net_device);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_disconnect::get mac vap ptr is null!}\r\n");
        goto fail;
    }

    mac_user = mac_user_get_user_stru(mac_vap->assoc_vap_id);
    if (mac_user == HI_NULL) {
        oam_warning_log1(0, OAM_SF_ANY,
            "{wal_cfg80211_disconnect::mac_user_get_user_stru pst_mac_user is null, user idx[%d]!}\r\n",
            mac_vap->assoc_vap_id);
        goto fail;
    }

    if (memcpy_s(mac_cfg_kick_user_param.auc_mac_addr, WLAN_MAC_ADDR_LEN,
                 mac_user->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_disconnect::mem safe function err!}");
        goto fail;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    ret = wal_cfg80211_start_disconnect(net_device, &mac_cfg_kick_user_param);
#else
    ret = wal_cfg80211_start_req(net_device, &mac_cfg_kick_user_param, sizeof(mac_cfg_kick_user_param_stru),
        WLAN_CFGID_KICK_USER, HI_TRUE);
#endif
    if (ret != HI_SUCCESS) {
        goto fail;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_add_key
 功能描述  : 配置ptk,gtk等密钥到物理层
 输入参数  : [1]wiphy
             [2]netdev
             [3]p_cfg80211_add_key_info
             [4]mac_addr
             [5]params
 输出参数  : hi_u32
 返 回 值  : 0:成功,其他:失败
**************************************************************************** */
hi_u32 wal_cfg80211_add_key(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index, bool pairwise,
    const hi_u8 *puc_mac_addr, oal_key_params_stru *params)
{
    mac_addkey_param_stru payload_params;
    hi_u32 ret;

    hi_unref_param(wiphy);
    /* 1.1 入参检查 */
    if ((netdev == HI_NULL) || (params == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_add_key::Param Check ERROR,pst_netdev, pst_params %p, %p, %p!}",
            (uintptr_t)netdev, (uintptr_t)params);
        goto fail;
    }

    /* 1.2 key长度检查，防止拷贝越界 */
    if ((params->key_len > OAL_WPA_KEY_LEN) || (params->seq_len > OAL_WPA_SEQ_LEN)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_add_key::Param Check ERROR! key_len[%x]  seq_len[%x]!}",
            (hi_s32)params->key_len, (hi_s32)params->seq_len);
        goto fail;
    }

    /* 2.1 消息参数准备 */
    if (memset_s(&payload_params, sizeof(payload_params), 0, sizeof(payload_params)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_key::mem safe function err!}");
        goto fail;
    }
    payload_params.key_index = key_index;

    if (puc_mac_addr != HI_NULL) {
        /* 不能使用内核下发的mac指针，可能被释放，需要拷贝到本地再使用 */
        if (memcpy_s(payload_params.auc_mac_addr, WLAN_MAC_ADDR_LEN, puc_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_key::mem safe function err!}");
            goto fail;
        }
    }
    payload_params.pairwise = pairwise;

    /* 2.2 获取相关密钥值 */
    payload_params.key.key_len = params->key_len;
    payload_params.key.seq_len = params->seq_len;
    payload_params.key.cipher  = params->cipher;
    if (memcpy_s(payload_params.key.auc_key, OAL_WPA_KEY_LEN, params->key, (hi_u32)params->key_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_key::mem safe function err!}");
        goto fail;
    }

    if (params->seq != HI_NULL && params->seq_len != 0) {
        if (memcpy_s(payload_params.key.auc_seq, OAL_WPA_SEQ_LEN, params->seq, (hi_u32)params->seq_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_key::mem safe function err!}");
            goto fail;
        }
    }
    oam_info_log3(0, OAM_SF_ANY, "{wal_cfg80211_add_key::key_len:%d, seq_len:%d, cipher:0x%08x!}", params->key_len,
        params->seq_len, params->cipher);

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &payload_params, sizeof(mac_addkey_param_stru), WLAN_CFGID_ADD_KEY, HI_TRUE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_add_key::return err code [%u]!}", ret);
        goto fail;
    }
    return HI_SUCCESS;

fail:
    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_remove_key
 功能描述  : 把ptk,gtk等密钥从物理层删除
 输入参数  : [1]wiphy
             [2]netdev
             [3]key_index
             [4]pairwise
             [5]puc_mac_addr
 输出参数  : hi_u32
 返 回 值  : 0:成功,其他:失败
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
hi_u32 wal_cfg80211_remove_key(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index, hi_bool pairwise,
    const hi_u8 *mac_addr)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_remove_key(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index, bool pairwise,
    const hi_u8 *mac_addr)
#endif
{
    mac_removekey_param_stru payload_params = { 0 };
    hi_u32 ret;

    hi_unref_param(wiphy);
    /* 1.1 入参检查 */
    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_remove_key::pst_netdev is null!}");
        goto fail;
    }

    /* 2.1 消息参数准备 */
    payload_params.key_index = key_index;

    if (mac_addr != HI_NULL) {
        /* 不能使用内核下发的mac指针，可能被释放，需要拷贝到本地再使用 */
        if (memcpy_s(payload_params.auc_mac_addr, OAL_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_remove_key::mem safe function err!}");
            goto fail;
        }
    }
    payload_params.pairwise = pairwise;

    oam_info_log2(0, OAM_SF_ANY, "{wal_cfg80211_remove_key::index:%d, pairwise:%d!}", key_index,
        payload_params.pairwise);

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &payload_params, sizeof(mac_removekey_param_stru), WLAN_CFGID_REMOVE_KEY,
        HI_TRUE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_remove_key::return err code [%u]!}", ret);
        goto fail;
    }
    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_set_default_key
 功能描述  : 使配置的密钥生效
 输入参数  : [1]wiphy
             [2]netdev
             [3]key_index
             [4]unicast
             [5]multicast
 返 回 值  : 0:成功,其他:失败
 修改内容  : 合并设置数据帧默认密钥和设置管理帧默认密钥函数
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 wal_cfg80211_set_default_key(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index,
    hi_bool unicast, hi_bool multicast)
#else
hi_s32 wal_cfg80211_set_default_key(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index, bool unicast,
    bool multicast)
#endif
{
    mac_setdefaultkey_param_stru payload_params = { 0 };
    hi_u32 ret;

    hi_unref_param(wiphy);
    /* 1.1 入参检查 */
    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_set_default_key::pst_netdev ptr is null!}\r\n");
        goto fail;
    }

    /* 2.1 消息参数准备 */
    payload_params.key_index = key_index;
    payload_params.unicast = unicast;
    payload_params.multicast = multicast;

    oam_info_log3(0, OAM_SF_ANY, "{wal_cfg80211_set_default_key::key_index:%d, unicast:%d, multicast:%d!}\r\n",
        key_index, payload_params.unicast, payload_params.multicast);

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &payload_params, sizeof(mac_setdefaultkey_param_stru), WLAN_CFGID_DEFAULT_KEY,
        HI_FALSE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_default_key::return err code [%u]!}", ret);
        goto fail;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_set_ssid
 功能描述  : 启动ap
 输入参数  : oal_net_device_stru   *pst_netdev
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_cfg80211_set_ssid(oal_net_device_stru *netdev, const hi_u8 *puc_ssid_ie, hi_u8 ssid_len)
{
    mac_cfg_ssid_param_stru      ssid_param = {0};
    hi_u32                       ret;

    /* 2.1 消息参数准备 */
    ssid_param.ssid_len = ssid_len;
    if (memcpy_s(ssid_param.ac_ssid, WLAN_SSID_MAX_LEN, (hi_s8 *)puc_ssid_ie, ssid_len) != EOK) {
        oam_error_log0(0, 0, "{wal_cfg80211_set_ssid::mem safe function err!}");
        return HI_FAIL;
    }

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &ssid_param, sizeof(mac_cfg_ssid_param_stru), WLAN_CFGID_SSID, HI_FALSE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_ssid::return err code [%u]!}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  :设置meshid
 输入参数  : oal_net_device_stru   *pst_netdev
                            hi_u8 *puc_ssid_ie
                            hi_u8 uc_ssid_len
 返 回 值  : static hi_s32
 修改历史      :
  1.日    期   : 2019年3月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_cfg80211_set_meshid(oal_net_device_stru *netdev, const hi_u8 *puc_meshid_ie, hi_u8 meshid_len)
{
    mac_cfg_ssid_param_stru      ssid_param = {0};
    hi_u32                       ret;

    /* 2.1 消息参数准备 */
    ssid_param.ssid_len = meshid_len;
    if (memcpy_s(ssid_param.ac_ssid, WLAN_SSID_MAX_LEN, (hi_s8 *)puc_meshid_ie, meshid_len) != EOK) {
        oam_error_log0(0, 0, "{wal_cfg80211_set_meshid::mem safe function err!}");
        return HI_FAIL;
    }

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &ssid_param, sizeof(mac_cfg_ssid_param_stru), WLAN_CFGID_MESHID, HI_FALSE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_meshid::return err code [%u]!}", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif

hi_u32 wal_cfg80211_configuration_beacon(const mac_vap_stru *mac_vap, const oal_beacon_data_stru *beacon_info,
    mac_beacon_param_stru *beacon_param)
{
    oal_beacon_parameters beacon_info_tmp = { 0 };
    hi_u32 ret;

    /* ****************************************************************************
        1.安全配置ie消息等
    **************************************************************************** */
    hi_u16 beacon_head_len = (hi_u16)beacon_info->head_len;
    hi_u16 beacon_tail_len = (hi_u16)beacon_info->tail_len;
    hi_u32 beacon_len = (hi_u32)beacon_head_len + (hi_u32)beacon_tail_len;
    hi_u8 *puc_beacon_info_tmp = (hi_u8 *)(oal_memalloc(beacon_len));
    if (puc_beacon_info_tmp == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::memalloc failed.}");
        return HI_ERR_CODE_PTR_NULL;
    } else {
        /* 复制beacon内容 （11b模式下，beacon_tail_len为0） */
        if (beacon_tail_len != 0) {
            ret = (hi_u32)memcpy_s(puc_beacon_info_tmp, beacon_head_len, beacon_info->head, beacon_head_len);
            ret |= (hi_u32)memcpy_s(puc_beacon_info_tmp + beacon_head_len, beacon_tail_len, beacon_info->tail,
                beacon_tail_len);
        } else {
            ret = (hi_u32)memcpy_s(puc_beacon_info_tmp, beacon_head_len, beacon_info->head, beacon_head_len);
        }
        if (ret != EOK) {
            oam_error_log0(0, 0, "{wal_cfg80211_fill_beacon_param::mem safe function err!}");
            oal_free(puc_beacon_info_tmp);
            return HI_FAIL;
        }
    }

    beacon_info_tmp.head     = puc_beacon_info_tmp;
    beacon_info_tmp.head_len = (hi_u32)beacon_head_len;
    beacon_info_tmp.tail     = puc_beacon_info_tmp + (hi_u32)beacon_head_len;
    beacon_info_tmp.tail_len = (hi_u32)beacon_tail_len;

    /* 获取 WPA/WPA2 信息元素 */
    ret = wal_parse_wpa_wpa2_ie(&beacon_info_tmp, beacon_param);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::failed to parse WPA/WPA2 ie!}");
        oal_free(puc_beacon_info_tmp);
        return ret;
    }

    ret = wal_parse_ht_vht_ie(mac_vap, &beacon_info_tmp, beacon_param);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::failed to parse HT/VHT ie!}");
        oal_free(puc_beacon_info_tmp);
        return ret;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    /* 获取mesh conf信息元素 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        ret = wal_parse_mesh_conf_ie(&beacon_info_tmp, beacon_param);
        if (ret != HI_SUCCESS) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::Mesh VAP fail parse ie!}");
            oal_free(puc_beacon_info_tmp);
            return ret;
        }
    }
#endif
    /* 释放临时申请的内存 */
    oal_free(puc_beacon_info_tmp);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_fill_beacon_param
 功能描述  : 将要下发的修改的beacon帧参数填入到入参结构体中
 输入参数  : mac_vap_stru                *pst_mac_vap,
             struct cfg80211_beacon_data *pst_beacon_info,
             mac_beacon_param_stru       *pst_beacon_param
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_fill_beacon_param(mac_vap_stru *mac_vap, oal_beacon_data_stru *beacon_info,
    mac_beacon_param_stru *beacon_param)
{
    hi_u32 loop, loop1;

    if (mac_vap == HI_NULL || beacon_info == HI_NULL || beacon_param == HI_NULL) {
        oam_error_log3(0, OAM_SF_ANY,
            "{wal_cfg80211_fill_beacon_param::param is NULL. pst_mac_vap=%p, pst_beacon_info=%p, pst_beacon_param=%p",
            (uintptr_t)mac_vap, (uintptr_t)beacon_info, (uintptr_t)beacon_param);
        return HI_ERR_CODE_PTR_NULL;
    }
    if (beacon_info->tail == HI_NULL || beacon_info->head == HI_NULL) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_ANY,
            "{wal_cfg80211_fill_beacon_param::beacon frame error tail = %p, head = %p!}", (uintptr_t)beacon_info->tail,
            (uintptr_t)beacon_info->head);
        return HI_ERR_CODE_PTR_NULL;
    }

    hi_u32 ret = wal_cfg80211_configuration_beacon(mac_vap, beacon_info, beacon_param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    oam_warning_log3(mac_vap->vap_id, OAM_SF_ANY,
        "{wal_cfg80211_fill_beacon_param::crypto_mode=%d, group_crypt=%d, en_protocol=%d!}", beacon_param->crypto_mode,
        beacon_param->group_crypto, beacon_param->protocol);

    oam_warning_log2(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::auth_type[0]=%d, auth_type[1]=%d}",
        beacon_param->auc_auth_type[0], beacon_param->auc_auth_type[1]);

#ifdef _PRE_WLAN_FEATURE_MESH
    /* 打印调试使用 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_ANY,
            "{wal_cfg80211_fill_beacon_param::mesh formation info = %d, mesh capability = %d}",
            beacon_param->mesh_formation_info, beacon_param->mesh_capability);
    }
#endif

    /* 对日本14信道作特殊判断，只在11b模式下才能启用14，非11b模式 降为11b */
    if ((mac_vap->channel.chan_number == 14) && (beacon_param->protocol != WLAN_LEGACY_11B_MODE)) { /* 14：代表信道号 */
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
                       "{wal_cfg80211_fill_beacon_param::ch 14 should in 11b, but is %d!}", beacon_param->protocol);
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::change protocol to 11b!}");
        beacon_param->protocol = WLAN_LEGACY_11B_MODE;
    }

    for (loop = 0; loop < MAC_PAIRWISE_CIPHER_SUITES_NUM; loop++) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::wpa pariwise[%d] = %d!}",
                         loop, beacon_param->auc_pairwise_crypto_wpa[loop]);
    }

    for (loop1 = 0; loop1 < MAC_PAIRWISE_CIPHER_SUITES_NUM; loop1++) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::wpa2 pariwise[%d] = %d!}",
            loop1, beacon_param->auc_pairwise_crypto_wpa2[loop1]);
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_change_beacon
 功能描述  : 修改ap beacon帧配置参数
 输入参数  : oal_wiphy_stru          *pst_wiphy
             oal_net_device_stru     *pst_netdev
             struct cfg80211_beacon_data *info
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 wal_cfg80211_change_beacon(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, oal_beacon_data_stru *beacon_info)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_change_beacon(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, oal_beacon_data_stru *beacon_info)
#endif
{
    mac_beacon_param_stru        beacon_param;  /* beacon info struct */
    mac_vap_stru                *mac_vap = HI_NULL;
    hi_u32                       ret;

    hi_unref_param(wiphy);

    /* 参数合法性检查 */
    if ((netdev == HI_NULL) || (beacon_info == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_change_beacon::pst_netdev = %p, pst_beacon_info = %p!}",
            (uintptr_t)netdev, (uintptr_t)beacon_info);
        goto fail;
    }

    /* 获取vap id */
    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_change_beacon::pst_mac_vap = %p}", (uintptr_t)mac_vap);
        goto fail;
    }

    /* 初始化beacon interval 和DTIM_PERIOD 参数 */
    if (memset_s(&beacon_param, sizeof(mac_beacon_param_stru), 0, sizeof(mac_beacon_param_stru)) != EOK) {
        oam_error_log0(0, 0, "{wal_cfg80211_change_beacon::mem safe function err!}");
        goto fail;
    }
    ret = wal_cfg80211_fill_beacon_param(mac_vap, beacon_info, &beacon_param);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
            "{wal_cfg80211_change_beacon::failed to fill beacon param, error[%d]}", ret);
        goto fail;
    }

    /* 设置操作类型 */
    beacon_param.operation_type = MAC_SET_BEACON;

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &beacon_param, sizeof(mac_beacon_param_stru),
        WLAN_CFGID_CFG80211_CONFIG_BEACON, HI_FALSE);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_change_beacon::Failed to start addset beacon, error[%d]!}", ret);
        goto fail;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_convert_width_to_value
 功能描述  : 将内核下发的带宽枚举转换成真实的带宽宽度值
 输入参数  : [1]l_channel_width
 输出参数  : 无
 返 回 值  : static hi_s32
**************************************************************************** */
static hi_u32 wal_cfg80211_convert_width_to_value(hi_s32 l_channel_width)
{
    hi_u32 l_channel_width_value = 0;

    switch (l_channel_width) {
        case 0: /* 0 内核带宽 */
        case 1: /* 1 内核带宽 */
            l_channel_width_value = 20; /* 20 真实的带宽值 */
            break;
        case 2: /* 2 内核带宽 */
            l_channel_width_value = 40; /* 40 真实的带宽值 */
            break;
        case 3: /* 3 内核带宽 */
        case 4: /* 4 内核带宽 */
            l_channel_width_value = 80; /* 80 真实的带宽值 */
            break;
        case 5: /* 5 内核带宽 */
            l_channel_width_value = 160; /* 160 真实的带宽值 */
            break;
        default:
            break;
    }

    return l_channel_width_value;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_set_channel_info
 功能描述  : 设置信道
 输入参数  : oal_wiphy_stru           *pst_wiphy
             oal_net_device_stru      *pst_netdev
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_set_channel_info(oal_net_device_stru *netdev)
{
    mac_cfg_channel_param_stru channel_param = { 0 };
    wlan_channel_bandwidth_enum_uint8 bandwidth;

    oal_ieee80211_channel *channel = (oal_ieee80211_channel *)GET_NET_DEV_CFG80211_WIRELESS(netdev)->preset_chandef.chan;
    hi_s32 l_bandwidth = GET_NET_DEV_CFG80211_WIRELESS(netdev)->preset_chandef.width;
    hi_s32 l_center_freq1 = GET_NET_DEV_CFG80211_WIRELESS(netdev)->preset_chandef.center_freq1;
    hi_s32 l_channel      = channel->hw_value;

    /* 判断信道在不在管制域内 */
    hi_u32 ret = mac_is_channel_num_valid(channel->band, (hi_u8)l_channel);
    if (ret != HI_SUCCESS) {
        oam_warning_log3(0, OAM_SF_ANY, "{wal_cfg80211_set_channel::channel Err.band=%d,ch=%d,ErrCode=%u}",
            channel->band, l_channel, ret);
        return ret;
    }

    /* 进行内核带宽值和WITP 带宽值转换 */
    hi_s32 l_channel_center_freq = oal_ieee80211_frequency_to_channel(l_center_freq1);
    hi_u32 l_bandwidth_value     = wal_cfg80211_convert_width_to_value(l_bandwidth);
    if (l_bandwidth_value == 0) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_channel::channelWidth Err,l_bandwidth=%d", l_bandwidth);
        return HI_FAIL;
    }

    if (l_bandwidth_value == 80) { /* 80：代表带宽值 */
        bandwidth = mac_get_bandwith_from_center_freq_seg0((hi_u8)l_channel, (hi_u8)l_channel_center_freq);
    } else if (l_bandwidth_value == 40) { /* 40：代表带宽值 */
        switch (l_channel_center_freq - l_channel) {
            case -2: /* -2: 内核带宽 */
                bandwidth = WLAN_BAND_WIDTH_40MINUS;
                break;
            case 2: /* 2: 内核带宽 */
                bandwidth = WLAN_BAND_WIDTH_40PLUS;
                break;
            default:
                bandwidth = WLAN_BAND_WIDTH_20M;
                break;
        }
    } else {
        bandwidth = WLAN_BAND_WIDTH_20M;
    }

    /* 2.1 消息参数准备 */
    channel_param.channel      = (hi_u8)channel->hw_value;
    channel_param.band         = channel->band;
    channel_param.en_bandwidth = bandwidth;

    oam_warning_log3(0, OAM_SF_ANY, "{wal_cfg80211_set_channel::channel=%d,band=%d,bandwidth=%d}",
        channel_param.channel, channel_param.band, channel_param.en_bandwidth);

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &channel_param, sizeof(mac_cfg_channel_param_stru),
        WLAN_CFGID_CFG80211_SET_CHANNEL, HI_TRUE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_channel_info::return err code [%u]!}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_u32 wal_wifi_set_bw(oal_net_device_stru *netdev, wal_wifi_bw_enum_int bw)
{
    hi_char ac_bw[WAL_BW_STR_MAX_LEN] = {0};
    if ((bw > WAL_WIFI_BW_HIEX_5M) || (bw < WAL_WIFI_BW_LEGACY_20M)) {
        oam_error_log0(0, 0, "hi_wifi_set_bandwidth invalid bw.");
        return HI_FAIL;
    }

    strcpy_s(ac_bw, WAL_BW_STR_MAX_LEN, "20");
    if (bw == WAL_WIFI_BW_HIEX_5M) {
        strcpy_s(ac_bw, WAL_BW_STR_MAX_LEN, "5");
    } else if (bw == WAL_WIFI_BW_HIEX_10M) {
        strcpy_s(ac_bw, WAL_BW_STR_MAX_LEN, "10");
    }

    if (wal_hipriv_set_bw(netdev, (hi_char *)ac_bw) != HI_SUCCESS) {
        oam_error_log0(0, 0, "wal_hipriv_set_bw failed.");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_start_ap
 功能描述  : 启动AP,配置AP 参数。
 输入参数  : oal_wiphy_stru              *pst_wiphy
             oal_net_device_stru         *pst_netdev
             struct cfg80211_ap_settings *settings
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 wal_cfg80211_start_ap(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, oal_ap_settings_stru *ap_settings)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 wal_cfg80211_start_ap(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, oal_ap_settings_stru *ap_settings)
#endif
{
    mac_beacon_param_stru beacon_param = { 0 }; /* beacon info struct */

    hi_unref_param(wiphy);

    /* 参数合法性检查 */
    if ((netdev == HI_NULL) || (ap_settings == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_start_ap:: %p, %p!}", (uintptr_t)netdev, (uintptr_t)ap_settings);
        goto fail;
    }

    /* 获取vap id */
    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_start_ap::pst_mac_vap = %p}", (uintptr_t)mac_vap);
        goto fail;
    }

    /* ****************************************************************************
        1.设置信道
    **************************************************************************** */
    if (wal_cfg80211_set_channel_info(netdev) != HI_SUCCESS) {
        goto fail;
    }

    /* ****************************************************************************
        2.设置ssid等信息
    **************************************************************************** */
    if ((ap_settings->ssid_len > 32) || (ap_settings->ssid_len == 0)) { /* 32: 长度上界 */
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_start_ap: len[%d].}", ap_settings->ssid_len);
        goto fail;
    }

    if (wal_cfg80211_set_ssid(netdev, ap_settings->ssid, (hi_u8)ap_settings->ssid_len) != HI_SUCCESS) {
        goto fail;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    /* ****************************************************************************
        2.Mesh设置meshid等信息,与ssid一致
    **************************************************************************** */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        if (wal_cfg80211_set_meshid(netdev, ap_settings->ssid, (hi_u8)ap_settings->ssid_len) != HI_SUCCESS) {
            goto fail;
        }
    }
#endif

    /* ****************************************************************************
        3.设置beacon时间间隔、tim period以及安全配置消息等
    **************************************************************************** */
    /* 初始化beacon interval 和DTIM_PERIOD 参数 */
    beacon_param.l_interval = ap_settings->beacon_interval;
    beacon_param.l_dtim_period = ap_settings->dtim_period;
    beacon_param.hidden_ssid = (ap_settings->hidden_ssid == 1);

    oam_warning_log3(0, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param:beacon_interval=%d,dtim_period=%d,hidden_ssid=%d}",
        ap_settings->beacon_interval, ap_settings->dtim_period, ap_settings->hidden_ssid);

    if (wal_cfg80211_fill_beacon_param(mac_vap, &(ap_settings->beacon), &beacon_param) != HI_SUCCESS) {
        goto fail;
    }

    /* 设置操作类型 */
    beacon_param.operation_type = MAC_ADD_BEACON;

    /* 抛事件给驱动 */
    if (wal_cfg80211_start_req(netdev, &beacon_param, sizeof(mac_beacon_param_stru), WLAN_CFGID_CFG80211_CONFIG_BEACON,
        HI_FALSE) != HI_SUCCESS) {
        goto fail;
    }

    /* ****************************************************************************
        4.启动ap
    **************************************************************************** */
    hi_u32 ret = wal_start_vap(netdev);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_start_ap::failed to start ap, error[%u]}", ret);
        goto fail;
    }
    /* 设置net_device里flags标志 */
    if ((oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) != 0) {
        oal_netdevice_flags(netdev) &= (~OAL_IFF_RUNNING);
    }
    if (wal_wifi_set_bw(netdev, g_bw) != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_start_ap::failed to set bw}");
        goto fail;
    }
    if ((oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) == 0) {
        oal_netdevice_flags(netdev) |= OAL_IFF_RUNNING;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_change_virtual_intf
 功能描述  : 转换AP，STA 状态
 输入参数  : [1]wiphy
             [2]net_dev
             [3]type        下一个状态
             [4]pul_flags
             [5]params
 输出参数  : 无
 返 回 值  : static hi_s32
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
hi_s32 wal_cfg80211_change_virtual_intf(oal_wiphy_stru *wiphy, oal_net_device_stru *net_dev, enum_nl80211_iftype type,
    hi_u32 *pul_flags, oal_vif_params_stru *params)
{
    wlan_p2p_mode_enum_uint8 p2p_mode;
    wlan_vap_mode_enum_uint8 vap_mode;
#ifdef _PRE_WLAN_FEATURE_P2P
    mac_cfg_del_vap_param_stru del_vap_param;
    mac_cfg_add_vap_param_stru add_vap_param;
    mac_vap_stru *mac_vap = HI_NULL;
#endif
    hi_unref_param(wiphy);

    /* 1.1 入参检查 */
    if (net_dev == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::pst_dev is null!}\r\n");
        return -HI_ERR_CODE_PTR_NULL;
    }

    if (params == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::pst_params ptr is null!}\r\n");
        return -HI_ERR_CODE_PTR_NULL;
    }

    /* 检查VAP 当前模式和目的模式是否相同，如果相同则直接返回 */
    if (net_dev->ieee80211Ptr->iftype == type) {
        oam_warning_log1(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::same iftype[%d],do not need change !}\r\n",
            type);
        return HI_SUCCESS;
    }

    oam_warning_log2(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::[%d][%d]}\r\n",
                     (net_dev->ieee80211Ptr->iftype), type);

    switch (type) {
        case NL80211_IFTYPE_MONITOR:
        case NL80211_IFTYPE_WDS:
        case NL80211_IFTYPE_ADHOC:
            oam_error_log1(0, OAM_SF_CFG,
                "{wal_cfg80211_change_virtual_intf::currently we do not support this type[%d]}\r\n", type);
            return -HI_ERR_WIFI_WAL_INVALID_PARAMETER;
#ifdef _PRE_WLAN_FEATURE_MESH
        case NL80211_IFTYPE_MESH_POINT:
            vap_mode = WLAN_VAP_MODE_MESH;
            p2p_mode = WLAN_LEGACY_VAP_MODE;
            break;
#endif
        case NL80211_IFTYPE_STATION:
            if (net_dev->ieee80211Ptr->iftype == NL80211_IFTYPE_AP) {
                /* 结束扫描,以防在20/40M扫描过程中关闭AP */
                wal_force_scan_complete(net_dev);

                /* AP关闭切换到STA模式,删除相关vap */
                if (HI_SUCCESS != wal_stop_vap(net_dev)) {
                    oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::wal_stop_vap enter an error.}");
                }
                if (HI_SUCCESS != wal_deinit_wlan_vap(net_dev)) {
                    oam_warning_log0(0, OAM_SF_CFG,
                        "{wal_cfg80211_change_virtual_intf::wal_deinit_wlan_vap enter an error.}");
                }

                net_dev->ieee80211Ptr->iftype = type;

                return HI_SUCCESS;
            }
            {
                net_dev->ieee80211Ptr->iftype = type; /* P2P BUG P2P_DEVICE 提前创建，不需要通过wpa_supplicant 创建 */
                oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::change to station}\r\n");
            }
            return HI_SUCCESS;
        case NL80211_IFTYPE_P2P_CLIENT:
            vap_mode = WLAN_VAP_MODE_BSS_STA;
            p2p_mode = WLAN_P2P_CL_MODE;
            break;
        case NL80211_IFTYPE_AP:
        case NL80211_IFTYPE_AP_VLAN:
            vap_mode = WLAN_VAP_MODE_BSS_AP;
            p2p_mode = WLAN_LEGACY_VAP_MODE;
            break;
        case NL80211_IFTYPE_P2P_GO:
            vap_mode = WLAN_VAP_MODE_BSS_AP;
            p2p_mode = WLAN_P2P_GO_MODE;
            break;
        default:
            oam_error_log1(0, OAM_SF_CFG,
                "{wal_cfg80211_change_virtual_intf::currently we do not support this type[%d]}\r\n", type);
            return -HI_ERR_CODE_PTR_NULL;
    }

    if ((type == NL80211_IFTYPE_AP) || (type == NL80211_IFTYPE_MESH_POINT)) {
        net_dev->ieee80211Ptr->iftype = type;
        if (wal_setup_vap(net_dev) != HI_SUCCESS) {
            return -HI_FAIL;
        }
        return HI_SUCCESS;
    }
#ifdef _PRE_WLAN_FEATURE_P2P
    /* 设备为P2P 设备才需要进行change virtual interface */
    mac_vap = oal_net_dev_priv(net_dev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG,
            "{wal_cfg80211_change_virtual_intf::can't get mac vap from netdevice priv data.}\r\n");
        return -HI_ERR_CODE_PTR_NULL;
    }

    if (is_legacy_vap(mac_vap)) {
        net_dev->ieee80211Ptr->iftype = type;
        return HI_SUCCESS;
    }

    if ((strcmp("p2p0", (const hi_char *)net_dev->name)) == 0) {
        /* 解决异常情况下,wpa_supplicant下发p2p0设备切换到p2p go/cli模式导致fastboot的问题 */
        oam_warning_log0(0, OAM_SF_CFG,
            "{wal_cfg80211_change_virtual_intf::p2p0 netdevice can not change to P2P CLI/GO.}\r\n");
        return -HI_FAIL;
    }

    /* 如果当前模式和目的模式不同，则需要:
       1. 停止 VAP
       2. 删除 VAP
       3. 重新创建对应模式VAP
       4. 启动VAP
    */
    /* 停止VAP */
    wal_netdev_stop(net_dev);
    if (memset_s(&del_vap_param, sizeof(del_vap_param), 0, sizeof(del_vap_param)) != EOK) {
        return -HI_FAIL;
    }
    /* 删除VAP */
    del_vap_param.net_dev = net_dev;
    /* 设备p2p 模式需要从net_device 中获取 */
    del_vap_param.p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(net_dev->ieee80211Ptr->iftype);
    if (wal_cfg80211_del_vap(&del_vap_param)) {
        return -HI_FAIL;
    }

    if (memset_s(&add_vap_param, sizeof(add_vap_param), 0, sizeof(add_vap_param)) != EOK) {
        return -HI_FAIL;
    }
    /* 重新创建对应模式VAP */
    add_vap_param.net_dev = net_dev;
    add_vap_param.vap_mode = vap_mode;
    add_vap_param.p2p_mode = p2p_mode;
    if (wal_cfg80211_add_vap(&add_vap_param) != HI_SUCCESS) {
        return -HI_FAIL;
    }
    /* 启动VAP */
    wal_netdev_open(net_dev);

    net_dev->ieee80211Ptr->iftype = type;
#endif
    return HI_SUCCESS;
}
#else
hi_u32 wal_cfg80211_intf_mode_check(oal_net_device_stru *netdev, nl80211_iftype_uint8 type)
{
    switch (type) {
        case NL80211_IFTYPE_MONITOR:
        case NL80211_IFTYPE_WDS:
        case NL80211_IFTYPE_ADHOC:
            oam_error_log1(0, OAM_SF_CFG,
                "{wal_cfg80211_change_virtual_intf::currently we do not support this type[%d]}\r\n", type);
            return HI_ERR_WIFI_WAL_INVALID_PARAMETER;
        case NL80211_IFTYPE_STATION:
            if (GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_AP) {
                /* 结束扫描,以防在20/40M扫描过程中关闭AP */
                wal_force_scan_complete(netdev);

                /* AP关闭切换到STA模式,删除相关vap */
                if (wal_stop_vap(netdev) != HI_SUCCESS) {
                    oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::wal_stop_vap enter an error.}");
                }
                if (wal_deinit_wlan_vap(netdev) != HI_SUCCESS) {
                    oam_warning_log0(0, OAM_SF_CFG,
                        "{wal_cfg80211_change_virtual_intf::wal_deinit_wlan_vap enter an error.}");
                }

                GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype = type;

                return HI_SUCCESS;
            }
            GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype = type; /* P2P BUG P2P_DEVICE 提前创建，不需要通过wpa_supplicant 创建 */
            oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::change to station}\r\n");
            return HI_SUCCESS;
#ifdef _PRE_WLAN_FEATURE_MESH
        case NL80211_IFTYPE_MESH_POINT:
#endif
        case NL80211_IFTYPE_P2P_CLIENT:
        case NL80211_IFTYPE_AP:
        case NL80211_IFTYPE_AP_VLAN:
        case NL80211_IFTYPE_P2P_GO:
            break;
        default:
            oam_error_log1(0, OAM_SF_CFG,
                "{wal_cfg80211_change_virtual_intf::currently we do not support this type[%d]}\r\n", type);
            return HI_ERR_CODE_PTR_NULL;
    }

    return HI_CONTINUE;
}

hi_u32 wal_cfg80211_change_virtual_intf(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, nl80211_iftype_uint8 type,
    hi_u32 *pul_flags, oal_vif_params_stru *params)
{
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u32 ret;
    hi_unref_param(wiphy);

    /* 1.1 入参检查 */
    if (netdev == HI_NULL || params == HI_NULL) {
        oam_error_log2(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::params null! netdev=%p, params=%p",
            (uintptr_t)netdev, (uintptr_t)params);
        return HI_ERR_CODE_PTR_NULL;
    }
    oam_warning_log1(0, OAM_SF_CFG, "wal_cfg80211_change_virtual_intf::iftype[%d],enter", type);

    if (GET_NET_DEV_CFG80211_WIRELESS(netdev) == NULL)
    {
        oam_warning_log0(0, OAM_SF_CFG, "wal_cfg80211_change_virtual_intf:: null");
    }

    /* 检查VAP 当前模式和目的模式是否相同，如果相同则直接返回 */
    if (GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == type) {
        oam_warning_log1(0, OAM_SF_CFG, "wal_cfg80211_change_virtual_intf::same iftype[%d],do not need change!", type);
        return HI_SUCCESS;
    }

    oam_warning_log2(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::[%d][%d]}\r\n",
        (GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype), type);
    *pul_flags = 0;

    ret = wal_cfg80211_intf_mode_check(netdev, type);
    if (ret != HI_CONTINUE) {
        return ret;
    }

    if ((type == NL80211_IFTYPE_AP) || (type == NL80211_IFTYPE_MESH_POINT)) {
        GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype = type;
        return wal_setup_vap(netdev);
    }

    /* 设备为P2P 设备才需要进行change virtual interface */
    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::oal_net_dev_priv fail!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (is_legacy_vap(mac_vap)) {
        GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype = type;
        return HI_SUCCESS;
    }

    if ((strcmp("p2p0", (const hi_char *)netdev->name)) == 0) {
        /* 解决异常情况下,wpa_supplicant下发p2p0设备切换到p2p go/cli模式导致fastboot的问题 */
        oam_warning_log0(0, OAM_SF_CFG,
            "{wal_cfg80211_change_virtual_intf::p2p0 netdevice can not change to P2P CLI/GO.}\r\n");
        return HI_FAIL;
    }

    /* 如果当前模式和目的模式不同，则需要:
       1. 停止 VAP
       2. 删除 VAP
       3. 重新创建对应模式VAP
       4. 启动VAP
    */
    /* 停止VAP */
    wal_netdev_stop(netdev);
    if (wal_deinit_wlan_vap(netdev) != HI_SUCCESS) {
        return HI_FAIL;
    }

    GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype = type;
    if (wal_init_wlan_vap(netdev) != HI_SUCCESS) {
        return HI_FAIL;
    }
    /* 启动VAP */
    wal_netdev_open(netdev);

    return HI_SUCCESS;
}
#endif

hi_u32 wal_cfg80211_del_send_event(oal_net_device_stru *netdev, const hi_u8 *mac_addr, mac_vap_stru *mac_vap)
{
    mac_cfg_kick_user_param_stru kick_user_param;
    hi_s32                       user_count_ok   = 0;
    hi_s32                       user_count_fail = 0;
    hi_u32                       ret;

    hi_unref_param(mac_vap);

    kick_user_param.us_reason_code = MAC_INACTIVITY;
#ifdef _PRE_WLAN_FEATURE_MESH
    kick_user_param.us_reason_code =
        (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) ? MAC_WPA_KICK_MESH_USER : kick_user_param.us_reason_code;
#endif

    if (memcpy_s(kick_user_param.auc_mac_addr, OAL_MAC_ADDR_LEN, mac_addr, OAL_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_send_event::mem safe function err!}");
        return HI_FAIL;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    ret = wal_cfg80211_start_disconnect(netdev, &kick_user_param);
#else
    ret = wal_cfg80211_start_req(netdev, &kick_user_param, sizeof(mac_cfg_kick_user_param_stru), WLAN_CFGID_KICK_USER,
        HI_TRUE);
#endif
    if (ret != HI_SUCCESS) {
        /* 由于删除的时候可能用户已经删除，此时再进行用户查找，会返回错误，输出ERROR打印，修改为warning */
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_del_send_event::kick_user Err=%d}", ret);
        user_count_fail++;
    } else {
        user_count_ok++;
    }

    if (user_count_fail > 0) {
        oam_info_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_del_send_event::%d user deleteErr}", user_count_fail);
        return HI_ERR_CODE_PTR_NULL;
    }

    oam_info_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_del_send_event::%d user delete OK}", user_count_ok);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_del_station
 功能描述  : 删除用户
 输入参数  : oal_wiphy_stru *pst_wiphy
             oal_net_device *pst_dev
             hi_u8 *puc_mac         用户mac 地址。如果mac = NULL,删除所有用户
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_del_station(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev,
    oal_station_del_parameters_stru *params)
{
    hi_u8 bcast_mac_addr[OAL_MAC_ADDR_LEN];
    hi_u8 user_idx;

    hi_unref_param(wiphy);
    if (netdev == HI_NULL) {
        goto fail;
    }

    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_station::can't get mac vap from netdevice priv data!}\r\n");
        goto fail;
    }

    /* 判断是否是AP模式 */
    if ((mac_vap->vap_mode != WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        && (mac_vap->vap_mode != WLAN_VAP_MODE_MESH)
#endif
    ) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_del_station::vap_mode=%d Err}", mac_vap->vap_mode);
        goto fail;
    }

    if (params->mac == HI_NULL) {
        /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
        memset_s(bcast_mac_addr, OAL_MAC_ADDR_LEN, 0xff, OAL_MAC_ADDR_LEN);

        params->mac = bcast_mac_addr;
        oam_info_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_del_station::deleting all user!}\r\n");
    } else {
        oam_info_log3(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_del_station::delete user:XX:XX:XX:%02X:%02X:%02X}",
            params->mac[3], params->mac[4], params->mac[5]); /* 3, 4, 5: 数组下标 */
    }

    hi_u32 ret = mac_vap_find_user_by_macaddr(mac_vap, (hi_u8 *)params->mac, OAL_MAC_ADDR_LEN, &user_idx);

    if (ret != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "{wal_cfg80211_del_station::user has been deleted}\r\n");
        goto fail;
    }
    ret = wal_cfg80211_del_send_event(netdev, params->mac, mac_vap);

    if (ret != HI_SUCCESS) {
        goto fail;
    }

    return HI_SUCCESS;

fail:
    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_check_cookie_timeout
 功能描述  : 删除cookie 列表中超时的cookie
 输入参数  : cookie_arry_stru *pst_cookie_array
             hi_u32 ul_current_time
 输出参数  : 无
 返 回 值  : hi_void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_check_cookie_timeout(cookie_arry_stru *cookie_array, hi_u8 *puc_cookie_bitmap)
{
    hi_u8               loops = 0;
    cookie_arry_stru   *tmp_cookie = HI_NULL;

    oam_warning_log0(0, OAM_SF_CFG, "{wal_check_cookie_timeout::time_out!}\r\n");
    for (loops = 0; loops < WAL_COOKIE_ARRAY_SIZE; loops++) {
        tmp_cookie = &cookie_array[loops];
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        if (hi_get_tick() > tmp_cookie->record_time + WAL_MGMT_TX_TIMEOUT_MSEC / HI_MILLISECOND_PER_TICK) {
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        if (oal_time_is_before(tmp_cookie->record_time + OAL_MSECS_TO_JIFFIES(WAL_MGMT_TX_TIMEOUT_MSEC))) {
#endif

            /* cookie array 中保存的cookie 值超时 */
            /* 清空cookie array 中超时的cookie */
            tmp_cookie->record_time = 0;
            tmp_cookie->ull_cookie = 0;
            /* 清除占用的cookie bitmap位 */
            oal_bit_clear_bit_one_byte(puc_cookie_bitmap, loops);
        }
    }
}

/* ****************************************************************************
 函 数 名  : wal_del_cookie_from_array
 功能描述  : 删除指定idx 的cookie
 输入参数  : [1]cookie_array
             [2]puc_cookie_bitmap
             [3]cookie_idx
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 wal_del_cookie_from_array(cookie_arry_stru *cookie_array, hi_u8 *puc_cookie_bitmap, hi_u8 cookie_idx)
{
    cookie_arry_stru *tmp_cookie = HI_NULL;

    /* 清除对应cookie bitmap 位 */
    oal_bit_clear_bit_one_byte(puc_cookie_bitmap, cookie_idx);

    /* 清空cookie array 中超时的cookie */
    tmp_cookie = &cookie_array[cookie_idx];
    tmp_cookie->ull_cookie = 0;
    tmp_cookie->record_time = 0;
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_add_cookie_to_array
 功能描述  : 添加cookie 到cookie array 中
 输入参数  : [1]cookie_array
             [2]puc_cookie_bitmap
             [3]puc_cookie_idx
             [4]pull_cookie
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 wal_add_cookie_to_array(cookie_arry_stru *cookie_array, hi_u8 *puc_cookie_bitmap, const hi_u64 *pull_cookie,
    hi_u8 *puc_cookie_idx)
{
    hi_u8 idx;
    cookie_arry_stru *tmp_cookie = HI_NULL;

    if (*puc_cookie_bitmap == 0xFF) {
        /* cookie array 满，返回错误 */
        oam_warning_log0(0, OAM_SF_CFG, "{wal_add_cookie_to_array::array full!}\r\n");
        return HI_FAIL;
    }

    /* 将cookie 添加到array 中 */
    idx = oal_bit_get_num_one_byte(*puc_cookie_bitmap);
    oal_bit_set_bit_one_byte(puc_cookie_bitmap, idx);

    tmp_cookie = &cookie_array[idx];
    tmp_cookie->ull_cookie = *pull_cookie;
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    tmp_cookie->record_time = hi_get_tick();
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    tmp_cookie->record_time = OAL_TIME_JIFFY;
#endif

    *puc_cookie_idx = idx;
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_check_cookie_from_array
 功能描述  : 从cookie array 中查找相应cookie index
 输入参数  : [1]puc_cookie_bitmap
             [2]cookie_idx
 输出参数  : 无
 返 回 值  : status hi_u32
**************************************************************************** */
static hi_u32 wal_check_cookie_from_array(const hi_u8 *puc_cookie_bitmap, hi_u8 cookie_idx)
{
    /* 从cookie bitmap中查找相应的cookie index，如果位图为0，表示已经被del */
    if (*puc_cookie_bitmap & (bit(cookie_idx))) {
        return HI_SUCCESS;
    }
    /* 找不到则返回FAIL */
    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_mgmt_do_tx
 功能描述  : WAL 层发送从wpa_supplicant  接收到的管理帧
 输入参数  : oal_net_device_stru    *pst_netdev        发送管理帧设备
             mac_mgmt_frame_stru    *pst_mgmt_tx_param 发送管理帧参数
 输出参数  : 无
 返 回 值  : static hi_u32 HI_SUCCESS 发送成功
                                   HI_FAIL 发送失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年8月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_mgmt_do_tx(oal_net_device_stru *netdev, const mac_mgmt_frame_stru *mgmt_tx_param,
    hi_bool en_offchan, hi_u32 wait)
{
    mac_vap_stru                    *mac_vap = HI_NULL;
    hmac_vap_stru                   *hmac_vap = HI_NULL;
    oal_mgmt_tx_stru                *mgmt_tx = HI_NULL;
    hi_u32                           wal_ret;
    hi_s32                           i_leftime;

    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_mgmt_do_tx::can't get mac vap from netdevice priv data.}\r\n");
        return HI_FAIL;
    }

    if (en_offchan == HI_TRUE) {
        if (mac_vap->vap_state != MAC_VAP_STATE_STA_LISTEN) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{wal_mgmt_do_tx::pst_mac_vap state[%d]not in listen!}\r\n",
                mac_vap->vap_state);
            return HI_INVALID;
        }
    }

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{wal_mgmt_do_tx::pst_hmac_vap ptr is null!}\r\n");
        return HI_FAIL;
    }

    mgmt_tx = &(hmac_vap->mgmt_tx);
    mgmt_tx->mgmt_tx_complete = HI_FALSE;
    mgmt_tx->mgmt_tx_status = HI_FALSE;

    /* 抛事件给驱动 */
    wal_ret = wal_cfg80211_start_req(netdev, mgmt_tx_param, sizeof(mac_mgmt_frame_stru), WLAN_CFGID_CFG80211_MGMT_TX,
        HI_FALSE);
    if (wal_ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_mgmt_do_tx::wal_send_cfg_event return err code:[%d]!}", wal_ret);
        return wal_ret;
    }

    i_leftime = hi_wait_event_timeout(mgmt_tx->wait_queue, HI_TRUE == mgmt_tx->mgmt_tx_complete,
        OAL_MSECS_TO_JIFFIES(wait)); // 使用非wifi目录定义宏函数,误报告警,lin_t e26告警屏蔽
    if (i_leftime == 0) {
        /* 定时器超时 */
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mgmt_do_tx::mgmt tx timeout!}\r\n");
        return HI_FAIL;
    } else if (i_leftime < 0) {
        /* 定时器内部错误 */
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mgmt_do_tx::mgmt tx timer error!}\r\n");
        return HI_FAIL;
    } else {
        /* 正常结束  */
        oam_info_log0(0, OAM_SF_ANY, "{wal_mgmt_do_tx::mgmt tx commpleted!}\r\n");
        return (hi_u32)((mgmt_tx->mgmt_tx_status == HI_FALSE) ? HI_FAIL : HI_SUCCESS);
    }
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifndef _PRE_HDF_LINUX
static hi_u32 wal_cfg80211_mgmt_tx_parameter_check(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev,
    struct cfg80211_mgmt_tx_params *pst_params, hi_u64 *pull_cookie)
{
    oal_net_device_stru         *netdev;
    oal_ieee80211_channel       *chan = HI_NULL;
    const hi_u8                 *puc_buf = HI_NULL;
    hi_unref_param(wiphy);

    if (pst_params == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::pst_params is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    chan = pst_params->chan;
    puc_buf = pst_params->buf;

    if ((wdev == HI_NULL) || (chan == HI_NULL) || (pull_cookie == HI_NULL) || (puc_buf == HI_NULL)) {
        oam_error_log3(0, OAM_SF_CFG,
            "{wal_cfg80211_mgmt_tx_parameter_check::wdev or chan or cookie or buf ptr is null, error %p, %p, %p!}\r\n",
            (uintptr_t)wdev, (uintptr_t)chan, (uintptr_t)pull_cookie);
        return HI_ERR_CODE_PTR_NULL;
    }

    netdev = wdev->netdev;
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::pst_netdev ptr is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::can't get mac vap fail!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::pst_hmac_vap ptr is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_mgmt_tx
 功能描述  : 发送管理帧
**************************************************************************** */
hi_s32 wal_cfg80211_mgmt_tx(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, oal_cfg80211_mgmt_tx_params_stru *params,
    hi_u64 *pull_cookie)
{
    mac_device_stru                 *mac_dev = (mac_device_stru *)mac_res_get_dev();
    mac_mgmt_frame_stru              mgmt_tx = {0};
    hi_u8                            cookie_idx;
    hi_u32                           ret;
    bool                             en_need_offchan = HI_FALSE;

    if (wal_cfg80211_mgmt_tx_parameter_check(wiphy, wdev, params, pull_cookie) != HI_SUCCESS) {
        return -HI_ERR_CODE_PTR_NULL;
    }

    oal_wireless_dev *pst_roc_wireless_dev = wdev;
    const hi_u8 *puc_buf = params->buf;
    hi_u32 len = params->len;
    hi_bool en_offchan = params->offchan;
    hi_u32 wait = params->wait;

    mac_vap_stru *mac_vap = oal_net_dev_priv(wdev->netdev);

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);

    mac_p2p_info_stru *p2p_info = &mac_dev->p2p_info;
    *pull_cookie = p2p_info->ull_send_action_id++; /* cookie值上层调用需要判断是否是这次的发送导致的callback */
    if (*pull_cookie == 0) {
        *pull_cookie = p2p_info->ull_send_action_id++;
    }
    const oal_ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *)puc_buf;
    if (oal_ieee80211_is_probe_resp(mgmt->frame_control)) {
        *pull_cookie = 0; /* set cookie default value */
        /* host should not send PROE RESPONSE,
           device will send immediately when receive probe request packet */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        HdfWifiEventMgmtTxStatus(wdev->netdev, puc_buf, len, HI_TRUE);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
        oal_cfg80211_mgmt_tx_status(wdev, *pull_cookie, puc_buf, params->len, HI_TRUE, GFP_KERNEL);
#endif
        return HI_SUCCESS;
    }

    /* 2.1 消息参数准备 */
    mgmt_tx.channel = oal_ieee80211_frequency_to_channel(params->chan->center_freq);
    if (wal_add_cookie_to_array(g_cookie_array, &g_cookie_array_bitmap, pull_cookie, &cookie_idx) != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::Failed to add cookies!}");
        return -HI_FAIL;
    }
    mgmt_tx.mgmt_frame_id = cookie_idx;
    mgmt_tx.us_len        = (hi_u16)len;
    mgmt_tx.puc_frame     = puc_buf;

    hmac_vap->mgmt_tx.mgmt_tx_complete = HI_FALSE;
    hmac_vap->mgmt_tx.mgmt_tx_status   = HI_FALSE;

    switch (mac_vap->vap_mode) {
        case WLAN_VAP_MODE_BSS_AP:
            oam_info_log3(mac_vap->vap_id, OAM_SF_ANY,
                "{wal_cfg80211_mgmt_tx::p2p mode[%d] (0=Legacy,1=GO,2=Dev,3=Gc), vap ch[%d], mgmt ch [%d]}",
                mac_vap->p2p_mode, mac_vap->channel.idx, mgmt_tx.channel);
            if ((mac_vap->channel.idx != mgmt_tx.channel) && is_p2p_go(mac_vap)) {
                if (mac_dev->p2p_info.pst_p2p_net_device == HI_NULL) {
                    oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::go mode but p2p dev is null}");
                    return -HI_FAIL;
                }
                pst_roc_wireless_dev = oal_netdevice_wdev(mac_dev->p2p_info.pst_p2p_net_device);
                en_need_offchan = HI_TRUE;
            }
            break;
        case WLAN_VAP_MODE_BSS_STA:
            if ((en_offchan == HI_TRUE) && (wiphy->flags & WIPHY_FLAG_OFFCHAN_TX)) {
                en_need_offchan = HI_TRUE;
            }
            if ((mac_vap->p2p_mode == WLAN_LEGACY_VAP_MODE) && (mac_vap->vap_state == MAC_VAP_STATE_UP)) {
                en_need_offchan = HI_FALSE;
            }
            break;
        default:
            break;
    }

    if ((en_need_offchan == HI_TRUE) && !chan) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::channel is null}\r\n");
        return -HI_FAIL;
    }

    if (wait == 0) {
        wait = WAL_MGMT_TX_TIMEOUT_MSEC;
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::wait is 0, set it to %d ms}", wait);
    }

    oam_info_log4(mac_vap->vap_id, OAM_SF_CFG,
        "{wal_cfg80211_mgmt_tx::offchannel[%d].channel[%d]vap state[%d],wait[%d]}\r\n",
        en_need_offchan, mgmt_tx.channel, mac_vap->vap_state, wait);
#ifdef _PRE_WLAN_FEATURE_P2P
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    if (en_need_offchan == HI_TRUE) {
        ret = wal_drv_remain_on_channel(wiphy, pst_roc_wireless_dev, chan, wait, pull_cookie,
            IEEE80211_ROC_TYPE_MGMT_TX);
        if (ret != HI_SUCCESS) {
            oam_warning_log4(mac_vap->vap_id, OAM_SF_CFG,
                "{wal_cfg80211_mgmt_tx::wal_drv_remain_on_channel[%d]!!!offchannel[%d].channel[%d],vap state[%d]}\r\n",
                ret, en_need_offchan, mgmt_tx.channel, mac_vap->vap_state);
            return -OAL_EBUSY;
        }
    }
#endif
#endif /* #ifdef _PRE_WLAN_FEATURE_P2P */

    hi_u32 start_time_stamp = OAL_TIME_JIFFY;
    hi_u8 retry = 0;
    /* 发送失败，则尝试重传 */
    do {
        ret = wal_mgmt_do_tx(netdev, &mgmt_tx, en_need_offchan, wait);
        retry++;
    } while ((ret != HI_SUCCESS) && (ret != HI_INVALID) && (retry < = WAL_MGMT_TX_RETRY_CNT) && 
        (oal_time_before(start_time_stamp, start_time_stamp + OAL_MSECS_TO_JIFFIES(wait))));

    if (ret != HI_SUCCESS) {
        /* 发送失败，处理超时帧的bitmap */
        wal_check_cookie_timeout(g_cookie_array, &g_cookie_array_bitmap);
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        HdfWifiEventMgmtTxStatus(wdev->netdev, puc_buf, len, HI_FALSE);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
        oal_cfg80211_mgmt_tx_status(wdev, *pull_cookie, puc_buf, params->len, HI_FALSE, GFP_KERNEL);
#endif
    } else {
        /* 正常结束  */
        *pull_cookie = g_cookie_array[hmac_vap->mgmt_tx.mgmt_frame_id].ull_cookie;
        wal_del_cookie_from_array(g_cookie_array, &g_cookie_array_bitmap, hmac_vap->mgmt_tx.mgmt_frame_id);
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        HdfWifiEventMgmtTxStatus(wdev->netdev, puc_buf, len, HI_FALSE);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
        oal_cfg80211_mgmt_tx_status(wdev, *pull_cookie, puc_buf, params->len, HI_FALSE, GFP_KERNEL);
#endif
    }

    return HI_SUCCESS;
}
#endif /* ifndef _PRE_HDF_LINUX */

#else
static hi_u32 wal_cfg80211_mgmt_tx_parameter_check(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev,
    oal_ieee80211_channel *chan, const hi_u8 *puc_buf, hi_u64 *pull_cookie)
{
    oal_net_device_stru *netdev;
    hi_unref_param(wiphy);
    if ((wdev == HI_NULL) || (chan == HI_NULL) || (pull_cookie == HI_NULL) || (puc_buf == HI_NULL)) {
        oam_error_log3(0, OAM_SF_CFG,
            "{wal_cfg80211_mgmt_tx_parameter_check::wdev or chan or cookie or buf ptr is null, error %p, %p, %p!}\r\n",
            (uintptr_t)wdev, (uintptr_t)chan, (uintptr_t)pull_cookie);
        return HI_ERR_CODE_PTR_NULL;
    }

    netdev = wdev->netdev;
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::pst_netdev ptr is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::can't get mac vap fail!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::pst_hmac_vap ptr is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_mgmt_tx
 功能描述  : 发送管理帧
**************************************************************************** */
hi_u32 wal_cfg80211_mgmt_tx(oal_wiphy_stru *wiphy, oal_wireless_dev *wdev, oal_ieee80211_channel *chan,
    const hi_u8 *puc_buf, hi_u32 len, hi_u64 *pull_cookie)
{
    mac_device_stru                 *mac_dev = (mac_device_stru *)mac_res_get_dev();
    mac_mgmt_frame_stru              mgmt_tx = {0};
    hi_u8                            cookie_idx;
    hi_u32                           ret;

    if (wal_cfg80211_mgmt_tx_parameter_check(wiphy, wdev, chan, puc_buf, pull_cookie) != HI_SUCCESS) {
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_vap_stru *mac_vap = oal_net_dev_priv(wdev->netdev);
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_p2p_info_stru *p2p_info = &mac_dev->p2p_info;
    *pull_cookie = p2p_info->ull_send_action_id++; /* cookie值上层调用需要判断是否是这次的发送导致的callback */
    if (*pull_cookie == 0) {
        *pull_cookie = p2p_info->ull_send_action_id++;
    }
    const oal_ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *)puc_buf;
    if (oal_ieee80211_is_probe_resp(mgmt->frame_control) == HI_TRUE) {
        *pull_cookie = 0; /* set cookie default value */
        /* host should not send PROE RESPONSE,
           device will send immediately when receive probe request packet */
        HdfWifiEventMgmtTxStatus(wdev->netdev, puc_buf, len, HI_TRUE);
        return HI_SUCCESS;
    }

    /* 2.1 消息参数准备 */
    mgmt_tx.channel = oal_ieee80211_frequency_to_channel(chan->center_freq);
    if (wal_add_cookie_to_array(g_cookie_array, &g_cookie_array_bitmap, pull_cookie, &cookie_idx) != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::Failed to add cookies!}\r\n");
        return HI_FAIL;
    }
    mgmt_tx.mgmt_frame_id = cookie_idx;
    mgmt_tx.us_len        = (hi_u16)len;
    mgmt_tx.puc_frame     = puc_buf;

    hmac_vap->mgmt_tx.mgmt_tx_complete = HI_FALSE;
    hmac_vap->mgmt_tx.mgmt_tx_status = HI_FALSE;

    hi_u32 start_time_stamp = hi_get_tick();

    hi_u32 end_time_stamp = start_time_stamp + 2 * WAL_MGMT_TX_TIMEOUT_MSEC / HI_MILLISECOND_PER_TICK; /* 2: 比例系数 */
    /* 发送失败，则尝试重传 */
    do {
        ret = wal_mgmt_do_tx(wdev->netdev, &mgmt_tx, 0, WAL_MGMT_TX_TIMEOUT_MSEC / HI_MILLISECOND_PER_TICK);
    } while ((ret != HI_SUCCESS) && (hi_get_tick() < end_time_stamp));

    if (ret != HI_SUCCESS) {
        /* 发送失败，处理超时帧的bitmap */
        wal_check_cookie_timeout(g_cookie_array, &g_cookie_array_bitmap);
    } else {
        /* 正常结束  */
        *pull_cookie = g_cookie_array[hmac_vap->mgmt_tx.mgmt_frame_id].ull_cookie;
        wal_del_cookie_from_array(g_cookie_array, &g_cookie_array_bitmap, hmac_vap->mgmt_tx.mgmt_frame_id);
    }
    HdfWifiEventMgmtTxStatus(wdev->netdev, puc_buf, len, ((ret != HI_SUCCESS) ? HI_FALSE : HI_TRUE));

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_cfg80211_mgmt_tx_status
 功能描述  : HMAC抛mgmt tx status到WAL, 唤醒wait queue
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_cfg80211_mgmt_tx_status(frw_event_mem_stru *event_mem)
{
    frw_event_stru                  *event = HI_NULL;
    dmac_crx_mgmt_tx_status_stru    *mgmt_tx_status_param = HI_NULL;
    hmac_vap_stru                   *hmac_vap = HI_NULL;
    oal_mgmt_tx_stru                *mgmt_tx = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx_status::event_mem is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_TX, "{wal_cfg80211_mgmt_tx_status::pst_hmac_vap null.vap_id[%d]}",
            event->event_hdr.vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    mgmt_tx_status_param = (dmac_crx_mgmt_tx_status_stru *)(event->auc_event_data);
    mgmt_tx = &(hmac_vap->mgmt_tx);
    mgmt_tx->mgmt_tx_complete = HI_TRUE;
    mgmt_tx->mgmt_tx_status   = mgmt_tx_status_param->tx_status;
    mgmt_tx->mgmt_frame_id    = mgmt_tx_status_param->mgmt_frame_id;

    /* 找不到相应的cookie值，说明已经超时被处理，不需要再唤醒 */
    if (HI_SUCCESS == wal_check_cookie_from_array(&g_cookie_array_bitmap, mgmt_tx->mgmt_frame_id)) {
        /* 让编译器优化时保证HI_WAIT_QUEUE_WAKE_UP在最后执行 */
        oal_smp_mb();
        hi_wait_queue_wake_up(&mgmt_tx->wait_queue);
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_start_req
 功能描述  : 向wal抛事件
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年1月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_start_req(oal_net_device_stru *netdev, const hi_void *ps_param, hi_u16 us_len,
    wlan_cfgid_enum_uint16 wid, hi_u8 need_rsp)
{
    wal_msg_write_stru              write_msg;
    wal_msg_stru                   *rsp_msg = HI_NULL;
    hi_u32                          ret;

    if (ps_param == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_start_req::param is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写 msg 消息头 */
    write_msg.wid = wid;
    write_msg.us_len = us_len;

    /* 填写 msg 消息体 */
    if (us_len > WAL_MSG_WRITE_MAX_LEN) {
        oam_error_log2(0, OAM_SF_SCAN, "{wal_cfg80211_start_req::us_len %d > WAL_MSG_WRITE_MAX_LEN %d err!}\r\n",
            us_len, WAL_MSG_WRITE_MAX_LEN);
        return HI_ERR_CODE_INVALID_CONFIG;
    }

    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), ps_param, us_len) != EOK) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_start_req::mem safe function err!}");
        return HI_FAIL;
    }
    /* **************************************************************************
           抛事件到wal层处理
    ************************************************************************** */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len, (hi_u8 *)&write_msg,
        need_rsp, need_rsp ? &rsp_msg : HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_cfg80211_start_req::wal_send_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }
    if (need_rsp && (rsp_msg != HI_NULL)) {
        /* 读取返回的错误码 */
        ret = wal_check_and_release_msg_resp(rsp_msg);
        if (ret != HI_SUCCESS) {
            oam_warning_log1(0, OAM_SF_SCAN, "{wal_cfg80211_start_req::wal_send_cfg_event return err code:[%u]}", ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_start_scan
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数

  2.日    期   : 2014年1月4日
    作    者   : HiSilicon
    修改内容   : 重构

**************************************************************************** */
hi_u32 wal_cfg80211_start_scan(oal_net_device_stru *netdev, const mac_cfg80211_scan_param_stru *scan_param)
{
    mac_cfg80211_scan_param_stru    *mac_cfg80211_scan_param = HI_NULL;
    hi_u32                           ret;

    if (scan_param == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_start_scan::scan failed, null ptr, pst_scan_param = null.}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 此处申请hmac层释放 */
    mac_cfg80211_scan_param =
        (mac_cfg80211_scan_param_stru *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, sizeof(mac_cfg80211_scan_param_stru));
    if (mac_cfg80211_scan_param == NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_start_scan::scan failed, alloc scan param memory failed!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (memcpy_s(mac_cfg80211_scan_param, sizeof(mac_cfg80211_scan_param_stru), scan_param,
        sizeof(mac_cfg80211_scan_param_stru)) != EOK) {
        oal_mem_free(mac_cfg80211_scan_param);
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_start_scan::mem safe function err!}");
        return HI_FAIL;
    }

    /* 1.传的是指针的指针, 2.sizeof指针  */
    ret = wal_cfg80211_start_req(netdev, &mac_cfg80211_scan_param, sizeof(uintptr_t), WLAN_CFGID_CFG80211_START_SCAN,
        HI_FALSE);
    if (ret != HI_SUCCESS) {
        /* 下发扫描失败，释放 */
        oal_mem_free(mac_cfg80211_scan_param);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_cfg80211_reset_bands
 功能描述  : 重新初始化wifi wiphy的bands
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年12月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_cfg80211_reset_bands(hi_void)
{
    hi_s32 i;

    /* 每次更新国家码,flags都会被修改,且上次修改的值不会被清除,相当于每次修改的国家码都会生效，
       因此更新国家需要清除flag标志 */
    for (i = 0; i < g_wifi_band_2ghz.n_channels; i++) {
        g_wifi_band_2ghz.channels[i].flags = 0;
    }
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
/* 不同操作系统函数指针结构体方式不同 */
static oal_cfg80211_ops_stru g_wal_cfg80211_ops =
{
    .scan                     = wal_cfg80211_scan,
    .connect                  = wal_cfg80211_connect,
    .disconnect               = wal_cfg80211_disconnect,
    .add_key                  = wal_cfg80211_add_key,
    .get_key                  = wal_cfg80211_get_key,
    .del_key                  = wal_cfg80211_remove_key,
    .set_default_key          = wal_cfg80211_set_default_key,
    .set_default_mgmt_key     = wal_cfg80211_set_default_mgmt_key,
    .set_wiphy_params         = wal_cfg80211_set_wiphy_params,
/* Hi1131 修改AP 配置接口 */
    .change_beacon            = wal_cfg80211_change_beacon,
    .start_ap                 = wal_cfg80211_start_ap,
    .stop_ap                  = wal_cfg80211_stop_ap,
    .change_bss               = wal_cfg80211_change_bss,
    .sched_scan_start         = wal_cfg80211_sched_scan_start,
    .sched_scan_stop          = wal_cfg80211_sched_scan_stop,
    .change_virtual_intf      = wal_cfg80211_change_virtual_intf,
    .add_station              = wal_cfg80211_add_station,
    .del_station              = wal_cfg80211_del_station,
    .change_station           = wal_cfg80211_change_station,
    .get_station              = wal_cfg80211_get_station,
    .dump_station             = wal_cfg80211_dump_station,
#ifdef _PRE_WLAN_FEATURE_P2P
    .remain_on_channel        = wal_cfg80211_remain_on_channel,
    .cancel_remain_on_channel = wal_cfg80211_cancel_remain_on_channel,
#endif
    .mgmt_tx                  = wal_cfg80211_mgmt_tx,
    .mgmt_frame_register      = wal_cfg80211_mgmt_frame_register,
    .set_bitrate_mask         = wal_cfg80211_set_bitrate_mask,
    .add_virtual_intf         = wal_cfg80211_add_virtual_intf,
    .del_virtual_intf         = wal_cfg80211_del_virtual_intf,
    .mgmt_tx_cancel_wait      = wal_cfg80211_mgmt_tx_cancel_wait,
    .start_p2p_device         = wal_cfg80211_start_p2p_device,
    .stop_p2p_device          = wal_cfg80211_stop_p2p_device,
    .set_power_mgmt           = wal_cfg80211_set_power_mgmt,
#if (LINUX_VERSION_CODE >= kernel_version(4,1,0))
    .abort_scan               = wal_cfg80211_abort_scan,
#endif /* (LINUX_VERSION_CODE >= kernel_version(4,1,0)) */
};
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX) */

wal_wifi_mode_enum_int wal_get_vap_mode(hi_void)
{
    return g_mode;
}

wal_wifi_bw_enum_int wal_get_bw_type(hi_void)
{
    return g_bw;
}

wal_phy_mode wal_get_protocol_type(hi_void)
{
    return g_proto;
}

/* ****************************************************************************
 功能描述  : wal_linux_cfg80211加载初始化
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 wal_cfg80211_init(hi_void)
{
    /* 单device下直接获取dev进行初始化 */
    mac_device_stru *mac_dev = mac_res_get_dev();
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    mac_dev->wiphy = oal_wiphy_new(sizeof(mac_wiphy_priv_stru));
#else
    mac_dev->wiphy = oal_wiphy_new(&g_wal_cfg80211_ops, sizeof(mac_wiphy_priv_stru));
#endif
    if (mac_dev->wiphy == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_init::oal_wiphy_new failed!}");
        return HI_FAIL;
    }

    /* 初始化wiphy 结构体内容 */
    oal_wiphy_stru *wiphy = mac_dev->wiphy;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
#ifdef _PRE_WLAN_FEATURE_P2P
    wiphy->iface_combinations   = g_sta_p2p_iface_combinations;
    wiphy->n_iface_combinations = hi_array_size(g_sta_p2p_iface_combinations);
    wiphy->mgmt_stypes          = g_wal_cfg80211_default_mgmt_stypes;
    wiphy->max_remain_on_channel_duration = 5000; /* 5000: 最大的时间间隔 */
    /* 使能驱动监听 */
    wiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL | WIPHY_FLAG_OFFCHAN_TX;
    wiphy->flags |= WIPHY_FLAG_HAVE_AP_SME;
    /* 1131注册支持pno调度扫描能力相关信息 */
    wiphy->max_sched_scan_ssids  = MAX_PNO_SSID_COUNT;
    wiphy->max_match_sets        = MAX_PNO_SSID_COUNT;
    wiphy->max_sched_scan_ie_len = WAL_MAX_SCAN_IE_LEN;
    wiphy->flags |= WIPHY_FLAG_SUPPORTS_SCHED_SCAN;
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    wiphy->interface_modes = bit(NL80211_IFTYPE_STATION) | bit(NL80211_IFTYPE_AP) | bit(NL80211_IFTYPE_P2P_CLIENT) |
        bit(NL80211_IFTYPE_P2P_GO);
#else
    wiphy->interface_modes = bit(NL80211_IFTYPE_STATION) | bit(NL80211_IFTYPE_AP);
#endif

#ifdef _PRE_WLAN_FEATURE_MESH
    wiphy->interface_modes |= bit(NL80211_IFTYPE_MESH_POINT);
#endif
    wiphy->max_scan_ssids = WLAN_SCAN_REQ_MAX_BSS;
    wiphy->max_scan_ie_len = WAL_MAX_SCAN_IE_LEN;
    wiphy->cipher_suites = g_wifi_cipher_suites;
    wiphy->n_cipher_suites = sizeof(g_wifi_cipher_suites) / sizeof(hi_u32);

    /* 不使能节能 */
    wiphy->flags &= ~WIPHY_FLAG_PS_ON_BY_DEFAULT;
    /* linux 3.14 版本升级，管制域重新修改 */
    wiphy->regulatory_flags |= REGULATORY_CUSTOM_REG;
    wiphy->signal_type       = CFG80211_SIGNAL_TYPE_MBM;
#endif
    wiphy->bands[IEEE80211_BAND_2GHZ] = &g_wifi_band_2ghz; /* 支持的频带信息 2.4G */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    oal_wiphy_apply_custom_regulatory(wiphy, wal_get_cfg_regdb());
    err_code = oal_wiphy_register(wiphy);
    if (err_code != 0) {
        oal_wiphy_free(mac_dev->wiphy);
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_init::oal_wiphy_register failed!}\r\n");
        return (hi_u32)err_code;
    }
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    oal_wiphy_apply_custom_regulatory();
    oal_wiphy_register(wiphy);
#endif

    /* P2P add_virtual_intf 传入wiphy 参数，在wiphy priv 指针保存wifi 驱动mac_devie_stru 结构指针 */
    mac_wiphy_priv_stru *wiphy_priv = (mac_wiphy_priv_stru *)(oal_wiphy_priv(wiphy));
    wiphy_priv->mac_device = mac_dev;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 卸载wihpy
 修改历史      :
  1.日    期   : 2013年9月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void wal_cfg80211_exit(hi_void)
{
    mac_device_stru *mac_dev = HI_NULL;

    /* 单device下直接获取dev进行去初始化 */
    mac_dev = mac_res_get_dev();
    /* 注销注册 wiphy device */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    oal_wiphy_unregister(mac_dev->wiphy);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    oal_wiphy_unregister();
#endif
    /* 卸载wiphy device */
    oal_wiphy_free(mac_dev->wiphy);
    return;
}

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
/* ****************************************************************************
 函 数 名  : wal_cfg80211_set_rekey_info
 功能描述  : 上层下发的rekey info，抛给wal层处理
 输入参数  : oal_net_device_stru      *pst_net_dev,
             mac_rekey_offload_stru   *pst_rekey_offload
 输出参数  : 无
 返 回 值  : HI_SUCCESS或其它错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年8月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_cfg80211_set_rekey_info(oal_net_device_stru *netdev, mac_rekey_offload_stru *rekey_offload)
{
    mac_rekey_offload_stru rekey_params;
    hi_u32 ret;

    /* 1 参数合法性检查 */
    if ((netdev == HI_NULL) || (rekey_offload == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_set_rekey_info::pst_net_dev = %p, pst_rekey_offload = %p!}",
            (uintptr_t)netdev, (uintptr_t)rekey_offload);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 2 消息参数准备 */
    if (memcpy_s(&rekey_params, sizeof(mac_rekey_offload_stru),
        rekey_offload, sizeof(mac_rekey_offload_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_set_rekey_info::mem safe function err!}");
        return HI_FAIL;
    }

    /* 抛事件给驱动 */
    ret = wal_cfg80211_start_req(netdev, &rekey_params, sizeof(mac_rekey_offload_stru), WLAN_CFGID_SET_REKEY, HI_TRUE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_rekey_info::wal_send_cfg_event return err code:[%d]!}", ret);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}
#endif /* _PRE_WLAN_FEATURE_REKEY_OFFLOAD */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
