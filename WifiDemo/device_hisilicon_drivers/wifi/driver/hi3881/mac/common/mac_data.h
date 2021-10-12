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

#ifndef __MAC_DATA_H__
#define __MAC_DATA_H__

#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "mac_user.h"
#include "mac_regdomain.h"
#include "wlan_mib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  枚举定义
**************************************************************************** */
typedef enum {
    MAC_DATA_DHCP_DISCOVER = 0,
    MAC_DATA_DHCP_OFFER,
    MAC_DATA_DHCP_REQ,
    MAC_DATA_DHCP_ACK,
    MAC_DATA_ARP_REQ,
    MAC_DATA_ARP_RSP,
    MAC_DATA_EAPOL,
    MAC_DATA_DHCPV6,
    MAC_DATA_VIP = MAC_DATA_DHCPV6, /* MAC_DATA_VIP == MAC_DATA_DHCPV6, 提高关键帧判断效率，减少mips */
    MAC_DATA_ND,
    MAC_DATA_TDLS,
    MAC_DATA_PPPOE,
    MAC_DATA_WAPI,
    MAC_DATA_VLAN,
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    MAC_DATA_RPL,
    MAC_DATA_6LO,
#endif
    MAC_DATA_BUTT
} mac_data_type_enum;
typedef hi_u8 mac_data_type_enum_uint8;

typedef enum {
    MAC_NETBUFF_PAYLOAD_ETH = 0,
    MAC_NETBUFF_PAYLOAD_SNAP,

    MAC_NETBUFF_PAYLOAD_BUTT
} mac_netbuff_payload_type;
typedef hi_u8 mac_netbuff_payload_type_uint8;

/* ****************************************************************************
  inline函数定义
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 获取arp帧的类型：request/response(暂时不考虑rarp!)
 输入参数  : arp ie头
 返 回 值  : 驱动私定义的关键帧类型
 修改历史      :
  1.日    期   : 2016年1月25日
**************************************************************************** */
static inline mac_data_type_enum_uint8 mac_get_arp_type_by_arphdr(const oal_eth_arphdr_stru *rx_arp_hdr)
{
    if (oal_net2host_short(rx_arp_hdr->us_ar_op) == MAC_ARP_REQUEST) {
        return MAC_DATA_ARP_REQ;
    } else if (oal_net2host_short(rx_arp_hdr->us_ar_op) == MAC_ARP_RESPONSE) {
        return MAC_DATA_ARP_RSP;
    }
    return MAC_DATA_BUTT;
}

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* ****************************************************************************
 功能描述  : 判断是否是RPL控制帧
 修改历史      :
  1.日    期   : 2019年5月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_is_rpl(oal_ipv6hdr_stru *ipv6hdr)
{
    if (ipv6hdr->nexthdr == OAL_IPPROTO_ICMPV6) {
        oal_icmp6hdr_stru *icmp6hdr = HI_NULL;
        icmp6hdr = (oal_icmp6hdr_stru *)(ipv6hdr + 1);
        if (icmp6hdr->icmp6_type == MAC_RPL_TYPE) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}
#endif

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_u8 mac_get_data_type(const oal_dev_netbuf_stru *dev_netbuf);
hi_u8 mac_get_data_type_from_80211(const oal_netbuf_stru *netbuf, hi_u16 us_mac_hdr_len);
hi_u8 mac_get_data_type_from_8023(const hi_u8 *puc_frame_hdr, mac_netbuff_payload_type hdr_type);
hi_u8 mac_is_dhcp_port(mac_ip_header_stru *ip_hdr);
hi_u8 mac_is_nd(oal_ipv6hdr_stru *ipv6hdr);
hi_u8 mac_is_eapol_key_ptk(mac_eapol_header_stru *eapol_header);
hi_u8 *mac_dhcp_get_type(hi_u8 *pos, const hi_u8 *packet_end, hi_u8 type);
hi_u8 mac_is_dhcp6(oal_ipv6hdr_stru *ipv6hdr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __MAC_DATA_H__ */
