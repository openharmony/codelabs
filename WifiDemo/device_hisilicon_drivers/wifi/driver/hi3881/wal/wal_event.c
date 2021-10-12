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
#include "hmac_ext_if.h"
#include "wal_main.h"
#include "wal_scan.h"
#include "wal_cfg80211.h"
#include "wal_ioctl.h"
#include "wal_hipriv.h"
#include "wal_linux_flowctl.h"
#include "wal_cfg80211_apt.h"
#include "hdf_wifi_event.h"
#include "net_adpater.h"
#include "hi_wifi_driver_wpa_if.h"
#ifdef _PRE_WLAN_FEATURE_ANY
#include "hmac_any.h"
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
#include "hmac_p2p.h"
#endif
#ifdef _PRE_WLAN_FEATURE_CSI
#include "hi_wifi_api.h"
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
#include "dmac_config.h"
#endif
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
#include "plat_firmware.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_CSI
hi_wifi_csi_data_cb             g_csi_data_func;
hi_u32                          g_csi_tsf_tmp = 0;    /* 存储本次上报的CSI数据的时间戳，方便与下一次时间戳比较 */
hi_u32                          g_csi_tsf_val = 0;    /* 存储上报的CSI数据的时间戳反转次数 */
#endif

hi_wifi_report_tx_params_callback g_wal_report_tx_params_callback = HI_NULL;

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
hi_u32 wal_scan_result_clean_timeout_fn(hi_void *arg)
{
    hi_u32 ret;
    wal_msg_write_stru write_msg;

    oal_net_device_stru *netdev = (oal_net_device_stru *)arg;
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_CLEAN_SCAN_RESULT, sizeof(hi_u32));
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_scan_result_clean_timeout_fn::return err code [%u]!}", ret);
    }

    return ret;
}

