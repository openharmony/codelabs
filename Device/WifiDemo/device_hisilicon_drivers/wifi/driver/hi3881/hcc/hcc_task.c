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

#include "oal_ext_if.h"
#include "hcc_task.h"
#include "hcc_host.h"
#include "oam_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HCC_TASK_PRIO 3
static volatile hi_u8 g_thread_exit_flag = HI_FALSE;
static hi_u8 g_thread_working = HI_FALSE;
hcc_task_stru g_hcc_task;

#define HCC_TASK_NAME          "hisi_hcc0"
#define HCC_TASK_TX_NAME       "hisi_hcc_tx"
#define HCC_TASK_RX_NAME       "hisi_hcc_rx"
#define TX_WRITE               0
#define RX_READ                1
#define DELAY_10_US            10
#define HCC_TASK_SIZE          0x4000

static hi_s32 hcc_thread_tx_wait_event_cond_check(hcc_handler_stru *hcc_handler)
{
    hi_s32 ret;
    /*
     * please first check the condition
     * which may be ok likely to reduce the cpu mips
     */
    ret = ((queues_len_check(hcc_handler, HCC_TX)) || ((hcc_handler->p_hmac_tcp_ack_need_schedule_func != HI_NULL) &&
        hcc_handler->p_hmac_tcp_ack_need_schedule_func() == HI_TRUE));
#ifdef _PRE_CONFIG_WLAN_THRANS_THREAD_DEBUG
    if (ret == HI_TRUE)
        hcc_handler->hcc_transer_info.thread_stat.wait_event_run_count++;
    if (ret == HI_FALSE)
        hcc_handler->hcc_transer_info.thread_stat.wait_event_block_count++;
#endif
    return ret;
}

static hi_s32 hcc_thread_rx_wait_event_cond_check(hcc_handler_stru *hcc_handler)
{
    hi_s32 ret;

    /*
     * please first check the condition
     * which may be ok likely to reduce the cpu mips
     */
    ret = ((queues_len_check(hcc_handler, HCC_RX)) || ((hcc_handler->p_hmac_tcp_ack_need_schedule_func != HI_NULL) &&
        hcc_handler->p_hmac_tcp_ack_need_schedule_func() == HI_TRUE));
#ifdef _PRE_CONFIG_WLAN_THRANS_THREAD_DEBUG
    if (ret == HI_TRUE)
        hcc_handler->hcc_transer_info.thread_stat.wait_event_run_count++;
    if (ret == HI_FALSE)
        hcc_handler->hcc_transer_info.thread_stat.wait_event_block_count++;
#endif
    return ret;
}

