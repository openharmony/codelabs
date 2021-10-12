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

#ifndef __FRW_MAIN_H__
#define __FRW_MAIN_H__

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
#define FRW_TASK_SIZE 0x2000 /* 驱动task栈大小,默认3k,可初始化配置 */
/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* 此枚举用于定义从FRW模块以上各模块的初始化状态 */
typedef enum {
    FRW_INIT_STATE_START,    /* 表示初始化刚启动，即FRW初始化开始 */
    FRW_INIT_STATE_FRW_SUCC, /* 表示FRW模块初始化成功 */
    FRW_INIT_STATE_HAL_SUCC, /* 表示HAL模块初始化成功 */
    /* 表示DMAC模块启动初始化成功，配置VAP已正常；如果在此状态之后再初始化，即为业务VAP的初始化 */
    FRW_INIT_STATE_DMAC_CONFIG_VAP_SUCC,
    /* 表示HMAC模块启动初始化成功，配置VAP已正常；如果在此状态之后再初始化，即为业务VAP的初始化 */
    FRW_INIT_STATE_HMAC_CONFIG_VAP_SUCC,
    FRW_INIT_STATE_ALL_SUCC, /* 此状态表示HMAC以上模块均已初始化成功 */

    FRW_INIT_STATE_BUTT
} frw_init_enum;
typedef hi_u8 frw_init_enum_uint8;

/* ****************************************************************************
  枚举说明: FRW模块ROM化预留回调接口类型定义
**************************************************************************** */
typedef enum {
    FRW_ROM_RESV_FUNC_QUEUE_INIT,
    FRW_ROM_RESV_FUNC_BUTT,
} frw_rom_resv_func_enum;
typedef hi_u8 frw_rom_resv_func_enum_uint8;

/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 frw_main_init(hi_bool mode, hi_u32 task_size);
hi_void frw_main_exit(hi_void);
hi_bool frw_get_offload_mode(hi_void);
hi_void frw_set_init_state(frw_init_enum_uint8 init_state);
frw_init_enum_uint8 frw_get_init_state(hi_void);
hi_void frw_set_rom_resv_func(frw_rom_resv_func_enum_uint8 func_id, hi_void *func);
hi_void *frw_get_rom_resv_func(frw_rom_resv_func_enum_uint8 func_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of frw_main */
