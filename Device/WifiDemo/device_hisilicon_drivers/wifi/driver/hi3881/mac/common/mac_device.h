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

#ifndef __MAC_DEVICE_H__
#define __MAC_DEVICE_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "frw_timer.h"
#include "mac_vap.h"
#include "mac_mib.h"
#include "mac_cfg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define MAC_DATARATES_PHY_80211G_NUM        12
#define DMAC_BA_LUT_IDX_BMAP_LEN            ((HAL_MAX_BA_LUT_SIZE + 7) >> 3)

/* DMAC SCANNER 扫描模式 */
#define MAC_SCAN_FUNC_MEAS          0x1
#define MAC_SCAN_FUNC_STATS         0x2
#define MAC_SCAN_FUNC_RADAR         0x4
#define MAC_SCAN_FUNC_BSS           0x8
#define MAC_SCAN_FUNC_P2P_LISTEN    0x10
#define MAC_SCAN_FUNC_ALL           (MAC_SCAN_FUNC_MEAS | MAC_SCAN_FUNC_STATS | MAC_SCAN_FUNC_RADAR | MAC_SCAN_FUNC_BSS)

#define MAC_DEV_MAX_40M_INTOL_USER_BITMAP_LEN 4

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
#define MAC_MIB_MESH_VENDOR_SPECIFIC    255
#define MAC_MIB_NEIGHBOR_OFFSET_SYNC    1
#define MAC_MIB_AUTH_PROTOCOL_SAE       1
#define MAC_MESH_MULTI_MAC_ADDR_MAX_NUM 10
#endif

#define MAC_FCS_MAX_CHL_NUM    2
#define MAC_FCS_DEFAULT_PROTECT_TIMEOUT    2000    /* 第一次发保护帧超时时间,单位:us */
#define MAC_FCS_DEFAULT_PROTECT_TIMEOUT2   1000    /* 第二次发保护帧超时时间,单位:us */
#define MAC_DBAC_ONE_PACKET_TIMEOUT        1000    /* dbac保护帧超时时间,单位:10us */
#define MAC_ONE_PACKET_TIMEOUT             1000    /* 非dbac保护帧超时时间,单位:10us */
#define MAC_FCS_CTS_MAX_DURATION           32767   /* us */

#define MAX_PNO_SSID_COUNT          16
#define MAX_PNO_REPEAT_TIMES        4
#define PNO_SCHED_SCAN_INTERVAL     (30 * 1000)

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
typedef enum {
    MAC_CH_TYPE_NONE      = 0,
    MAC_CH_TYPE_PRIMARY   = 1,
    MAC_CH_TYPE_SECONDARY = 2,
} mac_ch_type_enum;
typedef hi_u8 mac_ch_type_enum_uint8;

typedef enum {
    MAC_SCAN_OP_INIT_SCAN,
    MAC_SCAN_OP_FG_SCAN_ONLY,
    MAC_SCAN_OP_BG_SCAN_ONLY,

    MAC_SCAN_OP_BUTT
} mac_scan_op_enum;
typedef hi_u8 mac_scan_op_enum_uint8;

typedef enum {
    MAC_CHAN_NOT_SUPPORT = 0,      /* 管制域不支持该信道 */
    MAC_CHAN_AVAILABLE_ALWAYS,     /* 信道一直可以使用 */
    MAC_CHAN_AVAILABLE_TO_OPERATE, /* 经过检测(CAC, etc...)后，该信道可以使用 */
    MAC_CHAN_DFS_REQUIRED,         /* 该信道需要进行雷达检测 */
    MAC_CHAN_BLOCK_DUE_TO_RADAR,   /* 由于检测到雷达导致该信道变的不可用 */

    MAC_CHAN_STATUS_BUTT
} mac_chan_status_enum;
typedef hi_u8 mac_chan_status_enum_uint8;

/* device reset同步子类型枚举 */
typedef enum {
    MAC_RESET_SWITCH_SET_TYPE,
    MAC_RESET_SWITCH_GET_TYPE,
    MAC_RESET_STATUS_GET_TYPE,
    MAC_RESET_STATUS_SET_TYPE,
    MAC_RESET_SWITCH_SYS_TYPE = MAC_RESET_SWITCH_SET_TYPE,
    MAC_RESET_STATUS_SYS_TYPE = MAC_RESET_STATUS_SET_TYPE,

    MAC_RESET_SYS_TYPE_BUTT
} mac_reset_sys_type_enum;
typedef hi_u8 mac_reset_sys_type_enum_uint8;

