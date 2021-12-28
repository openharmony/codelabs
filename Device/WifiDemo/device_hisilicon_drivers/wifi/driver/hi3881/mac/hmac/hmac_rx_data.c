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
#include "mac_frame.h"
#include "mac_data.h"
#include "hmac_rx_data.h"
#include "dmac_ext_if.h"
#include "hmac_vap.h"
#include "hmac_ext_if.h"
#include "oam_ext_if.h"
#include "oal_ext_if.h"
#include "oal_net.h"
#include "hmac_frag.h"
#include "hmac_11i.h"
#include "mac_vap.h"
#include "hmac_blockack.h"
#include "hmac_mgmt_bss_comm.h"
#include "hcc_hmac_if.h"
#include "wal_cfg80211_apt.h"
#ifdef _PRE_WLAN_FEATURE_WAPI
#include "hmac_wapi.h"
#endif
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
#include "hmac_edca_opt.h"
#endif
#ifdef _PRE_HDF_LINUX
#include <linux/netdevice.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数实现
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_MESH
hi_void hmac_rx_process_data_mesh_tcp_ack_opt(hmac_vap_stru *hmac_vap, const oal_netbuf_head_stru *netbuf_header);
#endif

/* ****************************************************************************
 功能描述  : 将MSDU转化为以太网格式的帧
 输入参数  : pst_netbuf : 指向含有MSDU的netbuf的指针
             puc_da     : 目的地址
             puc_sa     : 源地址
 修改历史      :
  1.日    期   : 2013年12月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_rx_frame_80211_to_eth(oal_netbuf_stru *netbuf, const hi_u8 *da_mac_addr, hi_u8 da_addr_len,
    const hi_u8 *sa_mac_addr, hi_u8 sa_addr_len)
{
    mac_ether_header_stru *ether_hdr = HI_NULL;
    mac_llc_snap_stru *snap = HI_NULL;
    hi_u16 us_ether_type;

    snap = (mac_llc_snap_stru *)oal_netbuf_data(netbuf);
    us_ether_type = snap->us_ether_type;

    /* 将payload向前扩充6个字节，加上后面8个字节的snap头空间，构成以太网头的14字节空间 */
    oal_netbuf_push(netbuf, HMAC_RX_DATA_ETHER_OFFSET_LENGTH);
    ether_hdr = (mac_ether_header_stru *)oal_netbuf_data(netbuf);

    ether_hdr->us_ether_type = us_ether_type;
    if (memcpy_s(ether_hdr->auc_ether_shost, ETHER_ADDR_LEN, sa_mac_addr, sa_addr_len) != EOK) {
        return;
    }
    if (memcpy_s(ether_hdr->auc_ether_dhost, ETHER_ADDR_LEN, da_mac_addr, da_addr_len) != EOK) {
        return;
    }
}

/* ****************************************************************************
 功能描述  : 释放指定个数的netbuf
 输入参数  : (1)期望删除的netbuf的起始指针
             (2)需要删除的netbuf的个数
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2012年12月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_rx_free_netbuf(oal_netbuf_stru *netbuf, hi_u16 us_nums)
{
    oal_netbuf_stru *netbuf_temp = HI_NULL;
    hi_u16 us_netbuf_num;

    if (oal_unlikely(netbuf == HI_NULL)) {
        oam_error_log0(0, OAM_SF_RX, "{hmac_rx_free_netbuf::pst_netbuf null.}\r\n");
        return;
    }

    for (us_netbuf_num = us_nums; us_netbuf_num > 0; us_netbuf_num--) {
        netbuf_temp = oal_netbuf_next(netbuf);

        /* 减少netbuf对应的user引用计数 */
        oal_netbuf_free(netbuf);

        netbuf = netbuf_temp;
        if (netbuf == HI_NULL) {
            if (oal_unlikely(us_netbuf_num != 1)) {
                oam_error_log2(0, OAM_SF_RX,
                    "{hmac_rx_free_netbuf::pst_netbuf list broken, us_netbuf_num[%d]us_nums[%d].}", us_netbuf_num,
                    us_nums);
                return;
            }

            break;
        }
    }
}

/* ****************************************************************************
 功能描述  : free netbuff list
 修改历史      :
  1.日    期   : 2015年1月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_rx_free_netbuf_list(oal_netbuf_head_stru *netbuf_hdr, hi_u16 num_buf)
{
    oal_netbuf_stru *netbuf = HI_NULL;
    hi_u16 us_idx;

    if (oal_unlikely(netbuf_hdr == HI_NULL)) {
        oam_info_log0(0, OAM_SF_RX, "{hmac_rx_free_netbuf_list::pst_netbuf null.}");
        return;
    }

    for (us_idx = num_buf; us_idx > 0; us_idx--) {
        netbuf = oal_netbuf_delist(netbuf_hdr);
        if (netbuf != HI_NULL) {
            oal_netbuf_free(netbuf);
        }
    }
}

/* ****************************************************************************
 功能描述  : 将数据帧发送到WLAN侧的接口函数，将一个netbuf链抛给发送流程，每个
             netbuf的内容都是一个以太网格式的MSDU
 输入参数  : (1)指向事件头的指针
             (2)指向需要发送的netbuf的第一个元素的指针
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2012年11月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2016年06月20日
    作    者   : HiSilicon
**************************************************************************** */
static hi_u32 hmac_rx_transmit_to_wlan(frw_event_hdr_stru *event_hdr, oal_netbuf_head_stru *netbuf_head)
{
    oal_netbuf_stru *netbuf = HI_NULL; /* 从netbuf链上取下来的指向netbuf的指针 */
    hi_u32 netbuf_num;
    hi_u32 ret;
    oal_netbuf_stru *buf_tmp = HI_NULL; /* 暂存netbuf指针，用于while循环 */
    hmac_tx_ctl_stru *tx_ctl = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;

    if (oal_unlikely((event_hdr == HI_NULL) || (netbuf_head == HI_NULL))) {
        oam_error_log2(0, OAM_SF_RX, "{hmac_rx_transmit_to_wlan::param null, %p %p.}", (uintptr_t)event_hdr,
            (uintptr_t)netbuf_head);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取链头的net buffer */
    netbuf = oal_netbuf_peek(netbuf_head);

    /* 获取mac vap 结构 */
    ret = hmac_tx_get_mac_vap(event_hdr->vap_id, &mac_vap);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        netbuf_num = oal_netbuf_list_num(netbuf_head);
        hmac_rx_free_netbuf(netbuf, (hi_u16)netbuf_num);
        oam_warning_log3(event_hdr->vap_id, OAM_SF_RX,
            "{hmac_rx_transmit_to_wlan::find vap [%d] failed[%d], free [%d] netbuffer.}", event_hdr->vap_id, ret,
            netbuf_num);
        return ret;
    }

    /* 循环处理每一个netbuf，按照以太网帧的方式处理 */
    while (netbuf != HI_NULL) {
        buf_tmp = oal_netbuf_next(netbuf);
        set_oal_netbuf_next(netbuf, HI_NULL);
        set_oal_netbuf_prev(netbuf, HI_NULL);

        /* 转WLAN的报文长度必须大于ETHER_HDR_LEN,否则异常丢弃 */
        if (oal_netbuf_len(netbuf) < ETHER_HDR_LEN) {
            hmac_free_netbuf_list(netbuf);
            netbuf = buf_tmp;
            continue;
        }

        tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
        if (memset_s(tx_ctl, sizeof(hmac_tx_ctl_stru), 0, sizeof(hmac_tx_ctl_stru)) != EOK) {
            hmac_free_netbuf_list(netbuf);
            netbuf = buf_tmp;
            continue;
        }

        tx_ctl->event_type = FRW_EVENT_TYPE_WLAN_DTX;
        tx_ctl->event_sub_type = DMAC_TX_WLAN_DTX;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        /* set the queue map id when wlan to wlan */
        oal_skb_set_queue_mapping(netbuf, WLAN_NORMAL_QUEUE);
#endif

        ret = hmac_tx_lan_to_wlan(mac_vap, netbuf);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            hmac_free_netbuf_list(netbuf);
        }
        netbuf = buf_tmp;
    }

    return HI_SUCCESS;
}

