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

/* *****************************************************************************
 * 1 Header File Including
 * **************************************************************************** */
#include "oal_err_wifi.h"
#include "oam_ext_if.h"
#include "wlan_spec_1131h.h"
#include "mac_vap.h"
#include "hmac_ext_if.h"
#include "wal_ioctl.h"
#include "wal_hipriv.h"
#include "wal_customize.h"

/* *****************************************************************************
 * 2 Global Variable Definition
 * **************************************************************************** */
/* 只在第一次上电时从NVM 中读取参数 */
static hi_u8 g_cfg_flag = HI_FALSE;
/* ini定制化参数数组 */
static hi_u32 *g_customize_init_params = HI_NULL;
/* 当前定制化配置是采用哪种类型 */
static hi_u32 g_cur_init_config_type = CUS_TAG_INI;
/* 降低开机电流(注意:会牺牲部分射频性能),默认关闭 */
static hi_bool g_minimize_boot_current = HI_FALSE;
/* *****************************************************************************
 * 3 Function Definition
 * **************************************************************************** */
/* ****************************************************************************
 函 数 名  : init_int_to_str_for_country_code
 功能描述  :   国家码转换
 修改历史      :
  1.日    期   : 2019年05月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void init_int_to_str_for_country_code(hi_char *country_code, hi_s32 size_code)
{
    hi_char cuntry_char[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
                             'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    hi_s32 country_codeval = (hi_s32)g_customize_init_params[INIT_CFG_COUNTRY_CODE];
    hi_s32 idx;

    if (country_code != HI_NULL) {
        if ((size_code == COUNTRY_CODE_LEN) && (country_codeval != 0) &&
            (country_codeval < (hi_s32)((sizeof(cuntry_char) + 1) * 26))) {    /* 26个英文字母 */
            idx = (country_codeval / 26 > 0) ? (country_codeval / 26 - 1) : 0; /* 26个英文字母 */
            if (idx >= 0) {
                country_code[0] = cuntry_char[idx];
            } else {
                country_code[0] = '\0';
            }

            idx = (country_codeval % 26 > 0) ? (country_codeval % 26 - 1) : 0; /* 26个英文字母 */
            if (idx >= 0) {
                country_code[1] = cuntry_char[idx];
            } else {
                country_code[1] = '\0';
            }
            country_code[2] = '\0'; /* 2 国码第3位 */
        } else {
            country_code[0] = '\0';
        }
    } else {
        oam_warning_log0(0, 0, "init_int_to_str_for_country_code: pCountryCode is NULL!");
    }

    return;
}

/* ****************************************************************************
 函 数 名  : init_str_to_int_for_country_code
 功能描述  : 国家码转换
 修改历史    :
  1.日    期 : 2019年05月20日
    作    者   : HiSilicon
    修改内容 : 新生成函数
**************************************************************************** */
static hi_u32 init_str_to_int_for_country_code(const hi_char *country_code, hi_s32 size_code)
{
    hi_u32 country_codeval;

    if ((country_code != NULL) && (size_code == COUNTRY_CODE_LEN)) {
        if (country_code[1] >= 'A' && country_code[1] <= 'Z' && country_code[0] >= 'A' && country_code[0] <= 'Z') {
            country_codeval = ((hi_u32)(hi_u8)(country_code[1] - 'A') + 1) * 26 + /* 26个英文字母 */
                ((hi_u32)(hi_u8)(country_code[0] - 'A') + 1);
        } else {
            oam_warning_log0(0, OAM_SF_ANY, "init_str_to_int_for_country_code:: pCountryCode is illegal!");
            country_codeval = 0;
        }
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "init_str_to_int_for_country_code:: pCountryCode is error!");
        country_codeval = 0;
    }

    return country_codeval;
}

#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
static hi_void init_params_data(const wal_cfg_params *data)
{
    g_customize_init_params[INIT_CFG_COUNTRY_CODE]                     = data->init_cfg_country_code;
    g_customize_init_params[INIT_CFG_AMPDU_TX_MAX_NUM]                 = data->init_cfg_ampdu_tx_max_num;
    g_customize_init_params[INIT_CFG_USED_MEM_FOR_START]               = data->init_cfg_used_mem_for_start;
    g_customize_init_params[INIT_CFG_USED_MEM_FOR_STOP]                = data->init_cfg_used_mem_for_stop;
    g_customize_init_params[INIT_CFG_RX_ACK_LIMIT]                     = data->init_cfg_rx_ack_limit;
    g_customize_init_params[INIT_CFG_INT_UNIT_CTRL]                    = data->init_cfg_int_unit_ctrl;
    g_customize_init_params[INIT_CFG_SDIO_D2H_ASSEMBLE_COUNT]          = data->init_cfg_sdio_d2h_assemble_count;
    g_customize_init_params[INIT_CFG_SDIO_H2D_ASSEMBLE_COUNT]          = data->init_cfg_sdio_h2d_assemble_count;
    g_customize_init_params[INIT_CFG_POWERMGMT_SWITCH]                 = data->init_cfg_powermgmt_switch;
    g_customize_init_params[INIT_CFG_STA_DTIM_SETTING]                 = data->init_cfg_sta_dtim_setting;
    g_customize_init_params[INIT_CFG_LOGLEVEL]                         = data->init_cfg_loglevel;
    g_customize_init_params[INIT_CFG_PHY_SCALING_VALUE_11B]            = data->init_cfg_phy_scaling_value_11b;
    g_customize_init_params[INIT_CFG_PHY_U1_SCALING_VALUE_11G]         = data->init_cfg_phy_u1_scaling_value_11g;
    g_customize_init_params[INIT_CFG_PHY_U2_SCALING_VALUE_11G]         = data->init_cfg_phy_u2_scaling_value_11g;
    g_customize_init_params[INIT_CFG_PHY_U1_SCALING_VALUE_11N_2D4G]    = data->init_cfg_phy_u1_scaling_value_11n_2d4g;
    g_customize_init_params[INIT_CFG_PHY_U2_SCALING_VALUE_11N_2D4G]    = data->init_cfg_phy_u2_scaling_value_11n_2d4g;
    g_customize_init_params[INIT_CFG_PHY_U1_SCALING_VALUE_11N40M_2D4G] =
        data->init_cfg_phy_u1_scaling_value_11n40m_2d4g;
    g_customize_init_params[INIT_CFG_PHY_U2_SCALING_VALUE_11N40M_2D4G] =
        data->init_cfg_phy_u2_scaling_value_11n40m_2d4g;
    g_customize_init_params[INIT_CFG_PHY_U0_SCALING_VALUE_11N_2D4G]    = data->init_cfg_phy_u0_scaling_value_11n_5g;
    g_customize_init_params[INIT_CFG_PHY_U3_SCALING_VALUE_11N40M_2D4G] = data->init_cfg_phy_u3_scaling_value_11n40m_5g;
    g_customize_init_params[INIT_CFG_RTS_CLK_FREQ]                     = data->init_cfg_rts_clk_freq;
    g_customize_init_params[INIT_CFG_CLK_TYPE]                         = (hi_u32)data->init_cfg_clk_type;
    g_customize_init_params[INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND1]    =
        (hi_u32)data->init_cfg_rf_line_txrx_gain_db_2g_band1;
    g_customize_init_params[INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND2]    =
        (hi_u32)data->init_cfg_rf_line_txrx_gain_db_2g_band2;
    g_customize_init_params[INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND3]    =
        (hi_u32)data->init_cfg_rf_line_txrx_gain_db_2g_band3;
    g_customize_init_params[INIT_CFG_RF_TXPWR_CALI_REF_2G_VAL_BAND1]   = data->init_cfg_rf_txpwr_cali_ref_2g_val_band1;
    g_customize_init_params[INIT_CFG_RF_TXPWR_CALI_REF_2G_VAL_BAND2]   = data->init_cfg_rf_txpwr_cali_ref_2g_val_band2;
    g_customize_init_params[INIT_CFG_RF_TXPWR_CALI_REF_2G_VAL_BAND3]   = data->init_cfg_rf_txpwr_cali_ref_2g_val_band3;
    g_customize_init_params[INIT_CFG_TX_RATIO_LEVEL_0]                 = data->init_cfg_tx_ratio_level_0;
    g_customize_init_params[INIT_CFG_TX_PWR_COMP_VAL_LEVEL_0]          = data->init_cfg_tx_pwr_comp_val_level_0;
    g_customize_init_params[INIT_CFG_TX_RATIO_LEVEL_1]                 = data->init_cfg_tx_ratio_level_1;
    g_customize_init_params[INIT_CFG_TX_PWR_COMP_VAL_LEVEL_1]          = data->init_cfg_tx_pwr_comp_val_level_1;
    g_customize_init_params[INIT_CFG_TX_RATIO_LEVEL_2]                 = data->init_cfg_tx_ratio_level_2;
    g_customize_init_params[INIT_CFG_TX_PWR_COMP_VAL_LEVEL_2]          = data->init_cfg_tx_pwr_comp_val_level_2;
    g_customize_init_params[INIT_CFG_MORE_PWR]                         = data->init_cfg_more_pwr;
    g_customize_init_params[INIT_CFG_RANDOM_MAC_ADDR_SCAN]             = data->init_cfg_random_mac_addr_scan;
    g_customize_init_params[INIT_CFG_11AC2G_ENABLE]                    = data->init_cfg_11ac2g_enable;
    g_customize_init_params[INIT_CFG_DISABLE_CAPAB_2GHT40]             = data->init_cfg_disable_capab_2ght40;
    g_customize_init_params[INIT_CFG_BAND_EDGE_LIMIT_2G_11G_TXPWR]     = data->init_cfg_band_edge_limit_2g_11g_txpwr;
    g_customize_init_params[INIT_CFG_BAND_EDGE_LIMIT_2G_11N_HT20_TXPWR]       =
        data->init_cfg_band_edge_limit_2g_11n_ht20_txpwr;
    g_customize_init_params[INIT_CFG_BAND_EDGE_LIMIT_2G_11N_HT40_TXPWR]       =
        data->init_cfg_band_edge_limit_2g_11n_ht40_txpwr;
    g_customize_init_params[INIT_CFG_BAND_EDGE_LIMIT_2G_11G_DBB_SCALING]      =
        data->init_cfg_band_edge_limit_2g_11g_dbb_scaling;
    g_customize_init_params[INIT_CFG_BAND_EDGE_LIMIT_2G_11N_HT20_DBB_SCALING] =
        data->init_cfg_band_edge_limit_2g_11n_ht20_dbb_scaling;
    g_customize_init_params[INIT_CFG_BAND_EDGE_LIMIT_2G_11N_HT40_DBB_SCALING] =
        data->init_cfg_band_edge_limit_2g_11n_ht40_dbb_scaling;
    g_customize_init_params[INIT_CFG_CALI_TONE_AMP_GRADE]                     = data->init_cfg_cali_tone_amp_grade;
}

