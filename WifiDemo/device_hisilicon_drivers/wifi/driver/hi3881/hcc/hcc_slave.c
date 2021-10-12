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
#include "hcc_slave.h"
#include "oal_net.h"
#include "oal_util.h"
#include "oal_mem.h"
#include "hcc_comm.h"
#include "hcc_task.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  1 全局变量定义
**************************************************************************** */
#define HCC_FRW_EVENT_SIZE           WLAN_MEM_EVENT_SIZE1
#define SHIF_8_BIT                   8
#define SHIF_16_BIT                  16
#define SHIF_24_BIT                  24

voidfuncptr g_pm_wlan_forceslp_msg_proc = HI_NULL;
oal_dev_netbuf_stru *g_reserve_netbuff = HI_NULL; /* 用于内存申请不到时的备用内存 */
oal_dev_netbuf_head_stru g_hcc_tx_queue;
oal_dev_netbuf_head_stru g_hcc_rx_queue;
volatile hi_u8 g_sdio_init_ok = HI_FALSE;
struct hcc_handler g_hcc_slave_handler;

typedef hi_void (*hcc_descr_ctrl_func)(hi_void *data);
hcc_descr_ctrl_func g_descr_control_func[HCC_DESCR_TYPE_BUTT];
hcc_netbuf_queue_type g_current_queue_type = HCC_NETBUF_NORMAL_QUEUE;

hi_u32 g_hcc_panic_flag = 0;
hi_u16 g_used_mem_forstop = 20;
hi_u16 g_used_mem_forstart = 40;
hi_u32 g_flow_ctrl_reg_info = 0x0;
hi_u32 g_max_adma_descr_pkt_len = 0;
hi_u8 g_flowctrl_flashed = HI_FALSE;
hcc_msg_cb g_process_msg_callback = NULL;

volatile hcc_slave_tx_info g_tx_aggr_info = { 0 };
volatile hcc_slave_rx_info g_rx_aggr_info = { 0 };
hi_u32 g_flowctrl_pending = 0;   /* 1 means pending, 0 means no pending */
hi_u8  g_h2d_assem_descr[HISDIO_HOST2DEV_SCATT_SIZE];  /* Used to receive the first assem info descr. */
hi_u8  g_thruput_bypass_enable[HCC_THRUPUT_BYPASS_BUTT] = {0};

hi_u8  g_sdio_txpkt_index = 0;   /* 发送帧序号 */

/* HOST下发数据过程标志,SDIO_RX_BUSY表示正在传输,SDIO_RX_IDLE表示传输结束 */
hi_u8  g_chan_write_flag = CHAN_RX_IDLE;
hi_u32 g_d2h_hcc_assemble_count = HISDIO_DEV2HOST_SCATT_MAX;


/* ****************************************************************************
  2 函数实现
**************************************************************************** */
struct hcc_handler *hcc_get_default_handler(hi_void)
{
    return &g_hcc_slave_handler;
}

hi_u8 hcc_get_thruput_bypass_enable(hcc_thruput_bypass bypass_type)
{
    if (bypass_type >= HCC_THRUPUT_BYPASS_BUTT) {
        return 0;
    }
    return g_thruput_bypass_enable[bypass_type];
}

hi_void hcc_set_thruput_bypass_enable(hcc_thruput_bypass bypass_type, hi_u8 value)
{
    if (bypass_type >= HCC_THRUPUT_BYPASS_BUTT) {
        return;
    }

    g_thruput_bypass_enable[bypass_type] = value;
}

/* ****************************************************************************
 功能描述  : 获取hcc TCM头得扩展区有效数据首地址

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 *hcc_get_extend_payload_addr(const oal_dev_netbuf_stru *dev_netbuf)
{
    hi_u8 *hdr = HI_NULL;
    hcc_header_stru *hcc_hdr = HI_NULL;

    /* Device侧Netbuf 头/体分离,扩展区默认从高向低地址扩展! */
    hdr = oal_dev_netbuf_hcchdr(dev_netbuf);
    if (hdr == HI_NULL) {
        return HI_NULL;
    }
    hcc_hdr = (hcc_header_stru *)hdr;
    hdr += HCC_HDR_LEN;
    hdr += hcc_hdr->pad_hdr;
    if (hcc_hdr->pad_hdr > HCC_HDR_RESERVED_MAX_LEN) {
        return HI_NULL;
    }
    return hdr;
}

/* ****************************************************************************
 功能描述  : 获取hcc TCM头得扩展区首地址

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 *hcc_get_extend_addr(const oal_dev_netbuf_stru *dev_netbuf, hi_u32 extend_len)
{
    hi_u8 *hdr = HI_NULL;
    /* Device侧Netbuf 头/体分离,扩展区默认从高向低地址扩展! */
    if (extend_len > (hi_u32)HCC_HDR_RESERVED_MAX_LEN) {
        return HI_NULL;
    }

    hdr = oal_dev_netbuf_hcchdr(dev_netbuf);
    hdr += HCC_HDR_LEN;
    hdr += HCC_HDR_RESERVED_MAX_LEN - extend_len;
    return hdr;
}

hi_void hcc_delay_bug_on_detect(hi_void)
{
    if (g_hcc_panic_flag != 0) {
        g_hcc_panic_flag = 0;
        hi_diag_log_msg_e0(0, "hcc panic detected.\n");
    }
}

#undef CONFIG_HCC_NETBUF_LIST_REPEAT_CHECK
#ifdef CONFIG_HCC_NETBUF_LIST_REPEAT_CHECK
hi_u32 hcc_netbuf_list_repeat_check(oal_dev_netbuf_stru *dev_netbuf, oal_dev_netbuf_head_stru *head)
{
    hi_u32 status;
    oal_dev_netbuf_stru *dev_netbuf_temp = HI_NULL;

    status = hi_int_lock();
    dev_netbuf_temp = head->next;
    while ((hi_u32)dev_netbuf_temp != (hi_u32)head) {
        if (dev_netbuf_temp == dev_netbuf) {
            hi_int_restore(status);
            return HI_FALSE;
        }
        dev_netbuf_temp = dev_netbuf_temp->next;
    }

    hi_int_restore(status);
    return HI_TRUE;
}
#endif

