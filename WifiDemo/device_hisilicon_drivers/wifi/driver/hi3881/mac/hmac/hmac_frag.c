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
#include "frw_timer.h"
#include "hmac_frag.h"
#include "hmac_11i.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 报文分片处理
**************************************************************************** */
/* 规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 报文分片处理，功能内聚，建议屏蔽 */
static hi_u32 hmac_frag_process(oal_netbuf_stru *netbuf_original, hmac_tx_ctl_stru *tx_ctl, hi_u32 cip_hdrsize,
    hi_u32 max_tx_unit)
{
    mac_ieee80211_frame_stru *frag_header = HI_NULL;
    oal_netbuf_stru          *netbuf = HI_NULL;
    hi_u32                    mac_hdr_size;
    hi_u32                    offset;
    hi_s32                    l_remainder;

    /* 加密字节数包含在分片门限中，预留加密字节长度，由硬件填写加密头 */
    mac_hdr_size = tx_ctl->frame_header_length;
    offset = max_tx_unit - cip_hdrsize;
    l_remainder = (hi_s32)(oal_netbuf_len(netbuf_original) - offset);

    mac_ieee80211_frame_stru *mac_header = tx_ctl->frame_header;
    mac_header->frame_control.more_frag = HI_TRUE;

    hi_u32 total_hdrsize         = mac_hdr_size + cip_hdrsize;
    hi_u32 frag_num              = 1;
    oal_netbuf_stru *netbuf_prev = netbuf_original;

    do {
        hi_u32 frag_size = total_hdrsize + (hi_u32)l_remainder;

        /* 判断是否还有更多的分片 */
        frag_size = (frag_size > max_tx_unit) ? max_tx_unit : frag_size;

        netbuf = oal_netbuf_alloc(frag_size + MAC_80211_QOS_HTC_4ADDR_FRAME_LEN, MAC_80211_QOS_HTC_4ADDR_FRAME_LEN,
            4); /* align 4 */
        if (netbuf == HI_NULL) {
            /* 在外部释放之前申请的报文 */
            oam_error_log0(0, OAM_SF_ANY, "{hmac_frag_process::pst_netbuf null.}");
            return HI_ERR_CODE_PTR_NULL;
        }

        hmac_tx_ctl_stru *tx_ctl_copy = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
        /* 拷贝cb字段 */
        if (memcpy_s(tx_ctl_copy, sizeof(hmac_tx_ctl_stru), tx_ctl, sizeof(hmac_tx_ctl_stru)) != EOK) {
            oal_netbuf_free(netbuf);
            oam_error_log0(0, OAM_SF_CFG, "hmac_frag_process:: pst_tx_ctl memcpy_s fail.");
            return HI_FAIL;
        }

        /* netbuf的headroom大于802.11 mac头长度 */
        frag_header = (mac_ieee80211_frame_stru *)(oal_netbuf_payload(netbuf) - mac_hdr_size);
        tx_ctl_copy->mac_head_type = 1; /* 指示mac头部在skb中 */

        /* 拷贝帧头内容 */
        if (memcpy_s(frag_header, mac_hdr_size, mac_header, tx_ctl->frame_header_length) != EOK) {
            oal_netbuf_free(netbuf);
            oam_error_log0(0, OAM_SF_CFG, "hmac_frag_process:: pst_mac_header memcpy_s fail.");
            return HI_FAIL;
        }

        /* 赋值分片号 */
        frag_header->frag_num = frag_num;
        frag_num++;

        /* 计算分片报文帧体长度 */
        hi_u32 copy_offset = offset;

        hi_u32 ret = oal_netbuf_copydata(netbuf_original, copy_offset, oal_netbuf_payload(netbuf),
            (frag_size + MAC_80211_QOS_HTC_4ADDR_FRAME_LEN), (frag_size - total_hdrsize));
        if (ret != HI_SUCCESS) {
            oal_netbuf_free(netbuf);
            oam_error_log0(0, OAM_SF_CFG, "hmac_frag_process:: oal_netbuf_copydata return fail.");
            return ret;
        }

        oal_netbuf_set_len(netbuf, (frag_size - total_hdrsize));
        ((hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf))->frame_header = frag_header;
        ((hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf))->us_mpdu_len  = (hi_u16)(frag_size - total_hdrsize);
        set_oal_netbuf_next(netbuf_prev, netbuf);
        netbuf_prev = netbuf;

        oal_netbuf_push(netbuf, mac_hdr_size);

        /* 计算下一个分片报文的长度和偏移 */
        l_remainder -= (hi_s32)(frag_size - total_hdrsize);
        offset += (frag_size - total_hdrsize);
    } while (l_remainder > 0);

    frag_header->frame_control.more_frag = HI_FALSE;
    set_oal_netbuf_next(netbuf, HI_NULL);

    /* 原始报文作为分片报文的第一个 */
    oal_netbuf_trim(netbuf_original, oal_netbuf_len(netbuf_original) - (max_tx_unit - cip_hdrsize));

    tx_ctl->us_mpdu_len = (hi_u16)(oal_netbuf_len(netbuf_original));

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 报文分片处理
 修改历史      :
  1.日    期   : 2014年2月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_frag_process_proc(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf,
    hmac_tx_ctl_stru *tx_ctl)
{
    hi_u32 threshold;
    hi_u8  ic_header = 0;
    hi_u32 ret;
    hi_u32 last_frag;

    /* 获取分片门限 */
    threshold = hmac_vap->base_vap->mib_info->wlan_mib_operation.dot11_fragmentation_threshold;

    /* 调用加密接口在使用TKIP时对MSDU进行加密后在进行分片 */
    ret = hmac_en_mic(hmac_user, netbuf, &ic_header);
    if (ret != HI_SUCCESS) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ANY, "{hmac_frag_process_proc::hmac_en_mic failed[%d].}",
            ret);
        return ret;
    }
    /* D2手机ping不通问题,将门限值4*n+2 */
    threshold = (threshold & (~(BIT0 | BIT1))) + 2;

    /* 规避1151硬件bug,调整分片门限：TKIP加密时，当最后一个分片的payload长度小于等于8时，无法进行加密 */
    if (hmac_user->base_user->key_info.cipher_type == WLAN_80211_CIPHER_SUITE_TKIP) {
        last_frag = oal_netbuf_len(netbuf) % (threshold - (hi_u32)ic_header - tx_ctl->frame_header_length);
        if ((last_frag > 0) && (last_frag <= 8)) { /* 0:长度，8:长度 */
            threshold = threshold + 8;             /* 8:门限加8 */
            oam_info_log1(hmac_vap->base_vap->vap_id, OAM_SF_ANY,
                "{hmac_frag_process_proc::adjust the frag threshold to %d.}", threshold);
        }
    }
    /* 进行分片处理 */
    ret = hmac_frag_process(netbuf, tx_ctl, (hi_u32)ic_header, threshold);

    return ret;
}

