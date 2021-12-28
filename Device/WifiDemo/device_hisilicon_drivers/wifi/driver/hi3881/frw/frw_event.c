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
#include "frw_task.h"
#include "frw_event.h"
#include "frw_main.h"
#include "oam_ext_if.h"
#include "exception_rst.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FRW_ENQUEUE_FAIL_LIMIT 10

hi_u32 g_app_event_id = 0;
hi_u32 g_frw_enqueue_fail_nums = 0;

/* ****************************************************************************
  2 STRUCT定义
**************************************************************************** */
/* ****************************************************************************
  结构名  : frw_event_mgmt_stru
  结构说明: 事件管理结构体
**************************************************************************** */
typedef struct _frw_event_mgmt_stru_ {
    frw_event_queue_stru       *event_queue;    /* 事件队列 */
    frw_event_sched_queue_stru sched_queue[FRW_SCHED_POLICY_BUTT];       /* 可调度队列 */
    hi_atomic                  total_element_cnt;
} frw_event_mgmt_stru;

/* *****************************************************************************
    事件管理实体
****************************************************************************** */
frw_event_mgmt_stru g_ast_event_manager;

/* *****************************************************************************
    事件表全局变量
****************************************************************************** */
frw_event_table_item_stru g_ast_event_table[FRW_EVENT_TABLE_MAX_ITEMS];

/* 事件队列初始化预留回调用于修改事件队列配置值 */
typedef hi_u32 (*frw_event_init_queue_cb)(hi_void);

/* ****************************************************************************
  4 函数实现
**************************************************************************** */
hi_u32 get_app_event_id(hi_void)
{
    return g_app_event_id;
}

/* ****************************************************************************
 功能描述  : 申请事件内存
 输入参数  : us_length: payload长度 + 事件头长度
 返 回 值  : 成功: 指向frw_event_mem_stru的指针
             失败: HI_NULL
 修改历史      :
  1.日    期   : 2012年10月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
frw_event_mem_stru *frw_event_alloc(hi_u16 us_payload_length)
{
    us_payload_length += OAL_MEM_INFO_SIZE;
    return oal_mem_alloc_enhanced(OAL_MEM_POOL_ID_EVENT, (us_payload_length + FRW_EVENT_HDR_LEN));
}

/* ****************************************************************************
 功能描述  : 释放事件所占用的内存
 输入参数  : event_mem: 指向事件内存块的指针
 返 回 值  : HI_SUCCESS 或其它错误码

 修改历史      :
  1.日    期   : 2012年10月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 frw_event_free(frw_event_mem_stru *event_mem)
{
    hi_u32 ret;
    frw_event_stru *frw_event = HI_NULL;
    hi_unref_param(frw_event);

    ret = oal_mem_free_enhanced(event_mem);
    if (oal_warn_on(ret != HI_SUCCESS)) {
        frw_event = (frw_event_stru *)event_mem->puc_data;
        hi_diag_log_msg_i3(0, "[E]frw event free failed!, ret:%d, type:%d, subtype:%d",
            ret, frw_event->event_hdr.type, frw_event->event_hdr.sub_type);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : 根据事件内容获取相应的事件队列ID
 输入参数  : event_mem: 指向事件内存块的指针
 输出参数  : pus_qid      : 队列ID
 返 回 值  : HI_SUCCESS 或其它错误码
**************************************************************************** */
hi_u32 frw_event_to_qid(const frw_event_mem_stru *event_mem, hi_u16 *pus_qid)
{
    hi_u16 us_qid;
    frw_event_hdr_stru *event_hrd = HI_NULL;

    /* 获取事件头结构 */
    event_hrd = (frw_event_hdr_stru *)event_mem->puc_data;

    us_qid = event_hrd->vap_id * FRW_EVENT_TYPE_BUTT + event_hrd->type;
    /* 异常: 队列ID超过最大值 */
    if ((us_qid >= FRW_EVENT_MAX_NUM_QUEUES)) {
        oam_error_log4(0, OAM_SF_FRW,
            "{frw_event_to_qid, array overflow! us_qid[%d], vap_id[%d], en_type[%d],sub_type[%d]}",
            us_qid, event_hrd->vap_id, event_hrd->type, event_hrd->sub_type);
        return HI_ERR_CODE_ARRAY_OVERFLOW;
    }

    *pus_qid = us_qid;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 初始化事件队列
 修改历史      :
  1.日    期   : 2012年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 frw_event_init_event_queue(hi_void)
{
    hi_u8  vap_res_num = oal_mem_get_vap_res_num();
    hi_u16 us_total_cnt = vap_res_num * FRW_EVENT_TYPE_BUTT; /* 逻辑保证不会溢出 */
    hi_u16 us_qid;
    hi_u32 ret;
    frw_event_cfg_stru ast_event_cfg_vap[] = WLAN_FRW_EVENT_CFG_TABLE; /* 事件初始值,最大资源值4vap */
    /* 钩子函数，误包告警 -g- lin_t !e611 */
    frw_event_init_queue_cb func_cb = (frw_event_init_queue_cb)frw_get_rom_resv_func(FRW_ROM_RESV_FUNC_QUEUE_INIT);
    if (func_cb != HI_NULL) {
        /* 预留回调非空 原有代码不再需要 */
        return func_cb();
    }
    /* 根据支持的vap数量申请配置指针内存 */
    g_ast_event_manager.event_queue =
        (frw_event_queue_stru *)hi_malloc(HI_MOD_ID_WIFI_DRV, us_total_cnt * sizeof(frw_event_queue_stru));
    if (g_ast_event_manager.event_queue == HI_NULL) {
        hi_diag_log_msg_e0(0, "{frw_event_init_event_queue, hi_malloc event queue null.}");
        return HI_FAIL;
    }
    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s((hi_void *)g_ast_event_manager.event_queue, us_total_cnt * sizeof(frw_event_queue_stru), 0,
        us_total_cnt * sizeof(frw_event_queue_stru));
    /* 循环初始化事件队列 */
    for (us_qid = 0; us_qid < us_total_cnt; us_qid++) {
        ret = frw_event_queue_init(&g_ast_event_manager.event_queue[us_qid], ast_event_cfg_vap[us_qid].weight,
            ast_event_cfg_vap[us_qid].policy, FRW_EVENT_QUEUE_STATE_INACTIVE, ast_event_cfg_vap[us_qid].max_events);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            hi_free(HI_MOD_ID_WIFI_DRV, g_ast_event_manager.event_queue);
            g_ast_event_manager.event_queue = HI_NULL;
            hi_diag_log_msg_e0(0, "{frw_event_init_event_queue, frw_event_queue_init failed.}");
            return ret;
        }
    }

    return HI_SUCCESS;
}

