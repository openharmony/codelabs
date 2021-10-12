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
#include "frw_timer.h"
#include "mac_vap.h"
#include "hmac_blockack.h"
#include "hmac_main.h"
#include "hmac_rx_data.h"
#include "hmac_mgmt_bss_comm.h"
#include "hmac_user.h"
#include "mac_pm_driver.h"
#include "hmac_tx_data.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 判断seq1是否小于seq2
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u8 hmac_ba_seqno_lt(hi_u16 us_seq1, hi_u16 us_seq2)
{
    if (((us_seq1 < us_seq2) && ((us_seq2 - us_seq1) < DMAC_BA_MAX_SEQNO_BY_TWO)) ||
        ((us_seq1 > us_seq2) && ((us_seq1 - us_seq2) > DMAC_BA_MAX_SEQNO_BY_TWO))) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 判断seq1是否小于或等于seq2
 修改历史      :
  1.日    期   : 2014年8月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u8 hmac_ba_rx_seqno_leq(hi_u16 us_seq1, hi_u16 us_seq2)
{
    if (((us_seq1 <= us_seq2) && ((us_seq2 - us_seq1) < DMAC_BA_MAX_SEQNO_BY_TWO)) ||
        ((us_seq1 > us_seq2) && ((us_seq1 - us_seq2) > DMAC_BA_MAX_SEQNO_BY_TWO))) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 判断 us_seqno在bitmap中的bit为是否为1
 修改历史      :
  1.日    期   : 2015年5月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u8 hmac_ba_isset(const hmac_ba_rx_stru *ba_rx_hdl, hi_u16 us_seqno)
{
    hi_u16 us_index;

    if (hmac_baw_within(ba_rx_hdl->us_baw_head, HMAC_BA_BMP_SIZE, us_seqno)) {
        us_index = us_seqno & (HMAC_BA_BMP_SIZE - 1);

        if (hmac_ba_bit_isset(ba_rx_hdl->aul_rx_buf_bitmap, us_index)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 置位 us_seqno对应的 BA rx_bitmap
 修改历史      :
  1.日    期   : 2016年3月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ba_addto_rx_bitmap(hmac_ba_rx_stru *ba_rx_hdl, hi_u16 us_seqno)
{
    hi_u16 us_index;

    if (hmac_baw_within(ba_rx_hdl->us_baw_head, HMAC_BA_BMP_SIZE, us_seqno)) {
        us_index = us_seqno & (HMAC_BA_BMP_SIZE - 1);
        hmac_tx_buf_bitmap_set(ba_rx_hdl->aul_rx_buf_bitmap, us_index);
    }
}

/* ****************************************************************************
 功能描述  : 清除 BA会话 rx_bitmap位
 修改历史      :
  1.日    期   : 2016年3月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ba_clear_rx_bitmap(hmac_ba_rx_stru *ba_rx_hdl)
{
    hi_u16 us_index;

    us_index = dmac_ba_seqno_sub(ba_rx_hdl->us_baw_head, 1) & (HMAC_BA_BMP_SIZE - 1);
    hmac_tx_buf_bitmap_clr(ba_rx_hdl->aul_rx_buf_bitmap, us_index);
}

/* ****************************************************************************
 功能描述  : updata rx bitmap
 修改历史      :
  1.日    期   : 2016年3月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_ba_update_rx_bitmap(const hmac_user_stru *hmac_user, const mac_ieee80211_frame_stru *frame_hdr)
{
    hi_u8               is_4addr;
    hi_u8               is_tods;
    hi_u8               is_from_ds;
    hi_u8               tid;
    hmac_ba_rx_stru    *ba_rx_hdl = HI_NULL;

    /* 考虑四地址情况获取报文的tid */
    is_tods    = mac_hdr_get_to_ds((hi_u8 *)frame_hdr);
    is_from_ds = mac_hdr_get_from_ds((hi_u8 *)frame_hdr);
    is_4addr   = is_tods && is_from_ds;
    tid        = mac_get_tid_value((hi_u8 *)frame_hdr, is_4addr);

    ba_rx_hdl = hmac_user->ast_tid_info[tid].ba_rx_info;
    if (ba_rx_hdl == HI_NULL) {
        return;
    }
    if (ba_rx_hdl->ba_status != DMAC_BA_COMPLETE) {
        return;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_spin_lock(&ba_rx_hdl->st_ba_lock);
#endif

    while (hmac_ba_seqno_lt(ba_rx_hdl->us_baw_head,
        dmac_ba_seqno_sub(ba_rx_hdl->us_baw_start, (HMAC_BA_BMP_SIZE - 1))) == HI_TRUE) {
        ba_rx_hdl->us_baw_head = dmac_ba_seqno_add(ba_rx_hdl->us_baw_head, 1);
        hmac_ba_clear_rx_bitmap(ba_rx_hdl);
    }

    hmac_ba_addto_rx_bitmap(ba_rx_hdl, mac_get_seq_num((hi_u8 *)frame_hdr));

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_spin_unlock(&ba_rx_hdl->st_ba_lock);
#endif

    return;
}

/* ****************************************************************************
 功能描述  : This function compares the given sequence number with the specified
             upper and lower bounds and returns its position relative to them. 调用函数  :
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u8 hmac_ba_seqno_bound_chk(hi_u16 us_seq_lo, hi_u16 us_seq_hi, hi_u16 us_seq)
{
    hi_u8 lo_chk;
    hi_u8 en_hi_chk;
    hi_u8 chk_res = 0;

    lo_chk = hmac_ba_rx_seqno_leq(us_seq_lo, us_seq);
    en_hi_chk = hmac_ba_rx_seqno_leq(us_seq, us_seq_hi);
    if ((lo_chk == HI_TRUE) && (en_hi_chk == HI_TRUE)) {
        chk_res = DMAC_BA_BETWEEN_SEQLO_SEQHI;
    } else if (en_hi_chk == HI_FALSE) {
        chk_res = DMAC_BA_GREATER_THAN_SEQHI;
    }
    return chk_res;
}


/* ****************************************************************************
 功能描述  : 根据us_seq_num从Re-order 中取出对应buff
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hmac_rx_buf_stru *hmac_remove_frame_from_reorder_q(hmac_ba_rx_stru *ba_rx_hdl, hi_u16 us_seq_num)
{
    hi_u16 us_idx;
    hmac_rx_buf_stru *rx_buff = HI_NULL;

    us_idx = (us_seq_num & (WLAN_AMPDU_RX_BUFFER_SIZE - 1));
    rx_buff = &(ba_rx_hdl->ast_re_order_list[us_idx]);
    if ((rx_buff->in_use == 0) || (rx_buff->us_seq_num != us_seq_num)) {
        return HI_NULL;
    }
    rx_buff->in_use = 0;
    return rx_buff;
}

/* ****************************************************************************
 功能描述  :This function reads out the TX-Dscr indexed by the specified sequence number in
            the Retry-Q Ring-Buffer.
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hmac_rx_buf_stru *hmac_get_frame_from_reorder_q(hmac_ba_rx_stru *ba_rx_hdl, hi_u16 us_seq_num)
{
    hi_u16 us_idx;
    hmac_rx_buf_stru *rx_buff = HI_NULL;

    us_idx = (us_seq_num & (WLAN_AMPDU_RX_BUFFER_SIZE - 1));

    rx_buff = &(ba_rx_hdl->ast_re_order_list[us_idx]);

    if ((rx_buff->in_use == 0) || (rx_buff->us_seq_num != us_seq_num)) {
        return HI_NULL;
    }
    return rx_buff;
}

/* ****************************************************************************
 功能描述  : 获取Re-Order队列中的ba_buffer_frame
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hmac_rx_buf_stru *hmac_ba_buffer_frame_in_reorder(hmac_ba_rx_stru *ba_rx_hdl, hi_u16 us_seq_num)
{
    hi_u16 us_buf_index;
    hmac_rx_buf_stru *rx_buf = HI_NULL;

    us_buf_index = (us_seq_num & (WLAN_AMPDU_RX_BUFFER_SIZE - 1));

    rx_buf = &(ba_rx_hdl->ast_re_order_list[us_buf_index]);

    if (rx_buf->in_use == 1) {
        hmac_rx_free_netbuf_list(&rx_buf->netbuf_head, rx_buf->num_buf);
        oam_info_log1(0, OAM_SF_BA, "{hmac_ba_buffer_frame_in_reorder::slot already used, seq[%d].}", us_seq_num);
    } else {
        ba_rx_hdl->mpdu_cnt++;
    }

    rx_buf->in_use = 1;

    return rx_buf;
}

/* ****************************************************************************
 功能描述  : 冲刷重排序缓冲区至给定的sequence number位置
 修改历史      :
  1.日    期   : 2014年11月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ba_send_frames_with_gap(hmac_ba_rx_stru *ba_rx_hdl, oal_netbuf_head_stru *netbuf_header,
    hi_u16 us_last_seqnum, const mac_vap_stru *mac_vap)
{
    hi_u8 num_frms = 0;
    hi_u16 us_seq_num;
    hmac_rx_buf_stru *rx_buf = HI_NULL;
    hi_u8 loop_index;
    oal_netbuf_stru *netbuf = HI_NULL;

    us_seq_num = ba_rx_hdl->us_baw_start;

    while (us_seq_num != us_last_seqnum) {
        rx_buf = hmac_remove_frame_from_reorder_q(ba_rx_hdl, us_seq_num);
        if (rx_buf == HI_NULL) {
            us_seq_num = dmac_ba_seqno_add(us_seq_num, 1);
            continue;
        }
        ba_rx_hdl->mpdu_cnt--;
        netbuf = oal_netbuf_peek(&rx_buf->netbuf_head);
        if (oal_unlikely(netbuf == HI_NULL) && (mac_vap != HI_NULL)) {
            us_seq_num = dmac_ba_seqno_add(us_seq_num, 1);
            rx_buf->num_buf = 0;

            continue;
        }

        for (loop_index = 0; loop_index < rx_buf->num_buf; loop_index++) {
            netbuf = oal_netbuf_delist(&rx_buf->netbuf_head);
            if (netbuf != HI_NULL) {
                oal_netbuf_add_to_list_tail(netbuf, netbuf_header);
            }
        }
        rx_buf->num_buf = 0;
        num_frms++;
        us_seq_num = dmac_ba_seqno_add(us_seq_num, 1);
    }

    if (ba_rx_hdl->mpdu_cnt != 0) {
        oam_info_log1(0, OAM_SF_BA, "{hmac_ba_send_frames_with_gap::uc_mpdu_cnt=%d.}", ba_rx_hdl->mpdu_cnt);
    }
    return num_frms;
}

/* ****************************************************************************
 功能描述  : All MSDUs with sequence number starting from the
             start of the BA-Rx window are processed in order and
             are added to the list which will be passed up to hmac.
             Processing is stopped when the first missing MSDU is encountered.
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u16 hmac_ba_send_frames_in_order(hmac_ba_rx_stru *ba_rx_hdl, oal_netbuf_head_stru *netbuf_header,
    const mac_vap_stru *mac_vap)
{
    hi_u16 us_seq_num;
    hmac_rx_buf_stru *rx_buf = HI_NULL;
    hi_u8 loop_index;
    oal_netbuf_stru *netbuf = HI_NULL;

    us_seq_num = ba_rx_hdl->us_baw_start;
    rx_buf = hmac_remove_frame_from_reorder_q(ba_rx_hdl, us_seq_num);
    while (rx_buf != HI_NULL) {
        ba_rx_hdl->mpdu_cnt--;
        us_seq_num = hmac_ba_seqno_add(us_seq_num, 1);

        netbuf = oal_netbuf_peek(&rx_buf->netbuf_head);
        if ((netbuf == HI_NULL) && (mac_vap != HI_NULL)) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_BA, "{hmac_ba_send_frames_in_order::[%d] slot error.}",
                us_seq_num);
            rx_buf->num_buf = 0;
            rx_buf = hmac_remove_frame_from_reorder_q(ba_rx_hdl, us_seq_num);
            continue;
        }

        for (loop_index = 0; loop_index < rx_buf->num_buf; loop_index++) {
            netbuf = oal_netbuf_delist(&rx_buf->netbuf_head);
            if (netbuf != HI_NULL) {
                oal_netbuf_add_to_list_tail(netbuf, netbuf_header);
            }
        }

        rx_buf->num_buf = 0;
        rx_buf = hmac_remove_frame_from_reorder_q(ba_rx_hdl, us_seq_num);
    }

    return us_seq_num;
}

/* ****************************************************************************
 功能描述  : 处理接收到Blockack Req帧需要触发delba的处理函数
**************************************************************************** */
hi_void hmac_rx_bar_delba(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, hi_u8 tidno, hi_u8 *puc_da)
{
    mac_action_mgmt_args_stru action_args;
    hi_u32 ret;

    action_args.category = MAC_ACTION_CATEGORY_BA;
    action_args.action = MAC_BA_ACTION_DELBA;
    action_args.arg1 = tidno;               /* 该数据帧对应的TID号 */
    action_args.arg2 = MAC_RECIPIENT_DELBA; /* DELBA中，触发删除BA会话的发起端 */
    action_args.arg3 = MAC_POOR_CHANNEL;    /* DELBA中代表删除reason */
    action_args.puc_arg5 = puc_da;          /* DELBA中代表目的地址 */

    ret = hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_BA, "hmac_rx_bar_delba return NON SUCCESS.");
    }
}

/* ****************************************************************************
 功能描述  : 将报文缓存至重排序队列
 修改历史      :
  1.日    期   : 2014年11月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ba_buffer_rx_frame(hmac_ba_rx_stru *ba_rx_hdl, const hmac_rx_ctl_stru *cb_ctrl,
    oal_netbuf_head_stru *netbuf_header, hi_u16 us_seq_num, const mac_vap_stru *mac_vap)
{
    hmac_rx_buf_stru *rx_netbuf = HI_NULL;
    oal_netbuf_stru *netbuf = HI_NULL;
    hi_u8 netbuf_index;

    /* Get the pointer to the buffered packet */
    rx_netbuf = hmac_ba_buffer_frame_in_reorder(ba_rx_hdl, us_seq_num);

    /* Update the buffered receive packet details */
    rx_netbuf->us_seq_num = us_seq_num;
    rx_netbuf->num_buf = cb_ctrl->buff_nums; /* 标识该MPDU占用的netbuff个数，一般用于AMSDU */
    rx_netbuf->rx_time = (hi_u32)hi_get_milli_seconds();

    /* all buffers of this frame must be deleted from the buf list */
    for (netbuf_index = rx_netbuf->num_buf; netbuf_index > 0; netbuf_index--) {
        netbuf = oal_netbuf_delist(netbuf_header);
        if (oal_unlikely(netbuf != HI_NULL)) {
            oal_netbuf_add_to_list_tail(netbuf, &rx_netbuf->netbuf_head);
        } else {
            if (mac_vap != HI_NULL) {
                oam_error_log0(mac_vap->vap_id, OAM_SF_BA, "{hmac_ba_buffer_rx_frame:netbuff error in amsdu.}");
            }
        }
    }
}

/* ****************************************************************************
 功能描述  : 将重排序队列中可以上传的报文加到buf链表的尾部
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ba_reorder_rx_data(hmac_ba_rx_stru *ba_rx_hdl, oal_netbuf_head_stru *netbuf_header,
    const mac_vap_stru *mac_vap, hi_u16 us_seq_num)
{
    hi_u8 seqnum_pos;
    hi_u16 us_temp_winend;
    hi_u16 us_temp_winstart;

    seqnum_pos = hmac_ba_seqno_bound_chk(ba_rx_hdl->us_baw_start, ba_rx_hdl->us_baw_end, us_seq_num);
    if (seqnum_pos == DMAC_BA_BETWEEN_SEQLO_SEQHI) {
        ba_rx_hdl->us_baw_start = hmac_ba_send_frames_in_order(ba_rx_hdl, netbuf_header, mac_vap);
        ba_rx_hdl->us_baw_end = dmac_ba_seqno_add(ba_rx_hdl->us_baw_start, (ba_rx_hdl->us_baw_size - 1));
    } else if (seqnum_pos == DMAC_BA_GREATER_THAN_SEQHI) {
        us_temp_winend = us_seq_num;
        us_temp_winstart = hmac_ba_seqno_sub(us_temp_winend, (ba_rx_hdl->us_baw_size - 1));

        hmac_ba_send_frames_with_gap(ba_rx_hdl, netbuf_header, us_temp_winstart, mac_vap);
        ba_rx_hdl->us_baw_start = us_temp_winstart;
        ba_rx_hdl->us_baw_start = hmac_ba_send_frames_in_order(ba_rx_hdl, netbuf_header, mac_vap);
        ba_rx_hdl->us_baw_end = hmac_ba_seqno_add(ba_rx_hdl->us_baw_start, (ba_rx_hdl->us_baw_size - 1));
    } else {
        oam_info_log3(mac_vap->vap_id, OAM_SF_BA, "{hmac_ba_reorder_rx_data::else branch seqno[%d] ws[%d] we[%d].}",
            us_seq_num, ba_rx_hdl->us_baw_start, ba_rx_hdl->us_baw_end);
    }
}

/* ****************************************************************************
 功能描述  : 冲刷重排序队列
 输入参数  : pst_rx_ba: 接收会话句柄
 输出参数  : pst_rx_ba: 接收会话句柄
 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ba_flush_reorder_q(hmac_ba_rx_stru *rx_ba)
{
    hmac_rx_buf_stru *rx_buf = HI_NULL;
    hi_u16 us_index;

    for (us_index = 0; us_index < WLAN_AMPDU_RX_BUFFER_SIZE; us_index++) {
        rx_buf = &(rx_ba->ast_re_order_list[us_index]);

        if (rx_buf->in_use == HI_TRUE) {
            hmac_rx_free_netbuf_list(&rx_buf->netbuf_head, rx_buf->num_buf);

            rx_buf->in_use = HI_FALSE;
            rx_buf->num_buf = 0;
            rx_ba->mpdu_cnt--;
        }
    }

    if (rx_ba->mpdu_cnt != 0) {
        oam_warning_log1(0, OAM_SF_BA, "{hmac_ba_flush_reorder_q:: %d mpdu cnt left.}", rx_ba->mpdu_cnt);
    }
}

/* ****************************************************************************
 功能描述  : 检查是否能做ba重排序处理
 修改历史      :
  1.日    期   : 2013年11月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ba_check_rx_aggr(const mac_vap_stru *mac_vap, const mac_ieee80211_frame_stru *frame_hdr)
{
    /* 该vap是否是ht */
    if (mac_vap->mib_info->wlan_mib_sta_config.dot11_high_throughput_option_implemented == HI_FALSE) {
        return HI_FAIL;
    }
    /* 判断该帧是不是qos帧 */
    if (((hi_u8 *)frame_hdr)[0] != (WLAN_FC0_SUBTYPE_QOS | WLAN_FC0_TYPE_DATA)) {
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 过滤ampdu的每一个mpdu 有未确认报文需要入重传队列
 返 回 值  : 非HI_SUCCESS:表示异常，后面处理直接Drop
 修改历史      :
  1.日    期   : 2014年11月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_ba_filter_serv(const mac_vap_stru *mac_vap, const hmac_user_stru *hmac_user,
    const hmac_rx_ctl_stru *cb_ctrl, const mac_ieee80211_frame_stru *frame_hdr,
    const hmac_filter_serv_info_stru *filter_serv_info)
{
    hi_u8 is_tods = mac_hdr_get_to_ds((hi_u8 *)frame_hdr);
    hi_u8 is_from_ds = mac_hdr_get_from_ds((hi_u8 *)frame_hdr);
    hi_u8 is_4addr = is_tods && is_from_ds;
    hi_u8 tid = mac_get_tid_value((hi_u8 *)frame_hdr, is_4addr);

    if (hmac_ba_check_rx_aggr(mac_vap, frame_hdr) != HI_SUCCESS) {
        return HI_SUCCESS;
    }

    /* 考虑四地址情况获取报文的tid */
    hmac_ba_rx_stru *ba_rx_hdl = hmac_user->ast_tid_info[tid].ba_rx_info;
    if (ba_rx_hdl == HI_NULL) {
        return HI_SUCCESS;
    } else if (ba_rx_hdl->ba_status != DMAC_BA_COMPLETE) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_BA, "{hmac_ba_filter_serv::ba_status = %d.", ba_rx_hdl->ba_status);
        return HI_SUCCESS;
    }

    /* 暂时保存BA窗口的序列号，用于鉴别是否有帧上报 */
    hi_u16 us_baw_start_temp = ba_rx_hdl->us_baw_start;

    hi_u16 us_seq_num = mac_get_seq_num((hi_u8 *)frame_hdr);

    /* 兼容接收方向聚合和分片共存的情况 */
    if ((hi_u8)frame_hdr->frame_control.more_frag == HI_TRUE) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_BA, "{hmac_ba_filter_serv::[seq_num=%d] BA set UP!", us_seq_num);
        return HI_SUCCESS;
    }

    /* duplicate frame判断 */
    if (hmac_ba_seqno_lt(us_seq_num, ba_rx_hdl->us_baw_start) == HI_TRUE) {
        /* 上次非定时器上报，直接删除duplicate frame帧，否则，直接上报 */
        if ((ba_rx_hdl->timer_triggered == HI_FALSE) && (hmac_ba_isset(ba_rx_hdl, us_seq_num) == HI_TRUE)) {
            /* 确实已经收到该帧 */
            return HI_FAIL;
        }
        return HI_SUCCESS;
    }

    if (hmac_ba_seqno_lt(ba_rx_hdl->us_baw_tail, us_seq_num) == HI_TRUE) {
        ba_rx_hdl->us_baw_tail = us_seq_num;
    }

    /* 接收到的帧的序列号等于BAW_START，并且缓存队列帧个数为0，则直接上报给HMAC */
    if ((us_seq_num == ba_rx_hdl->us_baw_start) && (ba_rx_hdl->mpdu_cnt == 0)) {
        ba_rx_hdl->us_baw_start = dmac_ba_seqno_add(ba_rx_hdl->us_baw_start, 1);
        ba_rx_hdl->us_baw_end = dmac_ba_seqno_add(ba_rx_hdl->us_baw_end, 1);
    } else {
        /* Buffer the new MSDU */
        *(filter_serv_info->pen_is_ba_buf) = HI_TRUE;

        hmac_ba_buffer_rx_frame(ba_rx_hdl, cb_ctrl, filter_serv_info->netbuf_header, us_seq_num, mac_vap);

        /* put the reordered netbufs to the end of the list */
        hmac_ba_reorder_rx_data(ba_rx_hdl, filter_serv_info->netbuf_header, mac_vap, us_seq_num);

        /* Check for Sync loss and flush the reorder queue when one is detected */
        if ((ba_rx_hdl->us_baw_tail == dmac_ba_seqno_sub(ba_rx_hdl->us_baw_start, 1)) && (ba_rx_hdl->mpdu_cnt > 0)) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_BA, "{hmac_ba_filter_serv::Sync loss flush the reorder queue.}");
            hmac_ba_flush_reorder_q(ba_rx_hdl);
        }
    }

    if (us_baw_start_temp != ba_rx_hdl->us_baw_start) {
        ba_rx_hdl->timer_triggered = HI_FALSE;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : This function reorders the Reciver buffer and sends frames to the higher
             layer on reception of a Block-Ack-Request frame. It also updates the
             receiver buffer window.
 修改历史      :
  1.日    期   : 2014年11月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_reorder_ba_rx_buffer_bar(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, hi_u8 tidno,
    hi_u16 us_start_seq_num, hi_u8 *puc_sa_addr)
{
    oal_netbuf_head_stru netbuf_head;
    hi_u8 seqnum_pos;
    hmac_ba_rx_stru *rx_ba = HI_NULL;

    rx_ba = hmac_user->ast_tid_info[tidno].ba_rx_info;
    if (rx_ba == HI_NULL) {
        hmac_rx_bar_delba(hmac_vap, hmac_user, tidno, puc_sa_addr);
        return;
    }
    /* 针对 BAR 的SSN和窗口的start_num相等时，不需要移窗 */
    if (rx_ba->us_baw_start == us_start_seq_num) {
        return;
    }

    oal_netbuf_list_head_init(&netbuf_head);
    seqnum_pos = hmac_ba_seqno_bound_chk(rx_ba->us_baw_start, rx_ba->us_baw_end, us_start_seq_num);
    /* 针对BAR的SSN在窗口内才移窗 */
    if (seqnum_pos == DMAC_BA_BETWEEN_SEQLO_SEQHI) {
        hmac_ba_send_frames_with_gap(rx_ba, &netbuf_head, us_start_seq_num, hmac_vap->base_vap);
        rx_ba->us_baw_start = us_start_seq_num;
        rx_ba->us_baw_start = hmac_ba_send_frames_in_order(rx_ba, &netbuf_head, hmac_vap->base_vap);
        rx_ba->us_baw_end = hmac_ba_seqno_add(rx_ba->us_baw_start, (rx_ba->us_baw_size - 1));

        hmac_rx_lan_frame(&netbuf_head);
    } else if (seqnum_pos == DMAC_BA_GREATER_THAN_SEQHI) {
        /* 异常 */
        oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_BA,
            "{hmac_reorder_ba_rx_buffer_bar::receive a sn out of winsize bar, baw_start=%d baw_end=%d, seq_num=%d.}",
            rx_ba->us_baw_start, rx_ba->us_baw_end, us_start_seq_num);
    }
}