hi_void hcc_netbuf_add_to_list_tail(oal_dev_netbuf_stru *dev_netbuf, oal_dev_netbuf_head_stru *head)
{
    hi_u32 status;

#ifdef CONFIG_HCC_NETBUF_LIST_REPEAT_CHECK
    if (hcc_netbuf_list_repeat_check(dev_netbuf, head) != HI_TRUE) {
        return;
    }
#endif

    status = hi_int_lock();
    head->prev->next = dev_netbuf;
    dev_netbuf->next = (oal_dev_netbuf_stru *)head;
    head->prev = dev_netbuf;
    head->num++;
    hi_int_restore(status);
}

oal_dev_netbuf_stru *hcc_netbuf_delist(oal_dev_netbuf_head_stru *head)
{
    hi_u32 status;
    oal_dev_netbuf_stru *dev_netbuf = HI_NULL;

    status = hi_int_lock();
    if (head->num == 0) {
        hi_int_restore(status);
        return HI_NULL;
    }

    if ((uintptr_t)head->prev == (uintptr_t)head) {
        hi_int_restore(status);
        return HI_NULL;
    }

    dev_netbuf = head->next;
    if (head->num == 1) {
        /* one node */
        head->prev = (oal_dev_netbuf_stru *)head;
    }
    head->next = (oal_dev_netbuf_stru *)head->next->next;

    head->num--;
    hi_int_restore(status);
    return dev_netbuf;
}

hi_u32 hcc_slave_get_netbuf_align_len(const oal_dev_netbuf_stru *dev_netbuf, hi_u32 align_size)
{
    hi_u32 send_len;
    hi_u32 total_len;

    hcc_header_stru *hcc_hdr = (hcc_header_stru *)oal_dev_netbuf_hcchdr(dev_netbuf);
    if (hcc_hdr == HI_NULL) {
        hi_diag_log_msg_e0(0, "{hcc_slave_get_netbuf_align_len::hcc_hdr null.}");
        return 0;
    }
    total_len = hcc_hdr->pay_len + HCC_HDR_TOTAL_LEN;

    send_len = hi_byte_align(total_len, align_size);
    return send_len;
}

hi_void hcc_slave_build_header(const oal_dev_netbuf_stru *dev_netbuf, hi_u16 pay_load_len,
    const hcc_transfer_param *param)
{
    hcc_header_stru *hcc_hdr = HI_NULL;

    if ((dev_netbuf == HI_NULL) || (param == HI_NULL)) {
        hi_diag_log_msg_e0(0, "{hcc_slave_build_header:: net_buf/param null.}");
        return;
    }

    /* build hcc header */
    hcc_hdr = (hcc_header_stru *)oal_dev_netbuf_hcchdr(dev_netbuf);
    if (hcc_hdr == HI_NULL) {
        hi_diag_log_msg_e0(0, "{hcc_slave_build_header:: hcc_hdr null.}");
        return;
    }
    hcc_hdr->main_type   = param->main_type;
    hcc_hdr->sub_type    = param->sub_type;
    hcc_hdr->pay_len     = pay_load_len;
    hcc_hdr->seq         = g_sdio_txpkt_index++;
    hcc_hdr->pad_hdr     = HCC_HDR_RESERVED_MAX_LEN - param->extend_len;
    hcc_hdr->pad_payload = 0;  /* Device alloc netbuf's payload all 4B aligned! */
}

hi_void hcc_rx_process(oal_dev_netbuf_stru *dev_netbuf)
{
    hcc_netbuf_add_to_list_tail(dev_netbuf, &g_hcc_rx_queue);
    /* sched the hcc rx */
    hcc_task_sched();
}

/* ****************************************************************************
 功能描述  : 发送sdio 消息  低16 bits有效

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_s32 hcc_send_msg2host(hi_u32 msg)
{
    return hi_sdio_send_sync_msg(msg);
}

hi_s32 hcc_device2host_netbuf_multi_post_check(oal_dev_netbuf_stru *dev_netbuf, hi_s32 num)
{
#ifdef CONFIG_HCC_TX_MULTI_BUF_CHECK
    hi_s32 i;
    hi_u32 status;

    status = hi_int_lock();
    for (i = 0; i < num; i++) {
        if (g_tx_aggr_info.hcc_tx_aggr[i].netbuf == dev_netbuf) {
            hi_int_restore(status);
            hi_diag_log_msg_i3(0, "[ERROR][d2h]netbuf[%d], num: %d[%p]multi post!\r\n", i, num,
                g_tx_aggr_info.hcc_tx_aggr[i].netbuf);
            return HI_FAIL;
        }
    }
    hi_int_restore(status);
#else
    hi_unref_param(dev_netbuf);
    hi_unref_param(num);
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 发hcc消息到host侧，更新高优先级buffer credit值

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hcc_update_high_priority_buffer_credit(hi_u8 free_large_buffer, hi_u8 free_mgmt_buffer, hi_u8 tx_dscr_free_cnt)
{
    hi_sdio_extendfunc *hcc_extfunc = hi_sdio_get_extend_info();

    memset_s(hcc_extfunc, sizeof(hi_sdio_extendfunc), 0, sizeof(hi_sdio_extendfunc));
    /* 8-15bit 大包free个数；16-23bit管理帧free个数 */
    g_flow_ctrl_reg_info = ((g_flow_ctrl_reg_info & 0x000000FF) |
        ((free_large_buffer & 0xFF) << SHIF_8_BIT) |
        ((free_mgmt_buffer & 0xFF) << SHIF_16_BIT) |
        ((tx_dscr_free_cnt & 0xFF) << SHIF_24_BIT));
    if (hi_sdio_is_pending_msg(D2H_MSG_CREDIT_UPDATE) || hi_sdio_is_sending_msg(D2H_MSG_CREDIT_UPDATE)) {
        g_flowctrl_flashed = HI_TRUE;
        return;
    }
    /* 更新寄存器 */
    hcc_extfunc->credit_info = g_flow_ctrl_reg_info;
    hcc_extfunc->credit_isvalid = 1;
    (hi_void)hi_sdio_write_extinfo(hcc_extfunc);
    g_flowctrl_flashed = HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 从netbuf创建DMA表

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hcc_build_adma_descr_from_netbuf(hi_u8 *adma_table, hi_u32 *index, hi_u32 pkt_len,
    const oal_dev_netbuf_stru *dev_netbuf)
{
    hi_u32 *hcc_hdr = HI_NULL;
    hi_u32 *payload_hdr = HI_NULL;

    if (dev_netbuf == HI_NULL) {
        return;
    }

    hcc_hdr = (hi_u32 *)oal_dev_netbuf_hcchdr(dev_netbuf);
    payload_hdr = (hi_u32 *)oal_dev_netbuf_get_payload(dev_netbuf);

    /* 填写MAC头传输DMA表 */
    if (pkt_len >= HCC_HDR_TOTAL_LEN) {
        (hi_void)hi_sdio_set_admatable(adma_table, (*index)++, (hi_u32 *)hcc_hdr, HCC_HDR_TOTAL_LEN);
    }

    /* 填写Payload头传输DMA表 */
    if (pkt_len > HCC_HDR_TOTAL_LEN) {
        (hi_void)hi_sdio_set_admatable(adma_table, (*index)++, (hi_u32 *)payload_hdr, pkt_len - HCC_HDR_TOTAL_LEN);
    }

    if (g_max_adma_descr_pkt_len < pkt_len) {
        g_max_adma_descr_pkt_len = pkt_len;
    }
}

