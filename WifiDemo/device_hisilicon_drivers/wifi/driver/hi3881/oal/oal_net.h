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

#ifndef __OAL_NET_H__
#define __OAL_NET_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/in6.h>
#include <net/netlink.h>

#ifndef _PRE_HDF_LINUX
#include <net/iw_handler.h>
#include <linux/if.h>
#include <linux/netdevice.h>
#include <net/cfg80211.h>
#endif

#include <linux/etherdevice.h>
#include <linux/kobject.h>
#include "oal_mm.h"
#include "oal_netbuf.h"
#endif
#include "oal_err_wifi.h"
#include "oal_util.h"
#include "oal_schedule.h"
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "oal_netbuf.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#endif
#include "wlan_spec_1131h.h"
#include "wlan_types.h"
#include "net_device.h"
#include "hdf_wifi_cmd.h"
#include "wifi_mac80211_ops.h"
#include "eapol.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define OAL_IF_NAME_SIZE                  16

#define OAL_ASSOC_REQ_IE_OFFSET           28            /* 上报内核关联请求帧偏移量 */
#define OAL_ASSOC_RSP_IE_OFFSET           30            /* 上报内核关联响应帧偏移量 */
#define OAL_ASSOC_RSP_FIXED_OFFSET        6             /* 关联响应帧帧长FIXED PARAMETERS偏移量 */
#define OAL_MAC_ADDR_LEN                  6
#define OAL_PMKID_LEN                     16
#define OAL_WPA_KEY_LEN                   32
#define OAL_WPA_SEQ_LEN                   16
#define NETLINK_DEV_ERROR                 27
#define WLAN_DSCP_PRI_SHIFT               2
#define WLAN_IP_PRI_SHIFT                 5
#define WLAN_IPV6_PRIORITY_MASK           0x0FF00000
#define WLAN_IPV6_PRIORITY_SHIFT          20

#define LLC_UI                            0x3
#define SNAP_LLC_FRAME_LEN                8
#define SNAP_LLC_LSAP                     0xaa

#define ORG_CODE_LEN                      3
#define SNAP_RFC1042_ORGCODE_0            0x00
#define SNAP_RFC1042_ORGCODE_1            0x00
#define SNAP_RFC1042_ORGCODE_2            0x00
#define SNAP_BTEP_ORGCODE_0               0x00
#define SNAP_BTEP_ORGCODE_1               0x00
#define SNAP_BTEP_ORGCODE_2               0xf8

#define ETHER_ADDR_LEN                    MAC_ADDR_SIZE                 /* length of an Ethernet address */
#define IP6_ETHER_ADDR_LEN                16                /* efuse中ipv6 MAC地址大小 */
#define ETHER_HDR_LEN                     14
#define IP_HDR_LEN                        20

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* DHCP message types */
#define DHCP_DISCOVER                    1
#define DHCP_OFFER                       2
#define DHCP_REQUEST                     3
#define DHCP_DECLINE                     4
#define DHCP_ACK                         5
#define DHCP_NAK                         6
#define DHCP_RELEASE                     7
#define DHCP_INFORM                      8
#endif

#define WLAN_CCMP_ENCRYP_LEN              16                /* CCMP加密字节数 */
#define ether_is_multicast(_a)            (*(_a) & 0x01)    /* is address mcast? */

/* is address bcast? */
#define ether_is_broadcast(_a)            (((_a)[0] & (_a)[1] & (_a)[2] & (_a)[3] & (_a)[4] & (_a)[5]) == 0xFF)
#define WLAN_DATA_VIP_TID                 WLAN_TIDNO_BCAST

/* wiphy  */
#define IEEE80211_HT_MCS_MASK_LEN         10

/* ICMP codes for neighbour discovery messages */
#define OAL_NDISC_NEIGHBOUR_SOLICITATION    135
#define OAL_NDISC_NEIGHBOUR_ADVERTISEMENT   136

#define OAL_ND_OPT_SOURCE_LL_ADDR           1
#define OAL_ND_OPT_TARGET_LL_ADDR           2
#define OAL_IPV6_ADDR_ANY                   0x0000U
#define OAL_IPV6_ADDR_MULTICAST             0x0002U

#define OAL_IPV4_ADDR_SIZE                  4
#define OAL_IPV6_ADDR_SIZE                  16
#define OAL_IP_ADDR_MAX_SIZE                OAL_IPV6_ADDR_SIZE

/* IPv4受限广播: 255.255.255.255 , IPv4直接广播: 如192.168.10.255 */
#define oal_ipv4_is_broadcast(_a)           ((hi_u8)((_a)[3]) == 0xff)

/* IPv4多播范围: 224.0.0.0--239.255.255.255 */
#define oal_ipv4_multicast(_a)           ((hi_u8)((_a)[0]) >= 224 && ((hi_u8)((_a)[0]) <= 239))

/* IPv6组播地址: FFXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX(第一个字节全一) */
#define oal_ipv6_is_multicast(_a)           ((hi_u8)((_a)[0]) == 0xff)

/* wiphy  */
#define OAL_IEEE80211_MAX_SSID_LEN          32  /* 最大SSID长度 */

#define ICMPV6_NA_OPTION_LEN                8

/* ether type */
#define ETHER_TYPE_RARP         0x8035
#define ETHER_TYPE_PAE          0x888e  /* EAPOL PAE/802.1x */
#define ETHER_TYPE_IP           0x0800  /* IP protocol */
#define ETHER_TYPE_AARP         0x80f3  /* Appletalk AARP protocol */
#define ETHER_TYPE_IPX          0x8137  /* IPX over DIX protocol */
#define ETHER_TYPE_ARP          0x0806  /* ARP protocol */
#define ETHER_TYPE_IPV6         0x86dd  /* IPv6 */
#define ETHER_TYPE_TDLS         0x890d  /* TDLS */
#define ETHER_TYPE_VLAN         0x8100  /* VLAN TAG protocol */
#define ETHER_TYPE_WAI          0x88b4  /* WAI/WAPI */
#define ETHER_LLTD_TYPE         0x88D9  /* LLTD */
#define ETHER_ONE_X_TYPE        0x888E  /* 802.1x Authentication */
#define ETHER_TUNNEL_TYPE       0x88bd  /* 自定义tunnel协议 */
#define ETHER_TYPE_PPP_DISC     0x8863  /* PPPoE discovery messages */
#define ETHER_TYPE_PPP_SES      0x8864  /* PPPoE session messages */
#define ETHER_TYPE_6LO          0xa0ed  /* 6lowpan包头压缩 */

#define ETH_IP_ADDR_LEN              4  /* length of an Ethernet ip address */
#define ETH_SENDER_IP_ADDR_LEN       4  /* length of an Ethernet send ip address */
#define ETH_TARGET_IP_ADDR_LEN       4  /* length of an Ethernet target ip address */

#define oal_netif_running(_pst_net_dev) 0

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define OAL_EXCP_DATA_BUF_LEN 64

/* netlink 相关 */
#define oal_nlmsg_space(_len) NLMSG_SPACE(_len)
#define oal_netlink_cb(_skb) NETLINK_CB(_skb)
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define oal_smp_mb() smp_mb()
#else
#define oal_smp_mb()
#endif

typedef hi_u32 gfp_t;
#define oal_container_of(_member_ptr, _stru_type, _stru_member_name) \
    container_of(_member_ptr, _stru_type, _stru_member_name)

#define oal_netdevice_ops(_pst_dev)                         GET_NET_DEV_IF(_pst_dev)

#define oal_netdevice_last_rx(_pst_dev)                     GET_NET_DEV_LAST_RX_TIME(_pst_dev)
#define oal_netdevice_mac_addr(_pst_dev)                    GET_NET_DEV_MAC_ADDR(_pst_dev)
#define oal_netdevice_master(_pst_dev)                      GET_NET_DEV_OWNER(_pst_dev)


#if LINUX_VERSION_CODE >= kernel_version(2, 6, 34) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define oal_netdevice_qdisc(_pst_dev, pst_val) ((_pst_dev)->qdisc = (pst_val))
#else
#define oal_netdevice_qdisc(_pst_dev, pst_val)
#endif

#define oal_netdevice_wdev(_pst_dev)                        GET_NET_DEV_CFG80211_WIRELESS(_pst_dev)
#define oal_netdevice_headroom(_pst_dev)                    GET_NET_DEV_HEAD_ROOM(_pst_dev)
#define oal_netdevice_tailroom(_pst_dev)                    GET_NET_DEV_TAIL_ROOM(_pst_dev)
#define oal_netdevice_flags(_pst_dev)                       GET_NET_DEV_FLAGS(_pst_dev)
#define oal_netdevice_watchdog_timeo(_pst_dev)              GET_NET_DEV_WATCHDOG_TIME(_pst_dev)
#define oal_net_dev_priv(_pst_dev)                          GET_NET_DEV_PRIV(_pst_dev)
#define oal_net_dev_wireless_dev(_pst_dev)                  GET_NET_DEV_CFG80211_WIRELESS(_pst_dev)
#define oal_netdevice_specical_proc_priv(_pst_dev)          GET_NET_DEV_SPECIAL_PROC_PRIV(_pst_dev)
#define NL80211_RRF_NO_OFDM         (1<<0)
#define NL80211_RRF_DFS             (1<<4)
#define OAL_NL80211_MAX_NR_CIPHER_SUITES    5
#define OAL_NL80211_MAX_NR_AKM_SUITES       2
#define IEEE80211_MAX_SSID_LEN              32
#define NL80211_MAX_NR_CIPHER_SUITES        5
#define NL80211_MAX_NR_AKM_SUITES           2
#define NL80211_RRF_NO_OUTDOOR              (1<<3)
#ifndef ETH_ALEN
#define ETH_ALEN                            6
#endif
#ifndef GFP_ATOMIC
#define GFP_ATOMIC                          0
#endif

#define OAL_IFF_RUNNING             NET_DEVICE_IFF_RUNNING
#define OAL_SIOCIWFIRSTPRIV         0x8BE0

/* iw_priv参数类型OAL封装 */
#define OAL_IW_PRIV_TYPE_BYTE       0x1000       /* Char as number */
#define OAL_IW_PRIV_TYPE_CHAR       0x2000       /* Char as character */
#define OAL_IW_PRIV_TYPE_INT        0x4000       /* 32 bits integer */
#define OAL_IW_PRIV_TYPE_ADDR       0x6000       /* struct sockaddr */
#define OAL_IW_PRIV_SIZE_FIXED      0x0800       /* Variable or fixed number of args */