hi_s32 hcc_thread_process(hcc_handler_stru *hcc_handler, hi_u8 wr)
{
    hi_s32 ret = 0;
#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
    oal_bool_enum_uint8 en_device_is_sta;
    OAL_STATIC oal_uint32 s_ul_schedule_cnt = 0;

    en_device_is_sta = hcc_flowctl_get_device_mode(hcc);

    s_ul_schedule_cnt++;

    if (en_device_is_sta == OAL_TRUE) {
        /* Tx Tcp Data queue > Rx Tcp Ack
           Rx Tcp Data > Tx Tcp Ack
           Tx Tcp Data queue > Rx Tcp Data queue */
        ret += hcc_host_send_rx_queue(hcc, CTRL_QUEUE);
        ret += hcc_host_send_tx_queue(hcc, CTRL_QUEUE);

        ret += hcc_host_send_rx_queue(hcc, DATA_HI_QUEUE);
        ret += hcc_host_send_tx_queue(hcc, DATA_HI_QUEUE);

        if ((s_ul_schedule_cnt % LO_QUEUE_ADJUST_PRIORITY_PERIOD) == 0) {
            /* Tx Lo < Rx Lo */
            ret += hcc_host_send_rx_queue(hcc, DATA_LO_QUEUE);
            ret += hcc_host_send_tx_queue(hcc, DATA_LO_QUEUE);

            /* 下行TCP优先 */
            ret += hcc_host_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
            ret += hcc_host_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE);

            ret += hcc_host_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE);
            ret += hcc_host_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);
        } else {
            /* 下行TCP优先 */
            ret += hcc_host_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
            ret += hcc_host_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE);

            ret += hcc_host_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE);
            ret += hcc_host_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);

            /* Tx Lo < Rx Lo */
            ret += hcc_host_send_rx_queue(hcc, DATA_LO_QUEUE);
            ret += hcc_host_send_tx_queue(hcc, DATA_LO_QUEUE);
        }
        ret += hcc_host_send_rx_queue(hcc, DATA_UDP_VO_QUEUE);
        ret += hcc_host_send_tx_queue(hcc, DATA_UDP_VO_QUEUE);

        ret += hcc_host_send_rx_queue(hcc, DATA_UDP_VI_QUEUE);
        ret += hcc_host_send_tx_queue(hcc, DATA_UDP_VI_QUEUE);

        ret += hcc_host_send_rx_queue(hcc, DATA_UDP_BE_QUEUE);
        ret += hcc_host_send_tx_queue(hcc, DATA_UDP_BE_QUEUE);

        ret += hcc_host_send_rx_queue(hcc, DATA_UDP_BK_QUEUE);
        ret += hcc_host_send_tx_queue(hcc, DATA_UDP_BK_QUEUE);
    } else {
        /* Tx Tcp Data queue > Rx Tcp Ack
           Rx Tcp Data > Tx Tcp Ack
           Tx Tcp Data queue < Rx Tcp Data queue */
        ret += hcc_host_send_tx_queue(hcc, CTRL_QUEUE);
        ret += hcc_host_send_rx_queue(hcc, CTRL_QUEUE);

        ret += hcc_host_send_tx_queue(hcc, DATA_HI_QUEUE);
        ret += hcc_host_send_rx_queue(hcc, DATA_HI_QUEUE);

        if ((s_ul_schedule_cnt % LO_QUEUE_ADJUST_PRIORITY_PERIOD) == 0) {
            /* Tx Lo > Rx Lo */
            ret += hcc_host_send_tx_queue(hcc, DATA_LO_QUEUE);
            ret += hcc_host_send_rx_queue(hcc, DATA_LO_QUEUE);

            ret += hcc_host_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE);
            ret += hcc_host_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);

            ret += hcc_host_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
            ret += hcc_host_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE);
        } else {
            ret += hcc_host_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE);
            ret += hcc_host_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);

            ret += hcc_host_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
            ret += hcc_host_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE);

            /* Tx Lo > Rx Lo */
            ret += hcc_host_send_tx_queue(hcc, DATA_LO_QUEUE);
            ret += hcc_host_send_rx_queue(hcc, DATA_LO_QUEUE);
        }

        /* udp业务 */
        ret += hcc_host_send_tx_queue(hcc, DATA_UDP_VO_QUEUE);
        ret += hcc_host_send_rx_queue(hcc, DATA_UDP_VO_QUEUE);

        ret += hcc_host_send_tx_queue(hcc, DATA_UDP_VI_QUEUE);
        ret += hcc_host_send_rx_queue(hcc, DATA_UDP_VI_QUEUE);

        ret += hcc_host_send_tx_queue(hcc, DATA_UDP_BE_QUEUE);
        ret += hcc_host_send_rx_queue(hcc, DATA_UDP_BE_QUEUE);

        ret += hcc_host_send_tx_queue(hcc, DATA_UDP_BK_QUEUE);
        ret += hcc_host_send_rx_queue(hcc, DATA_UDP_BK_QUEUE);
    }
#else
    if (wr == TX_WRITE) {
        ret += hcc_host_proc_tx_queue(hcc_handler, DATA_HI_QUEUE);
        ret += hcc_host_proc_tx_queue(hcc_handler, DATA_LO_QUEUE);
    } else {
        ret += hcc_host_proc_rx_queue(hcc_handler, DATA_HI_QUEUE);
        ret += hcc_host_proc_rx_queue(hcc_handler, DATA_LO_QUEUE);
    }
#endif
    return ret;
}

hi_u8 hcc_get_thread_exit_flag(hi_void)
{
    return g_thread_exit_flag;
}

hi_void hcc_exit_task_thread(hcc_handler_stru *hcc)
{
    hi_u16 retry_time = 10000;
    g_thread_exit_flag = HI_TRUE;
    hcc_sched_transfer(hcc);
    while (g_thread_working && retry_time > 0) {
        udelay(DELAY_10_US);
        retry_time--;
    }
}

