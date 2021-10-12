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
#include "oam_ext_if.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "dmac_ext_if.h"
#include "hmac_device.h"
#include "hmac_wow.h"
#include "hmac_vap.h"
#include "wal_scan.h"
#include "oal_net.h"
#include "hcc_host.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WOW_HOST_TEST

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
hmac_wow_info_stru g_wow_info = { 0 };
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_completion     g_d2h_completion;                    /* 上电唤醒后wow模块device同步数据到host侧完成 */
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32             g_d2h_completion;                    /* 上电唤醒后wow模块device同步数据到host侧完成 */
#endif
hi_u32 g_wow_enable_completion;
hi_u32 g_wlan_resume_wifi_init_flag = 0;

/* ****************************************************************************
 函 数 名  : hmac_wow_set_host_state
 功能描述  :
 输入参数  : hi_u8 uc_sleep_state
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_set_host_state(hi_u8 sleep_state)
{
    oam_warning_log2(0, OAM_SF_WOW, "{hmac_wow_set_host_state::state[%d]->[%d]!}", g_wow_info.host_sleep_state,
        sleep_state);
    g_wow_info.host_sleep_state = sleep_state;
}

/* ****************************************************************************
 函 数 名  : hmac_wow_get_host_state
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : hi_u8
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u8 hmac_wow_get_host_state(hi_void)
{
    /*  mutex  */
    return g_wow_info.host_sleep_state;
}

/* ****************************************************************************
 函 数 名  : hmac_wow_init
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_init(hi_void)
{
    hi_u32 ret;

    if (g_wow_info.is_wow_init == HMAC_WOW_MODULE_STATE_INIT) {
        return;
    }
    if (memset_s(&g_wow_info, sizeof(hmac_wow_info_stru), 0, sizeof(hmac_wow_info_stru)) != EOK) {
        return;
    }

    if (!g_wlan_resume_wifi_init_flag) {
        hmac_wow_set_host_state(HMAC_HOST_STATE_WAKEUP);
    } else {
        hmac_wow_set_host_state(HMAC_HOST_STATE_SLEPT);
    }

    g_wow_info.wow_cfg.wow_en = MAC_WOW_DISABLE;
    g_wow_info.wow_cfg.wow_event = MAC_WOW_FIELD_MAGIC_PACKET | MAC_WOW_FIELD_NETPATTERN_TCP |
        MAC_WOW_FIELD_NETPATTERN_UDP | MAC_WOW_FIELD_DISASSOC | MAC_WOW_FIELD_AUTH_RX;
    hmac_wow_create_lock();

    /* 下发参数到 DMAC */
    if (!g_wlan_resume_wifi_init_flag) {
        ret = hmac_wow_set_dmac_cfg();
        if (ret != HI_SUCCESS) {
            oam_warning_log0(0, OAM_SF_ANY, "hmac_wow_set_dmac_cfg return NON SUCCESS. ");
        }
    }

    g_wow_info.is_wow_init = HMAC_WOW_MODULE_STATE_INIT;
}