static hi_void init_calibration_data(const wal_cfg_calibration* data)
{
    g_customize_init_params[INIT_CFG_DYN_CALI_DSCR_INTERVAL]    = data->init_cfg_dyn_cali_dscr_interval;

    g_customize_init_params[INIT_CFG_NVRAM_PA2GCCKA0]           = data->init_cfg_nvram_pa2gccka0;
    g_customize_init_params[INIT_CFG_NVRAM_PA2GA0]                 = data->init_cfg_nvram_pa2ga0;
    g_customize_init_params[INIT_CFG_NVRAM_PA2GCWA0]            = data->init_cfg_nvram_pa2gcwa0;

    g_customize_init_params[INIT_CFG_DPN24G_CH1_CORE0]          = data->init_cfg_dpn24g_ch1_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH2_CORE0]          = data->init_cfg_dpn24g_ch2_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH3_CORE0]          = data->init_cfg_dpn24g_ch3_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH4_CORE0]          = data->init_cfg_dpn24g_ch4_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH5_CORE0]          = data->init_cfg_dpn24g_ch5_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH6_CORE0]          = data->init_cfg_dpn24g_ch6_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH7_CORE0]          = data->init_cfg_dpn24g_ch7_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH8_CORE0]          = data->init_cfg_dpn24g_ch8_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH9_CORE0]          = data->init_cfg_dpn24g_ch9_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH10_CORE0]         = data->init_cfg_dpn24g_ch10_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH11_CORE0]         = data->init_cfg_dpn24g_ch11_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH12_CORE0]         = data->init_cfg_dpn24g_ch12_core0;
    g_customize_init_params[INIT_CFG_DPN24G_CH13_CORE0]         = data->init_cfg_dpn24g_ch13_core0;
    g_customize_init_params[INIT_CFG_DSSS2OFDM_DBB_PWR_BO_VAL]  = data->init_cfg_dsss2ofdm_dbb_pwr_bo_val;
    g_customize_init_params[INIT_CFG_CALI_DATA_MASK]            = data->init_cfg_cali_data_mask;
    g_customize_init_params[INIT_CFG_CALI_MASK]                 = data->init_cfg_cali_mask;
}

static hi_void init_rf_rx_insertion_data(const wal_cfg_rf_rx_insertion *data)
{
    g_customize_init_params[INIT_CFG_RF_RX_INSERTION_LOSS_2G_B1] = data->init_cfg_rf_rx_insertion_loss_2g_b1;
    g_customize_init_params[INIT_CFG_RF_RX_INSERTION_LOSS_2G_B2] = data->init_cfg_rf_rx_insertion_loss_2g_b2;
    g_customize_init_params[INIT_CFG_RF_RX_INSERTION_LOSS_2G_B3] = data->init_cfg_rf_rx_insertion_loss_2g_b3;
    g_customize_init_params[INIT_CFG_RF_LINE_RF_PWR_REF_RSSI_DB_2G_C0_MULT4] =
        data->init_cfg_rf_line_rf_pwr_ref_rssi_db_2g_c0_mult4;
}

static hi_void init_base_delta_power_data(const wal_cfg_base_delta_power *data)
{
    g_customize_init_params[INIT_CFG_NVRAM_MAX_TXPWR_BASE_2P4G] = data->init_cfg_nvram_max_txpwr_base_2p4g;
    g_customize_init_params[INIT_CFG_NVRAM_PARAMS0]             = data->init_cfg_nvram_params0;
    g_customize_init_params[INIT_CFG_NVRAM_PARAMS1]             = data->init_cfg_nvram_params1;
    g_customize_init_params[INIT_CFG_NVRAM_PARAMS2]             = data->init_cfg_nvram_params2;
    g_customize_init_params[INIT_CFG_NVRAM_PARAMS3]             = data->init_cfg_nvram_params3;
}

static hi_void init_sar_txpwr_data(const wal_cfg_sar_txpwr *data)
{
    g_customize_init_params[INIT_CFG_SAR_TXPWR_CTRL_2G] = data->init_cfg_sar_txpwr_ctrl_2g;
}

static hi_void init_band_txpwr_data(const wal_cfg_band_txpwr *data)
{
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH1]  = data->init_cfg_side_band_txpwr_limit_24g_ch1;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH2]  = data->init_cfg_side_band_txpwr_limit_24g_ch2;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH3]  = data->init_cfg_side_band_txpwr_limit_24g_ch3;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH4]  = data->init_cfg_side_band_txpwr_limit_24g_ch4;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH5]  = data->init_cfg_side_band_txpwr_limit_24g_ch5;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH6]  = data->init_cfg_side_band_txpwr_limit_24g_ch6;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH7]  = data->init_cfg_side_band_txpwr_limit_24g_ch7;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH8]  = data->init_cfg_side_band_txpwr_limit_24g_ch8;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH9]  = data->init_cfg_side_band_txpwr_limit_24g_ch9;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH10] = data->init_cfg_side_band_txpwr_limit_24g_ch10;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH11] = data->init_cfg_side_band_txpwr_limit_24g_ch11;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH12] = data->init_cfg_side_band_txpwr_limit_24g_ch12;
    g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH13] = data->init_cfg_side_band_txpwr_limit_24g_ch13;
}

