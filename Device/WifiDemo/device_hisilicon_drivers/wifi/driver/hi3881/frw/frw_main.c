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
#include "frw_main.h"
#include "frw_event.h"
#include "frw_timer.h"
#include "frw_task.h"
#include "oam_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* ROM化函数预留的回调接口 */
hi_void*            g_frw_rom_resv_func_cb[FRW_ROM_RESV_FUNC_BUTT] = {HI_NULL};
frw_init_enum_uint8 g_wlan_driver_init_state = FRW_INIT_STATE_BUTT;
hi_bool             g_frw_offload = HI_FALSE;    /* 默认非offload模式 */

/* ****************************************************************************
 功能描述  : 实现ROM化预留函数钩子注册，可以置空或者设置对应的钩子函数
**************************************************************************** */
hi_void frw_set_rom_resv_func(frw_rom_resv_func_enum_uint8 func_id, hi_void *func)
{
    if (func_id >= FRW_ROM_RESV_FUNC_BUTT) {
        return;
    }

    g_frw_rom_resv_func_cb[func_id] = func;
}

/* ****************************************************************************
 功能描述  : 获取对应的预留钩子函数
**************************************************************************** */
hi_void *frw_get_rom_resv_func(frw_rom_resv_func_enum_uint8 func_id)
{
    if (func_id >= FRW_ROM_RESV_FUNC_BUTT) {
        return HI_NULL;
    }

    return g_frw_rom_resv_func_cb[func_id];
}

/* ****************************************************************************
 功能描述  : 设置wifi驱动架构: OFFLOAD-TRUE 或者非OFFLOAD-FALSE
**************************************************************************** */
hi_void frw_set_offload_mode(hi_bool mode)
{
    g_frw_offload = mode;
}

/* ****************************************************************************
 功能描述  : 获取wifi驱动架构: OFFLOAD-TRUE 或者非OFFLOAD-FALSE
**************************************************************************** */
hi_bool frw_get_offload_mode(hi_void)
{
    return g_frw_offload;
}

/* ****************************************************************************
 功能描述  : FRW模块卸载

 修改历史      :
  1.日    期   : 2012年9月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void frw_main_exit(hi_void)
{
    /* 卸载事件管理模块 */
    frw_event_exit();
    /* FRW Task exit */
    frw_task_exit();
    /* 卸载成功后在置状态位 */
    frw_set_init_state(FRW_INIT_STATE_START);

    printk("frw_main_exit SUCCESSFULLY\r\n");
}

/* ****************************************************************************
 功能描述  : 设置初始化状态
 输入参数  : 初始化状态
 修改历史      :
  1.日    期   : 2012年11月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void frw_set_init_state(frw_init_enum_uint8 init_state)
{
    if (init_state >= FRW_INIT_STATE_BUTT) {
        return;
    }
    g_wlan_driver_init_state = init_state;
}

/* ****************************************************************************
 功能描述  : 获取初始化状态
 输入参数  : 初始化状态
 修改历史      :
  1.日    期   : 2012年11月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
frw_init_enum_uint8 frw_get_init_state(hi_void)
{
    return g_wlan_driver_init_state;
}

/* ****************************************************************************
 功能描述  : FRW模块初始化总入口，包含FRW模块内部所有特性的初始化。
 输入参数  : TRUE-OFFLOAD模式 FALSE-非OFFLOAD模式
 返 回 值  : 初始化返回值，成功或失败原因
 修改历史      :
  1.日    期   : 2012年9月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 frw_main_init(hi_bool mode, hi_u32 task_size)
{
    hi_unref_param(task_size);
    frw_set_init_state(FRW_INIT_STATE_START);
    /* 事件管理模块初始化 */
    if (oal_unlikely(frw_event_init() != HI_SUCCESS)) {
        oam_error_log0(0, 0, "{frw_main_init:: frw_event_init fail.}");
        return HI_FAIL;
    }
    frw_timer_init();

    if (oal_unlikely(frw_task_init() != HI_SUCCESS)) {
        oam_error_log0(0, 0, "{frw_main_init:: frw_task_init fail.}");
        frw_main_exit(); /* 失败后调用模块退出函数释放所有内存 */
        return HI_FAIL;
    }
    frw_set_offload_mode(mode);
    /* 启动成功后，输出打印 设置状态始终放最后 */
    frw_set_init_state(FRW_INIT_STATE_FRW_SUCC);

    printk("frw_main_init SUCCESSFULLY!\r\n");
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