/* ****************************************************************************
 函 数 名  : hmac_wow_get_cfg_vap
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : hi_void*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void *hmac_wow_get_cfg_vap(hi_void)
{
    return mac_vap_get_vap_stru(0);
}

/* ****************************************************************************
 函 数 名  : hmac_wow_tx_check_filter_switch
 功能描述  : 判断当前状态是否需要进入 wow 过滤接口
 输入参数  :
 输出参数  :
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年4月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_wow_tx_check_filter_switch(hi_void)
{
    hi_u8 sleep_state;

    sleep_state = hmac_wow_get_host_state();

    if (g_wow_info.wow_cfg.wow_en == MAC_WOW_ENABLE) {
        if ((sleep_state == HMAC_HOST_STATE_SLEEP_REQ) || (sleep_state == HMAC_HOST_STATE_DEV_READY_FOR_HOST_SLEEP) ||
            (sleep_state == HMAC_HOST_STATE_HOST_READY) || (sleep_state == HMAC_HOST_STATE_SLEPT)) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

/* ****************************************************************************
 函 数 名  : hmac_wow_stop_scan_assoc
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_stop_scan_assoc(hi_void)
{
    hi_u32 netdev_index;
    oal_net_device_stru *netdev = HI_NULL;

    /* 停止扫描 */
    for (netdev_index = 0; netdev_index < WLAN_VAP_NUM_PER_BOARD; netdev_index++) {
        netdev = oal_get_past_net_device_by_index(netdev_index);
        if (netdev != HI_NULL) {
            wal_force_scan_complete(netdev);
        }
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_stop_upper_layer_queue
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_stop_upper_layer_queue(hi_void)
{
    hi_u32 netdev_index;

    for (netdev_index = 0; netdev_index < WLAN_VAP_NUM_PER_BOARD; netdev_index++) {
        if (oal_get_past_net_device_by_index(netdev_index) != HI_NULL) {
            /* stop the netdev's queues */
            oal_net_tx_stop_all_queues(); /* 停止发送队列 */
        }
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_clear_data_channal
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_clear_data_channal(hi_void)
{
#ifdef _PRE_FEATURE_SDIO
    hcc_clear_all_queues(hcc_host_get_handler(), HI_TRUE);
#endif
}

/* ****************************************************************************
 函 数 名  : hmac_wow_clear_event_queue
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_clear_event_queue(hi_void)
{
    /* 清理事件 */
}

/* ****************************************************************************
 函 数 名  : hmac_wow_prepare_wakeup
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_prepare_wakeup(hi_void)
{
    /* wakeup 后操作 */
}

/* ****************************************************************************
 函 数 名  : hmac_wow_host_sleep_cmd
 功能描述  :
 输入参数  : mac_vap_stru  *pst_mac_vap
             hi_u32     ul_is_host_sleep
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_host_sleep_cmd(mac_vap_stru *mac_vap, hi_u32 is_host_sleep)
{
    hi_unref_param(mac_vap);

    if (g_wow_info.wow_cfg.wow_en == MAC_WOW_DISABLE) {
        oam_warning_log0(0, OAM_SF_WOW, "{hmac_wow_host_sleep_cmd::wow is not enabled.}");
        return;
    }

    if (is_host_sleep == HI_TRUE) {
        hmac_wow_host_sleep_request();
    } else {
        hmac_wow_host_wakeup_notify();
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_set_wow_cmd
 功能描述  :
 输入参数  : hi_u32 ul_wow_event
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_set_wow_cmd(hi_u32 wow_event)
{
    hi_u32 ret;

    if (g_wow_info.wow_cfg.wow_en == MAC_WOW_DISABLE) {
        oam_warning_log0(0, OAM_SF_WOW, "{hmac_wow_set_wow_cmd::wow is not enabled.}");
        return;
    }

    oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_set_wow_cmd, ul_wow_event[0x%X]}", wow_event);

    g_wow_info.wow_cfg.wow_event = wow_event;

    /* 下发参数到 DMAC */
    ret = hmac_wow_set_dmac_cfg();
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_WOW, "hmac_wow_set_dmac_cfg return NON SUCCESS. ");
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_set_wow_en_cmd
 功能描述  :
 输入参数  : hi_u32 ul_wow_en
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_set_wow_en_cmd(hi_u32 wow_en)
{
    hi_u32 ret;

    oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_set_wow_en_cmd, ul_wow_en[0x%x]}", wow_en);

    g_wow_info.wow_cfg.wow_en = (hi_u8)wow_en;

    if (!g_wow_info.wow_cfg.wow_en) {
        g_wow_info.host_sleep_state = HMAC_HOST_STATE_WAKEUP;
    }

    /* 下发参数到 DMAC */
    ret = hmac_wow_set_dmac_cfg();
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_WOW, "hmac_wow_set_dmac_cfg return NON SUCCESS. ");
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_add_pattern
 功能描述  :
 输入参数  : [1]us_pattern_index
             [2]ul_pattern_len
             [3]puc_pattern
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_wow_add_pattern(hi_u16 us_pattern_index, hi_u32 pattern_len, const hi_u8 *puc_pattern)
{
    if (oal_unlikely(puc_pattern == HI_NULL)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd::puc_pattern is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if ((us_pattern_index >= WOW_NETPATTERN_MAX_NUM) || (pattern_len == 0) || (pattern_len > WOW_NETPATTERN_MAX_LEN)) {
        oam_error_log2(0, OAM_SF_WOW, "{hmac_wow_add_pattern:: param error, index = %d, len = %d.}", us_pattern_index,
            pattern_len);
        return HI_FAIL;
    }

    if (memcpy_s(g_wow_info.wow_cfg.wow_pattern.ast_pattern[us_pattern_index].auc_pattern_data, WOW_NETPATTERN_MAX_LEN,
        puc_pattern, pattern_len) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_wow_add_pattern:: puc_pattern memcpy_s fail.");
        return HI_FAIL;
    }
    g_wow_info.wow_cfg.wow_pattern.ast_pattern[us_pattern_index].pattern_len = pattern_len;

    if (!((1 << us_pattern_index) & g_wow_info.wow_cfg.wow_pattern.us_pattern_map)) {
        g_wow_info.wow_cfg.wow_pattern.us_pattern_map |= (hi_u16)(1 << us_pattern_index);
        g_wow_info.wow_cfg.wow_pattern.us_pattern_num++;
    }

    oam_warning_log2(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd, add new pattern, len[%d], total_num[%d]}", pattern_len,
        g_wow_info.wow_cfg.wow_pattern.us_pattern_num);

    return HI_SUCCESS;
}

