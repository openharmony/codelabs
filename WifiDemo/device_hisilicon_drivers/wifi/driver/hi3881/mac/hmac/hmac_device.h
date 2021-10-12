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

#ifndef __HMAC_DEVICE_H__
#define __HMAC_DEVICE_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "frw_timer.h"
#include "mac_vap.h"
#include "hmac_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
/* 存储每个扫描到的bss信息 */
typedef struct {
    hi_list                dlist_head;        /* 链表指针 */
    mac_bss_dscr_stru      bss_dscr_info;    /* bss描述信息，包括上报的管理帧 */
} hmac_scanned_bss_info;

/* 存储在hmac device下的扫描结果维护的结构体 */
typedef struct {
    oal_spin_lock_stru  st_lock;
    hi_list             bss_list_head;
    hi_u32              bss_num;
} hmac_bss_mgmt_stru;

/* 扫描运行结果记录 */
typedef struct {
    hmac_bss_mgmt_stru          bss_mgmt;                    /* 存储扫描BSS结果的管理结构 */
    hi_u8                       chan_numbers;                /* 此次扫描总共需要扫描的信道个数 */
    hi_u8                       vap_id;                      /* 本次执行扫描的vap id */
    mac_scan_status_enum_uint8  scan_rsp_status;             /* 本次扫描完成返回的状态码，是成功还是被拒绝 */
    mac_vap_state_enum_uint8    vap_last_state;              /* 保存VAP进入扫描前的状态,AP/P2P GO模式下20/40M扫描专用 */

    mac_scan_cb_fn              fn_cb;                       /* 此次扫描结束的回调函数指针 */
    hi_u8                       is_any_scan;
    hi_u8                       resv[3]; /* 3 保留字节 */
    hi_u64                      ull_cookie;                  /* 保存P2P 监听结束上报的cookie 值 */
} hmac_scan_record_stru;

/* 扫描相关相关控制信息 */
typedef struct {
    /* scan 相关控制信息 */
    hi_u8                    is_scanning;                /* host侧的扫描请求是否正在执行 */
    hi_u8                    is_random_mac_addr_scan;    /* 是否为随机mac addr扫描，默认关闭(定制化宏开启下废弃) */
    hi_u8                    complete;                   /* 内核普通扫描请求是否完成标志 */
    hi_u8                    sched_scan_complete;        /* 调度扫描是否正在运行标记 */
    oal_spin_lock_stru       st_scan_request_spinlock;            /* 内核下发的request资源锁 */

    oal_cfg80211_scan_request_stru         *request;               /* 内核下发的扫描请求结构体 */
    oal_cfg80211_sched_scan_request_stru   *sched_scan_req;        /* 内核下发的调度扫描请求结构体 */

    frw_timeout_stru                       scan_timeout;            /* 扫描模块host侧的超时保护所使用的定时器 */
    /* 扫描运行记录管理信息，包括扫描结果和发起扫描者的相关信息 */
    hmac_scan_record_stru                  scan_record_mgmt;
    mac_channel_stru                       p2p_listen_channel;
    /* 记录请求信道列表的map,11b模式下收到非当前信道扫描帧过滤使用,5g管理帧不使用11b速率 */
    hi_u32                                 scan_2g_ch_list_map;
} hmac_scan_stru;

typedef struct {
    frw_timeout_stru    rx_dscr_opt_timer;     /* rx_dscr调整定时器 */
    hi_u32              rx_pkt_num;
    hi_u32              rx_pkt_opt_limit;
    hi_u32              rx_pkt_reset_limit;

    hi_u8               dscr_opt_state;        /* TRUE表示已调整 */
    hi_u8               dscr_opt_enable;
    hi_u8               resv[2]; /* 2 保留字节 */
} hmac_rx_dscr_opt_stru;

/* hmac device结构体，记录只保存在hmac的device公共信息 */
typedef struct {
    hmac_scan_stru         scan_mgmt;                            /* 扫描管理结构体 */
    hi_u32                 p2p_intf_status;
    oal_wait_queue_head_stru     netif_change_event;
    hi_u8                  auc_rx_ba_lut_idx_table[DMAC_BA_LUT_IDX_BMAP_LEN];   /* 接收端LUT表 */
#ifndef _PRE_WLAN_FEATURE_AMPDU_VAP
    hi_u8                  rx_ba_session_num;                   /* 该device下，rx BA会话的数目 */
    hi_u8                  tx_ba_session_num;                   /* 该device下，tx BA会话的数目 */
#endif
#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
    hmac_rx_dscr_opt_stru rx_dscr_opt;
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    d2h_btcoex_delba_event_stru      d2h_btcoex_delba;
#endif
    hi_u8                 resv1[4]; /* 4 BYTE 保留字段 */
} hmac_device_stru;

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
hmac_device_stru *hmac_get_device_stru(hi_void);
hi_u32 hmac_device_init(hi_void);
hi_u32 hmac_device_exit(hi_void);
#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
hi_u32 hmac_config_host_dev_init(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *param);
hi_u32 hmac_config_host_dev_exit(mac_vap_stru *pst_mac_vap, hi_u16 len, const hi_u8 *param);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_DEVICE_H__ */