/* ****************************************************************************
 函 数 名  : wal_scan_comp_proc_sta
 功能描述  : STA上报扫描完成事件处理
 输入参数  : event_mem: 事件内存
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年7月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2013年9月9日
    作    者   : HiSilicon
    修改内容   : 增加上报内核部分函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_scan_comp_proc_sta(frw_event_mem_stru *event_mem)
{
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_scan_comp_proc_sta::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    oal_net_device_stru *netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC, "{wal_scan_comp_proc_sta::get net device ptr null}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取hmac vap结构体 */
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log0(event->event_hdr.vap_id, OAM_SF_SCAN, "{wal_scan_comp_proc_sta::pst_hmac_vap is NULL!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 删除等待扫描超时定时器 */
    if (hmac_vap->scan_timeout.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(hmac_vap->scan_timeout));
    }

    /* 获取hmac device 指针 */
    hmac_device_stru *hmac_dev  = hmac_get_device_stru();
    mac_device_stru  *mac_dev   = mac_res_get_dev();
    hmac_scan_stru   *scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 无论是否是ANY扫描，扫描完成时需要恢复标志为当前非ANY扫描 */
    scan_mgmt->scan_record_mgmt.is_any_scan = HI_FALSE;

    /* 获取扫描结果的管理结构地址 */
    hmac_bss_mgmt_stru *bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt);

    /* 获取驱动上报的扫描结果结构体指针 */
    hmac_scan_rsp_stru *scan_rsp = (hmac_scan_rsp_stru *)event->auc_event_data;

    /* 如果扫描返回结果的非成功，打印维测信息 */
    if (scan_rsp->result_code != HMAC_MGMT_SUCCESS) {
        oam_warning_log1(event->event_hdr.vap_id, OAM_SF_SCAN, "wal_scan_comp_proc_sta:Err=%d", scan_rsp->result_code);
    }

    /* 扫描成功时上报所有扫描到的bss */
    if (scan_rsp->result_code == HMAC_MGMT_SUCCESS) {
        wal_inform_all_bss(netdev, mac_dev->wiphy, bss_mgmt, event->event_hdr.vap_id);
    }

    /* 对于内核下发的扫描request资源加锁 */
    oal_spin_lock(&(scan_mgmt->st_scan_request_spinlock));

    /* 上层下发的普通扫描进行对应处理 */
    if (scan_mgmt->request != HI_NULL) {
        /* 通知 kernel scan 已经结束 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
        HdfWifiEventScanDone(netdev, (WifiScanStatus)HISI_SCAN_SUCCESS);
        wal_free_scan_mgmt_resource(scan_mgmt);
#else
        oal_cfg80211_scan_done(scan_mgmt->request, 0);
        scan_mgmt->request = HI_NULL;
#endif
        scan_mgmt->complete = HI_TRUE;
    }

    if (scan_mgmt->sched_scan_req != HI_NULL) {
        /* 上报调度扫描结果 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        oal_cfg80211_sched_scan_result(mac_dev->wiphy);
#endif
        scan_mgmt->sched_scan_req = HI_NULL;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        scan_mgmt->sched_scan_complete = HI_TRUE;
#endif
    }

    /* 通知完内核，释放资源后解锁 */
    oal_spin_unlock(&(scan_mgmt->st_scan_request_spinlock));

    /* 创建扫描清除结果定时器 */
    frw_timer_create_timer(&(hmac_vap->scanresult_clean_timeout), wal_scan_result_clean_timeout_fn,
        WLAN_SCANRESULT_CLEAN_TIME, netdev, HI_FALSE);
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_asoc_comp_proc_sta
 功能描述  : STA上报关联完成事件处理
 输入参数  : event_mem: 事件内存
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年7月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_asoc_comp_proc_sta(frw_event_mem_stru *event_mem)
{
    frw_event_stru              *event = HI_NULL;
    oal_connet_result_stru       connet_result;
    oal_net_device_stru         *netdev = HI_NULL;
    hmac_asoc_rsp_stru          *asoc_rsp = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_asoc_comp_proc_sta::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event     = (frw_event_stru *)event_mem->puc_data;
    asoc_rsp  = (hmac_asoc_rsp_stru *)event->auc_event_data;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oal_free(asoc_rsp->puc_asoc_rsp_ie_buff);
        asoc_rsp->puc_asoc_rsp_ie_buff = HI_NULL;
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC, "{wal_asoc_comp_proc_sta::get net device ptr is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (memset_s(&connet_result, sizeof(oal_connet_result_stru), 0, sizeof(oal_connet_result_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_asoc_comp_proc_sta::mem safe function err!}");
        oal_free(asoc_rsp->puc_asoc_rsp_ie_buff);
        asoc_rsp->puc_asoc_rsp_ie_buff = HI_NULL;
        return HI_FAIL;
    }
    /* 准备上报内核的关联结果结构体 */
    if (memcpy_s(connet_result.auc_bssid, WLAN_MAC_ADDR_LEN, asoc_rsp->auc_addr_ap, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_asoc_comp_proc_sta::mem safe function err!}");
        oal_free(asoc_rsp->puc_asoc_rsp_ie_buff);
        asoc_rsp->puc_asoc_rsp_ie_buff = HI_NULL;
        return HI_FAIL;
    }
    connet_result.puc_req_ie = asoc_rsp->puc_asoc_req_ie_buff;
    connet_result.req_ie_len = asoc_rsp->asoc_req_ie_len;
    connet_result.puc_rsp_ie = asoc_rsp->puc_asoc_rsp_ie_buff;
    connet_result.rsp_ie_len = asoc_rsp->asoc_rsp_ie_len;
    connet_result.us_status_code = asoc_rsp->status_code;
    connet_result.us_freq = asoc_rsp->us_freq;
    connet_result.us_connect_status = asoc_rsp->result_code;

    /* 调用内核接口，上报关联结果 */
    oal_cfg80211_connect_result(netdev, &connet_result);

    /* 释放关联管理帧内存 */
    oal_mem_free(asoc_rsp->puc_asoc_req_ie_buff);
    oal_free(asoc_rsp->puc_asoc_rsp_ie_buff);
    asoc_rsp->puc_asoc_rsp_ie_buff = HI_NULL;

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_disasoc_comp_event_proc
 功能描述  : STA上报去关联完成事件处理
 输入参数  : event_mem: 事件内存
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年7月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_disasoc_comp_proc_sta(frw_event_mem_stru *event_mem)
{
    frw_event_stru              *event = HI_NULL;
    oal_disconnect_result_stru   disconnect_result;
    oal_net_device_stru         *netdev = HI_NULL;
    hi_u32                      *pul_reason_code = HI_NULL;
    hi_u16                       us_disass_reason_code;
    hi_u16                       us_dmac_reason_code;
    hi_u32                       ret;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_disasoc_comp_proc_sta::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_disasoc_comp_proc_sta::get net device ptr is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取去关联原因码指针 */
    pul_reason_code = (hi_u32 *)event->auc_event_data;
    us_disass_reason_code = (*pul_reason_code) & 0x0000ffff;
    us_dmac_reason_code = ((*pul_reason_code) >> 16) & 0x0000ffff; /* 右移16位 */

    if (memset_s(&disconnect_result, sizeof(oal_disconnect_result_stru), 0,
        sizeof(oal_disconnect_result_stru)) != EOK) {
        return HI_FAIL;
    }
    hi_bool locally_generated = (us_dmac_reason_code == DMAC_DISASOC_MISC_KICKUSER);

    /* 准备上报内核的关联结果结构体 */
    disconnect_result.us_reason_code = us_disass_reason_code;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    if (!locally_generated && (us_disass_reason_code == WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY ||
        us_disass_reason_code == WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA ||
        us_disass_reason_code == WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA)) {
        disconnect_result.us_reason_code = 0;
    }
#endif
    /* 调用内核接口，上报去关联结果 */
    ret = oal_cfg80211_disconnected(netdev, disconnect_result.us_reason_code, disconnect_result.pus_disconn_ie,
        disconnect_result.us_disconn_ie_len, locally_generated);
    if (ret != HI_SUCCESS) {
        oam_error_log1(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_disasoc_comp_proc_sta::cfg80211_disconnected fail[%d]!}", ret);
        return ret;
    }

    oam_warning_log3(event->event_hdr.vap_id, OAM_SF_ASSOC,
        "{wal_disasoc_comp_proc_sta reason_code[%d] ,dmac_reason_code[%d], locally[%d]OK!}", us_disass_reason_code,
        us_dmac_reason_code, locally_generated);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_connect_new_sta_proc_ap
 功能描述  : 驱动上报内核bss网络中新加入了一个STA
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_connect_new_sta_proc_ap(frw_event_mem_stru *event_mem)
{
    hi_u8                 user_mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    oal_station_info_stru station_info;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_connect_new_sta_proc_ap::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;

    /* 获取net_device */
    oal_net_device_stru *netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC, "{wal_connect_new_sta_proc_ap::netdev is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 固定长度的结构体进行内存初始化 */
    memset_s(&station_info, sizeof(oal_station_info_stru), 0, sizeof(oal_station_info_stru));

    /* 向内核标记填充了关联请求帧的ie信息 */
#if (LINUX_VERSION_CODE >= kernel_version(4, 0, 0))
    /* Linux 4.0 版本不需要STATION_INFO_ASSOC_REQ_IES 标识 */
#else
    station_info.filled |= STATION_INFO_ASSOC_REQ_IES;
#endif

    hmac_asoc_user_req_ie_stru *asoc_user_req_info = (hmac_asoc_user_req_ie_stru *)(event->auc_event_data);
    station_info.assoc_req_ies = asoc_user_req_info->puc_assoc_req_ie_buff;
    if (station_info.assoc_req_ies == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC, "{wal_connect_new_sta_proc_ap::asoc ie is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    station_info.assoc_req_ies_len = asoc_user_req_info->assoc_req_ie_len;

    /* 获取关联user mac addr */
    if (memcpy_s(user_mac_addr, WLAN_MAC_ADDR_LEN,
        (hi_u8 *)asoc_user_req_info->auc_user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_connect_new_sta_proc_ap::mem safe function err!}");
        return HI_FAIL;
    }

    /* 调用内核接口，上报STA关联结果 */
    hi_u32 ret = oal_cfg80211_new_sta(netdev, user_mac_addr, WLAN_MAC_ADDR_LEN, &station_info, GFP_ATOMIC);
    if (ret != HI_SUCCESS) {
        oam_error_log1(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_connect_new_sta_proc_ap::oal_cfg80211_new_sta fail[%d]!}", ret);
        return ret;
    }

    oam_warning_log3(event->event_hdr.vap_id, OAM_SF_ASSOC,
        "{wal_connect_new_sta_proc_ap mac[XX:XX:XX:%02X:%02X:%02X]}", user_mac_addr[3], user_mac_addr[4],
        user_mac_addr[5]); /* 3/4/5 MAC地址位数 */

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_disconnect_sta_proc_ap
 功能描述  : 驱动上报内核bss网络中删除了一个STA
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_disconnect_sta_proc_ap(frw_event_mem_stru *event_mem)
{
    frw_event_stru            *event = HI_NULL;
    oal_net_device_stru       *netdev = HI_NULL;
    hi_u8                      user_mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    hi_u32                     ret;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_disconnect_sta_proc_ap::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_disconnect_sta_proc_ap::get net device ptr is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 获取去关联user mac addr */
    if (memcpy_s(user_mac_addr, WLAN_MAC_ADDR_LEN,
        (hi_u8 *)event->auc_event_data, WLAN_MAC_ADDR_LEN) != EOK) { /* event->auc_event_data: 可变数组 */
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_disconnect_sta_proc_ap::mem safe function err!}");
        return HI_FAIL;
    }
    /* 调用内核接口，上报STA去关联结果 */
    ret = oal_cfg80211_del_sta(netdev, user_mac_addr, WLAN_MAC_ADDR_LEN, GFP_ATOMIC);
    if (ret != HI_SUCCESS) {
        oam_error_log1(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_disconnect_sta_proc_ap::cfg80211_del_sta fail[%d]}", ret);
        return ret;
    }

    oam_warning_log3(event->event_hdr.vap_id, OAM_SF_ASSOC,
        "{wal_disconnect_sta_proc_ap mac[XX:XX:XX:%02x:%02x:%02x]}",
        user_mac_addr[3], user_mac_addr[4], user_mac_addr[5]); /* 3 4 5 数组位数 */

    return HI_SUCCESS;
}

hi_void wal_set_tpc_mode(oal_net_device_stru *netdev, hi_u32 mode)
{
    wal_msg_write_stru write_msg;
    mac_ioctl_alg_param_stru *alg_param = (mac_ioctl_alg_param_stru *)(write_msg.auc_value);
    hi_u32 ret;

    alg_param->alg_cfg = MAC_ALG_CFG_TPC_MODE;
    alg_param->is_negtive = HI_FALSE;
    alg_param->value = mode;

    /* 抛事件到wal层处理 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ALG_PARAM, sizeof(mac_ioctl_alg_param_stru));

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_ioctl_alg_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_at_set_tpc::wal_send_cfg_event return err code [%u]!}", ret);
        return;
    }

    return;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : wal_mic_failure_proc
 功能描述  : 驱动上报内核mic错误
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年12月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_mic_failure_proc(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    oal_net_device_stru *netdev = HI_NULL;
    hmac_mic_event_stru *mic_event = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_CRYPTO, "{wal_mic_failure_proc::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    mic_event = (hmac_mic_event_stru *)(event->auc_event_data);

    /* 获取net_device */
    netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_CRYPTO, "{wal_mic_failure_proc::get net device ptr is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 调用内核接口，上报mic攻击 */
    oal_cfg80211_mic_failure(netdev, mic_event->auc_user_mac, mic_event->key_type, mic_event->l_key_id, HI_NULL,
        GFP_ATOMIC);

    oam_warning_log3(event->event_hdr.vap_id, OAM_SF_CRYPTO, "{wal_mic_failure_proc::mac[%x %x %x] OK!}",
        mic_event->auc_user_mac[3], mic_event->auc_user_mac[4], /* 3 4 数组位数 */
        mic_event->auc_user_mac[5]);                            /* 5 数组位数 */

    return HI_SUCCESS;
}
#endif
/* ****************************************************************************
 函 数 名  : wal_send_mgmt_to_host
 功能描述  : 驱动上报内核接收到管理帧
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2014年5月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_send_mgmt_to_host(frw_event_mem_stru *event_mem)
{
    frw_event_stru               *event = HI_NULL;
    oal_net_device_stru          *netdev = HI_NULL;
    hi_s32                     l_freq;
    hi_u8                    *puc_buf = HI_NULL;
    hi_u16                    us_len;
    hi_u8                    ret;
    hmac_rx_mgmt_event_stru      *mgmt_frame = HI_NULL;
    oal_ieee80211_mgmt           *ieee80211_mgmt = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_send_mgmt_to_host::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event       = (frw_event_stru *)event_mem->puc_data;
    mgmt_frame  = (hmac_rx_mgmt_event_stru *)(event->auc_event_data);

    /* 获取net_device */
    netdev = oal_get_netdev_by_name(mgmt_frame->ac_name);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ANY, "{wal_send_mgmt_to_host::get net device ptr is null!}");
        oal_free(mgmt_frame->puc_buf);
        return HI_ERR_CODE_PTR_NULL;
    }
    oal_dev_put(netdev);

    puc_buf = mgmt_frame->puc_buf;
    us_len  = mgmt_frame->us_len;
    l_freq  = mgmt_frame->l_freq;

    ieee80211_mgmt = (oal_ieee80211_mgmt *)puc_buf;
    /* 调用内核接口，上报接收到管理帧 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    ret = HdfWifiEventRxMgmt(netdev, l_freq, 0, puc_buf, us_len);
#else
    ret = oal_cfg80211_rx_mgmt(netdev, l_freq, puc_buf, us_len, GFP_ATOMIC);
#endif
    if (ret != HI_TRUE) {
        oam_warning_log2(event->event_hdr.vap_id, OAM_SF_ANY, "{wal_send_mgmt_to_host::fc[0x%04x], if_type[%d]!}",
            ieee80211_mgmt->frame_control, GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype);
        oam_warning_log3(event->event_hdr.vap_id, OAM_SF_ANY,
            "{wal_send_mgmt_to_host::cfg80211_rx_mgmt_ext fail[%d]!len[%d], freq[%d]}", ret, us_len, l_freq);
        oal_free(puc_buf);
        return HI_FAIL;
    }
    oam_info_log3(event->event_hdr.vap_id, OAM_SF_ANY, "{wal_send_mgmt_to_host::freq = %d, len = %d, TYPE[%04X] OK!}",
        l_freq, us_len, ieee80211_mgmt->frame_control);
    oal_free(puc_buf);
    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_P2P
/* ****************************************************************************
 函 数 名  : wal_p2p_listen_timeout
 功能描述  : HMAC上报监听超时
 输入参数  : frw_event_mem_stru *event_mem
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2014年11月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_p2p_listen_timeout(frw_event_mem_stru *event_mem)
{
    frw_event_stru               *event              = HI_NULL;
    oal_wireless_dev             *wdev               = HI_NULL;
    hmac_p2p_listen_expired_stru *p2p_listen_expired = HI_NULL;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mac_device_stru              *mac_dev         = HI_NULL;
    hi_u64                        ull_cookie;
#endif
    oal_ieee80211_channel_stru    listen_channel;
    hi_u32                        ret;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_P2P, "{wal_p2p_listen_timeout::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    p2p_listen_expired = (hmac_p2p_listen_expired_stru *)(event->auc_event_data);

    wdev = p2p_listen_expired->wdev;
    listen_channel = p2p_listen_expired->st_listen_channel;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    /* 获取mac_device_stru */
    mac_dev = mac_res_get_dev();
    ull_cookie = mac_dev->p2p_info.ull_last_roc_id;
    cfg80211_remain_on_channel_expired(wdev, ull_cookie, &listen_channel, GFP_ATOMIC);