/* ****************************************************************************
 功能描述  : 从重排序队列中获取skb链
 修改历史      :
  1.日    期   : 2014年5月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ba_rx_prepare_bufflist(const hmac_vap_stru *hmac_vap, hmac_rx_buf_stru *rx_buf,
    oal_netbuf_head_stru *netbuf_head)
{
    oal_netbuf_stru *netbuf = HI_NULL;
    hi_u8 loop_index;

    netbuf = oal_netbuf_peek(&rx_buf->netbuf_head);
    if ((netbuf == HI_NULL) && (hmac_vap != HI_NULL)) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_BA, "{hmac_ba_rx_prepare_bufflist::pst_netbuf null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    for (loop_index = 0; loop_index < rx_buf->num_buf; loop_index++) {
        netbuf = oal_netbuf_delist(&rx_buf->netbuf_head);
        if (netbuf != HI_NULL) {
            oal_netbuf_add_to_list_tail(netbuf, netbuf_head);
        } else {
            if (hmac_vap != HI_NULL) {
                oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_BA,
                    "{hmac_ba_rx_prepare_bufflist::uc_num_buf in reorder list is error.}");
            }
        }
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 上报重排序队列中超时的报文
 修改历史      :
  1.日    期   : 2014年5月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ba_send_reorder_timeout(hmac_ba_rx_stru *rx_ba, const hmac_vap_stru *hmac_vap,
    const hmac_ba_alarm_stru *alarm_data, hi_u16 *pus_timeout)
{
    hmac_rx_buf_stru *rx_buf = HI_NULL;
    hi_u32 time_diff;
    hi_u16 aus_rx_timeout[WLAN_WME_AC_BUTT] = {HMAC_BA_RX_BE_TIMEOUT, HMAC_BA_RX_BK_TIMEOUT,
                                               HMAC_BA_RX_VI_TIMEOUT, HMAC_BA_RX_VO_TIMEOUT};

    hi_u16 us_baw_head  = rx_ba->us_baw_start;
    hi_u16 us_baw_start = rx_ba->us_baw_start; /* 保存最初的窗口起始序列号 */
    hi_u32 rx_timeout   = (hi_u32)aus_rx_timeout[wlan_wme_tid_to_ac(alarm_data->tid)];
    hi_u16 us_baw_end   = hmac_ba_seqno_add(rx_ba->us_baw_tail, 1);
    hi_u8  buff_count   = 0;

    oal_netbuf_head_stru netbuf_head;
    oal_netbuf_list_head_init(&netbuf_head);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_spin_lock(&rx_ba->st_ba_lock);
