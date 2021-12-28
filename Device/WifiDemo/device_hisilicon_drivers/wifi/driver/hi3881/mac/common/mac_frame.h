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

#ifndef __MAC_FRAME_H__
#define __MAC_FRAME_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "wlan_types.h"
#include "oam_ext_if.h"
#include "wlan_mib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define MAC_IEEE80211_FCTL_FTYPE 0x000c /* 帧类型掩码 */
#define MAC_IEEE80211_FCTL_STYPE 0x00f0 /* 帧子类型掩码 */

#define MAC_IEEE80211_FTYPE_MGMT 0x0000 /* 管理帧 */
#define MAC_IEEE80211_FTYPE_CTL  0x0004  /* 控制帧 */
#define MAC_IEEE80211_FTYPE_DATA 0x0008 /* 数据帧 */

/* A-MSDU情况下，submsdu的偏移宏 */
#define MAC_SUBMSDU_HEADER_LEN          14          /* |da = 6|sa = 6|len = 2| submsdu的头的长度 */
#define MAC_SUBMSDU_LENGTH_OFFSET       12          /* submsdu的长度字段的偏移值 */
#define MAC_SUBMSDU_DA_OFFSET           0           /* submsdu的目的地址的偏移值 */
#define MAC_SUBMSDU_SA_OFFSET           6           /* submsdu的源地址的偏移值 */

#define MAC_80211_FRAME_LEN                 24      /* 非四地址情况下，MAC帧头的长度 */
#define MAC_80211_CTL_HEADER_LEN            16      /* 控制帧帧头长度 */
#define MAC_80211_4ADDR_FRAME_LEN           30
#define MAC_80211_QOS_FRAME_LEN             26
#define MAC_80211_QOS_4ADDR_FRAME_LEN       32
#define MAC_80211_QOS_HTC_4ADDR_FRAME_LEN   36

/* 信息元素长度定义 */
#define MAC_IE_HDR_LEN              2   /* 信息元素头部 1字节EID + 1字节长度 */
#define MAC_TIME_STAMP_LEN          8
#define MAC_BEACON_INTERVAL_LEN     2
#define MAC_CAP_INFO_LEN            2
#define MAC_SSID_OFFSET             12
#define MAC_LISTEN_INT_LEN          2
#define MAC_MAX_SUPRATES            8   /* WLAN_EID_RATES最大支持8个速率 */
#define MAC_DSPARMS_LEN             1   /* ds parameter set 长度 */
#define MAC_DEFAULT_TIM_LEN         4
#define MAC_TIM_LEN_EXCEPT_PVB      3   /* DTIM Period、DTIM Count与BitMap Control三个字段的长度 */
#define MAC_CONTRY_CODE_LEN         3   /* 国家码长度为3 */
#define MAC_PWR_CONSTRAINT_LEN      1   /* 功率限制ie长度为1 */
#define MAC_QUIET_IE_LEN            6   /* quiet信息元素长度 */
#define MAC_TPCREP_IE_LEN           2
#define MAC_ERP_IE_LEN              1
#define MAC_OBSS_SCAN_IE_LEN        14
#define MAC_XCAPS_LEN               1
#define MAC_XCAPS_EX_LEN            8   /* 初始值为5，由于11ac Operating Mode Notification特性标志为bit62 长度修改为8 */
#define MAC_WMM_PARAM_LEN           24  /* WMM parameters ie */
#define MAC_WMM_INFO_LEN            7   /* WMM info ie */
#define MAC_QOS_INFO_LEN            1
#define MAC_AC_PARAM_LEN            4
#define MAC_BSS_LOAD_IE_LEN         5
#define MAC_COUNTRY_REG_FIELD_LEN   3
#define MAC_LIS_INTERVAL_IE_LEN     2   /* listen interval信息元素长度 */
#define MAC_AID_LEN                 2
#define MAC_PWR_CAP_LEN             2
#define MAC_AUTH_ALG_LEN            2
#define MAC_AUTH_TRANS_SEQ_NUM_LEN  2   /* transaction seq num信息元素长度 */
#define MAC_STATUS_CODE_LEN         2
#define MAC_VHT_CAP_IE_LEN          12
#define MAC_VHT_INFO_IE_LEN         5
#define MAC_VHT_CAP_INFO_FIELD_LEN  4
#define MAC_TIMEOUT_INTERVAL_INFO_LEN           5
#define MAC_VHT_CAP_RX_MCS_MAP_FIELD_LEN        2  /* vht cap ie rx_mcs_map field length */
#define MAC_VHT_CAP_RX_HIGHEST_DATA_FIELD_LEN   2  /* vht cap ie rx_highest_data field length */
#define MAC_VHT_CAP_TX_MCS_MAP_FIELD_LEN        2  /* vht cap ie tx_mcs_map field length */
#define MAC_VHT_OPERN_INFO_FIELD_LEN            3  /* vht opern ie information field length */
#define MAC_2040_COEX_LEN                       1  /* 20/40 BSS Coexistence element */
#define MAC_2040_INTOLCHREPORT_LEN_MIN          1  /* 20/40 BSS Intolerant Channel Report element */
#define MAC_CHANSWITCHANN_LEN                   3  /* Channel Switch Announcement element */
#define MAC_SA_QUERY_LEN                        4  /* SA Query element len */
#define MAC_RSN_VERSION_LEN                     2  /* wpa/RSN version len */
#define MAC_RSN_CIPHER_COUNT_LEN                2  /* RSN IE Cipher count len */
#define MAC_11N_TXBF_CAP_OFFSET                 23
#define MAC_HT_NOTIFY_CHANNEL_WIDTH_LEN         3
#define MAC_WIDE_BW_CH_SWITCH_IE_LEN            3
#define MAC_CHANSWITCHANN_IE_LEN                3
#define MAC_EXT_CHANSWITCHANN_IE_LEN            4
#define MAC_MIN_TIM_LEN                         4
#define MAC_MIN_RSN_LEN                         2
#define MAC_MIN_WPS_IE_LEN                      5
#define MAC_VHT_OPERN_LEN                       5  /* vht opern ie length */
#define MAC_MIN_XRATE_LEN                       1
#define MAC_SEC_CH_OFFSET_IE_LEN                1
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
#define MAC_IE_VENDOR_SPEC_MESH_HDR_LEN 7 /* Element ID(1) + Length(1) + OUI(3) + TYPE(1) +SUBTYPE(1) */
#define MAC_IE_VENDOR_SPEC_MESH_MIN_LEN 5 /* OUI(3) + TYPE(1) +SUBTYPE(1) */
#define MAC_IE_VENDOR_SPEC_MESH_SUBTYPE_POS 6
#define MAC_ACTION_CATEGORY_LEN 1
#define MAC_ACTION_CODE_LEN 1
#define MAC_MIN_MESH_CONF_LEN 7
#endif
/* LLC头相关信息 */
#define MAC_LLC_HEARDER_LEN        8      /* LLC头长度 */
#define MAC_PROTOCOL_TYPE_LEN      2      /* 协议族类型长度 */
#define MAC_PROTOCOL_TYPE_IP       0x0800 /* IP协议族 */

/* IP头相关信息 */
#define MAC_IP_HEADER_LEN          20     /* IP头固定字段的长度 */
#define MAC_IP_ADDRE_LEN           4      /* IP地址的长度 */
#define MAC_PROTOCOL_TYPE_TCP      0x06

/* TCP头相关信息 */
#define MAC_TCP_HEADER_LEN         20     /* TCP头的固定字段的长度 */
#define MAC_TCP_FLAGS              13     /* TCP flags字段距TCP头首部的长度 */
/* UDP头相关信息 */
#define MAC_UDP_HEADER_LEN         8      /* UDP头的固定字段的长度 */

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
#define MAC_OPMODE_NOTIFY_LEN       1              /* Operating Mode Notification element len */
#endif

#define MAC_P2P_ATTRIBUTE_HDR_LEN    3   /* P2P_ATTRIBUTE信息元素头部 1字节ATTRIBUTE + 2字节长度 */
#define MAC_P2P_LISTEN_CHN_ATTR_LEN  5   /* LISTEN CHANNEL ATTRIBUTE长度 */
#define MAC_P2P_MIN_IE_LEN           4   /* P2P IE的最小长度 */
#define MAC_P2P_WILDCARD_SSID        8   /* P2P设备ssid标志长度 */

/* Quiet 信息 */
#define MAC_QUIET_PERIOD            0
#define MAC_QUIET_COUNT             MAC_QUIET_PERIOD
#define MAC_QUIET_DURATION          0x0000
#define MAC_QUIET_OFFSET            0x0000

/* RSN信息元素相关定义 */
#define MAC_RSN_IE_VERSION          1
#define MAC_RSN_CAP_LEN             2
#define MAC_PMKID_LEN               16

/* WPA 信息元素相关定义 */
#define MAC_WPA_IE_VERSION          1
#define WLAN_AKM_SUITE_WAPI_CERT    0x000FAC12

/* OUI相关定义 */
#define MAC_OUI_LEN                 3

#define MAC_OUITYPE_WPA             1
#define MAC_OUITYPE_WMM             2
#define MAC_OUITYPE_WPS             4
#define MAC_OUITYPE_P2P             9

/* sizes for DHCP options */
#ifndef DHCP_CHADDR_LEN
#define DHCP_CHADDR_LEN         16
#endif
#define SERVERNAME_LEN          64
#define BOOTFILE_LEN            128
#define DHCP_UDP_LEN            (14 + 20 + 8)
#define DHCP_FIXED_LEN          (DHCP_UDP_LEN + 226)
#define DHCP_OPTION_LEN         72


#ifdef _PRE_WLAN_FEATURE_BW_HIEX
#define MAC_OUITYPE_DBAC              5
#define MAC_OUISUBTYPE_DBAC_NOA       1
#endif

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
#define MAC_OUITYPE_MESH 2
#define MAC_OUISUBTYPE_MESH_HISI_RSN 1
#define MAC_OUISUBTYPE_MESH_HISI_OPTIMIZATION 2
#define MAC_OUISUBTYPE_MESH_HISI_REQ 3
#define MAC_OUISUBTYPE_MESH_HISI_STA_REQ 4
#define MAC_OUISUBTYPE_MESH_HISI_BEACON 5
#define MAC_OUISUBTYPE_MESH_HISI_RSP 6
#define MAC_OUISUBTYPE_MESH_HISI_MESHID 7
#endif

/* 华为OUI,公共定义 */
#define MAC_WLAN_OUI_VENDOR                 0x00E0FC
#define MAC_WLAN_OUI_VENDOR0                0x00        /* VENDOR OUI 定义 0x00E0FC */
#define MAC_WLAN_OUI_VENDOR1                0xE0
#define MAC_WLAN_OUI_VENDOR2                0xFC
#define MAC_WLAN_OUI_RSN0                   0x00        /* RSNA OUI 定义 0x000FAC */
#define MAC_WLAN_OUI_RSN1                   0x0F
#define MAC_WLAN_OUI_RSN2                   0xAC
#define MAC_WLAN_OUI_MICRO0                 0x00        /* WPA/WMM OUI 定义 0x0050F2 */
#define MAC_WLAN_OUI_MICRO1                 0x50
#define MAC_WLAN_OUI_MICRO2                 0xF2
#define MAC_WLAN_OUI_P2P0                   0x50        /* P2P OUI 定义 0x506F9A */
#define MAC_WLAN_OUI_P2P1                   0x6F
#define MAC_WLAN_OUI_P2P2                   0x9A
#define MAC_OUITYPE_ANY                     0xFD
#ifdef _PRE_WLAN_FEATURE_ANY_ROM
#define MAC_ANY_STA_TYPE                    0x5
#define MAC_ANY_AP_TYPE                     0x6
#define MAC_ANY_COOKIE_LEN                  8
#endif
#define MAC_ACTION_OUI_POS                  1
#define MAC_ACTION_VENDOR_TYPE_POS          4
#define MAC_ACTION_VENDOR_SUBTYPE_POS       5
#define MAC_ACTION_VENDOR_SPECIFIC_IE_POS   6

#define MAC_WMM_OUI_BYTE_ONE        0x00
#define MAC_WMM_OUI_BYTE_TWO        0x50
#define MAC_WMM_OUI_BYTE_THREE      0xF2
#define MAC_WMM_UAPSD_ALL           (BIT0 | BIT1 | BIT2 | BIT3)
#define MAC_OUISUBTYPE_WMM_INFO     0
#define MAC_OUISUBTYPE_WMM_PARAM    1
#define MAC_OUISUBTYPE_WMM_PARAM_OFFSET 6 /* wmm 字段中EDCA_INFO位置,表示是否携带EDCA参数 偏移6 */
#define MAC_WMM_QOS_INFO_POS        8   /* wmm 字段中qos info位置，偏移8 */
#define MAC_OUI_WMM_VERSION         1
#define MAC_HT_CAP_LEN              26  /* HT能力信息长度为26 */
#define MAC_HT_CAPINFO_LEN          2   /* HT Capabilities Info域长度为2 */
#define MAC_HT_AMPDU_PARAMS_LEN     1   /* A-MPDU parameters域长度为1 */
#define MAC_HT_SUP_MCS_SET_LEN      16  /* Supported MCS Set域长度为16 */
#define MAC_HT_EXT_CAP_LEN          2   /* Extended cap.域长度为2 */
#define MAC_HT_TXBF_CAP_LEN         4   /* Transmit Beamforming Cap.域长度为4 */
#define MAC_HT_ASEL_LEN             1   /* ASEL Cap.域长度为1 */
#define MAC_HT_OPERN_LEN            22  /* HT Operation信息长度为22 */
#define MAC_HT_BASIC_MCS_SET_LEN    16  /* HT info中的basic mcs set信息的长度 */
#define MAC_HT_CTL_LEN              4   /* HT CONTROL字段的长度 */
#define MAC_QOS_CTL_LEN             2   /* QOS CONTROL字段的长度 */
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
#define MAC_MESH_CONF_LEN                   7           /* Mesh Configuration 字段长度为7 */
#define MAC_MESH_FORMATION_LEN              1           /* Mesh Formation Field 长度为1 */
#define MAC_MESH_CAPABILITY_LEN             1           /* Mesh Capability Field 长度为1 */
#define MAC_MESH_HISI_OPTIMIZATION_LEN      3           /* Mesh Hisi Optimization 长度为3 */
#define MAC_OUISUBTYPE_LEN                  1           /* Mesh OUISUBTYPE 长度为1 */
#define MAC_RPL_TYPE                        155         /* ICMPv6报文中RPL协议的type值 */
#define MAC_MESH_MESHID_LEN_POS             7           /* Hisi Mesh id长度字段在私有字段中的位置 */
#define MAC_MESH_MESHID_OFFSET              8           /* Hisi Mesh id 字段Meshid内容的起始位置 */
#define MAC_MESH_HISI_BEACON_PRIO_POS       7           /* Hisi Beacon Prio字段在私有字段中的位置 */
#define MAC_MESH_HISI_IS_MBR_POS            8           /* Hisi En_is_mbr字段在私有字段中的位置 */
#define MAC_MESH_HISI_ACCEPT_STA_POS        9           /* Hisi accept_sta字段在私有字段中的位置 */
#endif

