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

#ifndef __HMAC_WAPI_WPI_H__
#define __HMAC_WAPI_WPI_H__

/* ****************************************************************************
  1 头文件包含
**************************************************************************** */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WPI_PR_KEYIN_LEN 32

typedef struct {
    hi_u8 *puc_key;
    hi_u8 *puc_iv;
    hi_u8  key_len;
    hi_u8  iv_len;
    hi_u8  resv[2]; /* resv 2byte */
} hmac_wapi_crypt_stru;

/* ****************************************************************************
  2 函数声明
**************************************************************************** */
hi_u32 hmac_wpi_encrypt(hmac_wapi_crypt_stru wpi_key, hi_u8 *puc_bufin, hi_u32 buflen, hi_u8 *puc_bufout);
hi_u32 hmac_wpi_decrypt(hmac_wapi_crypt_stru wpi_key, hi_u8 *puc_bufin, hi_u32 buflen, hi_u8 *puc_bufout);
hi_u32 hmac_wpi_pmac(hmac_wapi_crypt_stru wpi_key, hi_u8 *puc_buf, hi_u32 pamclen, hi_u8 *puc_mic, hi_u8 mic_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_WAPI_WPI_H__ */
