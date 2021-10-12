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

#include "oam_main.h"
#include "hi_types_base.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static struct kobject *g_sysfs_hi110x_oam = HI_NULL;
#endif

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : OAM模块初始化总入口，包含OAM模块内部所有特性的初始化。
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static ssize_t log_level_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    if (buf == HI_NULL) {
        return -HI_FAIL;
    }

    return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1,
        "loglevel:             \n"
        " 0    close log           \n"
        " 1    ERROR               \n"
        " 2    WARN                \n"
        " 3    INFO                \n");
}

STATIC ssize_t store_log_level_set(struct device *dev, struct kobj_attribute *attr, const char *buf, size_t count)
{
    hi_s32 input;
    if (buf == HI_NULL) {
        return -HI_FAIL;
    }

    input = oal_atoi(buf);
    if (input < 0 || input > 5) { /* input must range [0 5] */
        return -HI_FAIL;
    }

    g_level_log = (hi_u32)input;
    return count;
}

STATIC struct kobj_attribute g_oam_host_log_attr =
    __ATTR(loglevel, 0664, (void *)log_level_show, (void *)store_log_level_set); /* mode 0664 */

static struct attribute *g_oam_log_attrs[] = {
    &g_oam_host_log_attr.attr,
#ifdef _SDIO_TEST
    &oam_sdio_test_attr.attr,
#endif
    NULL
};

static struct attribute_group g_oam_state_group = {
    .attrs = g_oam_log_attrs,
};

hi_s32 oam_user_ctrl_init(void)
{
    hi_s32 ret;
    g_sysfs_hi110x_oam = kobject_create_and_add("hi3881_debug", HI_NULL);
    if (g_sysfs_hi110x_oam == HI_NULL) {
        oam_print_err("kobject_create_and_add fail!ret=%d", -ENOMEM);
        return -ENOMEM;
    }

    ret = sysfs_create_group(g_sysfs_hi110x_oam, &g_oam_state_group);
    if (ret) {
        oam_print_err("sysfs_create_group fail!ret=%d", ret);
    }
    return ret;
}

static hi_s32 oam_user_ctrl_exit(hi_void)
{
    if (g_sysfs_hi110x_oam != HI_NULL) {
        sysfs_remove_group(g_sysfs_hi110x_oam, &g_oam_state_group);
        kobject_put(g_sysfs_hi110x_oam);
    }
    return HI_SUCCESS;
}
#endif

hi_s32 oam_main_init(hi_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hi_s32 ret = oam_user_ctrl_init();
    if (ret != HI_SUCCESS) {
        return ret;
    }
#endif
    printk("oam_main_init SUCCESSFULLY!\r\n");
    return HI_SUCCESS;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/* ****************************************************************************
 功能描述  : OAM模块卸载
 返 回 值  : 模块卸载返回值，成功或失败原因
**************************************************************************** */
hi_void oam_main_exit(hi_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hi_s32 ret = oam_user_ctrl_exit();
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, 0, "oam_main_exit:: oam_user_ctrl_exit fail!");
    }
#endif
    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
