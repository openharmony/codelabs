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

#ifndef __HMAC_MGMT_CLASSIFIER_H__
#define __HMAC_MGMT_CLASSIFIER_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "dmac_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数声明
**************************************************************************** */
hi_u32 hmac_rx_process_mgmt_event(frw_event_mem_stru *event_mem);
hi_u32 hmac_mgmt_rx_delba_event(frw_event_mem_stru *event_mem);
hi_u32 hmac_mgmt_tx_action(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, mac_action_mgmt_args_stru *action_args);
hi_u32 hmac_mgmt_tx_priv_req(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, mac_priv_req_args_stru *priv_req);
hi_u32 hmac_mgmt_send_disasoc_deauth_event(frw_event_mem_stru *event_mem);
hi_u32 hmac_proc_disasoc_misc_event(frw_event_mem_stru *event_mem);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 hmac_mesh_report_new_peer_candidate(frw_event_mem_stru *event_mem);
#endif
#ifdef _PRE_WLAN_FEATURE_PROMIS
hi_u32 hmac_rx_process_mgmt_promis(frw_event_mem_stru *event_mem);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_MGMT_CLASSIFIER_H__ */