#endif

    while (us_baw_head != us_baw_end) {
        rx_buf = hmac_get_frame_from_reorder_q(rx_ba, us_baw_head);
        if (rx_buf == HI_NULL) {
            buff_count++;
            us_baw_head = hmac_ba_seqno_add(us_baw_head, 1);
            continue;
        }

        time_diff = (hi_u32)hi_get_milli_seconds() - rx_buf->rx_time;
        if (time_diff < rx_timeout) {
            *pus_timeout = (hi_u16)(rx_timeout - time_diff);
            break;
        }

        rx_ba->mpdu_cnt--;
        rx_buf->in_use = 0;
        buff_count++;
        if (hmac_ba_rx_prepare_bufflist(hmac_vap, rx_buf, &netbuf_head) != HI_SUCCESS) {
            us_baw_head = hmac_ba_seqno_add(us_baw_head, 1);
            continue;
        }
        us_baw_head = hmac_ba_seqno_add(us_baw_head, 1);
        rx_ba->us_baw_start = hmac_ba_seqno_add(rx_ba->us_baw_start, buff_count);
        rx_ba->us_baw_end = hmac_ba_seqno_add(rx_ba->us_baw_start, (rx_ba->us_baw_size - 1));

        buff_count = 0;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_spin_unlock(&rx_ba->st_ba_lock);
#endif

    /* 判断本次定时器超时是否有帧上报 */
    rx_ba->timer_triggered = (us_baw_start != rx_ba->us_baw_start) ? HI_TRUE : rx_ba->timer_triggered;
    hmac_rx_lan_frame(&netbuf_head);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : ba会话超时处理
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_ba_timeout_fn(hi_void *arg)
{
    hi_u8 *pm_wlan_need_stop_ba = mac_get_pm_wlan_need_stop_ba();
    hi_u16 aus_rx_timeout[WLAN_WME_AC_BUTT] = {HMAC_BA_RX_BE_TIMEOUT, HMAC_BA_RX_BK_TIMEOUT,
                                               HMAC_BA_RX_VI_TIMEOUT, HMAC_BA_RX_VO_TIMEOUT};

    hmac_ba_alarm_stru *alarm_data = (hmac_ba_alarm_stru *)arg;
    if ((alarm_data == HI_NULL) || (alarm_data->tid >= WLAN_TID_MAX_NUM)) {
        oam_error_log1(0, OAM_SF_BA, "{hmac_ba_timeout_fn::invalid param,%p.}", (uintptr_t)alarm_data);
        return HI_ERR_CODE_ARRAY_OVERFLOW;
    }
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(alarm_data->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_BA, "{hmac_ba_timeout_fn::pst_vap null. vap id %d.}", alarm_data->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }
    hmac_ba_rx_stru *rx_ba = (hmac_ba_rx_stru *)alarm_data->ba;
    if (rx_ba == HI_NULL) {
        oam_error_log0(0, OAM_SF_BA, "{hmac_ba_timeout_fn::pst_rx_ba is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    hi_u16 us_timeout = aus_rx_timeout[wlan_wme_tid_to_ac(alarm_data->tid)];
    if (rx_ba->mpdu_cnt > 0) {
        hmac_ba_send_reorder_timeout(rx_ba, hmac_vap, alarm_data, &us_timeout);
    }
    if ((*pm_wlan_need_stop_ba) == HI_TRUE) {
        return HI_SUCCESS;
    }
    frw_timer_restart_timer(&(rx_ba->ba_timer), us_timeout, HI_FALSE);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 重置rx ba结构体
 修改历史      :
  1.日    期   : 2014年12月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_ba_reset_rx_handle(hmac_ba_rx_stru **rx_ba, hi_u8 tid)
{
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    hmac_vap_stru *hmac_vap = HI_NULL;
#else
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
#endif
    hi_bool need_del_lut = HI_TRUE;

    if (oal_unlikely((*rx_ba == HI_NULL) || ((*rx_ba)->is_ba) != HI_TRUE)) {
        oam_warning_log0(0, OAM_SF_BA, "{hmac_ba_reset_rx_handle::rx ba not set yet.}");
        return;
    }
    if (tid >= WLAN_TID_MAX_NUM) {
        oam_error_log1(0, OAM_SF_BA, "{hmac_ba_reset_rx_handle::tid %d overflow.}", tid);
        return;
    }

    /* Step1: disable the flag of ba session */
    (*rx_ba)->is_ba = HI_FALSE;

    /* Step2: flush reorder q */
    hmac_ba_flush_reorder_q(*rx_ba);
    if ((*rx_ba)->lut_index == DMAC_INVALID_BA_LUT_INDEX) {
        need_del_lut = HI_FALSE;
        oam_warning_log1(0, OAM_SF_BA, "{hmac_ba_reset_rx_handle::no del, lut idx[%d]}\n", (*rx_ba)->lut_index);
    }

    /* Step3: if lut index is valid, del lut index alloc before */
    if (((*rx_ba)->ba_policy == MAC_BA_POLICY_IMMEDIATE) && (need_del_lut == HI_TRUE)) {
        oal_del_lut_index(hmac_dev->auc_rx_ba_lut_idx_table, (*rx_ba)->lut_index);
    }

    /* Step4: dec the ba session cnt maitence in device struc */
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    hmac_vap = hmac_vap_get_vap_stru((*rx_ba)->alarm_data.vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_BA, "{hmac_ba_reset_rx_handle::pst_hmac_vap is null.}");
        oal_mem_free(*rx_ba);
        *rx_ba = HI_NULL;
        return;
    }
    hmac_rx_ba_session_decr(hmac_vap);
#else
    hmac_rx_ba_session_decr(hmac_dev);
#endif
    /* Step5: Del Timer */
    if ((*rx_ba)->ba_timer.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&((*rx_ba)->ba_timer));
    }
    /* Step6: Free rx handle */
    oal_mem_free(*rx_ba);
    *rx_ba = HI_NULL;
}

/* ****************************************************************************
 功能描述  : 重置tx ba结构体
 修改历史      :
  1.日    期   : 2019年5月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_ba_reset_tx_handle(hmac_ba_tx_stru **tx_ba)
{
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    hmac_vap_stru *hmac_vap = HI_NULL;
#else
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
#endif

    if (oal_unlikely(*tx_ba == HI_NULL)) {
        oam_warning_log0(0, OAM_SF_BA, "{hmac_ba_reset_tx_handle::tx ba not set yet.}");
        return;
    }
    /* 清除发送方向会话句柄 */
    if ((*tx_ba)->addba_timer.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&((*tx_ba)->addba_timer));
    }
    /* 存在TX BA会话句柄，要-- */
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    hmac_vap = hmac_vap_get_vap_stru((*tx_ba)->alarm_data.vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_BA, "{hmac_ba_reset_tx_handle::pst_hmac_vap is null.}");
        oal_mem_free(*tx_ba);
        *tx_ba = HI_NULL;
        return;
    }
    hmac_tx_ba_session_decr(hmac_vap);