hi_void hmac_rx_set_msdu_state(oal_netbuf_stru *netbuf, hmac_msdu_proc_state_stru *msdu_state)
{
    if (msdu_state->procd_msdu_in_netbuf == 0) {
        msdu_state->curr_netbuf = netbuf;

        /* AMSDU时，首个netbuf的中包含802.11头，对应的payload需要偏移 */
        hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(msdu_state->curr_netbuf);

        msdu_state->puc_curr_netbuf_data = (hi_u8 *)(rx_ctrl->pul_mac_hdr_start_addr) + rx_ctrl->mac_header_len;
        msdu_state->msdu_nums_in_netbuf = rx_ctrl->msdu_in_buffer;
        msdu_state->us_submsdu_offset = 0;
    }
}

hi_u32 hmac_rx_msdu_proc(const hmac_vap_stru *hmac_vap, oal_netbuf_head_stru *netbuf_header, oal_netbuf_stru *netbuf,
    mac_ieee80211_frame_stru *frame_hdr, const hmac_rx_ctl_stru *rx_ctrl)
{
    hi_u8 sa_mac_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 da_mac_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 *source_mac_addr = HI_NULL;
    hi_u8 *dest_mac_addr   = HI_NULL;

    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(rx_ctrl->us_ta_user_idx);
    if (oal_unlikely(hmac_user == HI_NULL)) {
        return HI_ERR_CODE_PTR_NULL;
    }

    netbuf = hmac_defrag_process(hmac_user, netbuf, rx_ctrl->mac_header_len);
    if (netbuf == HI_NULL) {
        return HI_SUCCESS;
    }

    rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    frame_hdr = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;

    /* 从MAC头中获取源地址和目的地址 */
    mac_rx_get_sa(frame_hdr, &source_mac_addr);
    mac_rx_get_da(frame_hdr, &dest_mac_addr);
    if ((memcpy_s(sa_mac_addr, WLAN_MAC_ADDR_LEN, source_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(da_mac_addr, WLAN_MAC_ADDR_LEN, dest_mac_addr, WLAN_MAC_ADDR_LEN) != EOK)) {
        return HI_FAIL;
    }

    /* 将netbuf的data指针指向mac frame的payload处，也就是指向了8字节的snap头 */
    oal_netbuf_pull(netbuf, rx_ctrl->mac_header_len);

    /* 将MSDU转化为以太网格式的帧 */
    hmac_rx_frame_80211_to_eth(netbuf, da_mac_addr, WLAN_MAC_ADDR_LEN, sa_mac_addr, WLAN_MAC_ADDR_LEN);

    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化 */
    memset_s(oal_netbuf_cb(netbuf), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
    mac_ether_header_stru *ether = (mac_ether_header_stru *)oal_netbuf_data(netbuf);
    if (ether == HI_NULL) {
        return HI_FAIL;
    }
    if (hmac_11i_ether_type_filter(hmac_vap, ether->auc_ether_shost, ether->us_ether_type) != HI_SUCCESS) {
        return HI_FAIL;
    } else {
        /* 将MSDU加入到netbuf链的最后 */
        oal_netbuf_add_to_list_tail(netbuf, netbuf_header);
    }
#else
    /* 将MSDU加入到netbuf链的最后 */
    oal_netbuf_add_to_list_tail(netbuf, netbuf_header);
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 解析MPDU，如果是非AMSDU，则将MSDU还原为以太网格式的帧，并加入到
             netbuf链的最后，如果该MPDU是AMSDU，则解析出每一个MSDU，并且每一
             个MSDU占用一个netbuf
 输入参数  : pst_netbuf_header: 要交给发送流程的netbuf链表头
             pst_netbuf       : 当前要处理的MPDU的第一个netbuf
             pst_frame_hdr    : 当前要处理的MPDU的MAC头
 返 回 值  : 成功或者错误码
 修改历史      :
  1.日    期   : 2013年12月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_rx_prepare_msdu_list_to_wlan(const hmac_vap_stru *hmac_vap, oal_netbuf_head_stru *netbuf_header,
    oal_netbuf_stru *netbuf, mac_ieee80211_frame_stru *frame_hdr)
{
    hi_u32 ret;
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf); /* 获取该MPDU的控制信息 */

    /* 情况一:不是AMSDU聚合，则该MPDU对应一个MSDU，同时对应一个NETBUF,将MSDU还原
       成以太网格式帧以后直接加入到netbuf链表最后
     */
    if (rx_ctrl->amsdu_enable == HI_FALSE) {
        ret = hmac_rx_msdu_proc(hmac_vap, netbuf_header, netbuf, frame_hdr, rx_ctrl);
        return ret;
    } else { /* 情况二:AMSDU聚合 */
        return HI_FAIL;
    }
}

#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
static hi_void hmac_pkt_mem_opt_stat_reset(hmac_device_stru *hmac_dev, hi_u8 dscr_opt_state)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hmac_rx_dscr_opt_stru *dscr_opt = &hmac_dev->rx_dscr_opt;

    dscr_opt->dscr_opt_state = dscr_opt_state;
    dscr_opt->rx_pkt_num = 0;
    /* **************************************************************************
        抛事件到dmac模块,将统计信息报给dmac
    ************************************************************************** */
    event_mem = frw_event_alloc(0);
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_rx_dscr_opt_timeout_fn::event_mem null.}");
        return;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_DSCR_OPT, 0,
        FRW_EVENT_PIPELINE_STAGE_1, 0);

    /* 拷贝参数 */
    event->auc_event_data[0] = dscr_opt->dscr_opt_state;

    /* 分发事件 */
    hcc_hmac_tx_control_event(event_mem, 0);
    frw_event_free(event_mem);
}