hi_void hmac_check_if_del_pattern(hi_u16 index, const hi_u16 *pattern_num)
{
    /* 下面的~操作符表达式中所有变量都是无符号数,误报告警，lin_t e502告警屏蔽 */
    if ((1 << index) & g_wow_info.wow_cfg.wow_pattern.us_pattern_map) {
        g_wow_info.wow_cfg.wow_pattern.us_pattern_map &= ~(1 << index);
        g_wow_info.wow_cfg.wow_pattern.us_pattern_num--;
        g_wow_info.wow_cfg.wow_pattern.ast_pattern[index].pattern_len = 0;

        /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
        memset_s(g_wow_info.wow_cfg.wow_pattern.ast_pattern[index].auc_pattern_data, WOW_NETPATTERN_MAX_LEN, 0,
            WOW_NETPATTERN_MAX_LEN);

        oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd,del pattern,Del,totalNum=%d}", *pattern_num);
    } else {
        oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd,No del pattern,Del,totalNum=%d}", *pattern_num);
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_set_pattern_cmd
 功能描述  :
 输入参数  : hmac_cfg_wow_pattern_param_stru * pst_pattern
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_wow_set_pattern_cmd(const hmac_cfg_wow_pattern_param_stru *pattern)
{
    hi_u16 *pattern_num = &g_wow_info.wow_cfg.wow_pattern.us_pattern_num;
    hi_u16  index       = pattern->us_pattern_index;
    hi_u32  len         = pattern->pattern_len;

    if (pattern->us_pattern_option == MAC_WOW_PATTERN_PARAM_OPTION_ADD) {
        if ((index >= WOW_NETPATTERN_MAX_NUM) || (len == 0) || (len > WOW_NETPATTERN_MAX_LEN)) {
            oam_error_log2(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd::ADD::param Err,index=%d,len=%d}", index, len);
            return HI_FAIL;
        }
        if (memcpy_s(g_wow_info.wow_cfg.wow_pattern.ast_pattern[index].auc_pattern_data, WOW_NETPATTERN_MAX_LEN,
            pattern->auc_pattern_value, len) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "hmac_wow_set_pattern_cmd:: auc_pattern_value memcpy_s fail.");
            return HI_FAIL;
        }
        g_wow_info.wow_cfg.wow_pattern.ast_pattern[index].pattern_len = len;

        if (!((1 << index) & g_wow_info.wow_cfg.wow_pattern.us_pattern_map)) {
            g_wow_info.wow_cfg.wow_pattern.us_pattern_map |= (hi_u16)(1 << index);
            g_wow_info.wow_cfg.wow_pattern.us_pattern_num++;
        }

        oam_warning_log4(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd, add new pattern,len=%d,total_num=%d,Value[%X][%X]}",
            len, *pattern_num, pattern->auc_pattern_value[0], pattern->auc_pattern_value[1]);
    } else if (pattern->us_pattern_option == MAC_WOW_PATTERN_PARAM_OPTION_DEL) {
        if (index >= WOW_NETPATTERN_MAX_NUM) {
            oam_error_log1(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd::DEL: param Err,index=%d}", len);
            return HI_FAIL;
        }
        hmac_check_if_del_pattern(index, pattern_num);
    } else if (pattern->us_pattern_option == MAC_WOW_PATTERN_PARAM_OPTION_CLR) {
        /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
        /* memset按照结构体长度处理，误报lin_t419告警，告警屏蔽 */
        memset_s(&g_wow_info.wow_cfg.wow_pattern, sizeof(g_wow_info.wow_cfg.wow_pattern), 0,
            sizeof(g_wow_info.wow_cfg.wow_pattern));
        oam_warning_log0(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd, pattern clear}");
    } else {
        oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_set_pattern_cmd, error option[%d]}", pattern->us_pattern_option);
    }

    /* 下发参数到 DMAC */
    if (hmac_wow_set_dmac_cfg() != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_WOW, "hmac_wow_set_dmac_cfg return NON SUCCESS. ");
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : Host 侧发送 休眠请求异步处理
**************************************************************************** */
hi_u32 hmac_wow_host_sleep_request_asyn_handle(hi_void)
{
    hi_u32 ret;

    hmac_wow_set_host_state(HMAC_HOST_STATE_SLEEP_REQ);

    ret = hmac_wow_host_sleep_wakeup_notify(MAC_WOW_SLEEP_REQUEST);

    hmac_wow_stop_scan_assoc();

    hmac_wow_stop_upper_layer_queue();

    return ret;
}