const frw_event_sub_table_item_stru* frw_get_event_sub_table(hi_u8 type, hi_u8 pipeline)
{
    frw_event_table_item_stru *frw_event_table = HI_NULL;
    hi_u8 index;

    /* 根据事件类型及分段号计算事件表索引 */
    index = (hi_u8)((type << 1) | (pipeline & 0x01));
    frw_event_table = &g_ast_event_table[index];

    return frw_event_table->sub_table;
}

/* ****************************************************************************
 功能描述  : 初始化调度器
**************************************************************************** */
hi_u32 frw_event_init_sched(hi_void)
{
    hi_u16 us_qid;
    hi_u32 ret;

    /* 循环初始化调度器 */
    for (us_qid = 0; us_qid < FRW_SCHED_POLICY_BUTT; us_qid++) {
        ret = frw_event_sched_init(&g_ast_event_manager.sched_queue[us_qid]);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            oam_warning_log1(0, OAM_SF_FRW, "{frw_event_init_sched, frw_event_sched_init return != HI_SUCCESS!%d}",
                ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 事件分发接口(分发事件至核间通讯、事件队列、或者查表寻找相应事件处理函数)
**************************************************************************** */
hi_u32 frw_event_dispatch_event(frw_event_mem_stru *event_mem)
{
    frw_event_hdr_stru *event_hrd = HI_NULL;
#if defined(_PRE_MEM_DEBUG_MODE) || defined(_PRE_DEBUG_MODE)
    hi_u32 dog_tag;
#endif

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_event_dispatch_event: event_mem is null ptr!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 异常: 内存写越界 */
#if defined(_PRE_MEM_DEBUG_MODE) || defined(_PRE_DEBUG_MODE)
    dog_tag = (*((hi_u32 *)(event_mem->puc_origin_data + event_mem->us_len - OAL_DOG_TAG_SIZE)));
    if (dog_tag != OAL_DOG_TAG) {
        hi_diag_log_msg_i2(0, "[line = %d], frw_event_dispatch_event, ul_dog_tag changed is [%d]\r\n", __LINE__,
            dog_tag);
        return HI_ERR_CODE_MEM_DOG_TAG;
    }
#endif

    /* 获取事件头结构 */
    event_hrd = (frw_event_hdr_stru *)event_mem->puc_data;
    if (oal_unlikely(event_hrd->pipeline >= FRW_EVENT_PIPELINE_STAGE_BUTT)) {
        return HI_ERR_CODE_ARRAY_OVERFLOW;
    }

    /* 如果pipleline为0，则将事件入队。否则，
       根据事件类型，子类型以及分段号，执行相应的事件处理函数 */
    if (event_hrd->pipeline == FRW_EVENT_PIPELINE_STAGE_0) {
        return frw_event_post_event(event_mem);
    }

    return frw_event_lookup_process_entry(event_mem, event_hrd);
}

/* ****************************************************************************
 功能描述  : 事件管理模块初始化总入口
 返 回 值  : HI_SUCCESS 或其它错误码

 修改历史      :
  1.日    期   : 2012年10月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 frw_event_init(hi_void)
{
    hi_u32 ret;
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hi_event_init(4, HI_NULL); /* 4:设置最大事件数为4 */
    hi_event_create(&g_app_event_id);
#endif
    ret = frw_event_init_event_queue(); /* 初始化事件队列 */
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_FRW, "{frw_event_init, frw_event_init_event_queue != HI_SUCCESS!%d}", ret);
        return ret;
    }
    /* 初始化调度器 */
    ret = frw_event_init_sched();
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_FRW, "frw_event_init, frw_event_init_sched != HI_SUCCESS!%d", ret);
        return ret;
    }
    frw_task_event_handler_register(frw_event_process_all_event);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 销毁事件队列
