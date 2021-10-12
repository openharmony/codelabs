/* *
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

/* *
 * @file hi_wifi_api.h
 * @defgroup hi_wifi_basic WiFi Basic Settings
 * @ingroup hi_wifi
 */

#ifndef __HI_WIFI_API_H__
#define __HI_WIFI_API_H__

#include "hdf_ibus_intf.h"
#include "wifi_inc.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* *
 * mac transform string.CNcomment:地址转为字符串.CNend
 */
#ifndef MACSTR
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#endif

/* *
 * @ingroup hi_wifi_basic
 *
 * TKIP of cipher mode.CNcomment:加密方式为TKIP.CNend
 */
#define WIFI_CIPHER_TKIP BIT(3)

/* *
 * @ingroup hi_wifi_basic
 *
 * CCMP of cipher mode.CNcomment:加密方式为CCMP.CNend
 */
#define WIFI_CIPHER_CCMP BIT(4)

/* *
 * @ingroup hi_wifi_basic
 *
 * Channel numbers of 2.4G frequency.CNcomment:2.4G频段的信道数量.CNend
 */
#define WIFI_24G_CHANNEL_NUMS 14

/* *
 * @ingroup hi_wifi_basic
 *
 * max interiface name length.CNcomment:网络接口名最大长度.CNend
 */
#define WIFI_IFNAME_MAX_SIZE 16

/* *
 * @ingroup hi_wifi_basic
 *
 * The minimum timeout of a single reconnection.CNcomment:最小单次重连超时时间.CNend
 */
#define WIFI_MIN_RECONNECT_TIMEOUT 2

/* *
 * @ingroup hi_wifi_basic
 *
 * The maximum timeout of a single reconnection, representing an infinite number of loop reconnections.
 * CNcomment:最大单次重连超时时间，表示无限次循环重连.CNend
 */
#define WIFI_MAX_RECONNECT_TIMEOUT 65535

/* *
 * @ingroup hi_wifi_basic
 *
 * The minimum auto reconnect interval.CNcomment:最小自动重连间隔时间.CNend
 */
#define WIFI_MIN_RECONNECT_PERIOD 1

/* *
 * @ingroup hi_wifi_basic
 *
 * The maximum auto reconnect interval.CNcomment:最大自动重连间隔时间.CNend
 */
#define WIFI_MAX_RECONNECT_PERIOD 65535

/* *
 * @ingroup hi_wifi_basic
 *
 * The minmum times of auto reconnect.CNcomment:最小自动重连连接次数.CNend
 */
#define WIFI_MIN_RECONNECT_TIMES 1

/* *
 * @ingroup hi_wifi_basic
 *
 * The maximum times of auto reconnect.CNcomment:最大自动重连连接次数.CNend
 */
#define WIFI_MAX_RECONNECT_TIMES 65535

/* *
 * @ingroup hi_wifi_basic
 *
 * max scan number of ap.CNcomment:支持扫描ap的最多数目.CNend
 */
#define WIFI_SCAN_AP_LIMIT 32

/* *
 * @ingroup hi_wifi_basic
 *
 * length of status buff.CNcomment:获取连接状态字符串的长度.CNend
 */
#define WIFI_STATUS_BUF_LEN_LIMIT 512

/* *
 * @ingroup hi_wifi_basic
 *
 * Decimal only WPS pin code length.CNcomment:WPS中十进制pin码长度.CNend
 */
#define WIFI_WPS_PIN_LEN 8

/* *
 * @ingroup hi_wifi_basic
 *
 * default max num of vap.CNcomment:默认支持的设备最大个数.CNend
 */
#define WIFI_MAX_NUM_VAP 3

/* *
 * @ingroup hi_wifi_basic
 *
 * default max num of user.CNcomment:默认支持的用户最大个数.CNend
 */
#define WIFI_MAX_NUM_USER 4

/* *
 * @ingroup hi_wifi_basic
 *
 * default max num of station.CNcomment:默认支持的station最大个数.CNend
 */
#define WIFI_DEFAULT_MAX_NUM_STA WIFI_MAX_NUM_USER

/* *
 * @ingroup hi_wifi_basic
 *
 * return success value.CNcomment:返回成功标识.CNend
 */
#define HISI_OK 0

/* *
 * @ingroup hi_wifi_basic
 *
 * return failed value.CNcomment:返回值错误标识.CNend
 */
#define HISI_FAIL (-1)

/* *
 * @ingroup hi_wifi_basic
 *
 * Max length of SSID.CNcomment:SSID最大长度定义.CNend
 */
#define HI_WIFI_MAX_SSID_LEN 32

/* *
 * @ingroup hi_wifi_basic
 *
 * Length of MAC address.CNcomment:MAC地址长度定义.CNend
 */
#define HI_WIFI_MAC_LEN 6

/* *
 * @ingroup hi_wifi_basic
 *
 * String length of bssid, eg. 00:00:00:00:00:00.CNcomment:bssid字符串长度定义(00:00:00:00:00:00).CNend
 */
#define HI_WIFI_TXT_ADDR_LEN 17

/* *
 * @ingroup hi_wifi_basic
 *
 * Length of Key.CNcomment:KEY 密码长度定义.CNend
 */
#define HI_WIFI_AP_KEY_LEN 64

/* *
 * @ingroup hi_wifi_basic
 *
 * Maximum  length of Key.CNcomment:KEY 最大密码长度.CNend
 */
#define HI_WIFI_MAX_KEY_LEN 64

/* *
 * @ingroup hi_wifi_basic
 *
 * Return value of invalid channel.CNcomment:无效信道返回值.CNend
 */
#define HI_WIFI_INVALID_CHANNEL 0xFF

/* *
 * @ingroup hi_wifi_basic
 *
 * Index of Vendor IE.CNcomment:Vendor IE 最大索引.CNend
 */
#define HI_WIFI_VENDOR_IE_MAX_IDX 1

/* *
 * @ingroup hi_wifi_basic
 *
 * Max length of Vendor IE.CNcomment:Vendor IE 最大长度.CNend
 */
#define HI_WIFI_VENDOR_IE_MAX_LEN 255

/* *
 * @ingroup hi_wifi_basic
 *
 * Minimum length of custom's frame.CNcomment:用户定制报文最小长度值.CNend
 */
#define HI_WIFI_CUSTOM_PKT_MIN_LEN 24

/* *
 * @ingroup hi_wifi_basic
 *
 * Max length of custom's frame.CNcomment:用户定制报文最大长度值.CNend
 */
#define HI_WIFI_CUSTOM_PKT_MAX_LEN 1400

/* *
 * @ingroup hi_wifi_basic
 *
 * Max num of retry.CNcomment:软件重传的最大次数.CNend
 */
#define HI_WIFI_RETRY_MAX_NUM 15

/* *
 * @ingroup hi_wifi_basic
 *
 * Max time of retry.CNcomment:软件重传的最大时间.CNend
 */
#define HI_WIFI_RETRY_MAX_TIME 200

/* *
 * @ingroup hi_wifi_basic
 *
 * Freq compensation param count.CNcomment:（高温）频偏补偿参数个数.CNend
 */
#define HI_WIFI_FREQ_COMP_PARAM_CNT 3

/* *
 * @ingroup hi_wifi_basic
 *
 * country code bits .CNcomment:国家码字节数.CNend
 */
#define HI_WIFI_COUNTRY_CODE_BITS_CNT 3

/* *
 * @ingroup hi_wifi_basic
 *
 * DBB scale param count.CNcomment:dbb scale相关参数个数.CNend
 */
#define HI_WIFI_DBB_PARAM_CNT 7

/* *
 * @ingroup hi_wifi_basic
 *
 * Ch depend tx power offset count.CNcomment:信道相关发送功率参数个数.CNend
 */
#define HI_WIFI_CH_TX_PWR_PARAM_CNT 13

/* *
 * @ingroup hi_wifi_basic
 *
 * Reporting data type of monitor mode.CNcomment:混杂模式上报的数据类型.CNend
 */
typedef enum {
    HI_WIFI_MONITOR_OFF,                /**< close monitor mode. CNcomment: 关闭混杂模式.CNend */
    HI_WIFI_MONITOR_MCAST_DATA,         /**< report multi-cast data frame. CNcomment: 上报组播(广播)数据包.CNend */
    HI_WIFI_MONITOR_UCAST_DATA,         /**< report single-cast data frame. CNcomment: 上报单播数据包.CNend */
    HI_WIFI_MONITOR_MCAST_MANAGEMENT,   /**< report multi-cast mgmt frame. CNcomment: 上报组播(广播)管理包.CNend */
    HI_WIFI_MONITOR_UCAST_MANAGEMENT,   /**< report sigle-cast mgmt frame. CNcomment: 上报单播管理包.CNend */

    HI_WIFI_MONITOR_BUTT
} hi_wifi_monitor_mode;

/* *
 * @ingroup hi_wifi_basic
 *
 * Definition of protocol frame type.CNcomment:协议报文类型定义.CNend
 */
typedef enum {
    HI_WIFI_PKT_TYPE_BEACON,        /**< Beacon packet. CNcomment: Beacon包.CNend */
    HI_WIFI_PKT_TYPE_PROBE_REQ,     /**< Probe Request packet. CNcomment: Probe Request包.CNend */
    HI_WIFI_PKT_TYPE_PROBE_RESP,    /**< Probe Response packet. CNcomment: Probe Response包.CNend */
    HI_WIFI_PKT_TYPE_ASSOC_REQ,     /**< Assoc Request packet. CNcomment: Assoc Request包.CNend */
    HI_WIFI_PKT_TYPE_ASSOC_RESP,    /**< Assoc Response packet. CNcomment: Assoc Response包.CNend */

    HI_WIFI_PKT_TYPE_BUTT
} hi_wifi_pkt_type;

