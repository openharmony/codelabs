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

#ifndef __OAL_HCC_SLAVE_IF_H
#define __OAL_HCC_SLAVE_IF_H

#include "oal_net.h"
#include "oal_mem.h"
#include "hcc_comm.h"
#include "wlan_spec_1131h.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* *****************************************************************************
 * 宏定义
 * **************************************************************************** */
#define HCC_LONG_PACKAGE_SIZE (WLAN_LARGE_NETBUF_SIZE)
#ifndef FRW_EVENT_HCC_BURST_COUNT
#define FRW_EVENT_HCC_BURST_COUNT 64
#endif

/* *****************************************************************************
 * 枚举定义
 * **************************************************************************** */
typedef hi_void (*hcc_msg_cb)(hi_u32 msg);

typedef enum {
    HCC_TX_LINUX_BYPASS,
    HCC_TX_SDIO_HOST_BYPASS,
    HCC_TX_SDIO_SLAVE_BYPASS,
    HCC_TX_HAL_HARDWARE_BYPASS,
    HCC_THRUPUT_BYPASS_BUTT,
} hcc_thruput_bypass; /* oal_thruput_bypass_enum */

/* 发送结构体 */
typedef struct {
    hi_u32              tx_pkt_bytes;         /* 发送数据字节数 */
    oal_dev_netbuf_stru *netbuf;
} hcc_slave_tx_pack;

/* 发送方向信息结构体,Device To Host */
typedef struct {
    hi_u32             aggr_tx_num;         /* TX方向聚合帧数量 */
    hi_u32             tx_aggr_total_len;   /* TX方向聚合帧Pad之前的总长度 */
    hcc_slave_tx_pack  hcc_tx_aggr[HISDIO_DEV2HOST_SCATT_MAX]; /* TX方向聚合帧数据 */
} hcc_slave_tx_info;

/* 接收结构体 */
typedef struct {
    hi_u32               rx_pkt_bytes;         /* 发送数据字节数 ,包括HCC头+PayLoad */
    oal_dev_netbuf_stru  *netbuf;
} hcc_slave_rx_pack;

/* 接收方向信息结构体, Host To Device */
typedef struct {
    hi_u32             aggr_rx_num;          /* RX方向聚合帧数量 */
    hi_u32             trans_len;            /* RX方向传输报文长度 */
    hi_u32             rx_aggr_total_len;    /* TX方向聚合帧Pad之前的总长度 */
    hcc_slave_rx_pack  hcc_rx_aggr[HISDIO_HOST2DEV_SCATT_MAX + 1]; /* RX方向聚合帧数据,留1个用于放置Padding数据 */
} hcc_slave_rx_info;

typedef struct {
    hi_s32               len;                /* for hcc transfer */
    oal_dev_netbuf_stru  *net_buf;
} hcc_slave_netbuf;

typedef hi_s32 (*hcc_rx_pre_do)(hi_u8 stype, hcc_slave_netbuf *net_buf, hi_u8 **pre_do_context);
typedef hi_s32 (*hcc_rx_post_do)(hi_u8 stype, const hcc_slave_netbuf *net_buf, hi_u8 *pre_do_context);

typedef struct {
    hi_u32         pkts_count;
    hcc_rx_pre_do  pre_do;
    hcc_rx_post_do post_do;
    hi_void*       context;              /* the callback argument */
} hcc_rx_action;

typedef struct {
    hcc_rx_action action[HCC_ACTION_TYPE_BUTT];
} hcc_rx_action_info;

struct hcc_handler {
    hcc_rx_action_info rx_action_info;
};

/* *****************************************************************************
 * 全局变量声明
 * **************************************************************************** */
/* *****************************************************************************
  函数声明
***************************************************************************** */
hi_u32 hcc_slave_init(hi_void);
hi_u32 hcc_slave_reinit(hi_void);
hi_void hcc_slave_clean(hi_void);
hi_void hcc_slave_tx_queue_sched(hi_void);
hi_void hcc_slave_rx_queue_sched(hi_void);
struct hcc_handler *hcc_get_default_handler(hi_void);
hi_void hcc_slave_tx(oal_dev_netbuf_stru *dev_netbuf, hi_u16 pay_load_len, const hcc_transfer_param *param);
hi_void hcc_register_msg_callback(hcc_msg_cb msg_callback);
hi_u32 hcc_rx_register(struct hcc_handler *hcc, hi_u8 mtype, hcc_rx_post_do post_do, hcc_rx_pre_do pre_do);
hi_u8 hcc_get_thruput_bypass_enable(hcc_thruput_bypass bypass_type);
hi_void hcc_set_thruput_bypass_enable(hcc_thruput_bypass bypass_type, hi_u8 value);
hi_u8 *hcc_get_extend_payload_addr(const oal_dev_netbuf_stru *dev_netbuf);
hi_void *hcc_get_extern_address(const oal_dev_netbuf_stru *dev_netbuf, hi_u32 extend_len);
hi_u8 *hcc_get_extend_addr(const oal_dev_netbuf_stru *dev_netbuf, hi_u32 extend_len);
hi_void hcc_update_high_priority_buffer_credit(hi_u8 free_large_buffer, hi_u8 free_mgmt_buffer, hi_u8 tx_dscr_free_cnt);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hcc_slave.h */