/* ****************************************************************************
 函 数 名  : hmac_wow_host_wakeup_notify_asyn_handle
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_wow_host_wakeup_notify_asyn_handle(hi_void)
{
    hi_u32 ret;

    /* 这里需要异步操作 或者 外部循环， 等 liteos 接口明了 */
    hmac_wow_set_host_state(HMAC_HOST_STATE_WAKEUP);
    ret = hmac_wow_host_sleep_wakeup_notify(MAC_WOW_WAKEUP_NOTIFY);
    oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_host_wakeup_notify_asyn_handle, ret = %d}", ret);

    hmac_wow_prepare_wakeup();

    return ret;
}

/* ****************************************************************************
 功能描述  : Host 侧休眠请求
**************************************************************************** */
/* 此函数由 平台调用，需求为外部线程 */
hi_void hmac_wow_host_sleep_request(hi_void)
{
    hi_u8 sleep_state;

    sleep_state = hmac_wow_get_host_state();

    oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_host_sleep_request, cur_state = %d}", sleep_state);

    if (sleep_state == HMAC_HOST_STATE_WAKEUP) {
        hmac_wow_host_sleep_request_asyn_handle();

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        /* 获取锁, 被挂起 */
        hmac_wow_obtain_lock();
#endif
    } else {
        oam_error_log2(0, OAM_SF_WOW,
                       "{hmac_wow_host_sleep_request:: ERROR_state[%d], Expect_state[%d]!}",
                       sleep_state,
                       HMAC_HOST_STATE_WAKEUP);
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_host_wakeup_notify
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
/* 此函数由平台调用，需求为外部本线程，如果不是则需要调用线程通信接口 */
hi_void hmac_wow_host_wakeup_notify(hi_void)
{
    hi_u8 sleep_state;

    sleep_state = hmac_wow_get_host_state();

    oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_host_wakeup_notify, uc_sleep_state = %d}", sleep_state);
    if (sleep_state == HMAC_HOST_STATE_SLEPT) {
        hmac_wow_set_host_state(HMAC_HOST_STATE_WAKEUP);

        /* for debug */
        hmac_wow_host_wakeup_notify_asyn_handle();
    } else {
        oam_error_log2(0, OAM_SF_WOW, "{hmac_wow_host_wakeup_notify:: ERROR_state[%d], Expect_state[%d]!}",
                       sleep_state, HMAC_HOST_STATE_SLEPT);
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_create_lock
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_create_lock(hi_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    OAL_INIT_COMPLETION(&g_wow_info.sleep_req_done_event);
#endif
}

/* ****************************************************************************
 函 数 名  : hmac_wow_obtain_lock
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_obtain_lock(hi_void)
{
    hi_u32 ret;

    oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_obtain_lock, uc_sleep_state = %d}", hmac_wow_get_host_state());

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    ret = oal_wait_for_completion_timeout(&g_wow_info.sleep_req_done_event,
        (hi_u32)OAL_MSECS_TO_JIFFIES(WOW_SLEEP_REQ_WAIT_TIMEOUT));
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    ret = hi_event_wait(get_app_event_id(), HI_EVENT_SLEEP_REQUEST_DONE, &g_wow_info.sleep_req_done_event,
        WOW_SLEEP_REQ_WAIT_TIMEOUT, HI_EVENT_WAITMODE_OR | HI_EVENT_WAITMODE_CLR);
#endif
    if (ret != 0) {
        oam_info_log1(0, OAM_SF_WOW, "hmac_wow_obtain_lock, -> timeout[%d]!!!\n", ret);
    }

    oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_obtain_lock, Exit, uc_sleep_state = %d}", hmac_wow_get_host_state());
}

/* ****************************************************************************
 函 数 名  : hmac_wow_release_lock
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_release_lock(hi_void)
{
    oam_warning_log0(0, OAM_SF_WOW, "hmac_wow_release_lock Enter\n");
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    OAL_COMPLETE(&g_wow_info.sleep_req_done_event);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hi_event_send(get_app_event_id(), HI_EVENT_SLEEP_REQUEST_DONE);
#endif
}

hi_void hmac_wow_proc_dev_sleep_state(hi_void)
{
    hi_u32 ret;
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u8 dev_sleep_state = HMAC_HOST_STATE_SLEEP_REQ;

    mac_vap = mac_vap_get_vap_stru(0);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_proc_dev_sleep_state::pst_mac_vap is null.}");
        return;
    }

    /* **************************************************************************
        抛事件到DMAC层, 修改 Device 侧状态
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_DEV_STATE, sizeof(hi_u8), &dev_sleep_state);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_proc_dev_sleep_state::send WLAN_CFGID_SET_DEV_STATE event fail.}");
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_trigger_host_state
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
/* 如果此处还没有准备好，则需要在其他地方异步 hmac_wow_release_lock(); */
hi_void hmac_wow_trigger_host_state(hi_void)
{
    if (HMAC_HOST_STATE_DEV_READY_FOR_HOST_SLEEP == hmac_wow_get_host_state()) {
        /* if HOST is ready */
        hmac_wow_set_host_state(HMAC_HOST_STATE_HOST_READY);
        hmac_wow_clear_data_channal();
        hmac_wow_clear_event_queue();

        /* 确保镜像中为 SLEPT 状态 */
        hmac_wow_set_host_state(HMAC_HOST_STATE_SLEPT);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        /* 释放锁 */
        hmac_wow_release_lock();
#endif
    }
}

/* ****************************************************************************
 功能描述  : ready sleep event
 输入参数  :
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_wow_proc_dev_ready_slp_event(frw_event_mem_stru *event_mem)
{
    frw_event_stru          *event = HI_NULL;
    hmac_vap_stru           *hmac_vap = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_proc_dev_ready_slp_event::event_mem is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event    = (frw_event_stru *)event_mem->puc_data;
    hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_proc_dev_ready_slp_event::pst_hmac_vap is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_wow_proc_dev_ready_slp();

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hmac_wow_proc_dev_ready_slp
 功能描述  : Device is ready for Host's sleep request.
 输入参数  : 无
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_proc_dev_ready_slp(hi_void)
{
    hi_u8 sleep_state;

    sleep_state = hmac_wow_get_host_state();
    oam_warning_log1(0, OAM_SF_WOW, "hmac_wow_proc_dev_ready_slp:: cur_state[%d]", sleep_state);
    if (sleep_state == HMAC_HOST_STATE_SLEEP_REQ) {
        hmac_wow_set_host_state(HMAC_HOST_STATE_DEV_READY_FOR_HOST_SLEEP);

        /* 检查是否可以直接 unlock */
        hmac_wow_trigger_host_state();

        hmac_wow_proc_dev_sleep_state();
    } else if (sleep_state == HMAC_HOST_STATE_HOST_READY) {
        /* 确保镜像中为 SLEPT 状态 */
        hmac_wow_set_host_state(HMAC_HOST_STATE_SLEPT);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        /* 释放锁 */
        hmac_wow_release_lock();
#endif
    } else {
        oam_error_log3(0, OAM_SF_WOW, "{hmac_wow_proc_dev_ready_slp:: ERROR_state[%d], Expect_state[%d][%d]!}",
            sleep_state, HMAC_HOST_STATE_SLEEP_REQ, HMAC_HOST_STATE_HOST_READY);
    }
}

