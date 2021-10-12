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

/* *
 * @defgroup hi_wifi WiFi API
 */
/* *
 * @defgroup hi_wifi_driver_wpa Basic Settings
 * @ingroup hi_wifi
 */

#ifndef __DRIVER_HISI_COMMON_H__
#define __DRIVER_HISI_COMMON_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hi_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#ifndef ETH_ADDR_LEN
#define ETH_ADDR_LEN        6
#endif

#define HISI_SUCC            0
#define HISI_EFAIL           1
#define HISI_EINVAL         22

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN        32
#endif

#ifndef HISI_MAX_NR_CIPHER_SUITES
#define HISI_MAX_NR_CIPHER_SUITES    5
#endif

#ifndef HISI_WPAS_MAX_SCAN_SSIDS
#define HISI_WPAS_MAX_SCAN_SSIDS     2
#endif

#ifndef HISI_PTR_NULL
#define HISI_PTR_NULL  NULL
#endif

#ifndef HISI_MAX_NR_AKM_SUITES
#define HISI_MAX_NR_AKM_SUITES       2
#endif


#ifndef NOTIFY_DONE
#define NOTIFY_DONE            0x0000
#endif
#ifndef NETDEV_UP
#define NETDEV_UP              0x0001
#endif
#ifndef SCAN_AP_LIMIT
#define SCAN_AP_LIMIT            64
#endif
#ifndef NETDEV_DOWN
#define NETDEV_DOWN            0x0002
#endif

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
#define HISI_REKEY_OFFLOAD_KCK_LEN              16
#define HISI_REKEY_OFFLOAD_KEK_LEN              16
#define HISI_REKEY_OFFLOAD_REPLAY_COUNTER_LEN   8
#endif

#define HI_WIFI_HIPRIV_ARGC_MAX 10

/* ****************************************************************************
  4 枚举定义
**************************************************************************** */
typedef enum {
    HISI_FALSE   = 0,
    HISI_TRUE    = 1,

    HISI_BUTT
} hisi_bool_enum;

typedef hi_u8 hisi_bool_enum_uint8;

typedef enum {
    HISI_IFTYPE_UNSPECIFIED,
    HISI_IFTYPE_ADHOC,
    HISI_IFTYPE_STATION,
    HISI_IFTYPE_AP,
    HISI_IFTYPE_AP_VLAN,
    HISI_IFTYPE_WDS,
    HISI_IFTYPE_MONITOR,
    HISI_IFTYPE_MESH_POINT,
    HISI_IFTYPE_P2P_CLIENT,
    HISI_IFTYPE_P2P_GO,
    HISI_IFTYPE_P2P_DEVICE,

    /* keep last */
    NUM_HISI_IFTYPES,
    HISI_IFTYPE_MAX = NUM_HISI_IFTYPES - 1
} hisi_iftype_enum;
typedef hi_u8 hisi_iftype_enum_uint8;

typedef enum {
    HISI_KEYTYPE_GROUP,
    HISI_KEYTYPE_PAIRWISE,
    HISI_KEYTYPE_PEERKEY,
    NUM_HISI_KEYTYPES
} hisi_key_type_enum;
typedef hi_u8 hisi_key_type_enum_uint8;

typedef enum {
    HISI_KEY_DEFAULT_TYPE_INVALID,
    HISI_KEY_DEFAULT_TYPE_UNICAST,
    HISI_KEY_DEFAULT_TYPE_MULTICAST,
    NUM_HISI_KEY_DEFAULT_TYPES
} hisi_key_default_types_enum;
typedef hi_u8 hisi_key_default_types_enum_uint8;

typedef enum {
    HISI_NO_SSID_HIDING,
    HISI_HIDDEN_SSID_ZERO_LEN,
    HISI_HIDDEN_SSID_ZERO_CONTENTS
} hisi_hidden_ssid_enum;
typedef hi_u8 hisi_hidden_ssid_enum_uint8;

