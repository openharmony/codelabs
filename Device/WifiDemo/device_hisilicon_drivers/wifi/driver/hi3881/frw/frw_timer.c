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
#include "frw_timer.h"
#include "frw_main.h"
#ifdef _PRE_HDF_LINUX
#include "hdf_dlist.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
hi_list                     g_ast_timer_list;
oal_spin_lock_stru          g_ast_timer_list_spinlock;
hi_u32                      g_timer_id = 0;

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* 功能描述:FRW定时器超时处理事件 */
hi_void frw_timer_timeout_event(uintptr_t data)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru     *event = HI_NULL;
    frw_timeout_stru   *timeout = HI_NULL;
    hi_u32              ret;

    timeout = (frw_timeout_stru *)data;
    event_mem = frw_event_alloc(sizeof(hi_u32));
    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_timer_timeout_event:: event_mem == HI_NULL}");
        return;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    frw_field_setup((&event->event_hdr), type, (FRW_EVENT_TYPE_TIMEOUT));
    frw_field_setup((&event->event_hdr), sub_type, (FRW_TIMEOUT_TIMER_EVENT));
    frw_field_setup((&event->event_hdr), us_length, (WLAN_MEM_EVENT_SIZE1));
    frw_field_setup((&event->event_hdr), pipeline, (FRW_EVENT_PIPELINE_STAGE_0));
    frw_field_setup((&event->event_hdr), vap_id, (0));

    *(hi_u32 *)event->auc_event_data = timeout->timer_id;

    ret = frw_event_dispatch_event(event_mem);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_FRW, "{frw_timer_timeout_event::frw_event_dispatch_event failed[%d].}", ret);
    }

    /* 释放事件 */
    frw_event_free(event_mem);
}

/*
 * 功能描述:FRW定时器超时处理函数
 */ /* 是全局变量g_ast_frw_timeout_event_sub_table引用,不用const修饰,lin_t e801告警屏蔽，lin_t e818告警屏蔽 */
hi_u32 frw_timer_timeout_proc(frw_event_mem_stru *event_mem)
{
    hi_list            *timeout_entry = HI_NULL;
    frw_event_stru     *event = HI_NULL;
    frw_timeout_stru   *timeout_element = HI_NULL;
    hi_u32              timer_id;

    event = (frw_event_stru *)event_mem->puc_data;
    timer_id = *(hi_u32 *)event->auc_event_data;

    timeout_entry = g_ast_timer_list.next;
    while (timeout_entry != &g_ast_timer_list) {
        if (timeout_entry == HI_NULL) {
            oam_warning_log0(0, OAM_SF_FRW, "{frw_timer_timeout_proc:: pst_timeout_entry is null! }");
            break;
        }

        timeout_element = hi_list_entry(timeout_entry, frw_timeout_stru, entry);
        /* 如果该定时器没有使能或者待删除，则直接看下一个 */
        if ((timeout_element->is_deleting == HI_TRUE) ||
            (timeout_element->is_enabled == HI_FALSE)) {
            timeout_entry = timeout_entry->next;
            continue;
        }

        if ((timeout_element->timer_id == timer_id) &&
            (timeout_element->func != HI_NULL)) {
            timeout_element->func(timeout_element->timeout_arg);
            break;
        }
        timeout_entry = timeout_entry->next;
    }
    return HI_SUCCESS;
}

/*
 * 功能描述:删除所有定时器
 */
hi_void frw_timer_delete_all_timer(hi_void)
{
    /* 无用函数，待上层统一删除 */
}

const frw_event_sub_table_item_stru g_ast_frw_timeout_event_sub_table[FRW_TIMEOUT_SUB_TYPE_BUTT] = {
    { frw_timer_timeout_proc, HI_NULL, HI_NULL } /* FRW_TIMEOUT_TIMER_EVENT */
};

/* 功能描述:FRW定时器初始化 */
hi_void frw_timer_init(hi_void)
{
    oal_spin_lock_init(&g_ast_timer_list_spinlock);
    hi_list_init(&g_ast_timer_list);
    frw_event_table_register(FRW_EVENT_TYPE_TIMEOUT, FRW_EVENT_PIPELINE_STAGE_0, g_ast_frw_timeout_event_sub_table);
}

/*
 * 功能描述:添加定时器
 */
hi_void frw_timer_add_timer(frw_timeout_stru *timeout)
{
    if (timeout == HI_NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_timer_add_timer:: pst_timeout == HI_NULL}");
        return;
    }
    hi_list_tail_insert(&timeout->entry, &g_ast_timer_list);
}