/* ****************************************************************************
 功能描述  : 解分片超时处理
 修改历史      :
  1.日    期   : 2014年2月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_defrag_timeout_fn(hi_void *arg)
{
    hmac_user_stru *hmac_user = HI_NULL;
    oal_netbuf_stru *netbuf = HI_NULL;
    hmac_user = (hmac_user_stru *)arg;

    /* 超时后释放正在重组的分片报文 */
    if (hmac_user->defrag_netbuf != HI_NULL) {
        netbuf = hmac_user->defrag_netbuf;

        oal_netbuf_free(netbuf);
        hmac_user->defrag_netbuf = HI_NULL;
    }

    return HI_SUCCESS;
}

static hi_u32 hmac_defrag_process_frame(hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf, hi_u8 frag_num,
    const mac_ieee80211_frame_stru *mac_hdr, hi_u8 more_frag)
{
    /* 首片分片的分片号不为0则释放 */
    if (frag_num != 0) {
        oal_netbuf_free(netbuf);
        oam_info_log3(hmac_user->base_user->vap_id, OAM_SF_ANY,
            "{hmac_defrag_process:frag_num not Zero %d,seq_num %d,frag %d}", frag_num, mac_hdr->seq_num, more_frag);
        return HI_FAIL;
    }

    /* 启动超时定时器，超时释放重组报文 */
    frw_timer_create_timer(&hmac_user->defrag_timer, hmac_defrag_timeout_fn, HMAC_FRAG_TIMEOUT, hmac_user, HI_FALSE);
#ifdef _PRE_LWIP_ZERO_COPY
    oal_netbuf_stru *new_buf = oal_pbuf_netbuf_alloc(HMAC_MAX_FRAG_SIZE);
#else
    /* 内存池netbuf只有1600 可能不够，参照A公司申请2500操作系统原生态报文 */
    oal_netbuf_stru *new_buf = oal_netbuf_alloc(HMAC_MAX_FRAG_SIZE, 0, 4); /* align 4 */
#endif
    if (new_buf == HI_NULL) {
        oam_error_log0(hmac_user->base_user->vap_id, OAM_SF_ANY, "{hmac_defrag_process::Alloc new_buf null.}");
        oal_netbuf_free(netbuf);
        return HI_FAIL;
    }

    if (memcpy_s(oal_netbuf_cb(new_buf), oal_netbuf_cb_size(), oal_netbuf_cb(netbuf), oal_netbuf_cb_size()) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_defrag_process::mem safe function err!}");
        oal_netbuf_free(new_buf);
        oal_netbuf_free(netbuf);
        return HI_FAIL;
    }
    hmac_rx_ctl_stru *rx_ctl = (hmac_rx_ctl_stru *)oal_netbuf_cb(new_buf);
    hmac_user->defrag_netbuf = new_buf;

#ifdef _PRE_LWIP_ZERO_COPY
    oal_netbuf_reserve(new_buf, WLAN_MAX_MAC_HDR_LEN - rx_ctl->mac_header_len);
