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

#ifndef __OAL_CHANNEL_HOST_IF_H__
#define __OAL_CHANNEL_HOST_IF_H__

/* ****************************************************************************
  1 Include other Head file
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "oal_sdio_host_if.h"
#else
#include "plat_sdio.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define oal_get_bus_default_handler()     oal_get_sdio_default_handler()

#define oal_bus_wake_lock(bus)     oal_sdio_wake_lock(bus)
#define oal_bus_wake_unlock(bus)     oal_sdio_wake_unlock(bus)

#define oal_bus_message_register(bus, msg, cb, data)  oal_sdio_message_register(bus, msg, cb, data)
#define oal_bus_message_unregister(bus, msg)    oal_sdio_message_unregister(bus, msg)

#define oal_bus_patch_readsb(buf, len, ms_timeout)  sdio_patch_readsb(buf, len, ms_timeout)
#define oal_bus_patch_writesb(buf, len)  sdio_patch_writesb(buf, len)

#define oal_bus_claim_host(bus)     oal_sdio_claim_host(bus)
#define oal_bus_release_host(bus)     oal_sdio_release_host(bus)

#define oal_bus_send_msg(bus, val)     oal_sdio_send_msg(bus, val)

#define oal_disable_bus_state(bus, mask)     oal_disable_sdio_state(bus, mask)

#define oal_bus_rx_transfer_lock(bus)     oal_sdio_rx_transfer_lock(bus)
#define oal_bus_rx_transfer_unlock(bus)     oal_sdio_rx_transfer_unlock(bus)

#define oal_bus_init_module(bus, pdata)     oal_sdio_init_module(bus, pdata)

#define oal_bus_transfer_rx_register(bus, rx_handler) oal_sdio_transfer_rx_register(bus, rx_handler)
#define oal_bus_transfer_rx_unregister(bus)     oal_sdio_transfer_rx_unregister(bus)

#define oal_bus_func_init(bus)     oal_sdio_func_init(bus)
#define oal_bus_func_remove(bus)     oal_sdio_func_remove(bus)

#define oal_bus_transfer_prepare(bus)   oal_sdio_transfer_prepare(bus)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
