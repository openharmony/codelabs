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
#include "oal_kernel_file.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
static oal_kobject *g_conn_syfs_root_object = HI_NULL;
static oal_kobject *g_conn_syfs_root_boot_object = HI_NULL;

oal_kobject *oal_get_sysfs_root_object(hi_void)
{
    if (g_conn_syfs_root_object != HI_NULL)
        return g_conn_syfs_root_object;
    g_conn_syfs_root_object = kobject_create_and_add("hisys", HI_NULL);
    return g_conn_syfs_root_object;
}

oal_kobject *oal_get_sysfs_root_boot_object(hi_void)
{
    static oal_kobject *root_boot_object = HI_NULL;
    if (g_conn_syfs_root_boot_object != HI_NULL)
        return g_conn_syfs_root_boot_object;
    root_boot_object = oal_get_sysfs_root_object();
    if (root_boot_object == HI_NULL)
        return HI_NULL;
    g_conn_syfs_root_boot_object = kobject_create_and_add("boot", root_boot_object);
    return g_conn_syfs_root_boot_object;
}

oal_kobject *oal_conn_sysfs_root_obj_init(hi_void)
{
    return oal_get_sysfs_root_object();
}

hi_void oal_conn_sysfs_root_obj_exit(hi_void)
{
    if (g_conn_syfs_root_object != HI_NULL) {
        kobject_put(g_conn_syfs_root_object);
        g_conn_syfs_root_object = HI_NULL;
    }
}

hi_void oal_conn_sysfs_root_boot_obj_exit(hi_void)
{
    if (g_conn_syfs_root_boot_object != HI_NULL) {
        kobject_del(g_conn_syfs_root_boot_object);
        g_conn_syfs_root_boot_object = HI_NULL;
    }
}

#else
oal_kobject *oal_get_sysfs_root_object(hi_void)
{
    return HI_NULL;
}

oal_kobject *oal_conn_sysfs_root_obj_init(hi_void)
{
    return HI_NULL;
}

hi_void oal_conn_sysfs_root_obj_exit(hi_void)
{
    return;
}

hi_void oal_conn_sysfs_root_boot_obj_exit(hi_void)
{
    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
