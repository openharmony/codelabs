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

#ifndef __HMAC_TRAFFIC_CLASSIFY__
#define __HMAC_TRAFFIC_CLASSIFY__

/* ****************************************************************************
  1头文件包含
**************************************************************************** */
#include "oal_net.h"
#include "oal_ext_if.h"
#include "hmac_tx_data.h"
#include "mac_frame.h"
#include "mac_data.h"
#include "hmac_user.h"
#include "hmac_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2宏定义
**************************************************************************** */
/* ****************************************************************************
  3结构体
**************************************************************************** */
/* ****************************************************************************
    用户结构体: 包含了已识别业务、待识别业务序列
    这里借助hmac_user_stru，在hmac_user_stru结构体中添加宏定义字段:
    _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
**************************************************************************** */
/* ****************************************************************************
    RTP头结构体:结构体中没有包含最后的CSRC标识符
**************************************************************************** */
typedef struct {
    /* ---------------------------------------------------------------------------
                                    RTP头结构
    -----------------------------------------------------------------------------
    |version|P|X|   CSRC数   |M|          PT           |       序号             |
    |  2bit |1|1|    4bit    |1|        7bit           |         16bit          |
    -----------------------------------------------------------------------------
    |                               时间戳 32bit                                |
    -----------------------------------------------------------------------------
    |                                 SSRC 32bit                                |
    -----------------------------------------------------------------------------
    |               CSRC 每个CSRC标识符32bit 标识符个数由CSRC数决定             |
    --------------------------------------------------------------------------- */
    hi_u8       version_and_csrc;    /* 版本号2bit、填充位(P)1bit、扩展位(X)1bit、CSRC数目4bit */
    hi_u8       payload_type;        /* 标记1bit、有效载荷类型(PT)7bit */
    hi_u16      us_rtp_idx;          /* RTP发送序号 */
    hi_u32      rtp_time_stamp;      /* 时间戳 */
    hi_u32      ssrc;                /* SSRC */
} hmac_tx_rtp_hdr;

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
hi_void hmac_tx_traffic_classify(const hmac_tx_ctl_stru *tx_ctl, mac_ip_header_stru *ip, hi_u8 *puc_tid);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __HMAC_TRAFFIC_CLASSIFY__ */