typedef enum {
    MAC_TRY_INIT_SCAN_VAP_UP,
    MAC_TRY_INIT_SCAN_SET_CHANNEL,
    MAC_TRY_INIT_SCAN_START_DBAC,
    MAC_TRY_INIT_SCAN_RESCAN,

    MAC_TRY_INIT_SCAN_BUTT
} mac_try_init_scan_type;
typedef hi_u8 mac_try_init_scan_type_enum_uint8;

typedef enum {
    MAC_INIT_SCAN_NOT_NEED,
    MAC_INIT_SCAN_NEED,
    MAC_INIT_SCAN_IN_SCAN,
} mac_need_init_scan_res;
typedef hi_u8 mac_need_init_scan_res_enum_uint8;

/* 扫描状态，通过判断当前扫描的状态，判断多个扫描请求的处理策略以及上报扫描结果的策略 */
typedef enum {
    MAC_SCAN_STATE_IDLE,
    MAC_SCAN_STATE_RUNNING,

    MAC_SCAN_STATE_BUTT
} mac_scan_state_enum;
typedef hi_u8 mac_scan_state_enum_uint8;

typedef enum {
    MAC_FCS_STATE_STANDBY = 0, // free to use
    MAC_FCS_STATE_REQUESTED,   // requested by other module, but not in switching
    MAC_FCS_STATE_IN_PROGESS,  // in switching

    MAC_FCS_STATE_BUTT
} mac_fcs_state_enum;
typedef hi_u8 mac_fcs_state_enum_uint8;

typedef enum {
    MAC_FCS_SUCCESS = 0,
    MAC_FCS_ERR_NULL_PTR,
    MAC_FCS_ERR_INVALID_CFG,
    MAC_FCS_ERR_BUSY,
    MAC_FCS_ERR_UNKNOWN_ERR,
} mac_fcs_err_enum;
typedef hi_u8 mac_fcs_err_enum_uint8;

typedef enum {
    MAC_ACS_RSN_INIT,
    MAC_ACS_RSN_LONG_TX_BUF,
    MAC_ACS_RSN_LARGE_PER,
    MAC_ACS_RSN_MWO_DECT,
    MAC_ACS_RSN_RADAR_DECT,

    MAC_ACS_RSN_BUTT
} mac_acs_rsn_enum;
typedef hi_u8 mac_acs_rsn_enum_uint8;

typedef enum {
    MAC_ACS_SW_NONE = 0x0,
    MAC_ACS_SW_INIT = 0x1,
    MAC_ACS_SW_DYNA = 0x2,
    MAC_ACS_SW_BOTH = 0x3,

    MAC_ACS_SW_BUTT
} mac_acs_sw_enum;
typedef hi_u8 en_mac_acs_sw_enum_uint8;

typedef enum {
    MAC_ACS_SET_CH_DNYA = 0x0,
    MAC_ACS_SET_CH_INIT = 0x1,

    MAC_ACS_SET_CH_BUTT
} mac_acs_set_ch_enum;
typedef hi_u8 en_mac_acs_set_ch_enum_uint8;

/* 扫描完成事件返回状态码 */
typedef enum {
    MAC_SCAN_SUCCESS = 0, /* 扫描成功 */
    MAC_SCAN_TIMEOUT = 2, /* 扫描超时 */
    MAC_SCAN_REFUSED = 3, /* 扫描被拒绝 */
    MAC_SCAN_STATUS_BUTT, /* 无效状态码，初始化时使用此状态码 */
} mac_scan_status_enum;
typedef hi_u8 mac_scan_status_enum_uint8;

/* ****************************************************************************
  STRUCT定义
**************************************************************************** */
typedef void (*mac_scan_cb_fn)(void *p_scan_record);

typedef struct {
    hi_u32 offset_addr;
    hi_u32 value[MAC_FCS_MAX_CHL_NUM];
} mac_fcs_reg_record_stru;

typedef struct tag_mac_fcs_mgr_stru {
    volatile hi_u8              fcs_done;
    mac_fcs_state_enum_uint8    fcs_state;
    hi_u8                       vap_id;    /* 目标信道对应的vap id,用于设置带宽 */
    hi_u8                       uc_resv;
} mac_fcs_mgr_stru;