#define OAL_VLAN_PRIO_SHIFT         13

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define OAL_IPPROTO_UDP             IPPROTO_UDP         /* User Datagram Protocot */
#define OAL_IPPROTO_ICMPV6          IPPROTO_ICMPV6      /* ICMPv6 */
#else
#define OAL_IPPROTO_UDP             17         /* User Datagram Protocot */
#define OAL_IPPROTO_ICMPV6          58         /* ICMPv6 */
#endif
#define OAL_INIT_NET                init_net
#define OAL_THIS_MODULE             THIS_MODULE
#define OAL_IP4_ADDR                IP4_ADDR

#define HI_MAX_SCAN_CHANNELS                14
#define WLAN_SA_QUERY_TR_ID_LEN             2

#define oal_is_broadcast_ether_addr(a) (((a)[0] & (a)[1] & (a)[2] & (a)[3] & (a)[4] & (a)[5]) == 0xff)

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define OAL_EXCP_DATA_BUF_LEN               64
#endif

#define oal_ll_allocated_space(dev) \
    ((((dev)->hardHeaderLen + (dev)->neededHeadRoom + (dev)->neededTailRoom) & ~(15)) + 16)

/* 管制域相关结构体定义 */
#define mhz_to_khz(freq) ((freq) * 1000)
#define dbi_to_mbi(gain) ((gain) * 100)
#define dbm_to_mbm(gain) ((gain) * 100)

#define reg_rule(start, end, bw, gain, eirp, reg_flags) {                                                    \
        .freq_range.start_freq_khz = mhz_to_khz(start),  \
        .freq_range.end_freq_khz = mhz_to_khz(end),      \
        .freq_range.max_bandwidth_khz = mhz_to_khz(bw),  \
        .power_rule.max_antenna_gain = dbi_to_mbi(gain), \
        .power_rule.max_eirp = dbm_to_mbm(eirp),         \
        .flags = (reg_flags),                            \
    }

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* DHCP options */
enum dhcp_opt {
    DHCP_OPT_PAD                    = 0,
    DHCP_OPT_SUBNETMASK             = 1,
    DHCP_OPT_ROUTER                 = 3,
    DHCP_OPT_DNSSERVER              = 6,
    DHCP_OPT_HOSTNAME               = 12,
    DHCP_OPT_DNSDOMAIN              = 15,
    DHCP_OPT_MTU                    = 26,
    DHCP_OPT_BROADCAST              = 28,
    DHCP_OPT_STATICROUTE            = 33,
    DHCP_OPT_NISDOMAIN              = 40,
    DHCP_OPT_NISSERVER              = 41,
    DHCP_OPT_NTPSERVER              = 42,
    DHCP_OPT_VENDOR                 = 43,
    DHCP_OPT_IPADDRESS              = 50,
    DHCP_OPT_LEASETIME              = 51,
    DHCP_OPT_OPTIONSOVERLOADED      = 52,
    DHCP_OPT_MESSAGETYPE            = 53,
    DHCP_OPT_SERVERID               = 54,
    DHCP_OPT_PARAMETERREQUESTLIST   = 55,
    DHCP_OPT_MESSAGE                = 56,
    DHCP_OPT_MAXMESSAGESIZE         = 57,
    DHCP_OPT_RENEWALTIME            = 58,
    DHCP_OPT_REBINDTIME             = 59,
    DHCP_OPT_VENDORCLASSID          = 60,
    DHCP_OPT_CLIENTID               = 61,
    DHCP_OPT_USERCLASS              = 77,  /* RFC 3004 */
    DHCP_OPT_FQDN                   = 81,
    DHCP_OPT_DNSSEARCH              = 119, /* RFC 3397 */
    DHCP_OPT_CSR                    = 121, /* RFC 3442 */
    DHCP_OPT_SIXRD                  = 212, /* RFC 5969 */
    DHCP_OPT_MSCSR                  = 249, /* MS code for RFC 3442 */
    DHCP_OPT_END                    = 255
};

enum nl80211_iftype {
    NL80211_IFTYPE_UNSPECIFIED,
    NL80211_IFTYPE_ADHOC,
    NL80211_IFTYPE_STATION,
    NL80211_IFTYPE_AP,
    NL80211_IFTYPE_AP_VLAN,
    NL80211_IFTYPE_WDS,
    NL80211_IFTYPE_MONITOR,
    NL80211_IFTYPE_MESH_POINT,
    NL80211_IFTYPE_P2P_CLIENT,
    NL80211_IFTYPE_P2P_GO,
    NL80211_IFTYPE_P2P_DEVICE,
    /* keep last */
    NUM_NL80211_IFTYPES,
    NL80211_IFTYPE_MAX = NUM_NL80211_IFTYPES - 1
};

#define OAL_NETDEV_TX_OK     NETDEV_TX_OK
#define OAL_NETDEV_TX_BUSY   NETDEV_TX_BUSY
#define OAL_NETDEV_TX_LOCKED NETDEV_TX_LOCKED

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
typedef NetDevTxResult oal_net_dev_tx_enum;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef int32_t oal_net_dev_tx_enum;
#endif

typedef hi_u8 ieee80211_band_uint8;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)

typedef hi_u8 oal_nl80211_channel_type_uint8;

typedef hi_u8 nl80211_iftype_uint8;
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX) */

typedef enum {
    NETIF_FLOW_CTRL_OFF, /* stop flow_ctrl, continue to transfer data to driver */
    NETIF_FLOW_CTRL_ON,  /* start flow_ctrl, stop transferring data to driver */
    NETIF_FLOW_CTRL_BUTT
} netif_flow_ctrl_enum;
typedef hi_u8 netif_flow_ctrl_enum_uint8;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
enum cfg80211_signal_type {
    CFG80211_SIGNAL_TYPE_NONE,
    CFG80211_SIGNAL_TYPE_MBM,
    CFG80211_SIGNAL_TYPE_UNSPEC,
};
#endif

enum station_info_flags {
    STATION_INFO_INACTIVE_TIME      = 1 << 0,
    STATION_INFO_RX_BYTES           = 1 << 1,
    STATION_INFO_TX_BYTES           = 1 << 2,
    STATION_INFO_LLID               = 1 << 3,
    STATION_INFO_PLID               = 1 << 4,
    STATION_INFO_PLINK_STATE        = 1 << 5,
    STATION_INFO_SIGNAL             = 1 << 6,
    STATION_INFO_TX_BITRATE         = 1 << 7,
    STATION_INFO_RX_PACKETS         = 1 << 8,
    STATION_INFO_TX_PACKETS         = 1 << 9,
    STATION_INFO_TX_RETRIES         = 1 << 10,
    STATION_INFO_TX_FAILED          = 1 << 11,
    STATION_INFO_RX_DROP_MISC       = 1 << 12,
    STATION_INFO_SIGNAL_AVG         = 1 << 13,
    STATION_INFO_RX_BITRATE         = 1 << 14,
    STATION_INFO_BSS_PARAM          = 1 << 15,
    STATION_INFO_CONNECTED_TIME     = 1 << 16,
    STATION_INFO_ASSOC_REQ_IES      = 1 << 17,
    STATION_INFO_STA_FLAGS          = 1 << 18,
    STATION_INFO_BEACON_LOSS_COUNT  = 1 << 19,
    STATION_INFO_T_OFFSET           = 1 << 20,
    STATION_INFO_LOCAL_PM           = 1 << 21,
    STATION_INFO_PEER_PM            = 1 << 22,
    STATION_INFO_NONPEER_PM         = 1 << 23,
    STATION_INFO_RX_BYTES64         = 1 << 24,
    STATION_INFO_TX_BYTES64         = 1 << 25,
};

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)

enum nl80211_channel_type {
    NL80211_CHAN_NO_HT,
    NL80211_CHAN_HT20,
    NL80211_CHAN_HT40MINUS,
    NL80211_CHAN_HT40PLUS
};

typedef hi_u8 oal_nl80211_auth_type_enum_uint8;
typedef enum nl80211_channel_type oal_nl80211_channel_type;

typedef enum nl80211_mfp {
    NL80211_MFP_NO,
    NL80211_MFP_REQUIRED,
} oal_nl80211_mfp_enum;
typedef hi_u8 oal_nl80211_mfp_enum_uint8;

enum wiphy_flags {
    WIPHY_FLAG_CUSTOM_REGULATORY        = bit(0),
    WIPHY_FLAG_STRICT_REGULATORY        = bit(1),
    WIPHY_FLAG_DISABLE_BEACON_HINTS     = bit(2),
    WIPHY_FLAG_NETNS_OK                 = bit(3),
    WIPHY_FLAG_PS_ON_BY_DEFAULT         = bit(4),
    WIPHY_FLAG_4ADDR_AP                 = bit(5),
    WIPHY_FLAG_4ADDR_STATION            = bit(6),
    WIPHY_FLAG_CONTROL_PORT_PROTOCOL    = bit(7),
    WIPHY_FLAG_IBSS_RSN                 = bit(8),
    WIPHY_FLAG_MESH_AUTH                = bit(10),
    WIPHY_FLAG_SUPPORTS_SCHED_SCAN      = bit(11),
    /* use hole at 12 */
    WIPHY_FLAG_SUPPORTS_FW_ROAM         = bit(13),
    WIPHY_FLAG_AP_UAPSD                 = bit(14),
    WIPHY_FLAG_SUPPORTS_TDLS            = bit(15),
    WIPHY_FLAG_TDLS_EXTERNAL_SETUP      = bit(16),
    WIPHY_FLAG_HAVE_AP_SME              = bit(17),
    WIPHY_FLAG_REPORTS_OBSS             = bit(18),
    WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD    = bit(19),
    WIPHY_FLAG_OFFCHAN_TX               = bit(20),
    WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL    = bit(21),
};

enum nl80211_mesh_power_mode {
    NL80211_MESH_POWER_UNKNOWN,
    NL80211_MESH_POWER_ACTIVE,
    NL80211_MESH_POWER_LIGHT_SLEEP,
    NL80211_MESH_POWER_DEEP_SLEEP,

    __NL80211_MESH_POWER_AFTER_LAST,
    NL80211_MESH_POWER_MAX = __NL80211_MESH_POWER_AFTER_LAST - 1
};

