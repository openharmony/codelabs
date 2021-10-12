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

#ifndef __MAC_VAP_H__
#define __MAC_VAP_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "mac_user.h"
#include "mac_cfg.h"
#include "mac_regdomain.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define MAC_VAP_RES_ID_INVALID          0xFF /* 非法的vap res id值 */
#define MAC_NUM_DR_802_11A              8    /* 11A 5g模式时的数据速率(DR)个数 */
#define MAC_NUM_BR_802_11A              3    /* 11A 5g模式时的基本速率(BR)个数 */
#define MAC_NUM_NBR_802_11A             5    /* 11A 5g模式时的非基本速率(NBR)个数 */

#define MAC_NUM_DR_802_11B              4    /* 11B 2.4G模式时的数据速率(DR)个数 */
#define MAC_NUM_BR_802_11B              2    /* 11B 2.4G模式时的数据速率(BR)个数 */
#define MAC_NUM_NBR_802_11B             2    /* 11B 2.4G模式时的数据速率(NBR)个数 */

#define MAC_NUM_DR_802_11G              8    /* 11G 2.4G模式时的数据速率(DR)个数 */
#define MAC_NUM_BR_802_11G              3    /* 11G 2.4G模式时的基本速率(BR)个数 */
#define MAC_NUM_NBR_802_11G             5    /* 11G 2.4G模式时的非基本速率(NBR)个数 */

#define MAC_NUM_DR_802_11G_MIXED        12   /* 11G 混合模式时的数据速率(DR)个数 */
#define MAC_NUM_BR_802_11G_MIXED_ONE    4    /* 11G 混合1模式时的基本速率(BR)个数 */
#define MAC_NUM_NBR_802_11G_MIXED_ONE   8    /* 11G 混合1模式时的非基本速率(NBR)个数 */

#define MAC_NUM_BR_802_11G_MIXED_TWO    7    /* 11G 混合2模式时的基本速率(BR)个数 */
#define MAC_NUM_NBR_802_11G_MIXED_TWO   5    /* 11G 混合2模式时的非基本速率(NBR)个数 */

#define MAC_MAX_RATE_SINGLE_NSS_20M_11N 0   /* 1个空间流20MHz的最大速率 */
#define MAC_MAX_RATE_SINGLE_NSS_40M_11N 0   /* 1个空间流40MHz的最大速率 */
#define MAC_MAX_RATE_DOUBLE_NSS_20M_11N 0   /* 1个空间流80MHz的最大速率 */
#define MAC_MAX_RATE_DOUBLE_NSS_40M_11N 0   /* 2个空间流20MHz的最大速率 */
/* 11AC MCS相关的内容 */
#define MAC_MAX_SUP_MCS7_11AC_EACH_NSS   0   /* 11AC各空间流支持的最大MCS序号，支持0-7 */
#define MAC_MAX_SUP_MCS8_11AC_EACH_NSS   1   /* 11AC各空间流支持的最大MCS序号，支持0-8 */
#define MAC_MAX_SUP_MCS9_11AC_EACH_NSS   2   /* 11AC各空间流支持的最大MCS序号，支持0-9 */
#define MAC_MAX_UNSUP_MCS_11AC_EACH_NSS  3   /* 11AC各空间流支持的最大MCS序号，不支持n个空间流 */

#define MAC_MAX_RATE_SINGLE_NSS_20M_11AC 86  /* 1个空间流20MHz的最大速率 */
#define MAC_MAX_RATE_SINGLE_NSS_40M_11AC 200 /* 1个空间流40MHz的最大速率 */
#define MAC_MAX_RATE_SINGLE_NSS_80M_11AC 433 /* 1个空间流80MHz的最大速率 */
#define MAC_MAX_RATE_DOUBLE_NSS_20M_11AC 173 /* 2个空间流20MHz的最大速率 */
#define MAC_MAX_RATE_DOUBLE_NSS_40M_11AC 400 /* 2个空间流40MHz的最大速率 */
#define MAC_MAX_RATE_DOUBLE_NSS_80M_11AC 866 /* 2个空间流80MHz的最大速率 */

#define MAC_MESH_MAX_ID                     64        /* MBR+MR节点最大ID号 */
#define MAC_MESH_INVALID_ID                 0         /* Mesh MBR/MR节点无效ID号 */
#define MAC_MESH_DEFAULT_ID                 255       /* Mesh MBR节点默认ID号，分配默认id进行tbtt计算后的值 */
#define MAC_MESH_MAX_MBR_NUM                5         /* MBR节点最大数目 */

#define MAC_VAP_USER_HASH_MAX_VALUE         4         /* 31H最大支持8个用户 数量少HASH桶缩减1倍 */
/* HASH计算取MAC最后两个相加 降低复杂度 */
#define mac_calculate_hash_value(_puc_mac_addr) \
    (((_puc_mac_addr)[4] + (_puc_mac_addr)[5]) & (MAC_VAP_USER_HASH_MAX_VALUE - 1))

#define is_ap(_pst_mac_vap)  ((WLAN_VAP_MODE_BSS_AP  == (_pst_mac_vap)->vap_mode) || \
    ((_pst_mac_vap)->vap_mode == WLAN_VAP_MODE_MESH))

#define is_sta(_pst_mac_vap) (WLAN_VAP_MODE_BSS_STA == (_pst_mac_vap)->vap_mode)
#define is_p2p_dev(_pst_mac_vap)    (WLAN_P2P_DEV_MODE    == (_pst_mac_vap)->p2p_mode)
#define is_p2p_go(_pst_mac_vap)     (WLAN_P2P_GO_MODE     == (_pst_mac_vap)->p2p_mode)
#define is_p2p_cl(_pst_mac_vap)     (WLAN_P2P_CL_MODE     == (_pst_mac_vap)->p2p_mode)
#define is_legacy_vap(_pst_mac_vap) (WLAN_LEGACY_VAP_MODE == (_pst_mac_vap)->p2p_mode)

#define MAC_SEND_TWO_DEAUTH_FLAG    0xf000

#ifdef _PRE_WLAN_FEATURE_WOW
#define MAC_SSID_WAKEUP_TIME        (50 * 60)   /* 5 分钟,单位100ms */
#endif

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
#define MAC_MAX_REPORT_TIME         6           /* 状态为TX_SUCC时的连续上报的个数阈值 */
#define MAC_MAX_REPORT_TX_CNT       10          /* 需要上报的发送次数阈值，初始值同DMAC_MAX_SW_RETRIES */
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_ROM
#define WOW_NETPATTERN_MAX_NUM      4
#define WOW_NETPATTERN_MAX_LEN      64
#endif

#define H2D_SYNC_MASK_BARK_PREAMBLE (1<<1)
#define H2D_SYNC_MASK_MIB           (1<<2)
#define H2D_SYNC_MASK_PROT          (1<<3)

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* VAP状态机，AP STA共用一个状态枚举 */
typedef enum {
    /* ap sta公共状态 */
    MAC_VAP_STATE_INIT               = 0,
    MAC_VAP_STATE_UP                 = 1,       /* VAP UP */
    MAC_VAP_STATE_PAUSE              = 2,       /* pause , for ap &sta */
    /* ap 独有状态 */
    MAC_VAP_STATE_AP_WAIT_START      = 3,
    /* sta独有状态 */
    MAC_VAP_STATE_STA_FAKE_UP        = 4,
    MAC_VAP_STATE_STA_WAIT_SCAN      = 5,
    MAC_VAP_STATE_STA_SCAN_COMP      = 6,
    MAC_VAP_STATE_STA_JOIN_COMP      = 7,
    MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2 = 8,
    MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4 = 9,
    MAC_VAP_STATE_STA_AUTH_COMP      = 10,
    MAC_VAP_STATE_STA_WAIT_ASOC      = 11,
    MAC_VAP_STATE_STA_OBSS_SCAN      = 12,
    MAC_VAP_STATE_STA_BG_SCAN        = 13,
    MAC_VAP_STATE_STA_LISTEN         = 14, /* p2p0 监听 */
    MAC_VAP_STATE_BUTT,
} mac_vap_state_enum;
typedef hi_u8 mac_vap_state_enum_uint8;

/* 芯片验证，控制帧/管理帧类型 */
typedef enum {
    MAC_TEST_MGMT_BCST = 0,    /* 非beacon广播管理帧 */
    MAC_TEST_MGMT_MCST = 1,    /* 非beacon组播管理帧 */
    MAC_TEST_ATIM_UCST = 2,    /* 单播ATIM帧 */
    MAC_TEST_UCST = 3,         /* 单播管理帧 */
    MAC_TEST_CTL_BCST = 4,     /* 广播控制帧 */
    MAC_TEST_CTL_MCST = 5,     /* 组播控制帧 */
    MAC_TEST_CTL_UCST = 6,     /* 单播控制帧 */
    MAC_TEST_ACK_UCST = 7,     /* ACK控制帧 */
    MAC_TEST_CTS_UCST = 8,     /* CTS控制帧 */
    MAC_TEST_RTS_UCST = 9,     /* RTS控制帧 */
    MAC_TEST_BA_UCST = 10,     /* BA控制帧 */
    MAC_TEST_CF_END_UCST = 11, /* CF-End控制帧 */
    MAC_TEST_TA_RA_EUQAL = 12, /* RA,TA相同帧 */
    MAC_TEST_MAX_TYPE_NUM
} mac_test_frame_type;
typedef hi_u8 mac_test_frame_type_enum_uint8;

/* 设置发送描述符参数枚举 */
typedef enum {
    MAC_SET_DSCR_PARAM_DATA0,   /* data0：对应发送描述符14行，32bit 10进制值 */
    MAC_SET_DSCR_PARAM_DATA1,   /* data1：对应发送描述符19行 */
    MAC_SET_DSCR_PARAM_DATA2,   /* data2：对应发送描述符20行 */
    MAC_SET_DSCR_PARAM_DATA3,   /* data3：对应发送描述符21行 */
    MAC_SET_DSCR_PARAM_RATE,    /* 配置11b/g/n速率 */
    MAC_SET_DSCR_PARAM_POWER,   /* tx power: 对应发送描述符22行 */
    MAC_SET_DSCR_PARAM_SHORTGI, /* 配置short GI或long GI */

    MAC_SET_DSCR_PARAM_BUTT
} mac_set_dscr_param_enum;
typedef hi_u8 mac_set_dscr_param_enum_uint8;