/* ****************************************************************************
 函 数 名  : hmac_wow_msg_handle
 功能描述  : 处理device侧发来的msg
 输入参数  : 无
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 hmac_wow_msg_handle(hi_void)
{
    hmac_wow_msg_incr(WOW_D2H_MSG);
    oam_warning_log3(0, OAM_SF_WOW, "{hmac_wow_msg_handle, time[%d], CNT:SLP[%d],WKUP[%d],D2H[%d]}",
        WOW_H2D_SLP_MSG_CNT, WOW_H2D_WKUP_MSG_CNT, WOW_D2H_MSG_CNT);
    hmac_wow_proc_dev_ready_slp();
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hmac_wow_msg_incr
 功能描述  : channel msg counter 处理
 输入参数  : hi_u32 ul_msg_type 消息类型
 输出参数  :
 返 回 值  : hi_void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hmac_wow_msg_incr(hi_u32 msg_type)
{
    switch (msg_type) {
        case WOW_H2D_SLP_MSG:
            g_wow_info.debug_info.int_info.h2d_slp_msg_cnt++;
            break;
        case WOW_H2D_WKUP_MSG:
            g_wow_info.debug_info.int_info.h2d_wkup_msg_cnt++;
            break;
        case WOW_D2H_MSG:
            g_wow_info.debug_info.int_info.d2h_msg_cnt++;
            break;
        default:
            break;
    }
}

/* ****************************************************************************
 功能描述  : dmac 唤醒后同步 host data

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_wow_proc_dev_sync_host_event(frw_event_mem_stru *event_mem)
{
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u32 index;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_proc_dev_sync_host_event::frw_event_mem is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    frw_event_stru        *event          = (frw_event_stru *)event_mem->puc_data;
    mac_d2h_syn_hdr_stru  *sync_info_hdr  = (mac_d2h_syn_hdr_stru *)event->auc_event_data;
    mac_d2h_syn_data_stru *sync_info_data = (mac_d2h_syn_data_stru *)(sync_info_hdr + 1);

    g_wow_info.wakeup_reason = sync_info_hdr->wakeup_reason;

    oam_warning_log2(0, OAM_SF_WOW, "{hmac_wow_proc_dev_sync_host_event:: block_count[%d], reason[0x%08x].}",
        sync_info_hdr->data_blk_cnt, g_wow_info.wakeup_reason);

    for (index = 0; index < sync_info_hdr->data_blk_cnt; index++, sync_info_data++) {
        mac_vap = mac_vap_get_vap_stru(sync_info_data->vap_id);
        if (mac_vap == HI_NULL) {
            oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_proc_dev_sync_host_event:: vap is null, index[%d].}", index);
            continue;
        }

        if (mac_vap->vap_mode != WLAN_VAP_MODE_BSS_STA) {
            continue;
        }

        oam_warning_log3(0, OAM_SF_WOW, "{hmac_wow_proc_dev_sync_host_event::in loop, index[%d], vap[%d], user[%d].}",
            index, sync_info_data->vap_id, sync_info_data->user_idx);

        /* 信道信息同步到hmac */
        if (memcpy_s(&(mac_vap->channel), sizeof(mac_channel_stru), &(sync_info_data->channel),
            sizeof(mac_channel_stru)) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "hmac_wow_proc_dev_sync_host_event:: st_channel memcpy_s fail.");
            return HI_FAIL;
        }
        oam_warning_log2(mac_vap->vap_id, OAM_SF_WOW, "{hmac_wow_proc_dev_sync_host_event:: channel[%d], width[%d].}",
            mac_vap->channel.chan_number, mac_vap->channel.en_bandwidth);

        if ((mac_vap->ch_switch_info.waiting_to_shift_channel == HI_FALSE) &&
            (sync_info_data->ch_switch_info.waiting_to_shift_channel == HI_TRUE)) {
            if (memcpy_s(&(mac_vap->ch_switch_info), sizeof(mac_ch_switch_info_stru),
                &(sync_info_data->ch_switch_info), sizeof(mac_ch_switch_info_stru)) != EOK) {
                oam_error_log0(0, OAM_SF_CFG, "hmac_wow_proc_dev_sync_host_event:: st_ch_switch_info memcpy_s fail.");
                return HI_FAIL;
            }
        }
    }
    g_wow_info.wait_dev_data = HI_FALSE;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    OAL_COMPLETE(&g_d2h_completion);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hi_event_send(get_app_event_id(), HI_EVENT_D2H_READY_EVENT);
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hmac_wow_set_dmac_cfg
 功能描述  : H2D WOW 参数下发
 输入参数  : 无
 输出参数  : 无
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_wow_set_dmac_cfg(hi_void)
{
    hi_u32 ret;
    mac_vap_stru *mac_vap = HI_NULL;

    oam_warning_log2(0, OAM_SF_WOW, "hmac_wow_set_dmac_cfg, flag[%d], wow_event[0x%X]\n", g_wlan_resume_wifi_init_flag,
        g_wow_info.wow_cfg.wow_event);

    if (1 == g_wlan_resume_wifi_init_flag) {
        return HI_SUCCESS;
    }

    mac_vap = mac_vap_get_vap_stru(0);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_set_dmac_cfg::pst_mac_vap is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_WOW_PARAM, sizeof(g_wow_info.wow_cfg),
        (hi_u8 *)&g_wow_info.wow_cfg);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_WOW, "{hmac_wow_set_dmac_cfg::send_event failed[%d]}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : Host 侧发给 Device 侧 睡眠/ 唤醒通知
