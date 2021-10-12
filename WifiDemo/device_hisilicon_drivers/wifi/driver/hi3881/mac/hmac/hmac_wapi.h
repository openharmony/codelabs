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

#ifndef __HMAC_WAPI_H__
#define __HMAC_WAPI_H__
/* ****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "oal_net.h"
#include "mac_resource.h"
#include "hmac_vap.h"
#include "hmac_user.h"
#include "hmac_wapi_wpi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define WAPI_UCAST_INC                  2       /* 发送或者接收单播帧,pn的步进值 */
#define WAPI_BCAST_INC                  1       /* 发送或者接收组播帧,pn的步进值 */
#define WAPI_WAI_TYPE                  (hi_u16)0x88B4 /* wapi的以太类型 */

#define WAPI_BCAST_KEY_TYPE             1
#define WAPI_UCAST_KEY_TYPE             0

#define SMS4_MIC_LEN                    (hi_u8)16     /* SMS4封包MIC的长度 */

#define SMS4_PN_LEN                     16     /* wapi pn的长度 */
#define SMS4_KEY_IDX                    1      /* WAPI头中 keyidx占1个字节 */
#define SMS4_WAPI_HDR_RESERVE           1      /* WAPI头中保留字段 */
#define HMAC_WAPI_HDR_LEN               (hi_u8)(SMS4_PN_LEN + SMS4_KEY_IDX + SMS4_WAPI_HDR_RESERVE)
#define WAPI_PDU_LEN                    2      /* wapi头中，wapi pdu len字段所占字节数 */
#define SMS4_PADDING_LEN                16     /* mic data按照16字节对齐 */

#define SMS4_MIC_PART1_QOS_LEN          48 /* 按照协议，如果有qos字段，mic第一部分16字节对齐后的长度 */
#define SMS4_MIC_PART1_NO_QOS_LEN       32 /* 按照协议，如果没有qos字段，mic第一部分16字节对齐后的长度 */

#define WAPI_IE_VERSION                     1   /* wapi的version */
#define WAPI_IE_VER_SIZE                    2   /* wapi ver-ie 所占字节数 */
#define WAPI_IE_SUIT_TYPE_SIZE              1   /* suit type size */
#define WAPI_IE_WAPICAP_SIZE                2   /* wapi cap字段所占字节数 */
#define WAPI_IE_BKIDCNT_SIZE                2   /* wapi bkid数字段所占字节数 */
#define WAPI_IE_BKID_SIZE                   16  /* 一个bkid所占字节数 */
#define WAPI_IE_OUI_SIZE                    3   /* wapi oui字节数 */
#define WAPI_IE_SMS4                        1   /* wapi加密类型为sms4 */
#define WAPI_IE_SUITCNT_SIZE                2   /* wapi suit count所占字节数 */
/* wapi key len */
#define WAPI_MIC_SEQ_CONTROL_LEN            2

#define wapi_is_port_valid(wapi) ((wapi)->port_valid)

#define wapi_is_work(pst_hmac_vap)   ((pst_hmac_vap)->wapi)
#define padding(x, size)           (((x) + (size) - 1) & (~ ((size) - 1)))

