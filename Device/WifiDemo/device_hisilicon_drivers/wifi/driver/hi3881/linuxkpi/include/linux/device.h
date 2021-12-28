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

#ifndef __LIUNX_DEVICE_H__
#define __LIUNX_DEVICE_H__

#include "los_base.h"
#include "los_mux.h"
#include "linux/list.h"
#include "linux/pm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct bus_type;
struct device_driver;
struct dev_pm_op;

/* device define */
/* *
 * @ingroup los_drivers
 * Define the structure of the parameters used for device creation.
 */
struct device {
    const CHAR *name;
    LOS_DL_LIST children_list;
    LOS_DL_LIST parent_node;
    LOS_DL_LIST driver_node;
    LOS_DL_LIST bus_node;
    struct device *parent;
    VOID *p;                        /**< private data */
    LosMux mutex;
    struct bus_type *bus;           /**< type of bus device is on */
    struct device_driver *driver;   /**< which driver has allocated this device */
    VOID *driver_data;
    VOID *platform_data;            /**< Platform specific data, device core doesn't touch it */
    struct dev_pm_op   *ops;
    VOID (*release)(struct device *dev);
    struct dev_pm_info power;
};

/* driver define */
/* *
 * @ingroup los_drivers
 * Define the structure of the parameters used for driver creation.
 */
struct device_driver {
    const CHAR *name;
    struct bus_type *bus;
    INT32 (*probe)(struct device *dev);
    INT32 (*remove)(struct device *dev);
    VOID (*shutdown)(struct device *dev);
    INT32 (*suspend)(struct device *dev, INT32 state);
    INT32 (*resume)(struct device *dev);
    const struct dev_pm_op *pm;
    VOID *p;
    LOS_DL_LIST device_list;
    LOS_DL_LIST bus_node;
    LosMux mutex;
};

struct dev_pm_op {
    INT32 (*suspend)(struct device *ptr);
    INT32 (*resume)(struct device *ptr);
    INT32 (*prepare)(struct device *dev);
    INT32 (*complete)(struct device *dev);
};

/* *
 * @ingroup los_drivers
 * Define the structure of the parameters used for bus creation.
 */
struct bus_type {
    const CHAR *name;
    LOS_DL_LIST bus_node;
    INT32 (*match)(struct device *dev, struct device_driver *drv);
    INT32 (*probe)(struct device *dev);
    INT32 (*remove)(struct device *dev);
    VOID (*shutdown)(struct device *dev);
    const struct dev_pm_op *pm;
    LOS_DL_LIST device_list;
    LOS_DL_LIST driver_list;
    VOID *p;
    LosMux mutex;
};

/* errno */
/* *
 * @ingroup los_task
 * Task error code: Invalid input.
 *
 * Value: 0x02004110
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_INPUT_INVALID                        LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x10)

/* *
 * @ingroup los_task
 * Task error code: Bus register twice.
 *
 * Value: 0x02004111
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_BUS_REGISTERED                       LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x11)

/* *
 * @ingroup los_task
 * Task error code: Invalid input.
 *
 * Value: 0x02004112
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_BUS_INVALID                          LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x12)

/* *
 * @ingroup los_task
 * Task error code: Invalid input.
 *
 * Value: 0x02004113
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_DRIVER_BUS_INPUT_INVALID                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x13)

/* *
 * @ingroup los_task
 * Task error code: Do match function failed.
 *
 * Value: 0x02004114
 *
 * Solution: Check match function.
 */
#define LOS_ERRNO_DRIVER_BUS_MATCH_FAIL                       LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x14)

/* *
 * @ingroup los_task
 * Task error code:  Do probe function failed.
 *
 * Value: 0x02004115
 *
 * Solution: Check probe function.
 */
#define LOS_ERRNO_DRIVER_BUS_PROBE_FAIL                       LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x15)

/* *
 * @ingroup los_task
 * Task error code: Create mux failed.
 *
 * Value: 0x02004116
 *
 * Solution: Check the system state.
 */
#define LOS_ERRNO_DRIVER_BUS_MUX_FAIL                         LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x16)

/* *
 * @ingroup los_task
 * Task error code: Do match function failed.
 *
 * Value: 0x02004120
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_DRIVER_DRIVER_MATCH_FAIL                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x20)

/* *
 * @ingroup los_task
 * Task error code: Do probe function failed.
 *
 * Value: 0x02004121
 *
 * Solution:  This error code is not in use temporarily.
 */
