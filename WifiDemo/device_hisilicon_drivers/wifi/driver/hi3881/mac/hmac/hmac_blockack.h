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

#ifndef __DMAC_BLOCKACK_H__
#define __DMAC_BLOCKACK_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "dmac_ext_if.h"
#include "hmac_ext_if.h"
#include "hmac_user.h"
#include "hmac_main.h"
#include "hmac_rx_data.h"
#include "hmac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* Default values for receive timeout */
#define HMAC_BA_RX_VO_TIMEOUT 40  /* 40 milliseconds */
#define HMAC_BA_RX_VI_TIMEOUT 100 /* 100 milliseconds */
#define HMAC_BA_RX_BE_TIMEOUT 60  /* 100 milliseconds */
#define HMAC_BA_RX_BK_TIMEOUT 100 /* 100 milliseconds */

#define HMAC_BA_SEQNO_MASK                  0x0FFF      /* max sequece number */
#define hmac_ba_seqno_sub(_seq1, _seq2)     (((_seq1) - (_seq2)) & HMAC_BA_SEQNO_MASK)
#define hmac_ba_seqno_add(_seq1, _seq2)     (((_seq1) + (_seq2)) & HMAC_BA_SEQNO_MASK)

#define HMAC_BA_LUT_IDX_BMAP_LEN            ((HAL_MAX_BA_LUT_SIZE + 7) >> 3)

/* 计算seq num到ba窗start的偏移量 */
#define hmac_ba_index(_st, _seq) (((_seq) - (_st)) & 4095)

#define HMAC_TX_BUF_BITMAP_LOG2_WORD_SIZE 5 /* log2(32)等于 5 */

/* 发送BA窗口记录seq number的bitmap所使用的类型长度 */
#define HMAC_TX_BUF_BITMAP_WORD_SIZE 32

#define HMAC_TX_BUF_BITMAP_WORD_MASK (HMAC_TX_BUF_BITMAP_WORD_SIZE - 1)

#define hmac_tx_buf_bitmap_set(_bitmap, _idx) \
    (_bitmap[_idx >> HMAC_TX_BUF_BITMAP_LOG2_WORD_SIZE] |= (1 << ((_idx) & HMAC_TX_BUF_BITMAP_WORD_MASK)))

#define hmac_tx_buf_bitmap_clr(_bitmap, _idx) \
    (_bitmap[_idx >> HMAC_TX_BUF_BITMAP_LOG2_WORD_SIZE] &= ~((hi_u32)(1 << ((_idx) & HMAC_TX_BUF_BITMAP_WORD_MASK))))

#define HMAC_BA_BMP_SIZE 64

/* 判断index为n 在bitmap中的bit位是否是1 */
#define hmac_ba_bit_isset(_bm, _n) (((_n) < HMAC_BA_BMP_SIZE) && ((_bm)[(_n) >> 5] & (1 << ((_n) & 31))))

/* 判断一个seq num是否在发送窗口内 */
#define hmac_baw_within(_start, _bawsz, _seqno) ((((_seqno) - (_start)) & 4095) < (_bawsz))


typedef struct hmac_filter_serv_info_stru {
    oal_netbuf_head_stru *netbuf_header;
    hi_u8 *pen_is_ba_buf;
} hmac_filter_serv_info_stru;

/* ****************************************************************************
  3 函数声明
**************************************************************************** */
hi_u32 hmac_ba_filter_serv(const mac_vap_stru *mac_vap, const hmac_user_stru *hmac_user,
    const hmac_rx_ctl_stru *cb_ctrl, const mac_ieee80211_frame_stru *frame_hdr,
    const hmac_filter_serv_info_stru *filter_serv_info);

hi_u32 hmac_ba_timeout_fn(hi_void *arg);
hi_u8 hmac_mgmt_check_set_rx_ba_ok(const hmac_vap_stru *hmac_vap, hmac_ba_rx_stru *ba_rx_info,
    hmac_device_stru *hmac_dev);

hi_void hmac_reorder_ba_rx_buffer_bar(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, hi_u8 tidno,
    hi_u16 us_start_seq_num, hi_u8 *puc_sa_addr);
hi_void hmac_ba_reset_rx_handle(hmac_ba_rx_stru **rx_ba, hi_u8 tid);
hi_void hmac_ba_reset_tx_handle(hmac_ba_tx_stru **tx_ba);
hi_void hmac_up_rx_bar(hmac_vap_stru *hmac_vap, const hmac_rx_ctl_stru *rx_ctl);
hi_u8 hmac_is_device_ba_setup(hi_void);

hi_u32 hmac_create_ba_event(frw_event_mem_stru *event_mem);
hi_u32 hmac_del_ba_event(frw_event_mem_stru *event_mem);
hi_void hmac_ba_update_rx_bitmap(const hmac_user_stru *hmac_user, const mac_ieee80211_frame_stru *frame_hdr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of dmac_blockack.h */