typedef enum nl80211_key_type {
    NL80211_KEYTYPE_GROUP,
    NL80211_KEYTYPE_PAIRWISE,
    NL80211_KEYTYPE_PEERKEY,
    NUM_NL80211_KEYTYPES
} oal_nl80211_key_type;

enum rate_info_flags {
    RATE_INFO_FLAGS_MCS              = bit(0),
    RATE_INFO_FLAGS_VHT_MCS          = bit(1),
    RATE_INFO_FLAGS_40_MHZ_WIDTH     = bit(2),
    RATE_INFO_FLAGS_80_MHZ_WIDTH     = bit(3),
    RATE_INFO_FLAGS_80P80_MHZ_WIDTH  = bit(4),
    RATE_INFO_FLAGS_160_MHZ_WIDTH    = bit(5),
    RATE_INFO_FLAGS_SHORT_GI         = bit(6),
    RATE_INFO_FLAGS_60G              = bit(7),
};
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX) */

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
typedef hi_u8 oal_nl80211_channel_type_uint8;
typedef hi_u8 oal_nl80211_mfp_enum_uint8;
typedef enum nl80211_channel_type oal_nl80211_channel_type;
typedef enum nl80211_iftype enum_nl80211_iftype;
typedef hi_u8 nl80211_iftype_uint8;
typedef hi_u8 oal_nl80211_auth_type_enum_uint8;
typedef enum nl80211_key_type oal_nl80211_key_type;
#endif

typedef enum {
    OAL_PASSIVE_SCAN        = 0,
    OAL_ACTIVE_SCAN         = 1,
    OAL_SCAN_BUTT
} oal_scan_enum;

/* 内核下发的扫描频段 */
typedef enum {
    OAL_SCAN_2G_BAND        = 1,
    OAL_SCAN_5G_BAND        = 2,
    OAL_SCAN_ALL_BAND       = 3,

    OAL_SCAN_BAND_BUTT
} oal_scan_band_enum;

/* hostapd 下发私有命令 */
enum hwifi_ioctl_cmd {
    /*
     IOCTL_CMD的起始值由0修改为0x8EE0，修改原因：51 WiFi模块和类似于dhdutil之类的其他模块共用同一个ioctl通道，
     而51命令的枚举值从0开始，其他模块下发的ioctl命令也包含从0开始的这部分，这样就会同时“组播”到自己的模块和WiFi模块，
     从而对WiFi模块的功能产生影响。所以将51 WiFi模块命令的枚举值调整到0x8EE0起，便规避了其他模块命令的影响。
     */
    HWIFI_IOCTL_CMD_GET_STA_ASSOC_REQ_IE = 0x8EE0, /* get sta assocate request ie */
    HWIFI_IOCTL_CMD_SET_AP_AUTH_ALG,               /* set auth alg to driver */
    HWIFI_IOCTL_CMD_SET_COUNTRY,                   /* 设置国家码 */
    HWIFI_IOCTL_CMD_SET_SSID,                      /* 设置ssid */
    HWIFI_IOCTL_CMD_SET_MAX_USER,                  /* 设置最大用户数 */
    HWIFI_IOCTL_CMD_SET_FREQ,                      /* 设置频段 */
    HWIFI_IOCTL_CMD_SET_WPS_IE,                    /* 设置AP WPS 信息元素 */
    HWIFI_IOCTL_CMD_PRIV_CONNECT,                  /* linux-2.6.30 sta发起connect */
    HWIFI_IOCTL_CMD_PRIV_DISCONNECT,               /* linux-2.6.30 sta发起disconnect */
    HWIFI_IOCTL_CMD_SET_FRAG,                      /* 设置分片门限值 */
    HWIFI_IOCTL_CMD_SET_RTS,                       /* 设置RTS 门限值 */

    HWIFI_IOCTL_CMD_NUM
};

enum app_ie_type {
    OAL_APP_BEACON_IE       = 1,
    OAL_APP_PROBE_REQ_IE    = 1,
    OAL_APP_PROBE_RSP_IE    = 2,
    OAL_APP_ASSOC_REQ_IE    = 3,
    OAL_APP_ASSOC_RSP_IE    = 4,
    OAL_APP_IE_NUM
};
typedef hi_u8 en_app_ie_type_uint8;

typedef enum _wlan_net_queue_type_ {
    WLAN_HI_QUEUE = 0,
    WLAN_NORMAL_QUEUE,

    WLAN_TCP_DATA_QUEUE,
    WLAN_TCP_ACK_QUEUE,

    WLAN_UDP_BK_QUEUE,
    WLAN_UDP_BE_QUEUE,
    WLAN_UDP_VI_QUEUE,
    WLAN_UDP_VO_QUEUE,

    WLAN_NET_QUEUE_BUTT
} wlan_net_queue_type;
typedef struct NetDevice oal_net_device_stru;
/* ****************************************************************************
  4 结构体定义
**************************************************************************** */
typedef struct ieee80211_regdomain oal_ieee80211_regdomain_stru;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef hi_u8 oal_mem_state_enum_uint8;
typedef struct sock oal_sock_stru;
typedef struct module oal_module_stru;
typedef struct nlmsghdr oal_nlmsghdr_stru;

typedef struct kobj_uevent_env oal_kobj_uevent_env_stru;
struct dev_excp_globals {
    oal_sock_stru* nlsk;
    hi_u8      mode;
    hi_u8*     data;
    hi_u32     usepid;
};

struct dev_netlink_msg_hdr_stru {
    hi_u32 cmd;
    hi_u32 len;
};
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
#define ieee80211_band Ieee80211Band

struct ieee80211_hdr {
    hi_u16 frame_control;
    hi_u16 duration_id;
    hi_u8 addr1[OAL_MAC_ADDR_LEN];
    hi_u8 addr2[OAL_MAC_ADDR_LEN];
    hi_u8 addr3[OAL_MAC_ADDR_LEN];
    hi_u16 seq_ctrl;
    /* followed by 'u8 addr4[6];' if ToDS and FromDS is set in data frame */
};

struct ieee80211_freq_range {
    hi_u32 start_freq_khz;
    hi_u32 end_freq_khz;
    hi_u32 max_bandwidth_khz;
};

struct ieee80211_power_rule {
    hi_u32 max_antenna_gain;
    hi_u32 max_eirp;
};

struct ieee80211_reg_rule {
    struct ieee80211_freq_range freq_range;
    struct ieee80211_power_rule power_rule;
    hi_u32 flags;
};

struct ieee80211_msrment_ie {
    hi_u8 token;
    hi_u8 mode;
    hi_u8 type;
    hi_u8 request[0];
};

struct ieee80211_ext_chansw_ie {
    hi_u8 mode;
    hi_u8 new_operating_class;
    hi_u8 new_ch_num;
    hi_u8 count;
};

typedef struct ieee80211_mgmt {
    hi_u16 frame_control;
    hi_u16 duration;
    hi_u8 da[6];    /* 6: SIZE(0..6) */
    hi_u8 sa[6];    /* 6: SIZE(0..6) */
    hi_u8 bssid[6]; /* 6: SIZE(0..6) */
    hi_u16 seq_ctrl;
    union {
        struct {
            hi_u16 auth_alg;
            hi_u16 auth_transaction;
            hi_u16 status_code;
            /* possibly followed by Challenge text */
            hi_u8 variable[0];
        } auth;
        struct {
            hi_u16 reason_code;
        } deauth;
        struct {
            hi_u16 capab_info;
            hi_u16 listen_interval;
            /* followed by SSID and Supported rates */
            hi_u8 variable[0];
        } assoc_req;
        struct {
            hi_u16 capab_info;
            hi_u16 status_code;
            hi_u16 aid;
            /* followed by Supported rates */
            hi_u8 variable[0];
        } assoc_resp, reassoc_resp;
        struct {
            hi_u16 capab_info;
            hi_u16 listen_interval;
            hi_u8 current_ap[6]; /* 6: SIZE(0..6) */
            /* followed by SSID and Supported rates */
            hi_u8 variable[0];
        } reassoc_req;
        struct {
            hi_u16 reason_code;
        } disassoc;
        struct {
            hi_u64 timestamp;
            hi_u16 beacon_int;
            hi_u16 capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params, TIM */
            hi_u8 variable[4]; /* 4: FH Params, DS Params, CF Params, IBSS Params */
        } beacon;
        struct {
            hi_u64 timestamp;
            hi_u16 beacon_int;
            hi_u16 capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params */
            hi_u8 variable[4]; /* 4: FH Params, DS Params, CF Params, IBSS Params */
        } probe_resp;
        struct {
            hi_u8 category;
            union {
                struct {
                    hi_u8 action_code;
                    hi_u8 dialog_token;
                    hi_u8 status_code;
                    hi_u8 variable[0];
                } wme_action;
                struct {
                    hi_u8 action_code;
                    hi_u8 variable[0];
                } chan_switch;
                struct {
                    hi_u8 action_code;
                    struct ieee80211_ext_chansw_ie data;
                    hi_u8 variable[0];
                } ext_chan_switch;
                struct {
                    hi_u8 action_code;
                    hi_u8 dialog_token;
                    hi_u8 element_id;
                    hi_u8 length;
                    struct ieee80211_msrment_ie msr_elem;
                } measurement;
                struct {
                    hi_u8 action_code;
                    hi_u8 dialog_token;
                    hi_u16 capab;
                    hi_u16 timeout;
                    hi_u16 start_seq_num;
                } addba_req;
                struct {
                    hi_u8 action_code;
                    hi_u8 dialog_token;
                    hi_u16 status;
                    hi_u16 capab;
                    hi_u16 timeout;
                } addba_resp;
                struct {
                    hi_u8 action_code;
                    hi_u8 resv;
                    hi_u16 params;
                    hi_u16 reason_code;
                } delba;
                struct {
                    hi_u8 action_code;
                    hi_u8 variable[0];
                } self_prot;
                struct {
                    hi_u8 action_code;
                    hi_u8 variable[0];
                } mesh_action;
                struct {
                    hi_u8 action;
                    hi_u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
                } sa_query;
                struct {
                    hi_u8 action;
                    hi_u8 smps_control;
                } ht_smps;
                struct {
                    hi_u8 action_code;
                    hi_u8 chanwidth;
                } ht_notify_cw;
                struct {
                    hi_u8 action_code;
                    hi_u8 dialog_token;
                    hi_u16 capability;
                    hi_u8 variable[0];
                } tdls_discover_resp;
                struct {
                    hi_u8 action_code;
                    hi_u8 operating_mode;
                } vht_opmode_notif;
            } u;
        } action;
    } u;
} oal_ieee80211_mgmt;