#define LOS_ERRNO_DRIVER_DRIVER_PROBE_FAIL                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x21)

/* *
 * @ingroup los_task
 * Task error code: driver register twice.
 *
 * Value: 0x02004122
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DRIVER_REGISTERED                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x22)

/* *
 * @ingroup los_task
 * Task error code: Invalid input.
 *
 * Value: 0x02004123
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DRIVER_NOTFOUND                      LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x23)

/* *
 * @ingroup los_task
 * Task error code: Device already attach to driver.
 *
 * Value: 0x02004130
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DEVICE_BOUNDED                       LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x30)

/* *
 * @ingroup los_task
 * Task error code: Invalid input.
 *
 * Value: 0x02004131
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DEVICE_INITIALFAIL                   LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x31)

/* *
 * @ingroup los_task
 * Task error code: Device register twice.
 *
 * Value: 0x02004132
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DEVICE_REGISTERED                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x32)

/* *
 * @ingroup los_task
 * Task error code: Device busy.
 *
 * Value: 0x02004133
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DEVICE_BUSY                          LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x33)

/* bus api */
/* *
 * @ingroup los_drivers
 * @brief register a new bus system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to register a bus system that can manage its drivers and devices.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The param match and probe used for attaching the device and the driver.</li>
 * <li>The same bus node can not be registered twice.</li>
 * <li>Do not use device_list and driver_list by yourself.</li>
 * </ul>
 *
 * @param  bus      [IN]A pointer to bus_type.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID    Invalid input.bus or bus->name may be NULL.
 * @retval #LOS_ERRNO_DRIVER_BUS_REGISTERED   Invalid input.Node can not be registered twice.
 * @retval #LOS_ERRNO_DRIVER_BUS_MUX_FAIL     Mux creat failed.
 * @retval #LOS_OK                            The bus register success.
 * @par Dependency:
 * <ul><li>device.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 bus_register(struct bus_type *bus);

/* *
 * @ingroup los_drivers
 * @brief unregister a bus system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to unregister a bus system.</li>
 * </ul>
 *
 * @param  bus      [IN]A pointer to bus_type.bus_type bus can not be NULL.
 *
 * @par Dependency:
 * <ul><li>device.h: the header file that contains the API declaration.</li></ul>
 */
extern void bus_unregister(struct bus_type *bus);
extern UINT32 bus_rescan_devices(struct bus_type *bus);
extern UINT32 bus_add_driver(struct device_driver *drv);
extern struct bus_type *bus_get(struct bus_type *bus);
extern UINT32 bus_add_device(struct device *dev);
extern UINT32 bus_del_device(struct device *dev);
extern UINT32 bus_probe_device(struct device *dev);
extern UINT32 bus_remove_device(struct device *dev);
extern UINT32 bus_for_each_drv(struct bus_type *bus, struct device_driver *start, VOID *data,
                               INT32 (*fn)(struct device_driver *, VOID *));
extern UINT32 bus_for_each_dev(struct bus_type *bus, struct device *start, VOID *data,
                               INT32 (*fn)(struct device *, VOID *));
extern UINT32 bus_remove_driver(struct device_driver *drv);

#define bus_lock(bus)   (VOID)LOS_MuxLock(&(bus)->mutex, LOS_WAIT_FOREVER);
#define bus_unlock(bus) (VOID)LOS_MuxUnlock(&(bus)->mutex);