typedef enum {
    HISI_IOCTL_SET_AP = 0,
    HISI_IOCTL_NEW_KEY,
    HISI_IOCTL_DEL_KEY,
    HISI_IOCTL_SET_KEY,
    HISI_IOCTL_SEND_MLME,
    HISI_IOCTL_SEND_EAPOL,
    HISI_IOCTL_RECEIVE_EAPOL,
    HISI_IOCTL_ENALBE_EAPOL,
    HISI_IOCTL_DISABLE_EAPOL,
    HIIS_IOCTL_GET_ADDR,
    HISI_IOCTL_SET_MODE,
    HIIS_IOCTL_GET_HW_FEATURE,
    HISI_IOCTL_SCAN,
    HISI_IOCTL_DISCONNET,
    HISI_IOCTL_ASSOC,
    HISI_IOCTL_SET_NETDEV,
    HISI_IOCTL_CHANGE_BEACON,
    HISI_IOCTL_SET_REKEY_INFO,
    HISI_IOCTL_STA_REMOVE,
    HISI_IOCTL_SEND_ACTION,
    HISI_IOCTL_SET_MESH_USER,
    HISI_IOCTL_SET_MESH_GTK,
    HISI_IOCTL_EN_ACCEPT_PEER,
    HISI_IOCTL_EN_ACCEPT_STA,
    HISI_IOCTL_ADD_IF,
    HISI_IOCTL_PROBE_REQUEST_REPORT,
    HISI_IOCTL_REMAIN_ON_CHANNEL,
    HISI_IOCTL_CANCEL_REMAIN_ON_CHANNEL,
    HISI_IOCTL_SET_P2P_NOA,
    HISI_IOCTL_SET_P2P_POWERSAVE,
    HISI_IOCTL_SET_AP_WPS_P2P_IE,
    HISI_IOCTL_REMOVE_IF,
    HISI_IOCTL_GET_P2P_MAC_ADDR,
    HISI_IOCTL_GET_DRIVER_FLAGS,
    HISI_IOCTL_SET_USR_APP_IE,
    HISI_IOCTL_DELAY_REPORT,
    HWAL_EVENT_BUTT
} hisi_event_enum;
typedef hi_u8 hisi_event_enum_uint8;

typedef enum {
    HISI_ELOOP_EVENT_NEW_STA = 0,
    HISI_ELOOP_EVENT_DEL_STA,
    HISI_ELOOP_EVENT_RX_MGMT,
    HISI_ELOOP_EVENT_TX_STATUS,
    HISI_ELOOP_EVENT_SCAN_DONE,
    HISI_ELOOP_EVENT_SCAN_RESULT,
    HISI_ELOOP_EVENT_CONNECT_RESULT,
    HISI_ELOOP_EVENT_DISCONNECT,
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    HISI_ELOOP_EVENT_RX_DFX,
#endif
    HISI_ELOOP_EVENT_MESH_CLOSE,
    HISI_ELOOP_EVENT_NEW_PEER_CANDIDATE,
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    HISI_ELOOP_EVENT_MICHAEL_MIC_FAILURE,
#endif
    HISI_ELOOP_EVENT_REMAIN_ON_CHANNEL,
    HISI_ELOOP_EVENT_CANCEL_REMAIN_ON_CHANNEL,
    HISI_ELOOP_EVENT_CHANNEL_SWITCH,
    HISI_ELOOP_EVENT_EAPOL_RECV,
    HISI_ELOOP_EVENT_TIMEOUT_DISCONN,
    HISI_ELOOP_EVENT_BUTT
} hisi_eloop_event_enum;
typedef hi_u8 hisi_eloop_event_enum_uint8;

typedef enum {
    HISI_MFP_NO,
    HISI_MFP_OPTIONAL,
    HISI_MFP_REQUIRED,
} hisi_mfp_enum;
typedef hi_u8 hisi_mfp_enum_uint8;

typedef enum {
    HISI_AUTHTYPE_OPEN_SYSTEM = 0,
    HISI_AUTHTYPE_SHARED_KEY,
    HISI_AUTHTYPE_FT,
    HISI_AUTHTYPE_NETWORK_EAP,
    HISI_AUTHTYPE_SAE,

    /* keep last */
    __HISI_AUTHTYPE_NUM,
    HISI_AUTHTYPE_MAX = __HISI_AUTHTYPE_NUM - 1,
    HISI_AUTHTYPE_AUTOMATIC,

    HISI_AUTHTYPE_BUTT
} hisi_auth_type_enum;
typedef hi_u8 hisi_auth_type_enum_uint8;

typedef enum {
    HISI_SCAN_SUCCESS = 0,
    HISI_SCAN_FAILED = 1,
    HISI_SCAN_REFUSED = 2,
    HISI_SCAN_TIMEOUT = 3,
} hisi_scan_status_enum;
typedef hi_u8 hisi_scan_status_enum_uint8;

typedef enum {
    HISI_FRAME_TYPE_BEACON    = bit(0),
    HISI_FRAME_TYPE_PROBE_REQ = bit(1),
    HISI_FRAME_TYPE_BUTT
} hisi_frame_app_ie_enum;