/* device reset事件同步结构体 */
typedef struct {
    mac_reset_sys_type_enum_uint8   reset_sys_type;  /* 复位同步类型 */
    hi_u8                           value;           /* 同步信息值 */
    hi_u8                           uc_resv[2];      /* 2 byte预留字段 */
} mac_reset_sys_stru;

typedef struct {
    hi_u16                    us_num_networks;
    mac_ch_type_enum_uint8    ch_type;
    hi_u8                     auc_resv;
} mac_ap_ch_info_stru;

typedef struct {
    hi_u16    us_num_networks;    /* 记录当前信道下扫描到的BSS个数 */
    hi_u8     auc_resv[2];        /* 2 byte预留字段 */
    hi_u8     auc_bssid_array[WLAN_MAX_SCAN_BSS_PER_CH][WLAN_MAC_ADDR_LEN];  /* 记录当前信道下扫描到的所有BSSID */
} mac_bss_id_list_stru;

/* PNO扫描信息结构体 */
typedef struct {
    hi_s8           ac_match_ssid_set[MAX_PNO_SSID_COUNT][WLAN_SSID_MAX_LEN];
    hi_u8           auc_sour_mac_addr[WLAN_MAC_ADDR_LEN];   /* probe req帧中携带的发送端地址 */
    hi_u8           pno_scan_repeat;                     /* pno扫描重复次数 */
    hi_u8           is_random_mac_addr_scan;             /* 是否随机mac */
    hi_s32          l_ssid_count;                           /* 下发的需要匹配的ssid集的个数 */
    hi_s32          l_rssi_thold;                           /* 可上报的rssi门限 */
    hi_u32          pno_scan_interval;                   /* pno扫描间隔 */
    mac_scan_cb_fn  fn_cb;                               /* 函数指针必须放最后否则核间通信出问题 */
} mac_pno_scan_stru;

/* PNO调度扫描管理结构体 */
typedef struct {
    mac_pno_scan_stru       pno_sched_scan_params;             /* pno调度扫描请求的参数 */
    /* frw_timeout_stru      st_pno_sched_scan_timer;             pno调度扫描定时器 */
    /* pno调度扫描rtc时钟定时器，此定时器超时后，能够唤醒睡眠的device */
    hi_void                 *pno_sched_scan_timer;
    hi_u8                   curr_pno_sched_scan_times;         /* 当前pno调度扫描次数 */
    hi_u8                   is_found_match_ssid;               /* 是否扫描到了匹配的ssid */
    hi_u8                   auc_resv[2]; /* 2 byte预留字段 */
} mac_pno_sched_scan_mgmt_stru;

/* 扫描参数结构体 */
typedef struct {
    wlan_mib_desired_bsstype_enum_uint8 bss_type;            /* 要扫描的bss类型 */
    wlan_scan_type_enum_uint8           scan_type;           /* 主动/被动 */
    hi_u8                               auc_sour_mac_addr[WLAN_MAC_ADDR_LEN];    /* probe req帧中携带的发送端地址 */

    hi_u8                               max_scan_cnt_per_channel       : 4,   /* 每个信道的扫描次数 */
                                        max_send_probe_cnt_per_channel : 4;   /* 每次发送扫描请求帧的个数，默认为1 */
    hi_u8                               curr_channel_scan_count        : 4,   /* 记录当前信道的扫描次数 */
                                        is_p2p0_scan                  : 1,   /* 是否为p2p0 发起扫描 */
                                        working_in_home_chan           : 1,
                                        need_switch_back_home_channel  : 1,   /* 扫描完一个信道是否要切回工作信道 */
                                        is_random_mac_addr_scan        : 1;   /* 是否是随机mac addr扫描 */
    hi_u8                               auc_bssid[WLAN_SCAN_REQ_MAX_BSS][WLAN_MAC_ADDR_LEN];    /* 期望的bssid */

    hi_s8                               ac_ssid[WLAN_SCAN_REQ_MAX_BSS][WLAN_SSID_MAX_LEN];      /* 期望的ssid */
    hi_u8                               last_channel_band;
    hi_u8                               scan_func;                   /* DMAC SCANNER 扫描模式 */
    hi_u8                               p2p0_listen_channel;         /* 记录上层下发的p2 plisten channel */

    mac_channel_stru                    ast_channel_list[WLAN_MAX_CHANNEL_NUM];
    wlan_scan_mode_enum_uint8           scan_mode : 4;          /* 扫描模式:前景扫描 or 背景扫描 */
    hi_u8                               vap_id    : 4;          /* 下发扫描请求的vap id */
    hi_u8                               channel_nums;           /* 信道列表中信道的个数 */
    hi_u8                               channel_interval;
    hi_u16                              us_scan_time;           /* 扫描在某一信道停留此时间后，扫描结束, 10的整数倍ms */

    hi_u64                              ull_cookie;             /* P2P 监听下发的cookie 值 */
    hi_u32                              resv2;
    mac_scan_cb_fn                      fn_cb;                  /* 回调函数指针 */
} mac_scan_req_stru;

