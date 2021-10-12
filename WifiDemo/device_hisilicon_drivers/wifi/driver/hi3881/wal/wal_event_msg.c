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
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "wal_event_msg.h"
#include "wal_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
static wal_msg_queue g_wal_wid_msg_queue;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_atomic g_wal_config_seq_num = hi_atomic_init(0);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_atomic g_wal_config_seq_num = hi_atomic_init(0);
#else
hi_atomic g_wal_config_seq_num = 0;
#endif
/* 获取msg序列号宏 */
#define wal_get_msg_sn() (hi_atomic_inc_return(&g_wal_config_seq_num))

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : wal_msg_queue_init
 功能描述  : init the wid response queue
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_msg_queue_init(hi_void)
{
    if (memset_s((hi_void *)&g_wal_wid_msg_queue, sizeof(g_wal_wid_msg_queue), 0, sizeof(g_wal_wid_msg_queue)) != EOK) {
        oam_error_log0(0, 0, "{wal_msg_queue_init::mem safe func err!}");
        return;
    }
    oal_spin_lock_init(&g_wal_wid_msg_queue.st_lock);
    hi_list_init(&g_wal_wid_msg_queue.head);
    g_wal_wid_msg_queue.count = 0;
    hi_wait_queue_init_head(&g_wal_wid_msg_queue.wait_queue);
}

static hi_void _wal_msg_request_add_queue_(wal_msg_request_stru *msg)
{
    hi_list_head_insert_optimize(&msg->entry, &g_wal_wid_msg_queue.head);
    g_wal_wid_msg_queue.count++;
}

hi_u32 wal_check_and_release_msg_resp(wal_msg_stru *rsp_msg)
{
    wal_msg_write_rsp_stru *write_rsp_msg = HI_NULL;
    if (rsp_msg != HI_NULL) {
        hi_u32 err_code;
        wlan_cfgid_enum_uint16 wid;
        write_rsp_msg = (wal_msg_write_rsp_stru *)(rsp_msg->auc_msg_data);
        err_code = write_rsp_msg->err_code;
        wid = write_rsp_msg->wid;

        oal_free(rsp_msg);

        if (err_code != HI_SUCCESS) {
            oam_warning_log2(0, OAM_SF_SCAN, "{wal_check_and_release_msg_resp::detect err code:[%u],wid:[%u]}",
                err_code, wid);
            return err_code;
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_msg_request_add_queue
 功能描述  : add the request message into queue
 输入参数  : wal_msg_request_stru* pst_msg
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_msg_request_add_queue(wal_msg_request_stru *msg)
{
    oal_spin_lock_bh(&g_wal_wid_msg_queue.st_lock);
    _wal_msg_request_add_queue_(msg);
    oal_spin_unlock_bh(&g_wal_wid_msg_queue.st_lock);
}

static hi_void _wal_msg_request_remove_queue_(wal_msg_request_stru *msg)
{
    g_wal_wid_msg_queue.count--;
    hi_list_delete_optimize(&msg->entry);
}

/* ****************************************************************************
 函 数 名  : wal_msg_request_remove_queue
 功能描述  : remove the request message into queue
 输入参数  : wal_msg_request_stru* pst_msg
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_msg_request_remove_queue(wal_msg_request_stru *msg)
{
    oal_spin_lock_bh(&g_wal_wid_msg_queue.st_lock);
    _wal_msg_request_remove_queue_(msg);
    oal_spin_unlock_bh(&g_wal_wid_msg_queue.st_lock);
}

/* ****************************************************************************
 函 数 名  : wal_set_msg_response_by_addr
 功能描述  : set the request message response by the request message's address, the address is only
 输入参数  : wal_msg_request_stru* pst_msg
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_set_msg_response_by_addr(hi_u32 addr, hi_u8 *resp_mem, hi_u32 resp_ret, hi_u32 rsp_len)
{
    hi_u32 ret = HI_FAIL;
    hi_list *pos = HI_NULL;
    hi_list *entry_temp = HI_NULL;
    wal_msg_request_stru *request = HI_NULL;

    oal_spin_lock_bh(&g_wal_wid_msg_queue.st_lock);

    hi_list_for_each_safe(pos, entry_temp, (&g_wal_wid_msg_queue.head)) {
        request = (wal_msg_request_stru *)hi_list_entry(pos, wal_msg_request_stru, entry);
        if (request->request_address == (hi_u32)addr) {
            /* address match */
            if (oal_unlikely(request->resp_mem != NULL)) {
                oam_error_log0(0, OAM_SF_ANY,
                    "{wal_set_msg_response_by_addr::wal_set_msg_response_by_addr response had been set!");
            }
            request->resp_mem = resp_mem;
            request->ret = resp_ret;
            request->resp_len = rsp_len;
            ret = HI_SUCCESS;
            break;
        }
    }

    oal_spin_unlock_bh(&g_wal_wid_msg_queue.st_lock);

    return ret;
}

