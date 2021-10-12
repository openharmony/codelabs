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
#include "oal_mem.h"
#include "oam_ext_if.h"
#include "hcc_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 2 宏定义 */
/* ****************************************************************************
  2 结构体定义
**************************************************************************** */
/* ****************************************************************************
  结构名  : oal_mem_subpool_stat
  结构说明: 子内存池统计结构体，维测使用
**************************************************************************** */
typedef struct {
    hi_u16 us_free_cnt;  /* 本子内存池可用内存块数 */
    hi_u16 us_total_cnt; /* 本子内存池内存块总数 */
} oal_mem_subpool_stat;

/* ****************************************************************************
  结构名  : oal_mem_pool_stat
  结构说明: 单个内存池统计结构体，维测使用
**************************************************************************** */
typedef struct {
    hi_u16 us_mem_used_cnt;  /* 本内存池已用内存块 */
    hi_u16 us_mem_total_cnt; /* 本内存池一共有多少内存块 */

    oal_mem_subpool_stat ast_subpool_stat[WLAN_MEM_MAX_SUBPOOL_NUM];
} oal_mem_pool_stat;

/* ****************************************************************************
  结构名  : oal_mem_stat
  结构说明: 内存池统计结构体，维测使用
**************************************************************************** */
typedef struct {
    oal_mem_pool_stat ast_mem_start_stat[OAL_MEM_POOL_ID_BUTT]; /* 起始统计信息 */
    oal_mem_pool_stat ast_mem_end_stat[OAL_MEM_POOL_ID_BUTT];   /* 终止统计信息 */
} oal_mem_stat;

/* ****************************************************************************
  3 全局变量定义
**************************************************************************** */
/* *****************************************************************************
    共享描述符内存池配置信息全局变量
****************************************************************************** */
oal_mem_subpool_cfg_stru g_ast_shared_dscr_cfg_table[WLAN_MEM_SHARE_DSCR_SUBPOOL_CNT];

/* *****************************************************************************
    本地数据内存池配置信息全局变量 修改为根据定制化用户数量配置
****************************************************************************** */
oal_mem_subpool_cfg_stru g_ast_local_cfg_table[WLAN_MEM_LOCAL_SUBPOOL_CNT];

/* *****************************************************************************
    事件内存池配置信息全局变量
****************************************************************************** */
oal_mem_subpool_cfg_stru g_ast_event_cfg_table[WLAN_MEM_EVENT_SUBPOOL_CNT];

/* *****************************************************************************
    MIB内存池配置信息全局变量
****************************************************************************** */
oal_mem_subpool_cfg_stru g_ast_mib_cfg_table[WLAN_MEM_MIB_SUBPOOL_CNT];

/* *****************************************************************************
    netbuf内存池配置信息全局变量
****************************************************************************** */
oal_mem_subpool_cfg_stru g_ast_netbuf_cfg_table[OAL_MEM_NETBUF_POOL_ID_BUTT];

/* *****************************************************************************
    总的内存池配置信息全局变量
****************************************************************************** */
const oal_mem_pool_cfg_stru g_ast_mem_pool_cfg_table[] = {
    /*       内存池ID                           内存池子内存池个数               四字节对齐      内存池配置信息 */
    {OAL_MEM_POOL_ID_EVENT,           hi_array_size(g_ast_event_cfg_table),       {0, 0}, g_ast_event_cfg_table},
    {OAL_MEM_POOL_ID_LOCAL,           hi_array_size(g_ast_local_cfg_table),       {0, 0}, g_ast_local_cfg_table},
    {OAL_MEM_POOL_ID_MIB,             hi_array_size(g_ast_mib_cfg_table),         {0, 0}, g_ast_mib_cfg_table},
};

/* *****************************************************************************
    内存池信息全局变量，存储整个内存管理中所有内存池信息
    所有内存管理的函数都基于此全局变量进行操作
****************************************************************************** */
oal_mem_pool_stru g_ast_mem_pool[OAL_MEM_POOL_ID_BUTT];
/* *****************************************************************************
    malloc内存指针记录
****************************************************************************** */
hi_u8 *g_pauc_pool_base_addr[OAL_MEM_POOL_ID_BUTT] = {HI_NULL};

