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

#ifndef __OAL_DATA_COLLECT_H__
#define __OAL_DATA_COLLECT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 配置命令含义 */
typedef enum {
    WLAN_DEBUG_DATA_ACQ    = 0,       /* DEBUG数据采集 */
    WLAN_ABB_DATA_ACQ      = 1,       /* ABB数据采集 */
    WLAN_ABB_DATA_GEN      = 2,       /* ABB数据产生 */
    WLAN_CBB_DATA_ACQ      = 3,       /* CBB数据采集 */
    WLAN_MAC_DATA_ACQ      = 4,       /* MAC数据采集 */
    WLAN_PHY_DATA_ACQ      = 5,       /* PHY数据采集 */
    WLAN_CPU_TRACE_ACQ     = 6,       /* CPU_TRACE */
    WLAN_CPU_MONITOR_ACQ   = 7,       /* CPU_PC_MONITOR */
    WLAN_DATA_ACQ_STATUS   = 8,       /* 数据采集完成状态查询 */
    WLAN_DATA_ACQ_RECODE   = 9,       /* 数据采集完成获取数据 */

    WLAN_DATA_ACQ_BUTT
} wlan_data_acq_enum;
typedef hi_u8 wlan_data_acq_enum_uint8;

/* 数据采集状态 */
typedef enum {
    WLAN_DATA_ACQ_STATUS_INIT            = 0,  /*       初始化状态      */
    WLAN_DATA_ACQ_STATUS_ENABLE          = 1,  /*    处于数据采集状态   */
    WLAN_DATA_ACQ_STATUS_COMPLETE        = 2,  /*  处于数据采集完成状态 */

    WLAN_DATA_ACQ_STATUS_BUTT
} wlan_data_acq_status_enum;
typedef hi_u8 wlan_data_acq_status_enum_uint8;

/* 数据采集配置结构体 */
typedef struct {
    hi_u8                             vap_id;           /* 采集vap id */
    hi_u8                             monitor_sel;      /* 采集memory选择 */
    hi_u8                             trace_recycle;    /* 采集存储方式 */
    hi_u8                             monitor_mode;     /* 采集源头选择 */
    wlan_data_acq_enum_uint8          monitor_type;     /* 采集模式 */
    hi_u8                             mac_acq_type;     /* MAC数采类型选择 */
    hi_u8                             mac_acq_subtype;  /* MAC数采子类型选择 */
    wlan_data_acq_status_enum_uint8   daq_status;       /* 当前是否允许使能数据采集功能 */
    hi_u16                            us_monitor_laddr;    /* 初始地址配置 */
    hi_u16                            us_monitor_haddr;    /* 结束地址配置 */
    hi_u32                            phy_acq_type:4,      /* PHY数采类型选择 */
                                      phy_test_node_sel:8, /* PHY采样点选择 */
                                      phy_trig_cond:4,     /* PHY采样触发条件 */
                                      phy_smp_aft_trig:16; /* PHY采样触发后采样点数 */
} wlan_data_acq_stru;

typedef struct {
    hi_u32                            start_addr;     /* 初始地址 */
    hi_u32                            middle_addr1;   /* 中间地址1 */
    hi_u32                            middle_addr2;   /* 中间地址2 */
    hi_u32                            end_addr;       /* 结束地址 */
} wlan_acq_result_addr_stru;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