struct ieee80211_vht_mcs_info {
    hi_u16 rx_mcs_map;
    hi_u16 rx_highest;
    hi_u16 tx_mcs_map;
    hi_u16 tx_highest;
};

enum nl80211_hidden_ssid {
    NL80211_HIDDEN_SSID_NOT_IN_USE,
    NL80211_HIDDEN_SSID_ZERO_LEN,
    NL80211_HIDDEN_SSID_ZERO_CONTENTS
};

struct ieee80211_mcs_info {
    hi_u8   rx_mask[IEEE80211_HT_MCS_MASK_LEN];
    hi_u16  rx_highest;
    hi_u8   tx_params;
    hi_u8   reserved[3];  /* 3: bytes保留字段 */
};

struct ieee80211_ht_cap {
    /* 16 bytes MCS information */
    struct ieee80211_mcs_info mcs;
    hi_u32 tx_bf_cap_info;
    hi_u16 cap_info;
    hi_u16 extended_ht_cap_info;
    hi_u8 antenna_selection_info;
    hi_u8 ampdu_params_info;
    hi_u8 resv[2]; /* 2: bytes保留字段 */
};

struct ieee80211_vht_cap {
    hi_u32 vht_cap_info;
    struct ieee80211_vht_mcs_info supp_mcs;
};

typedef struct cfg80211_beacon_data {
    const hi_u8 *head, *tail;
    const hi_u8 *beacon_ies;
    const hi_u8 *proberesp_ies;
    const hi_u8 *assocresp_ies;
    const hi_u8 *probe_resp;

    size_t head_len, tail_len;
    size_t beacon_ies_len;
    size_t proberesp_ies_len;
    size_t assocresp_ies_len;
    size_t probe_resp_len;
} oal_beacon_data_stru;

struct oal_mac_address {
    hi_u8 addr[ETH_ALEN];
};

enum nl80211_acl_policy {
    NL80211_ACL_POLICY_ACCEPT_UNLESS_LISTED,
    NL80211_ACL_POLICY_DENY_UNLESS_LISTED,
};

enum nl80211_auth_type {
    NL80211_AUTHTYPE_OPEN_SYSTEM,
    NL80211_AUTHTYPE_SHARED_KEY,
    NL80211_AUTHTYPE_FT,
    NL80211_AUTHTYPE_NETWORK_EAP,
    NL80211_AUTHTYPE_SAE,
    __NL80211_AUTHTYPE_NUM,
    NL80211_AUTHTYPE_MAX = __NL80211_AUTHTYPE_NUM - 1,
    NL80211_AUTHTYPE_AUTOMATIC
};

struct cfg80211_acl_data {
    enum nl80211_acl_policy acl_policy;
    hi_s32 n_acl_entries;

    /* Keep it last */
    struct oal_mac_address mac_addrs[];
};

typedef struct ieee80211_channel {
    enum ieee80211_band band;
    hi_u16          center_freq;
    hi_u16          hw_value;
    hi_u32          flags;
    hi_s32          max_antenna_gain;
    hi_s32          max_power;
    hi_bool         beacon_found;
    hi_u8           resv[3];  /* 3: bytes保留字段 */
    hi_u32          orig_flags;
    hi_s32          orig_mag;
    hi_s32          orig_mpwr;
} oal_ieee80211_channel;

typedef struct cfg80211_chan_def {
    oal_ieee80211_channel *chan;
    oal_nl80211_channel_type width;
    hi_s32 center_freq1;
    hi_s32 center_freq2;
} oal_cfg80211_chan_def;

typedef struct cfg80211_ap_settings {
    struct cfg80211_chan_def        chandef;
    struct cfg80211_beacon_data     beacon;

    hi_s32                          beacon_interval;
    hi_s32                          dtim_period;
    const hi_u8*                    ssid;
    size_t                          ssid_len;

    enum nl80211_hidden_ssid        hidden_ssid;
    enum nl80211_auth_type          auth_type;
    struct CryptoSettings crypto;

    hi_s32                          inactivity_timeout;
    hi_bool                         privacy;
    hi_bool                         p2p_opp_ps;
    hi_u8                           p2p_ctwindow;
    hi_bool                         radar_required;

    const struct cfg80211_acl_data *acl;
} oal_ap_settings_stru;

struct cfg80211_ssid {
    hi_u8 ssid[IEEE80211_MAX_SSID_LEN];
    hi_u8 ssid_len;
    hi_u8 resv[3]; /* reserve 3byte */
};

struct cfg80211_match_set {
    struct cfg80211_ssid ssid;
    hi_s32 rssi_thold;
};

typedef struct cfg80211_sched_scan_request {
    struct cfg80211_ssid*        ssids;
    hi_s32                       n_ssids;
    hi_u32                       n_channels;
    hi_u32                       interval;
    const hi_u8*                 ie;
    size_t                       ie_len;
    hi_u32                       flags;
    struct cfg80211_match_set*   match_sets;
    hi_s32                       n_match_sets;
    hi_s32                       min_rssi_thold;
    hi_s32                       rssi_thold; /* just for backward compatible */

    /* internal */
    struct wiphy*                wiphy;
    struct oal_net_device_stru*  dev;
    hi_u32                       scan_start;
    /* keep last */
    struct ieee80211_channel*    channels[0];
} oal_cfg80211_sched_scan_request_stru;

typedef struct cfg80211_mgmt_tx_params {
    struct Ieee80211Channel *chan;
    bool offchan;
    uint32_t wait;
    const uint8_t *buf;
    size_t len;
    bool no_cck;
    bool dont_wait_for_ack;
    int32_t n_csa_offsets;
    const uint16_t *csa_offsets;
} cfg80211_mgmt_tx_params;

#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct ieee80211_mgmt               oal_ieee80211_mgmt;
typedef struct cfg80211_beacon_data         oal_beacon_data_stru;
typedef struct ieee80211_channel            oal_ieee80211_channel;
typedef struct cfg80211_ap_settings         oal_ap_settings_stru;
typedef struct bss_parameters               oal_bss_parameters;
typedef struct ieee80211_rate               oal_ieee80211_rate;
typedef struct ieee80211_sta_ht_cap         oal_ieee80211_sta_ht_cap;
typedef struct ieee80211_supported_band     oal_ieee80211_supported_band;
typedef struct wireless_dev                 oal_wireless_dev;
typedef struct rate_info                    oal_rate_info_stru;
typedef struct vif_params                   oal_vif_params_stru;
typedef struct cfg80211_ssid                oal_cfg80211_ssid_stru;
typedef struct station_parameters           oal_station_parameters_stru;
typedef struct cfg80211_mgmt_tx_params      oal_cfg80211_mgmt_tx_params_stru;
//typedef struct station_del_parameters       oal_station_del_parameters_stru;
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX) */

typedef struct StationDelParameters       oal_station_del_parameters_stru;

typedef struct oal_cpu_usage_stat {
    hi_u64 ull_user;
    hi_u64 ull_nice;
    hi_u64 ull_system;
    hi_u64 ull_softirq;
    hi_u64 ull_irq;
    hi_u64 ull_idle;
    hi_u64 ull_iowait;
    hi_u64 ull_steal;
    hi_u64 ull_guest;
} oal_cpu_usage_stat_stru;

struct oal_ether_header {
    hi_u8    auc_ether_dhost[ETHER_ADDR_LEN];
    hi_u8    auc_ether_shost[ETHER_ADDR_LEN];
    hi_u16   us_ether_type;
} __OAL_DECLARE_PACKED;

typedef struct oal_ether_header oal_ether_header_stru;

typedef IfReq oal_ifreq_stru;

typedef struct {
    hi_u32 handle;
} oal_qdisc_stru;

/* iw_handler_def结构体封装 */
typedef struct {
    hi_u16 cmd;   /* Wireless Extension command */
    hi_u16 flags; /* More to come ;-) */
} oal_iw_request_info_stru;

typedef struct {
    hi_void *pointer; /* Pointer to the data  (in user space) */
    hi_u16 length;    /* number of fields or size in bytes */
    hi_u16 flags;     /* Optional params */
} oal_iw_point_stru;

typedef struct {
    hi_s32     value;      /* The value of the parameter itself */
    hi_u8      fixed;      /* Hardware should not use auto select */
    hi_u8      disabled;   /* Disable the feature */
    hi_u16     flags;      /* Various specific flags (if any) */
} oal_iw_param_stru;

typedef struct {
    hi_s32       m;       /* Mantissa */
    hi_s16       e;       /* Exponent */
    hi_u8        i;       /* List index (when in range struct) */
    hi_u8        flags;   /* Flags (fixed/auto) */
} oal_iw_freq_stru;

typedef struct {
    hi_u8        qual;       /* link quality (%retries, SNR, %missed beacons or better...) */
    hi_u8        level;      /* signal level (dBm) */
    hi_u8        noise;      /* noise level (dBm) */
    hi_u8        updated;    /* Flags to know if updated */
} oal_iw_quality_stru;

typedef struct {
    hi_u16       sa_family;      /* address family, AF_xxx   */
    hi_u8        sa_data[14];    /* 14 bytes of protocol address */
} oal_sockaddr_stru;

typedef union {
    /* Config - generic */
    hi_char             name[OAL_IF_NAME_SIZE];
    oal_iw_point_stru   essid;      /* Extended network name */
    oal_iw_param_stru   nwid;       /* network id (or domain - the cell) */
    oal_iw_freq_stru    freq;       /* frequency or channel : * 0-1000 = channel * > 1000 = frequency in Hz */
    oal_iw_param_stru   sens;       /* signal level threshold */
    oal_iw_param_stru   bitrate;    /* default bit rate */
    oal_iw_param_stru   txpower;    /* default transmit power */
    oal_iw_param_stru   rts;        /* RTS threshold threshold */
    oal_iw_param_stru   frag;       /* Fragmentation threshold */
    hi_u32              mode;       /* Operation mode */
    oal_iw_param_stru   retry;      /* Retry limits & lifetime */
    oal_iw_point_stru   encoding;   /* Encoding stuff : tokens */
    oal_iw_param_stru   power;      /* PM duration/timeout */
    oal_iw_quality_stru qual;       /* Quality part of statistics */
    oal_sockaddr_stru   ap_addr;    /* Access point address */
    oal_sockaddr_stru   addr;       /* Destination address (hw/mac) */
    oal_iw_param_stru   param;      /* Other small parameters */
    oal_iw_point_stru   data;       /* Other large parameters */
} oal_iwreq_data_union;
void OalInitSpecialProcPriv(oal_net_device_stru *netdev);
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
typedef hi_u32 (*oal_iw_handler)(oal_net_device_stru* dev, oal_iw_request_info_stru* info,
    oal_iwreq_data_union* wrqu, hi_s8* extra);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef iw_handler oal_iw_handler;