#define MAC_QOS_CTRL_FIELD_OFFSET           24
#define MAC_QOS_CTRL_FIELD_OFFSET_4ADDR     30

#define MAC_TAG_PARAM_OFFSET               (MAC_80211_FRAME_LEN + MAC_TIME_STAMP_LEN +\
                                            MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN)

#define MAC_DEVICE_BEACON_OFFSET            (MAC_TIME_STAMP_LEN +\
                                            MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN)
#define MAC_LISTEN_INTERVAL_MAX_LEN         10  /* 定义关联STA最大LISTEN INTERVAL的值 */

#define MAC_MAX_START_SPACING               7

/* EDCA参数相关的宏 */
#define MAC_WMM_QOS_PARAM_AIFSN_MASK                       0x0F
#define MAC_WMM_QOS_PARAM_ACI_BIT_OFFSET                   5
#define MAC_WMM_QOS_PARAM_ACI_MASK                         0x03
#define MAC_WMM_QOS_PARAM_ECWMIN_MASK                      0x0F
#define MAC_WMM_QOS_PARAM_ECWMAX_MASK                      0xF0
#define MAC_WMM_QOS_PARAM_ECWMAX_BIT_OFFSET                4
#define MAC_WMM_QOS_PARAM_TXOPLIMIT_MASK                   0x00FF
#define MAC_WMM_QOS_PARAM_BIT_NUMS_OF_ONE_BYTE             8
#define MAC_WMM_QOS_PARAM_TXOPLIMIT_SAVE_TO_TRANS_TIMES    5

/* 关闭WMM后，所有帧进入此宏定义的队列 */
#define MAC_WMM_SWITCH_TID                                 6

/* TCP协议类型，chartiot tcp连接端口号 */
#define MAC_TCP_PROTOCAL                                   6
#define MAC_UDP_PROTOCAL                                   17
#define MAC_CHARIOT_NETIF_PORT                             10115
#define MAC_WFD_RTSP_PORT                                  7236

/* ICMP协议报文 */
#define MAC_ICMP_PROTOCAL                                  1

#define MAC_VENDER_IE                                      0xAC853D /* 打桩HW IE */
#define MAC_HISI_HISTREAM_IE                               0x11     /* histream IE */

#define MAC_IPV6_UDP_SRC_PORT                               546
#define MAC_IPV6_UDP_DES_PORT                               547

/* ARP types, 1: ARP request, 2:ARP response, 3:RARP request, 4:RARP response */
#define MAC_ARP_REQUEST         0x0001
#define MAC_ARP_RESPONSE        0x0002
#define MAC_RARP_REQUEST        0x0003
#define MAC_RARP_RESPONSE       0x0004

/* Neighbor Discovery */
#define MAC_ND_RSOL     133 /* Router Solicitation */
#define MAC_ND_RADVT    134 /* Router Advertisement */
#define MAC_ND_NSOL     135 /* Neighbor Solicitation */
#define MAC_ND_NADVT    136 /* Neighbor Advertisement */
#define MAC_ND_RMES     137 /* Redirect Message */

/* DHCP message types */
#define MAC_DHCP_DISCOVER       1
#define MAC_DHCP_OFFER          2
#define MAC_DHCP_REQUEST        3
#define MAC_DHCP_DECLINE        4
#define MAC_DHCP_ACK            5
#define MAC_DHCP_NAK            6
#define MAC_DHCP_RELEASE        7
#define MAC_DHCP_INFORM         8

/* DHCPV6 Message type */
#define MAC_DHCPV6_SOLICIT      1
#define MAC_DHCPV6_ADVERTISE    2
#define MAC_DHCPV6_REQUEST      3
#define MAC_DHCPV6_CONFIRM      4
#define MAC_DHCPV6_RENEW        5
#define MAC_DHCPV6_REBIND       6
#define MAC_DHCPV6_REPLY        7
#define MAC_DHCPV6_RELEASE      8
#define MAC_DHCPV6_DECLINE      9
#define MAC_DHCPV6_RECONFIGURE  10
#define MAC_DHCPV6_INFORM_REQ   11
#define MAC_DHCPV6_RELAY_FORW   12
#define MAC_DHCPV6_RELAY_REPLY  13
#define MAC_DHCPV6_LEASEQUERY   14
#define MAC_DHCPV6_LQ_REPLY     15

/* IGMP record type */
#define MAC_IGMP_QUERY_TYPE       0x11
#define MAC_IGMPV1_REPORT_TYPE    0x12
#define MAC_IGMPV2_REPORT_TYPE    0x16
#define MAC_IGMPV2_LEAVE_TYPE     0x17
#define MAC_IGMPV3_REPORT_TYPE    0x22


/* V3 group record types [grec_type] */
#define IGMPV3_MODE_IS_INCLUDE        1
#define IGMPV3_MODE_IS_EXCLUDE        2
#define IGMPV3_CHANGE_TO_INCLUDE      3
#define IGMPV3_CHANGE_TO_EXCLUDE      4
#define IGMPV3_ALLOW_NEW_SOURCES      5
#define IGMPV3_BLOCK_OLD_SOURCES      6

#define mac_is_golden_ap(puc_bssid) ((0x0 == (puc_bssid)[0]) && (0x13 == (puc_bssid)[1]) && (0xE9 == (puc_bssid)[2]))

/* p2p相关 */
/* GO negotiation */
#define P2P_PAF_GON_REQ     0
#define P2P_PAF_GON_RSP     1
#define P2P_PAF_GON_CONF    2
/* Provision discovery */
#define P2P_PAF_PD_REQ      7
/* P2P IE */
#define P2P_OUI_LEN         4
#define P2P_IE_HDR_LEN      6
#define P2P_ELEMENT_ID_SIZE 1
#define P2P_ATTR_ID_SIZE    1
#define P2P_ATTR_HDR_LEN    3
#define WFA_OUI_BYTE1       0x50
#define WFA_OUI_BYTE2       0x6F
#define WFA_OUI_BYTE3       0x9A
#define WFA_P2P_V1_0        0x09

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
typedef enum {
    MAC_AP_TYPE_NORMAL   = 0,
    MAC_AP_TYPE_GOLDENAP = 1,

    MAC_AP_TYPE_BUTT
} mac_ap_type_enum;
typedef hi_u8 mac_ap_type_enum_uint8;

/* ****************************************************************************
  枚举名  : wlan_ieee80211_frame_type_enum_uint8
  协议表格:
  枚举说明: 802.11 MAC帧头类型
**************************************************************************** */
typedef enum {
    MAC_IEEE80211_BASE_FRAME = 0,      /* 基础802.11帧类型 */
    MAC_IEEE80211_QOS_FRAME,           /* QoS帧类型 */
    MAC_IEEE80211_QOS_HTC_FRAME,       /* QoS + HTC帧类型 */
    MAC_IEEE80211_ADDR4_FRAME,         /* 四地址帧类型 */
    MAC_IEEE80211_QOS_ADDR4_FRAME,     /* QoS四地址帧类型 */
    MAC_IEEE80211_QOS_HTC_ADDR4_FRAME, /* QoS + HTC四地址帧类型 */

    MAC_IEEE80211_FRAME_BUTT
} mac_ieee80211_frame_type_enum;
typedef hi_u8 mac_ieee80211_frame_type_enum_uint8;

/* Action Frames: Category字段枚举 */
typedef enum {
    MAC_ACTION_CATEGORY_SPECMGMT                    = 0,
    MAC_ACTION_CATEGORY_QOS                         = 1,
    MAC_ACTION_CATEGORY_DLS                         = 2,
    MAC_ACTION_CATEGORY_BA                          = 3,
    MAC_ACTION_CATEGORY_PUBLIC                      = 4,
    MAC_ACTION_CATEGORY_RADIO_MEASURMENT            = 5,
    MAC_ACTION_CATEGORY_FAST_BSS_TRANSITION         = 6,
    MAC_ACTION_CATEGORY_HT                          = 7,
    MAC_ACTION_CATEGORY_SA_QUERY                    = 8,
    MAC_ACTION_CATEGORY_PROTECTED_DUAL_OF_ACTION    = 9,
    MAC_ACTION_CATEGORY_WNM                         = 10,
    MAC_ACTION_CATEGORY_MESH                        = 13,
    MAC_ACTION_CATEGORY_MULTIHOP                    = 14,
    MAC_ACTION_CATEGORY_SELF_PROTECTED              = 15,
    MAC_ACTION_CATEGORY_WFA                         = 17,
    MAC_ACTION_CATEGORY_VHT                         = 21,
    MAC_ACTION_CATEGORY_VENDOR_SPECIFIC_PROTECTED   = 126,
    MAC_ACTION_CATEGORY_VENDOR                      = 127,
}mac_action_category_enum;
typedef hi_u8 mac_category_enum_uint8;

/* HT Category下的Action值的枚举 */
typedef enum {
    MAC_HT_ACTION_NOTIFY_CHANNEL_WIDTH        = 0,
    MAC_HT_ACTION_SMPS                        = 1,
    MAC_HT_ACTION_PSMP                        = 2,
    MAC_HT_ACTION_SET_PCO_PHASE               = 3,
    MAC_HT_ACTION_CSI                         = 4,
    MAC_HT_ACTION_NON_COMPRESSED_BEAMFORMING  = 5,
    MAC_HT_ACTION_COMPRESSED_BEAMFORMING      = 6,
    MAC_HT_ACTION_ASEL_INDICES_FEEDBACK       = 7,

    MAC_HT_ACTION_BUTT
} mac_ht_action_type_enum;
typedef hi_u8 mac_ht_action_type_enum_uint8;
/* Timeout_Interval ie中的类型枚举 */
typedef enum {
    MAC_TIE_REASSOCIATION_DEADLINE_INTERVAL  = 1,   /* 毫秒级 */
    MAC_TIE_KEY_LIFETIME_INTERVAL            = 2,   /* 秒级 */
    MAC_TIE_ASSOCIATION_COMEBACK_TIME        = 3,   /* 毫秒级 */

    MAC_TIE_BUTT
} mac_timeout_interval_type_enum;
typedef hi_u8 mac_timeout_interval_type_enum_uint8;

/* SA QUERY Category下的Action值的枚举 */
typedef enum {
    MAC_SA_QUERY_ACTION_REQUEST          = 0,
    MAC_SA_QUERY_ACTION_RESPONSE         = 1
}mac_sa_query_action_type_enum;
typedef hi_u8 mac_sa_query_action_type_enum_uint8;
typedef enum {
    MAC_FT_ACTION_REQUEST          = 1,
    MAC_FT_ACTION_RESPONSE         = 2,
    MAC_FT_ACTION_CONFIRM          = 3,
    MAC_FT_ACTION_ACK              = 4,
    MAC_FT_ACTION_BUTT             = 5
}mac_ft_action_type_enum;
typedef hi_u8 mac_ft_action_type_enum_uint8;

/* VHT Category下的Action值的枚举 */
typedef enum {
    MAC_VHT_ACTION_COMPRESSED_BEAMFORMING   = 0,
    MAC_VHT_ACTION_GROUPID_MANAGEMENT       = 1,
    MAC_VHT_ACTION_OPREATE_MODE_NOTIFY      = 2,

    MAC_VHT_ACTION_BUTT
} mac_vht_action_type_enum;
typedef hi_u8 mac_vht_action_type_enum_uint8;

/* 校准模式的枚举 */
typedef enum {
    MAC_NOT_SURPPORT_CLB = 0,
    MAC_RSP_CLB_ONLY     = 1,
    MAC_SUPPOTR_CLB      = 3,
    MAC_CLB_BUTT
} mac_txbf_clb_enum;
typedef hi_u8 mac_txbf_clb_enum_uint8;

/* Spectrum Management Category下的Action枚举值 */
typedef enum {
    MAC_SPEC_CH_SWITCH_ANNOUNCE = 4 /*  Channel Switch Announcement */
} mac_specmgmt_action_type_enum;
typedef hi_u8 mac_specmgmt_action_type_enum_uint8;

/* BlockAck Category下的Action值的枚举 */
typedef enum {
    MAC_BA_ACTION_ADDBA_REQ       = 0,
    MAC_BA_ACTION_ADDBA_RSP       = 1,
    MAC_BA_ACTION_DELBA           = 2,

    MAC_BA_ACTION_BUTT
} mac_ba_action_type_enum;
typedef hi_u8 mac_ba_action_type_enum_uint8;

/* Public Category下的Action枚举值 */
typedef enum {
    MAC_PUB_COEXT_MGMT            = 0,  /* 20/40 BSS Coexistence Management */
    MAC_PUB_EX_CH_SWITCH_ANNOUNCE = 4,  /* Extended Channel Switch Announcement */
    MAC_PUB_VENDOR_SPECIFIC       = 9,
    MAC_PUB_GAS_INIT_RESP         = 11,  /* public Action: GAS Initial Response(0x0b) */
    MAC_PUB_GAS_COMBAK_RESP       = 13   /* public Action: GAS Comeback Response(0x0d) */
}mac_public_action_type_enum;
typedef hi_u8 mac_public_action_type_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* Self-Protected Category Action枚举值 */
typedef enum {
    MAC_SP_ACTION_MESH_PEERING_OPEN = 1,
    MAC_SP_ACTION_MESH_PEERING_CONFIRM = 2,
    MAC_SP_ACTION_MESH_PEERING_CLOSE = 3,
    MAC_SP_ACTION_MESH_GROUP_KEY_INFORM = 4,
    MAC_SP_ACTION_MESH_GROUP_KEY_ACK = 5,

    MAC_SP_ACTION_BUTT
} mac_sp_action_type_enum;
typedef hi_u8 mac_sp_action_type_enum_uint8;