/* 打印接收报文的rssi信息的调试开关相关的结构体 */
typedef struct {
    hi_u32     rssi_debug_switch;        /* 打印接收报文的rssi信息的调试开关 */
    hi_u32     rx_comp_isr_interval;     /* 间隔多少个接收完成中断打印一次rssi信息 */
    hi_u32     curr_rx_comp_isr_count;   /* 一轮间隔内，接收完成中断的产生个数 */
} mac_rssi_debug_switch_stru;

/* ACS 命令及回复格式 */
typedef struct {
    hi_u8  cmd;
    hi_u8  chip_id;
    hi_u8  device_id;
    hi_u8  uc_resv;

    hi_u32 len;      /* 总长度，包括上面前4个字节 */
    hi_u32 cmd_cnt;  /* 命令的计数 */
} mac_acs_response_hdr_stru;

typedef struct {
    hi_u8                       sw_when_connected_enable : 1;
    hi_u8                       drop_dfs_channel_enable  : 1;
    hi_u8                       lte_coex_enable          : 1;
    en_mac_acs_sw_enum_uint8    acs_switch               : 5;
} mac_acs_switch_stru;

/* DMAC SCAN 信道扫描BSS信息摘要结构 */
typedef struct {
    hi_s8                               rssi;                     /* bss的信号强度 */
    hi_u8                               channel_number;          /* 信道号 */
    hi_u8                               auc_bssid[WLAN_MAC_ADDR_LEN];

    /* 11n, 11ac信息 */
    hi_u8                               ht_capable;             /* 是否支持ht */
    hi_u8                               vht_capable;            /* 是否支持vht */
    wlan_bw_cap_enum_uint8              bw_cap;                 /* 支持的带宽 0-20M 1-40M */
    wlan_channel_bandwidth_enum_uint8   channel_bandwidth;      /* 信道带宽配置 */
} mac_scan_bss_stats_stru;

/* DMAC SCAN 信道统计测量结果结构体 */
typedef struct {
    hi_u8   channel_number;      /* 信道号 */
    hi_u8   auc_resv[3];         /* 3 byte预留字段 */

    hi_u8   stats_cnt;           /* 信道繁忙度统计次数 */
    hi_u8   free_power_cnt;      /* 信道空闲功率 */
    hi_s16  s_free_power_stats_20_m;
    hi_s16  s_free_power_stats_40_m;
    hi_s16  s_free_power_stats_80_m;

    hi_u32  total_stats_time_us;
    hi_u32  total_free_time_20_m_us;
    hi_u32  total_free_time_40_m_us;
    hi_u32  total_free_time_80_m_us;
    hi_u32  total_send_time_us;
    hi_u32  total_recv_time_us;
} mac_scan_chan_stats_stru;

typedef struct {
    hi_s8                               rssi;                         /* bss的信号强度 */
    hi_u8                               channel_number;              /* 信道号 */

    hi_u8                               ht_capable   : 1;            /* 是否支持ht */
    hi_u8                               vht_capable  : 1;            /* 是否支持vht */
    wlan_bw_cap_enum_uint8              bw_cap       : 3;            /* 支持的带宽 0-20M 1-40M */
    wlan_channel_bandwidth_enum_uint8   channel_bandwidth : 3;       /* 信道带宽配置 */
} mac_scan_bss_stats_simple_stru;

/* DMAC SCAN 回调事件结构体 */
typedef struct {
    hi_u8                   nchans;      /* 信道数量       */
    hi_u8                   nbss;        /* BSS数量 */
    hi_u8                   scan_func;   /* 扫描启动的功能 */

    hi_u8                   need_rank    : 1; // kernel write, app read
    hi_u8                   obss_on      : 1;
    hi_u8                   dfs_on       : 1;
    hi_u8                   uc_resv      : 1;
    hi_u8                   chip_id      : 2;
    hi_u8                   device_id    : 2;
} mac_scan_event_stru;

