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
#include "mac_regdomain.h"
#include "mac_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* 管制域信息全局变量 */
mac_regdomain_info_stru g_mac_regdomain;

mac_channel_info_stru g_ast_channel_list_2g[MAC_CHANNEL_FREQ_2_BUTT] = {
    {1,  MAC_INVALID_RC}, {2,  MAC_INVALID_RC}, {3,  MAC_INVALID_RC},
    {4,  MAC_INVALID_RC}, {5,  MAC_INVALID_RC}, {6,  MAC_INVALID_RC},
    {7,  MAC_INVALID_RC}, {8,  MAC_INVALID_RC}, {9,  MAC_INVALID_RC},
    {10, MAC_INVALID_RC}, {11, MAC_INVALID_RC}, {12, MAC_INVALID_RC},
    {13, MAC_INVALID_RC}, {14, MAC_INVALID_RC},
};

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 获取管制域信息
 修改历史      :
  1.日    期   : 2013年9月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
mac_regdomain_info_stru *mac_get_regdomain_info(hi_void)
{
    return &g_mac_regdomain;
}

/* ****************************************************************************
 功能描述  : 初始化默认管制域2.4G信息
 修改历史      :
  1.日    期   : 2019年7月28日
    作    者   : HiSilicon
**************************************************************************** */
hi_void mac_init_regdomain_2g(hi_void)
{
    mac_regclass_info_stru *regclass = HI_NULL;

    regclass = &(g_mac_regdomain.ast_regclass[0]);
    regclass->start_freq = MAC_RC_START_FREQ_2;
    regclass->ch_spacing = MAC_CH_SPACING_5MHZ;
    regclass->behaviour_bmap = 0;
    regclass->coverage_class = 0;
    regclass->max_reg_tx_pwr = MAC_RC_DEFAULT_MAX_TX_PWR;
    regclass->max_tx_pwr     = MAC_RC_DEFAULT_MAX_TX_PWR;
    regclass->channel_bmap = bit(MAC_CHANNEL1) |
                                    bit(MAC_CHANNEL2) |
                                    bit(MAC_CHANNEL3) |
                                    bit(MAC_CHANNEL4) |
                                    bit(MAC_CHANNEL5) |
                                    bit(MAC_CHANNEL6) |
                                    bit(MAC_CHANNEL7) |
                                    bit(MAC_CHANNEL8) |
                                    bit(MAC_CHANNEL9) |
                                    bit(MAC_CHANNEL10) |
                                    bit(MAC_CHANNEL11) |
                                    bit(MAC_CHANNEL12) |
                                    bit(MAC_CHANNEL13);
}

/* ****************************************************************************
 功能描述  : 初始化默认管制域信息
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_init_regdomain(hi_void)
{
    hi_char ac_default_country[WLAN_COUNTRY_STR_LEN] = "99";

    if (memcpy_s(g_mac_regdomain.ac_country, WLAN_COUNTRY_STR_LEN, ac_default_country, WLAN_COUNTRY_STR_LEN) != EOK) {
        return;
    }
    /* 初始默认的管制类个数为1 */
    g_mac_regdomain.regclass_num = 1;

    /* ************************************************************************
        初始化管制类1 2.4G
    ************************************************************************ */
    mac_init_regdomain_2g();
}

/* ****************************************************************************
 功能描述  : 依据管制域初始化信道列表
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_init_channel_list(hi_void)
{
    hi_u8 ch_idx;
    hi_u8 rc_num;
    hi_u8 rc_idx;
    mac_regdomain_info_stru *rd_info = HI_NULL;
    mac_regclass_info_stru  *rc_info = HI_NULL;

    rd_info = &g_mac_regdomain;
    /* 先初始化所有信道的管制类为无效 */
    for (ch_idx = 0; ch_idx < MAC_CHANNEL_FREQ_2_BUTT; ch_idx++) {
        g_ast_channel_list_2g[ch_idx].reg_class = MAC_INVALID_RC;
    }

    /* 然后根据管制域更新信道的管制类信息 */
    rc_num = rd_info->regclass_num;
    /* 更新2G频段上信道的管制类信息 */
    for (rc_idx = 0; rc_idx < rc_num; rc_idx++) {
        rc_info = &(rd_info->ast_regclass[rc_idx]);
        if (rc_info->start_freq != MAC_RC_START_FREQ_2) {
            continue;
        }
        for (ch_idx = 0; ch_idx < MAC_CHANNEL_FREQ_2_BUTT; ch_idx++) {
            if (rc_info->channel_bmap & bit(ch_idx)) {
                g_ast_channel_list_2g[ch_idx].reg_class = rc_idx;
            }
        }
    }
}

