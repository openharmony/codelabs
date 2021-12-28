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
#include "plat_firmware.h"
#include "plat_pm_wlan.h"
#include "oam_ext_if.h"
#include "frw_main.h"
#include "frw_timer.h"
#include "hmac_ext_if.h"
#include "wal_ioctl.h"
#include "wal_hipriv.h"
#include "wal_cfg80211.h"
#include "wal_linux_flowctl.h"
#include "net_adpater.h"
#ifdef _PRE_WLAN_FEATURE_CSI
#include "wal_event.h"
#endif
#include "wal_event_msg.h"
#include "wal_customize.h"
#include "hcc_hmac_if.h"
#include "wal_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* HOST CTX字表 */
const frw_event_sub_table_item_stru g_ast_wal_host_ctx_table[HMAC_HOST_CTX_EVENT_SUB_TYPE_BUTT] = {
    {wal_scan_comp_proc_sta, HI_NULL, HI_NULL},                  /* HMAC_HOST_CTX_EVENT_SUB_TYPE_SCAN_COMP_STA */
    {wal_asoc_comp_proc_sta, HI_NULL, HI_NULL},                  /* HMAC_HOST_CTX_EVENT_SUB_TYPE_ASOC_COMP_STA */
    {wal_disasoc_comp_proc_sta, HI_NULL, HI_NULL},               /* HMAC_HOST_CTX_EVENT_SUB_TYPE_DISASOC_COMP_STA */
    {wal_connect_new_sta_proc_ap, HI_NULL, HI_NULL},             /* HMAC_HOST_CTX_EVENT_SUB_TYPE_STA_CONNECT_AP */
    {wal_disconnect_sta_proc_ap, HI_NULL, HI_NULL},              /* HMAC_HOST_CTX_EVENT_SUB_TYPE_STA_DISCONNECT_AP */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    {wal_mic_failure_proc, HI_NULL, HI_NULL},                    /* HMAC_HOST_CTX_EVENT_SUB_TYPE_MIC_FAILURE */
#endif
    {wal_send_mgmt_to_host, HI_NULL, HI_NULL},                   /* HMAC_HOST_CTX_EVENT_SUB_TYPE_RX_MGMT */
#ifdef _PRE_WLAN_FEATURE_P2P
    {wal_p2p_listen_timeout, HI_NULL, HI_NULL},                  /* HMAC_HOST_CTX_EVENT_SUB_TYPE_LISTEN_EXPIRED */
#endif
    {wal_report_sta_assoc_info, HI_NULL, HI_NULL},               /* HMAC_HOST_CTX_EVENT_SUB_TYPE_STA_CONN_RESULT */
#ifdef _PRE_WLAN_FEATURE_FLOWCTL
    {wal_flowctl_backp_event_handler, HI_NULL, HI_NULL},         /* HMAC_HOST_CTX_EVENT_SUB_TYPE_FLOWCTL_BACKP */
#endif
    {wal_cfg80211_mgmt_tx_status, HI_NULL, HI_NULL},             /* HMAC_HOST_CTX_EVENT_SUB_TYPE_MGMT_TX_STATUS */
#ifdef _PRE_WLAN_FEATURE_ANY
    {wal_any_process_rx_data, HI_NULL, HI_NULL},                 /* HMAC_HOST_CTX_EVENT_SUB_TYPE_ANY_RX_DATA */
    {wal_any_process_tx_complete, HI_NULL, HI_NULL},             /* HMAC_HOST_CTX_EVENT_SUB_TYPE_ANY_TX_STATUS */
    {wal_any_process_scan_result, HI_NULL, HI_NULL},             /* HMAC_HOST_CTX_EVENT_SUB_TYPE_ANY_SCAN_RESULT */
    {wal_any_process_peer_info, HI_NULL, HI_NULL},               /* HMAC_HOST_CTX_EVENT_SUB_TYPE_ANY_PEER_INFO */
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
    {wal_mesh_close_peer_inform, HI_NULL, HI_NULL},              /* HMAC_HOST_CTX_EVENT_SUB_TYPE_PEER_CLOSE_MESH */
    {wal_mesh_new_peer_candidate, HI_NULL, HI_NULL},             /* HMAC_HOST_CTX_EVENT_SUB_TYPE_NEW_PEER_CANDIDATE */
    {wal_mesh_inform_tx_data_info, HI_NULL, HI_NULL},            /* HMAC_HOST_CTX_EVENT_SUB_TYPE_TX_DATA_INFO */
    {wal_mesh_report_mesh_user_info, HI_NULL, HI_NULL},          /* HMAC_HOST_CTX_EVENT_SUB_TYPE_MESH_USER_INFO */
#endif
#ifdef _PRE_WLAN_FEATURE_CSI
    {wal_csi_data_report, HI_NULL, HI_NULL},                     /* HMAC_HOST_CTX_EVENT_SUB_TYPE_CSI_REPORT */
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    {wal_p2p_action_tx_status, HI_NULL, HI_NULL},                /* HMAC_HOST_CTX_EVENT_SUB_TYPE_P2P_TX_STATUS */
#endif

#ifdef FEATURE_DAQ
    {wal_data_acq_status, HI_NULL, HI_NULL},                      /* HMAC_HOST_CTX_EVENT_SUB_TYPE_ACQ_STATUS */
    {wal_data_acq_result, HI_NULL, HI_NULL},                      /* HMAC_HOST_CTX_EVENT_SUB_TYPE_ACQ_RESULT */
#endif
#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    {wal_channel_switch_report, HI_NULL, HI_NULL},                /* HMAC_HOST_CTX_EVENT_SUB_TYPE_CHANNEL_SWITCH */
#endif
    {wal_get_efuse_mac_from_dev, HI_NULL, HI_NULL},               /* HMAC_HOST_CTX_EVENT_GET_MAC_FROM_EFUSE */
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    {wal_get_dbg_cal_data_from_dev, HI_NULL, HI_NULL},            /* HMAC_HOST_CTX_EVENT_GET_DBG_CAL_DATA */
#endif
    {wal_report_tx_params, HI_NULL, HI_NULL},                     /* HMAC_HOST_CTX_EVENT_REPORT_TX_PARAMS */
};

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
EVENT_CB_S                 g_backup_event;
#endif
hi_u32                     g_wlan_reusme_wifi_mode = 0;               /* 0:不启动wifi，1:启动AP，2: 启动 STA */
hi_u8                       g_wifi_exit_stop_flag = HI_FALSE;
#define WAL_HAL_INTERRUPT_COUNT   4
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : wal_event_fsm_init
 功能描述  : 注册事件处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : HI_SUCCESS
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_event_fsm_init(hi_void)
{
    frw_event_table_register(FRW_EVENT_TYPE_HOST_CTX, FRW_EVENT_PIPELINE_STAGE_0, g_ast_wal_host_ctx_table);
}

