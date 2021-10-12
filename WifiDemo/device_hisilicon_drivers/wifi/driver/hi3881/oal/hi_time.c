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
#include <hi_types_base.h>
#include <los_sys.h>
#include <hi_stdlib.h>
#include <time.h>
#include "hi_config.h"

#define SEC_TO_US   1000000
#define US_TO_NSEC  1000

hi_u32 hi_get_tick(hi_void)
{
    return (hi_u32)(LOS_TickCountGet() & 0xffffffff);
}

hi_u64 hi_get_tick64(hi_void)
{
    return LOS_TickCountGet();
}

hi_u32 hi_get_milli_seconds(hi_void)
{
    return ((hi_u32)(LOS_TickCountGet() & 0xffffffff) * HI_MILLISECOND_PER_TICK);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
hi_u32 hi_get_seconds(hi_void)
{
    struct timespec tp;

    if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0) {
        return (hi_u32)(tp.tv_sec);
    } else {
        return (hi_u32)(HI_ERR_FAILURE);
    }
}

hi_u64 hi_get_us(hi_void)
{
    struct timespec tp;

    if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0) {
        return (hi_u64)(hi_u32)tp.tv_sec * SEC_TO_US + (hi_u32)tp.tv_nsec / US_TO_NSEC;
    } else {
        return (hi_u64)(HI_ERR_FAILURE);
    }
}

hi_u32 hi_get_real_time(hi_void)
{
    struct timespec tp;

    if (clock_gettime(CLOCK_REALTIME, &tp) == 0) {
        return (hi_u32)(tp.tv_sec);
    } else {
        return (hi_u32)(HI_ERR_FAILURE);
    }
}

hi_u32 hi_set_real_time(hi_u32 sec)
{
    struct timespec tp;

    memset_s((hi_void *)&tp, sizeof(struct timespec), 0x0, sizeof(struct timespec));
    tp.tv_sec = (int)sec;
    tp.tv_nsec = 0;

    if (clock_settime(CLOCK_REALTIME, &tp) == 0) {
        return (hi_u32)(HI_ERR_SUCCESS);
    } else {
        return (hi_u32)(HI_ERR_FAILURE);
    }
}
#endif
