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
#include "hmac_cali_dpd.h"
#include "oam_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数声明
**************************************************************************** */
#define get2comp(a) (((a) >= 1024) ? ((a) - 2048) : (a))
#define set2comp(a) (((a) >= 0) ? (a) : (2048 + (a)))

hi_s64 g_ll_mk1_32[3][128] = {
    /* 3 128 元素个数 */
    {
        297,    869,    1414,    1931,    2423,    2888,    3328,    3742,    4132,    4498,    4840,    5158,\
        5454,    5727,    5979,    6209,    6418,   6607,    6775,    6924,    7054,    7165,    7258,    7333,\
        7391,    7432,    7457,    7466,    7459,    7437,    7401,    7351,    7287,    7210,    7121,    7019,\
        6906,    6781,    6645,    6499,    6343,    6178,    6004,    5821,    5630,    5431,    5226,    5014,\
        4795,    4571,    4341,    4107,    3868,    3626,    3380,    3131,    2879,    2626,    2370,    2114,\
        1857,    1600,    1343,    1087,    832,    578,    327,    78,    -167,    -410,    -648,    -882,    -1111,\
        -1334,    -1552,    -1764,    -1969,    -2166,    -2357,    -2539,    -2712,    -2876,    -3031,    -3176,\
        -3311,    -3434,    -3547,    -3647,    -3736,    -3811,    -3874,    -3922,    -3957,    -3977,    -3982,\
        -3972,    -3946,    -3903,    -3843,    -3766,    -3671,    -3558,    -3427,    -3276,    -3105,    -2915,\
        -2703,    -2471,    -2217,    -1942,    -1644,    -1323,    -978,    -610,    -218,    199,    641,    1109,\
        1603,    2124,      2671,    3246,    3849,    4481,    5141,    5830,    6550,    7300,
    },
    {
        -889,    -2600,    -4224,    -5761,    -7214,    -8584,    -9872,    -11080,    -12209,    -13262,    -14239,\
        -15142,    -15973,    -16733,    -17424,    -18047,    -18605,      -19097,    -19526,    -19894,    -20202,\
        -20451,    -20643,    -20780,    -20863,    -20894,    -20873,    -20804,    -20687,    -20524,    -20316,\
        -20065,    -19772,    -19440,    -19068,    -18660,    -18217,    -17740,    -17230,    -16690,    -16120,\
        -15523,    -14899,    -14251,    -13579,    -12887,    -12174,    -11442,    -10694,    -9930,    -9153,\
        -8363,    -7562,    -6752,    -5935,    -5111,    -4282,    -3451,    -2618,    -1785,    -953,    -124,\
        699,    1517,    2327,    3128,    3918,    4696,    5460,    6209,    6941,    7654,    8347,    9019,\
        9668,    10292,    10890,    11461,    12002,    12512,    12990,    13434,    13843,    14215,    14549,\
        14842,    15094,    15303,    15468,    15586,    15656,    15678,    15648,    15566,    15431,    15240,\
        14992,    14686,    14319,    13891,    13400,    12845,    12223,    11534,    10775,    9946,    9045,\
        8069,    7018,    5891,    4684,    3398,    2030,    580,    -956,    -2577,    -4286,    -6085,    -7974,\
        -9956,    -12031,    -14202,    -16469,    -18835,    -21301,    -23869,    -26540,    -29315,
    },
    {
        622,    1817,    2949,    4017,    5025,    5971,    6859,    7688,    8460,    9176,    9838,    10446,\
        11002,    11506,    11961,    12366,    12724,    13035,    13301,    13522,    13701,    13837,    13932,\
        13988,    14005,    13985,    13929,    13837,    13712,    13554,    13364,    13144,    12895,    12617,\
        12312,    11982,    11627,    11248,    10847,    10425,    9983,    9522,    9043,    8547,    8036,    7511,\
        6973,    6422,    5861,    5290,    4710,    4124,    3531,    2933,    2331,    1726,    1119,    513,\
        -94,    -698,    -1299,    -1896,    -2488,    -3073,    -3651,    -4219,    -4778,    -5326,    -5862,\
        -6384,    -6891,    -7383,    -7858,    -8315,    -8753,    -9171,    -9567,    -9940,    -10290,    -10614,\
        -10913,    -11184,    -11427,    -11640,    -11823,    -11974,    -12092,    -12175,    -12223,    -12235,\
        -12209,    -12145,    -12040,    -11894,    -11706,    -11475,    -11199,    -10877,    -10509,    -10092,\
        -9627,    -9111,    -8543,    -7923,    -7249,    -6520,    -5735,    -4893,    -3992,    -3032,    -2011,\
        -928,    218,    1428,    2703,    4045,    5454,    6933,    8481,    10100,    11792,    13557,    15396,\
        17312,    19304,    21374,    23523,    25753,
    },
};