#endif
typedef hi_u8 en_cfg80211_signal_type_uint8;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
/* 此结构体成员命名是为了保持跟linux一致 */
typedef struct ieee80211_rate oal_ieee80211_rate;
typedef struct ieee80211_sta_ht_cap oal_ieee80211_sta_ht_cap;

typedef struct ieee80211_rate {
    hi_u32 flags;
    hi_u16 bitrate;
    hi_u16 hw_value;
    hi_u16 hw_value_short;
    hi_u8 rsv[2]; /* 2: bytes保留字段 */
} oal_ieee80211_rate;

typedef struct ieee80211_sta_ht_cap {
    hi_u16          cap;                    /* use IEEE80211_HT_CAP_ */
    hi_u8           ht_supported;
    hi_u8           ampdu_factor;
    hi_u8           ampdu_density;
    hi_u8           auc_rsv[3];  /* 3: bytes保留字段 */
    struct          ieee80211_mcs_info mcs;
} oal_ieee80211_sta_ht_cap;

typedef struct ieee80211_supported_band {
    oal_ieee80211_channel*       channels;
    oal_ieee80211_rate*          bitrates;
    enum ieee80211_band         band;
    hi_s32                      n_channels;
    hi_s32                      n_bitrates;
    oal_ieee80211_sta_ht_cap    ht_cap;
} oal_ieee80211_supported_band;

typedef struct oal_wiphy_tag {
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    hi_u8                                       perm_addr[6];  /* 6: SIZE(0..6) */
    hi_u8                                       addr_mask[6];  /* 6: SIZE(0..6) */
    hi_u32                                      flags;
    en_cfg80211_signal_type_uint8               signal_type;
    hi_u8                                       max_scan_ssids;
    hi_u16                                      interface_modes;
    hi_u16                                      max_scan_ie_len;
    hi_u8                                       auc_rsv[2];    /* 2: bytes保留字段 */
    hi_s32                                      n_cipher_suites;
    const hi_u32*                               cipher_suites;
    hi_u32                                      frag_threshold;
    hi_u32                                      rts_threshold;
#endif
    oal_ieee80211_supported_band*               bands[IEEE80211_NUM_BANDS];
    hi_u8                                       priv[4];       /* 4: SIZE(0..4) */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    const struct ieee80211_txrx_stypes*         mgmt_stypes;
    const struct ieee80211_iface_combination*   iface_combinations;
    hi_s32                                      n_iface_combinations;
    hi_u16                                      max_remain_on_channel_duration;
    hi_u8                                       max_sched_scan_ssids;
    hi_u8                                       max_match_sets;
    hi_u16                                      max_sched_scan_ie_len;
    hi_void*                                    ctx;
#endif
} oal_wiphy_stru;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct wiphy oal_wiphy_stru;
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX) */

typedef struct {
    hi_u32 fake;
} oal_iw_statistics_stru;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* 私有IOCTL接口信息 */
typedef struct {
    hi_u32       cmd;                       /* ioctl命令号 */
    hi_u16       set_args;                  /* 类型和参数字符个数 */
    hi_u16       get_args;                  /* 类型和参数字符个数 */
    hi_char      name[OAL_IF_NAME_SIZE];    /* 私有命令名 */
} oal_iw_priv_args_stru;

typedef struct {
    const oal_iw_handler*    standard;
    hi_u16                   num_standard;
    hi_u16                   num_private;

    /* FangBaoshun For Hi1131 Compile */
    const oal_iw_handler *private;
    /* FangBaoshun For Hi1131 Compile */
    hi_u8                       auc_resv[2]; /* 2: bytes保留字段 */
    hi_u16                      num_private_args;

    const oal_iw_handler*        private_win32;

    const oal_iw_priv_args_stru* private_args;
    oal_iw_statistics_stru*     (*get_wireless_stats)(oal_net_device_stru* dev);
} oal_iw_handler_def_stru;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct iw_handler_def       oal_iw_handler_def_stru;
typedef struct iw_priv_args         oal_iw_priv_args_stru;
#endif

typedef struct NetDevStats            oal_net_device_stats_stru;

typedef IpV4Addr oal_ip_addr_t;

typedef struct ieee80211_mgmt               oal_ieee80211_mgmt_stru;
typedef struct ieee80211_channel            oal_ieee80211_channel_stru;
typedef struct cfg80211_bss                 oal_cfg80211_bss_stru;


#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
typedef struct linklayer_addr       oal_linklayer_addr;
typedef uniqid_t                    oal_uniqid_t;
typedef ip6_addr_t                  oal_ipv6_addr_t;
typedef linklayer_event_tx_info_t   oal_event_tx_info_stru;
typedef linklayer_event_new_peer_t  oal_event_new_peer_stru;
typedef linklayer_event_del_peer_t  oal_event_del_peer_stru;
#endif
typedef struct EapolData oal_hisi_eapol_stru;

typedef struct oal_net_notify {
    hi_u32 ip_addr;
    hi_u32 notify_type;
} oal_net_notify_stru;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
typedef struct wireless_dev {
    oal_net_device_stru*       netdev;
    oal_wiphy_stru*              wiphy;
    nl80211_iftype_uint8         iftype;
    hi_u8                        resv[3]; /* reserve 3byte */
    /* 1102新内核新增字段 add by lm */
    oal_cfg80211_chan_def preset_chandef;
} oal_wireless_dev;

#endif


typedef struct NetDeviceInterFace oal_net_device_ops_stru;


#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
typedef struct rate_info {
    hi_u8 flags;
    hi_u8 mcs;
    hi_u16 legacy;
    hi_u8 nss;
    hi_u8 resv;
} oal_rate_info_stru;
#endif

typedef struct {
    union {
        hi_u8       u6_addr8[16];  /* 16: u6_addr8 长度 */
        hi_u16      u6_addr16[8];  /* 8: u6_addr16 长度 */
        hi_u32      u6_addr32[4];  /* 4: u6_addr32 长度 */
    } in6_u;
} oal_in6_addr;

typedef struct {
    hi_u32      reserved: 5,
                override: 1,
                solicited: 1,
                router: 1,
                reserved2: 24;
} icmpv6_nd_advt;

typedef struct {
    hi_u8           version: 4,
                    priority: 4;
    hi_u8           flow_lbl[3];  /* 3: SIZE(0..3) */
    hi_u16          payload_len;

    hi_u8           nexthdr;
    hi_u8           hop_limit;

    oal_in6_addr    saddr;
    oal_in6_addr    daddr;
} oal_ipv6hdr_stru;

typedef struct {
    hi_u8  icmp6_type;
    hi_u8  icmp6_code;
    hi_u16 icmp6_cksum;
    union {
        hi_u32          data32[1];
        hi_u16          data16[2];  /* 2: SIZE(0..2) */
        hi_u8           data8[4];   /* 4: SIZE(0..4) */
        icmpv6_nd_advt  u_nd_advt;
    } icmp6_dataun;

#define ICMP6_SOLICITED icmp6_dataun.u_nd_advt.solicited
#define ICMP6_OVERRIDE icmp6_dataun.u_nd_advt.override
} oal_icmp6hdr_stru;

/* 多了4字节，记得减去4 */
typedef struct {
    oal_icmp6hdr_stru   icmph;
    oal_in6_addr        target;
    hi_u8               opt[4];  /* 4: SIZE(0..4) */
} oal_nd_msg_stru;

typedef struct {
    hi_u8 nd_opt_type;
    hi_u8 nd_opt_len;
} oal_nd_opt_hdr;

typedef struct oal_cfg80211_crypto_settings_tag {
    hi_u32 wpa_versions;
    hi_u32 cipher_group;
    hi_s32 n_ciphers_pairwise;
    hi_u32 ciphers_pairwise[OAL_NL80211_MAX_NR_CIPHER_SUITES];
    hi_s32 n_akm_suites;
    hi_u32 akm_suites[OAL_NL80211_MAX_NR_AKM_SUITES];

    hi_u8 control_port;
    hi_u8 auc_arry[3]; /* 3: SIZE(0..3) */
} oal_cfg80211_crypto_settings_stru;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
typedef struct {
    hi_s32 sk_wmem_queued;
} oal_sock_stru;
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
struct sta_bss_parameters {
    hi_u8 flags;
    hi_u8 dtim_period;
    hi_u16 beacon_interval;
};

struct nl80211_sta_flag_update {
    hi_u32 mask;
    hi_u32 set;
};

typedef struct oal_station_info_tag {
    hi_u32                          filled;
    hi_u32                          connected_time;
    hi_u32                          inactive_time;
    hi_u16                          llid;
    hi_u16                          plid;

    hi_u64                          rx_bytes;
    hi_u64                          tx_bytes;
    oal_rate_info_stru              txrate;
    oal_rate_info_stru              rxrate;

    hi_u32                          rx_packets;
    hi_u32                          tx_packets;
    hi_u32                          tx_retries;
    hi_u32                          tx_failed;

    hi_u32                          rx_dropped_misc;
    hi_s32                          generation;
    struct sta_bss_parameters       bss_param;
    struct nl80211_sta_flag_update  sta_flags;

    hi_s64                          t_offset;
    const hi_u8*                    assoc_req_ies;
    hi_u32                          assoc_req_ies_len;
    hi_u32                          beacon_loss_count;

    hi_u8                           plink_state;
    hi_s8                           signal;
    hi_s8                           signal_avg;
    hi_u8                           resv1;

    enum nl80211_mesh_power_mode    local_pm;
    enum nl80211_mesh_power_mode    peer_pm;
    enum nl80211_mesh_power_mode    nonpeer_pm;
    hi_u32                          resv2;
} oal_station_info_stru;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct station_info oal_station_info_stru;
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX) */

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
typedef struct oal_key_params_tag {
    hi_u8 *key;
    hi_u8 *seq;
    hi_s32 key_len;
    hi_s32 seq_len;
    hi_u32 cipher;
} oal_key_params_stru;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct key_params oal_key_params_stru;
#endif

