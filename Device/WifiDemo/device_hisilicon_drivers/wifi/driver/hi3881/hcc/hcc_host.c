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
  1 其他头文件包含
**************************************************************************** */
#include "hcc_host.h"
#include "hcc_task.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "hmac_ext_if.h"
#include "wal_main.h"
#ifdef CONFIG_MMC
#include "plat_pm_wlan.h"
#endif
#ifndef _PRE_FEATURE_HCC_TASK
#include "flow_control.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  全局变量定义
**************************************************************************** */
hi_u32 g_hcc_rx_thread_enable = 1;
hi_u32 g_hcc_assemble_count = 8;
hi_u32 g_hcc_credit_bottom_value = 2;
hi_u32 g_ul_tcp_ack_wait_sche_cnt = 1;
oal_netbuf_head_stru g_hcc_hmac_rx_netbuf;
hcc_handler_stru *g_hcc_host_handler = HI_NULL;
hi_u32 g_hcc_flowctrl_detect_panic = 0;

hi_u8 g_hcc_sched_stat[FRW_EVENT_TYPE_BUTT];
hi_u8 g_hcc_flowctrl_stat[FRW_EVENT_TYPE_BUTT];
static hi_u8 g_device_is_ready = HI_FALSE;

#define  MIN_DATA_LO_QUEUE                   6
#define  MIN_DES_CNT                         12
#define THRESHOLD_SIZE_10_BYTES     10
#define THRESHOLD_SIZE_20_BYTES     20
#define THRESHOLD_SIZE_40_BYTES     40
#define THRESHOLD_SIZE_60_BYTES     60
#define THRESHOLD_SIZE_80_BYTES     80
#define THRESHOLD_SIZE_120_BYTES    120
#define THRESHOLD_SIZE_200_BYTES    200
#define THRESHOLD_SIZE_400_BYTES    400

#define THRESHOLD_SIZE_128_BYTES    128
#define THRESHOLD_SIZE_256_BYTES    256
#define THRESHOLD_SIZE_512_BYTES    512
#define THRESHOLD_SIZE_1024_BYTES   1024

#define MAX_TIMES                           1000
#define MAX_TIME_VALUE                      1
#define MIN_TIME_VALUE                      1
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define PM_DISABLING_TIME                   1500
#else
#define PM_DISABLING_TIME                   2
#endif
#define LOW_PRIORITY_COUNT                  1000
#define KEY_FRAME_COUNT                     2000

static hi_u8 g_abandon_key_frame = HI_FALSE;
static hi_u8 g_list_overflow_flag = HI_FALSE;
static hi_u32 g_lo_buf_times = 0;

#define WLAN_HEART_BEAT_ENABLE_WAIT_TIMEOUT 200 /* 等待device回复心跳使能/禁用命令的超时时间：200 ms */
#define WLAN_HEART_BEAT_DEVICE_TIMEOUT (5 * 60 * 1000) /* device 5min没收到心跳报文，则复位 */
static oal_completion g_heart_beat_completion_ack;
hi_bool g_heart_beat_enable = HI_TRUE; /* 默认使能心跳 */
hi_u32 g_heart_beat_device_timeout = WLAN_HEART_BEAT_DEVICE_TIMEOUT;
hi_u32 g_priority_cnt = 0;
hi_u32 g_lopriority_cnt = 0;
hi_u32 g_des_cnt = 0;
/* ****************************************************************************
  函数定义
**************************************************************************** */
/* interface for getting hcc host handler */
hcc_handler_stru *hcc_host_get_handler(hi_void)
{
    return g_hcc_host_handler;
}

/* init hcc transfer_queue */
hi_void hcc_transfer_queues_init(hcc_handler_stru *hcc_handler)
{
    hi_s32 i, j;
    for (i = 0; i < HCC_DIR_COUNT; i++) {
        for (j = 0; j < HCC_QUEUE_COUNT; j++) {
            oal_netbuf_head_init(&hcc_handler->hcc_transer_info.hcc_queues[i].queues[j].data_queue);
        }
    }
}

static volatile hi_s32 g_flowctrl_info_flag = 0;

#ifdef _PRE_HDF_LINUX
static hi_void hcc_dev_flow_ctrl_timeout_isr(oal_timer_list_stru *timer)
{
    hi_unref_param(timer);
#else
static hi_void hcc_dev_flow_ctrl_timeout_isr(unsigned long arg)
{
    hi_unref_param(arg);
#endif
    hcc_handler_stru *hcc_handler = g_hcc_host_handler;
    if (hcc_handler == HI_NULL) {
        oam_error_log0(0, 0, "hcc is null");
        return;
    }
    /* flowctrl lock too much time. */
    oam_info_log1(0, 0, "{SDIO flow ctrl had off for %lu ms, it's a long time}",
                  hcc_handler->hcc_transer_info.tx_flow_ctrl.timeout);
    g_flowctrl_info_flag = 0;

    /* If work is idle,queue a new work. */
    if (oal_work_is_busy(&hcc_handler->hcc_transer_info.tx_flow_ctrl.worker.work) == 0) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        oal_queue_delayed_system_work(&hcc_handler->hcc_transer_info.tx_flow_ctrl.worker, 0);
#endif
    }
}

hi_void hcc_print_device_mem_info(hi_void)
{
#ifdef CONFIG_MMC
    hi_wifi_plat_pm_disable();
    oal_bus_send_msg(g_hcc_host_handler->bus, H2D_MSG_DEVICE_MEM_INFO);

    hi_wifi_plat_pm_enable();
#endif
}

hi_void hcc_trigger_device_panic(hi_void)
{
#ifdef CONFIG_MMC
    hi_wifi_plat_pm_disable();
    oal_bus_send_msg(g_hcc_host_handler->bus, H2D_MSG_TEST);
    hi_wifi_plat_pm_enable();
#endif
}

hi_void hcc_flowctrl_deadlock_detect_worker(oal_work_stru *pst_flow_work)
{
    hi_unref_param(pst_flow_work);
    hcc_handler_stru *hcc_handle = hcc_host_get_handler();
    if (hcc_handle == HI_NULL) {
        oam_error_log0(0, 0, "hcc_flowctrl_deadlock_detect_worker hcc is null");
        return;
    }
    oam_info_log1(0, 0, "{hcc_flowctrl_deadlock_detect_worker action,%d}",
                  g_flowctrl_info_flag);
    if (g_flowctrl_info_flag == 0) {
        g_flowctrl_info_flag = 1;
        oal_smp_mb();
        /* queue system_wq delay work,and send other message 20ms later. */
        /* print device mem */
        hcc_print_device_mem_info();
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        oal_queue_delayed_system_work(&hcc_handle->hcc_transer_info.tx_flow_ctrl.worker,
                                      OAL_MSECS_TO_JIFFIES(20)); /* delay 20 jiffes */
#endif
    } else if (g_flowctrl_info_flag == 1) {
        /* print device mem */
        if (g_hcc_flowctrl_detect_panic) {
            hcc_trigger_device_panic();
        } else {
            hcc_print_device_mem_info();
        }
    }

    return;
}