/* Mesh Category Action枚举值 */
typedef enum {
    MAC_MESH_ACTION_MESH_LINK_METRIC_REPORT = 0,
    MAC_MESH_ACTION_HWMP_MESH_PATH_SELECT,
    MAC_MESH_ACTION_GATE_ANNOUNCE,
    MAC_MESH_ACTION_CONGESTION_CTRL_NOTIFI,
    MAC_MESH_ACTION_MCCA_SETUP_REQ,
    MAC_MESH_ACTION_MCCA_SETUP_REP,
    MAC_MESH_ACTION_MCCA_ADV_REQ,
    MAC_MESH_ACTION_MCCA_ADV,
    MAC_MESH_ACTION_MCCA_TEARDOWN,
    MAC_MESH_ACTION_TBTT_ADJ_REQ,
    MAC_MESH_ACTION_TBTT_ADJ_RSP,

    MAC_MESH_ACTION_BUTT
} mac_mesh_action_type_enum;
#endif
/* 802.11n下的私有请求 */
typedef enum {
    MAC_A_MPDU_START = 0,
    MAC_A_MPDU_END = 1,

    MAC_A_MPDU_BUTT
} mac_priv_req_11n_enum;
typedef hi_u8 mac_priv_req_11n_enum_uint8;

/* Block ack的确认类型 */
typedef enum {
    MAC_BACK_BASIC = 0,
    MAC_BACK_COMPRESSED = 2,
    MAC_BACK_MULTI_TID = 3,

    MAC_BACK_BUTT
} mac_back_variant_enum;
typedef hi_u8 mac_back_variant_enum_uint8;

/* ACTION帧中，各个域的偏移量 */
typedef enum {
    MAC_ACTION_OFFSET_CATEGORY = 0,
    MAC_ACTION_OFFSET_ACTION = 1,
} mac_action_offset_enum;
typedef hi_u8 mac_action_offset_enum_uint8;

/* Reason Codes for Deauthentication and Disassociation Frames */
typedef enum {
    MAC_UNSPEC_REASON           = 1,
    MAC_AUTH_NOT_VALID          = 2,
    MAC_DEAUTH_LV_SS            = 3,
    MAC_INACTIVITY              = 4,
    MAC_AP_OVERLOAD             = 5,
    MAC_NOT_AUTHED              = 6,
    MAC_NOT_ASSOCED             = 7,
    MAC_DISAS_LV_SS             = 8,
    MAC_ASOC_NOT_AUTH           = 9,
    MAC_INVLD_ELEMENT           = 13,
    MAC_MIC_FAIL                = 14,
    MAC_IEEE_802_1X_AUTH_FAIL   = 23,
    MAC_UNSPEC_QOS_REASON       = 32,
    MAC_QAP_INSUFF_BANDWIDTH    = 33,
    MAC_POOR_CHANNEL            = 34,
    MAC_STA_TX_AFTER_TXOP       = 35,
    MAC_QSTA_LEAVING_NETWORK    = 36,
    MAC_QSTA_INVALID_MECHANISM  = 37,
    MAC_QSTA_SETUP_NOT_DONE     = 38,
    MAC_QSTA_TIMEOUT            = 39,
    MAC_QSTA_CIPHER_NOT_SUPP    = 45,

#ifdef _PRE_WLAN_FEATURE_MESH
    MAC_WPA_KICK_MESH_USER  = 46, /* 用于指明用户是由wpa发起来删除 */
    MAC_LWIP_KICK_MESH_USER = 47,
#endif
} mac_reason_code_enum;
typedef hi_u8 mac_reason_code_enum_uint16;

/* Capability Information field bit assignments  */
typedef enum {
    MAC_CAP_ESS             = 0x01,   /* ESS capability               */
    MAC_CAP_IBSS            = 0x02,   /* IBSS mode                    */
    MAC_CAP_POLLABLE        = 0x04,   /* CF Pollable                  */
    MAC_CAP_POLL_REQ        = 0x08,   /* Request to be polled         */
    MAC_CAP_PRIVACY         = 0x10,   /* WEP encryption supported     */
    MAC_CAP_SHORT_PREAMBLE  = 0x20,   /* Short Preamble is supported  */
    MAC_CAP_SHORT_SLOT      = 0x400,  /* Short Slot is supported      */
    MAC_CAP_PBCC            = 0x40,   /* PBCC                         */
    MAC_CAP_CHANNEL_AGILITY = 0x80,   /* Channel Agility              */
    MAC_CAP_SPECTRUM_MGMT   = 0x100,  /* Spectrum Management          */
    MAC_CAP_DSSS_OFDM       = 0x2000  /* DSSS-OFDM                    */
} mac_capability_enum;
typedef hi_u16 mac_capability_enum_uint16;

/* Status Codes for Authentication and Association Frames */
typedef enum {
    MAC_SUCCESSFUL_STATUSCODE       = 0,
    MAC_UNSPEC_FAIL                 = 1,
    MAC_UNSUP_CAP                   = 10,
    MAC_REASOC_NO_ASOC              = 11,
    MAC_FAIL_OTHER                  = 12,
    MAC_UNSUPT_ALG                  = 13,
    MAC_AUTH_SEQ_FAIL               = 14,
    MAC_CHLNG_FAIL                  = 15,
    MAC_AUTH_TIMEOUT                = 16,
    MAC_AP_FULL                     = 17,
    MAC_UNSUP_RATE                  = 18,
    MAC_SHORT_PREAMBLE_UNSUP        = 19,
    MAC_PBCC_UNSUP                  = 20,
    MAC_CHANNEL_AGIL_UNSUP          = 21,
    MAC_MISMATCH_SPEC_MGMT          = 22,
    MAC_MISMATCH_POW_CAP            = 23,
    MAC_MISMATCH_SUPP_CHNL          = 24,
    MAC_SHORT_SLOT_UNSUP            = 25,
    MAC_OFDM_DSSS_UNSUP             = 26,
    MAC_MISMATCH_HTCAP              = 27,
    MAC_MISMATCH_PCO                = 29,
    MAC_REJECT_TEMP                 = 30,
    MAC_MFP_VIOLATION               = 31,
    MAC_UNSPEC_QOS_FAIL             = 32,
    MAC_QAP_INSUFF_BANDWIDTH_FAIL   = 33,
    MAC_POOR_CHANNEL_FAIL           = 34,
    MAC_REMOTE_STA_NOT_QOS          = 35,
    MAC_REQ_DECLINED                = 37,
    MAC_INVALID_REQ_PARAMS          = 38,
    MAC_RETRY_NEW_TSPEC             = 39,
    MAC_INVALID_INFO_ELMNT          = 40,
    MAC_INVALID_GRP_CIPHER          = 41,
    MAC_INVALID_PW_CIPHER           = 42,
    MAC_INVALID_AKMP_CIPHER         = 43,
    MAC_UNSUP_RSN_INFO_VER          = 44,
    MAC_INVALID_RSN_INFO_CAP        = 45,
    MAC_CIPHER_REJ                  = 46,
    MAC_RETRY_TS_LATER              = 47,
    MAC_DLS_NOT_SUPP                = 48,
    MAC_DST_STA_NOT_IN_QBSS         = 49,
    MAC_DST_STA_NOT_QSTA            = 50,
    MAC_LARGE_LISTEN_INT            = 51,
    MAC_MISMATCH_VHTCAP             = 104,
} mac_status_code_enum;
typedef hi_u16 mac_status_code_enum_uint16;

/* BA会话管理确认策略 */
typedef enum {
    MAC_BA_POLICY_DELAYED = 0,
    MAC_BA_POLICY_IMMEDIATE,

    MAC_BA_POLICY_BUTT
} mac_ba_policy_enum;
typedef hi_u8 mac_ba_policy_enum_uint8;

/* 发起DELBA帧的端点的枚举 */
typedef enum {
    MAC_RECIPIENT_DELBA = 0, /* 数据的接收端 */
    MAC_ORIGINATOR_DELBA,    /* 数据的发起端 */

    MAC_BUTT_DELBA
} dmac_delba_initiator_enum;
typedef hi_u8 mac_delba_initiator_enum_uint8;

/* ****************************************************************************
  信息元素(Infomation Element)的Element ID
  协议521页，Table 8-54—Element IDs
**************************************************************************** */
typedef enum {
    MAC_EID_SSID                   = 0,
    MAC_EID_RATES                  = 1,
    MAC_EID_FHPARMS                = 2,
    MAC_EID_DSPARMS                = 3,
    MAC_EID_CFPARMS                = 4,
    MAC_EID_TIM                    = 5,
    MAC_EID_IBSSPARMS              = 6,
    MAC_EID_COUNTRY                = 7,
    MAC_EID_REQINFO                = 10,
    MAC_EID_QBSS_LOAD              = 11,
    MAC_EID_TCLAS                  = 14,
    MAC_EID_CHALLENGE              = 16,
    /* 17-31 reserved */
    MAC_EID_PWRCNSTR               = 32,
    MAC_EID_PWRCAP                 = 33,
    MAC_EID_TPCREQ                 = 34,
    MAC_EID_TPCREP                 = 35,
    MAC_EID_SUPPCHAN               = 36,
    MAC_EID_CHANSWITCHANN          = 37,   /* Channel Switch Announcement IE */
    MAC_EID_MEASREQ                = 38,
    MAC_EID_MEASREP                = 39,
    MAC_EID_QUIET                  = 40,
    MAC_EID_IBSSDFS                = 41,
    MAC_EID_ERP                    = 42,
    MAC_EID_TCLAS_PROCESS          = 44,
    MAC_EID_HT_CAP                 = 45,
    MAC_EID_QOS_CAP                = 46,
    MAC_EID_RESERVED_47            = 47,
    MAC_EID_RSN                    = 48,
    MAC_EID_RESERVED_49            = 49,
    MAC_EID_XRATES                 = 50,
    MAC_EID_AP_CHAN_REPORT         = 51,
    MAC_EID_NEIGHBOR_REPORT        = 52,
    MAC_EID_MOBILITY_DOMAIN        = 54,
    MAC_EID_FT                     = 55,
    MAC_EID_TIMEOUT_INTERVAL       = 56,
    MAC_EID_EXTCHANSWITCHANN       = 60,   /* Extended Channel Switch Announcement IE */
    MAC_EID_HT_OPERATION           = 61,
    MAC_EID_SEC_CH_OFFSET          = 62,   /* Secondary Channel Offset IE */
    MAC_EID_WAPI                   = 68,   /* IE for WAPI */
    MAC_EID_TIME_ADVERTISEMENT     = 69,
    MAC_EID_RRM                    = 70,   /* Radio resource measurement */
    MAC_EID_2040_COEXT             = 72,   /* 20/40 BSS Coexistence IE */
    MAC_EID_2040_INTOLCHREPORT     = 73,   /* 20/40 BSS Intolerant Channel Report IE */
    MAC_EID_OBSS_SCAN              = 74,   /* Overlapping BSS Scan Parameters IE */
    MAC_EID_MMIE                   = 76,   /* 802.11w Management MIC IE */
    MAC_EID_FMS_DESCRIPTOR         = 86,   /* 802.11v FMS descriptor IE */
    MAC_EID_FMS_REQUEST            = 87,   /* 802.11v FMS request IE */
    MAC_EID_FMS_RESPONSE           = 88,   /* 802.11v FMS response IE */
    MAC_EID_BSSMAX_IDLE_PERIOD     = 90,   /* BSS MAX IDLE PERIOD */
    MAC_EID_TFS_REQUEST            = 91,
    MAC_EID_TFS_RESPONSE           = 92,
    MAC_EID_TIM_BCAST_REQUEST      = 94,
    MAC_EID_TIM_BCAST_RESPONSE     = 95,
    MAC_EID_INTERWORKING           = 107,
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    MAC_EID_MESH_CONF = 113,
    MAC_EID_MESHID = 114,
    MAC_EID_BEACON_TIMING = 120,
#endif
    MAC_EID_EXT_CAPS               = 127,  /* Extended Capabilities IE */
    MAC_EID_VHT_TYPE               = 129,
    MAC_EID_11NTXBF                = 130,   /* 802.11n txbf */
    MAC_EID_RESERVED_133           = 133,
    MAC_EID_TPC                    = 150,
    MAC_EID_CCKM                   = 156,
    MAC_EID_VHT_CAP                = 191,
    MAC_EID_VHT_OPERN              = 192,  /* VHT Operation IE */
    MAC_EID_WIDE_BW_CH_SWITCH      = 194,  /* Wide Bandwidth Channel Switch IE */
    MAC_EID_OPMODE_NOTIFY          = 199,  /* Operating Mode Notification IE */
    MAC_EID_VENDOR                 = 221,  /* vendor private */
    MAC_EID_WMM                    = 221,
    MAC_EID_WPA                    = 221,
    MAC_EID_WPS                    = 221,
    MAC_EID_P2P                    = 221,
}mac_eid_enum;
typedef hi_u8 mac_eid_enum_uint8;

typedef enum {
    MAC_P2P_ATTRIBUTE_CAP          = 2,
    MAC_P2P_ATTRIBUTE_GROUP_OI     = 4,
    MAC_P2P_ATTRIBUTE_CFG_TIMEOUT  = 5,
    MAC_P2P_ATTRIBUTE_LISTEN_CHAN  = 6,
}mac_p2p_attribute_enum;
typedef hi_u8 mac_p2p_attribute_enum_uint8;

typedef enum {
    MAC_SMPS_STATIC_MODE     = 0,   /*   静态SMPS   */
    MAC_SMPS_DYNAMIC_MODE    = 1,   /*   动态SMPS   */
    MAC_SMPS_MIMO_MODE       = 3,   /* disable SMPS */

    MAC_SMPS_MODE_BUTT
} mac_mimo_power_save_enum;
typedef hi_u8 mac_mimo_power_save_mode_enum_uint8;

