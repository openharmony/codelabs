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
#ifndef _OAL_CHR_H
#define _OAL_CHR_H
#include "oal_util.h"
#include "exception_rst.h"
#include "oal_err_wifi.h"
#include "hi_wifi_api.h"

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

hi_u32 oal_register_ioctl(hi_void);

#else
hi_u32 oal_register_ioctl(hi_wifi_driver_event_cb event_cb);
#endif
hi_void oal_unregister_ioctl(hi_void);
hi_u32 hisi_sched_event(hi_wifi_driver_event event);
#endif
