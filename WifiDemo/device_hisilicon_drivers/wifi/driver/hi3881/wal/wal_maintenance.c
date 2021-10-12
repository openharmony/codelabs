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
#include "wal_ioctl.h"
#include "wal_event_msg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
#define DATA_ACQ_WAITTIME (5 * HZ)
/* ****************************************************************************
 函 数 名  : dmac_data_acq_start_from_hso
 功能描述  : 执行histudo设置的数采选项
**************************************************************************** */
hi_u32 wal_data_acq_start_from_hso(const hi_char *puc_ifname, const wlan_data_acq_stru *data_acq)
{
    oal_net_device_stru      *netdev = HI_NULL;
    wal_msg_write_stru        write_msg;
    hi_u32                    ret;

    if ((puc_ifname == HI_NULL) || (data_acq == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_start_from_hso::puc_ifname/pst_data_acq is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "{wal_data_acq_start_from_hso::pst_netdev is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件头 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DATA_ACQ_START, sizeof(wlan_data_acq_stru));
    /* 填写消息体 */
    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), data_acq, sizeof(wlan_data_acq_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_open_wmm::mem safe function err!}");
        return HI_FAIL;
    }

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(wlan_data_acq_stru),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_open_wmm:return err code %d!}\r\n", ret);
        return HI_FAIL;
    }

    return ret;
}

/* ****************************************************************************
 函 数 名  : wal_data_acq_report_to_hso
 功能描述  : 上报histudo数据采集是否完成
**************************************************************************** */
hi_u8 wal_data_acq_status_to_hso(const hi_char *puc_ifname, const wlan_data_acq_stru *data_acq)
{
    wal_msg_write_stru write_msg;

    if ((puc_ifname == HI_NULL) || (data_acq == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_status_to_hso::puc_ifname/pst_data_acq is null!}\r\n");
        return HI_FALSE;
    }

    oal_net_device_stru *netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "{wal_data_acq_status_to_hso::pst_netdev is null!}");
        return HI_FALSE;
    }

    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_status_to_hso::cann't acquire mac_vap from netdev!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_status_to_hso::hmac_vap_get_vap_stru, return null!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    hmac_vap->station_info_query_completed_flag = HI_FALSE;

    /* 填写事件头 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DATA_ACQ_STATUS, sizeof(wlan_data_acq_stru));
    /* 填写消息体 */
    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), data_acq, sizeof(wlan_data_acq_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_status_to_hso::mem safe function err!}");
        return HI_FALSE;
    }

    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(wlan_data_acq_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_data_acq_status_to_hso::wal_send_cfg_event return err code %d!}\r\n", ret);
        return HI_FALSE;
    }

    ret = (hi_u32)hi_wait_event_timeout((hmac_vap->query_wait_q), /* use非wifi目录定义宏函数,误告警,lin_t e26告警屏蔽 */
        (HI_TRUE == hmac_vap->station_info_query_completed_flag), DATA_ACQ_WAITTIME);
    if (ret == 0) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "wal_data_acq_result_addr_to_hso:query temp timeout.ret:%d", ret);
        return HI_FALSE;
    }

    return hmac_vap->acq_status_filter;
}

/* ****************************************************************************
 函 数 名  : wal_data_acq_report_to_hso
 功能描述  : 上报histudo数据采集结果
**************************************************************************** */
hi_u32 wal_data_acq_result_addr_to_hso(const hi_char *puc_ifname, const wlan_data_acq_stru *data_acq,
    wlan_acq_result_addr_stru *data_result_addr)
{
    wal_msg_write_stru write_msg;

    if ((puc_ifname == HI_NULL) || (data_acq == HI_NULL) || (data_result_addr == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_result_addr_to_hso::parameter is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    oal_net_device_stru *netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "{wal_data_acq_result_addr_to_hso::pst_netdev is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_result_addr_to_hso::cann't acquire mac_vap from netdev!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_result_addr_to_hso::hmac_vap_get_vap_stru, return null!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    hmac_vap->station_info_query_completed_flag = HI_FALSE;

    /* 填写事件头 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DATA_ACQ_REPORT, sizeof(wlan_data_acq_stru));
    /* 填写消息体 */
    if (memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value), data_acq, sizeof(wlan_data_acq_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_data_acq_status_to_hso::mem safe function err!}");
        return HI_FAIL;
    }

    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(wlan_data_acq_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_data_acq_status_to_hso::wal_send_cfg_event return err code %d!}\r\n", ret);
        return HI_FAIL;
    }

    ret = (hi_u32)hi_wait_event_timeout((hmac_vap->query_wait_q), /* use非wifi目录定义宏函数,误告警,lin_t e26告警屏蔽 */
        (HI_TRUE == hmac_vap->station_info_query_completed_flag), DATA_ACQ_WAITTIME);
    if (ret == 0) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "wal_data_acq_result_addr_to_hso: query temp timeout[%d]", ret);
        return HI_FAIL;
    }

    data_result_addr->start_addr   = hmac_vap->acq_result_addr.start_addr;
    data_result_addr->middle_addr1 = hmac_vap->acq_result_addr.middle_addr1;
    data_result_addr->middle_addr2 = hmac_vap->acq_result_addr.middle_addr2;
    data_result_addr->end_addr     = hmac_vap->acq_result_addr.end_addr;

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