**************************************************************************** */
hi_void frw_event_destroy_event_queue(hi_void)
{
    hi_u8 vap_res_num;
    hi_u16 us_total_cnt;
    hi_u16 us_qid;

    if (g_ast_event_manager.event_queue == HI_NULL) {
        return;
    }
    vap_res_num = oal_mem_get_vap_res_num();
    us_total_cnt = vap_res_num * FRW_EVENT_TYPE_BUTT; /* 逻辑保证不会溢出 */
    /* 循环销毁事件队列 */
    for (us_qid = 0; us_qid < us_total_cnt; us_qid++) {
        frw_event_queue_destroy(&g_ast_event_manager.event_queue[us_qid]);
    }
    /* 释放事件队列内存 */
    hi_free(HI_MOD_ID_WIFI_DRV, g_ast_event_manager.event_queue);
    g_ast_event_manager.event_queue = HI_NULL;
}

/* ****************************************************************************
 功能描述  : 事件管理模块卸载接口
 修改历史      :
  1.日    期   : 2012年10月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void frw_event_exit(hi_void)
{
    /* 销毁事件队列 */
    frw_event_destroy_event_queue();
}

/* ****************************************************************************
 功能描述  : 将事件内存放入相应的事件队列
 输入参数  : event_mem: 指向事件内存块的指针
 返 回 值  : HI_SUCCESS 或其它错误码

 修改历史      :
  1.日    期   : 2015年4月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 frw_event_queue_enqueue(frw_event_queue_stru *event_queue, frw_event_mem_stru *event_mem)
{
    return oal_queue_enqueue(&event_queue->queue, (hi_void *)event_mem);
}

/* ****************************************************************************
 功能描述  : 注册相应事件对应的事件处理函数
 输入参数  : en_type:       事件类型
             en_pipeline:   事件分段号
             pst_sub_table: 事件子表指针
 修改历史      :
  1.日    期   : 2012年10月12日
    作    者   : HiSilicon
**************************************************************************** */
hi_void frw_event_table_register(frw_event_type_enum_uint8 type, frw_event_pipeline_enum_uint8 pipeline,
    const frw_event_sub_table_item_stru *sub_table)
{
    hi_u8 index;

    if (oal_unlikely(sub_table == HI_NULL)) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_event_table_register: pst_sub_table is null ptr!}");
        return;
    }
    /* 根据事件类型及分段号计算事件表索引 */
    index = (hi_u8)((type << 1) | (pipeline & 0x01));
    if (oal_unlikely(index >= FRW_EVENT_TABLE_MAX_ITEMS)) {
        oam_error_log1(0, OAM_SF_FRW, "{frw_event_table_register, array overflow! %d}", index);
        return;
    }
    g_ast_event_table[index].sub_table = sub_table;
}

