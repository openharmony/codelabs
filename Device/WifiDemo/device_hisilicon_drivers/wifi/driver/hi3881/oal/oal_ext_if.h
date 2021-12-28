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

#ifndef __OAL_EXT_IF_H__
#define __OAL_EXT_IF_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hi_types.h"
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "oal_time.h"
#include "oal_netbuf.h"
#include "oal_thread.h"
#include "oal_workqueue.h"
#include "oal_channel_host_if.h"
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "oal_workqueue.h"
#include "oal_wait.h"
#include "oal_time.h"
#include "oal_timer.h"
#include "oal_netbuf.h"
#include "oal_cfg80211.h"
#include "oal_thread.h"
#include "oal_spinlock.h"
#include "oal_semaphore.h"
#include "oal_completion.h"
#include "oal_sdio_comm.h"
#include "oal_sdio_host_if.h"
#include "oal_wakelock.h"
#include "oal_mutex.h"
#include "oal_channel_host_if.h"
#include "oal_atomic.h"
#endif

#include "hi_list.h"
#include "hi_task.h"
#include "hi_time.h"
#include "oal_net.h"
#include "oal_err_wifi.h"
#include "oal_util.h"
#include "oal_queue.h"
#include "oal_mem.h"
#include "wlan_types.h"
#include "wlan_spec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* CSI特性中上报CSI数据的BUFF块数和白名单中最大MAC地址的宏 */
#define OAL_CSI_DATA_BUFF_NUM        6
#define OAL_CSI_MAX_MAC_NUM          6      /* 白名单中最多6个MAC地址 */
#ifdef _PRE_WLAN_FEATURE_CSI
#define OAL_CSI_DATA_REPORT_PERIOD   50
#define OAL_CSI_DATA_BUFF_SIZE       184    /* 上报的csi数据大小 */
#define OAL_CSI_TSF_SIZE             4      /* TSF时间戳大小 */
#define OAL_MEM_CSI_DATA_SIZE        (OAL_CSI_DATA_BUFF_SIZE * OAL_CSI_DATA_BUFF_NUM)
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
#define OAL_P2P_CTWINDOW_MAX 127
#endif

/* 用于查询产测校准参数和MAC地址 */
#define AT_RSP_ATE_PARAS         1
#define AT_RSP_ATE_MAC           2

#define AT_RSP_OK                0    /* AT命令执行成功标志 */
#define AT_ATE_MAC_SIZE          8    /* MAC地址占字节数(含填充字节) */
#define oal_get_at_rsp_size(at_rsp) (((at_rsp)->data_num + 1) << 2)  /* 2:左移2位,word转字节数 */
/* ****************************************************************************
  2 结构体定义
**************************************************************************** */
typedef struct {
    hi_s8   result;   /* 0:执行成功,1:执行失败 */
    hi_u8   data_num; /* 数据个数,每个4字节 */
    hi_u16  num;      /* 结果模式序号 */
} oal_at_rsp_stru;
/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
typedef enum {
    OAL_TRACE_ENTER_FUNC,
    OAL_TRACE_EXIT_FUNC,

    OAL_TRACE_DIRECT_BUTT
} oal_trace_direction_enum;
typedef hi_u8 oal_trace_direction_enum_uint8;

typedef enum {
    OAL_AT_ATE_PARAS_USED_CNT = 0,
    OAL_AT_ATE_PARAS_FREQ_OFFSET,
    OAL_AT_ATE_PARAS_BPWR_OFFSET_0,
    OAL_AT_ATE_PARAS_BPWR_OFFSET_1,
    OAL_AT_ATE_PARAS_BPWR_OFFSET_2,
    OAL_AT_ATE_PARAS_DBB_OFFSET_11N,
    OAL_AT_ATE_PARAS_DBB_OFFSET_11G,
    OAL_AT_ATE_PARAS_DBB_OFFSET_11B = 7,
    OAL_AT_ATE_PARAS_DBB_SCALE_0,
    OAL_AT_ATE_PARAS_DBB_SCALE_1,
    OAL_AT_ATE_PARAS_DBB_SCALE_2,
    OAL_AT_ATE_PARAS_DBB_SCALE_3,
    OAL_AT_ATE_PARAS_DBB_SCALE_4 = 12,
    OAL_AT_ATE_PARAS_HYBRID_DATA,

    OAL_AT_ATE_PARAS_BUTT
} oal_at_ate_paras_enum;
typedef hi_u8 oal_at_ate_paras_enum_uint8;

typedef enum {
    OAL_AT_ATE_MAC_ADDR = 0,
    OAL_AT_ATE_MAC_TIME_LEFT = 2,

    OAL_AT_ATE_MAC_BUTT
} oal_at_ate_mac_enum;
typedef hi_u8 oal_at_ate_mac_enum_uint8;

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 oal_main_init(const hi_u8 vap_num, const hi_u8 user_num);
hi_void oal_main_exit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_ext_if.h */
