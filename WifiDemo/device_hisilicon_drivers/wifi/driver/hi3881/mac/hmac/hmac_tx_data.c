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
#include "hmac_tx_data.h"
#include "mac_frame.h"
#include "mac_data.h"
#include "hmac_frag.h"
#include "hmac_ext_if.h"
#ifdef _PRE_WLAN_FEATURE_WAPI
#include "hmac_wapi.h"
#endif
#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
#include "hmac_traffic_classify.h"
#endif
#include "hmac_crypto_tkip.h"
#include "hmac_device.h"
#include "hcc_hmac_if.h"
#include "wal_customize.h"
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
#include "hmac_edca_opt.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
static hi_u16 g_us_noqos_frag_seqnum = 0; /* 保存非qos分片帧seqnum */
#endif

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
hi_u32 hmac_tx_data(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf);
#endif
/* ****************************************************************************
 函 数 名  : free_netbuf_list
 功能描述  : 释放一个netbuf链，此链中的skb或者都来自lan，或者都来自wlan
 输入参数  : pst_buf－SKB结构体单向链表，最后一个next指针务必为NULL，否则会出异常
 输出参数  :
 返 回 值  : 释放的buf数目
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u16 hmac_free_netbuf_list(oal_netbuf_stru *netbuf)
{
    oal_netbuf_stru     *buf_tmp = HI_NULL;
    hmac_tx_ctl_stru     *tx_cb = HI_NULL;
    hi_u16           us_buf_num = 0;

    if (netbuf != HI_NULL) {
        while (netbuf != HI_NULL) {
            buf_tmp = oal_netbuf_list_next(netbuf);
            us_buf_num++;
            set_oal_netbuf_next(netbuf, HI_NULL);
            tx_cb = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
            /* 如果frame_header部分是内存池申请的才从内存池释放，采用skb内部自身的，就不需要再释放 */
            if ((tx_cb->mac_head_type == 0) && (tx_cb->frame_header != HI_NULL)) {
                oal_free(tx_cb->frame_header);
                tx_cb->frame_header = HI_NULL;
            }

            oal_netbuf_free(netbuf);

            netbuf = buf_tmp;
        }
    } else {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_free_netbuf_list::pst_buf is null}");
    }

    return us_buf_num;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 函 数 名  : hmac_tid_num_set
 功能描述  : 根据优先级设置tid号
 修改历史      :
  1.日    期   : 2019年09月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_tid_num_set(hi_u32 buf_prio, hi_u8 *tid)
{
    switch (buf_prio) {
        case 0: /* 0:优先级 */
            *tid = WLAN_TIDNO_BEST_EFFORT;
            break;
        case 1: /* 1:优先级 */
            *tid = WLAN_TIDNO_BACKGROUND;
            break;
        case 2: /* 2:优先级 */
            *tid = WLAN_TIDNO_VIDEO;
            break;
        default:
            *tid = WLAN_TIDNO_VOICE;
            break;
    }
}
#endif

#ifdef _PRE_WLAN_FEATURE_CLASSIFY
/* ****************************************************************************
 功能描述  : 从lan过来的IP报文业务识别
**************************************************************************** */
static hi_void hmac_tx_classify_ipv4_data(hmac_tx_ctl_stru *tx_ctl, mac_ether_header_stru *ether_header, hi_u8 *puc_tid)
{
    mac_ip_header_stru *ip = HI_NULL;
    hi_u8 tid;
#ifdef _PRE_WLAN_FEATURE_SCHEDULE
    mac_tcp_header_stru *tcp = HI_NULL;
#endif

#if defined(_PRE_WLAN_FEATURE_EDCA_OPT_AP) || defined(_PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN)
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(tx_ctl->tx_vap_index);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(0, OAM_SF_TX, "{hmac_tx_classify_ipv4_data::get hmac_vap[%d] fail.}", tx_ctl->tx_vap_index);
        return;
    }
#endif
    /* 从IP TOS字段寻找优先级 */
    /* ----------------------------------------------------------------------
        tos位定义
     ----------------------------------------------------------------------
    | bit7~bit5 | bit4 |  bit3  |  bit2  |   bit1   | bit0 |
    | 包优先级  | 时延 | 吞吐量 | 可靠性 | 传输成本 | 保留 |
     ---------------------------------------------------------------------- */
    ip = (mac_ip_header_stru *)(ether_header + 1); /* 偏移一个以太网头，取ip头 */
    tid = ip->tos >> WLAN_IP_PRI_SHIFT;
#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
    if (hmac_vap->tx_traffic_classify_flag == HI_SWITCH_ON) {
        if (tid != 0) {
            return;
        }
        hmac_tx_traffic_classify(tx_ctl, ip, &tid);
    }
#endif /* _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN */

    /* 如果是DHCP帧，则进入VO队列发送 */
    if (mac_is_dhcp_port(ip)) {
        tid = WLAN_DATA_VIP_TID;
        tx_ctl->is_vipframe  = HI_TRUE;
        tx_ctl->is_needretry = HI_TRUE;
    } else if (ip->protocol == MAC_ICMP_PROTOCAL) {
        tx_ctl->high_prio_sch = HI_TRUE;
        /* 对于ping包采取软件重传策略 */
        tx_ctl->is_needretry = HI_TRUE;
#ifdef _PRE_WLAN_FEATURE_SCHEDULE
    } else if (ip->protocol == MAC_TCP_PROTOCAL) {
        /* 对于chariot信令报文进行特殊处理，防止断流 */
        tcp = (mac_tcp_header_stru *)(ip + 1);
        if ((oal_ntoh_16(tcp->us_dport) == MAC_CHARIOT_NETIF_PORT) ||
            (oal_ntoh_16(tcp->us_sport) == MAC_CHARIOT_NETIF_PORT)) {
            tid = WLAN_DATA_VIP_TID;
            tx_ctl->is_vipframe  = HI_TRUE;
            tx_ctl->is_needretry = HI_TRUE;
        }
#endif
    }

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    /* EDCA AP优化统计量刷新 */
    hmac_edca_opt_tx_pkts_stat(tx_ctl, tid, ip);
#endif
    *puc_tid = tid;
}

/* ****************************************************************************
 功能描述  : 从lan过来的IPV6报文业务识别
**************************************************************************** */
static hi_void hmac_tx_classify_ipv6_data(hmac_tx_ctl_stru *tx_ctl, mac_ether_header_stru *ether_header,
    hi_u32 buf_prio, hi_u8 *puc_tid)
{
    hi_u32 ipv6_hdr;
    hi_u32 pri;
    hi_u8  tid;

#if defined(_PRE_WLAN_FEATURE_MESH)
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(tx_ctl->tx_vap_index);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(0, OAM_SF_TX, "{hmac_tx_classify_ipv6_data::get hmac_vap[%d] fail.}", tx_ctl->tx_vap_index);
        return;
    }
#else
    hi_unref_param(buf_prio);
#endif
    /* 从IPv6 traffic class字段获取优先级 */
    /* ----------------------------------------------------------------------
        IPv6包头 前32为定义
     -----------------------------------------------------------------------
    | 版本号 | traffic class   | 流量标识 |
    | 4bit   | 8bit(同ipv4 tos)|  20bit   |
    ----------------------------------------------------------------------- */
    ipv6_hdr = *((hi_u32 *)(ether_header + 1)); /* 偏移一个以太网头，取ip头 */
    pri = (oal_net2host_long(ipv6_hdr) & WLAN_IPV6_PRIORITY_MASK) >> WLAN_IPV6_PRIORITY_SHIFT;
    tid = (hi_u8)(pri >> WLAN_IP_PRI_SHIFT);
    /* 如果是ND DHCPV6帧，则进入VO队列发送 */
    if (mac_is_nd((oal_ipv6hdr_stru *)(ether_header + 1)) || mac_is_dhcp6((oal_ipv6hdr_stru *)(ether_header + 1))) {
        tid = WLAN_DATA_VIP_TID;
        tx_ctl->is_vipframe  = HI_TRUE;
        tx_ctl->is_needretry = HI_TRUE;
    } else { /* 规避编程规范K&R检查 else与if拆分 */
#ifdef _PRE_WLAN_FEATURE_MESH
        if (mac_is_rpl((oal_ipv6hdr_stru *)(ether_header + 1))) {
            oam_warning_log0(0, OAM_SF_TX, "{hmac_tx_classify_ipv6_data::ETHER_TYPE_RPL.}");
            tid = WLAN_DATA_VIP_TID;
            tx_ctl->is_vipframe  = HI_TRUE;
            tx_ctl->is_needretry = HI_TRUE;
        } else if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) {
            /* 打桩，当有包头压缩时用pbuf flag判断 */
            /*
            uc_pbuf_flags:
            |BIT1       |BIT0           |
            |Ctrl PKT  |Compressed 6lo  |
            */
            if (tx_ctl->pbuf_flags & BIT1) {
                tid = WLAN_DATA_VIP_TID;
                tx_ctl->is_vipframe = HI_TRUE;
            } else {
                /* 优先级：0-3,超出3统一认为与3一致 */
                hmac_tid_num_set(buf_prio, &tid);
            }
        }