#endif
    /* 调用内核接口，上报报监听超时 */
    ret = HdfWifiEventCancelRemainOnChannel(wdev->netdev, listen_channel.center_freq);
    if (ret != HI_SUCCESS) {
        oam_error_log1(event->event_hdr.vap_id, OAM_SF_P2P, "{wal_p2p_listen_timeout!}", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 函 数 名  : wal_mesh_close_peer_inform
 功能描述  : MESH 驱动上报Wpa 通知其对指定远端节点发起取消配对流程
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2019年1月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_mesh_close_peer_inform(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    oal_net_device_stru *netdev = HI_NULL;
    hi_u32 *pul_reason_code = HI_NULL;
    hi_u16 us_disass_reason_code;
    hi_u16 us_dmac_reason_code;
    hi_u32 ret;
    hi_u8  user_mac_addr[WLAN_MAC_ADDR_LEN] = {0};

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_mesh_close_peer_inform::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_mesh_close_peer_inform::get net device ptr is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 获取待去关联的用户的Mac地址 */
    if (memcpy_s(user_mac_addr, WLAN_MAC_ADDR_LEN, (hi_u8 *)event->auc_event_data, WLAN_MAC_ADDR_LEN) !=
        EOK) { /* event->auc_event_data, 可变数组 */
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_mesh_close_peer_inform::mem safe function err!}");
        return HI_FAIL;
    }
    /* 获取去关联原因码指针 */
    pul_reason_code = (hi_u32 *)(event->auc_event_data + WLAN_MAC_ADDR_LEN); // 可变数组用法，lin_t e416告警屏蔽
    us_disass_reason_code = (*pul_reason_code) & 0x0000ffff;
    us_dmac_reason_code = ((*pul_reason_code) >> 16) & 0x0000ffff; /* 16 右移16位 */

    /* 调用内核接口，上报去关联结果 */
    ret = cfg80211_mesh_close(netdev, user_mac_addr, WLAN_MAC_ADDR_LEN, us_disass_reason_code);
    if (ret != HI_SUCCESS) {
        oam_error_log1(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_mesh_close_peer_inform::oal_cfg80211_mesh_close fail[%d]!}", ret);
        return ret;
    }

    oam_warning_log2(event->event_hdr.vap_id, OAM_SF_ASSOC,
        "{wal_mesh_close_peer_inform reason_code[%d] ,dmac_reason_code[%d]OK!}", us_disass_reason_code,
        us_dmac_reason_code);

    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : MESH 驱动上报Wpa 通知有符合关联条件的远端节点
 输入参数  : frw_event_mem_stru *event_mem
 返 回 值  :hi_u32
 修改历史      :
  1.日    期   : 2019年3月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_mesh_new_peer_candidate(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    oal_net_device_stru *netdev = HI_NULL;
    hi_u32 ret;
    hmac_report_new_peer_candidate_stru *puc_new_peer = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_mesh_new_peer_candidate::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    puc_new_peer = (hmac_report_new_peer_candidate_stru *)(event->auc_event_data);

    /* 获取net_device */
    netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_mesh_new_peer_candidate::get net device ptr is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 调用内核接口，上报去关联结果 */
    ret = cfg80211_mesh_new_peer_candidate(netdev, puc_new_peer);
    if (ret != HI_SUCCESS) {
        oam_error_log1(event->event_hdr.vap_id, OAM_SF_ASSOC,
            "{wal_mesh_new_peer_candidate::wal_mesh_new_peer_candidate fail[%d]!}", ret);
        return ret;
    }

    oam_info_log2(event->event_hdr.vap_id, OAM_SF_ASSOC, "{wal_mesh_new_peer_candidate:: mac addr = %x:%x!}",
        puc_new_peer->auc_mac_addr[4], puc_new_peer->auc_mac_addr[5]); /* 4 5 地址位数 */

    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_ANY
/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
static hi_wifi_any_callback         g_any_callback = {HI_NULL, HI_NULL};
static hi_wifi_any_scan_result_cb   g_scan_ret_cb = HI_NULL;
static hi_wifi_any_peer_info        g_peer_info;
static hi_u8                        g_query_completed_flag = HI_FALSE;
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 注册ANY设备接口的报文发送和接收回调函数，之前注册过的会被覆盖替换掉
 输入参数  : send_cb从用户传过来的发送回调函数
             recv_cb从用户传过来的接收回调函数
 返 回 值  : hi_void

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void wal_any_set_callback(hi_wifi_any_send_complete_cb send_cb, hi_wifi_any_recv_cb recv_cb)
{
    g_any_callback.send_cb = send_cb;
    g_any_callback.recv_cb = recv_cb;
    return;
}

/* ****************************************************************************
 功能描述  : 发起扫描发现ANY对端设备信息，包括MAC地址，信道和接收到的cookie等信息
 输入参数  : hi_wifi_any_scan_result_cb 扫描完成之后的结果回调处理函数
 返 回 值  : hi_void

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void wal_any_set_scan_callback(hi_wifi_any_scan_result_cb cb)
{
    g_scan_ret_cb = cb;
    return;
}

/* ****************************************************************************
 功能描述  : ANY WAL层配置HMAC层公共接口，不带参数场景使用
 输入参数  : wlan_cfgid_enum_uint16 配置操作枚举值
 返 回 值  : HI_SUCCESS 上报成功，其它错误码 上报失败

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_s32 wal_any_global_config(wlan_cfgid_enum_uint16 wid, oal_net_device_stru *netdev)
{
    wal_msg_write_stru write_msg;
    hi_u32 ret;

    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "{wal_any_global_config:pst_netdev is NULL, need to initialize ANY.}");
        return HI_FAIL;
    }
    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, wid, 0);

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH, (hi_u8 *)&write_msg, HI_FALSE,
        HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_any_global_config::return err code [%u]!}", ret);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : 上报接收到ANY帧事件处理
 输入参数  : event_mem: 事件内存
 返 回 值  : 成功返回HI_SUCCESS，失败返回其他值
 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_any_process_rx_data(frw_event_mem_stru *event_mem)
{
    frw_event_stru              *event = HI_NULL;
    oal_any_peer_param_stru     *peer_param = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_any_process_rx_data::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 获取ANY用户数据结构体指针 */
    peer_param = (oal_any_peer_param_stru *)event->auc_event_data;

    if (g_any_callback.recv_cb != HI_NULL) {
        /* 直接采用用户注册的接收回调函数进行处理 */
        g_any_callback.recv_cb(peer_param->auc_mac, peer_param->puc_data, peer_param->us_len, peer_param->seq_num);
    }

    /* 释放由HMAC申请的内存 */
    oal_mem_free(peer_param->puc_data);
    peer_param->puc_data = HI_NULL;
    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : 上报ANY帧发送状态事件处理
 输入参数  : event_mem: 事件内存
 返 回 值  : 成功返回HI_SUCCESS，失败返回其他值
 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_any_process_tx_complete(frw_event_mem_stru *event_mem)
{
    frw_event_stru          *event = HI_NULL;
    hi_u8                   *puc_data = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_any_process_tx_complete::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 获取帧发送状态数据 */
    puc_data = (hi_u8 *)event->auc_event_data;

    /* lin_t -e415 */ /* lin_t -e416 */
    if (g_any_callback.send_cb != HI_NULL) {
        /* 直接采用用户注册的发送回调函数进行处理 */ /* 可变数组用法，lin_t e415/e416告警屏蔽 */
        g_any_callback.send_cb(puc_data, puc_data[ETH_ALEN], puc_data[ETH_ALEN + 1]);
    }
    /* lin_t +e415 */ /* lin_t +e416 */

    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : 向用户上报扫描到的ANY设备信息
 返 回 值  : 成功返回HI_SUCCESS，失败返回其他值
 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_any_process_scan_result(frw_event_mem_stru *event_mem)
{
    frw_event_stru              *event = HI_NULL;
    hmac_any_device_list_stru   *puc_data = HI_NULL;
    hi_u8                        loop;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_any_process_scan_result::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 获取数据指针 */
    puc_data = *((hmac_any_device_list_stru **)(event->auc_event_data));
    if ((puc_data == HI_NULL) || (puc_data->dev_list == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC,
            "{wal_any_process_scan_result::puc_data or pst_dev_list is null, not initialized}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (g_scan_ret_cb != HI_NULL) {
        /* 直接采用用户注册的回调函数进行处理 */
        if (puc_data->dev_num != 0) {
            g_scan_ret_cb(puc_data->dev_list, puc_data->dev_num);
        } else {
            g_scan_ret_cb(HI_NULL, 0);
        }
    }

    /* 完成上报，释放扫描结果 */
    for (loop = 0; loop < puc_data->dev_num; loop++) {
        if (puc_data->dev_list[loop] != HI_NULL) {
            oal_mem_free(puc_data->dev_list[loop]);
            puc_data->dev_list[loop] = HI_NULL;
        }
    }
    puc_data->dev_num = 0;

    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : 向用户上报扫描到的ANY设备信息
 返 回 值  : 成功返回HI_SUCCESS，失败返回其他值
 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_any_process_peer_info(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    hi_wifi_any_peer_info *peer_info = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_any_process_peer_info::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_any_process_peer_info::pst_hmac_vap null.vap_id[%d]}",
            event->event_hdr.vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    peer_info = (hi_wifi_any_peer_info *)(event->auc_event_data); /* event->auc_event_data, 可变数组 */
    memcpy_s(&g_peer_info, sizeof(hi_wifi_any_peer_info), peer_info, sizeof(hi_wifi_any_peer_info));

    g_query_completed_flag = HI_TRUE;
    hi_wait_queue_wake_up_interrupt(&(hmac_vap->query_wait_q));

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 等待HMAC和DMAC返回查询ANY对端的结果
 返 回 值  : 成功返回HI_SUCCESS，不存在或失败返回其他值

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 wal_any_wait_query_result(hmac_vap_stru *hmac_vap, hi_wifi_any_peer_info *peer)
{
    hi_u32 ret;
    hi_u8                    auc_mac[ETH_ALEN] = {0};

    if (g_query_completed_flag == HI_FALSE) {
        memset_s(&g_peer_info, sizeof(hi_wifi_any_peer_info), 0, sizeof(hi_wifi_any_peer_info));

        ret = (hi_u32)hi_wait_event_timeout(hmac_vap->query_wait_q, (HI_TRUE == g_query_completed_flag),
            (5 * HZ)); /* 5 频率,非wifi目录定义宏函数,误报告警,lin_t e26告警屏蔽 */
        if (ret == 0) {
            oam_warning_log1(0, OAM_SF_ANY, "wal_any_wait_query_result: query temp timeout. ret:%d", ret);
            return HI_FAIL;
        }
    }

    /* 查询获取到结果之后恢复成false,便于下一次使用 */
    g_query_completed_flag = HI_FALSE;

    /* 底层如果返回的对端信息MAC地址为全0，约定为对端不存在 */
    if (memcmp(g_peer_info.mac, auc_mac, ETH_ALEN) == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "wal_any_wait_query_result: peer does not exist!");
        return HI_FAIL;
    }

    memcpy_s(peer, sizeof(hi_wifi_any_peer_info), &g_peer_info, sizeof(hi_wifi_any_peer_info));
    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_CSI
/* ****************************************************************************
 功能描述  : 打开CSI数据上报开关
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 wal_csi_switch(hi_u8 switch_flag)
{
    oal_net_device_stru              *netdev = HI_NULL;
    wal_msg_write_stru                write_msg;
    hi_u32                            ret;

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == HI_NULL) {
        oam_error_log0(0, OAM_SF_CSI, "{wal_csi_switch::pst_cfg_net_dev is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev);
#endif
    write_msg.auc_value[0] = switch_flag;

    /* **************************************************************************
     抛事件到hmac层处理
     ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_CSI_SWITCH, sizeof(hi_u8));
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u8),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_CSI, "{wal_csi_switch::wal_send_cfg_event return err code [%d].}", ret);
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 传递CSI数据上报的回调函数指针到WAL层的获取白名单事件注册函数
 修改历史      :
  1.日    期   : 2019年2月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void wal_csi_register_data_report_cb(hi_wifi_csi_data_cb func)
{
    g_csi_data_func = func;
}

/* ****************************************************************************
 功能描述  : CSI参数配置
 修改历史      :
  1.日    期   : 2019年2月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_s32 wal_csi_set_config(const hi_char *ifname, hi_u32 report_min_interval, const hi_wifi_csi_entry *entry_list,
    hi_s32 entry_num)
{
    wal_msg_write_stru            write_msg;
    mac_csi_config_stru           csi_config;
    hi_u8 idx = 0;

    for (idx = 0; idx < (hi_u8)entry_num; idx++) {
        /* 判断采样周期是否符合范围(0---2^15)，可参考CSI上报数据格式文档 */
        if (entry_list->sample_period >= CSI_REPORT_PERIOD_BUTT) {
            oam_error_log1(0, OAM_SF_CSI, "{wal_csi_set_config::sample period of the %d entry is illegal.}", idx);
            return HI_FAIL;
        }
        /* 判断帧类型是否符合范围 */
        if (entry_list->frame_type > (CSI_FRAME_TYPE_DATA | CSI_FRAME_TYPE_MGMT | CSI_FRAME_TYPE_CTRL)) {
            oam_error_log1(0, OAM_SF_CSI, "{wal_csi_set_config::frame_type of the %d entry is illegal.}", idx);
            return HI_FAIL;
        }
        if (memcpy_s(csi_config.ast_csi_param[idx].mac_addr, WLAN_MAC_ADDR_LEN,
                     entry_list->mac, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_CSI, "{wal_csi_set_config::memcpy_s mac addr err!}");
            return HI_FAIL;
        }
        csi_config.ast_csi_param[idx].sample_period = entry_list->sample_period;
        csi_config.ast_csi_param[idx].frame_type = entry_list->frame_type;
        entry_list++;
    }
    csi_config.entry_num = idx;
    csi_config.report_min_interval = report_min_interval;

    oal_net_device_stru *netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_warning_buf(0, OAM_SF_CSI, "{wal_csi_set_config::ifname [%s] len [%d]is not found.}", (hi_char *)ifname,
            strlen(ifname));
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 对固定长度的数组进行初始化,或对固定长度的结构体进行内存初始化 */
    memset_s(csi_config.resv, sizeof(csi_config.resv), 0, sizeof(csi_config.resv));
    /* 将结构体数组mac_csi_param_stru填充到事件中去 */
    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), &csi_config, sizeof(mac_csi_config_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CSI, "{wal_csi_set_config::mem safe function err!}");
        return HI_FAIL;
    }
    /* **************************************************************************
                                抛事件到hmac层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_CSI_SET_CONFIG, sizeof(mac_csi_config_stru));
    hi_u32 send_event_ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_csi_config_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (send_event_ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_CSI, "{wal_csi_set_config::wal_send_cfg_event err code [%d]!}", send_event_ret);
        return (hi_s32)send_event_ret;
    }
    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : 解析HAMC层抛到WAL层的上报CSI数据事件
 修改历史      :
  1.日    期   : 2019年2月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_csi_data_report(frw_event_mem_stru *event_mem)
{
    frw_event_stru            *event = HI_NULL;
    mac_csi_data_stru         *csi_report_data = HI_NULL;
    hi_u8                      auc_csi_data[OAL_CSI_DATA_BUFF_SIZE + OAL_CSI_TSF_SIZE];
    hi_u32                     tsf_tmp;

    event = (frw_event_stru *)event_mem->puc_data;
    csi_report_data = (mac_csi_data_stru *)event->auc_event_data;

    /* 对数组初始化为0，避免因未初始化从而对下面数据造成影响 */
    if (memset_s(auc_csi_data, sizeof(auc_csi_data), 0, sizeof(auc_csi_data)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_csi_data_report::memset_s err!}");
        return HI_FAIL;
    }

    /* 取出从dmac上报的CSI数据的前4字节的32位时间戳存到tmp数组中，再强转获得10进制时间戳 */
    tsf_tmp = (hi_u32)(((csi_report_data->csi_data[3] & 0xFF) << 24) | /* 3 数组位数 24 左移24位 */
        ((csi_report_data->csi_data[2] & 0xFF) << 16) |                /* 2 数组位数 16 左移16位 */
        ((csi_report_data->csi_data[1] & 0xFF) << 8) |                 /* 8 左移8位 */
        (csi_report_data->csi_data[0] & 0xFF));
    /* 将本次获取的32位时间戳与上次时间戳比较，若小于上次时间戳则说明逻辑上报的时间戳反转重新计时，
     * 此时需向前进1，即向第33位进1
     */
    if (tsf_tmp < g_csi_tsf_tmp) {
        g_csi_tsf_val++;
    }
    /* g_csi_tsf_val用于统计时间戳反转次数 */
    auc_csi_data[3] = (hi_u8)(g_csi_tsf_val >> 24); /* 3 数组位数 24 右移24位 */
    auc_csi_data[2] = (hi_u8)(g_csi_tsf_val >> 16); /* 2 数组位数 16 右移16位 */
    auc_csi_data[1] = (hi_u8)(g_csi_tsf_val >> 8);  /* 8 右移8位 */
    auc_csi_data[0] = (hi_u8)(g_csi_tsf_val);

    /* 将本次时间戳放到全局变量，以便于和下一次上报的时间戳比较 */
    g_csi_tsf_tmp = tsf_tmp;

    /* 将原来dmac上报的184字节时间戳拼接4个字节，构成188字节数据，其中时间戳为拼接后的数据开头8字节，
     * 由原来32位变为64位
     */
    if (memcpy_s(auc_csi_data + OAL_CSI_TSF_SIZE, OAL_CSI_DATA_BUFF_SIZE, csi_report_data->csi_data,
        csi_report_data->data_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_csi_data_report::memcpy_s err!}");
        return HI_FAIL;
    }

    /* 调用用户传进来的函数名，发送给用户:上报的csi数据，上报的数据长度 */
    if (g_csi_data_func != HI_NULL) {
        g_csi_data_func(auc_csi_data, (hi_s32)(csi_report_data->data_len + OAL_CSI_TSF_SIZE));
    }
    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
/* ****************************************************************************
 功能描述  : 解析HAMC层抛到WAL层的发送ACTION帧事件
 修改历史      :
  1.日    期   : 2019年8月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_p2p_action_tx_status(frw_event_mem_stru *event_mem)
{
    frw_event_stru              *tx_status_event   = HI_NULL;
    mac_p2p_tx_status_stru      *p2p_tx_status     = HI_NULL;
    struct wireless_dev         *wdev              = HI_NULL;
    mac_vap_stru                *mac_vap           = HI_NULL;
    mac_vap_stru                *tmp_mac_vap       = HI_NULL;
    hi_u32                       netdev_index;
    oal_net_device_stru         *netdev   = HI_NULL;
    hi_u8                        is_get_net_device = HI_FALSE;
    hi_u32                       ret               = HI_FAIL;

    tx_status_event = (frw_event_stru *)event_mem->puc_data;
    p2p_tx_status = (mac_p2p_tx_status_stru *)tx_status_event->auc_event_data;

    mac_vap = mac_vap_get_vap_stru(tx_status_event->event_hdr.vap_id);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CSI, "{wal_p2p_action_tx_status::mac_vap is null.}");
        return HI_FAIL;
    }

    /* 获取到当前发送ACTION帧的net_device */
    for (netdev_index = 0; netdev_index < WLAN_VAP_NUM_PER_BOARD; netdev_index++) {
        netdev = oal_get_past_net_device_by_index(netdev_index);
        if (netdev != HI_NULL && netdev->mlPriv != HI_NULL) {
            tmp_mac_vap = (mac_vap_stru *)netdev->mlPriv;
            if (mac_vap->vap_id == tmp_mac_vap->vap_id) {
                is_get_net_device = HI_TRUE;
                break;
            }
        }
    }
    
    hi_unref_param(wdev);
    if (is_get_net_device == HI_TRUE) {
        wdev = netdev->ieee80211Ptr; /* past_net_device不会是空指针，误报lint,-g- lin_t !e613 */
        HdfWifiEventMgmtTxStatus(wdev->netdev, p2p_tx_status->puc_buf, p2p_tx_status->len, p2p_tx_status->ack);
        ret = HI_SUCCESS;
    }

    return ret;
}
#endif

/* ****************************************************************************
 功能描述  : 解析HAMC层抛到WAL层的MAC地址
**************************************************************************** */
hi_u32 wal_get_efuse_mac_from_dev(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    hi_u32 ret;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_get_efuse_mac_from_dev::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    ret = wal_set_dev_addr_from_efuse((const hi_char *)event->auc_event_data, WLAN_MAC_ADDR_LEN);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_TX, "{wal_get_efuse_mac_from_dev::set dev_addr fail!}");
    }

    return HI_SUCCESS;
}