#endif

    /* 将分片报文拷贝到新申请的报文中并挂接到用户结构体下，释放原有的报文 */
    oal_netbuf_init(new_buf, oal_netbuf_len(netbuf));
    if (memcpy_s(oal_netbuf_data(new_buf), HMAC_MAX_FRAG_SIZE, oal_netbuf_data(netbuf), oal_netbuf_len(netbuf)) != EOK) {
        oam_error_log0(0, 0, "hmac_defrag_process_frame:: memcpy_s FAILED");
        oal_netbuf_free(hmac_user->defrag_netbuf);
        hmac_user->defrag_netbuf = HI_NULL;
        oal_netbuf_free(netbuf);
        return HI_FAIL;
    }
    rx_ctl->pul_mac_hdr_start_addr = (hi_u32 *)oal_netbuf_header(new_buf);
    oal_netbuf_free(netbuf);

    return HI_SUCCESS;
}

oal_netbuf_stru *hmac_get_defraged_netbuf(hmac_user_stru *hmac_user, mac_ieee80211_frame_stru *last_hdr)
{
    oal_netbuf_stru *netbuf = hmac_user->defrag_netbuf;
    /* 对重组好的报文进行mic检查 */
    if (hmac_de_mic(hmac_user, netbuf) != HI_SUCCESS) {
        oal_netbuf_free(netbuf);
        netbuf = HI_NULL;
        last_hdr = HI_NULL;
    }

    hmac_user->defrag_netbuf = HI_NULL;
    if (last_hdr == HI_NULL) {
        oam_error_log0(0, 0, "{get_defraged_netbuf::pst_last_hdr null.}");
        return HI_NULL;
    }

    last_hdr->frag_num = 0;
    frw_timer_immediate_destroy_timer(&hmac_user->defrag_timer);
    return netbuf;
}

/* ****************************************************************************
 功能描述  : 去分片处理
 修改历史      :
  1.日    期   : 2014年2月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
oal_netbuf_stru *hmac_defrag_process(hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf, hi_u32 hrdsize)
{
    mac_ieee80211_frame_stru *last_hdr = HI_NULL;

    mac_ieee80211_frame_stru *mac_hdr = (mac_ieee80211_frame_stru *)oal_netbuf_data(netbuf);
    hi_u8 more_frag = (hi_u8)mac_hdr->frame_control.more_frag;

    /* 如果没有什么可以去分片的则直接返回 */
    if (!more_frag && ((hi_u8)mac_hdr->frag_num == 0) && (hmac_user->defrag_netbuf == HI_NULL)) {
        return netbuf;
    }

    /* 首先检查到来的分片报文是不是属于正在重组的分片报文 */
    if (hmac_user->defrag_netbuf != HI_NULL) {
        frw_timer_restart_timer(&hmac_user->defrag_timer, HMAC_FRAG_TIMEOUT, HI_FALSE);
        last_hdr = (mac_ieee80211_frame_stru *)oal_netbuf_data(hmac_user->defrag_netbuf);
        /* 如果地址不匹配，序列号不匹配，分片号不匹配则释放现在正在重组的报文 */
        if (mac_hdr->seq_num != last_hdr->seq_num || (hi_u8)mac_hdr->frag_num != ((hi_u8)last_hdr->frag_num + 1) ||
            oal_compare_mac_addr(last_hdr->auc_address1, mac_hdr->auc_address1, WLAN_MAC_ADDR_LEN) ||
            oal_compare_mac_addr(last_hdr->auc_address2, mac_hdr->auc_address2, WLAN_MAC_ADDR_LEN)) {
            oal_netbuf_free(hmac_user->defrag_netbuf);
            frw_timer_immediate_destroy_timer(&hmac_user->defrag_timer);
            hmac_user->defrag_netbuf = HI_NULL;
        }
    }

    /* 判断到来的分片报文是否是第一个分片 */
    if (hmac_user->defrag_netbuf == HI_NULL) {
        if (hmac_defrag_process_frame(hmac_user, netbuf, (hi_u8)mac_hdr->frag_num, mac_hdr, more_frag) != HI_SUCCESS) {
            return HI_NULL;
        }
    } else {
        /* 此分片是期望的到来的分片，重启定时器，并进行重组 */
        frw_timer_restart_timer(&hmac_user->defrag_timer, HMAC_FRAG_TIMEOUT, HI_FALSE);
        oal_netbuf_pull(netbuf, hrdsize);
        /* 去分片失败释放当前分片报文 */
        if (oal_netbuf_concat(hmac_user->defrag_netbuf, netbuf) != HI_SUCCESS) {
            oal_netbuf_free(hmac_user->defrag_netbuf);
            oal_netbuf_free(netbuf);
            frw_timer_immediate_destroy_timer(&hmac_user->defrag_timer);
            hmac_user->defrag_netbuf = HI_NULL;
            return HI_NULL;
        }
        /* 记录最新分片报文的分片号 */
        last_hdr->seq_num   = mac_hdr->seq_num;
        last_hdr->frag_num  = mac_hdr->frag_num;
    }

    /* 判断是否重组完毕，存在更多报文返回空指针，重组完毕返回组好的报文 */
    if (more_frag) {
        return HI_NULL;
    }

    return hmac_get_defraged_netbuf(hmac_user, last_hdr);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
