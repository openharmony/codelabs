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
#include "hmac_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_FLOWCTL

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : wal_netdev_select_queue
 功能描述  : kernel给skb选择合适的tx subqueue;
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u16 wal_netdev_select_queue(oal_net_device_stru *netdev, oal_netbuf_stru *netbuf, hi_void *accel_priv,
    select_queue_fallback_t fallback)
{
    oal_ether_header_stru   *ether_header   = HI_NULL;
    mac_vap_stru            *mac_vap            = HI_NULL;
    hi_u8                assoc_id        = 0;
    hi_u8                tos             = 0;
    hi_u8                ac;
    hi_u16               us_subq;
    hi_u32               ret;

    /* 获取以太网头 */
    ether_header = (oal_ether_header_stru *)oal_netbuf_data(netbuf);

    mac_vap = (mac_vap_stru *)oal_net_dev_priv(netdev);
    /* 没有用户限速，则全部入index = 0 的subq, 并直接返回 */
    if (mac_vap->has_user_bw_limit == HI_FALSE) {
        return 0;
    }

    ret = mac_vap_find_user_by_macaddr(mac_vap, ether_header->auc_ether_dhost, ETHER_ADDR_LEN, &assoc_id);
    if (ret != HI_SUCCESS) {
        /* 没有找到用户的报文，均统一放入subq = 0的队列中 */
        oam_info_log0(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_find_user_by_macaddr::failed!}\r\n");
        return 0;
    }

    /* 获取skb的tos字段 */
    oal_netbuf_get_txtid(netbuf, &tos);

    /* 根据tos字段选择合适的队列 */
    ac = mac_tos_to_subq(tos);

    us_subq = (hi_u16)((assoc_id * WAL_NETDEV_SUBQUEUE_PER_USE) + ac);
    if (us_subq >= WAL_NETDEV_SUBQUEUE_MAX_NUM) {
        return 0;
    }
    return us_subq;
}

/* ****************************************************************************
 函 数 名  : wal_flowctl_backp_event_handler
 功能描述  : stop或者wake某个用户的某个subqueue
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_flowctl_backp_event_handler(frw_event_mem_stru *event_mem)
{
    frw_event_stru             *event               = (frw_event_stru *)event_mem->puc_data;
    mac_ioctl_queue_backp_stru *flowctl_backp_event = (mac_ioctl_queue_backp_stru *)(event->auc_event_data);
    hi_u8                       vap_id              = flowctl_backp_event->vap_id;

    /* 获取net_device */
    oal_net_device_stru *netdev = hmac_vap_get_net_device(vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(vap_id, OAM_SF_ANY, "{wal_flowctl_backp_event_handler::failed!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 如果对整个VAP stop或者wake */
    if (flowctl_backp_event->us_assoc_id == 0xFFFF) {
        if (flowctl_backp_event->is_stop == 1) {
            oal_net_tx_stop_all_queues(netdev);
        } else {
            oal_net_tx_wake_all_queues();
        }
        oam_info_log3(vap_id, OAM_SF_ANY,
            "{wal_flowctl_backp_event_handler::oal_net_tx_queues,stop_flag=%d,vap_id=%d,assoc_id=%d,tid=%d}",
            flowctl_backp_event->is_stop, flowctl_backp_event->vap_id, flowctl_backp_event->us_assoc_id,
            flowctl_backp_event->tidno);

        return HI_SUCCESS;
    }

    /* 如果对某个user stop或者wake */
    if (flowctl_backp_event->tidno == WLAN_TID_MAX_NUM) {
        for (hi_u8 ac = 0; ac <= MAC_LINUX_SUBQ_VO; ac++) {
            if (flowctl_backp_event->is_stop == 1) {
                oal_net_stop_subqueue(netdev);
            } else {
                oal_net_wake_subqueue(netdev);
            }
            oam_info_log3(vap_id, OAM_SF_ANY,
                "{wal_flowctl_backp_event_handler::oal_net_subqueue,stop=%d,vap_id=%d,assoc_id=%d,tid=%d}",
                flowctl_backp_event->is_stop, flowctl_backp_event->vap_id, flowctl_backp_event->us_assoc_id,
                flowctl_backp_event->tidno);
        }
        return HI_SUCCESS;
    }

    if (flowctl_backp_event->is_stop == 1) {
        oal_net_stop_subqueue(netdev);
    } else {
        oal_net_wake_subqueue(netdev);
    }
    oam_info_log3(vap_id, OAM_SF_ANY,
        "{wal_flowctl_backp_event_handler::oal_net_subqueue,stop_flag=%d,vap_id=%d,assoc_id=%d,tid=%d}",
        flowctl_backp_event->is_stop, flowctl_backp_event->vap_id, flowctl_backp_event->us_assoc_id,
        flowctl_backp_event->tidno);

    return HI_SUCCESS;
}

#endif /* endif of _PRE_WLAN_FEATURE_FLOWCTL */
#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
/* ****************************************************************************
 函 数 名  : wal_netdev_select_queue
 功能描述  : kernel给skb选择合适的tx subqueue;
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年3月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u16 wal_netdev_select_queue(oal_net_device_stru *netdev, oal_netbuf_stru *netbuf, hi_void *accel_priv,
    select_queue_fallback_t fallback)
{
    return oal_netbuf_select_queue(netbuf);
}

#endif /* endif of _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