hi_void hcc_trans_flow_ctrl_info_init(hcc_handler_stru *hcc_handle)
{
    hi_s32 i;

    hcc_handle->hcc_transer_info.tx_flow_ctrl.flowctrl_flag = D2H_MSG_FLOWCTRL_ON;
    hcc_handle->hcc_transer_info.tx_flow_ctrl.flowctrl_off_count = 0;
    hcc_handle->hcc_transer_info.tx_flow_ctrl.flowctrl_on_count = 0;
    oal_spin_lock_init(&hcc_handle->hcc_transer_info.tx_flow_ctrl.lock);
    hcc_handle->hcc_transer_info.tx_flow_ctrl.timeout = 20 * 1000; /* TIME 是 20 * 1000 ms */
    INIT_DELAYED_WORK(&hcc_handle->hcc_transer_info.tx_flow_ctrl.worker, hcc_flowctrl_deadlock_detect_worker);
    oal_timer_init(&hcc_handle->hcc_transer_info.tx_flow_ctrl.flow_timer,
        hcc_handle->hcc_transer_info.tx_flow_ctrl.timeout, hcc_dev_flow_ctrl_timeout_isr, 0);

    hcc_handle->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt = HCC_FLOW_HIGH_PRI_BUFF_CNT;
    hcc_handle->hcc_transer_info.tx_flow_ctrl.uc_lopriority_cnt = HCC_FLOW_LOW_PRI_BUFF_CNT;
    oal_spin_lock_init(&hcc_handle->hcc_transer_info.tx_flow_ctrl.st_hipri_lock);

    hcc_trans_queues_stru *hcc_tx_queues = &hcc_handle->hcc_transer_info.hcc_queues[HCC_TX];
    hcc_trans_queues_stru *hcc_rx_queues = &hcc_handle->hcc_transer_info.hcc_queues[HCC_RX];

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        /* TX queue */
        hcc_tx_queues->queues[i].flow_ctrl.enable = HI_TRUE;
        hcc_tx_queues->queues[i].flow_ctrl.flow_type = HCC_FLOWCTRL_SDIO;
        hcc_tx_queues->queues[i].flow_ctrl.is_stopped = HI_FALSE;
        hcc_tx_queues->queues[i].flow_ctrl.low_waterline = THRESHOLD_SIZE_512_BYTES;
        hcc_tx_queues->queues[i].flow_ctrl.high_waterline = THRESHOLD_SIZE_1024_BYTES;
        hcc_tx_queues->queues[i].netbuf_pool_type = HCC_NETBUF_NORMAL_QUEUE;

        /* RX queue */
        hcc_rx_queues->queues[i].flow_ctrl.enable = HI_TRUE;
        hcc_rx_queues->queues[i].flow_ctrl.is_stopped = HI_FALSE;
        hcc_rx_queues->queues[i].flow_ctrl.low_waterline = THRESHOLD_SIZE_128_BYTES;
        hcc_rx_queues->queues[i].flow_ctrl.high_waterline = THRESHOLD_SIZE_512_BYTES;
    }

    /* Additional high priority flow_ctrl settings */
    hcc_tx_queues->queues[DATA_HI_QUEUE].flow_ctrl.flow_type = HCC_FLOWCTRL_CREDIT;
    hcc_tx_queues->queues[DATA_HI_QUEUE].flow_ctrl.enable = HI_FALSE;
    hcc_tx_queues->queues[DATA_HI_QUEUE].netbuf_pool_type = HCC_NETBUF_HIGH_QUEUE;

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
    hcc_tx_queues->queues[CTRL_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_128_BYTES;
    hcc_tx_queues->queues[CTRL_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_256_BYTES;
    hcc_tx_queues->queues[CTRL_QUEUE].netbuf_pool_type = HCC_NETBUF_HIGH_QUEUE;

    hcc_tx_queues->queues[DATA_HI_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_128_BYTES;
    hcc_tx_queues->queues[DATA_HI_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_256_BYTES;
    hcc_tx_queues->queues[DATA_HI_QUEUE].netbuf_pool_type = HCC_NETBUF_HIGH_QUEUE;

    hcc_tx_queues->queues[DATA_LO_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_128_BYTES;
    hcc_tx_queues->queues[DATA_LO_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_256_BYTES;

    hcc_tx_queues->queues[DATA_TCP_DATA_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_128_BYTES;
    hcc_tx_queues->queues[DATA_TCP_DATA_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_256_BYTES;

    hcc_tx_queues->queues[DATA_TCP_ACK_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_128_BYTES;
    hcc_tx_queues->queues[DATA_TCP_ACK_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_256_BYTES;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hcc_tx_queues->queues[DATA_UDP_BK_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_10_BYTES;
    hcc_tx_queues->queues[DATA_UDP_BK_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_20_BYTES;

    hcc_tx_queues->queues[DATA_UDP_BE_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_20_BYTES;
    hcc_tx_queues->queues[DATA_UDP_BE_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_40_BYTES;
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hcc_tx_queues->queues[DATA_UDP_BK_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_200_BYTES;
    hcc_tx_queues->queues[DATA_UDP_BK_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_400_BYTES;

    hcc_tx_queues->queues[DATA_UDP_BE_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_200_BYTES;
    hcc_tx_queues->queues[DATA_UDP_BE_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_400_BYTES;
#endif
    hcc_tx_queues->queues[DATA_UDP_VI_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_40_BYTES;
    hcc_tx_queues->queues[DATA_UDP_VI_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_80_BYTES;

    hcc_tx_queues->queues[DATA_UDP_VO_QUEUE].flow_ctrl.low_waterline = THRESHOLD_SIZE_60_BYTES;
    hcc_tx_queues->queues[DATA_UDP_VO_QUEUE].flow_ctrl.high_waterline = THRESHOLD_SIZE_120_BYTES;
#endif
}

static hi_u32 hcc_host_check_header_vaild(const hcc_header_stru *hcc_hdr)
{
    if ((hcc_hdr->main_type >= HCC_ACTION_TYPE_BUTT) ||
        (HCC_HDR_LEN + hcc_hdr->pad_hdr + hcc_hdr->pad_payload > HCC_HDR_TOTAL_LEN)) {
        oam_error_log4(0, 0,
            "hcc_host_check_header_vaild:: hcc_hdr invalid!,main_type[%d],sub_type[%d],pad_hdr[%d],pad_payload[%d]",
            hcc_hdr->main_type, hcc_hdr->sub_type, hcc_hdr->pad_hdr, hcc_hdr->pad_payload);
        return HI_FALSE;
    }
    return HI_TRUE;
}

static hi_s32 hcc_host_rx(const hcc_handler_stru *hcc_handler, oal_netbuf_stru *netbuf)
{
    hcc_header_stru *hcc_hdr = HI_NULL;
    hi_s32 extend_len;

    (void)hcc_handler;
    hcc_hdr = (hcc_header_stru *)oal_netbuf_data(netbuf);
    if (hcc_host_check_header_vaild(hcc_hdr) != HI_TRUE) {
        oal_print_hex_dump((hi_u8 *)hcc_hdr, HCC_HDR_TOTAL_LEN, 16, "invalid hcc_rx header: "); /* 16进制 */
        return -OAL_EFAIL;
    }

    extend_len = HCC_HDR_TOTAL_LEN - HCC_HDR_LEN - hcc_hdr->pad_hdr;

    if (hcc_hdr->pad_payload) {
        hi_u8 *extend_addr = (hi_u8 *)hcc_hdr + HCC_HDR_LEN + hcc_hdr->pad_hdr;
        if (memmove_s(extend_addr + hcc_hdr->pad_payload, (hi_u32)extend_len, extend_addr,
            (hi_u32)extend_len) != EOK) {
            return -OAL_EFAIL;
        }
    }

    if (HCC_HDR_LEN + hcc_hdr->pad_hdr + hcc_hdr->pad_payload > oal_netbuf_len(netbuf)) {
        oam_error_log1(0, 0, "hcc_host_rx:: hcc_hdr too long, netbuf_len[%d]", (hi_s32)oal_netbuf_len(netbuf));
        oal_print_hex_dump((hi_u8 *)oal_netbuf_data(netbuf), (hi_s32)oal_netbuf_len(netbuf), 32,
            "rx wrong data: "); /* group_size 等于 32 */
        return -OAL_EINVAL;
    }

#ifdef CONFIG_HCC_DEBUG
    oam_info_log1(0, 0, "hcc_host_rx:: seq_num[%d]", hcc_hdr->seq);
    oal_print_hex_dump((hi_u8 *)hcc_hdr, HCC_HDR_TOTAL_LEN, 8, "hcc hdr:"); /* string len 8 */
    oal_print_hex_dump(oal_netbuf_data(netbuf) + HCC_HDR_TOTAL_LEN + hcc_hdr->pad_payload, hcc_hdr->pay_len, 8,
        "hcc payload:"); /* group_size 等于 8 */
#endif

    oal_netbuf_pull(netbuf, HCC_HDR_LEN + hcc_hdr->pad_hdr + hcc_hdr->pad_payload);

    /* 传出去的netbuf len 包含extend_len长度! */
    oal_netbuf_trim(netbuf, oal_netbuf_len(netbuf) - hcc_hdr->pay_len - (hi_u32)extend_len);

    set_oal_netbuf_next(netbuf, HI_NULL);
    set_oal_netbuf_prev(netbuf, HI_NULL);

    return HI_SUCCESS;
}

hi_s32 hcc_host_proc_rx_queue_impl(oal_netbuf_head_stru *head, void *handler, int32_t type)
{
    hcc_handler_stru *hcc_handler = (hcc_handler_stru *)handler;
    hi_s32 count = 0;
    hi_s32 pre_ret = HI_SUCCESS;
    hi_u8 *pre_context = HI_NULL;
    oal_netbuf_head_stru *netbuf_head = HI_NULL;
    oal_netbuf_stru *netbuf = HI_NULL;
    hcc_header_stru *hcc_hdr = HI_NULL;
    hcc_netbuf_stru hcc_netbuf;
    hcc_rx_action_stru *rx_action = HI_NULL;
    netbuf_head = head;

    for (;;) {
        netbuf = oal_netbuf_delist(netbuf_head);
        if (netbuf == HI_NULL) {
            break;
        }
        hcc_hdr = (hcc_header_stru *)oal_netbuf_data(netbuf);
        if (hcc_host_check_header_vaild(hcc_hdr) != HI_TRUE) {
            oal_print_hex_dump((hi_u8 *)hcc_hdr, HCC_HDR_TOTAL_LEN, 16, "invalid hcc header: "); /* 16进制 */
            oam_error_log0(0, 0, "hcc_host_send_rx_queue:: invalid hcc_header");
            count++;
            oal_netbuf_free(netbuf);
            return count;
        }
        rx_action = &hcc_handler->hcc_transer_info.rx_action_info.action[hcc_hdr->main_type];
        pre_ret = HI_SUCCESS;
        if (rx_action->pre_do != HI_NULL) {
            hcc_netbuf.pst_netbuf = netbuf;
            hcc_netbuf.len = (hi_s32)oal_netbuf_len(netbuf);
            pre_ret = rx_action->pre_do(hcc_hdr->sub_type, &hcc_netbuf, &pre_context);
        }
        if (pre_ret == HI_SUCCESS) {
            if (hcc_host_rx(hcc_handler, netbuf) == HI_SUCCESS) {
                if (rx_action->post_do != HI_NULL) {
                    hcc_netbuf.pst_netbuf = netbuf;
                    hcc_netbuf.len = (hi_s32)oal_netbuf_len(netbuf);
                    rx_action->post_do(hcc_hdr->sub_type, &hcc_netbuf, pre_context);
                } else {
                    oam_error_log2(0, 0, "hcc_host_send_rx_queue:: post_do is null, hcc_main_type[%d], sub_type[%d]",
                        hcc_hdr->main_type, hcc_hdr->sub_type);
                    oal_print_hex_dump((hi_u8 *)hcc_hdr, HCC_HDR_TOTAL_LEN, 32, "hcc invalid header: "); /* len 32 */
                    oal_print_hex_dump(oal_netbuf_data(netbuf), (hi_s32)oal_netbuf_len(netbuf), 32,
                        "hcc invalid header(payload): "); /* 32 进制 */
                    oal_netbuf_free(netbuf);
                }
            } else {
                oal_netbuf_free(netbuf);
            }
            hcc_handler->hcc_transer_info.hcc_queues[HCC_RX].queues[type].total_pkts++;
            count++;
        } else {
            /* keep the netbuf in list and skip the loop */
            oal_netbuf_addlist(netbuf_head, netbuf);
            break;
        }
    }

    return count;
}
hi_s32 hcc_host_proc_rx_queue(hcc_handler_stru *hcc_handler, hcc_queue_type_enum type)
{
    oal_netbuf_head_stru *netbuf_head = &hcc_handler->hcc_transer_info.hcc_queues[HCC_RX].queues[type].data_queue;
    return hcc_host_proc_rx_queue_impl(netbuf_head, hcc_handler, type);
}

static oal_netbuf_stru *hcc_tx_assem_descr_get(hcc_handler_stru *hcc_handler)
{
    return oal_netbuf_delist(&hcc_handler->tx_descr_info.tx_assem_descr_hdr);
}

static hi_void hcc_tx_assem_descr_put(hcc_handler_stru *hcc_handler, oal_netbuf_stru *netbuf)
{
    oal_netbuf_list_tail(&hcc_handler->tx_descr_info.tx_assem_descr_hdr, netbuf);
}

static hi_s32 hcc_send_single_descr(hcc_handler_stru *hcc_handler, oal_netbuf_stru *netbuf)
{
    hi_s32 ret;
    oal_netbuf_head_stru head_send;
    OAL_REFERENCE(hcc_handler);
    oal_netbuf_list_head_init(&head_send);
    oal_netbuf_list_tail(&head_send, netbuf);
    ret = oal_sdio_transfer_netbuf_list(hcc_handler->bus, &head_send, SDIO_WRITE);
    return ret;
}

hi_s32 hcc_send_descr_control_data(hcc_handler_stru *hcc_handler, hcc_descr_type descr_type, const hi_void *data,
    hi_u32 ul_size)
{
    hi_s32 ret;
    oal_netbuf_stru *netbuf = HI_NULL;
    struct hcc_descr_header *dscr_hdr = HI_NULL;
    netbuf = hcc_tx_assem_descr_get(hcc_handler);
    if (netbuf == HI_NULL) {
        return -OAL_ENOMEM;
    }

    dscr_hdr = (struct hcc_descr_header *)oal_netbuf_data(netbuf);
    dscr_hdr->descr_type = descr_type;

    if (ul_size) {
        if (OAL_WARN_ON(data == HI_NULL)) {
            hcc_tx_assem_descr_put(hcc_handler, netbuf);
            return -OAL_EINVAL;
        }
        if (OAL_WARN_ON(ul_size + sizeof(struct hcc_descr_header) > oal_netbuf_len(netbuf))) {
            oal_io_print2("invalid request size:%u,max size:%u\r\n",
                (hi_u32)(ul_size + sizeof(struct hcc_descr_header)), (hi_u32)oal_netbuf_len(netbuf));
            hcc_tx_assem_descr_put(hcc_handler, netbuf);
            return -OAL_EINVAL;
        }

        /* lint -e124 */
        if (memcpy_s((hi_void *)((hi_u8 *)oal_netbuf_data(netbuf) + sizeof(struct hcc_descr_header)), ul_size, data,
            ul_size) != EOK) {
            return -OAL_EINVAL;
        }
    }
    ret = hcc_send_single_descr(hcc_handler, netbuf);

    hcc_tx_assem_descr_put(hcc_handler, netbuf);
    return ret;
}

static hi_void hcc_restore_assemble_netbuf_list(hcc_handler_stru *hcc_handler, oal_netbuf_head_stru *head)
{
    hcc_queue_type_enum type;
    oal_netbuf_head_stru *assembled_head;

    type = hcc_handler->hcc_transer_info.tx_assem_info.assembled_queue_type;
    assembled_head = &hcc_handler->hcc_transer_info.tx_assem_info.assembled_head;

    if (type >= HCC_QUEUE_COUNT) {
        type = DATA_LO_QUEUE;
    }

    if (!oal_netbuf_list_empty(assembled_head)) {
        oal_netbuf_splice_sync(head, assembled_head);
    }
}

static hi_s32 hcc_send_assemble_reset(hcc_handler_stru *hcc_handler, oal_netbuf_head_stru *head)
{
    hi_s32 ret;

    hcc_handler->hcc_transer_info.tx_flow_ctrl.flowctrl_reset_count++;

    /* 当只发送一个聚合描述符包，并且聚合个数为0描述通知Device 重置聚合信息 */
    ret = hcc_send_descr_control_data(hcc_handler, HCC_DESCR_ASSEM_RESET, HI_NULL, 0);

    hcc_restore_assemble_netbuf_list(hcc_handler, head);

    return ret;
}


hi_s32 hcc_tx_netbuf_queue_switch(hcc_handler_stru *hcc_handler, hcc_netbuf_queue_type queue_type)
{
    return hcc_send_descr_control_data(hcc_handler, HCC_NETBUF_QUEUE_SWITCH, &queue_type, sizeof(queue_type));
}

hi_s32 hcc_tx_netbuf_test_and_switch_high_pri_queue(hcc_handler_stru *hcc_handler, hcc_netbuf_queue_type pool_type,
    hi_u8 is_config_frame)
{
    hi_s32 ret = OAL_EFAIL;
    if (pool_type == HCC_NETBUF_HIGH_QUEUE) {
        /* 此处在最高位表示是否是配置事件 */
        if (is_config_frame == 1) {
            ret = hcc_tx_netbuf_queue_switch(hcc_handler, ((hi_u8)HCC_NETBUF_HIGH_QUEUE) | HCC_CONFIG_FRAME);
        } else {
            ret = hcc_tx_netbuf_queue_switch(hcc_handler, HCC_NETBUF_HIGH_QUEUE);
        }
    }
    return ret;
}

hi_s32 hcc_tx_netbuf_restore_normal_pri_queue(hcc_handler_stru *hcc_handler, hcc_netbuf_queue_type pool_type)
{
    hi_s32 ret = OAL_EFAIL;
    if (pool_type == HCC_NETBUF_HIGH_QUEUE) {
        ret = hcc_tx_netbuf_queue_switch(hcc_handler, HCC_NETBUF_NORMAL_QUEUE);
    }
    return ret;
}

/* align_size must be power of 2 */
static oal_netbuf_stru *hcc_netbuf_len_align(oal_netbuf_stru *netbuf, hi_u32 align_size)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 len_algin, tail_room_len;
    hi_u32 len = oal_netbuf_len(netbuf);
    if (oal_is_aligned(len, align_size)) {
        return netbuf;
    }
    /* align the netbuf */
    len_algin = oal_round_up(len, align_size);
    if (len_algin < len) {
        oam_error_log2(0, 0, "hcc_netbuf_len_align::len_aglin[%d],len[%d]", len_algin, len);
        return HI_NULL;
    }

    tail_room_len = len_algin - len;

    if (oal_unlikely(tail_room_len > oal_netbuf_tailroom(netbuf))) {
#ifdef _PRE_LWIP_ZERO_COPY_DEBUG
        oam_error_log3(0, 0, "[hcc_send_tx_q_expand_tail] need_tail_room=len_aglin[%d]-len[%d], have_tail_room = %d",
            len_algin, len, oal_netbuf_tailroom(netbuf));
#endif
        /* tailroom not enough */
        ret = oal_netbuf_expand_head(netbuf, 0, (hi_s32)tail_room_len, GFP_KERNEL);
        if (OAL_WARN_ON(ret != HI_SUCCESS)) {
            oal_io_print1("alloc head room failed,expand tail len=%d\n", tail_room_len);
            return HI_NULL;
        }
    }

    oal_netbuf_put(netbuf, tail_room_len);
    return netbuf;
}

static hi_void hcc_build_next_assem_descr(hcc_handler_stru *hcc_handler, hcc_queue_type_enum type,
    oal_netbuf_head_stru *head, oal_netbuf_head_stru *next_assembled_head, const oal_netbuf_stru *descr_netbuf,
    hi_u32 remain_len)
{
    hi_s32 i = 0;
    hi_s32 len;
    hi_u8 *buf = HI_NULL;
    oal_netbuf_stru *netbuf = HI_NULL;
    oal_netbuf_stru *netbuf_t = HI_NULL;
    hi_u32 assemble_max_count, queue_len, current_trans_len;

    buf = (hi_u8 *)oal_netbuf_data(descr_netbuf);
    len = (hi_s32)oal_netbuf_len(descr_netbuf);
    assemble_max_count = oal_max(1, g_hcc_assemble_count);
    queue_len = oal_netbuf_list_len(head);
    current_trans_len = oal_min(queue_len, assemble_max_count);
    current_trans_len = oal_min(current_trans_len, remain_len);

#ifdef CONFIG_HCC_DEBUG
    oal_io_print4("build next descr, queue:[remain_len:%u][len:%u][trans_len:%u][max_assem_len:%u]",
                  remain_len, queue_len, current_trans_len, assemble_max_count);
#endif

    buf[0] = 0;

    if (current_trans_len == 0) {
        return;
    }

    for (; ;) {
        /* move the netbuf from head queue to prepare-send queue, head->tail */
        netbuf = oal_netbuf_delist(head);
        if (netbuf == HI_NULL) {
            oal_io_print1("why? this should never happaned! assem list len:%d\n",
                          oal_netbuf_list_len(next_assembled_head));
            break;
        }

        /* align the buff len to 32B */
        netbuf_t = hcc_netbuf_len_align(netbuf, HISDIO_H2D_SCATT_BUFFLEN_ALIGN);
        if (netbuf_t == HI_NULL) {
            /* return to the list */
            oal_netbuf_addlist(head, netbuf);
            break;
        }

        current_trans_len--;

        oal_netbuf_list_tail(next_assembled_head, netbuf_t);
        if (oal_likely(i >= len)) {
            oal_io_print3("hcc tx scatt num :%d over buff len:%d,assem count:%u\n", i, len, g_hcc_assemble_count);
            break;
        }

        buf[i++] = (hi_u8)(oal_netbuf_len(netbuf) >> HISDIO_H2D_SCATT_BUFFLEN_ALIGN_BITS);
        if (current_trans_len == 0) {
            /* send empty */
            if (i != len) {
                buf[i] = 0;
            }
            break;
        }
    }

    if (oal_likely(!oal_netbuf_list_empty(next_assembled_head))) {
        hcc_handler->hcc_transer_info.tx_assem_info.assembled_queue_type = type;
    }
}

static hi_void hcc_tx_netbuf_free(oal_netbuf_stru *pst_netbuf)
{
    hcc_tx_cb_stru *pst_cb_stru = HI_NULL;

    if (pst_netbuf == HI_NULL) {
        oal_io_print0("hcc_tx_netbuf_free: pst_netbuf is null\r\n");
        return;
    }

    pst_cb_stru = (hcc_tx_cb_stru *)oal_netbuf_cb(pst_netbuf);
    if (oal_unlikely(pst_cb_stru->magic != HCC_TX_WAKELOCK_MAGIC)) {
#ifdef CONFIG_PRINTK
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

        printk(KERN_EMERG "BUG: tx netbuf:%p on CPU#%d,magic:%08x should be %08x\n", pst_cb_stru,
            raw_smp_processor_id(), pst_cb_stru->magic, HCC_TX_WAKELOCK_MAGIC);
        print_hex_dump(KERN_ERR, "tx_netbuf_magic", DUMP_PREFIX_ADDRESS, 16, 1, /* eveny line print 16 */
            (hi_u8 *)pst_netbuf, sizeof(oal_netbuf_stru), true);
        printk(KERN_ERR "\n");
#endif
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        OAL_WARN_ON(1);
#endif
        return;
    }

    if (oal_likely(pst_cb_stru->destroy)) {
        pst_cb_stru->destroy(hcc_host_get_handler());
    }

    pst_cb_stru->magic = 0x0;

    oal_netbuf_free(pst_netbuf);
}

static hi_void hcc_tx_netbuf_list_free(oal_netbuf_head_stru *pst_netbuf_hdr)
{
    oal_netbuf_stru *pst_netbuf = HI_NULL;
    for (; ;) {
        pst_netbuf = oal_netbuf_delist(pst_netbuf_hdr);
        if (pst_netbuf == HI_NULL) {
            break;
        }
        hcc_tx_netbuf_free(pst_netbuf);
    }
}

static hi_s32 hcc_send_data_packet(hcc_handler_stru *hcc_handler,
                                   oal_netbuf_head_stru *head,
                                   hcc_queue_type_enum type,
                                   oal_netbuf_head_stru *next_assembled_head,
                                   hcc_send_mode mode,
                                   hi_u32 *remain_len)
{
    hi_u8 *buf = HI_NULL;
    hi_u32 total_send;
    hi_s32 ret = HI_SUCCESS;
    oal_netbuf_head_stru head_send;
    oal_netbuf_stru *netbuf = HI_NULL;
    oal_netbuf_stru *descr_netbuf = HI_NULL;
    oal_netbuf_stru *netbuf_t = HI_NULL;
    hi_u32 *info = HI_NULL;

    if (*remain_len == 0) {
        return HI_SUCCESS;
    }

#ifdef CONFIG_HCC_DEBUG
    oal_io_print3("send queue:%d, mode:%d,next assem len:%d", type, mode, oal_netbuf_list_len(next_assembled_head));
#endif

    descr_netbuf = hcc_tx_assem_descr_get(hcc_handler);
    if (descr_netbuf == HI_NULL) {
        ret = -OAL_ENOMEM;
        /*lint -e801*/
        goto failed_get_assem_descr;
    }

    info = hcc_handler->hcc_transer_info.tx_assem_info.info;

    oal_netbuf_list_head_init(&head_send);

    if (oal_netbuf_list_empty(next_assembled_head)) {
        /* single send */
        netbuf = oal_netbuf_delist(head);
        if (netbuf == HI_NULL) {
            oal_io_print1("netbuf is HI_NULL [len:%d]\n", oal_netbuf_list_len(head));
            ret = HI_FAIL;
            /*lint -e801*/
            goto failed_get_sig_buff;
        }

        netbuf_t = hcc_netbuf_len_align(netbuf, HISDIO_H2D_SCATT_BUFFLEN_ALIGN);
        if (oal_unlikely(netbuf_t == HI_NULL)) {
            /* return to the list */
            oal_netbuf_addlist(head, netbuf);
            ret = HI_FAIL;
            goto failed_align_netbuf;
        }
        oal_netbuf_list_tail(&head_send, netbuf);
        info[0]++;
    } else {
        hi_u32 assemble_len = oal_netbuf_list_len(next_assembled_head);
        if (assemble_len > HISDIO_HOST2DEV_SCATT_SIZE) {
            oam_error_log1(0, OAM_SF_ANY, "hcc_send_data_packet::assemble_len[%u] > HISDIO_HOST2DEV_SCATT_SIZE.",
                assemble_len);
        } else {
            /* move the assem list to send queue */
            oal_netbuf_splice_init(next_assembled_head, &head_send);
            info[assemble_len]++;
        }
    }

    total_send = oal_netbuf_list_len(&head_send);
    if (*remain_len >= total_send) {
        *remain_len -= total_send;
    } else {
        *remain_len = 0;
    }

    if (oal_netbuf_list_empty(next_assembled_head) != HI_TRUE) {
        oam_error_log0(0, 0, "hcc_send_data_packet::next_assembled_head empty.");
    }

    if (mode == HCC_ASSEM_SEND) {
        hcc_build_next_assem_descr(hcc_handler, type, head, next_assembled_head, descr_netbuf, *remain_len);
    } else {
        buf = oal_netbuf_data(descr_netbuf);
        *((hi_u32 *)buf) = 0;
    }

    /* add the assem descr buf */
    oal_netbuf_addlist(&head_send, descr_netbuf);

    ret = oal_sdio_transfer_netbuf_list(hcc_handler->bus, &head_send, SDIO_WRITE);

#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
    if (HI_NULL != g_pst_alg_process_func.p_auto_freq_count_func) {
        g_pst_alg_process_func.p_auto_freq_count_func(total_send);
    }
#endif
    wlan_pm_set_packet_cnt(total_send);
    hcc_handler->hcc_transer_info.hcc_queues[HCC_TX].queues[type].total_pkts += total_send;

    descr_netbuf = oal_netbuf_delist(&head_send);
    if (descr_netbuf == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hcc_send_data_packet::oal_netbuf_delist fail.descr_netbuf is HI_NULL.");
        ret = HI_SUCCESS;
        goto failed_get_assem_descr;
    }

    hcc_tx_assem_descr_put(hcc_handler, descr_netbuf);

    /* free the sent netbuf,release the wakelock */
    hcc_tx_netbuf_list_free(&head_send);

    return ret;

failed_align_netbuf:
failed_get_sig_buff:
    hcc_tx_assem_descr_put(hcc_handler, descr_netbuf);

failed_get_assem_descr:
    return ret;
}

hi_u32 hcc_queues_flow_ctrl_len(hcc_handler_stru *hcc_handler, hcc_chan_type dir)
{
    hi_s32 i;
    hi_u32 total;
    hcc_trans_queue_stru *p_queue;

    p_queue = hcc_handler->hcc_transer_info.hcc_queues[dir].queues;
    for (i = 0, total = 0; i < HCC_QUEUE_COUNT; i++, p_queue++) {
        if (p_queue->flow_ctrl.enable == HI_TRUE) {
            total += oal_netbuf_list_len(&p_queue->data_queue);
        }
    }

    return total;
}

hi_void hcc_tx_network_startall_queues(const hcc_handler_stru *hcc_handler)
{
    if (oal_likely(hcc_handler->hcc_transer_info.tx_flow_ctrl.net_startall)) {
        hcc_handler->hcc_transer_info.tx_flow_ctrl.net_startall();
    }
}

hi_void hcc_tx_sleep(hi_void)
{
    g_lo_buf_times++;
    if (g_lo_buf_times > MAX_TIMES) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        usleep_range(MIN_TIME_VALUE, MAX_TIME_VALUE);
#else
        hi_sleep(1);
#endif
        g_lo_buf_times = 0;
    }
}

hi_u8 hcc_list_overflow(hi_void)
{
    return g_list_overflow_flag;
}

hi_void hcc_clear_tx_queues(hcc_handler_stru *hcc)
{
    hi_s32 i;
    oal_netbuf_head_stru *head = HI_NULL;
    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        head = &hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].data_queue;
        if (oal_netbuf_list_len(head) > 0) {
            hcc_tx_netbuf_list_free(head);
        }
    }
}

hi_u8 hcc_discard_key_frame(hi_void)
{
    return g_abandon_key_frame;
}

static hi_void hcc_delay_time(hi_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    usleep_range(MIN_TIME_VALUE, MAX_TIME_VALUE);
#else
    hi_sleep(1);
#endif
}

hi_u8 hcc_config_frame(oal_netbuf_head_stru *head)
{
    unsigned long flags;
    oal_netbuf_stru *netbuf = HI_NULL;
    hi_u8 config_frame = 0;
    oal_spin_lock_irq_save((oal_spin_lock_stru *)&head->lock, &flags);
    if (!oal_netbuf_list_empty(head)) {
        netbuf = (oal_netbuf_stru *)oal_netbuf_head_next(head);
        hi_u8 *hcc_hdr = (hi_u8 *)oal_netbuf_data(netbuf);
        frw_hcc_extend_hdr_stru* ext_hdr = (frw_hcc_extend_hdr_stru*)(hcc_hdr + HCC_HDR_LEN
            + WLAN_MAX_MAC_HDR_LEN + HI_MAX_DEV_CB_LEN);
        if (ext_hdr->config_frame == 1) {
            config_frame = 1;
        }
    }
    oal_spin_unlock_irq_restore((oal_spin_lock_stru *)&head->lock, &flags);
    return config_frame;
}

hi_s32 hcc_host_proc_tx_queue_impl(oal_netbuf_head_stru *buffQueue, void *handler, int32_t type)
{
    hcc_handler_stru *hcc_handler = (hcc_handler_stru *)handler;
    hi_s32 ret = 0;
    hi_s32 count = 0;
    hi_u32 remain_len, remain_len_t;
    oal_netbuf_head_stru *head = HI_NULL;
    oal_netbuf_head_stru *next_assembled_head = HI_NULL;
    hcc_send_mode send_mode;
    hcc_trans_queue_stru *hcc_queue = HI_NULL;
    hcc_netbuf_queue_type pool_type;
    hi_u32 priority_cnt = 0;
    hi_u8 is_schedule = HI_TRUE;
#ifndef _PRE_FEATURE_HCC_TASK
    struct FlowControlModule *fcm = HI_NULL;
#endif
    if (type >= HCC_QUEUE_COUNT) {
        oam_error_log1(0, 0, "hcc_send_tx_queue:: invalid hcc_queue type[%d]", type);
        return count;
    }
    hcc_queue = &hcc_handler->hcc_transer_info.hcc_queues[HCC_TX].queues[type];
    head = buffQueue;
#ifndef _PRE_FEATURE_HCC_TASK
    fcm = GetFlowControlModule();
#endif
    hcc_tx_transfer_lock(hcc_handler);

    if (oal_netbuf_list_empty(head)) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        schedule();
#endif
        goto sdio_tx_exit;
    }

    if (hcc_handler->bus) {
        ret = oal_sdio_get_credit(hcc_handler->bus, &priority_cnt);
        if (ret < 0) {
            hcc_tx_transfer_unlock(hcc_handler);
            hcc_clear_tx_queues(hcc_handler);
            hcc_delay_time();
            return 0;
        }
    }
    oal_spin_lock(&(hcc_handler->hcc_transer_info.tx_flow_ctrl.st_hipri_lock));
    hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt =
        hisdio_large_pkt_get(priority_cnt) + hisdio_mgmt_pkt_get(priority_cnt);
    hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_lopriority_cnt = hisdio_large_pkt_get(priority_cnt);
    hcc_handler->hcc_transer_info.tx_flow_ctrl.des_cnt = hisdio_comm_reg_seq_pkt_get(priority_cnt);
    oal_spin_unlock(&(hcc_handler->hcc_transer_info.tx_flow_ctrl.st_hipri_lock));
    remain_len = oal_netbuf_list_len(head);

    g_priority_cnt = hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt;
    g_lopriority_cnt = hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_lopriority_cnt;
    g_des_cnt = hcc_handler->hcc_transer_info.tx_flow_ctrl.des_cnt;

    if (type == DATA_LO_QUEUE) {
        if (remain_len > LOW_PRIORITY_COUNT) {
            g_list_overflow_flag = HI_TRUE;
        } else {
            g_list_overflow_flag = HI_FALSE;
        }

        if (remain_len > KEY_FRAME_COUNT) {
            g_abandon_key_frame = HI_TRUE;
        } else {
            g_abandon_key_frame = HI_FALSE;
        }
    }

    if (hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT &&
        hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt <= MIN_DATA_LO_QUEUE) {
        hcc_tx_sleep();
        goto sdio_tx_exit;
    }

    if (hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_SDIO &&
        hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_lopriority_cnt <= MIN_DATA_LO_QUEUE) {
        hcc_tx_sleep();
        goto sdio_tx_exit;
    }

    if (hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_SDIO &&
        hcc_handler->hcc_transer_info.tx_flow_ctrl.des_cnt < MIN_DES_CNT) {
        hcc_tx_sleep();
        goto sdio_tx_exit;
    }
    g_lo_buf_times = 0;

    if (hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT) {
        remain_len = oal_min(hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt, remain_len);
    } else if (hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_SDIO) {
        remain_len = oal_min(hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_lopriority_cnt, remain_len);
    }

    remain_len_t = remain_len;
    next_assembled_head = &hcc_handler->hcc_transer_info.tx_assem_info.assembled_head;

    if (!oal_netbuf_list_empty(next_assembled_head)) {
        if (hcc_send_assemble_reset(hcc_handler, head) != HI_SUCCESS) {
            /* send one pkt */
            count = 1;
            hcc_delay_time();
            goto sdio_tx_exit;
        }
    }
    send_mode = hcc_queue->send_mode;
    pool_type = hcc_queue->netbuf_pool_type;

    while (remain_len != 0) {
        if (hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT) {
            hi_u8 is_config_frame = hcc_config_frame(head);
            ret = hcc_tx_netbuf_test_and_switch_high_pri_queue(hcc_handler, pool_type, is_config_frame);
            if (ret != HI_SUCCESS) {
                break;
            }
        }

        ret = hcc_send_data_packet(hcc_handler, head, type, next_assembled_head, send_mode, &remain_len);
        if (ret != HI_SUCCESS) {
            break;
        }
        count += (hi_s32)(remain_len_t - remain_len);

#if (!defined(CONFIG_PREEMPT) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION))
        cond_resched();
#endif
    }

    if (ret != HI_SUCCESS) {
        hcc_delay_time();
    }

    /* 正常发送完成，不需要再调度一次 */
    is_schedule = HI_FALSE;

    if (hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT) {
        hcc_tx_netbuf_restore_normal_pri_queue(hcc_handler, pool_type);
    }

sdio_tx_exit:
    hcc_tx_transfer_unlock(hcc_handler);
    if (wlan_pm_is_disabling()) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        usleep_range(PM_DISABLING_TIME, PM_DISABLING_TIME);
#else
        hi_sleep(PM_DISABLING_TIME);
#endif
    }
    /* 异常场景下才重新再调度一次 */
    if (is_schedule && !oal_netbuf_list_empty(head)) {
#ifdef _PRE_FEATURE_HCC_TASK
        hcc_sched_transfer(hcc_handler);
#else
        fcm->interface->schedFCM(fcm, FLOW_TX);
#endif
    }
    return count;
}

hi_s32 hcc_host_proc_tx_queue(hcc_handler_stru *hcc_handler, hcc_queue_type_enum type)
{
    oal_netbuf_head_stru *buffQueue =
        &((hcc_handler_stru *)hcc_handler)->hcc_transer_info.hcc_queues[HCC_TX].queues[type].data_queue;
    return hcc_host_proc_tx_queue_impl(buffQueue, hcc_handler, type);
}
#ifdef _PRE_FEATURE_HCC_TASK
static hi_void hcc_transfer_rx_handler(hcc_handler_stru *hcc_handler, oal_netbuf_stru *netbuf)
{
    /* get the rx buf and enqueue */
    oal_netbuf_list_tail(&hcc_handler->hcc_transer_info.hcc_queues[HCC_RX].queues[DATA_LO_QUEUE].data_queue, netbuf);
    if (0 == g_hcc_rx_thread_enable) {
        hcc_host_proc_rx_queue(hcc_handler, DATA_LO_QUEUE);
    }
}

static hi_void hcc_transfer_rx_handler_replace(hcc_handler_stru *hcc_handler, oal_netbuf_stru *netbuf_new)
{
    oal_netbuf_stru *netbuf_old = HI_NULL;
    hcc_trans_queue_stru *hcc_queue = &hcc_handler->hcc_transer_info.hcc_queues[HCC_RX].queues[DATA_LO_QUEUE];

    netbuf_old = oal_netbuf_delist(&hcc_queue->data_queue);
    if (netbuf_old != HI_NULL) {
        hcc_queue->loss_pkts++;
        oal_netbuf_free(netbuf_old);
    }

    oal_netbuf_list_tail(&hcc_queue->data_queue, netbuf_new);

    if (g_hcc_rx_thread_enable == 0) {
        hcc_host_proc_rx_queue(hcc_handler, DATA_LO_QUEUE);
    }
}
#endif
hi_void hcc_sched_transfer(hcc_handler_stru *hcc_handler)
{
    if (OAL_WARN_ON(hcc_handler == HI_NULL)) {
        oam_error_log0(0, 0, "hcc_sched_transfer:: hcc_handler is null!");
        return;
    }

    hi_wait_queue_wake_up_interrupt(&hcc_handler->hcc_transer_info.hcc_transfer_wq);
}

hi_void hcc_rx_sched_transfer(hcc_handler_stru *hcc_handler)
{
    if (OAL_WARN_ON(hcc_handler == HI_NULL)) {
        oam_error_log0(0, 0, "hcc_sched_transfer:: hcc_handler is null!");
        return;
    }
    hi_wait_queue_wake_up_interrupt(&hcc_handler->hcc_transer_info.hcc_rx_wq);
}

static hi_void hcc_rx_netbuf_list_handler(hcc_handler_stru *hcc_handler, oal_netbuf_head_stru *netbuf_head)
{
    hi_u32 scatt_count;
    oal_netbuf_stru *netbuf = HI_NULL;
    hcc_header_stru *hcc_hdr = HI_NULL;
#ifndef _PRE_FEATURE_HCC_TASK
    struct FlowControlModule *fcm = HI_NULL;
    hcc_convert_queue_para para;
#endif
    scatt_count = oal_netbuf_list_len(netbuf_head);
    if (scatt_count > HISDIO_DEV2HOST_SCATT_MAX) {
        oam_error_log1(0, 0, "hcc_rx_netbuf_list_handler:: scatt buffs overflow, scatt_count[%d]", scatt_count);
        scatt_count = 0;
    }

    hcc_handler->hcc_transer_info.rx_assem_info.info[scatt_count]++;

#ifdef _PRE_FEATURE_HCC_TASK
    hcc_trans_queues_stru *hcc_rx = &hcc_handler->hcc_transer_info.hcc_queues[HCC_RX];
#else
    fcm = GetFlowControlModule();
#endif

    for (;;) {
        netbuf = oal_netbuf_delist(netbuf_head);
        if (netbuf == HI_NULL) {
            break;
        }
        hcc_hdr = (hcc_header_stru *)oal_netbuf_data(netbuf);
        if (hcc_host_check_header_vaild(hcc_hdr) != HI_TRUE) {
            oal_print_hex_dump((hi_u8 *)hcc_hdr, HCC_HDR_TOTAL_LEN, 16, "invalid hcc header: "); /* group size 16 */
        }

#ifdef _PRE_FEATURE_HCC_TASK
        if (hcc_rx->queues[DATA_LO_QUEUE].flow_ctrl.enable == HI_TRUE) {
            if (oal_netbuf_list_len(&hcc_rx->queues[DATA_LO_QUEUE].data_queue) >
                hcc_rx->queues[DATA_LO_QUEUE].flow_ctrl.high_waterline) {
                hcc_transfer_rx_handler_replace(hcc_handler, netbuf);
            } else {
                hcc_transfer_rx_handler(hcc_handler, netbuf);
            }
        } else {
            hcc_transfer_rx_handler(hcc_handler, netbuf);
        }
#else
        if (fcm != NULL && fcm->op != NULL && fcm->op->getRxQueueId != NULL) {
            para.queueType = DATA_LO_QUEUE;
            para.isVip = HI_FALSE;
            FlowControlQueueID id = fcm->op->getRxQueueId((void *)(&para));
            fcm->interface->sendBuffToFCM(fcm, netbuf, id, FLOW_RX);
        }
#endif
    }
#ifdef _PRE_FEATURE_HCC_TASK
    if (g_hcc_rx_thread_enable == 1) {
        hcc_rx_sched_transfer(hcc_handler);
    }
#else
    fcm->interface->schedFCM(fcm, FLOW_RX);
#endif
}

hi_s32 sdio_transfer_rx_handler(hi_void *data)
{
    hi_s32 err_code;
    oal_netbuf_head_stru netbuf_head;
    hcc_handler_stru *hcc_handler = (hcc_handler_stru *)data;

    oal_netbuf_head_init(&netbuf_head);

    err_code = oal_sdio_build_rx_netbuf_list(hcc_handler->bus, &netbuf_head);
    if (err_code != HI_SUCCESS) {
        oam_error_log1(0, 0, "sdio_transfer_rx_handler:: sdio_build_rx_netbuf_list failed[%d]", err_code);
        return err_code;
    }

    err_code = oal_sdio_transfer_netbuf_list(hcc_handler->bus, &netbuf_head, SDIO_READ);
    if (err_code != HI_SUCCESS) {
        oal_netbuf_list_purge(&netbuf_head);
        oam_error_log1(0, 0, "sdio_transfer_rx_handler:: sdio_transfer_netbuf_list failed[%d]", err_code);
        return -OAL_EFAIL;
    }

    if (hcc_get_thread_exit_flag() == HI_TRUE || hi_wifi_get_host_exit_flag() == HI_TRUE) {
        oal_netbuf_list_purge(&netbuf_head);
        return -OAL_EFAIL;
    }
    hcc_rx_netbuf_list_handler(hcc_handler, &netbuf_head);
    return HI_SUCCESS;
}

hi_void hcc_tx_assem_descr_exit(hcc_handler_stru *hcc_handler)
{
    oal_netbuf_list_purge(&hcc_handler->tx_descr_info.tx_assem_descr_hdr);
}

hi_void hcc_host_tx_assem_info_reset(hcc_handler_stru *hcc_handler)
{
    memset_s(hcc_handler->hcc_transer_info.tx_assem_info.info, sizeof(hcc_handler->hcc_transer_info.tx_assem_info.info),
        0, sizeof(hcc_handler->hcc_transer_info.tx_assem_info.info));
}

hi_void hcc_host_rx_assem_info_reset(hcc_handler_stru *hcc_handler)
{
    memset_s(hcc_handler->hcc_transer_info.rx_assem_info.info, sizeof(hcc_handler->hcc_transer_info.rx_assem_info.info),
        0, sizeof(hcc_handler->hcc_transer_info.rx_assem_info.info));
}

hi_void hcc_assem_info_init(hcc_handler_stru *hcc_handler)
{
    hcc_handler->hcc_transer_info.tx_assem_info.assemble_max_count = g_hcc_assemble_count;
    hcc_host_tx_assem_info_reset(hcc_handler);
    hcc_host_rx_assem_info_reset(hcc_handler);
    oal_netbuf_list_head_init(&hcc_handler->hcc_transer_info.tx_assem_info.assembled_head);
}

hi_void hcc_trans_limit_parm_init(hcc_handler_stru *hcc_handler)
{
#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
    hi_s32 i;
    hcc_trans_queues_stru *pst_hcc_tx_queues = &hcc_handler->hcc_transer_info.hcc_queues[HCC_TX];

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        hcc_handler->hcc_transer_info.hcc_queues[HCC_TX].queues[i].burst_limit = (hi_u32)HCC_FLUSH_ALL;
        hcc_handler->hcc_transer_info.hcc_queues[HCC_RX].queues[i].burst_limit = (hi_u32)HCC_FLUSH_ALL;
    }

    pst_hcc_tx_queues->queues[CTRL_QUEUE].burst_limit = 256;          /* CTRL_QUEUE burst_limit 256 */
    pst_hcc_tx_queues->queues[DATA_HI_QUEUE].burst_limit = 256;       /* DATA_HI_QUEUE burst_limit 256 */
    pst_hcc_tx_queues->queues[DATA_LO_QUEUE].burst_limit = 256;       /* DATA_LO_QUEUE burst_limit 256 */
    pst_hcc_tx_queues->queues[DATA_TCP_DATA_QUEUE].burst_limit = 256; /* DATA_TCP_DATA_QUEUE burst_limit 256 */
    pst_hcc_tx_queues->queues[DATA_TCP_ACK_QUEUE].burst_limit = 256;  /* DATA_TCP_ACK_QUEUE burst_limit 256 */
    pst_hcc_tx_queues->queues[DATA_UDP_BK_QUEUE].burst_limit = 10;    /* DATA_UDP_BK_QUEUE burst_limit 10 */
    pst_hcc_tx_queues->queues[DATA_UDP_BE_QUEUE].burst_limit = 20;    /* DATA_UDP_BE_QUEUE burst_limit 20 */
    pst_hcc_tx_queues->queues[DATA_UDP_VI_QUEUE].burst_limit = 40;    /* DATA_UDP_VI_QUEUE burst_limit 40 */
    pst_hcc_tx_queues->queues[DATA_UDP_VO_QUEUE].burst_limit = 60;    /* DATA_UDP_VO_QUEUE burst_limit 60 */
#else
    hi_unref_param(hcc_handler);
#endif
}

hi_void hcc_trans_send_mode_init(hcc_handler_stru *hcc_handler)
{
    hi_s32 i;

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        hcc_handler->hcc_transer_info.hcc_queues[HCC_TX].queues[i].send_mode = HCC_ASSEM_SEND;
    }
    hcc_handler->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].send_mode = HCC_SINGLE_SEND;
}

hi_s32 hcc_tx_assem_descr_init(hcc_handler_stru *hcc_handler)
{
    hi_s32 i;
    hi_s32 ret = HI_SUCCESS;
    oal_netbuf_stru *netbuf = HI_NULL;

    oal_netbuf_head_init(&hcc_handler->tx_descr_info.tx_assem_descr_hdr);

    /* assem descr ping-pong buff, 2 should be ok */
    hcc_handler->tx_descr_info.descr_num = 2;

    for (i = 0; i < hcc_handler->tx_descr_info.descr_num; i++) {
        netbuf = oal_netbuf_alloc(HISDIO_HOST2DEV_SCATT_SIZE, 0, 0);
        if (netbuf == HI_NULL) {
            /*lint -e801*/
            goto failed_netbuf_alloc;
        }

        oal_netbuf_put(netbuf, HISDIO_HOST2DEV_SCATT_SIZE);
        memset_s(oal_netbuf_data(netbuf), oal_netbuf_len(netbuf), 0, oal_netbuf_len(netbuf));
        oal_netbuf_list_tail(&hcc_handler->tx_descr_info.tx_assem_descr_hdr, netbuf);
        if (!oal_is_aligned(((uintptr_t)oal_netbuf_data(netbuf)), 4)) { /* 4 字节对齐 */
            oam_error_log0(0, 0, "hcc_tx_assem_descr_init:: 4 aligned failed!");
        }
    }
    return ret;
failed_netbuf_alloc:
    oal_netbuf_list_purge(&hcc_handler->tx_descr_info.tx_assem_descr_hdr);
    return -OAL_ENOMEM;
}

static hi_void hcc_dev_flowctr_timer_del(hcc_handler_stru *hcc_handler)
{
    if (oal_in_interrupt()) {
        oal_timer_delete(&hcc_handler->hcc_transer_info.tx_flow_ctrl.flow_timer);
    } else {
        oal_timer_delete_sync(&hcc_handler->hcc_transer_info.tx_flow_ctrl.flow_timer);
    }
}

hi_void hcc_dev_flowctrl_on(hcc_handler_stru *hcc_handler, hi_u8 need_notify_dev)
{
    hcc_handler->hcc_transer_info.tx_flow_ctrl.flowctrl_on_count++;
#ifdef CONFIG_HCC_DEBUG
    oal_io_print("start tranferring to device\n");
#endif

    if (hcc_handler->hcc_transer_info.tx_flow_ctrl.flowctrl_flag == D2H_MSG_FLOWCTRL_OFF) {
        hcc_dev_flowctr_timer_del(hcc_handler);
        hcc_handler->hcc_transer_info.tx_flow_ctrl.flowctrl_flag = D2H_MSG_FLOWCTRL_ON;
        hcc_sched_transfer(hcc_handler);
    }

    if (need_notify_dev) {
        oam_info_log0(0, 0, "hcc_dev_flowctrl_on:: Host turn on dev flow ctrl");
        oal_bus_send_msg(hcc_handler->bus, H2D_MSG_FLOWCTRL_ON);
    }
}

hi_void hi_wifi_device_deinit(const hcc_handler_stru *hcc_handler)
{
    oal_bus_send_msg(hcc_handler->bus, H2D_MSG_PM_WLAN_OFF);
}

hi_void hcc_dev_flowctrl_off(hcc_handler_stru *hcc_handler)
{
    if (hcc_handler->hcc_transer_info.tx_flow_ctrl.flowctrl_flag == D2H_MSG_FLOWCTRL_ON) {
        oal_timer_start(&hcc_handler->hcc_transer_info.tx_flow_ctrl.flow_timer,
            hcc_handler->hcc_transer_info.tx_flow_ctrl.timeout);
    }

    hcc_handler->hcc_transer_info.tx_flow_ctrl.flowctrl_flag = D2H_MSG_FLOWCTRL_OFF;
    hcc_handler->hcc_transer_info.tx_flow_ctrl.flowctrl_off_count++;
#ifdef CONFIG_HCC_DEBUG
    oal_io_print("stop tranferring to device\n");
#endif
}

hi_s32 hcc_flow_on_callback(hi_void *data)
{
    hcc_dev_flowctrl_on((hcc_handler_stru *)data, 0);
    return HI_SUCCESS;
}

hi_s32 hcc_flow_off_callback(hi_void *data)
{
    hcc_dev_flowctrl_off((hcc_handler_stru *)data);
    return HI_SUCCESS;
}

hi_s32 hcc_message_register(const hcc_handler_stru *hcc_handler, hi_u8 msg, hcc_msg_rx cb, hi_void *data)
{
    return oal_bus_message_register(hcc_handler->bus, msg, cb, data);
}

hi_void hcc_message_unregister(const hcc_handler_stru *hcc_handler, hi_u8 msg)
{
    oal_bus_message_unregister(hcc_handler->bus, msg);
}

hi_s32 hcc_credit_update_callback(hi_void *data)
{
    hi_u8 large_cnt;
    hcc_handler_stru *hcc_handler = (hcc_handler_stru *)data;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)hcc_handler->bus->priData.data;

    large_cnt = hisdio_large_pkt_get(hi_sdio->sdio_extend->credit_info);
    if (large_cnt == 0) {
        oam_info_log0(0, OAM_SF_PWR, "hcc_credit_update_callback:: credit updata zero!");
    }

    oal_spin_lock(&(hcc_handler->hcc_transer_info.tx_flow_ctrl.st_hipri_lock));
    hcc_handler->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt = large_cnt;
    oal_spin_unlock(&(hcc_handler->hcc_transer_info.tx_flow_ctrl.st_hipri_lock));
    hcc_handler->hcc_transer_info.tx_flow_ctrl.flowctrl_hipri_update_count++;
    hcc_sched_transfer(hcc_handler);
    return HI_SUCCESS;
}

hi_s32 hcc_high_pkts_loss_callback(hi_void *data)
{
    OAL_REFERENCE(data);
    return HI_SUCCESS;
}

hi_s32 hcc_hmac_rx_extend_hdr_vaild_check(const frw_hcc_extend_hdr_stru *hcc_ext_hdr)
{
    if (hcc_ext_hdr->nest_type >= FRW_EVENT_TYPE_BUTT) {
        oam_error_log1(0, 0, "hcc_hmac_rx_extend_hdr_vaild_check:: invalid nest_type[%d]", hcc_ext_hdr->nest_type);
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_s32 hcc_hmac_rx_wifi_post_func(hi_u8 hcc_subtype, const hcc_netbuf_stru *hcc_netbuf, hi_u8 *context)
{
    hi_unref_param(hcc_subtype);
    hi_unref_param(context);
    frw_hcc_extend_hdr_stru *hcc_ext_hdr = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *hcc_event = HI_NULL;
    hcc_event_stru *event_payload = HI_NULL;
    hi_s32 err_code;

    hcc_ext_hdr = (frw_hcc_extend_hdr_stru *)oal_netbuf_data(hcc_netbuf->pst_netbuf);
    if (hcc_hmac_rx_extend_hdr_vaild_check(hcc_ext_hdr) != HI_TRUE) {
        oal_netbuf_free(hcc_netbuf->pst_netbuf);
        return -OAL_EINVAL;
    }

    hmac_vap = hmac_vap_get_vap_stru(hcc_ext_hdr->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, 0, "hcc_hmac_rx_wifi_post_func:: hmac_vap is null, vap_id[%d]", hcc_ext_hdr->vap_id);
        oal_netbuf_free(hcc_netbuf->pst_netbuf);
        return -OAL_EINVAL;
    }

    event_mem = frw_event_alloc(sizeof(hcc_event_stru));
    if (event_mem == HI_NULL) {
        oam_error_log0(0, 0, "hcc_hmac_rx_wifi_post_func:: frw_event alloc failed!");
        oal_netbuf_free(hcc_netbuf->pst_netbuf);
        return -OAL_ENOMEM;
    }

    oal_netbuf_pull(hcc_netbuf->pst_netbuf, sizeof(frw_hcc_extend_hdr_stru));

    hcc_event = frw_get_event_stru(event_mem);
    frw_event_hdr_init(&(hcc_event->event_hdr), hcc_ext_hdr->nest_type, hcc_ext_hdr->nest_sub_type,
        sizeof(hcc_event_stru), FRW_EVENT_PIPELINE_STAGE_1, hcc_ext_hdr->vap_id);

    event_payload = (hcc_event_stru *)frw_get_event_payload(event_mem);
    event_payload->netbuf = hcc_netbuf->pst_netbuf;
    event_payload->buf_len = oal_netbuf_len(hcc_netbuf->pst_netbuf);

    frw_event_task_lock();
    /* 更改HCC发送RX事件由函数直接调用为event事件抛送，防止HCC中断frw任务的执行造成异常 */
    err_code = (hi_s32)frw_event_post_event(event_mem);
    frw_event_task_unlock();
    if (err_code != HI_SUCCESS) {
        oam_error_log3(0, 0, "hcc_hmac_rx_wifi_post_func:: post_event failed, err_code[%d], main_type[%d], subtype[%d]",
            err_code, hcc_ext_hdr->nest_type, hcc_ext_hdr->nest_sub_type);
        oal_netbuf_free(hcc_netbuf->pst_netbuf);
    }

    frw_event_free(event_mem);

    return err_code;
}

hi_void dev_dump_info(hi_char *buf, hi_s32 length)
{
    hi_syserr_info *info = (hi_syserr_info *)buf;
    if (info == HI_NULL || length < (hi_s32)sizeof(hi_syserr_info)) {
        oam_error_log2(0, 0, "length = %d, %d", length, sizeof(hi_syserr_info));
        return;
    }
    oam_error_log0(0, 0, "==============device exception info=================");
    oam_error_log0(0, 0, "***Exc Info***");
    oam_error_log1(0, 0, "%s", (hi_u32)(uintptr_t)info->os_info.task.name);
    oam_error_log1(0, 0, "PC Task ID  = 0x%x", info->os_info.task.id);
    oam_error_log1(0, 0, "Cur Task ID = 0x%x", info->core_info.cur_task_id);
    oam_error_log0(0, 0, "***reg info***");
    oam_error_log1(0, 0, "mepc    = 0x%x", info->reg_info.mepc);
    oam_error_log1(0, 0, "mstatus = 0x%x", info->core_info.mstatus);
    oam_error_log1(0, 0, "mtval   = 0x%x", info->core_info.mtval);
    oam_error_log1(0, 0, "mcause  = 0x%x", info->core_info.mcause);
    oam_error_log1(0, 0, "ccause  = 0x%x", info->core_info.ccause);
    oam_error_log1(0, 0, "ra = 0x%x", info->reg_info.ra);
    oam_error_log1(0, 0, "sp = 0x%x", info->reg_info.sp);
    oam_error_log1(0, 0, "s0 = 0x%x", info->reg_info.s0);
    oam_error_log1(0, 0, "s1 = 0x%x", info->reg_info.s1);
    oam_error_log1(0, 0, "a0 = 0x%x", info->reg_info.a0);
    oam_error_log1(0, 0, "a1 = 0x%x", info->reg_info.a1);
    oam_error_log1(0, 0, "a2 = 0x%x", info->reg_info.a2);
    oam_error_log1(0, 0, "a3 = 0x%x", info->reg_info.a3);
    oam_error_log1(0, 0, "a4 = 0x%x", info->reg_info.a4);
    oam_error_log1(0, 0, "a5 = 0x%x", info->reg_info.a5);
    oam_error_log0(0, 0, "***memory***");
    oam_error_log1(0, 0, "Pool Size = 0x%x", info->os_info.mem.pool_size);
    oam_error_log1(0, 0, "Used Size = 0x%x", info->os_info.mem.cur_use_size);
    oam_error_log0(0, 0, "=====================================================");
}

hi_void dev_backtrace_info(const hi_char *buf, hi_s32 length)
{
    int i;

    oam_error_log0(0, 0, "==============device backtrace=================");
    oam_error_log0(0, 0, "***code trace***");
    for (i = 0; i < length / WORD_WIDTH; i++) {
        oam_error_log2(0, 0, "code trace -- addr:%d   0x%x", i, *((hi_u32 *)buf + i));
    }
    oam_error_log0(0, 0, "***code trace end***");
    oam_error_log0(0, 0, "===============================================");
}

hi_s32 hcc_oam_rx_log_func(hi_u8 hcc_subtype, const hcc_netbuf_stru *hcc_netbuf, hi_u8 *context)
{
    hi_unref_param(hcc_subtype);
    hi_unref_param(context);
    frw_hcc_extend_hdr_stru *hcc_ext_hdr = HI_NULL;
    hi_u16 buf_len;
    oal_netbuf_stru *netbuf = HI_NULL;
    hi_u32 *buf = HI_NULL;
    hi_s32 ret;
    hcc_ext_hdr = (frw_hcc_extend_hdr_stru *)oal_netbuf_data(hcc_netbuf->pst_netbuf);
    if (hcc_ext_hdr->nest_type != FRW_EVENT_TYPE_HCC) {
        ret = -OAL_EINVAL;
        goto end_oam_log;
    }
    netbuf = hcc_netbuf->pst_netbuf;
    buf_len = oal_netbuf_len(hcc_netbuf->pst_netbuf);
    if (buf_len <= HCC_EXTEND_TOTAL_SIZE) {
        ret = -OAL_EINVAL;
        goto end_oam_log;
    }
    buf = (hi_u32 *)((hi_u8 *)oal_netbuf_data(netbuf) + HCC_EXTEND_TOTAL_SIZE);
    switch (hcc_ext_hdr->nest_sub_type) {
        case OAM_DUMP_TYPE:
            dev_dump_info((hi_char *)buf, buf_len - HCC_EXTEND_TOTAL_SIZE);
            break;
        case OAM_BACK_TRACE_TYPE:
            dev_backtrace_info((hi_char *)buf, buf_len - HCC_EXTEND_TOTAL_SIZE);
            break;
        case OAM_LOG_TYPE:
            oam_info_log4(0, 0, "[CLIENT]id 0x%08x:0x%08x 0x%08x 0x%08x\n", buf[LOG_ID], buf[LOG_P0], buf[LOG_P1],
                buf[LOG_P2]);
            break;
        case OAM_PM_TYPE:
            wlan_pm_dump_device_pm_info((hi_char *)buf, buf_len - HCC_EXTEND_TOTAL_SIZE);
            break;
        default:
            break;
    }
    ret = HI_SUCCESS;
end_oam_log:
    if (netbuf != HI_NULL) {
        oal_netbuf_free(netbuf);
    }
    return ret;
}

static hi_s32 hcc_host_rx_register(hcc_handler_stru *hcc_handler, hcc_action_type_enum type, hcc_rx_post_do post_do,
    hcc_rx_pre_do pre_do)
{
    hcc_rx_action_stru *rx_action = HI_NULL;

    if (hcc_handler == HI_NULL) {
        return -OAL_EINVAL;
    }

    rx_action = &(hcc_handler->hcc_transer_info.rx_action_info.action[type]);

    if (rx_action->post_do != HI_NULL) {
        oam_error_log0(0, 0, "post_do has already register");
        return -OAL_EBUSY;
    }

    rx_action->post_do = post_do;
    rx_action->pre_do = pre_do;
    return HI_SUCCESS;
}

static hi_void hcc_hmac_tx_sched_info_init(hi_void)
{
    memset_s(g_hcc_sched_stat, sizeof(g_hcc_sched_stat), DATA_LO_QUEUE, sizeof(g_hcc_sched_stat));

    g_hcc_sched_stat[FRW_EVENT_TYPE_HIGH_PRIO] = DATA_HI_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_HOST_CRX] = DATA_HI_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_HOST_DRX] = DATA_LO_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_HOST_CTX] = DATA_HI_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_WLAN_CRX] = DATA_HI_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_WLAN_DRX] = DATA_LO_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_WLAN_CTX] = DATA_HI_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_WLAN_TX_COMP] = DATA_HI_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_TBTT] = DATA_HI_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_TIMEOUT] = DATA_HI_QUEUE;
    g_hcc_sched_stat[FRW_EVENT_TYPE_DMAC_MISC] = DATA_HI_QUEUE;

    memset_s(g_hcc_flowctrl_stat, sizeof(g_hcc_flowctrl_stat), HCC_FC_NONE, sizeof(g_hcc_flowctrl_stat));
}

hi_s32 hcc_hmac_adapt_init()
{
    hi_s32 err_code;
    hcc_hmac_tx_sched_info_init();
    err_code = hcc_host_rx_register(hcc_host_get_handler(), HCC_ACTION_TYPE_WIFI, hcc_hmac_rx_wifi_post_func, HI_NULL);
    return err_code;
}

void set_device_is_ready(hi_u8 is_ready)
{
    g_device_is_ready = is_ready;
}

hi_u8 get_device_is_ready(hi_void)
{
    return g_device_is_ready;
}

hi_u32 hcc_send_control_msg(hcc_handler_stru *hcc_handler, hi_u8 sub_type, hi_u8 *buf, int len)
{
    oal_netbuf_stru *netbuf = HI_NULL;
    hcc_transfer_param param = { 0 };
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hi_u8 *event_payload = HI_NULL;
    hi_u32 ret = HI_FAIL;

    frw_hcc_extend_hdr_stru *ext_hdr = HI_NULL;
    frw_event_hdr_stru *event_hdr = HI_NULL;
    if (hcc_handler == HI_NULL || get_device_is_ready() != HI_TRUE || len <= 0) {
        goto exit;
    }
    hcc_handler->hcc_transer_info.hcc_timer_status = TIMER_RUNING;
    event_mem = frw_event_alloc(len);
    if (oal_unlikely(event_mem == HI_NULL)) {
        goto exit;
    }
    event = (frw_event_stru *)event_mem->puc_data;
    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HCC, sub_type, len, FRW_EVENT_PIPELINE_STAGE_1, 0);
    event_payload = frw_get_event_payload(event_mem);
    if (event_payload == HI_NULL) {
        oam_error_log0(0, 0, "hcc_send_heartbeat:: event_payload is NULL");
        goto exit;
    }

    if (memcpy_s(event_payload, len, buf, len) != EOK) {
        goto exit;
    }

    netbuf = oal_netbuf_alloc(len + HCC_NETBUF_RESERVED_ROOM_SIZE, 0, 4); /* align 4 */
    if (netbuf == HI_NULL) {
        oam_error_log0(0, 0, "hcc_send_heartbeat:: netbuf_alloc failed!");
        goto exit;
    }

    oal_netbuf_put(netbuf, len);
    if (memcpy_s(oal_netbuf_data(netbuf), len, event_payload, len) != EOK) {
        oam_error_log0(0, 0, "hmac_hcc_tx_event_buf_to_netbuf:: memcpy_s failed!");
        oal_netbuf_free(netbuf);
        goto exit;
    }
    /* 1. 初始化HCC头 */
    hcc_hdr_param_init(&param, HCC_ACTION_TYPE_OAM, WIFI_CONTROL_TYPE, ((hi_u32)sizeof(frw_hcc_extend_hdr_stru)),
        HCC_FC_NONE, DATA_HI_QUEUE);
    /* 2. add extend area (extend head include HCC, MAC_HDR) */
    oal_netbuf_push(netbuf, sizeof(frw_hcc_extend_hdr_stru));
    /* 3. 初始化extent hdr */
    event_hdr = frw_get_event_hdr(event_mem);
    ext_hdr = (frw_hcc_extend_hdr_stru *)oal_netbuf_data(netbuf);
    ext_hdr->nest_type = event_hdr->type;
    ext_hdr->nest_sub_type = event_hdr->sub_type;
    ext_hdr->vap_id = event_hdr->vap_id;
    ext_hdr->config_frame = 1;
    /* 4. 插入高优先队列 */
    ret = hcc_host_tx(hcc_handler, netbuf, &param);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "hcc_tx_netbuf_normal:: hcc_host_tx fail[%d]", ret);
    }