hi_void hcc_rx_assem_info_reset(hi_void *data)
{
    hi_unref_param(data);
    for (hi_u16 i = 0; i < g_rx_aggr_info.aggr_rx_num; i++) {
        g_rx_aggr_info.hcc_rx_aggr[i].netbuf = HI_NULL;
        g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes = 0;
    }

    /* Reset assem info package */
    g_rx_aggr_info.aggr_rx_num = 0;
    g_rx_aggr_info.rx_aggr_total_len = 0;
}

hi_void hcc_rx_netbuf_queue_pri_switch(const hi_void *data)
{
    hcc_netbuf_queue_type queue_type = *(hcc_netbuf_queue_type *)data;

    if (queue_type >= HCC_NETBUF_QUEUE_BUTT) {
        return;
    }

    g_current_queue_type = queue_type;
}

hi_void hcc_process_single_package(hi_u32 len)
{
    hi_u32 pkt_len;
    hi_u32 remain_len;
    hi_u32 size = len;
    hi_u32 ret;
    oal_dev_netbuf_stru *dev_netbuf = HI_NULL;

    g_rx_aggr_info.aggr_rx_num = 1;
    /* strip the assem info */
    size -= HISDIO_HOST2DEV_SCATT_SIZE;
    if (size > HCC_LONG_PACKAGE_SIZE + HCC_HDR_TOTAL_LEN) {
        remain_len = size - HCC_LONG_PACKAGE_SIZE - HCC_HDR_TOTAL_LEN;
        pkt_len = HCC_LONG_PACKAGE_SIZE;
    } else {
        remain_len = 0;
        pkt_len = size - HCC_HDR_TOTAL_LEN;
    }

    /* single package, we think it's one pkt assembled, and if the pkt is too long,
       the pkt must algin to 512 */
    g_rx_aggr_info.hcc_rx_aggr[0].rx_pkt_bytes = pkt_len + HCC_HDR_TOTAL_LEN;

    /* 高优先级队列调用高优先级buffer申请接口 */
    if (g_current_queue_type == HCC_NETBUF_HIGH_QUEUE) {
        dev_netbuf = oal_mem_dev_netbuf_alloc(OAL_NORMAL_NETBUF, (hi_u16)pkt_len, OAL_NETBUF_PRIORITY_HIGH);
    } else {
        dev_netbuf = oal_mem_dev_netbuf_alloc(OAL_NORMAL_NETBUF, (hi_u16)pkt_len, OAL_NETBUF_PRIORITY_MID);
    }

    if (dev_netbuf == HI_NULL) {
        ret = hi_sdio_procmem_alloc_fail(g_current_queue_type, (hi_void**) &(g_rx_aggr_info.hcc_rx_aggr[0].netbuf),
                                         (hi_void*)g_reserve_netbuff);
        if (ret != HI_SUCCESS) {
            return;
        }
    } else {
        g_rx_aggr_info.hcc_rx_aggr[0].netbuf = dev_netbuf;
    }

    /* If single pkt too long? */
    if (remain_len) {
        g_rx_aggr_info.aggr_rx_num++;
        g_rx_aggr_info.hcc_rx_aggr[1].netbuf = g_reserve_netbuff;
        g_rx_aggr_info.hcc_rx_aggr[1].rx_pkt_bytes = remain_len;

        /* 单包过长，Host有下发限制，不应该走到这里。 */
        if (remain_len > HCC_LONG_PACKAGE_SIZE) {
            g_hcc_panic_flag = 1;
            hi_diag_log_msg_e1(0, "ERROR: remain_len is %d\r\n", remain_len);
        }
    }
    /* Total Len Useless!TBD */
    g_rx_aggr_info.rx_aggr_total_len = size - HISDIO_HOST2DEV_SCATT_SIZE;
}

