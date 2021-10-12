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

#ifndef __DMAC_EXT_IF_H__
#define __DMAC_EXT_IF_H__
/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "mac_device.h"
#include "frw_event.h"
#include "hcc_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define DMAC_UCAST_TX_COMP_TIMES 5 /* 建立BA会话前，需要产生单播帧的发送完成中断 */

#define DMAC_BA_SEQNO_MASK                  0x0FFF      /* max sequence number */
#define DMAC_BA_MAX_SEQNO_BY_TWO            2048
#define DMAC_BA_GREATER_THAN_SEQHI          1
#define DMAC_BA_BETWEEN_SEQLO_SEQHI         2
#define DMAC_BA_BMP_SIZE                    64
#define dmac_ba_seq_add(_seq1, _seq2)       (((_seq1) + (_seq2)) & DMAC_BA_SEQNO_MASK)
#define dmac_ba_seq_sub(_seq1, _seq2)       (((_seq1) - (_seq2)) & DMAC_BA_SEQNO_MASK)
#define dmac_ba_seqno_add(_seq1, _seq2)     (((_seq1) + (_seq2)) & DMAC_BA_SEQNO_MASK)
#define dmac_ba_seqno_sub(_seq1, _seq2)     (((_seq1) - (_seq2)) & DMAC_BA_SEQNO_MASK)

#define DMAC_INVALID_BA_LUT_INDEX          HAL_MAX_BA_LUT_SIZE
#define DMAC_TID_MAX_BUFS                  128          /* 发送BA窗口记录seq number的最大个数，必须是2的整数次幂 */
#define DMAC_TX_BUF_BITMAP_WORD_SIZE       32           /* 发送BA窗口记录seq number的bitmap所使用的类型长度 */
/* 发送BA窗口记录seq number的bit map的长度 */
#define DMAC_TX_BUF_BITMAP_WORDS \
    ((DMAC_TID_MAX_BUFS + DMAC_TX_BUF_BITMAP_WORD_SIZE - 1) / DMAC_TX_BUF_BITMAP_WORD_SIZE)
#define DMAC_WPA_802_11I                   BIT0         /* 安全加密 :  bss_info 中记录AP 能力标识， WPA or WPA2 */
#define DMAC_RSNA_802_11I                  BIT1

#define DMAC_TX_MAX_RISF_NUM                6
#define DMAC_TX_QUEUE_AGGR_DEPTH            2
#define DMAX_TX_QUEUE_SINGLE_DEPTH          2
#define DMAC_TX_QEUEU_MAX_PPDU_NUM          2
#define DMAC_TX_QUEUE_UAPSD_DEPTH           5
#define DMAC_TX_QUEUE_FAIL_CHECK_NUM        1000
#define DMAC_PA_ERROR_OFFSET 3
/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
  枚举说明: DMAC模块ROM化预留回调接口类型定义
**************************************************************************** */
typedef enum {
    DMAC_ROM_RESV_FUNC_FCS_START,
    DMAC_ROM_RESV_FUNC_RX_FILTER_AP,
    DMAC_ROM_RESV_FUNC_RX_FILTER_STA,
    DMAC_ROM_RESV_FUNC_RX_PROCESS_CONTROL,
    DMAC_ROM_RESV_FUNC_STA_UP_RX_BEACON,
    DMAC_ROM_RESV_FUNC_SCAN_MGMT_FILER,
    DMAC_ROM_RESV_FUNC_AP_UP_RX_OBSS_BEACON,
    DMAC_ROM_RESV_FUNC_AP_UP_RX_PROBE_REQ,
    DMAC_ROM_RESV_FUNC_MESH_CHECK_UNICAST_REPORT,
    DMAC_ROM_RESV_FUNC_MAC_MBCA_IE,
    DMAC_ROM_RESV_FUNC_TX_DEL_BA,
    DMAC_ROM_RESV_FUNC_HANDLE_CHAN_MGMT_STA,
    DMAC_ROM_RESV_FUNC_CHAN_SWITCH_SYNC,
    DMAC_ROM_RESV_FUNC_PSM_ALARM_CALLBACK,
    DMAC_ROM_RESV_FUNC_BCN_RX_ADJUST,
    DMAC_ROM_RESV_FUNC_TBTT_EVENT_HANDLE,
    DMAC_ROM_RESV_FUNC_BUTT
} dmac_rom_resv_func_enum;
typedef hi_u8 dmac_rom_resv_func_enum_uint8;
/* ****************************************************************************
  枚举说明: HOST DRX事件子类型定义
**************************************************************************** */
typedef enum {
    DMAC_TX_HOST_DRX = 0,

    DMAC_TX_HOST_DRX_BUTT
} dmac_tx_host_drx_subtype_enum;
typedef hi_u8 dmac_tx_host_drx_subtype_enum_uint8;

/* ****************************************************************************
  枚举说明: WLAN DTX事件子类型定义
**************************************************************************** */
typedef enum {
    DMAC_TX_WLAN_DTX = 0,

    DMAC_TX_WLAN_DTX_BUTT
} dmac_tx_wlan_dtx_subtype_enum;
typedef hi_u8 dmac_tx_wlan_dtx_subtype_enum_uint8;

/* ****************************************************************************
  枚举说明: WLAN CTX事件子类型定义
**************************************************************************** */
typedef enum {
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_ACTION = 0,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_ADD_USER,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_NOTIFY_ALG_ADD_USER,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_DEL_USER,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_BA_SYNC,               /* 收到wlan的Delba和addba rsp用于到dmac的同步 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_PRIV_REQ,              /* 11N自定义的请求的事件类型 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_SCAN_REQ,              /* 扫描请求 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_SCHED_SCAN_REQ,        /* PNO调度扫描请求 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_MGMT,                  /* 管理帧处理 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_RESET_PSM,             /* 收到认证请求 关联请求，复位用户的节能状态 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_SET_REG,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_CONN_RESULT,           /* 关联结果 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_EDCA_REG,      /* STA收到beacon和assoc rsp时，更新EDCA寄存器 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_SWITCH_TO_NEW_CHAN,    /* 切换至新信道事件 */
    DMAC_WALN_CTX_EVENT_SUB_TYPR_SELECT_CHAN,           /* 设置信道事件 */
    DMAC_WALN_CTX_EVENT_SUB_TYPR_DISABLE_TX,            /* 禁止硬件发送 */
    DMAC_WALN_CTX_EVENT_SUB_TYPR_ENABLE_TX,             /* 恢复硬件发送 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPR_RESTART_NETWORK,       /* 切换信道后，恢复BSS的运行 */
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_PSM_OPMODE_NOTIFY,     /* AP侧opmode notify帧时判断节能信息 */
#endif
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_SET_RX_FILTER,
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    DMAC_WLAN_CTX_EVENT_SUB_TYPR_EDCA_OPT,              /* edca优化中业务识别通知事件 */
#endif
#ifdef _PRE_WLAN_RF_110X_CALI_DPD
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_DPD_DATA_PROCESSED,
#endif
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_CALI_HMAC2DMAC,
#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_DSCR_OPT,
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_SET_MESH_USER_WHITELIST,    /* 设置Mesh用户白名单接收指定的组播/广播数据帧 */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_UNSET_MESH_USER_WHITELIST,  /* 删除Mesh用户白名单中的某个地址 */
#endif
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_CONN_FAIL_SET_CHANNEL,      /* 用于共存时关联失败恢复ap信道参数 */

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_BUTT
} dmac_wlan_ctx_event_sub_type_enum;
typedef hi_u8 dmac_wlan_ctx_event_sub_type_enum_uint8;