static hi_s32 hcc_task_rx_thread(hi_void *data)
{
    hi_s32 hcc_ret = 0;
#ifdef _PRE_WLAN_TCP_OPT
    static hi_u8 ack_loop_count = 0;
#endif
    hcc_handler_stru *hcc_handler = (hcc_handler_stru *)data;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    allow_signal(SIGTERM);
#endif
    oam_info_log0(0, 0, "hcc_task_rx_thread:: hcc_task_rx_thread enter");
    g_thread_exit_flag = HI_FALSE;
    for (; ;) {
        if (g_thread_exit_flag == HI_TRUE || oal_kthread_should_stop()) {
            oam_warning_log0(0, 0, "hcc_task_thread:: hcc_task leave");
            break;
        }
        hcc_ret = hi_wait_event_interruptible(hcc_handler->hcc_transer_info.hcc_rx_wq,
            (hcc_thread_rx_wait_event_cond_check(hcc_handler) == HI_TRUE));
        if (hcc_ret == -ERESTARTSYS || g_thread_exit_flag == HI_TRUE) {
            oam_warning_log0(0, 0, "hcc_task_rx_thread:: hcc_task was interupterd by a singnal");
            break;
        }
#ifdef _PRE_WLAN_TCP_OPT
        if (hcc_handler->p_hmac_tcp_ack_process_func != HI_NULL) {
            ack_loop_count++;
            if (ack_loop_count >= g_ul_tcp_ack_wait_sche_cnt) {
                ack_loop_count = 0;
                hcc_handler->p_hmac_tcp_ack_process_func();
            }
        }
#endif
        hcc_thread_process(hcc_handler, RX_READ);
    }
    return 0;
}

/* ****************************************************************************S
 hcc task 线程主程序
**************************************************************************** */
static hi_s32 hcc_task_tx_thread(hi_void *data)
{
    hi_s32 hcc_ret = 0;
#ifdef _PRE_WLAN_TCP_OPT
    static hi_u8 ack_loop_count = 0;
#endif
    hcc_handler_stru *hcc_handler = (hcc_handler_stru *)data;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    allow_signal(SIGTERM);
#endif
    oam_info_log0(0, 0, "hcc_task_tx_thread:: hcc_task_tx_thread enter");
    g_thread_exit_flag = HI_FALSE;
    g_thread_working = HI_TRUE;
    for (; ;) {
        if (g_thread_exit_flag == HI_TRUE || oal_kthread_should_stop()) {
            oam_warning_log0(0, 0, "hcc_task_tx_thread:: hcc_task leave");
            break;
        }
        hcc_ret = hi_wait_event_interruptible(hcc_handler->hcc_transer_info.hcc_transfer_wq,
            (hcc_thread_tx_wait_event_cond_check(hcc_handler) == HI_TRUE));
        if (hcc_ret == -ERESTARTSYS || g_thread_exit_flag == HI_TRUE) {
            oam_warning_log0(0, 0, "hcc_task_tx_thread:: hcc_task was interupterd by a singnal");
            break;
        }
#ifdef _PRE_WLAN_TCP_OPT
        if (hcc_handler->p_hmac_tcp_ack_process_func != HI_NULL) {
            ack_loop_count++;
            if (ack_loop_count >= g_ul_tcp_ack_wait_sche_cnt) {
                ack_loop_count = 0;
                hcc_handler->p_hmac_tcp_ack_process_func();
            }
        }
#endif
        hcc_thread_process(hcc_handler, TX_WRITE);
    }
    g_thread_working = HI_FALSE;
    return hcc_ret;
}

hi_u32 hcc_task_init(hcc_handler_stru *hcc_handler)
{
    oal_kthread_param_stru thread_param = { 0 };

    memset_s(&g_hcc_task, sizeof(hcc_task_stru), 0, sizeof(hcc_task_stru));

    hi_wait_queue_init_head(&g_hcc_task.hcc_wq);

    memset_s(&thread_param, sizeof(oal_kthread_param_stru), 0, sizeof(oal_kthread_param_stru));
    thread_param.l_cpuid = 0;
    thread_param.l_policy = OAL_SCHED_FIFO;
    thread_param.l_prio = HCC_TASK_PRIO;
    thread_param.ul_stacksize = HCC_TASK_SIZE;

    hcc_handler->hcc_transer_info.hcc_transfer_thread =
        oal_kthread_create(HCC_TASK_TX_NAME, hcc_task_tx_thread, hcc_handler, &thread_param);
    hcc_handler->hcc_transer_info.hcc_rx_thread =
        oal_kthread_create(HCC_TASK_RX_NAME, hcc_task_rx_thread, hcc_handler, &thread_param);
    if (IS_ERR_OR_NULL(hcc_handler->hcc_transer_info.hcc_transfer_thread) ||
        IS_ERR_OR_NULL(hcc_handler->hcc_transer_info.hcc_rx_thread)) {
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
