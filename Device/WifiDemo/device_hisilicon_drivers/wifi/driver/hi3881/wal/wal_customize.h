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

#ifndef __WAL_CUSTOMIZE_H__
#define __WAL_CUSTOMIZE_H__

/* *****************************************************************************
 * 1 Other Header File Including
 * **************************************************************************** */
#include "oal_err_wifi.h"
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "oam_ext_if.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* *****************************************************************************
 2 宏定义
***************************************************************************** */
#define INI_MODU_WIFI                       0x1
#define INI_MODU_GNSS                       0x2
#define INI_MODU_BT                         0x4
#define INI_MODU_FM                         0x8
#define INI_MODU_WIFI_PLAT                  0x10
#define INI_MODU_BFG_PLAT                   0x20
#define INI_MODU_PLAT                       0x40
#define INI_MODU_HOST_VERSION               0x80
#define INI_MODU_WIFI_MAC                   0x81
#define INI_MODU_COEXIST                    0x82

#define CUST_MODU_WIFI                      0x1
#define CUST_MODU_GNSS                      0x2
#define CUST_MODU_BT                        0x4
#define CUST_MODU_FM                        0x8
#define CUST_MODU_WIFI_PLAT                 0x10
#define CUST_MODU_BFG_PLAT                  0x20
#define CUST_MODU_PLAT                      0x40
#define CUST_MODU_DTS                       0x50
#define CUST_MODU_HOST_VERSION              0x80
#define CUST_MODU_WIFI_MAC                  0x81
#define CUST_MODU_COEXIST                   0x82
#define CUST_MODU_NVRAM                     0x83

#define INI_STR_MODU_LEN                    40
#define INI_MODE_GPSGLONASS                 0
#define INI_MODE_BDGPS                      1
#define INI_MODE_NORMAL                     0
#define INI_MODE_PERFORMANCE                1
#define INI_MODE_CERTIFY                    2
#define INI_MODE_CERTIFY_CE                 3

#define MAX_READ_LINE_NUM                   192
#define INI_FILE_PATH_LEN                   128
#define INI_READ_VALUE_LEN                  64
#define INI_VERSION_STR_LEN                 32

#define MAC_LEN                             6
#define NV_WLAN_NUM                         193
#define NV_WLAN_VALID_SIZE                  12

#ifndef MAC2STR
#define mac2str(a)                          (a)[0], "**", "**", "**", (a)[4], (a)[5]
#endif

#define CUS_TAG_INI                         0x01
#define CUS_TAG_NVM                         0x03
#define CALI_TXPWR_PA_DC_REF_MIN            0
#define CALI_TXPWR_PA_DC_REF_MAX            500
#define CALI_TXPWR_PA_DC_FRE_MIN            0
#define CALI_TXPWR_PA_DC_FRE_MAX            78
#define CALI_BT_TXPWR_PA_DC_REF_MAX         15000
#define CHN_EST_CTRL_EVB                    0x3C192240
#define CHN_EST_CTRL_MATE7                  0x3C193240
#define CHN_EST_CTRL_FPGA                   0x3C19A243
#define PHY_POWER_REF_2G_3798               0xDCE0F4
#define PHY_POWER_REF_2G_EVB0               0xDCE0F4
#define RTC_CLK_FREQ_MIN                    32000
#define RTC_CLK_FREQ_MAX                    33000
#define RF_LINE_TXRX_GAIN_DB_2G_MIN         (-32)
#define RF_LINE_TXRX_GAIN_DB_5G_MIN         (-12)
#define PSD_THRESHOLD_MIN                   (-15)
#define PSD_THRESHOLD_MAX                   (-10)
#define LNA_GAIN_DB_MIN                     (-10)
#define LNA_GAIN_DB_MAX                     20
#define MAX_BASE_TXPOWER_MIN                130    /* 最大发送功率的最小有效值:130 13.0dbm */
#define MAX_BASE_TXPOWER_MAX                238    /* 最大发送功率的最大有效值:238 23.8dbm */
#define MAX_DBB_SCALE                       0xEE   /* DBB SCALE最大有效值 */
#define TX_RATIO_MAX                        2000   /* tx占空比的最大有效值 */
#define TX_PWR_COMP_VAL_MAX                 50     /* 发射功率补偿值的最大有效值 */
#define MORE_PWR_MAX                        50     /* 根据温度额外补偿的发射功率的最大有效值 */
#define COUNTRY_CODE_LEN                    3      /* 国家码位数 */
#define WAL_FREQ_COMP_PARAM_CNT             3      /* 高温频偏个数 */
#define WAL_DBB_PARAM_CNT                   7      /* dbb scale相关参数个数 */
#define WAL_CH_TX_PWR_PARAM_CNT             13     /* 信道相关发送功率参数个数 */
#define NUM_OF_BAND_EDGE_LIMIT              6      /* FCC边带认证参数个数 */

