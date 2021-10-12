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

#ifndef __HMAC_SCAN_H__
#define __HMAC_SCAN_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hmac_vap.h"
#include "mac_device.h"
#include "hmac_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* 扫描到的bss的老化时间，小于此值，下发新扫描请求时，不删除此bss信息 */
#define HMAC_SCAN_MAX_SCANNED_BSS_EXPIRE  3000 /* 3000 milliseconds */
/* 扫描到的bss的rssi老化时间 */
#define HMAC_SCAN_MAX_SCANNED_RSSI_EXPIRE 1000 /* 1000 milliseconds */
/* 前景扫描每个信道扫描次数 */
#define FGSCAN_SCAN_CNT_PER_CHANNEL 2

/* ****************************************************************************
  3 函数声明
**************************************************************************** */
hmac_scanned_bss_info *hmac_scan_find_scanned_bss_by_bssid(const hmac_bss_mgmt_stru *bss_mgmt, const hi_u8 *puc_bssid);
hi_u32 hmac_scan_proc_scanned_bss(frw_event_mem_stru *event_mem);
hi_u32 hmac_scan_proc_scan_comp_event(frw_event_mem_stru *event_mem);
hi_u32 hmac_scan_proc_scan_req_event_exception(hmac_vap_stru *hmac_vap);
hi_u32 hmac_scan_proc_scan_req_event(hmac_vap_stru *hmac_vap, mac_scan_req_stru *scan_params);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_u32 hmac_scan_proc_sched_scan_req_event(const hmac_vap_stru *hmac_vap, const mac_pno_scan_stru *pno_scan_params);
#endif
hi_void hmac_scan_init(hmac_device_stru *hmac_dev);
hi_void hmac_scan_exit(hmac_device_stru *hmac_dev);
hi_void hmac_scan_proc_check_ssid(hmac_scanned_bss_info *scanned_bss, const hi_u8 *puc_ssid, hi_u8 ssid_len);
hi_void hmac_scan_set_sour_mac_addr_in_probe_req(const hmac_vap_stru *hmac_vap, hi_u8 *sa_mac_addr, hi_u8 mac_addr_len,
    hi_u8 is_rand_mac_addr_scan, hi_u8 is_p2p0_scan);
#ifdef _PRE_WLAN_FEATURE_QUICK_START
hi_u32 hmac_scan_update_bss_dscr(hmac_scanned_bss_info *scanned_bss, const dmac_tx_event_stru *dtx_event, hi_u8 vap_id);
hi_void hmac_scan_add_bss_to_list(hmac_scanned_bss_info *scanned_bss, hmac_device_stru *hmac_dev);
hmac_scanned_bss_info *hmac_scan_alloc_scanned_bss(hi_u32 mgmt_len);
hi_void hmac_scan_clean_scan_record(hmac_scan_record_stru *scan_record);
#endif
hi_u32 hmac_scan_clean_result(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_SCAN_H__ */
