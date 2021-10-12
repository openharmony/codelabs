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
#include "oam_ext_if.h"
#include "oal_ext_if.h"
#include "wal_main.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "mac_regdomain.h"
#include "mac_vap.h"
#include "hmac_device.h"
#include "hmac_vap.h"
#include "hmac_rx_filter.h"
#include "hmac_chan_mgmt.h"
#include "hmac_rx_filter.h"
#include "hmac_config.h"
#include "hmac_device.h"
#include "hmac_scan.h"
#include "hmac_rx_data.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
hmac_device_stru g_hmac_device;

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 获取对应HMAC DEVICE全局变量
**************************************************************************** */
hmac_device_stru *hmac_get_device_stru(hi_void)
{
    return &g_hmac_device;
}

/* ****************************************************************************
 功能描述  : 去初始化hmac device级别参数
 修改历史      :
  1.日    期   : 2015年1月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_device_exit(hi_void)
{
    mac_device_stru             *mac_dev = HI_NULL;
    hmac_device_stru            *hmac_dev = HI_NULL;
    hi_u32                       return_code;
    hmac_vap_stru               *hmac_vap = HI_NULL;
    mac_cfg_down_vap_param_stru  down_vap = {0};
    const hi_u8                 vap_idx = 0;

    hmac_dev = hmac_get_device_stru();
    /* 扫描模块去初始化 */
    hmac_scan_exit(hmac_dev);
#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
    hmac_pkt_mem_opt_exit(hmac_dev);