/* ****************************************************************************
 功能描述  : 获取1个信道索引的管制类信息
 输入参数  : uc_band: 频段，0-2.4G, 1-5G
             uc_ch_idx: 信道索引号
 修改历史      :
  1.日    期   : 2013年10月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
mac_regclass_info_stru *mac_get_channel_idx_rc_info(hi_u8 band, hi_u8 ch_idx)
{
    hi_u8 reg_class;

    if (mac_is_channel_idx_valid(band, ch_idx, &reg_class) != HI_SUCCESS) {
        return HI_NULL;
    }

    return &(g_mac_regdomain.ast_regclass[reg_class]);
}

/* ****************************************************************************
 功能描述  : 由信道索引值返回信道号
 修改历史      :
  1.日    期   : 2013年4月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_get_channel_num_from_idx(hi_u8 band, hi_u8 idx, hi_u8 *puc_channel_num)
{
    if (band == MAC_RC_START_FREQ_2) {
        if (idx >= MAC_CHANNEL_FREQ_2_BUTT) {
            return;
        }
        *puc_channel_num = g_ast_channel_list_2g[idx].chan_number;
    }
}

/* ****************************************************************************
 功能描述  : 通过信道号找到信道索引号
 修改历史      :
  1.日    期   : 2013年7月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 mac_get_channel_idx_from_num(hi_u8 band, hi_u8 channel_num, hi_u8 *puc_channel_idx)
{
    mac_channel_info_stru *channel = HI_NULL;
    hi_u8 total_channel_num = 0;
    hi_u8 idx;

    /* 根据频段获取信道信息 */
    switch (band) {
        case MAC_RC_START_FREQ_2:
            channel = g_ast_channel_list_2g;
            total_channel_num = (hi_u8)MAC_CHANNEL_FREQ_2_BUTT;
            break;
        default:
            return HI_ERR_CODE_INVALID_CONFIG;
    }
    /* 检查信道索引号 */
    for (idx = 0; idx < total_channel_num; idx++) {
        if (channel[idx].chan_number == channel_num) {
            *puc_channel_idx = idx;
            return HI_SUCCESS;
        }
    }
    return HI_ERR_CODE_INVALID_CONFIG;
}

