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

#ifndef __OAL_FILE_H__
#define __OAL_FILE_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/fs.h>
#include <linux/uaccess.h>
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <fs/driver.h>
#include <fcntl.h>
#include <hi_types.h>
#endif
#include "oal_mm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* 文件属性 */
#define OAL_O_ACCMODE           O_ACCMODE
#define OAL_O_RDONLY            O_RDONLY
#define OAL_O_WRONLY            O_WRONLY
#define OAL_O_RDWR              O_RDWR
#define OAL_O_CREAT             O_CREAT
#define OAL_O_TRUNC             O_TRUNC
#define OAL_O_APPEND            O_APPEND

#define OAL_SEEK_SET     SEEK_SET    /* Seek from beginning of file.  */
#define OAL_SEEK_CUR     SEEK_CUR    /* Seek from current position.  */
#define OAL_SEEK_END     SEEK_END    /* Set file pointer to EOF plus "offset" */

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define OAL_FILE_POS(pst_file)  (pst_file->fp->f_pos)
#define OAL_FILE_FAIL           HI_NULL
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define OAL_FILE_POS(pst_file)  oal_get_file_pos(pst_file)
#endif

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
  7 STRUCT定义
**************************************************************************** */
typedef struct _oal_file_stru_ {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    struct file *fp;
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    int fd;
#endif
} oal_file_stru;

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
 功能描述  : 打开文件
 输入参数  : pc_path: 文件路径，flags:打开方式,rights:打开权限
 输出参数  : 无
 返 回 值  : 文件句柄
**************************************************************************** */
static inline oal_file_stru *oal_file_open(const hi_char *pc_path, hi_s32 flags, hi_s32 rights)
{
    oal_file_stru *pst_file = NULL;
    pst_file = oal_kzalloc(sizeof(oal_file_stru), OAL_GFP_KERNEL);
    if (pst_file == NULL) {
        return HI_NULL;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    pst_file->fp = filp_open(pc_path, flags, rights);
    if (IS_ERR_OR_NULL(pst_file->fp)) {
        oal_free(pst_file);
        return HI_NULL;
    }
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    pst_file->fd = open(pc_path, flags, rights);
    if (pst_file->fd < 0) {
        oal_free(pst_file);
        return HI_NULL;
    }
#endif

    return pst_file;
}

/* ****************************************************************************
 功能描述  : 写文件
 输入参数  : file: 文件句柄
           : pc_string: 输入内容地址
           : ul_length: 输入内容长度
 输出参数  : 无
 返 回 值  : 文件句柄
**************************************************************************** */
static inline hi_u32 oal_file_write(const oal_file_stru *pst_file, const hi_char *pc_string, hi_u32 ul_length)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hi_u32 ul_ret;
    mm_segment_t fs;
    fs = get_fs();
    set_fs(KERNEL_DS);
    ul_ret = vfs_write(pst_file->fp, pc_string, ul_length, &(pst_file->fp->f_pos));
    set_fs(fs);
    return ul_ret;
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return (hi_u32)write(pst_file->fd, pc_string, ul_length);
#endif
}

/* ****************************************************************************
 功能描述  : 关闭文件
 输入参数  : pc_path: 文件路径
 输出参数  : 无
 返 回 值  : 文件句柄
**************************************************************************** */
static inline hi_void oal_file_close(oal_file_stru *pst_file)
{
    if (pst_file != HI_NULL) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        filp_close(pst_file->fp, NULL);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        close(pst_file->fd);
#endif
        oal_free(pst_file);
        pst_file = NULL;
    }
}

/* ****************************************************************************
 功能描述  : 内核读文件，从头开始读
 输入参数  : file:指向要读取的文件的指针
             puc_buf:从文件读出数据后存放的buf
             ul_count:指定要读取的长度
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
static inline hi_s32 oal_file_read(const oal_file_stru *pst_file, hi_u8 *pc_buf, hi_u32 ul_count)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return kernel_read(pst_file->fp, pc_buf, ul_count, &(pst_file->fp->f_pos));
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return read(pst_file->fd, pc_buf, ul_count);
#endif
}

static inline hi_s64 oal_file_lseek(const oal_file_stru *pst_file, hi_s64 offset, hi_s32 whence)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return vfs_llseek(pst_file->fp, offset, whence);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return lseek(pst_file->fd, offset, whence);
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_file.h */