typedef enum {
    MAC_SCN = 0,    /* 不存在次信道 */
    MAC_SCA = 1,    /* 次信道在主信道之上(Secondary Channel Above) */
    MAC_SCB = 3,    /* 次信道在主信道之下(Secondary Channel Below) */
    MAC_BW_5M = 4,  /* 自定义，工作带宽窄带5M */
    MAC_BW_10M = 5, /* 自定义，工作带宽窄带10M */
    MAC_SEC_CH_BUTT,
} mac_sec_ch_off_enum;
typedef hi_u8 mac_sec_ch_off_enum_uint8;

/* P2P相关 */
typedef enum {
    P2P_STATUS             =  0,
    P2P_MINOR_REASON_CODE  =  1,
    P2P_CAPABILITY         =  2,
    P2P_DEVICE_ID          =  3,
    GROUP_OWNER_INTENT     =  4,
    CONFIG_TIMEOUT         =  5,
    LISTEN_CHANNEL         =  6,
    P2P_GROUP_BSSID        =  7,
    EXTENDED_LISTEN_TIMING =  8,
    INTENDED_P2P_IF_ADDR   =  9,
    P2P_MANAGEABILITY      =  10,
    P2P_CHANNEL_LIST       =  11,
    NOTICE_OF_ABSENCE      =  12,
    P2P_DEVICE_INFO        =  13,
    P2P_GROUP_INFO         =  14,
    P2P_GROUP_ID           =  15,
    P2P_INTERFACE          =  16,
    P2P_OPERATING_CHANNEL  =  17,
    INVITATION_FLAGS       =  18
} attribute_id_t;

typedef enum {
    P2P_PUB_ACT_OUI_OFF1         = 2,
    P2P_PUB_ACT_OUI_OFF2         = 3,
    P2P_PUB_ACT_OUI_OFF3         = 4,
    P2P_PUB_ACT_OUI_TYPE_OFF     = 5,
    P2P_PUB_ACT_OUI_SUBTYPE_OFF  = 6,
    P2P_PUB_ACT_DIALOG_TOKEN_OFF = 7,
    P2P_PUB_ACT_TAG_PARAM_OFF    = 8
} p2p_pub_act_frm_off;

typedef enum {
    P2P_GEN_ACT_OUI_OFF1         = 1,
    P2P_GEN_ACT_OUI_OFF2         = 2,
    P2P_GEN_ACT_OUI_OFF3         = 3,
    P2P_GEN_ACT_OUI_TYPE_OFF     = 4,
    P2P_GEN_ACT_OUI_SUBTYPE_OFF  = 5,
    P2P_GEN_ACT_DIALOG_TOKEN_OFF = 6,
    P2P_GEN_ACT_TAG_PARAM_OFF    = 7
} p2p_gen_act_frm_off;

typedef enum {
    P2P_NOA           = 0,
    P2P_PRESENCE_REQ  = 1,
    P2P_PRESENCE_RESP = 2,
    GO_DISC_REQ       = 3
} p2p_gen_action_frm_type;

typedef enum {
    P2P_STAT_SUCCESS           = 0,
    P2P_STAT_INFO_UNAVAIL      = 1,
    P2P_STAT_INCOMP_PARAM      = 2,
    P2P_STAT_LMT_REACHED       = 3,
    P2P_STAT_INVAL_PARAM       = 4,
    P2P_STAT_UNABLE_ACCO_REQ   = 5,
    P2P_STAT_PREV_PROT_ERROR   = 6,
    P2P_STAT_NO_COMMON_CHAN    = 7,
    P2P_STAT_UNKNW_P2P_GRP     = 8,
    P2P_STAT_GO_INTENT_15      = 9,
    P2P_STAT_INCOMP_PROV_ERROR = 10,
    P2P_STAT_USER_REJECTED     = 11
} p2p_status_code_t;

/* Mesh相关 */
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
typedef enum {
    MAC_MESH_AUTH_PROTO_NULL = 0,
    MAC_MESH_AUTH_PROTO_SAE = 1,
    MAC_MESH_AUTH_PROTO_8021X = 2,
    MAC_MESH_AUTH_PROTO_VENDOR_SPEC = 255,
} mac_mesh_auth_proto_enum;
typedef hi_u8 mac_mesh_auth_proto_enum_uint8;
#endif

#define MAC_WLAN_OUI_WFA                    0x506f9a
#define MAC_WLAN_OUI_TYPE_WFA_P2P               9
#define MAC_WLAN_OUI_MICROSOFT              0x0050f2
#ifdef _PRE_WLAN_FEATURE_MESH
#define MAC_WLAN_OUI_HW 0x00E0FC
#endif
#define MAC_WLAN_OUI_TYPE_MICROSOFT_WPA         1
#define MAC_WLAN_OUI_TYPE_MICROSOFT_WMM         2
#define MAC_WLAN_OUI_TYPE_MICROSOFT_WPS         4

/* eapol key 结构宏定义 */
#define WPA_REPLAY_COUNTER_LEN      8
#define WPA_NONCE_LEN               32
#define WPA_KEY_RSC_LEN             8
#define IEEE802_1X_TYPE_EAPOL_KEY   3
#define WPA_KEY_INFO_KEY_TYPE       bit(3) /* 1 = Pairwise, 0 = Group key */

#define MAC_VHT_CHANGE (BIT1)
#define MAC_HT_CHANGE  (BIT2)
#define MAC_BW_CHANGE  (BIT3)
#define MAC_NO_CHANGE   0

/* ****************************************************************************
  STRUCT定义
**************************************************************************** */
#pragma pack(1)
/* 此文件中结构体与协议一致，要求1字节对齐，统一加__OAL_DECLARE_PACKED */
struct mac_ether_header {
    hi_u8    auc_ether_dhost[ETHER_ADDR_LEN];
    hi_u8    auc_ether_shost[ETHER_ADDR_LEN];
    hi_u16   us_ether_type;
} __OAL_DECLARE_PACKED;
typedef struct mac_ether_header mac_ether_header_stru;

typedef struct mac_llc_snap {
    hi_u8   llc_dsap;
    hi_u8   llc_ssap;
    hi_u8   control;
    hi_u8   auc_org_code[ORG_CODE_LEN];
    hi_u16  us_ether_type;
} mac_llc_snap_stru;

/* eapol帧头 */
typedef struct mac_eapol_header {
    hi_u8       version;
    hi_u8       type;
    hi_u16      us_length;
} mac_eapol_header_stru;

/* IEEE 802.11, 8.5.2 EAPOL-Key frames */
/* EAPOL KEY 结构定义 */
struct mac_eapol_key {
    hi_u8 type;
    /* Note: key_info, key_length, and key_data_length are unaligned */
    hi_u8 auc_key_info[2];          /* big endian:占2 byte */
    hi_u8 auc_key_length[2];        /* big endian:占2 byte */
    hi_u8 auc_replay_counter[WPA_REPLAY_COUNTER_LEN];
    hi_u8 auc_key_nonce[WPA_NONCE_LEN];
    hi_u8 auc_key_iv[16];           /* key_iv占16 byte */
    hi_u8 auc_key_rsc[WPA_KEY_RSC_LEN];
    hi_u8 auc_key_id[8];            /* Reserved in IEEE 802.11i/RSN:占8 byte */
    hi_u8 auc_key_mic[16];          /* mic占16 byte */
    hi_u8 auc_key_data_length[2];   /* big endian:占2 byte */
    /* followed by key_data_length bytes of key_data */
} __OAL_DECLARE_PACKED;
typedef struct mac_eapol_key mac_eapol_key_stru;

/*
 * Structure of the IP frame
 */
typedef struct mac_ip_header {
    hi_u8    version_ihl;
    hi_u8    tos;
    hi_u16   us_tot_len;
    hi_u16   us_id;
    hi_u16   us_frag_off;
    hi_u8    ttl;
    hi_u8    protocol;
    hi_u16   us_check;
    hi_u32   saddr;
    hi_u32   daddr;
    /* The options start here. */
} mac_ip_header_stru;

/*
 * Header in on cable format
 */
typedef struct mac_igmp_header {
    hi_u8  type;
    hi_u8  code;        /* For newer IGMP */
    hi_u16 us_csum;
    hi_u32 group;
} mac_igmp_header_stru;

/*  Group record format
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Record Type  |  Aux Data Len |     Number of Sources (N)     |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                       Multicast Address                       |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                       Source Address [1]                      |
      +-                                                             -+
      |                       Source Address [2]                      |
      +-                                                             -+
      .                               .                               .
      .                               .                               .
      .                               .                               .
      +-                                                             -+
      |                       Source Address [N]                      |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      .                                                               .
      .                         Auxiliary Data                        .
      .                                                               .
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct mac_igmp_v3_grec {
    hi_u8     grec_type;
    hi_u8     grec_auxwords;
    hi_u16    us_grec_nsrcs;
    hi_u32    grec_mca;
} mac_igmp_v3_grec_stru;

/* IGMPv3 report format
       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Type = 0x22  |    Reserved   |           Checksum            |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |           Reserved            |  Number of Group Records (M)  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      .                                                               .
      .                        Group Record [1]                       .
      .                                                               .
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      .                                                               .
      .                        Group Record [2]                       .
      .                                                               .
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                               .                               |
      .                               .                               .
      |                               .                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      .                                                               .
      .                        Group Record [M]                       .
      .                                                               .
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct mac_igmp_v3_report {
    hi_u8     type;
    hi_u8     resv1;
    hi_u16    us_csum;
    hi_u16    us_resv2;
    hi_u16    us_ngrec;
} mac_igmp_v3_report_stru;

typedef struct mac_tcp_header {
    hi_u16  us_sport;
    hi_u16  us_dport;
    hi_u32  seqnum;
    hi_u32  acknum;
    hi_u8   offset;
    hi_u8   flags;
    hi_u16  us_window;
    hi_u16  us_check;
    hi_u16  us_urgent;
} mac_tcp_header_stru;

/* UDP头部结构 */
typedef struct _udp_hdr_stru {
    hi_u16 us_src_port;
    hi_u16 us_des_port;
    hi_u16 us_udp_len;
    hi_u16 us_check_sum;
} udp_hdr_stru;

typedef struct dhcp_message {
    hi_u8 op;                           /* message type */
    hi_u8 hwtype;                       /* hardware address type */
    hi_u8 hwlen;                        /* hardware address length */
    hi_u8 hwopcount;                    /* should be zero in client message */
    hi_u32 xid;                         /* transaction id */
    hi_u16 secs;                        /* elapsed time in sec. from boot */
    hi_u16 flags;
    hi_u32 ciaddr;                      /* (previously allocated) client IP */
    hi_u32 yiaddr;                      /* 'your' client IP address */
    hi_u32 siaddr;                      /* should be zero in client's messages */
    hi_u32 giaddr;                      /* should be zero in client's messages */
    hi_u8 chaddr[DHCP_CHADDR_LEN];      /* client's hardware address */
    hi_u8 servername[SERVERNAME_LEN];   /* server host name */
    hi_u8 bootfile[BOOTFILE_LEN];       /* boot file name */
    hi_u32 cookie;
    hi_u8 options[DHCP_OPTION_LEN];     /* message options, cookie */
} dhcp_message_stru;

/* frame control字段结构体 */
struct mac_header_frame_control {
    hi_u16  protocol_version    : 2,        /* 协议版本 */
                type                : 2,        /* 帧类型 */
                sub_type            : 4,        /* 子类型 */
                to_ds               : 1,        /* 发送DS */
                from_ds             : 1,        /* 来自DS */
                more_frag           : 1,        /* 分段标识 */
                retry               : 1,        /* 重传帧 */
                power_mgmt          : 1,        /* 节能管理 */
                more_data           : 1,        /* 更多数据标识 */
                protected_frame     : 1,        /* 加密标识 */
                order               : 1;        /* 次序位 */
} __OAL_DECLARE_PACKED;
typedef struct mac_header_frame_control mac_header_frame_control_stru;

/* 基础802.11帧结构 */
typedef struct mac_ieee80211_frame {
    mac_header_frame_control_stru   frame_control;
    hi_u16
                                    duration_value      : 15,
                                    duration_flag       : 1;
    hi_u8                       auc_address1[WLAN_MAC_ADDR_LEN];
    hi_u8                       auc_address2[WLAN_MAC_ADDR_LEN];
    hi_u8                       auc_address3[WLAN_MAC_ADDR_LEN];
    hi_u16                      frag_num    : 4,
                                    seq_num     : 12;
} mac_ieee80211_frame_stru;

/* ps poll帧结构 */
typedef struct mac_ieee80211_pspoll_frame {
    mac_header_frame_control_stru   frame_control;
    hi_u16                      aid_value   : 14,                   /* ps poll 下的AID字段 */
                                aid_flag1   : 1,
                                aid_flag2   : 1;
    hi_u8                       auc_bssid[WLAN_MAC_ADDR_LEN];
    hi_u8                       auc_trans_addr[WLAN_MAC_ADDR_LEN];
} mac_ieee80211_pspoll_frame_stru;

union mac_ieee80211_qos_control {
    hi_u8                       qc_txop_limit;
    hi_u8                       qc_ps_buf_state_resv        : 1,
                                qc_ps_buf_state_inducated   : 1,
                                bit_qc_hi_priority_buf_ac       : 2,
                                qc_qosap_buf_load           : 4;
} __OAL_DECLARE_PACKED;
typedef union mac_ieee80211_qos_control mac_ieee80211_qos_control;

/* qos帧结构 */
struct mac_ieee80211_qos_frame {
    mac_ieee80211_frame_stru frame;

    /* qos info and control */
    hi_u8                           qc_tid          : 4,
                                    qc_eosp         : 1,
                                    qc_ack_polocy   : 2,
                                    qc_amsdu        : 1;
    mac_ieee80211_qos_control       qos_control;
    hi_u8                           auc_htc[0];
}__OAL_DECLARE_PACKED;
typedef struct mac_ieee80211_qos_frame mac_ieee80211_qos_frame_stru;