/* ****************************************************************************
 函 数 名  : wal_alloc_cfg_event
 功能描述  : WAL申请事件，并填充事件头
 输入参数  : pst_net_dev: net_device
 输出参数  : ppst_event_mem: 指向事件内存
             ppst_cfg_priv : 指向私有配置结构
             ppst_cfg_msg  : 指向配置消息
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_alloc_cfg_event(oal_net_device_stru *netdev, frw_event_mem_stru **event_mem, hi_void *resp_addr,
    wal_msg_stru **cfg_msg, hi_u16 us_len)
{
    mac_vap_stru               *mac_vap = HI_NULL;
    frw_event_mem_stru         *event_mem_info = HI_NULL;
    frw_event_stru             *event = HI_NULL;
    hi_u16                  us_resp_len = 0;

    wal_msg_rep_hdr *rep_hdr = NULL;

    mac_vap = oal_net_dev_priv(netdev);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        /* 规避wifi关闭状态下，下发hipriv命令显示error日志 */
        oam_warning_log1(0, OAM_SF_ANY,
            "{wal_alloc_cfg_event::oal_net_dev_priv(pst_net_dev) is null ptr! pst_net_dev=[%p]}", (uintptr_t)netdev);
        return HI_ERR_CODE_PTR_NULL;
    }

    us_resp_len += sizeof(wal_msg_rep_hdr);
    us_len += us_resp_len;
    event_mem_info = frw_event_alloc(us_len);
    if (oal_unlikely(event_mem_info == HI_NULL)) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_ANY,
            "{wal_alloc_cfg_event::event_mem null ptr error,request size:us_len:%d,resp_len:%d}", us_len, us_resp_len);
        return HI_ERR_CODE_PTR_NULL;
    }

    *event_mem = event_mem_info; /* 出参赋值 */
    event = (frw_event_stru *)event_mem_info->puc_data;
    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CRX, WAL_HOST_CRX_SUBTYPE_CFG, us_len,
        FRW_EVENT_PIPELINE_STAGE_0, mac_vap->vap_id);

    /* fill the resp hdr */
    rep_hdr = (wal_msg_rep_hdr *)event->auc_event_data;
    if (resp_addr == NULL) {
        /* no response */
        rep_hdr->request_address = (uintptr_t)0;
    } else {
        /* need response */
        rep_hdr->request_address = (uintptr_t)resp_addr;
    }

    *cfg_msg = (wal_msg_stru *)((hi_u8 *)event->auc_event_data + us_resp_len); /* 出参赋值 */

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_request_wait_event_condition
 功能描述  : 判断wal response 完成条件是否满足
 输入参数  : wal_msg_request_stru *pst_msg_stru

 输出参数  :
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_u32 wal_request_wait_event_condition(const wal_msg_request_stru *msg_stru)
{
    hi_u32 l_ret = HI_FALSE;

    oal_spin_lock_bh(&g_wal_wid_msg_queue.st_lock);
    if ((msg_stru->resp_mem != NULL) || (msg_stru->ret != HI_SUCCESS)) {
        l_ret = HI_TRUE;
    }
    oal_spin_unlock_bh(&g_wal_wid_msg_queue.st_lock);

    return l_ret;
}

hi_void wal_cfg_msg_task_sched(hi_void)
{
    hi_wait_queue_wake_up(&g_wal_wid_msg_queue.wait_queue);
}

