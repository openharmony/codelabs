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

#ifndef __HMAC_EXT_IF_H__
#define __HMAC_EXT_IF_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "mac_device.h"
#include "mac_vap.h"
#include "mac_user.h"
#include "mac_frame.h"
#include "mac_data.h"
#include "hmac_config.h"
#include "hmac_device.h"
#include "hmac_vap.h"
#include "hmac_p2p.h"
#ifdef _PRE_WLAN_FEATURE_WOW
#include "hmac_wow.h"
#endif
#ifdef _PRE_WLAN_FEATURE_WAPI
#include "hmac_wapi.h"
#endif
#include "hmac_11i.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* 返回值类型定义 */
typedef enum {
    HMAC_TX_PASS = 0, /* 继续往下 */
    HMAC_TX_BUFF = 1, /* 已被缓存 */
    HMAC_TX_DONE = 2, /* 组播转成单播已发送 */

    HMAC_TX_DROP_PROXY_ARP = 3,    /* PROXY ARP检查后丢弃 */
    HMAC_TX_DROP_USER_UNKNOWN,     /* 未知user */
    HMAC_TX_DROP_USER_NULL,        /* user结构体为NULL */
    HMAC_TX_DROP_USER_INACTIVE,    /* 目的user未关联 */
    HMAC_TX_DROP_SECURITY_FILTER,  /* 安全检查过滤掉 */
    HMAC_TX_DROP_BA_SETUP_FAIL,    /* BA会话创建失败 */
    HMAC_TX_DROP_AMSDU_ENCAP_FAIL, /* amsdu封装失败 */
    HMAC_TX_DROP_MUSER_NULL,       /* 组播user为NULL */
    HMAC_TX_DROP_MTOU_FAIL,        /* 组播转单播失败 */
    HMAC_TX_DROP_80211_ENCAP_FAIL, /* 802.11 head封装失败 */
    HMAC_TX_DROP_POLICY,           /* 策略丢包处理 */

    HMAC_TX_BUTT
} hmac_tx_return_type_enum;
typedef hi_u8 hmac_tx_return_type_enum_uint8;

/* ****************************************************************************
  枚举名  : hmac_host_ctx_event_sub_type_enum_uint8
  协议表格:
  枚举说明: HOST CTX事件子类型定义
**************************************************************************** */
typedef enum {
    HMAC_HOST_CTX_EVENT_SUB_TYPE_SCAN_COMP_STA = 0, /* STA　扫描完成子类型 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_ASOC_COMP_STA,     /* STA 关联完成子类型 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_DISASOC_COMP_STA,  /* STA 上报去关联完成 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_STA_CONNECT_AP,    /* AP 上报新加入BSS的STA情况 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_STA_DISCONNECT_AP, /* AP 上报离开BSS的STA情况 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    HMAC_HOST_CTX_EVENT_SUB_TYPE_MIC_FAILURE, /* 上报MIC攻击 */
#endif
    HMAC_HOST_CTX_EVENT_SUB_TYPE_RX_MGMT, /* 上报接收到的管理帧 */
#ifdef _PRE_WLAN_FEATURE_P2P
    HMAC_HOST_CTX_EVENT_SUB_TYPE_LISTEN_EXPIRED, /* 上报监听超时 */
#endif
    HMAC_HOST_CTX_EVENT_SUB_TYPE_STA_CONN_RESULT, /* 通知lwip sta是否关联 */
#ifdef _PRE_WLAN_FEATURE_FLOWCTL
    HMAC_HOST_CTX_EVENT_SUB_TYPE_FLOWCTL_BACKP, /* 上报流控反压消息 */
#endif
    HMAC_HOST_CTX_EVENT_SUB_TYPE_MGMT_TX_STATUS,

#ifdef _PRE_WLAN_FEATURE_ANY
    HMAC_HOST_CTX_EVENT_SUB_TYPE_ANY_RX_DATA,     /* 上报接收到的ANY类型帧的数据 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_ANY_TX_STATUS,   /* 上报ANY类型帧的发送状态 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_ANY_SCAN_RESULT, /* 上报所扫描到的ANY设备信息 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_ANY_PEER_INFO,   /* 上报查询的ANY对端设备信息 */
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
    HMAC_HOST_CTX_EVENT_SUB_TYPE_PEER_CLOSE_MESH,    /* 通知WPA与远端设备断开连接 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_NEW_PEER_CANDIDATE, /* 通知wpa有可关联远端节点 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_TX_DATA_INFO,       /* 通知lwip单播数据帧相关信息 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_MESH_USER_INFO,     /* 通知lwip用户关联状态信息 */
#endif
#ifdef _PRE_WLAN_FEATURE_CSI
    HMAC_HOST_CTX_EVENT_SUB_TYPE_CSI_REPORT, /* 底层获取的CSI数据上报到WAL层事件 */
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    HMAC_HOST_CTX_EVENT_SUB_TYPE_P2P_TX_STATUS, /* P2P发送ACTION帧状态上报到WAL层事件 */
#endif
#ifdef FEATURE_DAQ
    HMAC_HOST_CTX_EVENT_SUB_TYPE_ACQ_STATUS, /* 数采状态上报到WAL层事件 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_ACQ_RESULT, /* 数采结果上报到WAL层事件 */
#endif
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    HMAC_HOST_CTX_EVENT_SUB_TYPE_CHANNEL_SWITCH, /* 信道切换上到到WAL层事件 */
#endif
    HMAC_HOST_CTX_EVENT_GET_MAC_FROM_EFUSE, /* HOST侧获取efuse中的mac地址 */
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    HMAC_HOST_CTX_EVENT_GET_DBG_CAL_DATA, /* HOST侧获取device调试产测参数 */
#endif
    HMAC_HOST_CTX_EVENT_REPORT_TX_PARAMS, /* device侧上报alg模块的理论goodput到wal层事件 */
    HMAC_HOST_CTX_EVENT_SUB_TYPE_BUTT
} hmac_host_ctx_event_sub_type_enum;
typedef hi_u8 hmac_host_ctx_event_sub_type_enum_uint8;