/* bss安全相关信息结构体 */
typedef struct {
    hi_u8 bss_80211i_mode;                                  /* 指示当前AP的安全方式是WPA或WPA2。BIT0: WPA; BIT1:WPA2 */
    hi_u8 rsn_grp_policy;                                   /* 用于存放WPA2方式下，AP的组播加密套件信息 */
    hi_u8 auc_rsn_pairwise_policy[MAC_PAIRWISE_CIPHER_SUITES_NUM]; /* 用于存放WPA2方式下，AP的单播加密套件信息 */
    hi_u8 auc_rsn_auth_policy[MAC_AUTHENTICATION_SUITE_NUM];       /* 用于存放WPA2方式下，AP的认证套件信息 */
    hi_u8 auc_rsn_cap[2];                                   /* 2 byte 用于保存RSN能力信息，直接从帧内容中copy过来 */
    hi_u8 auc_wpa_pairwise_policy[MAC_PAIRWISE_CIPHER_SUITES_NUM]; /* 用于存放WPA方式下，AP的单播加密套件信息 */
    hi_u8 auc_wpa_auth_policy[MAC_AUTHENTICATION_SUITE_NUM];       /* 用于存放WPA方式下，AP的认证套件信息 */
    hi_u8 wpa_grp_policy;                                       /* 用于存放WPA方式下，AP的组播加密套件信息 */
    hi_u8 grp_policy_match;                                     /* 用于存放匹配的组播套件 */
    hi_u8 pairwise_policy_match;                                /* 用于存放匹配的单播套件 */
    hi_u8 auth_policy_match;                                    /* 用于存放匹配的认证套件 */
} mac_bss_80211i_info_stru;

/* 扫描结果 */
typedef struct {
    mac_scan_status_enum_uint8  scan_rsp_status;
    hi_u8                   auc_resv[7]; /* 7 byte预留字段 */
    hi_u64                  ull_cookie;
} mac_scan_rsp_stru;

/* 扫描到的BSS描述结构体 */
typedef struct {
    /* 基本信息 */
    wlan_mib_desired_bsstype_enum_uint8 bss_type;                    /* bss网络类型 */
    hi_u8                           dtim_period;                     /* dtime周期 */
    hi_u8                           dtim_cnt;                        /* dtime cnt */
    hi_u8                           ntxbf;                           /* 11n txbf */
    hi_u8                           new_scan_bss;                    /* 是否是新扫描到的BSS */
    hi_u8                           auc_resv1[1];
    hi_s8                           rssi;                             /* bss的信号强度 */
    hi_char                         ac_ssid[WLAN_SSID_MAX_LEN];         /* 网络ssid */
    hi_u16                          us_beacon_period;                   /* beacon周期 */
    hi_u16                          us_cap_info;                        /* 基本能力信息 */
    hi_u8                           auc_mac_addr[WLAN_MAC_ADDR_LEN];    /* 基础型网络 mac地址与bssid相同 */
    hi_u8                           auc_bssid[WLAN_MAC_ADDR_LEN];       /* 网络bssid */
    mac_channel_stru                channel;                         /* bss所在的信道 */
    hi_u8                           wmm_cap;                         /* 是否支持wmm */
    hi_u8                           uapsd_cap;                       /* 是否支持uapsd */
    hi_u8                           desired;                         /* 标志位，此bss是否是期望的 */
    hi_u8                           num_supp_rates;                  /* 支持的速率集个数 */
    hi_u8                           auc_supp_rates[WLAN_MAX_SUPP_RATES]; /* 支持的速率集 */
    hi_u8                           need_drop;                           /* 判断当前扫描结果是否需要过滤 */
    hi_u8                           auc_resv2[3];                        /* reserve 3byte */
#ifdef _PRE_WLAN_FEATURE_11D
    hi_char                         ac_country[WLAN_COUNTRY_STR_LEN];   /* 国家字符串 */
    hi_u8                           auc_resv3[1];
#endif
    hi_u8                           is_mesh_accepting_peer;          /* 是否支持Mesh连接 */
    hi_u8                           is_hisi_mesh;                    /* 是否是HISI-MESH节点 */

#ifdef _PRE_WLAN_FEATURE_ANY
    hi_u8                           supp_any;
    hi_u8                           is_any_sta;
#endif

    /* 安全相关的信息 */
    mac_bss_80211i_info_stru        bss_sec_info;                    /* 用于保存STA模式下，扫描到的AP安全相关信息 */

    /* 11n 11ac信息 */
    hi_u8                           ht_capable;                      /* 是否支持ht */
    hi_u8                           vht_capable;                     /* 是否支持vht */
    wlan_bw_cap_enum_uint8              bw_cap;                      /* 支持的带宽 0-20M 1-40M */
    wlan_channel_bandwidth_enum_uint8   channel_bandwidth;           /* 信道带宽 */
    hi_u8                           coex_mgmt_supp;                  /* 是否支持共存管理 */
    hi_u8                           ht_ldpc;                         /* 是否支持ldpc */
    hi_u8                           ht_stbc;                         /* 是否支持stbc */
    hi_u8                           wapi;
    hi_u32                          timestamp;                       /* 更新此bss的时间戳 */

    /* 管理帧信息 */
    hi_u32                          mgmt_len;                        /* 管理帧的长度 */
    hi_u8                           auc_mgmt_buff[4];                /* 4 byte 记录beacon帧或probe rsp帧 */
} mac_bss_dscr_stru;