/* ****************************************************************************
 功能描述  : 清空某个事件队列中的所有事件
 返 回 值  : HI_SUCCESS 或其它错误码

 修改历史      :
  1.日    期   : 2013年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 frw_event_flush_event_queue(frw_event_type_enum_uint8 event_type)
{
    frw_event_queue_stru *event_queue = HI_NULL;
    frw_event_mem_stru   *event_mem = HI_NULL;
    frw_event_hdr_stru   *event_hrd = HI_NULL;
    hi_u32               event_succ = 0;
    hi_u32               ret;
    hi_u16               us_qid;
    hi_u8                vap_id;
    hi_u8                vap_res_num = oal_mem_get_vap_res_num();

    if (g_ast_event_manager.event_queue == HI_NULL) {
        hi_diag_log_msg_e0(0, "{frw_event_flush_event_queue, event queue null.}");
        return event_succ;
    }
    /* 遍历每个核的每个vap对应的事件队列 */
    for (vap_id = 0; vap_id < vap_res_num; vap_id++) {
        us_qid = vap_id * FRW_EVENT_TYPE_BUTT + event_type;
        /* 根据核号 + 队列ID，找到相应的事件队列 */
        event_queue = &(g_ast_event_manager.event_queue[us_qid]);
        /* flush所有的event */
        while (event_queue->queue.element_cnt != 0) {
            event_mem = (frw_event_mem_stru *)frw_event_queue_dequeue(event_queue);
            if (event_mem == HI_NULL) {
                return event_succ;
            }
            hi_atomic_dec(&(g_ast_event_manager.total_element_cnt));

            /* 获取事件头结构 */
            event_hrd = (frw_event_hdr_stru *)event_mem->puc_data;

            /* 根据事件找到对应的事件处理函数 */
            ret = frw_event_lookup_process_entry(event_mem, event_hrd);
            if (ret != HI_SUCCESS) {
                oam_error_log1(0, OAM_SF_FRW,
                    "{frw_event_process_all_event: frw_event_lookup_process_entry return value :%d}", ret);
            }

            /* 释放事件内存 */
            frw_event_free(event_mem);

            event_succ++;
        }

        /* 如果事件队列变空，需要将其从调度队列上删除，并将事件队列状态置为不活跃(不可被调度) */
        if (event_queue->queue.element_cnt == 0) {
            frw_event_sched_deactivate_queue(&g_ast_event_manager.sched_queue[event_queue->policy], event_queue);
        }
    }

    return event_succ;
}