hi_void frw_timer_create_timer(frw_timeout_stru *timeout, frw_timeout_func timeout_func,
    hi_u32 timeoutval, hi_void *timeout_arg, hi_u8  is_periodic)
{
    if (timeout == HI_NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_timer_create_timer:: HI_NULL == pst_timeout}");
        return;
    }

    oal_spin_lock_bh(&g_ast_timer_list_spinlock);

    timeout->func = timeout_func;
    timeout->timeout_arg = timeout_arg;
    timeout->timeout     = timeoutval;
    timeout->is_periodic = is_periodic;
    timeout->is_enabled  = HI_TRUE; /* 默认使能 */
    timeout->is_deleting = HI_FALSE;

    if (timeout->is_registerd != HI_TRUE) {
        timeout->timer_id = g_timer_id++; /* timer id用于标识定时器的唯一性 */
        oal_timer_init(&timeout->timer, timeoutval, frw_timer_timeout_proc_event, (unsigned long)timeout->timer_id);
        oal_timer_add(&timeout->timer);
    } else {
        oal_timer_start(&timeout->timer, (unsigned long)timeout->timeout);
    }

    if (timeout->is_registerd != HI_TRUE) {
        timeout->is_running = HI_FALSE;
        timeout->is_registerd = HI_TRUE;
        frw_timer_add_timer(timeout);
    }
    oal_spin_unlock_bh(&g_ast_timer_list_spinlock);
    return;
}

#ifdef _PRE_HDF_LINUX
hi_void frw_timer_timeout_proc_event(oal_timer_list_stru *arg)
{
    frw_timeout_stru *timeout = NULL;
#else
hi_void frw_timer_timeout_proc_event(unsigned long arg)
{
#endif
    frw_event_mem_stru *event_mem;
    frw_event_stru     *event = HI_NULL;

    event_mem = frw_event_alloc(sizeof(frw_event_stru));
    /* 返回值检查 */
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_timer_timeout_proc_event:: FRW_EVENT_ALLOC failed!}");
        return;
    }
    event = (frw_event_stru *)event_mem->puc_data;
    /* 填充事件头 */
    frw_field_setup((&event->event_hdr), type, (FRW_EVENT_TYPE_TIMEOUT));
    frw_field_setup((&event->event_hdr), sub_type, (FRW_TIMEOUT_TIMER_EVENT));
    frw_field_setup((&event->event_hdr), us_length, (WLAN_MEM_EVENT_SIZE1));
    frw_field_setup((&event->event_hdr), pipeline, (FRW_EVENT_PIPELINE_STAGE_0));
    frw_field_setup((&event->event_hdr), vap_id, (0));
#ifdef _PRE_HDF_LINUX
    timeout = CONTAINER_OF(arg, frw_timeout_stru, timer);
    *(hi_u32 *)event->auc_event_data = timeout->timer_id;
#else
    *(hi_u32 *)event->auc_event_data = (hi_u32)arg;
#endif

    /* 抛事件 */
    frw_event_dispatch_event(event_mem);
    frw_event_free(event_mem);
}

hi_void frw_timer_immediate_destroy_timer(frw_timeout_stru *timeout)
{
    if (timeout == HI_NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_timer_immediate_destroy_timer:: HI_NULL == pst_timeout}");
        return;
    }

    /* 如果定时器未注册，则直接返回 */
    if (timeout->is_registerd == HI_FALSE) {
        return;
    }
    timeout->is_enabled   = HI_FALSE;
    timeout->is_registerd = HI_FALSE;
    timeout->is_deleting  = HI_FALSE;
    hi_s32 ret = oal_timer_delete(&timeout->timer);
    if (ret != 0 && ret != 1) { /* 非激活计时器的del_timer()返回0,激活计时器返回1 */
        oam_error_log1(0, OAM_SF_FRW, "{frw_timer_immediate_destroy_timer:: fail ret = %d}", ret);
    }

    oal_spin_lock_bh(&g_ast_timer_list_spinlock);
    hi_list_delete(&timeout->entry);
    oal_spin_unlock_bh(&g_ast_timer_list_spinlock);
}

hi_void frw_timer_restart_timer(frw_timeout_stru *timeout, hi_u32 timeoutval, hi_u8 is_periodic)
{
    if (timeout == HI_NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_timer_restart_timer:: HI_NULL == pst_timeout}");
        return;
    }

    if (timeout->is_registerd == HI_FALSE) {
        return;
    }
    timeout->timeout     = timeoutval;
    timeout->is_enabled  = HI_TRUE;
    timeout->is_periodic = is_periodic;
    timeout->is_deleting = HI_FALSE;
    hi_u32 ret = (hi_u32)oal_timer_start(&timeout->timer, (unsigned long)timeout->timeout);
    if (ret != 0 && ret != 1) { /* 非激活计时器的mod_timer()返回0,激活计时器返回1 */
        oam_error_log1(0, OAM_SF_FRW, "{frw_timer_immediate_destroy_timer:: fail ret = %d}", ret);
    }
}

/* 功能描述:停止定时器 */
hi_void frw_timer_stop_timer(frw_timeout_stru *timeout)
{
    if (timeout == HI_NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_timer_stop_timer:: HI_NULL == pst_timeout}");
        return;
    }
    if (timeout->is_registerd == HI_FALSE || timeout->is_enabled == HI_FALSE) {
        return;
    }
    timeout->is_enabled = HI_FALSE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