hi_void hcc_process_multi_package(hi_void)
{
    hi_u32 i;
    hi_u16 rx_pkt_len = 0;
    oal_dev_netbuf_stru *dev_netbuf = HI_NULL;

    /* assem transfer, host to device */
    for (i = 0; i < g_rx_aggr_info.aggr_rx_num; i++) {
        if (g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes <= (HCC_LONG_PACKAGE_SIZE + HCC_HDR_TOTAL_LEN)) {
            rx_pkt_len = (hi_u16)(g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes - HCC_HDR_TOTAL_LEN);
            dev_netbuf = oal_mem_dev_netbuf_alloc(OAL_NORMAL_NETBUF, rx_pkt_len, OAL_NETBUF_PRIORITY_HIGH);
        } else {
            dev_netbuf = HI_NULL;
        }

        if (dev_netbuf == HI_NULL) {
            hi_sdio_procmem_alloc_fail(g_current_queue_type,
                                       (hi_void**) &(g_rx_aggr_info.hcc_rx_aggr[i].netbuf),
                                       (hi_void*) g_reserve_netbuff);

            /* 聚合包每包大小超过了DEVICE最大内存池的长度，预留内存接收将会踩狗牌。 */
            if (g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes > HCC_LONG_PACKAGE_SIZE + HCC_HDR_TOTAL_LEN) {
                hi_diag_log_msg_e0(0, "rx_pkt is too large.\r\n");
            }
#ifdef CONFIG_HCC_DEBUG
            hi_diag_log_msg_i1(0, "netbuf fail,len:%d\n",
                g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes - HCC_HDR_TOTAL_LEN);
#endif
        } else {
            g_rx_aggr_info.hcc_rx_aggr[i].netbuf = dev_netbuf;
        }
    }
}

hi_s32 hcc_write_start_callback(hi_u32 len, hi_u8 *adma_table)
{
    hi_u32 index = 0;
    hi_u32 pad_len = 0;
    hi_u32 non_pad_total_len = 0;

    if (len < HISDIO_HOST2DEV_SCATT_SIZE) {
        return 0;
    }

    /* The flag protect sdio host to device transfer! */
    g_chan_write_flag = CHAN_RX_BUSY;
    g_rx_aggr_info.trans_len = len;

    /* The frist descr fixed to assem descr! */
    (hi_void)hi_sdio_set_admatable(adma_table, index++, (hi_u32 *)g_h2d_assem_descr, HISDIO_HOST2DEV_SCATT_SIZE);

    non_pad_total_len += HISDIO_HOST2DEV_SCATT_SIZE;

    if (g_rx_aggr_info.trans_len == HISDIO_HOST2DEV_SCATT_SIZE) {
        /* when transfer just assem scatt size buff, it's control message from host! */
        (hi_void)hi_sdio_complete_send(adma_table, index);
        return (hi_s32)index;
    }

    if (g_rx_aggr_info.aggr_rx_num == 0) {
        hcc_process_single_package(len);
    } else {
        hcc_process_multi_package();
    }

    if (g_rx_aggr_info.aggr_rx_num > HISDIO_HOST2DEV_SCATT_MAX) {
        g_hcc_panic_flag = 1;
        hi_diag_log_msg_e1(0, "Error: rx_num(%d) more than SCATT_MAX\r\n", g_rx_aggr_info.aggr_rx_num);
        return 0;
    }

    for (hi_u32 i = 0; i < g_rx_aggr_info.aggr_rx_num; i++) {
        non_pad_total_len += g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes;
        hcc_build_adma_descr_from_netbuf(adma_table,
                                         &index,
                                         g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes,
                                         g_rx_aggr_info.hcc_rx_aggr[i].netbuf);
    }

    if (non_pad_total_len < len) {
        pad_len = len - non_pad_total_len;
    }

    if (pad_len != 0) {
        (hi_void)hi_sdio_set_pad_admatab(pad_len, adma_table, index++);
    }

    /* IP问题需多加一行无用数据，表示表格终止 */
    (hi_void)hi_sdio_complete_send(adma_table, index);

    return (hi_s32)index;
}

hi_void hcc_write_over_aggr_package(hi_void)
{
    hi_u32 i;
    hi_u16 pkt_len;
    hi_u16 total_len = 0;
    hi_u8 *h2d_assem_descr = HI_NULL;

    for (i = 0; i < g_rx_aggr_info.aggr_rx_num; i++) {
        if (((uintptr_t)g_rx_aggr_info.hcc_rx_aggr[i].netbuf != (uintptr_t)g_reserve_netbuff)) {
            if (hcc_get_thruput_bypass_enable(HCC_TX_SDIO_SLAVE_BYPASS)) {
                oal_mem_dev_netbuf_free(g_rx_aggr_info.hcc_rx_aggr[i].netbuf);
            } else {
                /* not reserved mem */
                hcc_rx_process(g_rx_aggr_info.hcc_rx_aggr[i].netbuf);
            }
        }

        g_rx_aggr_info.hcc_rx_aggr[i].netbuf = HI_NULL;
        g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes = 0;
    }

    /* prepare the next assem info */
    g_rx_aggr_info.aggr_rx_num = 0;
    g_rx_aggr_info.rx_aggr_total_len = 0;

    /* assem info 64B */
    h2d_assem_descr = (hi_u8 *)g_h2d_assem_descr;
    for (i = 0; i < HISDIO_HOST2DEV_SCATT_MAX; i++) {
        if (h2d_assem_descr[i] == 0) {
            break;
        }

        pkt_len = (h2d_assem_descr[i] << HISDIO_H2D_SCATT_BUFFLEN_ALIGN_BITS);

        g_rx_aggr_info.hcc_rx_aggr[i].netbuf = HI_NULL;
        g_rx_aggr_info.hcc_rx_aggr[i].rx_pkt_bytes = pkt_len;
        total_len += pkt_len;
    }

    g_rx_aggr_info.aggr_rx_num = i;
    g_rx_aggr_info.rx_aggr_total_len = total_len;
}


