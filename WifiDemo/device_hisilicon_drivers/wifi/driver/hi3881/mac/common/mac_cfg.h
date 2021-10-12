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

#ifndef __MAC_CFG_H__
#define __MAC_CFG_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "oam_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  宏定义
*****************************************************************************/
#define MAC_NUM_2G_BAND             3           /* 2g band个数 */
#define MAC_NUM_2G_CH_NUM           13          /* 2g 信道个数 */
/*****************************************************************************
  枚举定义
**************************************************************************** */
typedef enum {
    MAC_CFG_RETRY_DATA      = 0,
    MAC_CFG_RETRY_MGMT      = 1,
    MAC_CFG_RETRY_TIMEOUT   = 2,
    MAC_CFG_RETRY_TYPE_BUTT,
} mac_cfg_retry_type_enum;
typedef hi_u8 mac_cfg_retry_type_enum_uint8;

typedef enum {
    MAC_PSM_OFFSET_TBTT,
    MAC_PSM_OFFSET_EXT_TBTT,
    MAC_PSM_OFFSET_BCN_TIMEOUT,
#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
    MAC_PSM_FREE_ARP_INTERVAL
#endif
} mac_cfg_psm_offset_type;
typedef hi_u8 mac_cfg_psm_offset_type_uint8;

typedef enum {
    MAC_STA_PM_SWITCH_OFF         = 0,        /* 关闭低功耗 */
    MAC_STA_PM_SWITCH_ON          = 1,        /* 打开低功耗 */
    MAC_STA_PM_MANUAL_MODE_ON     = 2,        /* 开启手动sta pm mode */
    MAC_STA_PM_MANUAL_MODE_OFF    = 3,        /* 关闭手动sta pm mode */
    MAC_STA_PM_SWITCH_BUTT,                   /* 最大类型 */
} mac_pm_switch_enum;
typedef hi_u8 mac_pm_switch_enum_uint8;

typedef enum {
    MAC_STA_PM_CTRL_TYPE_HOST    = 0,        /* 低功耗控制类型 HOST */
    MAC_STA_PM_CTRL_TYPE_DBAC    = 1,        /* 低功耗控制类型 DBAC */
    MAC_STA_PM_CTRL_TYPE_MONITOR = 2,        /* 低功耗控制类型 MONITOR */
    MAC_STA_PM_CTRL_TYPE_CSI     = 3,        /* 低功耗控制类型 CSI */
    MAC_STA_PM_CTRL_TYPE_ANY     = 4,        /* 低功耗控制类型 ANY */
    MAC_STA_PM_CTRL_TYPE_FLOW    = 5,        /* 低功耗控制类型 FLOW */
    MAC_STA_PM_CTRL_TYPE_BTCOEX  = 6,        /* 低功耗控制类型 BTCOEX */
    MAC_STA_PM_CTRL_TYPE_BUTT,               /* 最大类型，应小于8 */
} mac_pm_ctrl_type_enum;
typedef hi_u8 mac_pm_ctrl_type_enum_uint8;

/* *
 * enum nl80211_mfp - Management frame protection state
 * @NL80211_MFP_NO: Management frame protection not used
 * @NL80211_MFP_REQUIRED: Management frame protection required
 */
typedef enum {
    MAC_NL80211_MFP_NO,
    MAC_NL80211_MFP_REQUIRED,

    MAC_NL80211_MFP_BUTT
} mac_nl80211_mfp_enum;
typedef hi_u8 mac_nl80211_mfp_enum_uint8;

/* 配置发送描述符内部元素结构体 */
typedef enum {
    RF_PAYLOAD_ALL_ZERO = 0,
    RF_PAYLOAD_ALL_ONE,
    RF_PAYLOAD_RAND,
    RF_PAYLOAD_BUTT
} mac_rf_payload_enum;
typedef hi_u8 mac_rf_payload_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_MESH
typedef enum {
    HISI_PLINK_IDLE = 1,
    HISI_PLINK_OPN_SNT,
    HISI_PLINK_OPN_RCVD,
    HISI_PLINK_CNF_RCVD,
    HISI_PLINK_ESTAB,
    HISI_PLINK_HOLDING,
    HISI_PLINK_BLOCKED,

    HISI_PLINK_BUTT
} mac_mesh_plink_state_enum;
typedef hi_u8 mac_mesh_plink_state_enum_uint8;
#endif

