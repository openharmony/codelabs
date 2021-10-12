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

#include "mac_resource.h"
#include "oal_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  全局变量定义
**************************************************************************** */
mac_res_hash_stru g_mac_hash_res;

/* ****************************************************************************
  函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 资源池退出，释放动态申请的内存
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : HiSilicon
**************************************************************************** */
hi_void mac_res_exit(hi_void)
{
    hi_free(HI_MOD_ID_WIFI_DRV, g_mac_hash_res.user_hash_info);
    hi_free(HI_MOD_ID_WIFI_DRV, g_mac_hash_res.pul_idx);
    hi_free(HI_MOD_ID_WIFI_DRV, g_mac_hash_res.puc_user_cnt);

    g_mac_hash_res.user_hash_info = HI_NULL;
    g_mac_hash_res.pul_idx        = HI_NULL;
    g_mac_hash_res.puc_user_cnt   = HI_NULL;
}

/* ****************************************************************************
 功能描述  : 初始化MAC资源池内容
 修改历史      :
  1.日    期   : 2013年5月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 mac_res_init(hi_void)
{
    hi_u32 loop;
    hi_u8 user_num = oal_mem_get_user_res_num();
    hi_void *hash_info = HI_NULL;
    hi_void *hash_idx  = HI_NULL;
    hi_void *hash_cnt  = HI_NULL;

    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(&g_mac_hash_res, sizeof(mac_res_hash_stru), 0, sizeof(mac_res_hash_stru));

    /* **************************************************************************
            初始化HASH桶的资源管理内容
    ************************************************************************** */
    hash_info = hi_malloc(HI_MOD_ID_WIFI_DRV, sizeof(mac_res_user_hash_stru) * user_num);
    hash_idx  = hi_malloc(HI_MOD_ID_WIFI_DRV, sizeof(hi_u32) * user_num);
    hash_cnt  = hi_malloc(HI_MOD_ID_WIFI_DRV, sizeof(hi_u8) * user_num);
    if ((hash_info == HI_NULL) || (hash_idx == HI_NULL) || (hash_cnt == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{mac_res_init::malloc fail.}");
        goto exit;
    }
    /* 内存初始清0 */
    /* 安全编程规则6.6例外(3) 从堆中分配内存后，赋予初值 */
    memset_s(hash_info, (sizeof(mac_res_user_hash_stru) * user_num), 0, (sizeof(mac_res_user_hash_stru) * user_num));
    memset_s(hash_idx, (sizeof(hi_u32) * user_num), 0, (sizeof(hi_u32) * user_num));
    memset_s(hash_cnt, (sizeof(hi_u8) * user_num), 0, (sizeof(hi_u8) * user_num));

    g_mac_hash_res.user_hash_info = hash_info;
    g_mac_hash_res.pul_idx            = hash_idx;
    g_mac_hash_res.puc_user_cnt       = hash_cnt;
    oal_queue_set(&(g_mac_hash_res.queue), g_mac_hash_res.pul_idx, user_num);
    for (loop = 0; loop < user_num; loop++) {
        /* 初始值保存的是对应数组下标值加1 */
        oal_queue_enqueue(&(g_mac_hash_res.queue), (hi_void *)(uintptr_t)(loop + 1));
        /* 初始化对应的引用计数值为0 */
        g_mac_hash_res.puc_user_cnt[loop] = 0;
    }
    return HI_SUCCESS;

exit: // 操作失败之后，进行内存释放等操作，属于例外，lint_t e801告警屏蔽
    if (hash_info != HI_NULL) {
        hi_free(HI_MOD_ID_WIFI_DRV, hash_info);
    }
    if (hash_idx != HI_NULL) {
        hi_free(HI_MOD_ID_WIFI_DRV, hash_idx);
    }
    if (hash_cnt != HI_NULL) {
        hi_free(HI_MOD_ID_WIFI_DRV, hash_cnt);
    }
    return HI_FAIL;
}

/* ****************************************************************************
 功能描述  : 释放对应HASH的内存
 输入参数  : 对应HASH内存索引
 返 回 值  : HI_SUCCESS/HI_FAIL
 修改历史      :
  1.日    期   : 2013年5月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 mac_res_free_hash(hi_u32 hash_idx)
{
    if (oal_unlikely(hash_idx >= (hi_u32)g_mac_hash_res.queue.max_elements)) {
        return HI_FAIL;
    }
    if (g_mac_hash_res.puc_user_cnt[hash_idx] == 0) {
        oam_error_log1(0, OAM_SF_ANY, "mac_res_free_hash::cnt==0! idx:%d", hash_idx);
        return HI_SUCCESS;
    }
    (g_mac_hash_res.puc_user_cnt[hash_idx])--;
    if (g_mac_hash_res.puc_user_cnt[hash_idx] != 0) {
        return HI_SUCCESS;
    }
    /* 入队索引值需要加1操作 */
    oal_queue_enqueue(&(g_mac_hash_res.queue), (hi_void *)((uintptr_t)hash_idx + 1));
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取一个HASH资源
 输出参数  : HASH内存索引值
 返 回 值  : HI_SUCCESS/HI_FAIL
 修改历史      :
  1.日    期   : 2013年5月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 mac_res_alloc_hash(hi_u8 *puc_hash_idx)
{
    hi_u32 hash_idx_temp;

    hash_idx_temp = (hi_u32)(uintptr_t)oal_queue_dequeue(&(g_mac_hash_res.queue));
    /* 0为无效值 */
    if (hash_idx_temp == 0) {
        return HI_FAIL;
    }
    *puc_hash_idx = (hi_u8)(hash_idx_temp - 1);
    (g_mac_hash_res.puc_user_cnt[hash_idx_temp - 1])++;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取对应hash索引的内存
 输入参数  : 对应hash内存索引
 返 回 值  : 对应内存地址
 修改历史      :
  1.日    期   : 2013年5月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
mac_res_user_hash_stru *mac_res_get_hash(hi_u8 dev_idx)
{
    return &(g_mac_hash_res.user_hash_info[dev_idx]);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