#else
    hmac_tx_ba_session_decr(hmac_dev);
#endif
    oal_mem_free(*tx_ba);
    *tx_ba = HI_NULL;
}

/* ****************************************************************************
 功能描述  : 从空口接收ADDBA_REQ帧的处理函数
 修改历史      :
  1.日    期   : 2014年11月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_mgmt_check_set_rx_ba_ok(const hmac_vap_stru *hmac_vap, hmac_ba_rx_stru *ba_rx_info,
    hmac_device_stru *hmac_dev)
{
    /* 立即块确认判断 */
    if (ba_rx_info->ba_policy == MAC_BA_POLICY_IMMEDIATE) {
        if (hmac_vap->base_vap->mib_info->wlan_mib_sta_config.dot11_immediate_block_ack_option_implemented ==
            HI_FALSE) {
            /* 不支持立即块确认 */
            oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_BA,
                "{hmac_mgmt_check_set_rx_ba_ok::not support immediate Block Ack.}");
            return MAC_INVALID_REQ_PARAMS;
        } else {
            if (ba_rx_info->back_var != MAC_BACK_COMPRESSED) {
                /* 不支持非压缩块确认 */
                oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_BA,
                    "{hmac_mgmt_check_set_rx_ba_ok::not support non-Compressed Block Ack.}");
                return MAC_REQ_DECLINED;
            }
        }
    } else if (ba_rx_info->ba_policy == MAC_BA_POLICY_DELAYED) {
        /* 延迟块确认不支持 */
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_BA,
            "{hmac_mgmt_check_set_rx_ba_ok::not support delayed Block Ack.}");
        return MAC_INVALID_REQ_PARAMS;
    }
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    if (hmac_vap->rx_ba_session_num > WLAN_MAX_RX_BA) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_BA,
            "{hmac_mgmt_check_set_rx_ba_ok::uc_rx_ba_session_num[%d] is up to max.}\r\n", hmac_vap->rx_ba_session_num);
        return MAC_REQ_DECLINED;
    }