/* 设置发送描述符帧类型枚举 */
typedef enum {
    MAC_SET_DSCR_TYPE_UCAST_DATA  = 0,  /* 单播数据帧 */
    MAC_SET_DSCR_TYPE_MCAST_DATA,       /* 组播数据帧 */
    MAC_SET_DSCR_TYPE_BCAST_DATA,       /* 广播数据帧 */
    MAC_SET_DSCR_TYPE_UCAST_MGMT_2G,    /* 单播管理帧,仅2G */
    MAC_SET_DSCR_TYPE_MBCAST_MGMT_2G,   /* 组、广播管理帧,仅2G */

    MAC_SET_DSCR_TYPE_BUTT,
} mac_set_dscr_frame_type_enum;
typedef hi_u8 mac_set_dscr_frame_type_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_MESH
/* mesh节点类型枚举 */
typedef enum {
    MAC_HISI_MESH_UNSPEC = 0, /* 未确定mesh节点角色 */
    MAC_HISI_MESH_STA,        /* Mesh-STA节点角色 */
    MAC_HISI_MESH_MG,         /* Mesh-MG节点角色 */
    MAC_HISI_MESH_MBR,        /* Mesh-MBR节点角色 */

    MAC_HISI_MESH_NODE_BUTT,
} mac_hisi_mesh_node_type_enum;
typedef hi_u8 mac_hisi_mesh_node_type_enum_uint8;
#endif

typedef enum {
    WLAN_11B_PHY_PROTOCOL_MODE              = 0,   /* 11b CCK */
    WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE      = 1,   /* 11g/a OFDM */
    WLAN_HT_PHY_PROTOCOL_MODE               = 2,   /* 11n HT */
    WLAN_VHT_PHY_PROTOCOL_MODE              = 3,   /* 11ac VHT */
    WLAN_PHY_PROTOCOL_BUTT
} wlan_phy_protocol_enum;
typedef hi_u8 wlan_phy_protocol_enum_uint8;

/* ****************************************************************************
  结构体定义
**************************************************************************** */
typedef struct {
    hi_u8                            param;      /* 查询或配置信息 */
    wlan_phy_protocol_enum_uint8     protocol_mode;
    mac_rf_payload_enum_uint8        payload_flag;
    wlan_tx_ack_policy_enum_uint8    ack_policy;
    hi_u32                           payload_len;
} mac_cfg_tx_comp_stru;

typedef struct {
    hi_u8  offset_addr_a;
    hi_u8  offset_addr_b;
    hi_u16 us_delta_gain;
} mac_cfg_dbb_scaling_stru;

/* wfa edca参数配置 */
typedef struct {
    hi_u8  switch_code; /* 开关 */
    hi_u8  ac;          /* AC */
    hi_u16 us_val;     /* 数据 */
} mac_edca_cfg_stru;

/* PPM调整命令格式 */
typedef struct {
    hi_s8 ppm_val;    /* PPM差值 */
    hi_u8 clock_freq; /* 时钟频率 */
    hi_u8 uc_resv[1];
} mac_cfg_adjust_ppm_stru;

/* 管制域配置命令结构体 */
typedef struct {
    hi_void *mac_regdom;
} mac_cfg_country_stru;

/* 管制域最大发送功率配置 */
typedef struct {
    hi_u8 pwr;
    hi_u8 exceed_reg;
    hi_u8 auc_resv[2]; /* 2 byte保留字段，保证word对齐 */
} mac_cfg_regdomain_max_pwr_stru;