/* ****************************************************************************
 功能描述  : 根据管制域，判断信道索引号是否有效
 修改历史      :
  1.日    期   : 2013年9月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 mac_is_channel_idx_valid(hi_u8 band, hi_u8 ch_idx, hi_u8 *reg_class)
{
    hi_u8 max_ch_idx;
    mac_channel_info_stru *ch_info = HI_NULL;

    switch (band) {
        case MAC_RC_START_FREQ_2:
            max_ch_idx = MAC_CHANNEL_FREQ_2_BUTT;
            ch_info = &(g_ast_channel_list_2g[ch_idx]);
            break;
        default:
            return HI_ERR_CODE_INVALID_CONFIG;
    }

    if (ch_idx >= max_ch_idx) {
        return HI_ERR_CODE_ARRAY_OVERFLOW;
    }
    if (ch_info->reg_class != MAC_INVALID_RC) {
        if (reg_class != HI_NULL) {
            *reg_class = ch_info->reg_class;
        }
        return HI_SUCCESS;
    }
    return HI_ERR_CODE_INVALID_CONFIG;
}

/* ****************************************************************************
 功能描述  : 检测信道号是否合法
 输入参数  : en_band  : 频段
             uc_ch_num: 信道号
 返 回 值  : HI_TRUE或HI_FALSE
 修改历史      :
  1.日    期   : 2013年4月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 mac_is_channel_num_valid(hi_u8 band, hi_u8 ch_num)
{
    hi_u8 ch_idx;
    hi_u32 ret;

    ret = mac_get_channel_idx_from_num(band, ch_num, &ch_idx);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    ret = mac_is_channel_idx_valid(band, ch_idx, HI_NULL);
    return ret;
}

/* ****************************************************************************
 功能描述  : 获取信道号管制类信息
 输入参数  : uc_band: 频段，0-2.4G, 1-5G
             uc_ch_num: 信道索引号
 修改历史      :
  1.日    期   : 2013年10月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
mac_regclass_info_stru *mac_get_channel_num_rc_info(hi_u8 band, hi_u8 ch_num)
{
    hi_u8 channel_idx;

    if (mac_get_channel_idx_from_num(band, ch_num, &channel_idx) != HI_SUCCESS) {
        oam_warning_log2(0, OAM_SF_ANY,
            "{mac_get_channel_num_rc_info::mac_get_channel_idx_from_num failed. band:%d, ch_num:%d", band, ch_num);
        return HI_NULL;
    }
    return mac_get_channel_idx_rc_info(band, channel_idx);
}

/* ****************************************************************************
 功能描述  : 设置管制域最大发送功率
 输入参数  : uc_pwr       : 功率
             en_exceed_reg: 是否可以超出管制域限制
 修改历史      :
  1.日    期   : 2014年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_regdomain_set_max_power(hi_u8 pwr, hi_u8 exceed_reg)
{
    hi_u8 rc_idx;
    hi_u8 reg_pwr;

    for (rc_idx = 0; rc_idx < g_mac_regdomain.regclass_num; rc_idx++) {
        reg_pwr = g_mac_regdomain.ast_regclass[rc_idx].max_reg_tx_pwr;
        if (pwr <= reg_pwr || exceed_reg) {
            g_mac_regdomain.ast_regclass[rc_idx].max_tx_pwr = pwr;
        } else {
            oam_warning_log3(0, OAM_SF_TPC, "uc_pwr[%d] exceed reg_tx_pwr[%d], rc_idx[%d]", pwr, reg_pwr, rc_idx);
        }
    }
}

/* ****************************************************************************
 功能描述  : 2G频段填写国家码
 输入参数  : pst_rd_info: 管制域信息
             puc_buffer : 填写频带三元组起始buffer地址
 输出参数  : puc_len    : 所填写三元组的长度
 修改历史      :
  1.日    期   : 2013年11月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_set_country_ie_2g(mac_regdomain_info_stru *rd_info, hi_u8 *puc_buffer, hi_u8 *puc_len)
{
    hi_u8 rc_idx;
    hi_u8 lsb_bit_position;
    mac_regclass_info_stru *reg_class = HI_NULL;
    hi_u8 len = 0;

    for (rc_idx = 0; rc_idx < rd_info->regclass_num; rc_idx++) {
        /* 获取 Regulatory Class */
        reg_class = &(rd_info->ast_regclass[rc_idx]);
        /* 如果频段不匹配 */
        if (MAC_RC_START_FREQ_2 != reg_class->start_freq) {
            continue;
        }
        /* 异常检查，信道位图为0表示此管制域没有信道存在，不能少 */
        if (reg_class->channel_bmap == 0) {
            continue;
        }
        /* 获取信道位图的最低一位, 返回0代表bit0置1 */
        lsb_bit_position = oal_bit_find_first_bit_four_byte(reg_class->channel_bmap);
        /* 获取信道号，例如Channel_Map为1100，其对应的索引值为2与3，再由索引值找到信道号 */
        mac_get_channel_num_from_idx(MAC_RC_START_FREQ_2, lsb_bit_position, &puc_buffer[len++]);
        /* 获取信道数 */
        puc_buffer[len++] = (hi_u8)oal_bit_get_num_four_byte(reg_class->channel_bmap);
        /* 获取最大功率 */
        puc_buffer[len++] = reg_class->max_reg_tx_pwr;
    }
    *puc_len = len;
    return;
}

/* ****************************************************************************
 功能描述  : 获取国家字符
 修改历史      :
  1.日    期   : 2013年10月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_char *mac_regdomain_get_country(hi_void)
{
    return g_mac_regdomain.ac_country;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