#endif
    }
    *puc_tid = tid;
}

/* ****************************************************************************
 功能描述  : 从lan过来报文的业务识别
 修改历史      :
  1.日    期   : 2013年10月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年11月23日
    作    者   : HiSilicon
    修改内容   : 函数算法增量
**************************************************************************** */
static hi_void hmac_tx_classify_lan_to_wlan(oal_netbuf_stru *netbuf, hi_u8 *puc_tid)
{
    mac_ether_header_stru  *ether_header = HI_NULL;
    hmac_tx_ctl_stru       *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
    hi_u32                  buf_prio = oal_netbuf_priority(netbuf);
    hi_u8                   tid = 0;

    /* 获取以太网头 */
    ether_header = (mac_ether_header_stru *)oal_netbuf_data(netbuf);
    /* 大函数整改更换为if else语句 */
    if (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_IP)) {
        /* ipv4报文处理 */
        hmac_tx_classify_ipv4_data(tx_ctl, ether_header, &tid);
    } else if (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_IPV6)) {
        /* ipv6报文处理 */
        hmac_tx_classify_ipv6_data(tx_ctl, ether_header, buf_prio, &tid);
    } else if (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_PAE)) {
        /* 如果是EAPOL帧，则进入VO队列发送 */
        tid = WLAN_DATA_VIP_TID;
        tx_ctl->is_vipframe  = HI_TRUE;
        tx_ctl->is_needretry = HI_TRUE;
        /* 如果是4 次握手设置单播密钥，则设置tx cb 中bit_is_eapol_key_ptk 置一，dmac 发送不加密 */
        if (mac_is_eapol_key_ptk((mac_eapol_header_stru *)(ether_header + 1))) {
            tx_ctl->is_eapol_key_ptk = HI_TRUE;
        }
    } else if (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_TDLS)) {
        /* TDLS帧处理，建链保护，入高优先级TID队列 */
        tid = WLAN_DATA_VIP_TID;
        oam_info_log1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::TDLS tid=%d.}", tid);
    } else if ((ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_PPP_DISC)) ||
               (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_PPP_SES)) ||
               (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_WAI))) {
        /* PPPOE帧处理，建链保护(发现阶段, 会话阶段)，入高优先级TID队列 */
        tid = WLAN_DATA_VIP_TID;
        tx_ctl->is_vipframe  = HI_TRUE;
        tx_ctl->is_needretry = HI_TRUE;
        oam_info_log2(0, 0, "{hmac_tx_classify_lan_to_wlan::type=%d, tid=%d.}", ether_header->us_ether_type, tid);
    } else if (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_ARP)) {
        /* 如果是ARP帧，则进入VO队列发送 */
        tid = WLAN_DATA_VIP_TID;
        tx_ctl->is_vipframe = HI_TRUE;
    } else if (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_VLAN)) {
        /* 获取vlan tag的优先级 */
        oal_vlan_ethhdr_stru *vlan_ethhdr = (oal_vlan_ethhdr_stru *)oal_netbuf_data(netbuf);
        /* ------------------------------------------------------------------
            802.1Q(VLAN) TCI(tag control information)位定义
         -------------------------------------------------------------------
        |Priority | DEI  | Vlan Identifier |
        | 3bit    | 1bit |      12bit      |
         ------------------------------------------------------------------ */
        hi_u16 vlan_tci = oal_net2host_short(vlan_ethhdr->h_vlan_tci);
        tid = vlan_tci >> OAL_VLAN_PRIO_SHIFT; /* 右移13位，提取高3位优先级 */
        oam_info_log1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::VLAN tid=%d.}", tid);
    } else { /* 规避编程规范K&R检查 else与if拆分 */
#ifdef _PRE_WLAN_FEATURE_MESH
        if (ether_header->us_ether_type == oal_host2net_short(ETHER_TYPE_6LO)) {
            if (tx_ctl->pbuf_flags & BIT0) {
                /* 优先级：0-3,超出3统一认为与3一致 */
                hmac_tid_num_set(buf_prio, &tid);
            }
        }
#endif
    }
    /* 出参赋值 */
    *puc_tid = tid;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_update_tid
 功能描述  : 根据算法需求，tid = 1, 3, 5, 7的，分别更新为0, 2, 4, 6
             如果WMM功能关闭，直接填为DMAC_WMM_SWITCH_TID
 输入参数  : puc_tid 注意，此参数为入出参
 输出参数  : puc_tid
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void hmac_tx_update_tid(hi_u8 wmm, hi_u8 *puc_tid)
{
    if (oal_likely(wmm == HI_TRUE)) { /* wmm使能 */
        *puc_tid = (*puc_tid < WLAN_TIDNO_BUTT) ? wlan_tos_to_tid(*puc_tid) : WLAN_TIDNO_BCAST;
    } else {
        /* wmm不使能 */
        *puc_tid = MAC_WMM_SWITCH_TID;
    }
}

/* ****************************************************************************
 函 数 名  : hmac_tx_wmm_acm
 功能描述  : 根据热点配置ACM，重新选择队列
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_tx_wmm_acm(hi_u8 wmm, const hmac_vap_stru *hmac_vap, hi_u8 *puc_tid)
{
    hi_u8 ac;
    hi_u8 ac_new;

    if ((hmac_vap == HI_NULL) || (hmac_vap->base_vap->mib_info == HI_NULL) || (puc_tid == HI_NULL)) {
        return HI_FALSE;
    }

    if (wmm == HI_FALSE) {
        return HI_FALSE;
    }

    ac = wlan_wme_tid_to_ac(*puc_tid);
    ac_new = ac;
    while ((ac_new != WLAN_WME_AC_BK) &&
        (hmac_vap->base_vap->mib_info->wlan_mib_qap_edac[ac_new].dot11_qapedca_table_mandatory == HI_TRUE)) {
        switch (ac_new) {
            case WLAN_WME_AC_VO:
                ac_new = WLAN_WME_AC_VI;
                break;

            case WLAN_WME_AC_VI:
                ac_new = WLAN_WME_AC_BE;
                break;

            default:
                ac_new = WLAN_WME_AC_BK;
                break;
        }
    }

    if (ac_new != ac) {
        *puc_tid = wlan_wme_ac_to_tid(ac_new);
    }

    return HI_TRUE;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_classify
 功能描述  : 以太网包 业务识别
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数

  2.日    期   : 2013年10月10日
    作    者   : HiSilicon
    修改内容   : 增加wlan to wlan分支处理

**************************************************************************** */
static hi_void hmac_tx_classify(const hmac_vap_stru *hmac_vap, const mac_user_stru *user, oal_netbuf_stru *netbuf)
{
    hi_u8             tid     = 0;
    hi_u8             ret;
    hmac_tx_ctl_stru *tx_ctl  = HI_NULL;
    mac_device_stru  *mac_dev = HI_NULL;

    hmac_tx_classify_lan_to_wlan(netbuf, &tid);

    /* 非QoS站点，直接返回 */
    if (oal_unlikely(user->cap_info.qos != HI_TRUE)) {
        /* ROM化防止换行 */
        oam_info_log0(hmac_vap->base_vap->vap_id, OAM_SF_TX, "{hmac_tx_classify::user is a none QoS station.}");
        return;
    }

    mac_dev = mac_res_get_dev();
    ret = hmac_tx_wmm_acm(mac_dev->wmm, hmac_vap, &tid);
    if (ret != HI_TRUE) {
        oam_info_log0(hmac_vap->base_vap->vap_id, OAM_SF_TX, "hmac_tx_wmm_acm return NON SUCCESS. ");
    }

    tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
    if ((tx_ctl->is_vipframe != HI_TRUE) || (mac_dev->wmm == HI_FALSE)) {
        hmac_tx_update_tid(mac_dev->wmm, &tid);
    }

    /* 设置ac和tid到cb字段 */
    tx_ctl->tid  = tid;
    tx_ctl->ac   = wlan_wme_tid_to_ac(tid);
    /* VO/VI队列中数据帧设置成需要重传 */
    if (tx_ctl->ac == WLAN_WME_AC_VI || tx_ctl->ac == WLAN_WME_AC_VO) {
        tx_ctl->is_needretry = HI_TRUE;
    }

    return;
}
#endif

