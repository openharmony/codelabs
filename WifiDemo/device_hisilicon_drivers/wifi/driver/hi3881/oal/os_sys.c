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
#include <hi_config.h>

hi_u32 ms2systick(HI_IN hi_u32 ms, HI_IN hi_bool include0)
{
    hi_u32 tick;

    /* >10ms向下对齐 */
    if (ms > HI_MILLISECOND_PER_TICK) {
        tick = ms / HI_MILLISECOND_PER_TICK; /* convert from ms to ticks */
    }
    /* <10ms向上对齐 */
    else {
        if ((HI_TRUE == include0) && (0 == ms)) {
            tick = 0;
        } else {
            tick = 1;
        }
    }

    return tick;
}
