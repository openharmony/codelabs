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

#ifndef __WAL_SCAN_H__
#define __WAL_SCAN_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "wal_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_void wal_inform_all_bss(const oal_net_device_stru *netdev, oal_wiphy_stru *wiphy, hmac_bss_mgmt_stru *bss_mgmt,
    hi_u8 vap_id);
hi_u32 wal_start_scan_req(oal_net_device_stru *netdev, hmac_scan_stru *scan_mgmt);
hi_u32 wal_force_scan_complete(oal_net_device_stru *netdev);
hi_void wal_free_scan_mgmt_resource(hmac_scan_stru *scan_mgmt);

#define is_p2p_scan_req(request)                                                                                      \
    (((request)->n_ssids > 0) && (NULL != (request)->ssids) && ((request)->ssids[0].ssid_len == strlen("DIRECT-")) && \
        (0 == memcmp((request)->ssids[0].ssid, "DIRECT-", strlen("DIRECT-"))))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wal_scan.h */