/* DMAC模块 WLAN_DRX子类型定义 */
typedef enum {
    DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_AP,                 /* AP模式: DMAC WLAN DRX 流程 */
    DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_STA,                /* STA模式: DMAC WLAN DRX 流程 */
    DMAC_WLAN_DRX_EVENT_SUB_TYPE_TKIP_MIC_FAILE,        /* DMAC tkip mic faile 上报给HMAC */

    DMAC_WLAN_DRX_EVENT_SUB_TYPE_BUTT
} dmac_wlan_drx_event_sub_type_enum;
typedef hi_u8 dmac_wlan_drx_event_sub_type_enum_uint8;

/* DMAC模块 WLAN_CRX子类型定义 */
typedef enum {
    DMAC_WLAN_CRX_INIT,              /* DMAC 给 HMAC的初始化参数 */
    DMAC_WLAN_CRX_RX,                /* DMAC WLAN CRX 流程 */
    DMAC_WLAN_CRX_DELBA,             /* DMAC自身产生的DELBA帧 */
    DMAC_WLAN_CRX_SCAN_RESULT,       /* 扫描到一个bss信息，上报结果 */
    DMAC_WLAN_CRX_SCAN_COMP,         /* DMAC扫描完成上报给HMAC */
    DMAC_WLAN_CRX_OBSS_SCAN_COMP,    /* DMAC OBSS扫描上报HMAC */
    DMAC_WLAN_CRX_ACS_RESP,          /* ACS */
#ifdef _PRE_WLAN_FEATURE_FLOWCTL
    DMAC_WLAN_CRX_FLOWCTL_BACKP,     /* dmac向hmac发送流控制反压信息 */
#endif
    DMAC_WLAN_CRX_DISASSOC,          /* DMAC上报去关联事件到HMAC, HMAC会删除用户 */
    DMAC_WLAN_CRX_DEAUTH,            /* DMAC上报去认证事件到HMAC */
    DMAC_WLAN_CRX_CH_SWITCH_COMPLETE, /* 信道切换完成事件 */
    DMAC_WLAN_CRX_DBAC,              /* DBAC enable/disable事件 */
#ifdef _PRE_WLAN_RF_110X_CALI_DPD
    DMAC_WLAN_CRX_DPD_DATA_SEND,
#endif
#ifdef _PRE_WLAN_FEATURE_WOW_ROM
    DMAC_WLAN_CRX_DEV_SYNC_HOST,
#endif
    DMAC_WLAN_CRX_SLEEP_REQ,
#ifdef _PRE_WLAN_FEATURE_CSI
    DMAC_WLAN_CRX_CSI_REPORT,       /* DMAC采集CSI数据抛事件给HMAC */
#endif
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    DMAC_WLAN_CRX_NEW_PEER_REPORT,  /* DMAC上报new peer beacon帧 */
#endif
#ifdef _PRE_WLAN_FEATURE_PROMIS
    DMAC_WLAN_CRX_EVENT_PROMIS,
#endif
    DMAC_WLAN_CRX_SUB_TYPE_BUTT
} dmac_wlan_crx_event_sub_type_enum;
typedef hi_u8 dmac_wlan_crx_event_sub_type_enum_uint8;

/* 发向HOST侧的配置事件 */
typedef enum {
    DMAC_TO_HMAC_CREATE_BA,
    DMAC_TO_HMAC_DEL_BA,
    DMAC_TO_HMAC_SYN_CFG,
    DMAC_TO_HMAC_ALG_INFO_SYN,
    DMAC_TO_HMAC_VOICE_AGGR,
    DMAC_TO_HMAC_PROTECTION_INFO_SYN,

    DMAC_TO_HMAC_SYN_BUTT
} dmac_to_hmac_syn_type_enum;

/* MISC杂散事件 */
typedef enum {
    DMAC_MISC_SUB_TYPE_DISASOC,
#ifdef _PRE_WLAN_FEATURE_WOW
    DMAC_MISC_SUB_TYPE_DEV_READY_FOR_HOST_SLP,
#endif
    DMAC_MISC_SUB_TYPE_BUTT
} dmac_misc_sub_type_enum;

typedef enum {
    WAIT_ADD,
    DMAC_HCC_RX_EVENT_SUB_TYPE_BUTT
} dmac_hcc_rx_event_sub_type_enum;

typedef enum {
    DMAC_DISASOC_MISC_LINKLOSS = 0,
    DMAC_DISASOC_MISC_KEEPALIVE = 1,
    DMAC_DISASOC_MISC_CHANNEL_MISMATCH = 2,
    DMAC_DISASOC_MISC_WOW_RX_DISASSOC = 3,
    DMAC_DISASOC_MISC_WOW_RX_DEAUTH = 4,
    DMAC_DISASOC_MISC_KICKUSER = 5,
    DMAC_DISASOC_ROAM_HANDLE_FAIL = 6,
    DMAC_DISASOC_SA_QUERY_DEL_USER = 7,

    DMAC_DISASOC_MISC_BUTT
} dmac_disasoc_misc_reason_enum;
typedef hi_u16 dmac_disasoc_misc_reason_enum_uint16;

/* HMAC to DMAC同步类型 */
typedef enum {
    HMAC_TO_DMAC_SYN_INIT,
    HMAC_TO_DMAC_SYN_CREATE_CFG_VAP,
    HMAC_TO_DMAC_SYN_CFG,
    HMAC_TO_DMAC_SYN_ALG,

    HMAC_TO_DMAC_SYN_BUTT
} hmac_to_dmac_syn_type_enum;
typedef hi_u8 hmac_to_dmac_syn_type_enum_uint8;

/* TXRX函数回调出参定义 */
typedef enum {
    DMAC_TXRX_PASS = 0, /* 继续往下 */
    DMAC_TXRX_DROP = 1, /* 需要丢包 */
    DMAC_TXRX_SENT = 2, /* 已被发送 */
    DMAC_TXRX_BUFF = 3, /* 已被缓存 */

    DMAC_TXRX_BUTT
} dmac_txrx_output_type_enum;
typedef hi_u8 dmac_txrx_output_type_enum_uint8;

/* 天线训练状态 */
typedef enum {
    DMAC_USER_SMARTANT_NON_TRAINING        = 0,
    DMAC_USER_SMARTANT_NULLDATA_TRAINING   = 1,
    DMAC_USER_SMARTANT_DATA_TRAINING       = 2,

    DMAC_USER_SMARTANT_TRAINING_BUTT
} dmac_user_smartant_training_enum;
typedef hi_u8 dmac_user_smartant_training_enum_uint8;