/* *
 * @ingroup hi_wifi_basic
 *
 * Interface type of wifi.CNcomment:wifi 接口类型.CNend
 */
typedef enum {
    HI_WIFI_IFTYPE_UNSPECIFIED,
    HI_WIFI_IFTYPE_ADHOC,
    HI_WIFI_IFTYPE_STATION = 2, /* *< Station. CNcomment: STA类型.CNend */
    HI_WIFI_IFTYPE_AP = 3,      /* *< SoftAp. CNcomment: SoftAp类型.CNend */
    HI_WIFI_IFTYPE_AP_VLAN,
    HI_WIFI_IFTYPE_WDS,
    HI_WIFI_IFTYPE_MONITOR,
    HI_WIFI_IFTYPE_MESH_POINT = 7, /* *< Mesh. CNcomment: Mesh类型.CNend */
    HI_WIFI_IFTYPE_P2P_CLIENT,
    HI_WIFI_IFTYPE_P2P_GO,
    HI_WIFI_IFTYPE_P2P_DEVICE,

    HI_WIFI_IFTYPES_BUTT
} hi_wifi_iftype;

/* *
 * @ingroup hi_wifi_basic
 *
 * Definition of bandwidth type.CNcomment:接口带宽定义.CNend
 */
typedef enum {
    HI_WIFI_BW_HIEX_5M,    /* *< 5M bandwidth. CNcomment: 窄带5M带宽.CNend */
    HI_WIFI_BW_HIEX_10M,   /* *< 10M bandwidth. CNcomment: 窄带10M带宽.CNend */
    HI_WIFI_BW_LEGACY_20M, /* *< 20M bandwidth. CNcomment: 20M带宽.CNend */
    HI_WIFI_BW_BUTT
} hi_wifi_bw;

/* *
 * @ingroup hi_wifi_basic
 *
 * The protocol mode of softap and station interfaces.CNcomment:softap和station接口的protocol模式.CNend
 */
typedef enum {
    HI_WIFI_PHY_MODE_11BGN, /* *< 802.11BGN mode. CNcomment: 802.11BGN 模式.CNend */
    HI_WIFI_PHY_MODE_11BG,  /* *< 802.11BG mode. CNcomment: 802.11BG 模式.CNend */
    HI_WIFI_PHY_MODE_11B,   /* *< 802.11B mode. CNcomment: 802.11B 模式.CNend */
    HI_WIFI_PHY_MODE_BUTT
} hi_wifi_protocol_mode;

/* *
 * @ingroup hi_wifi_basic
 *
 * Authentication type enum.CNcomment:认证类型(连接网络不支持HI_WIFI_SECURITY_WPAPSK).CNend
 */
typedef enum {
    HI_WIFI_SECURITY_OPEN,               /* *< OPEN. CNcomment: 认证类型:开放.CNend */
    HI_WIFI_SECURITY_WEP,                /* *< WEP. CNcomment: 认证类型:WEP.CNend */
    HI_WIFI_SECURITY_WPA2PSK,            /* *< WPA-PSK. CNcomment: 认证类型:WPA2-PSK.CNend */
    HI_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX, /* *< WPA/WPA2-PSK MIX. CNcomment: 认证类型:WPA-PSK/WPA2-PSK混合.CNend */
    HI_WIFI_SECURITY_WPAPSK,             /* *< WPA-PSK. CNcomment: 认证类型:WPA-PSK.CNend */
    HI_WIFI_SECURITY_WPA,                /* *< WPA. CNcomment: 认证类型:WPA.CNend */
    HI_WIFI_SECURITY_WPA2,               /* *< WPA2. CNcomment: 认证类型:WPA2.CNend */
    HI_WIFI_SECURITY_SAE,                /* *< SAE. CNcomment: 认证类型:SAE.CNend */
    HI_WIFI_SECURITY_UNKNOWN             /* *< UNKNOWN. CNcomment: 其他认证类型:UNKNOWN.CNend */
} hi_wifi_auth_mode;

/* *
 * @ingroup hi_wifi_basic
 *
 * Encryption type enum.CNcoment:加密类型.CNend
 *
 */
typedef enum {
    HI_WIFI_PARIWISE_UNKNOWN,     /* *< UNKNOWN. CNcomment: 加密类型:UNKNOWN.CNend */
    HI_WIFI_PAIRWISE_AES,         /* *< AES. CNcomment: 加密类型:AES.CNend */
    HI_WIFI_PAIRWISE_TKIP,        /* *< TKIP. CNcomment: 加密类型:TKIP.CNend */
    HI_WIFI_PAIRWISE_TKIP_AES_MIX /* *< TKIP/AES MIX. CNcomment: 加密类型:TKIP AES混合.CNend */
} hi_wifi_pairwise;

/* *
 * @ingroup hi_wifi_basic
 *
 * PMF type enum.CNcomment:PMF管理帧保护模式类型.CNend
 */
typedef enum {
    HI_WIFI_MGMT_FRAME_PROTECTION_CLOSE,    /* *< Disable. CNcomment: 管理帧保护模式:关闭.CNend */
    HI_WIFI_MGMT_FRAME_PROTECTION_OPTIONAL, /* *< Optional. CNcomment: 管理帧保护模式:可选.CNend */
    HI_WIFI_MGMT_FRAME_PROTECTION_REQUIRED  /* *< Required. CNcomment: 管理帧保护模式:必须.CNend */
} hi_wifi_pmf_options;

/* *
 * @ingroup hi_wifi_basic
 *
 * Type of connect's status.CNcomment:连接状态.CNend
 */
typedef enum {
    HI_WIFI_DISCONNECTED, /* *< Disconnected. CNcomment: 连接状态:未连接.CNend */
    HI_WIFI_CONNECTED,    /* *< Connected. CNcomment: 连接状态:已连接.CNend */
} hi_wifi_conn_status;

/* *
 * @ingroup hi_wifi_basic
 *
 * wifi's operation mode.CNcomment:wifi的工作模式.CNend
 */
typedef enum {
    HI_WIFI_MODE_INFRA = 0,    /* *< STA模式 */
    HI_WIFI_MODE_AP    = 2,    /* *< AP 模式 */
    HI_WIFI_MODE_MESH  = 5     /* *< MESH 模式 */
} hi_wifi_mode;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of WiFi event.CNcomment:WiFi的事件类型.CNend
 */
typedef enum {
    HI_WIFI_EVT_UNKNOWN,             /* *< UNKNWON. CNcomment: UNKNOWN.CNend */
    HI_WIFI_EVT_SCAN_DONE,           /* *< Scan finish. CNcomment: STA扫描完成.CNend */
    HI_WIFI_EVT_CONNECTED,           /* *< Connected. CNcomment: 已连接.CNend */
    HI_WIFI_EVT_DISCONNECTED,        /* *< Disconnected. CNcomment: 断开连接.CNend */
    HI_WIFI_EVT_WPS_TIMEOUT,         /* *< WPS timeout. CNcomment: WPS事件超时.CNend */
    HI_WIFI_EVT_MESH_CONNECTED,      /* *< MESH connected. CNcomment: MESH已连接.CNend */
    HI_WIFI_EVT_MESH_DISCONNECTED,   /* *< MESH disconnected. CNcomment: MESH断开连接.CNend */
    HI_WIFI_EVT_AP_START,            /* *< AP start. CNcomment: AP开启.CNend */
    HI_WIFI_EVT_STA_CONNECTED,       /* *< STA connected with ap. CNcomment: AP和STA已连接.CNend */
    HI_WIFI_EVT_STA_DISCONNECTED,    /* *< STA disconnected from ap. CNcomment: AP和STA断开连接.CNend */
    HI_WIFI_EVT_STA_FCON_NO_NETWORK, /* *< STA connect, but can't find network. CNcomment: STA连接时扫描不到网络.CNend */
    HI_WIFI_EVT_MESH_CANNOT_FOUND,   /* *< MESH can't find network. CNcomment: MESH关联扫不到对端.CNend */
    HI_WIFI_EVT_MESH_SCAN_DONE,      /* *< MESH AP scan finish. CNcomment: MESH AP扫描完成.CNend */
    HI_WIFI_EVT_MESH_STA_SCAN_DONE,  /* *< MESH STA scan finish. CNcomment: MESH STA扫描完成.CNend */
    HI_WIFI_EVT_AP_SCAN_DONE,        /* *< AP scan finish. CNcomment: AP扫描完成.CNend */
    HI_WIFI_EVT_BUTT
} hi_wifi_event_type;

/* *
 * @ingroup hi_wifi_basic
 *
 * Scan type enum.CNcomment:扫描类型.CNend
 */
typedef enum {
    HI_WIFI_BASIC_SCAN,       /* *< Common and all channel scan. CNcomment: 普通扫描.CNend */
    HI_WIFI_CHANNEL_SCAN,     /* *< Specified channel scan. CNcomment: 指定信道扫描.CNend */
    HI_WIFI_SSID_SCAN,        /* *< Specified SSID scan. CNcomment: 指定SSID扫描.CNend */
    HI_WIFI_SSID_PREFIX_SCAN, /* *< Prefix SSID scan. CNcomment: SSID前缀扫描.CNend */
    HI_WIFI_BSSID_SCAN,       /* *< Specified BSSID scan. CNcomment: 指定BSSID扫描.CNend */
} hi_wifi_scan_type;

/* *
 * @ingroup iot_lp
 * Sleep level enumeration.
 */