exit:
    if (event_mem != HI_NULL) {
        frw_event_free(event_mem);
    }
    return ret;
}

#ifdef _PRE_HDF_LINUX
static void hcc_send_heartbeat(oal_timer_list_stru *timer)
{
    hi_unref_param(timer);
#else
static void hcc_send_heartbeat(unsigned long data)
{
    hi_unref_param(data);
#endif
    hcc_handler_stru *hcc_handler = g_hcc_host_handler;

    if (hi_wifi_get_heart_beat_enable() == HI_TRUE) {
        hcc_heartbeat_stru heartbeat = { 0 };
        heartbeat.verify_code = VERIFY_CODE;
        heartbeat.timeout = g_heart_beat_device_timeout;
        hi_u32 ret = hcc_send_control_msg(hcc_handler, OAM_HEATBAET_TYPE, (hi_u8 *)&heartbeat, sizeof(heartbeat));
        if (ret != HI_SUCCESS) {
            oam_error_log1(0, 0, "hcc_send_heartbeat:: send heartbeat fail[%d]", ret);
        }
        oam_info_log3(0, 0, "g_priority_cnt=0x%x, g_lopriority_cnt=0x%x, g_des_cnt=0x%x", g_priority_cnt,
            g_lopriority_cnt, g_des_cnt);
    }

    if (hcc_handler != HI_NULL) {
        hi_u32 ret = oal_timer_start(&hcc_handler->hcc_transer_info.hcc_timer, HCC_HEARTBEAT_TIMEOUT);
        if (ret != 0 && ret != 1) {
            oam_error_log1(0, OAM_SF_FRW, "{hearbeat timer start fail: fail ret = %d}", ret);
        }
        hcc_handler->hcc_transer_info.hcc_timer_status = TIMER_STOP;
    }
}

hi_s32 hcc_heart_beat_enable_device_callback(hi_void *data)
{
    hi_unref_param(data);
    OAL_COMPLETE(&g_heart_beat_completion_ack);
    return HI_SUCCESS;
}
#ifndef _PRE_FEATURE_HCC_TASK
static FlowControlQueueID HccGetTxQueueId(const void *para)
{
    hcc_convert_queue_para *input = (hcc_convert_queue_para *)para;
    if (input->queueType == DATA_HI_QUEUE) {
        return CTRL_QUEUE_ID;
    } else if (input->queueType == DATA_LO_QUEUE && input->isVip) {
        return VIP_QUEUE_ID;
    } else {
        return NORMAL_QUEUE_ID;
    }
}
static FlowControlQueueID HccGetRxQueueId(const void *para)
{
    hcc_convert_queue_para *input = (hcc_convert_queue_para *)para;
    if (input->queueType == DATA_LO_QUEUE) {
        return VIP_QUEUE_ID;
    } else {
        return CTRL_QUEUE_ID;
    }
}
static int32_t HccGetFwPriorityId(FlowControlQueueID id)
{
    if (id == CTRL_QUEUE_ID) {
        return DATA_HI_QUEUE;
    } else {
        return DATA_LO_QUEUE;
    }
}
static int32_t HccGetRxPriorityId(FlowControlQueueID id)
{
    if (id == VIP_QUEUE_ID) {
        return DATA_LO_QUEUE;
    } else {
        return DATA_HI_QUEUE;
    }
}
struct FlowControlModule *g_fcm = NULL;
static struct FlowControlOp g_flowControlOp = {
    .isDeviceStaOrP2PClient = NULL,
    .txDataPacket = hcc_host_proc_tx_queue_impl,
    .rxDataPacket = hcc_host_proc_rx_queue_impl,
    .getTxQueueId = HccGetTxQueueId,
    .getRxQueueId = HccGetRxQueueId,
    .getTxPriorityId = HccGetFwPriorityId,
    .getRxPriorityId = HccGetRxPriorityId,
};
#endif

hi_u32 hcc_host_init(struct BusDev *bus)
{
#ifdef _PRE_FEATURE_HCC_TASK
    hi_u32 err_code;
#endif
    hcc_handler_stru *hcc_handler = HI_NULL;
    g_list_overflow_flag = HI_FALSE;
    g_abandon_key_frame = HI_FALSE;
    g_hcc_host_handler = HI_NULL;
    hcc_handler = (hcc_handler_stru *)oal_memalloc(sizeof(hcc_handler_stru));
    if (hcc_handler == HI_NULL) {
        oam_error_log0(0, 0, "hcc_host_init:: malloc hcc_handler fail!");
        return HI_FAIL;
    }
    if (memset_s(hcc_handler, sizeof(hcc_handler_stru), 0, sizeof(hcc_handler_stru)) != EOK) {
        oal_free(hcc_handler);
        return HI_FAIL;
    }
#ifndef _PRE_FEATURE_HCC_TASK
    g_fcm = InitFlowControl((void *)hcc_handler);
    if (g_fcm == HI_NULL) {
        oam_error_log0(0, 0, "hcc_host_init:: InitFlowControl failed!");
        goto flow_control_init_err;
    }
    oam_error_log0(0, 0, "hcc_host_init:: InitFlowControl success!");
    g_fcm->op = &g_flowControlOp;
#endif
    hcc_handler->hdr_rever_max_len = HCC_HDR_RESERVED_MAX_LEN;
    bus->priData.data = (void *)oal_bus_init_module(bus, (hi_void *)hcc_handler);
    if (bus->priData.data == HI_NULL) {
        oam_error_log0(0, 0, "hcc_host_init:: channel_init failed!");
        goto sdio_init_err;
    }
    bus->priData.release = oal_sdio_exit_module;
    hcc_handler->bus = bus;
    if (oal_bus_func_init(bus)) {
        oam_error_log0(0, 0, "hcc_host_init:: sdio_probe failed!");
        goto sdio_probe_err;
    }

    oal_timer_init(&hcc_handler->hcc_transer_info.hcc_timer, HCC_HEARTBEAT_TIMEOUT, hcc_send_heartbeat, 0);
    set_device_is_ready(HI_FALSE);
    oal_timer_add(&hcc_handler->hcc_transer_info.hcc_timer);
    hcc_handler->hcc_transer_info.hcc_timer_status = TIMER_ADD;
    hcc_message_register(hcc_handler, D2H_MSG_HEART_BEAT_OPEN_ACK, hcc_heart_beat_enable_device_callback, HI_NULL);
    hcc_message_register(hcc_handler, D2H_MSG_HEART_BEAT_CLOSE_ACK, hcc_heart_beat_enable_device_callback, HI_NULL);

    /* disable hcc default */
    oal_atomic_set(&hcc_handler->state, HCC_OFF);

    hi_wait_queue_init_head(&hcc_handler->hcc_transer_info.hcc_transfer_wq); /* queues init */
    hi_wait_queue_init_head(&hcc_handler->hcc_transer_info.hcc_rx_wq);
    hcc_transfer_queues_init(hcc_handler);
    hcc_trans_flow_ctrl_info_init(hcc_handler);

    OAL_MUTEX_INIT(&hcc_handler->tx_transfer_lock);

    hi_wait_queue_init_head(&hcc_handler->hcc_transer_info.tx_flow_ctrl.wait_queue);

#ifdef _PRE_FEATURE_HCC_TASK
    err_code = hcc_task_init(hcc_handler);
    if (err_code != HI_SUCCESS) {
        oam_error_log0(0, 0, "hcc_host_init:: hcc_task_init failed");
        goto hcc_tast_init_err;
    }
#endif

    if (oal_bus_transfer_rx_register(bus, sdio_transfer_rx_handler) != HI_SUCCESS) {
        oam_error_log0(0, 0, "hcc_host_init:: sdio rx transfer callback register failed");
        goto rx_cb_reg_failed;
    }

    hcc_assem_info_init(hcc_handler);
    hcc_trans_limit_parm_init(hcc_handler);
    hcc_trans_send_mode_init(hcc_handler);

    if (!oal_is_aligned(HISDIO_HOST2DEV_SCATT_SIZE, HISDIO_H2D_SCATT_BUFFLEN_ALIGN)) {
        oam_error_log0(0, 0, "hcc_host_init:: HISDIO_HOST2DEV_SCATT_SIZE failed");
    }

    if (hcc_tx_assem_descr_init(hcc_handler) != HI_SUCCESS) {
        oam_error_log0(0, 0, "hcc_host_init:: hcc_tx_assem_descr_init failed");
        goto failed_tx_assem_descr_alloc;
    }

    if (hcc_message_register(hcc_handler, D2H_MSG_FLOWCTRL_ON, hcc_flow_on_callback, hcc_handler) != HI_SUCCESS) {
        oam_error_log0(0, 0, "hcc_host_init:: hcc_message_register_flowctrl_on failed!");
        goto failed_reg_flowon_msg;
    }

    if (hcc_message_register(hcc_handler, D2H_MSG_FLOWCTRL_OFF, hcc_flow_off_callback, hcc_handler) != HI_SUCCESS) {
        oam_error_log0(0, 0, "hcc_host_init:: hcc_message_register_flowctrl_off failed!");
        goto failed_reg_flowoff_msg;
    }

    hcc_message_register(hcc_handler, D2H_MSG_CREDIT_UPDATE, hcc_credit_update_callback, hcc_handler);
    hcc_message_register(hcc_handler, D2H_MSG_HIGH_PKT_LOSS, hcc_high_pkts_loss_callback, hcc_handler);

    /* 注册维测回调处理函数 */
    if (hcc_host_rx_register(hcc_handler, HCC_ACTION_TYPE_OAM, hcc_oam_rx_log_func, HI_NULL) != HI_SUCCESS) {
        goto failed_reg_oam_fuction;
    }

    oal_wake_lock_init(&hcc_handler->tx_wake_lock, "hcc_tx");

    g_hcc_host_handler = hcc_handler;

    return HI_SUCCESS;
failed_reg_oam_fuction:
    hcc_message_unregister(hcc_handler, D2H_MSG_FLOWCTRL_OFF);
failed_reg_flowoff_msg:
    hcc_message_unregister(hcc_handler, D2H_MSG_FLOWCTRL_ON);
failed_reg_flowon_msg:
    hcc_tx_assem_descr_exit(hcc_handler);
failed_tx_assem_descr_alloc:
    oal_bus_transfer_rx_unregister(bus);
rx_cb_reg_failed:

#ifdef _PRE_FEATURE_HCC_TASK
    oal_kthread_stop(hcc_handler->hcc_transer_info.hcc_transfer_thread);
    hcc_handler->hcc_transer_info.hcc_transfer_thread = HI_NULL;
hcc_tast_init_err:
#endif
    OAL_MUTEX_DESTROY(&hcc_handler->tx_transfer_lock);
    oal_timer_delete(&hcc_handler->hcc_transer_info.hcc_timer);
    hcc_handler->hcc_transer_info.hcc_timer_status = TIMER_DEL;
sdio_probe_err:
    oal_bus_func_remove(bus);
sdio_init_err:
#ifndef _PRE_FEATURE_HCC_TASK
    DeInitFlowControl(g_fcm);
flow_control_init_err:
#endif
    oal_free(hcc_handler);
    return HI_FAIL;
}

hi_void hcc_clear_all_queues(hcc_handler_stru *hcc_handler, hi_s32 is_need_lock)
{
    hi_s32 i;
    oal_netbuf_head_stru *pst_head = HI_NULL;

    if (hcc_handler == HI_NULL) {
        oam_error_log0(0, 0, "hcc_clear_all_queues:: hcc_handler is NULL");
        return;
    }

    if (is_need_lock == HI_TRUE) {
        hcc_tx_transfer_lock(hcc_handler);
    }

    /* Restore assem queues */
    hcc_queue_type_enum type = hcc_handler->hcc_transer_info.tx_assem_info.assembled_queue_type;
    hcc_restore_assemble_netbuf_list(hcc_handler,
        &hcc_handler->hcc_transer_info.hcc_queues[HCC_TX].queues[type].data_queue);

    /* Clear all tx queues */
    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        hi_u32 list_len;
        pst_head = &hcc_handler->hcc_transer_info.hcc_queues[HCC_TX].queues[i].data_queue;
        list_len = oal_netbuf_list_len(pst_head);
        hcc_tx_netbuf_list_free(pst_head);
        if (list_len != 0) {
            oam_info_log2(0, 0, "hcc_clear_all_queues:: Clear queue:%d,total %u hcc rx pkts!\n", i, list_len);
        }
    }

    if (is_need_lock == HI_TRUE) {
        hcc_tx_transfer_unlock(hcc_handler);
    }

    /* Clear all rx queues */
    if (is_need_lock == HI_TRUE) {
        hcc_rx_transfer_lock(hcc_handler);
    }

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        hi_u32 list_len;
        pst_head = &hcc_handler->hcc_transer_info.hcc_queues[HCC_RX].queues[i].data_queue;
        list_len = oal_netbuf_list_len(pst_head);
        hcc_tx_netbuf_list_free(pst_head);
        if (list_len != 0) {
            oam_info_log2(0, 0, "hcc_clear_all_queues:: Clear queue:%d,total %u hcc rx pkts!\n", i, list_len);
        }
    }

    if (is_need_lock == HI_TRUE) {
        hcc_rx_transfer_unlock(hcc_handler);
    }
}