hi_s32 hcc_write_over_callback(hi_void)
{
    struct hcc_descr_header *dscr_hdr = HI_NULL;
    hi_void *h2d_ctrl_descr = (hi_void *)g_h2d_assem_descr;

    if (g_rx_aggr_info.trans_len == HISDIO_HOST2DEV_SCATT_SIZE) {
        /* dscr control data */
        dscr_hdr = (struct hcc_descr_header *)h2d_ctrl_descr;
        if (dscr_hdr->descr_type < HCC_DESCR_TYPE_BUTT && g_descr_control_func[dscr_hdr->descr_type] != HI_NULL) {
            g_descr_control_func[dscr_hdr->descr_type]((hi_u8 *)h2d_ctrl_descr + sizeof(struct hcc_descr_header));
        }
    } else {
        hcc_write_over_aggr_package();
    }

#ifdef CONFIG_HCC_DEBUG
    hi_diag_log_msg_i2(0, "h2d: next assem num: %d, totol: %d\r\n", g_rx_aggr_info.aggr_rx_num,
        g_rx_aggr_info.rx_aggr_total_len);
#endif
    g_chan_write_flag = CHAN_RX_IDLE;

    if (g_pm_wlan_forceslp_msg_proc != HI_NULL) {
        g_pm_wlan_forceslp_msg_proc();
    }
    return HI_SUCCESS;
}

hi_s32 hcc_read_start_callback(hi_u32 len, hi_u8 *adma_table)
{
    hi_u32 i;
    hi_u32 index = 0;
    hi_u32 total_pad;
    hi_u32 pad_len;
    hi_unref_param(len);

    if (g_tx_aggr_info.aggr_tx_num == 0) {
        return 0;
    }

    if (g_tx_aggr_info.tx_aggr_total_len < HISDIO_BLOCK_SIZE) {
        pad_len = hi_byte_align(g_tx_aggr_info.tx_aggr_total_len, 4); /* 4 bytes align */
    } else {
        pad_len = hi_byte_align(g_tx_aggr_info.tx_aggr_total_len, HISDIO_BLOCK_SIZE);
    }

    if (pad_len >= g_tx_aggr_info.tx_aggr_total_len) {
        total_pad = pad_len - g_tx_aggr_info.tx_aggr_total_len;
    } else {
        total_pad = 0;
    }

    if (g_tx_aggr_info.aggr_tx_num > HISDIO_DEV2HOST_SCATT_MAX) {
#ifdef CONFIG_HCC_DEBUG
        hi_diag_log_msg_i1(0, "unvaild d2h aggr num: %d\r\n", g_tx_aggr_info.aggr_tx_num);
#endif
    }

    for (i = 0; i < g_tx_aggr_info.aggr_tx_num; i++) {
        if (g_tx_aggr_info.hcc_tx_aggr[i].netbuf == HI_NULL) {
            return 0;
        }

        hcc_build_adma_descr_from_netbuf(adma_table, &index,
                                         g_tx_aggr_info.hcc_tx_aggr[i].tx_pkt_bytes,
                                         g_tx_aggr_info.hcc_tx_aggr[i].netbuf);
    }

    /* The padding descr */
    if (total_pad != 0) {
#ifdef CONFIG_HCC_DEBUG
        hi_diag_log_msg_i1(0, "add pad %u descr\r\n", total_pad);
#endif
        (hi_void)hi_sdio_set_pad_admatab(total_pad, adma_table, index++);
    }

    /* IP问题需多加一行无用数据,表示表格终止 */
    (hi_void)hi_sdio_complete_send(adma_table, index);
    return (hi_s32)index;
}

hi_s32 hcc_read_over_callback(hi_void)
{
    hi_sdio_status_info status_info;

    for (hi_u32 i = 0; i < g_tx_aggr_info.aggr_tx_num; i++) {
        oal_mem_dev_netbuf_free(g_tx_aggr_info.hcc_tx_aggr[i].netbuf);
        g_tx_aggr_info.hcc_tx_aggr[i].netbuf = HI_NULL;
    }

    g_tx_aggr_info.tx_aggr_total_len = 0;
    g_tx_aggr_info.aggr_tx_num = 0;

    (hi_void)hi_sdio_get_status(&status_info);
    status_info.tx_status = CHAN_TX_IDLE;
    (hi_void)hi_sdio_set_status(&status_info);

    if (g_pm_wlan_forceslp_msg_proc != HI_NULL) {
        g_pm_wlan_forceslp_msg_proc();
    }
    return HI_SUCCESS;
}


hi_void hcc_slave_tx(oal_dev_netbuf_stru *dev_netbuf, hi_u16 pay_load_len, const hcc_transfer_param *param)
{
    /* build the head */
    hcc_slave_build_header(dev_netbuf, pay_load_len, param);

    /* put the pst_buf enqueue */
    hcc_netbuf_add_to_list_tail(dev_netbuf, &g_hcc_tx_queue);

    /* sched the hcc tx */
    hcc_task_sched();
}

hi_u32 hcc_oam_fill_payload_data(const hi_u8 *data, hi_u16 pay_load_len, oal_dev_netbuf_stru **dev_netbuf)
{
    hi_u8 *cb = HI_NULL;
    oal_dev_netbuf_stru *temp_netbuf;
    temp_netbuf = oal_mem_dev_netbuf_alloc(OAL_NORMAL_NETBUF, (hi_u16)pay_load_len, OAL_NETBUF_PRIORITY_HIGH);
    if (temp_netbuf == HI_NULL) {
        return HI_ERR_FAILURE;
    }
    cb = oal_dev_netbuf_cb(temp_netbuf->us_index);
    if (cb == HI_NULL) {
        oal_mem_dev_netbuf_free(temp_netbuf);
        return HI_ERR_FAILURE;
    }
    memset_s(cb, OAL_TX_CB_LEN, 0, OAL_TX_CB_LEN);
    if (memcpy_s((hi_u8 *)(oal_dev_netbuf_get_payload(temp_netbuf)), pay_load_len,
        (hi_u8 *)data, pay_load_len) != HI_SUCCESS) {
        oal_mem_dev_netbuf_free(temp_netbuf);
        return HI_ERR_FAILURE;
    }
    *dev_netbuf = temp_netbuf;
    return HI_ERR_SUCCESS;
}

