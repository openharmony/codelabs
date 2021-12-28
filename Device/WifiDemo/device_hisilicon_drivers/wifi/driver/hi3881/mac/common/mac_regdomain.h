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

#ifndef __MAC_REGDOMAIN_H__
#define __MAC_REGDOMAIN_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "wlan_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* 默认管制域最大发送功率 */
#define MAC_RC_DEFAULT_MAX_TX_PWR 20 /* 20dBm */
#define MAC_INVALID_RC 255           /* 无效的管制类索引值 */

#define MAC_AFFECTED_CHAN_OFFSET_START_FREQ_5 0
#define MAC_AFFECTED_CHAN_OFFSET_START_FREQ_2 3

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* 一个管制类的起始频率枚举 */
typedef enum {
    MAC_RC_START_FREQ_2 = WLAN_BAND_2G, /* 2.407 */
    MAC_RC_START_FREQ_BUTT,
} mac_rc_start_freq_enum;
typedef hi_u8 mac_rc_start_freq_enum_uint8;

/* 管制类信道间距 */
typedef enum {
    MAC_CH_SPACING_5MHZ = 0,
    MAC_CH_SPACING_10MHZ,
    MAC_CH_SPACING_20MHZ,
    MAC_CH_SPACING_25MHZ,
    MAC_CH_SPACING_40MHZ,

    MAC_CH_SPACING_BUTT
} mac_ch_spacing_enum;
typedef hi_u8 mac_ch_spacing_enum_uint8;

/* 雷达认证标准枚举 */
typedef enum {
    MAC_DFS_DOMAIN_NULL  = 0,
    MAC_DFS_DOMAIN_FCC   = 1,
    MAC_DFS_DOMAIN_ETSI  = 2,
    MAC_DFS_DOMAIN_MKK   = 3,
    MAC_DFS_DOMAIN_KOREA = 4,

    MAC_DFS_DOMAIN_BUTT
} mac_dfs_domain_enum;
typedef hi_u8 mac_dfs_domain_enum_uint8;

/* 5GHz频段: 信道号对应的信道索引值 */
typedef enum {
    MAC_CHANNEL36  = 0,
    MAC_CHANNEL40  = 1,
    MAC_CHANNEL44  = 2,
    MAC_CHANNEL48  = 3,
    MAC_CHANNEL52  = 4,
    MAC_CHANNEL56  = 5,
    MAC_CHANNEL60  = 6,
    MAC_CHANNEL64  = 7,
    MAC_CHANNEL100 = 8,
    MAC_CHANNEL104 = 9,
    MAC_CHANNEL108 = 10,
    MAC_CHANNEL112 = 11,
    MAC_CHANNEL116 = 12,
    MAC_CHANNEL120 = 13,
    MAC_CHANNEL124 = 14,
    MAC_CHANNEL128 = 15,
    MAC_CHANNEL132 = 16,
    MAC_CHANNEL136 = 17,
    MAC_CHANNEL140 = 18,
    MAC_CHANNEL144 = 19,
    MAC_CHANNEL149 = 20,
    MAC_CHANNEL153 = 21,
    MAC_CHANNEL157 = 22,
    MAC_CHANNEL161 = 23,
    MAC_CHANNEL165 = 24,
    MAC_CHANNEL184 = 25,
    MAC_CHANNEL188 = 26,
    MAC_CHANNEL192 = 27,
    MAC_CHANNEL196 = 28,
    MAC_CHANNEL_FREQ_5_BUTT = 29,
} mac_channel_freq_5_enum;
typedef hi_u8 mac_channel_freq_5_enum_uint8;

/* 2.4GHz频段: 信道号对应的信道索引值 */
typedef enum {
    MAC_CHANNEL1  = 0,
    MAC_CHANNEL2  = 1,
    MAC_CHANNEL3  = 2,
    MAC_CHANNEL4  = 3,
    MAC_CHANNEL5  = 4,
    MAC_CHANNEL6  = 5,
    MAC_CHANNEL7  = 6,
    MAC_CHANNEL8  = 7,
    MAC_CHANNEL9  = 8,
    MAC_CHANNEL10 = 9,
    MAC_CHANNEL11 = 10,
    MAC_CHANNEL12 = 11,
    MAC_CHANNEL13 = 12,
    MAC_CHANNEL14 = 13,
    MAC_CHANNEL_FREQ_2_BUTT = 14,
} mac_channel_freq_2_enum;
typedef hi_u8 mac_channel_freq_2_enum_uint8;

typedef enum {
    MAC_RC_DFS = BIT0,
} mac_behaviour_bmap_enum;

#define MAC_MAX_SUPP_CHANNEL MAC_CHANNEL_FREQ_2_BUTT