#endif

    /* 由于配置vap初始化在HMAC做，所以配置VAP卸载也在HMAC做 */
    hmac_vap = hmac_vap_get_vap_stru(WLAN_CFG_VAP_ID);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_device_exit::pst_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    return_code = hmac_config_del_vap(hmac_vap->base_vap, sizeof(mac_cfg_down_vap_param_stru), (hi_u8 *)&down_vap);
    if (return_code != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{hmac_device_exit::hmac_config_del_vap failed[%d].}", return_code);
        return return_code;
    }
    mac_dev = mac_res_get_dev();
    while (mac_dev->auc_vap_id[0] != 0) {
        hmac_vap = hmac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (hmac_vap == HI_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{hmac_device_exit::hmac_vap null.}");
            return HI_ERR_CODE_PTR_NULL;
        }
        return_code = hmac_vap_destroy(hmac_vap);
        if (return_code != HI_SUCCESS) {
            oam_warning_log1(0, OAM_SF_ANY, "{hmac_device_exit::hmac_vap_destroy failed[%d].}", return_code);
            return return_code;
        }
    }
    /* 卸载用户资源 */
    hmac_user_res_exit();
    /* 卸载VAP资源 */
    hmac_vap_res_exit();

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hmac_cfg_vap_init
 功能描述  : 配置VAP初始化
 输入参数  : uc_dev_id: 设备id
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_cfg_vap_init(const mac_device_stru *mac_dev)
{
    hi_u32           ret;
    hmac_vap_stru   *hmac_vap = HI_NULL;
    hi_u8            vap_idx;

    /* 初始化流程中，只初始化配置vap，其他vap需要通过配置添加 配置vap id必须为0 否则异常 */
    vap_idx = mac_vap_alloc_vap_res();
    if (oal_unlikely(vap_idx != WLAN_CFG_VAP_ID)) {
        oam_error_log1(0, OAM_SF_CFG, "{hmac_cfg_vap_init::alloc_vap_res fail. id=[%d].}", vap_idx);
        return HI_FAIL;
    }

    hmac_vap = hmac_vap_get_vap_stru(WLAN_CFG_VAP_ID);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{hmac_cfg_vap_init::pst_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_cfg_add_vap_param_stru param = { 0 }; /* 构造配置VAP参数结构体 */
    param.vap_mode = WLAN_VAP_MODE_CONFIG;
    ret = hmac_vap_init(hmac_vap, WLAN_CFG_VAP_ID, &param);
    if (ret != HI_SUCCESS) {
        oam_error_log1(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{hmac_cfg_vap_init::hmac_vap_init failed[%d].}", ret);
        return ret;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* 此时dmac未上电，帧过滤无法下发 */
#else
    /* 设置帧过滤寄存器 */
    hmac_set_rx_filter_value(hmac_vap->base_vap);
#endif

    ret = hmac_vap_creat_netdev(hmac_vap, WLAN_CFG_VAP_NAME, (hi_s8 *)(mac_dev->auc_hw_addr), WLAN_MAC_ADDR_LEN);
    if (ret != HI_SUCCESS) {
        oam_error_log1(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{hmac_cfg_vap_init::hmac_vap_creat_netdev failed[%d].}", ret);
        return ret;
    }

#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    ret = hmac_cfg_vap_send_event(mac_dev);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_cfg_vap_send_event::hmac_config_send_event fail[%d].", ret);
        return ret;
    }
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 初始化hmac device级别参数
 修改历史      :
  1.日    期   : 2015年1月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_device_init(hi_void)
{
    mac_device_stru     *mac_dev = HI_NULL;
    hmac_device_stru    *hmac_dev = HI_NULL;
    hi_u32           ret;

    mac_dev = mac_res_get_dev();
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    mac_device_init(mac_dev);
#endif
    hmac_dev = hmac_get_device_stru();
    /* 结构体初始化 */
    if (memset_s(hmac_dev, sizeof(hmac_device_stru), 0, sizeof(hmac_device_stru)) != EOK) {
        return HI_FAIL;
    }
    /* 扫描模块初始化 */
    hmac_scan_init(hmac_dev);
    /* 初始化P2P 等待队列 */
    hi_wait_queue_init_head(&(hmac_dev->netif_change_event));

#ifndef _PRE_WLAN_FEATURE_AMPDU_VAP
    /* 初始化device下的rx tx BA会话数目 */
    hmac_dev->rx_ba_session_num = 0;
    hmac_dev->tx_ba_session_num = 0;
#endif
    /* hmac mac vap资源初始化 */
    ret = hmac_vap_res_init();
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_device_init::hmac_init_vap_res failed.}");
        return HI_FAIL;
    }
    /* hmac mac user 资源初始化 */
    ret = hmac_user_res_init();
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_device_init::hmac_user_res_init failed.}");
        return HI_FAIL;
    }
    /* 配置vap初始化 */
    ret = hmac_cfg_vap_init(mac_dev);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{hmac_device_init::cfg_vap_init failed[%d].}", ret);
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
/* 功能描述  : 上下电流程中host device_stru的初始化函数 */
hi_u32 hmac_config_host_dev_init(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *param)
{
#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    mac_device_stru     *mac_device;
    hi_u32              ul_loop = 0;
#endif
#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
    hmac_device_stru *hmac_device;
#endif
    hi_unref_param(param);
    hi_unref_param(len);

    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_device_init:: pst_mac_device NULL pointer!}");
        return HI_FALSE;
    }

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    mac_device = mac_res_get_dev();
    if (mac_device == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_device_init:: pst_mac_device NULL pointer!}");
        return HI_FALSE;
    }

    for (ul_loop = 0; ul_loop < MAC_MAX_SUPP_CHANNEL; ul_loop++) {
        mac_device->st_ap_channel_list[ul_loop].us_num_networks = 0;
        mac_device->st_ap_channel_list[ul_loop].en_ch_type      = MAC_CH_TYPE_NONE;
    }
#endif

#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
    hmac_device = hmac_get_device_stru();
    if (oal_unlikely(hmac_device == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_config_host_dev_init::pst_hmac_device null!}");
        return HI_FALSE;
    }
    hmac_pkt_mem_opt_init(hmac_device);
#endif

    /* 补充上下电时候需要初始化的hmac_device_stru下的信息 */
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 下电流程中host device_stru的去初始化函数
**************************************************************************** */
hi_u32 hmac_config_host_dev_exit(mac_vap_stru *pst_mac_vap, hi_u16 len, const hi_u8 *param)
{
#ifdef _PRE_WLAN_FEATURE_PKT_MEM_OPT
    hmac_device_stru *hmac_device = hmac_get_device_stru();
    if (oal_unlikely(hmac_device == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_config_host_dev_exit::pst_hmac_device null!}");
        return HI_FALSE;
    }

    hmac_pkt_mem_opt_exit(hmac_device);
#endif

    hi_unref_param(pst_mac_vap);
    hi_unref_param(len);
    hi_unref_param(param);
    return HI_SUCCESS;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