/* 四地址帧结构体 */
struct mac_ieee80211_frame_addr4 {
    mac_header_frame_control_stru   frame_control;
    hi_u16
                                    duration_value      : 15,
                                    duration_flag       : 1;
    hi_u8                       auc_address1[WLAN_MAC_ADDR_LEN];
    hi_u8                       auc_address2[WLAN_MAC_ADDR_LEN];
    hi_u8                       auc_address3[WLAN_MAC_ADDR_LEN];
    hi_u16                      frag_num            : 4,
                                    seq_num             : 12;
    hi_u8                       auc_address4[WLAN_MAC_ADDR_LEN];
} __OAL_DECLARE_PACKED;
typedef struct mac_ieee80211_frame_addr4 mac_ieee80211_frame_addr4_stru;

/* qos四地址帧结构 */
struct mac_ieee80211_qos_frame_addr4 {
    mac_ieee80211_frame_addr4_stru frame_addr4;
    /* qos info and control */
    hi_u8                           qc_tid          : 4,
                                    qc_eosp         : 1,
                                    qc_ack_polocy   : 2,
                                    qc_amsdu        : 1;
    mac_ieee80211_qos_control qos_control;
} __OAL_DECLARE_PACKED;
typedef struct mac_ieee80211_qos_frame_addr4 mac_ieee80211_qos_frame_addr4_stru;

/* qos htc 四地址帧结构 */
struct mac_ieee80211_qos_htc_frame_addr4 {
    mac_ieee80211_qos_frame_addr4_stru qos_frame_addr4;
    hi_u32 htc;
} __OAL_DECLARE_PACKED;
typedef struct mac_ieee80211_qos_htc_frame_addr4 mac_ieee80211_qos_htc_frame_addr4_stru;

/* Ref. 802.11-2012.pdf, 8.4.1.4 Capability information field, 中文注释参考白皮书 */
struct mac_cap_info {
    hi_u16  ess                     : 1,        /* 由BSS中的AP设置为1 */
                ibss                : 1,        /* 由一个IBSS中的站点设置为1，ap总是设置其为0 */
                cf_pollable         : 1,        /* 标识CF-POLL能力 */
                cf_poll_request     : 1,        /* 标识CF-POLL能力  */
                privacy             : 1,        /* 1=需要加密, 0=不需要加密 */
                short_preamble      : 1,        /* 802.11b短前导码 */
                pbcc                : 1,        /* 802.11g */
                channel_agility     : 1,        /* 802.11b */
                spectrum_mgmt       : 1,        /* 频谱管理: 0=不支持, 1=支持 */
                qos                 : 1,        /* QOS: 0=非QOS站点, 1=QOS站点 */
                short_slot_time     : 1,        /* 短时隙: 0=不支持, 1=支持 */
                apsd                : 1,        /* 自动节能: 0=不支持, 1=支持 */
                radio_measurement   : 1,        /* Radio检测: 0=不支持, 1=支持 */
                dsss_ofdm           : 1,        /* 802.11g */
                delayed_block_ack   : 1,        /* 延迟块确认: 0=不支持, 1=支持 */
                immediate_block_ack : 1;        /* 立即块确认: 0=不支持, 1=支持 */
} __OAL_DECLARE_PACKED;
typedef struct mac_cap_info mac_cap_info_stru;

/* Ref. 802.11-2012.pdf, 8.4.2.58.2 HT Capabilities Info field */
struct mac_frame_ht_cap {
    hi_u16          ldpc_coding_cap             : 1,        /* LDPC 编码 capability    */
                        supported_channel_width : 1,    /* STA 支持的带宽          */
                        sm_power_save           : 2,    /* SM 省电模式             */
                        ht_green_field          : 1,    /* 绿野模式                */
                        short_gi_20mhz          : 1,    /* 20M下短保护间隔         */
                        short_gi_40mhz          : 1,    /* 40M下短保护间隔         */
                        tx_stbc                 : 1,    /* Indicates support for the transmission of PPDUs using STBC */
                        rx_stbc                 : 2,    /* 支持 Rx STBC            */
                        ht_delayed_block_ack    : 1,    /* Indicates support for HT-delayed Block Ack opera-tion. */
                        max_amsdu_length        : 1,    /* Indicates maximum A-MSDU length. */
                        dsss_cck_mode_40mhz     : 1,    /* 40M下 DSSS/CCK 模式     */
                        bit_resv                : 1,
                        /* Indicates whether APs receiving this information or reports of this informa-tion are
                            required to prohibit 40 MHz transmissions */
                        forty_mhz_intolerant    : 1,
                        lsig_txop_protection    : 1;    /* 支持 L-SIG TXOP 保护    */
} __OAL_DECLARE_PACKED;
typedef struct mac_frame_ht_cap mac_frame_ht_cap_stru;

typedef struct mac_vht_cap_info {
    hi_u32  max_mpdu_length             : 2,
                supported_channel_width : 2,
                rx_ldpc                 : 1,
                short_gi_80mhz          : 1,
                short_gi_160mhz         : 1,
                tx_stbc                 : 1,
                rx_stbc                 : 3,
                su_beamformer_cap       : 1,
                su_beamformee_cap       : 1,
                num_bf_ant_supported    : 3,
                num_sounding_dim        : 3,
                mu_beamformer_cap       : 1,
                mu_beamformee_cap       : 1,
                vht_txop_ps             : 1,
                htc_vht_capable         : 1,
                max_ampdu_len_exp       : 3,
                vht_link_adaptation     : 2,
                rx_ant_pattern          : 1,
                tx_ant_pattern          : 1,
                bit_resv                    : 2;
} mac_vht_cap_info_stru;

typedef struct mac_11ntxbf_info {
    hi_u8           ntxbf                   :1,          /* 11n txbf  能力 */
                    reserve                 :7;
    hi_u8           auc_reserve[3]; /* 3 byte保留字段 */
} mac_11ntxbf_info_stru;

struct mac_11ntxbf_vendor_ie {
    hi_u8                        id;          /* element ID */
    hi_u8                        len;         /* length in bytes */
    hi_u8                        auc_oui[MAC_OUI_LEN];
    hi_u8                        ouitype;
    mac_11ntxbf_info_stru        ntxbf;
} __OAL_DECLARE_PACKED;
typedef struct mac_11ntxbf_vendor_ie mac_11ntxbf_vendor_ie_stru;

/* 厂家自定义IE 数据结构，摘自linux 内核 */
struct mac_ieee80211_vendor_ie {
    hi_u8 element_id;
    hi_u8 len;
    hi_u8 auc_oui[MAC_OUI_LEN];
    hi_u8 oui_type;
} __OAL_DECLARE_PACKED;
typedef struct mac_ieee80211_vendor_ie mac_ieee80211_vendor_ie_stru;

/* 建立BA会话时，BA参数域结构定义 */
struct mac_ba_parameterset {
#if (_PRE_BIG_CPU_ENDIAN == _PRE_CPU_ENDIAN)            /* BIG_ENDIAN */
    hi_u16  buffersize          : 10,               /* B6-15  buffer size */
                tid             : 4,                /* B2-5   TID */
                bapolicy        : 1,                /* B1   block ack policy */
                amsdusupported  : 1;                /* B0   amsdu supported */
#else
    hi_u16  amsdusupported      : 1,                /* B0   amsdu supported */
                bapolicy        : 1,                /* B1   block ack policy */
                tid             : 4,                /* B2-5   TID */
                buffersize      : 10;               /* B6-15  buffer size */
#endif
} __OAL_DECLARE_PACKED;
typedef struct mac_ba_parameterset mac_ba_parameterset_stru;

/* BA会话过程中的序列号参数域定义 */
struct mac_ba_seqctrl {
#if (_PRE_BIG_CPU_ENDIAN == _PRE_CPU_ENDIAN) /* BIG_ENDIAN */
    hi_u16 startseqnum : 12, /* B4-15  starting sequence number */
    fragnum            : 4;  /* B0-3  fragment number */
#else
    hi_u16 fragnum     : 4,  /* B0-3  fragment number */
    startseqnum        : 12; /* B4-15  starting sequence number */
#endif
} __OAL_DECLARE_PACKED;
typedef struct mac_ba_seqctrl mac_ba_seqctrl_stru;

/* Quiet信息元素结构体 */
struct mac_quiet_ie {
    hi_u8     quiet_count;
    hi_u8     quiet_period;
    hi_u16    quiet_duration;
    hi_u16    quiet_offset;
} __OAL_DECLARE_PACKED;
typedef struct mac_quiet_ie mac_quiet_ie_stru;

/* erp 信息元素结构体 */
struct mac_erp_params {
    hi_u8       non_erp       : 1,
                use_protection: 1,
                preamble_mode : 1,
                bit_resv      : 5;
} __OAL_DECLARE_PACKED;
typedef struct mac_erp_params mac_erp_params_stru;

/* rsn信息元素 rsn能力字段结构体 */
struct mac_rsn_cap {
    hi_u16  pre_auth                : 1,
                no_pairwise         : 1,
                ptska_relay_counter : 2,
                gtska_relay_counter : 2,
                mfpr                : 1,
                mfpc                : 1,
                rsv0                : 1,
                peer_key            : 1,
                spp_amsdu_capable   : 1,
                spp_amsdu_required  : 1,
                pbac                : 1,
                ext_key_id          : 1,
                rsv1                : 2;
} __OAL_DECLARE_PACKED;
typedef struct mac_rsn_cap mac_rsn_cap_stru;

/* obss扫描ie obss扫描参数结构体 */
struct mac_obss_scan_params {
    hi_u16 us_passive_dwell;
    hi_u16 us_active_dwell;
    hi_u16 us_scan_interval;
    hi_u16 us_passive_total_per_chan;
    hi_u16 us_active_total_per_chan;
    hi_u16 us_transition_delay_factor;
    hi_u16 us_scan_activity_thresh;
} __OAL_DECLARE_PACKED;
typedef struct mac_obss_scan_params mac_obss_scan_params_stru;

/* 扩展能力信息元素结构体定义 */
typedef struct mac_ext_cap_ie {
    hi_u8   coexistence_mgmt: 1,
                resv1                : 1,
                ext_chan_switch      : 1,
                resv2                : 1,
                psmp                 : 1,
                resv3                : 1,
                s_psmp               : 1,
                event                : 1;
    hi_u8   resv4                : 4,
                proxyarp             : 1,
                resv13               : 3;
    hi_u8   resv5                : 8;
    hi_u8   resv6                : 8;
    hi_u8   resv7                         : 5,
                tdls_prhibited                : 1,
                tdls_channel_switch_prhibited : 1,
                resv8                         : 1;

    hi_u8   resv9                : 8;
    hi_u8   resv10               : 8;

    hi_u8   resv11                        : 6,
                operating_mode_notification   : 1, /* 11ac Operating Mode Notification特性标志 */
                resv12                        : 1;
} mac_ext_cap_ie_stru;
/* qos info字段结构体定义 */
struct mac_qos_info {
    hi_u8   params_count: 4,
                bit_resv        : 3,
                uapsd       : 1;
}__OAL_DECLARE_PACKED;
typedef struct mac_qos_info mac_qos_info_stru;

/* wmm信息元素 ac参数结构体 */
typedef struct mac_wmm_ac_params {
    hi_u8   aifsn : 4,
                acm   : 1,
                aci   : 2,
                bit_resv  : 1;
    hi_u8   ecwmin: 4,
                ecwmax: 4;
    hi_u16  us_txop;
} mac_wmm_ac_params_stru;
/* BSS load信息元素结构体 */
struct mac_bss_load {
    hi_u16 us_sta_count;            /* 关联的sta个数 */
    hi_u8  chan_utilization;        /* 信道利用率 */
    hi_u8  resv;
    hi_u16 us_aac;
} __OAL_DECLARE_PACKED;
typedef struct mac_bss_load mac_bss_load_stru;

/* country信息元素 管制域字段 */
struct mac_country_reg_field {
    hi_u8 first_channel;         /* 第一个信道号 */
    hi_u8 channel_num;           /* 信道个数 */
    hi_u8 max_tx_pwr;            /* 最大传输功率，dBm */
} __OAL_DECLARE_PACKED;
typedef struct mac_country_reg_field mac_country_reg_field_stru;

/* ht capabilities信息元素支持的ampdu parameters字段结构体定义 */
struct mac_ampdu_params {
    hi_u8  max_ampdu_len_exponent  : 2,
               min_mpdu_start_spacing  : 3,
               bit_resv                    : 3;
}__OAL_DECLARE_PACKED;
typedef struct mac_ampdu_params mac_ampdu_params_stru;

/* ht cap信息元素 支持的mcs集字段 结构体定义 */
typedef struct mac_sup_mcs_set {
    hi_u8   auc_rx_mcs[WLAN_HT_MCS_BITMASK_LEN];
    hi_u16  rx_highest_rate: 10,
                resv1          : 6;
    hi_u32  tx_mcs_set_def : 1,
                tx_rx_not_equal: 1,
                tx_max_stream  : 2,
                tx_unequal_modu: 1,
                resv2          : 27;
}mac_sup_mcs_set_stru;

/* vht信息元素，支持的mcs集字段 */
typedef struct mac_vht_sup_mcs_set {
    hi_u32  rx_mcs_map      : 16,
                rx_highest_rate : 13,
                bit_resv            : 3;
    hi_u32  tx_mcs_map      : 16,
                tx_highest_rate : 13,
                resv2           : 3;
}mac_vht_sup_mcs_set_stru;
/* ht capabilities信息元素支持的extended cap.字段结构体定义 */
struct mac_ext_cap {
    hi_u16  pco           : 1,                   /* */
                pco_trans_time: 2,
                resv1         : 5,
                mcs_fdbk      : 2,
                htc_sup       : 1,
                rd_resp       : 1,
                resv2         : 4;
}__OAL_DECLARE_PACKED;
typedef struct mac_ext_cap mac_ext_cap_stru;