/* ****************************************************************************
 功能描述  : 配置hmac_pkt_mem_opt_cfg参数
 输入参数  : ul_cfg_type:0 enable使能开关
                         1 opt_limit
                         2 reset_limit
 修改历史      :
  1.日    期   : 2015年10月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_pkt_mem_opt_cfg(hi_u32 cfg_tpye, hi_u32 cfg_value)
{
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    hmac_rx_dscr_opt_stru *dscr_opt = HI_NULL;

    if (cfg_tpye > 2) { /* 大于2 无效类型 */
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_rx_dscr_opt_cfg::invalid cfg tpye.}");
        return;
    }

    oam_warning_log2(0, OAM_SF_ANY, "{hmac_rx_dscr_opt_cfg::cfg type[%d], cfg value[%d].}", cfg_tpye, cfg_value);
    dscr_opt = &hmac_dev->rx_dscr_opt;
    if (cfg_tpye == 0) {
        dscr_opt->dscr_opt_enable = (hi_u8)cfg_value;
        if (dscr_opt->dscr_opt_enable == HI_FALSE && dscr_opt->dscr_opt_state == HI_TRUE) {
            hmac_pkt_mem_opt_stat_reset(hmac_dev, HI_FALSE);
        }
    } else if (cfg_tpye == 1) {
        dscr_opt->rx_pkt_opt_limit = cfg_value;
    } else if (cfg_tpye == 2) { /* 等于2 reset */
        dscr_opt->rx_pkt_reset_limit = cfg_value;
    }
}

hi_u32 hmac_pkt_mem_opt_timeout_fn(hi_void *arg)
{
    hmac_device_stru *hmac_dev = HI_NULL;
    hmac_rx_dscr_opt_stru *dscr_opt = HI_NULL;

    if (oal_unlikely(arg == HI_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_rx_dscr_opt_timeout_fn::p_arg is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_dev = (hmac_device_stru *)arg;
    dscr_opt = &hmac_dev->rx_dscr_opt;

    if (dscr_opt->dscr_opt_enable != HI_TRUE) {
        return HI_SUCCESS;
    }

    oam_info_log2(0, OAM_SF_ANY, "{hmac_rx_dscr_opt_timeout_fn::state[%d], pkt_num[%d]}", dscr_opt->dscr_opt_state,
        dscr_opt->rx_pkt_num);

    /* rx_dscr未调整状态时, 检测到RX业务,调整描述符 */
    if (dscr_opt->dscr_opt_state == HI_FALSE && dscr_opt->rx_pkt_num > dscr_opt->rx_pkt_opt_limit) {
        hmac_pkt_mem_opt_stat_reset(hmac_dev, HI_TRUE);
    } else if (dscr_opt->dscr_opt_state == HI_TRUE && dscr_opt->rx_pkt_num < dscr_opt->rx_pkt_reset_limit) {
        /* rx_dscr已调整状态时, 未检测到RX业务, 调整回描述符,保证TX性能 */
        hmac_pkt_mem_opt_stat_reset(hmac_dev, HI_FALSE);
    } else {
        dscr_opt->rx_pkt_num = 0;
    }

    return HI_SUCCESS;
}

hi_void hmac_pkt_mem_opt_init(hmac_device_stru *hmac_dev)
{
    hmac_dev->rx_dscr_opt.dscr_opt_state = HI_FALSE;
    hmac_dev->rx_dscr_opt.rx_pkt_num = 0;
    hmac_dev->rx_dscr_opt.rx_pkt_opt_limit = WLAN_PKT_MEM_PKT_OPT_LIMIT;
    hmac_dev->rx_dscr_opt.rx_pkt_reset_limit = WLAN_PKT_MEM_PKT_RESET_LIMIT;
    /* 功能无效，暂时关闭 */
    hmac_dev->rx_dscr_opt.dscr_opt_enable = HI_FALSE;

    frw_timer_create_timer(&(hmac_dev->rx_dscr_opt.rx_dscr_opt_timer), hmac_pkt_mem_opt_timeout_fn,
        WLAN_PKT_MEM_OPT_TIME_MS, hmac_dev, HI_TRUE);
}

hi_void hmac_pkt_mem_opt_exit(hmac_device_stru *hmac_dev)
{
    if (hmac_dev->rx_dscr_opt.rx_dscr_opt_timer.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(hmac_dev->rx_dscr_opt.rx_dscr_opt_timer));
    }
}

static hi_void hmac_pkt_mem_opt_rx_pkts_stat(const oal_ip_header_stru *ip)
{
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    /* 过滤IP_LEN 小于 WLAN_SHORT_NETBUF_SIZE的报文 */
    if (oal_net2host_short(ip->us_tot_len) < WLAN_SHORT_NETBUF_SIZE) {
        return;
    }

    if ((ip->protocol == MAC_UDP_PROTOCAL) || (ip->protocol == MAC_TCP_PROTOCAL)) {
        hmac_dev->rx_dscr_opt.rx_pkt_num++;
    }
}
#endif

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
static hi_u32 hmac_rx_transmit_edca_opt_ap(const hmac_vap_stru *hmac_vap, mac_ether_header_stru *ether_hdr)
{
    mac_vap_stru         *mac_vap = hmac_vap->base_vap;
    hmac_user_stru       *hmac_user = HI_NULL;
    mac_ip_header_stru   *ip = HI_NULL;
    hi_u8                assoc_id = 0xff;

    if (((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
        ) && (hmac_vap->edca_opt_flag_ap == HI_TRUE)) {
        /* Mesh IPv6可能进行报头压缩，暂不纳入此处处理 */
        if (oal_host2net_short(ETHER_TYPE_IP) == ether_hdr->us_ether_type) {
            if (mac_vap_find_user_by_macaddr(mac_vap, ether_hdr->auc_ether_shost, ETHER_ADDR_LEN, &assoc_id) !=
                HI_SUCCESS) {
                oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_M2U,
                    "{hmac_rx_transmit_edca_opt_ap::find_user_by_macaddr[XX:XX:XX:%02x:%02x:%02x]failed}",
                    (hi_u32)(ether_hdr->auc_ether_shost[3]),  /* 3 元素索引 */
                    (hi_u32)(ether_hdr->auc_ether_shost[4]),  /* 4 元素索引 */
                    (hi_u32)(ether_hdr->auc_ether_shost[5])); /* 5 元素索引 */
                return HI_FAIL;
            }
            hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(assoc_id);
            if (hmac_user == HI_NULL) {
                oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_RX,
                    "{hmac_rx_transmit_edca_opt_ap::hmac_user_get_user_stru fail. assoc_id: %u}", assoc_id);
                return HI_FAIL;
            }

            ip = (mac_ip_header_stru *)(ether_hdr + 1);

            /* mips优化:解决开启业务统计性能差10M问题 */
            if (((ip->protocol == MAC_UDP_PROTOCAL) && (hmac_user->txrx_data_stat[WLAN_WME_AC_BE][WLAN_RX_UDP_DATA] <
                (HMAC_EDCA_OPT_PKT_NUM + 10))) || /* 10 用于计算 */
                ((ip->protocol == MAC_TCP_PROTOCAL) && (hmac_user->txrx_data_stat[WLAN_WME_AC_BE][WLAN_RX_TCP_DATA] <
                (HMAC_EDCA_OPT_PKT_NUM + 10)))) { /* 10 用于计算 */
                hmac_edca_opt_rx_pkts_stat(hmac_user, WLAN_TIDNO_BEST_EFFORT, ip);
            }
        }
    }

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 将MSDU转发到LAN的接口，包括地址转换等信息的设置
             说明:本函数接收到的netbuf数据域是从snap头开始
 输入参数  : (1)指向vap的指针
             (2)指向需要发送的msdu的指针
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2012年11月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_rx_transmit_msdu_to_lan(const hmac_vap_stru *hmac_vap, hmac_msdu_stru *msdu)
{
    /* 获取netbuf，该netbuf的data指针已经指向payload处 */
    oal_netbuf_stru *netbuf = msdu->netbuf;

    set_oal_netbuf_prev(netbuf, HI_NULL);
    set_oal_netbuf_next(netbuf, HI_NULL);

    hmac_rx_frame_80211_to_eth(netbuf, msdu->auc_da, WLAN_MAC_ADDR_LEN, msdu->auc_sa, WLAN_MAC_ADDR_LEN);

    mac_ether_header_stru *ether_hdr = (mac_ether_header_stru *)oal_netbuf_data(netbuf);
    if (oal_unlikely(ether_hdr == HI_NULL)) {
        oal_netbuf_free(netbuf);
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_rx_transmit_msdu_to_lan::pst_ether_hdr null.}");
        return;
    }
#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
    hi_u8 *mac_addr = msdu->auc_ta;

    if (HI_SUCCESS != hmac_11i_ether_type_filter(hmac_vap, mac_addr, ether_hdr->us_ether_type)) {
        /* 接收安全数据过滤 */
        oal_netbuf_free(netbuf);
        return;
    }
#endif
    /* 获取net device hmac创建的时候，需要记录netdevice指针 */
    oal_net_device_stru *netdev = hmac_vap->net_device;

    /* 对protocol模式赋值 */
    oal_eth_type_trans(netbuf, netdev);

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    if (hmac_rx_transmit_edca_opt_ap(hmac_vap, ether_hdr) != HI_SUCCESS) {
        oal_netbuf_free(netbuf);
        return;
    }
#endif

#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
    hmac_pkt_mem_opt_rx_pkts_stat((oal_ip_header_stru *)(ether_hdr + 1));
#endif

    /* 将skb转发给桥 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    netbuf->dev = netdev;

    /* 将skb的data指针指向以太网的帧头 */
    /* 由于前面pull了14字节，这个地方要push回去 */
    oal_netbuf_push(netbuf, ETHER_HDR_LEN);
#endif

#ifdef _PRE_HDF_LINUX
    netbuf->dev = (struct net_device *)netdev;
#endif

    if (HI_TRUE == hmac_get_rxthread_enable()) {
        hmac_rxdata_netbuf_enqueue(netbuf);

        hmac_rxdata_sched();
    } else {
        oal_netif_rx_ni(netbuf);
    }

    /* 置位net_dev->jiffies变量 */
    oal_netdevice_last_rx(netdev) = OAL_TIME_JIFFY;
}

