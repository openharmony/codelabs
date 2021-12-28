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

#ifndef __HMAC_ENCAP_FRAME_AP_H__
#define __HMAC_ENCAP_FRAME_AP_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "mac_vap.h"
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
typedef enum {
    /* 加密方式为open */
    HMAC_AP_AUTH_SEQ1_OPEN_ANY = 0,
    /* 加密方式为wep,处理重传帧 */
    HMAC_AP_AUTH_SEQ1_WEP_NOT_RESEND,
    /* 加密方式为wep,处理非重传帧 */
    HMAC_AP_AUTH_SEQ1_WEP_RESEND,
    /* 加密方式为open */
    HMAC_AP_AUTH_SEQ3_OPEN_ANY,
    /* 加密方式为WEP,assoc状态为auth comlete */
    HMAC_AP_AUTH_SEQ3_WEP_COMPLETE,
    /* 加密方式为WEP,assoc状态为assoc */
    HMAC_AP_AUTH_SEQ3_WEP_ASSOC,
    /* 什么也不做 */
    HMAC_AP_AUTH_DUMMY,

    HMAC_AP_AUTH_BUTT
} hmac_ap_auth_process_code_enum;
typedef hi_u8 hmac_ap_auth_process_code_enum_uint8;

/* ****************************************************************************
  3 STRUCT定义
**************************************************************************** */
typedef struct tag_hmac_auth_rsp_param_stru {
    /* 收到auth request是否为重传帧 */
    hi_u8 auth_resend;
    /* 记录是否为wep */
    hi_u8 is_wep_allowed;
    /* 记录认证的类型 */
    hi_u16 us_auth_type;
    /* 记录函数处理前，user的关联状态 */
    mac_user_asoc_state_enum_uint8 user_asoc_state;
    hi_u8 resv[3]; /* 3 保留字节 */
} hmac_auth_rsp_param_stru;

typedef hmac_ap_auth_process_code_enum_uint8 (*hmac_auth_rsp_fun)(const hmac_auth_rsp_param_stru *pst_auth_rsp_param,
    hi_u8 *puc_code, mac_user_asoc_state_enum_uint8 *pst_usr_ass_stat);

typedef struct tag_hmac_auth_rsp_handle_stru {
    hmac_auth_rsp_param_stru auth_rsp_param;
    hmac_auth_rsp_fun auth_rsp_fun;
} hmac_auth_rsp_handle_stru;

typedef struct hmac_asoc_rsp_ap_info_stru {
    hi_u8 *puc_sta_addr;
    hi_u8 *puc_asoc_rsp;

    hi_u16 status_code;
    hi_u8 assoc_id;
    hi_u8 rsv0;

    hi_u16 us_type;
    hi_u16 us_resv1;
} hmac_asoc_rsp_ap_info_stru;

typedef struct {
    hi_u8 *mac_addr;
    hi_u8 addr_len;
    hi_u8 resv[3]; /* 3 byte保留字段 */
} hmac_mac_addr_stru;

/* ****************************************************************************
  4 函数声明
**************************************************************************** */
hi_u16 hmac_encap_auth_rsp(mac_vap_stru *mac_vap, oal_netbuf_stru *auth_rsp, const oal_netbuf_stru *auth_req,
    hi_u8 *puc_chtxt, hi_u16 chtxt_len);

hi_u32 hmac_mgmt_encap_asoc_rsp_ap(mac_vap_stru *mac_ap, hmac_asoc_rsp_ap_info_stru *asoc_rsp_ap_info);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 hmac_encap_mesh_peering_open_frame(mac_vap_stru *mac_vap, hi_u8 *puc_data,
    const mac_action_data_stru *action_data);
hi_u32 hmac_encap_mesh_peering_confirm_frame(mac_vap_stru *mac_vap, hi_u8 *puc_data,
    const mac_action_data_stru *action_data);
hi_u32 hmac_encap_mesh_peering_close_frame(const mac_vap_stru *mac_vap, hi_u8 *puc_data,
    const mac_action_data_stru *action_data);
hi_u32 hmac_encap_mesh_group_key_inform_frame(const mac_vap_stru *mac_vap, hi_u8 *puc_data,
    const mac_action_data_stru *action_data);
hi_u32 hmac_encap_mesh_group_key_ack_frame(const mac_vap_stru *mac_vap, hi_u8 *puc_data,
    const mac_action_data_stru *action_data);

#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_ENCAP_FRAME_AP_H__ */