/* 算法的报文探测标志 (注:对于1102该枚举只允许使用3bit空间, 因此有效枚举值最大为7) */
typedef enum {
    DMAC_USER_ALG_NON_PROBE                     = 0,    /* 非探测报文 */
    DMAC_USER_ALG_TXBF_SOUNDING                 = 1,    /* TxBf sounding报文 */
    DMAC_USER_ALG_AUOTRATE_PROBE                = 2,    /* Autorate探测报文 */
    DMAC_USER_ALG_AGGR_PROBE                    = 3,    /* 聚合探测报文 */
    DMAC_USER_ALG_TPC_PROBE                     = 4,    /* TPC探测报文 */
    DMAC_USER_ALG_TX_MODE_PROBE                 = 5,    /* 发送模式探测报文(TxBf, STBC, Chain) */
    DMAC_USER_ALG_SMARTANT_NULLDATA_PROBE       = 6,    /* 智能天线NullData训练报文 */
    DMAC_USER_ALG_SMARTANT_DATA_PROBE           = 7,    /* 智能天线Data训练报文 */

    DMAC_USER_ALG_PROBE_BUTT
} dmac_user_alg_probe_enum;
typedef hi_u8 dmac_user_alg_probe_enum_uint8;

/* BA会话的状态枚举 */
typedef enum {
    DMAC_BA_INIT = 0,   /* BA会话未建立 */
    DMAC_BA_INPROGRESS, /* BA会话建立过程中 */
    DMAC_BA_COMPLETE,   /* BA会话建立完成 */
    DMAC_BA_HALTED,     /* BA会话节能暂停 */
    DMAC_BA_FAILED,     /* BA会话建立失败 */

    DMAC_BA_BUTT
} dmac_ba_conn_status_enum;
typedef hi_u8 dmac_ba_conn_status_enum_uint8;

/* Type of Tx Descriptor status */
typedef enum {
    DMAC_TX_INVALID   = 0,                /* 无效 */
    DMAC_TX_SUCC,                         /* 成功 */
    DMAC_TX_FAIL,                         /* 发送失败（超过重传限制：接收响应帧超时） */
    DMAC_TX_TIMEOUT,                      /* lifetime超时（没法送出去） */
    DMAC_TX_RTS_FAIL,                     /* RTS 发送失败（超出重传限制：接收cts超时） */
    DMAC_TX_NOT_COMPRASS_BA,              /* 收到的BA是非压缩块确认 */
    DMAC_TX_TID_MISMATCH,                 /* 收到的BA中TID与发送时填写在描述符中的TID不一致 */
    DMAC_TX_KEY_SEARCH_FAIL,              /* Key search failed */
    DMAC_TX_AMPDU_MISMATCH,               /* 描述符异常 */
    DMAC_TX_PENDING,                      /* tx pending：mac发送过该帧，但是没有成功，等待重传 */
    DMAC_TX_FAIL_ACK_ERROR,               /* 发送失败（超过重传限制：接收到的响应帧错误） */
    DMAC_TX_RTS_FAIL_CTS_ERROR,           /* RTS发送失败（超出重传限制：接收到的CTS错误） */
    DMAC_TX_FAIL_ABORT,                   /* 发送失败（因为abort） */
    DMAC_TX_FAIL_STATEMACHINE_PHY_ERROR,  /* MAC发送该帧异常结束（状态机超时、phy提前结束等原因） */
    DMAC_TX_SOFT_PSM_BACK,                /* 软件节能回退 */
    DMAC_TX_SOFT_RESERVE,                 /* reserved */
} dmac_tx_dscr_status_enum;
typedef hi_u8 dmac_tx_dscr_status_enum_uint8;

typedef enum {
    DMAC_TX_MODE_NORMAL  = 0,
    DMAC_TX_MODE_RIFS    = 1,
    DMAC_TX_MODE_AGGR    = 2,
    DMAC_TX_MODE_BUTT
} dmac_tx_mode_enum;
typedef hi_u8 dmac_tx_mode_enum_uint8;

/* mib index定义 */
typedef enum {
    WLAN_MIB_INDEX_LSIG_TXOP_PROTECTION_OPTION_IMPLEMENTED,
    WLAN_MIB_INDEX_HT_GREENFIELD_OPTION_IMPLEMENTED,
    WLAN_MIB_INDEX_SPEC_MGMT_IMPLEMENT,
    WLAN_MIB_INDEX_FORTY_MHZ_OPERN_IMPLEMENT,
    WLAN_MIB_INDEX_2040_COEXT_MGMT_SUPPORT,
    WLAN_MIB_INDEX_FORTY_MHZ_INTOL,
    WLAN_MIB_INDEX_VHT_CHAN_WIDTH_OPTION_IMPLEMENT,
    WLAN_MIB_INDEX_MINIMUM_MPDU_STARTING_SPACING,

    WLAN_MIB_INDEX_OBSSSCAN_TRIGGER_INTERVAL, /* 8 */
    WLAN_MIB_INDEX_OBSSSCAN_TRANSITION_DELAY_FACTOR,
    WLAN_MIB_INDEX_OBSSSCAN_PASSIVE_DWELL,
    WLAN_MIB_INDEX_OBSSSCAN_ACTIVE_DWELL,
    WLAN_MIB_INDEX_OBSSSCAN_PASSIVE_TOTAL_PER_CHANNEL,
    WLAN_MIB_INDEX_OBSSSCAN_ACTIVE_TOTAL_PER_CHANNEL,
    WLAN_MIB_INDEX_OBSSSCAN_ACTIVITY_THRESHOLD, /* 14 */

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    WLAN_MIB_INDEX_MESH_ACCEPTING_PEER,
    WLAN_MIB_INDEX_MESH_SECURITY_ACTIVATED,
#endif
    WLAN_MIB_INDEX_BUTT
} wlan_mib_index_enum;
typedef hi_u16 wlan_mib_index_enum_uint16;

typedef enum {
    DMAC_TID_PAUSE_RESUME_TYPE_BA = 0,
    DMAC_TID_PAUSE_RESUME_TYPE_PS = 1,
    DMAC_TID_PAUSE_RESUME_TYPE_BUTT
} dmac_tid_pause_type_enum;
typedef hi_u8 dmac_tid_pause_type_enum_uint8;

/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
typedef struct {
    dmac_disasoc_misc_reason_enum_uint16 disasoc_reason;
    hi_u8 user_idx;
    hi_u8 uc_resv;
} dmac_disasoc_misc_stru;

/* 接收帧的统计信息 */
typedef struct {
    hi_u32 total_num;         /* 总数 */
    hi_u32 self_fcs_correct;  /* 发给自己的FCS正确的单播帧 */
    hi_u32 other_fcs_correct; /* 不是发给自己的FCS正确的单播帧 */
    hi_u32 total_fcs_error;   /* FCS错误的所有帧 */
} dmac_rx_fcs_statistic;

typedef struct {
    hi_u8                   tid_num;                            /* 需要发送的tid队列号 */
    dmac_tx_mode_enum_uint8 tx_mode;                            /* 调度tid的发送模式 */
    hi_u8                   mpdu_num[DMAC_TX_QUEUE_AGGR_DEPTH]; /* 调度得到的需要发送的mpdu个数 */
    hi_u8                   user_idx;                           /* 要发送的tid队列隶属的user */
    hi_u8                   ba_is_jamed;                        /* 当前BA窗是否卡死的标志位 */
    hi_u8                   uc_resv[2];                         /* 2 byte保留字段 */
} mac_tid_schedule_output_stru;