#ifdef _PRE_WAPI_DEBUG
#define wapi_tx_drop_inc(pst_wapi)              pst_wapi->debug.ultx_ucast_drop++
#define wapi_tx_wai_inc(pst_wapi)               pst_wapi->debug.ultx_wai++
#define wapi_tx_port_valid(pst_wapi)            pst_wapi->debug.ultx_port_valid++
#define wapi_rx_port_valid(wapi)            wapi->debug.ulrx_port_valid++
#define wapi_rx_idx_err(wapi)               wapi->debug.ulrx_idx_err++
#define wapi_rx_netbuf_len_err(wapi)        wapi->debug.ulrx_netbuff_len_err++
#define wapi_rx_idx_update_err(wapi)        wapi->debug.ulrx_idx_update_err++
#define wapi_rx_key_en_err(pst_wapi)            pst_wapi->debug.ulrx_key_en_err++
#define wapi_rx_memalloc_err(wapi)          wapi->debug.ulrx_memalloc_err++
#define wapi_rx_mic_err(wapi)               wapi->debug.ulrx_mic_calc_fail++
#define wapi_rx_decrypt_ok(wapi)            wapi->debug.ulrx_decrypt_ok++
#define wapi_tx_memalloc_err(wapi)          wapi->debug.ultx_memalloc_err++
#define wapi_tx_mic_err(wapi)               wapi->debug.ultx_mic_calc_fail++
#define wapi_tx_encrypt_ok(wapi)            wapi->debug.ultx_encrypt_ok++
#else
#define wapi_tx_drop_inc(pst_wapi)
#define wapi_tx_wai_inc(pst_wapi)
#define wapi_tx_port_valid(pst_wapi)
#define wapi_rx_port_valid(pst_wapi)
#define wapi_rx_idx_err(pst_wapi)
#define wapi_rx_netbuf_len_err(pst_wapi)
#define wapi_rx_idx_update_err(pst_wapi)
#define wapi_rx_key_en_err(pst_wapi)
#define wapi_rx_memalloc_err(pst_wapi)
#define wapi_rx_mic_err(pst_wapi)
#define wapi_rx_decrypt_ok(pst_wapi)
#define wapi_tx_memalloc_err(pst_wapi)
#define wapi_tx_mic_err(pst_wapi)
#define wapi_tx_wai_drop_inc(pst_wapi)
#define wapi_tx_encrypt_ok(pst_wapi)
#endif /* #ifdef WAPI_DEBUG_MODE */

/* ****************************************************************************
  3 STRUCT定义
**************************************************************************** */
typedef struct {
    hi_u8 auc_framectl[2];               /* 帧控制 2: 元素个数 */
    hi_u8 auc_adress1[OAL_MAC_ADDR_LEN]; /* 地址1 */
    hi_u8 auc_adress2[OAL_MAC_ADDR_LEN]; /* 地址2 */
    hi_u8 auc_seqctl[2];                 /* 序列控制 2: 元素个数 */
    hi_u8 auc_adress3[OAL_MAC_ADDR_LEN]; /* 地址3 */
    hi_u8 auc_adress4[OAL_MAC_ADDR_LEN]; /* 地址4 */
} wapi_mic_hdr_stru;

typedef struct {
    hi_u16 us_mic_len;
    hi_u16 pdu_len;
    hi_u8  auc_calc_mic[SMS4_MIC_LEN];
    hi_u8  auc_pn_swap[SMS4_PN_LEN];      /* 保存变换后的pn,用来计算mic和加密 */
    hmac_wapi_crypt_stru wpi_key_ck;
    hmac_wapi_crypt_stru wpi_key_ek;
    hi_u8 key_index;
    hi_u8 mac_hdr_len;
} hmac_wapi_encrypt_stru;

typedef struct {
    hi_u8 *puc_mic;
    hi_u16 us_mic_len;
    hi_u8 rsv[2]; /* 2 字节补齐 */
} mic_date_stru;

/* ****************************************************************************
  4 函数声明
**************************************************************************** */
hi_u32 hmac_wapi_deinit(hmac_wapi_stru *wapi);
hi_u32 hmac_wapi_init(hmac_wapi_stru *wapi, hi_u8 pairwise);
#ifdef _PRE_WAPI_DEBUG
hi_u32 hmac_wapi_display_info(mac_vap_stru *mac_vap, hi_u16 us_usr_idx);
#endif /* #ifdef _PRE_DEBUG_MODE */
hi_u32 hmac_wapi_add_key(hmac_wapi_stru *wapi, hi_u8 key_index, const hi_u8 *puc_key);

/* ****************************************************************************
 功能描述  : 将port重置
 修改历史      :
  1.日    期   : 2015年5月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void hmac_wapi_reset_port(hmac_wapi_stru *wapi)
{
    wapi->port_valid = HI_FALSE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_WAPI_H__ */