typedef struct cfg80211_add_key_info_stru {
    hi_u8    key_index;
    hi_bool  pairwise;
} cfg80211_add_key_info_stru;

typedef struct cfg80211_get_key_info_stru {
    hi_u8 key_index;
    hi_u8 arsvd[2]; /* 2: 保留字段 */
    bool pairwise;
    const hi_u8 *puc_mac_addr;
} cfg80211_get_key_info_stru;

/* VLAN以太网头 liteos封装 */
typedef struct {
    hi_u8       h_dest[ETHER_ADDR_LEN];
    hi_u8       h_source[ETHER_ADDR_LEN];
    hi_u16      h_vlan_proto;
    hi_u16      h_vlan_tci;
    hi_u16      h_vlan_encapsulated_proto;
} oal_vlan_ethhdr_stru; /* linux 结构体 */


#if (LINUX_VERSION_CODE >= kernel_version(2, 6, 34)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
typedef struct cfg80211_pmksa oal_cfg80211_pmksa_stru;
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
typedef struct oal_cfg80211_ssid_tag {
    hi_u8 ssid[OAL_IEEE80211_MAX_SSID_LEN];
    hi_u8 ssid_len;
    hi_u8 auc_arry[3]; /* 3: SIZE(0..3) */
} oal_cfg80211_ssid_stru;

/* linux-2.6.34内核才有以下两个结构体，加密相关 */
typedef struct oal_cfg80211_connect_params_tag {
    oal_ieee80211_channel_stru *channel;

    hi_u8*                              bssid;
    hi_u8*                              ssid;
    hi_u8*                              ie;

    hi_u32                              ssid_len;
    hi_u32                              ie_len;
    oal_cfg80211_crypto_settings_stru   crypto;
    const hi_u8*                        key;

    oal_nl80211_auth_type_enum_uint8    auth_type;
    hi_u8                               privacy;
    hi_u8                               key_len;
    hi_u8                               key_idx;
    oal_nl80211_mfp_enum_uint8          mfp;
    hi_u8                               auc_resv[3];  /* 3: 保留字节 */
} oal_cfg80211_connect_params_stru;

typedef struct vif_params {
    hi_s32 use_4addr;
    hi_u8 *macaddr;
} oal_vif_params_stru;

typedef struct oal_cfg80211_scan_request_tag {
    oal_cfg80211_ssid_stru*        ssids;
    hi_u32                         n_ssids;
    hi_u32                         n_channels;
    hi_u32                         ie_len;

    /* internal */
    oal_wiphy_stru*                wiphy;
    oal_net_device_stru*           dev;

    struct wireless_dev*           wdev;

    hi_u8                          aborted;
    hi_u8                          prefix_ssid_scan_flag;
    hi_u8                          resv[2];  /* 2: 保留字段 */

    hi_u8*                         ie;

    /* keep last */
    oal_ieee80211_channel_stru*    channels[HI_MAX_SCAN_CHANNELS];
} oal_cfg80211_scan_request_stru;
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX) */

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifndef _PRE_HDF_LINUX
typedef struct cfg80211_connect_params      oal_cfg80211_connect_params_stru;
typedef struct cfg80211_scan_request        oal_cfg80211_scan_request_stru;
#endif
typedef struct cfg80211_sched_scan_request  oal_cfg80211_sched_scan_request_stru;
typedef struct ethtool_ops                  oal_ethtool_ops_stru;
#endif

typedef struct beacon_parameters {
    hi_u8 *head, *tail;
    hi_u32 interval, dtim_period;
    hi_u32 head_len, tail_len;
} oal_beacon_parameters;

typedef struct {
    oal_proc_dir_entry_stru *proc_net;
} oal_net_stru;

typedef struct module oal_module_stru;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
typedef struct {
    hi_u32 nlmsg_len;   /* 消息长度，包括首部在内 */
    hi_u16 nlmsg_type;  /* 消息内容的类型 */
    hi_u16 nlmsg_flags; /* 附加的标志 */
    hi_u32 nlmsg_seq;   /* 序列号 */
    hi_u32 nlmsg_pid;   /* 发送进程的端口ID */
} oal_nlmsghdr_stru;
#endif

/* netlink相关 */
#define OAL_NLMSG_ALIGNTO               4
#define oal_nlmsg_align(_len)           (((_len) + OAL_NLMSG_ALIGNTO - 1) & ~(OAL_NLMSG_ALIGNTO - 1))
#define OAL_NLMSG_HDRLEN                ((hi_s32) oal_nlmsg_align(sizeof(oal_nlmsghdr_stru)))
#define oal_nlmsg_length(_len)          ((_len) + oal_nlmsg_align(OAL_NLMSG_HDRLEN))
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define oal_nlmsg_space(_len)           oal_nlmsg_align(oal_nlmsg_length(_len))
#endif
#define oal_nlmsg_data(_nlh)            ((hi_void*)(((hi_s8*)(_nlh)) + oal_nlmsg_length(0)))
#define oal_nlmsg_payload(_nlh, _len)   ((_nlh)->nlmsg_len - oal_nlmsg_space((_len)))
#define oal_nlmsg_ok(_nlh, _len)        ((_len) >= (hi_s32)(sizeof(oal_nlmsghdr_stru)) && \
        (_nlh)->nlmsg_len >= (sizeof(oal_nlmsghdr_stru)) && \
        (_nlh)->nlmsg_len <= (_len))

/* IPv6伪首部，用于校验和计算（伪首部中下一首部字段的值必须为58） */
typedef struct {
    oal_in6_addr   saddr;
    oal_in6_addr   daddr;
    hi_u32         payload_len;
    hi_u32         nexthdr;
} oal_ipv6_pseudo_hdr_stru;

/* net_device ioctl结构体定义 */
/* hostapd/wpa_supplicant 下发的信息元素结构 */
/* 该结构为事件内存池大小，保存从hostapd/wpa_supplicant下发的ie 信息 */
/* 注意: 整个结构体长度为事件内存池大小，如果事件内存池有修改，则需要同步修改app 数据结构 */
typedef struct oal_app_ie {
    hi_u32                  ie_len;
    en_app_ie_type_uint8    app_ie_type;
    hi_u8                   auc_rsv[3];  /* 3: 保留字段 */
    /* auc_ie 中保存信息元素，长度 = (事件内存池大小 - 保留长度) */
    hi_u8 auc_ie[WLAN_WPS_IE_MAX_SIZE];
} oal_app_ie_stru;

typedef struct oal_dhcp_packet {
    hi_u8           op;          /* packet opcode type */
    hi_u8           htype;       /* hardware addr type */
    hi_u8           hlen;        /* hardware addr length */
    hi_u8           hops;        /* gateway hops */
    hi_u32          xid;         /* transaction ID */
    hi_u16          secs;        /* seconds since boot began */
    hi_u16          flags;       /* flags */
    hi_u32          ciaddr;      /* client IP address */
    hi_u32          yiaddr;      /* 'your' IP address */
    hi_u32          siaddr;      /* server IP address */
    hi_u32          giaddr;      /* gateway IP address */
    hi_u8           chaddr[16];  /* 16: client hardware address */
    hi_u8           sname[64];   /* 64: server host name */
    hi_u8           file[128];   /* 128: boot file name */
    hi_u8           options[4];  /* 4: variable-length options field */
} oal_dhcp_packet_stru;

/* 不分平台通用结构体 */
typedef struct {
    hi_u8 auc_ssid[OAL_IEEE80211_MAX_SSID_LEN]; /* ssid array */
    hi_u8 ssid_len;                             /* length of the array */
    hi_u8 auc_arry[3];                          /* 3: 数组元素个数 */
} oal_ssids_stru;

/* net_device ioctl结构体定义 */
typedef struct oal_net_dev_ioctl_data_tag {
    hi_s32 l_cmd; /* 命令号 */
    union {
        struct {
            hi_u8    auc_mac[OAL_MAC_ADDR_LEN];
            hi_u8    auc_rsv[2];            /* 2: 保留字段 */
            hi_u32   buf_size;              /* 用户空间ie 缓冲大小 */
            hi_u8*   puc_buf;               /* 用户空间ie 缓冲地址 */
        } assoc_req_ie;                         /* AP 模式，用于获取STA 关联请求ie 信息 */

        struct {
            hi_u32 auth_alg;
        } auth_params;

        struct {
            hi_u8 auc_country_code[4]; /* 4: 国家码长度 */
        } country_code;

        hi_u8 ssid[OAL_IEEE80211_MAX_SSID_LEN + 4]; /* 4: 加4 */
        hi_u32 vap_max_user;

        struct {
            hi_s32 l_freq;
            hi_s32 l_channel;
            hi_s32 l_ht_enabled;
            hi_s32 l_sec_channel_offset;
            hi_s32 l_vht_enabled;
            hi_s32 l_center_freq1;
            hi_s32 l_center_freq2;
            hi_s32 l_bandwidth;
        } freq;

        oal_app_ie_stru app_ie; /* beacon ie,index 0; proberesp ie index 1; assocresp ie index 2 */

        struct {
            hi_s32                      l_freq;              /* ap所在频段，与linux-2.6.34内核中定义不同 */
            hi_u32                      ssid_len;            /* SSID 长度 */
            hi_u32                      ie_len;
            hi_u8*                      puc_ie;
            const hi_u8*                puc_ssid;               /* 期望关联的AP SSID  */
            const hi_u8*                puc_bssid;              /* 期望关联的AP BSSID  */
            hi_u8                       privacy;             /* 是否加密标志 */
            oal_nl80211_auth_type_enum_uint8    auth_type;           /* 认证类型，OPEN or SHARE-KEY */

            hi_u8                       wep_key_len;         /* WEP KEY长度 */
            hi_u8                       wep_key_index;       /* WEP KEY索引 */
            const hi_u8*                puc_wep_key;            /* WEP KEY密钥 */
            oal_cfg80211_crypto_settings_stru   crypto;              /* 密钥套件信息 */
        } cfg80211_connect_params;
        struct {
            hi_u8            auc_mac[OAL_MAC_ADDR_LEN];
            hi_u16           us_reason_code;                        /* 去关联 reason code */
        } kick_user_params;

        hi_s32 l_frag; /* 分片门限值 */
        hi_s32 l_rts;  /* RTS 门限值 */
    } pri_data;
} oal_net_dev_ioctl_data_stru;