typedef struct {
    hi_u32                          tx_seqnum;                        /* 最近一次tx上报的SN号 */
    hi_u16                          us_seqnum_used_times;                /* 软件使用了ul_tx_seqnum的次数 */
    hi_u16                          us_incr_constant;                    /* 维护非Qos 分片帧的递增常量 */
} mac_tx_seqnum_struc;

typedef struct {
    hi_u64                          ull_send_action_id;          /* P2P action id/cookie */
    hi_u64                          ull_last_roc_id;
    hi_u8                           p2p_device_num:4,            /* 当前device下的P2P_DEVICE数量 MAX 1 */
                                    p2p_goclient_num:4;          /* 当前device下的P2P_CL/P2P_GO数量 MAX 1 */
    hi_u8                           p2p0_vap_idx;
    /* P2P0/P2P_CL 共用VAP 结构，监听场景下保存VAP 进入监听前的状态 */
    mac_vap_state_enum_uint8        last_vap_state;
    hi_u8                           p2p_ps_pause;                /* P2P 节能是否处于pause状态 */
    oal_net_device_stru             *pst_p2p_net_device;
    oal_nl80211_channel_type_uint8  listen_channel_type;
    hi_u8                           resv[3];                     /* reserve 3byte */
    oal_ieee80211_channel_stru      st_listen_channel;
} mac_p2p_info_stru;