#else
    if (hmac_dev->rx_ba_session_num > WLAN_MAX_RX_BA) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_BA,
            "{hmac_mgmt_check_set_rx_ba_ok::uc_rx_ba_session_num[%d] is up to max.}\r\n", hmac_dev->rx_ba_session_num);
        return MAC_REQ_DECLINED;
    }
#endif
    /* 获取BA LUT INDEX */
    ba_rx_info->lut_index = oal_get_lut_index(hmac_dev->auc_rx_ba_lut_idx_table,
                                              HMAC_BA_LUT_IDX_BMAP_LEN, HAL_MAX_BA_LUT_SIZE);
    /* LUT index表已满 */
    if (ba_rx_info->lut_index == DMAC_INVALID_BA_LUT_INDEX) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_BA,
            "{hmac_mgmt_check_set_rx_ba_ok::ba lut index table full.}\n");
        return MAC_REQ_DECLINED;
    }

    return MAC_SUCCESSFUL_STATUSCODE;
}

/* ****************************************************************************
 功能描述  : 接收到Blockack Req帧的处理函数
 修改历史      :
  1.日    期   : 2015年1月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_up_rx_bar(hmac_vap_stru *hmac_vap, const hmac_rx_ctl_stru *rx_ctl)
{
    hi_u8 *puc_payload = HI_NULL;
    mac_ieee80211_frame_stru *frame_hdr = HI_NULL;
    hi_u8 *sa_mac_addr = HI_NULL;
    hi_u8 tidno;
    hmac_user_stru *hmac_user = HI_NULL;
    hi_u16 us_start_seqnum;

    frame_hdr = (mac_ieee80211_frame_stru *)(rx_ctl->pul_mac_hdr_start_addr);
    sa_mac_addr = frame_hdr->auc_address2;

    /*  获取用户指针 */
    hmac_user = mac_vap_get_hmac_user_by_addr(hmac_vap->base_vap, sa_mac_addr, WLAN_MAC_ADDR_LEN);
    if (hmac_user == HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_up_rx_bar::pst_ta_user is null.}");
        return;
    }

    /* 获取帧头和payload指针 */
    puc_payload = (hi_u8 *)(rx_ctl->pul_mac_hdr_start_addr) + rx_ctl->mac_header_len;

    /* *********************************************************************** */
    /*                     BlockAck Request Frame Format                     */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BAR Control|BlockAck Starting    |FCS|  */
    /* |             |        |  |  |           |Sequence number      |   |  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |2          |2                    |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    tidno = (puc_payload[1] & 0xF0) >> 4; /* 1:下标，4:右移4位 */
    us_start_seqnum = mac_get_bar_start_seq_num(puc_payload);

    hmac_reorder_ba_rx_buffer_bar(hmac_vap, hmac_user, tidno, us_start_seqnum, sa_mac_addr);
}

