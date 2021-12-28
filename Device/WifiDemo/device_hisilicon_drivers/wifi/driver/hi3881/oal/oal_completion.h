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

#ifndef __OAL_COMPLETION_H__
#define __OAL_COMPLETION_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include <linux/completion.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
typedef struct completion oal_completion;

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
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
  7 宏定义
**************************************************************************** */
#define OAL_INIT_COMPLETION(_my_completion) init_completion(_my_completion)
#define OAL_COMPLETE(_my_completion) complete(_my_completion)
#define OAL_WAIT_FOR_COMPLETION(_my_completion) wait_for_completion(_my_completion)
#define OAL_COMPLETE_ALL(_my_completion) complete_all(_my_completion)

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 同步：等待超时检查
 输入参数  : 无
 输出参数  : 无
 返 回 值  : Return: 0 if timed out, and positive (at least 1, or number of jiffies left till timeout) if completed.
**************************************************************************** */
static inline hi_u32 oal_wait_for_completion_timeout(oal_completion *pst_completion, hi_u32 ul_timeout)
{
    return wait_for_completion_timeout(pst_completion, ul_timeout);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_completion.h */