hi_u32 wal_send_cfg_wait_event(wal_msg_stru **rsp_msg, wal_msg_request_stru *msg_request)
{
    /* **************************************************************************
        等待事件返回
    ************************************************************************** */
    wal_wake_lock();

    /* info, boolean argument to function */ /* 使用非wifi目录定义宏函数,误报告警,lin_t e26告警屏蔽 */
    hi_u32 wal_ret = (hi_u32)hi_wait_event_timeout(g_wal_wid_msg_queue.wait_queue,
        HI_TRUE == wal_request_wait_event_condition(msg_request), (10 * HZ)); /* 10 频率 */
    /* response had been set, remove it from the list */
    wal_msg_request_remove_queue(msg_request);

    if (oal_warn_on(wal_ret == 0)) {
        /* 超时 */
        oam_warning_log2(0, OAM_SF_ANY, "[E]timeout,request ret=%d,addr:0x%lx\n", msg_request->ret,
            msg_request->request_address);
        if (msg_request->resp_mem != HI_NULL) {
            oal_free(msg_request->resp_mem);
            msg_request->resp_mem = HI_NULL;
        }
        wal_wake_unlock();
        return HI_FAIL;
    }

    wal_msg_stru *rsp_msg_info = (wal_msg_stru *)(msg_request->resp_mem);
    if (rsp_msg_info == HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_send_cfg_event:: msg mem null!}");
        /* 上面由need_rsp非空，来保证这儿rsp_msg非空，误报告警，lin_t e613告警屏蔽 */
        *rsp_msg = HI_NULL;

        wal_wake_unlock();
        return HI_FAIL;
    }

    if (rsp_msg_info->msg_hdr.us_msg_len == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_send_cfg_event:: no msg resp!}");
        /* 上面由need_rsp非空，来保证这儿rsp_msg非空，误报告警，lin_t e613告警屏蔽 */
        *rsp_msg = HI_NULL;

        oal_free(rsp_msg_info);

        wal_wake_unlock();
        return HI_FAIL;
    }
    /* 发送配置事件返回的状态信息 */
    /* 上面由need_rsp非空，来保证这儿rsp_msg非空，误报告警，lin_t e613告警屏蔽 */
    *rsp_msg = rsp_msg_info;

    wal_wake_unlock();
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_send_cfg_event
 功能描述  : WAL发送事件
 输入参数  : pst_net_dev: net_device
             en_msg_type: 消息类型
             uc_len:      消息长度
             puc_param:   消息地址
             en_need_rsp: 是否需要返回消息处理: HI_TRUE-是; HI_FALSE-否

 输出参数  : ppst_rsp_msg 二级指针，返回的response 动态内存需要用free释放!
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 wal_send_cfg_event(oal_net_device_stru *netdev, wal_msg_type_enum_uint8 msg_type, hi_u16 us_len,
    const hi_u8 *puc_param, hi_u8 need_rsp, wal_msg_stru **rsp_msg)
{
    wal_msg_stru                *cfg_msg = HI_NULL;
    frw_event_mem_stru          *event_mem = HI_NULL;
    wal_msg_request_stru         msg_request;

    /* 规则6.6：禁止使用内存操作类危险函数 例外(1)对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s((&msg_request), sizeof(msg_request), 0, sizeof(msg_request));
    msg_request.request_address = (uintptr_t)&msg_request;

    if (rsp_msg != NULL) {
        *rsp_msg = NULL;
    }

    if (oal_warn_on((need_rsp == HI_TRUE) && (rsp_msg == HI_NULL))) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_send_cfg_event::HI_NULL == ppst_rsp_msg!}\r\n");
        return HI_FAIL;
    }
    /* 申请事件 */
    if (oal_unlikely(wal_alloc_cfg_event(netdev, &event_mem, ((need_rsp == HI_TRUE) ? &msg_request : NULL), &cfg_msg,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len) != HI_SUCCESS)) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_send_cfg_event::wal_alloc_cfg_event return err!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写配置消息 */
    wal_cfg_msg_hdr_init(&(cfg_msg->msg_hdr), msg_type, us_len, (hi_u8)wal_get_msg_sn());
    /* 填写WID消息 */
    if (puc_param != HI_NULL) {
        /* cfg_msg->auc_msg_data, 可变数组,cfg_msg->auc_msg_data长度已经申请us_len */
        if (memcpy_s(cfg_msg->auc_msg_data, us_len, puc_param, us_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_send_cfg_event::mem safe function err!}");
            frw_event_free(event_mem);
            return HI_FAIL;
        }
    }
    if (need_rsp == HI_TRUE) {
        /* add queue before post event! */
        wal_msg_request_add_queue(&msg_request);
    }
    /* 分发事件 */
    frw_event_dispatch_event(event_mem);
    frw_event_free(event_mem);

    if (need_rsp != HI_TRUE) {
        return HI_SUCCESS;
    }

    /* context can't in interrupt */
    if (oal_warn_on(oal_in_interrupt())) {
        oam_error_log0(0, OAM_SF_ANY, "oal_in_interrupt");
    }

    return wal_send_cfg_wait_event(rsp_msg, &msg_request);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