/* ****************************************************************************
 功能描述  : 冲刷指定VAP、指定事件类型的所有事件，同时可以指定是丢弃这些事件还是全部处理
 输入参数  : uc_vap_id:     VAP ID值
             en_event_type: 事件类型
             en_drop:       事件丢弃(1)或者处理(0)

 修改历史      :
  1.日    期   : 2013年12月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 frw_event_vap_flush_event(hi_u8 vap_id, frw_event_type_enum_uint8 event_type, hi_u8 drop)
{
    hi_u16              us_qid;
    hi_u32              ret;
    frw_event_queue_stru   *event_queue = HI_NULL;
    frw_event_mem_stru     *event_mem = HI_NULL;
    frw_event_hdr_stru     *event_hrd = HI_NULL;

    if (event_type == FRW_EVENT_TYPE_WLAN_TX_COMP) {
        vap_id = 0;
    }

    if (g_ast_event_manager.event_queue == HI_NULL) {
        hi_diag_log_msg_e0(0, "{frw_event_flush_event_queue, event queue null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    us_qid = vap_id * FRW_EVENT_TYPE_BUTT + event_type;
    /* 根据核号 + 队列ID，找到相应的事件队列 */
    event_queue = &(g_ast_event_manager.event_queue[us_qid]);
    /* 如果事件队列本身为空，没有事件，不在调度队列，返回错误 */
    if (event_queue->queue.element_cnt == 0) {
        return HI_FAIL;
    }

    /* flush所有的event */
    while (event_queue->queue.element_cnt != 0) {
        event_mem = (frw_event_mem_stru *)frw_event_queue_dequeue(event_queue);
        if (event_mem == HI_NULL) {
            return HI_FAIL;
        }
        hi_atomic_dec(&(g_ast_event_manager.total_element_cnt));

        /* 处理事件，否则直接释放事件内存而丢弃事件 */
        if (drop == 0) {
            /* 获取事件头结构 */
            event_hrd = (frw_event_hdr_stru *)event_mem->puc_data;
            /* 根据事件找到对应的事件处理函数 */
            ret = frw_event_lookup_process_entry(event_mem, event_hrd);
            if (ret != HI_SUCCESS) {
                oam_warning_log0(vap_id, OAM_SF_FRW, "frw_event_lookup_process_entry return NON SUCCESS. ");
            }
        }

        /* 释放事件内存 */
        frw_event_free(event_mem);
    }

    /* 若事件队列已经变空，需要将其从调度队列上删除，并将事件队列状态置为不活跃(不可被调度) */
    if (event_queue->queue.element_cnt == 0) {
        frw_event_sched_deactivate_queue(&g_ast_event_manager.sched_queue[event_queue->policy], event_queue);
    } else {
        oam_error_log1(vap_id, OAM_SF_FRW, "{flush vap event failed, left!=0: type=%d}", event_type);
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 根据核id和事件类型，判断vap事件队列是否空
 输入参数  : event_type:  事件ID;
 修改历史      :
  1.日    期   : 2015年4月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 frw_is_vap_event_queue_empty(hi_u8 vap_id, hi_u8 event_type)
{
    frw_event_queue_stru *event_queue = HI_NULL;
    hi_u16               us_qid;

    us_qid = (hi_u16)(vap_id * FRW_EVENT_TYPE_BUTT + event_type);
    /* 根据核号 + 队列ID，找到相应的事件队列 */
    if (g_ast_event_manager.event_queue == HI_NULL) {
        hi_diag_log_msg_e0(0, "{frw_event_flush_event_queue, event queue null.}");
        return HI_TRUE;
    }
    event_queue = &(g_ast_event_manager.event_queue[us_qid]);
    if (event_queue->queue.element_cnt != 0) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

/* ****************************************************************************
 功能描述  : 判断是否有事件需要调度

 修改历史      :
  1.日    期   : 2015年4月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u8 frw_task_thread_condition_check(hi_void)
{
    return (hi_atomic_read(&g_ast_event_manager.total_element_cnt) != 0);
}

/* ****************************************************************************
 功能描述  : 处理事件队列中的所有事件
 patch修改 : FRW处理事件时先cnt递减
**************************************************************************** */
hi_void frw_event_process_all_event(hi_void)
{
    frw_event_mem_stru            *event_mem = HI_NULL;
    frw_event_sched_queue_stru    *sched_queue = HI_NULL;
    frw_event_hdr_stru            *event_hrd = HI_NULL;

    /* 获取核号 */
    sched_queue = g_ast_event_manager.sched_queue;
    /* 调用事件调度模块，选择一个事件 */
    event_mem = (frw_event_mem_stru *)frw_event_schedule(sched_queue);
    while (event_mem != HI_NULL) {
        hi_atomic_dec(&g_ast_event_manager.total_element_cnt);
        /* 获取事件头结构 */
        event_hrd = (frw_event_hdr_stru *)event_mem->puc_data;
        if (event_hrd != HI_NULL) {
            /* 根据事件找到对应的事件处理函数 */
            if (frw_event_lookup_process_entry(event_mem, event_hrd) != HI_SUCCESS) {
                oam_warning_log0(0, OAM_SF_FRW, "frw_event_process_all_event_patch return NON SUCCESS.");
            }
        }
        /* 释放事件内存 */
        frw_event_free(event_mem);
        /* 调用事件调度模块，选择一个事件 */
        event_mem = (frw_event_mem_stru *)frw_event_schedule(sched_queue);
    }
}

/* ****************************************************************************
 功能描述  : 事件内存出队
 输入参数  : pst_event_queue: 事件队列
 返 回 值  : HI_SUCCESS 或其它错误码

 修改历史      :
  1.日    期   : 2015年4月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
frw_event_mem_stru *frw_event_queue_dequeue(frw_event_queue_stru *event_queue)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    unsigned long irq_flag;

    oal_spin_lock_irq_save(&event_queue->st_lock, &irq_flag);
    event_mem = (frw_event_mem_stru *)oal_queue_dequeue(&event_queue->queue);
    oal_spin_unlock_irq_restore(&event_queue->st_lock, &irq_flag);
    return event_mem;
}

/* ****************************************************************************
 功能描述  : 将事件内存放入相应的事件队列
 输入参数  : event_mem: 指向事件内存块的指针
 返 回 值  : HI_SUCCESS 或其它错误码
**************************************************************************** */
hi_u32 frw_event_post_event(frw_event_mem_stru *event_mem)
{
    hi_u16                     us_qid;
    frw_event_queue_stru       *event_queue = HI_NULL;
    hi_u32                     ret;
    unsigned long              irq_flag;
    unsigned long              sched_flag;
    frw_event_hdr_stru         *event_hdr = HI_NULL;
    frw_event_sched_queue_stru *sched_queue = HI_NULL;

    /* 获取事件队列ID */
    ret = frw_event_to_qid(event_mem, &us_qid);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_FRW, "{frw_event_post_event, frw_event_to_qid return != HI_SUCCESS!%d}", ret);
        return ret;
    }
    /* 根据核号 + 队列ID，找到相应的事件队列 */
    if (g_ast_event_manager.event_queue == HI_NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_event_post_event, event queue null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    event_queue = &(g_ast_event_manager.event_queue[us_qid]);
    /* 检查policy */
    if (oal_unlikely(event_queue->policy >= FRW_SCHED_POLICY_BUTT)) {
        oam_error_log1(0, OAM_SF_FRW, "{frw_event_post_event, array overflow!%d}", event_queue->policy);
        return HI_ERR_CODE_ARRAY_OVERFLOW;
    }
    /* 获取调度队列 */
    sched_queue = &(g_ast_event_manager.sched_queue[event_queue->policy]);

    /* 先取得引用，防止enqueue与取得引用之间被释放 */
    event_mem->user_cnt++;
    oal_spin_lock_irq_save(&sched_queue->st_lock, &sched_flag);
    /* 事件入队 */
    oal_spin_lock_irq_save(&event_queue->st_lock, &irq_flag);
    ret = frw_event_queue_enqueue(event_queue, event_mem);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oal_spin_unlock_irq_restore(&event_queue->st_lock, &irq_flag);
        oal_spin_unlock_irq_restore(&sched_queue->st_lock, &sched_flag);
        event_hdr = (frw_event_hdr_stru *)(event_mem->puc_data);
        oam_error_log4(0, OAM_SF_FRW,
            "frw_event_post_event:: enqueue fail, type:%d, sub type:%d, pipeline:%d,max num:%d", event_hdr->type,
            event_hdr->sub_type, event_hdr->pipeline, event_queue->queue.max_elements);
        g_frw_enqueue_fail_nums++;
        /* 释放事件内存引用 */
        frw_event_free(event_mem);
        if (g_frw_enqueue_fail_nums > FRW_ENQUEUE_FAIL_LIMIT) {
            oal_frw_exception_report();
        }
        return ret;
    }
    g_frw_enqueue_fail_nums = 0;
    hi_atomic_inc(&(g_ast_event_manager.total_element_cnt));
    /* 根据所属调度策略，将事件队列加入可调度队列 */
    ret = frw_event_sched_activate_queue_no_lock(sched_queue, event_queue);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oal_spin_unlock_irq_restore(&event_queue->st_lock, &irq_flag);
        oal_spin_unlock_irq_restore(&sched_queue->st_lock, &sched_flag);
        oam_error_log0(0, OAM_SF_FRW, "{frw_event_post_event, sched_activate_queue failed!}");
        return ret;
    }
    oal_spin_unlock_irq_restore(&event_queue->st_lock, &irq_flag);
    oal_spin_unlock_irq_restore(&sched_queue->st_lock, &sched_flag);
    frw_task_sched();

    return HI_SUCCESS;
}