/* DMAC与HMAC模块共用的WLAN DRX事件结构体 */
typedef struct {
    oal_dev_netbuf_stru *netbuf;            /* netbuf链表一个元素 */
    hi_u16              us_netbuf_num;      /* netbuf链表的个数 */
    hi_u8               auc_resv[2];        /* 字节对齐, 2 byte保留字段 */
} dmac_wlan_drx_event_stru;

/* DMAC与HMAC模块共用的WLAN CRX事件结构体 */
typedef struct {
    oal_dev_netbuf_stru *netbuf; /* 指向管理帧对应的netbuf */
    hi_u8 *puc_chtxt;            /* Shared Key认证用的challenge text */
} dmac_wlan_crx_event_stru;

#ifdef _PRE_WLAN_FEATURE_BTCOEX
typedef struct {
    hi_u8 need_delba;
    hi_u8 ba_size;
    hi_u8 auc_reserve[2]; /* 2 byte保留字段 */
} d2h_btcoex_delba_event_stru;
#endif

typedef struct {
    hi_u8 user_index;
    hi_u8 tid;
    hi_u8 vap_id;
    hi_u8 cur_protocol;
} dmac_to_hmac_ctx_event_stru;

typedef struct {
    hi_u8 user_index;
    hi_u8 cur_bandwidth;
    hi_u8 cur_protocol;
    hi_u8 uc_resv;
} dmac_to_hmac_syn_info_event_stru;

typedef struct {
    hi_u8 vap_id;
    hi_u8 voice_aggr;  /* 是否支持Voice聚合 */
    hi_u8 auc_resv[2]; /* 2 byte保留字段 */
} dmac_to_hmac_voice_aggr_event_stru;

/* mic攻击 */
typedef struct {
    hi_u8                  auc_user_mac[WLAN_MAC_ADDR_LEN];
    hi_u8                  auc_reserve[2]; /* 2 byte保留字段 */
    oal_nl80211_key_type   key_type;
    hi_s32                 l_key_id;
} dmac_to_hmac_mic_event_stru;

/* DMAC与HMAC模块共用的DTX事件结构体 */
typedef struct {
    void                   *netbuf;         /* netbuf链表一个元素 */
    hi_u32                  us_frame_len;
} dmac_tx_event_stru;

#ifdef _PRE_WLAN_RF_110X_CALI_DPD
typedef struct {
    hi_u32        us_dpd_data[128];  /* dpd calibration data,128 byte */
    hi_u16        us_data_len;       /* data length */
    hi_u8         reserve[2];        /* 2 byte保留字段 */
} dpd_cali_data_stru;
#endif

typedef struct {
    mac_channel_stru                     channel;
    mac_ch_switch_info_stru              ch_switch_info;

    hi_u8 switch_immediately; /* 1 - 马上切换  0 - 暂不切换, 推迟到tbtt中切换 */
    hi_u8 check_cac;
    hi_u8 auc_resv[2];        /* 2 byte保留字段 */
} dmac_set_chan_stru;

typedef struct {
    wlan_ch_switch_status_enum_uint8  ch_switch_status;      /* 信道切换状态 */
    hi_u8                             announced_channel;     /* 新信道号 */
    wlan_channel_bandwidth_enum_uint8 announced_bandwidth;   /* 新带宽模式 */
    hi_u8                             ch_switch_cnt;         /* 信道切换计数 */
    hi_u8                             csa_present_in_bcn;    /* Beacon帧中是否包含CSA IE */
    hi_u8                             auc_reserve[3];        /* 3 byte保留字段 */
} dmac_set_ch_switch_info_stru;

/*
    (1)DMAC与HMAC模块共用的CTX子类型ACTION对应的事件的结构体
    (2)当DMAC自身产生DELBA帧时，使用该结构体向HMAC模块抛事件
*/
typedef struct {
    mac_category_enum_uint8 action_category;     /* ACTION帧的类型 */
    hi_u8                   action;              /* 不同ACTION类下的子帧类型 */
    hi_u8                   user_idx;
    hi_u8                   uc_resv;

    hi_u32                  us_frame_len;        /* 帧长度 */

    hi_u8                   hdr_len;             /* 帧头长度 */
    hi_u8                   tidno;               /* tidno，部分action帧使用 */
    hi_u8                   initiator;           /* 触发端方向 */
    /* 以下为接收到req帧，发送rsp帧后，需要同步到dmac的内容 */
    hi_u8                   stauts;              /* rsp帧中的状态 */

    hi_u16                  us_baw_start;        /* 窗口开始序列号 */
    hi_u16                  us_baw_size;         /* 窗口大小 */

    hi_u8                   ampdu_max_num;       /* BA会话下的最多聚合的AMPDU的个数 */
    hi_u8                   amsdu_supp;          /* 是否支持AMSDU */
    hi_u16                  us_ba_timeout;       /* BA会话交互超时时间 */

    mac_back_variant_enum_uint8     back_var;    /* BA会话的变体 */
    hi_u8                   dialog_token;        /* ADDBA交互帧的dialog token */
    hi_u8                   ba_policy;           /* Immediate=1 Delayed=0 */
    hi_u8                   lut_index;           /* LUT索引 */
    hi_u8                   auc_mac_addr[WLAN_MAC_ADDR_LEN];    /* 用于DELBA查找HMAC用户 */
    hi_u8                   resv[2];             /* 2 byte保留字段 */
} dmac_ctx_action_event_stru;

/* 添加用户事件payload结构体 */
typedef struct {
    hi_u8   user_idx;     /* 用户index */
    hi_u8   uc_resv;
    hi_u16  us_sta_aid;

    hi_u8   auc_user_mac_addr[WLAN_MAC_ADDR_LEN];
    hi_u8   auc_bssid[WLAN_MAC_ADDR_LEN];

    mac_vht_hdl_stru          vht_hdl;
    mac_user_ht_hdl_stru      ht_hdl;
    mac_ap_type_enum_uint8    ap_type;
    hi_u8                     resv[3]; /* 3 byte保留字段 */
} dmac_ctx_add_user_stru;

/* 删除用户事件结构体 */
typedef dmac_ctx_add_user_stru dmac_ctx_del_user_stru;

/* 扫描请求事件payload结构体 */
typedef struct {
    mac_scan_req_stru *scan_params; /* 将扫描参数传下去 */
} dmac_ctx_scan_req_stru;

typedef struct {
    hi_u8 scan_idx;
    hi_u8 auc_resv[3]; /* 3 byte保留字段 */
    mac_scan_chan_stats_stru chan_result;
} dmac_crx_chan_result_stru;

/* Update join req 参数写寄存器的结构体定义 */
typedef struct {
    hi_u8             auc_bssid[WLAN_MAC_ADDR_LEN];           /* 加入的AP的BSSID  */
    hi_u16            us_beacon_period;
    mac_channel_stru  current_channel;                     /* 要切换的信道信息 */
    hi_u32            beacon_filter;                       /* 过滤beacon帧的滤波器开启标识位 */
    hi_u32            non_frame_filter;                    /* 过滤no_frame帧的滤波器开启标识位 */
    hi_char           auc_ssid[WLAN_SSID_MAX_LEN];            /* 加入的AP的SSID  */
    hi_u8             dtim_period;                         /* dtim period      */
    hi_u8             dot11_forty_m_hz_operation_implemented;
    hi_u8             auc_resv;
} dmac_ctx_join_req_set_reg_stru;