typedef enum {
    /* 业务调度算法配置参数,请添加到对应的START和END之间 */
    MAC_ALG_CFG_SCHEDULE_START,

    MAC_ALG_CFG_SCHEDULE_VI_CTRL_ENA,
    MAC_ALG_CFG_SCHEDULE_BEBK_MIN_BW_ENA,
    MAC_ALG_CFG_SCHEDULE_MVAP_SCH_ENA,
    MAC_ALG_CFG_FLOWCTRL_ENABLE_FLAG,
    MAC_ALG_CFG_SCHEDULE_VI_SCH_LIMIT,
    MAC_ALG_CFG_SCHEDULE_VO_SCH_LIMIT,
    MAC_ALG_CFG_SCHEDULE_VI_DROP_LIMIT,
    MAC_ALG_CFG_SCHEDULE_VI_MSDU_LIFE_MS,
    MAC_ALG_CFG_SCHEDULE_VO_MSDU_LIFE_MS,
    MAC_ALG_CFG_SCHEDULE_BE_MSDU_LIFE_MS,
    MAC_ALG_CFG_SCHEDULE_BK_MSDU_LIFE_MS,
    MAC_ALG_CFG_SCHEDULE_VI_LOW_DELAY_MS,
    MAC_ALG_CFG_SCHEDULE_VI_HIGH_DELAY_MS,
    MAC_ALG_CFG_SCHEDULE_VI_CTRL_MS,
    MAC_ALG_CFG_SCHEDULE_SCH_CYCLE_MS,
    MAC_ALG_CFG_SCHEDULE_TRAFFIC_CTRL_CYCLE,
    MAC_ALG_CFG_SCHEDULE_CIR_NVIP_KBPS,
    MAC_ALG_CFG_SCHEDULE_CIR_NVIP_KBPS_BE,
    MAC_ALG_CFG_SCHEDULE_CIR_NVIP_KBPS_BK,
    MAC_ALG_CFG_SCHEDULE_CIR_VIP_KBPS,
    MAC_ALG_CFG_SCHEDULE_CIR_VIP_KBPS_BE,
    MAC_ALG_CFG_SCHEDULE_CIR_VIP_KBPS_BK,
    MAC_ALG_CFG_SCHEDULE_CIR_VAP_KBPS,
    MAC_ALG_CFG_SCHEDULE_SM_TRAIN_DELAY,
    MAC_ALG_CFG_VIDEO_DROP_PKT_LIMIT,
    MAC_ALG_CFG_SCHEDULE_LOG_START,
    MAC_ALG_CFG_SCHEDULE_VAP_SCH_PRIO,
    MAC_ALG_CFG_SCHEDULE_LOG_END,
    MAC_ALG_CFG_SCHEDULE_END,
    /* AUTORATE算法配置参数，请添加到对应的START和END之间 */
    MAC_ALG_CFG_AUTORATE_START,
    MAC_ALG_CFG_AUTORATE_ENABLE,
    MAC_ALG_CFG_AUTORATE_USE_LOWEST_RATE,
    MAC_ALG_CFG_AUTORATE_SHORT_STAT_NUM,
    MAC_ALG_CFG_AUTORATE_SHORT_STAT_SHIFT,
    MAC_ALG_CFG_AUTORATE_LONG_STAT_NUM,
    MAC_ALG_CFG_AUTORATE_LONG_STAT_SHIFT,
    MAC_ALG_CFG_AUTORATE_MIN_PROBE_INTVL_PKTNUM,
    MAC_ALG_CFG_AUTORATE_MAX_PROBE_INTVL_PKTNUM,
    MAC_ALG_CFG_AUTORATE_PROBE_INTVL_KEEP_TIMES,
    MAC_ALG_CFG_AUTORATE_DELTA_GOODPUT_RATIO,
    MAC_ALG_CFG_AUTORATE_VI_PROBE_PER_LIMIT,
    MAC_ALG_CFG_AUTORATE_VO_PROBE_PER_LIMIT,
    MAC_ALG_CFG_AUTORATE_AMPDU_DURATION,
    MAC_ALG_CFG_AUTORATE_MCS0_CONT_LOSS_NUM,
    MAC_ALG_CFG_AUTORATE_UP_PROTOCOL_DIFF_RSSI,
    MAC_ALG_CFG_AUTORATE_RTS_MODE,
    MAC_ALG_CFG_AUTORATE_LEGACY_1ST_LOSS_RATIO_TH,
    MAC_ALG_CFG_AUTORATE_HT_VHT_1ST_LOSS_RATIO_TH,
    MAC_ALG_CFG_AUTORATE_LOG_ENABLE,
    MAC_ALG_CFG_AUTORATE_VO_RATE_LIMIT,
    MAC_ALG_CFG_AUTORATE_JUDGE_FADING_PER_TH,
    MAC_ALG_CFG_AUTORATE_AGGR_OPT,
    MAC_ALG_CFG_AUTORATE_AGGR_PROBE_INTVL_NUM,
    MAC_ALG_CFG_AUTORATE_DBG_VI_STATUS,
    MAC_ALG_CFG_AUTORATE_DBG_AGGR_LOG,
    MAC_ALG_CFG_AUTORATE_AGGR_NON_PROBE_PCK_NUM,
    MAC_ALG_CFG_AUTORATE_AGGR_MIN_AGGR_TIME_IDX,
    MAC_ALG_CFG_AUTORATE_MAX_AGGR_NUM,
    MAC_ALG_CFG_AUTORATE_LIMIT_1MPDU_PER_TH,
    MAC_ALG_CFG_AUTORATE_BTCOEX_PROBE_ENABLE,
    MAC_ALG_CFG_AUTORATE_BTCOEX_AGGR_ENABLE,
    MAC_ALG_CFG_AUTORATE_COEX_STAT_INTVL,
    MAC_ALG_CFG_AUTORATE_COEX_LOW_ABORT_TH,
    MAC_ALG_CFG_AUTORATE_COEX_HIGH_ABORT_TH,
    MAC_ALG_CFG_AUTORATE_COEX_AGRR_NUM_ONE_TH,
    MAC_ALG_CFG_AUTORATE_DYNAMIC_BW_ENABLE,
    MAC_ALG_CFG_AUTORATE_THRPT_WAVE_OPT,
    MAC_ALG_CFG_AUTORATE_GOODPUT_DIFF_TH,
    MAC_ALG_CFG_AUTORATE_PER_WORSE_TH,
    MAC_ALG_CFG_AUTORATE_RX_CTS_NO_BA_NUM,
    MAL_ALG_CFG_AUTORATE_VOICE_AGGR,
    MAC_ALG_CFG_AUTORATE_FAST_SMOOTH_SHIFT,
    MAC_ALG_CFG_AUTORATE_FAST_SMOOTH_AGGR_NUM,
    MAC_ALG_CFG_AUTORATE_SGI_PUNISH_PER,
    MAC_ALG_CFG_AUTORATE_SGI_PUNISH_NUM,
    MAC_ALG_CFG_AUTORATE_VI_HOLD_RATE_RSSI_TH,
    MAC_ALG_CFG_AUTORATE_VI_HOLDING_RATE,
    MAC_ALG_CFG_AUTORATE_END,
    /* AUTORATE算法日志配置参数，请添加到对应的START和END之间 */
    MAC_ALG_CFG_AUTORATE_LOG_START,
    MAC_ALG_CFG_AUTORATE_STAT_LOG_START,
    MAC_ALG_CFG_AUTORATE_SELECTION_LOG_START,
    MAC_ALG_CFG_AUTORATE_FIX_RATE_LOG_START,
    MAC_ALG_CFG_AUTORATE_STAT_LOG_WRITE,
    MAC_ALG_CFG_AUTORATE_SELECTION_LOG_WRITE,
    MAC_ALG_CFG_AUTORATE_FIX_RATE_LOG_WRITE,
    MAC_ALG_CFG_AUTORATE_AGGR_STAT_LOG_START,
    MAC_ALG_CFG_AUTORATE_AGGR_STAT_LOG_WRITE,
    MAC_ALG_CFG_AUTORATE_LOG_END,
    /* AUTORATE算法系统测试命令，请添加到对应的START和END之间 */
    MAC_ALG_CFG_AUTORATE_TEST_START,
    MAC_ALG_CFG_AUTORATE_DISPLAY_RATE_SET,
    MAC_ALG_CFG_AUTORATE_CONFIG_FIX_RATE,
    MAC_ALG_CFG_AUTORATE_CYCLE_RATE,
    MAC_ALG_CFG_AUTORATE_DISPLAY_RX_RATE,
    MAC_ALG_CFG_AUTORATE_TEST_END,
    /* 抗干扰算法配置参数，请添加到对应的START和END之间 */
    MAC_ALG_CFG_ANTI_INTF_START,
    MAC_ALG_CFG_ANTI_INTF_IMM_ENABLE,
    MAC_ALG_CFG_ANTI_INTF_UNLOCK_ENABLE,
    MAC_ALG_CFG_ANTI_INTF_RSSI_STAT_CYCLE,
    MAC_ALG_CFG_ANTI_INTF_UNLOCK_CYCLE,
    MAC_ALG_CFG_ANTI_INTF_UNLOCK_DUR_TIME,
    MAC_ALG_CFG_ANTI_INTF_NAV_IMM_ENABLE,
    MAC_ALG_CFG_ANTI_INTF_GOODPUT_FALL_TH,
    MAC_ALG_CFG_ANTI_INTF_KEEP_CYC_MAX_NUM,
    MAC_ALG_CFG_ANTI_INTF_KEEP_CYC_MIN_NUM,
    MAC_ALG_CFG_ANTI_INTF_TX_TIME_FALL_TH,
    MAC_ALG_CFG_ANTI_INTF_PER_PROBE_EN,
    MAC_ALG_CFG_ANTI_INTF_PER_FALL_TH,
    MAC_ALG_CFG_ANTI_INTF_GOODPUT_JITTER_TH,
    MAC_ALG_CFG_ANTI_INTF_DEBUG_MODE,
    MAC_ALG_CFG_ANTI_INTF_END,
    /* EDCA优化算法配置参数，请添加到对应的START和END之间 */
    MAC_ALG_CFG_EDCA_OPT_START,
    MAC_ALG_CFG_EDCA_OPT_CO_CH_DET_CYCLE,
    MAC_ALG_CFG_EDCA_OPT_AP_EN_MODE,
    MAC_ALG_CFG_EDCA_OPT_STA_EN,
    MAC_ALG_CFG_EDCA_OPT_STA_WEIGHT,
    MAC_ALG_CFG_EDCA_OPT_NONDIR_TH,
    MAC_ALG_CFG_EDCA_OPT_TH_UDP,
    MAC_ALG_CFG_EDCA_OPT_TH_TCP,
    MAC_ALG_CFG_EDCA_OPT_DEBUG_MODE,
    MAC_ALG_CFG_EDCA_OPT_END,
    /* CCA优化算法配置参数，请添加到对应的START和END之间 */
    MAC_ALG_CFG_CCA_OPT_START,
    MAC_ALG_CFG_CCA_OPT_ALG_EN_MODE,
    MAC_ALG_CFG_CCA_OPT_DEBUG_MODE,
    MAC_ALG_CFG_CCA_OPT_SET_T1_COUNTER_TIME,
    MAC_ALG_CFG_CCA_OPT_END,
    /* CCA OPT算法日志配置参数，请添加到对应的START和END之间 */
    MAC_ALG_CFG_CCA_OPT_LOG_START,
    MAC_ALG_CFG_CCA_OPT_STAT_LOG_START,
    MAC_ALG_CFG_CCA_OPT_STAT_LOG_WRITE,
    MAC_ALG_CFG_CCA_OPT_LOG_END,

    /* TPC算法配置参数, 请添加到对应的START和END之间 */
    MAC_ALG_CFG_TPC_START,
    MAC_ALG_CFG_TPC_MODE,
    MAC_ALG_CFG_TPC_DEBUG,
    MAC_ALG_CFG_TPC_POWER_LEVEL,
    MAC_ALG_CFG_TPC_LOG,
    MAC_ALG_CFG_TPC_MANAGEMENT_MCAST_FRM_POWER_LEVEL,
    MAC_ALG_CFG_TPC_CONTROL_FRM_POWER_LEVEL,
    MAC_ALG_CFG_TPC_OVER_TMP_TH,
    MAC_ALG_CFG_TPC_DPD_ENABLE_RATE,
    MAC_ALG_CFG_TPC_TARGET_RATE_11B,
    MAC_ALG_CFG_TPC_TARGET_RATE_11AG,
    MAC_ALG_CFG_TPC_TARGET_RATE_HT20,
    MAC_ALG_CFG_TPC_NO_MARGIN_POW,
    MAC_ALG_CFG_TPC_POWER_AMEND,
    MAC_ALG_CFG_TPC_END,
    /* TPC算法日志配置参数，请添加到对应的START和END之间 */
    MAC_ALG_CFG_TPC_LOG_START,
    MAC_ALG_CFG_TPC_STAT_LOG_START,
    MAC_ALG_CFG_TPC_STAT_LOG_WRITE,
    MAC_ALG_CFG_TPC_PER_PKT_LOG_START,
    MAC_ALG_CFG_TPC_PER_PKT_LOG_WRITE,
    MAC_ALG_CFG_TPC_GET_FRAME_POW,
    MAC_ALG_CFG_TPC_RESET_STAT,
    MAC_ALG_CFG_TPC_RESET_PKT,
    MAC_ALG_CFG_TPC_LOG_END,
    MAC_ALG_CFG_BUTT
} mac_alg_cfg_enum;
typedef hi_u8 mac_alg_cfg_enum_uint8;

typedef enum { /* hi1131-cb */
    SHORTGI_20_CFG_ENUM,
    SHORTGI_40_CFG_ENUM,
    SHORTGI_80_CFG_ENUM,
    SHORTGI_BUTT_CFG
} short_gi_cfg_type;

typedef enum {
    MAC_SET_BEACON = 0,
    MAC_ADD_BEACON = 1,

    MAC_BEACON_OPERATION_BUTT
} mac_beacon_operation_type;
typedef hi_u8 mac_beacon_operation_type_uint8;

typedef enum {
    MAC_WMM_SET_PARAM_TYPE_DEFAULT,
    MAC_WMM_SET_PARAM_TYPE_UPDATE_EDCA,

    MAC_WMM_SET_PARAM_TYPE_BUTT
} mac_wmm_set_param_type_enum;
typedef hi_u8 mac_wmm_set_param_type_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
typedef enum {
    MAC_APSD_SP_LEN_ALL  = 0,     /* 对应bit5 bit6 为00 */
    MAC_APSD_SP_LEN_TWO  = 2,     /* 对应bit5 bit6 为01 */
    MAC_APSD_SP_LEN_FOUR = 4,     /* 对应bit5 bit6 为10 */
    MAC_APSD_SP_LEN_SIX  = 6,     /* 对应bit5 bit6 为11 */
    MAC_APSD_SP_LEN_BUTT
} mac_apsd_sp_len_enum;
typedef hi_u8 mac_apsd_sp_len_enum_uint8;
#endif

#ifdef _PRE_WLAN_FEATURE_STA_PM
/* Power save modes specified by the user */
typedef enum {
    NO_POWERSAVE     = 0,
    MIN_FAST_PS      = 1,
    MAX_FAST_PS      = 2,
    MIN_PSPOLL_PS    = 3,
    MAX_PSPOLL_PS    = 4,
    NUM_PS_MODE      = 5
} ps_user_mode_enum;
typedef hi_u8 ps_user_mode_enum_uint8;
#endif

#define MAC_VAP_AP_STATE_BUTT (MAC_VAP_STATE_AP_WAIT_START + 1)
#define MAC_VAP_STA_STATE_BUTT MAC_VAP_STATE_BUTT

typedef enum {
    HISTREAM_SWITCH_OFF   = 0,        /* Histream 功能使能关闭  */
    HISTREAM_SWITCH_ON    = 1,        /* Histream 功能使能打开  */
    HISTREAM_SWITCH_ON_BUTT,          /* 最大类型，应小于 8  */
} mac_histream_switch_enum;

#ifdef _PRE_WLAN_FEATURE_WOW
typedef enum {
    MAC_WOW_DISABLE,
    MAC_WOW_ENABLE,
    MAC_WOW_EN_BUTT
} mac_wow_en_enum;

typedef enum {
    MAC_WOW_WAKEUP_NOTIFY,
    MAC_WOW_SLEEP_REQUEST,
    MAC_WOW_NOTIFY_TYPE_BUTT
} mac_wow_notify_type_enum;

/* 去关联开关统一到 MAC_WOW_FIELD_DISASSOC,
   MAC_WOW_FIELD_DISASSOC_RX 因没有 netbuf 备份，目前合入 MAC_WOW_FIELD_DISASSOC_TX 开关类别 */
typedef enum {
    MAC_WOW_FIELD_ALL_CLEAR          = 0,          /* Clear all events */
    MAC_WOW_FIELD_MAGIC_PACKET       = BIT0,       /* Wakeup on Magic Packet */
    MAC_WOW_FIELD_NETPATTERN_TCP     = BIT1,       /* Wakeup on TCP NetPattern */
    MAC_WOW_FIELD_NETPATTERN_UDP     = BIT2,       /* Wakeup on UDP NetPattern */
    MAC_WOW_FIELD_DISASSOC           = BIT3,       /* 去关联/去认证，Wakeup on Disassociation/Deauth */
    MAC_WOW_FIELD_AUTH_RX            = BIT4,       /* 对端关联请求，Wakeup on auth */
    MAC_WOW_FIELD_HOST_WAKEUP        = BIT5,       /* Host wakeup */
    MAC_WOW_FIELD_TCP_UDP_KEEP_ALIVE = BIT6,       /* Wakeup on TCP/UDP keep alive timeout */
    MAC_WOW_FIELD_OAM_LOG_WAKEUP     = BIT7,       /* OAM LOG wakeup */
    MAC_WOW_FIELD_SSID_WAKEUP        = BIT8,       /* SSID Scan wakeup */
} mac_wow_field_enum;

typedef enum {
    MAC_WOW_WKUP_REASON_TYPE_NULL               = 0,        /* None */
    MAC_WOW_WKUP_REASON_TYPE_MAGIC_PACKET       = 1,        /* Wakeup on Magic Packet */
    MAC_WOW_WKUP_REASON_TYPE_NETPATTERN_TCP     = 2,        /* Wakeup on TCP NetPattern */
    MAC_WOW_WKUP_REASON_TYPE_NETPATTERN_UDP     = 3,        /* Wakeup on UDP NetPattern */
    MAC_WOW_WKUP_REASON_TYPE_DISASSOC_RX        = 4,        /* 对端去关联/去认证，Wakeup on Disassociation/Deauth */
    MAC_WOW_WKUP_REASON_TYPE_DISASSOC_TX        = 5,        /* 对端去关联/去认证，Wakeup on Disassociation/Deauth */
    MAC_WOW_WKUP_REASON_TYPE_AUTH_RX            = 6,        /* 本端端关联请求，Wakeup on auth */
    MAC_WOW_WKUP_REASON_TYPE_TCP_UDP_KEEP_ALIVE = 7,        /* Wakeup on TCP/UDP keep alive timeout */
    MAC_WOW_WKUP_REASON_TYPE_HOST_WAKEUP        = 8,        /* Host wakeup */
    MAC_WOW_WKUP_REASON_TYPE_OAM_LOG            = 9,        /* OAM LOG wakeup */
    MAC_WOW_WKUP_REASON_TYPE_SSID_SCAN          = 10,       /* SSID Scan wakeup */
    MAC_WOW_WKUP_REASON_TYPE_BUT
} mac_wow_wakeup_reason_type_enum;
#endif

/* ****************************************************************************
  STRUCT定义
**************************************************************************** */
/* channel结构体 */
typedef struct {
    hi_u8 chan_number;                              /* 主20MHz信道号 */
    wlan_channel_band_enum_uint8 band;              /* 频段 */
    wlan_channel_bandwidth_enum_uint8 en_bandwidth; /* 带宽模式 */
    hi_u8 idx;                                      /* 信道索引号 */
} mac_channel_stru;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
typedef struct {
    hi_u8 type;    /* shortgi 20/40/80     */
    hi_u8 enable;  /* 1:enable; 0:disable  */
    hi_u8 resv[2]; /* 2 byte保留字段 */
} shortgi_cfg_stru;
#endif

#define SHORTGI_CFG_STRU_LEN 4

typedef struct {
    hi_u8 announced_channel; /* 新信道号 AP用 */
    hi_u8 ch_switch_cnt;     /* 信道切换计数 AP用 */
    hi_u8 new_channel;       /* STA用 可以考虑跟上面合并 */
    hi_u8 new_ch_swt_cnt;    /* STA用 可以考虑跟上面合并 */

    wlan_channel_bandwidth_enum_uint8 announced_bandwidth : 4; /* 新带宽模式 AP用 */
    wlan_channel_bandwidth_enum_uint8 new_bandwidth       : 4; /* STA用 可以考虑跟上面合并 */
    hi_u8 ch_swt_cnt;                                          /* ap上一次发送的切换个数 */
    hi_u8 csa_rsv_cnt;                                         /* ap csa 计数不减的计数 */
    wlan_ch_switch_status_enum_uint8 ch_switch_status : 2;     /* 信道切换状态 */
    wlan_bw_switch_status_enum_uint8 bw_switch_status : 2;     /* 带宽切换状态 31H只支持20M 功能不支持 */
    hi_u8                  csa_present_in_bcn : 1,                   /* Beacon帧中是否包含CSA IE */
                                         waiting_to_shift_channel : 1,
                                         channel_swt_cnt_zero : 1,
                                         bad_ap         : 1 ;        /* 场景识别出ap发送beacon带csa但不切信道 */    
     /*
     * ---|--------|--------------------|-----------------|-----------
     * 0        3                    0                 0
     * X        A                    B                 C
     *
     * sta的信道切换可通过上图帮助理解, 数字为切换计数器，
     * X->A A之前为未发生任务信道切换时,切换计数器为0
     * 从A->B时间段为sta等待切换状态: en_waiting_to_shift_channel为true
     * 从B->C为sta信道切换中,即等待ap加beacon状态: en_waiting_for_ap为true
     * C-> 为sta收到了ap的beacon，标准信道切换结束
     *
     * A点通常中收到csa ie(beacon/action...), B点通常为tbtt中断中切换计数器变为
     * 0或者csa ie中计数器为0，C点则为收到beacon
     *
     * 从A->C的过程中，会过滤重复收到的csa ie或者信道切换动作
     *
     */
} mac_ch_switch_info_stru;

typedef struct {
    hi_u8 mac_rate; /* MAC对应速率 */
    hi_u8 phy_rate; /* PHY对应速率 */
    hi_u8 mbps;     /* 速率 */
    hi_u8 auc_resv[1];
} mac_data_rate_stru;

typedef struct {
    hi_u8 rs_nrates;   /* 速率个数 */
    hi_u8 auc_resv[3]; /* 3 BYTE保留字段 */
    mac_data_rate_stru ast_rs_rates[WLAN_MAX_SUPP_RATES];
} mac_rateset_stru;

typedef struct {
    hi_u8 br_rate_num;  /* 基本速率个数 */
    hi_u8 nbr_rate_num; /* 非基本速率个数 */
    hi_u8 max_rate;     /* 最大基本速率 */
    hi_u8 min_rate;     /* 最小基本速率 */
    mac_rateset_stru rate;
} mac_curr_rateset_stru;

/* wme参数 */
typedef struct {
    hi_u8 aifsn;        /* AIFSN parameters 逻辑4bit mib使用uint32 */
    hi_u8 logcwmin;     /* cwmin in exponential form, 单位2^n -1 slot 逻辑4bit mib使用uint32 */
    hi_u16 us_logcwmax; /* cwmax in exponential form, 单位2^n -1 slot 同cwin 对齐使用u16 */
    hi_u32 txop_limit;  /* txopLimit, us */
} mac_wme_param_stru;

/* MAC vap能力特性标识 */
typedef struct {
    hi_u32  uapsd                          : 1,
                txop_ps                        : 1,
                wpa                            : 1,
                wpa2                           : 1,
                dsss_cck_mode_40mhz            : 1,                 /* 是否允许在40M上使用DSSS/CCK, 1-允许, 0-不允许 */
                rifs_tx_on                     : 1,
                tdls_prohibited                : 1,                 /* tdls全局禁用开关， 0-不关闭, 1-关闭 */
                tdls_channel_switch_prohibited : 1,                 /* tdls信道切换全局禁用开关， 0-不关闭, 1-关闭 */
                hide_ssid                      : 1,                 /* AP开启隐藏ssid,  0-关闭, 1-开启 */
                wps                            : 1,                 /* AP WPS功能:0-关闭, 1-开启 */
                ac2g                           : 1,                 /* 2.4G下的11ac:0-关闭, 1-开启 */
                keepalive                      : 1,                 /* vap KeepAlive功能开关: 0-关闭, 1-开启 */
                smps                           : 2,                 /* vap 当前SMPS能力 */
                dpd_enbale                     : 1,                 /* dpd是否开启 */
                dpd_done                       : 1,                 /* dpd是否完成 */
                ntxbf                          : 1,                 /* 11n txbf能力 */
                disable_2ght40                 : 1,                 /* 2ght40禁止位，1-禁止，0-不禁止 */
                auto_dtim_enbale               : 1,                 /* 动态DTIM能力 */
                hide_meshid                    :1,                  /* 是否隐藏MeshID */
                bit_resv                           : 12;
} mac_cap_flag_stru;

/* VAP收发包统计 */
typedef struct {
    /* net_device用统计信息, net_device统计经过以太网的报文 */
    hi_u32 rx_packets_to_lan;                /* 接收流程到LAN的包数 */
    hi_u32 rx_bytes_to_lan;                  /* 接收流程到LAN的字节数 */
    hi_u32 rx_dropped_packets;               /* 接收流程中丢弃的包数 */
    hi_u32 rx_vap_non_up_dropped;            /* vap没有up丢弃的包的个数 */
    hi_u32 rx_dscr_error_dropped;            /* 描述符出错丢弃的包的个数 */
    hi_u32 rx_first_dscr_excp_dropped;       /* 描述符首包异常丢弃的包的个数 */
    hi_u32 rx_alg_filter_dropped;            /* 算法过滤丢弃的包的个数 */
    hi_u32 rx_feature_ap_dropped;            /* AP特性帧过滤丢包个数 */
    hi_u32 rx_null_frame_dropped;            /* 收到NULL帧的数目 */
    hi_u32 rx_transmit_addr_checked_dropped; /* 发送端地址过滤失败丢弃 */
    hi_u32 rx_dest_addr_checked_dropped;     /* 目的地址过滤失败丢弃 */
    hi_u32 rx_multicast_dropped;             /* 组播帧失败(netbuf copy失败)丢弃 */

    hi_u32 tx_packets_from_lan; /* 发送流程LAN过来的包数 */
    hi_u32 tx_bytes_from_lan;   /* 发送流程LAN过来的字节数 */
    hi_u32 tx_dropped_packets;  /* 发送流程中丢弃的包数 */

    /* 其它报文统计信息 */
} mac_vap_stats_stru;

typedef struct {
    hi_u8 user_idx;
    wlan_protocol_enum_uint8 avail_protocol_mode; /* 用户协议模式 */
    wlan_protocol_enum_uint8 cur_protocol_mode;
    wlan_protocol_enum_uint8 protocol_mode;
} mac_h2d_user_protocol_stru;

typedef struct {
    hi_u8 user_idx;
    hi_u8 arg1;
    hi_u8 arg2;
    hi_u8 uc_resv;

    /* 协议模式信息 */
    wlan_protocol_enum_uint8 cur_protocol_mode;
    wlan_protocol_enum_uint8 protocol_mode;
    hi_u8 avail_protocol_mode; /* 用户和VAP协议模式交集, 供算法调用 */

    wlan_bw_cap_enum_uint8 bandwidth_cap;   /* 用户带宽能力信息 */
    wlan_bw_cap_enum_uint8 avail_bandwidth; /* 用户和VAP带宽能力交集,供算法调用 */
    wlan_bw_cap_enum_uint8 cur_bandwidth;   /* 默认值与en_avail_bandwidth相同,供算法调用修改 */

    hi_u8 user_pmf;
    mac_user_asoc_state_enum_uint8 user_asoc_state; /* 用户关联状态 */
} mac_h2d_usr_info_stru;

typedef struct {
    mac_user_cap_info_stru user_cap_info; /* 用户能力信息 */
    hi_u8 user_idx;
    hi_u8 auc_resv[3]; /* 3 BYTE保留字段 */
} mac_h2d_usr_cap_stru;

typedef struct {
    hi_u8 user_idx;
    hi_u8 uc_resv;
    wlan_protocol_enum_uint8 protocol_mode; /* 用户协议模式 */
    /* legacy速率集信息 */
    hi_u8 avail_rs_nrates;
    hi_u8 auc_avail_rs_rates[WLAN_MAX_SUPP_RATES];
    /* ht速率集信息 */
    mac_user_ht_hdl_stru ht_hdl;
} mac_h2d_usr_rate_info_stru;

typedef struct {
    hi_u16                         us_sta_aid;
    hi_u8                          uapsd_cap;
    hi_u8                          auc_resv[1];
}mac_h2d_vap_info_stru;

typedef struct {
    hi_u8 user_idx;
    wlan_protocol_enum_uint8 avail_protocol_mode; /* 用户协议模式 */
    wlan_bw_cap_enum_uint8 bandwidth_cap;         /* 用户带宽能力信息 */
    wlan_bw_cap_enum_uint8 avail_bandwidth;       /* 用户和VAP带宽能力交集,供算法调用 */
    wlan_bw_cap_enum_uint8 cur_bandwidth;         /* 默认值与en_avail_bandwidth相同,供算法调用修改 */
    hi_u8 auc_rsv[3];                             /* 3 BYTE保留字段 */
} mac_h2d_user_bandwidth_stru;

typedef struct {
    mac_channel_stru channel;
    hi_u8 user_idx;
    wlan_bw_cap_enum_uint8 bandwidth_cap;   /* 用户带宽能力信息 */
    wlan_bw_cap_enum_uint8 avail_bandwidth; /* 用户和VAP带宽能力交集,供算法调用 */
    wlan_bw_cap_enum_uint8 cur_bandwidth;   /* 默认值与en_avail_bandwidth相同,供算法调用修改 */
} mac_d2h_syn_info_stru;

typedef struct {
    mac_channel_stru channel; /* vap所在的信道 */
    mac_ch_switch_info_stru ch_switch_info;
    hi_u8 user_idx;
    hi_u8 vap_id;
    hi_u8 auc_rsv[2]; /* 2 BYTE保留字段 */
} mac_d2h_syn_data_stru;

typedef struct {
    hi_u32 data_blk_cnt; /* 需要传输的数据块个数 */
    hi_u32 wakeup_reason;
} mac_d2h_syn_hdr_stru;

typedef struct {
    mac_channel_stru channel; /* vap所在的信道 */
    mac_ch_switch_info_stru ch_switch_info;
    mac_user_ht_hdl_stru ht_hdl; /* HT capability IE和 operation IE的解析信息 */
    mac_vht_hdl_stru vht_hdl;    /* VHT capability IE和 operation IE的解析信息 */
    hi_u8 user_idx;
    hi_u8 vap_id;
    wlan_bw_cap_enum_uint8 bandwidth_cap;   /* 用户带宽能力信息 */
    wlan_bw_cap_enum_uint8 avail_bandwidth; /* 用户和VAP带宽能力交集,供算法调用 */
    wlan_bw_cap_enum_uint8 cur_bandwidth;   /* 默认值与en_avail_bandwidth相同,供算法调用修改 */
    hi_u8 auc_rsv[3];                       /* 3 BYTE保留字段 */
} mac_h2d_syn_data_stru;

/* WOW传输消息类型 */
typedef enum {
    MAC_WOW_SLEEP_NOTIFY_MSG,
    MAC_WOW_SYNC_DATA_MSG,
    MAC_WOW_MSG_BUTT,
} mac_wow_msg_enum;
typedef hi_u8 mac_wow_msg_enum_uint8;

typedef struct {
    mac_wow_msg_enum_uint8 msg_type; /* 传输的消息类型 */
    hi_u8 notify_param;              /* 睡眠通知参数 */
    hi_u8 auc_resv[2];               /* 2 BYTE保留字段 */
    hi_u32 data_blk_cnt;             /* 需要传输的数据块个数 */
} mac_h2d_syn_info_hdr_stru;

typedef struct {
    hi_u8 user_idx;
    mac_user_asoc_state_enum_uint8 asoc_state;
    hi_u8 rsv[2]; /* 2 BYTE保留字段 */
} mac_h2d_user_asoc_state_stru;

typedef struct {
    hi_u8 auc_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 auc_pmkid[WLAN_PMKID_LEN];
    hi_u8 auc_resv0[2]; /* 2 BYTE保留字段 */
} mac_pmkid_info_stu;

typedef struct {
    hi_u8 num_elems;
    hi_u8 auc_resv0[3]; /* 3 BYTE保留字段 */
    mac_pmkid_info_stu ast_elem[WLAN_PMKID_CACHE_SIZE];
} mac_pmkid_cache_stru;

typedef struct {
    /* word 0 */
    wlan_prot_mode_enum_uint8           protection_mode;                         /* 保护模式 */
    hi_u8                           obss_non_erp_aging_cnt;                      /* 指示OBSS中non erp 站点的老化时间 */
    hi_u8                           obss_non_ht_aging_cnt;                       /* 指示OBSS中non ht 站点的老化时间 */
    /* 指示保护策略是否开启，HI_SWITCH_ON 打开， HI_SWITCH_OFF 关闭 */
    hi_u8                           auto_protection        : 1;
    hi_u8                           obss_non_erp_present   : 1;                  /* 指示obss中是否存在non ERP的站点 */
    hi_u8                           obss_non_ht_present    : 1;                  /* 指示obss中是否存在non HT的站点 */
    /* 指rts_cts 保护机制是否打开, HI_SWITCH_ON 打开， HI_SWITCH_OFF 关闭 */
    hi_u8                           rts_cts_protect_mode   : 1;
    /* 指示L-SIG protect是否打开, HI_SWITCH_ON 打开， HI_SWITCH_OFF 关闭 */
    hi_u8                           lsig_txop_protect_mode : 1;
    hi_u8                           reserved               : 3;

    /* word 1 */
    hi_u8                           sta_no_short_slot_num;                    /* 不支持short slot的STA个数 */
    hi_u8                           sta_no_short_preamble_num;                /* 不支持short preamble的STA个数 */
    hi_u8                           sta_non_erp_num;                          /* 不支持ERP的STA个数 */
    hi_u8                           sta_non_ht_num;                           /* 不支持HT的STA个数 */
    /* word 2 */
    hi_u8                           sta_non_gf_num;                           /* 支持ERP/HT,不支持GF的STA个数 */
    hi_u8                           sta_20_m_only_num;                        /* 只支持20M 频段的STA个数 */
    hi_u8                           sta_no_40dsss_cck_num;                    /* 不用40M DSSS-CCK STA个数  */
    hi_u8                           sta_no_lsig_txop_num;                     /* 不支持L-SIG TXOP Protection STA个数 */
} mac_protection_stru;

/* 用于同步保护相关的参数 */
typedef struct {
    wlan_mib_ht_protection_enum_uint8 dot11_ht_protection;
    hi_u8 dot11_rifs_mode;
    hi_u8 dot11_lsigtxop_full_protection_activated;
    hi_u8 dot11_non_gf_entities_present;
    mac_protection_stru protection;
} mac_h2d_protection_stru;

typedef struct {
    hi_u8 *puc_ie; /* APP 信息元素 */
    hi_u32 ie_len; /* APP 信息元素长度 */
} mac_app_ie_stru;

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
/* STA UAPSD 配置命令 */
typedef struct {
    hi_u8 max_sp_len;
    hi_u8 delivery_map;
    hi_u8 trigger_map;
    hi_u8 uc_resv;
} mac_cfg_uapsd_sta_stru;
#endif

/* RTS 发送参数 */
typedef struct {
    wlan_legacy_rate_value_enum_uint8   auc_rate[WLAN_TX_RATE_MAX_NUM];           /* 发送速率，单位mpbs */
    /* 协议模式, 取值参见wlan_phy_protocol_enum_uint8 */
    wlan_phy_protocol_enum_uint8        auc_protocol_mode[WLAN_TX_RATE_MAX_NUM];
    wlan_channel_band_enum_uint8        band;
    hi_u8                               auc_recv[3];                              /* 3 byte保留字段 */
} mac_cfg_rts_tx_param_stru;

/* VAP的数据结构 */
typedef struct {
    hi_u8 vap_id;                      /* vap ID 即资源池索引值 */
    wlan_vap_mode_enum_uint8 vap_mode; /* vap模式  */
    /* BSSID，非MAC地址，MAC地址是mib中的auc_dot11StationID  */
    hi_u8 auc_bssid[WLAN_MAC_ADDR_LEN];

    mac_vap_state_enum_uint8 vap_state; /* VAP状态 */
    wlan_protocol_enum_uint8 protocol;  /* 工作的协议模式 */
    hi_u8 tx_power;                     /* 传输功率, 单位dBm */
    /* 初始为0，AP模式下，每跟新一次wmm参数这个变量加1,在beacon帧和assoc rsp中会填写，4bit，不能超过15；STA模式下
        解析帧并更新这个值 */
    hi_u8 wmm_params_update_count;

    mac_channel_stru channel; /* vap所在的信道 */
    mac_ch_switch_info_stru ch_switch_info;

    hi_u8                           has_user_bw_limit       : 1,    /* 该vap是否存在user限速 */
                                        vap_bw_limit        : 1,    /* 该vap是否已限速 */
                                        voice_aggr          : 1,    /* 该vap是否针对VO业务支持聚合 */
                                        support_any         : 1,    /* 该vap是否当前支持ANY功能 */
                                        uapsd_cap           : 1,    /* 保存与STA关联的AP是否支持uapsd能力信息 */
                                        user_pmf_cap        : 1,    /* STA侧在未创建user前，存储目标user的pmf使能信息 */
                                        mesh_accept_sta     : 1, /* 表示当前Mesh是否支持sta关联(与Accepting Peer不同) */
                                        mesh_tbtt_adjusting : 1;    /* 表示Mesh是否正在调整TBTT */

    hi_u8                           user_nums;                                   /* VAP下已挂接的用户个数 */
    hi_u8                           multi_user_idx;                              /* 组播用户ID */
    hi_u8                           cache_user_id;                               /* cache user对应的userID */

    hi_u8                           al_tx_flag  : 1,          /* 常发标志 */
                                        payload_flag: 2,      /* payload内容:0:全0  1:全1  2:random */
                                        first_run   : 1,      /* 常发关闭再次打开标志 */
                                        need_send_keepalive_null : 1, /* 标志sta Pause状态下是否需要发送Keepalive帧 */
                                        is_conn_to_mesh : 1,  /* 标志sta是否关联到Mesh */
                                        csi_flag : 1,         /* 标志当前MAC_VAP下CSI功能是否挂载 */
                                        reserved : 1;
    wlan_p2p_mode_enum_uint8            p2p_mode;             /* 0:非P2P设备; 1:P2P_GO; 2:P2P_Device; 3:P2P_CL */
    hi_u8                           p2p_listen_channel;       /* P2P Listen channel */
    /* VAP为STA模式时保存user(ap)的资源池索引；VAP为AP模式时，不用此成员变量 */
    hi_u8                           assoc_vap_id;

    hi_u8                           report_times_limit;       /* MESH 状态不变，连续上报该次数后无需继续上报发送信息 */
    hi_u8                           report_tx_cnt_limit;      /* MESH 需上报的发送次数阈值 */
    hi_u8                           priority;                 /* 当前mesh节点的优先级,用于选择潜在配对节点(0-256) */
    hi_u8                           mnid;                     /* 供决定节点发送时隙,由mesh协议栈提供(0表示非法值) */

    hi_u8                           is_mbr;                   /* 标识是否是MBR节点(true:MBR,false:MR) */
    hi_u8                           vap_rx_nss;
    hi_u8                           auc_cache_user_mac_addr[WLAN_MAC_ADDR_LEN];     /* cache user对应的MAC地址 */

    /* VAP为STA模式时保存AP分配给STA的AID(从响应帧获取),取值范围1~2007; VAP为AP模式时，不用此成员变量 */
    hi_u16                          us_sta_aid;
    hi_u16                          us_assoc_user_cap_info;                         /* sta要关联的用户的能力信息 */

    hi_list                             ast_user_hash[MAC_VAP_USER_HASH_MAX_VALUE]; /* hash数组,使用HASH结构内的DLIST */
    hi_list                             mac_user_list_head;          /* 关联用户节点双向链表,使用USER结构内的DLIST */
    mac_cap_flag_stru                   cap_flag;                    /* vap能力特性标识 */
    wlan_mib_ieee802dot11_stru         *mib_info;        /* mib信息(当时配置vap时，可以直接将指针值为NULL，节省空间)  */
    mac_curr_rateset_stru               curr_sup_rates;              /* 当前支持的速率集 */
    mac_protection_stru                 protection;                  /* 与保护相关变量 */
    mac_app_ie_stru                     ast_app_ie[OAL_APP_IE_NUM];
#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
    mac_cfg_uapsd_sta_stru sta_uapsd_cfg; /* UAPSD的配置信息 */
#endif
    oal_spin_lock_stru cache_user_lock; /* cache_user lock */
#ifdef _PRE_WLAN_FEATURE_BW_HIEX
    hi_u8                           selfcts;                                /* 是否使能窄带切宽带发送selfcts */
    hi_u8                           duration;                               /* selfcts的占用信道时间，单位ms */
    hi_u16                          us_per;                                 /* 发送selfcts的PER阈值 */
#endif
} mac_vap_stru;

typedef struct {
    mac_vap_stru *mac_vap;
    hi_s8 pc_param[4]; /* 查询或配置信息,占4 byte */
} mac_cfg_event_stru;

/* HOSTAPD 设置 Beacon 信息 */
typedef struct {
    hi_s32 l_interval;    /* beacon interval */
    hi_s32 l_dtim_period; /* DTIM period     */
    hi_u8 privacy;
    hi_u8 crypto_mode;  /* WPA/WPA2 */
    hi_u8 group_crypto; /* 组播密钥类型 */
    hi_u8 hidden_ssid;
    hi_u8 auc_auth_type[MAC_AUTHENTICATION_SUITE_NUM]; /* akm 类型 */
    hi_u8 auc_pairwise_crypto_wpa[MAC_PAIRWISE_CIPHER_SUITES_NUM];
    hi_u8 auc_pairwise_crypto_wpa2[MAC_PAIRWISE_CIPHER_SUITES_NUM];
    hi_u16 us_rsn_capability;
    hi_u8 shortgi_20;
    hi_u8 shortgi_40;
    hi_u8 shortgi_80;
    wlan_protocol_enum_uint8 protocol;

    hi_u8 smps_mode;
    mac_beacon_operation_type_uint8 operation_type;
    hi_u8 auc_resv1[2]; /* 2 byte保留字段 */
#ifdef _PRE_WLAN_FEATURE_MESH
    hi_u8 mesh_auth_protocol;
    hi_u8 mesh_formation_info;
    hi_u8 mesh_capability;
    hi_u8 auc_resv2[1];
#endif
} mac_beacon_param_stru;

/* CSI参数配置结构体 */
typedef struct {
    hi_u8 mac_addr[WLAN_MAC_ADDR_LEN]; /* 配置的MAC地址 */
    hi_u8 sample_period;               /* 配置的采样周期 */
    hi_u8 frame_type;                  /* 配置的帧类型 */
} csi_entry_stru;

typedef struct {
    csi_entry_stru      ast_csi_param[OAL_CSI_MAX_MAC_NUM];    /* 最多配置6个mac地址上报 */
    hi_u32              report_min_interval;
    hi_u8               entry_num;
    hi_u8               resv[3];  /* 3 byte保留字段 */
} mac_csi_config_stru;

/* 上报的CSI数据结构体 */
typedef struct {
    hi_u8               csi_data[OAL_CSI_DATA_BUFF_SIZE];      /* 将184字节CSI数据上报到wal层 */
    hi_u32              data_len;
} mac_csi_data_stru;

typedef struct {
    hi_u8 default_key;
    hi_u8 key_index;
    hi_u8 key_len;
    hi_u8 auc_wep_key[WLAN_WEP104_KEY_LEN];
} mac_wep_key_param_stru;

typedef struct mac_pmksa_tag {
    hi_u8 auc_bssid[OAL_MAC_ADDR_LEN];
    hi_u8 auc_pmkid[OAL_PMKID_LEN];
} mac_pmksa_stru;

typedef struct {
    hi_u8 key_index;
    hi_u8 pairwise;
    hi_u8 auc_mac_addr[OAL_MAC_ADDR_LEN];
    mac_key_params_stru key;
} mac_addkey_param_stru;

typedef struct {
    hi_s32 key_len;
    hi_u8 auc_key[OAL_WPA_KEY_LEN];
} mac_key_stru;

typedef struct {
    hi_s32 seq_len;
    hi_u8 auc_seq[OAL_WPA_SEQ_LEN];
} mac_seq_stru;

typedef struct {
    hi_u8 key_index;
    hi_u8 pairwise;
    hi_u8 auc_mac_addr[OAL_MAC_ADDR_LEN];
    hi_u8 cipher;
    hi_u8 auc_rsv[3]; /* 3 byte保留字段 */
    mac_key_stru key;
    mac_seq_stru seq;
} mac_addkey_hmac2dmac_param_stru;

typedef struct {
    oal_net_device_stru     *netdev;
    hi_u8                key_index;
    hi_u8      pairwise;
    hi_u8                auc_resv1[2]; /* 2 byte保留字段 */
    hi_u8               *puc_mac_addr;
    hi_void                *cookie;
    hi_void               (*callback)(hi_void *cookie, oal_key_params_stru *key_param);
} mac_getkey_param_stru;

typedef struct {
    hi_u8                key_index;
    hi_u8      pairwise;
    hi_u8                auc_mac_addr[OAL_MAC_ADDR_LEN];
} mac_removekey_param_stru;

typedef struct {
    hi_u8                key_index;
    hi_u8      unicast;
    hi_u8      multicast;
    hi_u8                auc_resv1[1];
} mac_setdefaultkey_param_stru;

/* 算法参数枚举，参数值 */
typedef struct {
    mac_alg_cfg_enum_uint8  alg_cfg;     /* 配置命令枚举 */
    hi_u8                   is_negtive;  /* 配置参数值是否为负 */
    hi_u8                   uc_resv[2];  /* 2 字节对齐 */
    hi_u32                  value;       /* 配置参数值 */
} mac_ioctl_alg_param_stru;

/* AUTORATE LOG 算法参数枚举，参数值 */
typedef struct {
    mac_alg_cfg_enum_uint8  alg_cfg;                        /* 配置命令枚举 */
    hi_u8               auc_mac_addr[WLAN_MAC_ADDR_LEN];    /* MAC地址 */
    hi_u8               ac_no;                              /* AC类型 */
    hi_u8               auc_resv[2];                        /* 2 BYTE保留字段 */
    hi_u16              us_value;                           /* 配置参数值 */
} mac_ioctl_alg_ar_log_param_stru;

/* AUTORATE 测试相关的命令参数 */
typedef struct {
    mac_alg_cfg_enum_uint8  alg_cfg;                         /* 配置命令枚举 */
    hi_u8               auc_mac_addr[WLAN_MAC_ADDR_LEN];     /* MAC地址 */
    hi_u8               auc_resv[1];
    hi_u16              us_value;                            /* 命令参数 */
} mac_ioctl_alg_ar_test_param_stru;

/* TXMODE LOG 算法参数枚举，参数值 */
typedef struct {
    mac_alg_cfg_enum_uint8  alg_cfg;                        /* 配置命令枚举 */
    hi_u8               ac_no;                              /* AC类型 */
    hi_u8               auc_mac_addr[WLAN_MAC_ADDR_LEN];    /* MAC地址 */
    hi_u8               auc_resv1[2];                       /* 2 BYTE保留字段 */
    hi_u16              us_value;                           /* 配置参数值 */
} mac_ioctl_alg_txbf_log_param_stru;
/* 算法配置命令接口 */
typedef struct {
    hi_u8 argc;
    hi_u8 auc_argv_offset[DMAC_ALG_CONFIG_MAX_ARG];
} mac_ioctl_alg_config_stru;

/* TPC LOG 算法参数枚举，参数值 */
typedef struct {
    mac_alg_cfg_enum_uint8 alg_cfg;        /* 配置命令枚举 */
    hi_u8 auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC地址 */
    hi_u8 ac_no;                           /* AC类型 */
    hi_u16 us_value;                       /* 配置参数值 */
    hi_u16 resv;
    hi_char *pc_frame_name; /* 获取特定帧功率使用该变量 */
} mac_ioctl_alg_tpc_log_param_stru;

/* cca opt LOG 算法参数枚举，参数值 */
typedef struct {
    hi_u16 us_value;                /* 统计总时间 */
    mac_alg_cfg_enum_uint8 alg_cfg; /* 配置命令枚举 */
    hi_u8 auc_resv;
} mac_ioctl_alg_cca_opt_log_param_stru;

/* 加组播转单播黑名单 */
typedef struct {
    hi_u32 deny_group_addr;
} mac_add_m2u_deny_table_stru;

/* 清空组播转单播黑名单 */
typedef struct {
    hi_u8 m2u_clear_deny_table;
    hi_u8 m2u_show_deny_table;
} mac_clg_m2u_deny_table_stru;

/* print snoop table */
typedef struct {
    hi_u8 m2u_show_snoop_table;
} mac_show_m2u_snoop_table_stru;

/* add snoop table */
typedef struct {
    hi_u8 m2u_add_snoop_table;
} mac_add_m2u_snoop_table_stru;

typedef struct {
    hi_u8 proxyarp;
    hi_u8 auc_rsv[3]; /* 3 BYTE保留字段 */
} mac_proxyarp_en_stru;

typedef struct {
    hi_u64                          ull_cookie;
    hi_u32                          listen_duration;             /* 监听时间   */
    hi_u8                           uc_listen_channel;           /* 监听的信道 */
    wlan_channel_bandwidth_enum_uint8   listen_channel_type;     /* 监听信道类型 */
    hi_u8                           home_channel;                /* 监听结束返回的信道 */
    wlan_channel_bandwidth_enum_uint8   home_channel_type;       /* 监听结束，返回主信道类型 */
    /* P2P0和P2P_CL 公用VAP 结构，保存进入监听前VAP 的状态，便于监听结束时恢复该状态 */
    mac_vap_state_enum_uint8            last_vap_state;
    wlan_channel_band_enum_uint8        band;
    hi_u16                              resv;
    oal_ieee80211_channel_stru          st_listen_channel;
} mac_remain_on_channel_param_stru;

/* WPAS 管理帧发送结构 */
typedef struct {
    hi_s32               channel;
    hi_u8               mgmt_frame_id;
    hi_u8               rsv;
    hi_u16              us_len;
    const hi_u8    *puc_frame;
} mac_mgmt_frame_stru;

/* P2P发送action帧状态结构体 */
typedef struct {
    hi_u8 *puc_buf;
    hi_u32 len;
    hi_u8  ack;
    hi_u8  resv[3]; /* 3 BYTE保留字段 */
} mac_p2p_tx_status_stru;

#ifdef _PRE_WLAN_FEATURE_WOW
/* WOW ssid wakeup 参数配置，所有配置 */
typedef struct {
    hi_u8 ssid_set_flag;
    hi_s8 ac_ssid[WLAN_SSID_MAX_LEN]; /* 32+1 */
    hi_u8 auc_res[2];                 /* 2 BYTE保留字段 */
} wow_ssid_cfg_stru;
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_ROM
/* WOW netpattern 参数配置,单个配置 */
typedef struct {
    hi_u8 auc_pattern_data[WOW_NETPATTERN_MAX_LEN];
    hi_u32 pattern_len;
} wow_pattern_stru;

/* WOW netpattern 参数配置，所有配置 */
typedef struct {
    wow_pattern_stru ast_pattern[WOW_NETPATTERN_MAX_NUM];
    hi_u16 us_pattern_map;
    hi_u16 us_pattern_num;
} wow_pattern_cfg_stru;

typedef struct {
    hi_u8 wow_en;
    hi_u8 auc_res[3]; /* 复用为pno配置auc_res[0]为扫描信道,auc_res[1]和auc_res[2]为pno周期,总3byte */
    hi_u32 wow_event;
    wow_pattern_cfg_stru wow_pattern;
} mac_wow_cfg_stu;
#endif

/* RF寄存器定制化结构体 */
typedef struct {
    hi_u16 us_rf_reg117;
    hi_u16 us_rf_reg123;
    hi_u16 us_rf_reg124;
    hi_u16 us_rf_reg125;
    hi_u16 us_rf_reg126;
    hi_u8 auc_resv[2]; /* 2 BYTE 保留字段 */
} mac_cus_dts_rf_reg;

/* FCC认证 参数结构体 */
typedef struct {
    hi_u8 index;       /* 下标表示偏移 */
    hi_u8 max_txpower; /* 最大发送功率 */
    hi_u8 dbb_scale;   /* dbb scale */
    hi_u8 uc_resv;
} mac_cus_band_edge_limit_stru;

/* 定制化 校准配置参数 */
typedef struct {
    /* dts */
    hi_u16 aus_cali_txpwr_pa_dc_ref_2g_val[13]; /* txpwr分信道ref值,占13 short */
    hi_s16 us_cali_txpwr_pa_dc_ref_5g_val_band1;
    hi_s16 us_cali_txpwr_pa_dc_ref_5g_val_band2;
    hi_s16 us_cali_txpwr_pa_dc_ref_5g_val_band3;
    hi_s16 us_cali_txpwr_pa_dc_ref_5g_val_band4;
    hi_s16 us_cali_txpwr_pa_dc_ref_5g_val_band5;
    hi_s16 us_cali_txpwr_pa_dc_ref_5g_val_band6;
    hi_s16 us_cali_txpwr_pa_dc_ref_5g_val_band7;
    hi_s8 band_5g_enable;
    hi_u8 tone_amp_grade;
    hi_s8 auc_resv_wifi_cali[2]; /* 占2 BYTE */
    /* bt tmp */
    hi_u16 us_cali_bt_txpwr_pa_ref_band1;
    hi_u16 us_cali_bt_txpwr_pa_ref_band2;
    hi_u16 us_cali_bt_txpwr_pa_ref_band3;
    hi_u16 us_cali_bt_txpwr_pa_ref_band4;
    hi_u16 us_cali_bt_txpwr_pa_ref_band5;
    hi_u16 us_cali_bt_txpwr_pa_ref_band6;
    hi_u16 us_cali_bt_txpwr_pa_ref_band7;
    hi_u16 us_cali_bt_txpwr_pa_ref_band8;
    hi_u16 us_cali_bt_txpwr_numb;
    hi_u16 us_cali_bt_txpwr_pa_fre1;
    hi_u16 us_cali_bt_txpwr_pa_fre2;
    hi_u16 us_cali_bt_txpwr_pa_fre3;
    hi_u16 us_cali_bt_txpwr_pa_fre4;
    hi_u16 us_cali_bt_txpwr_pa_fre5;
    hi_u16 us_cali_bt_txpwr_pa_fre6;
    hi_u16 us_cali_bt_txpwr_pa_fre7;
    hi_u16 us_cali_bt_txpwr_pa_fre8;
    hi_u8 bt_tone_amp_grade;
    hi_u8 auc_resv_bt_cali[1];
} mac_cus_dts_cali_stru;

/* dbb scaling 参数结构体 */
typedef struct {
    hi_u32 dbb_scale[9]; /* DBB幅值 9 WORD */
} dbb_scaling_stru;

typedef struct {
    hi_u32 tx_pwr[MAC_NUM_2G_CH_NUM]; /* FCC各信道各速率的发送功率 */
} fcc_tx_pwr_stru;

typedef struct {
    hi_s16 high_temp_th; /* 高温阈值 */
    hi_s16 low_temp_th;  /* 回滞低温阈值 */
    hi_s16 comp_val;     /* 补偿值 */
} freq_comp_stru;
/* ======================== cfg id对应的get set函数 ==================================== */
typedef struct {
    wlan_cfgid_enum_uint16      cfgid;
    hi_u8                       auc_resv[2];    /* 2 字节对齐 */
    hi_u32                      (*get_func)(mac_vap_stru *mac_vap, hi_u8 *puc_len, hi_u8 *puc_param);
    hi_u32                      (*set_func)(mac_vap_stru *mac_vap, hi_u8 uc_len, hi_u8 *puc_param);
} mac_cfgid_stru;