/* 获取当前管制域国家码字符配置命令结构体 */
typedef struct {
    hi_char ac_country[WLAN_COUNTRY_STR_LEN];
    hi_u8   auc_resv[1];
} mac_cfg_get_country_stru;

/* query消息格式:2字节WID x N */
typedef struct {
    hi_u8 tid;
    hi_u8 uc_resv[3]; /* 3 byte保留字段，保证word对齐 */
} mac_cfg_get_tid_stru;

/* 定制化 linkloss门限配置参数 */
typedef struct {
    hi_u8 linkloss_threshold_wlan_near;
    hi_u8 linkloss_threshold_wlan_far;
    hi_u8 linkloss_threshold_p2p;
    hi_u8 auc_resv[1];
} mac_cfg_linkloss_threshold;

/* 定制化 power ref 2g 5g配置参数 */
typedef struct {
    hi_u32 power_ref_2g;
} mac_cfg_power_ref;

/* customize rf cfg struct */
typedef struct {
    hi_s8 rf_gain_db_2g_mult4;  /* 外部PA/LNA bypass时的增益(0.25dB) */
    hi_s8 rf_gain_db_2g_mult10; /* 外部PA/LNA bypass时的增益(0.1dB) */
} mac_cfg_gain_db_2g_band;

typedef struct {
    mac_cfg_gain_db_2g_band ac_gain_db_2g[MAC_NUM_2G_BAND];
} mac_cfg_customize_rf;

#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct _mac_cfg_set_mesh_user_param_stru {
    hi_u8 auc_addr[WLAN_MAC_ADDR_LEN];
    mac_mesh_plink_state_enum_uint8 plink_sta;
    hi_u8 set;
    hi_u8 bcn_prio;
    hi_u8 is_mbr;
    hi_u8 mesh_initiative_peering;
} mac_cfg_set_mesh_user_param_stru;

typedef struct _mac_cfg_set_multi_mac_addr_stru {
    hi_u8 auc_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 set; /* 0 - 删除某多播地址、1 - 设置某多播地址 */
    hi_u8 rsv;
} mac_cfg_set_multi_mac_addr_stru;

typedef mac_cfg_set_multi_mac_addr_stru mac_cfg_unset_multi_mac_addr_stru;

typedef struct _mac_cfg_auto_peer_params_stru {
    hi_s8 rssi_low;    /* 用户数少的时候用的rssi门限 */
    hi_s8 rssi_middle; /* 用户数适中的时候用的rssi门限 */
    hi_s8 rssi_high;   /* 用户数多的时候用的rssi门限 */
} mac_cfg_auto_peer_params_stru;

typedef struct _mac_cfg_mesh_nodeinfo_stru {
    mac_hisi_mesh_node_type_enum_uint8 node_type; /* 本节点角色 */
    hi_u8 mesh_accept_sta;                        /* 是否接受sta关联 */
    hi_u8 user_num;                               /* 关联用户数 */
    hi_u8 privacy;                                /* 是否加密 */
    hi_u8 chan;                                   /* 信道号 */
    hi_u8 priority;                               /* bcn优先级 */
    hi_u8 rsv[2];                                 /* 2 byte保留 */
} mac_cfg_mesh_nodeinfo_stru;
#endif

/* 接收帧的FCS统计信息 */
typedef struct {
    hi_u32 data_op; /* 数据操作模式:<0>保留,<1>清除 */
    hi_u32 print_info; /* 打印数据内容:<0>所有数据 <1>总帧数 <2>self fcs correct, <3>other fcs correct, <4>fcs error */
} mac_cfg_rx_fcs_info_stru;

/* 剔除用户配置命令参数 */
typedef struct {
    hi_u8  auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC地址 */
    hi_u16 us_reason_code;                 /* 去关联 reason code */
} mac_cfg_kick_user_param_stru;

/* 暂停tid配置命令参数 */
typedef struct {
    hi_u8 auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC地址 */
    hi_u8 tid;
    hi_u8 is_paused;
} mac_cfg_pause_tid_param_stru;