/* wait join写寄存器参数的结构体定义 */
typedef struct {
    hi_u32 dtim_period;                  /* dtim period */
    hi_u32 dtim_cnt;                     /* dtim count  */
    hi_u8  auc_bssid[WLAN_MAC_ADDR_LEN]; /* 加入的AP的BSSID  */
    hi_u16 us_tsf_bit0;                  /* tsf bit0  */
} dmac_ctx_set_dtim_tsf_reg_stru;

/* wait join misc写寄存器参数的结构体定义 */
typedef struct {
    hi_u32 beacon_filter; /* 过滤beacon帧的滤波器开启标识位 */
} dmac_ctx_join_misc_set_reg_stru;

/* wait join写寄存器参数的结构体定义 */
typedef struct {
    hi_u16 user_index;  /* user index */
    hi_u8  auc_resv[2]; /* 2 byte保留字段 */
} dmac_ctx_asoc_set_reg_stru;

/* sta更新edca参数寄存器的结构体定义 */
typedef struct {
    hi_u8 vap_id;
    mac_wmm_set_param_type_enum_uint8 set_param_type;
    hi_u8 auc_resv[2]; /* 2 byte保留字段 */
    wlan_mib_dot11_qapedca_entry_stru ast_wlan_mib_qap_edac[WLAN_WME_AC_BUTT];
} dmac_ctx_sta_asoc_set_edca_reg_stru;

#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct {
    hi_u8 auc_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 set; /* 0 - 删除某多播地址、1 - 设置某多播地址 */
    hi_u8 rsv;
} dmac_ctx_mesh_mac_addr_whitelist_stru;
#endif

/* DMAC模块模块接收流程控制信息数据结构定义, 与hal_rx_ctl_stru结构体保持一致 */
typedef struct {
    /* word 0 */
    hi_u8                   vap_id            : 5;
    hi_u8                   amsdu_enable      : 1;
    hi_u8                   is_first_buffer   : 1;
    hi_u8                   is_fragmented     : 1;
    hi_u8                   msdu_in_buffer;
    hi_u8                   da_user_idx       : 4;
    hi_u8                   ta_user_idx       : 4;
    hi_u8                   mac_header_len    : 6;  /* mac header帧头长度 */
    hi_u8                   is_beacon         : 1;
    hi_u8                   reserved1         : 1;
    /* word 1 */
    hi_u16                  us_frame_len;            /* 帧头与帧体的总长度 */
    hi_u8                   mac_vap_id         : 4;
    hi_u8                   buff_nums          : 4;  /* 每个MPDU占用的buf数 */
    hi_u8                   channel_number;          /* 接收帧的信道 */
} dmac_rx_info_stru;

/* rx cb字段 20字节可用 不允许再增加字节! */
typedef struct {
    hi_s8 rssi_dbm;
    union {
        struct {
            hi_u8   bit_vht_mcs       : 4;
            hi_u8   bit_nss_mode      : 2;
            hi_u8   bit_protocol_mode : 2;
        } st_vht_nss_mcs;                                   /* 11ac的速率集定义 */
        struct {
            hi_u8   bit_ht_mcs        : 6;
            hi_u8   bit_protocol_mode : 2;
        } st_ht_rate;                                       /* 11n的速率集定义 */
        struct {
            hi_u8   bit_legacy_rate   : 4;
            hi_u8   bit_reserved1     : 2;
            hi_u8   bit_protocol_mode : 2;
        } st_legacy_rate;                                   /* 11a/b/g的速率集定义 */
    } un_nss_rate;

    hi_u8 uc_short_gi;
    hi_u8 uc_bandwidth;
} hal_rx_statistic_stru;

#pragma pack(push, 1)
typedef struct {
    /* word 0 */
    hi_u8   cipher_protocol_type  : 3;
    hi_u8   ampdu                 : 1;
    hi_u8   dscr_status           : 4;
    hi_u8   stbc                  : 2;
    hi_u8   gi                    : 1;
    hi_u8   smoothing             : 1;
    hi_u8   preabmle              : 1;
    hi_u8   rsvd                  : 3;
    hi_u8   auc_resv[2];   /* 2: 预留数组大小 */
} hal_rx_status_stru;
#pragma pack(pop)

typedef struct {
    dmac_rx_info_stru           rx_info;         /* hal传给dmac的数据信息 */
    hal_rx_status_stru          rx_status;       /* 保存加密类型及帧长信息 */
    hal_rx_statistic_stru       rx_statistic;    /* 保存接收描述符的统计信息 */
} dmac_rx_ctl_stru;

/* hmac to dmac配置同步消息结构 */
typedef struct {
    wlan_cfgid_enum_uint16 syn_id;          /* 同步事件ID */
    hi_u16                 us_len;          /* 事件payload长度 */
    hi_u8                  auc_msg_body[4]; /* 事件payload, 4byte */
} hmac_to_dmac_cfg_msg_stru;

typedef hmac_to_dmac_cfg_msg_stru dmac_to_hmac_cfg_msg_stru;

/* HMAC到DMAC配置同步操作 */
typedef struct {
    wlan_cfgid_enum_uint16  cfgid;
    hi_u8                   auc_resv[2]; /* 2 byte保留字段 */
    hi_u32(*set_func)(mac_vap_stru *mac_vap, hi_u8 uc_len, const hi_u8 *puc_param);
} dmac_config_syn_stru;

typedef dmac_config_syn_stru hmac_config_syn_stru;

/* tx cb字段 当前已用19字节，只有20字节可用 不允许再增加字节! */
struct dmac_tx_ctl {
    /* ampdu中包含的MPDU个数,实际描述符填写的值为此值-1 */
    hi_u8                               mpdu_num                : 7;
    hi_u8                               netbuf_num              : 1;   /* 每个MPDU占用的netbuf数目 */

    hi_u8                               frame_header_length     : 6;   /* 该MPDU的802.11头长度 */
    hi_u8                               is_first_msdu           : 1;   /* 是否是第一个子帧，HI_FALSE不是 HI_TRUE是 */
    hi_u8                               is_amsdu                : 1;   /* 是否AMSDU: HI_FALSE不是，HI_TRUE是 */

    /* 取值:FRW_EVENT_TYPE_WLAN_DTX和FRW_EVENT_TYPE_HOST_DRX，作用:在释放时区分是内存池的netbuf还是原生态的 */
    frw_event_type_enum_uint8           en_event_type           : 6;
    hi_u8                               is_needretry            : 1;
    hi_u8                               is_vipframe             : 1;   /* 该帧是否是EAPOL帧、DHCP帧 */

    /* dmac tx 到 tx complete 传递的user结构体，目标用户地址 */
    hi_u8                               tx_user_idx             : 4;
    dmac_user_alg_probe_enum_uint8      is_probe_data           : 3;    /* 是否探测帧 */
    /* 该MPDU是单播还是多播:HI_FALSE单播，HI_TRUE多播 */
    hi_u8                               ismcast                 : 1;

