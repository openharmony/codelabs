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

#ifndef __WAL_REGDB_H__
#define __WAL_REGDB_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#ifndef array_size
#define array_size(array) (sizeof(array) / sizeof((array)[0]))
#endif

/* ****************************************************************************
  3 全局变量声明
**************************************************************************** */
extern const oal_ieee80211_regdomain_stru g_default_regdom;

/* ****************************************************************************
  4 函数声明
**************************************************************************** */
const oal_ieee80211_regdomain_stru *wal_regdb_find_db(const hi_char *pc_str);
hi_void wal_set_cfg_regdb(const oal_ieee80211_regdomain_stru *regdom);
const oal_ieee80211_regdomain_stru *wal_get_cfg_regdb(hi_void);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
struct callback_head {
    struct callback_head *next;
    hi_void (*func)(struct callback_head *head);
};
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
#define RCU_HEAD callback_head

struct ieee80211_regdomain {
    struct RCU_HEAD RCU_HEAD;
    hi_u32 n_reg_rules;
    hi_char alpha2[2]; /* 元素个数为2 */
    hi_u8 dfs_region;
    struct ieee80211_reg_rule reg_rules[];
};

#ifndef ARRAY_SIZE
#define array_size(array) (sizeof(array) / sizeof((array)[0]))
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wal_regdb.h */
