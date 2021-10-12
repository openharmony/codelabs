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

#ifndef __PLAT_SDIO_H__
#define __PLAT_SDIO_H__
/* ****************************************************************************
  1 Include other Head file
**************************************************************************** */
#include "hi_types.h"

/* ****************************************************************************
  2 Define macro
**************************************************************************** */
/* ****************************************************************************
  3 STRUCT DEFINE
**************************************************************************** */
/* ****************************************************************************
  4 EXTERN VARIABLE
**************************************************************************** */
/* ****************************************************************************
  5 EXTERN FUNCTION
**************************************************************************** */
extern hi_s32 sdio_patch_writesb(hi_u8 *buf, hi_u32 len);
extern hi_s32 sdio_patch_readsb(hi_u8 *buf, hi_u32 len, hi_u32 timeout);

#endif