hi_void hmac_rx_msdu_frame_classify(const hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf,
    mac_ieee80211_frame_stru *frame_hdr, hmac_msdu_stru *msdu, hmac_user_stru *hmac_user)
{
    hi_u8 *mac_addr = HI_NULL;
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);

#ifdef _PRE_WLAN_FEATURE_WAPI
    hi_bool pairwise = !ether_is_multicast(frame_hdr->auc_address1);
    hmac_wapi_stru *wapi = hmac_user_get_wapi_ptr(hmac_vap->base_vap, pairwise, hmac_user->base_user->us_assoc_id);

    if (wapi == HI_NULL) {
        oam_warning_log0(0, OAM_SF_WPA, "{hmac_rx_lan_frame_classify:: get pst_wapi Err!.}");
        return;
    }

    if ((wapi_is_port_valid(wapi) == HI_TRUE) && (wapi->wapi_netbuff_rxhandle != HI_NULL)) {
        netbuf = wapi->wapi_netbuff_rxhandle(wapi, netbuf);
        if (netbuf == HI_NULL) {
            oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_rx_lan_frame_classify:WapiDecrypt Err}");
            return;
        }

        /* 重新获取该MPDU的控制信息 */
        rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    }
#endif

    netbuf = hmac_defrag_process(hmac_user, netbuf, rx_ctrl->mac_header_len);
    if (netbuf == HI_NULL) {
        return;
    }

    /* 打印出关键帧(dhcp)信息 */
    hi_u8 datatype = mac_get_data_type_from_80211(netbuf, rx_ctrl->mac_header_len);
    if (datatype <= MAC_DATA_VIP) {
        oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_RX,
            "{hmac_rx_lan_frame_classify:user=%d,type=%u,len=%u}[0~3dhcp 4arp_req 5arp_rsp 6eapol]",
            rx_ctrl->us_ta_user_idx, datatype, rx_ctrl->us_frame_len);
    }

    /* 对当前的msdu进行赋值 */
    msdu->netbuf = netbuf;

    /* 将netbuf的data指针指向mac frame的payload处 */
    oal_netbuf_pull(netbuf, rx_ctrl->mac_header_len);

    /* 获取源地址和目的地址 */
    mac_rx_get_sa(frame_hdr, &mac_addr);
    if (memcpy_s(msdu->auc_sa, WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        return;
    }

    mac_rx_get_da(frame_hdr, &mac_addr);
    if (memcpy_s(msdu->auc_da, WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        return;
    }

    /* 将MSDU转发到LAN */
    hmac_rx_transmit_msdu_to_lan(hmac_vap, msdu);
}

/* ****************************************************************************
 功能描述  : HMAC接收模块，WLAN到LAN的转发接口
 输入参数  : (1)对应MPDU的第一个netbuf的指针
             (2)对应的MPDU占用的netbuf的数目
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2012年12月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_rx_lan_frame_classify(const hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf,
    mac_ieee80211_frame_stru *frame_hdr)
{
    hmac_msdu_stru msdu = { 0 };                                    /* 保存解析出来的每一个MSDU */
    hi_u8 *mac_addr = HI_NULL;

    if (oal_unlikely(frame_hdr == HI_NULL)) {
        oam_error_log0(0, OAM_SF_RX, "{hmac_rx_lan_frame_classify::params null.}");
        return HI_FAIL;
    }

    /* 获取该MPDU的控制信息 */
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);

    mac_get_transmit_addr(frame_hdr, &mac_addr);
    if (memcpy_s(msdu.auc_ta, WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }

    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(rx_ctrl->us_ta_user_idx);
    if (oal_unlikely((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL))) {
        oam_error_log3(hmac_vap->base_vap->vap_id, OAM_SF_RX,
            "{hmac_rx_lan_frame_classify::hmac_user null,user_idx=%d,net_buf ptr addr=%p,cb ptr addr=%p}",
            rx_ctrl->us_ta_user_idx, (uintptr_t)netbuf, (uintptr_t)rx_ctrl);

        /* 打印此net buf相关信息 */
        oam_error_log4(hmac_vap->base_vap->vap_id, OAM_SF_RX,
            "{hmac_rx_lan_frame_classify:vap id=%d,mac_hdr_len=%d,frame_len=%d,mac_hdr_start_addr=%p}", rx_ctrl->vap_id,
            rx_ctrl->mac_header_len, rx_ctrl->us_frame_len, (uintptr_t)rx_ctrl->pul_mac_hdr_start_addr);

        return HI_FAIL;
    }

    hmac_ba_update_rx_bitmap(hmac_user, frame_hdr);

    /* 情况一:不是AMSDU聚合，则该MPDU对应一个MSDU，同时对应一个NETBUF */
    if (rx_ctrl->amsdu_enable == HI_FALSE) {
        hmac_rx_msdu_frame_classify(hmac_vap, netbuf, frame_hdr, &msdu, hmac_user);
    } else { /* 情况二:AMSDU聚合 */
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : copy netbuff
 修改历史      :
  1.日    期   : 2015年1月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_rx_copy_netbuff(oal_netbuf_stru **dest_netbuf, const oal_netbuf_stru *src_netbuf, hi_u8 vap_id,
    mac_ieee80211_frame_stru **ppul_mac_hdr_start_addr)
{
    hmac_rx_ctl_stru *rx_ctrl = HI_NULL;

    hi_unref_param(vap_id);
    *dest_netbuf = oal_netbuf_alloc(WLAN_LARGE_NETBUF_SIZE, 0, 4); /* align 4 */
    if (oal_unlikely(*dest_netbuf == HI_NULL)) {
        oam_warning_log0(vap_id, OAM_SF_RX, "{hmac_rx_copy_netbuff::pst_netbuf_copy null.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    /* 信息复制 */
    if (memcpy_s(oal_netbuf_cb(*dest_netbuf), oal_netbuf_cb_size(), oal_netbuf_cb(src_netbuf),
        sizeof(hmac_rx_ctl_stru)) != EOK) {
        oal_netbuf_free(*dest_netbuf);
        oam_error_log0(0, OAM_SF_CFG, "hmac_rx_copy_netbuff:: pst_src_netbuf memcpy_s fail.");
        return HI_FAIL;
    }
    if (memcpy_s(oal_netbuf_data(*dest_netbuf), oal_netbuf_tailroom(*dest_netbuf), oal_netbuf_data(src_netbuf),
        oal_netbuf_len(src_netbuf)) != EOK) {
        oal_netbuf_free(*dest_netbuf);
        oam_error_log0(0, OAM_SF_CFG, "hmac_rx_copy_netbuff:: pst_src_netbuf memcpy_s fail.");
        return HI_FAIL;
    }
    /* 设置netbuf长度、TAIL指针 */
    oal_netbuf_put(*dest_netbuf, oal_netbuf_len(src_netbuf));
    /* 调整MAC帧头的指针copy后，对应的mac header的头已经发生变化) */
    rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(*dest_netbuf);
    rx_ctrl->pul_mac_hdr_start_addr = (hi_u32 *)oal_netbuf_data(*dest_netbuf);
    *ppul_mac_hdr_start_addr = (mac_ieee80211_frame_stru *)oal_netbuf_data(*dest_netbuf);

    return HI_SUCCESS;
}

hi_void hmac_rx_netbuf_add_to_list_tail(oal_netbuf_head_stru *netbuf_header, hmac_rx_ctl_stru *rx_ctrl, hi_u32 ret,
    hi_u8 buf_nums, hi_u8 is_ba_buf)
{
    oal_netbuf_stru *netbuf = HI_NULL;
    hi_u8 netbuf_num;

    hi_unref_param(rx_ctrl);

    if (ret != HI_SUCCESS) {
        hmac_rx_free_netbuf_list(netbuf_header, buf_nums);
        return;
    }

    if (is_ba_buf == HI_TRUE) {
        return;
    }

    /* 如果不buff进reorder队列，则重新挂到链表尾，保序 */
    for (netbuf_num = 0; netbuf_num < buf_nums; netbuf_num++) {
        netbuf = oal_netbuf_delist(netbuf_header);
        if (oal_likely(netbuf != HI_NULL)) {
            oal_netbuf_add_to_list_tail(netbuf, netbuf_header);
        } else {
            oam_warning_log0(rx_ctrl->vap_id, OAM_SF_RX, "{hmac_rx_netbuf_add_to_list_tail::no buff error.}");
        }
    }
}

/* ****************************************************************************
 功能描述  :
 修改历史      :
  1.日    期   : 2015年1月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_rx_process_data_filter(oal_netbuf_head_stru *netbuf_header, oal_netbuf_stru *temp_netbuf,
    hi_u16 us_netbuf_num)
{
    hi_u8  buf_nums;
    hi_u32 ret = HI_SUCCESS;

    while (us_netbuf_num != 0) {
        hi_u8 is_ba_buf = HI_FALSE;
        oal_netbuf_stru *netbuf = temp_netbuf;
        if (netbuf == HI_NULL) {
            oam_warning_log1(0, OAM_SF_RX, "{hmac_rx_process_data_filter::us_netbuf_num = %d}", us_netbuf_num);
            break;
        }

        hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
        buf_nums = rx_ctrl->buff_nums;

        /* 获取下一个要处理的MPDU */
        oal_netbuf_get_appointed_netbuf(netbuf, buf_nums, &temp_netbuf);
        us_netbuf_num = hi_sub(us_netbuf_num, buf_nums);
#ifdef _PRE_WLAN_FEATURE_AMPDU
        hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(rx_ctrl->us_ta_user_idx);
        if (oal_unlikely((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL) ||
            (hmac_user->base_user->is_user_alloced != MAC_USER_ALLOCED))) {
            hmac_rx_free_netbuf_list(netbuf_header, buf_nums);
            oam_info_log0(rx_ctrl->vap_id, OAM_SF_RX, "{hmac_rx_process_data_filter::user null.}");
            continue;
        }
        mac_ieee80211_frame_stru *frame_hdr = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;
#endif

        mac_vap_stru *mac_vap = mac_vap_get_vap_stru(rx_ctrl->mac_vap_id);
        if (oal_unlikely(mac_vap == HI_NULL)) {
            hmac_rx_free_netbuf_list(netbuf_header, buf_nums);
            oam_warning_log0(rx_ctrl->vap_id, OAM_SF_RX, "{hmac_rx_process_data_filter::pst_vap null.}");
            continue;
        }

        if (mac_vap->vap_id == 0 || mac_vap->vap_id > WLAN_VAP_NUM_PER_DEVICE) {
            oam_error_log1(0, OAM_SF_RX, "{hmac_rx_process_data_filter::Invalid vap_id.vap_id[%u]}", mac_vap->vap_id);
            hmac_rx_free_netbuf_list(netbuf_header, buf_nums);
            continue;
        }

#ifdef _PRE_WLAN_FEATURE_AMPDU
        hmac_filter_serv_info_stru filter_serv_info;
        filter_serv_info.netbuf_header = netbuf_header;
        filter_serv_info.pen_is_ba_buf = &is_ba_buf;
        ret = HI_SUCCESS;
        if (rx_ctrl->amsdu_enable == HI_FALSE) {
            ret = hmac_ba_filter_serv(mac_vap, hmac_user, rx_ctrl, frame_hdr, &filter_serv_info);
        }
#endif

        hmac_rx_netbuf_add_to_list_tail(netbuf_header, rx_ctrl, ret, buf_nums, is_ba_buf);
    }
}

/* ****************************************************************************
 功能描述  : 逐一处理需要上报的帧
 修改历史      :
  1.日    期   : 2015年1月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_rx_lan_frame(const oal_netbuf_head_stru *netbuf_header)
{
    hi_u32 netbuf_num;
    oal_netbuf_stru *temp_netbuf = HI_NULL;
    oal_netbuf_stru *netbuf = HI_NULL;
    hi_u8 buf_nums;
    hmac_rx_ctl_stru *rx_ctrl = HI_NULL;
    mac_ieee80211_frame_stru *frame_hdr = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    hi_u32 err_code;

    netbuf_num = oal_netbuf_get_buf_num(netbuf_header);
    temp_netbuf = oal_netbuf_peek(netbuf_header);

    while (netbuf_num != 0) {
        netbuf = temp_netbuf;
        if (netbuf == NULL) {
            break;
        }

        rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
        frame_hdr = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;
        buf_nums = rx_ctrl->buff_nums;

        netbuf_num = hi_sub(netbuf_num, buf_nums);
        oal_netbuf_get_appointed_netbuf(netbuf, buf_nums, &temp_netbuf);

        hmac_vap = hmac_vap_get_vap_stru(rx_ctrl->mac_vap_id);
        if (hmac_vap == HI_NULL) {
            oam_error_log1(0, OAM_SF_RX, "{hmac_rx_lan_frame::hmac_vap_get_vap_stru null. vap_id:%u}",
                rx_ctrl->mac_vap_id);
            continue;
        }
        rx_ctrl->us_da_user_idx = hmac_vap->base_vap->assoc_vap_id;

        err_code = hmac_rx_lan_frame_classify(hmac_vap, netbuf, frame_hdr);
        if (err_code != HI_SUCCESS) {
            hmac_rx_free_netbuf(netbuf, (hi_u16)buf_nums);
        }
    }
}

static hi_void hmac_rx_process_data_insert_list(hi_u16 us_netbuf_num, oal_netbuf_stru *temp_netbuf,
    oal_netbuf_head_stru *netbuf_header, const hmac_vap_stru *hmac_vap)
{
    oal_netbuf_stru *netbuf = HI_NULL; /* 用于保存当前处理的MPDU的第一个netbuf指针 */
#ifdef _PRE_WLAN_FEATURE_PROMIS
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hmac_rx_ctl_stru *rx_ctrl = HI_NULL;
    hi_u8 bssid[WLAN_MAC_ADDR_LEN] = {0};
    mac_ieee80211_frame_stru *mac_frame = HI_NULL;
#endif
#endif
    while (us_netbuf_num != 0) {
        netbuf = temp_netbuf;
        if (netbuf == HI_NULL) {
            break;
        }
        temp_netbuf = oal_netbuf_next(netbuf);
#ifdef _PRE_WLAN_FEATURE_PROMIS
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        mac_device_stru *mac_dev = mac_res_get_dev();
        if (mac_dev->promis_switch) {
            /* 处理上报的其他BSS组播数据包 */
            hi_u32 ret = hwal_send_others_bss_data(netbuf);
            if (ret != HI_SUCCESS) {
                oam_error_log1(0, OAM_SF_RX, "hwal_send_others_bss_data failed! ul_ret=%d", ret);
            }
            rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
            if (rx_ctrl == HI_NULL) {
                oal_netbuf_free(netbuf);
                us_netbuf_num--;
                continue;
            }
            mac_frame = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;
            mac_get_bssid((const hi_u8 *)mac_frame, (hi_u8 *)bssid, WLAN_MAC_ADDR_LEN);
            if (memcmp(bssid, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != 0) {
                oal_netbuf_free(netbuf);
                us_netbuf_num--;
                continue;
            }
        }
#endif
#endif
        oal_netbuf_add_to_list_tail(netbuf, netbuf_header);
        us_netbuf_num--;
    }
    if (us_netbuf_num != 0) {
        oam_error_log1(0, OAM_SF_RX, "{hmac_rx_process_data_insert_list::us_netbuf_num[%d].}", us_netbuf_num);
    }
}

/* ****************************************************************************
 功能描述  : AP模式下，HMAC模块接收WLAN_DRX事件(数据帧)的处理函数
 输入参数  : 事件结构体指针
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2013年3月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_rx_process_data_ap(frw_event_mem_stru *event_mem)
{
    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_rx_process_data_ap::evenevent_memt is NULL!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 获取事件头和事件结构体指针 */
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_stru *event_hdr = &(event->event_hdr);
    dmac_wlan_drx_event_stru *wlan_rx_event = (dmac_wlan_drx_event_stru *)(event->auc_event_data);
    /* 用于临时保存下一个需要处理的netbuf指针 */
    oal_netbuf_stru *temp_netbuf = (oal_netbuf_stru *)wlan_rx_event->netbuf;
    hi_u16 us_netbuf_num = wlan_rx_event->us_netbuf_num; /* netbuf链表的个数 */
    oal_netbuf_head_stru netbuf_header;                  /* 存储上报给网络层的数据 */

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event_hdr->vap_id);
    if (hmac_vap == HI_NULL || hmac_vap->base_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_RX, "{hmac_rx_process_data_ap::hmac_vap/mac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 将所有netbuff全部入链表 */
    oal_netbuf_list_head_init(&netbuf_header);
    hmac_rx_process_data_insert_list(us_netbuf_num, temp_netbuf, &netbuf_header, hmac_vap);

    if (oal_netbuf_list_empty(&netbuf_header) == HI_TRUE) {
        return HI_SUCCESS;
    }

    /* 将Dmac上报的帧进入reorder队列过滤一下 */
    hmac_rx_process_data_filter(&netbuf_header, (oal_netbuf_stru *)wlan_rx_event->netbuf, wlan_rx_event->us_netbuf_num);

#ifdef _PRE_WLAN_FEATURE_MESH
    if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        /* 将需要上报的帧逐一出队处理 */
        hmac_rx_process_data_mesh_tcp_ack_opt(hmac_vap, &netbuf_header);
    } else {
        hmac_rx_process_data_ap_tcp_ack_opt(hmac_vap, &netbuf_header);
    }
#else
    hmac_rx_process_data_ap_tcp_ack_opt(hmac_vap, &netbuf_header);
#endif

    return HI_SUCCESS;
}

