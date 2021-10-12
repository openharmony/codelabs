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

#ifndef __WAL_MAINTENANCE_H__
#define __WAL_MAINTENANCE_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hi_types.h"
#include "oal_data_collect.h"

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
hi_u32 wal_data_acq_start_from_hso(const hi_char *puc_ifname, const wlan_data_acq_stru *data_acq);
hi_u8 wal_data_acq_status_to_hso(const hi_char *puc_ifname, const wlan_data_acq_stru *data_acq);
hi_u32 wal_data_acq_result_addr_to_hso(const hi_char *puc_ifname, const wlan_data_acq_stru *data_acq,
    wlan_acq_result_addr_stru *data_result_addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wal_maintenance.h */
