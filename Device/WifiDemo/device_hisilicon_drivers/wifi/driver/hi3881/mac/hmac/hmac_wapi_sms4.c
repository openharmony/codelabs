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
#include "hmac_wapi_sms4.h"
#include "oam_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
static hi_u32 g_gaul_sbox[256] = { /* 256:数组长度 */
    0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
    0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3, 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
    0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
    0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
    0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba, 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
    0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
    0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
    0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52, 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
    0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
    0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
    0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60, 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
    0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
    0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
    0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd, 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
    0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
    0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
};

static hi_u32 g_gaul_ck[32] = { /* 32:数组长度 */
    0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
    0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
    0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
    0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
    0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
    0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
    0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
    0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
};

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : hmac_sms4_crypt
 功能描述  : SMS4的加解密函数
 输入参数  : [1]puc_Input 为输入信息分组
             [2]puc_Output 为输出分组
             [3]pul_rk 为轮密钥
 输出参数  : 无
 返 回 值  : 无
 修改历史      :
  1.日    期   : 2012年5月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

  1.日    期   : 2015年5月20日
    作    者   : HiSilicon
    修改内容   : 从1101移植到1102
**************************************************************************** */
hi_void hmac_sms4_crypt(const hi_u8 *puc_input, hi_u8 input_len, hi_u8 *puc_output, const hi_u32 *puc_rk, hi_u8 rk_len)
{
    hi_u32 x[4] = {0}; /* 4:长度 */

    if (puc_input != HI_NULL) {
        if (memcpy_s(x, sizeof(x), puc_input, input_len) != EOK) { /* 16:大小 */
            oam_error_log0(0, OAM_SF_CFG, "hmac_sms4_crypt:: puc_input memcpy_s fail.");
            return;
        }
    }
#if (_PRE_LITTLE_CPU_ENDIAN == _PRE_CPU_ENDIAN)
    x[0] = rotl32(x[0], 16);                                        /* 0:下标，16:位移位数 */
    x[0] = ((x[0] & 0x00FF00FF) << 8) ^ ((x[0] & 0xFF00FF00) >> 8); /* 0:下标，8:位移位数 */
    x[1] = rotl32(x[1], 16);                                        /* 1:下标，16:位移位数 */
    x[1] = ((x[1] & 0x00FF00FF) << 8) ^ ((x[1] & 0xFF00FF00) >> 8); /* 1:下标，8:位移位数 */
    x[2] = rotl32(x[2], 16);                                        /* 2:下标，16:位移位数 */
    x[2] = ((x[2] & 0x00FF00FF) << 8) ^ ((x[2] & 0xFF00FF00) >> 8); /* 2:下标，8:位移位数 */
    x[3] = rotl32(x[3], 16);                                        /* 3:下标，16:位移位数 */
    x[3] = ((x[3] & 0x00FF00FF) << 8) ^ ((x[3] & 0xFF00FF00) >> 8); /* 3:下标，8:位移位数 */
#endif

    for (hi_u32 r = 0; r < rk_len; r += 4) {             /* 0:起始，32:条件，4:递增 */
        hi_u32 mid = x[1] ^ x[2] ^ x[3] ^ puc_rk[r + 0]; /* 0:下标，1:下标，2:下标，3:下标 */
        mid = byte_sub(g_gaul_sbox, mid);
        x[0] ^= l1(mid);                          /* 0:下标 */
        mid = x[2] ^ x[3] ^ x[0] ^ puc_rk[r + 1]; /* 0:下标，1:下标，2:下标，3:下标 */
        mid = byte_sub(g_gaul_sbox, mid);
        x[1] ^= l1(mid);                          /* 1:下标 */
        mid = x[3] ^ x[0] ^ x[1] ^ puc_rk[r + 2]; /* 0:下标，1:下标，2:下标，3:下标 */
        mid = byte_sub(g_gaul_sbox, mid);
        x[2] ^= l1(mid);                          /* 2:下标 */
        mid = x[0] ^ x[1] ^ x[2] ^ puc_rk[r + 3]; /* 0:下标，1:下标，2:下标，3:下标 */
        mid = byte_sub(g_gaul_sbox, mid);
        x[3] ^= l1(mid); /* 3:下标 */
    }
#if (_PRE_LITTLE_CPU_ENDIAN == _PRE_CPU_ENDIAN)
    x[0] = rotl32(x[0], 16);                                        /* 0:下标，16:位移位数 */
    x[0] = ((x[0] & 0x00FF00FF) << 8) ^ ((x[0] & 0xFF00FF00) >> 8); /* 0:下标，8:位移位数 */
    x[1] = rotl32(x[1], 16);                                        /* 1:下标，16:位移位数 */
    x[1] = ((x[1] & 0x00FF00FF) << 8) ^ ((x[1] & 0xFF00FF00) >> 8); /* 1:下标，8:位移位数 */
    x[2] = rotl32(x[2], 16);                                        /* 2:下标，16:位移位数 */
    x[2] = ((x[2] & 0x00FF00FF) << 8) ^ ((x[2] & 0xFF00FF00) >> 8); /* 2:下标，8:位移位数 */
    x[3] = rotl32(x[3], 16);                                        /* 3:下标，16:位移位数 */
    x[3] = ((x[3] & 0x00FF00FF) << 8) ^ ((x[3] & 0xFF00FF00) >> 8); /* 3:下标，8:位移位数 */
#endif
    hi_u32 *p = (hi_u32 *)puc_output;
    p[0] = x[3]; /* 0:下标，3:下标 */
    p[1] = x[2]; /* 1:下标，2:下标 */
    p[2] = x[1]; /* 2:下标，1:下标 */
    p[3] = x[0]; /* 3:下标，0:下标 */
}