hi_u32 hcc_oam_log_tx(const hi_u8 *data, hi_u16 data_len, hi_u16 cmd_id)
{
    oal_dev_netbuf_stru *dev_netbuf = HI_NULL;
    hcc_transfer_param param = { 0 };
    hi_u16 pay_load_len;
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    frw_hcc_extend_hdr_stru *ext_hdr = HI_NULL;
    frw_event_hdr_stru *event_hdr = HI_NULL;
    if (g_sdio_init_ok == HI_FALSE) {
        return HI_ERR_SUCCESS;
    }
    pay_load_len = data_len;
    event_mem = frw_event_alloc(data_len);
    if (oal_unlikely(event_mem == HI_NULL)) {
        return HI_ERR_CODE_PTR_NULL;
    }
    event = (frw_event_stru *)event_mem->puc_data;
    /* 需要把数据拷贝到event_mem中 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HCC, cmd_id, pay_load_len, FRW_EVENT_PIPELINE_STAGE_1, 0);
    if (hcc_oam_fill_payload_data(data, pay_load_len, &dev_netbuf) != HI_ERR_SUCCESS) {
        goto failed_netbuf;
    }
    ext_hdr = (frw_hcc_extend_hdr_stru *)hcc_get_extend_addr(dev_netbuf, HCC_EXTEND_TOTAL_SIZE);
    event_hdr = frw_get_event_hdr(event_mem);
    if (ext_hdr == HI_NULL || event_hdr == HI_NULL) {
        goto failed_netbuf;
    }
    ext_hdr->nest_type = event_hdr->type;
    ext_hdr->nest_sub_type = event_hdr->sub_type;
    ext_hdr->vap_id = event_hdr->vap_id;
    hcc_hdr_param_init(&param, HCC_ACTION_TYPE_OAM, WIFI_CONTROL_TYPE, HCC_EXTEND_TOTAL_SIZE, DATA_HI_QUEUE,
        HCC_FC_NONE);
    hcc_slave_tx(dev_netbuf, pay_load_len, &param);
    frw_event_free(event_mem);
    return HI_ERR_SUCCESS;
failed_netbuf:
    if (dev_netbuf != HI_NULL) {
        oal_mem_dev_netbuf_free(dev_netbuf);
    }
    if (event_mem != HI_NULL) {
        frw_event_free(event_mem);
    }
    return HI_ERR_FAILURE;
}


hi_void *hcc_get_extern_address(const oal_dev_netbuf_stru *dev_netbuf, hi_u32 extend_len)
{
    if (extend_len > (HCC_HDR_TOTAL_LEN - HCC_HDR_LEN)) {
        /* invalid extend len */
#ifdef CONFIG_HCC_DEBUG
        hi_diag_log_msg_i1(0, "invalid extend len:%u\r\n", extend_len);
#endif
        return HI_NULL;
    }

    return (hi_void *)((uintptr_t)oal_dev_netbuf_hcchdr(dev_netbuf) + HCC_HDR_TOTAL_LEN - extend_len);
}