**************************************************************************** */
hi_u32 hmac_wow_host_sleep_wakeup_notify(hi_u8 is_sleep_req)
{
    hi_u32 ret;
    mac_vap_stru *mac_vap = HI_NULL;
    mac_h2d_syn_info_hdr_stru sync_hdr = { 0 };

    mac_vap = mac_vap_get_vap_stru(0);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_WOW, "{hmac_wow_host_sleep_wakeup_notify::pst_mac_vap is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    sync_hdr.msg_type = MAC_WOW_SLEEP_NOTIFY_MSG;
    sync_hdr.notify_param = is_sleep_req;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_HOST_SLEEP_NOTIFY, sizeof(mac_h2d_syn_info_hdr_stru),
        (hi_u8 *)&sync_hdr);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_WOW,
            "{hmac_config_host_sleep_wakeup_notify::send_event failed[%d],uc_is_sleep_req = %d}", ret, is_sleep_req);
    }

    return ret;
}


/* ****************************************************************************
 函 数 名  : wlan_suspend
 功能描述  : 睡眠入口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年7月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wlan_suspend(hi_void)
{
    hi_u8 sleep_state;

    if ((g_wow_info.wow_cfg.wow_event == MAC_WOW_FIELD_ALL_CLEAR) || (g_wow_info.wait_dev_data == HI_TRUE)) {
        oam_warning_log2(0, OAM_SF_WOW, "{wlan_suspend, [wow], NOT_ALLOWED, EVENT[%08X],wait[%d]}",
            g_wow_info.wow_cfg.wow_event, g_wow_info.wait_dev_data);
        return;
    }

    sleep_state = hmac_wow_get_host_state();
    if (sleep_state != HMAC_HOST_STATE_WAKEUP) {
        oam_warning_log1(0, OAM_SF_WOW, "{wlan_suspend, [wow], NOT_ALLOWED, uc_sleep_state = %d}", sleep_state);
        return;
    }

    oam_warning_log3(0, OAM_SF_WOW, "{wlan_suspend,[wow], CNT:SLP[%d],WKUP[%d],D2H[%d]}", WOW_H2D_SLP_MSG_CNT,
        WOW_H2D_WKUP_MSG_CNT, WOW_D2H_MSG_CNT);

    /* 业务暂停接口 */
    wlan_wifi_suspend();
    oam_warning_log0(0, OAM_SF_WOW, "{host_pow_off.}");
}

