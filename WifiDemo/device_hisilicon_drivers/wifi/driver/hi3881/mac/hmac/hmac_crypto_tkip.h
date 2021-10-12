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

#ifndef __HMAC_CRYPTO_FRAME_H__
#define __HMAC_CRYPTO_FRAME_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "mac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define michael_block(l, r) do {                    \
        r ^= rotl32(l, 17); \
        l += r;             \
        r ^= xswap(l);      \
        l += r;             \
        r ^= rotl32(l, 3);  \
        l += r;             \
        r ^= rotr32(l, 2);  \
        l += r;             \
    } while (0)
#define IEEE80211_WEP_MICLEN 8 /* trailing MIC */

#define IEEE80211_FC1_DIR_NODS              0x00        /* STA->STA */
#define IEEE80211_FC1_DIR_TODS              0x01        /* STA->AP  */
#define IEEE80211_FC1_DIR_FROMDS            0x02        /* AP ->STA */
#define IEEE80211_FC1_DIR_DSTODS            0x03        /* AP ->AP  */

#define WEP_IV_FIELD_SIZE       4       /* wep IV field size */
#define EXT_IV_FIELD_SIZE       4       /* ext IV field size */
#define AUC_HDR_SIZE            16

typedef struct hmac_michael_mic_info_stru {
    hi_u32 offset;
    hi_u32 data_len;
    hi_u8  *puc_mic;
} hmac_michael_mic_info_stru;

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 hmac_crypto_tkip_enmic(wlan_priv_key_param_stru *key, oal_netbuf_stru *netbuf);
hi_u32 hmac_crypto_tkip_demic(wlan_priv_key_param_stru *key, oal_netbuf_stru *netbuf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_CRYPTO_FRAME_H__ */
