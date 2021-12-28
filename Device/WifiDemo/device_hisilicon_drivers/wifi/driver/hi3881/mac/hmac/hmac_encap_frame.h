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

#ifndef __HMAC_ENCAP_FRAME_H__
#define __HMAC_ENCAP_FRAME_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hmac_user.h"
#include "mac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数声明
**************************************************************************** */
hi_u16 hmac_mgmt_encap_deauth(const mac_vap_stru *mac_vap, hi_u8 *puc_data, const hi_u8 *da_mac_addr,
    hi_u16 us_err_code);
hi_u16 hmac_mgmt_encap_disassoc(const mac_vap_stru *mac_vap, hi_u8 *puc_data, const hi_u8 *da_mac_addr,
    hi_u16 us_err_code);
hi_u16 hmac_encap_sa_query_req(const mac_vap_stru *mac_vap, hi_u8 *puc_data, const hi_u8 *da_mac_addr,
    hi_u16 us_trans_id);
hi_u16 hmac_encap_sa_query_rsp(const mac_vap_stru *mac_vap, const hi_u8 *hdr, hi_u8 *puc_data);
hi_void hmac_check_sta_base_rate(hi_u8 *user, mac_status_code_enum_uint16 *pen_status_code);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_ENCAP_FRAME_H__ */