#define FREQ_COMP_TEMP_MIN (-40)
#define FREQ_COMP_TEMP_MAX 140

#define FREQ_COMP_VAL_MIN (-10000)
#define FREQ_COMP_VAL_MAX 10000

/* *****************************************************************************
 2 枚举定义
***************************************************************************** */
/* 定制化 INIT CONFIG ID */
typedef enum {
    /* COUNTRY CODE */
    INIT_CFG_COUNTRY_CODE = 0,
    /* 性能 */
    INIT_CFG_AMPDU_TX_MAX_NUM,
    INIT_CFG_USED_MEM_FOR_START,
    INIT_CFG_USED_MEM_FOR_STOP,
    INIT_CFG_RX_ACK_LIMIT, /* 4 */
    INIT_CFG_INT_UNIT_CTRL,
    INIT_CFG_SDIO_D2H_ASSEMBLE_COUNT, /* 5 */
    INIT_CFG_SDIO_H2D_ASSEMBLE_COUNT,
    /* 低功耗 */
    INIT_CFG_POWERMGMT_SWITCH,
    /* STA DTIM参数配置 */
    INIT_CFG_STA_DTIM_SETTING,
    /* 可维可测 */
    INIT_CFG_LOGLEVEL,
    /* PHY DBB SCALING */
    INIT_CFG_PHY_SCALING_VALUE_11B, /* 10 */
    INIT_CFG_PHY_U1_SCALING_VALUE_11G,
    INIT_CFG_PHY_U2_SCALING_VALUE_11G,
    INIT_CFG_PHY_U1_SCALING_VALUE_11N_2D4G,
    INIT_CFG_PHY_U2_SCALING_VALUE_11N_2D4G,
    INIT_CFG_PHY_U1_SCALING_VALUE_11N40M_2D4G,
    INIT_CFG_PHY_U2_SCALING_VALUE_11N40M_2D4G,
    INIT_CFG_PHY_U0_SCALING_VALUE_11N_2D4G,
    INIT_CFG_PHY_U3_SCALING_VALUE_11N40M_2D4G,
    /* 时钟信息 */
    INIT_CFG_RTS_CLK_FREQ,
    INIT_CFG_CLK_TYPE, /* 20 */
    /* 2G RF前端 */
    INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND1,
    INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND2,
    INIT_CFG_RF_LINE_TXRX_GAIN_DB_2G_BAND3,

    /* 2G RF Tx POWER REF值 */
    INIT_CFG_RF_TXPWR_CALI_REF_2G_VAL_BAND1,
    INIT_CFG_RF_TXPWR_CALI_REF_2G_VAL_BAND2,
    INIT_CFG_RF_TXPWR_CALI_REF_2G_VAL_BAND3,
    /* 温度上升导致发射功率下降过多的功率补偿 */
    INIT_CFG_TX_RATIO_LEVEL_0,        /* tx占空比 */
    INIT_CFG_TX_PWR_COMP_VAL_LEVEL_0, /* 发射功率补偿值 */
    INIT_CFG_TX_RATIO_LEVEL_1,
    INIT_CFG_TX_PWR_COMP_VAL_LEVEL_1, /* 30 */
    INIT_CFG_TX_RATIO_LEVEL_2,
    INIT_CFG_TX_PWR_COMP_VAL_LEVEL_2,
    INIT_CFG_MORE_PWR, /* 根据温度额外补偿的发射功率 */
    /* SCAN */
    INIT_CFG_RANDOM_MAC_ADDR_SCAN,
    /* 11AC2G */
    INIT_CFG_11AC2G_ENABLE, /* 11ac2g开关 */ /* 63 */
    INIT_CFG_DISABLE_CAPAB_2GHT40,           /* 2ght40禁止开关 */

    /* FCC认证 */
    INIT_CFG_BAND_EDGE_LIMIT_2G_11G_TXPWR, /* FCC CH1,CH11 band edge limit */ /* 21 */
    INIT_CFG_BAND_EDGE_LIMIT_2G_11N_HT20_TXPWR,                               /* FCC CH1,CH11 band edge limit */
    INIT_CFG_BAND_EDGE_LIMIT_2G_11N_HT40_TXPWR,                               /* 39 */
    INIT_CFG_BAND_EDGE_LIMIT_2G_11G_DBB_SCALING,                              /* FCC CH1,CH11 dbb scaling */
    INIT_CFG_BAND_EDGE_LIMIT_2G_11N_HT20_DBB_SCALING,                         /* FCC CH1,CH11 dbb scaling */
    INIT_CFG_BAND_EDGE_LIMIT_2G_11N_HT40_DBB_SCALING,

    INIT_CFG_CALI_TONE_AMP_GRADE,

    /* base power/单位（0.1dbm）, 2.4G主路的base power
    basepower定义为芯片口出来的最大发射功率(例如21dBm)
    减去loss(通常为3~4dBm左右)得到basepower就是(18dBm，此时需要写入定制化项中的就是180)
    取值范围：DR最大功率值 ~ 120 */
    INIT_CFG_NVRAM_MAX_TXPWR_BASE_2P4G,
    /* delta power/单位（0.1dbm） */
    INIT_CFG_NVRAM_PARAMS0,
    INIT_CFG_NVRAM_PARAMS1,
    INIT_CFG_NVRAM_PARAMS2,
    /* DPD打开时，高阶功率值 */
    INIT_CFG_NVRAM_PARAMS3,
    /* 动态校准参数
    二次拟合和DPN系数请参考实验室动态校准拟合方式获取，调整前端匹配后，系数需要重新拟合
    自动化生成系数，二次项/一次项不超过给出给出参考值的10倍
    2.4G/5G 每个DPN差值绝对值不超过5dB */
    INIT_CFG_DYN_CALI_DSCR_INTERVAL,
    INIT_CFG_NVRAM_PA2GCCKA0, /* 50 */
    INIT_CFG_NVRAM_PA2GA0,
    INIT_CFG_NVRAM_PA2GCWA0,
    INIT_CFG_DPN24G_CH1_CORE0,
    INIT_CFG_DPN24G_CH2_CORE0,
    INIT_CFG_DPN24G_CH3_CORE0,
    INIT_CFG_DPN24G_CH4_CORE0,
    INIT_CFG_DPN24G_CH5_CORE0,
    INIT_CFG_DPN24G_CH6_CORE0,
    INIT_CFG_DPN24G_CH7_CORE0,
    INIT_CFG_DPN24G_CH8_CORE0, /* 60 */
    INIT_CFG_DPN24G_CH9_CORE0,
    INIT_CFG_DPN24G_CH10_CORE0,
    INIT_CFG_DPN24G_CH11_CORE0,
    INIT_CFG_DPN24G_CH12_CORE0,
    INIT_CFG_DPN24G_CH13_CORE0,
    INIT_CFG_DSSS2OFDM_DBB_PWR_BO_VAL,
    /* 校准开关 */
    INIT_CFG_CALI_DATA_MASK,
    INIT_CFG_CALI_MASK,
    /* RF前端定制化（不同产品需要重新适配该值） */
    INIT_CFG_RF_RX_INSERTION_LOSS_2G_B1,
    INIT_CFG_RF_RX_INSERTION_LOSS_2G_B2,
    INIT_CFG_RF_RX_INSERTION_LOSS_2G_B3,
    INIT_CFG_RF_LINE_RF_PWR_REF_RSSI_DB_2G_C0_MULT4,
    /* SAR Control相关NV */
    INIT_CFG_SAR_TXPWR_CTRL_2G,
    /* 边带功率控制相关NV */
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH1,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH2,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH3,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH4,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH5,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH6,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH7,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH8,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH9,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH10,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH11,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH12,
    INIT_CFG_SIDE_BAND_TXPWR_LIMIT_24G_CH13,
    /* RF reg寄存器定制化（涉及rf reg寄存器的定制项不建议修改，只用于hisi调试使用） */
    INIT_CFG_RF_PA_VDD_REG_100_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_101_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_102_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_103_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_104_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_105_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_106_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_107_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_108_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_109_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_110_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_111_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_112_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_113_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_114_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_115_MODE_1AND2,
    INIT_CFG_RF_PA_VDD_REG_100_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_101_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_102_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_103_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_104_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_105_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_106_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_107_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_108_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_109_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_110_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_111_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_112_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_113_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_114_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_115_MODE_3AND4,
    INIT_CFG_RF_PA_VDD_REG_100_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_101_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_102_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_103_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_104_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_105_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_106_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_107_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_108_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_109_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_110_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_111_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_112_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_113_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_114_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_115_MODE_5AND6,
    INIT_CFG_RF_PA_VDD_REG_100_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_101_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_102_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_103_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_104_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_105_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_106_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_107_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_108_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_109_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_110_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_111_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_112_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_113_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_114_MODE_7AND8,
    INIT_CFG_RF_PA_VDD_REG_115_MODE_7AND8,
    /* XTAL高温PLL频偏补偿定制化 */
    INIT_CFG_RF_HIGH_TEMP_THRESHOLD,
    INIT_CFG_RF_LOW_TEMP_THRESHOLD,
    INIT_CFG_RF_PPM_COMPESATION,
    /* DPD校准定制化（1131H改动）
    dpd_cali_ch_core0：DPD使用的校准信道，目前使用3个信道（视实际调试情况增加）；
    dpd_use_cail_ch_idx0_core0：BW20 CH1~CH8使用的系数的校准信道；
    dpd_use_cail_ch_idx1_core0：BW20 CH9~CH13使用的系数的校准信道；
    dpd_amp_t0_t3:dpd补偿中温度补偿数据（温度码0到3），每个数据8bit，占用两个16进制数位；
    dpd_amp_t4_t7:dpd补偿中温度补偿数据（温度码4到7）；
    dpd_comp_tpc_temp:是否开启TPC和温度补偿,0bit:开启温度补偿，
        bit:开启TPC补偿，
        2-3bit：软复位TPC门限，
        4bit:是否开启软复位,
        5bit:DPD补偿是否只补偿幅度;
    */
    INIT_CFG_DPD_CALI_CH_CORE0,
    INIT_CFG_DPD_USE_CAIL_CH_IDX0_CORE0,
    INIT_CFG_DPD_USE_CAIL_CH_IDX1_CORE0,
    INIT_CFG_DPD_AMP_T0_T3,
    INIT_CFG_DPD_AMP_T4_T7,
    INIT_CFG_DPD_COMP_TPC_TEMP,

    INIT_CFG_BUTT,
} wlan_cfg_init;

