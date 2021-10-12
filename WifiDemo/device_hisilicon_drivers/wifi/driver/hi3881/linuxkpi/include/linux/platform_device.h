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

#ifndef __LINUX_PLATFORM_DEVICE_H__
#define __LINUX_PLATFORM_DEVICE_H__

#include "linux/device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define IORESOURCE_IO  0x00000100 /* PCI/ISA I/O ports */
#define IORESOURCE_MEM 0x00000200
#define IORESOURCE_REG 0x00000300 /* Register offsets */
#define IORESOURCE_IRQ 0x00000400
#define IORESOURCE_DMA 0x00000800
#define IORESOURCE_BUS 0x00001000

typedef uintptr_t resource_size_t;

struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;
    unsigned long desc;
    struct resource *parent, *sibling, *child;
};

/* *
 * @ingroup los_drivers
 * Define the structure of the parameters used for platform device creation.
 */
struct platform_device {
    const char *name;
    struct device dev;
    int id;
    UINT32 num_resources;
    struct resource *resource;
};

struct pm_message_t {
    int event;
};

/* *
 * @ingroup los_drivers
 * Define the structure of the parameters used for platform driver creation.
 */
struct platform_driver {
    int (*probe)(struct platform_device *);
    int (*remove)(struct platform_device *);
    void (*shutdown)(struct platform_device *);
    int (*suspend)(struct platform_device *);
    int (*resume)(struct platform_device *);
    struct device_driver driver;
};

/* *
 * @ingroup los_drivers
 * @brief register a platform_driver to platform bus.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to register a platform_driver to platform bus.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The platform bus system is a soft bus that is used to deal the host device and driver.</li>
 * <li>The same bus node can not be registered twice.</li>
 * </ul>
 *
 * @param  drv      [IN]A point to platform_driver.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID        Invalid input.drv and drv.driver->name can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DRIVER_REGISTERED    Driver register twice.
 * @retval #LOS_ERRNO_DRIVER_BUS_MUX_FAIL         Mux create failed.
 * @retval #LOS_ERRNO_DRIVER_BUS_INVALID          Bus is not in system.
 * @retval #LOS_ERRNO_DRIVER_BUS_MATCH_FAIL       Do match failed.
 * @retval #LOS_ERRNO_DRIVER_BUS_PROBE_FAIL       Do probe failed.
 * @retval #LOS_OK        The platform_driver register success.
 * @par Dependency:
 * <ul><li>platform_device.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 platform_driver_register(struct platform_driver *drv);

/* *
 * @ingroup los_drivers
 * @brief unregister a  platform_driver from the platform bus.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to unregister a platform_driver from the platform bus.</li>
 * </ul>
 *
 * @param  drv      [IN]A point to platform_driver. drv/drv->name/drv->bus can not be NULL.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID        Invalid input.drv and drv.driver->name can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DRIVER_NOTFOUND      Driver not found.
 * @retval #LOS_ERRNO_DRIVER_BUS_INVALID          Bus is not in system.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_BUSY          Device busy.
 * @retval #LOS_OK                                The platform_driver unregister success.
 * @par Dependency:
 * <ul><li>platform_device.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 platform_driver_unregister(struct platform_driver *drv);

/* *
 * @ingroup los_drivers
 * @brief register a platform_device to platform bus.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to register a platform_device to platform bus.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The same platform_device node can not be registered twice.</li>
 * </ul>
 *
 * @param  dev      [IN] A point to platform_device. num_resources can not bigger than the count of resource_array.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID       Invalid input.pdev and pdev->name can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_REGISTERED    Device register twice.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_INITIALFAIL   Mux create failed.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_BOUNDED       Do attach failed.device has bounded.
 * @retval #LOS_ERRNO_DRIVER_BUS_MATCH_FAIL       Do match failed.
 * @retval #LOS_ERRNO_DRIVER_BUS_PROBE_FAIL       Do probe failed.
 * @retval #LOS_OK        The platform_device register success.
 * @par Dependency:
 * <ul><li>platform_device.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 platform_device_register(struct platform_device *dev);

/* *
 * @ingroup los_drivers
 * @brief unregister a  platform_device from the platform bus.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to unregister a platform_device from the platform bus.</li>
 * </ul>
 *
 * @param  dev      [IN]A point to platform_device.If dev is NULL,this function will do nothing.
 *
 * @par Dependency:
 * <ul><li>platform_device.h: the header file that contains the API declaration.</li></ul>
 */
extern VOID platform_device_unregister(struct platform_device *dev);

extern UINT32 platform_device_add(struct platform_device *dev);

extern struct resource *platform_get_resource(struct platform_device *, unsigned int, unsigned int);
extern struct resource *platform_get_resource_byname(struct platform_device *, unsigned int, const char *);
extern long platform_get_irq(struct platform_device *, unsigned int);

extern VOID *platform_ioremap_resource(struct resource *res);

#define to_platform_driver(drv) (container_of((drv), struct platform_driver, driver))
#define to_platform_device(x) container_of((x), struct platform_device, dev)

STATIC INLINE resource_size_t resource_size(const struct resource *res)
{
    return res->end - res->start + 1;
}

STATIC INLINE VOID *platform_get_drvdata(const struct platform_device *dev)
{
    if (dev == NULL) {
        PRINT_WARN("platform_get_drvdata :the input dev is NULL!\n");
        return NULL;
    }
    return dev_get_drvdata(&dev->dev);
}

STATIC INLINE VOID platform_set_drvdata(struct platform_device *dev, VOID *data)
{
    if (dev == NULL) {
        PRINT_WARN("platform_set_drvdata :the input dev is NULL!\n");
        return;
    }
    dev_set_drvdata(&dev->dev, data);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