static hi_void init_rf_reg1_data(const wal_cfg_rf_reg1 *data)
{
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_100_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_100_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_101_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_101_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_102_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_102_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_103_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_103_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_104_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_104_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_105_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_105_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_106_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_106_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_107_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_107_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_108_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_108_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_109_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_109_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_110_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_110_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_111_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_111_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_112_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_112_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_113_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_113_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_114_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_114_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_115_MODE_1AND2] = data->init_cfg_rf_pa_vdd_reg_115_mode_1and2;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_100_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_100_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_101_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_101_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_102_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_102_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_103_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_103_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_104_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_104_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_105_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_105_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_106_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_106_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_107_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_107_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_108_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_108_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_109_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_109_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_110_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_110_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_111_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_111_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_112_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_112_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_113_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_113_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_114_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_114_mode_3and4;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_115_MODE_3AND4] = data->init_cfg_rf_pa_vdd_reg_115_mode_3and4;
}

static hi_void init_rf_reg2_data(const wal_cfg_rf_reg2 *data)
{
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_100_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_100_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_101_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_101_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_102_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_102_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_103_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_103_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_104_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_104_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_105_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_105_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_106_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_106_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_107_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_107_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_108_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_108_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_109_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_109_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_110_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_110_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_111_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_111_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_112_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_112_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_113_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_113_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_114_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_114_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_115_MODE_5AND6] = data->init_cfg_rf_pa_vdd_reg_115_mode_5and6;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_100_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_100_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_101_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_101_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_102_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_102_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_103_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_103_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_104_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_104_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_105_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_105_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_106_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_106_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_107_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_107_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_108_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_108_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_109_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_109_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_110_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_110_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_111_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_111_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_112_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_112_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_113_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_113_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_114_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_114_mode_7and8;
    g_customize_init_params[INIT_CFG_RF_PA_VDD_REG_115_MODE_7AND8] = data->init_cfg_rf_pa_vdd_reg_115_mode_7and8;
}

static hi_void init_xtal_compesation_data(const rf_cfg_xtal_compesation *data)
{
    g_customize_init_params[INIT_CFG_RF_HIGH_TEMP_THRESHOLD] = data->init_cfg_rf_high_temp_threshold;
    g_customize_init_params[INIT_CFG_RF_LOW_TEMP_THRESHOLD]  = data->init_cfg_rf_low_temp_threshold;
    g_customize_init_params[INIT_CFG_RF_PPM_COMPESATION]     = data->init_cfg_rf_ppm_compesation;
}

static hi_void init_dpd_calibration_data(const wal_cfg_dpd_calibration *data)
{
    g_customize_init_params[INIT_CFG_DPD_CALI_CH_CORE0]          = data->init_cfg_dpd_cali_ch_core0;
    g_customize_init_params[INIT_CFG_DPD_USE_CAIL_CH_IDX0_CORE0] = data->init_cfg_dpd_use_cail_ch_idx0_core0;
    g_customize_init_params[INIT_CFG_DPD_USE_CAIL_CH_IDX1_CORE0] = data->init_cfg_dpd_use_cail_ch_idx1_core0;
    g_customize_init_params[INIT_CFG_DPD_AMP_T0_T3]              = data->init_cfg_dpd_amp_t0_t3;
    g_customize_init_params[INIT_CFG_DPD_AMP_T4_T7]              = data->init_cfg_dpd_amp_t4_t7;
    g_customize_init_params[INIT_CFG_DPD_COMP_TPC_TEMP]          = data->init_cfg_dpd_comp_tpc_temp;
}