/* 配置用户是否为vip */
typedef struct {
    hi_u8 auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC地址 */
    hi_u8 vip_flag;
} mac_cfg_user_vip_param_stru;

/* 暂停tid配置命令参数 */
typedef struct {
    hi_u8 aggr_tx_on;
    hi_u8 tid;
    hi_u8 max_num;
    hi_u8 resv;
} mac_cfg_ampdu_tx_on_param_stru;

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
/* 设置host某个队列的每次调度报文个数，low_waterline, high_waterline */
typedef struct {
    hi_u8  queue_type;
    hi_u8  auc_resv[1];
    hi_u16 us_burst_limit;
    hi_u16 us_low_waterline;
    hi_u16 us_high_waterline;
} mac_cfg_flowctl_param_stru;
#endif

/* 使能qempty命令 */
typedef struct {
    hi_u8 is_on;
    hi_u8 auc_resv[3]; /* 3 byte保留字段，保证word对齐 */
} mac_cfg_resume_qempty_stru;

/* 发送mpdu/ampdu命令参数  */
typedef struct {
    hi_u8  tid;
    hi_u8  packet_num;
    hi_u16 us_packet_len;
    hi_u8  auc_ra_mac[OAL_MAC_ADDR_LEN];
} mac_cfg_mpdu_ampdu_tx_param_stru;
/* AMPDU相关的配置命令参数 */
typedef struct {
    hi_u8 auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* 用户的MAC ADDR */
    hi_u8 tidno;                           /* 对应的tid号 */
    hi_u8 auc_reserve[1];                  /* 确认策略 */
} mac_cfg_ampdu_start_param_stru;

typedef mac_cfg_ampdu_start_param_stru mac_cfg_ampdu_end_param_stru;

/* BA会话相关的配置命令参数 */
typedef struct {
    hi_u8                       auc_mac_addr[WLAN_MAC_ADDR_LEN];    /* 用户的MAC ADDR */
    hi_u8                       tidno;                              /* 对应的tid号 */
    mac_ba_policy_enum_uint8    ba_policy;                          /* BA确认策略 */
    hi_u16                      us_buff_size;                       /* BA窗口的大小 */
    hi_u16                      us_timeout;                         /* BA会话的超时时间 */
} mac_cfg_addba_req_param_stru;

typedef struct {
    hi_u8                          auc_mac_addr[WLAN_MAC_ADDR_LEN];    /* 用户的MAC ADDR */
    hi_u8                          tidno;                              /* 对应的tid号 */
    mac_delba_initiator_enum_uint8 direction;                          /* 删除ba会话的发起端 */
    hi_u8                          auc_reserve[1];                     /* 删除原因 */
} mac_cfg_delba_req_param_stru;

typedef struct {
    hi_u8 auc_mac_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 amsdu_max_num; /* amsdu最大个数 */
    hi_u8 auc_reserve;
} mac_cfg_amsdu_start_param_stru;

/* 设置用户配置参数 */
typedef struct {
    hi_u8                               function_index;
    hi_u8                               auc_reserve[2]; /* 2 byte保留字段，保证word对齐 */
    mac_set_dscr_frame_type_enum_uint8  type;           /* 配置的帧类型 */
    hi_s32                              l_value;
} mac_cfg_set_dscr_param_stru;

/* non-HT协议模式下速率配置结构体 */
typedef struct {
    wlan_legacy_rate_value_enum_uint8       rate;            /* 速率值 */
    wlan_phy_protocol_enum_uint8            protocol_mode;   /* 对应的协议 */
    hi_u8                                   auc_reserve[2];  /* 2 byte保留 */
} mac_cfg_non_ht_rate_stru;

/* 用户相关的配置命令参数 */
typedef struct {
    hi_u8 auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC地址 */
    hi_u8 ht_cap;                          /* ht能力 */
    hi_u8 user_idx;                        /* 用户索引 */
} mac_cfg_add_user_param_stru;
typedef mac_cfg_add_user_param_stru mac_cfg_del_user_param_stru;