/* ****************************************************************************
 功能描述  : WAL模块初始化总入口，包含WAL模块内部所有特性的初始化。
 返 回 值  : 初始化返回值，成功或失败原因
 修改历史      :
  1.日    期   : 2012年9月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 wal_main_init(hi_void)
{
    hi_u32 ret;
    frw_init_enum_uint8 init_state;

    wal_msg_queue_init();

    init_state = frw_get_init_state();
    /* WAL模块初始化开始时，说明HMAC肯定已经初始化成功 */
    if ((init_state == FRW_INIT_STATE_BUTT) || (init_state < FRW_INIT_STATE_HMAC_CONFIG_VAP_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_main_init::invalid state value [%d]!}", init_state);
        frw_timer_delete_all_timer();
        return HI_FAIL;
    }

    wal_event_fsm_init();
    wal_init_dev_addr();

#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
    /* 创建proc */
    ret = wal_hipriv_create_proc(HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_main_init_etc::wal_hipriv_create_proc_etc fail[%d]!}\r\n", ret);

        frw_timer_delete_all_timer();
        return -OAL_EFAIL;
    }
#endif
    ret = wal_customize_set_config();
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_main_init::customize init failed [%d]!}", ret);
        frw_timer_delete_all_timer();
        return HI_FAIL;
    }
    ret = wal_cfg80211_init();
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_main_init::wal_cfg80211_init fail[%d]!}", ret);
        frw_timer_delete_all_timer();
        return HI_FAIL;
    }
    /* 在host侧如果WAL初始化成功，即为全部初始化成功 */
    frw_set_init_state(FRW_INIT_STATE_ALL_SUCC);

    printk("wal_main_init SUCCESSFULLY\r\n");
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 卸载前删除所有vap
**************************************************************************** */
hi_void wal_destroy_all_vap(hi_void)
{
    oal_net_device_stru* netdev = HI_NULL;
    hi_u8                vap_id = WLAN_SERVICE_VAP_START_ID;

    /* 删除业务vap之前处理frw中事件 */
    frw_event_process_all_event();

    /* 删除业务vap */
    for (; vap_id < WLAN_SERVICE_VAP_NUM_PER_DEVICE; vap_id++) {
        netdev = hmac_vap_get_net_device(vap_id);
        if (netdev != HI_NULL) {
            oal_net_device_close(netdev);
            wal_hipriv_del_vap(netdev);
            hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(vap_id);
            if (hmac_vap != HI_NULL) {
                hmac_vap->net_device = HI_NULL;
            }
        }
    }
    return;
}

