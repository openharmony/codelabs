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

/* ****************************************************************************
  1 Header File Including
**************************************************************************** */
#include "oal_sdio.h"
#include "oal_sdio_host_if.h"
#include "oal_mm.h"

#include "plat_sdio.h"
#include "plat_pm.h"
#include "plat_firmware.h"
#include "oal_time.h"
#include "oam_ext_if.h"

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
extern struct sdio_func *g_p_gst_sdio_func;
#endif

/* ****************************************************************************
  2 Global Variable Definition
**************************************************************************** */
/* ****************************************************************************
  3 Function Definition
**************************************************************************** */
/*
 * Description  : provide interface for pm driver
 * Input        : hi_u8* buf, hi_u32 len
 * Output       : None
 * Return Value : hi_s32
 *
 */
hi_s32 sdio_patch_writesb(hi_u8 *buf, hi_u32 len)
{
    int ret;
    struct BusDev *bus = oal_get_bus_default_handler();

    if (bus == NULL || bus->priData.data == NULL) {
        return -FAILURE;
    }

    if (buf == NULL || len == 0) {
        return -FAILURE;
    }

    len = HISDIO_ALIGN_4_OR_BLK(len);

    bus->ops.claimHost(bus);
    ret = bus->ops.bulkWrite(bus, HISDIO_REG_FUNC1_FIFO, len, buf, 0);
    bus->ops.releaseHost(bus);
    return ret;
}

/*
 * Description  : provide interface for pm driver
 * Input        : hi_u8* buf, hi_u32 len hi_u32 timeout (ms)
 * Output       : None
 * Return Value : hi_s32
 */
hi_s32 sdio_patch_readsb(hi_u8 *buf, hi_u32 len, hi_u32 timeout)
{
    hi_u8   int_mask;
    hi_u8  *ver_info = HI_NULL;
    int     ret = 0;
    unsigned long timeout_jiffies;
    hi_u32  xfer_count;
    hi_u32  i;
    struct BusDev *bus = oal_get_bus_default_handler();
    hi_u8  *bus_buf = NULL;

    if (bus == NULL || bus->priData.data == NULL) {
        return -FAILURE;
    }

    if (buf == NULL || len == 0) {
        return -FAILURE;
    }
    bus->ops.claimHost(bus);
    timeout_jiffies = OAL_TIME_JIFFY + OAL_MSECS_TO_JIFFIES(timeout);
    for (;;) {
        ret =  bus->ops.readData(bus, HISDIO_REG_FUNC1_INT_STATUS, ONE_BYTE, &int_mask);
        if (ret) {
            bus->ops.releaseHost(bus);
            return -FAILURE;
        }

        if (int_mask & HISDIO_FUNC1_INT_MASK) {
            /* sdio int came */
            break;
        }

        if (oal_time_after(OAL_TIME_JIFFY, timeout_jiffies) > 0) {
            bus->ops.releaseHost(bus);
            return -FAILURE;
        }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        cpu_relax();
#endif
    }

    bus_buf = &int_mask;
    ret = bus->ops.writeData(bus, HISDIO_REG_FUNC1_INT_STATUS, ONE_BYTE, bus_buf);
    if (ret < 0) {
        bus->ops.releaseHost(bus);
        return -FAILURE;
    }

    timeout_jiffies = OAL_TIME_JIFFY + OAL_MSECS_TO_JIFFIES(timeout);
    for (;;) {
        ret = bus->ops.readData(bus, HISDIO_REG_FUNC1_INT_STATUS, ONE_BYTE, &int_mask);
        if (ret) {
            bus->ops.releaseHost(bus);
            return -FAILURE;
        }

        if ((int_mask & HISDIO_FUNC1_INT_MASK) == 0) {
            /* sdio int came */
            break;
        }

        if (oal_time_after((hi_u32)OAL_TIME_JIFFY, timeout_jiffies) > 0) {
            bus->ops.releaseHost(bus);
            return -FAILURE;
        }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        cpu_relax();
#endif
    }
    bus_buf = (hi_u8 *)&xfer_count;
    ret = bus->ops.readData(bus, HISDIO_REG_FUNC1_XFER_COUNT, FOUR_BYTE, bus_buf);
    if (ret < 0) {
        bus->ops.releaseHost(bus);
        return -FAILURE;
    }

    if (xfer_count < len) {
        len = xfer_count;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    ver_info = oal_kzalloc((xfer_count + 1), OAL_GFP_KERNEL);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    ver_info = (hi_u8 *)memalign(32, SKB_DATA_ALIGN(xfer_count + 1)); /* 32 */
#endif
    if (ver_info == NULL) {
        bus->ops.releaseHost(bus);
        return -ENOMEM;
    }

    /* 安全编程规则6.6例外(3) 从堆中分配内存后，赋予初值 */
    memset_s(ver_info, xfer_count + 1, 0, xfer_count);
    ret = bus->ops.bulkRead(bus, HISDIO_REG_FUNC1_FIFO, xfer_count, ver_info, 0);
    if (ret >= 0) {
        for (i = 0; i < len; i++) {
            buf[i] = ver_info[i];
        }
    }
    oal_free(ver_info);

    bus->ops.releaseHost(bus);

    return xfer_count;
}