typedef struct {
    hi_u16 us_ar_hrd;   /* format of hardware address */
    hi_u16 us_ar_pro;   /* format of protocol address */
    hi_u8  ar_hln;   /* length of hardware address */
    hi_u8  ar_pln;   /* length of protocol address */
    hi_u16 us_ar_op;    /* ARP opcode (command) */

    hi_u8 auc_ar_sha[ETHER_ADDR_LEN];         /* sender hardware address */
    hi_u8 auc_ar_sip[ETH_SENDER_IP_ADDR_LEN]; /* sender IP address */
    hi_u8 auc_ar_tha[ETHER_ADDR_LEN];         /* target hardware address */
    hi_u8 auc_ar_tip[ETH_TARGET_IP_ADDR_LEN]; /* target IP address */
} oal_eth_arphdr_stru;

typedef struct oal_eth_header_info_stru {
    hi_u32  type;
    hi_void *daddr;
    hi_void *saddr;
    hi_u32 len;
} oal_eth_header_info_stru;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
struct ieee80211_txrx_stypes {
    hi_u16 tx;
    hi_u16 rx;
};

typedef struct ieee80211_iface_limit {
    hi_u16 max;
    hi_u16 types;
} oal_ieee80211_iface_limit;

typedef struct ieee80211_iface_combination {
    const struct ieee80211_iface_limit*  limits;
    hi_u32                               num_different_channels;
    hi_u16                               max_interfaces;
    hi_u8                                n_limits;
    hi_u8                                radar_detect_widths;
    hi_bool                              beacon_int_infra_match;
    hi_u8                                resv[3]; /* 3: 保留字段 */
} oal_ieee80211_iface_combination;
#endif

typedef struct oal_arp_create_info_stru {
    hi_u32 l_type;
    hi_u32 l_ptype;
    hi_u32 dest_ip;
    hi_u32 src_ip;
    hi_u8 *puc_dest_hw;
    hi_u8 *puc_src_hw;
    hi_u8 *puc_target_hw;
} oal_arp_create_info_stru;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
typedef struct oal_cfg80211_ops_tag {
    hi_s32 (*add_key)(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev,
        cfg80211_add_key_info_stru *p_cfg80211_add_key_info, const hi_u8 *mac_addr, oal_key_params_stru *params);
    hi_s32 (*get_key)(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev,
        cfg80211_get_key_info_stru *p_cfg80211_get_key_info, hi_void *cookie,
        hi_void (*callback)(hi_void *cookie, oal_key_params_stru *key));
    hi_s32 (*del_key)(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index, hi_bool en_pairwise,
        const hi_u8 *mac_addr);
    hi_s32 (*set_default_key)(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index, hi_bool en_unicast,
        hi_bool en_multicast);
    hi_s32 (*set_default_mgmt_key)(oal_wiphy_stru *wiphy, oal_net_device_stru *netdev, hi_u8 key_index);
    hi_s32 (*scan)(oal_wiphy_stru *pst_wiphy, oal_cfg80211_scan_request_stru *pst_request);
    hi_s32 (*connect)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *netdev,
        oal_cfg80211_connect_params_stru *pst_sme);
    hi_s32 (*disconnect)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *netdev, hi_u16 us_reason_code);
    hi_s32 (*set_channel)(oal_wiphy_stru *pst_wiphy, oal_ieee80211_channel *pst_chan,
        oal_nl80211_channel_type en_channel_type);
    hi_s32 (*set_wiphy_params)(oal_wiphy_stru *pst_wiphy, hi_u32 ul_changed);
    hi_s32 (*add_beacon)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *netdev, oal_beacon_parameters *pst_info);
    hi_s32 (*change_virtual_intf)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *netdev, nl80211_iftype_uint8 en_type,
        hi_u32 *pul_flags, oal_vif_params_stru *pst_params);
    hi_s32 (*add_station)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *netdev, hi_u8 *mac_addr, hi_void *sta_parms);
    hi_s32 (*del_station)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *netdev, const hi_u8 *mac_addr);
    hi_s32 (*change_station)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *netdev, hi_u8 *mac_addr,
        hi_void *pst_sta_parms);
    hi_s32 (*get_station)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *netdev, hi_u8 *mac_addr,
        oal_station_info_stru *pst_sta_info);
    hi_s32 (*dump_station)(oal_wiphy_stru *wiphy, oal_net_device_stru *dev, hi_s32 idx, hi_u8 *mac,
        oal_station_info_stru *pst_sta_info);
    hi_s32 (*change_beacon)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *pst_netdev,
        oal_beacon_data_stru *pst_beacon_info);
    hi_s32 (*start_ap)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *pst_netdev,
        oal_ap_settings_stru *pst_ap_settings);
    hi_s32 (*stop_ap)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *pst_netdev);
    hi_s32 (*change_bss)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *pst_netdev, hi_void *pst_bss_params);
    hi_s32 (*set_power_mgmt)(oal_wiphy_stru *pst_wiphy, oal_net_device_stru *pst_ndev, hi_bool enabled,
        hi_s32 ul_timeout);
    hi_s32 (*sched_scan_start)(oal_wiphy_stru *wiphy, oal_net_device_stru *dev,
        struct cfg80211_sched_scan_request *request);
    hi_s32 (*sched_scan_stop)(oal_wiphy_stru *wiphy, oal_net_device_stru *dev);
    hi_s32 (*remain_on_channel)(oal_wiphy_stru *wiphy, struct wireless_dev *wdev, struct ieee80211_channel *chan,
        hi_u32 duration, hi_u64 *cookie);
    hi_s32 (*cancel_remain_on_channel)(oal_wiphy_stru *wiphy, struct wireless_dev *wdev, hi_u64 cookie);
    hi_s32 (*mgmt_tx)(oal_wiphy_stru *wiphy, struct wireless_dev *wdev, struct ieee80211_channel *chan, hi_bool offchan,
        hi_u32 wait, const hi_u8 *buf, size_t len, hi_bool no_cck, hi_bool dont_wait_for_ack, hi_u64 *cookie);
    hi_void (*mgmt_frame_register)(struct wiphy *wiphy, struct wireless_dev *wdev, hi_u16 frame_type, hi_bool reg);
    struct wireless_dev *(*add_virtual_intf)(oal_wiphy_stru *wiphy, const hi_char *name, nl80211_iftype_uint8 type,
        hi_u32 *flags, struct vif_params *params);
    hi_s32 (*del_virtual_intf)(oal_wiphy_stru *wiphy, struct wireless_dev *wdev);
    hi_s32 (*mgmt_tx_cancel_wait)(oal_wiphy_stru *wiphy, struct wireless_dev *wdev, hi_u64 cookie);
    hi_s32 (*start_p2p_device)(oal_wiphy_stru *wiphy, struct wireless_dev *wdev);
    hi_void (*stop_p2p_device)(oal_wiphy_stru *wiphy, struct wireless_dev *wdev);
} oal_cfg80211_ops;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct cfg80211_ops oal_cfg80211_ops_stru;
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */

/* ****************************************************************************
  5 函数声明
**************************************************************************** */
hi_void netif_set_flow_ctrl_status(const oal_net_device_stru *netdev, netif_flow_ctrl_enum_uint8 status);

static inline oal_net_device_stru *oal_get_past_net_device_by_index(hi_u32 netdev_index)
{
    return NetDeviceGetInstByIndex(netdev_index);
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 oal_dev_netlink_create(hi_void);
hi_s32 oal_dev_netlink_send(hi_u8 *data, hi_s32 data_len);
hi_s32 oal_init_dev_excp_handler(hi_void);
hi_void oal_deinit_dev_excp_handler(hi_void);
#endif

hi_s32 oal_ieee80211_channel_to_frequency(hi_s32 l_channel, wlan_channel_band_enum_uint8 band);
hi_s32 oal_ieee80211_frequency_to_channel(hi_s32 l_center_freq);
oal_ieee80211_channel_stru *oal_ieee80211_get_channel(const oal_wiphy_stru *wiphy, hi_s32 freq);
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
oal_net_device_stru *oal_net_alloc_netdev(const hi_char *puc_name, hi_u8 max_name_len);
#endif
oal_net_device_stru *oal_net_alloc_netdev_mqs(const hi_char *puc_name);
hi_void oal_net_clear_netdev(oal_net_device_stru *netdev);
hi_void oal_net_free_netdev(oal_net_device_stru *netdev);
hi_u32 oal_net_register_netdev(oal_net_device_stru *netdev, nl80211_iftype_uint8 type);
hi_void oal_net_unregister_netdev(oal_net_device_stru *netdev);
oal_net_device_stru *oal_get_netdev_by_name(const hi_char *pc_name);
oal_net_device_stru *oal_get_netdevice_by_name(const hi_char *pc_name, hi_u32 len);
hi_u32 oal_net_check_and_get_devname(nl80211_iftype_uint8 type, char *dev_name, hi_u32 *len);
hi_void oal_dev_destroy_all(hi_void);


/* ****************************************************************************
  6 内联函数
**************************************************************************** */
/* ==================BEGIN : Linux wiphy 结构相关的处理函数================== */
/* ****************************************************************************
 功能描述  :

 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
static inline oal_wiphy_stru *oal_wiphy_new(hi_u32 sizeof_priv)
{
    return (oal_wiphy_stru *)oal_kzalloc(sizeof_priv + sizeof(oal_wiphy_stru), 0);
}
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline oal_wiphy_stru *oal_wiphy_new(oal_cfg80211_ops_stru *ops, hi_u32 sizeof_priv)
{
    return wiphy_new(ops, sizeof_priv);
}
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX) */

/* ****************************************************************************
 功能描述  : 注册wiphy
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
hi_void oal_wiphy_register(oal_wiphy_stru *wiphy);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_s32 oal_wiphy_register(oal_wiphy_stru *wiphy)
{
    return wiphy_register(wiphy);
}
#endif
oal_wiphy_stru *oal_wiphy_get(hi_void);

/* ****************************************************************************
 功能描述  :

 修改历史      :
  1.日    期   : 2013年9月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
static inline hi_void oal_wiphy_unregister(hi_void)
{
    return;
}
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_void oal_wiphy_unregister(oal_wiphy_stru *wiphy)
{
    return wiphy_unregister(wiphy);
}
#endif

/* ****************************************************************************
 功能描述  :

 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_void oal_wiphy_free(oal_wiphy_stru *wiphy)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    oal_free(wiphy);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    wiphy_free(wiphy);
#endif
}

/* ****************************************************************************
 功能描述  : 获取私有指针 指向hmac_device_stru指针

 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : HiSilicon
    修改内容   : 由mac_device_stru指向hmac_device_stru
**************************************************************************** */
static inline hi_void *oal_wiphy_priv(oal_wiphy_stru *wiphy)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    return wiphy->priv;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return wiphy_priv(wiphy);
#endif
}