/* 定制化 时钟配置参数 */
typedef struct {
    hi_u32 rtc_clk_freq;
    hi_u8  clk_type;
    hi_u8  auc_resv[3]; /* 3 byte保留字段，保证word对齐 */
} mac_cfg_pm_param;

typedef struct {
    mac_cfg_psm_offset_type_uint8 type;
    hi_u8  resv;
    hi_u16 value;
} mac_cfg_psm_offset;

typedef struct {
    hi_u16 us_tx_ratio;        /* tx占空比 */
    hi_u16 us_tx_pwr_comp_val; /* 发射功率补偿值 */
} mac_tx_ratio_vs_pwr_stru;

/* 定制化TX占空比&温度补偿发射功率的参数 */
typedef struct {
    mac_tx_ratio_vs_pwr_stru ast_txratio2pwr[3]; /* 3档占空比分别对应功率补偿值 */
    hi_u32 more_pwr;                             /* 根据温度额外补偿的发射功率 */
} mac_cfg_customize_tx_pwr_comp_stru;

typedef struct {
    mac_cfg_retry_type_enum_uint8       type;
    hi_u8                               limit;
    hi_u8                               auc_rsv[2]; /* 2 byte保留字段，保证word对齐 */
} mac_cfg_retry_param_stru;

typedef struct {
    hi_u8 auc_mac_da[WLAN_MAC_ADDR_LEN];
    hi_u8 category;
    hi_u8 auc_resv[1];
} mac_cfg_send_action_param_stru;

typedef struct {
    hi_s32 l_is_psm; /* 是否进入节能 */
    hi_s32 l_is_qos; /* 是否发qosnull */
    hi_s32 l_tidno;  /* tid号 */
} mac_cfg_tx_nulldata_stru;

/* 获取mpdu数目需要的参数 */
typedef struct {
    hi_u8 auc_user_macaddr[WLAN_MAC_ADDR_LEN];
    hi_u8 auc_resv[2]; /* 2 byte保留字段，保证word对齐 */
} mac_cfg_get_mpdu_num_stru;

#ifdef _PRE_DEBUG_MODE
typedef struct {
    hi_u8 auc_user_macaddr[WLAN_MAC_ADDR_LEN];
    hi_u8 param;
    hi_u8 tid_no;
} mac_cfg_ampdu_stat_stru;
#endif

typedef struct {
    hi_u8 aggr_num_switch; /* 控制聚合个数开关 */
    hi_u8 aggr_num;        /* 聚合个数 */
    hi_u8 auc_resv[2];     /* 2 byte保留字段，保证word对齐 */
} mac_cfg_aggr_num_stru;

typedef struct {
    hi_u32 mib_idx;
    hi_u32 mib_value;
} mac_cfg_set_mib_stru;

typedef struct {
    hi_u8 bypass_type;
    hi_u8 value;
    hi_u8 auc_resv[2]; /* 2 byte保留字段，保证word对齐 */
} mac_cfg_set_thruput_bypass_stru;

typedef struct {
    hi_u8 performance_log_switch_type;
    hi_u8 value;
    hi_u8 auc_resv[2]; /* 2 byte保留字段，保证word对齐 */
} mac_cfg_set_performance_log_switch_stru;

typedef struct {
    hi_u32 timeout;
    hi_u8  is_period;
    hi_u8  stop_start;
    hi_u8  auc_resv[2]; /* 2 byte保留字段，保证word对齐 */
} mac_cfg_test_timer_stru;

typedef struct {
    hi_u8  user_idx;
    hi_u8  uc_resv;
    hi_u16 us_rx_pn;
} mac_cfg_set_rx_pn_stru;

typedef struct {
    hi_u32 frag_threshold;
} mac_cfg_frag_threshold_stru;

typedef struct {
    hi_u32 rts_threshold;
} mac_cfg_rts_threshold_stru;