typedef enum {
    INIT_CONFIG_PARAMS = 0x20,
    INIT_CONFIG_CALIBRATION,
    INIT_CONFIG_RF_RX_INSERTION,
    INIT_CONFIG_BASE_DELTA_POWER,
    INIT_CONFIG_SAR_TXPWR,
    INIT_CONFIG_BAND_TXPWR,
    INIT_CONFIG_RF_REG1,
    INIT_CONFIG_RF_REG2,
    INIT_CONFIG_DPD_CALIBRATION,
    INIT_CONFIG_NVM_BUTT,
} wal_config_items;

#define INIT_CONFIG_XTAL_COMPESATION 1

/* *****************************************************************************
  3 结构体定义
***************************************************************************** */
typedef struct {
    hi_u32 init_cfg_nvram_max_txpwr_base_2p4g;

    hi_u32 init_cfg_nvram_params0;
    hi_u32 init_cfg_nvram_params1;
    hi_u32 init_cfg_nvram_params2;
    hi_u32 init_cfg_nvram_params3;
} wal_cfg_base_delta_power;

typedef struct {
    hi_u32 init_cfg_country_code;
    hi_u32 init_cfg_ampdu_tx_max_num;
    hi_u32 init_cfg_used_mem_for_start;
    hi_u32 init_cfg_used_mem_for_stop;
    hi_u32 init_cfg_rx_ack_limit;
    hi_u32 init_cfg_int_unit_ctrl;
    hi_u32 init_cfg_sdio_d2h_assemble_count;
    hi_u32 init_cfg_sdio_h2d_assemble_count;
    hi_u32 init_cfg_powermgmt_switch;
    hi_u32 init_cfg_sta_dtim_setting;
    hi_u32 init_cfg_loglevel;
    hi_u32 init_cfg_phy_scaling_value_11b;
    hi_u32 init_cfg_phy_u1_scaling_value_11g;
    hi_u32 init_cfg_phy_u2_scaling_value_11g;
    hi_u32 init_cfg_phy_u1_scaling_value_11n_2d4g;
    hi_u32 init_cfg_phy_u2_scaling_value_11n_2d4g;
    hi_u32 init_cfg_phy_u1_scaling_value_11n40m_2d4g;
    hi_u32 init_cfg_phy_u2_scaling_value_11n40m_2d4g;
    hi_u32 init_cfg_phy_u0_scaling_value_11n_5g;
    hi_u32 init_cfg_phy_u3_scaling_value_11n40m_5g;
    hi_u32 init_cfg_rts_clk_freq;
    hi_s32 init_cfg_clk_type;
    hi_s32 init_cfg_rf_line_txrx_gain_db_2g_band1;
    hi_s32 init_cfg_rf_line_txrx_gain_db_2g_band2;
    hi_s32 init_cfg_rf_line_txrx_gain_db_2g_band3;
    hi_u32 init_cfg_rf_txpwr_cali_ref_2g_val_band1;
    hi_u32 init_cfg_rf_txpwr_cali_ref_2g_val_band2;
    hi_u32 init_cfg_rf_txpwr_cali_ref_2g_val_band3;
    hi_u32 init_cfg_tx_ratio_level_0;
    hi_u32 init_cfg_tx_pwr_comp_val_level_0;
    hi_u32 init_cfg_tx_ratio_level_1;
    hi_u32 init_cfg_tx_pwr_comp_val_level_1;
    hi_u32 init_cfg_tx_ratio_level_2;
    hi_u32 init_cfg_tx_pwr_comp_val_level_2;
    hi_u32 init_cfg_more_pwr;
    hi_u32 init_cfg_random_mac_addr_scan;
    hi_u32 init_cfg_11ac2g_enable;
    hi_u32 init_cfg_disable_capab_2ght40;
    hi_u32 init_cfg_band_edge_limit_2g_11g_txpwr;
    hi_u32 init_cfg_band_edge_limit_2g_11n_ht20_txpwr;
    hi_u32 init_cfg_band_edge_limit_2g_11n_ht40_txpwr;
    hi_u32 init_cfg_band_edge_limit_2g_11g_dbb_scaling;
    hi_u32 init_cfg_band_edge_limit_2g_11n_ht20_dbb_scaling;
    hi_u32 init_cfg_band_edge_limit_2g_11n_ht40_dbb_scaling;
    hi_u32 init_cfg_cali_tone_amp_grade;
} wal_cfg_params;