hi_void hmac_rx_process_no_multicast_proc(const hmac_vap_stru *hmac_vap, const hi_u8 *mac_addr,
    oal_netbuf_stru *netbuf, oal_netbuf_head_stru *w2w_netbuf_hdr)
{
    hi_u8 user_idx;
    hmac_rx_ctl_stru         *rx_ctrl   = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    mac_ieee80211_frame_stru *frame_hdr = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;

    /* 获取目的地址对应的用户指针 */
    hi_u32 rslt = mac_vap_find_user_by_macaddr(hmac_vap->base_vap, mac_addr, WLAN_MAC_ADDR_LEN, &user_idx);
    if (rslt == HI_ERR_CODE_PTR_NULL) { /* 查找用户失败 */
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_rx_process_no_multicast_proc::get user Err}");

        /* 释放当前处理的MPDU占用的netbuf */
        hmac_rx_free_netbuf(netbuf, (hi_u16)rx_ctrl->buff_nums);
        return;
    }

    /* 没有找到对应的用户 */
    if (rslt != HI_SUCCESS) {
        /* 目的用户不在AP的用户表中，调用wlan_to_lan转发接口 */
        rslt = hmac_rx_lan_frame_classify(hmac_vap, netbuf, frame_hdr);
        if (rslt != HI_SUCCESS) {
            oam_warning_log1(rx_ctrl->vap_id, OAM_SF_RX, "hmac_rx_process_data_ap_tcp_ack_opt:: rx_lan_frame_fail[%d]",
                rslt);
            hmac_rx_free_netbuf(netbuf, (hi_u16)rx_ctrl->buff_nums);
        }
        return;
    }

    /* 目的用户已在AP的用户表中，进行WLAN_TO_WLAN转发 */
    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_rx_process_no_multicast_proc::hmac_user null}");

        hmac_rx_free_netbuf(netbuf, (hi_u16)rx_ctrl->buff_nums);
        return;
    }

    if (hmac_user->base_user->user_asoc_state != MAC_USER_STATE_ASSOC) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX,
            "{hmac_rx_process_no_multicast_proc::the station is not associated with ap.}");

        hmac_rx_free_netbuf(netbuf, (hi_u16)rx_ctrl->buff_nums);
        hmac_mgmt_send_deauth_frame(hmac_vap->base_vap, mac_addr, WLAN_MAC_ADDR_LEN, MAC_NOT_AUTHED);
        return;
    }

    /* 将目的地址的资源池索引值放到cb字段中，user的asoc id会在关联的时候被赋值 */
    rx_ctrl->us_da_user_idx = hmac_user->base_user->us_assoc_id;

    /* 将MPDU解析成单个MSDU，把所有的MSDU组成一个netbuf链 */
    if (hmac_rx_prepare_msdu_list_to_wlan(hmac_vap, w2w_netbuf_hdr, netbuf, frame_hdr) != HI_SUCCESS) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "hmac_rx_prepare_msdu_list_to_wlan return NON SUCCESS");
        hmac_rx_free_netbuf(netbuf, (hi_u16)rx_ctrl->buff_nums);
    }
}

