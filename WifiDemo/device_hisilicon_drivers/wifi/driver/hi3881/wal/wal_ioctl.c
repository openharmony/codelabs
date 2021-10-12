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

/* ****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "hmac_ext_if.h"
#include "wal_main.h"
#include "wal_ioctl.h"
#include "wal_event_msg.h"
#include "wal_hipriv.h"
#include "net_adpater.h"
#include "wal_11d.h"

#include "wal_customize.h"

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "lwip/netifapi.h"
#endif
#ifdef _PRE_WLAN_FEATURE_ANY
#include "hi_any_api.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_PRIV_CMD_SIZE 4096

/* ****************************************************************************
  私有命令函数表. 私有命令格式:
         设备名 命令名 参数
  hipriv "Hisilicon0 create vap0"
**************************************************************************** */
#define CMD_SET_AP_WPS_P2P_IE   "SET_AP_WPS_P2P_IE"
#define CMD_P2P_SET_NOA         "P2P_SET_NOA"
#define CMD_P2P_SET_PS          "P2P_SET_PS"
#define CMD_SET_POWER_ON        "SET_POWER_ON"
#define CMD_SET_POWER_MGMT_ON   "SET_POWER_MGMT_ON"
#define CMD_COUNTRY             "COUNTRY"
#define CMD_SET_QOS_MAP         "SET_QOS_MAP"
#define CMD_TX_POWER            "TX_POWER"
#define CMD_WPAS_GET_CUST       "WPAS_GET_CUST"
#define CMD_SET_SSID            "SET_SSID"

/* ****************************************************************************
  结构体定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef struct {
    hi_u32   ap_max_user;                      /* ap最大用户数 */
    hi_char  ac_ap_mac_filter_mode[257];          /* AP mac地址过滤命令参数,最长257 */
    hi_s32   ap_power_flag;                     /* AP上电标志 */
} wal_ap_config_stru;
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

/* ****************************************************************************
  2 函数声明
**************************************************************************** */
/* 静态函数声明 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_u32 wal_ioctl_get_mode(oal_net_device_stru *netdev, hi_s8 *pc_param);
static hi_u32 wal_ioctl_get_essid(oal_net_device_stru *netdev, hi_s8 *pc_param);
static hi_u32 wal_ioctl_get_bss_type(oal_net_device_stru *netdev, hi_s8 *pc_param);
static hi_u32 wal_ioctl_set_bss_type(oal_net_device_stru *netdev, hi_s8 *pc_param);
static hi_u32 wal_ioctl_get_freq(oal_net_device_stru *netdev, hi_s8 *pc_param);
static hi_u32 wal_ioctl_get_txpower(oal_net_device_stru *netdev, hi_s8 *pc_param);
static hi_u32 wal_ioctl_get_apaddr(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
                                   oal_sockaddr_stru *addr, hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_iwrate(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_param_stru *param, hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_iwsense(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_param_stru *param, hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_rtsthres(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_param_stru *param, hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_fragthres(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_param_stru *param, hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_iwencode(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_point_stru *param, hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_iwrange(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_point_stru *param, hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_param(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *iw,
    hi_s8 *pc_extra);
static hi_u32 wal_ioctl_set_param(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *iw,
    hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_iwname(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_s8 *pc_name,
    hi_s8 *pc_extra);
static hi_s32 wal_ioctl_set_wme_params(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *iw,
    hi_s8 *pc_extra);
static hi_u32 wal_ioctl_get_wme_params(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *iw,
    hi_s8 *pc_extra);
static hi_u32 wal_ioctl_setcountry(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *w,
    hi_s8 *pc_extra);
static hi_u32 wal_ioctl_getcountry(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *w,
    hi_s8 *pc_extra);
static hi_u32 wal_ioctl_reduce_sar(oal_net_device_stru *netdev, hi_u8 tx_power);
static hi_s32 wal_ioctl_set_ap_config(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iwreq_data_union *wrqu, hi_s8 *pc_extra);
static hi_s32 wal_ioctl_get_assoc_list(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iwreq_data_union *wrqu, hi_s8 *pc_extra);
static hi_s32 wal_ioctl_set_mac_filters(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iwreq_data_union *wrqu, hi_s8 *pc_extra);
static hi_s32 wal_ioctl_set_ap_sta_disassoc(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iwreq_data_union *wrqu, hi_s8 *pc_extra);
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_ethtool_ops_stru g_wal_ethtool_ops = { 0 };
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static wal_ap_config_stru g_st_ap_config_info = { 0 }; /* AP配置信息,需要在vap 创建后下发的 */

/* ****************************************************************************
  标准ioctl命令函数表.
**************************************************************************** */
static const oal_iw_handler g_ast_iw_handlers[] = {
    HI_NULL,                                 /* SIOCSIWCOMMIT, */
    (oal_iw_handler)wal_ioctl_get_iwname,    /* SIOCGIWNAME, */
    HI_NULL,                                 /* SIOCSIWNWID, */
    HI_NULL,                                 /* SIOCGIWNWID, */
    (oal_iw_handler)wal_ioctl_set_freq,      /* SIOCSIWFREQ, 设置频点/信道 */
    (oal_iw_handler)wal_ioctl_get_freq,      /* SIOCGIWFREQ, 获取频点/信道 */
    (oal_iw_handler)wal_ioctl_set_bss_type,  /* SIOCSIWMODE, 设置bss type */
    (oal_iw_handler)wal_ioctl_get_bss_type,  /* SIOCGIWMODE, 获取bss type */
    HI_NULL,                                 /* SIOCSIWSENS, */
    (oal_iw_handler)wal_ioctl_get_iwsense,   /* SIOCGIWSENS, */
    HI_NULL, /* SIOCSIWRANGE, */             /* not used */
    (oal_iw_handler)wal_ioctl_get_iwrange,   /* SIOCGIWRANGE, */
    HI_NULL, /* SIOCSIWPRIV, */              /* not used */
    HI_NULL, /* SIOCGIWPRIV, */              /* kernel code */
    HI_NULL, /* SIOCSIWSTATS, */             /* not used */
    HI_NULL,                                 /* SIOCGIWSTATS, */
    HI_NULL,                                 /* SIOCSIWSPY, */
    HI_NULL,                                 /* SIOCGIWSPY, */
    HI_NULL,                                 /* -- hole -- */
    HI_NULL,                                 /* -- hole -- */
    HI_NULL,                                 /* SIOCSIWAP, */
    (oal_iw_handler)wal_ioctl_get_apaddr,    /* SIOCGIWAP, */
    HI_NULL,                                 /* SIOCSIWMLME, */
    HI_NULL,                                 /* SIOCGIWAPLIST, */
    HI_NULL,                                 /* SIOCSIWSCAN, */
    HI_NULL,                                 /* SIOCGIWSCAN, */
    (oal_iw_handler)wal_ioctl_set_essid,     /* SIOCSIWESSID, 设置ssid */
    (oal_iw_handler)wal_ioctl_get_essid,     /* SIOCGIWESSID, 读取ssid */
    HI_NULL,                                 /* SIOCSIWNICKN */
    HI_NULL,                                 /* SIOCGIWNICKN */
    HI_NULL,                                 /* -- hole -- */
    HI_NULL,                                 /* -- hole -- */
    HI_NULL,                                 /* SIOCSIWRATE */
    (oal_iw_handler)wal_ioctl_get_iwrate,    /* SIOCGIWRATE */
    HI_NULL,                                 /* SIOCSIWRTS */
    (oal_iw_handler)wal_ioctl_get_rtsthres,  /* SIOCGIWRTS */
    HI_NULL,                                 /* SIOCSIWFRAG */
    (oal_iw_handler)wal_ioctl_get_fragthres, /* SIOCGIWFRAG */
    (oal_iw_handler)wal_ioctl_set_txpower,   /* SIOCSIWTXPOW, 设置传输功率限制 */
    (oal_iw_handler)wal_ioctl_get_txpower,   /* SIOCGIWTXPOW, 设置传输功率限制 */
    HI_NULL,                                 /* SIOCSIWRETRY */
    HI_NULL,                                 /* SIOCGIWRETRY */
    HI_NULL,                                 /* SIOCSIWENCODE */
    (oal_iw_handler)wal_ioctl_get_iwencode,  /* SIOCGIWENCODE */
    HI_NULL,                                 /* SIOCSIWPOWER */
    HI_NULL,                                 /* SIOCGIWPOWER */
    HI_NULL,                                 /* -- hole -- */
    HI_NULL,                                 /* -- hole -- */
    HI_NULL,                                 /* SIOCSIWGENIE */
    HI_NULL,                                 /* SIOCGIWGENIE */
    HI_NULL,                                 /* SIOCSIWAUTH */
    HI_NULL,                                 /* SIOCGIWAUTH */
    HI_NULL,                                 /* SIOCSIWENCODEEXT */
    HI_NULL                                  /* SIOCGIWENCODEEXT */
};

