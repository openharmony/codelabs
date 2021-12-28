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
#ifndef __OAL_KERNEL_FILE_H__
#define __OAL_KERNEL_FILE_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#ifndef HAVE_PCLINT_CHECK

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/kobject.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#endif
#endif
#include "hi_types_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define OAL_KERNEL_DS           KERNEL_DS

/* 文件属性 */
#define OAL_O_ACCMODE           O_ACCMODE
#define OAL_O_RDONLY            O_RDONLY
#define OAL_O_WRONLY            O_WRONLY
#define OAL_O_RDWR              O_RDWR
#define OAL_O_CREAT             O_CREAT
#define OAL_O_TRUNC             O_TRUNC
#define OAL_O_APPEND            O_APPEND

#define OAL_PRINT_FORMAT_LENGTH 512                     /* 打印格式字符串的最大长度 */

typedef struct file             oal_file;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
typedef mm_segment_t            oal_mm_segment_t;
typedef struct device_attribute oal_device_attribute_stru;
typedef struct device           oal_device_stru;
#endif
typedef struct kobject          oal_kobject;

/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_s32 oal_sysfs_create_group(oal_kobject *kobj, const struct attribute_group *grp)
{
    return sysfs_create_group(kobj, grp);
}

static inline hi_void oal_sysfs_remove_group(oal_kobject *kobj, const struct attribute_group *grp)
{
    sysfs_remove_group(kobj, grp);
}

static inline hi_s32 oal_debug_sysfs_create_group(oal_kobject *kobj, const struct attribute_group *grp)
{
    return sysfs_create_group(kobj, grp);
}

static inline hi_void oal_debug_sysfs_remove_group(oal_kobject *kobj, const struct attribute_group *grp)
{
    sysfs_remove_group(kobj, grp);
}
#endif

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
extern oal_kobject *oal_get_sysfs_root_object(hi_void);
extern oal_kobject *oal_get_sysfs_root_boot_object(hi_void);
extern oal_kobject *oal_conn_sysfs_root_obj_init(hi_void);
extern hi_void oal_conn_sysfs_root_obj_exit(hi_void);
extern hi_void oal_conn_sysfs_root_boot_obj_exit(hi_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_main */
