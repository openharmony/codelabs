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
#include "oam_ext_if.h"
#include "mac_frame.h"
#include "dmac_ext_if.h"
#include "hmac_crypto_tkip.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数实现
**************************************************************************** */
static hi_u32 xswap(hi_u32 val)
{
    return ((val & 0x00ff00ff) << 8) | ((val & 0xff00ff00) >> 8); /* 8 左移右移位数 */
}

static hi_u32 get_le32_split(hi_u8 b0, hi_u8 b1, hi_u8 b2, hi_u8 b3)
{
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24); /* 8 16 24 移动位数 */
}

static hi_u32 get_le32(const hi_u8 *p, hi_u8 le_len)
{
    hi_unref_param(le_len);
    return get_le32_split(p[0], p[1], p[2], p[3]); /* 2 3 数组索引 */
}

static hi_void put_le32(hi_u8 *p, hi_u32 v)
{
    p[0] = (hi_u8)v;
    p[1] = (hi_u8)(v >> 8);  /* 右移8位 */
    p[2] = (hi_u8)(v >> 16); /* 2 元素索引 右移16位 */
    p[3] = (hi_u8)(v >> 24); /* 3 元素索引 右移24位 */
}

/* ****************************************************************************
 功能描述  : mic所需的源mac，目的mac，及TID信息构造
 输出参数  : hi_void
 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_crypto_tkip_michael_hdr(mac_ieee80211_frame_stru *frame_header, hi_u8 *auc_hdr, hi_u8 hdr_len)
{
    mac_ieee80211_frame_addr4_stru *frame_4addr_hdr = (mac_ieee80211_frame_addr4_stru *)frame_header;

    hi_u8 frame_dir = (frame_4addr_hdr->frame_control.to_ds) ? 1 : 0;
    frame_dir += (frame_4addr_hdr->frame_control.from_ds) ? 2 : 0; /* 自增2 */

    if (frame_dir == IEEE80211_FC1_DIR_NODS) {
        if ((memcpy_s(auc_hdr, hdr_len, frame_4addr_hdr->auc_address1, WLAN_MAC_ADDR_LEN) != EOK) ||
            (memcpy_s(auc_hdr + WLAN_MAC_ADDR_LEN, hdr_len - WLAN_MAC_ADDR_LEN, frame_4addr_hdr->auc_address2,
                      WLAN_MAC_ADDR_LEN) != EOK)) {
            goto addrerr;
        }
    } else if (frame_dir == IEEE80211_FC1_DIR_TODS) {
        if ((memcpy_s(auc_hdr, hdr_len, frame_4addr_hdr->auc_address3, WLAN_MAC_ADDR_LEN) != EOK) ||
            (memcpy_s(auc_hdr + WLAN_MAC_ADDR_LEN, hdr_len - WLAN_MAC_ADDR_LEN, frame_4addr_hdr->auc_address2,
                      WLAN_MAC_ADDR_LEN) != EOK)) {
            goto addrerr;
        }
    } else if (frame_dir == IEEE80211_FC1_DIR_FROMDS) {
        if ((memcpy_s(auc_hdr, hdr_len, frame_4addr_hdr->auc_address1, WLAN_MAC_ADDR_LEN) != EOK) ||
            (memcpy_s(auc_hdr + WLAN_MAC_ADDR_LEN, hdr_len - WLAN_MAC_ADDR_LEN, frame_4addr_hdr->auc_address3,
                      WLAN_MAC_ADDR_LEN) != EOK)) {
            goto addrerr;
        }
    } else {
        if ((memcpy_s(auc_hdr, hdr_len, frame_4addr_hdr->auc_address3, WLAN_MAC_ADDR_LEN) != EOK) ||
            (memcpy_s(auc_hdr + WLAN_MAC_ADDR_LEN, hdr_len - WLAN_MAC_ADDR_LEN, frame_4addr_hdr->auc_address4,
                      WLAN_MAC_ADDR_LEN) != EOK)) {
            goto addrerr;
        }
    }

    auc_hdr[12] = 0; /* 12 元素索引 */

    if (frame_4addr_hdr->frame_control.sub_type == WLAN_QOS_DATA) {
        if (frame_dir == IEEE80211_FC1_DIR_DSTODS) {
            auc_hdr[12] = ((mac_ieee80211_qos_frame_addr4_stru *)frame_4addr_hdr)->qc_tid; /* 12 元素索引 */
        } else {
            auc_hdr[12] = ((mac_ieee80211_qos_frame_stru *)frame_4addr_hdr)->qc_tid; /* 12 元素索引 */
        }
    }

    auc_hdr[13] = auc_hdr[14] = auc_hdr[15] = 0; /* 13 14 15 reserved */

    return;