hi_void wal_register_tx_params_callback(hi_wifi_report_tx_params_callback func)
{
    g_wal_report_tx_params_callback = func;
}

hi_u32 wal_report_tx_params(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    hi_wifi_report_tx_params *data = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_get_efuse_mac_from_dev::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    data = (hi_wifi_report_tx_params *)event->auc_event_data;

    if (g_wal_report_tx_params_callback == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    return g_wal_report_tx_params_callback(data);
}

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
hi_u32 wal_get_dbg_cal_data_from_dev(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    const hi_u8 data_strlen = 77; /* 77:数据字符串的长度 */
    hi_char data_str[data_strlen];
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_get_efuse_mac_from_dev::event_mem is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 固定长度的结构体进行内存初始化 */
    memset_s(data_str, data_strlen, '\0', data_strlen);
    event = (frw_event_stru *)event_mem->puc_data;
    hi_u32 *cal_data = (hi_u32 *)event->auc_event_data;
    if (snprintf_s(data_str, data_strlen, data_strlen - 1, "0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x",
        cal_data[0],          /* 0:6个补偿值之一 */
        cal_data[1],          /* 1:6个补偿值之一 */
        cal_data[2],          /* 2:6个补偿值之一 */
        cal_data[3],          /* 3:6个补偿值之一 */
        cal_data[4],          /* 4:6个补偿值之一 */
        cal_data[5],          /* 5:6个补偿值之一 */
        cal_data[6]) == -1) { /* 6:6个补偿值之一 */
        printk("ERROR\r\n");
        return HI_FAIL;
    }
    /* 写到wifi_cfg */
    if (firmware_write_cfg((hi_u8 *)WIFI_CFG_DBB_PARAMS, (hi_u8 *)data_str, data_strlen - 1) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "wal_get_dbg_cal_data_from_dev:: save to wifi_cfg failed!");
        printk("ERROR\r\n");
        return HI_FAIL;
    }
    printk("OK\r\n");
    return HI_SUCCESS;
}
#endif

