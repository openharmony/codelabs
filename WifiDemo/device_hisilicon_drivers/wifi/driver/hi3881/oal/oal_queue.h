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

#ifndef __OAL_DATA_STRU_H__
#define __OAL_DATA_STRU_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hi_types.h"
#include "oal_err_wifi.h"
#include "oal_mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define OAL_QUEUE_DESTROY

/* 判断x是否是2的整数幂 */
#define oal_is_not_pow_of_2(_x) ((_x) & ((_x)-1))

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
typedef struct {
    hi_u8 element_cnt;  /* 本队列中已经存放的元素个数 */
    hi_u8 max_elements; /* 本队列中所能存放的最大元素个数 */
    hi_u8 tail_index;   /* 指向下一个元素入队位置的索引 */
    hi_u8 head_index;   /* 指向当前元素出队位置的索引 */
    uintptr_t *pul_buf; /* 队列缓存 */
} oal_queue_stru;

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 设置队列参数
 输入参数  : pst_queue      : 队列指针
             pul_buf        : 指向队列缓冲区的指针
             us_max_elements: 最大元素个数
 输出参数  : 无
 返 回 值  : 无
**************************************************************************** */
static inline hi_void oal_queue_set(oal_queue_stru *queue, hi_u32 *pul_buf, hi_u8 max_elements)
{
    queue->pul_buf = (uintptr_t *)pul_buf;

    queue->tail_index  = 0;
    queue->head_index  = 0;
    queue->element_cnt = 0;
    queue->max_elements = max_elements;
}

/* ****************************************************************************
 功能描述  : 销毁队列
 输入参数  : pst_queue: 队列指针
 输出参数  : 无
 返 回 值  : 无
**************************************************************************** */
static inline hi_void oal_queue_destroy(oal_queue_stru *queue)
{
    if (queue == HI_NULL) {
        return;
    }

    if (queue->pul_buf == HI_NULL) {
        return;
    }

    oal_mem_free(queue->pul_buf);

    oal_queue_set(queue, HI_NULL, 0);
}

/* ****************************************************************************
 功能描述  : 元素入队
 输入参数  : pst_queue: 队列指针
             p_element: 元素指针
 输出参数  : 无
 返 回 值  : HI_SUCCESS 或其它错误码
**************************************************************************** */
static inline hi_u32 oal_queue_enqueue(oal_queue_stru *queue, hi_void *element)
{
    /* 异常: 队列已满 */
    if (queue->element_cnt == queue->max_elements) {
        return HI_FAIL;
    }

    hi_u8 tail_index = queue->tail_index;
    /* 将元素的地址保存在队列中 */
    queue->pul_buf[tail_index++] = (hi_u32)(uintptr_t)element;
    queue->tail_index = ((tail_index >= queue->max_elements) ? 0 : tail_index);
    queue->element_cnt++;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 元素出队
 输入参数  : pst_queue: 队列指针
 输出参数  : 无
 返 回 值  : 成功: 事件指针
             失败: HI_NULL
**************************************************************************** */
static inline hi_void *oal_queue_dequeue(oal_queue_stru *queue)
{
    /* 异常: 队列为空 */
    if (queue->element_cnt == 0) {
        return HI_NULL;
    }

    hi_u8 head_index = queue->head_index;

    hi_void *element = (hi_void *)queue->pul_buf[head_index++];

    queue->head_index = ((head_index >= queue->max_elements) ? 0 : head_index);
    queue->element_cnt--;

    return element;
}

/* ****************************************************************************
 功能描述  : 获取队列中已经存放的元素个数
 输入参数  : pst_queue: 队列指针
 输出参数  : 无
 返 回 值  : 队列中存放的事件个数
**************************************************************************** */
static inline hi_u8 oal_queue_get_length(const oal_queue_stru *queue)
{
    if (queue == HI_NULL) {
        return 0;
    }

    return queue->element_cnt;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_data_stru.h */