/* device结构体 */
typedef struct {
    /* device下的vap，此处只记录VAP ID */
    hi_u8                               auc_vap_id[WLAN_SERVICE_VAP_NUM_PER_DEVICE];
    hi_u8                               vap_num           : 3,             /* 当前device下的业务VAP数量(AP+STA) MAX 3 */
                                        sta_num           : 3,             /* 当前device下的STA数量 MAX 2 */
                                        reset_in_progress : 1,             /* 复位处理中 */
    /* 标识是否已经被分配，(HI_TRUE初始化完成，HI_FALSE未初始化 ) */
                                        device_state      : 1;
    /* 从eeprom或flash获得的mac地址，ko加载时调用hal接口赋值 */
    hi_u8                               auc_hw_addr[WLAN_MAC_ADDR_LEN];
    /* 已配置VAP的信道号，其后的VAP配置值不能与此值矛盾，仅在非DBAC时使用 */
    hi_u8                               max_channel;
    /* 已配置VAP的频段，其后的VAP配置值不能与此值矛盾，仅在非DBAC时使用 */
    wlan_channel_band_enum_uint8        max_band;

    /* 已配置VAP的最带带宽值，其后的VAP配置值不能与此值矛盾，仅在非DBAC时使用 */
    wlan_channel_bandwidth_enum_uint8   max_bandwidth;
    wlan_protocol_cap_enum_uint8        protocol_cap;                        /* 协议能力 */
    wlan_band_cap_enum_uint8            band_cap;                            /* 频段能力 */
    wlan_bw_cap_enum_uint8              bandwidth_cap;                       /* 带宽能力 */

    hi_u8                               wmm          : 1,                    /* wmm使能开关 */
                                        reset_switch : 1,                    /* 是否使能复位功能 */
                                        dbac_same_ch : 1,                    /* 是否同信道dbac */
                                        in_suspend   : 1,
                                        dbac_enabled : 1,
                                        dbac_running : 1,                    /* DBAC是否在运行 */
                                        dbac_has_vip_frame : 1,              /* 标记DBAC运行时收到了关键帧 */
                                        arpoffload_switch  : 1;
    hi_u8                               ldpc_coding : 1,                    /* 是否支持接收LDPC编码的包 */
                                        tx_stbc     : 1,                    /* 是否支持最少2x1 STBC发送 */
                                        rx_stbc     : 3,                    /* 是否支持stbc接收 */
                                        promis_switch : 1,                  /* 混杂模式开关 */
                                        mu_bfmee    : 1,                    /* 是否支持多用户beamformee */
                                        resv        : 1;
    hi_s16                              s_upc_amend;         /* UPC修正值 */
    hi_u32                              duty_ratio_lp;       /* 进入低功耗前发送占空比 */
    hi_u32                              rx_nondir_duty_lp;   /* 进入低功耗前接收non-direct包的占空比 */
    hi_u32                              rx_dir_duty_lp;      /* 进入低功耗前接收direct包的占空比 */
    hi_u32                              beacon_interval;     /* device级别beacon interval,device下所有VAP约束为同一值 */
    hi_u32                              duty_ratio;          /* 占空比统计 */

    mac_data_rate_stru                  mac_rates_11g[MAC_DATARATES_PHY_80211G_NUM];  /* 11g速率 */
    mac_pno_sched_scan_mgmt_stru       *pno_sched_scan_mgmt; /* pno调度扫描管理结构体指针，内存动态申请，从而节省内存 */
    mac_scan_req_stru                   scan_params;         /* 最新一次的扫描参数信息 */
    frw_timeout_stru                    scan_timer;          /* 扫描定时器用于切换信道 */
    frw_timeout_stru                    obss_scan_timer;     /* obss扫描定时器，循环定时器 */
    mac_channel_stru                    p2p_vap_channel;     /* p2p listen时记录p2p的信道，用于p2p listen结束后恢复 */
    frw_timeout_stru                    go_scan_timer;
    frw_timeout_stru                    keepalive_timer;                    /* keepalive定时器 */

    hi_u8                               tid;
    hi_u8                               asoc_user_cnt;                      /* 关联用户数 */
    hi_u8                               wapi;
    hi_u8                               sar_pwr_limit;                      /* CE认证SAR标准最大功率限制 */
    /* linux内核中的device物理信息 */
    /* 用于存放和VAP相关的wiphy设备信息，在AP/STA模式下均要使用；可以多个VAP对应一个wiphy */
    oal_wiphy_stru                     *wiphy;

    /* 针对Device的成员，可移动到dmac_device OFFLOAD模式下可用DEVICE包含 */
    hi_u8                               scan_chan_idx;                       /* 当前扫描信道索引 */
    /* 当前扫描状态，根据此状态处理obss扫描和host侧下发的扫描请求，以及扫描结果的上报处理 */
    mac_scan_state_enum_uint8           curr_scan_state;
    hi_u8                               auc_original_mac_addr[WLAN_MAC_ADDR_LEN]; /* 扫描开始前保存原始的MAC地址 */
    mac_channel_stru                    home_channel;                        /* 记录工作信道 供切回时使用 */
    mac_scan_chan_stats_stru            chan_result;                         /* dmac扫描时 一个信道的信道测量记录 */
    mac_fcs_mgr_stru                    fcs_mgr;
    mac_p2p_info_stru                   p2p_info;                           /* P2P 相关信息 */
    /* 每个AP发送一次CSA帧，该计数加1。AP切换完信道后，该计数清零 */
    hi_u8                               csa_cnt;
    hi_u8                               txop_enable;                        /* 发送无聚合时采用TXOP模式 */
    hi_u8                               tx_ba_num;                          /* 发送方向BA会话个数 */
    hi_u8                               nss_num;
    hi_u32                              auc_resv;                          /* 预留字段 */
} mac_device_stru;

#pragma pack(push, 1)
/* 上报的扫描结果的扩展信息，放在上报host侧的管理帧netbuf的后面 */
typedef struct {
    hi_s32                           l_rssi;                    /* 信号强度 */
    wlan_mib_desired_bsstype_enum_uint8 bss_type;               /* 扫描到的bss类型 */
    hi_u8                           channel;                    /* 当前扫描的信道 */
    hi_u8                           auc_resv[2];                /* 2 byte预留字段 */
} mac_scanned_result_extend_info_stru;
#pragma pack(pop)