/* ****************************************************************************
 功能描述  : AP模式下，HMAC模块接收WLAN_DRX事件(数据帧)的处理函数
 输入参数  : 事件结构体指针
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2013年3月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_rx_process_data_ap_tcp_ack_opt(const hmac_vap_stru *hmac_vap, const oal_netbuf_head_stru *netbuf_header)
{
    mac_ieee80211_frame_stru *copy_frame_hdr = HI_NULL; /* 保存mac帧的指针 */
    oal_netbuf_stru          *netbuf_copy = HI_NULL;    /* 用于保存组播帧copy */
    oal_netbuf_stru          *temp_netbuf = oal_netbuf_peek(netbuf_header);
    hi_u8                    *mac_addr = HI_NULL;       /* 保存用户目的地址的指针 */
    hi_u16 us_netbuf_num = (hi_u16)oal_netbuf_get_buf_num(netbuf_header);
    oal_netbuf_head_stru w2w_netbuf_hdr; /* 保存wlan to wlan的netbuf链表的头 */
    frw_event_hdr_stru   event_hdr;

    event_hdr.vap_id = hmac_vap->base_vap->vap_id;
    /* 循环收到的每一个MPDU，处情况如下:
       1、组播帧时，调用WLAN TO WLAN和WLAN TO LAN接口
       2、其他，根据实际情况，调用WLAN TO LAN接口或者WLAN TO WLAN接口 */
    oal_netbuf_list_head_init(&w2w_netbuf_hdr);
    while (us_netbuf_num != 0) {
        oal_netbuf_stru *netbuf = temp_netbuf;
        if (netbuf == HI_NULL) {
            break;
        }
        hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
        /* 获取帧头信息 */
        mac_ieee80211_frame_stru *frame_hdr = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;
        /* 获取下一个要处理的MPDU */
        oal_netbuf_get_appointed_netbuf(netbuf, rx_ctrl->buff_nums, &temp_netbuf);
        us_netbuf_num = hi_sub(us_netbuf_num, rx_ctrl->buff_nums);
        hmac_vap = hmac_vap_get_vap_stru(rx_ctrl->mac_vap_id);
        if (oal_unlikely(hmac_vap == HI_NULL)) {
            hmac_rx_free_netbuf(netbuf, (hi_u16)rx_ctrl->buff_nums);
            continue;
        }

        /* 获取接收端地址  */
        mac_rx_get_da(frame_hdr, &mac_addr);
        /* 单播报文处理 */
        if (ether_is_multicast(mac_addr) == HI_FALSE) {
            hmac_rx_process_no_multicast_proc(hmac_vap, mac_addr, netbuf, &w2w_netbuf_hdr);
            continue;
        }
        /* 目的地址为组播地址时，进行WLAN_TO_WLAN和WLAN_TO_LAN的转发 */
        if (hmac_rx_copy_netbuff(&netbuf_copy, netbuf, rx_ctrl->mac_vap_id, &copy_frame_hdr) == HI_SUCCESS) {
            /* 将MPDU解析成单个MSDU，把所有的MSDU组成一个netbuf链 */
            if (hmac_rx_prepare_msdu_list_to_wlan(hmac_vap, &w2w_netbuf_hdr, netbuf_copy, copy_frame_hdr) !=
                HI_SUCCESS) {
                oam_warning_log0(0, OAM_SF_RX, "hmac_rx_prepare_msdu_list_to_wlan return NON SUCCESS");
                oal_netbuf_free(netbuf_copy);
            }
        }
        /* 上报网络层 WLAN_TO_LAN */
        hi_u32 err_code = hmac_rx_lan_frame_classify(hmac_vap, netbuf, frame_hdr);
        if (err_code != HI_SUCCESS) {
            hmac_rx_free_netbuf(netbuf, (hi_u16)rx_ctrl->buff_nums);
        }
    }

    /*  将MSDU链表交给发送流程处理 WLAN_TO_WLAN */
    if ((oal_netbuf_list_empty(&w2w_netbuf_hdr) == HI_FALSE) && (oal_netbuf_tail(&w2w_netbuf_hdr) != HI_NULL) &&
        (oal_netbuf_peek(&w2w_netbuf_hdr) != HI_NULL)) {
        set_oal_netbuf_next((oal_netbuf_tail(&w2w_netbuf_hdr)), HI_NULL);
        set_oal_netbuf_prev((oal_netbuf_peek(&w2w_netbuf_hdr)), HI_NULL);

        hmac_rx_transmit_to_wlan(&event_hdr, &w2w_netbuf_hdr);
    }
}