typedef enum {
    HI_NO_SLEEP,    /* *< no sleep type.CNcomment:不睡模式 CNend */
    HI_LIGHT_SLEEP, /* *< light sleep type.CNcomment:浅睡模式 CNend */
    HI_DEEP_SLEEP,  /* *< deep sleep type.CNcomment:深睡模式 CNend */
} hi_plat_type;

/* *
 * @ingroup hi_wifi_basic
 * wow pattern type
 */
typedef enum {
    HI_WOW_PATTERN_ADD,
    HI_WOW_PATTERN_DEL,
    HI_WOW_PATTERN_CLR,
} hi_wifi_wow_pattern_type;

/* *
 * @ingroup hi_wifi_basic
 *
 * parameters of scan.CNcomment:station接口scan参数.CNend
 */
typedef struct {
    char ssid[HI_WIFI_MAX_SSID_LEN + 1];  /* *< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    unsigned char bssid[HI_WIFI_MAC_LEN]; /* *< BSSID. CNcomment: BSSID.CNend */
    unsigned char ssid_len;               /* *< SSID length. CNcomment: SSID长度.CNend */
    unsigned char channel;                /* *< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
    hi_wifi_scan_type scan_type;          /* *< Scan type. CNcomment: 扫描类型.CNend */
} hi_wifi_scan_params;

/* *
 * @ingroup hi_wifi_basic
 *
 * Struct of scan result.CNcomment:扫描结果结构体.CNend
 */
typedef struct {
    char ssid[HI_WIFI_MAX_SSID_LEN + 1];  /* *< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    unsigned char bssid[HI_WIFI_MAC_LEN]; /* *< BSSID. CNcomment: BSSID.CNend */
    unsigned int channel;                 /* *< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
    hi_wifi_auth_mode auth;               /* *< Authentication type. CNcomment: 认证类型.CNend */
    int rssi;                             /* *< Signal Strength. CNcomment: 信号强度.CNend */
    unsigned char wps_flag : 1;           /* *< WPS flag. CNcomment: WPS标识.CNend */
    unsigned char wps_session : 1;        /* *< WPS session:PBC-0/PIN-1. CNcomment: WPS会话类型,PBC-0/PIN-1.CNend */
    unsigned char wmm : 1;                /* *< WMM flag. CNcomment: WMM标识.CNend */
    unsigned char resv : 1;               /* *< Reserved. CNcomment: 预留.CNend */
    unsigned char hisi_mesh_flag : 1;     /* *< MESH flag. CNcomment: MESH标识.CNend */
} hi_wifi_ap_info;

/* *
 * @ingroup hi_wifi_basic
 *
 * Struct of connect parameters.CNcomment:station连接结构体.CNend
 */
typedef struct {
    char ssid[HI_WIFI_MAX_SSID_LEN + 1];  /* *< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    hi_wifi_auth_mode auth;               /* *< Authentication mode. CNcomment: 认证类型.CNend */
    char key[HI_WIFI_MAX_KEY_LEN + 1];    /* *< Secret key. CNcomment: 秘钥.CNend */
    unsigned char bssid[HI_WIFI_MAC_LEN]; /* *< BSSID. CNcomment: BSSID.CNend */
    hi_wifi_pairwise pairwise;            /* *< Encryption type. CNcomment: 加密方式,不需指定时置0.CNend */
} hi_wifi_assoc_request;

/* *
 * @ingroup hi_wifi_basic
 *
 * Struct of fast connect parameters.CNcomment:station快速连接结构体.CNend
 */
