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
#include "oal_main.h"
#include "oal_mem.h"
#include "oal_net.h"
#include "oam_ext_if.h"
#include "hcc_hmac_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 根据配置的资源数量初始化资源池配置
**************************************************************************** */
static hi_u32 oal_main_init_mem_pool_cfg(const hi_u8 vap_num, const hi_u8 user_num)
{
    const hi_u16 mem_dog_size = OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE;
    hi_u8 loop;

    /* 本地变量内存池各子池基础使用数量, 驱动加载即使用的数量 */
    oal_mem_subpool_cfg_stru ast_base_cfg[WLAN_MEM_LOCAL_SUBPOOL_CNT] = {
        {WLAN_MEM_LOCAL_SIZE1 + mem_dog_size, 22}, {WLAN_MEM_LOCAL_SIZE2 + mem_dog_size, 10},
        {WLAN_MEM_LOCAL_SIZE3 + mem_dog_size, 3},  {WLAN_MEM_LOCAL_SIZE4 + mem_dog_size, 2},
        {WLAN_MEM_LOCAL_SIZE5 + mem_dog_size, 3},  {WLAN_MEM_LOCAL_SIZE6 + mem_dog_size, 1}
    };

    oal_mem_subpool_cfg_stru ast_event_cfg[WLAN_MEM_EVENT_SUBPOOL_CNT];
    oal_mem_subpool_cfg_stru ast_local_cfg[WLAN_MEM_LOCAL_SUBPOOL_CNT];
    oal_mem_subpool_cfg_stru ast_mib_cfg[WLAN_MEM_MIB_SUBPOOL_CNT] = {{WLAN_MEM_MIB_SIZE, vap_num}};
    hi_u8 user_base[] = { 18, 0, 3, 1, 0, 2 }; /* 本地变量内存池各子池:添加用户时使用数量 */
    hi_u8 vap_base[]  = { 15, 20, 2, 5, 0, 0 }; /* 本地变量内存池各子池:添加vap时使用数量 */

    /* 事件池支持单用户场景以及其他两种模式配置 */
    ast_event_cfg[0].us_size = WLAN_MEM_EVENT_SIZE1 + mem_dog_size;
    ast_event_cfg[1].us_size = WLAN_MEM_EVENT_SIZE2 + mem_dog_size;
    ast_event_cfg[0].us_cnt = (user_num == 1) ? WLAN_MEM_EVENT_CNT1 : WLAN_MEM_EVENT_MULTI_USER_CNT1;
    ast_event_cfg[1].us_cnt = (user_num == 1) ? WLAN_MEM_EVENT_CNT2 : WLAN_MEM_EVENT_MULTI_USER_CNT2;

    hi_u32 ret = oal_mem_set_subpool_config(ast_event_cfg, OAL_MEM_POOL_ID_EVENT, WLAN_MEM_EVENT_SUBPOOL_CNT);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* 01:根据支持的用户数量配置本地变量内存池 */
    for (loop = 0; loop < WLAN_MEM_LOCAL_SUBPOOL_CNT; loop++) {
        ast_local_cfg[loop].us_size = ast_base_cfg[loop].us_size;

        /* 配置vap的使用量算在基准值里面 */
        ast_local_cfg[loop].us_cnt = ast_base_cfg[loop].us_cnt + user_num * user_base[loop] + vap_num * vap_base[loop];
    }

    ret = oal_mem_set_subpool_config(ast_local_cfg, OAL_MEM_POOL_ID_LOCAL, WLAN_MEM_LOCAL_SUBPOOL_CNT);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* 02:根据支持的vap数量配置MIB内存池 mib池子池只有一个 配置vap没有mib,数量在vap资源上-1 */
    ret = oal_mem_set_subpool_config(ast_mib_cfg, OAL_MEM_POOL_ID_MIB, WLAN_MEM_MIB_SUBPOOL_CNT);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : OAL模块初始化总入口，包含OAL模块内部所有特性的初始化。
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_u32 oal_main_init(const hi_u8 vap_num, const hi_u8 user_num)
{
    /* 保存用户配置的vap 和user资源数 */
    if ((oal_mem_set_vap_res_num(vap_num, WLAN_SERVICE_VAP_NUM_PER_DEVICE) != HI_SUCCESS) ||
        (oal_mem_set_user_res_num(user_num, WLAN_ACTIVE_USER_MAX_NUM) != HI_SUCCESS)) {
        oam_error_log0(0, 0, "oal_main_init: set user/vap failed.");
        return HI_FAIL;
    }
    /* 内存池配置初始化 vap user数量前面已经有校验 */
    if (oal_main_init_mem_pool_cfg(vap_num, user_num) != HI_SUCCESS) {
        oam_error_log0(0, 0, "oal_main_init: init mem pool cfg failed.");
        return HI_FAIL;
    }
    /* 内存池初始化 */
    if (oal_mem_init_pool() != HI_SUCCESS) {
        oam_error_log0(0, 0, "oal_main_init: oal_mem_init_pool failed.");
        return HI_FAIL;
    }
    printk("oal_main_init SUCCESSFULLY\r\n");
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : OAL模块卸载
 返 回 值  : 模块卸载返回值，成功或失败原因
**************************************************************************** */
hi_void oal_main_exit(hi_void)
{
    hcc_hmac_exit();
    /* 内存池卸载 */
    oal_mem_exit();

    printk("oal_main_exit SUCCESSFULLY\r\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