    hi_u8                               retried_num             : 4;
    hi_u8                               need_rsp                : 1;   /* WPAS send mgmt,need dmac response tx status */
    hi_u8                               is_eapol                : 1;   /* 该帧是否是EAPOL帧 1102可以去掉 */
    /* 节能特性用，标识一个MPDU是否从节能队列中取出来的 */
    hi_u8                               is_get_from_ps_queue    : 1;
    hi_u8                               is_eapol_key_ptk        : 1;   /* 4 次握手过程中设置单播密钥EAPOL KEY 帧标识 */

    hi_u8                               tx_vap_index            : 3;
    hi_u8                               mgmt_frame_id           : 4;   /* wpas 发送管理帧的frame id */
    hi_u8                               roam_data               : 1;

    hi_u8                               ac                      : 3;   /* ac */
    wlan_tx_ack_policy_enum_uint8       ack_policy              : 3;   /* ACK 策略 */
    hi_u8                               is_any_frame            : 1;
    hi_u8                               high_prio_sch           : 1;

    hi_u8                               alg_pktno;                     /* 算法用到的字段，唯一标示该报文 */

    hi_u8                               timestamp_us[8];               /* 维测使用入TID队列时的时间戳 */

    hi_u8                               tid                     : 4;
    hi_u8                               tx_user_idx_bak         : 4;
    hi_u8                               tsf;
    /* mpdu字节数，维测用，不包括头尾，不包括snap，不包括padding */
    hi_u16                              us_mpdu_bytes;
} __OAL_DECLARE_PACKED;
typedef struct dmac_tx_ctl dmac_tx_ctl_stru;

typedef struct {
    hi_u32      best_rate_goodput_kbps;
    hi_u32      rate_kbps;           /* 速率大小(单位:kbps) */
    hi_u8       aggr_subfrm_size;    /* 聚合子帧数门限值 */
    hi_u8       per;                 /* 该速率的PER(单位:%) */
    hi_u16      resv;
} dmac_tx_normal_rate_stats_stru;

typedef struct {
    hi_void                             *ba;
    hi_u8                               tid;
    mac_delba_initiator_enum_uint8      direction;
    hi_u8                               mac_user_idx;
    hi_u8                               vap_id;
    hi_u16                              us_timeout_times;
    hi_u8                               uc_resv[2]; /* 2 byte保留字段 */
} dmac_ba_alarm_stru;

/* 一个站点下的每一个TID下的聚合接收的状态信息 */
typedef struct {
    hi_u16                      us_baw_start;         /* 第一个未收到的MPDU的序列号 */
    hi_u8                       is_ba;                /* Session Valid Flag */
    hi_u8                       auc_resv;

    /* 建立BA会话相关的信息 */
    dmac_ba_conn_status_enum_uint8  ba_conn_status;    /* BA会话的状态 */
    mac_back_variant_enum_uint8     back_var;          /* BA会话的变体 */
    hi_u8                       ba_policy;             /* Immediate=1 Delayed=0 */
    hi_u8                       lut_index;             /* 接收端Session H/w LUT Index */
    hi_u8                      *puc_transmit_addr;     /* BA会话发送端地址 */
} dmac_ba_rx_stru;

typedef struct {
    hi_u8    in_use;
    hi_u8    uc_resv[1];
    hi_u16   us_seq_num;
    hi_void *tx_dscr;
} dmac_retry_queue_stru;

typedef struct {
    hi_u16                      us_baw_start;           /* 第一个未确认的MPDU的序列号 */
    hi_u16                      us_last_seq_num;        /* 最后一个发送的MPDU的序列号 */

    hi_u16                      us_baw_size;            /* Block_Ack会话的buffer size大小 */
    hi_u16                      us_baw_head;            /* bitmap中记录的第一个未确认的包的位置 */

    hi_u16                      us_baw_tail;            /* bitmap中下一个未使用的位置 */
    hi_u8                       is_ba;                  /* Session Valid Flag */
    dmac_ba_conn_status_enum_uint8  ba_conn_status;     /* BA会话的状态 */

    mac_back_variant_enum_uint8 back_var;                /* BA会话的变体 */
    hi_u8                       dialog_token;            /* ADDBA交互帧的dialog token */
    hi_u8                       ba_policy;               /* Immediate=1 Delayed=0 */
    hi_u8                       amsdu_supp;              /* BLOCK ACK支持AMSDU的标识 */

    hi_u8                      *puc_dst_addr;            /* BA会话接收端地址 */
    hi_u16                      us_ba_timeout;           /* BA会话交互超时时间 */
    hi_u8                       ampdu_max_num;           /* BA会话下，能够聚合的最大的mpdu的个数 */
    hi_u8                       mac_user_idx;

    hi_u16                      us_pre_baw_start;        /* 记录前一次判断ba窗是否卡死时的ssn */
    hi_u16                      us_pre_last_seq_num;     /* 记录前一次判断ba窗是否卡死时的lsn */

    hi_u16                      ba_jamed_cnt;            /* BA窗卡死统计次数 */
    hi_u8                       resv[2];                 /* 2个保留对齐字段 */

    hi_u32                      aul_tx_buf_bitmap[DMAC_TX_BUF_BITMAP_WORDS];
} dmac_ba_tx_stru;

/* 11n下的参数，需要在关联时进行设置 */
typedef struct {
    hi_u8               ampdu_max_num;
    hi_u8               auc_reserve[1];
    hi_u16              us_ampdu_max_size;
} dmac_ht_handle_stru;

#ifdef _PRE_DEBUG_MODE
typedef oam_stats_ampdu_stat_stru dmac_tid_ampdu_stat_stru;
#endif

typedef struct {
    hi_u8               tid          : 4,            /* 通信标识符 */
                        is_paused    : 2,            /* TID被暂停调度 */
                        is_delba_ing : 1,            /* 该tid是否正在发delba */
                        uc_resv      : 1;
    hi_u8               retry_num;                   /* tid队列中重传报文的个数 */
    hi_u16              us_mpdu_num;                    /* MPDU个数 */

    hi_u8               user_idx;                    /* 无效值为MAC_RES_MAC_USER_NUM */
    hi_u8               vap_id;
    dmac_tx_mode_enum_uint8 tx_mode;                 /* 发送模式: rifs,aggr,normal发送 */
    hi_u8               rx_wrong_ampdu_num;          /* 该tid未建立BA会话时收到的聚合子帧数(一般是DELBA失败) */

    hi_list             hdr;                         /* tid缓存队列头 */

    hi_void             *alg_priv;                   /* TID级别算法私有结构体 */
    dmac_tx_normal_rate_stats_stru rate_stats;       /* 速率算法在发送完成中统计出的信息 */
    dmac_ba_tx_stru     *ba_tx_hdl;
    dmac_ba_rx_stru     *ba_rx_hdl;
    dmac_ht_handle_stru ht_tx_hdl;

#ifdef _PRE_DEBUG_MODE
    dmac_tid_ampdu_stat_stru *tid_ampdu_stat; /* ampdu业务流程统计信息 */
#endif
} dmac_tid_stru;