typedef struct {
    hi_u32 init_cfg_dyn_cali_dscr_interval;

    hi_u32 init_cfg_nvram_pa2gccka0;
    hi_u32 init_cfg_nvram_pa2ga0;
    hi_u32 init_cfg_nvram_pa2gcwa0;

    hi_u32 init_cfg_dpn24g_ch1_core0;
    hi_u32 init_cfg_dpn24g_ch2_core0;
    hi_u32 init_cfg_dpn24g_ch3_core0;
    hi_u32 init_cfg_dpn24g_ch4_core0;
    hi_u32 init_cfg_dpn24g_ch5_core0;
    hi_u32 init_cfg_dpn24g_ch6_core0;
    hi_u32 init_cfg_dpn24g_ch7_core0;
    hi_u32 init_cfg_dpn24g_ch8_core0;
    hi_u32 init_cfg_dpn24g_ch9_core0;
    hi_u32 init_cfg_dpn24g_ch10_core0;
    hi_u32 init_cfg_dpn24g_ch11_core0;
    hi_u32 init_cfg_dpn24g_ch12_core0;
    hi_u32 init_cfg_dpn24g_ch13_core0;

    hi_u32 init_cfg_dsss2ofdm_dbb_pwr_bo_val;

    hi_u32 init_cfg_cali_data_mask;
    hi_u32 init_cfg_cali_mask;
} wal_cfg_calibration;