/* ****************************************************************************
  STRUCT定义
**************************************************************************** */
/* 管制类结构体: 每个管制类保存的信息 */
typedef struct {
    mac_rc_start_freq_enum_uint8    start_freq;          /* 起始频率 */
    mac_ch_spacing_enum_uint8       ch_spacing;          /* 信道间距 */
    hi_u8                       behaviour_bmap;      /* 允许的行为位图 位图定义见mac_behaviour_bmap_enum */
    hi_u8                       coverage_class;      /* 覆盖类 */
    hi_u8                       max_reg_tx_pwr;      /* 管制类规定的最大发送功率, 单位dBm */
    /* 实际使用的最大发送功率, 单位dBm，此值命令可配，可以比管制域规定功率大，TPC算法用此值作为最大发送功率 */
    hi_u8                       max_tx_pwr;
    hi_u8                       auc_resv[2];         /* 2 BYTE保留字段 */
    hi_u32                      channel_bmap;        /* 支持信道位图，例 0011表示支持的信道的index为0 1 */
} mac_regclass_info_stru;

/* 管制域信息结构体 */
/* 管制类值、管制类位图与管制类信息 数组下表的关系
    管制类取值        : .... 7  6  5  4  3  2  1  0
    管制类位图        : .... 1  1  0  1  1  1  0  1
    管制类信息数组下标: .... 5  4  x  3  2  1  x  0
*/
typedef struct {
    hi_char                   ac_country[WLAN_COUNTRY_STR_LEN];       /* 国家字符串 */
    hi_u8                     dfs_domain:4;                   /* DFS 雷达标准 mac_dfs_domain_enum_uint8 */
    hi_u8                     regclass_num:4;                 /* 管制类个数 */
    mac_regclass_info_stru    ast_regclass[WLAN_MAX_RC_NUM];  /* 管制域包含的管制类信息，注意 此成员只能放在最后一项! */
} mac_regdomain_info_stru;

/* channel info结构体 */
typedef struct {
    hi_u8 chan_number; /* 信道号 */
    hi_u8 reg_class;   /* 管制类在管制域中的索引号 */
} mac_channel_info_stru;

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_void mac_init_channel_list(hi_void);
hi_void mac_init_regdomain(hi_void);
hi_void mac_regdomain_set_max_power(hi_u8 pwr, hi_u8 exceed_reg);
hi_void mac_get_channel_num_from_idx(hi_u8 band, hi_u8 idx, hi_u8 *puc_channel_num);
hi_void mac_set_country_ie_2g(mac_regdomain_info_stru *rd_info, hi_u8 *puc_buffer, hi_u8 *puc_len);
hi_char *mac_regdomain_get_country(hi_void);
hi_u32 mac_is_channel_idx_valid(hi_u8 band, hi_u8 ch_idx, hi_u8 *reg_class);
hi_u32 mac_is_channel_num_valid(hi_u8 band, hi_u8 ch_num);
hi_u32 mac_get_channel_idx_from_num(hi_u8 band, hi_u8 channel_num, hi_u8 *puc_channel_idx);
mac_regdomain_info_stru *mac_get_regdomain_info(hi_void);
mac_regclass_info_stru *mac_get_channel_num_rc_info(hi_u8 band, hi_u8 ch_num);

/* ****************************************************************************
 功能描述  : 通过信道号找到频段，由调用者保证输入的信道号合法，从而降低本接口的复杂性，提高效率
 输入参数  : hi_u8 uc_channel_num
 返 回 值  : wlan_channel_band_enum_uint8
 修改历史      :
  1.日    期   : 2015年8月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline wlan_channel_band_enum_uint8 mac_get_band_by_channel_num(hi_u8 channel_num)
{
    return (wlan_channel_band_enum_uint8)((channel_num <= MAC_CHANNEL_FREQ_2_BUTT) ? WLAN_BAND_2G : WLAN_BAND_BUTT);
}

/* ****************************************************************************
 功能描述  : 获取当前频段所支持的最大信道数目
 输入参数  : en_band: 频段
 返 回 值  : 当前频段所支持的最大信道数目
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 mac_get_num_supp_channel(wlan_channel_band_enum_uint8 band)
{
    switch (band) {
        case WLAN_BAND_2G: /* 2.4GHz */
            return (hi_u8)MAC_CHANNEL_FREQ_2_BUTT;
        default:
            return 0;
    }
}

/* ****************************************************************************
 功能描述  : 获取受影响的信道偏移值
**************************************************************************** */
static inline hi_u8 mac_get_affected_ch_idx_offset(wlan_channel_band_enum_uint8 band)
{
    switch (band) {
        case WLAN_BAND_2G: /* 2.4GHz */
            return (hi_u8)MAC_AFFECTED_CHAN_OFFSET_START_FREQ_2;
        default:
            return 0;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MAC_REGDOMAIN_H__ */