/* driver api */
/* *
 * @ingroup los_drivers
 * @brief register a new driver to the bus system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to register a driver to the bus system.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The param probe function is applied after attach the device to the driver.</li>
 * <li>The param remove function is used when device or driver is deleted.</li>
 * </ul>
 *
 * @param  drv      [IN]A pointer to device_driver.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID        Invalid input.drv/drv->name/drv->bus can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DRIVER_REGISTERED    Invalid input.drv node register twice.
 * @retval #LOS_ERRNO_DRIVER_BUS_MUX_FAIL         Mux creat failed.
 * @retval #LOS_ERRNO_DRIVER_BUS_INVALID          drv->bus is not in system.
 * @retval #LOS_ERRNO_DRIVER_BUS_MATCH_FAIL       Do match failed.
 * @retval #LOS_ERRNO_DRIVER_BUS_PROBE_FAIL       Do probe failed.
 * @retval #LOS_OK        The driver register success.
 * @par Dependency:
 * <ul><li>device.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 driver_register(struct device_driver *drv);

/* *
 * @ingroup los_drivers
 * @brief unregister a  driver from the bus system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to unregister a driver from the bus system.</li>
 * </ul>
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID        Invalid input.drv and drv.driver->name can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DRIVER_NOTFOUND      Driver not found.
 * @retval #LOS_ERRNO_DRIVER_BUS_INVALID          Bus is not in system.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_BUSY          Device busy.
 * @retval #LOS_OK                                The driver unregister success.
 * @param  drv      [IN]A pointer to device_driver.drv/drv->name/drv->bus can not be NULL.
 *
 * @par Dependency:
 * <ul><li>device.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 driver_unregister(struct device_driver *drv);
extern UINT32 driver_for_each_device(struct device_driver *drv, struct device *start, VOID *data,
                                     INT32 (*fn)(struct device *, VOID *));
extern struct device_driver *driver_find(const CHAR *name, struct bus_type *bus);
extern UINT32 driver_attach(struct device_driver *drv);
extern UINT32 driver_detach(struct device_driver *drv);
extern UINT32 driver_probe_device(struct device_driver *drv, struct device *dev);
extern UINT32 driver_remove_device(struct device_driver *drv, struct device *dev);

#define driver_lock(_drv)   (VOID)LOS_MuxLock(&(_drv)->mutex, LOS_WAIT_FOREVER)
#define driver_unlock(_drv) (VOID)LOS_MuxUnlock(&(_drv)->mutex)


/* device api */
/* *
 * @ingroup los_drivers
 * @brief unregister a device from the bus system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to unregister a device from the bus system.</li>
 * </ul>
 * @param  dev      [IN]A pointer to device.dev/dev->name can not be NULL.
 *
 * @par Dependency:
 * <ul><li>device.h: the header file that contains the API declaration.</li></ul>
 */
extern void device_unregister(struct device *dev);

/* *
 * @ingroup los_drivers
 * @brief register a new device to the bus system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to register a device to the bus system.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The same dev node can not be registered twice.</li>
 * </ul>
 *
 * @param  dev      [IN]A pointer to device.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID        Invalid input.dev/dev->name/dev->bus can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_REGISTERED    Device register twice.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_INITIALFAIL   Mux create failed.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_BOUNDED       Do attach failed.device has bounded.
 * @retval #LOS_ERRNO_DRIVER_BUS_MATCH_FAIL       Do match failed.
 * @retval #LOS_ERRNO_DRIVER_BUS_PROBE_FAIL       Do probe failed.
 * @retval #LOS_OK        The device register success.
 * @par Dependency:
 * <ul><li>device.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 device_register(struct device *dev);
extern UINT32 device_initialize(struct device *dev);
extern BOOL device_is_registered(struct device *dev);
extern UINT32 device_remove(struct device *dev);
extern UINT32 linux_device_attach(struct device *dev);
extern UINT32 device_add(struct device *dev);
extern VOID device_del(struct device *dev);

STATIC INLINE VOID *dev_get_platdata(const struct device *dev)
{
    if (dev == NULL) {
        PRINT_WARN("dev_get_platdata :the input dev is NULL!\n");
        return NULL;
    }
    return dev->platform_data;
}

STATIC INLINE VOID dev_set_platdata(struct device *dev, VOID *data)
{
    if (dev == NULL) {
        PRINT_WARN("dev_set_platdata :the input dev is NULL!\n");
        return;
    }
    dev->platform_data = data;
}

STATIC INLINE VOID *dev_get_drvdata(const struct device *dev)
{
    if (dev == NULL) {
        PRINT_WARN("dev_get_drvdata :the input dev is NULL!\n");
        return NULL;
    }
    return dev->driver_data;
}

STATIC INLINE VOID dev_set_drvdata(struct device *dev, VOID *data)
{
    if (dev == NULL) {
        PRINT_WARN("dev_set_drvdata :the input dev is NULL!\n");
        return;
    }
    dev->driver_data = data;
}

#define device_lock(dev)   (VOID)LOS_MuxLock(&(dev)->mutex, LOS_WAIT_FOREVER)
#define device_unlock(dev) (VOID)LOS_MuxUnlock(&(dev)->mutex)

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