/* ****************************************************************************
  私有ioctl命令参数定义定义
**************************************************************************** */
#ifndef _PRE_HDF_LINUX
static const oal_iw_priv_args_stru g_ast_iw_priv_args[] = {
    { WAL_IOCTL_PRIV_SET_MODE,       OAL_IW_PRIV_TYPE_CHAR | 24, 0, "mode" },         /* 参数类型是char, 个数为24 */
    { WAL_IOCTL_PRIV_GET_MODE,       0, OAL_IW_PRIV_TYPE_CHAR | 24, "get_mode" },     /* 参数类型是char, 个数为24 */
    { WAL_IOCTL_PRIV_SET_COUNTRY,    OAL_IW_PRIV_TYPE_CHAR | 3, 0,  "setcountry" }, /* 参数类型是char, 个数为3 */
    { WAL_IOCTL_PRIV_GET_COUNTRY,    0, OAL_IW_PRIV_TYPE_CHAR | 3,  "getcountry" }, /* 参数类型是char, 个数为3 */

    { WAL_IOCTL_PRIV_SET_AP_CFG, OAL_IW_PRIV_TYPE_CHAR |  256, 0, "AP_SET_CFG" }, /* 参数类型是char, 个数为256 */
    { WAL_IOCTL_PRIV_AP_MAC_FLTR, OAL_IW_PRIV_TYPE_CHAR | 256,                   /* 参数类型是char, 个数为256 */
      OAL_IW_PRIV_TYPE_CHAR | OAL_IW_PRIV_SIZE_FIXED | 0, "AP_SET_MAC_FLTR"},
    { WAL_IOCTL_PRIV_AP_GET_STA_LIST, 0, OAL_IW_PRIV_TYPE_CHAR | 1024, "AP_GET_STA_LIST" }, /* 个数为1024 */
    { WAL_IOCTL_PRIV_AP_STA_DISASSOC, OAL_IW_PRIV_TYPE_CHAR | 256,                          /* 个数为256 */
      OAL_IW_PRIV_TYPE_CHAR | 0, "AP_STA_DISASSOC"},

    /* sub-ioctl函数入口 */
    { WAL_IOCTL_PRIV_SETPARAM,       OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, /* 个数为2 */
      0, "setparam"},
    {WAL_IOCTL_PRIV_GETPARAM,       OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
     OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "getparam"},

    /* sub-ioctl标志，name为'\0', 数字1表示set命令后面跟1个参数, get命令得到1个值 */
    {WAL_IOCTL_PRIV_SETPARAM,       OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, ""},
    {WAL_IOCTL_PRIV_SETPARAM,       OAL_IW_PRIV_TYPE_BYTE | OAL_IW_PRIV_SIZE_FIXED | OAL_IW_PRIV_TYPE_ADDR, 0, ""},
    {WAL_IOCTL_PRIV_GETPARAM,       0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "" },
    {WAL_IOCTL_PRIV_GETPARAM,       0, OAL_IW_PRIV_TYPE_BYTE | OAL_IW_PRIV_SIZE_FIXED | OAL_IW_PRIV_TYPE_ADDR, ""},
    {WLAN_CFGID_SHORTGI,            OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "shortgi20"},
    {WLAN_CFGID_SHORTGI,            0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_shortgi20"},
    {WLAN_CFGID_SHORT_PREAMBLE,     OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "shpreamble"},
    {WLAN_CFGID_SHORT_PREAMBLE,     0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_shpreamble"},
    {WLAN_CFGID_PROT_MODE,          OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "protmode"},
    {WLAN_CFGID_PROT_MODE,          0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_protmode"},
    {WLAN_CFGID_AUTH_MODE,          OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "authmode"},
    {WLAN_CFGID_AUTH_MODE,          0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_authmode"},
    {WLAN_CFGID_BEACON_INTERVAL,    OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "bintval"},
    {WLAN_CFGID_BEACON_INTERVAL,    0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_bintval"},
    {WLAN_CFGID_TID,                0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_tid"},
    /* U-APSD命令 */
    {WLAN_CFGID_UAPSD_EN,          OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "uapsden"},
    {WLAN_CFGID_UAPSD_EN,           0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_uapsden"},
    {WLAN_CFGID_DTIM_PERIOD,        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "dtim_period"},
    {WLAN_CFGID_DTIM_PERIOD,        0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_dtim_period"},

    /* EDCA参数配置命令 sub-ioctl入口 */
    {WAL_IOCTL_PRIV_SET_WMM_PARAM, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 3, 0, "setwmmparam"}, /* 个数为3 */
    {WAL_IOCTL_PRIV_GET_WMM_PARAM, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2,                    /* 个数为2 */
                                   OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, "getwmmparam"},   /* 个数为2 */

    /* sub-ioctl标志，name为'\0', 2表示set命令后跟两个参数 */
    {WAL_IOCTL_PRIV_SET_WMM_PARAM,       OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, ""},    /* 个数为2 */
    {WAL_IOCTL_PRIV_GET_WMM_PARAM,       OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
                                         OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "" },
    {WLAN_CFGID_EDCA_TABLE_CWMIN,        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, "cwmin"}, /* 个数为2 */
    {WLAN_CFGID_EDCA_TABLE_CWMIN,        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_cwmin"},
    {WLAN_CFGID_EDCA_TABLE_CWMAX,        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, "cwmax"}, /* 个数为2 */
    {WLAN_CFGID_EDCA_TABLE_CWMAX,        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_cwmax"},
    {WLAN_CFGID_EDCA_TABLE_AIFSN,        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, "aifsn"}, /* 个数为2 */
    {WLAN_CFGID_EDCA_TABLE_AIFSN,        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_aifsn"},
    {WLAN_CFGID_EDCA_TABLE_TXOP_LIMIT, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, "txoplimit"}, /* 个数为2 */
    {WLAN_CFGID_EDCA_TABLE_TXOP_LIMIT,       OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_txoplimit"},
    {WLAN_CFGID_EDCA_TABLE_MSDU_LIFETIME, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2,             /* 个数为2 */
        0, "lifetime"},
    {WLAN_CFGID_EDCA_TABLE_MSDU_LIFETIME,    OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_lifetime"},
    {WLAN_CFGID_EDCA_TABLE_MANDATORY, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, "mandatory"}, /* 个数为2 */
    {WLAN_CFGID_EDCA_TABLE_MANDATORY, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_mandatory"},

    {WLAN_CFGID_QEDCA_TABLE_CWMIN, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, "qcwmin"},   /* 个数为2 */
    {WLAN_CFGID_QEDCA_TABLE_CWMIN, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_qcwmin"},
    {WLAN_CFGID_QEDCA_TABLE_CWMAX, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, "qcwmax"},  /* 个数为2 */
    {WLAN_CFGID_QEDCA_TABLE_CWMAX, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_qcwmax"},
    {WLAN_CFGID_QEDCA_TABLE_AIFSN, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2, 0, "qaifsn"},   /* 个数为2 */
    {WLAN_CFGID_QEDCA_TABLE_AIFSN, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_qaifsn"},
    {WLAN_CFGID_QEDCA_TABLE_TXOP_LIMIT, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2,            /* 个数为2 */
        0, "qtxoplimit"},
    {WLAN_CFGID_QEDCA_TABLE_TXOP_LIMIT, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_qtxoplimit"},
    {WLAN_CFGID_QEDCA_TABLE_MSDU_LIFETIME, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2,         /* 个数为2 */
        0, "qlifetime"},
    {WLAN_CFGID_QEDCA_TABLE_MSDU_LIFETIME,    OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_qlifetime"},
    {WLAN_CFGID_QEDCA_TABLE_MANDATORY, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 2,            /* 个数为2 */
        0, "qmandatory"},
    {WLAN_CFGID_QEDCA_TABLE_MANDATORY, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,
        OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_qmandatory"},

};
#endif /* ifndef _PRE_HDF_LINUX */

/* ****************************************************************************
  私有ioctl命令函数表.
**************************************************************************** */
static const oal_iw_handler g_ast_iw_priv_handlers[] = {
    (oal_iw_handler)wal_ioctl_set_param, /* SIOCWFIRSTPRIV+0 */            /* sub-ioctl set 入口 */
    (oal_iw_handler)wal_ioctl_get_param, /* SIOCWFIRSTPRIV+1 */            /* sub-ioctl get 入口 */
    HI_NULL, /* SIOCWFIRSTPRIV+2 */                                        /* setkey */
    (oal_iw_handler)wal_ioctl_set_wme_params, /* SIOCWFIRSTPRIV+3 */       /* setwmmparams */
    HI_NULL, /* SIOCWFIRSTPRIV+4 */                                        /* delkey */
    (oal_iw_handler)wal_ioctl_get_wme_params, /* SIOCWFIRSTPRIV+5 */       /* getwmmparams */
    HI_NULL, /* SIOCWFIRSTPRIV+6 */                                        /* setmlme */
    HI_NULL, /* SIOCWFIRSTPRIV+7 */                                        /* getchaninfo */
    (oal_iw_handler)wal_ioctl_setcountry, /* SIOCWFIRSTPRIV+8 */           /* setcountry */
    (oal_iw_handler)wal_ioctl_getcountry, /* SIOCWFIRSTPRIV+9 */           /* getcountry */
    HI_NULL, /* SIOCWFIRSTPRIV+10 */                                       /* addmac */
    HI_NULL, /* SIOCWFIRSTPRIV+11 */                                       /* getscanresults */
    HI_NULL, /* SIOCWFIRSTPRIV+12 */                                       /* delmac */
    HI_NULL, /* SIOCWFIRSTPRIV+13 */                                       /* getchanlist */
    HI_NULL, /* SIOCWFIRSTPRIV+14 */                                       /* setchanlist */
    HI_NULL, /* SIOCWFIRSTPRIV+15 */                                       /* kickmac */
    HI_NULL, /* SIOCWFIRSTPRIV+16 */                                       /* chanswitch */
    (oal_iw_handler)wal_ioctl_get_mode, /* SIOCWFIRSTPRIV+17 */            /* 获取模式, 例: iwpriv vapN get_mode */
    (oal_iw_handler)wal_ioctl_set_mode, /* SIOCWFIRSTPRIV+18 */            /* 设置模式, 例: iwpriv vapN mode 11g */
    HI_NULL, /* SIOCWFIRSTPRIV+19 */                                       /* getappiebuf */
    HI_NULL, /* SIOCWFIRSTPRIV+20 */                                       /* null */
    (oal_iw_handler)wal_ioctl_get_assoc_list, /* SIOCWFIRSTPRIV+21 */      /* APUT取得关联STA列表 */
    (oal_iw_handler)wal_ioctl_set_mac_filters, /* SIOCWFIRSTPRIV+22 */     /* APUT设置STA过滤 */
    (oal_iw_handler)wal_ioctl_set_ap_config, /* SIOCWFIRSTPRIV+23 */       /* 设置APUT参数 */
    (oal_iw_handler)wal_ioctl_set_ap_sta_disassoc, /* SIOCWFIRSTPRIV+24 */ /* APUT去关联STA */
    HI_NULL, /* SIOCWFIRSTPRIV+25 */                                       /* getStatistics */
    HI_NULL, /* SIOCWFIRSTPRIV+26 */                                       /* sendmgmt */
    HI_NULL, /* SIOCWFIRSTPRIV+27 */                                       /* null  */
    HI_NULL, /* SIOCWFIRSTPRIV+28 */                                       /* null */
    HI_NULL, /* SIOCWFIRSTPRIV+29 */                                       /* getaclmac */
    HI_NULL, /* SIOCWFIRSTPRIV+30 */                                       /* sethbrparams */
    HI_NULL, /* SIOCWFIRSTPRIV+29 */                                       /* getaclmac */
    HI_NULL, /* SIOCWFIRSTPRIV+30 */                                       /* sethbrparams */
    HI_NULL, /* SIOCWFIRSTPRIV+31 */                                       /* setrxtimeout */
};

/* ****************************************************************************
  无线配置iw_handler_def定义
**************************************************************************** */
#ifndef _PRE_HDF_LINUX
oal_iw_handler_def_stru g_iw_handler_def =
{
    .standard           = g_ast_iw_handlers,
    .num_standard       = hi_array_size(g_ast_iw_handlers),
    .private            = g_ast_iw_priv_handlers,
    .num_private        = hi_array_size(g_ast_iw_priv_handlers),
    .private_args       = g_ast_iw_priv_args,
    .num_private_args   = hi_array_size(g_ast_iw_priv_args),
    .get_wireless_stats = HI_NULL
};
#endif
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

/* ****************************************************************************
  协议模式字符串定义
**************************************************************************** */
const wal_ioctl_mode_map_stru g_ast_mode_map[] = {
    {"11b",         WLAN_LEGACY_11B_MODE,       WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
    {"11bg",        WLAN_MIXED_ONE_11G_MODE,    WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
    {"11bgn",       WLAN_HT_MODE,               WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
    { HI_NULL, 0, 0, 0, { 0 } }
};

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_iw_handler_def_stru *wal_get_g_iw_handler_def(hi_void)
{
#ifndef _PRE_HDF_LINUX
    return &g_iw_handler_def;
#else
    return NULL;
#endif
}
#endif
/* ****************************************************************************
 函 数 名  : wal_get_cmd_one_arg
 功能描述  : 获取字符串第一个参数 以空格为参数区分标识
 输入参数  : pc_cmd         : 传入的字符串
 输出参数  : pc_arg         : 第一个参数
             pul_cmd_offset : 第一个参数的长度
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年12月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_get_cmd_one_arg(const hi_char *pc_cmd, hi_char *pc_arg, hi_u32 pc_arg_len, hi_u32 *pul_cmd_offset)
{
    const hi_char *pc_cmd_copy = HI_NULL;
    hi_u32 pos = 0;

    if (oal_unlikely((pc_cmd == HI_NULL) || (pc_arg == HI_NULL) || (pul_cmd_offset == HI_NULL))) {
        oam_error_log3(0, 0,
            "{wal_get_cmd_one_arg::pc_cmd/pc_arg/pul_cmd_offset null ptr error %p, %p, %p!}\r\n",
            (uintptr_t)pc_cmd, (uintptr_t)pc_arg, (uintptr_t)pul_cmd_offset);
        return HI_ERR_CODE_PTR_NULL;
    }

    pc_cmd_copy = pc_cmd;

    /* 去掉字符串开始的空格 */
    while (*pc_cmd_copy == ' ') {
        ++pc_cmd_copy;
    }

    while ((*pc_cmd_copy != ' ') && (*pc_cmd_copy != '\0')) {
        pc_arg[pos] = *pc_cmd_copy;
        ++pos;
        ++pc_cmd_copy;

        if (oal_unlikely(pos >= pc_arg_len)) {
            oam_warning_log1(0, 0, "{wal_get_cmd_one_arg::ul_pos >= WAL_HIPRIV_CMD_NAME_MAX_LEN, ul_pos %d!}\r\n", pos);
            return HI_ERR_CODE_ARRAY_OVERFLOW;
        }
    }

    pc_arg[pos] = '\0';

    /* 字符串到结尾，返回错误码 */
    if (0 == pos) {
        oam_info_log0(0, 0, "{wal_get_cmd_one_arg::return param pc_arg is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    *pul_cmd_offset = (hi_u32)(pc_cmd_copy - pc_cmd);

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_P2P
/* ****************************************************************************
 函 数 名  : wal_wireless_iftype_to_mac_p2p_mode
 输入参数  : enum nl80211_iftype en_iftype
 输出参数  : 无
 返 回 值  : wlan_p2p_mode_enum_uint8
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
wlan_p2p_mode_enum_uint8 wal_wireless_iftype_to_mac_p2p_mode(nl80211_iftype_uint8 iftype)
{
    wlan_p2p_mode_enum_uint8 p2p_mode = WLAN_LEGACY_VAP_MODE;

    switch (iftype) {
        case NL80211_IFTYPE_P2P_CLIENT:
            p2p_mode = WLAN_P2P_CL_MODE;
            break;
        case NL80211_IFTYPE_P2P_GO:
            p2p_mode = WLAN_P2P_GO_MODE;
            break;
        case NL80211_IFTYPE_P2P_DEVICE:
            p2p_mode = WLAN_P2P_DEV_MODE;
            break;
#ifdef _PRE_WLAN_FEATURE_MESH
        case NL80211_IFTYPE_MESH_POINT:
#endif
        case NL80211_IFTYPE_AP:
        case NL80211_IFTYPE_STATION:
            p2p_mode = WLAN_LEGACY_VAP_MODE;
            break;
        default:
            p2p_mode = WLAN_P2P_BUTT;
    }
    return p2p_mode;
}
#endif

/* ****************************************************************************
 功能描述  : cfg vap h2d
 输入参数  : pst_net_dev: net_device
 修改历史      :
  1.日    期   : 2015年6月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 wal_cfg_vap_h2d_event(oal_net_device_stru *netdev)
{
    hi_unref_param(netdev);
    oal_net_device_stru         *netdev_cfg = HI_NULL;
    hi_u32                       ret;
    wal_msg_stru                *rsp_msg = HI_NULL;
    wal_msg_write_stru           write_msg;

    netdev_cfg = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev_cfg == HI_NULL) {
        oam_warning_log0(0, 0, "{wal_init_wlan_vap::pst_cfg_net_dev is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev_cfg);
#endif
    /* **************************************************************************
    抛事件到wal层处理
    ************************************************************************** */
    /* 填写消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_CFG_VAP_H2D, sizeof(mac_cfg_vap_h2d_stru));
    ((mac_cfg_vap_h2d_stru *)write_msg.auc_value)->net_dev = netdev_cfg;

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev_cfg, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_vap_h2d_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_cfg_vap_h2d_event::wal_alloc_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }

    /* 处理返回消息 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_cfg_vap_h2d_event::hmac cfg vap h2d fail,err code[%u]\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/* ****************************************************************************
 函 数 名  : wal_host_dev_config
 功能描述  : 02 host device_sruc配置接口，目前用于上下电流程
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 wal_host_dev_config(const oal_net_device_stru *netdev, wlan_cfgid_enum_uint16 wid)
{
    oal_wireless_dev           *wdev = HI_NULL;
    mac_wiphy_priv_stru        *wiphy_priv = HI_NULL;
    hmac_vap_stru              *hmac_vap = HI_NULL;
    mac_device_stru            *mac_dev = HI_NULL;
    oal_net_device_stru        *netdev_cfg = HI_NULL;
    hi_u32                      ret;
    wal_msg_stru               *rsp_msg = HI_NULL;
    wal_msg_write_stru          write_msg;

    wdev = (oal_wireless_dev *)oal_netdevice_wdev(netdev);
    if ((wdev == HI_NULL) || (wdev->wiphy == HI_NULL)) {
        oam_warning_log0(0, 0, "{wal_init_wlan_vap::pst_wdev is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    wiphy_priv = (mac_wiphy_priv_stru *)oal_wiphy_priv(wdev->wiphy);
    mac_dev = wiphy_priv->mac_device;
    hmac_vap = hmac_vap_get_vap_stru(WLAN_CFG_VAP_ID);
    if (mac_dev == HI_NULL || hmac_vap == HI_NULL || hmac_vap->net_device == HI_NULL) {
        oam_warning_log2(0, 0, "{wal_init_wlan_vap::mac_dev/hmac_vap is null! mac_dev=%p,hmac_vap=%p}",
            (uintptr_t)mac_dev, (uintptr_t)hmac_vap);
        return HI_ERR_CODE_PTR_NULL;
    }
    netdev_cfg = hmac_vap->net_device;

    /* **************************************************************************
    抛事件到wal层处理
    ************************************************************************** */
    /* 填写消息 */
    wal_write_msg_hdr_init(&write_msg, wid, 0);

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev_cfg, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH, (hi_u8 *)&write_msg, HI_TRUE,
        &rsp_msg);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_cfg_vap_h2d_event::wal_alloc_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }

    /* 处理返回消息 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_cfg_vap_h2d_event::hmac cfg vap h2d fail,err code[%u]\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_host_dev_init
 功能描述  : 02 host device_sruc的初始化接口，目前用于上下电流程
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_host_dev_init(const oal_net_device_stru *netdev)
{
    return wal_host_dev_config(netdev, WLAN_CFGID_HOST_DEV_INIT);
}

/* ****************************************************************************
 函 数 名  : wal_host_dev_init
 功能描述  : 02 host device_sruc的去初始化接口，目前用于上下电流程
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_host_dev_exit(const oal_net_device_stru *netdev)
{
    return wal_host_dev_config(netdev, WLAN_CFGID_HOST_DEV_EXIT);
}
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_u32 wal_ioctl_get_mode(oal_net_device_stru *netdev, hi_s8 *pc_param)
{
    return HI_FAIL;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_ioctl_set_mode
 功能描述  : 设置模式: 包括协议、频段、带宽
 输入参数  : pst_net_dev: net device
             pc_param    : 参数
 输出参数  : 无
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_ioctl_set_mode(oal_net_device_stru *netdev, hi_char *pc_param)
{
    hi_char            ac_mode_str[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};     /* 预留协议模式字符串空间 */
    hi_u8              prot_idx;
    wal_msg_write_stru write_msg;
    hi_u32             off_set = 0;

    if (oal_unlikely((netdev == HI_NULL) || (pc_param == HI_NULL))) {
        oam_error_log2(0, 0, "{wal_ioctl_set_mode::pst_net_dev/p_param null ptr error %p %p!}\r\n", (uintptr_t)netdev,
            (uintptr_t)pc_param);
        return HI_ERR_WIFI_WAL_FAILURE;
    }

    /* 设备在up状态不允许配置，必须先down */
    if (0 != (OAL_IFF_RUNNING & oal_netdevice_flags(netdev))) {
        oam_error_log1(0, 0, "{wal_ioctl_set_mode::device busy %d!}\r\n", oal_netdevice_flags(netdev));
        return HI_ERR_WIFI_WAL_BUSY;
    }

    /* pc_param指向传入模式参数, 将其取出存放到ac_mode_str中 */
    hi_u32 ret = wal_get_cmd_one_arg(pc_param, ac_mode_str, WAL_HIPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_mode::mode get failed [%d]!}\r\n", ret);
        return ret;
    }
    ac_mode_str[sizeof(ac_mode_str) - 1] = '\0'; /* 确保以null结尾 */

    for (prot_idx = 0; HI_NULL != g_ast_mode_map[prot_idx].pc_name; prot_idx++) {
        if (0 == strcmp(g_ast_mode_map[prot_idx].pc_name, ac_mode_str)) {
            break;
        }
    }

    if (HI_NULL == g_ast_mode_map[prot_idx].pc_name) {
        oam_warning_log0(0, 0, "{wal_ioctl_set_mode::unrecognized protocol string!}\r\n");
        return HI_FAIL;
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_MODE, sizeof(mac_cfg_mode_param_stru));

    mac_cfg_mode_param_stru *mode_param = (mac_cfg_mode_param_stru *)(write_msg.auc_value);
    mode_param->protocol     = g_ast_mode_map[prot_idx].mode;
    mode_param->band         = g_ast_mode_map[prot_idx].band;
    mode_param->en_bandwidth = g_ast_mode_map[prot_idx].en_bandwidth;

    oam_info_log3(0, OAM_SF_CFG, "{wal_ioctl_set_mode::protocol[%d],band[%d],bandwidth[%d]!}", mode_param->protocol,
        mode_param->band, mode_param->en_bandwidth);

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_mode_param_stru),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_mode::wal_alloc_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_u32 wal_ioctl_get_essid(oal_net_device_stru *netdev, hi_s8 *pc_param)
{
    return HI_FAIL;
}
#endif

/* ****************************************************************************
 函 数 名  : oal_strim
 功能描述  : 去掉字符串开始与结尾的空格
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_char *oal_strim(hi_char *str, hi_u8 max_cmd_len)
{
    hi_u32   size;
    hi_char* str_end = HI_NULL;

    hi_unref_param(max_cmd_len);
    while (*str == ' ') {
        ++str;
    }

    size = strlen((const hi_char *)str);
    if (!size) {
        return str;
    }

    str_end = str + size - 1;
    while ((str_end >= str) && (*str_end == ' ')) {
        str_end--;
    }

    *(str_end + 1) = '\0';

    return str;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_set_essid
 功能描述  : 设置ssid
 输入参数  : [1]net_dev
             [2]pc_param
 输出参数  : 无
 返 回 值  : static hi_u32
**************************************************************************** */
/* 结构体数组g_ast_hipriv_cmd的成员，其中wal_hipriv_getcountry修改了对应变量，lint_t e818告警屏蔽 */
hi_u32 wal_ioctl_set_essid(oal_net_device_stru *netdev, hi_char *pc_param)
{
    wal_msg_write_stru  write_msg;
    hi_u32              off_set;
    hi_char             ac_ssid[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};
    mac_vap_stru       *mac_vap = oal_net_dev_priv(netdev);

    if (mac_vap == NULL) {
        oam_warning_log0(0, 0, "{wal_ioctl_set_essid::pst_mac_vap is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        /* 设备在up状态且是AP时，不允许配置，必须先down */
        if (0 != (OAL_IFF_RUNNING & oal_netdevice_flags(netdev))) {
            oam_error_log1(mac_vap->vap_id, 0, "{wal_ioctl_set_essid::device is busy, please down it firste %d!}",
                oal_netdevice_flags(netdev));
            return HI_FAIL;
        }
    }

    /* pc_param指向传入模式参数, 将其取出存放到ac_mode_str中 */
    hi_u32 ret = wal_get_cmd_one_arg(pc_param, ac_ssid, WAL_HIPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_essid::wal_get_cmd_one_arg vap name return err_code %d!}", ret);
        return ret;
    }

    hi_char *pc_ssid  = oal_strim(ac_ssid, WAL_HIPRIV_CMD_NAME_MAX_LEN); /* 去掉字符串开始结尾的空格 */
    hi_u8    ssid_len = (hi_u8)strlen(pc_ssid);
    oam_info_log1(mac_vap->vap_id, 0, "{wal_ioctl_set_essid:: ssid length %d!}", ssid_len);
    ssid_len = (ssid_len > WLAN_SSID_MAX_LEN - 1) ? (WLAN_SSID_MAX_LEN - 1) : ssid_len; /* -1为\0预留空间 */
    oam_info_log1(mac_vap->vap_id, 0, "{wal_ioctl_set_essid:: ssid length is %d!}", ssid_len);

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SSID, sizeof(mac_cfg_ssid_param_stru));

    /* 填写WID对应的参数 */
    mac_cfg_ssid_param_stru *param = (mac_cfg_ssid_param_stru *)(write_msg.auc_value);
    param->ssid_len = ssid_len;
    if (memcpy_s(param->ac_ssid, WLAN_SSID_MAX_LEN, pc_ssid, ssid_len) != EOK) {
        oam_error_log0(0, 0, "{wal_ioctl_set_essid::mem safe function err!}");
        return HI_FAIL;
    }

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_ssid_param_stru),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, 0, "{wal_ioctl_set_essid:: wal_alloc_cfg_event Err=%u}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_u32 wal_ioctl_get_bss_type(oal_net_device_stru *netdev, hi_s8 *pc_param)
{
    return HI_FAIL;
}
static hi_u32 wal_ioctl_set_bss_type(oal_net_device_stru *netdev, hi_s8 *pc_param)
{
    return HI_FAIL;
}
static hi_u32 wal_ioctl_get_freq(oal_net_device_stru *netdev, hi_s8 *pc_param)
{
    return HI_FAIL;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_ioctl_set_freq
 功能描述  : 设置频点/信道
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
/* 结构体数组g_ast_hipriv_cmd的成员，其中wal_hipriv_getcountry修改了对应变量，lint_t e818告警屏蔽 */
hi_u32 wal_ioctl_set_freq(oal_net_device_stru *netdev, hi_char *pc_param)
{
    wal_msg_write_stru      write_msg;
    hi_s32                  channel;
    hi_u32                  off_set;
    hi_char                 ac_freq[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};
    hi_u32                  ret;

    /* 设备在up状态不允许配置，必须先down */
    if (0 != (OAL_IFF_RUNNING & oal_netdevice_flags(netdev))) {
        oam_error_log1(0, 0, "{wal_ioctl_set_freq::device is busy, please down it first %d!}\r\n",
            oal_netdevice_flags(netdev));
        return HI_ERR_WIFI_WAL_BUSY;
    }

    /* pc_param指向新创建的net_device 的name, 将其取出存放到ac_name中 */
    ret = wal_get_cmd_one_arg(pc_param, ac_freq, WAL_HIPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_freq::wal_get_cmd_one_arg vap name return err_code %u!}\r\n", ret);
        return ret;
    }

    channel = oal_atoi(ac_freq);
    if ((channel < 1) || (channel > 14)) { /* 信道号最大为14 */
        oam_warning_log1(0, 0, "{wal_ioctl_set_freq::channel set fail = %d!}\r\n", channel);
        return HI_FAIL;
    }
    oam_info_log1(0, 0, "{wal_ioctl_set_freq::channel = %d!}\r\n", channel);
    ret = mac_is_channel_num_valid(WLAN_BAND_2G, channel);
    if (ret != HI_SUCCESS) {
        oam_error_log2(0, OAM_SF_CFG, "{wal_ioctl_set_freq::channel=%d,Err=%d}", channel, ret);
        return HI_FAIL;
    }
    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    /* 填写消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_CURRENT_CHANEL, sizeof(hi_s32));
    *((hi_s32 *)(write_msg.auc_value)) = channel;

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_freq::return err code %d!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_u32 wal_ioctl_get_txpower(oal_net_device_stru *netdev, hi_s8 *pc_param)
{
    return HI_FAIL;
}
#endif

/* 结构体数组g_ast_hipriv_cmd的成员，其中wal_hipriv_getcountry修改了对应变量，lint_t e818告警屏蔽 */
hi_u32 wal_ioctl_set_txpower(oal_net_device_stru *netdev, hi_char *pc_param)
{
    wal_msg_write_stru       write_msg;
    hi_s32                   l_pwer;
    hi_u32                   off_set;
    hi_char                  ac_val[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};
    hi_u32                   ret;

    ret = wal_get_cmd_one_arg(pc_param, ac_val, WAL_HIPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_txpower::wal_get_cmd_one_arg vap name return err_code %d!}\r\n", ret);
        return ret;
    }

    l_pwer = oal_atoi(ac_val);

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_TX_POWER, sizeof(hi_s32));
    *((hi_s32 *)(write_msg.auc_value)) = l_pwer;

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_txpower::return err code %u!}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 获取字符串第一个参数，以逗号为命令区分标识，并扣除命令前置字符
 输入参数  : pc_cmd         : 传入的字符串
             puc_token      : 命令前置字串
 输出参数  : pc_arg         : 扣掉命令前置字串后的参数
             pul_cmd_offset : 第一个参数的长度 调用函数  :
**************************************************************************** */
static hi_u32 wal_get_parameter_from_cmd(hi_char *pc_cmd, hi_char *pc_arg, const hi_char *puc_token,
    hi_char *pul_cmd_offset, hi_u32 ul_param_max_len)
{
    hi_char   *pc_cmd_copy = HI_NULL;
    hi_char    ac_cmd_copy[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN];
    hi_u32     pos = 0;
    hi_u32     arg_len;

    if (oal_unlikely((pc_cmd == HI_NULL) || (pc_arg == HI_NULL) || (pul_cmd_offset == HI_NULL))) {
        oam_error_log3(0, 0,
            "{wal_get_parameter_from_cmd::pc_cmd/pc_arg/pul_cmd_offset null ptr error %d, %d, %d, %d!}\r\n", pc_cmd,
            pc_arg, pul_cmd_offset);
        return HI_FAIL;
    }

    pc_cmd_copy = pc_cmd;

    /* 去掉字符串开始的逗号 */
    while (*pc_cmd_copy == ',') {
        ++pc_cmd_copy;
    }
    /* 取得逗号前的字符串 */
    while ((*pc_cmd_copy != ',') && (*pc_cmd_copy != '\0')) {
        ac_cmd_copy[pos] = *pc_cmd_copy;
        ++pos;
        ++pc_cmd_copy;

        if (oal_unlikely(pos >= ul_param_max_len)) {
            oam_warning_log1(0, 0,
                "{wal_get_parameter_from_cmd::ul_pos >= WAL_HIPRIV_CMD_NAME_MAX_LEN, ul_pos %d!}\r\n", pos);
            return HI_FAIL;
        }
    }
    ac_cmd_copy[pos] = '\0';
    /* 字符串到结尾，返回错误码 */
    if (0 == pos) {
        oam_info_log0(0, 0, "{wal_get_parameter_from_cmd::return param pc_arg is null!}\r\n");
        return HI_FAIL;
    }
    *pul_cmd_offset = (hi_u32)(pc_cmd_copy - pc_cmd);

    /* 检查字符串是否包含期望的前置命令字符 */
    if (0 != memcmp(ac_cmd_copy, puc_token, strlen(puc_token))) {
        return HI_FAIL;
    } else {
        /* 扣除前置命令字符，回传参数 */
        arg_len = strlen(ac_cmd_copy) - strlen(puc_token);
        memcpy_s(pc_arg, arg_len, ac_cmd_copy + strlen(puc_token), arg_len);
        pc_arg[arg_len] = '\0';
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_apaddr
 功能描述  : 获取BSSID
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_apaddr(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, oal_sockaddr_stru *addr,
    hi_s8 *pc_extra)
{
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u8          mac_addr[WLAN_MAC_ADDR_LEN] = {0};

    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_warning_log0(0, 0, "{wal_ioctl_get_apaddr::pst_mac_vap is null!}\r\n");
        return HI_FAIL;
    }

    if (mac_vap->vap_state == MAC_VAP_STATE_UP) {
        if (memcpy_s(addr->sa_data, WLAN_MAC_ADDR_LEN, mac_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, 0, "{wal_ioctl_get_apaddr::mem safe function err!}");
            return HI_FAIL;
        }
    } else {
        if (memcpy_s(addr->sa_data, WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, 0, "{wal_ioctl_get_apaddr::mem safe function err!}");
            return HI_FAIL;
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_iwrate
 功能描述  : iwconfig获取rate，不支持，返回-1
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_iwrate(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_param_stru *param, hi_s8 *pc_extra)
{
    /* iwconfig获取rate，不支持此命令，则返回-1 */
    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_iwrate
 功能描述  : iwconfig获取sense，不支持，返回-1
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_iwsense(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_param_stru *param, hi_s8 *pc_extra)
{
    /* iwconfig获取sense，不支持此命令，则返回-1 */
    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_rtsthres
 功能描述  : iwconfig获取rtsthres
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_rtsthres(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_param_stru *param, hi_s8 *pc_extra)
{
    mac_vap_stru *mac_vap = HI_NULL;

    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_warning_log0(0, 0, "{wal_ioctl_get_rtsthres::pst_mac_vap is null!}\r\n");
        return HI_FAIL;
    }

    param->value    = (hi_s32)mac_vap->mib_info->wlan_mib_operation.dot11_rts_threshold;
    param->disabled = (WLAN_RTS_MAX == param->value);
    param->fixed    = 1;

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_fragthres
 功能描述  : iwconfig获取fragthres
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_fragthres(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_param_stru *param, hi_s8 *pc_extra)
{
    mac_vap_stru *mac_vap = HI_NULL;

    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_warning_log0(0, 0, "{wal_ioctl_get_fragthres::pst_mac_vap is null!}\r\n");
        return HI_FAIL;
    }

    param->value    = (hi_s32)mac_vap->mib_info->wlan_mib_operation.dot11_fragmentation_threshold;
    param->disabled = (WLAN_FRAG_THRESHOLD_MAX == param->value);
    param->fixed    = 1;

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_iwencode
 功能描述  : iwconfig获取encode, 不支持
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_iwencode(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_point_stru *param, hi_s8 *pc_extra)
{
    /* 不支持iwconfig获取encode，直接返回-1 */
    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_iwrange
 功能描述  : iwconfig获取iwrange, 不支持
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_iwrange(oal_net_device_stru *netdev, oal_iw_request_info_stru *info,
    oal_iw_point_stru *param, hi_s8 *pc_extra)
{
    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_param
 功能描述  : iwpriv私有获取参数命令入口
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_param(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *iw,
    hi_s8 *pc_extra)
{
    hi_u32                       ret;
    wal_msg_stru                *rsp_msg = HI_NULL;
    wal_msg_query_stru           query_msg;
    wal_msg_rsp_stru            *query_rsp_msg = HI_NULL;
    hi_s32                      *pl_param = HI_NULL;

    pl_param = (hi_s32 *)pc_extra;
    oam_info_log1(0, 0, "{wal_ioctl_get_param::return err code %d!}\r\n", pl_param[0]);

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    query_msg.wid = (hi_u16)pl_param[0];

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_QUERY, WAL_MSG_WID_LENGTH, (hi_u8 *)&query_msg, HI_TRUE, &rsp_msg);
    if ((ret != HI_SUCCESS) || (rsp_msg == HI_NULL)) {
        oam_warning_log1(0, 0, "{wal_ioctl_get_param::return err code %u!}\r\n", ret);
        return ret;
    }

    /* 处理返回消息 */
    query_rsp_msg = (wal_msg_rsp_stru *)(rsp_msg->auc_msg_data);

    /* 业务处理 */
    pl_param[0] = *((hi_s32 *)(query_rsp_msg->auc_value));

    oal_free(rsp_msg);
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_set_param
 功能描述  : iwpriv私有设置参数命令入口
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_set_param(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *iw,
    hi_s8 *pc_extra)
{
    hi_s32 l_error = 0;
    wal_msg_write_stru write_msg;
    hi_u32 ret;

    /* 设备在up状态不允许配置，必须先down */
    if ((OAL_IFF_RUNNING & oal_netdevice_flags(netdev)) != 0) {
        oam_error_log1(0, 0, "{wal_ioctl_set_param::device is busy, please down it first %d!}\r\n",
            oal_netdevice_flags(netdev));
        return HI_FAIL;
    }

    hi_s32 *pl_param      = (hi_s32 *)pc_extra;
    hi_s32 l_subioctl_id  = pl_param[0];    /* 获取sub-ioctl的ID */
    hi_s32 l_value        = pl_param[1];    /* 获取要设置的值 */
    oam_info_log2(0, 0,  "{wal_ioctl_set_param::subioctl_id, value is %d, %d!}\r\n", l_subioctl_id, l_value);

    if (l_value < 0) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_param::input value is negative %d!}\r\n", l_value);
        return HI_FAIL;
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, (hi_u16)l_subioctl_id, sizeof(hi_s32));
    if (l_subioctl_id == WLAN_CFGID_PROT_MODE) {
        if (l_value >= WLAN_PROT_BUTT) { /* 参数检查 */
            l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
        }
    } else if (l_subioctl_id == WLAN_CFGID_AUTH_MODE) {
        if (l_value >= WLAN_WITP_ALG_AUTH_BUTT) { /* 参数检查 */
            l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
        }
    } else if (l_subioctl_id == WLAN_CFGID_BEACON_INTERVAL) {
        if (l_value > WLAN_BEACON_INTVAL_MAX || l_value < WLAN_BEACON_INTVAL_MIN) {
            l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
        }
    } else if (l_subioctl_id == WLAN_CFGID_DTIM_PERIOD) {
        if (l_value > WLAN_DTIM_PERIOD_MAX || l_value < WLAN_DTIM_PERIOD_MIN) {
            oam_error_log1(0, 0, "{wal_ioctl_set_param::input dtim_period invalid %d!}\r\n", l_value);
            l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
        }
    }

    if (l_error != 0) { /* 参数异常 */
        return l_error;
    }

    *((hi_s32 *)(write_msg.auc_value)) = l_value; /* 填写set消息的payload */

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_param::return err code %u!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* 编程规范规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 参数有效性判断，逻辑简单，功能聚合性不拆分 */
static hi_u32 wal_ioctl_check_wme_params(hi_s32 l_subioctl_id, hi_s32 l_value)
{
    hi_s32 l_error = 0;
    switch (l_subioctl_id) { /* 根据sub-ioctl id填写WID */
        case WLAN_CFGID_EDCA_TABLE_CWMIN:
            if ((l_value > WLAN_QEDCA_TABLE_CWMIN_MAX) || (l_value < WLAN_QEDCA_TABLE_CWMIN_MIN)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_EDCA_TABLE_CWMAX:
            if ((l_value > WLAN_QEDCA_TABLE_CWMAX_MAX) || (l_value < WLAN_QEDCA_TABLE_CWMAX_MIN)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_EDCA_TABLE_AIFSN:
            if ((l_value < WLAN_QEDCA_TABLE_AIFSN_MIN) || (l_value > WLAN_QEDCA_TABLE_AIFSN_MAX)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_EDCA_TABLE_TXOP_LIMIT:
            if ((l_value > WLAN_QEDCA_TABLE_TXOP_LIMIT_MAX) || (l_value < WLAN_QEDCA_TABLE_TXOP_LIMIT_MIN)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_EDCA_TABLE_MSDU_LIFETIME:
            if (l_value > WLAN_QEDCA_TABLE_MSDU_LIFETIME_MAX) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_EDCA_TABLE_MANDATORY:
            if ((l_value != HI_TRUE) && (l_value != HI_FALSE)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_QEDCA_TABLE_CWMIN:
            if ((l_value > WLAN_QEDCA_TABLE_CWMIN_MAX) || (l_value < WLAN_QEDCA_TABLE_CWMIN_MIN)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_QEDCA_TABLE_CWMAX:
            if ((l_value > WLAN_QEDCA_TABLE_CWMAX_MAX) || (l_value < WLAN_QEDCA_TABLE_CWMAX_MIN)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_QEDCA_TABLE_AIFSN:
            if ((l_value < WLAN_QEDCA_TABLE_AIFSN_MIN) || (l_value > WLAN_QEDCA_TABLE_AIFSN_MAX)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_QEDCA_TABLE_TXOP_LIMIT:
            if (l_value > WLAN_QEDCA_TABLE_TXOP_LIMIT_MAX) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_QEDCA_TABLE_MSDU_LIFETIME:
            if (l_value > WLAN_QEDCA_TABLE_MSDU_LIFETIME_MAX) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }
            break;

        case WLAN_CFGID_QEDCA_TABLE_MANDATORY:
            if ((l_value != HI_TRUE) && (l_value != HI_FALSE)) {
                l_error = HI_ERR_WIFI_WAL_INVALID_PARAMETER;
            }

            break;

        default:
            break;
    }

    if (l_error != 0) { /* 参数异常 */
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_set_wme_params
 功能描述  : iwpriv私有设置参数命令入口
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_s32 wal_ioctl_set_wme_params(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *iw,
    hi_s8 *pc_extra)
{
    hi_u32                       ret;
    hi_s32                      *pl_param = HI_NULL;
    hi_s32                       l_subioctl_id;
    hi_s32                       l_ac;
    hi_s32                       l_value;
    wal_msg_write_stru           write_msg;
    wal_msg_wmm_stru            *wmm_params = HI_NULL;

    /* 设备在up状态不允许配置，必须先down */
    if (0 != (OAL_IFF_RUNNING & oal_netdevice_flags(netdev))) {
        oam_error_log1(0, 0, "{wal_ioctl_set_wme_params::device is busy, please down it first %d!}\r\n",
            oal_netdevice_flags(netdev));
        return HI_FAIL;
    }

    pl_param      = (hi_s32 *)pc_extra;
    l_subioctl_id = pl_param[0];    /* 获取sub-ioctl的ID */
    l_ac          = pl_param[1];
    l_value       = pl_param[2];    /* 2: 获取要设置的值 */

    oam_info_log3(0, 0, "{wal_ioctl_set_wme_params::the subioctl_id,l_ac,value is %d, %d, %d!}\r\n", l_subioctl_id,
        l_ac, l_value);

    /* ac取值0~3, value不能为负值 */
    if ((l_value < 0) || (l_ac < 0) || (l_ac >= WLAN_WME_AC_BUTT)) {
        oam_warning_log2(0, 0, "{wal_ioctl_set_wme_params::input value is negative %d, %d!}\r\n", l_value, l_ac);
        return HI_FAIL;
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    if (wal_ioctl_check_wme_params(l_subioctl_id, l_value) != HI_SUCCESS) { /* 参数异常 */
        return HI_FAIL;
    }

    wal_write_msg_hdr_init(&write_msg, (hi_u16)l_subioctl_id, sizeof(wal_msg_wmm_stru));

    wmm_params               = (wal_msg_wmm_stru *)(write_msg.auc_value);
    wmm_params->cfg_id    = (hi_u16)l_subioctl_id;
    wmm_params->ac        = (hi_u32)l_ac;                     /* 填写set消息的payload */
    wmm_params->value     = (hi_u32)l_value;                  /* 填写set消息的payload */

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(wal_msg_wmm_stru),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_wme_params::return err code %u!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_get_wme_params
 功能描述  : iwpriv私有获取参数命令入口
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_wme_params(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *iw,
    hi_s8 *pc_extra)
{
    hi_s32                     *param = HI_NULL;
    mac_vap_stru               *mac_vap = HI_NULL;
    wal_msg_query_stru          query_msg;
    wal_msg_stru               *rsp_msg = HI_NULL;
    wal_msg_rsp_stru           *query_rsp_msg = HI_NULL;
    hmac_config_wmm_para_stru  *wmm_para = HI_NULL;
    hi_u32                      ret;
    hi_s32                     *pl_param = HI_NULL;

    param = (hi_s32 *)pc_extra;
    mac_vap = oal_net_dev_priv(netdev);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, 0, "{wal_ioctl_get_wme_params::oal_net_dev_priv(pst_net_dev) is null ptr.}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }
    pl_param = (hi_s32 *)pc_extra;
    oam_info_log1(0, OAM_SF_WMM, "{wal_ioctl_get_wme_params::return err code %d!}\r\n", pl_param[0]);
    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    query_msg.wid = (hi_u16)pl_param[0];

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_QUERY, WAL_MSG_WID_LENGTH, (hi_u8 *)&query_msg, HI_TRUE, &rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_get_wme_params::fail to get wmm params, error[%u]}", ret);
        return ret;
    }

    /* 处理返回消息 */
    query_rsp_msg = (wal_msg_rsp_stru *)(rsp_msg->auc_msg_data);

    /* 业务处理 */
    wmm_para = (hmac_config_wmm_para_stru*)(query_rsp_msg->auc_value);
    param[0] = wmm_para->value;

    oal_free(rsp_msg);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_setcountry
 功能描述  : 设置管制域国家码
 输入参数  : pst_net_dev: net device
             pst_info   : 请求的信息
             p_w        : 请求的信息
             pc_extra   : 国家字符串
 输出参数  : 无
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年10月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_setcountry(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *w,
    hi_s8 *pc_extra)
{
    hi_u32 ret;

    /* 设备在up状态不允许配置，必须先down */
    if (0 != (OAL_IFF_RUNNING & oal_netdevice_flags(netdev))) {
        oam_info_log1(0, 0, "{wal_ioctl_setcountry::country is %d, %d!}\r\n", oal_netdevice_flags(netdev));
        return HI_FAIL;
    }

    ret = wal_regdomain_update(netdev, pc_extra, MAC_CONTRY_CODE_LEN);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_setcountry::regdomain_update return err code %u!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_getcountry
 功能描述  : 读取国家码
 输入参数  : pst_net_dev: net device
             pst_info   : 请求的信息
             p_w        : 请求的信息
 输出参数  : pc_extra   : 读取到的国家码
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年10月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_getcountry(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_void *w,
    hi_s8 *pc_extra)
{
    hi_u32                          ret;
    wal_msg_query_stru              query_msg;
    wal_msg_stru                   *rsp_msg = HI_NULL;
    wal_msg_rsp_stru               *query_rsp_msg = HI_NULL;
    mac_cfg_get_country_stru       *get_country = HI_NULL;
    oal_iw_point_stru              *iw_point = (oal_iw_point_stru *)w;

    /* **************************************************************************
       抛事件到wal层处理
    ************************************************************************** */
    query_msg.wid = WLAN_CFGID_COUNTRY;

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_QUERY, WAL_MSG_WID_LENGTH, (hi_u8 *)&query_msg, HI_TRUE, &rsp_msg);
    if ((ret != HI_SUCCESS) || (rsp_msg == HI_NULL)) {
        oam_error_log1(0, 0, "{wal_ioctl_getcountry:: wal_alloc_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }
    /* 处理返回消息 */
    query_rsp_msg = (wal_msg_rsp_stru *)(rsp_msg->auc_msg_data);
    /* 业务处理 */
    get_country = (mac_cfg_get_country_stru *)(query_rsp_msg->auc_value);
    if (get_country->ac_country != HI_NULL) {
        if (memcpy_s(pc_extra, WLAN_COUNTRY_STR_LEN, get_country->ac_country, WLAN_COUNTRY_STR_LEN) != EOK) {
            oam_error_log0(0, 0, "{wal_ioctl_getcountry::mem safe function err!}");
            oal_free(rsp_msg);
            return HI_FAIL;
        }
    }
    iw_point->length = WLAN_COUNTRY_STR_LEN;
    oal_free(rsp_msg);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : netd下发命令取得关联设备列表
 输入参数  : oal_iw_request_info_stru *pst_info
             oal_iwreq_data_union *pst_wrqu
**************************************************************************** */
static hi_s32 wal_ioctl_get_assoc_list(oal_net_device_stru *net_dev, oal_iw_request_info_stru *info,
    oal_iwreq_data_union *wrqu, hi_s8 *pc_extra)
{
    hi_s32                       l_ret;
    wal_msg_query_stru              st_query_msg;
    wal_msg_stru                   *pst_rsp_msg = HI_NULL;
    wal_msg_rsp_stru               *pst_query_rsp_msg = HI_NULL;
    hi_char                       *pc_sta_list = HI_NULL;
    oal_netbuf_stru                *pst_response_netbuf = HI_NULL;

    if (oal_unlikely(info == HI_NULL || wrqu == HI_NULL || pc_extra == HI_NULL)) {
        oam_warning_log3(0, 0,
            "{wal_ioctl_get_assoc_list:: param is HI_NULL , pst_info = %p , pst_wrqu = %p , pc_extra = %p}\n", info,
            wrqu, pc_extra);
        return -OAL_EFAIL;
    }

    /* 上层在任何时候都可能下发此命令，需要先判断当前netdev的状态并及时返回 */
    if (oal_unlikely(oal_net_dev_priv(net_dev) == HI_NULL)) {
        return -OAL_EFAIL;
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    st_query_msg.wid = WLAN_CFGID_GET_STA_LIST;

    /* 发送消息 */
    l_ret = wal_send_cfg_event(net_dev, WAL_MSG_TYPE_QUERY, WAL_MSG_WID_LENGTH, (hi_u8 *)&st_query_msg, HI_TRUE,
        &pst_rsp_msg);
    if ((l_ret != HI_SUCCESS) || (pst_rsp_msg == HI_NULL)) {
        oam_error_log1(0, 0, "{wal_ioctl_get_assoc_list:: wal_alloc_cfg_event return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* 处理返回消息 */
    pst_query_rsp_msg = (wal_msg_rsp_stru *)(pst_rsp_msg->auc_msg_data);
    /* 业务处理 */
    if (pst_query_rsp_msg->us_len >= sizeof(oal_netbuf_stru)) {
        /* 获取hmac保存的netbuf指针 */
        memcpy_s(&pst_response_netbuf, sizeof(oal_netbuf_stru), pst_query_rsp_msg->auc_value, sizeof(oal_netbuf_stru));
        if (pst_response_netbuf != HI_NULL) {
            /* 保存ap保存的sta地址信息 */
            pc_sta_list = (hi_char *)oal_netbuf_data(pst_response_netbuf);
            wrqu->data.length = (hi_u16)(oal_netbuf_len(pst_response_netbuf) + 1);
            memcpy_s(pc_extra, wrqu->data.length, pc_sta_list, wrqu->data.length);
            pc_extra[oal_netbuf_len(pst_response_netbuf)] = '\0';
            oal_netbuf_free(pst_response_netbuf);
        } else {
            l_ret = -OAL_ENOMEM;
        }
    } else {
        oal_print_hex_dump((hi_u8 *)pst_rsp_msg->auc_msg_data, pst_query_rsp_msg->us_len, 32,
            "query msg: "); /* group size 32 */
        l_ret = -OAL_EINVAL;
    }

    if (l_ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "{wal_ioctl_get_assoc_list::process failed,ret=%d}", l_ret);
    } else {
        oal_io_print2("wal_ioctl_get_assoc_list,pc_sta_list is:%s,len:%d\n", pc_extra, wrqu->data.length);
    }

    oal_free(pst_rsp_msg);
    return l_ret;
}

/* ****************************************************************************
 功能描述  : set ap mac filter mode to hmac
**************************************************************************** */
static hi_s32 wal_config_mac_filter(oal_net_device_stru *net_dev, hi_char *pc_command)
{
    hi_char                    parsed_command[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN];
    hi_char                   *pc_parse_command = HI_NULL;
    hi_u32                     ul_mac_mode;
    hi_u32                     ul_mac_cnt;
    hi_u32                     ul_i;
#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY
    wal_msg_write_stru         st_write_msg;
    hi_u16                     us_len;
    wal_msg_stru              *pst_rsp_msg = HI_NULL;
    hi_u32                     ul_err_code;
    hi_s32                     l_ret = 0;
#endif
    hi_u32                     ul_ret;
    hi_char                    off_set;

    if (pc_command == HI_NULL) {
        return -OAL_EINVAL;
    }
    pc_parse_command = pc_command;

    /* 解析MAC_MODE */
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, parsed_command, "MAC_MODE=", &off_set,
        WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if (ul_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_config_mac_filter::wal_get_parameter_from_cmd return err_code %u.}\r\n", ul_ret);
        return -OAL_EINVAL;
    }
    /* 检查参数是否合法 0,1,2 */
    ul_mac_mode = (hi_u32)oal_atoi(parsed_command);
    if (ul_mac_mode > 2) { /* mac mode 2 */
        oam_warning_log4(0, 0, "{wal_config_mac_filter::invalid MAC_MODE[%c%c%c%c]!}\r\n", (hi_u8)parsed_command[0],
            (hi_u8)parsed_command[1],                            /* command 0/1 */
            (hi_u8)parsed_command[2], (hi_u8)parsed_command[3]); /* command 2/3 */
        return -OAL_EINVAL;
    }

    /* 设置过滤模式 */
#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY
    ul_ret = wal_hipriv_send_cfg_uint32_data(net_dev, parsed_command, WLAN_CFGID_BLACKLIST_MODE);
    if (ul_ret != HI_SUCCESS) {
        return (hi_s32)ul_ret;
    }
#endif
    /* 解析MAC_CNT */
    pc_parse_command += off_set;
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, parsed_command, "MAC_CNT=", &off_set,
        WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if (ul_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_config_mac_filter::wal_get_parameter_from_cmd return err_code [%u]!}\r\n", ul_ret);
        return -OAL_EINVAL;
    }
    ul_mac_cnt = (hi_u32)oal_atoi(parsed_command);

    for (ul_i = 0; ul_i < ul_mac_cnt; ul_i++) {
        pc_parse_command += off_set;
        ul_ret = wal_get_parameter_from_cmd(pc_parse_command, parsed_command, "MAC=", &off_set,
            WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
        if (ul_ret != HI_SUCCESS) {
            oam_warning_log1(0, 0, "{wal_config_mac_filter::wal_get_parameter_from_cmd return err_code [%u]!}\r\n",
                ul_ret);
            return -OAL_EINVAL;
        }
        /* 5.1  检查参数是否符合MAC长度 */
        if (WLAN_MAC_ADDR_LEN * 2 != strlen(parsed_command)) { /* mac len mul 2 */
            oam_warning_log0(0, 0, "{wal_config_mac_filter::invalid MAC format}\r\n");
            return -OAL_EINVAL;
        }
        /* 6. 添加过滤设备 */
#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY
        /* **************************************************************************
                             抛事件到wal层处理
        ************************************************************************** */
        memset_s((hi_u8 *)&st_write_msg, sizeof(st_write_msg), 0, sizeof(st_write_msg));
        oal_strtoaddr(parsed_command, st_write_msg.auc_value); /* 将字符 ac_name 转换成数组 mac_add[6] */

        us_len = OAL_MAC_ADDR_LEN; /* OAL_SIZEOF(hi_u8); */

        if (ul_i == (ul_mac_cnt - 1)) {
            /* 等所有的mac地址都添加完成后，才进行关联用户确认，是否需要删除 */
            wal_write_msg_hdr_init(&st_write_msg, WLAN_CFGID_ADD_BLACK_LIST, us_len);
        } else {
            wal_write_msg_hdr_init(&st_write_msg, WLAN_CFGID_ADD_BLACK_LIST_ONLY, us_len);
        }

        /* 6.1  发送消息 */
        l_ret = wal_send_cfg_event(net_dev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
            (hi_u8 *)&st_write_msg, HI_TRUE, &pst_rsp_msg);
        if ((l_ret != HI_SUCCESS) || (pst_rsp_msg == HI_NULL)) {
            oam_error_log1(0, 0, "{wal_config_mac_filter:: wal_send_cfg_event return err code %d!}\r\n", l_ret);
            return l_ret;
        }

        /* 6.2  读取返回的错误码 */
        ul_err_code = wal_check_and_release_msg_resp(pst_rsp_msg);
        if (ul_err_code != HI_SUCCESS) {
            oam_error_log1(0, OAM_SF_CFG, "{wal_config_mac_filter::wal_send_cfg_event return err code:[%x]!}\r\n",
                ul_err_code);
            return -OAL_EFAIL;
        }
#endif
    }

    /* 每次设置完成mac地址过滤后，清空此中间变量 */
    memset_s(g_st_ap_config_info.ac_ap_mac_filter_mode, sizeof(g_st_ap_config_info.ac_ap_mac_filter_mode), 0,
        sizeof(g_st_ap_config_info.ac_ap_mac_filter_mode));

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : deauth auc_mac_addr in ap mode
**************************************************************************** */
static hi_s32 wal_kick_sta(oal_net_device_stru *net_dev, hi_u8 *mac_addr, hi_u8 addr_len)
{
#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY
    wal_msg_write_stru          st_write_msg;
    wal_msg_stru               *pst_rsp_msg = HI_NULL;
    hi_u32                  ul_err_code;
    mac_cfg_kick_user_param_stru   *pst_kick_user_param;
    hi_s32                       l_ret;
#endif

    if (mac_addr == NULL) {
        oam_error_log0(0, 0, "{wal_kick_sta::argument auc_mac_addr is null.\n");
        return -OAL_EFAIL;
    }
    hi_unref_param(addr_len);

#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_KICK_USER, sizeof(mac_cfg_kick_user_param_stru));

    pst_kick_user_param = (mac_cfg_kick_user_param_stru *)(st_write_msg.auc_value);
    oal_set_mac_addr(pst_kick_user_param->auc_mac_addr, mac_addr);

    pst_kick_user_param->us_reason_code = MAC_AUTH_NOT_VALID;

    l_ret = wal_send_cfg_event(net_dev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_kick_user_param_stru), (hi_u8 *)&st_write_msg, HI_TRUE,
        &pst_rsp_msg);
    if ((l_ret != HI_SUCCESS) || (pst_rsp_msg == HI_NULL)) {
        oam_error_log1(0, 0, "{wal_kick_sta:: wal_send_cfg_event return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* 4.4  读取返回的错误码 */
    ul_err_code = wal_check_and_release_msg_resp(pst_rsp_msg);
    if (ul_err_code != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_CFG, "{wal_kick_sta::wal_send_cfg_event return err code: [%x]!}\r\n", ul_err_code);
        return -OAL_EFAIL;
    }
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : netd下发命令设置黑名单或白名单
 输入参数  : oal_iw_request_info_stru *pst_info
             oal_iwreq_data_union *pst_wrqu
**************************************************************************** */
static hi_s32 wal_ioctl_set_mac_filters(oal_net_device_stru *net_dev, oal_iw_request_info_stru *info,
    oal_iwreq_data_union *wrqu, hi_s8 *pc_extra)
{
    mac_vap_stru           *pst_vap           = HI_NULL;
    hi_char                *pc_command        = HI_NULL;
    hi_s32                  l_ret;
    hi_u32                  ul_ret;
    hi_char                 parsed_command[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN];
    hi_char                *pc_parse_command = HI_NULL;
    hi_u32                  ul_mac_mode;
    hi_u32                  ul_mac_cnt;
    hi_u8                   mac_addr[WLAN_MAC_ADDR_LEN];
    hi_char                 off_set;

    if (oal_unlikely(info == HI_NULL || wrqu == HI_NULL || pc_extra == HI_NULL)) {
        oam_warning_log3(0, 0,
            "{wal_ioctl_set_mac_filters:: param is HI_NULL , pst_info = %p , pst_wrqu = %p , pc_extra = %p}\n", info,
            wrqu, pc_extra);
        return -OAL_EFAIL;
    }

    /* 1. 申请内存保存netd 下发的命令和数据 */
    pc_command = oal_memalloc((hi_s32)(wrqu->data.length + 1));
    if (pc_command == HI_NULL) {
        return -OAL_ENOMEM;
    }

    /* 2. 拷贝netd 命令到内核态中 */
    memset_s(pc_command, (hi_u32)(wrqu->data.length + 1), 0, (hi_u32)(wrqu->data.length + 1));
    ul_ret =
        oal_copy_from_user(pc_command, (hi_u32)(wrqu->data.length), wrqu->data.pointer, (hi_u32)(wrqu->data.length));
    if (ul_ret != HI_SUCCESS) {
        oam_error_log0(0, 0, "{wal_ioctl_set_mac_filters::oal_copy_from_user: -OAL_EFAIL }\r\n");
        oal_free(pc_command);
        return -OAL_EFAIL;
    }
    pc_command[wrqu->data.length] = '\0';

    oal_io_print2("wal_ioctl_set_mac_filters,data len:%d, command is:%s\n", wrqu->data.length, pc_command);

    pc_parse_command = pc_command;

    memset_s(g_st_ap_config_info.ac_ap_mac_filter_mode, sizeof(g_st_ap_config_info.ac_ap_mac_filter_mode), 0,
        sizeof(g_st_ap_config_info.ac_ap_mac_filter_mode));
    l_ret = strcpy_s(g_st_ap_config_info.ac_ap_mac_filter_mode, sizeof(g_st_ap_config_info.ac_ap_mac_filter_mode),
        pc_command);
    if (l_ret != EOK) {
        oam_error_log0(0, 0, "{wal_ioctl_set_mac_filters::strcpy_s fail }\r\n");
    }

    pst_vap = oal_net_dev_priv(net_dev);
    if (pst_vap == HI_NULL) {
        oam_warning_log0(0, 0, "{wal_ioctl_set_mac_filters::netdevice vap is null,just save it.}\r\n");
        oal_free(pc_command);
        return HI_SUCCESS;
    }

    /* 3  解析MAC_MODE */
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, parsed_command, "MAC_MODE=", &off_set,
        WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if (ul_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_mac_filters::wal_get_parameter_from_cmd return err_code [%u]!}\r\n",
            ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }
    /* 3.1 检查参数是否合法 0,1,2 */
    ul_mac_mode = (hi_u32)oal_atoi(parsed_command);
    if (ul_mac_mode > 2) { /* mac mode 2 */
        oam_warning_log4(0, 0, "{wal_ioctl_set_mac_filters::invalid MAC_MODE[%c%c%c%c]!}", (hi_u8)parsed_command[0],
            (hi_u8)parsed_command[1],                            /* command 0/1 */
            (hi_u8)parsed_command[2], (hi_u8)parsed_command[3]); /* command 2/3 */
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    /* 5 解析MAC_CNT */
    pc_parse_command += off_set;
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, parsed_command, "MAC_CNT=", &off_set,
        WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if (ul_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_mac_filters::wal_get_parameter_from_cmd return err_code [%u]!}\r\n",
            ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }
    ul_mac_cnt = (hi_u32)oal_atoi(parsed_command);

    l_ret = wal_config_mac_filter(net_dev, pc_command);
    if (l_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_ioctl_set_mac_filters::wal_config_mac_filter return err_code [%u]!}\r\n", l_ret);
    }

    /* 如果是白名单模式，且下发允许MAC地址为空，即不允许任何设备关联，需要去关联所有已经关联的STA */
    if ((ul_mac_cnt == 0) && (ul_mac_mode == 2)) { /* mac mode equal 2 */
        oam_warning_log0(0, 0, "{wal_ioctl_set_mac_filters::delete all user!}");

        memset_s(mac_addr, WLAN_MAC_ADDR_LEN, 0xff, WLAN_MAC_ADDR_LEN);
        l_ret = wal_kick_sta(net_dev, mac_addr, WLAN_MAC_ADDR_LEN);
    }

    oal_free(pc_command);
    return l_ret;
}

/* ****************************************************************************
 功能描述  : set ap max user count to hmac
 输入参数  : oal_net_device_stru *pst_net_dev
                         : hi_u32 ul_ap_max_user
**************************************************************************** */
static hi_s32 wal_set_ap_max_user(oal_net_device_stru *net_dev, hi_u32 ap_max_user)
{
    wal_msg_write_stru          st_write_msg;
    wal_msg_stru               *pst_rsp_msg = HI_NULL;
    hi_u32                  ul_err_code;
    hi_s32                   l_ret;

    oam_warning_log1(0, 0, "{wal_set_ap_max_user:: ap_max_user is : %u.}\r\n", ap_max_user);

    if (ap_max_user == 0) {
        oam_warning_log1(0, 0, "{wal_set_ap_max_user::invalid ap max user(%u),ignore this set.}\r\n", ap_max_user);
        return HI_SUCCESS;
    }

    wal_write_msg_hdr_init(&st_write_msg, WLAN_CFGID_SET_MAX_USER, sizeof(ap_max_user));
    *((hi_u32 *)st_write_msg.auc_value) = ap_max_user;
    l_ret = wal_send_cfg_event(net_dev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(ap_max_user),
        (hi_u8 *)&st_write_msg, HI_TRUE, &pst_rsp_msg);
    if ((l_ret != HI_SUCCESS) || (pst_rsp_msg == HI_NULL)) {
        oam_error_log1(0, 0, "{wal_set_ap_max_user:: wal_send_cfg_event return err code %d!}\r\n", l_ret);

        return l_ret;
    }

    /* 读取返回的错误码 */
    ul_err_code = wal_check_and_release_msg_resp(pst_rsp_msg);
    if (ul_err_code != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_CFG, "{wal_set_ap_max_user::wal_send_cfg_event return err code: [%d]!}\r\n",
            ul_err_code);
        return -OAL_EFAIL;
    }

    /* 每次设置最大用户数完成后，都清空为非法值0 */
    g_st_ap_config_info.ap_max_user = 0;

    return l_ret;
}

/* ****************************************************************************
 功能描述  : 设置netd下发APUT config参数 (最大用户数)
 输入参数  : oal_iw_request_info_stru *pst_info
             oal_iwreq_data_union *pst_wrqu
**************************************************************************** */
static hi_s32 wal_ioctl_set_ap_config(oal_net_device_stru *net_dev, oal_iw_request_info_stru *info,
    oal_iwreq_data_union *wrqu, hi_s8 *pc_extra)
{
    hi_char                *pc_command        = HI_NULL;
    hi_char                *pc_parse_command  = HI_NULL;
    hi_s32                  l_ret             = HI_SUCCESS;
    hi_u32                  ul_ret;
    hi_char                 ac_parse_command[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN];
    hi_char                 off_set;

    /* 1. 申请内存保存netd 下发的命令和数据 */
    pc_command = oal_memalloc((hi_s32)(wrqu->data.length + 1));
    if (pc_command == HI_NULL) {
        return -OAL_ENOMEM;
    }
    /* 2. 拷贝netd 命令到内核态中 */
    memset_s(pc_command, (hi_u32)(wrqu->data.length + 1), 0, (hi_u32)(wrqu->data.length + 1));
    ul_ret = oal_copy_from_user(pc_command, (hi_u32)(wrqu->data.length),
                                wrqu->data.pointer, (hi_u32)(wrqu->data.length));
    if (ul_ret != HI_SUCCESS) {
        oam_error_log0(0, 0, "{wal_ioctl_set_ap_config::oal_copy_from_user: -OAL_EFAIL }\r\n");
        oal_free(pc_command);
        return -OAL_EFAIL;
    }
    pc_command[wrqu->data.length] = '\0';

    oal_io_print2("wal_ioctl_set_ap_config,data len:%u,command is:%s\n", (hi_u32)wrqu->data.length, pc_command);

    pc_parse_command = pc_command;
    /* 3.   解析参数 */
    /* 3.1  解析ASCII_CMD */
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parse_command, "ASCII_CMD=", &off_set,
        WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if (ul_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0,
            "{wal_ioctl_set_ap_config::wal_get_parameter_from_cmd ASCII_CMD return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    if ((0 != strcmp("AP_CFG", ac_parse_command))) {
        oam_warning_log0(0, 0, "{wal_ioctl_set_ap_config::sub_command != 'AP_CFG' }");
        oal_io_print1("{wal_ioctl_set_ap_config::sub_command %6s...!= 'AP_CFG' }", ac_parse_command);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    /* 3.2  解析CHANNEL，目前不处理netd下发的channel信息 */
    pc_parse_command += off_set;
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parse_command, "CHANNEL=", &off_set,
        WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if (ul_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0,
            "{wal_ioctl_set_ap_config::wal_get_parameter_from_cmd CHANNEL return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    /* 3.3  解析MAX_SCB */
    pc_parse_command += off_set;
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parse_command, "MAX_SCB=", &off_set,
        WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if (ul_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0,
            "{wal_ioctl_set_ap_config::wal_get_parameter_from_cmd MAX_SCB return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    g_st_ap_config_info.ap_max_user = (hi_u32)oal_atoi(ac_parse_command);

    oam_warning_log1(0, 0, "{wal_ioctl_set_ap_config:: 1131_debug:: ul_ap_max_user = [%d]!}\r\n",
        g_st_ap_config_info.ap_max_user);

    if (oal_net_dev_priv(net_dev) != HI_NULL) {
        l_ret = wal_set_ap_max_user(net_dev, (hi_u32)oal_atoi(ac_parse_command));
    }

    /* 5. 结束释放内存 */
    oal_free(pc_command);
    return l_ret;
}

/* ****************************************************************************
 功能描述  : netd下发命令去关联STA
 输入参数  : oal_iw_request_info_stru *pst_info
             oal_iwreq_data_union *pst_wrqu
**************************************************************************** */
static hi_s32 wal_ioctl_set_ap_sta_disassoc(oal_net_device_stru *net_dev, oal_iw_request_info_stru *info,
    oal_iwreq_data_union *wrqu, hi_s8 *pc_extra)
{
    hi_char                       *pc_command        = HI_NULL;
    hi_s32                       l_ret;
    hi_u32                      ul_ret;
    hi_char                        parsed_command[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN] = {0};
    hi_u8                       mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    hi_char                      off_set;

    /* 1. 申请内存保存netd 下发的命令和数据 */
    pc_command = oal_memalloc((hi_s32)(wrqu->data.length + 1));
    if (pc_command == HI_NULL) {
        return -OAL_ENOMEM;
    }

    /* 2. 拷贝netd 命令到内核态中 */
    memset_s(pc_command, (hi_u32)(wrqu->data.length + 1), 0, (hi_u32)(wrqu->data.length + 1));
    ul_ret = oal_copy_from_user(pc_command, (hi_u32)(wrqu->data.length),
                                wrqu->data.pointer, (hi_u32)(wrqu->data.length));
    if (ul_ret != HI_SUCCESS) {
        oam_error_log0(0, 0, "{wal_ioctl_set_ap_sta_disassoc::oal_copy_from_user: -OAL_EFAIL }\r\n");
        oal_free(pc_command);
        return -OAL_EFAIL;
    }
    pc_command[wrqu->data.length] = '\0';

    oal_io_print1("wal_ioctl_set_ap_sta_disassoc,command is:%s\n", pc_command);

    /* 3. 解析命令获取MAC */
    ul_ret = wal_get_parameter_from_cmd(pc_command, parsed_command, "MAC=", &off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if (ul_ret != HI_SUCCESS) {
        oam_warning_log1(0, 0,
            "{wal_ioctl_set_ap_sta_disassoc::wal_get_parameter_from_cmd MAC return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }
    /* 3.1  检查参数是否符合MAC长度 */
    if (WLAN_MAC_ADDR_LEN * 2 != strlen(parsed_command)) { /* 返回值不等于mac地址的2倍  */
        oam_warning_log0(0, 0, "{wal_ioctl_set_ap_sta_disassoc::invalid MAC format}\r\n");
        oal_free(pc_command);
        return -OAL_EINVAL;
    }
    oal_strtoaddr(parsed_command, mac_addr, WLAN_MAC_ADDR_LEN); /* 将字符 ac_name 转换成数组 mac_add[6] */

    oam_warning_log0(0, 0, "{wal_ioctl_set_ap_sta_disassoc::Geting CMD from APP to DISASSOC!!}");
    l_ret = wal_kick_sta(net_dev, mac_addr, WLAN_MAC_ADDR_LEN);

    /* 5. 结束释放内存 */
    oal_free(pc_command);
    return l_ret;
}
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

/* ****************************************************************************
 函 数 名  : wal_set_mac_to_mib
 功能描述  : 设置wlan接口mac地址
 输入参数  : oal_net_device_stru *pst_net_dev
 输出参数  : 无
 返 回 值  : 错误码
 调用函数  :
 被调函数  :
**************************************************************************** */
hi_u32 wal_set_mac_to_mib(oal_net_device_stru *netdev)
{
    hi_u32                        ret;
    wal_msg_write_stru            write_msg;
    mac_cfg_staion_id_param_stru *param = HI_NULL;

    /* 填写WID消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_STATION_ID, sizeof(mac_cfg_staion_id_param_stru));

    /* 设置配置命令参数 */
    param = (mac_cfg_staion_id_param_stru *)write_msg.auc_value;
    if (memcpy_s(param->auc_station_id, WLAN_MAC_ADDR_LEN, netdev->macAddr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{wal_set_mac_to_mib::mem safe function err!}");
        return HI_FAIL;
    }

    if (strcmp("p2p0", netdev->name) == 0) {
        param->p2p_mode = WLAN_P2P_DEV_MODE;
    }

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_staion_id_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_set_mac_to_mib::return err code [%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : wal_ioctl_get_iwname
 功能描述  : 获得无线的名字, 用来确认无限扩展的存在.
 输入参数  : pst_net_dev: 指向net_device的指针
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年2月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_get_iwname(oal_net_device_stru *netdev, oal_iw_request_info_stru *info, hi_s8 *pc_name,
    hi_s8 *pc_extra)
{
    hi_s8 ac_iwname[] = "IEEE 802.11";

    if (memcpy_s(pc_name, sizeof(ac_iwname), ac_iwname, sizeof(ac_iwname)) != EOK) {
        oam_error_log0(0, 0, "{wal_ioctl_get_iwname::mem safe function err!}");
        return HI_FAIL;
    }
    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
/* ****************************************************************************
 功能描述  : 设置WPS p2p信息元素

 修改历史      :
  1.日    期   : 2014年11月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_ioctl_set_wps_p2p_ie(oal_net_device_stru *netdev, const hi_u8 *puc_buf, hi_u32 len,
    en_app_ie_type_uint8 type)
{
    wal_msg_write_stru              write_msg;
    wal_msg_stru                   *rsp_msg = HI_NULL;
    oal_app_ie_stru                 wps_p2p_ie;

    if (len > WLAN_WPS_IE_MAX_SIZE) {
        oam_error_log1(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie:: wrong ul_len: [%u]!}\r\n", len);
        return HI_FAIL;
    }

    /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(&wps_p2p_ie, sizeof(oal_app_ie_stru), 0, sizeof(oal_app_ie_stru));

    switch (type) {
        case OAL_APP_BEACON_IE:
        case OAL_APP_PROBE_RSP_IE:
        case OAL_APP_ASSOC_RSP_IE:
            wps_p2p_ie.app_ie_type = type;
            wps_p2p_ie.ie_len = len;
            if (memcpy_s(wps_p2p_ie.auc_ie, sizeof(wps_p2p_ie.auc_ie), puc_buf, len) != EOK) {
                oam_error_log0(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie::mem safe function err!}");
                return HI_FAIL;
            }
            break;
        default:
            oam_error_log1(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie:: wrong type: [%x]!}\r\n", type);
            return HI_FAIL;
    }

    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), &wps_p2p_ie, sizeof(wps_p2p_ie)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie::mem safe function err!}");
        return HI_FAIL;
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_WPS_P2P_IE, sizeof(wps_p2p_ie));

    /* 发送消息 */
    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(wps_p2p_ie),
        (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if ((ret != HI_SUCCESS) || (rsp_msg == HI_NULL)) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_ioctl_set_wps_p2p_ie:: wal_alloc_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }

    /* 读取返回的错误码 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_ioctl_set_wps_p2p_ie::wal_send_cfg_event return err code: [%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_set_p2p_noa
 功能描述  : 设置p2p noa节能参数
 输入参数  : oal_net_device_stru *pst_net_dev
             hi_s8 *pc_paramm
 输出参数  : 无
 返 回 值  : static hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

*************************************************************************** */
hi_u32 wal_ioctl_set_p2p_noa(oal_net_device_stru *netdev, const mac_cfg_p2p_noa_param_stru *p2p_noa_param)
{
    wal_msg_write_stru write_msg;
    wal_msg_stru *rsp_msg = HI_NULL;
    hi_u32 ret;

    if (p2p_noa_param != HI_NULL) {
        if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), p2p_noa_param,
            sizeof(mac_cfg_p2p_noa_param_stru)) != EOK) {
            oam_error_log0(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_noa::mem safe function err!}");
            return HI_FAIL;
        }
    }
    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_P2P_PS_NOA, sizeof(mac_cfg_p2p_noa_param_stru));

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_p2p_noa_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if ((ret != HI_SUCCESS) || (rsp_msg == HI_NULL)) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_noa:: wal_alloc_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }

    /* 读取返回的错误码 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_noa::wal_send_cfg_event return err code:  [%d]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_ioctl_set_p2p_ops
 功能描述  : 设置p2p ops节能参数
 输入参数  : oal_net_device_stru *pst_net_dev
             hi_s8 *pc_paramm
 输出参数  : 无
 返 回 值  : static hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

*************************************************************************** */
hi_u32 wal_ioctl_set_p2p_ops(oal_net_device_stru *netdev, const mac_cfg_p2p_ops_param_stru *p2p_ops_param)
{
    wal_msg_write_stru write_msg;
    wal_msg_stru      *rsp_msg = HI_NULL;
    hi_u32             ret;

    if (p2p_ops_param != HI_NULL) {
        if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), p2p_ops_param,
            sizeof(mac_cfg_p2p_ops_param_stru)) != EOK) {
            oam_error_log0(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_ops::mem safe function err!}");
            return HI_FAIL;
        }
    }
    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_P2P_PS_OPS, sizeof(mac_cfg_p2p_ops_param_stru));

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_p2p_ops_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if ((ret != HI_SUCCESS) || (rsp_msg == HI_NULL)) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_ops:: wal_alloc_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }

    /* 读取返回的错误码 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_ops::wal_send_cfg_event return err code:[%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : wal_ioctl_reduce_sar
 功能描述  : 设置RTS 门限值
 输入参数  :
 输出参数  : 无
 返 回 值  : hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年12月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_ioctl_reduce_sar(oal_net_device_stru *netdev, hi_u8 tx_power)
{
    hi_u32                      ret;
    wal_msg_write_stru          write_msg;

    oam_warning_log1(0, 0, "wal_ioctl_reduce_sar::supplicant set tx_power[%d] for reduce SAR purpose.\r\n", tx_power);
    if (tx_power > 100) { /* 大于100无效 */
        oam_warning_log1(0, OAM_SF_CFG,
            "wal_ioctl_reduce_sar::reduce sar failed, reason:invalid tx_power[%d] set by supplicant!", tx_power);
        return HI_ERR_CODE_INVALID_CONFIG;
    }
    /* vap未创建时，不处理supplicant命令 */
    if (oal_net_dev_priv(netdev) == NULL) {
        oam_warning_log0(0, OAM_SF_CFG, "wal_ioctl_reduce_sar::vap not created yet, ignore the cmd!");
        return HI_ERR_WIFI_WAL_INVALID_PARAMETER;
    }
    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_REDUCE_SAR, sizeof(hi_u8));
    *((hi_u8 *)write_msg.auc_value) = tx_power;
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u8),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, 0, "wal_ioctl_reduce_sar::wal_send_cfg_event failed, error no[%u]!\r\n", ret);
        return ret;
    }
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 函 数 名  : wal_start_vap
 功能描述  :
 输入参数  : oal_net_device_stru *pst_net_dev
 输出参数  : 无
 返 回 值  : static hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_start_vap(oal_net_device_stru *netdev)
{
    wal_msg_write_stru      write_msg;
    hi_u32                  ret;
    wal_msg_stru           *rsp_msg = HI_NULL;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 p2p_mode;
    oal_wireless_dev       *wdev = HI_NULL;
#endif

    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, 0, "{wal_start_vap::pst_net_dev is null ptr!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    /* 填写消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_START_VAP, sizeof(mac_cfg_start_vap_param_stru));
    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->net_dev = netdev;
#ifdef _PRE_WLAN_FEATURE_P2P
    wdev = netdev->ieee80211Ptr;
    p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(wdev->iftype);
    if (WLAN_P2P_BUTT == p2p_mode) {
        oam_error_log0(0, 0, "{wal_start_vap::wal_wireless_iftype_to_mac_p2p_mode return BUFF}\r\n");
        return HI_FAIL;
    }
    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->p2p_mode = p2p_mode;

    oam_warning_log1(0, 0, "{wal_start_vap::en_p2p_mode:%d}\r\n", p2p_mode);
#endif
    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->mgmt_rate_init_flag = HI_TRUE;

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_start_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_start_vap::wal_alloc_cfg_event return err code %u!}\r\n", ret);
        return ret;
    }

    /* 处理返回消息 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{wal_start_vap::hmac start vap fail, err code[%u]!}\r\n", ret);
        return ret;
    }

    if ((oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) == 0) {
        oal_netdevice_flags(netdev) |= OAL_IFF_RUNNING;
    }

    /* AP/Mesh模式,启动VAP后,启动发送队列 */
    oal_net_tx_wake_all_queues(); /* 启动发送队列 */

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_stop_vap
 功能描述  : 停用vap
 输入参数  : oal_net_device_stru: net_device
 输出参数  : 无
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_stop_vap(oal_net_device_stru *netdev)
{
    wal_msg_write_stru      write_msg;
    wal_msg_stru           *rsp_msg = HI_NULL;
    hi_u32                  ret;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 p2p_mode;
    oal_wireless_dev   *wdev = HI_NULL;
#endif

    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "{wal_stop_vap::pst_net_dev is null ptr!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 如果不是up状态，不能直接返回成功,防止netdevice状态与VAP状态不一致的情况 */
    if ((oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) == 0) {
        oam_warning_log0(0, 0, "{wal_stop_vap::vap is already down,continue to reset hmac vap state.}\r\n");
    }

    /* **************************************************************************
                           抛事件到wal层处理
    ************************************************************************** */
    /* 填写WID消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DOWN_VAP, sizeof(mac_cfg_down_vap_param_stru));
    ((mac_cfg_down_vap_param_stru *)write_msg.auc_value)->net_dev = netdev;
#ifdef _PRE_WLAN_FEATURE_P2P
    wdev = netdev->ieee80211Ptr;
    p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(wdev->iftype);
    if (WLAN_P2P_BUTT == p2p_mode) {
        oam_error_log0(0, 0, "{wal_stop_vap::wal_wireless_iftype_to_mac_p2p_mode return BUFF}\r\n");
        return HI_FAIL;
    }
    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->p2p_mode = p2p_mode;

    oam_warning_log1(0, 0, "{wal_stop_vap::en_p2p_mode:%d}\r\n", p2p_mode);
#endif

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_down_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);

    if (HI_SUCCESS != wal_check_and_release_msg_resp(rsp_msg)) {
        oam_warning_log0(0, 0, "wal_stop_vap::wal_check_and_release_msg_resp fail");
    }

    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, 0, "{wal_stop_vap::wal_alloc_cfg_event return err code %d!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_u32 wal_get_vap_p2p_mode(const oal_wireless_dev *wdev, wlan_vap_mode_enum_uint8 *vap_mode,
    wlan_p2p_mode_enum_uint8 *p2p_mode)
{
    hi_unref_param(p2p_mode);

    switch (wdev->iftype) {
        case NL80211_IFTYPE_STATION:
            *vap_mode = WLAN_VAP_MODE_BSS_STA;
            break;
        case NL80211_IFTYPE_AP:
            *vap_mode = WLAN_VAP_MODE_BSS_AP;
            break;
#ifdef _PRE_WLAN_FEATURE_P2P
        case NL80211_IFTYPE_P2P_CLIENT:
            *vap_mode = WLAN_VAP_MODE_BSS_STA;
            *p2p_mode = WLAN_P2P_CL_MODE;
            break;
        case NL80211_IFTYPE_P2P_GO:
            *vap_mode = WLAN_VAP_MODE_BSS_AP;
            *p2p_mode = WLAN_P2P_GO_MODE;
            break;
        case NL80211_IFTYPE_P2P_DEVICE:
            *vap_mode = WLAN_VAP_MODE_BSS_STA;
            *p2p_mode = WLAN_P2P_DEV_MODE;
            break;
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
        case NL80211_IFTYPE_MESH_POINT:
            *vap_mode = WLAN_VAP_MODE_MESH;
            break;
#endif
        default:
            oam_warning_log1(0, 0, "{wal_get_vap_p2p_mode::iftype[%d] is not supported!}", wdev->iftype);
            return HI_ERR_SUCCESS;
    }

    return HI_CONTINUE;
}

/* ****************************************************************************
 函 数 名  : wal_init_wlan_vap
 功能描述  : 初始化wlan0,p2p0的vap
 输入参数  : oal_net_device_stru *pst_cfg_net_dev
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_init_wlan_vap(oal_net_device_stru *netdev)
{
    wal_msg_stru             *rsp_msg = HI_NULL;
    wal_msg_write_stru        write_msg = {0};
    wlan_vap_mode_enum_uint8  vap_mode = 0;
    wlan_p2p_mode_enum_uint8  p2p_mode = WLAN_LEGACY_VAP_MODE;
    mac_vap_stru             *mac_vap = oal_net_dev_priv(netdev);

    if (mac_vap != HI_NULL) {
        oam_warning_log0(0, 0, "{wal_init_wlan_vap::pst_mac_vap is already exist}\r\n");
        return HI_SUCCESS;
    }

    oal_wireless_dev      *wdev       = (oal_wireless_dev *)oal_netdevice_wdev(netdev);
    oal_net_device_stru   *netdev_cfg = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if ((wdev == HI_NULL) || (netdev_cfg == HI_NULL)) {
        oam_warning_log2(0, 0, "{wal_init_wlan_vap::wdev[%p]/netdev_cfg[%p] is null}", (uintptr_t)wdev,
            (uintptr_t)netdev_cfg);
        return HI_ERR_CODE_PTR_NULL;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev_cfg);
#endif

    /* 仅用于WIFI和AP打开时创建VAP */
    if (wal_get_vap_p2p_mode(wdev, &vap_mode, &p2p_mode) == HI_ERR_SUCCESS) {
        return HI_ERR_SUCCESS;
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    /* 填写消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ADD_VAP, sizeof(mac_cfg_add_vap_param_stru));
    ((mac_cfg_add_vap_param_stru *)write_msg.auc_value)->net_dev      = netdev;
    ((mac_cfg_add_vap_param_stru *)write_msg.auc_value)->vap_mode     = vap_mode;
    ((mac_cfg_add_vap_param_stru *)write_msg.auc_value)->cfg_vap_indx = WLAN_CFG_VAP_ID;
    ((mac_cfg_add_vap_param_stru *)write_msg.auc_value)->p2p_mode     = p2p_mode;

    hi_u32 ret = wal_send_cfg_event(netdev_cfg, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_add_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(WLAN_CFG_VAP_ID, 0, "{wal_init_wlan_vap::return err code %u!}\r\n", ret);
        return ret;
    }

    /* 读取返回的错误码 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(WLAN_CFG_VAP_ID, 0, "{wal_init_wlan_vap::hmac add vap fail, err code[%u]!}\r\n", ret);
        return ret;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    mac_device_stru *mac_dev = mac_res_get_dev();
    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, 0, "{wal_init_wlan_vap::oal_net_dev_priv(pst_net_dev) is null ptr.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (p2p_mode == WLAN_P2P_DEV_MODE) {
        mac_dev->p2p_info.p2p0_vap_idx = mac_vap->vap_id;
    }
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_deinit_wlan_vap
 功能描述  : 仅用于WIFI和AP关闭时删除VAP
 输入参数  : oal_net_device_stru *pst_net_dev
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_deinit_wlan_vap(oal_net_device_stru *netdev)
{
    wal_msg_write_stru           write_msg;
    wal_msg_stru                *rsp_msg = HI_NULL;
    mac_vap_stru                *mac_vap = HI_NULL;
    hi_u32                       ret;

#ifdef _PRE_WLAN_FEATURE_P2P
    oal_wireless_dev *wdev = GET_NET_DEV_CFG80211_WIRELESS(netdev);
    wlan_p2p_mode_enum_uint8    p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(wdev->iftype);
#endif

    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, 0, "{wal_deinit_wlan_vap::pst_del_vap_param null ptr !}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == HI_NULL) {
        oam_warning_log0(0, 0, "{wal_deinit_wlan_vap::pst_mac_vap is already null}\r\n");
        return HI_SUCCESS;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    if (0 == strcmp("p2p0", netdev->name)) {
        p2p_mode = WLAN_P2P_DEV_MODE;
    }
    oam_warning_log1(mac_vap->vap_id, 0, "{wal_deinit_wlan_vap::en_p2p_mode:%d}\r\n", p2p_mode);
#endif

#ifdef _PRE_WLAN_FEATURE_ANY
    if (mac_vap->support_any == HI_TRUE) {
        hi_wifi_any_deinit();
    }
#endif
    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    /* 删除vap 时需要将参数赋值 */
    ((mac_cfg_del_vap_param_stru *)write_msg.auc_value)->net_dev = netdev;
#ifdef _PRE_WLAN_FEATURE_P2P
    ((mac_cfg_del_vap_param_stru *)write_msg.auc_value)->p2p_mode = p2p_mode;
#endif

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DEL_VAP, sizeof(mac_cfg_del_vap_param_stru));
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_del_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);

    if (HI_SUCCESS != wal_check_and_release_msg_resp(rsp_msg)) {
        oam_warning_log0(0, 0, "wal_deinit_wlan_vap::wal_check_and_release_msg_resp fail.");
    }

    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(mac_vap->vap_id, 0, "{wal_deinit_wlan_vap::return error code %d}\r\n", ret);
    }

    oal_net_dev_priv(netdev) = NULL;

    return ret;
}

/* ****************************************************************************
 函 数 名  : wal_get_mac_addr
 功能描述  : 判断mac地址是否合法
 输入参数  : 无
 输出参数  : hi_u8 *buf
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年1月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_macaddr_check(const hi_u8 *mac_addr)
{
    if ((HI_TRUE == mac_addr_is_zero(mac_addr)) || ((mac_addr[0] & 0x1) == 0x1)) {
        return HI_FAIL;
    }

    return HI_SUCCESS;
}


/* ****************************************************************************
 功能描述  : wal侧通过change virtual iftype创建vap的接口
 修改历史      :
  1.日    期   : 2015年12月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.    日    期   : 2019年2月12日
    作    者   : HiSilicon
    修改内容   : 修改函数名为wal_setup_vap，将iftype赋值移到函数外

**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 wal_setup_vap(oal_net_device_stru *netdev)
{
    hi_u32 ret = HI_FAIL;
    struct netif *netif = HI_NULL;

    if (oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) {
        /* 切换到Mesh前如果网络设备处于UP状态，需要先down 网络设备 */
        hi_diag_log_msg_i0(0, "wal_setup_vap:stop netdevice.");
        wal_netdev_stop(netdev);

        ret = wal_deinit_wlan_vap(netdev);
        if (ret != HI_SUCCESS) {
            hi_diag_log_msg_i0(0, "wal_setup_vap:wlan_deinit_wlan_vap failed");
            return ret;
        }
    }

    netif = netif_find((const hi_char *)netdev->name);
    if (netif != HI_NULL) {
        /* 设置IPv6 linklocal address(SLAAC) */
        if ((GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_AP) ||
            (GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_MESH_POINT)) {
#ifdef _PRE_WLAN_FEATURE_LWIP_IPV6_AUTOCONFIG
            (hi_void)netifapi_set_ip6_autoconfig_disabled(netdev->lwip_netif);
            (hi_void)netifapi_set_accept_ra(netif, HI_FALSE);
            (hi_void)netifapi_set_ipv6_forwarding(netif, HI_TRUE);
            (hi_void)netifapi_set_ra_enable(netif, HI_TRUE);
#endif
            oam_warning_log0(0, 0, "[ERR]wal_setup_vap:netifapi_netif_add_ip6_linklocal_address failed");
        }
    }

    ret = wal_init_wlan_vap(netdev);

    return ret;
}
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 wal_setup_vap(oal_net_device_stru *net_dev)
{
    hi_u32 ret = HI_FAIL;

    if (oal_netdevice_flags(net_dev) & OAL_IFF_RUNNING) {
        /* 切换到Mesh前如果网络设备处于UP状态，需要先down 网络设备 */
        hi_diag_log_msg_i0(0, "wal_setup_vap:stop netdevice.");
        wal_netdev_stop(net_dev);

        ret = wal_deinit_wlan_vap(net_dev);
        if (ret != HI_SUCCESS) {
            hi_diag_log_msg_i0(0, "wal_setup_vap:wlan_deinit_wlan_vap failed");
            return ret;
        }
    }

    ret = wal_init_wlan_vap(net_dev);

    return ret;
}
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : P2P命令字执行
 返 回 值  : 统计结果指针
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_P2P
hi_u32 wal_p2p_cmd_exec(oal_net_device_stru *netdev, hi_u8 *puc_command, hi_u32 len)
{
    if (memcmp(puc_command, CMD_P2P_SET_NOA, strlen(CMD_P2P_SET_NOA)) == 0) {
        hi_u32 skip = strlen(CMD_P2P_SET_NOA) + 1;
        mac_cfg_p2p_noa_param_stru p2p_noa_param;
        if ((skip + sizeof(p2p_noa_param)) > len) {
            oam_error_log2(0, 0, "{wal_priv_cmd_exec::CMD_P2P_SET_NOA param len is [%d], need %d.}", len,
                skip + sizeof(p2p_noa_param));
            return HI_FAIL;
        }
        if (memcpy_s(&p2p_noa_param, sizeof(mac_cfg_p2p_noa_param_stru), puc_command + skip,
            sizeof(mac_cfg_p2p_noa_param_stru)) != EOK) {
            oam_error_log0(0, 0, "wal_priv_cmd_exec: copy noa param failed!");
            return HI_FAIL;
        }
        return wal_ioctl_set_p2p_noa(netdev, &p2p_noa_param);
    } else if (memcmp(puc_command, CMD_P2P_SET_PS, strlen(CMD_P2P_SET_PS)) == 0) {
        hi_u32 skip = strlen(CMD_P2P_SET_PS) + 1;
        mac_cfg_p2p_ops_param_stru p2p_ops_param;
        if ((skip + sizeof(p2p_ops_param)) > len) {
            oam_error_log2(0, 0, "{wal_priv_cmd_exec::CMD_P2P_SET_PS param len is %d, need %d.}", len,
                skip + sizeof(p2p_ops_param));
            return HI_FAIL;
        }
        if (memcpy_s(&p2p_ops_param, sizeof(mac_cfg_p2p_ops_param_stru), puc_command + skip,
            sizeof(mac_cfg_p2p_ops_param_stru)) != EOK) {
            oam_error_log0(0, 0, "wal_priv_cmd_exec: copy ops param failed!");
            return HI_FAIL;
        }
        return wal_ioctl_set_p2p_ops(netdev, &p2p_ops_param);
    }
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 根据命令字执行对应的命令
 输入参数  : net device指针
 返 回 值  : 统计结果指针
**************************************************************************** */
hi_u32 wal_priv_cmd_exec(oal_net_device_stru *netdev, hi_u8 *puc_command, hi_u32 len)
{
    if (0 == memcmp(puc_command, CMD_COUNTRY, strlen(CMD_COUNTRY))) {
        const hi_s8 *country_code = HI_NULL;
        hi_s8        auc_country_code[MAC_CONTRY_CODE_LEN] = {0}; /* 3: 元素个数 */

        /* 格式:COUNTRY CN */
        if (len < (strlen((hi_s8 *)CMD_COUNTRY) + 3)) { /* 3: 偏移量 */
            oam_warning_log0(0, 0, "{wal_priv_cmd_exec::country cn len error.}");
            return HI_FAIL;
        }
        country_code = (hi_char *)puc_command + strlen((hi_char *)CMD_COUNTRY) + 1;
        if (memcpy_s(auc_country_code, sizeof(auc_country_code), country_code, 2) != EOK) { /* 复制长度为2 */
            oam_error_log0(0, 0, "wal_priv_cmd_exec: copy country code failed!");
            return HI_FAIL;
        }
        return wal_regdomain_update(netdev, auc_country_code, MAC_CONTRY_CODE_LEN);
#ifdef _PRE_WLAN_FEATURE_P2P
    } else if (memcmp(puc_command, CMD_SET_AP_WPS_P2P_IE, strlen(CMD_SET_AP_WPS_P2P_IE)) == 0) {
        hi_u32 skip = strlen(CMD_SET_AP_WPS_P2P_IE) + 1;
        /* 结构体类型 */
        oal_app_ie_stru *wps_p2p_ie = (oal_app_ie_stru *)(puc_command + skip);
        return wal_ioctl_set_wps_p2p_ie(netdev, wps_p2p_ie->auc_ie, wps_p2p_ie->ie_len, wps_p2p_ie->app_ie_type);
#endif
    } else if (memcmp(puc_command, CMD_TX_POWER, strlen(CMD_TX_POWER)) == 0) {
        hi_u32 skip_length = strlen((hi_char *)CMD_TX_POWER) + 1;
        hi_u8 txpwr = (hi_u8)oal_atoi((hi_char *)puc_command + skip_length);
        if (wal_ioctl_reduce_sar(netdev, txpwr) != HI_SUCCESS) {
            oam_warning_log0(0, 0, "{wal_priv_cmd_exec::return err code [%u]!}");
            /* 驱动打印错误码，返回成功，防止supplicant 累计4次 ioctl失败导致wifi异常重启 */
            return HI_SUCCESS;
        }
    }
#ifdef _PRE_WLAN_FEATURE_P2P
    /* p2p相关配置命令执行 */
    return wal_p2p_cmd_exec(netdev, puc_command, len);
#endif
    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : 私有接口函数
 输入参数  : net device指针
 返 回 值  : 统计结果指针
**************************************************************************** */
hi_u32 wal_priv_cmd(oal_net_device_stru *netdev, oal_ifreq_stru *ifr, hi_s32 cmd)
{
    wal_wifi_priv_cmd_stru  priv_cmd    = {0};
    hi_u8                          *puc_command    = HI_NULL;
    hi_u32                          ret;

#ifndef _PRE_HDF_LINUX
    if (ifr->ifr_data == HI_NULL) {
        return HI_FAIL;
    }
    if (oal_copy_from_user((hi_u8 *)&priv_cmd, sizeof(wal_wifi_priv_cmd_stru), ifr->ifr_data,
        sizeof(wal_wifi_priv_cmd_stru)) != 0) {
        return HI_FAIL;
    }

    if ((priv_cmd.l_total_len > MAX_PRIV_CMD_SIZE) || (priv_cmd.l_total_len < 0)) {
        oam_error_log1(0, 0, "{wal_priv_cmd::priavte command len:%d too long}", priv_cmd.l_total_len);
        return HI_FAIL;
    }
#endif

    /* 申请内存保存wpa_supplicant 下发的命令和数据 */
    puc_command = oal_memalloc((hi_u32)(priv_cmd.l_total_len + 1)); /* total len 为priv cmd 后面buffer 长度 */
    if (puc_command == HI_NULL) {
        oam_error_log0(0, 0, "{wal_priv_cmd::mem alloc failed.}");
        return HI_FAIL;
    }
    /* 拷贝wpa_supplicant 命令到内核态中 */
    if (memset_s(puc_command, (hi_u32)(priv_cmd.l_total_len + 1), 0, (hi_u32)(priv_cmd.l_total_len + 1)) != EOK) {
        oam_error_log0(0, 0, "wal_priv_cmd: mem safe function err!");
        oal_free(puc_command);
        return HI_FAIL;
    }
#ifndef _PRE_HDF_LINUX
    ret = oal_copy_from_user(puc_command, (hi_u32)(priv_cmd.l_total_len), ((hi_u8 *)ifr->ifr_data) + 8, /* 数据偏移量8 */
        (hi_u32)(priv_cmd.l_total_len));
#endif
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "{wal_priv_cmd::oal_copy_from_user: err code [%u] }", ret);
        oal_free(puc_command);
        return ret;
    }
    puc_command[priv_cmd.l_total_len] = '\0';
    oam_info_log2(0, 0, "{wal_priv_cmd:: private cmd total_len:%d, used_len:%d}",
        priv_cmd.l_total_len, priv_cmd.l_used_len);

    /* 执行命令字 */
    ret = wal_priv_cmd_exec(netdev, puc_command, (hi_u32)priv_cmd.l_total_len);
    oal_free(puc_command);
    return ret;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