hi1131_complex_stru g_ll_pa_val[128] = {
    /* 128 元素个数 */
    {1, 0},
    {5, -1},
    {8, -3},
    {12, -4},
    {15, -5},
    {18, -6},
    {22, -7},
    {26, -8},
    {29, -9},
    {33, -11},
    {36, -12},
    {40, -13},
    {43, -14},
    {46, -15},
    {50, -16},
    {53, -18},
    {57, -19},
    {60, -20},
    {64, -21},
    {67, -22},
    {71, -24},
    {74, -25},
    {77, -26},
    {81, -27},
    {84, -28},
    {88, -29},
    {91, -31},
    {94, -32},
    {98, -33},
    {101, -34},
    {105, -35},
    {108, -37},
    {111, -38},
    {115, -39},
    {118, -40},
    {121, -42},
    {124, -43},
    {128, -44},
    {131, -45},
    {135, -46},
    {138, -47},
    {141, -49},
    {144, -50},
    {147, -51},
    {151, -52},
    {154, -53},
    {157, -55},
    {160, -56},
    {164, -57},
    {167, -58},
    {170, -59},
    {173, -60},
    {176, -61},
    {180, -63},
    {183, -64},
    {187, -65},
    {190, -66},
    {193, -67},
    {196, -68},
    {200, -69},
    {203, -70},
    {206, -71},
    {210, -72},
    {213, -73},
    {216, -75},
    {220, -75},
    {223, -76},
    {227, -78},
    {230, -79},
    {234, -80},
    {238, -81},
    {241, -82},
    {245, -83},
    {248, -84},
    {251, -85},
    {255, -86},
    {259, -87},
    {262, -89},
    {266, -90},
    {270, -91},
    {273, -92},
    {277, -93},
    {281, -94},
    {284, -96},
    {288, -97},
    {292, -98},
    {296, -100},
    {300, -101},
    {304, -102},
    {307, -103},
    {311, -104},
    {316, -105},
    {319, -107},
    {323, -108},
    {327, -109},
    {331, -111},
    {335, -112},
    {339, -114},
    {343, -115},
    {347, -116},
    {351, -118},
    {355, -119},
    {359, -121},
    {363, -122},
    {367, -123},
    {371, -125},
    {375, -126},
    {380, -128},
    {384, -129},
    {388, -131},
    {392, -132},
    {396, -134},
    {400, -135},
    {404, -137},
    {409, -138},
    {413, -140},
    {417, -142},
    {421, -143},
    {425, -144},
    {429, -146},
    {433, -148},
    {437, -149},
    {441, -151},
    {445, -153},
    {449, -154},
    {453, -156},
    {457, -157},
    {460, -159},
};

hi_u16 g_us_max_pos = 128;
hi_u32 g_dpd_data[128]; /* dpd calibration data 128: 元素个数 */