addrerr:
    oam_error_log0(0, OAM_SF_CFG, "hmac_crypto_tkip_michael_hdr:: auc_address memcpy_s fail,check addr num.");
    return;
}

/* ****************************************************************************
 功能描述  : 生成mic校验码
 输入参数  : key 密钥(8byte)
 返 回 值  : 0:成功,其他:失败
 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* 规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 算法函数，功能内聚，建议屏蔽 */
static hi_u32 hmac_crypto_tkip_michael_mic(const hi_u8 *puc_key, const oal_netbuf_stru *netbuf,
    mac_ieee80211_frame_stru *frame_header, const hmac_michael_mic_info_stru *michael_mic_info)
{
    hi_u8  auc_hdr[AUC_HDR_SIZE] = {0};
    hi_u32 offset = michael_mic_info->offset;
    hi_u32 data_len = michael_mic_info->data_len;

    hmac_crypto_tkip_michael_hdr(frame_header, auc_hdr, AUC_HDR_SIZE);

    hi_u32 msb = get_le32(puc_key, 4);     /* 4 一次计算长度 */
    hi_u32 lsb = get_le32(puc_key + 4, 4); /* 4 偏移量 */

    /* Michael MIC pseudo header: DA, SA, 3 x 0, Priority */
    msb ^= get_le32(auc_hdr, 4); /* 4 一次计算长度 */
    michael_block(msb, lsb);
    msb ^= get_le32(&auc_hdr[4], 4); /* 4 元素索引 */
    michael_block(msb, lsb);
    msb ^= get_le32(&auc_hdr[8], 4); /* 8 元素索引, 4 一次计算长度 */
    michael_block(msb, lsb);
    msb ^= get_le32(&auc_hdr[12], 4); /* 12 元素索引, 4 一次计算长度 */
    michael_block(msb, lsb);

    /* first buffer has special handling */
    const hi_u8 *puc_data = oal_netbuf_data(netbuf) + offset;
    hi_u32 space = oal_netbuf_len(netbuf) - offset;

    for (;;) {
        space = (space > data_len) ? data_len : space;

        /* collect 32-bit blocks from current buffer */
        while (space >= sizeof(hi_u32)) {
            msb ^= get_le32(puc_data, 4); /* 4 一次计算长度 */
            michael_block(msb, lsb);

            puc_data += sizeof(hi_u32);
            space    -= sizeof(hi_u32);
            data_len -= sizeof(hi_u32);
        }

        if (data_len < sizeof(hi_u32)) {
            break;
        }

        netbuf = oal_netbuf_list_next(netbuf);
        if (netbuf == NULL) {
            return HI_ERR_CODE_SECURITY_BUFF_NUM;
        }

        if (space != 0) {
            /*
             * Block straddles buffers, split references.
             */
            const hi_u8 *puc_data_next = oal_netbuf_data(netbuf);
            if ((sizeof(hi_u32) - space) > oal_netbuf_len(netbuf)) {
                return HI_ERR_CODE_SECURITY_BUFF_LEN;
            }

            if (space == 1) {
                msb ^= get_le32_split(puc_data[0], puc_data_next[0], puc_data_next[1], puc_data_next[2]); /* 2 */
                puc_data = puc_data_next + 3;       /* 3 用于计算 */
                space = oal_netbuf_len(netbuf) - 3; /* 3 用于计算 */
            } else if (space == 2) {                /* 2 case 标志 */
                msb ^= get_le32_split(puc_data[0], puc_data[1], puc_data_next[0], puc_data_next[1]);
                puc_data = puc_data_next + 2;                                                   /* 2 用于计算 */
                space = oal_netbuf_len(netbuf) - 2;                                             /* 2 用于计算 */
            } else if (space == 3) {                                                            /* 3 case 标志 */
                msb ^= get_le32_split(puc_data[0], puc_data[1], puc_data[2], puc_data_next[0]); /* 2 元素索引 */
                puc_data = puc_data_next + 1;
                space = oal_netbuf_len(netbuf) - 1;
            }

            michael_block(msb, lsb);
            data_len -= sizeof(hi_u32);
        } else {
            /*
             * Setup for next buffer.
             */
            puc_data = oal_netbuf_data(netbuf);
            space = oal_netbuf_len(netbuf);
        }
    }

    /* Last block and padding (0x5a, 4..7 x 0) */
    if (data_len == 0) {
        msb ^= get_le32_split(0x5a, 0, 0, 0);
    } else if (data_len == 1) {
        msb ^= get_le32_split(puc_data[0], 0x5a, 0, 0);
    } else if (data_len == 2) { /* 2 case 标志 */
        msb ^= get_le32_split(puc_data[0], puc_data[1], 0x5a, 0);
    } else if (data_len == 3) {                                             /* 3 case 标志 */
        msb ^= get_le32_split(puc_data[0], puc_data[1], puc_data[2], 0x5a); /* 2 数组索引 */
    }

    michael_block(msb, lsb);
    /* l ^= 0; */
    michael_block(msb, lsb);

    put_le32(michael_mic_info->puc_mic, msb);
    put_le32(michael_mic_info->puc_mic + 4, lsb); /* 4 偏置项 */

    return HI_SUCCESS;
}