typedef struct {
    /* software_retry值 */
    hi_u8 software_retry;
    /* 是否取test设置的值，为0则为正常流程所设 */
    hi_u8 retry_test;
    hi_u8 resv[2]; /* 2 byte保留字段，保证word对齐 */
} mac_cfg_set_soft_retry_stru;

/* STA PS 发送参数 */
#ifdef _PRE_WLAN_FEATURE_STA_PM
typedef struct {
    hi_u8 vap_ps_mode;
} mac_cfg_ps_mode_param_stru;

typedef struct {
    hi_u16 beacon_timeout;
    hi_u16 tbtt_offset;
    hi_u16 ext_tbtt_offset;
} mac_cfg_ps_param_stru;
#endif

typedef struct {
    hi_u8 show_ip_addr : 4;         /* show ip addr */
    hi_u8 show_arpoffload_info : 4; /* show arpoffload 维测 */
} mac_cfg_arpoffload_info_stru;

typedef struct {
    hi_u8 in_suspend;        /* 亮暗屏 */
    hi_u8 arpoffload_switch; /* arpoffload开关 */
} mac_cfg_suspend_stru;

typedef struct {
    mac_pm_ctrl_type_enum_uint8 pm_ctrl_type; /* mac_pm_ctrl_type_enum */
    mac_pm_switch_enum_uint8 pm_enable;       /* mac_pm_switch_enum */
} mac_cfg_ps_open_stru;

/* ======================== cfg id对应的参数结构体 ==================================== */
/* 创建VAP参数结构体, 对应cfgid: WLAN_CFGID_ADD_VAP */
typedef struct {
    wlan_vap_mode_enum_uint8 vap_mode;
    hi_u8 cfg_vap_indx;
    hi_u8 muti_user_id; /* 添加vap 对应的muti user index */
    hi_u8 vap_id;       /* 需要添加的vap id */

    wlan_p2p_mode_enum_uint8 p2p_mode; /* 0:非P2P设备; 1:P2P_GO; 2:P2P_Device; 3:P2P_CL */
    hi_u8                 ac2g_enable           :1,
                          disable_capab_2ght40  :1,
                          uapsd_enable    :1,
                          reserve1        :5;
    hi_u8                 auc_resv[2]; /* 2 byte保留字段，保证word对齐 */
    oal_net_device_stru      *net_dev;
} mac_cfg_add_vap_param_stru;
typedef mac_cfg_add_vap_param_stru mac_cfg_del_vap_param_stru;

/* 协议参数 对应cfgid: WLAN_CFGID_MODE */
typedef struct {
    wlan_protocol_enum_uint8            protocol;       /* 协议 */
    wlan_channel_band_enum_uint8        band;           /* 频带 */
    wlan_channel_bandwidth_enum_uint8   en_bandwidth;   /* 带宽 */
    hi_u8                               channel_idx;    /* 主20M信道号 */
} mac_cfg_mode_param_stru;

/* 启用VAP参数结构体 对应cfgid: WLAN_CFGID_START_VAP */
typedef struct {
    hi_u8 mgmt_rate_init_flag; /* start vap时候，管理帧速率是否需要初始化 */
    hi_u8 protocol;
    hi_u8 band;
    hi_u8 uc_bandwidth;
#ifdef _PRE_WLAN_FEATURE_P2P_ROM
    wlan_p2p_mode_enum_uint8 p2p_mode;
    hi_u8                    auc_resv2[3]; /* 3 byte保留字段，保证word对齐 */
#endif
    oal_net_device_stru *net_dev;
} mac_cfg_start_vap_param_stru;
typedef mac_cfg_start_vap_param_stru mac_cfg_down_vap_param_stru;

/* 设置mac地址参数 对应cfgid: WLAN_CFGID_STATION_ID */
typedef struct {
    hi_u8                   auc_station_id[WLAN_MAC_ADDR_LEN];
    wlan_p2p_mode_enum_uint8    p2p_mode;
    hi_u8                   auc_resv[1];
} mac_cfg_staion_id_param_stru;

