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

#ifndef __HI_TYPES_H__
#define __HI_TYPES_H__

#include <hi_types_base.h>
#include <hi_errno.h>


/* linux错误码 */
#define OAL_SUCC                0
#define OAL_EFAIL               1   /* 内核通用错误返回值 -1 */
#define OAL_EIO                 5   /* I/O error */
#define OAL_ENOMEM              12  /* Out of memory */
#define OAL_EFAUL               14  /* Bad address */
#define OAL_EBUSY               16  /* Device or resource busy */
#define OAL_ENODEV              19  /* No such device */
#define OAL_EINVAL              22  /* Invalid argument */

#define OAL_REFERENCE(data) ((void)(data))


#endif // __HI_TYPES_H__