/* ****************************************************************************
  5 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  6 消息头定义
**************************************************************************** */
/* ****************************************************************************
  7 消息定义
**************************************************************************** */
/* ****************************************************************************
  8 STRUCT定义
**************************************************************************** */
typedef struct _hisi_driver_scan_status_stru {
    hisi_scan_status_enum scan_status;
} hisi_driver_scan_status_stru;

typedef struct _hisi_ioctl_command_stru {
    hi_u32      cmd;
    hi_void    *buf;
} hisi_ioctl_command_stru;

typedef hi_s32 (*hisi_send_event_cb)(const hi_char *, hi_s32, hi_u8 *, hi_u32);

#ifdef _PRE_WLAN_FEATURE_P2P
typedef struct _hisi_remain_on_channel_stru {
    hi_s32 freq;
    hi_s32 duration;
} hisi_on_channel_stru;

typedef struct _hisi_if_add_stru {
    hi_s8 type;
} hisi_if_add_stru;
#define WIFI_IFNAME_MAX_SIZE 16
typedef struct _hisi_if_remove_stru {
    hi_u8 ifname[WIFI_IFNAME_MAX_SIZE];
} hisi_if_remove_stru;

typedef struct _hisi_get_p2p_addr_stru {
    hi_s8 type;
    hi_u8 mac_addr[ETH_ADDR_LEN];
    hi_u8 resv;
} hisi_get_p2p_addr_stru;

typedef struct _hisi_p2p_noa_stru {
    hi_s32 start;
    hi_s32 duration;
    hi_u8 count;
    hi_u8 resv[3]; /* 3 byte保留字段 */
} hisi_p2p_noa_stru;

typedef struct _hisi_p2p_power_save_stru {
    hi_s32 legacy_ps;
    hi_s8 opp_ps;
    hi_u8 ctwindow;
    hi_u8 resv[2]; /* 2 BYTE 保留字段 */
} hisi_p2p_power_save_stru;
#endif

typedef struct _hisi_get_drv_flags_stru {
    hi_u64 drv_flags;
} hisi_get_drv_flags_stru;

typedef struct _hisi_new_sta_info_stru {
    hi_s32 reassoc;
    hi_u32 ielen;
    hi_u8 *ie;
    hi_u8 macaddr[ETH_ADDR_LEN];
    hi_u8 resv[2]; /* 2 BYTE保留字段 */
} hisi_new_sta_info_stru;

typedef struct _hisi_rx_mgmt_stru {
    hi_u8 *buf;
    hi_u32 len;
    hi_s32 sig_mbm;
    hi_u32 freq;
} hisi_rx_mgmt_stru;

typedef struct _hisi_tx_status_stru {
    hi_u8               *buf;
    hi_u32               len;
    hisi_bool_enum_uint8 ack;
    hi_u8 resv[3]; /* 3 BYTE保留字段 */
} hisi_tx_status_stru;

typedef struct _hisi_mlme_data_stru {
    hi_u32 freq;
    hi_u32 data_len;
    hi_u8 *data;
    hi_u64 *send_action_cookie;
} hisi_mlme_data_stru;

typedef struct _hisi_beacon_data_stru {
    hi_u32 head_len;
    hi_u32 tail_len;
    hi_u8 *head;
    hi_u8 *tail;
} hisi_beacon_data_stru;

typedef struct _hisi_action_data_stru {
    hi_u8 *dst;
    hi_u8 *src;
    hi_u8 *bssid;
    hi_u8 *data;
    hi_u32 data_len;
} hisi_action_data_stru;

#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct _hisi_set_mesh_user_data_stru {
    hi_u8 *puc_addr;
    hi_u8 plink_state;
    hi_u8 set;
    hi_u8 bcn_prio; /* 上报lwip使用 */
    hi_u8 is_mbr;
    hi_u8 mesh_initiative_peering;
} hisi_set_mesh_user_data_stru;

typedef struct _hisi_enable_auto_peer_stru {
    hi_u8 enable_auto_peer;
} hisi_enable_auto_peer_stru;

typedef struct _hisi_set_mesh_user_gtk_stru {
    hi_u8 *puc_addr;
    hi_u8 *puc_gtk;
    hi_u8 gtk_len;
    hi_u8 resv[3]; /* 3 BYTE保留字段 */
} hisi_set_mesh_user_gtk_stru;