/* ****************************************************************************
 函 数 名  : wal_main_exit
 功能描述  : WAL模块卸载
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 模块卸载返回值，成功或失败原因
 调用函数  : 无
 被调函数  : 无

 修改历史      :
  1.日    期   : 2012年9月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_main_exit(hi_void)
{
    if (frw_get_init_state() != FRW_INIT_STATE_ALL_SUCC) {
        oam_info_log0(0, 0, "{wal_main_exit::frw state wrong.\n");
        return;
    }
    /* down掉所有的vap */
    wal_destroy_all_vap();
    wal_cfg80211_exit();
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
    /* 删除proc */
    wal_hipriv_remove_proc();
#endif
    /* 卸载成功时，将初始化状态置为HMAC初始化成功 */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    frw_set_init_state(FRW_INIT_STATE_HAL_SUCC);
#else
    frw_set_init_state(FRW_INIT_STATE_HMAC_CONFIG_VAP_SUCC);
#endif
}

/* ****************************************************************************
 函 数 名  : hisi_wifi_resume_process
 功能描述  : 恢复wifi进程
**************************************************************************** */
hi_void hisi_wifi_resume_process(hi_void)
{
    oam_info_log1(0, 0, "{hisi_wifi_resume_process::wifi_init, wifi_mode = %u}\n", g_wlan_reusme_wifi_mode);

    /* g_ul_wlan_reusme_wifi_mode：0:不启动wifi，1:启动AP，2: 启动 STA */
    if (g_wlan_reusme_wifi_mode == 1) {
        oam_info_log0(0, 0, "{hisi_wifi_resume_process::cmd_wifi_init_module:: end!}\n");
        msleep(3000);                          /* 3000: 睡眠时间 */
    } else if (g_wlan_reusme_wifi_mode == 2) { /* 2: 启动STA */
        oam_warning_log0(0, 0, "hisi_wifi_resume_process:: wait development");
    } else {
        /* nothing */
    }
}

/* ****************************************************************************
 功能描述  : Host部分 加载初始化
 修改历史      :
  1.日    期   : 2019年06月04日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hi_wifi_host_init(struct BusDev *bus)
{
    hi_u32 ret;
    g_wifi_exit_stop_flag = HI_FALSE;
    ret = hcc_hmac_init(bus);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "hi_wifi_host_init: hcc_hmac_init return error code: %d", ret);
        goto hcc_hmac_init_fail;
    }

    if (plat_firmware_init() != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "plat_firmware_init error\n");
        goto plat_firmware_init_fail;
    }

    if (wlan_pm_open() != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "wlan_pm_open error\n");
        goto wlan_pm_open_fail;
    }

    ret = hmac_main_init();
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "hi_wifi_host_init: hmac_main_init return error code: %d", ret);
        goto hmac_main_init_fail;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* 将从wifi_cfg读出的配置项同步给wal_customize */
    ret = firmware_sync_cfg_paras_to_wal_customize();
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "hi_wifi_host_init: wal_main_init return error code: %d", ret);
        goto hmac_main_init_fail;
    }