/* SSID参数 对应cfgid: WLAN_CFGID_SSID */
typedef struct {
    hi_u8   ssid_len;
    hi_u8   auc_resv[2]; /* 2 byte保留字段，保证word对齐 */
    hi_char ac_ssid[WLAN_SSID_MAX_LEN];
} mac_cfg_ssid_param_stru;

/* HOSTAPD 设置工作频段，信道和带宽参数 */
typedef struct {
    wlan_channel_band_enum_uint8 band;              /* 频带 */
    wlan_channel_bandwidth_enum_uint8 en_bandwidth; /* 带宽 */
    hi_u8 channel;                                  /* 信道编号 */
    hi_u8 rsv;
} mac_cfg_channel_param_stru;

/* HOSTAPD 设置wiphy 物理设备信息，包括RTS 门限值，分片报文门限值 */
typedef struct {
    hi_u8 frag_threshold_changed;
    hi_u8 rts_threshold_changed;
    hi_u8 rsv[2]; /* 2 byte保留字段，保证word对齐 */
    hi_u32 frag_threshold;
    hi_u32 rts_threshold;
} mac_cfg_wiphy_param_stru;

/* P2P OPS 节能配置参数 */
typedef struct {
    hi_s32 ops_ctrl;
    hi_u8 ct_window;
    hi_u8 resv[3]; /* reserve 3byte */
    hi_s32 pause_ops;
} mac_cfg_p2p_ops_param_stru;

/* P2P NOA节能配置参数 */
typedef struct {
    hi_u32 start_time;
    hi_u32 duration;
    hi_u32 interval;
    hi_u8  count;
    hi_u8  resv[3]; /* 3 byte保留字段，保证word对齐 */
} mac_cfg_p2p_noa_param_stru;

/* P2P 节能控制命令 */
typedef struct {
    hi_u8 p2p_statistics_ctrl; /* 0:清除P2P 统计值； 1:打印输出统计值 */
    hi_u8 auc_rsv[3];          /* 3 byte保留字段，保证word对齐 */
} mac_cfg_p2p_stat_param_stru;

/* 组播转单播 发送参数 */
typedef struct {
    hi_u8 m2u_mcast_mode;
    hi_u8 m2u_snoop_on;
} mac_cfg_m2u_snoop_on_param_stru;

/* =================== 以下为解析内核配置参数转化为驱动内部参数下发的结构体 ======================== */
/* 解析内核配置的扫描参数后，下发给驱动的扫描参数 */
typedef struct {
    oal_ssids_stru          ssids[WLAN_SCAN_REQ_MAX_BSS];
    hi_u32                  l_ssid_num;
    const hi_u8            *puc_ie;
    hi_u32                  ie_len;
    hi_u32                 *pul_channels_2_g;
    /* WLAN/P2P 特性情况下，p2p0 和p2p-p2p0 cl 扫描时候，需要使用不同设备，增加bit_is_p2p0_scan来区分 */
    hi_u8                   is_p2p0_scan : 1;       /* 是否为p2p0 发起扫描 */
    hi_u8                   rsv          : 7;       /* 保留位 */
    hi_u8                   scan_type;
    hi_u8                   num_channels_2_g;
    hi_u8                   auc_arry;
} mac_cfg80211_scan_param_stru;

typedef struct {
    mac_cfg80211_scan_param_stru *mac_cfg80211_scan_param;
} mac_cfg80211_scan_param_pst_stru;

/* 解析内核配置的connect参数后，下发给驱动的connect参数 */
typedef struct {
    hi_u8               wpa_versions;
    hi_u8               cipher_group;
    hi_u8               n_ciphers_pairwise;
    hi_u8               ciphers_pairwise[OAL_NL80211_MAX_NR_CIPHER_SUITES];
    hi_u8               n_akm_suites;
    hi_u8               akm_suites[OAL_NL80211_MAX_NR_AKM_SUITES];

    hi_u8 control_port;
} mac_cfg80211_crypto_settings_stru;