/* ****************************************************************************
 函 数 名  : hmac_tx_filter_security
 功能描述  : 针对来自以太网的数据帧做安全过滤
 输入参数  : hmac_vap_stru     *pst_hmac_vap
             oal_netbuf_stru  *pst_buf
             hmac_user_stru   *pst_hmac_user
             hmac_tx_ctl_stru  *pst_tx_ctl
 输出参数  : HI_TRUE
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 hmac_tx_filter_security(const hmac_vap_stru *hmac_vap, const oal_netbuf_stru *netbuf,
    const hmac_user_stru *hmac_user)
{
    mac_ether_header_stru   *ether_header = HI_NULL;
    mac_user_stru           *mac_user     = HI_NULL;
    mac_vap_stru            *mac_vap      = HI_NULL;
    hi_u32                   ret          = HI_SUCCESS;

    mac_vap  = hmac_vap->base_vap;
    mac_user = hmac_user->base_user;

    if (mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated == HI_TRUE) { /* 判断是否使能WPA/WPA2 */
        if (mac_user->port_valid != HI_TRUE) {                                 /* 判断端口是否打开 */
            /* 获取以太网头 */
            ether_header = (mac_ether_header_stru *)oal_netbuf_data(netbuf);
            /* 发送数据时，针对非EAPOL 的数据帧做过滤 */
            if (hi_swap_byteorder_16(ETHER_TYPE_PAE) != ether_header->us_ether_type) {
                oam_info_log2(0, OAM_SF_TX, "{hmac_tx_filter_security::TYPE 0x%04x, 0x%04x.}",
                    hi_swap_byteorder_16(ether_header->us_ether_type), ETHER_TYPE_PAE);
                ret = HI_FAIL;
            }
        }
    }

    return ret;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_ba_setup
 功能描述  : 自动触发BA会话的建立
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_tx_ba_setup(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, hi_u8 tidno)
{
    mac_action_mgmt_args_stru action_args; /* 用于填写ACTION帧的参数 */
    hi_u8  ampdu_support;
    hi_u32 ret = HI_SUCCESS;

    /* 建立BA会话，是否需要判断VAP的AMPDU的支持情况，因为需要实现建立BA会话时，一定发AMPDU */
    ampdu_support = hmac_user_xht_support(hmac_user);
    if (ampdu_support) {
        /*
        建立BA会话时，st_action_args结构各个成员意义如下
        (1)uc_category:action的类别
        (2)uc_action:BA action下的类别
        (3)ul_arg1:BA会话对应的TID
        (4)ul_arg2:BUFFER SIZE大小
        (5)ul_arg3:BA会话的确认策略
        (6)ul_arg4:TIMEOUT时间
        */
        action_args.category = MAC_ACTION_CATEGORY_BA;
        action_args.action   = MAC_BA_ACTION_ADDBA_REQ;
        action_args.arg1     = tidno;                                      /* 该数据帧对应的TID号 */
        action_args.arg2     = (hi_u32)hmac_vap->max_ampdu_num;        /* ADDBA_REQ中，buffer_size的默认大小 */
        action_args.arg3     = MAC_BA_POLICY_IMMEDIATE;                       /* BA会话的确认策略 */
        action_args.arg4     = 0;                                             /* BA会话的超时时间设置为0 */
        oam_warning_log1(0, OAM_SF_TX, "hisi_customize_wifi::[ba buffer size:%d]", action_args.arg2);
        /* 建立BA会话 */
        ret = hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args);
    } else {
        if (hmac_user->ast_tid_info[tidno].ba_tx_info != HI_NULL) {
            action_args.category = MAC_ACTION_CATEGORY_BA;
            action_args.action   = MAC_BA_ACTION_DELBA;
            action_args.arg1     = tidno;                                         /* 该数据帧对应的TID号 */
            action_args.arg2     = MAC_ORIGINATOR_DELBA;                             /* 发送端删除ba */
            action_args.arg3     = MAC_UNSPEC_REASON;                                /* BA会话删除原因 */
            action_args.puc_arg5     = hmac_user->base_user->user_mac_addr;   /* 用户mac地址 */
            /* 删除BA会话 */
            ret = hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args);
        }
    }

    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_TX, "hmac_mgmt_tx_action return NON SUCCESS. ");
    }
}

hi_void hmac_check_if_mgmt_tx_action(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, hi_u8 tidno)
{
    if (hmac_vap->ampdu_tx_on_switch == HI_FALSE) {
        mac_action_mgmt_args_stru action_args = {0}; /* 用于填写ACTION帧的参数 */
        action_args.category = MAC_ACTION_CATEGORY_BA;
        action_args.action   = MAC_BA_ACTION_DELBA;
        action_args.arg1     = tidno;
        action_args.arg2     = MAC_ORIGINATOR_DELBA;
        action_args.arg3     = MAC_UNSPEC_REASON;
        action_args.puc_arg5 = hmac_user->base_user->user_mac_addr;

        if (hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args) != HI_SUCCESS) {
            oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_CFG, "hmac_mgmt_tx_action return NON SUCCESS. ");
        }
    }
}
/* ****************************************************************************
 函 数 名  : hmac_tx_ba_check
 功能描述  : 判断是否需要建立BA会话
 输入参数  : 无
 输出参数  : 无
 返 回 值  : HI_TRUE代表需要建立BA会话
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

修改历史      :
  2.日    期   : 2015年7月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u8 hmac_tid_need_ba_session(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, hi_u8 tidno,
    const oal_netbuf_stru *netbuf)
{
    if ((hmac_vap == HI_NULL) || (hmac_user == HI_NULL) || (netbuf == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        return HI_FALSE;
    }

    if (hmac_vap_ba_is_setup(hmac_user, tidno) == HI_TRUE) {
        hmac_check_if_mgmt_tx_action(hmac_vap, hmac_user, tidno);
        return HI_FALSE;
    }

    /* 配置命令不允许建立聚合时返回 */
    /* 窄带不建立聚合 */
    /* 针对VO业务, 根据VAP标志位确定是否建立BA会话 */
    /* 判断HMAC VAP的是否支持聚合 */
    if ((hmac_vap->ampdu_tx_on_switch == HI_FALSE) || (hmac_user_xht_support(hmac_user) == HI_FALSE) ||
        ((wlan_wme_tid_to_ac(tidno) == WLAN_WME_AC_VO) && (hmac_vap->base_vap->voice_aggr == HI_FALSE)) ||
        (!((hmac_vap->tx_aggr_on) || (hmac_vap->base_vap->cap_flag.rifs_tx_on)))) {
        return HI_FALSE;
    }

#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    if (hmac_vap->tx_ba_session_num >= WLAN_MAX_TX_BA) {
        return HI_FALSE;
    }
#else
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    if (hmac_dev->tx_ba_session_num >= WLAN_MAX_TX_BA) {
        return HI_FALSE;
    }
