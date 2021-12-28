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

#ifndef __OAM_EXT_IF_H__
#define __OAM_EXT_IF_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_util.h"
#include "oal_ext_if.h"
#include "hi_types.h"
#include "oam_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WIFI_DMT
#define OAM_PRINT_FORMAT_LENGTH 1024 /* ????????????:DMT???????????,??????? */
#else
#define OAM_PRINT_FORMAT_LENGTH 256 /* ???????????? */
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define oam_excp_record(_uc_vap_id, _excp_id) oam_exception_record(_uc_vap_id, _excp_id);

#define oam_error_log(vid, eid, fmt, args...) oam_logn(vid, eid, OAM_LOG_LEVEL_ERROR, fmt, ##args)
#define oam_error_log0(vid, eid, fmt)                   oam_log(vid, eid, OAM_LOG_LEVEL_ERROR, fmt, 0, 0, 0, 0, 0)
#define oam_error_log1(vid, eid, fmt, p1)               oam_log(vid, eid, OAM_LOG_LEVEL_ERROR, fmt, 1, p1,0, 0, 0)
#define oam_error_log2(vid, eid, fmt, p1, p2)           oam_log(vid, eid, OAM_LOG_LEVEL_ERROR, fmt, 2, p1, p2, 0, 0)
#define oam_error_log3(vid, eid, fmt, p1, p2, p3)       oam_log(vid, eid, OAM_LOG_LEVEL_ERROR, fmt, 3, p1, p2, p3, 0)
#define oam_error_log4(vid, eid, fmt, p1, p2, p3, p4)   oam_log(vid, eid, OAM_LOG_LEVEL_ERROR, fmt, 4, p1, p2, p3, p4)
#define oam_error_buf(vid, eid, fmt, buff, size)        oam_print_err(fmt, buff, size)

#define oam_warning_log0(vid, eid, fmt)                 oam_log(vid, eid, OAM_LOG_LEVEL_WARNING, fmt, 0, 0, 0, 0, 0)
#define oam_warning_log1(vid, eid, fmt, p1)             oam_log(vid, eid, OAM_LOG_LEVEL_WARNING, fmt, 1, p1,0, 0, 0)
#define oam_warning_log2(vid, eid, fmt, p1, p2)         oam_log(vid, eid, OAM_LOG_LEVEL_WARNING, fmt, 2, p1, p2, 0, 0)
#define oam_warning_log3(vid, eid, fmt, p1, p2, p3)     oam_log(vid, eid, OAM_LOG_LEVEL_WARNING, fmt, 3, p1, p2, p3, 0)
#define oam_warning_log4(vid, eid, fmt, p1, p2, p3, p4) oam_log(vid, eid, OAM_LOG_LEVEL_WARNING, fmt, 4, p1, p2, p3, p4)
#define oam_warning_buf(vid, eid, fmt, buff, size)      oam_print_warn(fmt, buff, size)

#define oam_info_log(vid, eid, fmt, args...)                    oam_logn(vid, eid, OAM_LOG_LEVEL_INFO, fmt, ##args)
#define oam_info_log0(vid, eid, fmt)                    oam_log(vid, eid, OAM_LOG_LEVEL_INFO, fmt, 0, 0, 0, 0, 0)
#define oam_info_log1(vid, eid, fmt, p1)                oam_log(vid, eid, OAM_LOG_LEVEL_INFO, fmt, 1, p1,0, 0, 0)
#define oam_info_log2(vid, eid, fmt, p1, p2)            oam_log(vid, eid, OAM_LOG_LEVEL_INFO, fmt, 2, p1, p2, 0, 0)
#define oam_info_log3(vid, eid, fmt, p1, p2, p3)        oam_log(vid, eid, OAM_LOG_LEVEL_INFO, fmt, 3, p1, p2, p3, 0)
#define oam_info_log4(vid, eid, fmt, p1, p2, p3, p4)    oam_log(vid, eid, OAM_LOG_LEVEL_INFO, fmt, 4, p1, p2, p3, p4)
#define oam_info_buf(vid, eid, fmt, buff, size)         oam_print_info(fmt, buff, size)

