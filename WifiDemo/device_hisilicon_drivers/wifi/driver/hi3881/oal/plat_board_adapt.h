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

#ifndef __BOARD_H__
#define __BOARD_H__

/* ****************************************************************************
  1 Include other Head file
**************************************************************************** */
#include "hi_types.h"

/* ****************************************************************************
  2 Define macro
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define IO_MUX_REG_BASE (g_io_mux_base)
#endif
#define REG_MUXCTRL_SDIO_CLK_MAP            (IO_MUX_REG_BASE + 0x048)
#define REG_MUXCTRL_SDIO_CDATA1_MAP         (IO_MUX_REG_BASE + 0x060)
#define REG_MUXCTRL_SDIO_CDATA0_MAP         (IO_MUX_REG_BASE + 0x064)
#define REG_MUXCTRL_SDIO_CDATA3_MAP         (IO_MUX_REG_BASE + 0x058)
#define REG_MUXCTRL_SDIO_CCMD_MAP           (IO_MUX_REG_BASE + 0x04c)
#define REG_MUXCTRL_SDIO_CDATA2_MAP         (IO_MUX_REG_BASE + 0x05c)

#define REG_MUXCTRL_WIFI_DATA_INTR_GPIO_MAP (IO_MUX_REG_BASE + 0x68)
#define REG_MUXCTRL_HOST_WAK_DEV_GPIO_MAP   (IO_MUX_REG_BASE + 0x44)
#define REG_MUXCTRL_DEV_WAK_HOST_GPIO_MAP   (IO_MUX_REG_BASE + 0x50)
#define REG_MUXCTRL_WLAN_RST_GPIO_MAP       (IO_MUX_REG_BASE + 0x40)
#define REG_MUXCTRL_WLAN_PWR_GPIO_MAP       (IO_MUX_REG_BASE + 0x74)
#define REG_GIPO_DIR                        (IO_MUX_REG_BASE + 0x400)
#define REG_GIPO_DATA                       (IO_MUX_REG_BASE + 0x3FC)
#define REG_GPIO_6_4_BIT                    0x00000010
#define REG_GPIO_8_6_BIT                    0x00000040

#define REG_PAD_CTRL_BASE                   (g_io_mux_base + 0x800)

#define REG_CTRL_SDIO_CLK                   (REG_PAD_CTRL_BASE + 0x40)
#define REG_CTRL_SDIO_CMD                   (REG_PAD_CTRL_BASE + 0x58)
#define REG_CTRL_SDIO_DATA0                 (REG_PAD_CTRL_BASE + 0x54)
#define REG_CTRL_SDIO_DATA1                 (REG_PAD_CTRL_BASE + 0x48)
#define REG_CTRL_SDIO_DATA2                 (REG_PAD_CTRL_BASE + 0x4C)
#define REG_CTRL_SDIO_DATA3                 (REG_PAD_CTRL_BASE + 0x44)

#define SDIO_CLK_DS_1V8                     0x40
#define SDIO_CMD_DS_1V8                     0x120
#define SDIO_DATA0_DS_1V8                   0x120
#define SDIO_DATA1_DS_1V8                   0x120
#define SDIO_DATA2_DS_1V8                   0x120
#define SDIO_DATA3_DS_1V8                   0x120

#define WIFI_SDIO_INDEX                     2

#define PLAT_EXCEPTION_DEV_PANIC_LR_ADDR    0x5CBC
#define PLAT_EXCEPTION_DEV_PANIC_PC_LR_LEN  0x8

#define DATA_INT_GPIO_GROUP                 6
#define DATA_INT_GPIO_GROUP_PIN             5

#define WLAN_PM_H2D_GPIO_GROUP              6
#define WLAN_PM_H2D_GPIO_GROUP_PIN          7
#ifdef _PRE_WLAN_FEATURE_MCU
#define WIFI_WAK_FLAG_GPIO_GROUP            7
#define WIFI_WAK_FLAG_GPIO_PIN              0

#define DEV_WAK_HOST_GPIO_GROUP             7
#define DEV_WAK_HOST_GPIO_PIN               1
#else
#define WLAN_EN_GPIO_GROUP                  7
#define WLAN_EN_GPIO_PIN                    0

#define POWER_EN_GPIO_GROUP                 7
#define POWER_EN_GPIO_PIN                   1

#define DEV_WAK_HOST_GPIO_GROUP             6
#define DEV_WAK_HOST_GPIO_PIN               2
#endif

#define GPIO6_5_MUXCTRL_REG53_OFFSET        0x0D4
#define GPIO6_7_MUXCTRL_REG55_OFFSET        0x0DC
#define GPIO7_0_MUXCTRL_REG56_OFFSET        0x0E0
#define GPIO7_1_MUXCTRL_REG57_OFFSET        0x0E4

#define GPIO6_5_MUXCTRL_REG53_VALUE         0
#define GPIO6_7_MUXCTRL_REG55_VALUE         0
#define GPIO7_0_MUXCTRL_REG56_VALUE         0
#define GPIO7_1_MUXCTRL_REG57_VALUE         0

#define GPIO_LOW_LEVEl                      0
#define GPIO_HIGH_LEVEL                     1

#define GPIO_IS_EDGE                        0
#define GPIO_IS_LEVEL                       1

#define GPIO_IBE_SINGLE                     0
#define GPIO_IBE_DOUBLE                     1


#define GPIO_IEV_NEGEDGE                    0
#define GPIO_IEV_POSEDGE                    1

#define GPIO_IEV_LOW_LEVEL                  0
#define GPIO_IEV_HIGH_LEVEL                 1

#define GPIO_IE_DISABLE                     0
#define GPIO_IE_ENABLE                      1

#define NUM_HAL_INTERRUPT_GPIO              31
#define WALN_IRQ                            NUM_HAL_INTERRUPT_GPIO
#define INVALID_IRQ                         (-1)

#define SDIO1_POLAR_VAL                     0x2

/*****************************************************************************
  3 STRUCT DEFINE
*****************************************************************************/
/*****************************************************************************
  4 EXTERN VARIABLE
**************************************************************************** */
/* ****************************************************************************
  5 EXTERN FUNCTION
**************************************************************************** */
hi_s32 hi110x_board_init(hi_void);
hi_void hi110x_board_exit(hi_void);
hi_void board_power_on(hi_void);
hi_void board_power_off(hi_void);
int board_get_bwkup_gpio_val(hi_void);
int board_get_wlan_wkup_gpio_val(hi_void);
int board_get_wlan_host_to_dev_gpio_val(hi_void);
hi_void board_set_host_to_dev_gpio_val_low(hi_void);
hi_void board_set_host_to_dev_gpio_val_high(hi_void);
hi_void set_host_to_dev_gpio_val(int val);
hi_void board_set_wlan_h2d_pm_state(unsigned int ul_value);
hi_u32 board_get_and_clear_wlan_sdio_gpio_intr_state(hi_void);
hi_u32 board_get_and_clear_wlan_wkup_gpio_intr_state(hi_void);
hi_void wlan_rst(hi_void);
hi_void sdio_card_detect_change(hi_s32 val);
hi_void board_gpio_pin_power_off_init(void);
hi_void board_gpio_set_power_off_value(hi_u8 level);

#endif