#endif

    /* 需要先发送5个单播帧，再进行BA会话的建立 */
    if ((hmac_user->base_user->cap_info.qos) && (hmac_user->ast_tid_info[tidno].ba_flag < DMAC_UCAST_TX_COMP_TIMES)) {
        hmac_user->ast_tid_info[tidno].ba_flag++;
        return HI_FALSE;
    } else if (hmac_user->base_user->cap_info.qos == HI_FALSE) {
        /* 针对关闭WMM，非QOS帧处理 */
        return HI_FALSE;
    }

    /* tx ba尝试次数超过最大值 则不允许再次建立BA */
    if ((hmac_user->ast_tid_info[tidno].ba_tx_info == HI_NULL) &&
        (hmac_user->ast_tid_info[tidno].tx_ba_attemps < HMAC_ADDBA_EXCHANGE_ATTEMPTS)) {
        hmac_user->ast_tid_info[tidno].tx_ba_attemps++;
    } else {
        return HI_FALSE;
    }

    return HI_TRUE;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_ucast_process
 功能描述  : 单播处理
 输入参数  : pst_vap VAP结构体; pst_buf netbuf结构体;pst_user 用户结构体
 输出参数  :
 返 回 值  :
 调用函数  : hmac_tx_mpdu_process_ap
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2013年09月12日
    作    者   : HiSilicon
    修改内容   : 修改函数，增加安全过滤

**************************************************************************** */
hmac_tx_return_type_enum_uint8 hmac_tx_ucast_process(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf,
    hmac_user_stru *hmac_user, const hmac_tx_ctl_stru *tx_ctl)
{
    hmac_tx_return_type_enum_uint8 ret = HMAC_TX_PASS;

    /* 安全过滤 */
#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
    if (oal_unlikely(hmac_tx_filter_security(hmac_vap, netbuf, hmac_user) != HI_SUCCESS)) {
        return HMAC_TX_DROP_SECURITY_FILTER;
    }
#endif

    /* 以太网业务识别 */
#ifdef _PRE_WLAN_FEATURE_CLASSIFY
    hmac_tx_classify(hmac_vap, hmac_user->base_user, netbuf);
#endif

    /* 如果是EAPOL、DHCP帧，则不允许主动建立BA会话 */
    if (tx_ctl->is_vipframe == HI_FALSE) {
#ifdef _PRE_WLAN_FEATURE_AMPDU
        if (hmac_tid_need_ba_session(hmac_vap, hmac_user, tx_ctl->tid, netbuf) == HI_TRUE) {
            /* 自动触发建立BA会话，设置AMPDU聚合参数信息在DMAC模块的处理addba rsp帧的时刻后面 */
            hmac_tx_ba_setup(hmac_vap, hmac_user, tx_ctl->tid);
        }
#endif
    }

    return ret;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_is_need_frag
 功能描述  : 检查该报文是否需要分片
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年2月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u8 hmac_tx_is_need_frag(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user,
    const oal_netbuf_stru *netbuf, const hmac_tx_ctl_stru *tx_ctl)
{
    hi_u32 threshold;
    hi_u32 last_frag;
    hi_u32 netbuf_len;
    /* 判断报文是否需要进行分片 */
    /* 1、长度大于门限          */
    /* 2、是legac协议模式       */
    /* 3、不是广播帧            */
    /* 4、不是聚合帧            */
    /* 6、DHCP帧不进行分片      */
    /* 7、mesh不分片 */
    if (tx_ctl->is_vipframe == HI_TRUE) {
        return HI_FALSE;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        return HI_FALSE;
    }
#endif

    threshold = hmac_vap->base_vap->mib_info->wlan_mib_operation.dot11_fragmentation_threshold;
    threshold = (threshold & (~(BIT0 | BIT1))) + 2; /* 加2 */
    /* 规避1151硬件bug,调整分片门限：TKIP加密时，当最后一个分片的payload长度
       小于等于8时，无法进行加密 */
    if (WLAN_80211_CIPHER_SUITE_TKIP == hmac_user->base_user->key_info.cipher_type) {
        last_frag = (oal_netbuf_len(netbuf) + 8) % (threshold - tx_ctl->frame_header_length - /* 加8 */
            (WEP_IV_FIELD_SIZE + EXT_IV_FIELD_SIZE));
        if ((last_frag > 0) && (last_frag <= 8)) { /* 8 边界 */
            threshold = threshold + 8;             /* 自增 8 */
        }
    }

    netbuf_len =
        (tx_ctl->mac_head_type == 1) ? oal_netbuf_len(netbuf) : (oal_netbuf_len(netbuf) + tx_ctl->frame_header_length);

    return (hi_u8)((netbuf_len > threshold) && (!tx_ctl->ismcast) && (!tx_ctl->is_amsdu) &&
        (hmac_user->base_user->cur_protocol_mode < WLAN_HT_MODE || hmac_vap->base_vap->protocol < WLAN_HT_MODE) &&
        (HI_FALSE == hmac_vap_ba_is_setup(hmac_user, tx_ctl->tid)));
}

/* ****************************************************************************
 函 数 名  : hmac_tx_set_frame_ctrl
 功能描述  : 设置帧控制
 输入参数  : ul_qos 是否是QOS站点 pst_tx_ctl CB字段 pst_hdr 802.11头
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void hmac_tx_set_frame_ctrl(hi_u32 qos, const hmac_tx_ctl_stru *tx_ctl,
    mac_ieee80211_qos_frame_addr4_stru *hdr_addr4)
{
    mac_ieee80211_qos_frame_stru *hdr = HI_NULL;
    hi_u8 is_amsdu;
    if (qos == HMAC_TX_BSS_QOS) {
        if (tx_ctl->netbuf_num == 1) {
            is_amsdu = HI_FALSE;
        } else {
            is_amsdu = tx_ctl->is_amsdu;
        }

        /* 设置帧控制字段 */
        mac_hdr_set_frame_control((hi_u8 *)hdr_addr4, (WLAN_FC0_SUBTYPE_QOS | WLAN_FC0_TYPE_DATA));

        /* 更新帧头长度 */
        if (tx_ctl->use_4_addr == HI_FALSE) {
            hdr = (mac_ieee80211_qos_frame_stru *)hdr_addr4;
            /* 设置QOS控制字段 */
            hdr->qc_tid        = tx_ctl->tid;
            hdr->qc_eosp       = 0;
            hdr->qc_ack_polocy = tx_ctl->ack_policy;
            hdr->qc_amsdu      = is_amsdu;
            hdr->qos_control.qc_txop_limit = 0;
        } else {
            /* 设置QOS控制字段 */
            hdr_addr4->qc_tid        = tx_ctl->tid;
            hdr_addr4->qc_eosp       = 0;
            hdr_addr4->qc_ack_polocy = tx_ctl->ack_policy;
            hdr_addr4->qc_amsdu      = is_amsdu;
            hdr_addr4->qos_control.qc_txop_limit = 0;
        }

        /* 由DMAC考虑是否需要HTC */
    } else {
        /* 设置帧控制字段 */
        mac_hdr_set_frame_control((hi_u8 *)hdr_addr4, WLAN_FC0_TYPE_DATA | WLAN_FC0_SUBTYPE_DATA);
    }
}