typedef struct _hisi_enable_accept_peer_stru {
    hi_u8 enable_accept_peer;
} hisi_enable_accept_peer_stru;

typedef struct _hisi_enable_accept_sta_stru {
    hi_u8 enable_accept_sta;
} hisi_enable_accept_sta_stru;
#endif

/* 延时上报关联状态配置参数结构体 */
typedef struct _hisi_delay_report_stru {
    hi_u8  enable;   /* 配置开关 */
    hi_u16 timeout; /* 延时时间 */
    hi_u8  resv;
} hisi_delay_report_stru;

typedef struct _hisi_freq_params_stru {
    hi_s32 l_mode;
    hi_s32 l_freq;
    hi_s32 l_channel;

    /* for HT */
    hi_s32 l_ht_enabled;

    /* 0 = HT40 disabled, -1 = HT40 enabled,
     * secondary channel below primary, 1 = HT40
     * enabled, secondary channel above primary */
    hi_s32 l_sec_channel_offset;

    /* for VHT */
    hi_s32 l_vht_enabled;

    /* valid for both HT and VHT, center_freq2 is non-zero
     * only for bandwidth 80 and an 80+80 channel */
    hi_s32 l_center_freq1;
    hi_s32 l_center_freq2;
    hi_s32 l_bandwidth;
} hisi_freq_params_stru;

/* 密钥信息设置数据传递结构体 */
typedef struct _hisi_key_ext_stru {
    hi_s32                          type;
    hi_u32                          key_idx;
    hi_u32                          key_len;
    hi_u32                          seq_len;
    hi_u32                          cipher;
    hi_u8                          *addr;
    hi_u8                          *key;
    hi_u8                          *seq;
    hisi_bool_enum_uint8                def;
    hisi_bool_enum_uint8                defmgmt;
    hisi_key_default_types_enum_uint8   default_types;
    hi_u8                               resv;
} hisi_key_ext_stru;

/* AP信息设置相关数据传递结构体 */
typedef struct _hisi_ap_settings_stru {
    hisi_freq_params_stru        freq_params;
    hisi_beacon_data_stru        beacon_data;
    hi_u32                       ssid_len;
    hi_s32                       l_beacon_interval;
    hi_s32                       l_dtim_period;
    hi_u8                       *puc_ssid;
    hisi_hidden_ssid_enum_uint8  hidden_ssid;
    hisi_auth_type_enum_uint8    auth_type;
    hi_u8                        resv[2]; /* 2 BYTE 保留字段 */
} hisi_ap_settings_stru;

/* 设置模式结构体 */
typedef struct _hisi_set_mode_stru {
    hi_u8                   bssid[ETH_ADDR_LEN];
    hisi_iftype_enum_uint8  iftype;
    hi_u8                   resv;
} hisi_set_mode_stru;

typedef struct _hisi_tx_eapol_stru {
    hi_u8 *buf;
    hi_u32 len;
} hisi_tx_eapol_stru;

typedef struct _hisi_rx_eapol_stru {
    hi_u8 *buf;
    hi_u32 len;
} hisi_rx_eapol_stru;

typedef struct _hisi_enable_eapol_stru {
    hi_void *callback;
    hi_void *contex;
} hisi_enable_eapol_stru;

typedef struct _hisi_ieee80211_channel_stru {
    hi_u16 channel;
    hi_u8  resv[2]; /* 2 BYTE保留字段 */
    hi_u32 freq;
    hi_u32 flags;
} hisi_ieee80211_channel_stru;

typedef struct _hisi_hw_feature_data_stru {
    hi_s32 channel_num;
    hi_u16 bitrate[12]; /* 占12 short */
    hi_u16 ht_capab;
    hi_u8  resv[2];                                    /* 2 BYTE保留字段 */
    hisi_ieee80211_channel_stru iee80211_channel[14]; /* 14维 */
} hisi_hw_feature_data_stru;

typedef struct _hisi_driver_scan_ssid_stru {
    hi_u8  auc_ssid[MAX_SSID_LEN];
    hi_u32 ssid_len;
} hisi_driver_scan_ssid_stru;

typedef struct _hisi_scan_stru {
    hisi_driver_scan_ssid_stru *ssids;
    hi_s32                     *freqs;
    hi_u8                      *extra_ies;
    hi_u8                      *bssid;
    hi_u8                       num_ssids;
    hi_u8                       num_freqs;
    hi_u8                       prefix_ssid_scan_flag;
    hi_u8                       fast_connect_flag;
    hi_u32                      extra_ies_len;
} hisi_scan_stru;