/* ****************************************************************************
 函 数 名  : hi1131_cali_get_phase
 功能描述  : 两整数相除结果四舍五入
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_s64 hmac_cali_div_round_closest(hi_s64 ll_a, hi_s64 ll_b)
{
    hi_s64 ll_result = 0;
    if ((ll_b == 0) || ((ll_b << 1) == 0)) {
        ll_result = (hi_s64)ll_a;
    } else {
        if (((ll_a >= 0) && (ll_b > 0)) || ((ll_a <= 0) && (ll_b < 0))) {
            ll_result = (hi_s64)(((ll_a << 1) + ll_b) / (ll_b << 1));
        } else {
            ll_result = (hi_s64)(((ll_a << 1) - ll_b) / (ll_b << 1));
        }
    }

    return ll_result;
}

/* ****************************************************************************
 函 数 名  : hi1131_cali_get_phase
 功能描述  : 复数除以整数结果四舍五入
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi1131_complex_stru complex_div_round_closest(hi1131_complex_stru a, hi_s64 ll_b)
{
    hi1131_complex_stru result;

    result.ll_real = hmac_cali_div_round_closest(a.ll_real, ll_b);
    result.ll_imag = hmac_cali_div_round_closest(a.ll_imag, ll_b);

    return result;
}

/* ****************************************************************************
 函 数 名  : dpd_cordic
 功能描述  : DPD Calibration Data Cordic
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_dpd_cordic(hi_s64 ll_real, hi_s64 ll_image, hi_s64 *pll_amp, hi_s64 *pll_theta1, hi_s64 *pll_theta2)
{
    hi_u8   us_idx = 0;
    hi_s64  ll_x[13] = {0}; /* 13 元素个数 */
    hi_s64  ll_y[13] = {0}; /* 13 元素个数 */
    hi_s64  ll_flag;

    hi_s64  ll_real_table[13] = {1447, 1891, 2008, 2037, 2045, 2046, 2047, 2047, 2047, 2047, 2047, 2047, 2047}; /* 13 */
    hi_s64  ll_imag_table[13] = {1447, 783, 399, 201, 100, 50, 25, 13, 6, 3, 2, 1, 0}; /* 元素个数为13 */

    hi_s64  ll_theta1 = 2047;
    hi_s64  ll_theta2 = 0;
    hi_s64  ll_temp1 = 0;
    hi_s64  ll_temp2 = 0; /* real and image */
    hi_s64  ll_thetatmp1;
    hi_s64  ll_thetatmp2; /* real and image */
    hi_s64  ll_divider = 2048;

    ll_x[0] = oal_abs(ll_real);
    ll_y[0] = oal_abs(ll_image);
    ll_flag = oal_abs(ll_image);

    for (us_idx = 0; us_idx < 10; us_idx++) { /* 循环次数为10 */
        if (ll_flag > 0) {
            ll_temp1 = ll_real_table[us_idx];
            ll_temp2 = -ll_imag_table[us_idx];
        } else if (ll_flag < 0) {
            ll_temp1 = ll_real_table[us_idx];
            ll_temp2 = ll_imag_table[us_idx];
        } else { /* y = 0 */
            break;
        }

        ll_x[us_idx + 1] = hmac_cali_div_round_closest((ll_x[us_idx] * ll_temp1 - ll_y[us_idx] * ll_temp2), ll_divider);
        ll_y[us_idx + 1] = hmac_cali_div_round_closest((ll_y[us_idx] * ll_temp1 + ll_x[us_idx] * ll_temp2), ll_divider);

        ll_thetatmp1 = hmac_cali_div_round_closest((ll_theta1 * ll_temp1 - ll_theta2 * ll_temp2), ll_divider);
        ll_thetatmp2 = hmac_cali_div_round_closest((ll_theta2 * ll_temp1 + ll_theta1 * ll_temp2), ll_divider);
        ll_theta1 = ll_thetatmp1;
        ll_theta2 = ll_thetatmp2;

        ll_flag = ll_y[us_idx + 1];
    }

    if (ll_real < 0) {
        ll_theta1 = -ll_theta1;
    }
    if (ll_image < 0) {
        ll_theta2 = -ll_theta2;
    }

    *pll_amp = (hi_u32)ll_x[us_idx];
    *pll_theta1 = (hi_s32)ll_theta1;
    *pll_theta2 = (hi_s32)ll_theta2;
}