/* ****************************************************************************
 功能描述  : 3地址ap模式设置MAC头地址
**************************************************************************** */
static hi_u32 hmac_tx_ap_set_addresses(const hmac_vap_stru *hmac_vap, const hmac_tx_ctl_stru *tx_ctl,
    mac_ieee80211_frame_addr4_stru *hdr, const hmac_set_addresses_info_stru *set_addresses_info)
{
    /* From DS标识位设置 */
    mac_hdr_set_from_ds((hi_u8 *)hdr, 1);
    /* to DS标识位设置 */
    mac_hdr_set_to_ds((hi_u8 *)hdr, 0);
    /* Set Address1 field in the WLAN Header with destination address */
    if (memcpy_s(hdr->auc_address1, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_daddr, WLAN_MAC_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }
    /* Set Address2 field in the WLAN Header with the BSSID */
    if (memcpy_s(hdr->auc_address2, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }
    /* AMSDU情况，地址3填写BSSID */
    if (tx_ctl->is_amsdu) {
        /* Set Address3 field in the WLAN Header with the BSSID */
        if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
    } else {
        /* Set Address3 field in the WLAN Header with the source address */
        if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_saddr, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
    }
    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : 3地址mesh模式设置MAC头地址
**************************************************************************** */
static hi_u32 hmac_tx_mesh_set_addresses(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user,
    const hmac_tx_ctl_stru *tx_ctl, mac_ieee80211_frame_addr4_stru *hdr,
    const hmac_set_addresses_info_stru *set_addresses_info)
{
    if (hmac_user->base_user->is_mesh_user == HI_TRUE) {
        /* From DS标识位设置 */
        mac_hdr_set_from_ds((hi_u8 *)hdr, 0);
        /* to DS标识位设置 */
        mac_hdr_set_to_ds((hi_u8 *)hdr, 0);
        /* Set Address3 field in the WLAN Header with the BSSID */
        if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
        /* AMSDU情况，地址2填写BSSID */
        if (tx_ctl->is_amsdu) {
            /* Set Address3 field in the WLAN Header with the BSSID */
            if (memcpy_s(hdr->auc_address2, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) !=
                EOK) {
                return HI_FAIL;
            }
        } else {
            /* Set Address3 field in the WLAN Header with the source address */
            if (memcpy_s(hdr->auc_address2, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_saddr, WLAN_MAC_ADDR_LEN) !=
                EOK) {
                return HI_FAIL;
            }
        }
    } else {
        /* From DS标识位设置 */
        mac_hdr_set_from_ds((hi_u8 *)hdr, 1);
        /* to DS标识位设置 */
        mac_hdr_set_to_ds((hi_u8 *)hdr, 0);
        /* Set Address2 field in the WLAN Header with the BSSID */
        if (memcpy_s(hdr->auc_address2, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
        /* AMSDU情况，地址3填写BSSID */
        if (tx_ctl->is_amsdu) {
            /* Set Address3 field in the WLAN Header with the BSSID */
            if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) !=
                EOK) {
                return HI_FAIL;
            }
        } else {
            /* Set Address3 field in the WLAN Header with the source address */
            if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_saddr, WLAN_MAC_ADDR_LEN) !=
                EOK) {
                return HI_FAIL;
            }
        }
    }

    /* Set Address1 field in the WLAN Header with destination address */
    if (memcpy_s(hdr->auc_address1, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_daddr, WLAN_MAC_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : sta模式设置MAC头地址
**************************************************************************** */
static hi_u32 hmac_tx_sta_set_addresses(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user,
    const hmac_tx_ctl_stru *tx_ctl, mac_ieee80211_frame_addr4_stru *hdr,
    const hmac_set_addresses_info_stru *set_addresses_info)
{
    /* From DS标识位设置 */
    mac_hdr_set_from_ds((hi_u8 *)hdr, 0);
    /* to DS标识位设置 */
    mac_hdr_set_to_ds((hi_u8 *)hdr, 1);
    /* Set Address1 field in the WLAN Header with BSSID */
    if (memcpy_s(hdr->auc_address1, WLAN_MAC_ADDR_LEN,
                 hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }
    if (set_addresses_info->us_ether_type == hi_swap_byteorder_16(ETHER_LLTD_TYPE)) {
        /* Set Address2 field in the WLAN Header with the source address */
        if (memcpy_s(hdr->auc_address2, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_saddr, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
    } else {
        /* Set Address2 field in the WLAN Header with the source address */
        if (memcpy_s(hdr->auc_address2, WLAN_MAC_ADDR_LEN,
            hmac_vap->base_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
    }
    if (tx_ctl->is_amsdu) /* AMSDU情况，地址3填写BSSID */ {
        /* Set Address3 field in the WLAN Header with the BSSID */
        if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN,
            hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
    } else {
        /* Set Address3 field in the WLAN Header with the destination address */
        if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_daddr, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 4地址wds模式设置MAC头地址
**************************************************************************** */
static hi_u32 hmac_tx_wds_set_addresses(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user,
    const hmac_tx_ctl_stru *tx_ctl, mac_ieee80211_frame_addr4_stru *hdr,
    const hmac_set_addresses_info_stru *set_addresses_info)
{
    /* TO DS标识位设置 */
    mac_hdr_set_to_ds((hi_u8 *)hdr, 1);
    /* From DS标识位设置 */
    mac_hdr_set_from_ds((hi_u8 *)hdr, 1);
    /* 地址1是 RA */
    if (memcpy_s(hdr->auc_address1, WLAN_MAC_ADDR_LEN,
                 hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }
    /* 地址2是 TA (当前只有BSSID) */
    if (memcpy_s(hdr->auc_address2, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }

    /* AMSDU情况，地址3和地址4填写BSSID */
    if (tx_ctl->is_amsdu) {
        /* 地址3是 BSSID */
        if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }

        /* 地址4也是 BSSID */
        if (memcpy_s(hdr->auc_address4, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
    } else {
        /* 地址3是 DA */
        if (memcpy_s(hdr->auc_address3, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_daddr, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
        /* 地址4是 SA */
        if (memcpy_s(hdr->auc_address4, WLAN_MAC_ADDR_LEN, set_addresses_info->puc_saddr, WLAN_MAC_ADDR_LEN) != EOK) {
            return HI_FAIL;
        }
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置帧地址控制
 修改历史      :
  1.日    期   : 2012年11月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_tx_set_addresses(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user,
    const hmac_tx_ctl_stru *tx_ctl, mac_ieee80211_frame_addr4_stru *hdr,
    const hmac_set_addresses_info_stru *set_addresses_info)
{
    /* 分片号置成0，后续分片特性需要重新赋值 */
    hdr->frag_num    = 0;
    hdr->seq_num     = 0;

    /* From AP */
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) && (!(tx_ctl->use_4_addr))) {
        return hmac_tx_ap_set_addresses(hmac_vap, tx_ctl, hdr, set_addresses_info);
#ifdef _PRE_WLAN_FEATURE_MESH
    } else if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) && (!(tx_ctl->use_4_addr))) {
        return hmac_tx_mesh_set_addresses(hmac_vap, hmac_user, tx_ctl, hdr, set_addresses_info);
#endif
    } else if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        return hmac_tx_sta_set_addresses(hmac_vap, hmac_user, tx_ctl, hdr, set_addresses_info);
    } else { /* WDS */
        return hmac_tx_wds_set_addresses(hmac_vap, hmac_user, tx_ctl, hdr, set_addresses_info);
    }
}

/* ****************************************************************************
 函 数 名  : hmac_tx_encap
 功能描述  : 802.11帧头封装 AP模式
 输入参数  : pst_vap－vap结构体
             pst_user－用户结构体
             pst_buf－BUF结构体
 输出参数  : pst_ret_hdr－返回的头部
 返 回 值  : HI_NULL 或者 802.11帧头指针
 调用函数  : AMSDU模块以及本文件
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
/* 编程规范规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 帧封装函数，功能聚合完整，不建议拆分 */
hi_u32 hmac_tx_encap(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf)
{
    mac_ieee80211_qos_frame_addr4_stru       *hdr = HI_NULL;             /* 802.11头 */
    mac_ieee80211_qos_htc_frame_addr4_stru   *hdr_max = HI_NULL;         /* 最大的802.11头，申请空间使用 */
    hi_u32                                   qos = HMAC_TX_BSS_NOQOS;
    hi_u16                                   us_ether_type = 0;
    hi_u8                                    auc_saddr[ETHER_ADDR_LEN];   /* 原地址指针 */
    hi_u8                                    auc_daddr[ETHER_ADDR_LEN];   /* 目的地址指针 */
    hi_u32                                   ret;
    hmac_set_addresses_info_stru             set_addresses_info;

    /* 获取CB */
    hmac_tx_ctl_stru                         *tx_ctl = (hmac_tx_ctl_stru *)(oal_netbuf_cb(netbuf));

    /* 增加长度校验 防止上层下发长度异常的报文 */
    if (oal_netbuf_len(netbuf) < sizeof(mac_ether_header_stru)) {
        oam_error_log1(0, 0, "{hmac_tx_encap::netbuff len is invalid: %d!}", oal_netbuf_len(netbuf));
        return HI_FAIL;
    }
    /* 获取以太网头, 原地址，目的地址, 以太网类型 */
    mac_ether_header_stru *ether_hdr = (mac_ether_header_stru *)oal_netbuf_data(netbuf);
    if (memcpy_s((hi_u8 *)auc_daddr, ETHER_ADDR_LEN, ether_hdr->auc_ether_dhost, ETHER_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }
    if (memcpy_s((hi_u8 *)auc_saddr, ETHER_ADDR_LEN, ether_hdr->auc_ether_shost, ETHER_ADDR_LEN) != EOK) {
        return HI_FAIL;
    }

    /* 如果skb中data指针前预留的空间大于802.11 mac head len，则不需要格外申请内存存放802.11头 */
    if (oal_netbuf_headroom(netbuf) >= MAC_80211_QOS_HTC_4ADDR_FRAME_LEN) {
        tx_ctl->mac_head_type = 1; /* 指示mac头部在skb中 */
    } else {
        /* 申请最大的80211头 */
        hdr_max = (mac_ieee80211_qos_htc_frame_addr4_stru *)oal_memalloc(MAC_80211_QOS_HTC_4ADDR_FRAME_LEN);
        if (oal_unlikely(hdr_max == HI_NULL)) {
            oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_TX, "{hmac_tx_encap::pst_hdr null.}");
            return HI_ERR_CODE_PTR_NULL;
        }

        hdr = (mac_ieee80211_qos_frame_addr4_stru *)hdr_max;
        tx_ctl->mac_head_type = 0; /* 指示mac头部不在skb中，申请了额外内存存放的 */
    }

    /* 非amsdu帧 */
    if (tx_ctl->is_amsdu == HI_FALSE) {
        us_ether_type = ether_hdr->us_ether_type;
    } else {
        /* 如果是AMSDU的第一个子帧，需要从snap头中获取以太网类型，如果是以太网帧，可以
           直接从以太网头中获取 */
        mac_llc_snap_stru *snap_hdr = (mac_llc_snap_stru *)((hi_u8 *)ether_hdr + ETHER_HDR_LEN);
        us_ether_type = snap_hdr->us_ether_type;
    }

    /* 非组播帧，获取用户的QOS能力位信息 */
    if (tx_ctl->ismcast == HI_FALSE) {
        /* 根据用户结构体的cap_info，判断是否是QOS站点 */
        qos = hmac_user->base_user->cap_info.qos;
        tx_ctl->is_qosdata = hmac_user->base_user->cap_info.qos;
    }

    /* 对于LAN to WLAN的非AMSDU聚合帧，填充LLC SNAP头 */
    if (tx_ctl->is_amsdu == HI_FALSE) {
        mac_set_snap(netbuf, us_ether_type, (ETHER_HDR_LEN - SNAP_LLC_FRAME_LEN));
        /* 更新frame长度 */
        tx_ctl->us_mpdu_len = (hi_u16)oal_netbuf_len(netbuf);

        /* 非amsdu聚合帧，记录mpdu字节数，不包括snap */
        tx_ctl->us_mpdu_bytes = (hi_u16)(tx_ctl->us_mpdu_len - SNAP_LLC_FRAME_LEN);
    }

    tx_ctl->frame_header_length = hmac_get_frame_header_len(qos, tx_ctl);

    /* mac头部在skb中时，netbuf的data指针指向mac头。但是mac_set_snap函数中已经将data指针指向了llc头。
       因此这里要重新push到mac头。  */
    if (tx_ctl->mac_head_type == 1) {
        oal_netbuf_push(netbuf, tx_ctl->frame_header_length);
        hdr = (mac_ieee80211_qos_frame_addr4_stru *)oal_netbuf_data(netbuf);
    }

    /* 设置帧控制 */
    hmac_tx_set_frame_ctrl(qos, tx_ctl, hdr);

    /* 设置地址 */
    set_addresses_info.puc_saddr = auc_saddr;
    set_addresses_info.puc_daddr = auc_daddr;
    set_addresses_info.us_ether_type = us_ether_type;
    ret = hmac_tx_set_addresses(hmac_vap, hmac_user, tx_ctl, (mac_ieee80211_frame_addr4_stru *)hdr,
        &set_addresses_info);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        if (tx_ctl->mac_head_type == 0) {
            oal_free(hdr_max);
        }
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_TX,
            "{hmac_tx_encap::hmac_tx_set_addresses failed[%d].}", ret);
        return ret;
    }

    /* 挂接802.11头 */
    tx_ctl->frame_header = (mac_ieee80211_frame_stru *)hdr;
    /* 分片处理 */
    if (HI_TRUE == hmac_tx_is_need_frag(hmac_vap, hmac_user, netbuf, tx_ctl)) {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
        hmac_nonqos_frame_set_sn(tx_ctl);
#endif
        ret = hmac_frag_process_proc(hmac_vap, hmac_user, netbuf, tx_ctl);
    }

    return ret;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_mpdu_process
 功能描述  : 单个MPDU处理函数
 输入参数  : pst_event－事件结构体
             pst_vap－vap结构体
             pst_buf－BUF结构体
             pst_tx_ctl－CB结构体
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hmac_tx_return_type_enum_uint8 hmac_tx_lan_mpdu_process_sta(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf,
    hmac_tx_ctl_stru *tx_ctl)
{
    hmac_user_stru                  *hmac_user = HI_NULL;      /* 目标STA结构体 */
    mac_ether_header_stru           *ether_hdr = HI_NULL; /* 以太网头 */
    hi_u32                           ret;
    hmac_tx_return_type_enum_uint8   hmac_tx_ret;
    hi_u8                            user_idx;
    hi_u8                           *puc_ether_payload = HI_NULL;

    ether_hdr = (mac_ether_header_stru *)oal_netbuf_data(netbuf);
    tx_ctl->tx_vap_index = hmac_vap->base_vap->vap_id;

    user_idx = hmac_vap->base_vap->assoc_vap_id;

    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if (hmac_user == HI_NULL) {
        return HMAC_TX_DROP_USER_NULL;
    }

    if (hi_swap_byteorder_16(ETHER_TYPE_ARP) == ether_hdr->us_ether_type) {
        ether_hdr++;
        puc_ether_payload = (hi_u8 *)ether_hdr;
        /* The source MAC address is modified only if the packet is an   */
        /* ARP Request or a Response. The appropriate bytes are checked. */
        /* Type field (2 bytes): ARP Request (1) or an ARP Response (2)  */
        if ((puc_ether_payload[6] == 0x00) &&                                 /* 6 元素索引 */
            (puc_ether_payload[7] == 0x02 || puc_ether_payload[7] == 0x01)) { /* 7 元素索引 */
            /* Set Address2 field in the WLAN Header with source address */
            if (memcpy_s(puc_ether_payload + 8, WLAN_MAC_ADDR_LEN, /* 8 偏移量 */
                hmac_vap->base_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
                return HMAC_TX_DROP_80211_ENCAP_FAIL;
            }
        }
    }

    tx_ctl->us_tx_user_idx = user_idx;

    hmac_tx_ret = hmac_tx_ucast_process(hmac_vap, netbuf, hmac_user, tx_ctl);
    if (oal_unlikely(hmac_tx_ret != HMAC_TX_PASS)) {
        return hmac_tx_ret;
    }

    /* 封装802.11头 */
    ret = hmac_tx_encap(hmac_vap, hmac_user, netbuf);
    if (oal_unlikely((ret != HI_SUCCESS))) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_TX,
            "{hmac_tx_lan_mpdu_process_sta::hmac_tx_encap failed[%d].}", ret);
        return HMAC_TX_DROP_80211_ENCAP_FAIL;
    }

    return HMAC_TX_PASS;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_mpdu_process
 功能描述  : 单个MPDU处理函数
 输入参数  : pst_event－事件结构体
             pst_vap－vap结构体
             pst_buf－BUF结构体
             pst_tx_ctl－CB结构体
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hmac_tx_return_type_enum_uint8 hmac_tx_lan_mpdu_process_ap(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf,
    hmac_tx_ctl_stru *tx_ctl)
{
    hmac_user_stru *hmac_user = HI_NULL;      /* 目标STA结构体 */
    hi_u8           user_idx = 0;

    /* 判断是组播或单播,对于lan to wlan的单播帧，返回以太网地址 */
    mac_ether_header_stru *ether_hdr = (mac_ether_header_stru *)oal_netbuf_data(netbuf);
    hi_u8                 *mac_addr = ether_hdr->auc_ether_dhost; /* 目的地址 */

    /* 单播数据帧 */
    if (oal_likely(!ether_is_multicast(mac_addr))) {
        hi_u32 ret = mac_vap_find_user_by_macaddr(hmac_vap->base_vap, mac_addr, ETHER_ADDR_LEN, &user_idx);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            oam_info_log3(hmac_vap->base_vap->vap_id, OAM_SF_TX,
                "{hmac_tx_lan_mpdu_process_ap::hmac_tx_find_user failed xx:xx:xx:%2x:%2x:%2x}", mac_addr[3],
                mac_addr[4], mac_addr[5]); /* 3 4 5 元素索引 */
            return HMAC_TX_DROP_USER_UNKNOWN;
        }

        /* 转成HMAC的USER结构体 */
        hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
        if (oal_unlikely((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL))) {
            return HMAC_TX_DROP_USER_NULL;
        }

        /* 用户状态判断 */
        if (oal_unlikely(hmac_user->base_user->user_asoc_state != MAC_USER_STATE_ASSOC)) {
            return HMAC_TX_DROP_USER_INACTIVE;
        }

        /* 目标user指针 */
        tx_ctl->us_tx_user_idx = user_idx;

        hmac_tx_return_type_enum_uint8 hmac_tx_ret = hmac_tx_ucast_process(hmac_vap, netbuf, hmac_user, tx_ctl);
        if (oal_unlikely(hmac_tx_ret != HMAC_TX_PASS)) {
            return hmac_tx_ret;
        }
    } else { /* 组播 or 广播 */
        /* 设置组播标识位 */
        tx_ctl->ismcast = HI_TRUE;

        /* 更新ACK策略 */
        tx_ctl->ack_policy = WLAN_TX_NO_ACK;

        /* 获取组播用户 */
        hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(hmac_vap->base_vap->multi_user_idx);
        if (oal_unlikely(hmac_user == HI_NULL)) {
            oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_TX,
                "{hmac_tx_lan_mpdu_process_ap::get multi user failed[%d].}", hmac_vap->base_vap->multi_user_idx);
            return HMAC_TX_DROP_MUSER_NULL;
        }

        tx_ctl->us_tx_user_idx = hmac_vap->base_vap->multi_user_idx;
        tx_ctl->tid  = WLAN_TIDNO_BCAST;
        tx_ctl->ac   = wlan_wme_tid_to_ac(tx_ctl->tid);
    }

    /* 封装802.11头 */
    hi_u32 rst = hmac_tx_encap(hmac_vap, hmac_user, netbuf);
    if (oal_unlikely((rst != HI_SUCCESS))) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_TX,
            "{hmac_tx_lan_mpdu_process_ap::hmac_tx_encap failed[%d].}", rst);
        return HMAC_TX_DROP_80211_ENCAP_FAIL;
    }

    return HMAC_TX_PASS;
}

static hi_u32 hmac_tx_lan_to_wlan_no_tcp_opt_vap(const mac_vap_stru *mac_vap, oal_netbuf_stru *netbuf,
    hmac_vap_stru *hmac_vap, hmac_tx_ctl_stru **tx_ctl_ptr, hmac_tx_return_type_enum_uint8 *hmac_tx_ret)
{
    hmac_tx_ctl_stru *tx_ctl = *tx_ctl_ptr;
    if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        /*  处理当前 MPDU */
        if (mac_vap->mib_info->wlan_mib_sta_config.dot11_qos_option_implemented == HI_FALSE) {
            tx_ctl->ac = WLAN_WME_AC_VO; /* AP模式 关WMM 入VO队列 */
            tx_ctl->tid = wlan_wme_ac_to_tid(tx_ctl->ac);
        }

        *hmac_tx_ret = hmac_tx_lan_mpdu_process_ap(hmac_vap, netbuf, tx_ctl);
    } else if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        /* 处理当前MPDU */
        tx_ctl->ac = WLAN_WME_AC_VO; /* STA模式 非qos帧入VO队列 */
        tx_ctl->tid = wlan_wme_ac_to_tid(tx_ctl->ac);

        *hmac_tx_ret = hmac_tx_lan_mpdu_process_sta(hmac_vap, netbuf, tx_ctl);
#ifdef _PRE_WLAN_FEATURE_WAPI
        if (*hmac_tx_ret == HMAC_TX_PASS) {
            hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(mac_vap->assoc_vap_id);
            if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
                oam_warning_log1(0, 0, "hmac_tx_lan_to_wlan_no_tcp_opt_vap::usrid==%u !}", mac_vap->assoc_vap_id);
                return HMAC_TX_DROP_USER_NULL;
            }

            /* 获取wapi对象 组播/单播 */
            mac_ieee80211_frame_stru *mac_hdr = ((hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf))->frame_header;
            hi_bool pairwise = !ether_is_multicast(mac_hdr->auc_address1);
            hmac_wapi_stru *wapi = hmac_user_get_wapi_ptr(mac_vap, pairwise, (hi_u8)hmac_user->base_user->us_assoc_id);
            if (wapi == HI_NULL) {
                oam_error_log0(0, 0, "{hmac_tx_lan_to_wlan_no_tcp_opt_vap::pst_wapi null.}");
                return HI_FAIL;
            }
            if ((wapi_is_port_valid(wapi) == HI_TRUE) && (wapi->wapi_netbuff_txhandle != HI_NULL)) {
                netbuf = wapi->wapi_netbuff_txhandle(wapi, netbuf);
                /*  由于wapi可能修改netbuff，此处需要重新获取一下cb */
                tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
                *tx_ctl_ptr = tx_ctl;
            }
        }

#endif /* #ifdef _PRE_WLAN_FEATURE_WAPI */
    }

    return HI_SUCCESS;
}

static hi_u32 hmac_tx_lan_to_wlan_no_tcp_opt_vap_mode(mac_vap_stru *mac_vap, const hmac_vap_stru *hmac_vap)
{
    /* VAP模式判断 */
    if (mac_vap->vap_mode != WLAN_VAP_MODE_BSS_AP && mac_vap->vap_mode != WLAN_VAP_MODE_BSS_STA
#ifdef _PRE_WLAN_FEATURE_MESH
        && (mac_vap->vap_mode != WLAN_VAP_MODE_MESH)
#endif
    ) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan_no_tcp_opt_vap_mode::en_vap_mode=%d.}",
            mac_vap->vap_mode);
        return HI_ERR_CODE_CONFIG_UNSUPPORT;
    }

    /* 如果关联用户数量为0，则丢弃报文 */
    if (oal_unlikely(hmac_vap->base_vap->user_nums == 0)) {
        return HI_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    /* 发数据，只发一次，避免反复申请tx描述符地址 */
    if (mac_vap->al_tx_flag == HI_SWITCH_ON) {
        if (mac_vap->first_run != HI_FALSE) {
            return HI_SUCCESS;
        }
        mac_vap_set_al_tx_first_run(mac_vap, HI_TRUE);
    }
#endif

    return HI_CONTINUE;
}

static hi_u32 hmac_tx_lan_to_wlan_no_tcp_opt_to_dmac(const mac_vap_stru *mac_vap, oal_netbuf_stru *netbuf,
    const hmac_tx_ctl_stru *tx_ctl, hmac_tx_return_type_enum_uint8 hmac_tx_ret)
{
    hi_u32 ret = (hi_u32)hmac_tx_ret;

    if (oal_likely(hmac_tx_ret == HMAC_TX_PASS)) {
        /* 抛事件，传给DMAC */
        frw_event_mem_stru *event_mem = frw_event_alloc(sizeof(dmac_tx_event_stru));
        if (oal_unlikely(event_mem == HI_NULL)) {
            oam_error_log0(0, OAM_SF_TX, "{hmac_tx_lan_to_wlan_no_tcp_opt_to_dmac::frw_event_alloc failed.}");
            return HI_ERR_CODE_ALLOC_MEM_FAIL;
        }

        frw_event_stru *event = (frw_event_stru *)event_mem->puc_data; /* 事件结构体 */

        /* 填写事件头 */
        frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_DRX, DMAC_TX_HOST_DRX, sizeof(dmac_tx_event_stru),
            FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

        dmac_tx_event_stru *dtx_stru = (dmac_tx_event_stru *)event->auc_event_data;
        dtx_stru->netbuf = netbuf;
        dtx_stru->us_frame_len = tx_ctl->us_mpdu_len;

        /* 调度事件 */
        ret = hcc_hmac_tx_data_event(event_mem, netbuf, HI_FALSE);
        /* 释放事件 */
        frw_event_free(event_mem);
    } else if ((oal_unlikely(hmac_tx_ret == HMAC_TX_BUFF)) || (hmac_tx_ret == HMAC_TX_DONE)) {
        hmac_free_netbuf_list(netbuf);
        ret = HI_SUCCESS;
    }

    return ret;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_wlan_to_wlan_ap
 功能描述  : (1)WLAN TO WLAN流程tx入口函数，接收rx流程抛过来的netbuf或者netbuf链，
             其中每一个netbuf里面有一个MSDU，每一个MSDU的内容是一个以太网格式的
             帧。如果是netbuf链，第一个netbuf的prev指针为空，最后一个netbuf的
             next指针为空。
             (2)函数循环调用LAN TO WLAN的入口函数来处理每一个MSDU，这样就把
             WLAN TO WLAN流程统一到了LAN TO WLAN流程
 输入参数  : pst_event_mem－事件内存块
 输出参数  : 无
 返 回 值  : HI_SUCCESS或其它错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_tx_wlan_to_wlan_ap(oal_mem_stru *event_mem)
{
    frw_event_stru         *event = HI_NULL;        /* 事件结构体 */
    mac_vap_stru           *mac_vap = HI_NULL;
    oal_netbuf_stru        *netbuf = HI_NULL;          /* 从netbuf链上取下来的指向netbuf的指针 */
    oal_netbuf_stru        *buf_tmp = HI_NULL;      /* 暂存netbuf指针，用于while循环 */
    hmac_tx_ctl_stru       *tx_ctl = HI_NULL;
    hi_u32                  ret;
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_proc_dev_ready_slp_event::event_mem is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取事件 */
    event = (frw_event_stru *)event_mem->puc_data;

    /* 获取PAYLOAD中的netbuf链 */
    netbuf = (oal_netbuf_stru *)(*((uintptr_t *)(event->auc_event_data)));

    ret = hmac_tx_get_mac_vap(event->event_hdr.vap_id, &mac_vap);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_TX, "{hmac_tx_wlan_to_wlan_ap::hmac_tx_get_mac_vap failed[%d].}", ret);
        hmac_free_netbuf_list(netbuf);
        return ret;
    }

    /* 循环处理每一个netbuf，按照以太网帧的方式处理 */
    while (netbuf != HI_NULL) {
        buf_tmp = oal_netbuf_next(netbuf);

        set_oal_netbuf_next(netbuf, HI_NULL);
        set_oal_netbuf_prev(netbuf, HI_NULL);

        /* 由于此netbuf来自接收流程，是从内存池申请的，而以太网过来的netbuf是从
           操作系统申请的，二者的释放方式不一样，后续要通过事件类型字段来选择正确
           的释放方式
        */
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

/* ****************************************************************************
 功能描述  : hmac AP模式 处理HOST DRX事件，注册到事件管理模块中
             PAYLOAD是一个NETBUF
 输入参数  : pst_vap: vap指针
             pst_buf: netbuf指针
 返 回 值  : HI_SUCCESS或其它错误码
**************************************************************************** */
hi_u32 hmac_tx_lan_to_wlan(mac_vap_stru *mac_vap, oal_netbuf_stru *netbuf)
{
    hmac_tx_return_type_enum_uint8 hmac_tx_ret = HMAC_TX_PASS;

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id); /* VAP结构体 */
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hi_u32 ret = hmac_tx_lan_to_wlan_no_tcp_opt_vap_mode(mac_vap, hmac_vap);
    if (ret != HI_CONTINUE) {
        return ret;
    }

    /* 初始化CB tx rx字段 , CB字段在前面已经被清零， 在这里不需要重复对某些字段赋零值 */
    hmac_tx_ctl_stru* tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
    tx_ctl->mpdu_num               = 1;
    tx_ctl->netbuf_num             = 1;
    tx_ctl->frame_type             = WLAN_DATA_BASICTYPE;
    tx_ctl->is_probe_data          = DMAC_USER_ALG_NON_PROBE;
    tx_ctl->ack_policy             = WLAN_TX_NORMAL_ACK;
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    /* 变量初始化赋值 */
    tx_ctl->ack_policy             = hmac_vap->ack_policy;
#endif
    tx_ctl->tx_vap_index           = mac_vap->vap_id;
    tx_ctl->us_tx_user_idx         = MAC_INVALID_USER_ID;
    tx_ctl->ac                     = WLAN_WME_AC_BE;                  /* 初始化入BE队列 */

    /* 由于LAN TO WLAN和WLAN TO WLAN的netbuf都走这个函数，为了区分，需要先判断
       到底是哪里来的netbuf然后再对CB的事件类型字段赋值
    */
    if (tx_ctl->event_type != FRW_EVENT_TYPE_WLAN_DTX) {
        tx_ctl->event_type          = FRW_EVENT_TYPE_HOST_DRX;
        tx_ctl->event_sub_type      = DMAC_TX_HOST_DRX;
    }

    /* 此处数据可能从内核而来，也有可能由dev报上来再通过空口转出去，注意一下 */
    hi_u8 data_type = mac_get_data_type_from_8023((hi_u8 *)oal_netbuf_data(netbuf), MAC_NETBUFF_PAYLOAD_ETH);
    /* 维测，输出一个关键帧打印 */
    if ((data_type <= MAC_DATA_DHCP_ACK) || (data_type == MAC_DATA_ARP_REQ) || (data_type == MAC_DATA_ARP_RSP) ||
        (data_type == MAC_DATA_EAPOL)) {
        tx_ctl->is_vipframe = HI_TRUE;
        oam_warning_log2(mac_vap->vap_id, OAM_SF_TX,
            "{hmac_tx_lan_to_wlan:type:%u,len:%u}[0~3:dhcp 4:arp_req 5:arp_rsp 6:eapol]", data_type,
            oal_netbuf_len(netbuf));
    }
#ifdef _PRE_WLAN_FEATURE_MESH
    if (data_type == MAC_DATA_RPL) {
        oam_warning_log1(0, OAM_SF_TX, "{hmac_tx_lan_to_wlan:Mesh rpl msg,len=%u}", oal_netbuf_len(netbuf));
    }
#endif

    ret = hmac_tx_lan_to_wlan_no_tcp_opt_vap(mac_vap, netbuf, hmac_vap, &tx_ctl, &hmac_tx_ret);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return hmac_tx_lan_to_wlan_no_tcp_opt_to_dmac(mac_vap, netbuf, tx_ctl, hmac_tx_ret);
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 函 数 名  : hmac_unicast_data_tx_event_info
 功能描述  : 处理dmac上报单播数据帧发送信息,上报WAL
 输入参数  : [1]pst_mac_vap
             [2]uc_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_unicast_data_tx_event_info(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hi_unref_param(len);

    return hmac_send_event_to_host(mac_vap, puc_param, sizeof(dmac_tx_info_report_stru),
        HMAC_HOST_CTX_EVENT_SUB_TYPE_TX_DATA_INFO);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