/* 复位原因定义 */
typedef enum {
    DMAC_RESET_REASON_SW_ERR = 0,
    DMAC_RESET_REASON_HW_ERR,
    DMAC_RESET_REASON_CONFIG,
    DMAC_RETST_REASON_OVER_TEMP,

    DMAC_RESET_REASON_BUTT
} dmac_reset_mac_submod_enum;
typedef hi_u8 dmac_reset_mac_submod_enum_uint8;

typedef struct {
    hi_u8 reason;
    hi_u8 event;
    hi_u8 auc_des_addr[WLAN_MAC_ADDR_LEN];
} dmac_diasoc_deauth_event;

#define DMAC_QUERY_EVENT_LEN 48 /* 消息内容的长度 */
typedef enum {
    OAL_QUERY_STATION_INFO_EVENT      = 1,
    OAL_ATCMDSRV_DBB_NUM_INFO_EVENT   = 2,
    OAL_ATCMDSRV_FEM_PA_INFO_EVENT    = 3,
    OAL_ATCMDSRV_GET_RX_PKCG          = 4,
    OAL_ATCMDSRV_LTE_GPIO_CHECK       = 5,
    OAL_QUERY_EVNET_BUTT
} oal_query_event_id_enum;

typedef struct {
    hi_u8 query_event;
    hi_u8 auc_query_sta_addr[WLAN_MAC_ADDR_LEN];
} dmac_query_request_event;

typedef struct {
    hi_u8 query_event;
    hi_s8 reserve[DMAC_QUERY_EVENT_LEN];
} dmac_query_response_event;

typedef struct {
    hi_u8 query_event;
    hi_u8 auc_query_sta_addr[WLAN_MAC_ADDR_LEN]; /* sta mac地址 */
} dmac_query_station_info_request_event;

typedef struct {
    hi_s32 signal;          /* 信号强度 */
    hi_u32 rx_packets;      /* total packets received   */
    hi_u32 tx_packets;      /* total packets transmitted    */
    hi_u32 rx_bytes;        /* total bytes received     */
    hi_u32 tx_bytes;        /* total bytes transmitted  */
    hi_u32 tx_retries;      /* 发送重传次数 */
    hi_u32 rx_dropped_misc; /* 接收失败次数 */
    hi_u32 tx_failed;       /* 发送失败次数  */
    /* word10 */
    hi_u16 asoc_id;      /* Association ID of the STA */
    hi_s16 s_free_power; /* 底噪 */
    /* word11 */
    oal_rate_info_stru txrate; /* vap当前速率 */
    hi_u8 query_event;         /* 消息号 */
    hi_u8 resv;
} dmac_query_station_info_response_event;

typedef struct {
    hi_u32 cycles;                   /* 统计间隔时钟周期数 */
    hi_u32 sw_tx_succ_num;           /* 软件统计发送成功ppdu个数 */
    hi_u32 sw_tx_fail_num;           /* 软件统计发送失败ppdu个数 */
    hi_u32 sw_rx_ampdu_succ_num;     /* 软件统计接收成功的ampdu个数 */
    hi_u32 sw_rx_mpdu_succ_num;      /* 软件统计接收成功的mpdu个数 */
    hi_u32 sw_rx_ppdu_fail_num;      /* 软件统计接收失败的ppdu个数 */
    hi_u32 hw_rx_ampdu_fcs_fail_num; /* 硬件统计接收ampdu fcs校验失败个数 */
    hi_u32 hw_rx_mpdu_fcs_fail_num;  /* 硬件统计接收mpdu fcs校验失败个数 */
} dmac_thruput_info_sync_stru;

typedef struct {
    hi_u8 tx_status;
    hi_u8 mgmt_frame_id;
    hi_u8 user_idx;
    hi_u8 uc_resv;
} dmac_crx_mgmt_tx_status_stru;

/* 函数执行控制枚举 */
typedef enum {
    DMAC_RX_FRAME_CTRL_GOON,   /* 本数据帧需要继续处理 */
    DMAC_RX_FRAME_CTRL_DROP,   /* 本数据帧需要丢弃 */
    DMAC_RX_FRAME_CTRL_BA_BUF, /* 本数据帧被BA会话缓存 */

    DMAC_RX_FRAME_CTRL_BUTT
} dmac_rx_frame_ctrl_enum;
typedef hi_u8 dmac_rx_frame_ctrl_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* 单播数据帧发送信息上报lwip */
typedef struct {
    /* Word1-2 */
    hi_u8   auc_da[WLAN_MAC_ADDR_LEN];           /* MSDU发送的目的地址 */
    hi_u16  us_length;                           /* 帧的长度(不包括802.11 头部) */
    /* Word3 */
    hi_u8   tx_count;
    wlan_mesh_tx_status_enum_uint8 mesh_tx_status;
    hi_u8   resv[2];                          /* 2 byte保留字段 */
    /* Word4-5 */
    hi_u32  bw;                               /* 带宽 */
    hi_u32  rate_kbps;                        /* 发送速率 */
} dmac_tx_info_report_stru;

typedef struct {
    hi_u8 auc_da[WLAN_MAC_ADDR_LEN]; /* MSDU发送的目的地址 */
    wlan_mesh_tx_status_enum_uint8 mesh_tx_status;
    hi_u8 rsv;
} dmac_tx_info_sync_stru;
#endif

/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* 从hcc头里面解析mpdu len */
#define dmac_get_frame_subtype_by_txcb(_pst_tx_ctrl) \
    (((mac_ieee80211_frame_stru *)((hi_u8 *)(_pst_tx_ctrl) + HI_MAX_DEV_CB_LEN))->frame_control.sub_type)
#ifdef HAVE_PCLINT_CHECK
#define dmac_get_mpdu_len_by_txcb(_pst_tx_ctrl) \
    (((hcc_header_stru *)((hi_u8 *)(_pst_tx_ctrl) - (OAL_HCC_HDR_LEN)))->pay_len)
#else
#define dmac_get_mpdu_len_by_txcb(_pst_tx_ctrl) \
    (((hcc_header_stru *)((hi_u8 *)(_pst_tx_ctrl) - (OAL_HCC_HDR_LEN + OAL_PAD_HDR_LEN)))->pay_len)
#endif
#define dmac_get_frame_type_by_txcb(_pst_tx_ctrl) \
    (((mac_ieee80211_frame_stru *)((hi_u8 *)(_pst_tx_ctrl) + HI_MAX_DEV_CB_LEN))->frame_control.type)

/* ****************************************************************************
 功能描述  : 获取seqnum的值
**************************************************************************** */
static inline hi_u16 dmac_get_seqnum_by_txcb(const dmac_tx_ctl_stru *tx_ctrl)
{
    return ((mac_ieee80211_frame_stru *)((hi_u8 *)tx_ctrl + HI_MAX_DEV_CB_LEN))->seq_num;
}

/* ****************************************************************************
 功能描述  : 通过cb字段获取mac头指针 CB字段后即为MAC头
**************************************************************************** */
static inline mac_ieee80211_frame_stru *dmac_get_mac_hdr_by_txcb(dmac_tx_ctl_stru *tx_ctrl)
{
    return (mac_ieee80211_frame_stru *)((hi_u8 *)tx_ctrl + HI_MAX_DEV_CB_LEN);
}