typedef struct {
    hi_wifi_assoc_request req; /* *< Association request. CNcomment: 关联请求.CNend */
    unsigned char channel;     /* *< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
} hi_wifi_fast_assoc_request;

/* *
 * @ingroup hi_wifi_basic
 *
 * Status of sta's connection.CNcomment:获取station连接状态.CNend
 */
typedef struct {
    char ssid[HI_WIFI_MAX_SSID_LEN + 1];  /* *< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    unsigned char bssid[HI_WIFI_MAC_LEN]; /* *< BSSID. CNcomment: BSSID.CNend */
    unsigned int channel;                 /* *< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
    hi_wifi_conn_status status;           /* *< Connect status. CNcomment: 连接状态.CNend */
} hi_wifi_status;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of wifi scan done.CNcomment:扫描完成事件.CNend
 */
typedef struct {
    unsigned short bss_num; /* *< numbers of scan result. CNcomment: 扫描到的ap数目.CNend */
} event_wifi_scan_done;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of wifi connected CNcomment:wifi的connect事件信息.CNend
 */
typedef struct {
    char ssid[HI_WIFI_MAX_SSID_LEN + 1];   /* *< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    unsigned char bssid[HI_WIFI_MAC_LEN];  /* *< BSSID. CNcomment: BSSID.CNend */
    unsigned char ssid_len;                /* *< SSID length. CNcomment: SSID长度.CNend */
    char ifname[WIFI_IFNAME_MAX_SIZE + 1]; /* *< Iftype name. CNcomment: 接口名称.CNend */
} event_wifi_connected;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of wifi disconnected.CNcomment:wifi的断开事件信息.CNend
 */
typedef struct {
    unsigned char bssid[HI_WIFI_MAC_LEN];  /* *< BSSID. CNcomment: BSSID.CNend */
    unsigned short reason_code;            /* *< reason code. CNcomment: 断开原因.CNend */
    char ifname[WIFI_IFNAME_MAX_SIZE + 1]; /* *< Iftype name. CNcomment: 接口名称.CNend */
} event_wifi_disconnected;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of ap connected sta.CNcomment:ap连接sta事件信息.CNend
 */
typedef struct {
    char addr[HI_WIFI_MAC_LEN]; /* *< user's mac address of SoftAp. CNcomment: 连接AP的sta地址.CNend */
} event_ap_sta_connected;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of ap disconnected sta.CNcomment:ap断开sta事件信息.CNend
 */
typedef struct {
    unsigned char addr[HI_WIFI_MAC_LEN]; /* *< User's mac address of SoftAp. CNcomment: AP断开STA的MAC地址.CNend */
    unsigned short reason_code;          /* *< Reason code. CNcomment: AP断开连接的原因值.CNend */
} event_ap_sta_disconnected;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of ap start.CNcomment:ap启动事件信息.CNend
 */
typedef struct {
    char ifname[WIFI_IFNAME_MAX_SIZE + 1]; /* *< Iftype name. CNcomment: 接口名称.CNend */
} event_ap_start;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of mesh connected.CNcomment:mesh的connect事件信息.CNend
 */
typedef struct {
    unsigned char addr[HI_WIFI_MAC_LEN]; /* *< User's mac address of MESH. CNcomment: MESH连接的peer MAC地址.CNend */
} event_mesh_connected;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type of mesh disconnected.CNcomment:mesh的disconnect事件信息.CNend
 */
typedef struct {
    unsigned char addr[HI_WIFI_MAC_LEN]; /* *< User's mac address of MESH. CNcomment: 断开连接的peer MAC地址.CNend */
    unsigned short reason_code;          /* *< Reason code. CNcomment: MESH断开连接的原因.CNend */
} event_mesh_disconnected;

/* *
 * @ingroup hi_wifi_basic
 *
 * Event type wifi information.CNcomment:wifi的事件信息体.CNend
 */
typedef union {
    event_wifi_scan_done wifi_scan_done;       /* *< Scan finish event. CNcomment: WIFI扫描完成事件信息.CNend */
    event_wifi_connected wifi_connected;       /* *< STA's connected event. CNcomment: STA的连接事件信息.CNend */
    event_wifi_disconnected wifi_disconnected; /* *< STA's dsiconnected event. CNcomment: STA的断连事件信息.CNend */
    event_ap_sta_connected ap_sta_connected;   /* *< AP's connected event . CNcomment: AP的连接事件信息.CNend */
    event_ap_sta_disconnected ap_sta_disconnected; /* *< AP's disconnected event. CNcomment: AP的断连事件信息.CNend */
    event_ap_start ap_start;                   /* *< AP's start success event. CNcomment: AP启动成功信息.CNend */
    event_mesh_connected mesh_connected;       /* *< MESH's connected event. CNcomment: MESH连接事件信息.CNend */
    event_mesh_disconnected mesh_disconnected; /* *< MESH's disconnected event. CNcomment: MESH断连事件信息.CNend */
} hi_wifi_event_info;

/* *
 * @ingroup hi_wifi_basic
 *
 * Struct of WiFi event.CNcomment:WiFi事件结构体.CNend
 *
 */
typedef struct {
    hi_wifi_event_type event; /* *< Event type. CNcomment: 事件类型.CNend */
    hi_wifi_event_info info;  /* *< Event information. CNcomment: 事件信息.CNend */
} hi_wifi_event;

/* *
 * @ingroup hi_wifi_basic
 *
 * Struct of softap's basic config.CNcomment:softap基本配置.CNend
 *
 */
typedef struct {
    char ssid[HI_WIFI_MAX_SSID_LEN + 1];    /* *< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    char key[HI_WIFI_AP_KEY_LEN + 1];       /* *< Secret key. CNcomment: 秘钥.CNend */
    unsigned char channel_num;              /* *< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
    int ssid_hidden;                        /* *< Hidden ssid. CNcomment: 是否隐藏SSID.CNend */
    hi_wifi_auth_mode authmode;             /* *< Authentication mode. CNcomment: 认证方式.CNend */
    hi_wifi_pairwise pairwise;              /* *< Encryption type. CNcomment: 加密方式,不需指定时置0.CNend */
} hi_wifi_softap_config;

/* *
 * @ingroup hi_wifi_basic
 *
 * mac address of softap's user.CNcomment:与softap相连的station mac地址.CNend
 *
 */
typedef struct {
    unsigned char mac[HI_WIFI_MAC_LEN]; /* *< MAC address.CNcomment:与softap相连的station mac地址.CNend */
} hi_wifi_ap_sta_info;

/* *
 * @ingroup hi_wifi_basic
 *
 * Struct of frame filter config in monitor mode.CNcomment:混杂模式报文接收过滤设置.CNend
 */
typedef struct {
    char mdata_en : 1; /* *< get multi-cast data frame flag. CNcomment: 使能接收组播(广播)数据包.CNend */
    char udata_en : 1; /* *< get single-cast data frame flag. CNcomment: 使能接收单播数据包.CNend */
    char mmngt_en : 1; /* *< get multi-cast mgmt frame flag. CNcomment: 使能接收组播(广播)管理包.CNend */
    char umngt_en : 1; /* *< get single-cast mgmt frame flag. CNcomment: 使能接收单播管理包.CNend */
    char resvd    : 4; /* *< reserved bits. CNcomment: 保留字段.CNend */
} hi_wifi_ptype_filter;

/* *
 * @ingroup hi_wifi_basic
 *
 * Struct of customize params.CNcomment:配置定制化参数.CNend
 */
typedef struct {
    char country_code[HI_WIFI_COUNTRY_CODE_BITS_CNT];
    int rssi_offset;
    unsigned int freq_comp[HI_WIFI_FREQ_COMP_PARAM_CNT];
    unsigned int dbb_params[HI_WIFI_DBB_PARAM_CNT];
    unsigned int ch_txpwr_offset[HI_WIFI_CH_TX_PWR_PARAM_CNT];
} hi_wifi_customize_params;

/* *
 * @ingroup hi_wifi_basic
 *
 * Struct of report goodput.CNcomment:吞吐量数据.CNend
 */
typedef struct {
    unsigned int be_datarate;
    unsigned int be_avg_retry;
    unsigned int bk_datarate;
    unsigned int bk_avg_retry;
    unsigned int vi_datarate;
    unsigned int vi_avg_retry;
    unsigned int vo_datarate;
    unsigned int vo_avg_retry;
} hi_wifi_report_tx_params;

typedef enum {
    DEV_PANIC = 1,
    DRIVER_HUNG,
    UNKNOWN,
} hi_wifi_driver_event;

/* *
 * @ingroup hi_wifi_driver_event_cb
 *
 * report driving events to application layer.CNcommment:向应用层上报驱动事件.CNend
 */
typedef int (*hi_wifi_driver_event_cb)(hi_wifi_driver_event event);

/* *
 * @ingroup hi_wifi_basic
 *
 * callback function definition of monitor mode.CNcommment:混杂模式收包回调接口定义.CNend
 */
typedef int (*hi_wifi_promis_cb)(void *recv_buf, int frame_len, signed char rssi);

/* *
 * @ingroup hi_wifi_basic
 *
 * callback function definition of wifi event.CNcommment:wifi事件回调接口定义.CNend
 */
typedef void (*hi_wifi_event_cb)(const hi_wifi_event *event);

/* *
 * @ingroup hi_wifi_basic
 *
 * callback function definition of wifi event to get goodput and average send times.
 * CNcommment:wifi获取吞吐量、平均发送次数事件回调接口定义.CNend
 */
typedef unsigned int (*hi_wifi_report_tx_params_callback)(hi_wifi_report_tx_params *);

/* *
* @ingroup  hi_wifi_basic
* @brief  Wifi initialize.CNcomment:wifi初始化.CNend
*
* @par Description:
        Wifi driver initialize.CNcomment:wifi驱动初始化，不创建wifi设备.CNend
*
* @attention  NULL
* @param  max_port_count   [IN]  Type #uint8_t, max port num
*
* @retval #HISI_OK  Excute successfully
* @retval #Other    Error code
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since Hi3881_V100R001C00
*/
int hi_wifi_init(uint8_t max_port_count, struct BusDev *bus);


/* *
 * @ingroup  hi_wifi_basic
 * @brief  Wifi de-initialize.CNcomment:wifi去初始化.CNend
 *
 * @par Description:
 * Wifi driver de-initialize.CNcomment:wifi驱动去初始化.CNend
 *
 * @attention  NULL
 * @param  NULL
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other    Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3861_V100R001C00
 */
int hi_wifi_deinit(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set protocol mode of sta.CNcomment:设置station接口的protocol模式.CNend
 *
 * @par Description:
 * Set protocol mode of sta, set before calling hi_wifi_sta_start().\n
 * CNcomment:配置station接口的protocol模式, 在sta start之前调用.CNend
 *
 * @attention  Default mode 802.11BGN CNcomment:默认模式 802.11BGN.CNend
 * @param  mode            [IN]     Type #hi_wifi_protocol_mode, protocol mode.
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_set_protocol_mode(hi_wifi_protocol_mode mode);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get protocol mode of.CNcomment:获取station接口的protocol模式.CNend
 *
 * @par Description:
 * Get protocol mode of station.CNcomment:获取station接口的protocol模式.CNend
 *
 * @attention  NULL
 * @param      NULL
 *
 * @retval #hi_wifi_protocol_mode protocol mode.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
hi_wifi_protocol_mode hi_wifi_sta_get_protocol_mode(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Config pmf settings of sta.CNcomment:配置station的pmf.CNend
 *
 * @par Description:
 * Config pmf settings of sta, set before sta start.CNcomment:配置station的pmf, 在sta start之前调用.CNend
 *
 * @attention  Default pmf enum value 1. CNcomment:默认pmf枚举值1.CNend
 * @param  pmf           [IN]     Type #hi_wifi_pmf_options, pmf enum value.CNcoment:pmf枚举值.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_set_pmf(hi_wifi_pmf_options pmf);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get pmf settings of sta.CNcomment:获取station的pmf设置.CNend
 *
 * @par Description:
 * Get pmf settings of sta.CNcomment:获取station的pmf设置.CNend
 *
 * @attention  NULL
 * @param      NULL
 *
 * @retval #hi_wifi_pmf_options pmf enum value.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
hi_wifi_pmf_options hi_wifi_get_pmf(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Start wifi station.CNcomment:开启STA.CNend
 *
 * @par Description:
 * Start wifi station.CNcomment:开启STA.CNend
 *
 * @attention  1. Multiple interfaces of the same type are not supported.CNcomment:1. 不支持使用多个同类型接口.CNend\n
 * 2. Dual interface coexistence support: STA + AP or STA + MESH.
 * CNcomment:2. 双接口共存支持：STA + AP or STA + MESH.CNend\n
 * 3. Start timeout 5s.CNcomment:3. 启动超时时间5s.CNend\n
 * 4. The memories of <ifname> and <len> should be requested by the caller，
 * the input value of len must be the same as the length of ifname（the recommended length is 17Bytes）.\n
 * CNcomment:4. <ifname>和<len>由调用者申请内存，用户写入len的值必须与ifname长度一致（建议长度为17Bytes）.CNend
 * @param  ifname          [IN/OUT]     Type #char *, device name.CNcomment:接口名.CNend
 * @param  len             [IN/OUT]     Type #int *, length of device name.CNcomment:接口名长度.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_start(char *ifname, int *len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Close wifi station.CNcomment:关闭STA.CNend
 *
 * @par Description:
 * Close wifi station.CNcomment:关闭STA.CNend
 *
 * @attention  NULL
 * @param  NULL
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_stop(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Start sta basic scanning in all channels.CNcomment:station进行全信道基础扫描.CNend
 *
 * @par Description:
 * Start sta basic scanning in all channels.CNcomment:启动station全信道基础扫描.CNend
 *
 * @attention  NULL
 * @param     NULL
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_scan(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Start station scanning with specified parameter.CNcomment:station执行带特定参数的扫描.CNend
 *
 * @par Description:
 * Start station scanning with specified parameter.CNcomment:station执行带特定参数的扫描.CNend
 *
 * @attention  1. advance scan can scan with ssid only,channel only,bssid only,prefix_ssid only，
 * and the combination parameters scanning does not support.\n
 * CNcomment:1. 高级扫描分别单独支持 ssid扫描，信道扫描，bssid扫描，ssid前缀扫描, 不支持组合参数扫描方式.CNend\n
 * 2. Scanning mode, subject to the type set by scan_type.
 * CNcomment:2 .扫描方式，以scan_type传入的类型为准。CNend\n
 * 3. SSID only supports ASCII characters.
 * CNcomment:3. SSID 只支持ASCII字符.CNend
 * @param  sp            [IN]    Type #hi_wifi_scan_params * parameters of scan.CNcomment:扫描网络参数设置.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_advance_scan(hi_wifi_scan_params *sp);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  sta start scan.CNcomment:station进行扫描.CNend
 *
 * @par Description:
 * Get station scan result.CNcomment:获取station扫描结果.CNend
 * @attention  1. The memories of <ap_list> and <ap_num> memories are requested by the caller. \n
 * The <ap_list> size up to : sizeof(hi_wifi_ap_info ap_list) * 32. \n
 * CNcomment:1. <ap_list>和<ap_num>由调用者申请内存,
 * <ap_list>size最大为：sizeof(hi_wifi_ap_info ap_list) * 32.CNend \n
 * 2. ap_num: parameters can be passed in to specify the number of scanned results.The maximum is 32. \n
 * CNcomment:2. ap_num: 可以传入参数，指定获取扫描到的结果数量，最大为32。CNend \n
 * 3. If the user callback function is used, ap num refers to bss_num in event_wifi_scan_done. \n
 * CNcomment:3. 如果使用上报用户的回调函数，ap_num参考event_wifi_scan_done中的bss_num。CNend \n
 * 4. ap_num should be same with number of hi_wifi_ap_info structures applied,
 * Otherwise, it will cause memory overflow. \n
 * CNcomment:4. ap_num和申请的hi_wifi_ap_info结构体数量一致，否则可能造成内存溢出。CNend \n
 * 5. SSID only supports ASCII characters. \n
 * CNcomment:5. SSID 只支持ASCII字符.CNend \n
 * 6. The rssi in the scan results needs to be divided by 100 to get the actual rssi.\n
 * CNcomment:6. 扫描结果中的rssi需要除以100才能获得实际的rssi.CNend
 * @param  ap_list         [IN/OUT]    Type #hi_wifi_ap_info * scan result.CNcomment:扫描的结果.CNend
 * @param  ap_num          [IN/OUT]    Type #unsigned int *, number of scan result.CNcomment:扫描到的网络数目.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_scan_results(hi_wifi_ap_info *ap_list, unsigned int *ap_num);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  sta start connect.CNcomment:station进行连接网络.CNend
 *
 * @par Description:
 * sta start connect.CNcomment:station进行连接网络.CNend
 *
 * @attention  1.<ssid> and <bssid> cannot be empty at the same time. CNcomment:1. <ssid>与<bssid>不能同时为空.CNend\n
 * 2. When <auth_type> is set to OPEN, the <passwd> parameter is not required.
 * CNcomment:2. <auth_type>设置为OPEN时，无需<passwd>参数.CNend\n
 * 3. This function is non-blocking.CNcomment:3. 此函数为非阻塞式.CNend\n
 * 4. Pairwise can be set, default is 0.CNcomment:4. pairwise 可设置, 默认为0.CNend\n
 * 5. If the station is already connected to a network, disconnect the existing connection and
 * then connect to the new network.\n
 * CNcomment:5. 若station已接入某个网络，则先断开已有连接，然后连接新网络.CNend\n
 * 6. If the wrong SSID, BSSID or key is passed in, the HISI_OK will be returned,
 * but sta cannot connect the ap.
 * CNcomment:6. 如果传入错误的ssid，bssid或者不正确的密码，返回成功，但连接ap失败。CNend\n
 * 7. SSID only supports ASCII characters.
 * CNcomment:7. SSID 只支持ASCII字符.CNend \n
 * 8. Only support auth mode as bellow:
 * HI_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX,
 * HI_WIFI_SECURITY_WPA2PSK,
 * HI_WIFI_SECURITY_WEP,
 * HI_WIFI_SECURITY_OPEN
 * CNcomment:8. 只支持以下认证模式：
 * HI_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX,
 * HI_WIFI_SECURITY_WPA2PSK,
 * HI_WIFI_SECURITY_WEP,
 * HI_WIFI_SECURITY_OPEN \n
 * 9. WEP supports 64 bit and 128 bit encryption.
 * for 64 bit encryption, the encryption key is 10 hexadecimal characters or 5 ASCII characters;
 * for 128 bit encryption, the encryption key is 26 hexadecimal characters or 13 ASCII characters。\n
 * CNcomment:9. WEP支持64位和128位加密，对于64位加密，加密密钥为10个十六进制字符或5个ASCII字符；
 * 对于128位加密，加密密钥为26个十六进制字符或13个ASCII字符。CNend\n
 * 10. When the key of WEP is in the form of ASCII character,
 * the key in the input struct needs to be added with double quotation marks;
 * when the key of WEP is in the form of hexadecimal character,
 * the key in the input struct does not need to add double quotation marks.\n
 * CNcomment:10. WEP的秘钥为ASCIl字符形式时，入参结构体中的key需要添加双引号；
 * WEP的秘钥为为十六进制字符时，入参结构体中的key不需要添加双引号。CNend\n
 *
 * @param  req    [IN]    Type #hi_wifi_assoc_request * connect parameters of network.CNcomment:连接网络参数设置.CNend
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_connect(hi_wifi_assoc_request *req);

/* *
* @ingroup  hi_wifi_basic
* @brief  Start fast connect.CNcomment:station进行快速连接网络.CNend
*
* @par Description:
*           Start fast connect.CNcomment:station进行快速连接网络.CNend
*
* @attention  1. <ssid> and <bssid> cannot be empty at the same time. CNcomment:1．<ssid>与<bssid>不能同时为空.CNend\n
*             2. When <auth_type> is set to OPEN, the <passwd> parameter is not required.
*                CNcomment:2．<auth_type>设置为OPEN时，无需<passwd>参数.CNend\n
*             3. <chn> There are differences in the range of values, and China is 1-13.
*                CNcomment:3．<chn>取值范围不同区域有差异，中国为1-13.CNend\n
*             4. This function is non-blocking.CNcomment:4．此函数为非阻塞式.CNend\n
*             5. Pairwise can be set, set to zero by default.CNcomment:5. pairwise 可设置,默认置零.CNend\n
*             6. If the wrong SSID, BSSID or key is passed in, the HISI_FAIL will be returned,
*                and sta cannot connect the ap.
*                CNcomment:7. 如果传入错误的ssid，bssid或者不正确的密码，返回失败并且连接ap失败。CNend\n
*             7. SSID only supports ASCII characters.
*                CNcomment:8. SSID 只支持ASCII字符.CNend \n
* @param fast_request [IN] Type #hi_wifi_fast_assoc_request *,fast connect parameters. CNcomment:快速连接网络参数.CNend

* @retval #HISI_OK        Execute successfully.
* @retval #HISI_FAIL      Execute failed.
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since Hi3881_V100R001C00
*/
int hi_wifi_sta_fast_connect(hi_wifi_fast_assoc_request *fast_request);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Disconnect from network.CNcomment:station断开相连的网络.CNend
 *
 * @par Description:
 * Disconnect from network.CNcomment:station断开相连的网络.CNend
 *
 * @attention  NULL
 * @param  NULL
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_disconnect(void);

/* *
* @ingroup  hi_wifi_basic
* @brief  Set reconnect policy.CNcomment:station设置重新连接网络机制.CNend
*
* @par Description:
*           Set reconnect policy.CNcomment:station设置重新连接网络机制.CNend
*
* @attention  1. It is recommended called after STA start or connected successfully.
*             CNcomment:1. 在STA启动后或者关联成功后调用该接口.CNend\n
*             2. The reconnection policy will be triggered when the station is disconnected from ap.\n
*             CNcomment:2. 重连机制将于station下一次去关联时生效,当前已经去关联设置无效.CNend\n
*             3. The Settings will take effect on the next reconnect timer.\n
*             CNcomment:3. 重关联过程中更新重关联配置将于下一次重连计时生效.CNend\n
*             4. After calling station connect/disconnect or station stop, stop reconnecting.
*             CNcomment:4. 调用station connect/disconnect或station stop，停止重连.CNend\n
*             5. If the target network cannot be found by scanning,
                 the reconnection policy cannot trigger to take effect.\n
*             CNcomment:5. 若扫描不到目标网络，重连机制无法触发生效.CNend\n
*             6. When the <seconds> value is 65535, it means infinite loop reconnection.
*             CNcomment:6. <seconds>取值为65535时，表示无限次循环重连.CNend\n
*             7.Enable reconnect, user and lwip will not receive disconnect event when disconnected from ap until 15
*               seconds later and still don't reconnect to ap successfully.
*             CNcomment:7. 使能自动重连,wifi将在15s内尝试自动重连并在此期间不上报去关联事件到用户和lwip协议栈,
*                          做到15秒内重连成功用户和上层网络不感知.CNend\n
*             8.Must call again if add/down/delete SoftAp or MESH's interface status after last call.
*             CNcomment:8. 调用后如果添加/禁用/删除了SoftAp,MESH接口的状态,需要再次调用该接口.CNend\n

* @param  enable        [IN]    Type #int enable reconnect.0-disable/1-enable.CNcomment:使能重连网络参数.CNend
* @param  seconds       [IN]    Type #unsigned int reconnect timeout in seconds for once, range:[2-65535].
*                                                  CNcomment:单次重连超时时间，取值[2-65535].CNend
* @param  period        [IN]    Type #unsigned int reconnect period in seconds, range:[1-65535].
                                                   CNcomment:重连间隔周期，取值[1-65535].CNend
* @param  max_try_count [IN]    Type #unsigned int max reconnect try count number，range:[1-65535].
                                                   CNcomment:最大重连次数，取值[1-65535].CNend
*
* @retval #HISI_OK        Execute successfully.
* @retval #HISI_FAIL      Execute failed.
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since Hi3881_V100R001C00
*/
int hi_wifi_sta_set_reconnect_policy(int enable, unsigned int seconds, unsigned int period, unsigned int max_try_count);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get status of sta.CNcomment:获取station连接的网络状态.CNend
 *
 * @par Description:
 * Get status of sta.CNcomment:获取station连接的网络状态.CNend
 *
 * @attention  NULL
 * @param  connect_status  [IN/OUT]    Type #hi_wifi_status *, connect status， memory is requested by the caller.
 * CNcomment:连接状态, 由调用者申请内存.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_get_connect_info(hi_wifi_status *connect_status);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Start pbc connect in WPS.CNcomment:设置WPS进行pbc连接.CNend
 *
 * @par Description:
 * Start pbc connect in WPS.CNcomment:设置WPS进行pbc连接.CNend
 *
 * @attention  1. bssid can be NULL or MAC. CNcomment:1. bssid 可以指定mac或者填NULL.CNend
 * @param  bssid   [IN]  Type #unsigned char * mac address
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_wps_pbc(unsigned char *bssid);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Start pin connect in WPS.CNcomment:WPS通过pin码连接网络.CNend
 *
 * @par Description:
 * Start pin connect in WPS.CNcomment:WPS通过pin码连接网络.CNend
 *
 * @attention  1. Bssid can be NULL or MAC. CNcomment:1. bssid 可以指定mac或者填NULL.CNend \n
 * 2. Decimal only WPS pin code length is 8 Bytes.CNcomment:2. WPS中pin码仅限十进制，长度为8 Bytes.CNend
 * @param  pin      [IN]   Type #char * pin code
 * @param  bssid    [IN]   Type #unsigned char * mac address
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_wps_pin(char *pin, unsigned char *bssid);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get pin code.CNcomment:WPS获取pin码.CNend
 *
 * @par Description:
 * Get pin code.CNcomment:WPS获取pin码.CNend
 *
 * @attention  Decimal only WPS pin code length is 8 Bytes.CNcomment:WPS中pin码仅限十进制，长度为8 Bytes.CNend
 * @param  pin    [IN/OUT]   Type #char *, pin code buffer, should be obtained, length is 9 Bytes.
 * The memory is requested by the caller.\n
 * CNcomment:待获取pin码,长度为9 Bytes。由调用者申请内存.CNend
 * @param  len    [IN]   Type #unsigned int, length of pin code.CNcomment:pin码的长度.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_wps_pin_get(char *pin, unsigned int len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  register user callback interface.CNcomment:注册回调函数接口.CNend
 *
 * @par Description:
 * register user callback interface.CNcomment:注册回调函数接口.CNend
 *
 * @attention  NULL
 * @param  event_cb  [OUT]    Type #hi_wifi_event_cb, event callback .CNcomment:回调函数.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_register_event_callback(hi_wifi_event_cb event_cb);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set protocol mode of softap.CNcomment:设置softap接口的protocol模式.CNend
 *
 * @par Description:
 * Set protocol mode of softap.CNcomment:设置softap接口的protocol模式.CNend\n
 * Initiallize config, set before softap start.CNcomment:初始配置,在softap start之前调用.CNend
 *
 * @attention  Default mode(802.11BGN) CNcomment:默认模式（802.11BGN）.CNend
 * @param  mode            [IN]     Type  #hi_wifi_protocol_mode protocol mode.
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_softap_set_protocol_mode(hi_wifi_protocol_mode mode);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get protocol mode of softap.CNcomment:获取softap接口的protocol模式.CNend
 *
 * @par Description:
 * Get protocol mode of softap.CNcomment:获取softap接口的protocol模式.CNend
 *
 * @attention  NULL
 * @param      NULL
 *
 * @retval #hi_wifi_protocol_mode protocol mode.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
hi_wifi_protocol_mode hi_wifi_softap_get_protocol_mode(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set softap's beacon interval.CNcomment:设置softap的beacon周期.CNend
 *
 * @par Description:
 * Set softap's beacon interval.CNcomment:设置softap的beacon周期.CNend. \n
 * Initialized config sets before interface starts.CNcomment:初始配置softap启动之前调用.CNend
 *
 * @attention  NULL
 * @param  beacon_period      [IN]     Type  #int beacon period in milliseconds, range(33ms~1000ms), default(100ms)
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_softap_set_beacon_period(int beacon_period);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set softap's dtim count.CNcomment:设置softap的dtim周期.CNend
 *
 * @par Description:
 * Set softap's dtim count.CNcomment:设置softap的dtim周期.CNend \n
 * Initialized config sets before interface starts.CNcomment:初始配置softap启动之前调用.CNend
 *
 * @attention  NULL
 * @param  dtim_period     [IN]     Type  #int, dtim period , range(1~30), default(2)
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_softap_set_dtim_period(int dtim_period);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set update time of softap's group key.CNcomment:配置softap组播秘钥更新时间.CNend
 *
 * @par Description:
 * Set update time of softap's group key.CNcomment:配置softap组播秘钥更新时间.CNend\n
 * Initialized config sets before interface starts.CNcomment:初始配置softap启动之前调用.CNend\n
 * If you need to use the rekey function, it is recommended to use WPA+WPA2-PSK + CCMP encryption.
 * CNcomment:若需要使用rekey功能，推荐使用WPA+WPA2-PSK + CCMP加密方式.CNend
 *
 * @attention  When using wpa2psk-only + CCMP encryption, rekey is forced to 86400s by default.
 * CNcomment:当使用wpa2psk-only + CCMP加密方式时  ，rekey默认强制改为 86400.CNend
 * @param  wpa_group_rekey [IN]     Type  #int, update time in seconds, range(30s-86400s), default(86400s)
 * CNcomment:更新时间以秒为单位，范围（30s-86400s）,默认（86400s）.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_softap_set_group_rekey(int wifi_group_rekey);

/* *
* @ingroup  hi_wifi_basic
* @brief  Set short-gi of softap.CNcomment:设置softap的SHORT-GI功能.CNend
*
* @par Description:
*           Enable or disable short-gi of softap.CNcomment:开启或则关闭softap的SHORT-GI功能.CNend\n
*           Initialized config sets before interface starts.CNcomment:初始配置softap启动之前调用.CNend
* @attention  NULL
* @param  flag            [IN]    Type  #int, enable(1) or disable(0). default enable(1).
                                        CNcomment:使能标志，默认使能（1）.CNend
*
* @retval #HISI_OK        Execute successfully.
* @retval #HISI_FAIL      Execute failed.
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since Hi3881_V100R001C00
*/
int hi_wifi_softap_set_shortgi(int flag);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Start softap interface.CNcomment:开启SoftAP.CNend
 *
 * @par Description:
 * Start softap interface.CNcomment:开启SoftAP.CNend
 *
 * @attention  1. Multiple interfaces of the same type are not supported.CNcomment:不支持使用多个同类型接口.CNend\n
 * 2. Dual interface coexistence support: STA + AP. CNcomment:双接口共存支持：STA + AP.CNend \n
 * 3. Start timeout 5s.CNcomment:启动超时时间5s。CNend \n
 * 4. Softap key length range(8 Bytes - 64 Bytes).CNcomment:softap key长度范围（8 Bytes - 64 Bytes）.CNend \n
 * 5. Only support auth mode as bellow: \n
 * HI_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX, \n
 * HI_WIFI_SECURITY_WPA2PSK, \n
 * HI_WIFI_SECURITY_OPEN \n
 * CNcomment:5. 只支持以下认证模式：\n
 * HI_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX, \n
 * HI_WIFI_SECURITY_WPA2PSK, \n
 * HI_WIFI_SECURITY_OPEN.CNend \n
 * 6. The memories of <ifname> and <len> should be requested by the caller，
 * the input value of len must be the same as the length of ifname（the recommended length is 17Bytes）.\n
 * CNcomment:6. <ifname>和<len>由调用者申请内存，用户写入len的值必须与ifname长度一致（建议长度为17Bytes）.CNend \n
 * 7. SSID only supports ASCII characters. \n
 * CNcomment:7. SSID 只支持ASCII字符.CNend
 * @param  conf            [IN]      Type  #hi_wifi_softap_config *, softap's configuration.CNcomment:SoftAP配置.CNend
 * @param  ifname          [IN/OUT]  Type  #char *, interface name.CNcomment:接口名字.CNend
 * @param  len             [IN/OUT]  Type  #int *, interface name length.CNcomment:接口名字长度.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_softap_start(hi_wifi_softap_config *conf, char *ifname, int *len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Close softap interface.CNcomment:关闭SoftAP.CNend
 *
 * @par Description:
 * Close softap interface.CNcomment:关闭SoftAP.CNend
 *
 * @attention  NULL
 * @param  NULL
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_softap_stop(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get all user's information of softap.CNcomment:softap获取已连接的station的信息.CNend
 *
 * @par Description:
 * Get all user's information of softap.CNcomment:softap获取已连接的station的信息.CNend
 *
 * @attention  1.sta_list: malloc by user.CNcomment:1.扫描结果参数。由用户动态申请。CNend \n
 * 2.sta_list: max size is hi_wifi_ap_sta_info * 6.
 * CNcomment:2.sta_list 足够的结构体大小，最大为hi_wifi_ap_sta_info * 6。CNend \n
 * 3.sta_num:parameters can be passed in to specify the number of connected sta.The maximum is 6.
 * CNcomment:3.可以传入参数，指定获取已接入的sta个数，最大为6。CNend \n
 * 4.sta_num should be the same with number of hi_wifi_ap_sta_info structures applied, Otherwise,
 * it will cause memory overflow.\n
 * CNcomment:4.sta_num和申请的hi_wifi_ap_sta_info结构体数量一致，否则可能造成内存溢出。CNend
 * @param  sta_list        [IN/OUT]  Type  #hi_wifi_ap_sta_info *, station information.CNcomment:STA信息.CNend
 * @param  sta_num         [IN/OUT]  Type  #unsigned int *, station number.CNcomment:STA个数.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_softap_get_connected_sta(hi_wifi_ap_sta_info *sta_list, unsigned int *sta_num);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Softap deauth user by mac address.CNcomment:softap指定断开连接的station网络.CNend
 *
 * @par Description:
 * Softap deauth user by mac address.CNcomment:softap指定断开连接的station网络.CNend
 *
 * @attention  NULL
 * @param  addr             [IN]     Type  #const unsigned char *, station mac address.CNcomment:MAC地址.CNend
 * @param  addr_len         [IN]     Type  #unsigned char, station mac address length, must be 6.
 * CNcomment:MAC地址长度,必须为6.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_softap_deauth_sta(const unsigned char *addr, unsigned char addr_len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  set mac address.CNcomment:设置MAC地址.CNend
 *
 * @par Description:
 * Set original mac address.CNcomment:设置起始mac地址.CNend\n
 * mac address will increase or recycle when adding or deleting device.
 * CNcomment:添加设备mac地址递增，删除设备回收对应的mac地址.CNend
 *
 * @attention  NULL
 * @param  mac_addr          [IN]     Type #char *, mac address.CNcomment:MAC地址.CNend
 * @param  mac_len           [IN]     Type #unsigned char, mac address length.CNcomment:MAC地址长度.CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other    Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_set_macaddr(const char *mac_addr, unsigned char mac_len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  get mac address.CNcomment:获取MAC地址.CNend
 *
 * @par Description:
 * Get original mac address.CNcomment:获取mac地址.CNend\n
 * mac address will increase or recycle when adding device or deleting device.
 * CNcomment:添加设备mac地址递增，删除设备回收对应的mac地址.CNend
 *
 * @attention  NULL
 * @param  mac_addr          [OUT]    Type #char *, mac address.
 * @param  mac_len           [IN]     Type #unsigned char, mac address length.
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other    Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_get_macaddr(char *mac_addr, unsigned char mac_len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set country code.CNcomment:设置国家码.CNend
 *
 * @par Description:
 * Set country code(two uppercases).CNcomment:设置国家码，由两个大写字符组成.CNend
 *
 * @attention  1.Before setting the country code, you must call hi_wifi_init to complete the initialization.
 * CNcomment:设置国家码之前，必须调用hi_wifi_init初始化完成.CNend\n
 * 2.cc_len should be greater than or equal to 3.CNcomment:cc_len应大于等于3.CNend
 * @param  cc               [IN]     Type  #char *, country code.CNcomment:国家码.CNend
 * @param  cc_len           [IN]     Type  #unsigned char, country code length.CNcomment:国家码长度.CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_set_country(const char *cc, unsigned char cc_len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get country code.CNcomment:获取国家码.CNend
 *
 * @par Description:
 * Get country code.CNcomment:获取国家码，由两个大写字符组成.CNend
 *
 * @attention  1.Before getting the country code, you must call hi_wifi_init to complete the initialization.
 * CNcomment:获取国家码之前，必须调用hi_wifi_init初始化完成.CNend
 * @param  cc               [OUT]     Type  #char *, country code.CNcomment:国家码.CNend
 * @param  len              [IN/OUT]  Type  #int *, country code length.CNcomment:国家码长度.CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_get_country(char *cc, int *len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set bandwidth.CNcomment:设置带宽.CNend
 *
 * @par Description:
 * Set bandwidth, support 5M/10M/20M.CNcomment:设置接口的工作带宽，支持5M 10M 20M带宽的设置.CNend
 *
 * @attention  NULL
 * @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
 * @param  ifname_len       [IN]     Type  #unsigned char, interface name length.CNcomment:接口名长度.CNend
 * @param  bw               [IN]     Type  #hi_wifi_bw, bandwidth enum.CNcomment:带宽.CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_set_bandwidth(const char *ifname, unsigned char ifname_len, hi_wifi_bw bw);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get bandwidth.CNcomment:获取带宽.CNend
 *
 * @par Description:
 * Get bandwidth.CNcomment:获取带宽.CNend
 *
 * @attention  NULL
 * @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
 * @param  ifname_len       [IN]     Type  #unsigned char, interface name length.CNcomment:接口名长度.CNend
 *
 * @retval #bandwidth enum.CNcomment:带宽的枚举值.CNend
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
hi_wifi_bw hi_wifi_get_bandwidth(const char *ifname, unsigned char ifname_len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set channel.CNcomment:设置信道.CNend
 *
 * @par Description:
 * Set channel.CNcomment:设置信道.CNend
 *
 * @attention  NULL
 * @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
 * @param  ifname_len       [IN]     Type  #unsigned char, interface name length.CNcomment:接口名长度.CNend
 * @param  channel          [IN]     Type  #int , listen channel.CNcomment:信道号.CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_set_channel(const char *ifname, unsigned char ifname_len, int channel);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get channel.CNcomment:获取信道.CNend
 *
 * @par Description:
 * Get channel.CNcomment:获取信道.CNend
 *
 * @attention  NULL
 * @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
 * @param  ifname_len       [IN]     Type  #unsigned char, interface name length.CNcomment:接口名长度.CNend
 *
 * @retval #HI_WIFI_INVALID_CHANNEL
 * @retval #Other                   chanel value.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_get_channel(const char *ifname, unsigned char ifname_len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set monitor mode.CNcomment:设置混杂模式.CNend
 *
 * @par Description:
 * Enable/disable monitor mode of interface.CNcomment:设置指定接口的混杂模式使能.CNend
 *
 * @attention  NULL
 * @param  ifname           [IN]     Type  #const char * interface name.CNcomment:接口名.CNend
 * @param  enable           [IN]     Type  #int enable(1) or disable(0).CNcomment:开启/关闭.CNend
 * @param  filter           [IN]     Type  #hi_wifi_ptype_filter * filtered frame type enum.CNcomment:过滤列表.CNend
 *
 * @retval #HI_ERR_SUCCESS  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_promis_enable(const char *ifname, int enable, const hi_wifi_ptype_filter *filter);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Register receive callback in monitor mode.CNcomment:注册混杂模式的收包回调函数.CNend
 *
 * @par Description:
 * 1.Register receive callback in monitor mode.CNcomment:1.注册混杂模式的收包回调函数.CNend\n
 * 2.Wifi driver will put the receive frames to this callback.
 * CNcomment:2.驱动将混杂模式的收到的报文递交到注册的回调函数处理.CNend
 *
 * @attention  NULL
 * @param  data_cb          [IN]     Type  #hi_wifi_promis_cb callback function pointer.CNcomment:混杂模式回调函数.CNend
 *
 * @retval #HI_ERR_SUCCESS  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_promis_set_rx_callback(hi_wifi_promis_cb data_cb);

/* *
 * @ingroup  hi_wifi_basic
 * @brief    Open/close system power save.CNcomment:开启/关闭WiFi低功耗模式.CNend
 *
 * @par Description:
 * Open/close system power save.CNcomment:开启/关闭WiFi低功耗模式.CNend
 *
 * @attention  NULL
 * @param  enable     [IN] Type  #unsigned char, enable(1) or disable(0).CNcomment:开启/关闭WiFi低功耗.CNend
 *
 * @retval #HI_ERR_SUCCESS  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_set_pm_switch(unsigned char enable);

/* *
* @ingroup  hi_wifi_basic
* @brief    Set wow wakeup pattern .CNcomment:设置wow wakeup 模式.CNend
*
* @attention: set specific pattern of TCP/UPD for wow wakeup host
  CNcomment: 设置wow 唤醒 host侧 的TCP/UDP包类型, 最大支持设置4种唤醒包. CNend
* @param  ifname          [IN]     Type  #const char *, device name.
* @param  type            [IN]     Type  #hi_wifi_wow_pattern_type, operation_type.
* @param  index           [IN]     Type  #unsigned char, patter_index. invalid value: 0, 1, 2, 3
* @param  pattern         [IN]     Type  #char *, hex payload of TCP/UDP.

* @retval #HISI_OK         Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since Hi3881_V100R001C00
*/
unsigned char hi_wifi_wow_set_pattern(const char *ifname, hi_wifi_wow_pattern_type type, unsigned char index,
    char *pattern);

/* *
* @ingroup  hi_wifi_basic
* @brief    Set wow_sleep .CNcomment:设置wow_sleep 睡眠.CNend
*
* @attention: the only valid parameter is 1 now, it means that host request for sleep. other value of parameter
'en' is not support
  该API当前唯一有效参数为1， 用来设置host侧请求睡眠，其他参数值暂不支持
* @param  ifname          [IN]     Type  #const char *, device name.
* @param  en              [IN]     Type  #unsigned char, wow_sleep switch, 1-sleep, 0-wakeup.

* @retval #HISI_OK         Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since Hi3881_V100R001C00
*/
unsigned char hi_wifi_wow_host_sleep_switch(const char *ifname, unsigned char en);

/* *
 * @ingroup  hi_wifi_basic
 * @brief    Set dhcp offload on/off.CNcomment:设置dhcp offload 打开/关闭.CNend
 *
 * @par Description:
 * Set dhcp offload on with ip address, or set dhcp offload off.
 * CNcomment:设置dhcp offload打开、并且设置相应ip地址，或者设置dhcp offload关闭.CNend
 *
 * @attention  NULL
 * @param  ifname          [IN]     Type  #const char *, device name.
 * @param  en              [IN]     Type  #unsigned char, dhcp offload type, 1-on, 0-off.
 * @param  ip              [IN]     Type  #unsigned int, ip address in network byte order, eg:192.168.50.4 ->
 * 0x0432A8C0.
 *
 * @retval #HISI_OK         Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned char hi_wifi_dhcp_offload_setting(const char *ifname, unsigned char en, unsigned int ip);

/* *
 * @ingroup  hi_wifi_basic
 * @brief    Set arp offload on/off.CNcomment:设置arp offload 打开/关闭.CNend
 *
 * @par Description:
 * Set arp offload on with ip address, or set arp offload off.
 * CNcomment:设置arp offload打开、并且设置相应ip地址，或者设置arp offload关闭.CNend
 *
 * @attention  NULL
 * @param  ifname          [IN]     Type  #const char *, device name.
 * @param  en              [IN]     Type  #unsigned char, arp offload type, 1-on, 0-off.
 * @param  ip              [IN]     Type  #unsigned int, ip address in network byte order, eg:192.168.50.4 ->
 * 0x0432A8C0.
 *
 * @retval #HISI_OK         Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned char hi_wifi_arp_offload_setting(const char *ifname, unsigned char en, unsigned int ip);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get rssi value.CNcomment:获取rssi值.CNend
 *
 * @par Description:
 * Get current rssi of ap which sta connected to.CNcomment:获取sta当前关联的ap的rssi值.CNend
 *
 * @attention  NULL
 * @param  NULL
 *
 * @retval #0x7F          Invalid value.
 * @retval #Other         rssi
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_sta_get_ap_rssi(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set retry params.CNcomment:设置软件重传策略.CNend
 *
 * @par Description:
 * Set retry params.CNcomment:设置指定接口的软件重传策略.CNend
 *
 * @attention  1.Need call befora start sta or softap.CNcomment:1.本API需要在STA或AP start之后调用.CNend
 * @param  ifname    [IN]     Type  #const char * interface name.CNcomment:接口名.CNend
 * @param  type      [IN]     Type  #unsigned char retry type.
 * CNcomment:0:次数重传（数据帧）; 1:次数重传（管理帧）; 2:时间重传.CNend
 * @param  limit     [IN]     Type  #unsigned char limit value.
 * CNcomment:重传次数(0~15次)/重传时间(0~200个时间粒度,时间粒度10ms).CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned int hi_wifi_set_retry_params(const char *ifname, unsigned char type, unsigned char limit);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set sta plat pm mode.CNcomment:设置STA的平台低功耗模式.CNend
 *
 * @par Description:
 * Set sta pm mode.CNcomment:设置STA的FAST_PS、PSPOLL_PS、uapsd低功耗模式.CNend
 *
 * @attention  NULL
 * @param  sleep_mode      [IN]     Type  #unsigned char, 0-no_sleep, 1-light_sleep, 2-deep_sleep.
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_set_plat_ps_mode(unsigned char sleep_mode);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set sta plat pm disable.CNcomment:去使能device的平台低功耗.CNend
 *
 * @par Description:
 * Set sta plat pm disable.CNcomment:去使能device的平台低功耗.CNend
 *
 * @attention  NULL
 * @param NULL
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned int hi_wifi_plat_pm_disable(void);


/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set sta plat pm enable.CNcomment:使能device的平台低功耗.CNend
 *
 * @par Description:
 * Set sta plat pm enable.CNcomment:使能device的平台低功耗.CNend
 *
 * @attention  NULL
 * @param NULL
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned int hi_wifi_plat_pm_enable(void);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  host notifies device of host's sleep state.CNcomment:host将睡眠状态告知device.CNend
 *
 * @par Description:
 * host notifies device of host's sleep state.CNcomment:host将睡眠状态告知device.CNend
 *
 * @attention  Once the device receives the host's sleep message, it will no longer send data to
 * the host unless the host wakes up the device or the device wakes up the Host.CNcomment:一旦device
 * 收到host的睡眠消息，将不再向host发送数据，除非host唤醒device或者device唤醒Host.CNend
 * @param  slp          [IN]     Type  #bool, host sleep status, 0-wake, 1-sleep.
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned int hi_wifi_host_request_sleep(bool slp);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set cca threshold.CNcomment:设置CCA门限.CNend
 *
 * @par Description:
 * Set cca threshold.CNcomment:设置CCA门限.CNend
 *
 * @attention  CNcomment:1.threshold设置范围是-128~126时，阈值固定为设置值.CNend\n
 * CNcomment:2.threshold设置值为127时，恢复默认阈值-62dBm，并使能动态调整.CNend
 * @param  ifname          [IN]     Type #char *, device name. CNcomment:接口名.CNend
 * @param  threshold       [IN]     Type #char, threshold. CNcomment:门限值.CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 *
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned int hi_wifi_set_cca_threshold(const char *ifname, signed char threshold);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Set tcp mode.CNcomment:设置tpc开关.CNend
 *
 * @par Description:
 * Set tpc mode.CNcomment:设置tpc开关.CNend
 *
 * @attention  1.Mode set to 1, enable auto power control. set to 0, disable it.
 * CNcomment:1.mode范围是0~1,1:打开发送功率自动控制,0:关闭发送功率自动控制.CNend
 * @param  ifname          [IN]     Type #char *, device name. CNcomment:接口名.CNend
 * @param  ifname_len      [IN]     Type #unsigned char, interface name length.CNcomment:接口名长度.CNend
 * @param  tpc_value       [IN]     Type #unsigned int, tpc_value. CNcomment:tpc开关.CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 *
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned int hi_wifi_set_tpc(const char *ifname, unsigned char ifname_len, unsigned int tpc_value);

/* *
* @ingroup  hi_wifi_basic
* @brief  Set customize parameters.CNcomment:设置定制化参数.CNend
*
* @par Description:
*           Set customize parameters:设置定制化参数.CNend
*
* @attention  1.rssi_offset:rx rssi compnesation val. CNcomment:1.rssi_offset:rx rssi的补偿偏移值.CNend
* @attention  2.freq_comp:3 elements refer to enter and quit temp freq compensation threshold, and compensation val.
              CNcomment:2.freq_comp:3个元素分别对应进入频偏补偿的温度阈值，退出补偿的温度阈值和高温频偏补偿值.CNend
* @attention  3.dbb_params:first 5 elements are dbb scales, 6th is freq and band power offset, 7th is evm related val.
              CNcomment:3.dbb_params:前5个是dbb scale配置值，第6个是频偏和band功率补偿值，第7个是evm的配置值.CNend
* @attention  4.ch_txpwr_offset:tx power offset of 13 channels, for FCC.ch 14 or upper use the cfg val of ch 13.
              CNcomment:4.ch_txpwr_offset:对应13个信道的功率偏移，配置符合FCC要求的功率,大于13信道使用13信道的值.CNend
* @param  params          [IN]     Type #hi_wifi_customize_params *, parameters. CNcomment:定制化参数.CNend
*
* @retval #HISI_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since Hi3881_V100R001C00
*/
unsigned int hi_wifi_set_customize_params(hi_wifi_customize_params *params);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Register function callback of report datarate and average send times.
 *         CNcomment:注册上报实际吞吐量、平均发送次数函数回调.CNend
 *
 * @par Description:
 *         Register function callback of report datarate and average send times
 *         CNcomment:注册上报实际吞吐量、平均发送次数函数回调.CNend
 *
 * @attention  1.function callback type must been suited to hi_wifi_report_tx_params_callback.
               CNcomment:1.回调函数必须是 hi_wifi_report_tx_params_callback 类型.CNend
 *
 * @param  func      [IN] type #hi_wifi_report_tx_params_callback, report tx params callback. CNcomment:上报实际吞吐量、
 平均发送次数函数回调。Nend
 * @retval None
 * @par Dependency:
 *            @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
void hi_wifi_register_tx_params_callback(hi_wifi_report_tx_params_callback func);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  Get datarate.CNcomment:获取实际速率及平均发送次数.CNend
 *
 * @par Description:
 * Get goodput and average send times.CNcomment:获取实际速率及平均发送次数.CNend
 *
 * @attention  1. Call hi_wifi_register_datarate_callback before call this function.
 * CNcomment:1. 调用本函数前，调用 hi_wifi_register_datarate_callback 注册回调.CNend
 * 2. after call this function, result will be reported by register function.
 * CNcomment:2. 函数执行结果，通过注册的回调函数通知.CNend
 * @param  ifname          [IN]     Type #char *, device name. CNcomment:接口名.CNend
 * @param  ifname_len      [IN]     Type #unsigned char, interface name length.CNcomment:接口名长度.CNend
 *
 * @retval #HISI_OK  Excute successfully
 * @retval #Other           Error code
 *
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned int hi_wifi_get_tx_params(const char *ifname, unsigned char ifname_len);

/* *
 * @ingroup  hi_wifi_basic
 * @brief  register driver callback interface.CNcomment:注册驱动事件回调函数接口.CNend
 *
 * @par Description:
 * register driver callback interface.CNcomment:注册驱动事件回调函数接口.CNend
 *
 * @attention  NULL
 * @param  event_cb  [OUT]    Type #hi_wifi_driver_event_cb, event callback .CNcomment:回调函数.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_register_driver_event_callback(hi_wifi_driver_event_cb event_cb);

/* *
 * @ingroup
 * @brief  sdio send message to reset device. CNcomment:SDIO发送复位设备消息.CNend
 *
 * @par Description:
 * sdio send message to reset device.CNcomment:SDIO发送复位设备消息.CNend
 *
 * @attention  NULL
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #HISI_FAIL      Execute failed.
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
int hi_wifi_soft_reset_device(void);

/* *
* @ingroup
* @brief  enable device's gpio soft reset function. CNcomment:使能设备GPIO软复位功能.CNend
*
* @par Description:
*     when function enabled, device will reset after receive GPIO5's  Falling edge interrupt. default not enabled.
CNcomment:使能该功能后，device响应GPIO5的下降沿中断进行软复位，默认该功能未使能.CNend
*
* @attention  should call after wifi init.CNcomment:须在WIFI初始化成功后调用.CNend
*
* @retval #HISI_OK        Execute successfully.
* @retval #Other           Error code
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since Hi3881_V100R001C00
*/
unsigned int hi_wifi_open_gpio_soft_reset_device(void);

/* *
 * @ingroup
 * @brief  disable device's gpio soft reset function. CNcomment:禁用设备GPIO软复位功能.CNend
 *
 * @par Description:
 * disable device's gpio soft reset function. CNcomment:禁用设备GPIO软复位功能.CNend
 *
 * @attention  should call after wifi init.CNcomment:须在WIFI初始化成功后调用.CNend
 *
 * @retval #HISI_OK        Execute successfully.
 * @retval #Other           Error code
 * @par Dependency:
 * @li hi_wifi_api.h: WiFi API
 * @see  NULL
 * @since Hi3881_V100R001C00
 */
unsigned int hi_wifi_close_gpio_soft_reset_device(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hi_wifi_api.h */
