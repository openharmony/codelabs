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

/* ****************************************************************************
    1 头文件包含
**************************************************************************** */
#include "oam_ext_if.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define PRINT printk
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define PRINT dprintf

hi_u32 oam_log_level_set(hi_u32 log_level)
{
    if (log_level >= OAM_LOG_LEVEL_BUTT) {
        return HI_FAIL;
    }

    g_level_log = log_level;
    PRINT("\r\nSet log level to %d\r\n", log_level);
    return HI_SUCCESS;
}
#endif


static const char* g_log_tag[OAM_LOG_LEVEL_BUTT] = {
    [OAM_LOG_LEVEL_ERROR]   = "E",
    [OAM_LOG_LEVEL_WARNING] = "W",
    [OAM_LOG_LEVEL_INFO]    = "I",
    [OAM_LOG_LEVEL_DEBUG]   = "D",
    [OAM_LOG_LEVEL_VERBOSE] = "V",
};

hi_void oal_print_nlogs(const hi_char *pfile_name, const hi_char *pfuc_name, hi_u16 us_line_no, void *pfunc_addr,
    hi_u8 uc_vap_id, hi_u8 en_feature_id, hi_u8 clog_level, hi_u8 uc_param_cnt, hi_char *fmt, ...)
{
    hi_char buffer[OAM_PRINT_FORMAT_LENGTH] = {0};
    hi_s32 offset;
    hi_s32 tmp;

    hi_unref_param(pfile_name);
    hi_unref_param(pfuc_name);
    hi_unref_param(pfunc_addr);
    hi_unref_param(uc_vap_id);
    hi_unref_param(en_feature_id);
    hi_unref_param(uc_param_cnt);


    if (clog_level > g_level_log || clog_level >= OAM_LOG_LEVEL_BUTT) {
        return;
    }
    offset = snprintf_s(buffer, OAM_PRINT_FORMAT_LENGTH, OAM_PRINT_FORMAT_LENGTH - 1, "[%d][%s:%d]",
        oal_get_curr_time_ms(), g_log_tag[clog_level], us_line_no);
    if (offset == -1) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    tmp = vsprintf_s(buffer + offset, OAM_PRINT_FORMAT_LENGTH - offset - 1, fmt, args);
    if (tmp == -1) {
        va_end(args);
        return;
    }
    va_end(args);

    offset = offset + tmp;
    if (snprintf_s(buffer + offset,
        OAM_PRINT_FORMAT_LENGTH - offset, OAM_PRINT_FORMAT_LENGTH - offset - 1, "\r\n") == -1) {
        return;
    }
    PRINT(buffer);
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif