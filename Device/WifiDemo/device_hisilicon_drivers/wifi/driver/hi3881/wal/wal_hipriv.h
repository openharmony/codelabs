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

#ifndef __WAL_HIPRIV_H__
#define __WAL_HIPRIV_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "wal_main.h"
#include "mac_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* 私有配置命令字符串最大长度，对应本地内存池一级大小 */
#define WAL_HIPRIV_CMD_MAX_LEN       (WLAN_MEM_LOCAL_SIZE3 - 4)

#define WAL_HIPRIV_CMD_NAME_MAX_LEN  36                             /* 字符串中每个单词的最大长度 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define WAL_HIPRIV_PROC_ENTRY_NAME   "hipriv"
#endif
#ifdef _PRE_WLAN_FEATURE_BW_HIEX
#define WAL_HIPRIV_SELFCTS_DURATION_MAX      32
#define WAL_HIPRIV_SELFCTS_PER_MAX           1000
#endif

#define WAL_HIPRIV_HT_MCS_MIN           0
#define WAL_HIPRIV_HT_MCS_MAX           7
#define WAL_HIPRIV_VHT_MCS_MIN          0
#define WAL_HIPRIV_VHT_MCS_MAX          9
#define WAL_HIPRIV_NSS_MIN              1
#define WAL_HIPRIV_NSS_MAX              4
#define WAL_HIPRIV_CH_NUM               4
#define WAL_HIPRIV_SNPRINTF_DST         10

#define WAL_HIPRIV_BOOL_NIM             0
#define WAL_HIPRIV_BOOL_MAX             1
#define WAL_HIPRIV_FREQ_SKEW_ARG_NUM    8

#define WAL_HIPRIV_MS_TO_S                   1000   /* ms和s之间倍数差 */
#define WAL_HIPRIV_KEEPALIVE_INTERVAL_MIN    5000   /* 受默认老化计数器出发时间所限制 */
#define WAL_HIPRIV_KEEPALIVE_INTERVAL_MAX    0xffff /* timer间隔时间限制所致(oal_uin16) */

#define CAL_BAND_POWER_OFFSET_MAX            60
#define CAL_BAND_POWER_OFFSET_MIN            -60
#define CAL_RATE_POWER_OFFSET_MAX            7
#define CAL_RATE_POWER_OFFSET_MIN            -8
#define CAL_FREP_OFFSET_MAX                  127
#define CAL_FREP_OFFSET_MIN                  -128
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
#define HI_WIFI_MODE_11BGN                  0
#define HI_WIFI_MODE_11BG                   1
#define HI_WIFI_MODE_11B                    2
#endif

#define HI_CCA_THRESHOLD_LO (-128)
#define HI_CCA_THRESHOLD_HI 127

typedef hi_u32 (*wal_hipriv_cmd_func)(oal_net_device_stru *netdev, hi_char *pc_param);

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* 性能测试相关 */
typedef enum {
    WAL_ALWAYS_TX_DISABLE,      /* 禁用常发 */
    WAL_ALWAYS_TX_RF,           /* 保留给RF测试广播报文 */
    WAL_ALWAYS_TX_AMPDU_ENABLE, /* 使能AMPDU聚合包常发 */
    WAL_ALWAYS_TX_MPDU,         /* 使能非聚合包常发 */
    WAL_ALWAYS_TX_DC,           /* 使能DC常发,用于CE认证测频偏 */
    WAL_ALWAYS_TX_BUTT
} wal_device_always_tx_state_enum;

/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
/* 私有命令入口结构定义 */
typedef struct {
    hi_char *pc_cmd_name;     /* 命令字符串 */
    wal_hipriv_cmd_func func; /* 命令对应处理函数 */
} wal_hipriv_cmd_entry_stru;

/* 算法参数配置结构体 */
typedef struct {
    hi_char *pc_name;               /* 配置命令字符串 */
    mac_alg_cfg_enum_uint8 alg_cfg; /* 配置命令对应的枚举值 */
    hi_u8 auc_resv[3];              /* 3: 字节对齐 */
} wal_ioctl_alg_cfg_stru;

/* TPC工作模式 */
typedef enum {
    ALG_TPC_MODE_DISABLE = 0, /* 禁用TPC模式: 直接采用RF配置的功率增益 */
    ALG_TPC_MODE_FIX_POWER = 1, /* 固定功率模式: 数据帧的Data0采用配置的, Data1~3以及管理帧、控制帧都用最大功率 */
    /* 自适应功率模式: 数据帧的Data0采用自适应功率, Data1~3以及管理帧、控制帧都用最大功率 */
    ALG_TPC_MODE_ADAPT_POWER = 2,

    ALG_TPC_MODE_BUTT
} alg_tpc_mode_enum;
typedef hi_u8 alg_tpc_mode_enum_uint8;

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 wal_hipriv_set_rate(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_mcs(oal_net_device_stru *netdev, hi_char *pc_param);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
hi_u32 wal_hipriv_create_proc(hi_void *proc_arg);
#endif
#endif
hi_u32 wal_hipriv_del_vap(oal_net_device_stru *netdev);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 wal_hipriv_vap_info(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
hi_u32 wal_hipriv_remove_proc(hi_void);
#endif
#endif
hi_u32 wal_hipriv_get_mac_addr(const hi_char *pc_param, hi_u8 mac_addr[], hi_u8 addr_len, hi_u32 *pul_total_offset);
hi_u32 wal_hipriv_get_bw(oal_net_device_stru *netdev, hal_channel_assemble_enum_uint8 *pen_bw_index);
hi_u32 wal_hipriv_set_bw(oal_net_device_stru *netdev, hi_char *pc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 wal_hipriv_reg_write(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
hi_u32 wal_hipriv_getcountry(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_vap_state(oal_net_device_stru *netdev, hi_char *pc_param);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 wal_hipriv_get_mesh_node_info(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
hi_u32 wal_hipriv_sta_pm_on(oal_net_device_stru *netdev, const hi_char *pc_param);
#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
hi_u32 wal_hipriv_set_uapsd_cap(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_uapsd_para(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_uapsd_cap(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 wal_hipriv_sta_set_psm_offset(oal_net_device_stru *netdev, hi_char *param);
hi_u32 wal_hipriv_sta_set_offload_param(oal_net_device_stru *netdev, hi_char *param);
hi_u32 wal_hipriv_sta_set_hw_ps_mode(oal_net_device_stru *netdev, hi_char *param);
hi_u32 wal_hipriv_set_pm_switch(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
#endif
#if defined _PRE_WLAN_FEATURE_SIGMA
hi_u32 wal_hipriv_rts_threshold(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
hi_u32 wal_hipriv_frag_threshold(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
hi_u32 wal_hipriv_setcountry(oal_net_device_stru *netdev, hi_char *pc_param);
#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
hi_u32 wal_hipriv_ampdu_tx_on(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
hi_u32 wal_hipriv_set_shortgi20(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
#if defined _PRE_WLAN_FEATURE_SIGMA
hi_u32 wal_hipriv_set_stbc_cap(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_addba_req(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
hi_u32 wal_hipriv_entry(const hi_char *pc_buffer, hi_u32 count);
#endif
hi_u32 wal_hipriv_tx_proc(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_rx_proc(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_rx_fcs_info(oal_net_device_stru *netdev, hi_char *pc_param);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 wal_hipriv_user_info(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
hi_u32 wal_hipriv_get_netdev(const hi_char *pc_cmd, oal_net_device_stru **netdev, hi_u32 *pul_off_set);
hi_u32 wal_hipriv_always_tx(oal_net_device_stru *netdev, hi_u8 tx_flag);
hi_u32 wal_hipriv_always_rx(oal_net_device_stru *netdev, hi_u8 rx_flag, hi_u8 mac_filter_flag);
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 wal_hipriv_get_cal_data(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_cal_band_power(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_cal_rate_power(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_rate_power(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_cal_freq_power(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_dataefuse(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_set_customer_mac(oal_net_device_stru *netdev, hi_char *pc_param);
hi_u32 wal_hipriv_get_customer_mac(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
hi_u32 hi_hipriv_set_tx_pwr_offset(oal_net_device_stru *netdev, hi_char *pc_param);
#ifdef _PRE_WLAN_FEATURE_WOW
hi_u32 wal_get_add_wow_pattern_param(hi_u8 index, hi_char *pattern, hmac_cfg_wow_pattern_param_stru *cfg_wow_param);
#endif
hi_bool is_under_ps(hi_void);
hi_void set_under_ps(hi_bool under_ps);
hi_void set_under_mfg(hi_u32 under_mfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wal_hipriv.h */
