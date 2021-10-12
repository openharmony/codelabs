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
#include "frw_event_sched.h"
#include "oam_ext_if.h"
#include "frw_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* ****************************************************************************
  3 函数原型声明
**************************************************************************** */
/* ****************************************************************************
  4 全局变量定义
**************************************************************************** */
/* ****************************************************************************
  5 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 重置调度队列上各个事件队列的权重计数器
 输入参数  : pst_sched_queue: 调度队列指针
**************************************************************************** */
hi_void frw_event_sched_reset_weight(frw_event_sched_queue_stru *sched_queue)
{
    hi_list *list = HI_NULL;
    frw_event_queue_stru *event_queue = HI_NULL;

    /* 遍历整个调度链表 */
    hi_list_for_each(list, &sched_queue->head) {
        /* 获取调度链表中的一个事件队列 */
        event_queue = hi_list_entry(list, frw_event_queue_stru, list);
        /* 只是重置恢复状态VAP的权重值 */
        if (event_queue->vap_state == FRW_VAP_STATE_RESUME) {
            /* 重置事件队列的权重计数器 */
            event_queue->weight_cnt = event_queue->weight;
            /* 更新调度队列上的总权重计数器 */
            sched_queue->total_weight_cnt += event_queue->weight;
        }
    }
}

/* ****************************************************************************
 功能描述  : 事件调度入口函数

 修改历史      :
  1.日    期   : 2012年10月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void *frw_event_schedule(frw_event_sched_queue_stru *sched_queue)
{
    hi_void                     *event = HI_NULL;
    frw_event_sched_queue_stru  *queue = HI_NULL;

    /* 遍历全部调度类 */
    queue = &sched_queue[FRW_SCHED_POLICY_HI];

    /* 如果高优先级调度队列为空，则取下一个调度队列 */
    if (!hi_is_list_empty(&queue->head)) {
        /* 从调度类中挑选下一个待处理的事件 */
        event = frw_event_sched_pick_next_event_queue_wrr(queue);
        if (event != HI_NULL) {
            return event;
        }
    }

    queue = &sched_queue[FRW_SCHED_POLICY_NORMAL];

    /* 如果普通优先级调度队列为空，则取下一个调度队列 */
    if (!hi_is_list_empty(&queue->head)) {
        /* 从调度类中挑选下一个待处理的事件 */
        event = frw_event_sched_pick_next_event_queue_wrr(queue);
        if (event != HI_NULL) {
            return event;
        }
    }

    return HI_NULL;
}
/* ****************************************************************************
 功能描述  : 从调度队列删除一个事件队列
 输入参数  : pst_sched_queue: 调度队列指针
             pst_event_queue: 事件队列指针

 修改历史      :
  1.日    期   : 2015年3月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数

*****************************************************************************/
hi_void frw_event_sched_deactivate_queue_no_lock(frw_event_sched_queue_stru  *sched_queue,
                                                 frw_event_queue_stru  *event_queue)
{
    if (event_queue->queue.element_cnt != 0) {
        return;
    }

    /* 更新调度队列上的总权重计数器 */
    sched_queue->total_weight_cnt -= event_queue->weight_cnt;
    /* 将事件队列的权重计数器清零 */
    event_queue->weight_cnt = 0;
    /* 将事件队列从调度链表上删除 */
    hi_list_delete(&event_queue->list);
    /* 将事件队列置为不活跃状态 */
    event_queue->state = FRW_EVENT_QUEUE_STATE_INACTIVE;
}