/* 一个内存块结构大小 + 一个指针大小 */
#define OAL_MEM_CTRL_BLK_SIZE (sizeof(oal_mem_stru *) + sizeof(oal_mem_stru))

/* *****************************************************************************
    控制块内存空间，为内存块结构体和指向内存块结构体的指针分配空间
    由函数oal_mem_ctrl_blk_alloc调用
****************************************************************************** */
oal_mem_ctrl_blk_stru g_ctrl_blk;
hi_u8 g_vap_res_num;  /* vap资源数量 内存规格 api入参指定 */
hi_u8 g_user_res_num; /* vap资源数量 内存规格 api入参指定 */

/* ****************************************************************************
 功能描述  : 配置驱动支持同时启动的vap个数 用户配置范围1-3, mac层固定占用一个配置vap,故vap资源需要+1
 修改历史      :
  1.日    期   : 2019年6月28日
    作    者   : HiSilicon
**************************************************************************** */
hi_u32 oal_mem_set_vap_res_num(const hi_u8 vap_res_num, const hi_u8 vap_spec)
{
    if ((vap_res_num == 0) || (vap_res_num > vap_spec)) {
        hi_diag_log_msg_e1(0, "oal_mem_set_vap_res_num, invalid vap res num = %d!", vap_res_num);
        return HI_FAIL;
    }
    g_vap_res_num = vap_res_num + WLAN_CFG_VAP_NUM_PER_DEVICE;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取驱动支持vap个数
**************************************************************************** */
hi_u8 oal_mem_get_vap_res_num(hi_void)
{
    return g_vap_res_num;
}

/* ****************************************************************************
 功能描述  : 配置驱动支持最大接入的用户个数多VAP时共享 范围1-8
**************************************************************************** */
hi_u32 oal_mem_set_user_res_num(const hi_u8 user_res_num, const hi_u8 user_spec)
{
    if ((user_res_num == 0) || (user_res_num > user_spec)) {
        hi_diag_log_msg_e1(0, "oal_mem_set_user_res_num, invalid user res num = %d!", user_res_num);
        return HI_FAIL;
    }
    g_user_res_num = user_res_num;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取驱动支持最大接入的用户个数
**************************************************************************** */
hi_u8 oal_mem_get_user_res_num(hi_void)
{
    return g_user_res_num;
}

/* ****************************************************************************
 功能描述  : 配置对应内存池size和cnt，供定制化调用，修改内存池配置
**************************************************************************** */
hi_u32 oal_mem_set_subpool_config(const oal_mem_subpool_cfg_stru *subpool_cfg, oal_mem_pool_id_enum_uint8 pool_id,
    hi_u8 subpool_num)
{
    oal_mem_subpool_cfg_stru    *cfg = HI_NULL;
    hi_u8                       loop;

    if (pool_id >= OAL_MEM_POOL_ID_BUTT) {
        return HI_FAIL;
    }

    cfg = g_ast_mem_pool_cfg_table[pool_id].subpool_cfg_info;
    for (loop = 0; loop < subpool_num; loop++) {
        cfg[loop].us_size = subpool_cfg[loop].us_size;
        cfg[loop].us_cnt = subpool_cfg[loop].us_cnt;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取对应内存池申请的内存个数
 输入参数  : en_pool_id     : 内存池ID
 输出参数  : pul_total_cnt  : 对应内存池占用的总个数
**************************************************************************** */
hi_u16 oal_mem_get_total_cnt_in_pool(oal_mem_pool_id_enum_uint8 pool_id)
{
    hi_u16 us_subpool_idx;
    hi_u16 us_total_cnt; /* 本内存池总字节数 */
    const oal_mem_pool_cfg_stru *mem_pool_cfg = HI_NULL;

    mem_pool_cfg = &g_ast_mem_pool_cfg_table[pool_id];
    us_total_cnt = 0;
    for (us_subpool_idx = 0; us_subpool_idx < mem_pool_cfg->subpool_cnt; us_subpool_idx++) {
        us_total_cnt += mem_pool_cfg->subpool_cfg_info[us_subpool_idx].us_cnt;
    }
    return us_total_cnt;
}

/* ****************************************************************************
 功能描述  : 为每个内存块结构体或指向内存块结构体的指针提供内存
 输入参数  : ul_size:要分配内存的大小
 返 回 值  : 指向一块内存的指针 或空指针
**************************************************************************** */
hi_u8 *oal_mem_ctrl_blk_alloc(hi_u32 size)
{
    hi_u8 *puc_alloc = HI_NULL;

    size = hi_byte_align(size, 4); /* 4: 4bytes 对齐 */
    if ((g_ctrl_blk.idx + size) > g_ctrl_blk.max_size) {
        hi_diag_log_msg_e1(0, "oal_mem_ctrl_blk_alloc, not_enough memory, size = %d!", size);
        return HI_NULL;
    }
    puc_alloc = g_ctrl_blk.puc_base_addr + g_ctrl_blk.idx;
    g_ctrl_blk.idx += size;
    return puc_alloc;
}

/* ****************************************************************************
 功能描述  : 创建子内存池
 输入参数  : en_pool_id   : 内存池ID
             puc_base_addr: 内存池基地址
 返 回 值  : HI_SUCCESS或其它错误码
**************************************************************************** */
hi_u32 oal_mem_create_subpool(oal_mem_pool_id_enum_uint8 pool_id, hi_u8 *puc_base_addr)
{
    oal_mem_pool_stru      *mem_pool = HI_NULL;
    oal_mem_subpool_stru   *mem_subpool = HI_NULL;
    oal_mem_stru           *mem = HI_NULL;
    oal_mem_stru          **stack_mem = HI_NULL;
    hi_u8                   subpool_id;
    hi_u32                  blk_id;

    mem_pool = &g_ast_mem_pool[pool_id];
    /* 申请可用内存地址索引表，每个内存池申请一次，后面分割给每个子内存池使用 */
    stack_mem = (oal_mem_stru **)oal_mem_ctrl_blk_alloc(sizeof(oal_mem_stru *) * mem_pool->us_mem_total_cnt);
    if (stack_mem == HI_NULL) {
        hi_diag_log_msg_e0(0, "oal_mem_create_subpool, pointer is NULL!");
        return HI_ERR_CODE_MEM_ALLOC_CTRL_BLK_FAIL;
    }

    /* 申请oal_mem_stru结构体，每个内存池申请一次，后面分割给每个子内存池使用 */
    mem = (oal_mem_stru *)oal_mem_ctrl_blk_alloc(sizeof(oal_mem_stru) * mem_pool->us_mem_total_cnt);
    if (mem == HI_NULL) {
        hi_diag_log_msg_e0(0, "oal_mem_create_subpool, pointer is NULL!");
        return HI_ERR_CODE_MEM_ALLOC_CTRL_BLK_FAIL;
    }

    /* 安全编程规则6.6例外(3) 从堆中分配内存后，赋予初值 */
    memset_s(stack_mem, sizeof(oal_mem_stru *) * mem_pool->us_mem_total_cnt, 0,
        sizeof(oal_mem_stru *) * mem_pool->us_mem_total_cnt);
    memset_s((hi_void *)mem, sizeof(oal_mem_stru) * mem_pool->us_mem_total_cnt, 0,
        sizeof(oal_mem_stru) * mem_pool->us_mem_total_cnt);

    /* 记录该内存池初始oal_mem_stru结构的指针，检查内存信息时使用 */
    mem_pool->mem_start_addr = mem;

    /* 设置各子池所有内存块结构体信息，建立各内存块与payload的关系 */
    for (subpool_id = 0; subpool_id < mem_pool->subpool_cnt; subpool_id++) {
        /* 得到每一级子内存池信息 */
        mem_subpool = &(mem_pool->ast_subpool_table[subpool_id]);
        /* 建立子内存数索引表和可用内存索引表的关系 */
        mem_subpool->free_stack = (hi_void **)stack_mem;
        oal_spin_lock_init(&mem_subpool->st_spinlock);
        for (blk_id = 0; blk_id < mem_subpool->us_total_cnt; blk_id++) {
            mem->pool_id        = pool_id;
            mem->subpool_id     = subpool_id;
            mem->us_len         = mem_subpool->us_len;
            mem->mem_state_flag = OAL_MEM_STATE_FREE;
            mem->user_cnt       = 0;
            mem->puc_origin_data   = puc_base_addr;       /* 建立oal_mem_st与对应payload的关系 */
            mem->puc_data          = mem->puc_origin_data;
           *stack_mem = mem;
            stack_mem++;
            mem++;
            puc_base_addr += mem_subpool->us_len;
        }
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置每个内存池的子内存池结构体
 输入参数  : en_pool_id       : 内存池ID
             puc_data_mem_addr: 内存池基地址
 返 回 值  : HI_SUCCESS 或其它错误码
**************************************************************************** */
hi_u32 oal_mem_create_pool(oal_mem_pool_id_enum_uint8 pool_id, hi_u8 *puc_base_addr)
{
    hi_u8                           subpool_id = 0;
    hi_u8                           subpool_cnt;
    oal_mem_pool_stru              *mem_pool = HI_NULL;
    oal_mem_subpool_stru           *mem_subpool = HI_NULL;
    const oal_mem_pool_cfg_stru    *mem_pool_cfg = HI_NULL;
    oal_mem_subpool_cfg_stru       *mem_subpool_cfg = HI_NULL;

    mem_pool = &g_ast_mem_pool[pool_id];
    mem_pool_cfg = &g_ast_mem_pool_cfg_table[pool_id];
    /* 初始化内存池的通用变量 */
    subpool_cnt = mem_pool_cfg->subpool_cnt;
    mem_pool->subpool_cnt = mem_pool_cfg->subpool_cnt;
    mem_pool->us_mem_used_cnt = 0;
    mem_pool->us_max_byte_len = mem_pool_cfg->subpool_cfg_info[subpool_cnt - 1].us_size;

    if (mem_pool->subpool_cnt > WLAN_MEM_MAX_SUBPOOL_NUM) {
        hi_diag_log_msg_e0(0, "oal_mem_create_pool, exceeds the max subpool number!");
        return HI_ERR_CODE_MEM_EXCEED_SUBPOOL_CNT;
    }

    /* 设置每一级子内存池 */
    mem_pool->us_mem_total_cnt = 0;
    for (subpool_id = 0; subpool_id < subpool_cnt; subpool_id++)  {
        mem_subpool_cfg           = mem_pool_cfg->subpool_cfg_info + subpool_id;
        mem_subpool               = &(mem_pool->ast_subpool_table[subpool_id]);
        mem_subpool->us_free_cnt  = mem_subpool_cfg->us_cnt;
        mem_subpool->us_total_cnt = mem_subpool_cfg->us_cnt;
        mem_subpool->us_len       = mem_subpool_cfg->us_size;
        mem_pool->us_mem_total_cnt += mem_subpool_cfg->us_cnt;   /* 设置总内存块数 */
    }
    return oal_mem_create_subpool(pool_id, puc_base_addr);
}

/* ****************************************************************************
 功能描述  : 分配内存
 输入参数  : uc_pool_id  : 所申请内存的内存池ID
             us_len      : 所申请内存块长度
 返 回 值  : 成功: 指向所分配内存起始地址的指针
             失败: 空指针
**************************************************************************** */
hi_void *oal_mem_alloc(oal_mem_pool_id_enum_uint8 pool_id, hi_u16 us_len)
{
    oal_mem_stru *mem = HI_NULL;

    /* 异常: 申请长度为零 */
    if (oal_unlikely(us_len == 0)) {
        return HI_NULL;
    }
    us_len += OAL_MEM_INFO_SIZE;
    mem = oal_mem_alloc_enhanced(pool_id, us_len);
    if (oal_unlikely(mem == HI_NULL)) {
        return HI_NULL;
    }
    mem->puc_data = mem->puc_origin_data + OAL_MEM_INFO_SIZE;
    *((uintptr_t *)(mem->puc_data - OAL_MEM_INFO_SIZE)) = (uintptr_t)mem;
    return (hi_void *)mem->puc_data;
}

/* ****************************************************************************
 功能描述  : 释放内存
 输入参数  : p_data      : 要释放内存块地址
 返 回 值  : HI_SUCCESS 或者其它错误码
**************************************************************************** */
hi_u32 oal_mem_free(const hi_void *data)
{
    oal_mem_stru *mem = HI_NULL;

    if (oal_unlikely(data == HI_NULL)) {
        return HI_ERR_CODE_PTR_NULL;
    }
    mem = (oal_mem_stru *)(*((uintptr_t *)((hi_u8 *)data - OAL_MEM_INFO_SIZE)));
    return oal_mem_free_enhanced(mem);
}

/* ****************************************************************************
 功能描述  : 恢复(释放)已经分配的内存
**************************************************************************** */
hi_void oal_mem_release(hi_void)
{
    hi_u32 pool_id;

    if (g_ctrl_blk.puc_base_addr != HI_NULL) {
        hi_free(HI_MOD_ID_WIFI_DRV, (hi_void *)g_ctrl_blk.puc_base_addr);
        g_ctrl_blk.puc_base_addr = HI_NULL;
    }
    for (pool_id = 0; pool_id < OAL_MEM_POOL_ID_BUTT; pool_id++) {
        if (g_pauc_pool_base_addr[pool_id] != HI_NULL) {
            hi_free(HI_MOD_ID_WIFI_DRV, (hi_void *)g_pauc_pool_base_addr[pool_id]);
            g_pauc_pool_base_addr[pool_id] = HI_NULL;
        }
    }
}

/* ****************************************************************************
 功能描述  : 初始化控制块内存
**************************************************************************** */
hi_u32 oal_mem_init_ctrl_blk(hi_void)
{
    hi_u16 us_total_cnt = 0;
    hi_u8 pool_id;

    g_ctrl_blk.idx = 0;
    for (pool_id = 0; pool_id < OAL_MEM_POOL_ID_BUTT; pool_id++) {
        us_total_cnt += oal_mem_get_total_cnt_in_pool(pool_id);
    }
    g_ctrl_blk.max_size = (hi_u32)(us_total_cnt * OAL_MEM_CTRL_BLK_SIZE);
    /* 申请内存 */
    g_ctrl_blk.puc_base_addr = (hi_u8 *)hi_malloc(HI_MOD_ID_WIFI_DRV, g_ctrl_blk.max_size);
    if (g_ctrl_blk.puc_base_addr == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 内存模块卸载接口
 返 回 值  : HI_SUCCESS
**************************************************************************** */
hi_void oal_mem_exit(hi_void)
{
    /* 卸载普通内存池 */
    oal_mem_release();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

/* ****************************************************************************
 功能描述  : 分配内存
 输入参数  : uc_pool_id  : 所申请内存的内存池ID
             us_len      : 所申请内存块长度
 返 回 值  : 分配的内存块结构体指针，或空指针
**************************************************************************** */
oal_mem_stru *oal_mem_alloc_enhanced(oal_mem_pool_id_enum_uint8 pool_id, hi_u16 us_len)
{
    oal_mem_pool_stru    *mem_pool = HI_NULL;
    oal_mem_subpool_stru *mem_subpool = HI_NULL;
    oal_mem_stru         *mem = HI_NULL;
    unsigned long         irq_flag = 0;
    hi_u8                subpool_id;

    /* 获取内存池 */
    mem_pool = &g_ast_mem_pool[pool_id];
#ifdef _PRE_DEBUG_MODE
    us_len += OAL_DOG_TAG_SIZE;
#endif
    /* 异常: 申请长度不在该内存池内  */
    if (oal_unlikely(us_len > mem_pool->us_max_byte_len)) {
        return HI_NULL;
    }
    for (subpool_id = 0; subpool_id < mem_pool->subpool_cnt; subpool_id++) {
        mem_subpool = &(mem_pool->ast_subpool_table[subpool_id]);
        oal_spin_lock_irq_save(&mem_subpool->st_spinlock, &irq_flag);
        if ((mem_subpool->us_len < us_len) || (mem_subpool->us_free_cnt == 0)) {
            oal_spin_unlock_irq_restore(&mem_subpool->st_spinlock, &irq_flag);
            continue;
        }
        /* 获取一个没有使用的oal_mem_stru结点 */
        mem_subpool->us_free_cnt--;
        mem = (oal_mem_stru *)mem_subpool->free_stack[mem_subpool->us_free_cnt];
        mem->puc_data = mem->puc_origin_data;
        mem->user_cnt = 1;
        mem->mem_state_flag = OAL_MEM_STATE_ALLOC;
        mem_pool->us_mem_used_cnt++;
        oal_spin_unlock_irq_restore(&mem_subpool->st_spinlock, &irq_flag);
        break;
    }
    return mem;
}

hi_u32 oal_mem_free_enhanced(oal_mem_stru *mem)
{
    oal_mem_pool_stru      *mem_pool = HI_NULL;
    oal_mem_subpool_stru   *mem_subpool = HI_NULL;
    unsigned long           irq_flag;

    if (oal_unlikely(mem == HI_NULL)) {
        return HI_ERR_CODE_PTR_NULL;
    }
    if (mem->pool_id >= OAL_MEM_POOL_ID_BUTT) {
        return HI_ERR_CODE_PTR_NULL;
    }
    mem_pool = &g_ast_mem_pool[mem->pool_id];
    if (mem->subpool_id >= mem_pool->subpool_cnt) {
        return HI_ERR_CODE_PTR_NULL;
    }
    mem_subpool = &(mem_pool->ast_subpool_table[mem->subpool_id]);
    oal_spin_lock_irq_save(&mem_subpool->st_spinlock, &irq_flag);
    /* 异常: 释放一块已经被释放的内存 */
    if (oal_unlikely(mem->mem_state_flag == OAL_MEM_STATE_FREE)) {
        oal_spin_unlock_irq_restore(&mem_subpool->st_spinlock, &irq_flag);
        return HI_ERR_CODE_MEM_ALREADY_FREE;
    }
    /* 异常: 释放一块引用计数为0的内存 */
    if (oal_unlikely(mem->user_cnt == 0)) {
        oal_spin_unlock_irq_restore(&mem_subpool->st_spinlock, &irq_flag);
        return HI_ERR_CODE_MEM_USER_CNT_ERR;
    }
    mem->user_cnt--;
    /* 该内存块上是否还有其他共享用户，直接返回 */
    if (mem->user_cnt != 0) {
        oal_spin_unlock_irq_restore(&mem_subpool->st_spinlock, &irq_flag);
        return HI_SUCCESS;
    }
    /* 异常: 该子内存池可用内存块数目超过整个子内存池总内存块数 */
    if (oal_unlikely(mem_subpool->us_free_cnt >= mem_subpool->us_total_cnt)) {
        oal_spin_unlock_irq_restore(&mem_subpool->st_spinlock, &irq_flag);
        return HI_ERR_CODE_MEM_EXCEED_TOTAL_CNT;
    }
    mem->mem_state_flag = OAL_MEM_STATE_FREE;
    mem_subpool->free_stack[mem_subpool->us_free_cnt] = (hi_void *)mem;
    mem_subpool->us_free_cnt++;
    mem_pool->us_mem_used_cnt--;
    oal_spin_unlock_irq_restore(&mem_subpool->st_spinlock, &irq_flag);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取对应内存池占用的总字节数
 输入参数  : en_pool_id     : 内存池ID
 输出参数  : pul_total_bytes: 对应内存池占用的总字节数
**************************************************************************** */
hi_u32 oal_mem_get_total_bytes_in_pool(oal_mem_pool_id_enum_uint8 pool_id)
{
    hi_u32                      total_bytes = 0;         /* 本内存池总字节数 */
    oal_mem_subpool_cfg_stru    *mem_subpool_cfg = HI_NULL;
    hi_u16                      us_size;
    hi_u16                      us_cnt;
    hi_u8                       subpool_idx;
    hi_u8                       subpool_cnt;

    mem_subpool_cfg = g_ast_mem_pool_cfg_table[pool_id].subpool_cfg_info;
    subpool_cnt = g_ast_mem_pool_cfg_table[pool_id].subpool_cnt;

    for (subpool_idx = 0; subpool_idx < subpool_cnt; subpool_idx++) {
        us_size = mem_subpool_cfg[subpool_idx].us_size;
        us_cnt  = mem_subpool_cfg[subpool_idx].us_cnt;
        total_bytes += us_size * us_cnt;
    }
    return total_bytes;
}

/* ****************************************************************************
 功能描述  : 初始化全部内存池
 返 回 值  : HI_SUCCESS 或其它错误码
**************************************************************************** */
hi_u32 oal_mem_init_pool(hi_void)
{
    hi_u32    total_bytes;
    hi_u32    pool_id;
    hi_u32    ret;
    hi_u8    *puc_base_addr = HI_NULL;

    if (oal_mem_init_ctrl_blk() != HI_SUCCESS) {
        hi_diag_log_msg_e0(0, "oal_mem_init_pool, init ctrl blk fail!");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }
    for (pool_id = 0; pool_id < OAL_MEM_POOL_ID_BUTT; pool_id++) {
        total_bytes = oal_mem_get_total_bytes_in_pool((hi_u8)pool_id);
        puc_base_addr = (hi_u8 *)hi_malloc(HI_MOD_ID_WIFI_DRV, total_bytes);
        if (puc_base_addr == HI_NULL) {
            oal_mem_release();
            hi_diag_log_msg_e1(0, "oal_mem_init_pool, memory allocation fail, size=%d!", total_bytes);
            return HI_ERR_CODE_ALLOC_MEM_FAIL;
        }
        /* 记录每个内存池oal_malloc分配的地址 */
        g_pauc_pool_base_addr[pool_id] = puc_base_addr;
        puc_base_addr = (hi_u8 *)hi_byte_align((uintptr_t)puc_base_addr, 4); /* 4: 4bytes 对齐 */
        ret = oal_mem_create_pool((hi_u8)pool_id, puc_base_addr);
        if (ret != HI_SUCCESS) {
            oal_mem_release();
            hi_diag_log_msg_e0(0, "oal_mem_init_pool, oal_mem_create_pool failed!");
            return ret;
        }
    }
    return HI_SUCCESS;
}

void oal_mem_dump_cfg(void)
{
    oal_mem_pool_stru *mem_pool = HI_NULL;
    oal_mem_subpool_stru *mem_subpool = HI_NULL;
    int pool_id;
    int sub_id;

    oam_print("%8s %8s %8s %8s %8s %8s\r\n", "pool_id", "max_bytes", "sub_cnt", "used_cnt", "total_cnt", "start_addr");

    for (pool_id = 0; pool_id < OAL_MEM_POOL_ID_BUTT; pool_id++) {
        mem_pool = &g_ast_mem_pool[pool_id];

        oam_print("%8d %8u %8u %8u %8u %p\r\n", pool_id, mem_pool->us_max_byte_len, mem_pool->subpool_cnt,
            mem_pool->us_mem_used_cnt, mem_pool->us_mem_total_cnt, mem_pool->mem_start_addr);

        oam_print("\t\t%8s %8s %8s %8s\r\n", "sub_id", "length", "free_cnt", "total_cnt");

        for (sub_id = 0; sub_id < mem_pool->subpool_cnt; sub_id++) {
            mem_subpool = &g_ast_mem_pool[pool_id].ast_subpool_table[sub_id];
            oam_print("\t\t%8d %8u %8u %8u\r\n", sub_id, mem_subpool->us_len, mem_subpool->us_free_cnt,
                mem_subpool->us_total_cnt);
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