/* ht cap信息元素的Transmit Beamforming Capabilities字段结构体定义 */
typedef struct mac_txbf_cap {
    hi_u32  implicit_txbf_rx                : 1,
                rx_stagg_sounding               : 1,
                tx_stagg_sounding               : 1,
                rx_ndp                          : 1,
                tx_ndp                          : 1,
                implicit_txbf                   : 1,
                calibration                     : 2,
                explicit_csi_txbf               : 1,
                explicit_noncompr_steering      : 1,
                explicit_compr_steering         : 1,
                explicit_txbf_csi_fdbk          : 2,
                explicit_noncompr_bf_fdbk       : 2,
                explicit_compr_bf_fdbk          : 2,
                minimal_grouping                : 2,
                csi_num_bf_antssup              : 2,
                noncompr_steering_num_bf_antssup: 2,
                compr_steering_num_bf_antssup   : 2,
                csi_maxnum_rows_bf_sup          : 2,
                chan_estimation                 : 2,
                resv2                           : 3;
}mac_txbf_cap_stru;
/* ht cap信息元素的Asel(antenna selection) Capabilities字段结构体定义 */
struct mac_asel_cap {
    hi_u8  asel                         : 1,
               explicit_sci_fdbk_tx_asel    : 1,
               antenna_indices_fdbk_tx_asel : 1,
               explicit_csi_fdbk            : 1,
               antenna_indices_fdbk         : 1,
               rx_asel                      : 1,
               trans_sounding_ppdu          : 1,
               bit_resv                         : 1;
}__OAL_DECLARE_PACKED;
typedef struct mac_asel_cap mac_asel_cap_stru;

/* ht opern元素, ref 802.11-2012 8.4.2.59 */
struct mac_ht_opern {
    hi_u8   primary_channel;

    hi_u8   secondary_chan_offset             : 2,
                sta_chan_width                    : 1,
                rifs_mode                         : 1,
                resv1                             : 4;
    hi_u8   ht_protection                     : 2,
                nongf_sta_present                 : 1,
                resv2                             : 1,
                obss_nonht_sta_present            : 1,
                resv3                             : 3;
    hi_u8   resv4                             : 8;
    hi_u8   resv5                             : 6,
                dual_beacon                       : 1,
                dual_cts_protection               : 1;
    hi_u8   secondary_beacon                  : 1,
                lsig_txop_protection_full_support : 1,
                pco_active                        : 1,
                pco_phase                         : 1,
                resv6                             : 4;

    hi_u8   auc_basic_mcs_set[MAC_HT_BASIC_MCS_SET_LEN];
}__OAL_DECLARE_PACKED;
typedef struct mac_ht_opern mac_ht_opern_stru;

/* vht opern结构体 */
struct mac_opmode_notify {
    hi_u8   channel_width       : 2,     /* 当前最大允许带宽能力 */
                bit_resv        : 2,     /* 保留 */
                rx_nss          : 3,     /* 当前最大允许空间流能力 */
                rx_nss_type     : 1;     /* 是否为TXBF下的rx nss能力，1-是，0不是 */
} __OAL_DECLARE_PACKED;
typedef struct mac_opmode_notify mac_opmode_notify_stru;

/* vht opern结构体 */
struct mac_vht_opern {
    hi_u8   channel_width;
    hi_u8   channel_center_freq_seg0;
    hi_u8   channel_center_freq_seg1;
    hi_u8   resv;
    hi_u16  us_basic_mcs_set;
} __OAL_DECLARE_PACKED;
typedef struct mac_vht_opern mac_vht_opern_stru;

/* 02 dev侧用#pragma pack(1)/#pragma pack()方式达到一字节对齐 */
#pragma pack()

/* ACTION帧的参数格式，注:不同的action帧下对应的参数不同 */
typedef struct {
    hi_u8       category;    /* ACTION的类别 */
    hi_u8       action;      /* 不同ACTION类别下的分类 */
    hi_u8       uc_resv[2];  /* 2 byte保留字段 */
    hi_u32      arg1;
    hi_u32      arg2;
    hi_u32      arg3;
    hi_u32      arg4;
    hi_u8      *puc_arg5;
} mac_action_mgmt_args_stru;

/* 私有管理帧通用的设置参数信息的结构体 */
typedef struct {
    hi_u8       type;
    hi_u8       arg1;        /* 对应的tid序号 */
    hi_u8       arg2;        /* 接收端可接收的最大的mpdu的个数(针对AMPDU_START命令) */
    hi_u8       arg3;        /* 确认策略 */
    hi_u8       user_idx;    /* 对应的用户 */
    hi_u8       auc_resv[3]; /* 3 byte保留字段 */
} mac_priv_req_args_stru;

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
struct mesh_formation_info {
    hi_u8 connected_to_mesh_gate: 1,    /* 是否有指向Mesh Gate的路径(本Mesh方案不用) */
          number_of_peerings: 6,        /* 当前已连接数 */
          connected_to_as: 1;           /* 是否有到AS的连接(IEEE 802.1x 认证) */
} __OAL_DECLARE_PACKED;
typedef struct mesh_formation_info mesh_formation_info_stru;

struct mesh_capability_field {
    hi_u8 accepting_add_mesh_peerings: 1,
          mcca_supported: 1,
          mcca_enabled: 1,
          forwarding: 1,
          mbca_enabled: 1,
          tbtt_adjusting: 1,
          mesh_power_save_level: 1,
          bit_resv: 1;
} __OAL_DECLARE_PACKED;
typedef struct mesh_capability_field mesh_capability_stru;

struct mesh_report_control_field {
    hi_u8 status_number                 : 4,
          beacon_timing_element_number  : 3,
          more_beacon_timing_elements   : 1;
    hi_u8 resv[3];              /* reserve 3byte */
};
typedef struct mesh_report_control_field mesh_report_control_stru;

typedef struct {
    hi_u8 neighbor_sta_id;
    hi_u8 neighbor_tbtt[3];            /* tbtt占用3 byte */
    hi_u8 neighbor_beacon_interval[2]; /* beacon_interval占用2byte */
    hi_u8 auc_rsv[2];                  /* 2 byte保留字段 */
} mesh_beacon_timing_information_stru;

struct mac_mesh_conf_ie {
    hi_u8 ie_id;
    hi_u8 len;
    hi_u8 active_path_sel_proto_id;
    hi_u8 active_path_sel_metric_id;
    hi_u8 congestion_control_mode_id;
    hi_u8 syn_method_id;
    hi_u8 auth_proto_id;
    mesh_formation_info_stru mesh_formation_info;
    mesh_capability_stru mesh_capa;
} __OAL_DECLARE_PACKED;
typedef struct mac_mesh_conf_ie mac_mesh_conf_ie_stru;
#endif

#ifdef _PRE_WLAN_FEATURE_ANY
typedef struct mac_action_header {
    hi_u8 category;
    hi_u8 auc_oui[MAC_OUI_LEN];
    hi_u8 type;
    hi_u8 seq_num;
    hi_u8 sub_type;
    hi_u8 length;
} mac_action_header_stru;
#endif

typedef struct _mac_action_data_stru {
    hi_u32 freq;
    hi_u32 wait;
    hi_u8 dst[WLAN_MAC_ADDR_LEN];
    hi_u8 src[WLAN_MAC_ADDR_LEN];
    hi_u8 bssid[WLAN_MAC_ADDR_LEN];
    hi_u8 resv[2]; /* 2 byte保留字段 */
    hi_u8 *data;
    hi_u32 data_len;
    hi_u32 no_cck;
} mac_action_data_stru;

typedef struct mac_set_quiet_ie_info_stru {
    hi_u8 qcount;
    hi_u8 qperiod;
    hi_u8 resv[2]; /* 2 byte保留字段 */
    hi_u16 us_qduration;
    hi_u16 us_qoffset;
} mac_set_quiet_ie_info_stru;

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_void mac_set_power_cap_ie(hi_u8 *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_supported_channel_ie(hi_u8 *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_wmm_ie_sta(hi_u8 *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_listen_interval_ie(hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_status_code_ie(hi_u8 *puc_buffer, mac_status_code_enum_uint16 status_code);
hi_void mac_set_aid_ie(hi_u8 *puc_buffer, hi_u16 aid);
hi_void mac_set_beacon_interval_field(hi_void *vap, hi_u8 *puc_buffer);
hi_void mac_set_cap_info_ap(hi_void *vap, hi_u8 *puc_cap_info);
hi_void mac_set_cap_info_sta(hi_void *vap, hi_u8 *puc_cap_info);
hi_u16 mac_set_ssid_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len, hi_u16 us_frm_type);
hi_void mac_set_supported_rates_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_dsss_params(hi_void *vap, hi_u16 us_frm_type, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_pwrconstraint_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_erp_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_exsup_rates_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_bssload_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_ht_capabilities_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_ht_opern_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_ext_capabilities_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_tpc_report_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_vht_capabilities_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_vht_opern_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_null_data_encap(hi_u8 *header, hi_u16 us_fc, const hi_u8 *da_mac_addr, const hi_u8 *sa_mac_addr);
hi_u32 mac_prepare_action_frame_head(hi_u8 *puc_header, const hi_u8 *da_mac_addr, const hi_u8 *sa_mac_addr);
hi_u8 mac_prepare_action_frame_body(hi_u8 *puc_body, hi_u8 body_len, hi_u8 category, const hi_u8 *puc_elements,
    hi_u8 element_len);
hi_void mac_set_snap(oal_netbuf_stru *netbuf, hi_u16 us_ether_type, hi_u8 offset);
hi_void mac_add_app_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u16 *pus_ie_len, en_app_ie_type_uint8 type);
hi_void mac_add_wps_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u16 *pus_ie_len, en_app_ie_type_uint8 type);
hi_void mac_check_sta_base_rate(hi_u8 *mac_user, mac_status_code_enum_uint16 *pen_status_code);
hi_void mac_set_wmm_params_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 is_qos, hi_u8 *puc_ie_len);
hi_void mac_set_quiet_ie(hi_void *vap, hi_u8 *puc_buffer, const mac_set_quiet_ie_info_stru *mac_set_quiet_ie_info,
    hi_u8 *puc_ie_len);
hi_void mac_set_security_ie_authenticator(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len, hi_u8 mode);
hi_void mac_set_timeout_interval_ie(hi_u8 *puc_buffer, hi_u8 *puc_ie_len, hi_u32 type, hi_u32 timeout);
#ifdef _PRE_WLAN_FEATURE_11D
hi_void mac_set_country_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
#endif
hi_u8 mac_get_dtim_period(hi_u8 *puc_frame_body, hi_u16 us_frame_body_len);
hi_u8 mac_get_dtim_cnt(hi_u8 *puc_frame_body, hi_u16 us_frame_body_len);
hi_u8 mac_get_bss_type(hi_u16 us_cap_info);
hi_u8 *mac_get_wmm_ie(hi_u8 *puc_beacon_body, hi_u16 us_frame_len, hi_u16 us_offset);
hi_u8 *mac_find_ie(hi_u8 eid, hi_u8 *puc_ies, hi_u32 l_len);
hi_u8 *mac_find_vendor_ie(hi_u32 oui, hi_u8 oui_type, hi_u8 *puc_ies, hi_s32 l_len);
hi_u8 *mac_get_ssid(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len, hi_u8 *puc_ssid_len);
hi_u16 mac_get_beacon_period(const hi_u8 *puc_beacon_body);
hi_u16 mac_get_rsn_capability(const hi_u8 *puc_rsn_ie);
hi_u32 mac_check_mac_privacy_ap(hi_u16 us_cap_info, hi_u8 *mac_ap);
hi_u32 mac_check_mac_privacy_sta(hi_u16 us_cap_info, hi_u8 *mac_sta);
hi_u32 mac_check_privacy(mac_cap_info_stru *cap_info, hi_u8 *mac_vap);
hi_u8 mac_check_mac_privacy(hi_u16 us_cap_info, hi_u8 *vap);
#ifdef _PRE_WLAN_FEATURE_PMF
wlan_pmf_cap_status_uint8 mac_get_pmf_cap(hi_u8 *puc_ie, hi_u32 ie_len);
#endif
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
hi_void mac_set_meshid_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_hisi_mesh_optimization_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_mesh_configuration_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len);
hi_void mac_set_mesh_vendor_subtype(hi_u8 *puc_buffer, hi_u8 subtype, hi_u8 *puc_ie_len);
hi_u8 mac_check_is_mesh_vap(hi_u8 *puc_buffer, hi_u8 puc_ie_len);
hi_void mac_set_report_control_field(hi_void *vap, hi_u8 *puc_buffer, mesh_report_control_stru mesh_report_ctl);
hi_void mac_set_mesh_formation_info_field(hi_void *vap, hi_u8 *puc_buffer);
hi_void mac_set_mesh_capability_field(hi_void *vap, hi_u8 *puc_buffer);
hi_void mac_set_rsn_mesh_ie_authenticator(hi_void *vap, hi_u8 *puc_buffer, hi_u16 us_frm_type, hi_u8 *puc_ie_len);
hi_void mac_set_mesh_ssid_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len, hi_u8 is_mesh_req);
hi_u8 *mac_find_mesh_vendor_ie(hi_u8 oui_sub_type, hi_u8 *puc_ies, hi_u32 l_len);
hi_u8 *mac_get_meshid(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len, hi_u8 *puc_meshid_len);
hi_u8 mac_get_hisi_beacon_prio(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len);
hi_u8 mac_get_hisi_en_is_mbr(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len);
hi_u8 mac_get_hisi_accept_sta(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len);
#endif
#ifdef _PRE_WLAN_FEATURE_DBAC
hi_u8 mac_dbac_is_vip_data(const oal_dev_netbuf_stru *dev_netbuf);
hi_u8 mac_dbac_is_vip_mgmt(const oal_dev_netbuf_stru *dev_netbuf);
#endif
hi_void mac_get_qos_ctrl(const hi_u8 *puc_mac_hdr, hi_u8 *puc_qos_ctrl);
hi_u8 mac_check_is_vendor_action(hi_u32 oui, const hi_u8 *puc_ies, hi_u16 us_len);
hi_u8 mac_find_vendor_action(hi_u32 oui, hi_u8 oui_type, const hi_u8 *puc_ies, hi_s32 l_len);
hi_u16 mac_set_mgmt_frame_header(hi_u8 *mac_header, hi_u16 frame_type, const hi_u8 *addr1, const hi_u8 *addr2,
    const hi_u8 *addr3);