/* ****************************************************************************
 函 数 名  : wlan_wifi_suspend
 功能描述  : 业务暂停接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年7月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wlan_wifi_suspend(hi_void)
{
    /* sleep req */
    hmac_wow_host_sleep_request();
}

/* ****************************************************************************
 函 数 名  : wlan_wifi_resume
 功能描述  : 业务恢复接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年7月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wlan_wifi_resume(hi_void)
{
    oam_warning_log1(0, OAM_SF_WOW, "{wlan_wifi_resume, uc_sleep_state = %d}\n", hmac_wow_get_host_state());
    /* wifi resume,recovery data process */
    hisi_wifi_resume_process();
    /* wakeup notify */
    hmac_wow_host_wakeup_notify();
}

/* ****************************************************************************
 函 数 名  : hmac_wow_check_event
 功能描述  : 判断当前是否需要过滤事件
 输入参数  :
 输出参数  :
 返 回 值  : hi_u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年4月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_wow_check_event(const frw_event_hdr_stru *event_hrd, hi_u8 event_dir)
{
    hi_u8 cnt;

    if (event_hrd == HI_NULL) {
        return HI_FALSE;
    }

    if (HI_TRUE == hmac_wow_tx_check_filter_switch() && (FRW_EVENT_PIPELINE_STAGE_1 == event_hrd->pipeline)) {
        switch (event_dir) {
            case EVENT_TX_TYPE:
                g_wow_info.debug_info.event_info.tx_cnt++;
                cnt = g_wow_info.debug_info.event_info.tx_cnt;
                break;
            case EVENT_RX_TYPE:
                g_wow_info.debug_info.event_info.rx_cnt++;
                cnt = g_wow_info.debug_info.event_info.rx_cnt;
                break;
            case EVENT_DUAL_TYPE:
                g_wow_info.debug_info.event_info.dual_cnt++;
                cnt = g_wow_info.debug_info.event_info.dual_cnt;
                break;
            case EVENT_TYPE_BUTT:
            default:
                return HI_FALSE;
        }
        oam_warning_log4(0, OAM_SF_WOW,
            "hmac_wow_check_event, wow_blocked, event|sub[0x%08X],dir[%d],cnt[%d]state[%d]\n",
            (event_hrd->type << 16) | event_hrd->sub_type, event_dir, cnt, hmac_wow_get_host_state()); /* 左移16位 */
        return HI_TRUE;
    }

    return HI_FALSE;
}

