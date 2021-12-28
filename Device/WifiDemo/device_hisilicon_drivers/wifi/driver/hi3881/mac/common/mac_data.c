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
  头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "wlan_types.h"
#include "mac_vap.h"
#include "mac_device.h"
#include "mac_data.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 从带mac头的80211帧中获取以太类型
 修改历史      :
  1.日    期   : 2015年7月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_get_data_type_from_80211(const oal_netbuf_stru *netbuf, hi_u16 us_mac_hdr_len)
{
    mac_llc_snap_stru *snap = HI_NULL;

    if (netbuf == HI_NULL) {
        return MAC_DATA_BUTT;
    }
    snap = (mac_llc_snap_stru *)(oal_netbuf_data(netbuf) + us_mac_hdr_len);
    return mac_get_data_type_from_8023((hi_u8 *)snap, MAC_NETBUFF_PAYLOAD_SNAP);
}

/* ****************************************************************************
 功能描述  : 判断该帧是否为4 次握手的EAPOL KEY 单播密钥协商帧
 输入参数  : mac_eapol_header_stru  *pst_eapol_header
 返 回 值  : hi_u8
 修改历史      :
  1.日    期   : 2015年8月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_is_eapol_key_ptk(mac_eapol_header_stru *eapol_header)
{
    mac_eapol_key_stru *key = HI_NULL;

    if (IEEE802_1X_TYPE_EAPOL_KEY == eapol_header->type) {
        if ((hi_u16)(oal_net2host_short(eapol_header->us_length)) >= (hi_u16)sizeof(mac_eapol_key_stru)) {
            key = (mac_eapol_key_stru *)(eapol_header + 1);
            if (key->auc_key_info[1] & WPA_KEY_INFO_KEY_TYPE) {
                return HI_TRUE;
            }
        }
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 判断是否是dhcp帧 port
 修改历史      :
  1.日    期   : 2014年6月27日
    作    者   : HiSilicon
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_is_dhcp_port(mac_ip_header_stru *ip_hdr)
{
    udp_hdr_stru *udp_hdr = HI_NULL;
    if ((ip_hdr->protocol == MAC_UDP_PROTOCAL) && ((oal_net2host_short(ip_hdr->us_frag_off) & 0x1FFF) == 0)) {
        udp_hdr = (udp_hdr_stru *)(ip_hdr + 1);

        if (oal_net2host_short(udp_hdr->us_des_port) == 67 || /* 67 端口为dhcp端口 */
            oal_net2host_short(udp_hdr->us_des_port) == 68) { /* 68 端口也为dhcp端口 */
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

WIFI_ROM_TEXT hi_u8 *mac_dhcp_get_type(hi_u8 *pos, const hi_u8 *packet_end, hi_u8 type)
{
    hi_u8 *opt = HI_NULL;
    if (pos == HI_NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "{mac_dhcp_get_type::pos is null.}");
        return HI_NULL;
    }

    if (packet_end == HI_NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "{mac_dhcp_get_type::packet_end is null.}");
        return HI_NULL;
    }

    /* 获取 DHCP 类型 */
    while ((pos < packet_end) && (*pos != 0xFF)) {
        opt = pos++;

        /* Padding */
        if (*opt == 0) {
            continue;
        }
        pos += *pos + 1;
        if (pos >= packet_end) {
            break;
        }
        /* Message Type */
        if ((type == *opt) && (opt[1] != 0)) {
            return opt;
        }
    }

    return HI_NULL;
}

/* ****************************************************************************
 功能描述  : 判断是否是nd帧
 修改历史      :
  1.日    期   : 2014年8月7日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_is_nd(oal_ipv6hdr_stru *ipv6hdr)
{
    oal_icmp6hdr_stru *icmp6hdr = HI_NULL;

    if (ipv6hdr->nexthdr == OAL_IPPROTO_ICMPV6) {
        icmp6hdr = (oal_icmp6hdr_stru *)(ipv6hdr + 1);
        if ((icmp6hdr->icmp6_type == MAC_ND_RSOL) || (icmp6hdr->icmp6_type == MAC_ND_RADVT) ||
            (icmp6hdr->icmp6_type == MAC_ND_NSOL) || (icmp6hdr->icmp6_type == MAC_ND_NADVT) ||
            (icmp6hdr->icmp6_type == MAC_ND_RMES)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

WIFI_ROM_TEXT static hi_u8 mac_get_ipv4_data_type(mac_ip_header_stru *ip)
{
    udp_hdr_stru *udp_hdr = HI_NULL;
    dhcp_message_stru *dhcp_message = HI_NULL;
    hi_u8 *tmp = HI_NULL;
    hi_u8 datatype = MAC_DATA_BUTT;

    if (mac_is_dhcp_port(ip)) {
        udp_hdr = (udp_hdr_stru *)(ip + 1);
        dhcp_message = (dhcp_message_stru *)(udp_hdr + 1);
        tmp = mac_dhcp_get_type(dhcp_message->options, dhcp_message->options + sizeof(dhcp_message->options),
            DHCP_OPT_MESSAGETYPE);
        if (tmp == HI_NULL) {
            datatype = MAC_DATA_BUTT;
        } else {
            if (*(tmp + 2) == DHCP_DISCOVER) { /* 偏移2 byte，获取DHCP type */
                datatype = MAC_DATA_DHCP_DISCOVER;
            } else if (*(tmp + 2) == DHCP_OFFER) { /* 偏移2 byte，获取DHCP type */
                datatype = MAC_DATA_DHCP_OFFER;
            } else if (*(tmp + 2) == DHCP_REQUEST) { /* 偏移2 byte，获取DHCP type */
                datatype = MAC_DATA_DHCP_REQ;
            } else if (*(tmp + 2) == DHCP_ACK) { /* 偏移2 byte，获取DHCP type */
                datatype = MAC_DATA_DHCP_ACK;
            } else {
                datatype = MAC_DATA_BUTT;
            }
        }
    }
    return datatype;
}

