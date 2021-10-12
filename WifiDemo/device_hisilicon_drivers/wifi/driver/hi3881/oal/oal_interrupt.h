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

#ifndef __OAL_INTERRUPT_H__
#define __OAL_INTERRUPT_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include <linux/interrupt.h>
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <asm/hal_platform_ints.h>
#include "gpio_if.h"
#endif
#include "hdf_wifi_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
typedef void (*irq_func)(unsigned int irq, void *data);

typedef struct {
    unsigned int groupnumber;
    unsigned int bitnumber;

    unsigned char value;
#define GPIO_VALUE_HIGH 1
#define GPIO_VALUE_LOW 0
    unsigned char direction;
#define GPIO_DIR_IN 0
#define GPIO_DIR_OUT 1

    unsigned char irq_status;
    unsigned char irq_enable;
#define GPIO_IRQ_ENABLE         1
#define GPIO_IRQ_DISABLE        0
    irq_func  irq_handler;
    unsigned int irq_type;
#define IRQ_TYPE_NONE           0x00000000
#define IRQ_TYPE_EDGE_RISING    0x00000001
#define IRQ_TYPE_EDGE_FALLING   0x00000002
#define IRQ_TYPE_EDGE_BOTH      (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define IRQ_TYPE_LEVEL_HIGH     0x00000004
#define IRQ_TYPE_LEVEL_LOW      0x00000008
#define IRQ_TYPE_LEVEL_MASK     (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH)
    void *data;
} gpio_groupbit_info;

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
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifndef IRQF_NO_SUSPEND
#define IRQF_NO_SUSPEND 0x0000
#endif

#ifndef IRQF_DISABLED
#define IRQF_DISABLED 0x0000
#endif

static uint8_t wifi_get_gpio_bit_num(void);

#define GPIO_TO_IRQ(group, bit) ((group) * (wifi_get_gpio_bit_num()) + (bit) + (OS_USER_HWI_MAX))
#define IRQ_TO_GPIO_GROUP(irq)  (((irq) - (OS_USER_HWI_MAX)) / (wifi_get_gpio_bit_num()))
#define IRQ_TO_GPIO_BIT(irq)    (((irq) - (OS_USER_HWI_MAX)) % (wifi_get_gpio_bit_num()))
#endif

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
static uint8_t wifi_get_gpio_bit_num(void)
{
    const struct HdfConfigWifiRoot *rootConfig = HdfWifiGetModuleConfigRoot();

    return rootConfig->wifiConfig.board.gpioArgs[1];
}

static inline hi_s32 oal_request_irq(hi_u32 irq, irq_handler_t handler, unsigned long flags, const hi_char *name,
    hi_void *dev)
{
    uint16_t gpio;
    uint8_t gpio_bit_num;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return request_irq(irq, handler, flags, name, dev);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (irq <= OS_USER_HWI_MAX) {
        return request_irq(irq, handler, flags, name, dev);
    } else {
        gpio_groupbit_info st_gpio_info = {0};
        st_gpio_info.groupnumber     = IRQ_TO_GPIO_GROUP(irq);
        st_gpio_info.bitnumber       = IRQ_TO_GPIO_BIT(irq);
        st_gpio_info.irq_handler     = (irq_func)handler;
        st_gpio_info.irq_type        = 0;
        st_gpio_info.data            = dev;

        gpio_bit_num = wifi_get_gpio_bit_num();
        gpio = st_gpio_info.groupnumber * gpio_bit_num + st_gpio_info.bitnumber;
        return GpioSetIrq(gpio, st_gpio_info.irq_type, (GpioIrqFunc)st_gpio_info.irq_handler, st_gpio_info.data);
    }
#endif
}

static inline hi_void oal_free_irq(hi_u32 irq, hi_void *dev)
{
    free_irq(irq, dev);
}

static inline hi_void oal_enable_irq(hi_u32 irq)
{
    enable_irq(irq);
}

static inline hi_void oal_disable_irq(hi_u32 irq)
{
    disable_irq(irq);
}

static inline hi_void oal_disable_irq_nosync(hi_u32 irq)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    disable_irq_nosync(irq);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    disable_irq(irq);
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_completion.h */