/* ****************************************************************************
 功能描述  : 对复数求共轭
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi1131_complex_stru hmac_dpd_cali_conj(hi1131_complex_stru a)
{
    a.ll_imag = -a.ll_imag;

    return a;
}

/* ****************************************************************************
 函 数 名  : hmac_dpd_complex_add
 功能描述  : 复数相乘
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi1131_complex_stru hmac_dpd_complex_add(hi1131_complex_stru a, hi1131_complex_stru b)
{
    hi1131_complex_stru return_code;

    return_code.ll_real = (hi_s64)(a.ll_real + b.ll_real);
    return_code.ll_imag = (hi_s64)(a.ll_imag + b.ll_imag);

    return return_code;
}

/* ****************************************************************************
 函 数 名  : hmac_dpd_complex_multiply_const
 功能描述  : 复数相乘
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi1131_complex_stru hmac_dpd_complex_multiply_const(hi1131_complex_stru a, hi_s64 ll_b)
{
    hi1131_complex_stru return_code;

    return_code.ll_real = (hi_s64)(a.ll_real * ll_b);
    return_code.ll_imag = (hi_s64)(a.ll_imag * ll_b);

    return return_code;
}

/* ****************************************************************************
 函 数 名  : hi1131_cali_complex_multiply
 功能描述  : 复数相乘
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi1131_complex_stru hmac_dpd_complex_multiply(hi1131_complex_stru a, hi1131_complex_stru b)
{
    hi1131_complex_stru return_code;

    return_code.ll_real = (hi_s64)((hi_s64)(a.ll_real) * (hi_s64)(b.ll_real) -
        (hi_s64)(a.ll_imag) * (hi_s64)(b.ll_imag));
    return_code.ll_imag = (hi_s64)((hi_s64)(a.ll_real) * (hi_s64)(b.ll_imag) +
        (hi_s64)(a.ll_imag) * (hi_s64)(b.ll_real));

    return return_code;
}

/* ****************************************************************************
 函 数 名  : hi1131_cali_complex_div
 功能描述  : 复数除法
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年3月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi1131_complex_stru hmac_dpd_complex_div(hi1131_complex_stru complex_num, hi1131_complex_stru complex_denom)
{
    hi1131_complex_stru mul_res;
    hi1131_complex_stru conj_res;
    hi_s64 ll_power;
    hi_s64 ll_real;
    hi_s64 ll_imag;
    hi_s64 ll_temp;

    /* 除数取共轭 */
    conj_res = hmac_dpd_cali_conj(complex_denom);

    /* 复数乘法 被除数*conj(除数) */
    mul_res = hmac_dpd_complex_multiply(complex_num, conj_res);

    /* 计算Power */
    ll_real = complex_denom.ll_real;
    ll_imag = complex_denom.ll_imag;
    ll_power = ll_real * ll_real + ll_imag * ll_imag;

    /* 如果分母为0，则上报错误 */
    if (ll_power == 0) {
        oam_warning_log0(0, OAM_SF_CALIBRATE, "hi1131_cali_complex_div: l_power is zero!\n");
        return mul_res;
    } else {
        /* 实部 四舍五入 */
        ll_temp = (((mul_res.ll_real) % ll_power) << 1);
        mul_res.ll_real = ((mul_res.ll_real) / ll_power);
        if (ll_power < oal_abs(ll_temp)) {
            if (ll_temp > 0) {
                mul_res.ll_real = mul_res.ll_real + 1;
            } else {
                mul_res.ll_real = mul_res.ll_real - 1;
            }
        }
        /* 虚部 四舍五入 */
        ll_temp = (((mul_res.ll_imag) % ll_power) << 1);
        mul_res.ll_imag = ((mul_res.ll_imag) / ll_power);
        if (ll_power < oal_abs(ll_temp)) {
            if (ll_temp > 0) {
                mul_res.ll_imag = mul_res.ll_imag + 1;
            } else {
                mul_res.ll_imag = mul_res.ll_imag - 1;
            }
        }
    }

    /* 返回结果 */
    return mul_res;
}