#define oam_debug_log(vid, eid, fmt, args...)                    oam_logn(vid, eid, OAM_LOG_LEVEL_DEBUG, fmt, ##args)
#define oam_debug_log0(vid, eid, fmt)                    oam_log(vid, eid, OAM_LOG_LEVEL_DEBUG, fmt, 0, 0, 0, 0, 0)
#define oam_debug_log1(vid, eid, fmt, p1)                oam_log(vid, eid, OAM_LOG_LEVEL_DEBUG, fmt, 1, p1,0, 0, 0)
#define oam_debug_log2(vid, eid, fmt, p1, p2)            oam_log(vid, eid, OAM_LOG_LEVEL_DEBUG, fmt, 2, p1, p2, 0, 0)
#define oam_debug_log3(vid, eid, fmt, p1, p2, p3)        oam_log(vid, eid, OAM_LOG_LEVEL_DEBUG, fmt, 3, p1, p2, p3, 0)
#define oam_debug_log4(vid, eid, fmt, p1, p2, p3, p4)    oam_log(vid, eid, OAM_LOG_LEVEL_DEBUG, fmt, 4, p1, p2, p3, p4)

#ifdef _PRE_HDF_LINUX
#define oam_print(fmt, arg...) printk(fmt, ##arg)

#define LOG(fmt, arg...) printk(fmt "\r\n", ##arg)
#define LOGE(fmt, arg...) printk("[E/" LOG_TAG "]" fmt "\r\n", ##arg)
#define LOGW(fmt, arg...) printk("[W/" LOG_TAG "]" fmt "\r\n", ##arg)
#define LOGI(fmt, arg...) printk("[I/" LOG_TAG "]" fmt "\r\n", ##arg)
#define LOGD(fmt, arg...) printk("[D/" LOG_TAG "]" fmt "\r\n", ##arg)
#define LOGV(fmt, arg...) printk("[V/" LOG_TAG "]" fmt "\r\n", ##arg)
#else
#define oam_print(fmt, arg...) dprintf(fmt, ##arg)

#define LOG(fmt, arg...) dprintf(fmt "\r\n", ##arg)
#define LOGE(fmt, arg...) dprintf("[E/" LOG_TAG "]" fmt "\r\n", ##arg)
#define LOGW(fmt, arg...) dprintf("[W/" LOG_TAG "]" fmt "\r\n", ##arg)
#define LOGI(fmt, arg...) dprintf("[I/" LOG_TAG "]" fmt "\r\n", ##arg)
#define LOGD(fmt, arg...) dprintf("[D/" LOG_TAG "]" fmt "\r\n", ##arg)
#define LOGV(fmt, arg...) dprintf("[V/" LOG_TAG "]" fmt "\r\n", ##arg)
#endif

