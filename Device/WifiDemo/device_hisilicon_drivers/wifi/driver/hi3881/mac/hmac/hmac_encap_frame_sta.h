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

#ifndef __PREPARE_FRAME_STA_H__
#define __PREPARE_FRAME_STA_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hi_types.h"
#include "oal_err_wifi.h"
#include "hmac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数声明
**************************************************************************** */
hi_u32 hmac_mgmt_encap_asoc_req_sta(const hmac_vap_stru *hmac_vap, hi_u8 *puc_req_frame);
hi_u16 hmac_mgmt_encap_auth_req(const hmac_vap_stru *hmac_vap, hi_u8 *puc_mgmt_frame);
hi_u16 hmac_mgmt_encap_auth_req_seq3(const hmac_vap_stru *hmac_vap, hi_u8 *puc_mgmt_frame, hi_u8 *puc_mac_hrd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __PREPARE_FRAME_STA_H__ */
