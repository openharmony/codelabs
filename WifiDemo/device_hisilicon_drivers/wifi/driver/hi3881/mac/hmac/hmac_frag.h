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

#ifndef __HMAC_FRAG_H__
#define __HMAC_FRAG_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "hmac_tx_data.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define HMAC_FRAG_TIMEOUT   2000
#define HMAC_MAX_FRAG_SIZE  1600

/* ****************************************************************************
  3 函数声明
**************************************************************************** */
oal_netbuf_stru *hmac_defrag_process(hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf, hi_u32 hrdsize);
hi_u32 hmac_frag_process_proc(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf,
    hmac_tx_ctl_stru *tx_ctl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_FRAG_H__ */
