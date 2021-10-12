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
#include "oal_err_wifi.h"
#include "hmac_wapi_sms4.h"
#include "hmac_wapi_wpi.h"
#include "hmac_wapi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : ofb encrypt
 输入参数  : hi_u8 *puc_iv    为IV存储区起始地址
             hi_u8 *puc_bufin    为明文存储区起始地址
             hi_u32 ul_buflen    为明文（以Byte为单位）长度
             hi_u8 *puc_key      为会话密钥存储区起始地址
             hi_u8* puc_bufout   为密文存储区起始地址，
                                密文存储区空间与明文存储区空间大小相同
 修改历史      :
  1.日    期   : 2012年5月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数

  2.日    期   : 2015年5月28日
    作    者   : HiSilicon
    修改内容   : 移植
**************************************************************************** */
hi_u32 hmac_wpi_encrypt(hmac_wapi_crypt_stru wpi_key, hi_u8 *puc_bufin, hi_u32 buflen, hi_u8 *puc_bufout)
{
    hi_u32 aul_iv_out[4] = { 0 }; /* 元素个数为4 */
    hi_u32 *pul_in = HI_NULL;
    hi_u32 *pul_out = HI_NULL;
    hi_u8 *puc_out = HI_NULL;
    hi_u8 *puc_in = HI_NULL;
    hi_u32 counter;
    hi_u32 comp;
    hi_u32 loop;
    hi_u32 aul_pr_keyin[WPI_PR_KEYIN_LEN] = { 0 };
    hi_u8 *puc_iv = wpi_key.puc_iv;
    hi_u8 iv_len = wpi_key.iv_len;
    hi_u8 *puc_key = wpi_key.puc_key;
    hi_u8 key_len = wpi_key.key_len;

    if (buflen < 1) {
#ifdef WAPI_DEBUG_MODE
        g_stMacDriverStats.ulsms4ofbinparminvalid++;
#endif
        return HI_FAIL;
    }

    hmac_sms4_keyext(puc_key, key_len, aul_pr_keyin, WPI_PR_KEYIN_LEN);

    counter = buflen / 16; /* 16 用于计算 */
    comp = buflen % 16;    /* 16 用于计算 */

    /* get the iv */
    hmac_sms4_crypt(puc_iv, iv_len, (hi_u8 *)aul_iv_out, aul_pr_keyin, WPI_PR_KEYIN_LEN);
    pul_in = (hi_u32 *)puc_bufin;
    pul_out = (hi_u32 *)puc_bufout;

    for (loop = 0; loop < counter; loop++) {
        pul_out[0] = pul_in[0] ^ aul_iv_out[0];
        pul_out[1] = pul_in[1] ^ aul_iv_out[1];
        pul_out[2] = pul_in[2] ^ aul_iv_out[2]; /* 2 元素索引 */
        pul_out[3] = pul_in[3] ^ aul_iv_out[3]; /* 3 元素索引 */

        hmac_sms4_crypt((hi_u8 *)aul_iv_out, 4, (hi_u8 *)aul_iv_out, aul_pr_keyin, WPI_PR_KEYIN_LEN); /* iv_out len 4 */
        pul_in += 4;  /* 自增4 */
        pul_out += 4; /* 自增4 */
    }

    puc_in = (hi_u8 *)pul_in;
    puc_out = (hi_u8 *)pul_out;
    puc_iv = (hi_u8 *)aul_iv_out;

    for (loop = 0; loop < comp; loop++) {
        puc_out[loop] = puc_in[loop] ^ puc_iv[loop];
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : ofb decrypt
 输入参数  : hi_u8* puc_iv    为IV存储区起始地址
             hi_u8* puc_bufin    为密文存储区起始地址
             hi_u32 ul_buflen    为密文（以Byte为单位）长度
             hi_u8* puc_key      为会话密钥存储区起始地址
             hi_u8* puc_bufout   为明文存储区起始地址
 修改历史      :
  1.日    期   : 2012年5月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数

  2.日    期   : 2015年5月28日
    作    者   : HiSilicon
    修改内容   : 移植
**************************************************************************** */
hi_u32 hmac_wpi_decrypt(hmac_wapi_crypt_stru wpi_key, hi_u8 *puc_bufin, hi_u32 buflen, hi_u8 *puc_bufout)
{
    return hmac_wpi_encrypt(wpi_key, puc_bufin, buflen, puc_bufout);
}

/* ****************************************************************************
 功能描述  : 计算mic
 输入参数  : hi_u8* puc_iv    为IV存储区起始地址
             hi_u8* pucBuf      为text存储区起始地址
             hi_u32 ulPamclen   为text长度（以Byte为单位，且应为16Byte的整数倍）除以16的倍数
             hi_u8* pucKey      为用于计算MIC的密钥KEY存储区起始地址
             hi_u8* pucMic      为MIC存储区起始地址
 修改历史      :
  1.日    期   : 2012年5月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_wpi_pmac(hmac_wapi_crypt_stru wpi_key, hi_u8 *puc_buf, hi_u32 pamclen, hi_u8 *puc_mic, hi_u8 mic_len)
{
    hi_u32 aul_mic_tmp[4] = { 0 };  /* 元素个数为4 */
    hi_u32 loop;
    hi_u32 *pul_in = HI_NULL;
    hi_u32 aul_pr_mac_keyin[WPI_PR_KEYIN_LEN] = { 0 };
    hi_u8 *puc_iv = wpi_key.puc_iv;
    hi_u8  iv_len = wpi_key.iv_len;
    hi_u8 *puc_key = wpi_key.puc_key;
    hi_u8  key_len = wpi_key.key_len;

    if (mic_len < SMS4_MIC_LEN) {
        return HI_FAIL;
    }
    if ((pamclen < 1) || (pamclen > 4096)) { /* 4096 边界 */
        return HI_FAIL;
    }

    hmac_sms4_keyext(puc_key, key_len, aul_pr_mac_keyin, WPI_PR_KEYIN_LEN);
    pul_in = (hi_u32 *)puc_buf;
    hmac_sms4_crypt(puc_iv, iv_len, (hi_u8 *)aul_mic_tmp, aul_pr_mac_keyin, WPI_PR_KEYIN_LEN);

    for (loop = 0; loop < pamclen; loop++) {
        aul_mic_tmp[0] ^= pul_in[0];
        aul_mic_tmp[1] ^= pul_in[1];
        aul_mic_tmp[2] ^= pul_in[2]; /* 2 元素索引 */
        aul_mic_tmp[3] ^= pul_in[3]; /* 3 元素索引 */
        pul_in += 4;                 /* 自增4 */
        hmac_sms4_crypt((hi_u8 *)aul_mic_tmp, 4, (hi_u8 *)aul_mic_tmp, aul_pr_mac_keyin, WPI_PR_KEYIN_LEN); /* len 4 */
    }

    pul_in = (hi_u32 *)puc_mic;
    pul_in[0] = aul_mic_tmp[0];
    pul_in[1] = aul_mic_tmp[1];
    pul_in[2] = aul_mic_tmp[2]; /* 2 元素索引 */
    pul_in[3] = aul_mic_tmp[3]; /* 3 元素索引 */

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
