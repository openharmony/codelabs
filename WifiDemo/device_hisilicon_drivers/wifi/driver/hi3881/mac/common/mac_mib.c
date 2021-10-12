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

#include "oal_mem.h"
#include "wlan_types.h"
#include "mac_vap.h"
#include "mac_device.h"
#include "mac_regdomain.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* ****************************************************************************
 功能描述  : 设置Mesh ID值
 输入参数  : [1]mac_vap mac vap结构体
             [2]len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_mib_set_meshid(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    mac_cfg_ssid_param_stru *param = HI_NULL;
    hi_u8 *puc_mib_meshid = HI_NULL;
    hi_u8 meshid_len;

    hi_unref_param(len);

    param = (mac_cfg_ssid_param_stru *)puc_param;
    meshid_len = param->ssid_len; /* 长度不包括字符串结尾'\0' */
    /* uc_len 长度不包括字符串结尾'\0' */
    if (meshid_len > WLAN_SSID_MAX_LEN - 1) {
        meshid_len = WLAN_SSID_MAX_LEN - 1;
    }
    puc_mib_meshid = mac_vap->mib_info->wlan_mib_mesh_sta_cfg.auc_dot11_mesh_id;
    if (memcpy_s(puc_mib_meshid, WLAN_SSID_MAX_LEN, param->ac_ssid, meshid_len) != EOK) {
        return HI_FAIL;
    }
    puc_mib_meshid[meshid_len] = '\0';
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取Mesh ID值
 输入参数  : [1]mac_vap mac vap结构体
             [2]puc_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_mib_get_meshid(const mac_vap_stru *mac_vap, hi_u8 *puc_len, const hi_u8 *puc_param)
{
    mac_cfg_ssid_param_stru *param = HI_NULL;
    hi_u8 meshid_len;
    hi_u8 *puc_mib_meshid = HI_NULL;

    puc_mib_meshid = mac_vap->mib_info->wlan_mib_mesh_sta_cfg.auc_dot11_mesh_id;
    meshid_len = (hi_u8)strlen((hi_char *)puc_mib_meshid);

    param = (mac_cfg_ssid_param_stru *)puc_param;

    param->ssid_len = meshid_len;
    if (memcpy_s(param->ac_ssid, WLAN_SSID_MAX_LEN, puc_mib_meshid, meshid_len) != EOK) {
        return HI_FAIL;
    }

    *puc_len = sizeof(mac_cfg_ssid_param_stru);

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 获取实现的pairwise cipher suites个数
 输入参数  : pst_mac_vap: 指向vap
 输出参数  : puc_num    : 实现的pairwise cipher suites个数
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_mib_get_wpa_pairwise_cipher_suite(const mac_vap_stru *mac_vap, hi_u8 *puc_num)
{
    hi_u8 num = 0;
    hi_u8 loop;
    wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru *wlan_mib_rsna_cfg_wpa_pairwise_cipher;

    wlan_mib_rsna_cfg_wpa_pairwise_cipher = mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher;

    for (loop = 0; loop < WLAN_PAIRWISE_CIPHER_SUITES; loop++) {
        if (wlan_mib_rsna_cfg_wpa_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_activated) {
            num++;
        }
    }
    *puc_num = num;
}

/* ****************************************************************************
 功能描述  : 获取实现的pairwise cipher suites个数
 输入参数  : pst_mac_vap: 指向vap
 输出参数  : puc_num    : 实现的pairwise cipher suites个数
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_mib_get_wpa2_pairwise_cipher_suite(const mac_vap_stru *mac_vap, hi_u8 *puc_num)
{
    hi_u8 num = 0;
    hi_u8 loop;
    wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru *wlan_mib_rsna_cfg_wpa2_pairwise_cipher =
        mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher;

    for (loop = 0; loop < WLAN_PAIRWISE_CIPHER_SUITES; loop++) {
        if (wlan_mib_rsna_cfg_wpa2_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_activated) {
            num++;
        }
    }
    *puc_num = num;
}

/* ****************************************************************************
 功能描述  : 获取实现的authentication suites个数
 输入参数  : pst_mac_vap: 指向vap
 输出参数  : puc_num    : 实现的authentication suites个数
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2019年1月23日
    作    者   : HiSilicon
    修改内容   : 新增参数uc_is_mesh_rsn

**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_mib_get_authentication_suite(const mac_vap_stru *mac_vap, hi_u8 *puc_num)
{
    hi_u8 num = 0;
    hi_u8 loop;
    hi_u8 auth_suite_num = MAC_AUTHENTICATION_SUITE_NUM;
    wlan_mib_dot11_rsna_cfg_authentication_suites_en_stru *wlan_mib_rsna_cfg_auth_suite =
        mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite;

    for (loop = 0; loop < auth_suite_num; loop++) {
        if (wlan_mib_rsna_cfg_auth_suite[loop].dot11_rsna_config_authentication_suite_activated) {
            num++;
        }
    }
    *puc_num = num;
}

/* ****************************************************************************
 功能描述  : 获取实现的pairwise cipher suites 加密套件值
 输入参数  : pst_mac_vap        : 指向vap
 输出参数  : puc_pairwise_value : 实现的pairwise cipher suites 加密套件值
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_mib_get_wpa2_pairwise_cipher_suite_value(const mac_vap_stru *mac_vap,
    hi_u8 *puc_pairwise_value, hi_u8 pairwise_len)
{
    hi_u8 loop;
    hi_u8 pairwise_index = 0;
    wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru *wlan_mib_rsna_cfg_wpa2_pairwise_cipher;

    wlan_mib_rsna_cfg_wpa2_pairwise_cipher = mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher;

    for (loop = 0; loop < pairwise_len; loop++) {
        if (wlan_mib_rsna_cfg_wpa2_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_activated) {
            puc_pairwise_value[pairwise_index++] =
                wlan_mib_rsna_cfg_wpa2_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_implemented;
        }
    }
}

/* ****************************************************************************
 功能描述  : 获取实现的pairwise cipher suites 加密套件值
 输入参数  : pst_mac_vap        : 指向vap
 输出参数  : puc_pairwise_value : 实现的pairwise cipher suites 加密套件值
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_mib_get_wpa_pairwise_cipher_suite_value(const mac_vap_stru *mac_vap,
    hi_u8 *puc_pairwise_value, hi_u8 pairwise_len)
{
    hi_u8 loop;
    hi_u8 pairwise_index = 0;
    wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru *wlan_mib_rsna_cfg_wpa_pairwise_cipher;

    wlan_mib_rsna_cfg_wpa_pairwise_cipher = mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher;

    for (loop = 0; loop < pairwise_len; loop++) {
        if (wlan_mib_rsna_cfg_wpa_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_activated) {
            puc_pairwise_value[pairwise_index++] =
                wlan_mib_rsna_cfg_wpa_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_implemented;
        }
    }
}

/* ****************************************************************************
 功能描述  : 设置 RSN认证套件信息
 修改历史      :
  1.日    期   : 2014年08月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_mib_set_rsna_auth_suite(const mac_vap_stru *mac_vap, hi_u8 auth_value)
{
    hi_u8 index;

    if ((auth_value == WLAN_AUTH_SUITE_1X) || (auth_value == WLAN_AUTH_SUITE_PSK)) {
        index = 0;
    } else if ((auth_value == WLAN_AUTH_SUITE_1X_SHA256) || (auth_value == WLAN_AUTH_SUITE_PSK_SHA256)) {
        index = 1;
    } else {
        return;
    }
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[index].dot11_rsna_config_authentication_suite_implemented =
        auth_value;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[index].dot11_rsna_config_authentication_suite_activated =
        HI_TRUE;
}

WIFI_ROM_TEXT hi_void mac_mib_set_rsnaconfig_wpa_pairwise_cipher_implemented(const mac_vap_stru *mac_vap,
    hi_u8 pairwise_value)
{
    hi_u8 index;

    if (pairwise_value == WLAN_80211_CIPHER_SUITE_CCMP) {
        index = 0;
    } else if (pairwise_value == WLAN_80211_CIPHER_SUITE_TKIP) {
        index = 1;
    } else {
        hi_diag_log_msg_e1(0, "{mac_mib_set_RSNAConfigWpaPairwiseCipherImplemented::invalid uc_pairwise_value[%d].}",
            pairwise_value);
        return;
    }
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[index].dot11_rsna_config_pairwise_cipher_implemented =
        pairwise_value;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[index].dot11_rsna_config_pairwise_cipher_activated =
        HI_TRUE;
}

WIFI_ROM_TEXT hi_void mac_mib_set_rsnaconfig_wpa2_pairwise_cipher_implemented(const mac_vap_stru *mac_vap,
    hi_u8 pairwise_value)
{
    hi_u8 index;

    if (pairwise_value == WLAN_80211_CIPHER_SUITE_CCMP) {
        index = 0;
    } else if (pairwise_value == WLAN_80211_CIPHER_SUITE_TKIP) {
        index = 1;
    } else {
        hi_diag_log_msg_e1(0, "{mac_mib_set_RSNAConfigWpa2PairwiseCipherImplemented::invalid uc_pairwise_value[%d].}",
            pairwise_value);
        return;
    }
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[index].dot11_rsna_config_pairwise_cipher_implemented =
        pairwise_value;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[index].dot11_rsna_config_pairwise_cipher_activated =
        HI_TRUE;
}

/* ****************************************************************************
 功能描述  : 设置ssid mib值
 输入参数  : pst_mac_vap: 指向mac vap结构体
             uc_len     : 参数长度
             pc_param   : 参数
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_mib_set_ssid(const mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    mac_cfg_ssid_param_stru *param = HI_NULL;
    hi_u8 ssid_len;
    hi_u8 *puc_mib_ssid = HI_NULL;
    hi_unref_param(len);
    param = (mac_cfg_ssid_param_stru *)puc_param;
    ssid_len = param->ssid_len; /* 长度不包括字符串结尾'\0' */
    if (ssid_len > WLAN_SSID_MAX_LEN - 1) {
        ssid_len = WLAN_SSID_MAX_LEN - 1;
    }
    puc_mib_ssid = mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_desired_ssid;
    if (memcpy_s(puc_mib_ssid, WLAN_SSID_MAX_LEN, param->ac_ssid, ssid_len) != EOK) {
        return HI_FAIL;
    }
    puc_mib_ssid[ssid_len] = '\0';
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 读取ssid mib值
**************************************************************************** */
hi_u32 mac_mib_get_ssid(const mac_vap_stru *mac_vap, hi_u8 *puc_len, const hi_u8 *puc_param)
{
    mac_cfg_ssid_param_stru *param = HI_NULL;
    hi_u8 ssid_len;
    hi_u8 *puc_mib_ssid = HI_NULL;

    puc_mib_ssid = mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_desired_ssid;
    ssid_len = (hi_u8)strlen((hi_char *)puc_mib_ssid);
    param = (mac_cfg_ssid_param_stru *)puc_param;
    param->ssid_len = ssid_len;
    if (memcpy_s(param->ac_ssid, WLAN_SSID_MAX_LEN, puc_mib_ssid, ssid_len) != EOK) {
        return HI_FAIL;
    }
    *puc_len = sizeof(mac_cfg_ssid_param_stru);
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