hi_bool hcc_check_header_vaild(const hcc_header_stru *hdr)
{
    if ((hdr->main_type >= HCC_ACTION_TYPE_BUTT) || (hdr->sub_type >= WIFI_SUB_TYPE_BUTT) ||
        ((HCC_HDR_LEN + (hi_u16)(hdr->pad_hdr) + (hi_u16)(hdr->pad_payload)) > HCC_HDR_TOTAL_LEN)) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

hi_bool hcc_exec_action(oal_dev_netbuf_stru *nb_prefetch, const hcc_header_stru *hcc_head,
    oal_dev_netbuf_stru *nb_postdo)
{
    hcc_slave_netbuf slave_netbuf;
    hi_u8 *pre_context = HI_NULL;
    hcc_rx_action *rx_action = &g_hcc_slave_handler.rx_action_info.action[hcc_head->main_type];

    /* prepare */
    if (rx_action->pre_do) {
        slave_netbuf.net_buf = nb_prefetch;
        slave_netbuf.len = (hi_s32)hcc_head->pay_len;
        if (rx_action->pre_do(hcc_head->sub_type, &slave_netbuf, &pre_context) != HI_SUCCESS) {
            printf("rx_action->pre_do break\n");
            return HI_FALSE;
        }
    }
    /* prepare do ok,delist the netbuf. */
    nb_postdo = hcc_netbuf_delist(&g_hcc_rx_queue);
    if (nb_postdo == HI_NULL) {
        printf("dev_netbuf_postdo break\n");
        return HI_FALSE;
    }

    if (rx_action->post_do != HI_NULL) {
        nb_postdo->next = HI_NULL;
        hcc_head = (hcc_header_stru *)oal_dev_netbuf_hcchdr(nb_postdo);
        if (hcc_head == HI_NULL) {
            hi_diag_log_msg_e0(0, "{hcc_slave_rx_queue_sched::pst_hcc_head null.}");
            printf("dev_netbuf_postdo continue\n");
            return HI_TRUE;
        }
        slave_netbuf.net_buf = nb_postdo;
        slave_netbuf.len = (hi_s32)hcc_head->pay_len;
        (hi_void)rx_action->post_do(hcc_head->sub_type, &slave_netbuf, pre_context);
    } else {
        /* Debug,unregistered mtype */
        hi_diag_log_msg_e0(0, "rx_action->post_do is null\r\n");
        oal_mem_dev_netbuf_free(nb_postdo);
        printf("dev_netbuf_postdo else\n");
    }
    return HI_TRUE;
}

hi_void hcc_slave_rx_queue_sched(hi_void)
{
    oal_dev_netbuf_stru *dev_netbuf_prefetch = HI_NULL;
    oal_dev_netbuf_stru *dev_netbuf_postdo = HI_NULL;

    hcc_delay_bug_on_detect();

    for (hi_u32 i = 0; i < FRW_EVENT_HCC_BURST_COUNT; ++i) {
        if (g_hcc_rx_queue.num == 0) {
            break;
        }

        /* action prepare, if failed keep the netbuf in the list */
        dev_netbuf_prefetch = (oal_dev_netbuf_stru *)oal_netbuf_head_next(&g_hcc_rx_queue);
        hcc_header_stru *hcc_head = (hcc_header_stru *)oal_dev_netbuf_hcchdr(dev_netbuf_prefetch);
        if (hcc_head == HI_NULL) {
            hi_diag_log_msg_e0(0, "{hcc_slave_rx_queue_sched::pst_hcc_head null.}");
            continue;
        }
        if (hcc_check_header_vaild(hcc_head) != HI_TRUE) {
            hi_diag_log_msg_e0(0, "hcc rx header valid\r\n");
            dev_netbuf_postdo = hcc_netbuf_delist(&g_hcc_rx_queue);
            oal_mem_dev_netbuf_free(dev_netbuf_postdo);
            continue;
        }

        if (!hcc_exec_action(dev_netbuf_prefetch, hcc_head, dev_netbuf_postdo)) {
            break;
        }
    }
}

hi_void hcc_slave_tx_queue_sched(hi_void)
{
    hi_u32 i;
    hi_u32 total_len = 0;
    hi_sdio_status_info status_info;
    hi_sdio_extendfunc *hcc_extfunc = hi_sdio_get_extend_info();

    hi_u32 status = hi_int_lock();
    (hi_void)hi_sdio_get_status(&status_info);
    if ((status_info.tx_status == CHAN_TX_BUSY) || (status_info.allow_sleep == HI_TRUE) ||
        (status_info.work_status != SDIO_CHAN_WORK) || (g_hcc_tx_queue.num == 0)) {
        hi_int_restore(status);
        return;
    }

    status_info.tx_status = CHAN_TX_BUSY;
    (hi_void)hi_sdio_set_status(&status_info);

    memset_s(hcc_extfunc, sizeof(hi_sdio_extendfunc), 0, sizeof(hi_sdio_extendfunc));

    /* 是否有高优先级buffer credit值要更新 */
    if (g_flowctrl_flashed == HI_TRUE) {
        if (!hi_sdio_is_sending_msg(D2H_MSG_CREDIT_UPDATE) && !hi_sdio_is_pending_msg(D2H_MSG_CREDIT_UPDATE)) {
            /* 更新寄存器 */
            hcc_extfunc->credit_info = g_flow_ctrl_reg_info;
            hcc_extfunc->credit_isvalid = 1;
            (hi_void)hi_sdio_write_extinfo(hcc_extfunc);
            g_flowctrl_flashed = HI_FALSE;
        }
    }

    memset_s(hcc_extfunc, sizeof(hi_sdio_extendfunc), 0, sizeof(hi_sdio_extendfunc));

    for (i = 0; i < g_d2h_hcc_assemble_count; ++i) {
        oal_dev_netbuf_stru *dev_netbuf = hcc_netbuf_delist(&g_hcc_tx_queue);
        if (dev_netbuf == HI_NULL) {
            break;
        }

        /* set the tx aggr information */
        hi_u32 send_len = hcc_slave_get_netbuf_align_len(dev_netbuf, HISDIO_D2H_SCATT_BUFFLEN_ALIGN);
        total_len += send_len;
        g_tx_aggr_info.hcc_tx_aggr[i].netbuf = dev_netbuf;
        g_tx_aggr_info.hcc_tx_aggr[i].tx_pkt_bytes = send_len;
        hcc_extfunc->comm_reg[i] = (send_len >> HISDIO_D2H_SCATT_BUFFLEN_ALIGN_BITS) & 0xFF;
    }

    /* The sdio register must access by 4 bytes */
    hcc_extfunc->valid_commreg_cnt = (hi_s32)(i & (~3)); /* 3 is used for 4 btyes down align. */
    hcc_extfunc->commreg_isvalid = 1;
    /* tx_aggr_total_len 32 B对齐，未作512对齐 */
    g_tx_aggr_info.tx_aggr_total_len = total_len;
    g_tx_aggr_info.aggr_tx_num = i;

    if (total_len < HISDIO_BLOCK_SIZE) {
        hcc_extfunc->xfer_count = hi_byte_align(total_len, 4); /* 4 bytes align */
    } else {
        hcc_extfunc->xfer_count = hi_byte_align(total_len, HISDIO_BLOCK_SIZE); /* 4 bytes align */
    }
    /* write ext info */
    (hi_void)hi_sdio_write_extinfo(hcc_extfunc);

    hi_sdio_send_data(hcc_extfunc->xfer_count);

    hi_int_restore(status);
}

/* ****************************************************************************
 功能描述  : 收到host的sleep request消息处理，应答允许或forbid sleep应答

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hcc_sleep_request_msg_proc(hi_void)
{
    hi_u32 msg;
    hi_sdio_status_info status_info;

    (hi_void)hi_sdio_get_status(&status_info);
    if ((status_info.tx_status != CHAN_TX_BUSY) &&
        (g_hcc_tx_queue.num == 0) &&
        (g_chan_write_flag == CHAN_RX_IDLE)) {
        msg = D2H_MSG_ALLOW_SLEEP;
        status_info.sleep_status = SLEEP_ALLOW_SND;
        status_info.allow_sleep = HI_TRUE;
    } else {
        msg = D2H_MSG_DISALLOW_SLEEP;
        status_info.sleep_status = SLEEP_DISALLOW_SND;
    }

    hi_sdio_send_sync_msg(msg);
    (hi_void)hi_sdio_set_status(&status_info);
}

hi_void hcc_msg_process(hi_u32 msg)
{
    if (msg & (1 << H2D_MSG_PM_WLAN_OFF)) {
        printf("reboot\n");
        hi_diag_log_msg_i0(0, "recieve msg H2D_MSG_PM_WLAN_OFF, need reboot\n");
        hi_reboot();
    }
}

hi_void hcc_msg_callback(hi_u32 msg)
{
    static hi_u8 msg_callback_flag = 0;
    if (g_process_msg_callback != HI_NULL) {
        g_process_msg_callback(msg);
    } else {
        if (msg_callback_flag == 0) { /* print related log every 256 messages. */
            hi_diag_log_msg_e0(0, "call hcc_register_msg_callback to register callback firstly\r\n");
        }
        ++msg_callback_flag;
    }
}