/* ****************************************************************************
  内联函数定义
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_UAPSD
/* ****************************************************************************
 功能描述  : 设置U-APSD使能
 修改历史      :
**************************************************************************** */
static inline hi_void mac_vap_set_uapsd_en(mac_vap_stru *mac_vap, hi_u8 value)
{
    mac_vap->cap_flag.uapsd = (value) ? HI_TRUE : HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 读取beacon interval的值
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_vap_get_uapsd_en(const mac_vap_stru *mac_vap)
{
    return mac_vap->cap_flag.uapsd;
}

/* ****************************************************************************
 功能描述  : 设置vap的uapsd参数
 返 回 值  : hi_void
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_uapsd_para(mac_vap_stru *mac_vap, const mac_cfg_uapsd_sta_stru *uapsd_info)
{
    hi_u8 ac;

    mac_vap->sta_uapsd_cfg.max_sp_len = uapsd_info->max_sp_len;
    for (ac = 0; ac < WLAN_WME_AC_BUTT; ac++) {
        mac_vap->sta_uapsd_cfg.delivery_map = uapsd_info->delivery_map;
        mac_vap->sta_uapsd_cfg.trigger_map = uapsd_info->trigger_map;
    }
}
#endif

/* ****************************************************************************
 功能描述  : 检查dmac list的检查
 修改历史      :
  1.日    期   : 2015年04月02日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年05月02日
    作    者   : HiSilicon
    修改内容   : 修改函数名及返回值
**************************************************************************** */
static inline hi_u8 mac_vap_user_exist(const hi_list *new_code, const hi_list *head)
{
    hi_list *user_list_head = HI_NULL;
    hi_list *member_entry = HI_NULL;

    hi_list_for_each_safe(member_entry, user_list_head, head) {
        if (new_code == member_entry) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* ****************************************************************************
功能描述  : 初始化Mesh相关Mib值
修改历史      :
 1.日    期   : 2019年1月29日
    作    者   : HiSilicon
   修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_init_mib_mesh(const mac_vap_stru *mac_vap)
{
    wlan_mib_ieee802dot11_stru *mib_info;
    mib_info = mac_vap->mib_info;
    mib_info->wlan_mib_sta_config.dot11_mesh_activated = (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) ?
        HI_TRUE : HI_FALSE;
    mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_accepting_additional_peerings = HI_TRUE;
    mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_security_activated = HI_FALSE;
    mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_active_authentication_protocol = 0;
    mib_info->wlan_mib_mesh_sta_cfg.dot11_mbca_activated = HI_FALSE;
}
#endif

/* ****************************************************************************
 功能描述  : 设置vap的发送功率
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_tx_power(mac_vap_stru *mac_vap, hi_u8 tx_power)
{
    mac_vap->tx_power = tx_power;
}

/* ****************************************************************************
 功能描述  : 设置vap的aid
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_aid(mac_vap_stru *mac_vap, hi_u16 us_aid)
{
    mac_vap->us_sta_aid = us_aid;
}

/* ****************************************************************************
 功能描述  : 设置vap的assoc_vap_id 该参数只在STA有效
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_void mac_vap_set_assoc_id(mac_vap_stru *mac_vap, hi_u8 assoc_vap_id)
{
    mac_vap->assoc_vap_id = assoc_vap_id;
}

/* ****************************************************************************
 功能描述  : 设置vap的assoc_vap_id 该参数只在STA有效
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_uapsd_cap(mac_vap_stru *mac_vap, hi_u8 uapsd_cap)
{
    mac_vap->uapsd_cap = uapsd_cap & BIT0;
}

/* ****************************************************************************
 功能描述  : 设置vap的p2p模式
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_p2p_mode(mac_vap_stru *mac_vap, wlan_p2p_mode_enum_uint8 p2p_mode)
{
    mac_vap->p2p_mode = p2p_mode;
}

/* ****************************************************************************
 功能描述  : 设置vap的组播用户id
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_multi_user_idx(mac_vap_stru *mac_vap, hi_u8 multi_user_idx)
{
    mac_vap->multi_user_idx = multi_user_idx;
}

/* ****************************************************************************
 功能描述  : 设置vap的常发payload长度
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_al_tx_payload_flag(mac_vap_stru *mac_vap, hi_u8 paylod)
{
    mac_vap->payload_flag = paylod;
}

/* ****************************************************************************
 功能描述  : 设置vap的常发模式
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_al_tx_first_run(mac_vap_stru *mac_vap, hi_u8 flag)
{
    mac_vap->first_run = flag;
}

/* ****************************************************************************
 功能描述  : 设置vap的wmm update count
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_wmm_params_update_count(mac_vap_stru *mac_vap, hi_u8 update_count)
{
    mac_vap->wmm_params_update_count = update_count;
}

/* ****************************************************************************
 功能描述  : 设置vap的hide ssid
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_set_hide_ssid(mac_vap_stru *mac_vap, hi_u8 value)
{
    mac_vap->cap_flag.hide_ssid = value;
}

/* ****************************************************************************
 功能描述  : 获取Vap的P2P模式
 修改历史      :
  1.日    期   : 2014年11月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline wlan_p2p_mode_enum_uint8 mac_get_p2p_mode(const mac_vap_stru *mac_vap)
{
    return (mac_vap->p2p_mode);
}

/* ****************************************************************************
 功能描述  : 清除保存的ie
 输入参数  : mac_vap_stru *pst_mac_vap
           enum WPS_IE_TYPE en_type
 修改历史      :
 1.日    期   : 2015年4月28日
    作    者   : HiSilicon
  修改内容   : 新生成函数
 **************************************************************************** */
static inline hi_void mac_vap_clear_app_ie(mac_vap_stru *mac_vap, en_app_ie_type_uint8 type)
{
    if (type < OAL_APP_IE_NUM) {
        if (mac_vap->ast_app_ie[type].puc_ie != HI_NULL) {
            oal_mem_free(mac_vap->ast_app_ie[type].puc_ie);
            mac_vap->ast_app_ie[type].puc_ie = HI_NULL;
        }
        mac_vap->ast_app_ie[type].ie_len = 0;
    }
    return;
}

static inline hi_void mac_vap_free_mib(mac_vap_stru *mac_vap)
{
    if (mac_vap->mib_info != HI_NULL) {
        wlan_mib_ieee802dot11_stru *mib_info = mac_vap->mib_info;
        /* 先置空再释放 */
        mac_vap->mib_info = HI_NULL;
        oal_mem_free(mib_info);
    }
}

/* ****************************************************************************
 功能描述  : legacy协议初始化vap能力
 修改历史      :
  1.日    期   : 2013年11月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_cap_init_legacy(mac_vap_stru *mac_vap)
{
    mac_vap->cap_flag.rifs_tx_on = HI_FALSE;
    mac_vap->cap_flag.smps = WLAN_MIB_MIMO_POWER_SAVE_MIMO;
}

/* ****************************************************************************
 功能描述  : ht vht协议初始化vap能力
 修改历史      :
  1.日    期   : 2013年11月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_cap_init_htvht(mac_vap_stru *mac_vap)
{
    mac_vap->cap_flag.rifs_tx_on = HI_FALSE;
    mac_vap->cap_flag.smps = WLAN_MIB_MIMO_POWER_SAVE_MIMO;
}

/* ****************************************************************************
 功能描述  : 设置BSSID
 输入参数  : [1]mac_vap,
             [2]puc_bssid
             [3]ssid_len
 返 回 值  : 无
**************************************************************************** */
static inline hi_void mac_vap_set_bssid(mac_vap_stru *mac_vap, const hi_u8 *puc_bssid, hi_u8 ssid_len)
{
    if (memcpy_s(mac_vap->auc_bssid, WLAN_MAC_ADDR_LEN, puc_bssid, ssid_len) != EOK) {
        return;
    }
}

/* ****************************************************************************
 功能描述  : VAP状态迁移事件以消息形式上报SDT
 输入参数  : en_vap_state:将要变为的状态
 修改历史      :
  1.日    期   : 2013年12月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_vap_state_change(mac_vap_stru *mac_vap, mac_vap_state_enum_uint8 vap_state)
{
    mac_vap->vap_state = vap_state;
}

/* ****************************************************************************
 功能描述  : 查询自动保护机制是否开启
 修改历史      :
  1.日    期   : 2014年1月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_vap_protection_autoprot_is_enabled(const mac_vap_stru *mac_vap)
{
    return mac_vap->protection.auto_protection;
}

/* ****************************************************************************
 功能描述  : 获取vap的 mac地址
 输入参数  : dmac_vap_stru *pst_dmac_vap
 修改历史      :
  1.日    期   : 2015年7月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 *mac_vap_get_mac_addr(const mac_vap_stru *mac_vap)
{
    /* _PRE_WLAN_FEATURE_P2P + */
    if (is_p2p_dev(mac_vap)) {
        /* 获取P2P DEV MAC 地址，赋值到probe req 帧中 */
        return mac_vap->mib_info->wlan_mib_sta_config.auc_p2p0_dot11_station_id;
    } else {
        /* 设置地址2为自己的MAC地址 */
        return mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id;
    }
}

static inline hi_void mac_protection_set_lsig_txop_mechanism(mac_vap_stru *mac_vap, hi_u8 flag)
{
    /* 数据帧/管理帧发送时候，需要根据bit_lsig_txop_protect_mode值填写发送描述符中的L-SIG TXOP enable位 */
    mac_vap->protection.lsig_txop_protect_mode = flag;
}

/* ****************************************************************************
 功能描述  : 检测认证方式是否匹配
 输入参数  : wlan_mib_ieee802dot11_stru *pst_mib_info
             hi_u8 uc_policy
 返 回 值  : hi_u8    HI_TRUE:匹配成功
                                    HI_FALSE:匹配失败
 修改历史      :
  1.日    期   : 2013年8月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_check_auth_policy(const wlan_mib_ieee802dot11_stru *mib_info, hi_u8 policy)
{
    hi_u8 loop;

    for (loop = 0; loop < MAC_AUTHENTICATION_SUITE_NUM; loop++) {
        /* 检测认证套件是否使能和匹配 */
        if ((mib_info->ast_wlan_mib_rsna_cfg_auth_suite[loop].dot11_rsna_config_authentication_suite_activated) &&
            (policy ==
            mib_info->ast_wlan_mib_rsna_cfg_auth_suite[loop].dot11_rsna_config_authentication_suite_implemented)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_void mac_dec_p2p_num(const mac_vap_stru *mac_vap);
hi_void mac_inc_p2p_num(const mac_vap_stru *mac_vap);
hi_u8 mac_is_wep_allowed(const mac_vap_stru *mac_vap);
mac_wme_param_stru *mac_get_wmm_cfg(wlan_vap_mode_enum_uint8 vap_mode);
hi_u32 mac_device_find_user_by_macaddr(const mac_vap_stru *mac_vap, const hi_u8 *sta_mac_addr, hi_u8 addr_len,
    hi_u8 *puc_user_idx);
hi_void mac_protection_set_rts_tx_param(mac_vap_stru *mac_vap, hi_u8 flag, wlan_prot_mode_enum_uint8 prot_mode,
    mac_cfg_rts_tx_param_stru *rts_tx_param);
hi_bool mac_protection_lsigtxop_check(const mac_vap_stru *mac_vap);

/* ****************************************************************************
    VAP操作函数
**************************************************************************** */
hi_u32 mac_vap_res_exit(hi_void);
hi_u32 mac_vap_res_init(const hi_u8 vap_num);
hi_u8 mac_vap_alloc_vap_res(hi_void);
hi_void mac_vap_free_vap_res(hi_u8 idx);
mac_vap_stru *mac_vap_get_vap_stru(hi_u8 idx);
hi_u32 mac_vap_is_valid(hi_u8 idx);
hi_void mac_vap_exit(mac_vap_stru *mac_vap);
hi_void mac_vap_init_rates(mac_vap_stru *mac_vap);
hi_void mac_vap_init_rates_by_protocol(mac_vap_stru *mac_vap, wlan_protocol_enum_uint8 vap_protocol,
    mac_data_rate_stru *rates);
hi_u32 mac_vap_del_user(mac_vap_stru *mac_vap, hi_u8 user_idx);
hi_u32 mac_vap_add_assoc_user(mac_vap_stru *mac_vap, hi_u8 user_idx);
hi_u32 mac_vap_find_user_by_macaddr(mac_vap_stru *mac_vap, const hi_u8 *sta_mac_addr, hi_u8 mac_addr_len,
    hi_u8 *puc_user_idx);
hi_u32 mac_vap_init(mac_vap_stru *mac_vap, hi_u8 vap_id, const mac_cfg_add_vap_param_stru *param);
hi_void mac_vap_init_wme_param(const mac_vap_stru *mac_vap);
hi_void mac_vap_check_bss_cap_info_phy_ap(hi_u16 us_cap_info, const mac_vap_stru *mac_vap);
hi_void mac_vap_get_bandwidth_cap(mac_vap_stru *mac_vap, wlan_bw_cap_enum_uint8 *pen_cap);
hi_void mac_vap_init_user_security_port(const mac_vap_stru *mac_vap, mac_user_stru *mac_user);
hi_void mac_vap_change_mib_by_bandwidth(const mac_vap_stru *mac_vap, wlan_channel_bandwidth_enum_uint8 bandwidth);
hi_u32 mac_vap_config_vht_ht_mib_by_protocol(const mac_vap_stru *mac_vap);
hi_u32 mac_vap_set_default_key(const mac_vap_stru *mac_vap, hi_u8 key_index);
hi_u32 mac_vap_set_default_mgmt_key(const mac_vap_stru *mac_vap, hi_u8 key_index);
hi_u32 mac_vap_set_beacon(mac_vap_stru *mac_vap, const mac_beacon_param_stru *beacon_param);
hi_u32 mac_vap_add_beacon(mac_vap_stru *mac_vap, const mac_beacon_param_stru *beacon_param);
hi_u32 mac_vap_init_by_protocol(mac_vap_stru *mac_vap, wlan_protocol_enum_uint8 protocol);
hi_u32 mac_vap_save_app_ie(mac_vap_stru *mac_vap, const oal_app_ie_stru *app_ie, en_app_ie_type_uint8 type);
hi_u32 mac_vap_init_privacy(mac_vap_stru *mac_vap, mac_cfg80211_connect_security_stru *mac_sec_param);
hi_u32 mac_vap_set_current_channel(mac_vap_stru *mac_vap, wlan_channel_band_enum_uint8 band, hi_u8 channel);
hi_u32 mac_vap_add_key(const mac_vap_stru *mac_vap, mac_user_stru *mac_user, hi_u8 key_id,
    const mac_key_params_stru *key);
hi_u8 mac_vap_get_default_key_id(const mac_vap_stru *mac_vap);
hi_u8 mac_vap_get_curr_baserate(mac_vap_stru *mac_vap, hi_u8 br_idx);
mac_user_stru *mac_vap_get_user_by_addr(mac_vap_stru *mac_vap, const hi_u8 *mac_addr);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_void mac_vap_set_mib_mesh(const mac_vap_stru *mac_vap, hi_u8 mesh_auth_protocol);
#endif
wlan_prot_mode_enum_uint8 mac_vap_get_user_protection_mode(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user);
hi_void mac_vap_set_cb_tx_user_idx(mac_vap_stru *mac_vap, hi_void *tx_ctl, const hi_u8 *mac_addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __MAC_VAP_H__ */