typedef struct {
    hi_u32 init_cfg_rf_rx_insertion_loss_2g_b1;
    hi_u32 init_cfg_rf_rx_insertion_loss_2g_b2;
    hi_u32 init_cfg_rf_rx_insertion_loss_2g_b3;
    hi_u32 init_cfg_rf_line_rf_pwr_ref_rssi_db_2g_c0_mult4;
} wal_cfg_rf_rx_insertion;

typedef struct {
    hi_u32 init_cfg_sar_txpwr_ctrl_2g;
} wal_cfg_sar_txpwr;

typedef struct {
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch1;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch2;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch3;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch4;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch5;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch6;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch7;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch8;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch9;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch10;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch11;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch12;
    hi_u32 init_cfg_side_band_txpwr_limit_24g_ch13;
} wal_cfg_band_txpwr;

typedef struct {
    hi_u32 init_cfg_rf_pa_vdd_reg_100_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_101_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_102_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_103_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_104_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_105_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_106_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_107_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_108_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_109_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_110_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_111_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_112_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_113_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_114_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_115_mode_1and2;
    hi_u32 init_cfg_rf_pa_vdd_reg_100_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_101_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_102_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_103_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_104_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_105_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_106_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_107_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_108_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_109_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_110_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_111_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_112_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_113_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_114_mode_3and4;
    hi_u32 init_cfg_rf_pa_vdd_reg_115_mode_3and4;
} wal_cfg_rf_reg1;

