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

#ifndef __HMAC_RX_DATA_H__
#define __HMAC_RX_DATA_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "mac_frame.h"
#include "dmac_ext_if.h"
#include "hmac_user.h"
#include "oal_net.h"
#include "hmac_device.h"
#include "hmac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define HMAC_RX_DATA_ETHER_OFFSET_LENGTH 6 /* 代表发送lan的包，需要填写以太网头，需要向前偏移6 */

/* ****************************************************************************
  3 结构体定义
**************************************************************************** */
/* 每一个MSDU包含的内容的结构体的定义 */
typedef struct {
    hi_u8 auc_sa[WLAN_MAC_ADDR_LEN]; /* MSDU发送的源地址 */
    hi_u8 auc_da[WLAN_MAC_ADDR_LEN]; /* MSDU接收的目的地址 */
    hi_u8 auc_ta[WLAN_MAC_ADDR_LEN]; /* MSDU接收的发送地址 */
    hi_u8 priority;
    hi_u8 auc_resv;

    oal_netbuf_stru *netbuf; /* MSDU对应的netbuf指针(可以使clone的netbuf) */
} hmac_msdu_stru;

/* 处理MPDU的MSDU的处理状态的结构体的定义 */
typedef struct {
    oal_netbuf_stru        *curr_netbuf;          /* 当前处理的netbuf指针 */
    hi_u8              *puc_curr_netbuf_data;         /* 当前处理的netbuf的data指针 */
    hi_u16              us_submsdu_offset;            /* 当前处理的submsdu的偏移量,   */
    hi_u8               msdu_nums_in_netbuf;       /* 当前netbuf包含的总的msdu数目 */
    hi_u8               procd_msdu_in_netbuf;      /* 当前netbuf中已处理的msdu数目 */
} hmac_msdu_proc_state_stru;

/* HMAC模块接收流程处理MSDU状态 */
typedef enum {
    MAC_PROC_ERROR = 0,
    MAC_PROC_LAST_MSDU,
    MAC_PROC_MORE_MSDU,
    MAC_PROC_LAST_MSDU_ODD,

    MAC_PROC_BUTT
} hmac_msdu_proc_status_enum;
typedef hi_u8 hmac_msdu_proc_status_enum_uint8;

/* ****************************************************************************
  4 函数声明
**************************************************************************** */
hi_u32 hmac_rx_process_data_ap(frw_event_mem_stru *event_mem);
hi_void hmac_rx_process_data_ap_tcp_ack_opt(const hmac_vap_stru *hmac_vap, const oal_netbuf_head_stru *netbuf_header);
hi_u32 hmac_rx_process_data_sta(frw_event_mem_stru *event_mem);
hi_void hmac_rx_free_netbuf(oal_netbuf_stru *netbuf, hi_u16 us_nums);
hi_void hmac_rx_free_netbuf_list(oal_netbuf_head_stru *netbuf_hdr, hi_u16 num_buf);

hi_void hmac_rx_lan_frame(const oal_netbuf_head_stru *netbuf_header);
#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
hi_void hmac_pkt_mem_opt_init(hmac_device_stru *hmac_dev);
hi_void hmac_pkt_mem_opt_exit(hmac_device_stru *hmac_dev);
hi_void hmac_pkt_mem_opt_cfg(hi_u32 cfg_tpye, hi_u32 cfg_value);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_RX_DATA_H__ */