#ifdef _HI113X_PRINTK_STDOUT
extern hi_u32 g_level_log;
#define oam_print_err(FORMAT, ARGS...)                                                                               \
    if (g_level_log > 0) {                                                                                           \
        printk(KERN_ERR "[ERROR]:%s:%s:%d:" FORMAT "\n", hi_strrchr(__FILE__, '/'), __FUNCTION__, __LINE__, ##ARGS); \
    }
#define oam_print_warn(FORMAT, ARGS...)                                                                        \
    if (g_level_log > 1) {                                                                                     \
        printk(KERN_WARNING "[WARN]:%s:%s:%d:" FORMAT "\n", hi_strrchr(__FILE__, '/'), __FUNCTION__, __LINE__, \
            ##ARGS);                                                                                           \
    }
#define oam_print_info(FORMAT, ARGS...)                                                                              \
    if (g_level_log > 2) {                                                                                           \
        printk(KERN_INFO "[INFO]:%s:%s:%d:" FORMAT "\n", hi_strrchr(__FILE__, '/'), __FUNCTION__, __LINE__, ##ARGS); \
    }
#else
#define OAM_PRINT_ERR(FORMAT, ARGS...)
#define OAM_PRINT_WARN(FORMAT, ARGS...)
#define OAM_PRINT_INFO(FORMAT, ARGS...)
#endif

#define HI_DIAG_PRINTF dprintf

/* 字符串以0结尾上报，实际字符串内容最大长度 */
#define OAM_REPORT_MAX_STRING_LEN (WLAN_MEM_LOCAL_SIZE5 - 1) /* 以\0结束 */

/* ****************************************************************************
  6 ENUM定义
**************************************************************************** */

typedef hi_u8 oam_log_level_enum_uint8;

/* 特性宏的缩写见gst_oam_feature_list */
typedef enum {
    OAM_SF_SCAN                 = 0,
    OAM_SF_AUTH,
    OAM_SF_ASSOC,
    OAM_SF_FRAME_FILTER,
    OAM_SF_WMM,

    OAM_SF_DFS                  = 5,
    OAM_SF_NETWORK_MEASURE,
    OAM_SF_ENTERPRISE_VO,
    OAM_SF_HOTSPOTROAM,
    OAM_SF_NETWROK_ANNOUNCE,

    OAM_SF_NETWORK_MGMT         = 10,
    OAM_SF_NETWORK_PWS,
    OAM_SF_PROXYARP,
    OAM_SF_TDLS,
    OAM_SF_CALIBRATE,

    OAM_SF_EQUIP_TEST           = 15,
    OAM_SF_CRYPTO,
    OAM_SF_WPA,
    OAM_SF_WEP,
    OAM_SF_WPS,

    OAM_SF_PMF                  = 20,
    OAM_SF_WAPI,
    OAM_SF_BA,
    OAM_SF_AMPDU,
    OAM_SF_AMSDU,

    OAM_SF_STABILITY            = 25,
    OAM_SF_TCP_OPT,
    OAM_SF_ACS,
    OAM_SF_AUTORATE,
    OAM_SF_TXBF,

    OAM_SF_DYN_RECV             = 30,       /* dynamin recv */
    OAM_SF_VIVO,                            /* video_opt voice_opt */
    OAM_SF_MULTI_USER,
    OAM_SF_MULTI_TRAFFIC,
    OAM_SF_ANTI_INTF,

    OAM_SF_EDCA                 = 35,
    OAM_SF_SMART_ANTENNA,
    OAM_SF_TPC,
    OAM_SF_TX_CHAIN,
    OAM_SF_RSSI,

    OAM_SF_WOW                  = 40,
    OAM_SF_GREEN_AP,
    OAM_SF_PWR,                             /* psm uapsd fastmode */
    OAM_SF_SMPS,
    OAM_SF_TXOP,

    OAM_SF_WIFI_BEACON          = 45,
    OAM_SF_KA_AP,                           /* keep alive ap */
    OAM_SF_MULTI_VAP,
    OAM_SF_2040,                            /* 20m+40m coex */
    OAM_SF_DBAC,

    OAM_SF_PROXYSTA             = 50,
    OAM_SF_UM,                              /* user management */
    OAM_SF_P2P,                             /* P2P 特性 */
    OAM_SF_M2U,
    OAM_SF_IRQ,                             /* top half */

    OAM_SF_TX                   = 55,
    OAM_SF_RX,
    OAM_SF_DUG_COEX,
    OAM_SF_CFG,                             /* wal dmac config函数 */
    OAM_SF_FRW,                             /* frw层 */

    OAM_SF_KEEPALIVE            = 60,
    OAM_SF_COEX,
    OAM_SF_HS20                 = 62,        /* HotSpot 2.0特性 */
    OAM_SF_MWO_DET,
    OAM_SF_CCA_OPT,

    OAM_SF_DFT,
    OAM_SF_FIRMWARE,
    OAM_SF_HEARTBEAT,
    OAM_SF_SDIO,
    OAM_SF_BACKUP,
    OAM_SF_ANY, /* rifs protection shortgi frag datarate countrycode
                    coustom_security startup_time lsig monitor wds
                    hidessid */
#ifdef _PRE_WLAN_FEATURE_CSI
    OAM_SF_CSI,
#endif

    OAM_SOFTWARE_FEATURE_BUTT
} oam_feature_enum;

/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
#ifdef _PRE_DEBUG_MODE
typedef struct {
    /* 接收方向统计 */
    hi_u32 ba_recipient_tid_recv_pkt; /* 该TID建立BA会话后，接收数据包数目 */
    hi_u32 ba_recipient_no_ba_session;
    hi_u32 ba_recipient_recv_ampdu_no_ba;
    hi_u32 ba_recipient_send_delba_count;
    hi_u32 ba_recipient_dup_frame_count;
    hi_u32 ba_recipient_dup_frame_drop_count;
    hi_u32 ba_recipient_dup_frame_up_count;
    hi_u32 ba_recipient_direct_up_count;
    hi_u32 ba_recipient_buffer_frame_count;
    hi_u32 ba_recipient_buffer_frame_overlap_count;
    hi_u32 ba_recipient_between_baw_count;
    hi_u32 ba_recipient_greater_baw_count;
    hi_u32 ba_recipient_sync_loss_count;
    hi_u32 ba_recipient_update_hw_baw_count;
} oam_stats_ampdu_stat_stru;
#endif

/* ****************************************************************************
  10 函数声明    OAM模块对外提供的接口
**************************************************************************** */
hi_s32 oam_main_init(hi_void);
hi_void oam_main_exit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oam_ext_if.h */