typedef struct {
    mac_device_stru *mac_device;
} mac_wiphy_priv_stru;

/* ****************************************************************************
  内联函数
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : mac_is_dbac_running
 功能描述  : 判断对应device上dbac功能是否在运行
 输入参数  : mac_device_stru *pst_device
 返 回 值  : static  inline  hi_u8
 修改历史      :
  1.日    期   : 2014年7月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_is_dbac_running(const mac_device_stru *mac_dev)
{
    return mac_dev->dbac_enabled && mac_dev->dbac_running;
}

/* ****************************************************************************
 功能描述  : 数据帧是否需要入tid队列
 输入参数  : mac_device_stru *pst_device
             mac_vap_stru *pst_vap
 修改历史      :
  1.日    期   : 2014年7月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_need_enqueue_tid_for_dbac(const mac_device_stru *mac_dev, const mac_vap_stru *mac_vap)
{
    return (mac_dev->dbac_enabled && (mac_vap->vap_state == MAC_VAP_STATE_PAUSE));
}

static inline hi_u8 mac_device_is_scaning(const mac_device_stru *mac_dev)
{
    return (mac_dev->curr_scan_state == MAC_SCAN_STATE_RUNNING);
}

static inline hi_u8 mac_device_is_listening(const mac_device_stru *mac_dev)
{
    return ((mac_dev->curr_scan_state == MAC_SCAN_STATE_RUNNING) &&
        (mac_dev->scan_params.scan_func & MAC_SCAN_FUNC_P2P_LISTEN));
}

static inline hi_void mac_device_set_channel(mac_device_stru *mac_dev, const mac_cfg_channel_param_stru *channel_param)
{
    mac_dev->max_channel = channel_param->channel;
    mac_dev->max_band = channel_param->band;
    mac_dev->max_bandwidth = channel_param->en_bandwidth;
}

static inline hi_void mac_device_get_channel(const mac_device_stru *mac_dev, mac_cfg_channel_param_stru *channel_param)
{
    channel_param->channel = mac_dev->max_channel;
    channel_param->band = mac_dev->max_band;
    channel_param->en_bandwidth = mac_dev->max_bandwidth;
}

/* ****************************************************************************
   函数声明
**************************************************************************** */
mac_device_stru *mac_res_get_dev(hi_void);
const hi_u8 *mac_get_mac_bcast_addr(hi_void);
hi_void mac_device_init(mac_device_stru *mac_dev);
hi_void mac_device_set_vap_id(mac_device_stru *mac_dev, mac_vap_stru *mac_vap,
                              const mac_cfg_add_vap_param_stru *param, hi_u8 vap_idx, hi_u8 is_add_vap);
hi_void mac_device_find_up_vap(const mac_device_stru *mac_dev, mac_vap_stru **mac_vap);
hi_u8 mac_device_has_other_up_vap(const mac_device_stru *mac_dev, const mac_vap_stru *mac_vap_ref);
hi_u32 mac_device_calc_up_vap_num(const mac_device_stru *mac_dev);
hi_u32 mac_device_find_up_vap_with_mode(const mac_device_stru *mac_dev, mac_vap_stru **mac_vap,
    wlan_vap_mode_enum_uint8 vap_mode);
hi_u32 mac_device_find_up_p2p_go(const mac_device_stru *mac_dev, mac_vap_stru **mac_vap);
hi_u32 mac_device_find_2up_vap(const mac_device_stru *mac_dev, mac_vap_stru **mac_vap1, mac_vap_stru **mac_vap2);
hi_u32 mac_device_is_p2p_connected(const mac_device_stru *mac_dev);

hi_void mac_fcs_release(mac_fcs_mgr_stru *fcs_mgr);
hi_u8 mac_fcs_get_protect_cnt(const mac_vap_stru *mac_vap);
hi_u32 mac_fcs_get_prot_datarate(const mac_vap_stru *mac_vap);
hi_u8 mac_fcs_get_fake_q_id(const mac_vap_stru *mac_vap);

hi_void mac_fcs_init(mac_fcs_mgr_stru *fcs_mgr);
mac_fcs_err_enum_uint8 mac_fcs_request(mac_fcs_mgr_stru *fcs_mgr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MAC_DEVICE_H__ */