/* ****************************************************************************
 功能描述  : 判断是否是DHCP6帧
 修改历史      :
  1.日    期   : 2014年8月7日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_is_dhcp6(oal_ipv6hdr_stru *ipv6hdr)
{
    udp_hdr_stru *udp_hdr = HI_NULL;

    if (ipv6hdr->nexthdr == MAC_UDP_PROTOCAL) {
        udp_hdr = (udp_hdr_stru *)(ipv6hdr + 1);
        if (udp_hdr->us_des_port == oal_host2net_short(MAC_IPV6_UDP_DES_PORT) ||
            udp_hdr->us_des_port == oal_host2net_short(MAC_IPV6_UDP_SRC_PORT)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

WIFI_ROM_TEXT static hi_u8 mac_get_ipv6_data_type(oal_ipv6hdr_stru *ipv6)
{
    hi_u8 datatype = MAC_DATA_BUTT;

    if (mac_is_nd(ipv6)) {
        datatype = MAC_DATA_ND;
    } else if (mac_is_dhcp6(ipv6)) {
        datatype = MAC_DATA_DHCPV6;
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    } else if (mac_is_rpl(ipv6)) {
        datatype = MAC_DATA_RPL;
#endif
    }

    return datatype;
}
/* ****************************************************************************
 功能描述  : 根据数据帧(802.3)的类型，判断帧类型
 输入参数  : puc_frame_hdr: 为去除80211头的数据帧，可为以太头或snap头
             uc_hdr_type: 指针指向数据类型，用来计算获取data_type时刻的偏移
  1.日    期   : 2015年1月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_get_data_type_from_8023(const hi_u8 *puc_frame_hdr, mac_netbuff_payload_type hdr_type)
{
    const hi_u8              *puc_frame_body = HI_NULL;
    hi_u16              ether_type;
    hi_u8               datatype = MAC_DATA_BUTT;

    if (puc_frame_hdr == HI_NULL) {
        return datatype;
    }
    /* 偏移一个以太网头，取ip头 */
    if (hdr_type == MAC_NETBUFF_PAYLOAD_ETH) {
        ether_type = oal_net2host_short(((mac_ether_header_stru *)puc_frame_hdr)->us_ether_type);
        puc_frame_body = puc_frame_hdr + (hi_u16)sizeof(mac_ether_header_stru);
    } else if (hdr_type == MAC_NETBUFF_PAYLOAD_SNAP) {
        ether_type = oal_net2host_short(((mac_llc_snap_stru *)puc_frame_hdr)->us_ether_type);
        puc_frame_body = puc_frame_hdr + (hi_u16)sizeof(mac_llc_snap_stru);
    } else {
        return datatype;
    }

    switch (ether_type) {
        case ETHER_TYPE_IP:
            datatype = mac_get_ipv4_data_type((mac_ip_header_stru *)puc_frame_body);
            break;
        case ETHER_TYPE_IPV6:
            datatype = mac_get_ipv6_data_type((oal_ipv6hdr_stru *)puc_frame_body);
            break;
        case ETHER_TYPE_PAE:
            datatype = MAC_DATA_EAPOL;
            break;
        /* TDLS帧处理，建链保护，入高优先级TID队列 */
        case ETHER_TYPE_TDLS:
            datatype = MAC_DATA_TDLS;
            break;
        /* PPPOE帧处理，建链保护(发现阶段, 会话阶段)，入高优先级TID队列 */
        case ETHER_TYPE_PPP_DISC:
        case ETHER_TYPE_PPP_SES:
            datatype = MAC_DATA_PPPOE;
            break;
        case ETHER_TYPE_WAI:
            datatype = MAC_DATA_WAPI;
            break;
        case ETHER_TYPE_VLAN:
            datatype = MAC_DATA_VLAN;
            break;
        case ETHER_TYPE_ARP:
            /* 如果是ARP帧，则进入VO队列发送 */
            datatype = mac_get_arp_type_by_arphdr((oal_eth_arphdr_stru *)puc_frame_body);
            break;
        /* _PRE_WLAN_FEATURE_MESH + */
        case ETHER_TYPE_6LO:
            datatype = MAC_DATA_6LO;
            break;
        default:
            break;
    }

    return datatype;
}

/* 代码ROM段结束位置 新增ROM代码请放在SECTION中 */
#undef __WIFI_ROM_SECTION__

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