hi_u32 hmac_crypto_tkip_enmic_key(wlan_priv_key_param_stru *key, oal_netbuf_stru *netbuf, const hmac_tx_ctl_stru *cb,
    hi_u32 pktlen)
{
    hi_u8 auc_mic[IEEE80211_WEP_MICLEN] = { 0 };
    hmac_michael_mic_info_stru michael_mic_info;

    /* 3.1 获取mic及密钥 */
    hi_u8 *puc_mic_tail = (hi_u8 *)OAL_NETBUF_TAIL(netbuf);
    hi_u8 *puc_tx_mic_key = key->auc_key + WLAN_TEMPORAL_KEY_LENGTH;

    oal_netbuf_put(netbuf, IEEE80211_WEP_MICLEN);

    /* 4.1 计算mic */
    michael_mic_info.offset = cb->frame_header_length;
    michael_mic_info.data_len = pktlen - cb->frame_header_length;
    michael_mic_info.puc_mic = auc_mic;
    hi_u32 ret = hmac_crypto_tkip_michael_mic(puc_tx_mic_key, netbuf, cb->frame_header, &michael_mic_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* 4.1 拷贝mic到帧尾部 */
    if (memcpy_s(puc_mic_tail, IEEE80211_WEP_MICLEN, auc_mic, IEEE80211_WEP_MICLEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_crypto_tkip_enmic:: auc_mic memcpy_s fail.");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 添加tikip mic校验
 输出参数  : hi_u32
 返 回 值  : 0:成功,其他:失败
 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_crypto_tkip_enmic(wlan_priv_key_param_stru *key, oal_netbuf_stru *netbuf)
{
    if (((hi_u8)key->cipher) != WLAN_80211_CIPHER_SUITE_TKIP) {
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }

    /* 1.1 计算整个报文的长度，不处理存在多个netbuf的情况 */
    hi_u32 pktlen = oal_netbuf_len(netbuf);
    if (oal_netbuf_next(netbuf) != HI_NULL) {
        return HI_ERR_CODE_SECURITY_BUFF_NUM;
    }

    hmac_tx_ctl_stru *cb = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);

    /* 2.1 在netbuf上增加mic空间 */
    if (oal_netbuf_tailroom(netbuf) < IEEE80211_WEP_MICLEN) {
        /* 2.2 如果原来的netbuf长度不够，需要重新申请 */
#ifdef _PRE_LWIP_ZERO_COPY
        /* 零拷贝时,尾部空间不足需要往前扩展 */
        if ((netbuf->mem_head != HI_NULL) && (oal_netbuf_headroom(netbuf) > IEEE80211_WEP_MICLEN)) {
            hi_u8 *tmp_buff = oal_memalloc(pktlen);
            if (tmp_buff == HI_NULL) {
                return HI_ERR_CODE_PTR_NULL;
            }
            if (memcpy_s(tmp_buff, pktlen, oal_netbuf_data(netbuf), pktlen) != EOK) {
                oal_free(tmp_buff);
                return HI_FAIL;
            }
            netbuf->data -= IEEE80211_WEP_MICLEN;
            netbuf->tail -= IEEE80211_WEP_MICLEN;
            if (memcpy_s(oal_netbuf_data(netbuf), pktlen, tmp_buff, pktlen) != EOK) {
                oal_free(tmp_buff);
                return HI_FAIL;
            }
            oal_free(tmp_buff);
        } else {
#endif /* #ifdef _PRE_LWIP_ZERO_COPY */
            /* sbkbuff时，直接往后扩展 */
            oam_error_log0(0, 0, "hmac_crypto_tkip_enmic:: netbuf is exceptional!");
            netbuf = oal_netbuf_realloc_tailroom(netbuf, IEEE80211_WEP_MICLEN);
            if (netbuf == HI_NULL) {
                return HI_ERR_CODE_PTR_NULL;
            }
#ifdef _PRE_LWIP_ZERO_COPY
        }
#endif
        /* 扩展后MAC_HDR地址有变化，需要更新 */
        if (cb->mac_head_type == 1) {
            cb->frame_header = (mac_ieee80211_frame_stru *)oal_netbuf_header(netbuf);
        }
    }

    return hmac_crypto_tkip_enmic_key(key, netbuf, cb, pktlen);
}

/* ****************************************************************************
 功能描述  : 比较tkip mic校验，并剥掉mic尾
 输出参数  : hi_u32
 返 回 值  : 0:成功,其他:失败
 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_crypto_tkip_demic(wlan_priv_key_param_stru *key, oal_netbuf_stru *netbuf)
{
    hmac_rx_ctl_stru *cb = HI_NULL;
    hi_u32 hdrlen; /* 接收时，ul_pktlen里包含80211mac头 */
    hi_u8 *puc_rx_mic_key = HI_NULL;
    hi_u32 ret;
    hi_u32 pktlen;
    hi_u8 auc_mic[IEEE80211_WEP_MICLEN] = { 0 };
    hi_u8 auc_mic_peer[IEEE80211_WEP_MICLEN] = { 0 };
    hmac_michael_mic_info_stru michael_mic_info;

    if (WLAN_80211_CIPHER_SUITE_TKIP != ((hi_u8)key->cipher)) {
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }

    /* 1.1 计算整个报文的长度，不处理存在多个netbuf的情况 */
    pktlen = oal_netbuf_len(netbuf);
    if (HI_NULL != oal_netbuf_list_next(netbuf)) {
        return HI_ERR_CODE_SECURITY_BUFF_NUM;
    }

    /* 2.1 从CB中获取80211头长度 */
    cb = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    hdrlen = cb->mac_header_len;

    /* 3.1 只处理数据帧 */
    /* 4.1 获取解密密钥，接收密钥需要偏移8个字节 */
    puc_rx_mic_key = key->auc_key + WLAN_TEMPORAL_KEY_LENGTH + WLAN_MIC_KEY_LENGTH;

    /* 5.1 计算mic */
    michael_mic_info.offset = hdrlen;
    michael_mic_info.data_len = pktlen - (hdrlen + IEEE80211_WEP_MICLEN);
    michael_mic_info.puc_mic = auc_mic;
    ret = hmac_crypto_tkip_michael_mic(puc_rx_mic_key, netbuf,
        (mac_ieee80211_frame_stru *)cb->pul_mac_hdr_start_addr, &michael_mic_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* 6.1 获取对端的mic并跟本地计算的mic进行比较 */
    ret = oal_netbuf_copydata(netbuf, pktlen - IEEE80211_WEP_MICLEN, (hi_void *)auc_mic_peer, IEEE80211_WEP_MICLEN,
        IEEE80211_WEP_MICLEN);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (memcmp(auc_mic, auc_mic_peer, IEEE80211_WEP_MICLEN)) {
        return HI_ERR_CODE_SECURITY_WRONG_KEY;
    }

    /* 7.1 去掉mic尾部 */
    oal_netbuf_trim(netbuf, IEEE80211_WEP_MICLEN);
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