/* ****************************************************************************
 功能描述  : device侧获取mac头的值
**************************************************************************** */
static inline hi_u32 *dmac_get_mac_hdr_by_rxcb(dmac_rx_info_stru *cb_ctrl)
{
    return (hi_u32 *)((hi_u8 *)cb_ctrl + HI_MAX_DEV_CB_LEN);
}


/* ****************************************************************************
  HOST侧 CB字段以及获取CB字段的宏定义 待hmac整改完成后移动到hmac
**************************************************************************** */
/* HMAC模块接收流程控制信息数据结构定义 只使用20bytes hal的维测信息hmac不需要 */
typedef struct {
    /* word 0 */
    hi_u8                   vap_id            : 5;
    hi_u8                   amsdu_enable      : 1;
    hi_u8                   is_first_buffer   : 1;
    hi_u8                   is_fragmented     : 1;
    hi_u8                   msdu_in_buffer;
    hi_u8                   buff_nums         : 4;      /* 每个MPDU占用的buf数目 */
    hi_u8                   is_beacon         : 1;
    hi_u8                   reserved1         : 3;
    hi_u8                   mac_header_len;             /* mac header帧头长度 */
    /* word 1 */
    hi_u16                  us_frame_len;               /* 帧头与帧体的总长度 */
    hi_u16                  us_da_user_idx;             /* 目的地址用户索引 */
    /* word 2 */
    hi_u32                 *pul_mac_hdr_start_addr;     /* 对应的帧的帧头地址,虚拟地址 */
    /* word 3 */
    hi_u8                   us_ta_user_idx;             /* 发送端地址用户索引 */
    hi_u8                   mac_vap_id;
    hi_u8                   channel_number;             /* 接收帧的信道 */
    /* word 4 */
    hi_s8                   rssi_dbm;
} hmac_rx_ctl_stru;

/* host侧netbuf控制字段(CB)，总长度为48字节 (目前已用42字节) */
typedef struct {
    hi_u8                               mpdu_num;                /* ampdu中包含的MPDU个数,实际描述符填写的值为此值-1 */
    hi_u8                               netbuf_num;              /* 每个MPDU占用的netbuf数目 */
    hi_u8                               resv[2];                 /* 2 byte保留字段 */
    hi_u32                              us_mpdu_len;             /* 每个MPDU的长度不包括mac header length */

    hi_u8                               is_amsdu               : 1;    /* 是否AMSDU: HI_FALSE不是，HI_TRUE是 */
    /* 该MPDU是单播还是多播:HI_FALSE单播，HI_TRUE多播 */
    hi_u8                               ismcast                : 1;
    hi_u8                               is_eapol               : 1;    /* 该帧是否是EAPOL帧 */
    hi_u8                               use_4_addr             : 1;    /* 是否使用4地址，由WDS特性决定 */
    /* 节能特性用，标识一个MPDU是否从节能队列中取出来的 */
    hi_u8                               is_get_from_ps_queue   : 1;
    hi_u8                               is_first_msdu          : 1;    /* 是否是第一个子帧，HI_FALSE不是 HI_TRUE是 */
    hi_u8                               need_pause_tid         : 1;
    hi_u8                               is_bar                 : 1;
    hi_u8                               frame_header_length;           /* 该MPDU的802.11头长度 */
    hi_u8                               is_qosdata             : 1;    /* 指示该帧是否是qos data */
    /* 0: 802.11 mac头不在skb中，另外申请了内存存放； 1: 802.11 mac头在skb中 */
    hi_u8                               mac_head_type          : 1;
    hi_u8                               is_vipframe            : 1;    /* 该帧是否是EAPOL帧、DHCP帧 */
    hi_u8                               is_needretry           : 1;
    /* 该帧的SN号由软件维护，硬件禁止维护(目前仅用于非QOS分片帧 ) */
    hi_u8                               seq_ctrl_bypass        : 1;
    hi_u8                               need_rsp               : 1;    /* WPAS send mgmt,need dmac response tx status */
    hi_u8                               is_eapol_key_ptk       : 1;    /* 4 次握手过程中设置单播密钥EAPOL KEY 帧标识 */
    hi_u8                               roam_data              : 1;

    wlan_frame_type_enum_uint8          frame_type;                    /* 帧类型：数据帧，管理帧... */
    mac_ieee80211_frame_stru           *frame_header;                  /* 该MPDU的帧头指针 */

    hi_u8                               ac;                            /* ac */
    hi_u8                               tid;                           /* tid */
    /* 取值:FRW_EVENT_TYPE_WLAN_DTX和FRW_EVENT_TYPE_HOST_DRX，作用:在释放时区分是内存池的netbuf还是原生态的 */
    frw_event_type_enum_uint8           event_type;
    hi_u8                               event_sub_type;  /* amsdu抛事件用的 */
    hi_u8                               rsv[4];          /* 替换hal_tx_dscr_stru resv 4 byte */
    hi_u16                              us_eapol_ts;     /* eapol帧时间戳 */
    hi_u16                              us_mpdu_bytes;   /* mpdu字节数，维测用，不包括头尾，不包括snap，不包括padding */
    hi_u8                               rsv1[8];         /* 维测使用入TID队列时的时间戳 */

    hi_u32                              alg_pktno;       /* 算法用到的字段，唯一标示该报文 */
    hi_u16                              us_seqnum;       /* 记录软件分配的seqnum */
    wlan_tx_ack_policy_enum_uint8       ack_policy;      /* ACK 策略 */
    hi_u8                               tx_vap_index;

    hi_u8                               us_tx_user_idx;  /* dmac tx 到 tx complete 传递的user结构体，目标用户地址 */
    hi_u8                               retried_num;
    dmac_user_alg_probe_enum_uint8      is_probe_data;                   /* 是否是探测帧 */
    hi_u8                               mgmt_frame_id           : 4;     /* wpas 发送管理帧的frame id */
    hi_u8                               is_any_frame            : 1;
#ifdef _PRE_WLAN_FEATURE_MESH
    /* pbuf告知当前数据帧优先级的flags(用于ipv6包头压缩告知驱动优先级) */
    hi_u8                               pbuf_flags              : 2;
#endif
    hi_u8                               high_prio_sch           : 1;    /* 优先调度 */
} hmac_tx_ctl_stru;

hi_u32 dmac_tid_pause(dmac_tid_stru *tid, hi_u8 type);
hi_u32 dmac_from_hmac_rx_control_handle(frw_event_mem_stru *event_mem);
hi_u32 dmac_from_hmac_rx_data_handle(frw_event_mem_stru *event_mem, oal_dev_netbuf_stru *dev_netbuf, hi_u16 netbuf_len);
hi_u32 dmac_alg_get_qap_wme_info(const mac_vap_stru *mac_vap, hi_u8 wme_type, mac_wme_param_stru *wme_info);
hi_void dmac_set_rom_resv_func(dmac_rom_resv_func_enum_uint8 func_id, hi_void *func);
hi_void *dmac_get_rom_resv_func(dmac_rom_resv_func_enum_uint8 func_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of dmac_ext_if.h */