#ifdef FEATURE_DAQ
/* ****************************************************************************
 功能描述  : 解析HAMC层抛到WAL层的数采状态事件
 修改历史      :
  1.日    期   : 2019年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_data_acq_status(frw_event_mem_stru *event_mem)
{
    frw_event_stru                  *event = HI_NULL;
    hmac_vap_stru                   *hmac_vap = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_cfg80211_mgmt_tx_status::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_TX, "{wal_cfg80211_mgmt_tx_status::pst_hmac_vap null.vap_id[%d]}",
            event->event_hdr.vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap->acq_status_filter = *(hi_u8 *)(event->auc_event_data);
    hmac_vap->station_info_query_completed_flag = HI_TRUE;
    hi_wait_queue_wake_up_interrupt(&(hmac_vap->query_wait_q));

    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : 解析HAMC层抛到WAL层的数采结果事件
 修改历史      :
  1.日    期   : 2019年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_data_acq_result(frw_event_mem_stru *event_mem)
{
    wlan_acq_result_addr_stru       *data_result_addr = HI_NULL;
    hmac_vap_stru                   *hmac_vap = HI_NULL;
    frw_event_stru                  *event = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_cfg80211_mgmt_tx_status::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_TX, "{wal_cfg80211_mgmt_tx_status::pst_hmac_vap null.vap_id[%d]}",
            event->event_hdr.vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    data_result_addr = (wlan_acq_result_addr_stru *)(event->auc_event_data);

    hmac_vap->acq_result_addr.start_addr = data_result_addr->start_addr;
    hmac_vap->acq_result_addr.middle_addr1 = data_result_addr->middle_addr1;
    hmac_vap->acq_result_addr.middle_addr2 = data_result_addr->middle_addr2;
    hmac_vap->acq_result_addr.end_addr = data_result_addr->end_addr;

    hmac_vap->station_info_query_completed_flag = HI_TRUE;
    hi_wait_queue_wake_up_interrupt(&(hmac_vap->query_wait_q));

    return HI_SUCCESS;
}
#endif

#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
/* ****************************************************************************
 功能描述  : 解析HAMC层抛到WAL层的信道切换事件
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_channel_switch_report(frw_event_mem_stru *event_mem)
{
    frw_event_stru              *event = HI_NULL;
    oal_net_device_stru         *netdev = HI_NULL;
    hi_s32                      l_freq;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_channel_switch_report::event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_TX, "{wal_channel_switch_report::get net device ptr is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    l_freq = *(hi_s32 *)event->auc_event_data;

    /* 调用内核接口，上报去关联结果 */
    hi_u32 ret = HdfWifiEventCsaChannelSwitch(netdev, l_freq);
    if (ret != HI_SUCCESS) {
        oam_error_log1(event->event_hdr.vap_id, OAM_SF_TX,
            "{wal_channel_switch_report::cfg80211_disconnected fail[%d]!}\r\n", ret);
        return ret;
    }

    hi_diag_log_msg_i1(0, "{wal_channel_switch_report new channel_freq %d!}", (hi_u32)l_freq);

    return HI_SUCCESS;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