/* ****************************************************************************
 函 数 名  : hmac_dpd_complex_div_const
 功能描述  : 复数除以整数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi1131_complex_stru hmac_dpd_complex_div_const(hi1131_complex_stru a, hi_s64 ll_b)
{
    hi1131_complex_stru return_code;

    return_code.ll_real = (hi_s64)(a.ll_real >> ll_b);
    return_code.ll_imag = (hi_s64)(a.ll_imag >> ll_b);

    return return_code;
}

/* ****************************************************************************
 函 数 名  : hmac_dpd_complex_div_const
 功能描述  : 复数相乘
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
inline hi_s64 hmac_dpd_complex_pow(hi1131_complex_stru a)
{
    return (hi_s64)((hi_s64)(a.ll_real) * (hi_s64)(a.ll_real) + (hi_s64)(a.ll_imag) * (hi_s64)(a.ll_imag));
}

hi_void hmac_init_ll_pa_val(const hi_u32 *hi1131_dpd_cali_data_read)
{
    hi_u16 us_idx;
    hi_s32 l_real, l_imag;

    for (us_idx = 0; us_idx < DPD_CALI_LUT_LENGTH; us_idx++) {
        l_real = get2comp((((hi_u32)(*(hi1131_dpd_cali_data_read + us_idx))) & 0x7ff));
        l_imag = get2comp(((((hi_u32)(*(hi1131_dpd_cali_data_read + us_idx))) >> 11) & 0x7ff));

        oam_warning_log2(0, OAM_SF_CALIBRATE, "{hi1131_dpd_cali_data_read:: l_real = %d, l_imag = %d}", l_real, l_imag);

        g_ll_pa_val[us_idx].ll_real = (hi_s64)l_real;
        g_ll_pa_val[us_idx].ll_imag = (hi_s64)l_imag;
    }
}

static inline hi_void hmac_init_dpd_sum(hi1131_complex_stru *sum, hi_u16 us_j)
{
    hi_u16 us_idx;

    sum->ll_real = 0;
    sum->ll_imag = 0;
    for (us_idx = 0; us_idx < DPD_CALI_LUT_LENGTH ; us_idx++) {
        *sum = hmac_dpd_complex_add(*sum, hmac_dpd_complex_multiply_const(g_ll_pa_val[us_idx],
            g_ll_mk1_32[us_j][us_idx]));
    }
}

/* 规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 算法函数，功能内聚，建议屏蔽 */
hi_u32 hmac_rf_cali_dpd_corr_calc(const hi_u32 *hi1131_dpd_cali_data_read, hi_u32 *hi1131_dpd_cali_data_calc)
{
    hi_u16 us_idx, us_j;
    hi_s64 ll_amp = 0;
    hi1131_complex_stru vv1_32, vv0_32, xll_32, yll_32, dpd_lut_fixed, tmp1, tmp2, y_opt_32, sum;
    hi1131_complex_stru ak_16[3]; /* 3:元素个数 */
    hi_u32 vo_fixed;
    hi_s64 ll_theta0, ll_theta1;
    hi_s64 ll_dpd_base = 0;

    /* 初始化g_ll_pa_val */
    hmac_init_ll_pa_val(hi1131_dpd_cali_data_read);
    /* g_ll_pa_val上报值, g_ll_mk1_32给定矩阵值 */
    for (us_j = 0; us_j < 3; us_j++) { /* 循环3次 */
        hmac_init_dpd_sum(&sum, us_j);
        ak_16[us_j] = complex_div_round_closest(sum, 1 << 11); /* round(oal_sum/BIT28); 11: 左移11位 */

        oam_warning_log3(0, OAM_SF_CALIBRATE, "st_ak_16[%d]=%d+j%d\n", us_j, ak_16[us_j].ll_real, ak_16[us_j].ll_imag);
    }

    for (us_idx = 0; us_idx < DPD_CALI_LUT_LENGTH; us_idx++) {
        vv1_32.ll_real = 512 * (1 << 5); /* 512 用于计算 左移5位 */
        vv1_32.ll_imag = 0;

        vo_fixed = 4 + 8 * us_idx; /* 4 8 用于计算 */

        for (us_j = 0; us_j < 10; us_j++) { /* 10 循环次数 */
            xll_32 = complex_div_round_closest(hmac_dpd_complex_multiply_const(vv1_32, vo_fixed), 1 << 7); /* 7:bits */
            hmac_dpd_cordic(xll_32.ll_real, xll_32.ll_imag, &ll_amp, &ll_theta0, &ll_theta1);

            tmp1 = complex_div_round_closest(hmac_dpd_complex_multiply_const(ak_16[1], ll_amp), 1 << 17); /* 17:bits */
            tmp1 = hmac_dpd_complex_add(ak_16[0], tmp1);
            tmp2 = hmac_dpd_complex_multiply_const(ak_16[2], hmac_dpd_complex_pow(xll_32)); /* 2 元素索引 */
            tmp2 = complex_div_round_closest(tmp2, ((hi_s64)1 << 34));                      /* 左移34位 */
            vv0_32 = hmac_dpd_complex_add(tmp1, tmp2);
            yll_32 = complex_div_round_closest(hmac_dpd_complex_multiply(xll_32, vv0_32), 1 << 9);   /* 9 */
            y_opt_32 = hmac_dpd_complex_multiply_const(g_ll_pa_val[127], ((hi_s64)(vo_fixed) << 2)); /* 127 左移2 */
            vv1_32 = hmac_dpd_complex_div(hmac_dpd_complex_multiply(y_opt_32, vv1_32), yll_32);

            vv1_32.ll_real = oal_max(-((1 << 15) - 1), oal_min((1 << 15) - 1, vv1_32.ll_real)); /* 左移15 */
            vv1_32.ll_imag = oal_max(-((1 << 15) - 1), oal_min((1 << 15) - 1, vv1_32.ll_imag)); /* 左移15 */
        }

        dpd_lut_fixed = complex_div_round_closest(vv1_32, 1 << 5); /* 左移5位 round(vv1_32/BIT5); */
        if (us_idx == 0) {
            ll_dpd_base = dpd_lut_fixed.ll_real;
            if (ll_dpd_base == 0) {
                oam_error_log0(0, OAM_SF_CALIBRATE, " dpd: the first dpd lut elements if zero!!!\r\n");
            }
        }
        dpd_lut_fixed = complex_div_round_closest(hmac_dpd_complex_multiply_const(dpd_lut_fixed, 512),
            ll_dpd_base); /* 512 函数入参 */
        *(hi1131_dpd_cali_data_calc + us_idx) =
            (set2comp((hi_s32)dpd_lut_fixed.ll_real)) | ((set2comp((hi_s32)dpd_lut_fixed.ll_imag)) << 11); /* 左移11 */
        oam_warning_log2(0, OAM_SF_CALIBRATE, "hi1131_dpd_cali_data_calc::real = %d, imag = %d\r\n",
            (hi_s32)dpd_lut_fixed.ll_real, (hi_s32)dpd_lut_fixed.ll_imag);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