/* ****************************************************************************
 功能描述  : STA模式下，HMAC模块接收WLAN_DRX事件(数据帧)的处理函数
 输入参数  : 事件结构体指针
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2013年3月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
************************ **************************************************** */
hi_u32 hmac_rx_process_data_sta(frw_event_mem_stru *event_mem)
{
    hi_u16 us_netbuf_num;               /* netbuf链表的个数 */
    oal_netbuf_head_stru netbuf_header; /* 存储上报给网络层的数据 */

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_RX, "{hmac_rx_process_data_sta::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取事件头和事件结构体指针 */
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_stru *event_hdr = &(event->event_hdr);
    dmac_wlan_drx_event_stru *wlan_rx_event = (dmac_wlan_drx_event_stru *)(event->auc_event_data);
    oal_netbuf_stru *temp_netbuf = (oal_netbuf_stru *)wlan_rx_event->netbuf;
    us_netbuf_num = wlan_rx_event->us_netbuf_num;

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event_hdr->vap_id);
    if (hmac_vap == HI_NULL || hmac_vap->base_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_RX, "{hmac_rx_process_data_sta::hmac_vap/mac_vap null.}");
        hmac_rx_free_netbuf(temp_netbuf, us_netbuf_num);
        return HI_ERR_CODE_PTR_NULL;
    }
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* If mib info is null ptr,release the netbuf */
    if (hmac_vap->base_vap->mib_info == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_rx_process_data_sta::pst_mib_info null.}");
        hmac_rx_free_netbuf(temp_netbuf, us_netbuf_num);
        return HI_SUCCESS;
    }