hi_void frw_event_sub_rx_adapt_table_init(frw_event_sub_table_item_stru *pst_sub_table, hi_u32 ul_table_nums,
    frw_event_mem_stru *(*p_rx_adapt_func)(frw_event_mem_stru *))
{
    hi_u32 i;
    frw_event_sub_table_item_stru *pst_curr_table = HI_NULL;
    for (i = 0; i < ul_table_nums; i++) {
        pst_curr_table = pst_sub_table + i;
        pst_curr_table->p_rx_adapt_func = p_rx_adapt_func;
    }
}

/* ****************************************************************************
 功能描述  : 根据事件类型，子类型以及分段号，找到相应事件处理函数
 输入参数  : event_mem: 指向事件内存块的指针
 返 回 值  : HI_SUCCESS 或其它错误码

 修改历史      :
  1.日    期   : 2012年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 frw_event_lookup_process_entry(frw_event_mem_stru *event_mem, const frw_event_hdr_stru *event_hrd)
{
    frw_event_table_item_stru *frw_event_table = HI_NULL;
    hi_u8                     index;
    hi_u8                     sub_type;
    frw_event_mem_stru       *rx_adapt_event_mem = HI_NULL;
    hi_u32                    err_code;

    sub_type = event_hrd->sub_type;

    /* 根据事件类型及分段号计算事件表索引 */
    index = (hi_u8)((event_hrd->type << 1) | (event_hrd->pipeline & 0x01));
    if (oal_unlikely(index >= FRW_EVENT_TABLE_MAX_ITEMS)) {
        hi_diag_log_msg_e3(0, "{frw_event_lookup_process_entry::array overflow! type[%d], sub_type[%d], pipeline[%d]}",
            event_hrd->type, sub_type, event_hrd->pipeline);
        return HI_ERR_CODE_ARRAY_OVERFLOW;
    }

    /* 先把全局变量变成局部变量 */
    frw_event_table = &g_ast_event_table[index];
    if (frw_event_table->sub_table == HI_NULL) {
        hi_diag_log_msg_e2(0, "{frw_event_lookup_process_entry::pst_sub_table is NULL! sub_type[%d], index[%d].}",
            sub_type, index);
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 直接函数调用 */
    if (frw_event_table->sub_table[sub_type].func == HI_NULL) {
        hi_diag_log_msg_e2(0, "{frw_event_lookup_process_entry:: p_func is NULL! sub_type[%d], index[%d].}", sub_type,
            index);
        return HI_ERR_CODE_PTR_NULL;
    }

    if (event_hrd->pipeline == 0) {
        if (frw_event_table->sub_table[sub_type].func != HI_NULL) {
            err_code = frw_event_table->sub_table[sub_type].func(event_mem);
            return err_code;
        } else {
            oam_error_log2(0, OAM_SF_FRW,
                "{frw_event_lookup_process_entry:: func is NULL! sub_type[%d], index[0x%x], pipeline=0.}", sub_type,
                index);
            return HI_ERR_CODE_PTR_NULL;
        }
    }

    /* For rx adapt */
    if (frw_event_table->sub_table[sub_type].p_rx_adapt_func == HI_NULL) {
        oam_warning_log2(0, 0, "frw_event_lookup_process_entry:: rx_adapt_func is NULL, type[%d], sub_type[%d]",
            event_hrd->type, sub_type);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* rx adapt first */
    rx_adapt_event_mem = frw_event_table->sub_table[sub_type].p_rx_adapt_func(event_mem);
    if (rx_adapt_event_mem == HI_NULL) {
        oam_error_log0(0, 0, "frw_event_lookup_process_entry:: rx_adapt_event_mem NULL");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (frw_event_table->sub_table[sub_type].func != HI_NULL) {
        /* then call action frame */
        err_code = frw_event_table->sub_table[sub_type].func(rx_adapt_event_mem);
    } else {
        err_code = HI_ERR_CODE_PTR_NULL;
    }

    frw_event_free(rx_adapt_event_mem);

    return err_code;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