hi_void hcc_clear_rx_queues(hcc_handler_stru *hcc)
{
    hi_s32 i;
    oal_netbuf_head_stru *head = HI_NULL;
    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        head = &hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[i].data_queue;
        if (oal_netbuf_list_len(head) == 0) {
            continue;
        }
        for (; ;) {
            oal_netbuf_stru *netbuf = oal_netbuf_delist(head);
            if (netbuf == HI_NULL) {
                break;
            }
            oal_netbuf_free(netbuf);
        }
    }
}

hi_void hcc_delete_hearbeat_timer(hcc_handler_stru *hcc)
{
    hi_u16 retry_time = 10000;
    if (hcc == HI_NULL || hcc->hcc_transer_info.hcc_timer_status == TIMER_DEL) {
        return;
    }

    while (retry_time > 0 && hcc->hcc_transer_info.hcc_timer_status == TIMER_RUNING) {
        udelay(1);
        retry_time--;
    }
    printk("hcc_delete_hearbeat_timer\n");
    oal_timer_delete(&hcc->hcc_transer_info.hcc_timer);
    hcc->hcc_transer_info.hcc_timer_status = TIMER_DEL;
}

hi_void hcc_host_exit(hcc_handler_stru *hcc)
{
    struct BusDev *bus = NULL;
    printk("hcc_host_exit start\n");
    if (hcc == HI_NULL) {
        return;
    }
    bus = hcc->bus;
    hcc_delete_hearbeat_timer(hcc);
    oal_unregister_sdio_intr(bus);
    oal_wake_lock_exit(&hcc->tx_wake_lock);
    hcc_message_unregister(hcc, D2H_MSG_FLOWCTRL_OFF);
    hcc_message_unregister(hcc, D2H_MSG_FLOWCTRL_ON);
    hcc_message_unregister(hcc, D2H_MSG_HEART_BEAT_OPEN_ACK);
    hcc_message_unregister(hcc, D2H_MSG_HEART_BEAT_CLOSE_ACK);
    hcc_tx_assem_descr_exit(hcc);
#ifdef _PRE_FEATURE_HCC_TASK
    hcc_exit_task_thread(hcc);
    oal_kthread_stop(hcc->hcc_transer_info.hcc_transfer_thread);
    oal_kthread_stop(hcc->hcc_transer_info.hcc_rx_thread);
#endif
    hcc_clear_tx_queues(hcc);
    hcc_clear_rx_queues(hcc);
    oal_bus_transfer_rx_unregister(bus);
    oal_bus_send_msg(oal_get_sdio_default_handler(), H2D_MSG_PM_WLAN_OFF);
    hcc->hcc_transer_info.hcc_transfer_thread = HI_NULL;
    OAL_MUTEX_DESTROY(&hcc->tx_transfer_lock);
    oal_timer_delete_sync(&hcc->hcc_transer_info.tx_flow_ctrl.flow_timer);
    oal_cancel_delayed_work_sync(&hcc->hcc_transer_info.tx_flow_ctrl.worker);
    oal_bus_func_remove(bus);
    bus->priData.release(bus->priData.data);
    oal_free(hcc);
    g_hcc_host_handler = HI_NULL;
    printk("hcc_host_exit finished\n");
}