#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
hi_u8 hmac_is_device_ba_setup(hi_void)
{
    hi_u8 vap_id;
    hmac_vap_stru *hmac_vap = HI_NULL;

    for (vap_id = 0; vap_id < WLAN_VAP_NUM_PER_DEVICE; vap_id++) {
        hmac_vap = hmac_vap_get_vap_stru(vap_id);
        if (hmac_vap == HI_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{hmac_is_device_ba_setup pst_mac_vap is null.}");
            continue;
        }
        if ((hmac_vap->base_vap->vap_state != MAC_VAP_STATE_UP) &&
            (hmac_vap->base_vap->vap_state != MAC_VAP_STATE_PAUSE)) {
            continue;
        }
        if ((hmac_vap->tx_ba_session_num != 0) || (hmac_vap->rx_ba_session_num != 0)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}
#else
hi_u8 hmac_is_device_ba_setup(hi_void)
{
    hmac_device_stru *hmac_dev = hmac_get_device_stru();

    if ((hmac_dev->tx_ba_session_num != 0) || (hmac_dev->rx_ba_session_num != 0)) {
        return HI_TRUE;
    }
    return HI_FALSE;
}
#endif

/* ****************************************************************************
 功能描述  : 创建BA 会话事件处理函数
 修改历史      :
  1.日    期   : 2013年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_create_ba_event(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    hmac_tx_ctl_stru cb;
    dmac_to_hmac_ctx_event_stru *create_ba_event = HI_NULL;

    event = (frw_event_stru *)event_mem->puc_data;

    create_ba_event = (dmac_to_hmac_ctx_event_stru *)event->auc_event_data;

    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(create_ba_event->user_index);
    if (hmac_user == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ANY, "{hmac_create_ba_event::pst_hmac_user null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap = hmac_vap_get_vap_stru(create_ba_event->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ANY, "{hmac_create_ba_event::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    cb.tid = create_ba_event->tid;

    hmac_tx_ba_setup(hmac_vap, hmac_user, cb.tid);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 删除 BA会话事件处理函数
 修改历史      :
  1.日    期   : 2013年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_del_ba_event(frw_event_mem_stru *event_mem)
{
    hi_u8  del_fail_flag = HI_FALSE;
    mac_action_mgmt_args_stru    action_args; /* 用于填写ACTION帧的参数 */
    hmac_tid_stru               *hmac_tid     = HI_NULL;
    frw_event_stru              *event        = (frw_event_stru *)event_mem->puc_data;
    dmac_to_hmac_ctx_event_stru *del_ba_event = (dmac_to_hmac_ctx_event_stru *)event->auc_event_data;
    hmac_user_stru              *hmac_user    = (hmac_user_stru *)hmac_user_get_user_stru(del_ba_event->user_index);
    hi_u8 tid;

    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_del_ba_event::pst_hmac_user null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(del_ba_event->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_del_ba_event::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_user->base_user->cur_protocol_mode = del_ba_event->cur_protocol;
    hi_u32 ret = hmac_config_user_info_syn(hmac_vap->base_vap, hmac_user->base_user);
    if (ret != HI_SUCCESS) {
        return ret;
    }
#endif

    for (tid = 0; tid < WLAN_TID_MAX_NUM; tid++) {
        hmac_tid = &hmac_user->ast_tid_info[tid];
        if (hmac_tid->ba_tx_info == HI_NULL) {
            continue; /* 指针为空 未建立BA */
        }
        action_args.category = MAC_ACTION_CATEGORY_BA;
        action_args.action   = MAC_BA_ACTION_DELBA;
        action_args.arg1     = tid;                                 /* 该数据帧对应的TID号 */
        action_args.arg2     = MAC_ORIGINATOR_DELBA;                /* ADDBA_REQ中，buffer_size的默认大小 */
        action_args.arg3     = MAC_UNSPEC_REASON;                   /* BA会话的确认策略 */
        action_args.puc_arg5 = hmac_user->base_user->user_mac_addr; /* ba会话对应的user */

        /* 删除BA会话 */
        if (hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args) != HI_SUCCESS) {
            oam_warning_log2(hmac_vap->base_vap->vap_id, OAM_SF_ANY,
                "{hmac_del_ba_event::hmac_mgmt_tx_action failed,tid[%d],user id[%d].}", tid, del_ba_event->user_index);
            del_fail_flag = HI_TRUE;
            continue;
        }
    }

    if (del_fail_flag) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_ANY,
            "{hmac_del_ba_event::send delba occur fail, notify alg resume old protocol!.}");
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
