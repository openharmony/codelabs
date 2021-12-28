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
#ifndef __OAL_SDIO_IF_H__
#define __OAL_SDIO_IF_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifndef HAVE_PCLINT_CHECK
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/host.h>
#include <linux/pm_runtime.h>
#include <linux/random.h>
#endif
#include "oal_time.h"

#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/delay.h>
#endif
#include "hdf_ibus_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define sdio_get_max_block_count(func)  ((func)->card->host->max_blk_count)
#define sdio_get_max_req_size(func)     ((func)->card->host->max_req_size)
#define sdio_get_max_blk_size(func)     ((func)->card->host->max_blk_size)
#define sdio_get_max_seg_size(func)     ((func)->card->host->max_seg_size)
#define sdio_get_max_segs(func)         ((func)->card->host->max_segs)

/* *
 * sdio_memcpy_fromio - read a chunk of memory from a SDIO function
 * @func: SDIO function to access
 * @dst: buffer to store the data
 * @addr: address to begin reading from
 * @count: number of bytes to read
 *
 * Reads from the address space of a given SDIO function. Return
 * value indicates if the transfer succeeded or not.
 */
static inline hi_s32 oal_sdio_memcpy_fromio(struct BusDev *bus, hi_void *dst, hi_u32 addr, hi_s32 count)
{
    hi_s32 ret;
#ifdef CONFIG_HISI_SDIO_TIME_DEBUG
    oal_time_t_stru time_start;
    time_start = oal_ktime_get();
#endif
    ret = bus->ops.readData(bus, addr, count, (hi_u8 *)dst);
#ifdef CONFIG_HISI_SDIO_TIME_DEBUG
    if (oal_unlikely(ret)) {
        /* If sdio transfer failed, dump the sdio info */
        hi_u64 trans_us;
        oal_time_t_stru time_stop = oal_ktime_get();
        trans_us = (hi_u64)oal_ktime_to_us(oal_ktime_sub(time_stop, time_start));
        printk(KERN_WARNING "[E]sdio_memcpy_fromio fail=%d, time cost:%llu us,[dst:%p,addr:%u,count:%d]\n", ret,
            trans_us, dst, addr, count);
    }
#endif
    return ret;
}

#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

#ifdef CONFIG_MMC
#define sdio_get_max_block_count(func) (func->card->host->max_blk_num)
#define sdio_get_max_req_size(func)    (func->card->host->max_request_size)
#define sdio_get_max_blk_size(func)    (func->card->host->max_blk_size)
#define sdio_en_timeout(func)          (func->en_timeout_ms)

#define sdio_func_num(func) (func->func_num)

#define SDIO_ANY_ID (~0)

#define SDIO_DEVICE(vend, dev) .class = SDIO_ANY_ID,      \
                               .vendor = (vend),          \
                               .device = (dev)

struct sdio_device_id {
    unsigned char class;       /* Standard interface or SDIO_ANY_ID */
    unsigned short int vendor; /* Vendor or SDIO_ANY_ID */
    unsigned short int device; /* Device ID or SDIO_ANY_ID */
};

#define sdio_get_drvdata(func) (func->data)
#define sdio_set_drvdata(func, priv) (func->data = (void *)priv)

/*
 * SDIO function device driver
 */
struct sdio_driver {
    char *name;
    const struct sdio_device_id *id_table;
};

static inline hi_s32 sdio_register_driver(struct sdio_driver *driver)
{
    hi_unref_param(driver);
    return HI_SUCCESS;
}

/* *
 * sdio_memcpy_fromio - read a chunk of memory from a SDIO function
 * @func: SDIO function to access
 * @dst: buffer to store the data
 * @addr: address to begin reading from
 * @count: number of bytes to read
 *
 * Reads from the address space of a given SDIO function. Return
 * value indicates if the transfer succeeded or not.
 */
static inline hi_s32 oal_sdio_memcpy_fromio(struct BusDev *bus, hi_void *dst, hi_u32 addr, hi_s32 count)
{
    hi_s32 ret;
#ifdef CONFIG_HISI_SDIO_TIME_DEBUG
    oal_time_t_stru time_start;
    time_start = oal_ktime_get();
#endif
    ret = bus->ops.readData(bus, addr, count, (hi_u8 *)dst);
#ifdef CONFIG_HISI_SDIO_TIME_DEBUG
    if (oal_unlikely(ret)) {
        /* If sdio transfer failed, dump the sdio info */
        hi_u64 trans_us;
        oal_time_t_stru time_stop = oal_ktime_get();
        trans_us = (hi_u64)oal_ktime_to_us(oal_ktime_sub(time_stop, time_start));
        printk(KERN_WARNING"[E]sdio_memcpy_fromio fail=%d, time cost:%llu us,[dst:%p,addr:%u,count:%d]\n",
               ret, trans_us, dst, addr, count);
    }
#endif
    return ret;
}

#else /* CONFIG_MMC */

#define sdio_get_max_block_count(func) (func->card->host->max_blk_count)
#define sdio_get_max_req_size(func) (func->card->host->max_req_size)
#define sdio_get_max_blk_size(func) (func->card->host->max_blk_size)
#define sdio_en_timeout(func) (func->enable_timeout)
#define sdio_func_num(func) (func->num)

/* *
 * sdio_memcpy_fromio - read a chunk of memory from a SDIO function
 * @func: SDIO function to access
 * @dst: buffer to store the data
 * @addr: address to begin reading from
 * @count: number of bytes to read
 *
 * Reads from the address space of a given SDIO function. Return
 * value indicates if the transfer succeeded or not.
 */
static inline hi_s32 oal_sdio_memcpy_fromio(struct sdio_func *func, hi_void *dst, hi_u32 addr, hi_s32 count)
{
    hi_s32 ret;
#ifdef CONFIG_HISI_SDIO_TIME_DEBUG
    oal_time_t_stru time_start;
    time_start = oal_ktime_get();
#endif
    ret = sdio_memcpy_fromio(func, dst, addr, count);
#ifdef CONFIG_HISI_SDIO_TIME_DEBUG
    if (oal_unlikely(ret)) {
        /* If sdio transfer failed, dump the sdio info */
        hi_u64 trans_us;
        oal_time_t_stru time_stop = oal_ktime_get();
        trans_us = (hi_u64)oal_ktime_to_us(oal_ktime_sub(time_stop, time_start));
        printk(KERN_WARNING"[E]sdio_memcpy_fromio fail=%d, time cost:%llu us,[dst:%p,addr:%u,count:%d]\n",
               ret, trans_us, dst, addr, count);
    }
#endif
    return ret;
}

static inline hi_s32 sdio_register_driver(struct sdio_driver *driver)
{
    return HI_SUCCESS;
}

static inline hi_void sdio_claim_host(struct sdio_func *func)
{
    return;
}

static inline hi_void sdio_release_host(struct sdio_func *func)
{
    return;
}

static inline int sdio_require_irq(struct sdio_func *func, sdio_irq_handler_t *handler)
{
    return sdio_claim_irq(func, handler);
}
#endif /* end of CONFIG_MMC */

#endif /* end of (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_sdio_if.h */