typedef struct {
    hi_u32 init_cfg_rf_pa_vdd_reg_100_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_101_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_102_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_103_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_104_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_105_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_106_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_107_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_108_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_109_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_110_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_111_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_112_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_113_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_114_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_115_mode_5and6;
    hi_u32 init_cfg_rf_pa_vdd_reg_100_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_101_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_102_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_103_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_104_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_105_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_106_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_107_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_108_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_109_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_110_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_111_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_112_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_113_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_114_mode_7and8;
    hi_u32 init_cfg_rf_pa_vdd_reg_115_mode_7and8;
} wal_cfg_rf_reg2;

typedef struct {
    hi_s32 init_cfg_rf_high_temp_threshold;
    hi_s32 init_cfg_rf_low_temp_threshold;
    hi_s32 init_cfg_rf_ppm_compesation;
} rf_cfg_xtal_compesation;

typedef struct {
    hi_u32 init_cfg_dpd_cali_ch_core0;
    hi_u32 init_cfg_dpd_use_cail_ch_idx0_core0;
    hi_u32 init_cfg_dpd_use_cail_ch_idx1_core0;
    hi_u32 init_cfg_dpd_amp_t0_t3;
    hi_u32 init_cfg_dpd_amp_t4_t7;
    hi_u32 init_cfg_dpd_comp_tpc_temp;
} wal_cfg_dpd_calibration;

typedef struct {
    hi_s32 rssi_offset;
    hi_s32 freq_comp[WAL_FREQ_COMP_PARAM_CNT];
    hi_u32 dbb_params[WAL_DBB_PARAM_CNT];
    hi_u32 ch_txpwr_offset[WAL_CH_TX_PWR_PARAM_CNT];
} wal_customize_params;

/* *****************************************************************************
 * 10 Function Declare
 * **************************************************************************** */
hi_u32 wal_get_init_value(hi_u32 cfg_id);
hi_u32 wal_customize_init(hi_void);
hi_u32 wal_customize_exit(hi_void);
hi_u32 wal_customize_set_config(hi_void);
#ifdef _PRE_DEBUG_MODE
hi_u32 wal_print_init_params(oal_net_device_stru *netdev, hi_char *pc_param);
#endif
hi_u32 wal_set_init_value(hi_u32 cfg_id, const hi_u32 *data, hi_u8 size);
hi_u32 wal_cfg_dbb(const hi_u32 *data, hi_u8 size);
hi_u32 wal_cfg_country_code(const hi_char *country_code, hi_s32 size);
hi_u32 wal_cfg_fcc_tx_pwr(const hi_u32 *data, hi_u8 size);
hi_u32 wal_cfg_freq_comp_val(const hi_u32 *data, hi_u8 size);
hi_u32 wal_cfg_rssi_ofset(hi_s32 data);
hi_void wal_set_boot_current_flag(hi_bool minimize);
hi_u32 wal_sync_boot_current_to_dev(const hi_char *ifname);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