hi_s32 queues_flow_ctrl_check(hcc_handler_stru *hcc_handler, hcc_chan_type dir)
{
    hi_s32 i;
    hcc_trans_queue_stru *p_queue;
    hcc_tx_flow_ctrl_info_stru *p_flow_ctrl;

    p_queue = hcc_handler->hcc_transer_info.hcc_queues[dir].queues;
    p_flow_ctrl = &hcc_handler->hcc_transer_info.tx_flow_ctrl;
    for (i = 0; i < HCC_QUEUE_COUNT; i++, p_queue++) {
        /*
         * queue flow_ctrl function check
         * flow_ctrl function is enabled
         */
        if (p_queue->flow_ctrl.enable == HI_TRUE) {
            /*
             * Normal priority queue
             * flow_ctrl is disabled
             */
            if (p_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_SDIO) {
                if (oal_netbuf_list_len(&p_queue->data_queue) &&
                    (p_flow_ctrl->flowctrl_flag == D2H_MSG_FLOWCTRL_ON)) {
                    return HI_TRUE;
                }
            }
        } else { /* flow_ctrl function is disabled */
            /* Normal priority queue */
            if (p_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_SDIO) {
                /* queue length is not zero */
                if (oal_netbuf_list_len(&p_queue->data_queue)) {
                    return HI_TRUE;
                }
            } else { /* credit queue */
                /* queue length is not zero */
                if (oal_netbuf_list_len(&p_queue->data_queue)) {
                    return HI_TRUE;
                }
            }
        }
    }

    return HI_FALSE;
}