typedef struct _hisi_crypto_settings_stru {
    hi_u32 wpa_versions;
    hi_u32 cipher_group;
    hi_s32 l_n_ciphers_pairwise;
    hi_u32 aul_ciphers_pairwise[HISI_MAX_NR_CIPHER_SUITES];
    hi_s32 l_n_akm_suites;
    hi_u32 aul_akm_suites[HISI_MAX_NR_AKM_SUITES];
} hisi_crypto_settings_stru;

typedef struct _hisi_associate_params_stru {
    hi_u8 *bssid;
    hi_u8 *ssid;
    hi_u8 *ie;
    hi_u8 *key;
    hi_u8 auth_type;
    hi_u8 privacy;
    hi_u8 key_len;
    hi_u8 key_idx;
    hi_u8 mfp;
    hi_u8 auto_conn; /* 自动重关联标志 */
    hi_u8 resv[2];   /* 2 BYTE保留字段 */
    hi_u32 freq;
    hi_u32 ssid_len;
    hi_u32 ie_len;
    hisi_crypto_settings_stru *crypto;
} hisi_associate_params_stru;


typedef struct _hisi_connect_result_stru {
    hi_u8              *req_ie;
    hi_u32              req_ie_len;
    hi_u8              *resp_ie;
    hi_u32              resp_ie_len;
    hi_u8               bssid[ETH_ADDR_LEN];
    hi_u8               resv[2]; /* 2 BYTE保留字段 */
    hi_u16              status;
    hi_u16              freq;
} hisi_connect_result_stru;

typedef struct _hisi_scan_result_stru {
    hi_s32 flags;
    hi_u8  bssid[ETH_ADDR_LEN];
    hi_s16 caps;
    hi_s32 freq;
    hi_s16 beacon_int;
    hi_s32 qual;
    hi_u32 beacon_ie_len;
    hi_s32 level;
    hi_u32 age;
    hi_u32 ie_len;
    hi_u8  *variable;
} hisi_scan_result_stru;

typedef struct _hisi_disconnect_stru {
    hi_u8  *ie;
    hi_u16 reason;
    hi_u8  rsv[2]; /* 2 BYTE保留字段 */
    hi_u32 ie_len;
} hisi_disconnect_stru;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct _hisi_michael_mic_failure_stru {
    hi_u8 user_mac_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 reserve[2]; /* 2 BYTE保留字段 */
    oal_nl80211_key_type key_type;
    hi_s32 l_key_id;
} hisi_michael_mic_failure_stru;
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
typedef struct _hisi_app_ie_stru {
    hi_u32 ie_len;
    hi_u8  app_ie_type;
    hi_u8  auc_rsv[3]; /* 3 BYTE保留字段 */
    /* auc_ie 中保存信息元素 */
    hi_u8 *puc_ie;
} hisi_app_ie_stru;
#endif
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
typedef struct _hisi_rekey_offload_stru {
    hi_u8 auc_kck[HISI_REKEY_OFFLOAD_KCK_LEN];
    hi_u8 auc_kek[HISI_REKEY_OFFLOAD_KEK_LEN];
    hi_u8 auc_replay_ctr[HISI_REKEY_OFFLOAD_REPLAY_COUNTER_LEN];
} hisi_rekey_offload_stru;
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct _hisi_mesh_close_peer_stru {
    hi_u8 auc_macaddr[ETH_ADDR_LEN];
    hi_u16 us_reason;
} hisi_mesh_close_peer_stru;

typedef struct _hisi_mesh_new_peer_candidate_stru {
    hi_u8 peer_addr[ETH_ADDR_LEN];
    hi_u8 bcn_prio;
    hi_u8 is_mbr;
    hi_s8 rssi;
    hi_u8 reserved[3]; /* 3 BYTE保留字段 */
} hisi_mesh_new_peer_candidate_stru;

typedef struct {
    hi_u8 set; /* 0: del, 1: add */
    hi_u8 ie_type;
    hi_u16 ie_len;
    hisi_frame_app_ie_enum frame_type;
    hi_u8 *ie;
} hisi_usr_app_ie_stru;
#endif

typedef struct _hisi_ch_switch_stru {
    hi_s32 freq;
} hisi_ch_switch_stru;

/* ****************************************************************************
  9 UNION定义
**************************************************************************** */
/* ****************************************************************************
  10 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  11 函数声明
**************************************************************************** */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of driver_hisi_common.h */
