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

#include "hdf_wlan_sdio.h"
#include "hdf_wlan_config.h"
#ifdef __KERNEL__
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
#include <linux/mmc/sdio.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/host.h>
#include <linux/pm_runtime.h>
#include <linux/random.h>
#include <linux/completion.h>
#else
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/delay.h>
#endif
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_wlan_chipdriver_manager.h"


#ifdef __KERNEL__
#define REG_WRITE(ADDR, VALUE)                                                                     \
    do {                                                                                           \
        void __iomem *reg = ioremap(ADDR, sizeof(uint32_t));                                       \
        if (reg == NULL) {                                                                         \
            HDF_LOGE("%s:ioremap failed!addr=0x%08x", __func__, ADDR);                             \
            break;                                                                                 \
        }                                                                                          \
        HDF_LOGW("%s: Change register[0x%08x] %04x to %04x", __func__, ADDR, readl(reg), (VALUE)); \
        writel(VALUE, reg);                                                                        \
        iounmap(reg);                                                                              \
    } while (0)

#define REG_SET_BITS(ADDR, VALUE)                                                                               \
    do {                                                                                                        \
        void __iomem *reg = ioremap(ADDR, sizeof(uint32_t));                                                    \
        if (reg == NULL) {                                                                                      \
            HDF_LOGE("%s: ioremap failed!addr=0x%08x", __func__, ADDR);                                         \
            break;                                                                                              \
        }                                                                                                       \
        HDF_LOGW("%s: Change register[0x%08x] %04x to %04x", __func__, ADDR, readl(reg), readl(reg) | (VALUE)); \
        writel(readl(reg) | (VALUE), reg);                                                                      \
        iounmap(reg);                                                                                           \
    } while (0)
#else
#define REG_WRITE(ADDR, VALUE)                                                                     \
    do {                                                                                           \
        int reg = IO_DEVICE_ADDR(ADDR);                                                            \
        HDF_LOGW("%s: Change register[0x%08x] %04x to %04x", __func__, ADDR, readl(reg), (VALUE)); \
        writel(VALUE, reg);                                                                        \
    } while (0)

#define REG_SET_BITS(ADDR, VALUE)                                                                               \
    do {                                                                                                        \
        int reg = IO_DEVICE_ADDR(ADDR);                                                                         \
        HDF_LOGW("%s: Change register[0x%08x] %04x to %04x", __func__, ADDR, readl(reg), readl(reg) | (VALUE)); \
        writel(readl(reg) | (VALUE), reg);                                                                      \
    } while (0)
#endif


static int32_t ConfigHi3516DV300SDIO(uint8_t busId)
{
    if (busId == 2) {
        HDF_LOGE("%s: Config Hi3516DV300 SDIO bus %d", __func__, busId);
        const uint32_t PMC_REG_ADDR_REG0 = 0x12090000;
        const uint32_t PIN_REG_ADDR_CLK = 0x112F0008;
        const uint32_t PIN_REG_ADDR_CMD = 0x112F000C;
        const uint32_t PIN_REG_ADDR_DATA0 = 0x112F0010;
        const uint32_t PIN_REG_ADDR_DATA1 = 0x112F0014;
        const uint32_t PIN_REG_ADDR_DATA2 = 0x112F0018;
        const uint32_t PIN_REG_ADDR_DATA3 = 0x112F001C;

        REG_SET_BITS(PMC_REG_ADDR_REG0, 0x0080);
        REG_WRITE(PIN_REG_ADDR_CLK, 0x601);
        REG_WRITE(PIN_REG_ADDR_CMD, 0x501);
        REG_WRITE(PIN_REG_ADDR_DATA0, 0x501);
        REG_WRITE(PIN_REG_ADDR_DATA1, 0x501);
        REG_WRITE(PIN_REG_ADDR_DATA2, 0x501);
        REG_WRITE(PIN_REG_ADDR_DATA3, 0x501);
        return HDF_SUCCESS;
    }

    HDF_LOGE("%s: SDIO bus ID %d not supportted!", __func__, busId);
    return HDF_FAILURE;
}

static int32_t ConfigHi3518EV300SDIO(uint8_t busId)
{
    if (busId == 1) {
        HDF_LOGE("%s: Config Hi3518EV300 SDIO bus %d", __func__, busId);
        const uint32_t PIN_REG_ADDR_CLK = 0x112c0048;
        const uint32_t PIN_REG_ADDR_CMD = 0x112C004C;
        const uint32_t PIN_REG_ADDR_DATA0 = 0x112C0064;
        const uint32_t PIN_REG_ADDR_DATA1 = 0x112c0060;
        const uint32_t PIN_REG_ADDR_DATA2 = 0x112c005c;
        const uint32_t PIN_REG_ADDR_DATA3 = 0x112c0058;

        REG_WRITE(PIN_REG_ADDR_CLK, 0x1a04);
        REG_WRITE(PIN_REG_ADDR_CMD, 0x1004);
        REG_WRITE(PIN_REG_ADDR_DATA0, 0x1004);
        REG_WRITE(PIN_REG_ADDR_DATA1, 0x1004);
        REG_WRITE(PIN_REG_ADDR_DATA2, 0x1004);
        REG_WRITE(PIN_REG_ADDR_DATA3, 0x1004);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: SDIO bus ID %d not supportted!", __func__, busId);
    return HDF_FAILURE;
}

int32_t HdfWlanConfigSDIO(uint8_t busId)
{
    struct HdfConfigWlanRoot *config = HdfWlanGetModuleConfigRoot();
    if (config == NULL || config->wlanConfig.hostChipName == NULL) {
        HDF_LOGE("%s: No config or chip name is NULL!", __func__);
        return HDF_FAILURE;
    }
    if (strcmp("hi3516dv300", config->wlanConfig.hostChipName) == 0) {
        return ConfigHi3516DV300SDIO(busId);
    }
    if (strcmp("hi3518ev300", config->wlanConfig.hostChipName) == 0) {
        return ConfigHi3518EV300SDIO(busId);
    }
    HDF_LOGE("%s: platform chip not supported!", __func__);
    return HDF_FAILURE;
}