/* ****************************************************************************
 功能描述  :

 修改历史      :
  1.日    期   : 2013年9月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
static inline hi_void oal_wiphy_apply_custom_regulatory(hi_void)
{
    return;
}
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_void oal_wiphy_apply_custom_regulatory(oal_wiphy_stru *wiphy,
    HI_CONST oal_ieee80211_regdomain_stru *regd)
{
    wiphy_apply_custom_regulatory(wiphy, regd);
}
#endif
/* ==================END : Linux wiphy 结构相关的处理函数================== */
/* ****************************************************************************
 功能描述  : 获取协议模式
 输入参数  : pst_netbuf: skb指针
             pst_device: net device结构体指针

 修改历史      :
  1.日    期   : 2012年12月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_u16 oal_eth_type_trans(oal_netbuf_stru *netbuf, oal_net_device_stru *device)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /* 将netbuf的data指针指向mac frame的payload处 --fix by fangbaoshun for liteos */
    /* 此处linux有复杂操作，liteos做了简化，protocol为0，先pull 14字节，后面会用到，最后交给系统之前push回去。 */
    (void)device;
    oal_netbuf_pull(netbuf, sizeof(oal_ether_header_stru));

    return 0;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    return eth_type_trans(netbuf, device);
#else
    return 0;
#endif
}

/* ****************************************************************************
 功能描述  : 网络设备启动接口

 修改历史      :
  1.日    期   : 2012年11月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
static inline hi_void oal_ether_setup(hi_void)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_void oal_ether_setup(oal_net_device_stru *net_device)
#endif
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    if (net_device == HI_NULL) {
        return;
    }

    ether_setup(net_device);
#endif
    return;
}

/* ****************************************************************************
 功能描述  : 调用oal_dev_get_by_name后需要调用dev_put,是net_dev的引用计数减1
 输入参数  : _pst_dev: 指向net_dev的指针

 修改历史      :
  1.日    期   : 2013年2月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
#define oal_dev_put(_pst_dev)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define oal_dev_put(_pst_dev) dev_put(_pst_dev)
#endif

/* ****************************************************************************
 功能描述  : 唤醒所有的队列

 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_void oal_net_tx_wake_all_queues(hi_void)
{
    return;
}

/* ****************************************************************************
 功能描述  : 停止所有的队列

 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_void oal_net_tx_stop_all_queues(hi_void)
{
    return;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : wake网络设备的某个subqueue

 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_void oal_net_wake_subqueue(oal_net_device_stru *netdev, hi_u16 queue_idx)
{
    if (netdev == HI_NULL) {
        return;
    }
#ifndef _PRE_HDF_LINUX
    return netif_wake_subqueue(netdev, queue_idx);
#endif
}

/* ****************************************************************************
 功能描述  : 暂停网络设备的某个subqueue

 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_void oal_net_stop_subqueue(oal_net_device_stru *netdev, hi_u16 queue_idx)
{
    if (netdev == HI_NULL) {
        return;
    }
#ifndef _PRE_HDF_LINUX
    return netif_stop_subqueue(netdev, queue_idx);
#endif
}

static inline oal_net_device_stru *oal_net_alloc_netdev(hi_u32 sizeof_priv, const hi_char *netdev_name, hi_void *set_up)
{
#ifdef _PRE_HDF_LINUX
    if (netdev_name == HI_NULL || set_up == HI_NULL) {
        return HI_NULL;
    }
    oal_net_device_stru *netdev = NetDeviceInit(netdev_name, strlen(netdev_name), WIFI_LINK, FULL_OS);
    if (netdev != NULL) {
        netdev->funType.wlanType = PROTOCOL_80211_IFTYPE_STATION;
    }
    return netdev;
#else
    oal_net_device_stru *tmp_netdev = HI_NULL;

    if ((netdev_name == HI_NULL) || (set_up == HI_NULL)) {
        return HI_NULL;
    }

#if (LINUX_VERSION_CODE >= kernel_version(3, 18, 0))
    tmp_netdev = alloc_netdev(sizeof_priv, netdev_name, NET_NAME_UNKNOWN, set_up);
    if (tmp_netdev == HI_NULL) {
        printk("oal_net_alloc_netdev***[%d]\n", __LINE__);
    }
    return tmp_netdev;
#else
    return alloc_netdev(sizeof_priv, netdev_name, set_up);
#endif
#endif /* ifdef _PRE_HDF_LINUX */
}
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : wake网络设备的某个subqueue

 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_void oal_net_wake_subqueue(const oal_net_device_stru *netdev)
{
    if (netdev == HI_NULL) {
        oal_io_print0("oal_net_wake_subqueue::pst_dev = NULL!!! \r\n");
        return;
    }

    return;
}

/* ****************************************************************************
 功能描述  : 暂停网络设备的某个subqueue

 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_void oal_net_stop_subqueue(const oal_net_device_stru *netdev)
{
    if (netdev == HI_NULL) {
        oal_io_print0("oal_net_stop_subqueue::pst_dev = NULL!!! \r\n");
        return;
    }

    return;
}

/* ****************************************************************************
 功能描述  : net device的open函数
 输入参数  : net device指针
 返 回 值  : 成功或者失败原因

 修改历史      :
  1.日    期   : 2012年12月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_s32 oal_net_device_open(oal_net_device_stru *netdev)
{
    /* netdevice相关接口函数后续统一优化 */
    netdev->flags |= OAL_IFF_RUNNING;

    return HI_SUCCESS;
}
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

/* ****************************************************************************
 功能描述  : net device的close函数
 输入参数  : net device指针
 返 回 值  : 成功或者失败原因

 修改历史      :
  1.日    期   : 2012年12月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_s32 oal_net_device_close(oal_net_device_stru *netdev)
{
    /* netdevice相关接口函数后续统一优化 */
    oal_netdevice_flags(netdev) &= ~OAL_IFF_RUNNING;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : net device的初始化函数
 输入参数  : net device指针
 返 回 值  : 成功或者失败原因

 修改历史      :
  1.日    期   : 2012年12月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
/* 作为全局变量g_wal_net_dev_ops成员引用，不用const修饰，lin_t e818告警屏蔽 */
static inline hi_s32 oal_net_device_init(oal_net_device_stru *netdev)
{
    /* netdevice相关接口函数后续统一优化 */
    if (netdev == HI_NULL) {
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 将skb发给桥
 输入参数  : pst_netbuf: skb指针
 返 回 值  : 1，drop；0，succ

 修改历史      :
  1.日    期   : 2012年12月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u32 oal_netif_rx(oal_netbuf_stru *netbuf)
{
    return NetIfRx((struct NetDevice *)netbuf->dev, netbuf);
}

/* ****************************************************************************
 功能描述  : 将skb发给桥  !in_interrupt()
 输入参数  : pst_netbuf: skb指针
 返 回 值  : 1，drop；0，succ

 修改历史      :
  1.日    期   : 2012年12月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u32 oal_netif_rx_ni(oal_netbuf_stru *netbuf)
{
    return NetIfRxNi((struct NetDevice *)netbuf->dev, netbuf);
}


/* ****************************************************************************
 功能描述  : 创建netlink
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
oal_sock_stru *oal_netlink_kernel_create(hi_void);
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 创建netlink
**************************************************************************** */
static inline oal_sock_stru *oal_netlink_kernel_create(hi_void (*input)(oal_netbuf_stru *pst_netbuf))
{
    struct netlink_kernel_cfg cfg;

    memset_s(&cfg, sizeof(cfg), 0, sizeof(cfg));
    cfg.groups = 0;
    cfg.input = input;
    cfg.cb_mutex = NULL;

    return netlink_kernel_create(&init_net, NETLINK_DEV_ERROR, &cfg);
}
#endif

/* ****************************************************************************
 功能描述  : 释放资源
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_void oal_netlink_kernel_release(hi_void);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_void oal_netlink_kernel_release(hi_void)
{
    return;
}
#endif

/* ****************************************************************************
 功能描述  : 获取nlmsg头

 修改历史      :
  1.日    期   : 2013年10月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline oal_nlmsghdr_stru *oal_nlmsg_hdr(const oal_netbuf_stru *netbuf)
{
    return (oal_nlmsghdr_stru *)oal_netbuf_header(netbuf);
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline oal_nlmsghdr_stru *oal_nlmsg_put(oal_netbuf_stru *netbuf, hi_u32 pid, hi_u32 ul_seq, hi_u32 type,
    hi_u32 payload, hi_u32 flags)
{
    return nlmsg_put(netbuf, pid, ul_seq, type, payload, flags);
}
#endif

/* ****************************************************************************
 功能描述  : 获取nlmsg消息大小

 修改历史      :
  1.日    期   : 2013年10月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_s32 oal_nlmsg_msg_size(hi_s32 l_payload)
{
    return OAL_NLMSG_HDRLEN + l_payload;
}

/* ****************************************************************************
 功能描述  : 获取nlmsg大小

 修改历史      :
  1.日    期   : 2013年10月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_s32 oal_nlmsg_total_size(hi_s32 l_payload)
{
    return oal_nlmsg_align(oal_nlmsg_msg_size(l_payload));
}

/* ****************************************************************************
 功能描述  : 释放单播资源

 修改历史      :
  1.日    期   : 2013年10月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
static inline hi_s32 oal_netlink_unicast(oal_netbuf_stru *netbuf)
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_s32 oal_netlink_unicast(oal_sock_stru *pst_sock, oal_netbuf_stru *netbuf, hi_u32 pid, hi_u32 nonblock)
#endif
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    oal_netbuf_free(netbuf);
    return HI_SUCCESS;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return netlink_unicast(pst_sock, netbuf, pid, nonblock);
#endif
}

/* ****************************************************************************
 功能描述  : 获取arp头的长度

 修改历史      :
  1.日    期   : 2014年8月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_s32 oal_arp_hdr_len(const oal_net_device_stru *netdev)
{
    /* ARP header, plus 2 device addresses, plus 2 IP addresses. */
    return sizeof(oal_eth_arphdr_stru) +
        (netdev->addrLen + sizeof(hi_u32)) * 2; /* ARP header, plus 2 device addresses */
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_net.h */
