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

#ifndef __OAM_MAIN_H__
#define __OAM_MAIN_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oam_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define OAM_BEACON_HDR_LEN              24
#define OAM_TIMER_MAX_LEN               36
#define OAM_PRINT_CRLF_NUM              20              /* 输出换行符的个数 */

#define OAM_FEATURE_NAME_ABBR_LEN       12              /* 特性名称缩写最大长度 */

#define DEV_OM_FRAME_DELIMITER          0x7e
#define DEV_OM_MSG_TYPE_LOG             1

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oam_main */