hi_void hcc_read_err_callback(hi_void)
{
    hi_diag_log_msg_i0(0, "hcc_read_err_callback occurs.\r\n");
}

hi_void hcc_soft_rst_callback(hi_void)
{
    sdio_soft_reset_valid(); /* must set before reinit. */
    (hi_void)hi_sdio_reinit();
}

hi_void hcc_slave_clean(hi_void)
{
    if (g_reserve_netbuff != HI_NULL) {
        oal_mem_dev_netbuf_free(g_reserve_netbuff);
        g_reserve_netbuff = HI_NULL;
    }
}

/* ****************************************************************************
 功能描述  : 注册消息处理中断函数

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hcc_register_msg_callback(hcc_msg_cb msg_callback)
{
    g_process_msg_callback = msg_callback;
}

/* ****************************************************************************
 功能描述  : HCC slave初始化

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hcc_slave_init(hi_void)
{
    hi_sdio_intcallback callback;
    g_sdio_init_ok = HI_FALSE;
    /* 申请一个保留netbuff用来申请不到内存时使用 */
    g_reserve_netbuff = oal_mem_dev_netbuf_alloc(OAL_NORMAL_NETBUF, HCC_LONG_PACKAGE_SIZE, OAL_NETBUF_PRIORITY_HIGH);
    if (g_reserve_netbuff == HI_NULL) {
        return HI_FAIL;
    }
    /* 逻辑需要保证宏HCC_LONG_PACKAGE_SIZE是 4 字节整数倍 */
    if ((HCC_LONG_PACKAGE_SIZE % 4) != 0) {
        hi_diag_log_msg_e0(0, "must 4 bytes algin\r\n");
        return HI_FAIL;
    }

    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s((hi_void *)&g_tx_aggr_info, sizeof(hcc_slave_tx_info), 0, sizeof(hcc_slave_tx_info));
    memset_s((hi_void *)&g_rx_aggr_info, sizeof(hcc_slave_rx_info), 0, sizeof(hcc_slave_rx_info));
    memset_s((hi_void *)g_h2d_assem_descr, HISDIO_HOST2DEV_SCATT_SIZE, 0, HISDIO_HOST2DEV_SCATT_SIZE);
    memset_s((hi_void *)&g_hcc_slave_handler, sizeof(struct hcc_handler), 0, sizeof(struct hcc_handler));
    memset_s((hi_void *)g_descr_control_func, sizeof(g_descr_control_func), 0, sizeof(g_descr_control_func));

    g_descr_control_func[HCC_DESCR_ASSEM_RESET] = hcc_rx_assem_info_reset;
    g_descr_control_func[HCC_NETBUF_QUEUE_SWITCH] = hcc_rx_netbuf_queue_pri_switch;
    g_current_queue_type = HCC_NETBUF_NORMAL_QUEUE;

    oal_dev_netbuf_list_head_init(&g_hcc_tx_queue);
    oal_dev_netbuf_list_head_init(&g_hcc_rx_queue);

    callback.rdover_callback     = hcc_read_over_callback;
    callback.rdstart_callback    = hcc_read_start_callback;
    callback.wrstart_callback    = hcc_write_start_callback;
    callback.wrover_callback     = hcc_write_over_callback;
    callback.processmsg_callback = hcc_msg_callback;
    callback.rderr_callback      = hcc_read_err_callback;
    callback.soft_rst_callback   = hcc_soft_rst_callback;
    (hi_void)hi_sdio_register_callback(&callback);
    (hi_void)hi_sdio_memory_init();

    hcc_register_msg_callback(hcc_msg_process);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : HCC slave重新初始化

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hcc_slave_reinit(hi_void)
{
    hi_sdio_intcallback callback;
    hi_u32 ret;

    /* 申请一个保留netbuff用来申请不到内存时使用 */
    g_reserve_netbuff = oal_mem_dev_netbuf_alloc(OAL_NORMAL_NETBUF, HCC_LONG_PACKAGE_SIZE, OAL_NETBUF_PRIORITY_HIGH);
    if (g_reserve_netbuff == HI_NULL) {
        return HI_FAIL;
    }
    // 逻辑需要保证宏HCC_LONG_PACKAGE_SIZE是 4 字节整数倍,lin_t e506/e774告警屏蔽
    if ((HCC_LONG_PACKAGE_SIZE % 4) != 0) {
        hi_diag_log_msg_e0(0, "must 4 bytes algin\r\n");
        return HI_FAIL;
    }

    callback.rdover_callback        = hcc_read_over_callback;
    callback.rdstart_callback       = hcc_read_start_callback;
    callback.wrstart_callback       = hcc_write_start_callback;
    callback.wrover_callback        = hcc_write_over_callback;
    callback.processmsg_callback    = hcc_msg_callback;
    callback.rderr_callback         = hcc_read_err_callback;
    callback.soft_rst_callback      = hcc_soft_rst_callback;

    ret = hi_sdio_register_callback(&callback);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    ret = hi_sdio_reinit();
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : HCC接收处理注册函数

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hcc_rx_register(struct hcc_handler *hcc, hi_u8 mtype, hcc_rx_post_do post_do, hcc_rx_pre_do pre_do)
{
    hcc_rx_action *rx_action = HI_NULL;

    if ((hcc == HI_NULL) || (post_do == HI_NULL) || (mtype >= HCC_ACTION_TYPE_BUTT)) {
        return HI_FAIL;
    }

    rx_action = &hcc->rx_action_info.action[mtype];

    if (rx_action->post_do != HI_NULL) {
        return HI_FAIL;
    }

    rx_action->post_do = post_do;
    rx_action->pre_do = pre_do;

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