/* ****************************************************************************
 功能描述  : 向调度队列添加一个新的事件队列
 输入参数  : past_sched_queue: 调度队列指针
             pst_event_queue : 事件队列指针

 修改历史      :
  1.日    期   : 2012年11月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数

  2.日    期   : 2015年4月23日
    作    者   : HiSilicon
    修改内容   : 封装为不加锁接口，保持接口对称性，供加锁接口调用
*****************************************************************************/
hi_u32 frw_event_sched_activate_queue_no_lock(frw_event_sched_queue_stru   *sched_queue,
                                              frw_event_queue_stru         *event_queue)
{
    /* 如果事件队列已经在可执行队列上(处于激活状态)，则直接返回成功 */
    if (event_queue->state == FRW_EVENT_QUEUE_STATE_ACTIVE) {
        return HI_SUCCESS;
    }

    /* 置为事件队列的权重计数器 */
    event_queue->weight_cnt = event_queue->weight;
    /* 更新调度队列上的总权重计数器 */
    sched_queue->total_weight_cnt += event_queue->weight_cnt;

    /* 将事件队列加入调度链表的末尾 */
    hi_list_tail_insert(&event_queue->list, &sched_queue->head);

    /* 将事件队列置为激活状态 */
    event_queue->state = FRW_EVENT_QUEUE_STATE_ACTIVE;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 调度器初始化
 输入参数  : pst_sched_queue: 调度队列指针

 修改历史      :
  1.日    期   : 2012年11月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 frw_event_sched_init(frw_event_sched_queue_stru *sched_queue)
{
    /* 初始化锁 */
    /* 初始化调度队列总权重计数器 */
    sched_queue->total_weight_cnt = 0;

    /* 初始化调度链表头 */
    hi_list_init(&sched_queue->head);
    oal_spin_lock_init(&sched_queue->st_lock);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置事件队列参数
 输入参数  : pst_prio_queue: 事件队列指针
             us_weight     : 队列权重
             en_policy     : 队列调度策略
             en_state      : 事件队列状态
*****************************************************************************/
hi_void frw_event_queue_set(frw_event_queue_stru *event_queue, hi_u8 weight,
                            frw_sched_policy_enum_uint8 policy, frw_event_queue_state_enum_uint8 state)
{
    event_queue->weight     = weight;
    event_queue->weight_cnt = 0;
    event_queue->policy     = policy;
    event_queue->state      = state;
    event_queue->vap_state  = FRW_VAP_STATE_RESUME;
}

/* ****************************************************************************
 功能描述  : 队列初始化, uc_max_events必须是2的整数次幂
 输入参数  : pst_queue      : 队列指针
             uc_max_events: 最大元素个数
**************************************************************************** */
hi_u32 oal_queue_init(oal_queue_stru *queue, hi_u8 max_events)
{
    hi_u32 *pul_buf = HI_NULL;

    if (max_events == 0) {
        return HI_SUCCESS;
    } else {
        if (oal_unlikely(oal_is_not_pow_of_2(max_events))) {
            return HI_ERR_CODE_CONFIG_UNSUPPORT;
        }

        pul_buf = (hi_u32 *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, (hi_u16)(max_events * sizeof(hi_u32)));
        if (oal_unlikely(pul_buf == HI_NULL)) {
            return HI_ERR_CODE_ALLOC_MEM_FAIL;
        }
        /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
        memset_s(pul_buf, max_events * sizeof(hi_u32), 0, max_events * sizeof(hi_u32));
        oal_queue_set(queue, pul_buf, max_events);

        return HI_SUCCESS;
    }
}

/* ****************************************************************************
 功能描述  : 事件队列初始化
 输入参数  : pst_event_queue: 事件队列指针
             us_weight      : 队列权重
             en_policy      : 队列调度策略
             en_state       : 事件队列状态
             us_max_events  : 最大事件个数

 修改历史      :
  1.日    期   : 2012年10月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

*****************************************************************************/
hi_u32 frw_event_queue_init(frw_event_queue_stru *event_queue, hi_u8 weight,
                            frw_sched_policy_enum_uint8 policy,
                            frw_event_queue_state_enum_uint8 state, hi_u8 max_events)
{
    hi_u32 ret;

    /* 初始化锁 */
    oal_spin_lock_init(&event_queue->st_lock);
    ret = oal_queue_init(&event_queue->queue, max_events);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_FRW, "{frw_event_queue_init:: OAL_QUEUE_INIT return != HI_SUCCESS! %d}", ret);
        frw_event_queue_set(event_queue, 0, FRW_SCHED_POLICY_BUTT, FRW_EVENT_QUEUE_STATE_INACTIVE);

        return ret;
    }

    frw_event_queue_set(event_queue, weight, policy, state);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 销毁事件队列
 输入参数  : pst_event_queue: 事件队列指针

 修改历史      :
  1.日    期   : 2012年10月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void frw_event_queue_destroy(frw_event_queue_stru *event_queue)
{
    oal_queue_destroy(&event_queue->queue);

    frw_event_queue_set(event_queue, 0, FRW_SCHED_POLICY_BUTT, FRW_EVENT_QUEUE_STATE_INACTIVE);
}

/* ****************************************************************************
 功能描述  : 从调度类中挑选下一个待处理的事件

 修改历史      :
  1.日    期   : 2012年10月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void *frw_event_sched_pick_next_event_queue_wrr(frw_event_sched_queue_stru *sched_queue)
{
    hi_list                   *list = HI_NULL;
    frw_event_queue_stru      *event_queue = HI_NULL;
    hi_void                   *event = HI_NULL;
    unsigned long             flag;
    unsigned long             irq_flag;

    oal_spin_lock_irq_save(&sched_queue->st_lock, &flag);

    /* 遍历整个调度链表 */
    hi_list_for_each(list, &sched_queue->head) {
        event_queue = hi_list_entry(list, frw_event_queue_stru, list);
        oal_spin_lock_irq_save(&event_queue->st_lock, &irq_flag);
        /* 如果事件队列的vap_state为暂停，则跳过，继续挑选下一个事件队列 */
        if (event_queue->vap_state == FRW_VAP_STATE_PAUSE) {
            oal_spin_unlock_irq_restore(&event_queue->st_lock, &irq_flag);
            continue;
        }

        /* 如果事件队列的权重计数器为0，则挑选下一个事件队列 */
        if (event_queue->weight_cnt == 0) {
            oal_spin_unlock_irq_restore(&event_queue->st_lock, &irq_flag);
            continue;
        }

        /* 更新事件队列权重计数器 */
        event_queue->weight_cnt--;
        /* 更新调度队列的总权重计数器 */
        sched_queue->total_weight_cnt--;
        /* 从事件队列中取出一个事件 */
        event = (frw_event_mem_stru *)oal_queue_dequeue(&event_queue->queue);

        /* 如果事件队列变空，需要将其从调度队列上删除，并将事件队列状态置为不活跃(不可被调度) */
        frw_event_sched_deactivate_queue_no_lock(sched_queue, event_queue);
        /* } */
        oal_spin_unlock_irq_restore(&event_queue->st_lock, &irq_flag);
        break;
    }

    /* 如果调度队列的总权重计数器为0，则需要重置调度队列上各个事件队列的权重计数器 */
    if (sched_queue->total_weight_cnt == 0) {
        frw_event_sched_reset_weight(sched_queue);
    }

    oal_spin_unlock_irq_restore(&sched_queue->st_lock, &flag);
    return event;
}

/* ****************************************************************************
 功能描述  : 从调度队列删除一个事件队列
 输入参数  : pst_sched_queue: 调度队列指针
             pst_event_queue: 事件队列指针

 修改历史      :
  1.日    期   : 2012年11月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void frw_event_sched_deactivate_queue(frw_event_sched_queue_stru *sched_queue, frw_event_queue_stru *event_queue)
{
    unsigned long flag;

    /* 关中断，加锁 */
    oal_spin_lock_irq_save(&sched_queue->st_lock, &flag);

    frw_event_sched_deactivate_queue_no_lock(sched_queue, event_queue);

    /* 解锁，开中断 */
    oal_spin_unlock_irq_restore(&sched_queue->st_lock, &flag);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
