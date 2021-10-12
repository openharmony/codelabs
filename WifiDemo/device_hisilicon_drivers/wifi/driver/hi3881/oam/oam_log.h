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

#ifndef __OAM_LOG_H__
#define __OAM_LOG_H__

#include "hi_types.h"
#include "oal_err_wifi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 oam_log_level_set(hi_u32 log_level);

hi_void oal_print_nlogs(const hi_char *pfile_name, const hi_char *pfuc_name, hi_u16 us_line_no, void *pfunc_addr,
    hi_u8 uc_vap_id, hi_u8 en_feature_id, hi_u8 clog_level, hi_u8 uc_param_cnt, hi_char *fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* end of oam_log.h */
