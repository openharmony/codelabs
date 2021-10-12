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
#include "hcc_hmac_if.h"
#include "hmac_ext_if.h"
#include "hcc_comm.h"
#include "hcc_host.h"
#include "hmac_tx_data.h"
#include "oam_ext_if.h"
#include "oal_util.h"
#ifndef _PRE_FEATURE_HCC_TASK
#include "flow_control.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#define WIDTH_BIT32 2
static hcc_hmac_rx_event_handle g_s_handle = { NULL, NULL };

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
#ifdef _PRE_LWIP_ZERO_COPY
static inline hi_bool hcc_is_data_frame(const frw_event_mem_stru *event_mem)
{
    frw_event_hdr_stru *event_hdr = HI_NULL;

    event_hdr = frw_get_event_hdr(event_mem);
    return (event_hdr->type == FRW_EVENT_TYPE_WLAN_DRX);
}
#endif

#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
static hi_u32 hcc_hmac_rx_event_register(const hcc_hmac_rx_event_handle *handle)
{
    if (handle == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    if (memcpy_s(&g_s_handle, sizeof(hcc_hmac_rx_event_handle), handle, sizeof(hcc_hmac_rx_event_handle)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hcc_hmac_rx_event_register:: handle memcpy_s fail.");
        return HI_FAIL;
    }
    return HI_SUCCESS;
}
#endif

static hi_void hcc_hmac_rx_event_unregister(hi_void)
{
    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(&g_s_handle, sizeof(hcc_hmac_rx_event_handle), 0, sizeof(hcc_hmac_rx_event_handle));
}

/* ****************************************************************************
 功能描述  : HCC到HMAC的控制数据分发

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hcc_to_hmac_control_event_dispatch(frw_event_mem_stru *event_mem)
{
    if (g_s_handle.control == NULL) {
        oam_warning_log0(0, 0, "hcc_to_hmac_control_event_dispatch: control is NULL");
        return HI_FAIL;
    }

    return g_s_handle.control(event_mem);
}

/* init extend head for sdio host tx */
static hi_void hcc_hmac_tx_adapt_extend_hdr_init(const frw_event_mem_stru *hcc_event_mem,
    const oal_netbuf_stru *netbuf, hi_u8 config_frame)
{
    frw_hcc_extend_hdr_stru *ext_hdr = HI_NULL;
    frw_event_hdr_stru *event_hdr = HI_NULL;

    event_hdr = frw_get_event_hdr(hcc_event_mem);

    ext_hdr = (frw_hcc_extend_hdr_stru *)oal_netbuf_data(netbuf);
    ext_hdr->nest_type = event_hdr->type;
    ext_hdr->nest_sub_type = event_hdr->sub_type;
    ext_hdr->vap_id = event_hdr->vap_id;
    ext_hdr->config_frame = config_frame;
}

/* hcc sdio host init hcc hdr */
hi_u32 hcc_hmac_tx_hcc_hdr_init(oal_netbuf_stru *netbuf, const hcc_transfer_param *param)
{
    hcc_header_stru *hcc_hdr = HI_NULL;
    oal_netbuf_stru *netbuf_temp = netbuf;
    hi_u32 pad_hdr;
    hi_u32 headroom;
    hi_u32 payload_len;

    /* calculate the pad length to ensure the hcc_total_len is 64Bytes */
    pad_hdr = HCC_HDR_RESERVED_MAX_LEN - param->extend_len;
    headroom = pad_hdr + HCC_HDR_LEN;
    payload_len = oal_netbuf_len(netbuf) - param->extend_len;

    if (oal_netbuf_headroom(netbuf_temp) < headroom) {
        oam_error_log2(0, 0, "hcc_hmac_tx_hcc_hdr_init:: headroom[%d] is not enough, need[%d]",
            oal_netbuf_headroom(netbuf_temp), headroom);
        return HI_FAIL;
    }
    hcc_hdr = (hcc_header_stru *)oal_netbuf_push(netbuf_temp, headroom);
    if (hcc_hdr == HI_NULL) {
        return HI_FAIL;
    }

    hcc_hdr->main_type = param->main_type;
    hcc_hdr->sub_type = param->sub_type;
    hcc_hdr->pay_len = payload_len;
    hcc_hdr->seq = g_sdio_txpkt_index++;
    hcc_hdr->pad_hdr = HCC_HDR_RESERVED_MAX_LEN - param->extend_len;
    hcc_hdr->pad_payload = 0; /* Device alloc netbuf's payload all 4B aligned! */

    return HI_SUCCESS;
}

static hi_void hcc_tx_netbuf_destroy(hcc_handler_stru *hcc_handler)
{
    if (hcc_handler != HI_NULL) {
        oal_wake_unlock(&hcc_handler->tx_wake_lock);
    }
}

hi_void hcc_sort_key_frame(hcc_trans_queue_stru *hcc_queue, oal_netbuf_stru *netbuf)
{
    oal_netbuf_head_stru *head;
    hi_u32 flags;
    oal_netbuf_stru *netbuf_tmp = HI_NULL;
    oal_netbuf_stru *netbuf_pos = HI_NULL;
    hi_u8 is_insert = HI_FALSE;
    head = &hcc_queue->data_queue;

    oal_spin_lock_irq_save((oal_spin_lock_stru *)&head->lock, (unsigned long *)&flags);
    oal_skb_queue_walk_safe(head, netbuf_pos, netbuf_tmp)
    {
        dmac_tx_ctl_stru *dmac_tx_ctrl = HI_NULL;
        hi_u8 *hcc_hdr = (hi_u8 *)oal_netbuf_data(netbuf_pos);
        dmac_tx_ctrl = (dmac_tx_ctl_stru *)(hcc_hdr + HCC_HDR_LEN + sizeof(frw_hcc_extend_hdr_stru));
        if (dmac_tx_ctrl != HI_NULL && dmac_tx_ctrl->is_vipframe != HI_TRUE &&
            dmac_tx_ctrl->high_prio_sch != HI_TRUE) {
            oal_netbuf_append(head, netbuf, netbuf_pos);
            is_insert = HI_TRUE;
            break;
        }
    }
    oal_spin_unlock_irq_restore((oal_spin_lock_stru *)&head->lock, (unsigned long *)&flags);

    if (!is_insert) {
        oal_netbuf_list_tail(head, netbuf);
    }
}

/* hcc sdio host tx */
hi_u32 hcc_host_tx(hcc_handler_stru *hcc_handler, oal_netbuf_stru *netbuf, const hcc_transfer_param *param)
{
    hi_u32 err_code;
    hcc_tx_cb_stru *hcc_cb = HI_NULL;
    hi_u8 is_vipframe = HI_FALSE;
#ifdef _PRE_FEATURE_HCC_TASK
    hcc_trans_queue_stru *hcc_queue = HI_NULL;
    hcc_queue = &hcc_handler->hcc_transer_info.hcc_queues[HCC_TX].queues[param->queue_id];
#else
    struct FlowControlModule *fcm = HI_NULL;
    hcc_convert_queue_para para;
    (void)hcc_handler;
#endif
    /* 1. build hcc header */
    err_code = hcc_hmac_tx_hcc_hdr_init(netbuf, param);
    if (err_code != HI_SUCCESS) {
        return HI_FAIL;
    }

    /* 2. init hcc cb ctrl */
    hcc_cb = (hcc_tx_cb_stru *)oal_netbuf_cb(netbuf);
    hcc_cb->destroy = hcc_tx_netbuf_destroy;
    hcc_cb->magic = HCC_TX_WAKELOCK_MAGIC;
    if (param->queue_id == DATA_LO_QUEUE) {
        dmac_tx_ctl_stru *dmac_tx_ctrl = HI_NULL;
        hi_u8 *hcc_hdr = (hi_u8 *)oal_netbuf_data(netbuf);
        dmac_tx_ctrl = (dmac_tx_ctl_stru *)(hcc_hdr + HCC_HDR_LEN + sizeof(frw_hcc_extend_hdr_stru));
        if (dmac_tx_ctrl != HI_NULL &&
            dmac_tx_ctrl->is_vipframe != HI_TRUE && dmac_tx_ctrl->high_prio_sch != HI_TRUE &&
            hcc_list_overflow()) {
            /* 非关键帧 */
            oal_netbuf_free(netbuf);
            return HI_SUCCESS;
        }

        if (dmac_tx_ctrl != HI_NULL &&
            (dmac_tx_ctrl->is_vipframe == HI_TRUE || dmac_tx_ctrl->high_prio_sch == HI_TRUE)) {
            is_vipframe = HI_TRUE;
        }

        if (is_vipframe && hcc_discard_key_frame()) {
            oal_netbuf_free(netbuf);
            return HI_SUCCESS;
        }
    }

#ifdef _PRE_FEATURE_HCC_TASK
    if (is_vipframe) {
        hcc_sort_key_frame(hcc_queue, netbuf);
    } else {
        /* 3. netbuf enqueue */
        oal_netbuf_list_tail(&hcc_queue->data_queue, netbuf);
    }

    /* 4. sched hcc tx */
    hcc_sched_transfer(hcc_handler);
#else
    fcm = GetFlowControlModule();
    if (fcm != NULL && fcm->op != NULL && fcm->op->getTxQueueId != NULL) {
        para.queueType = param->queue_id;
        para.isVip = is_vipframe;
        FlowControlQueueID id = fcm->op->getTxQueueId((void *)(&para));
        fcm->interface->sendBuffToFCM(fcm, netbuf, id, FLOW_TX);
        fcm->interface->schedFCM(fcm, FLOW_TX);
    }
#endif
    return HI_SUCCESS;
}

#define HCC_NETBUF_RESERVED_ROOM_SIZE (HCC_HDR_TOTAL_LEN + HISDIO_H2D_SCATT_BUFFLEN_ALIGN)
/* ****************************************************************************
 功能描述  : HMAC通过HCC SIDO发送到DMAC的控制数据事件
 修改历史      :
  1.日    期   : 2019-12-21
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hcc_tx_netbuf_normal(const frw_event_mem_stru *event_mem, oal_netbuf_stru *netbuf, hi_u32 hdr_len)
{
    hcc_transfer_param param = { 0 };
    hi_u32 err_code;
    hi_u32 queue_id;
    hi_u32 fc_type;
    hi_u8 config_frame = 1;

    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    if (event == HI_NULL) {
        return HI_FAIL;
    }
    hi_u32 sub_type = (event->event_hdr.type == FRW_EVENT_TYPE_HOST_DRX) ? WIFI_DATA_TYPE : WIFI_CONTROL_TYPE;

    if (event->event_hdr.type >= FRW_EVENT_TYPE_BUTT) {
        oam_error_log1(0, 0, "hcc_tx_netbuf_normal:: invalid event type[%d]!", event->event_hdr.type);
        return HI_FAIL;
    }
    queue_id = g_hcc_sched_stat[event->event_hdr.type];
    fc_type = g_hcc_flowctrl_stat[event->event_hdr.type];

    /* HMAC_TO_DMAC_SYN_CFG事件的SET_WPS_P2P_IE在sdio发送时申请管理帧 */
    if (event->event_hdr.type == FRW_EVENT_TYPE_HOST_CRX && event->event_hdr.sub_type == HMAC_TO_DMAC_SYN_CFG) {
        hmac_to_dmac_cfg_msg_stru *syn_msg = (hmac_to_dmac_cfg_msg_stru *)event->auc_event_data;
        if (syn_msg != HI_NULL && syn_msg->syn_id == WLAN_CFGID_SET_WPS_P2P_IE) {
            config_frame = 0;
        }
    }

    /* FRW_EVENT_TYPE_WLAN_CTX事件在sdio发送时申请管理帧 */
    if (event->event_hdr.type == FRW_EVENT_TYPE_WLAN_CTX) {
        config_frame = 0;
    }

    /* 1. 初始化HCC头 */
    hcc_hdr_param_init(&param, HCC_ACTION_TYPE_WIFI, sub_type,
                       (hdr_len + (hi_u32)sizeof(frw_hcc_extend_hdr_stru)), fc_type, queue_id);
    if (oal_netbuf_headroom(netbuf) < sizeof(frw_hcc_extend_hdr_stru)) {
        oam_error_log2(0, 0, "hcc_hmac_tx_hcc_hdr_init:: headroom[%d] is not enough, need[%d]",
            oal_netbuf_headroom(netbuf), sizeof(frw_hcc_extend_hdr_stru));
        return HI_FAIL;
    }

    /* 2. add extend area (extend head include HCC, MAC_HDR) */
    oal_netbuf_push(netbuf, sizeof(frw_hcc_extend_hdr_stru));
    /* 3. 初始化extent hdr */
    hcc_hmac_tx_adapt_extend_hdr_init(event_mem, netbuf, config_frame);

    err_code = hcc_host_tx(hcc_host_get_handler(), netbuf, &param);
    if (err_code != HI_SUCCESS) {
        oam_error_log1(0, 0, "hcc_tx_netbuf_normal:: hcc_host_tx fail[%d]", err_code);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* HCC 将控制事件中的payload 转换为netbuf */
hi_u32 hmac_hcc_tx_event_buf_to_netbuf(const frw_event_mem_stru *event_mem, hi_u16 payload_len)
{
    hi_u8 *event_payload = HI_NULL;
    oal_netbuf_stru *netbuf = HI_NULL;

    event_payload = frw_get_event_payload(event_mem);
    if (event_payload == HI_NULL) {
        oam_error_log0(0, 0, "hmac_hcc_tx_netbuf_adapt:: event_payload is NULL");
        return HI_FAIL;
    }

    netbuf = oal_netbuf_alloc(payload_len + HCC_NETBUF_RESERVED_ROOM_SIZE, 0, 4); /* align 4 */
    if (netbuf == HI_NULL) {
        oam_error_log0(0, 0, "hmac_hcc_tx_event_buf_to_netbuf:: netbuf_alloc failed!");
        return HI_FAIL;
    }

    oal_netbuf_put(netbuf, payload_len);
    if (memcpy_s(oal_netbuf_data(netbuf), payload_len, event_payload, payload_len) != EOK) {
        oam_error_log0(0, 0, "hmac_hcc_tx_event_buf_to_netbuf:: memcpy_s failed!");
        oal_netbuf_free(netbuf);
        return HI_FAIL;
    }

    return hcc_tx_netbuf_normal(event_mem, netbuf, 0);
}

/* ****************************************************************************
 功能描述  : HMAC通过HCC发送到DMAC的控制数据事件

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hcc_hmac_tx_control_event(frw_event_mem_stru *event_mem, hi_u16 payload_len)
{
    if (event_mem == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }
    return hmac_hcc_tx_event_buf_to_netbuf(event_mem, payload_len);
}

/* hmac cb convert to dmac cb */
hi_void hcc_to_dmac_tx_ctr_convert(dmac_tx_ctl_stru *dst_cb, const hmac_tx_ctl_stru *src_cb)
{
    dst_cb->alg_pktno           = (hi_u8)src_cb->alg_pktno;
    dst_cb->en_event_type       = src_cb->event_type;
    dst_cb->frame_header_length = src_cb->frame_header_length;
    dst_cb->is_needretry        = src_cb->is_needretry;
    dst_cb->is_vipframe         = src_cb->is_vipframe;
    dst_cb->mpdu_num            = src_cb->mpdu_num;
    dst_cb->netbuf_num          = src_cb->netbuf_num;
    dst_cb->retried_num         = src_cb->retried_num;
    dst_cb->tx_user_idx         = (hi_u8)src_cb->us_tx_user_idx;
    dst_cb->tx_vap_index        = src_cb->tx_vap_index;
    dst_cb->ismcast             = src_cb->ismcast;
    dst_cb->is_eapol            = src_cb->is_eapol;
    dst_cb->is_first_msdu       = src_cb->is_first_msdu;
    dst_cb->is_get_from_ps_queue = src_cb->is_get_from_ps_queue;
    dst_cb->is_probe_data       = src_cb->is_probe_data;
    dst_cb->us_mpdu_bytes       = src_cb->us_mpdu_bytes;
    dst_cb->mgmt_frame_id       = src_cb->mgmt_frame_id;
    dst_cb->is_eapol_key_ptk    = src_cb->is_eapol_key_ptk;
    dst_cb->need_rsp            = src_cb->need_rsp;
    dst_cb->ac                  = src_cb->ac;
    dst_cb->is_amsdu            = src_cb->is_amsdu;
    dst_cb->ack_policy          = src_cb->ack_policy;
    dst_cb->tid                 = (src_cb->tid & 0x0F);
    dst_cb->high_prio_sch       = src_cb->high_prio_sch;
    /* 解决STA user idx等于0的问题，此处用bit_reserved4作为user idx的备份 */
    dst_cb->tx_user_idx_bak     = (hi_u8)src_cb->us_tx_user_idx;
}

/* fill cb for hcc_hdr */
hi_void hcc_adjust_netbuf_data(oal_netbuf_stru *netbuf)
{
    dmac_tx_ctl_stru dmac_tx_ctrl;
    hmac_tx_ctl_stru *hmac_tx_ctrl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
    hi_u8 *netbuf_hdr = (hi_u8 *)oal_netbuf_data(netbuf);

    if (memset_s(&dmac_tx_ctrl, sizeof(dmac_tx_ctl_stru), 0, sizeof(dmac_tx_ctl_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hcc_adjust_netbuf_data:: memset_s is error.}");
        return;
    }

    hcc_to_dmac_tx_ctr_convert(&dmac_tx_ctrl, hmac_tx_ctrl);

    if (memcpy_s(netbuf_hdr, sizeof(dmac_tx_ctl_stru), (hi_u8 *)&dmac_tx_ctrl, sizeof(dmac_tx_ctl_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hcc_adjust_netbuf_data:: memcpy_s is error.}");
        return;
    }

    if (memmove_s(netbuf_hdr + HI_MAX_DEV_CB_LEN, hmac_tx_ctrl->frame_header_length, hmac_tx_ctrl->frame_header,
        hmac_tx_ctrl->frame_header_length) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hcc_adjust_netbuf_data:: memmove_s is error.}");
        return;
    }

    if (hmac_tx_ctrl->mac_head_type == 0) {
        oal_free(hmac_tx_ctrl->frame_header);
    }
}

/* calculate headroom add_length for hcc hdr */
hi_u32 hcc_check_headroom_add_length(const hmac_tx_ctl_stru *tx_ctrl)
{
    hi_u32 headroom_add;

    if (tx_ctrl->mac_head_type == 1) {
        /* case 1: mac head is maintence in netbuff */
        headroom_add = HI_MAX_DEV_CB_LEN + WLAN_MAX_MAC_HDR_LEN - tx_ctrl->frame_header_length;
    } else {
        /* case 2: mac head not maintence in netbuff */
        headroom_add = HI_MAX_DEV_CB_LEN + WLAN_MAX_MAC_HDR_LEN;
    }

    return headroom_add;
}

hi_u32 hcc_host_tx_data_adapt(const frw_event_mem_stru *event_mem, oal_netbuf_stru *netbuf)
{
    hmac_tx_ctl_stru                 *tx_ctrl = HI_NULL;
    hi_u32                           headroom_add;
    hi_s32                           err_code;
    hi_u32                           netbuf_old_addr;
    hi_u32                           netbuf_new_addr;
    hi_u32                           addr_offset;

    tx_ctrl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
    if (OAL_WARN_ON(tx_ctrl->use_4_addr)) {
        oam_error_log0(0, OAM_SF_ANY, "{hcc_host_tx_data_adapt:: use 4 address.}");
        return HI_FAIL;
    }

    headroom_add = hcc_check_headroom_add_length(tx_ctrl);
    if (headroom_add > oal_netbuf_headroom(netbuf)) {
        err_code = oal_netbuf_expand_head(netbuf, (hi_s32)headroom_add - (hi_s32)oal_netbuf_headroom(netbuf),
                                          0, GFP_ATOMIC);
        if (OAL_WARN_ON(err_code != HI_SUCCESS)) {
            oam_error_log0(0, OAM_SF_ANY, "{hcc_host_tx_data_adapt:: alloc headroom failed.}");
            return HI_ERR_CODE_ALLOC_MEM_FAIL;
        }

        if (tx_ctrl->mac_head_type == 1) {
            tx_ctrl->frame_header = (mac_ieee80211_frame_stru *)oal_netbuf_data(netbuf);
        }
    }

    /* 修改netbuff的data指针和len */
    oal_netbuf_push(netbuf, headroom_add);
    hcc_adjust_netbuf_data(netbuf);

    /* 使netbuf四字节对齐 */
    netbuf_old_addr = (uintptr_t)(oal_netbuf_data(netbuf) + HI_MAX_DEV_CB_LEN + WLAN_MAX_MAC_HDR_LEN);
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    netbuf_new_addr = oal_round_down(netbuf_old_addr, 32); /* boundary 32 */
#else
    netbuf_new_addr = oal_round_down(netbuf_old_addr, 4);  /* boundary 4 */
#endif
    addr_offset = netbuf_old_addr - netbuf_new_addr;

    /* 未对齐时在host侧做数据搬移，此处牺牲host，解放device */
    if (addr_offset) {
        if (addr_offset < oal_netbuf_headroom(netbuf)) {
            if (memmove_s((hi_u8 *)oal_netbuf_data(netbuf) - addr_offset, oal_netbuf_len(netbuf),
                (hi_u8 *)oal_netbuf_data(netbuf), oal_netbuf_len(netbuf)) != EOK) {
                return HI_FAIL;
            }
            oal_netbuf_push(netbuf, addr_offset);
            oal_netbuf_trim(netbuf, addr_offset);
        }
    }

    /* netbuf不管成功与否都由发送函数释放 */
    return hcc_tx_netbuf_normal(event_mem, netbuf, HI_MAX_DEV_CB_LEN + WLAN_MAX_MAC_HDR_LEN);
}

/* SDIO 传输帧类型适配预处理 */
hi_u32 hcc_host_tx_data_adapt_pre_do(frw_event_mem_stru *event_mem)
{
    oal_netbuf_stru                 *netbuf_head = HI_NULL;
    oal_netbuf_stru                 *current_netbuf = HI_NULL;
    oal_netbuf_stru                 *netbuf_original = HI_NULL;
    dmac_tx_event_stru              *event_payload = HI_NULL;
    hi_u32                           err_code;

    /* 取业务事件信息 */
    event_payload = (dmac_tx_event_stru *)frw_get_event_payload(event_mem);
    netbuf_head = event_payload->netbuf;
    netbuf_original = netbuf_head;

    while (netbuf_head != HI_NULL) {
        /* 必须在netbuf抛出之前指向下一个netbuf，防止frw_event_dispatch_event 中重置 netbuf->next */
        current_netbuf = netbuf_head;
        netbuf_head = oal_netbuf_next(netbuf_head);

        err_code = hcc_host_tx_data_adapt(event_mem, current_netbuf);
        if (err_code != HI_SUCCESS) {
            oam_warning_log1(0, 0, "hcc_host_tx_data_adapt_pre_do:: tx_data_adapt failed[%d]", err_code);
            if (netbuf_original == current_netbuf) {
                set_oal_netbuf_next(netbuf_original, HI_NULL);
                hmac_free_netbuf_list(netbuf_head);
                return err_code;
            } else {
                hmac_free_netbuf_list(current_netbuf);
                return HI_SUCCESS;
            }
        }
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : HMAC通过HCC发送到DMAC的用户数据事件
 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* hcc_hmac_tx_data_event->hcc_to_dmac_data_event_dispatch->hcc_to_dmac_data_event_post
   ->frw_event_dispatch_event->frw_event_post_event,452行进行了修改，lin_t e818告警屏蔽 */
hi_u32 hcc_hmac_tx_data_event(frw_event_mem_stru *event_mem, oal_netbuf_stru *netbuf, hi_bool mgmt)
{
    hi_unref_param(netbuf);
    hi_unref_param(mgmt);
    if (event_mem == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    return hcc_host_tx_data_adapt_pre_do(event_mem);
}

/* ****************************************************************************
 功能描述  : HCC-HMAC初始化

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hcc_hmac_init(struct BusDev *bus)
{
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    hcc_hmac_rx_event_handle handle;

    handle.control = hmac_from_dmac_rx_control_handle;
    handle.data = hmac_from_dmac_rx_data_handle;

    return hcc_hmac_rx_event_register(&handle);
#else
    hi_u32 err_code = hcc_host_init(bus);
    if (err_code != HI_SUCCESS) {
        oam_error_log1(0, 0, "hcc_host_init fail![%d]", err_code);
        return err_code;
    }

    hi_s32 adapt_err_code = hcc_hmac_adapt_init();
    if (adapt_err_code != HI_SUCCESS) {
        oam_error_log1(0, 0, "hcc_hmac_adapt_init fail![%d]", adapt_err_code);
        return adapt_err_code;
    }

    printk("hcc_hmac_init SUCCESSFULLY\r\n");
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : HCC-HMAC卸载

 修改历史      :
  1.日    期   : 2019-05-30
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hcc_hmac_exit(hi_void)
{
    hcc_hmac_rx_event_unregister();
    hcc_host_exit(hcc_host_get_handler());
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