typedef struct {
    hi_u8                           channel;              /* ap所在信道编号，eg 1,2,11,36,40... */
    hi_u8                           ssid_len;             /* SSID 长度 */
    mac_nl80211_mfp_enum_uint8      mfp;
    hi_u8                           wapi;

    hi_u8 *puc_ie;
    hi_u8 *puc_ssid;  /* 期望关联的AP SSID  */
    hi_u8 *puc_bssid; /* 期望关联的AP BSSID  */

    hi_u8 privacy;                              /* 是否加密标志 */
    oal_nl80211_auth_type_enum_uint8 auth_type; /* 认证类型，OPEN or SHARE-KEY */

    hi_u8 wep_key_len;        /* WEP KEY长度 */
    hi_u8 wep_key_index;      /* WEP KEY索引 */
    const hi_u8 *puc_wep_key; /* WEP KEY密钥 */

    mac_cfg80211_crypto_settings_stru crypto; /* 密钥套件信息 */
    hi_u32 ie_len;
} mac_cfg80211_connect_param_stru;

typedef struct {
    hi_u8                               privacy;             /* 是否加密标志 */
    oal_nl80211_auth_type_enum_uint8    auth_type;           /* 认证类型，OPEN or SHARE-KEY */
    hi_u8                               wep_key_len;         /* WEP KEY长度 */
    hi_u8                               wep_key_index;       /* WEP KEY索引 */
    hi_u8                               auc_wep_key[WLAN_WEP104_KEY_LEN];            /* WEP KEY密钥 */
    mac_nl80211_mfp_enum_uint8          mgmt_proteced;       /* 此条链接pmf是否使能 */
    wlan_pmf_cap_status_uint8           pmf_cap;             /* 设备pmf能力 */
    hi_u8                               wps_enable;

    mac_cfg80211_crypto_settings_stru   crypto;              /* 密钥套件信息 */
    hi_s8                               rssi;                /* 关联AP的RSSI信息 */
    hi_u8                               auc_rsv[3]; /* 3 byte保留字段，保证word对齐 */
} mac_cfg80211_connect_security_stru;

typedef struct {
    hi_u8 auc_mac_addr[OAL_MAC_ADDR_LEN];
    hi_u8 auc_rsv[2]; /* 2 byte保留字段，保证word对齐 */
} mac_cfg80211_init_port_stru;

#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
typedef enum {
    MAC_CONFIG_IPV4 = 0, /* 配置IPv4地址 */
    MAC_CONFIG_IPV6,     /* 配置IPv6地址 */
    MAC_CONFIG_BUTT
} mac_ip_type;
typedef hi_u8 mac_ip_type_enum_uint8;

typedef enum {
    MAC_IP_ADDR_DEL = 0, /* 删除IP地址 */
    MAC_IP_ADDR_ADD,     /* 增加IP地址 */
    MAC_IP_OPER_BUTT
} mac_ip_oper;
typedef hi_u8 mac_ip_oper_enum_uint8;

typedef struct {
    mac_ip_type_enum_uint8 type;
    mac_ip_oper_enum_uint8 oper;
    hi_u8 resv[2]; /* 填充2字节 */
    union {
        hi_u32 ipv4; /* 注意：网络字节序、大端。 */
        hi_u8 ipv6[OAL_IP_ADDR_MAX_SIZE];
    } ip;
} mac_ip_addr_config_stru;
#endif

typedef struct {
    hi_u8 band_num;
    hi_u8 resv[3]; /* 3 byte保留字段 */
    hi_s32 offset;
} mac_cfg_cal_bpower;

typedef struct {
    hi_u8 protol;
    hi_u8 rate;
    hi_u8 resv[2]; /* 2 byte保留字段 */
    hi_s32 val;
} mac_cfg_cal_rpower;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __MAC_CFG_H__ */
