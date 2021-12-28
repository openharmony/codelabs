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
#include "hcc_host.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_u32 hi_wifi_open_gpio_soft_reset_device(hi_void)
{
    hcc_open_gpio_soft_rest soft_reset = { 0 };
    soft_reset.is_open = 1;
    return hcc_send_control_msg(hcc_host_get_handler(), OAM_SET_SOFT_RST, (hi_u8 *)&soft_reset, sizeof(soft_reset));
}

hi_u32 hi_wifi_close_gpio_soft_reset_device(hi_void)
{
    hcc_open_gpio_soft_rest soft_reset = { 0 };
    soft_reset.is_open = 0;
    return hcc_send_control_msg(hcc_host_get_handler(), OAM_SET_SOFT_RST, (hi_u8 *)&soft_reset, sizeof(soft_reset));
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