/* ****************************************************************************
 函 数 名  : hmac_sms4_keyext
 功能描述  : SMS4的密钥扩展算法
 输入参数  : [1]puc_key
             [2]pul_rk
             [3]ul_crypt_flag
 参数说明  ：puc_key为加密密钥，pul_rk为子密钥，ul_crypt_flag为加解密标志
 输出参数  : 无
 返 回 值  : hi_void
**************************************************************************** */
hi_void hmac_sms4_keyext(const hi_u8 *puc_key, hi_u8 key_len, hi_u32 *puc_rk, hi_u8 rk_len)
{
    hi_u32 r;
    hi_u32 mid;
    hi_u32 x[4] = { 0 }; /* 4:下标，0:赋值 */
    if (puc_key != HI_NULL) {
        if (memcpy_s(x, sizeof(x), puc_key, key_len) != EOK) { /* 16:大小 */
            oam_error_log0(0, OAM_SF_CFG, "hmac_sms4_keyext:: puc_Key memcpy_s fail.");
            return;
        } /* 注意输入参数地址对齐问题 */
    }
#if (_PRE_LITTLE_CPU_ENDIAN == _PRE_CPU_ENDIAN)
    x[0] = rotl32(x[0], 16);                                      /* 0:下标，16:位移位数 */
    x[0] = ((x[0] & 0xFF00FF) << 8) ^ ((x[0] & 0xFF00FF00) >> 8); /* 0:下标，8:位移位数 */
    x[1] = rotl32(x[1], 16);                                      /* 1:下标，16:位移位数 */
    x[1] = ((x[1] & 0xFF00FF) << 8) ^ ((x[1] & 0xFF00FF00) >> 8); /* 1:下标，8:位移位数 */
    x[2] = rotl32(x[2], 16);                                      /* 2:下标，16:位移位数 */
    x[2] = ((x[2] & 0xFF00FF) << 8) ^ ((x[2] & 0xFF00FF00) >> 8); /* 2:下标，8:位移位数 */
    x[3] = rotl32(x[3], 16);                                      /* 3:下标，16:位移位数 */
    x[3] = ((x[3] & 0xFF00FF) << 8) ^ ((x[3] & 0xFF00FF00) >> 8); /* 3:下标，8:位移位数 */
#endif
    x[0] ^= 0xa3b1bac6; /* 0:下标 */
    x[1] ^= 0x56aa3350; /* 1:下标 */
    x[2] ^= 0x677d9197; /* 2:下标 */
    x[3] ^= 0xb27022dc; /* 3:下标 */

    for (r = 0; r < rk_len; r += 4) {                /* 0:起始，32:条件，4:递增 */
        mid = x[1] ^ x[2] ^ x[3] ^ g_gaul_ck[r + 0]; /* 0:下标，1:下标，2:下标，3:下标 */
        mid = byte_sub(g_gaul_sbox, mid);
        puc_rk[r + 0] = x[0] ^= l2(mid);             /* 0:下标 */
        mid = x[2] ^ x[3] ^ x[0] ^ g_gaul_ck[r + 1]; /* 0:下标，1:下标，2:下标，3:下标 */
        mid = byte_sub(g_gaul_sbox, mid);
        puc_rk[r + 1] = x[1] ^= l2(mid);             /* 1:下标 */
        mid = x[3] ^ x[0] ^ x[1] ^ g_gaul_ck[r + 2]; /* 0:下标，1:下标，2:下标，3:下标 */
        mid = byte_sub(g_gaul_sbox, mid);
        puc_rk[r + 2] = x[2] ^= l2(mid);             /* 2:下标 */
        mid = x[0] ^ x[1] ^ x[2] ^ g_gaul_ck[r + 3]; /* 0:下标，1:下标，2:下标，3:下标 */
        mid = byte_sub(g_gaul_sbox, mid);
        puc_rk[r + 3] = x[3] ^= l2(mid); /* 3:下标 */
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
