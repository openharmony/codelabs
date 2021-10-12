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

#ifndef __WAL_EVENT_MSG_H__
#define __WAL_EVENT_MSG_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "wlan_mib.h"
#include "frw_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define WAL_MSG_WRITE_MSG_HDR_LENGTH    (sizeof(wal_msg_hdr_stru))
#define WAL_MSG_WRITE_MAX_LEN           (WLAN_MEM_EVENT_SIZE2 - FRW_EVENT_HDR_LEN - WAL_MSG_WRITE_MSG_HDR_LENGTH)
#define WAL_MSG_HDR_LENGTH              sizeof(wal_msg_hdr_stru)
#define WAL_MSG_WID_LENGTH              sizeof(wlan_cfgid_enum_uint16)
#define WAL_MSG_QUERY_LEN               16
#define WAL_BW_STR_MAX_LEN              20

/* 填写配置消息头 */
#define wal_cfg_msg_hdr_init(_pst_cfg_msg_hdr, _en_type, _us_len, _uc_sn) \
    do {                                                                  \
        (_pst_cfg_msg_hdr)->msg_type = (_en_type);                        \
        (_pst_cfg_msg_hdr)->us_msg_len = (_us_len);                       \
        (_pst_cfg_msg_hdr)->msg_sn = (_uc_sn);                            \
    } while (0)

/* 填写write msg消息头 */
#define wal_write_msg_hdr_init(_pst_write_msg, _en_wid, _us_len) \
    do {                                                         \
        (_pst_write_msg)->wid = (_en_wid);                       \
        (_pst_write_msg)->us_len = (_us_len);                    \
    } while (0)

/* ****************************************************************************
  4 枚举定义
**************************************************************************** */
/* 配置消息类型 */
typedef enum {
    WAL_MSG_TYPE_QUERY,    /* 查询 */
    WAL_MSG_TYPE_WRITE,    /* 设置 */
    WAL_MSG_TYPE_RESPONSE, /* 返回 */

    WAL_MSG_TYPE_BUTT
} wal_msg_type_enum;
typedef hi_u8 wal_msg_type_enum_uint8;

/* ****************************************************************************
  5 STRUCT定义
**************************************************************************** */
/* 配置消息头 */
typedef struct {
    wal_msg_type_enum_uint8 msg_type; /* msg type:W or Q */
    hi_u8  msg_sn;                     /* msg 序列号 */
    hi_u16 us_msg_len;                /* msg 长度 */
} wal_msg_hdr_stru;

/* 配置消息 */
typedef struct {
    wal_msg_hdr_stru msg_hdr; /* 配置消息头 */
    hi_u8 auc_msg_data[];     /* 配置消息payload */
} wal_msg_stru;

/* write消息时的返回消息 */
typedef struct {
    wlan_cfgid_enum_uint16 wid;
    hi_u8  auc_resv[2]; /* 2: 保留字节 */
    hi_u32 err_code;   /* write消息返回的错误码 */
} wal_msg_write_rsp_stru;

/* write消息格式 */
typedef struct {
    wlan_cfgid_enum_uint16 wid;
    hi_u16 us_len;
    hi_u8  auc_value[WAL_MSG_WRITE_MAX_LEN];
} wal_msg_write_stru;

/* response消息格式，与Write消息格式相同 */
typedef wal_msg_write_stru wal_msg_rsp_stru;

/* query消息格式:2字节WID x N */
typedef struct {
    wlan_cfgid_enum_uint16 wid;
} wal_msg_query_stru;

/* WMM SET消息格式 */
typedef struct {
    wlan_cfgid_enum_uint16 cfg_id;
    hi_u8  uc_resv[2]; /* 2: 保留字节 */
    hi_u32 ac;
    hi_u32 value;
} wal_msg_wmm_stru;

/* WMM query消息格式:2字节WID x N */
typedef struct {
    wlan_cfgid_enum_uint16 wid;
    hi_u8 uc_resv[2]; /* 2: 保留字节 */
    hi_u32 ac;
} wal_msg_wmm_query_stru;

/* WID request struct */
typedef struct {
    hi_list             entry;
    uintptr_t           request_address;
    hi_void             *resp_mem;
    hi_u32              resp_len;
    hi_u32              ret;
} wal_msg_request_stru;

typedef struct {
    uintptr_t request_address;
} wal_msg_rep_hdr;

typedef struct {
    oal_spin_lock_stru st_lock;
    hi_list head;
    oal_wait_queue_head_stru wait_queue;
    hi_u32 count;
} wal_msg_queue;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

hi_u32 wal_check_and_release_msg_resp(wal_msg_stru *rsp_msg);
hi_void wal_msg_queue_init(hi_void);
hi_void wal_cfg_msg_task_sched(hi_void);
hi_u32 wal_set_msg_response_by_addr(hi_u32 addr, hi_u8 *resp_mem, hi_u32 resp_ret, hi_u32 rsp_len);

#endif /* end of wal_event_msg.h */