hi_s32 queues_len_check(hcc_handler_stru *hcc_handler, hcc_chan_type dir)
{
    hi_s32 i;
    hcc_trans_queue_stru *p_queue;

    p_queue = hcc_handler->hcc_transer_info.hcc_queues[dir].queues;

    for (i = 0; i < HCC_QUEUE_COUNT; i++, p_queue++) {
        if (oal_netbuf_list_len(&p_queue->data_queue)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

/* 是否正在进行心跳控制，防止重入 */
hi_bool g_heart_beat_enabling = HI_FALSE;
hi_s32 hi_wifi_enable_heart_beat(hi_bool enable)
{
    if (g_heart_beat_enabling == HI_TRUE) {
        return HI_FAIL;
    }

    g_heart_beat_enabling = HI_TRUE;
    OAL_INIT_COMPLETION(&g_heart_beat_completion_ack);
    struct BusDev *bus = oal_get_bus_default_handler();

    hi_s32 ret = 0;
    if (enable) {
        ret = oal_bus_send_msg(bus, H2D_MSG_HEART_BEAT_OPEN);
    } else {
        ret = oal_bus_send_msg(bus, H2D_MSG_HEART_BEAT_CLOSE);
    }

    if (ret != HI_SUCCESS) {
        g_heart_beat_enabling = HI_FALSE;
        return HI_FAIL;
    }

    hi_u32 wait_ret = oal_wait_for_completion_timeout(&g_heart_beat_completion_ack,
        (hi_u32)OAL_MSECS_TO_JIFFIES(WLAN_HEART_BEAT_ENABLE_WAIT_TIMEOUT));
    if (wait_ret != 0) { /* 等于0表示等待回复失败 */
        g_heart_beat_enable = enable;
        g_heart_beat_enabling = HI_FALSE;
        return HI_SUCCESS;
    }

    g_heart_beat_enabling = HI_FALSE;
    return HI_FAIL;
}

hi_bool hi_wifi_get_heart_beat_enable(hi_void)
{
    return g_heart_beat_enable;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
