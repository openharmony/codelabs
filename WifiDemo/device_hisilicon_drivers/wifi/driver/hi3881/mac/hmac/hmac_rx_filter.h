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

#ifndef __HMAC_RX_FILTER_H__
#define __HMAC_RX_FILTER_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hi_types.h"
#include "oal_err_wifi.h"
#include "mac_vap.h"
#include "mac_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数声明
**************************************************************************** */
hi_u32 hmac_set_rx_filter_value(const mac_vap_stru *mac_vap);
hi_u32 hmac_calc_up_vap_num(const mac_device_stru *mac_dev);
hi_u8 hmac_find_is_ap_up(const mac_device_stru *mac_dev);
hi_u8 hmac_find_is_ap(const mac_device_stru *mac_dev);
#ifdef _PRE_WLAN_FEATURE_PROMIS
hi_u8 hwal_get_promis_filter(void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_RX_FILTER_H__ */