/* ****************************************************************************
 功能描述  : This function sets the 'frame control' bits in the MAC header of the
             input frame to the given 16-bit value.
**************************************************************************** */
static inline hi_void mac_hdr_set_frame_control(hi_u8 *puc_header, hi_u16 us_fc)
{
    *(hi_u16 *)puc_header = us_fc;
}

/* ****************************************************************************
 功能描述  : 设置MAC头duration字段
 输入参数  : puc_header : 指向mac帧头
             us_duration: 要设置的值
**************************************************************************** */
static inline hi_void mac_hdr_set_duration(hi_u8 *puc_header, hi_u16 frame_type, hi_u16 us_duration)
{
    hi_u16 *pus_dur = (hi_u16 *)(puc_header + WLAN_HDR_DUR_OFFSET);
    if (frame_type == WLAN_FC0_SUBTYPE_BEACON) {
        *pus_dur = 0;
    } else {
        *pus_dur = us_duration;
    }
}

/* ****************************************************************************
 功能描述  : 设置MAC头分片序号字段
 输入参数  : puc_header : 指向mac帧头
             uc_frag_num: 分片序号
**************************************************************************** */
static inline hi_void mac_hdr_set_fragment_number(hi_u8 *puc_header, hi_u8 frag_num)
{
    puc_header[WLAN_HDR_FRAG_OFFSET] &= 0xF0;
    puc_header[WLAN_HDR_FRAG_OFFSET] |= (frag_num & 0x0F);
}

/* ****************************************************************************
 功能描述  : This function sets the 'from ds' bit in the MAC header of the input frame
             to the given value stored in the LSB bit.
             The bit position of the 'from ds' in the 'frame control field' of the MAC
             header is represented by the bit pattern 0x00000010.
 输入参数  : puc_header－80211头部指针
             uc_from_ds －value
**************************************************************************** */
static inline hi_void mac_hdr_set_from_ds(hi_u8 *puc_header, hi_u8 from_ds)
{
    ((mac_header_frame_control_stru *)(puc_header))->from_ds = from_ds;
}

/* ****************************************************************************
 功能描述  : This function extracts the 'from ds' bit from the MAC header of the input frame.
             Returns the value in the LSB of the returned value.
 输入参数  : header－80211头部指针
**************************************************************************** */
static inline hi_u8 mac_hdr_get_from_ds(const hi_u8 *puc_header)
{
    return (hi_u8)((mac_header_frame_control_stru *)(puc_header))->from_ds;
}

/* ****************************************************************************
 功能描述  : This function sets the 'to ds' bit in the MAC header of the input frame
             to the given value stored in the LSB bit.
             The bit position of the 'to ds' in the 'frame control field' of the MAC
             header is represented by the bit pattern 0x00000001
 输入参数  : puc_header－80211头部指针
             uc_to_ds －value
**************************************************************************** */
static inline hi_void mac_hdr_set_to_ds(hi_u8 *puc_header, hi_u8 to_ds)
{
    ((mac_header_frame_control_stru *)(puc_header))->to_ds = to_ds;
}

/* ****************************************************************************
 功能描述  : This function extracts the 'to ds' bit from the MAC header of the input frame.
             Returns the value in the LSB of the returned value.
 输入参数  : puc_header－80211头部指针
**************************************************************************** */
static inline hi_u8 mac_hdr_get_to_ds(const hi_u8 *puc_header)
{
    return (hi_u8)((mac_header_frame_control_stru *)(puc_header))->to_ds;
}

/* ****************************************************************************
 功能描述  : 四地址获取帧头中的tid
**************************************************************************** */
static inline hi_u8 mac_get_tid_value_4addr(const hi_u8 *puc_header)
{
    return (puc_header[MAC_QOS_CTRL_FIELD_OFFSET_4ADDR] & 0x07); /* B0 - B2 */
}

/* ****************************************************************************
 功能描述  : 四地址获取帧头中的tid
**************************************************************************** */
static inline hi_u8 mac_get_tid_value(const hi_u8 *puc_header, hi_u8 is_4addr)
{
    if (is_4addr) {
        return (puc_header[MAC_QOS_CTRL_FIELD_OFFSET_4ADDR] & 0x07); /* B0 - B2 */
    } else {
        return (puc_header[MAC_QOS_CTRL_FIELD_OFFSET] & 0x07); /* B0 - B2 */
    }
}

/* ****************************************************************************
 功能描述  : 获取接受侦的seqence number
**************************************************************************** */
static inline hi_u16 mac_get_seq_num(const hi_u8 *puc_header)
{
    hi_u16 us_seq_num;

    us_seq_num = puc_header[23];         /* us_seq_num的bit 4 ~ 7,即为puc_header[23]的低4bit */
    us_seq_num <<= 4;                    /* 即将puc_header[23]的低4bit，放到us_seq_num的bit 4 ~ 7中 */
    us_seq_num |= (puc_header[22] >> 4); /* us_seq_num的最低4bit即为puc_header[22]的高4bit */
    return us_seq_num;
}

/* ****************************************************************************
 功能描述  : 设置帧的序列号
 输入参数  : [1]puc_header
             [2]us_seq_num
 返 回 值  : 无
**************************************************************************** */
static inline hi_void mac_set_seq_num(hi_u8 *puc_header, hi_u16 us_seq_num)
{
    puc_header[23] = (hi_u8)us_seq_num >> 4;    /* puc_header[23]的低4bit即为us_seq_num的bit 4 ~ 7 */
    puc_header[22] &= 0x0F;                     /* 将puc_header[22]的bit 4~7清0 */
    puc_header[22] |= (hi_u8)(us_seq_num << 4); /* puc_header[22]的高4bit即为us_seq_num的最低4bit */
}

/* ****************************************************************************
 功能描述  : 获取BAR帧中的start seq num值
**************************************************************************** */
static inline hi_u16 mac_get_bar_start_seq_num(const hi_u8 *puc_payload)
{
    hi_u16 seq_num = ((puc_payload[2] & 0xF0) >> 4) | /* puc_payload[2]右移位4bit以获取start seq num值的最低4bit */
        (puc_payload[3] << 4); /* puc_payload[3]左移位4bit以获取start seq num值的bit 4 ~ 7 */
    return seq_num;
}

/* ****************************************************************************
 功能描述  : 4地址获取qos帧确认策略
**************************************************************************** */
static inline hi_u8 mac_get_ack_policy_4addr(const hi_u8 *puc_header)
{
    return ((puc_header[MAC_QOS_CTRL_FIELD_OFFSET_4ADDR] & 0x60) >> 5); /* B5 - B6 */
}

/* ****************************************************************************
 功能描述  : 4地址获取qos帧确认策略
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_get_ack_policy(const hi_u8 *puc_header, hi_u8 is_4addr)
{
    if (is_4addr) {
        return ((puc_header[MAC_QOS_CTRL_FIELD_OFFSET_4ADDR] & 0x60) >> 5); /* B5 - B6 */
    } else {
        return ((puc_header[MAC_QOS_CTRL_FIELD_OFFSET] & 0x60) >> 5); /* B5 - B6 */
    }
}

/* ****************************************************************************
 功能描述  : 获取收到的帧的目的地址
             参考协议 <802.11权威指南> 81页
 输入参数  : 指向接收到帧的帧头指针
 输出参数  : 指向目的地址的指针
**************************************************************************** */
static inline hi_void mac_rx_get_da(mac_ieee80211_frame_stru *mac_header, hi_u8 **da_mac_addr)
{
    /* IBSS、from AP */
    if (mac_header->frame_control.to_ds == 0) {
        *da_mac_addr = mac_header->auc_address1;
    } else { /* WDS、to AP */
        *da_mac_addr = mac_header->auc_address3;
    }
}

/* ****************************************************************************
 功能描述  : 获取收到的帧的源地址
             参考协议 <802.11权威指南> 81页
 输入参数  : 指向接收到帧的帧头指针
 输出参数  : 指向源地址的指针
**************************************************************************** */
static inline hi_void mac_rx_get_sa(mac_ieee80211_frame_stru *mac_header, hi_u8 **sa_mac_addr)
{
    if (mac_header->frame_control.from_ds == 0) {
        /* IBSS、to AP */
        *sa_mac_addr = mac_header->auc_address2;
    } else if ((mac_header->frame_control.from_ds == 1) && (mac_header->frame_control.to_ds == 0)) {
        /* from AP */
        *sa_mac_addr = mac_header->auc_address3;
    } else {
        /* WDS */
        *sa_mac_addr = ((mac_ieee80211_frame_addr4_stru *)mac_header)->auc_address4;
    }
}

/* ****************************************************************************
 功能描述  : 获取收到的帧的发送端地址
             参考协议 <802.11权威指南> 81页
 输入参数  : 指向接收到帧的帧头指针
 输出参数  : 指向 TA 的指针
**************************************************************************** */
static inline hi_void mac_get_transmit_addr(mac_ieee80211_frame_stru *mac_header, hi_u8 **puc_bssid)
{
    /* 对于IBSS, STA, AP, WDS 场景下，获取发送端地址 */
    *puc_bssid = mac_header->auc_address2;
}

/*****************************************************************************
 功能描述  : 判断该帧是否是组播帧
**************************************************************************** */
static inline hi_u8 mac_is_grp_addr(const hi_u8 *mac_addr)
{
    return (mac_addr[0] & BIT0);
}

/* ****************************************************************************
 功能描述  : 通过帧头判断是否是action帧
**************************************************************************** */
static inline hi_u8 mac_ieeee80211_is_action(const hi_u8 *puc_header)
{
    hi_u8 is_action = ((puc_header[0] & (MAC_IEEE80211_FCTL_FTYPE | MAC_IEEE80211_FCTL_STYPE)) ==
        (WLAN_ACTION << 4)); /* WLAN_ACTION 占据bit 4~7 */
    return is_action;
}

/* ****************************************************************************
 功能描述  : 获取报文的子类型
 修改历史      :
  1.日    期   : 2013年4月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_get_frame_sub_type(const hi_u8 *puc_mac_header)
{
    return (puc_mac_header[0] & 0xFC);
}

/* ****************************************************************************
 功能描述  : 获取802.11帧子类型的值(0~15)
             帧第一个字节的高四位
**************************************************************************** */
static inline hi_u8 mac_frame_get_subtype_value(const hi_u8 *puc_mac_header)
{
    return ((puc_mac_header[0] & 0xF0) >> 4); /* 获取802.11帧子类型的值,右移4bit，即取值范围0~15 */
}

/* ****************************************************************************
 功能描述  : 获取报文类型
**************************************************************************** */
static inline hi_u8 mac_get_frame_type(const hi_u8 *puc_mac_header)
{
    return (puc_mac_header[0] & 0x0C);
}

/* ****************************************************************************
 功能描述  : 获取80211帧帧类型，取值0~2
 修改历史      :
  1.日    期   : 2013年12月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_frame_get_type_value(const hi_u8 *puc_mac_header)
{
    return (puc_mac_header[0] & 0x0C) >> 2; /* 获取80211帧帧类型，取值范围为0~2 */
}


/* ****************************************************************************
 功能描述  : 获取认证算法字段
**************************************************************************** */
static inline hi_u16 mac_get_auth_alg(const hi_u8 *puc_mac_hdr)
{
    hi_u16 auth_alg =
        (hi_u16)((puc_mac_hdr[MAC_80211_FRAME_LEN + 1] << 8) | /* puc_mac_hdr[24 + 1] 设置为auth_alg的bit 8 ~15 */
        puc_mac_hdr[MAC_80211_FRAME_LEN]);                     /* puc_mac_hdr[24] 设置为auth_alg的低8bit */
    return auth_alg;
}

/* ****************************************************************************
 功能描述  : 获取认证状态字段
**************************************************************************** */
static inline hi_u16 mac_get_auth_status(const hi_u8 *puc_mac_hdr)
{
    hi_u16 auth_status =
        (hi_u16)((puc_mac_hdr[MAC_80211_FRAME_LEN + 5] << 8) | /* puc_mac_hdr[24 + 5] 设置为auth_status的bit 8 ~15 */
        puc_mac_hdr[MAC_80211_FRAME_LEN + 4]);                 /* puc_mac_hdr[24 + 4] 设置为auth_status的低8bit */
    return auth_status;
}

/* ****************************************************************************
 功能描述  : 获取认证帧序列号
**************************************************************************** */
static inline hi_u16 mac_get_auth_seq_num(const hi_u8 *puc_mac_hdr)
{
    hi_u16 auth_seq_num =
        (hi_u16)((puc_mac_hdr[MAC_80211_FRAME_LEN + 3] << 8) | /* puc_mac_hdr[24 + 3] 设置为auth_seq_num的bit 8 ~15 */
        puc_mac_hdr[MAC_80211_FRAME_LEN + 2]);                 /* puc_mac_hdr[24 + 2] 设置为auth_seq_num的低8bit */
    return auth_seq_num;
}

/* ****************************************************************************
 功能描述  : 设置protected frame subfield
**************************************************************************** */
static inline hi_void mac_set_wep(hi_u8 *puc_hdr, hi_u8 wep)
{
    puc_hdr[1] &= 0xBF;
    puc_hdr[1] |= (hi_u8)(wep << 6); /* wep设置为protected frame subfield的bit 6开始的位置 */
}

/* ****************************************************************************
 功能描述  : 设置帧控制字段的受保护字段
**************************************************************************** */
static inline hi_void mac_set_protectedframe(hi_u8 *puc_mac_hdr)
{
    puc_mac_hdr[1] |= 0x40;
}

/* ****************************************************************************
 功能描述  : 获取帧头中保护位信息
**************************************************************************** */
static inline hi_u8 mac_get_protectedframe(hi_u8 *puc_mac_hdr)
{
    mac_ieee80211_frame_stru *mac_hdr = (mac_ieee80211_frame_stru *)puc_mac_hdr;
    return (hi_u8)(mac_hdr->frame_control.protected_frame);
}

/* ****************************************************************************
 功能描述  : 获取帧控制字段的受保护字段
**************************************************************************** */
static inline hi_u8 mac_is_protectedframe(const hi_u8 *puc_mac_hdr)
{
    return ((puc_mac_hdr[1] & 0x40) >> 6); /* 右移6 bit 获取帧控制字段的受保护字段 */
}