/* ****************************************************************************
 函 数 名  : init_nvm_param
 功能描述  : 读取nvm 中的默认配置参数
 输入参数  :
 输出参数  : 无
 返 回 值  : HI_SUCCESS 或 失败错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2019年05月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
/* 编程规范规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 初始化函数结构简单，功能聚合性不拆分 */
static hi_u32 init_nvm_param(hi_void)
{
    hi_u32                      ret;
    wal_cfg_params              params;
    wal_cfg_base_delta_power    base_delta_power;
    wal_cfg_calibration         calibration;
    wal_cfg_rf_rx_insertion     rx_insertion;
    wal_cfg_sar_txpwr           sar_txpwr;
    wal_cfg_band_txpwr          band_txpwr;
    wal_cfg_rf_reg1             rf_reg1;
    wal_cfg_rf_reg2             rf_reg2;
    wal_cfg_dpd_calibration     dpd_cali;
    rf_cfg_xtal_compesation         rf_xtal_pll;

    /* read INIT_CONFIG_PARAMS */
    ret = hi_nv_read(INIT_CONFIG_PARAMS, (hi_void *)&params, sizeof(params), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_PARAMS is error \n");
        wal_cfg_params temp = { 92,         64,         25,         15,         10,         64,         8,
                                1,          1,          2,          0x9c9c9c9c, 0x666C6C6C, 0x666C6C6C, 0x57575757,
                                0x5c5c5757, 0x5a5a5a5a, 0x5d5d5a5a, 0x414F,     0x424b,     32768,      0,
                                -1,         -1,         -1,         115,        115,        115,        500,
                                17,         650,        13,         280,        5,          7,          0,
                                1,          1,          140,        140,        140,        0x68,       0x62,
                                0x62,       1,          0 };
        if (memcpy_s(&params, sizeof(wal_cfg_params), &temp, sizeof(wal_cfg_params)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_params_data(&params);

    /* read INIT_CONFIG_BASE_DELTA_POWER */
    ret = hi_nv_read(INIT_CONFIG_BASE_DELTA_POWER, (hi_void *)&base_delta_power, sizeof(base_delta_power), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_BASE_DELTA_POWER is error \n");
        wal_cfg_base_delta_power temp = { 170, 0, 0, 0, 0 };
        if (memcpy_s(&base_delta_power, sizeof(wal_cfg_base_delta_power), &temp, sizeof(wal_cfg_base_delta_power)) !=
            EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_base_delta_power_data(&base_delta_power);

    /* read INIT_CONFIG_CALIBRATION */
    ret = hi_nv_read(INIT_CONFIG_CALIBRATION, (hi_void *)&calibration, sizeof(calibration), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_CALIBRATION is error \n");
        wal_cfg_calibration temp = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 250, 0x67, 0 };
        if (memcpy_s(&calibration, sizeof(wal_cfg_calibration), &temp, sizeof(wal_cfg_calibration)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_calibration_data(&calibration);

    /* read INIT_CONFIG_RF_RX_INSERTION */
    ret = hi_nv_read(INIT_CONFIG_RF_RX_INSERTION, (hi_void *)&rx_insertion, sizeof(rx_insertion), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_RF_RX_INSERTION is error \n");
        wal_cfg_rf_rx_insertion temp = { 0, 0, 0, 0 };
        if (memcpy_s(&rx_insertion, sizeof(wal_cfg_rf_rx_insertion), &temp, sizeof(wal_cfg_rf_rx_insertion)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_rf_rx_insertion_data(&rx_insertion);

    /* read INIT_CONFIG_SAR_TXPWR */
    ret = hi_nv_read(INIT_CONFIG_SAR_TXPWR, (hi_void *)&sar_txpwr, sizeof(sar_txpwr), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_SAR_TXPWR is error] \n");
        wal_cfg_sar_txpwr temp = { 0xffffff };
        if (memcpy_s(&sar_txpwr, sizeof(wal_cfg_sar_txpwr), &temp, sizeof(wal_cfg_sar_txpwr)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_sar_txpwr_data(&sar_txpwr);

    /* read INIT_CONFIG_BAND_TXPWR */
    ret = hi_nv_read(INIT_CONFIG_BAND_TXPWR, (hi_void *)&band_txpwr, sizeof(band_txpwr), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_BAND_TXPWR is error\n");
        wal_cfg_band_txpwr temp = { 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
                                    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff };
        if (memcpy_s(&band_txpwr, sizeof(wal_cfg_band_txpwr), &temp, sizeof(wal_cfg_band_txpwr)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_band_txpwr_data(&band_txpwr);

    /* read INIT_CONFIG_RF_REG1 */
    ret = hi_nv_read(INIT_CONFIG_RF_REG1, (hi_void *)&rf_reg1, sizeof(rf_reg1), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_RF_REG1 is error\n");
        wal_cfg_rf_reg1 temp = { 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208,
                                 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208,
                                 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208,
                                 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208,
                                 0x12081208, 0x12081208, 0x12081208, 0x12081208 };
        if (memcpy_s(&rf_reg1, sizeof(wal_cfg_rf_reg1), &temp, sizeof(wal_cfg_rf_reg1)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_rf_reg1_data(&rf_reg1);

    /* read INIT_CONFIG_RF_REG2 */
    ret = hi_nv_read(INIT_CONFIG_RF_REG2, (hi_void *)&rf_reg2, sizeof(rf_reg2), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_RF_REG2 is error \n");
        wal_cfg_rf_reg2 temp = { 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208,
                                 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208,
                                 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208,
                                 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208, 0x12081208,
                                 0x12081208, 0x12081208, 0x12081208, 0x12081208 };
        if (memcpy_s(&rf_reg2, sizeof(wal_cfg_rf_reg2), &temp, sizeof(wal_cfg_rf_reg2)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_rf_reg2_data(&rf_reg2);

    /* read INIT_CONFIG_DPD_CALIBRATION */
    ret = hi_nv_read(INIT_CONFIG_DPD_CALIBRATION, (hi_void *)&dpd_cali, sizeof(dpd_cali), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_DPD_CALIBRATION is error \n");
        wal_cfg_dpd_calibration temp = { 0X71, 0X11117777, 0X0007DDDD, 0X40404040, 0X40404040, 0X00010F11 };
        if (memcpy_s(&dpd_cali, sizeof(wal_cfg_dpd_calibration), &temp, sizeof(wal_cfg_dpd_calibration)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_dpd_calibration_data(&dpd_cali);

    /* read INIT_CONFIG_XTAL_COMPESATION */
    ret = hi_factory_nv_read(INIT_CONFIG_XTAL_COMPESATION, (hi_void *)&rf_xtal_pll, sizeof(rf_xtal_pll), 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: hi_nv_read INIT_CONFIG_XTAL_COMPESATION is error \n");
        rf_cfg_xtal_compesation temp = { 110, 105, -20 };
        if (memcpy_s(&rf_xtal_pll, sizeof(rf_cfg_xtal_compesation), &temp, sizeof(rf_cfg_xtal_compesation)) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "init_nvm_param:: mem safe function err \n");
            return HI_FAIL;
        }
    }
    init_xtal_compesation_data(&rf_xtal_pll);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名: init_ini_param
 功能描述: 读取ini 中的默认配置参数
 输入参数:
 输出参数: 无
 返 回 值: HI_SUCCESS 或 失败错误码
 调用函数:
 被调函数:

 修改历史:
 1.日    期: 2019年05月27日
    作    者   : HiSilicon
   修改内容: 新生成函数

**************************************************************************** */
static hi_u32 init_ini_param(hi_void)
{
    oam_error_log0(0, OAM_SF_ANY, "The current custom is only supported nvm file!\n");
    return HI_FAIL;
}

/* ****************************************************************************
 函 数 名: init_read_customize_params
 功能描述: netdev open 调用的定制化总入口
           读取ini文件，更新 g_ul_customize_init_params 全局数组
 输入参数:
 输出参数: 无
 返 回 值: HI_SUCCESS 或 失败错误码
 调用函数:
 被调函数:

 修改历史:
 1.日    期: 2019年05月27日
    作    者   : HiSilicon
   修改内容: 新生成函数

**************************************************************************** */
static hi_u32 init_read_customize_params(hi_void)
{
    if (CUS_TAG_NVM == g_cur_init_config_type) {
        if (init_nvm_param() != HI_SUCCESS) {
            oam_error_log0(0, OAM_SF_ANY, "init_read_customize_params:: init nvm is error!\n");
        }
    } else if (CUS_TAG_INI == g_cur_init_config_type) {
        if (init_ini_param() != HI_SUCCESS) {
            oam_error_log0(0, OAM_SF_ANY, "init_read_customize_params:: init ini file is error!\n");
        }
    } else {
        oam_error_log1(0, OAM_SF_ANY, "init_read_customize_params:: cfg type[%d] is not correct!\n",
            g_cur_init_config_type);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}
#endif

#ifdef _PRE_DEBUG_MODE
hi_void init_print_params_proc(hi_char *pc_print_buff, hi_u32 buf_size, hi_u32 local, hi_void *data, hi_u8 len)
{
    hi_u32 cur_pos = 0;
    hi_u32 index = 0;
    hi_u32 string_len = 0;
    hi_s32 string_tmp_len;

    hi_u32 ret = hi_nv_read((hi_u8)local, data, len, 0);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "init_print_params:: the read nvm is error!\n");
        return;
    }

    hi_u32 *custom = (hi_u32 *)data;
    if (custom == HI_NULL) {
        return;
    }

    while ((index < len) && (string_len < buf_size - 1)) {
        if ((local == INIT_CONFIG_CALIBRATION) || (local == INIT_CONFIG_SAR_TXPWR) ||
            (local == INIT_CONFIG_BAND_TXPWR)  || (local == INIT_CONFIG_RF_REG1) ||
            (local == INIT_CONFIG_RF_REG2)     || (local == INIT_CONFIG_DPD_CALIBRATION)) {
            string_tmp_len = snprintf_s(pc_print_buff + string_len, (buf_size - string_len),
                (buf_size - string_len - 1), "0X%X\n", custom[cur_pos]);
        } else {
            string_tmp_len = snprintf_s(pc_print_buff + string_len, (buf_size - string_len),
                (buf_size - string_len - 1), "%d\n", custom[cur_pos]);
        }

        if (string_tmp_len == (-1)) {
            oam_error_log0(0, OAM_SF_ANY, "init_print_params:: snprintf_s error!\n");
            continue;
        }

        string_len += (hi_u32)string_tmp_len;
        index += 4; /* 4 步长 */
        cur_pos++;
    }
}

/* ****************************************************************************
 函 数 名: init_print_params
 功能描述: netdev open 调用的定制化总入口
 输入参数:
 输出参数: 无
 返 回 值: 无
 调用函数:
 被调函数:

 修改历史  :
 1.日    期: 2019年05月27日
    作    者   : HiSilicon
   修改内容: 新生成函数

**************************************************************************** */
/* 规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 包含本函数的宏没有打开，建议屏蔽 */
static hi_void init_print_params(hi_char *pc_print_buff, hi_u32 buf_size, hi_u32 local)
{
    hi_u8 len = 0;
    hi_void *data = HI_NULL;

    wal_cfg_params           params;
    wal_cfg_base_delta_power base_delta_power;
    wal_cfg_calibration      calibration;
    wal_cfg_rf_rx_insertion  rx_insertion;
    wal_cfg_sar_txpwr        sar_txpwr;
    wal_cfg_band_txpwr       band_txpwr;
    wal_cfg_rf_reg1          rf_reg1;
    wal_cfg_rf_reg2          rf_reg2;
    wal_cfg_dpd_calibration  dpd_cali;

    if (memset_s(pc_print_buff, buf_size, 0x00, buf_size) != EOK) {
        oam_error_log0(0, 0, "{init_print_params::mem safe func err!}");
        return;
    }

    if (local == INIT_CONFIG_PARAMS) {
        data = (hi_void *)&params;
        len = sizeof(params);
    } else if (local == INIT_CONFIG_CALIBRATION) {
        data = (hi_void *)&calibration;
        len = sizeof(calibration);
    } else if (local == INIT_CONFIG_RF_RX_INSERTION) {
        data = (hi_void *)&rx_insertion;
        len = sizeof(rx_insertion);
    } else if (local == INIT_CONFIG_BASE_DELTA_POWER) {
        data = (hi_void *)&base_delta_power;
        len = sizeof(base_delta_power);
    } else if (local == INIT_CONFIG_SAR_TXPWR) {
        data = (hi_void *)&sar_txpwr;
        len = sizeof(sar_txpwr);
    } else if (local == INIT_CONFIG_BAND_TXPWR) {
        data = (hi_void *)&band_txpwr;
        len = sizeof(band_txpwr);
    } else if (local == INIT_CONFIG_RF_REG1) {
        data = (hi_void *)&rf_reg1;
        len = sizeof(rf_reg1);
    } else if (local == INIT_CONFIG_RF_REG2) {
        data = (hi_void *)&rf_reg2;
        len = sizeof(rf_reg2);
    } else if (local == INIT_CONFIG_DPD_CALIBRATION) {
        data = (hi_void *)&dpd_cali;
        len = sizeof(dpd_cali);
    }

    init_print_params_proc(pc_print_buff, buf_size, local, data, len);
}
#endif

/* ****************************************************************************
 函 数 名: wal_customize_init_country
 功能描述: 国家码定制化
 输入参数:
 输出参数: 无
 返 回 值: HI_SUCCESS 或 失败错误码
 调用函数:
 被调函数:

 修改历史  :
 1.日    期: 2015年10月22日
    作    者   : HiSilicon
   修改内容: 新生成函数

**************************************************************************** */
static hi_u32 wal_customize_init_country(oal_net_device_stru *netdev)
{
    hi_u32 ret;
    hi_char country_code[COUNTRY_CODE_LEN] = {'\0'};
    init_int_to_str_for_country_code(&country_code[0], COUNTRY_CODE_LEN);

    ret = wal_hipriv_setcountry(netdev, &country_code[0]);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_country::wal_send_cfg_event return err code [%u]!}\r\n",
            ret);
        return ret;
    }

    return ret;
}

#ifdef _PRE_DEBUG_MODE
/* ****************************************************************************
 函 数 名  : wal_customize_init_log
 功能描述  : 日志相关
 输入参数  :
 输出参数  : 无
 返 回 值  : HI_SUCCESS 或 失败错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_customize_init_log(oal_net_device_stru *netdev)
{
    wal_msg_write_stru  write_msg;
    hi_u32              ret;
    hi_u32              l_loglevel;

    /* log_level */
    l_loglevel = wal_get_init_value(INIT_CFG_LOGLEVEL);
    if (l_loglevel < OAM_LOG_LEVEL_ERROR || l_loglevel > OAM_LOG_LEVEL_INFO) {
        oam_error_log3(0, OAM_SF_ANY,
            "{wal_customize_init_log::loglevel[%d] out of range[%d,%d], check value in ini file!}\r\n", l_loglevel,
            OAM_LOG_LEVEL_ERROR, OAM_LOG_LEVEL_INFO);
        return HI_FAIL;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_ALL_LOG_LEVEL, sizeof(hi_s32));
    *((hi_u32 *)(write_msg.auc_value)) = l_loglevel;
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_log::return err code[%u]!}\r\n", ret);
    }

    return ret;
}
#endif

#if _PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC
/* ****************************************************************************
 函 数 名  : wal_customize_init_clock
 功能描述  : hw 时钟
 输入参数  :
 输出参数  : 无
 返 回 值  : HI_SUCCESS 或 失败错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_customize_init_clock(oal_net_device_stru *netdev)
{
    wal_msg_write_stru      write_msg;
    hi_u32                  ret;
    hi_u32                  freq;
    hi_u8                   type;

    freq = (hi_u32)wal_get_init_value(INIT_CFG_RTS_CLK_FREQ);
    type = (hi_u8)!!wal_get_init_value(INIT_CFG_CLK_TYPE);

    if (freq < RTC_CLK_FREQ_MIN || freq > RTC_CLK_FREQ_MAX) {
        oam_error_log3(0, OAM_SF_ANY,
            "{wal_customize_init_clock::clock_freq[%d] out of range[%d,%d], check value in ini file!}\r\n", freq,
            RTC_CLK_FREQ_MIN, RTC_CLK_FREQ_MAX);
        return HI_FAIL;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_PM_CFG_PARAM, sizeof(mac_cfg_pm_param));
    ((mac_cfg_pm_param*)(write_msg.auc_value))->rtc_clk_freq = freq;
    ((mac_cfg_pm_param*)(write_msg.auc_value))->clk_type     = type;

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_pm_param),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_clock::wal_send_cfg_event return err code [%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif
/* ****************************************************************************
 函 数 名  : wal_customize_init_rf
 功能描述  : hw 2g 5g 前端
 输入参数  :
 输出参数  : 无
 返 回 值  : HI_SUCCESS 或 失败错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年12月14日
    作    者   : HiSilicon
    修改内容   : 增加针对温度上升导致发射功率下降过多的功率补偿

**************************************************************************** */
static hi_u32 wal_customize_init_rf(oal_net_device_stru *netdev)
{
    wal_msg_write_stru write_msg;
    hi_u8 error_param = HI_FALSE; /* 参数有效性标志，任意参数值不合法则置为1，所有参数不下发 */
    mac_cfg_customize_tx_pwr_comp_stru *tx_pwr = HI_NULL;
    hi_u8 idx, level;

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_CUS_RF,
        sizeof(mac_cfg_customize_rf) + sizeof(mac_cfg_customize_tx_pwr_comp_stru));

    mac_cfg_customize_rf *customize_rf = (mac_cfg_customize_rf *)(write_msg.auc_value);

    /* 配置: 2g rf */
    for (idx = 0; idx < MAC_NUM_2G_BAND; ++idx) {
        /* 获取各2p4g 各band 0.25db及0.1db精度的线损值 */
        hi_s8 rf_power_loss = (hi_s8)wal_get_init_value(INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND1 + idx);
        if ((rf_power_loss >= RF_LINE_TXRX_GAIN_DB_2G_MIN) && (rf_power_loss <= 0)) {
            customize_rf->ac_gain_db_2g[idx].rf_gain_db_2g_mult4 = rf_power_loss;
        } else {
            oam_error_log1(0, OAM_SF_CALIBRATE, "wal_customize_init_rf:: c_mult4[%d]", (hi_s32)rf_power_loss);

            error_param = HI_TRUE; /* 值超出有效范围，标记置为TRUE */
        }
    }

    /* 配置: 功率补偿 */
    tx_pwr = (mac_cfg_customize_tx_pwr_comp_stru *)(write_msg.auc_value + sizeof(mac_cfg_customize_rf));

    for (level = 0; level < 3; ++level) { /* 3 不超过3层 */
        tx_pwr->ast_txratio2pwr[level].us_tx_ratio =
            (hi_u16)wal_get_init_value(INIT_CFG_TX_RATIO_LEVEL_0 + 2 * level); /* 2 比例系数 */

        /* 判断tx占空比是否有效 */
        if (tx_pwr->ast_txratio2pwr[level].us_tx_ratio > TX_RATIO_MAX) {
            oam_error_log2(0, OAM_SF_ANY, "{wal_customize_init_rf::cfg_id[%d]:tx_ratio[%d] out of range}",
                INIT_CFG_TX_RATIO_LEVEL_0 + 2 * level, tx_pwr->ast_txratio2pwr[level].us_tx_ratio); /* 2 比例 */

            error_param = HI_TRUE; /* 值超出有效范围，标记置为TRUE */
        }

        tx_pwr->ast_txratio2pwr[level].us_tx_pwr_comp_val =
            (hi_u16)wal_get_init_value(INIT_CFG_TX_RATIO_LEVEL_0 + 2 * level + 1); /* 2 比例系数 */

        /* 判断发射功率补偿值是否有效 */
        if (tx_pwr->ast_txratio2pwr[level].us_tx_pwr_comp_val > TX_PWR_COMP_VAL_MAX) {
            oam_error_log2(0, OAM_SF_ANY, "{wal_customize_init_rf::cfg_id[%d]:tx_pwr_comp_val[%d] out of range}",
                INIT_CFG_TX_RATIO_LEVEL_0 + 2 * level + 1, tx_pwr->ast_txratio2pwr[level].us_tx_pwr_comp_val); /* 2 */

            error_param = HI_TRUE; /* 值超出有效范围，标记置为TRUE */
        }
    }

    /* 判断根据温度额外补偿的发射功率值是否有效 */
    tx_pwr->more_pwr = (hi_u32)wal_get_init_value(INIT_CFG_MORE_PWR);
    if (tx_pwr->more_pwr > MORE_PWR_MAX) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_rf::[INIT_CFG_MORE_PWR]more_pwr = %d}", tx_pwr->more_pwr);

        error_param = HI_TRUE; /* 值超出有效范围，标记置为TRUE */
    }

    /* 如果上述参数中有不正确的，直接返回 */
    if (error_param) {
        return HI_FAIL;
    }

    /* 如果所有参数都在有效范围内，则下发配置值 */
    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_customize_rf) + sizeof(mac_cfg_customize_tx_pwr_comp_stru),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_rf::EVENT[wal_send_cfg_event] failed, Err = %u}", ret);
    }

    return ret;
}

static hi_u32 wal_customize_init_dts_cali(mac_cus_dts_cali_stru *cus_cali)
{
    hi_u8 idx = 0;
    hi_u8 error_param = HI_FALSE; /* 参数有效性标志，任意参数值不合法则置为1，所有参数不下发 */
    /* * 配置: TXPWR_PA_DC_REF * */
    /* 2G REF: 分3个信道 */
    for (idx = 0; idx < MAC_NUM_2G_BAND; idx++) {
        hi_u16 s_ref_val = (hi_u16)wal_get_init_value(INIT_CFG_RF_TXPWR_CALI_REF_2G_VAL_BAND1 + idx);
        if (s_ref_val <= CALI_TXPWR_PA_DC_REF_MAX) {
            cus_cali->aus_cali_txpwr_pa_dc_ref_2g_val[idx] = s_ref_val;
        } else {
            /* 值超出有效范围，标记置为TRUE */
            error_param = HI_TRUE;
            oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_cali::[dts]2g ref err, id[%d], check dts file!}\r\n",
                INIT_CFG_RF_TXPWR_CALI_REF_2G_VAL_BAND1 + idx);
        }
    }

    /* 如果上述参数中有不正确的，直接返回 */
    if (error_param == HI_TRUE) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_cali:: params wrong value, do not send cfg event!}\r\n");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

hi_u32 wal_customize_init_band_edge_limit(mac_cus_band_edge_limit_stru *band_edge_limit)
{
    hi_u8 idx = 0;
    hi_u8 error_param = HI_FALSE; /* 参数有效性标志，任意参数值不合法则置为1，所有参数不下发 */
    /* 实际需要进行配置的FCC认证band数 */
    const hi_u8 fcc_auth_band_num = 3; /* 3 band数 */
    /* 赋值idx、txpwr */
    for (idx = 0; idx < fcc_auth_band_num; ++idx) {
        hi_u8 max_txpwr = (hi_u8)wal_get_init_value(INIT_CFG_BAND_EDGE_LIMIT_2G_11G_TXPWR + idx);
        band_edge_limit[idx].index = idx;
        band_edge_limit[idx].max_txpower = max_txpwr;
    }
    /* 赋值scale */
    for (idx = 0; idx < fcc_auth_band_num; ++idx) {
        hi_u8 dbb_scale = (hi_u8)wal_get_init_value(INIT_CFG_BAND_EDGE_LIMIT_2G_11G_DBB_SCALING + idx);
        band_edge_limit[idx].dbb_scale = dbb_scale;
    }
    /* 如果上述参数中有不正确的，直接返回 */
    if (error_param) {
        oam_error_log0(0, OAM_SF_ANY,
            "{wal_customize_init_band_edge_limit::one or more params have wrong value, do not send cfg event!}\r\n");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_customize_init_cali
 功能描述  : 定制化参数::ini::校准
 输入参数  :
 输出参数  : 无
 返 回 值  : HI_SUCCESS 或 失败错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_customize_init_cali(oal_net_device_stru *netdev)
{
    wal_msg_write_stru      write_msg = {0};
    hi_u32                  ret;
    mac_cus_dts_cali_stru   cus_cali = {0};
    mac_cus_band_edge_limit_stru*  band_edge_limit = HI_NULL;

    if (wal_customize_init_dts_cali(&cus_cali) != HI_SUCCESS) {
        return HI_FAIL;
    }

    /* 如果所有参数都在有效范围内，则下发配置值 */
    if (memcpy_s(write_msg.auc_value, sizeof(mac_cus_dts_cali_stru), (hi_s8 *)&cus_cali,
        sizeof(mac_cus_dts_cali_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_cali::mem safe function err!}");
        return HI_FAIL;
    }

    hi_u32 offset = sizeof(mac_cus_dts_cali_stru);

    /* 配置: FCC认证 */
    /* 申请内存存放边带功率信息,本函数结束后释放,申请内存大小: 6 * 4 = 24字节 */
    band_edge_limit =
        oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, NUM_OF_BAND_EDGE_LIMIT * sizeof(mac_cus_band_edge_limit_stru));
    if (band_edge_limit == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_cali::alloc fcc auth mem fail, return null ptr!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (wal_customize_init_band_edge_limit(band_edge_limit)) {
        /* 释放pst_band_edge_limit内存 */
        oal_mem_free(band_edge_limit);
        return HI_FAIL;
    }

    if (memcpy_s(write_msg.auc_value + offset, NUM_OF_BAND_EDGE_LIMIT * sizeof(mac_cus_band_edge_limit_stru),
        (hi_s8 *)band_edge_limit, NUM_OF_BAND_EDGE_LIMIT * sizeof(mac_cus_band_edge_limit_stru)) != EOK) {
        oal_mem_free(band_edge_limit);
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_cali::mem safe function err!}");
        return HI_FAIL;
    }
    offset += (NUM_OF_BAND_EDGE_LIMIT * sizeof(mac_cus_band_edge_limit_stru));

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_CUS_DTS_CALI, (hi_u16)offset);
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, (hi_u16)(WAL_MSG_WRITE_MSG_HDR_LENGTH + offset),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_cali::wal_send_cfg_event failed, error no[%u]!}\r\n", ret);
        oal_mem_free(band_edge_limit);
        return ret;
    }
    oal_mem_free(band_edge_limit);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_customize_init_dbb
 功能描述  : 解析从nv或dts中读取的字符数组，存入结构体数组中
 输入参数  :
 输出参数  : 无
 返 回 值  : HI_SUCCESS 或 失败错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_u32 wal_customize_init_dbb(oal_net_device_stru *netdev)
{
    hi_u8  idx;
    hi_u32 ret;
    hi_u32 offset = 0;
    wal_msg_write_stru write_msg;
    dbb_scaling_stru dbb_scaling_params;

    if (memset_s(&dbb_scaling_params, sizeof(dbb_scaling_stru), 0, sizeof(dbb_scaling_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_dbb::mem safe function err!}");
        return HI_FAIL;
    }
    if (memset_s(&write_msg, sizeof(wal_msg_write_stru), 0, sizeof(wal_msg_write_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_dbb::mem safe function err!}");
        return HI_FAIL;
    }
    /* 结构体数组赋值 */
    for (idx = 0; idx < 7; idx++) { /* 7 结构体数组长度 */
        hi_u32 ref_val = (hi_u32)wal_get_init_value(INIT_CFG_PHY_SCALING_VALUE_11B + idx);
        dbb_scaling_params.dbb_scale[idx] = ref_val;
    }
    if (memcpy_s(write_msg.auc_value, sizeof(dbb_scaling_stru), (hi_s8 *)&dbb_scaling_params,
        sizeof(dbb_scaling_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_dbb::mem safe function err!}");
        return HI_FAIL;
    }
    offset += sizeof(dbb_scaling_stru);

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_CUS_NVRAM_PARAM, (hi_u16)offset);

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, (hi_u16)(WAL_MSG_WRITE_MSG_HDR_LENGTH + offset),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_dbb_main::return err code [%u]!}\r\n", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 读取每个信道的发送功率(FCC)
 修改历史      :
  1.日    期   : 2019年12月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 wal_customize_init_fcc_tx_pwr(oal_net_device_stru *netdev)
{
    hi_u8  idx;
    hi_u32 ret;
    hi_u32 offset = 0;
    wal_msg_write_stru write_msg;
    fcc_tx_pwr_stru fcc_tx_pwr_params;

    if (memset_s(&fcc_tx_pwr_params, sizeof(fcc_tx_pwr_stru), 0, sizeof(fcc_tx_pwr_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_fcc_tx_pwr::mem safe function err!}");
        return HI_FAIL;
    }
    if (memset_s(&write_msg, sizeof(wal_msg_write_stru), 0, sizeof(wal_msg_write_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_fcc_tx_pwr::mem safe function err!}");
        return HI_FAIL;
    }
    /* 结构体数组赋值 */
    for (idx = 0; idx < MAC_NUM_2G_CH_NUM; idx++) { /* 9 结构体数组长度 */
        fcc_tx_pwr_params.tx_pwr[idx] = wal_get_init_value(INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH1 + idx);
    }
    if (memcpy_s(write_msg.auc_value, sizeof(fcc_tx_pwr_stru),
        (hi_s8*)&fcc_tx_pwr_params, sizeof(fcc_tx_pwr_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_fcc_tx_pwr::mem safe function err!}");
        return HI_FAIL;
    }
    offset += sizeof(fcc_tx_pwr_stru);

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_CUS_FCC_TX_PWR, (hi_u16)offset);

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, (hi_u16)(WAL_MSG_WRITE_MSG_HDR_LENGTH + offset),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_fcc_tx_pwr::return err code [%u]!}\r\n", ret);
    }

    return ret;
}

#ifdef _PRE_XTAL_FREQUENCY_COMPESATION_ENABLE
static hi_u32 wal_customize_init_freq_comp(oal_net_device_stru *netdev)
{
    wal_msg_write_stru write_msg;
    freq_comp_stru freq_comp_params;

    if (memset_s(&freq_comp_params, sizeof(freq_comp_stru), 0, sizeof(freq_comp_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_freq_comp::mem safe function err!}");
        return HI_FAIL;
    }
    if (memset_s(&write_msg, sizeof(wal_msg_write_stru), 0, sizeof(wal_msg_write_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_freq_comp::mem safe function err!}");
        return HI_FAIL;
    }
    /* 结构体数组赋值 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    freq_comp_params.high_temp_th = 0;
#else
    freq_comp_params.high_temp_th = (hi_s16)wal_get_init_value(INIT_CFG_RF_HIGH_TEMP_THRESHOLD);
#endif
    if (freq_comp_params.high_temp_th == 0) {
        freq_comp_params.high_temp_th = 105; /* 105:频偏温度值下限 */
    }
    if (freq_comp_params.high_temp_th < FREQ_COMP_TEMP_MIN || freq_comp_params.high_temp_th > FREQ_COMP_TEMP_MAX) {
        oam_error_log3(0, OAM_SF_ANY, "{wal_customize_init_freq_comp::high_threshold[%d] out of range[%d,%d]!}\r\n",
            freq_comp_params.high_temp_th, FREQ_COMP_TEMP_MIN, FREQ_COMP_TEMP_MAX);
        return HI_FAIL;
    }
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    freq_comp_params.low_temp_th = 0;
#else
    freq_comp_params.low_temp_th = (hi_s16)wal_get_init_value(INIT_CFG_RF_LOW_TEMP_THRESHOLD);
#endif
    if (freq_comp_params.low_temp_th == 0) {
        freq_comp_params.low_temp_th = 100; /* 100:频偏温度值下限 */
    }
    if (freq_comp_params.low_temp_th < FREQ_COMP_TEMP_MIN || freq_comp_params.low_temp_th > FREQ_COMP_TEMP_MAX) {
        oam_error_log3(0, OAM_SF_ANY, "{wal_customize_init_freq_comp::low_threshold[%d] out of range[%d,%d]!}\r\n",
            freq_comp_params.low_temp_th, FREQ_COMP_TEMP_MIN, FREQ_COMP_TEMP_MAX);
        return HI_FAIL;
    }
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    freq_comp_params.comp_val = 0;
#else
    freq_comp_params.comp_val = (hi_s16)wal_get_init_value(INIT_CFG_RF_PPM_COMPESATION);
#endif
    if (freq_comp_params.comp_val == 0) {
        freq_comp_params.comp_val = (-30); /* 30:频偏值 */
    }
    if (freq_comp_params.comp_val < FREQ_COMP_VAL_MIN || freq_comp_params.comp_val > FREQ_COMP_VAL_MAX) {
        oam_error_log3(0, OAM_SF_ANY, "{wal_customize_init_freq_comp::ppm_compesation[%d] out of range[%d,%d]!}\r\n",
            freq_comp_params.comp_val, FREQ_COMP_VAL_MIN, FREQ_COMP_VAL_MAX);
        return HI_FAIL;
    }

    if (memcpy_s(write_msg.auc_value, sizeof(freq_comp_stru),
        (hi_s8*)&freq_comp_params, sizeof(freq_comp_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_customize_init_freq_comp::mem safe function err!}");
        return HI_FAIL;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_FREQ_COMP, (hi_u16)sizeof(freq_comp_stru));

    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        (hi_u16)(WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(freq_comp_stru)), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_customize_init_freq_comp::return err code [%u]!}\r\n", ret);
    }

    return HI_SUCCESS;
}
#endif

hi_u32 wal_cfg_dbb(const hi_u32 *data, hi_u8 size)
{
    if (size != 7) { /* 7:dbb配置参数个数加evm配置参数 */
        oam_error_log1(0, OAM_SF_ANY, "wal_cfg_dbb: input data size should be 13, now %d!", size);
        return HI_FAIL;
    }
    return wal_set_init_value(INIT_CFG_PHY_SCALING_VALUE_11B, data, size);
}

hi_u32 wal_cfg_country_code(const hi_char *country_code, hi_s32 size)
{
    hi_u32 code = init_str_to_int_for_country_code(country_code, size);
    return wal_set_init_value(INIT_CFG_COUNTRY_CODE, &code, 1);
}

hi_u32 wal_cfg_fcc_tx_pwr(const hi_u32 *data, hi_u8 size)
{
    if (size != 13) { /* 13:fcc发送功率配置参数个数 */
        oam_error_log1(0, OAM_SF_ANY, "wal_cfg_fcc_tx_pwr: input data size should be 13, now %d!", size);
        return HI_FAIL;
    }
    mdelay(3); /* 3:延时,避免sdio异常 */
    return wal_set_init_value(INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH1, data, size);
}

hi_u32 wal_cfg_freq_comp_val(const hi_u32 *data, hi_u8 size)
{
    if (size != 3) { /* 3:高温频偏配置参数个数 */
        oam_error_log1(0, OAM_SF_ANY, "wal_cfg_freq_comp_val: input data size should be 3, now %d!", size);
        return HI_FAIL;
    }
    return wal_set_init_value(INIT_CFG_RF_HIGH_TEMP_THRESHOLD, data, size);
}

hi_u32 wal_cfg_rssi_ofset(hi_s32 data)
{
    if ((data < RF_LINE_TXRX_GAIN_DB_2G_MIN) || (data > 0)) {
        oam_error_log1(0, OAM_SF_ANY, "wal_cfg_rssi_ofset: input data out of range.Should in [-32, 0]!", data);
        return HI_FAIL;
    }
    return wal_set_init_value(INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND1, (hi_u32 *)&data, 1);
}

/* ****************************************************************************
 功能描述  : 配置定制化参数入口
 修改历史      :
    日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_customize_set_config(hi_void)
{
    hi_u32 ret = HI_SUCCESS;
    oal_net_device_stru *netdev = HI_NULL;

    if (!g_cfg_flag && (g_customize_init_params != HI_NULL)) {
        oam_error_log1(0, OAM_SF_ANY, "In wal_customize_set_config g_uc_cfg_flag[%d] is not inited !", g_cfg_flag);
        return HI_FAIL;
    }

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wifi_set_country Hisilicon0 device not found.");
        /* 释放从NVM读取的默认配置 */
        oal_free(g_customize_init_params);
        g_customize_init_params = HI_NULL;
        return HI_FAIL;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev);
#endif

#ifdef _PRE_XTAL_FREQUENCY_COMPESATION_ENABLE
    /* RF PLL补偿系数 */
    if (wal_customize_init_freq_comp(netdev) != HI_SUCCESS) {
        ret |= BIT1; /* BIT0为HI_FAIL不可用 */
    }

#endif

    /* DBB scaling */
    if (wal_customize_init_dbb(netdev) != HI_SUCCESS) {
        ret |= BIT3;
    }
    /* FCC tx pwr */
    if (wal_customize_init_fcc_tx_pwr(netdev) != HI_SUCCESS) {
        ret |= BIT4;
    }
    /* 国家码 device init时国家码已初始化 */
    if (wal_customize_init_country(netdev) != HI_SUCCESS) {
        ret |= BIT5;
    }
    /* RF */
    if (wal_customize_init_rf(netdev) != HI_SUCCESS) {
        ret |= BIT7;
    }
    /* 校准 */
    if (wal_customize_init_cali(netdev) != HI_SUCCESS) {
        ret |= BIT2;
    }
#ifdef CUSTOM_DBG
    printk("wal_customize_init_cali success.ret 0x%x\r\n", ret);
#endif
    /* 释放从NVM读取的默认配置 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    free(g_customize_init_params);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_free(g_customize_init_params);
#endif
    g_customize_init_params = HI_NULL;

    return ret;
}

/* ****************************************************************************
 功能描述: 初始化定制化信息
 修改历史:
 1.日    期: 2019年05月27日
    作    者   : HiSilicon
   修改内容: 新生成函数
**************************************************************************** */
hi_u32 wal_customize_init(hi_void)
{
    hi_u32 i;
    hi_char country_code[] = "NC"; /* 初始国家码为CN,CN第1个字符和第2个字符替换位置结果为NC */

    if (g_cfg_flag) {
        return HI_SUCCESS;
    }

#ifdef _PRE_FCC_INITIAL_PARAMS_MODE
    wal_customize_params wifi_customize_params = {0, {105, 100, -30},
        {0x61636263, 0x6A6A6A6A, 0x4F536061, 0x60686768, 0x51516161, 0x00000000, 0x01000000},
        {0x8aa8aaa1, 0x24424441, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
         0x00000001, 0x00000001, 0x02202221, 0x68868881, 0xffffffff, 0xffffffff}};
#else /* CFG_CE_MODE */
    wal_customize_params wifi_customize_params = {0, {105, 100, -30},
        {0x6B6B6D6E, 0x67676767, 0x50545F60, 0x565E5E5E, 0x4F4F5656, 0x00000000, 0x01000000},
        {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
         0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}};
#endif

    g_customize_init_params = (hi_u32 *)oal_memalloc(sizeof(hi_u32) * INIT_CFG_BUTT);
    if (g_customize_init_params == HI_NULL) {
        oam_error_log0(0, 0, "In wal_customize_init g_ul_customize_init_params is not malloc!");
        return HI_ERR_MALLOC_FAILUE;
    }
    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(g_customize_init_params, sizeof(hi_u32) * INIT_CFG_BUTT, 0, sizeof(hi_u32) * INIT_CFG_BUTT);
    /* 初始化dbb参数 */
    for (i = 0; i < WAL_DBB_PARAM_CNT; ++i) {
        g_customize_init_params[INIT_CFG_PHY_SCALING_VALUE_11B + i] = wifi_customize_params.dbb_params[i];
    }
    /* 初始化fcc参数 */
    for (i = 0; i < WAL_CH_TX_PWR_PARAM_CNT; ++i) {
        g_customize_init_params[INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH1 + i] = wifi_customize_params.ch_txpwr_offset[i];
    }
    /* 初始化高温频偏参数 */
    for (i = 0; i < WAL_FREQ_COMP_PARAM_CNT; ++i) {
        g_customize_init_params[INIT_CFG_RF_HIGH_TEMP_THRESHOLD + i] = wifi_customize_params.freq_comp[i];
    }
    /* 初始化rssi参数 */
    g_customize_init_params[INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND1] = wifi_customize_params.rssi_offset;
    /* 初始化国家码 */
    g_cfg_flag = HI_TRUE;
    hi_u32 ret = wal_cfg_country_code(country_code, COUNTRY_CODE_LEN);
    if (ret != HI_SUCCESS) {
        return ret;
    }
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    ret = init_read_customize_params();
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, "In wal_customize_init failed.");
        oal_free(g_customize_init_params);
        g_customize_init_params = HI_NULL;
        return ret;
    }
#endif
    printk("wal_customize_init SUCCESSFULLY!\r\n");
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 初始化定制化信息
 修改历史      :
  1.日    期   : 2019年05月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 wal_customize_exit(hi_void)
{
    if (!g_cfg_flag) {
        oam_error_log0(0, 0, "In wal_customize_exit g_uc_cfg_flag invalid!");
        return HI_FAIL;
    }

    if (g_customize_init_params != HI_NULL) {
        oal_free(g_customize_init_params);
        g_customize_init_params = HI_NULL;
    }

    g_cfg_flag = HI_FALSE;

    printk("wal_customize_exit SUCCESSFULLY\r\n");
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_get_init_value
 功能描述  :
 输入参数  :
 输出参数  : 无
 返 回 值  : HI_SUCCESS 或 失败错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年11月2日
    作    者   : HiSilicon
    修改内容   : 增加tag用于判断ini和dts

**************************************************************************** */
hi_u32 wal_get_init_value(hi_u32 cfg_id)
{
    hi_u32 *pgal_params = HI_NULL;
    hi_u32 l_wlan_cfg_butt;

    if (g_cfg_flag && (g_customize_init_params != HI_NULL)) {
        if ((CUS_TAG_NVM == g_cur_init_config_type) || (CUS_TAG_INI == g_cur_init_config_type)) {
            pgal_params = &g_customize_init_params[0];
            l_wlan_cfg_butt = INIT_CFG_BUTT;
        } else {
            oam_error_log1(0, OAM_SF_ANY, "wal_customize tag number[0x%2x] not correct!\n", g_cur_init_config_type);
            return HI_FAIL;
        }

        if (l_wlan_cfg_butt <= cfg_id) {
            oam_error_log2(0, OAM_SF_ANY, "wal_customize cfg id:%d out of range, max cfg id:%d\n", cfg_id,
                l_wlan_cfg_butt);
            return HI_FAIL;
        }

        return pgal_params[cfg_id];
    } else {
        oam_error_log0(0, OAM_SF_ANY, "The custom must be config is inited process!\n");
        return 0;
    }
}

/* ****************************************************************************
 功能描述  :设置参数值
 修改历史     :
 1.日    期   : 2020年03月26日
    作    者   : HiSilicon
   修改内容   : 新生成函数
**************************************************************************** */
hi_u32 wal_set_init_value(hi_u32 cfg_id, const hi_u32 *data, hi_u8 size)
{
    hi_u32 i;

    if (g_cfg_flag && (g_customize_init_params != HI_NULL)) {
        if ((cfg_id + size) >= INIT_CFG_BUTT) {
            oam_error_log2(0, OAM_SF_ANY, "wal_set_init_value cfg id:%d out of range, max cfg id:%d\n",
                (cfg_id + size), INIT_CFG_BUTT);
            return HI_FAIL;
        }
        for (i = 0; i < size; ++i) {
            g_customize_init_params[cfg_id + i] = data[i];
        }
    } else {
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

#ifdef _PRE_DEBUG_MODE
/* ****************************************************************************
 函 数 名  : wal_dump_init_param
 功能描述  : 初始化定制化信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2019年05月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* 结构体数组g_ast_hipriv_cmd的成员，其中wal_hipriv_getcountry修改了对应变量，lint_t e818告警屏蔽 */
hi_u32 wal_print_init_params(oal_net_device_stru *netdev, hi_char *pc_param)
{
    hi_u32 local = 0x20;
    hi_char *pc_print_buff = HI_NULL;

    if ((netdev == HI_NULL) || (pc_param == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "wal_print_init_params: The param is error!\n");
        return HI_FAIL;
    }

    if (!g_cfg_flag) {
        oam_error_log1(0, OAM_SF_ANY, "wal_print_init_params: g_uc_cfg_flag[%d] not correct!\n", g_cfg_flag);
        return HI_FAIL;
    }

    pc_print_buff = (hi_char *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, OAM_REPORT_MAX_STRING_LEN);
    if (pc_print_buff == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wal_print_init_params:: malloc mem is error!\n");
        return HI_ERR_MALLOC_FAILUE;
    }

    while (local < INIT_CONFIG_NVM_BUTT) {
        init_print_params(pc_print_buff, OAM_REPORT_MAX_STRING_LEN, local);
        local++;
    }

    oal_mem_free(pc_print_buff);
    return HI_SUCCESS;
}
#endif

hi_void wal_set_boot_current_flag(hi_bool minimize)
{
    g_minimize_boot_current = minimize;
}

hi_u32 wal_sync_boot_current_to_dev(const hi_char *ifname)
{
    wal_msg_write_stru          write_msg = {0};
    hi_u8                      *param = HI_NULL;
    oal_net_device_stru        *netdev = HI_NULL;
    hi_u32                      ret;

    if (ifname == HI_NULL) {
        oam_error_log0(0, 0, "{wal_minimize_boot_current:: ifname is NULL.}");
        return HI_FAIL;
    }
    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "wal_minimize_boot_current:: device not found.");
        return HI_FAIL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev);
#endif
    /* 解析并设置配置命令参数 */
    param = (hi_u8 *)(write_msg.auc_value);
    *param = !g_minimize_boot_current; /* 取反下发 */

    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_MINIMIZE_BOOT_CURRET, sizeof(hi_u8));

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u8),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_minimize_boot_current::return err code [%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