#ifndef _PRE_WLAN_FEATURE_QUICK_START
/* ****************************************************************************
 函 数 名  : hisi_wlan_suspend
 功能描述  : 强制睡眠 API接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年01月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hisi_wlan_suspend(hi_void)
{
    wlan_suspend();
}
#endif
/* ****************************************************************************
 函 数 名  : hisi_wlan_set_wow_event
 功能描述  : 设置强制睡眠功能开关接口
 输入参数  : ul_event 事件开关值
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年01月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hisi_wlan_set_wow_event(hi_u32 event)
{
    hmac_wow_set_wow_cmd(event);
}

/* ****************************************************************************
 函 数 名  : hisi_wlan_add_netpattern
 功能描述  : 强制睡眠netpattern唤醒报文格式的添加API接口
 输入参数  : [1]netpattern_index: netpattern 的索引, 0~3
             [2]puc_netpattern_data: netpattern 的内容
             [3]netpattern_len  : netpattern 的内容长度, 0~64
 输出参数  : 无
 返 回 值  : 0  : 成功
             非0: 失败
**************************************************************************** */
hi_u32 hisi_wlan_add_netpattern(hi_u32 netpattern_index, hi_uchar *puc_netpattern_data, hi_u32 netpattern_len)
{
    hmac_cfg_wow_pattern_param_stru pattern;

    if (memset_s(&pattern, sizeof(hmac_cfg_wow_pattern_param_stru), 0,
        sizeof(hmac_cfg_wow_pattern_param_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hisi_wlan_add_netpattern:: memset_s fail.");
        return HI_FAIL;
    }

    pattern.us_pattern_option = MAC_WOW_PATTERN_PARAM_OPTION_ADD;
    pattern.us_pattern_index  = (hi_u16)netpattern_index;
    pattern.pattern_len       = netpattern_len;

    if ((pattern.us_pattern_index >= WOW_NETPATTERN_MAX_NUM) || (pattern.pattern_len == 0) ||
        (pattern.pattern_len > WOW_NETPATTERN_MAX_LEN) || (puc_netpattern_data == HI_NULL)) {
        oam_error_log3(0, OAM_SF_WOW,
            "{hisi_wlan_add_netpattern::WOW,ADD::param error, index = %d, len = %d, puc_netpattern_data[%p].}",
            pattern.us_pattern_index, pattern.pattern_len, (uintptr_t)puc_netpattern_data);
        return HI_FAIL;
    }

    if (memcpy_s(&pattern.auc_pattern_value[0], WOW_NETPATTERN_MAX_LEN, puc_netpattern_data,
                 pattern.pattern_len) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hisi_wlan_add_netpattern:: puc_netpattern_data memcpy_s fail.");
        return HI_FAIL;
    }

    return hmac_wow_set_pattern_cmd(&pattern);
}

/* ****************************************************************************
 函 数 名  : hisi_wlan_del_netpattern
 功能描述  : 强制睡眠netpattern唤醒报文格式的删除API接口
 输入参数  :   ul_netpattern_index: netpattern 的索引, 0~3
 输出参数  : 无
 返 回 值  : 0  : 成功
             非0: 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年01月05日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hisi_wlan_del_netpattern(hi_u32 netpattern_index)
{
    hmac_cfg_wow_pattern_param_stru pattern = {0};

    pattern.us_pattern_option   = MAC_WOW_PATTERN_PARAM_OPTION_DEL;
    pattern.us_pattern_index    = (hi_u16)netpattern_index;

    return hmac_wow_set_pattern_cmd(&pattern);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
