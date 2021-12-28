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

#ifndef __HMAC_UAPSD_H__
#define __HMAC_UAPSD_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "mac_user.h"
#include "hmac_ext_if.h"
#include "dmac_ext_if.h"
#include "hmac_user.h"
#include "hmac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define HMAC_UAPSD_SEND_ALL 0xff /* 发送队列中所有报文,设置为UINT8变量最大值 */
#define HMAC_UAPSD_WME_LEN  8

/* ****************************************************************************
  3 函数声明
**************************************************************************** */
hi_u32 hmac_config_set_uapsden(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param);
hi_void hmac_uapsd_update_user_para(const hi_u8 *puc_mac_hdr, hi_u8 sub_type, hi_u32 msg_len,
    const hmac_user_stru *hmac_user);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_UAPSD_H__ */
