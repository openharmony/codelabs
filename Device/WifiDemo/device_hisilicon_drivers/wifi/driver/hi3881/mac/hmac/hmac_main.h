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

#ifndef __HMAC_MAIN_H__
#define __HMAC_MAIN_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oam_ext_if.h"
#include "mac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  STRUCT定义
**************************************************************************** */
typedef struct {
    oal_semaphore_stru rxdata_sema;
    hi_s32 rxdata_taskid;
    oal_wait_queue_head_stru rxdata_wq;
    oal_netbuf_head_stru rxdata_netbuf_head;
    hi_u32 pkt_loss_cnt;
    hi_u8 rxthread_enable;
} hmac_rxdata_thread_stru;

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_void hmac_main_exit(hi_void);
hi_u32 hmac_main_init(hi_void);
hi_u32 hmac_config_send_event(const mac_vap_stru *mac_vap, wlan_cfgid_enum_uint16 cfg_id, hi_u16 us_len,
    const hi_u8 *puc_param);
hi_void hmac_rxdata_netbuf_enqueue(oal_netbuf_stru *netbuf);
hi_void hmac_rxdata_sched(hi_void);
hi_u8 hmac_get_rxthread_enable(hi_void);
hi_u32 hmac_init_event_process(frw_event_mem_stru *event_mem);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_MAIN_H__ */