#endif

    /* 将所有netbuff全部入链表 */
    oal_netbuf_list_head_init(&netbuf_header);
    hmac_rx_process_data_insert_list(us_netbuf_num, temp_netbuf, &netbuf_header, hmac_vap);

    if (oal_netbuf_list_empty(&netbuf_header) == HI_TRUE) {
        return HI_SUCCESS;
    }

    hmac_rx_process_data_filter(&netbuf_header, (oal_netbuf_stru *)wlan_rx_event->netbuf, wlan_rx_event->us_netbuf_num);
    hmac_rx_lan_frame(&netbuf_header);
    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : mesh模式下，HMAC模块接收WLAN_DRX事件(数据帧)的处理函数
 输入参数  : 事件结构体指针
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2019年2月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_rx_process_data_mesh_tcp_ack_opt(hmac_vap_stru *hmac_vap, const oal_netbuf_head_stru *netbuf_header)
{
    mac_ieee80211_frame_stru *frame_hdr = HI_NULL;   /* 保存mac帧的指针 */
    hi_u8                    *da_mac_addr = HI_NULL;          /* 保存用户目的地址的指针 */
    hmac_rx_ctl_stru         *rx_ctrl = HI_NULL;     /* 每一个MPDU的控制信息 */
    hi_u16                    us_netbuf_num;             /* netbuf链表的个数 */
    hi_u8                     buf_nums;               /* 每个mpdu占有buf的个数 */
    oal_netbuf_stru          *netbuf = HI_NULL;      /* 用于保存当前处理的MPDU的第一个netbuf指针 */
    oal_netbuf_stru          *temp_netbuf = HI_NULL; /* 用于临时保存下一个需要处理的netbuf指针 */
    hi_u32                    err_code;

    temp_netbuf = oal_netbuf_peek(netbuf_header);
    us_netbuf_num = (hi_u16)oal_netbuf_get_buf_num(netbuf_header);

    while (us_netbuf_num != 0) {
        netbuf = temp_netbuf;
        if (netbuf == HI_NULL) {
            break;
        }

        rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);

        /* 获取帧头信息 */
        frame_hdr = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;

        /* 获取当前MPDU占用的netbuf数目 */
        buf_nums = rx_ctrl->buff_nums;

        /* 获取下一个要处理的MPDU */
        oal_netbuf_get_appointed_netbuf(netbuf, buf_nums, &temp_netbuf);
        us_netbuf_num = hi_sub(us_netbuf_num, buf_nums);

        hmac_vap = hmac_vap_get_vap_stru(rx_ctrl->mac_vap_id);
        if (oal_unlikely(hmac_vap == HI_NULL)) {
            oam_warning_log0(rx_ctrl->vap_id, OAM_SF_RX, "{hmac_rx_process_data_mesh_tcp_ack_opt::pst_vap null.}");
            hmac_rx_free_netbuf(netbuf, (hi_u16)buf_nums);
            continue;
        }
        /* 获取接收端地址  */
        mac_rx_get_da(frame_hdr, &da_mac_addr);

        err_code = hmac_rx_lan_frame_classify(hmac_vap, netbuf, frame_hdr); /* 上报网络层 */
        if (err_code != HI_SUCCESS) {
            oam_warning_log1(rx_ctrl->vap_id, OAM_SF_RX,
                "hmac_rx_process_data_ap_tcp_ack_opt:: rx_lan_frame_fail[%d]", err_code);
            hmac_rx_free_netbuf(netbuf, (hi_u16)buf_nums);
        }
    }

    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