/* Status code for MLME operation confirm */
typedef enum {
    HMAC_MGMT_SUCCESS = 0,
    HMAC_MGMT_INVALID = 1,
    HMAC_MGMT_TIMEOUT = 2,
    HMAC_MGMT_REFUSED = 3,
    HMAC_MGMT_TOMANY_REQ = 4,
    HMAC_MGMT_ALREADY_BSS = 5
} hmac_mgmt_status_enum;
typedef hi_u8 hmac_mgmt_status_enum_uint8;

/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
/* 扫描结果 */
typedef struct {
    hi_u8 num_dscr;
    hi_u8 result_code;
    hi_u8 auc_resv[2]; /* 2:预留数组大小 */
} hmac_scan_rsp_stru;

/* 关联结果 */
typedef struct {
    hmac_mgmt_status_enum_uint8 result_code; /* 关联成功,超时等 */
    hi_u8 auc_resv1[1];
    mac_status_code_enum_uint16 status_code; /* ieee协议规定的16位状态码  */

    hi_u8 auc_addr_ap[WLAN_MAC_ADDR_LEN];
    hi_u16 us_freq;

    hi_u32 asoc_req_ie_len;
    hi_u32 asoc_rsp_ie_len;

    hi_u8 *puc_asoc_req_ie_buff;
    hi_u8 *puc_asoc_rsp_ie_buff;
} hmac_asoc_rsp_stru;

/* mic攻击 */
typedef struct {
    hi_u8 auc_user_mac[WLAN_MAC_ADDR_LEN];
    hi_u8 auc_reserve[2]; /* 2:预留数组大小 */
    oal_nl80211_key_type key_type;
    hi_s32 l_key_id;
} hmac_mic_event_stru;

/* 上报接收到管理帧事件的数据结构 */
typedef struct {
    hi_u8  *puc_buf;
    hi_u16  us_len;
    hi_u8   rsv[2];   /* 2:预留数组大小 */
    hi_s32  l_freq;
    hi_char ac_name[OAL_IF_NAME_SIZE];
} hmac_rx_mgmt_event_stru;

typedef struct {
    hi_u32 cfg_id;
    hi_u32 ac;
    hi_u32 value;
} hmac_config_wmm_para_stru;

typedef struct {
    hi_u8 is_assoc;                      /* 标识是关联事件/去关联事件 */
    hi_u8 conn_to_mesh;                  /* 标识是否关联到Mesh/普通AP */
    hi_u8 rssi;                          /* 关联的节点的扫描RSSI */
    hi_u8 auc_mac_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 auc_rsv[3];                    /* 3:预留数组大小 */
}hmac_sta_report_assoc_info_stru;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_EXT_IF_H__ */