/* ****************************************************************************
 功能描述  : 获取认证帧中的challenge txt
**************************************************************************** */
static inline hi_u8 *mac_get_auth_ch_text(hi_u8 *puc_mac_hdr)
{
    return &(puc_mac_hdr[MAC_80211_FRAME_LEN + 6]); /* 偏移(MAC_80211_FRAME_LEN + 6)byte,获取认证帧中的challenge txt */
}

/* ****************************************************************************
 功能描述  : 是否为4地址 控制位为from ds | to ds均为1
**************************************************************************** */
static inline hi_u8 mac_is_4addr(const hi_u8 *puc_mac_hdr)
{
    return (hi_u8)(mac_hdr_get_to_ds(puc_mac_hdr) && mac_hdr_get_from_ds(puc_mac_hdr));
}

/* ****************************************************************************
 功能描述  : 拷贝MAC头的地址1
**************************************************************************** */
static inline hi_void mac_get_address1(const hi_u8 *puc_mac_hdr, hi_u8 mac_hdr_len, hi_u8 *mac_addr, hi_u8 addr_len)
{
    if (memcpy_s(mac_addr, addr_len, puc_mac_hdr + WLAN_HDR_ADDR1_OFFSET, mac_hdr_len) != EOK) {
        oam_error_log0(0, 0, "{mac_get_address1::memcpy_s fail.}");
        return;
    }
}

/* ****************************************************************************
 功能描述  : 拷贝MAC头的地址2
**************************************************************************** */
static inline hi_void mac_get_address2(const hi_u8 *puc_mac_hdr, hi_u8 mac_hdr_len, hi_u8 *mac_addr, hi_u8 addr_len)
{
    if (memcpy_s(mac_addr, addr_len, puc_mac_hdr + WLAN_HDR_ADDR2_OFFSET, mac_hdr_len) != EOK) {
        oam_error_log0(0, 0, "{mac_get_address2::memcpy_s fail.}");
        return;
    }
}

/* ****************************************************************************
 功能描述  : 拷贝MAC头的地址3
**************************************************************************** */
static inline hi_void mac_get_address3(const hi_u8 *puc_mac_hdr, hi_u8 mac_hdr_len, hi_u8 *mac_addr, hi_u8 addr_len)
{
    if (memcpy_s(mac_addr, addr_len, puc_mac_hdr + WLAN_HDR_ADDR3_OFFSET, mac_hdr_len) != EOK) {
        oam_error_log0(0, 0, "{mac_get_address3::memcpy_s fail.}");
        return;
    }
}

/* ****************************************************************************
 功能描述  : 获取关联帧中的状态信息
 输入参数  : puc_mac_header:关联帧
**************************************************************************** */
static inline mac_status_code_enum_uint16 mac_get_asoc_status(const hi_u8 *puc_mac_payload)
{
    mac_status_code_enum_uint16 mac_status;

    mac_status = (mac_status_code_enum_uint16)((puc_mac_payload[3] << 8) | /* payload[3]设置为mac_status bit 8 ~15 */
        puc_mac_payload[2]);                                               /* payload[2]设置为mac_status低8bit */
    return mac_status;
}

/* ****************************************************************************
 功能描述  : 获取关联帧中的关联ID
**************************************************************************** */
static inline hi_u16 mac_get_asoc_id(const hi_u8 *puc_mac_payload)
{
    hi_u16 us_asoc_id;

    us_asoc_id = puc_mac_payload[4] | /* payload[4]设置为asoc_id低8bit */
        (puc_mac_payload[5] << 8);    /* payload[5]设置为asoc_id低bit 8 ~ 15 */
    us_asoc_id &= 0x3FFF;             /* 取低14位 */
    return us_asoc_id;
}

/* ****************************************************************************
 功能描述  : 根据"from ds"bit,从帧中提取bssid(mac地址)
 输入参数  : puc_mac_header:mac帧头，puc_bssid:mac帧bssid
 输出参数  : puc_bssid:mac帧bssid，
**************************************************************************** */
static inline hi_void mac_get_bssid(const hi_u8 *puc_mac_hdr, hi_u8 *puc_bssid, hi_u8 ssid_len)
{
    if (mac_hdr_get_from_ds(puc_mac_hdr)) {
        mac_get_address2(puc_mac_hdr, WLAN_MAC_ADDR_LEN, puc_bssid, ssid_len);
    } else if (mac_hdr_get_to_ds(puc_mac_hdr)) {
        mac_get_address1(puc_mac_hdr, WLAN_MAC_ADDR_LEN, puc_bssid, ssid_len);
    } else {
        mac_get_address3(puc_mac_hdr, WLAN_MAC_ADDR_LEN, puc_bssid, ssid_len);
    }
}

/* ****************************************************************************
 功能描述  : 判断LLC 帧类型是否为EAPOL 类型帧
 输入参数  : mac_llc_snap_stru *pst_mac_llc_snap
 返 回 值  : HI_TRUE     是EAPOL 类型数据
             HI_FALSE  不是EAPOL 类型数据
**************************************************************************** */
static inline hi_bool mac_frame_is_eapol(const mac_llc_snap_stru *mac_llc_snap)
{
    return (hi_swap_byteorder_16(mac_llc_snap->us_ether_type) == ETHER_ONE_X_TYPE);
}

/* ****************************************************************************
 功能描述  : 获取action帧的Category
 输入参数  : [1]puc_data
 返 回 值  : hi_u16
 返 回 值  : hi_u8
**************************************************************************** */
static inline hi_u8 mac_get_action_category(const hi_u8 *puc_data)
{
    hi_u8 us_action_cate;
    us_action_cate = puc_data[0];
    return us_action_cate;
}

/* ****************************************************************************
 功能描述  :获取action帧的action code字段
 输入参数  :[1]puc_data
 返 回 值  :hi_u16
**************************************************************************** */
static inline hi_u8 mac_get_action_code(const hi_u8 *puc_data)
{
    hi_u8 us_action_code;
    us_action_code = puc_data[1];
    return us_action_code;
}

/* ****************************************************************************
 功能描述  : 填充tansmit beamforming capbilities域信息
 输入参数  : pst_vap :指向vap
             puc_buffer :指向buffer
 修改历史      :
  1.日    期   : 2013年4月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_set_txbf_cap_field(hi_u8 *puc_buffer)
{
    /* ************** Transmit Beamforming Capability Field *********************
     |-------------------------------------------------------------------------|
     |   Implicit | Rx Stagg | Tx Stagg  | Rx NDP   | Tx NDP   | Implicit      |
     |   TxBF Rx  | Sounding | Sounding  | Capable  | Capable  | TxBF          |
     |   Capable  | Capable  | Capable   |          |          | Capable       |
     |-------------------------------------------------------------------------|
     |      B0    |     B1   |    B2     |   B3     |   B4     |    B5         |
     |-------------------------------------------------------------------------|
     |              | Explicit | Explicit Non- | Explicit      | Explicit      |
     |  Calibration | CSI TxBF | Compr Steering| Compr steering| TxBF CSI      |
     |              | Capable  | Cap.          | Cap.          | Feedback      |
     |-------------------------------------------------------------------------|
     |  B6       B7 |   B8     |       B9      |       B10     | B11  B12      |
     |-------------------------------------------------------------------------|
     | Explicit Non- | Explicit | Minimal  | CSI Num of | Non-Compr Steering   |
     | Compr BF      | Compr BF | Grouping | Beamformer | Num of Beamformer    |
     | Fdbk Cap.     | Fdbk Cap.|          | Ants Supp  | Ants Supp            |
     |-------------------------------------------------------------------------|
     | B13       B14 | B15  B16 | B17  B18 | B19    B20 | B21        B22       |
     |-------------------------------------------------------------------------|
     | Compr Steering    | CSI Max Num of     |   Channel     |                |
     | Num of Beamformer | Rows Beamformer    | Estimation    | Reserved       |
     | Ants Supp         | Supported          | Capability    |                |
     |-------------------------------------------------------------------------|
     | B23           B24 | B25            B26 | B27       B28 | B29  B31       |
     |-------------------------------------------------------------------------|
    ************************************************************************** */
    /* 31h不支持TXBF 能力位清零 */
    puc_buffer[0] = 0;
    puc_buffer[1] = 0;
    puc_buffer[2] = 0; /* puc_buffer[2]清0 */
    puc_buffer[3] = 0; /* puc_buffer[3]清0 */
}

/* ****************************************************************************
 功能描述  : 填充asel(antenna selection) capabilities域信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 修改历史      :
  1.日    期   : 2013年4月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_set_asel_cap_field(hi_u8 *puc_buffer)
{
    /* ************* Antenna Selection Capability Field *************************
     |-------------------------------------------------------------------|
     |  Antenna  | Explicit CSI  | Antenna Indices | Explicit | Antenna  |
     | Selection | Fdbk based TX | Fdbk based TX   | CSI Fdbk | Indices  |
     |  Capable  | ASEL Capable  | ASEL Capable    | Capable  | Fdbk Cap.|
     |-------------------------------------------------------------------|
     |    B0     |     B1        |      B2         |    B3    |    B4    |
     |-------------------------------------------------------------------|

     |------------------------------------|
     |  RX ASEL |   Transmit   |          |
     |  Capable |   Sounding   | Reserved |
     |          | PPDU Capable |          |
     |------------------------------------|
     |    B5    |     B6       |    B7    |
     |------------------------------------|
    ************************************************************************** */
    /* 31h 清0 均不支持 */
    puc_buffer[0] = 0;
}

/* ****************************************************************************
 功能描述  : 判断是否是wmm ie
 修改历史      :
  1.日    期   : 2013年6月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_is_wmm_ie(const hi_u8 *puc_ie)
{
    /* --------------------------------------------------------------------- */
    /* WMM Information/Parameter Element Format                              */
    /* --------------------------------------------------------------------- */
    /* | OUI | OUIType | OUISubtype | Version | QoSInfo | OUISubtype based | */
    /* --------------------------------------------------------------------- */
    /* |3    | 1       | 1          | 1       | 1       | ---------------- | */
    /* --------------------------------------------------------------------- */
    if ((puc_ie[0] == MAC_EID_WMM) && (puc_ie[2] == MAC_WMM_OUI_BYTE_ONE) &&            /* check puc_ie[0]、[2] */
        (puc_ie[3] == MAC_WMM_OUI_BYTE_TWO) && (puc_ie[4] == MAC_WMM_OUI_BYTE_THREE) && /* check puc_ie[3]、[4] */
        (puc_ie[5] == MAC_OUITYPE_WMM) && /* puc_ie[5] check是否为WMM Type */
        ((puc_ie[6] == MAC_OUISUBTYPE_WMM_INFO) || (puc_ie[6] == MAC_OUISUBTYPE_WMM_PARAM)) && /* check puc_ie[6] */
        (puc_ie[7] == MAC_OUI_WMM_VERSION)) { /* puc_ie[7] check是否为Version field 0x1 */
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 设置Channel Switch Announcement IE
 输入参数  : pst_mac_vap: MAC VAP结构体指针
 输出参数  : puc_buffer : 帧体指针
             puc_ie_len : IE的长度
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2014年3月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_set_csa_ie(hi_u8 channel, hi_u8 csa_cnt, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    /*  Channel Switch Announcement Information Element Format               */
    /* --------------------------------------------------------------------- */
    /* | Element ID | Length | Chnl Switch Mode | New Chnl | Ch Switch Cnt | */
    /* --------------------------------------------------------------------- */
    /* | 1          | 1      | 1                | 1        | 1             | */
    /* --------------------------------------------------------------------- */
    /* 设置Channel Switch Announcement Element */
    puc_buffer[0] = MAC_EID_CHANSWITCHANN;
    puc_buffer[1] = MAC_CHANSWITCHANN_LEN;
    puc_buffer[2] = 1;       /* ask all associated STAs to stop transmission:byte 2 */
    puc_buffer[3] = channel; /* byte 3 设置为信道号 */
    puc_buffer[4] = csa_cnt; /* byte 4 设置为信道切换计数值 */
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_CHANSWITCHANN_LEN;
}

#ifdef _PRE_WLAN_FEATURE_BW_HIEX
/* ****************************************************************************
 功能描述  : 获取私有NOA帧的离开时间(ms)
 修改历史      :
  1.日    期   : 2019年4月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_get_noa_duration(const hi_u8 *puc_ies, hi_u16 us_len, hi_u16 *pus_duration)
{
    if (us_len < MAC_ACTION_VENDOR_SPECIFIC_IE_POS + 2) { /* duration字段由2个字节组成,单位为us */
        oam_warning_log1(0, OAM_SF_DBAC, "{mac_get_noa_duration: msg len %d.}", us_len);
        return HI_FALSE;
    }
    *pus_duration = puc_ies[MAC_ACTION_VENDOR_SPECIFIC_IE_POS] << 8; /* puc_ies[6]存于duration bit 8 ~ 15 */
    *pus_duration |= (hi_u16)puc_ies[MAC_ACTION_VENDOR_SPECIFIC_IE_POS + 1];

    return HI_TRUE;
}
#endif

/* ****************************************************************************
 * 功能描述  : 设置帧头mac地址
 * 输入参数  : addr1，addr2，addr3: mac addrsss
 * *************************************************************************** */
static inline hi_u16 mac_hdr_set_mac_addrsss(hi_u8 *mac_header, const hi_u8 *addr1, const hi_u8 *addr2,
    const hi_u8 *addr3)
{
    if (memcpy_s(mac_header + WLAN_HDR_ADDR1_OFFSET, WLAN_MAC_ADDR_LEN, addr1, WLAN_MAC_ADDR_LEN) != EOK ||
        memcpy_s(mac_header + WLAN_HDR_ADDR2_OFFSET, WLAN_MAC_ADDR_LEN, addr2, WLAN_MAC_ADDR_LEN) != EOK ||
        memcpy_s(mac_header + WLAN_HDR_ADDR3_OFFSET, WLAN_MAC_ADDR_LEN, addr3, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_warning_log0(0, OAM_SF_TX_CHAIN, "{mac_set_mgmt_frame_header_mac_addrsss: memcpy_s failed.}");
        return 0;
    }
    return (hi_u16)(WLAN_HDR_ADDR3_OFFSET + WLAN_MAC_ADDR_LEN);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __MAC_FRAME_H__ */
