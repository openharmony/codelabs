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
#ifndef __OAL_MM_H__
#define __OAL_MM_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/slab.h>
#include <linux/hardirq.h>
#include <linux/vmalloc.h>
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <string.h>
#endif
#include "hi_types_base.h"
#include "hi_stdlib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define OAL_GFP_KERNEL GFP_KERNEL
#define OAL_GFP_ATOMIC GFP_ATOMIC

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 申请核心态的内存空间，并填充0。对于Linux操作系统而言，
             需要考虑中断上下文和内核上下文的不同情况(GFP_KERNEL和GFP_ATOMIC)。
 输入参数  : ul_size: alloc mem size
 返 回 值  : alloc mem addr
**************************************************************************** */
static inline hi_void *oal_memalloc(hi_u32 ul_size)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hi_s32 l_flags = GFP_KERNEL;
    hi_void *puc_mem_space = HI_NULL;

    /* 不睡眠或在中断程序中标志置为GFP_ATOMIC */
    if (in_interrupt() || irqs_disabled()) {
        l_flags = GFP_ATOMIC;
    }

    puc_mem_space = kmalloc(ul_size, l_flags);
    if (puc_mem_space == HI_NULL) {
        return HI_NULL;
    }

    return puc_mem_space;
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return malloc(ul_size);
#endif
}

static inline hi_void *oal_kzalloc(hi_u32 ul_size, hi_s32 l_flags)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return kzalloc(ul_size, l_flags);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    (void)(l_flags);
    return calloc(1, ul_size);
#endif
}

static inline hi_void *oal_vmalloc(hi_u32 ul_size)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return vmalloc(ul_size);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return malloc(ul_size);
#endif
}

/* ****************************************************************************
 功能描述  : 释放核心态的内存空间。
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_void oal_free(const hi_void *p_buf)
{
    kfree(p_buf);
}
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
static inline hi_void oal_free(hi_void *p_buf)
{
    free(p_buf);
}
#endif

static inline hi_void oal_vfree(hi_void *p_buf)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    vfree(p_buf);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    free(p_buf);
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_mm.h */