#endif
    ret = wal_main_init();
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "hi_wifi_host_init: wal_main_init return error code: %d", ret);
        goto wal_main_init_fail;
    }

    printk("hi_wifi_host_init SUCCESSFULLY\r\n");
    return HI_SUCCESS;

wal_main_init_fail:
    wal_main_exit();
hmac_main_init_fail:
    hmac_main_exit();
wlan_pm_open_fail:
    wlan_pm_exit();
    plat_firmware_clear();
plat_firmware_init_fail:
hcc_hmac_init_fail:
    hcc_hmac_exit();
    return HI_FAIL;
}

/* ****************************************************************************
 功能描述  : Host部分 卸载
 修改历史      :
  1.日    期   : 2019年06月04日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hi_wifi_host_exit(hi_void)
{
    g_wifi_exit_stop_flag = HI_TRUE;
    wal_main_exit();
    hmac_main_exit();
    plat_firmware_clear();

    printk("wifi host exit successfully\r\n");
    return;
}

/* ****************************************************************************
 功能描述  : 平台初始化函数总入口
 输入参数  : vap_num : 最大支持的同时工作的vap数量
             user_num: 最大支持接入的用户数量,多vap时共享
 修改历史      :
  1.日    期   : 2014年11月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hi_wifi_plat_init(const hi_u8 vap_num, const hi_u8 user_num)
{
#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    hi_bool offload_mode = HI_TRUE; /* DMAC OFFLOAD: IPC场景 */
#else
    hi_bool offload_mode = HI_FALSE; /* DMAC HOST合一: IOT场景 */
#endif
    hi_u32 wifi_task_size = (hi_u32)FRW_TASK_SIZE;

    oam_info_log2(0, OAM_SF_ANY, "hi_wifi_plat_init vap_num[%d], user_num[%d]", vap_num, user_num);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if (oam_main_init() != HI_SUCCESS) {
        oam_error_log0(0, 0, "plat_init: oam_main_init return error code.");
        goto oam_main_init_fail;
    }
#endif

    if (wal_customize_init() != HI_SUCCESS) {
        oam_error_log0(0, 0, "plat_init: wal_customize_init return error code.");
        goto wal_customize_init_fail;
    }

    if (oal_main_init(vap_num, user_num) != HI_SUCCESS) {
        oam_error_log0(0, 0, "plat_init: oal_main_init return error code.");
        goto oal_main_init_fail;
    }

    if (frw_main_init(offload_mode, wifi_task_size) != HI_SUCCESS) {
        oam_error_log0(0, 0, "plat_init: frw_main_init return error code.");
        goto frw_main_init_fail;
    }

    printk("hi_wifi_plat_init SUCCESSFULLY\r\n");
    return HI_SUCCESS;

frw_main_init_fail:
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oam_main_exit();
oam_main_init_fail:
#endif
    oal_main_exit();
oal_main_init_fail:
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    wal_customize_exit();
#endif
wal_customize_init_fail:
    return HI_FAIL;
}

/* ****************************************************************************
 功能描述  : 平台卸载函数总入口
 修改历史      :
  1.日    期   : 2014年11月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hi_wifi_plat_exit(hi_void)
{
    oam_main_exit();
    if (frw_get_init_state() != FRW_INIT_STATE_FRW_SUCC) {
        oam_error_log0(0, 0, "{hi_wifi_plat_exit:: frw init state error.}");
        return;
    }
    frw_main_exit();
    oal_main_exit();
    wal_customize_exit();
}

hi_u8 hi_wifi_get_host_exit_flag(hi_void)
{
    return g_wifi_exit_stop_flag;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
