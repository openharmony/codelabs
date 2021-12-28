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
#include "hcc_hmac_if.h"
#include "frw_timer.h"
#include "hmac_config.h"
#include "hmac_user.h"
#include "hmac_vap.h"
#include "hmac_mgmt_classifier.h"
#include "mac_ie.h"
#include "mac_pm_driver.h"
#include "hmac_rx_filter.h"
#include "hmac_device.h"
#include "plat_pm_wlan.h"
#include "hmac_fsm.h"
#include "hmac_mgmt_bss_comm.h"
#include "hmac_mgmt_ap.h"
#include "hmac_mgmt_sta.h"
#include "hmac_tx_data.h"
#include "hmac_scan.h"
#include "hmac_sme_sta.h"
#include "hmac_blockack.h"
#include "hmac_p2p.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/* ****************************************************************************
  2 内部函数声明
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_BTCOEX
hi_u32 hmac_btcoex_delba_foreach_user(mac_vap_stru *mac_vap);
#endif

/* ****************************************************************************
  3 全局变量定义
**************************************************************************** */
hi_bool g_wlan_pm_on = HI_FALSE;
frw_timeout_stru g_pm_apdown_timer = { 0 };
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
hmac_reg_info_receive_event g_hmac_reg_info_receive_event = { 0 };
#endif

typedef struct {
    wlan_protocol_enum_uint8 protocol_mode; /* wid枚举 */
} hmac_protocol_stru;

#define PM_APDOWN_ENTERY_TIME 200000

#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
static hi_bool g_hist_ampdu_tx_on = HI_TRUE; /* 缓存聚合开关标志 */
#endif
#endif
/* ****************************************************************************
  3 函数定义
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 删除BA会话的配置命令(相当于接收到DELBA帧)
**************************************************************************** */
hi_u32 hmac_config_delba_req(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_delba_req_param_stru *delba_req = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    mac_action_mgmt_args_stru action_args; /* 用于填写ACTION帧的参数 */
    hmac_tid_stru *hmac_tid = HI_NULL;
    hi_u32 ret;

    hi_unref_param(us_len);

    delba_req = (mac_cfg_delba_req_param_stru *)puc_param;
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    /* 获取用户对应的索引 */
    hmac_user = mac_vap_get_hmac_user_by_addr(mac_vap, delba_req->auc_mac_addr, WLAN_MAC_ADDR_LEN);
    if (hmac_vap == HI_NULL || hmac_user == HI_NULL) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_delba_req::hmac_vap/hmac_user null! hmac_vap=%p, hmac_user=%p}", (uintptr_t)hmac_vap,
            (uintptr_t)hmac_user);
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_tid = &(hmac_user->ast_tid_info[delba_req->tidno]);

    /* 查看会话是否存在 */
    if (delba_req->direction == MAC_RECIPIENT_DELBA) {
        if (hmac_tid->ba_rx_info == HI_NULL) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_delba_req::the rx hdl is not exist.}");
            return HI_SUCCESS;
        }
    } else {
        if (hmac_tid->ba_tx_info == HI_NULL) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_delba_req::the tx hdl is not exist.}");
            return HI_SUCCESS;
        }
    }

    /*
       建立BA会话时，st_action_args(DELBA_REQ)结构各个成员意义如下
       (1)uc_category:action的类别
       (2)uc_action:BA action下的类别
       (3)ul_arg1:BA会话对应的TID
       (4)ul_arg2:删除ba会话的发起端
       (5)ul_arg3:删除ba会话的原因
       (6)ul_arg5:ba会话对应的用户
     */
    action_args.category = MAC_ACTION_CATEGORY_BA;
    action_args.action = MAC_BA_ACTION_DELBA;
    action_args.arg1 = delba_req->tidno;            /* 该数据帧对应的TID号 */
    action_args.arg2 = delba_req->direction;        /* ADDBA_REQ中，buffer_size的默认大小 */
    action_args.arg3 = MAC_UNSPEC_REASON;           /* BA会话的确认策略 */
    action_args.puc_arg5 = delba_req->auc_mac_addr; /* ba会话对应的user */

    /* 建立BA会话 */
    ret = hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "hmac_mgmt_tx_action return NON SUCCESS. ");
    }

    return HI_SUCCESS;
}

#if defined(_PRE_WLAN_FEATURE_SIGMA) || defined(_PRE_DEBUG_MODE)
/* ****************************************************************************
 功能描述  : 建立BA会话的配置命令
**************************************************************************** */
hi_u32 hmac_config_addba_req(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_addba_req_param_stru *addba_req = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    mac_action_mgmt_args_stru action_args; /* 用于填写ACTION帧的参数 */
    hi_u8 ampdu_support;
    hi_u32 ret;

    hi_unref_param(us_len);

    addba_req = (mac_cfg_addba_req_param_stru *)puc_param;
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    /* 获取用户对应的索引 */
    hmac_user = mac_vap_get_hmac_user_by_addr(mac_vap, addba_req->auc_mac_addr, WLAN_MAC_ADDR_LEN);
    if (hmac_vap == HI_NULL || hmac_user == HI_NULL) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_addba_req::hmac_vap/hmac_user null! hmac_vap=%p, hmac_user=%p}", (uintptr_t)hmac_vap,
            (uintptr_t)hmac_user);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 建立BA会话，是否需要判断VAP的AMPDU的支持情况，因为需要实现建立BA会话时，一定发AMPDU */
    ampdu_support = hmac_user_xht_support(hmac_user);
    /* 手动创建ba会话，不受其他额外限制 */
    if (ampdu_support) {
        /*
           建立BA会话时，st_action_args(ADDBA_REQ)结构各个成员意义如下
           (1)uc_category:action的类别
           (2)uc_action:BA action下的类别
           (3)ul_arg1:BA会话对应的TID
           (4)ul_arg2:BUFFER SIZE大小
           (5)ul_arg3:BA会话的确认策略
           (6)ul_arg4:TIMEOUT时间
         */
        action_args.category = MAC_ACTION_CATEGORY_BA;
        action_args.action = MAC_BA_ACTION_ADDBA_REQ;
        action_args.arg1 = addba_req->tidno;        /* 该数据帧对应的TID号 */
        action_args.arg2 = addba_req->us_buff_size; /* ADDBA_REQ中，buffer_size的默认大小 */
        action_args.arg3 = addba_req->ba_policy;    /* BA会话的确认策略 */
        action_args.arg4 = addba_req->us_timeout;   /* BA会话的超时时间设置为0 */

        /* 建立BA会话 */
        ret = hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args);
        if (ret != HI_SUCCESS) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "hmac_mgmt_tx_action return NON SUCCESS. ");
        }
    }
    return HI_SUCCESS;
}
#endif

/*****************************************************************************
 功能描述  : 将带宽枚举值转换为对应的字符信息 供VAPinfo命令打印
**************************************************************************** */
static hi_u32 hmac_config_bw2string(hi_u32 bw)
{
    switch (bw) {
        case WLAN_BAND_WIDTH_20M:
            return 0x20;
        case WLAN_BAND_WIDTH_40PLUS:
            return 0x40B;
        case WLAN_BAND_WIDTH_40MINUS:
            return 0x40A;
        case WLAN_BAND_WIDTH_80PLUSPLUS:
            return 0x80AA;
        case WLAN_BAND_WIDTH_80PLUSMINUS:
            return 0x80AB;
        case WLAN_BAND_WIDTH_80MINUSPLUS:
            return 0x80BA;
        case WLAN_BAND_WIDTH_80MINUSMINUS:
            return 0x80BB;
        case WLAN_BAND_WIDTH_5M:
            return 0x5;
        case WLAN_BAND_WIDTH_10M:
            return 0x10;
        default:
            return 0xFFFF;
    }
}

/* ****************************************************************************
 功能描述  : 打印vap参数信息
**************************************************************************** */
hi_u32 hmac_config_vap_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hmac_vap_stru   *hmac_vap = HI_NULL;
    mac_user_stru   *mac_user = HI_NULL;
    hi_u8           loop;

    hi_unref_param(us_len);
    hi_unref_param(puc_param);

    if (mac_vap->vap_mode == WLAN_VAP_MODE_CONFIG) {
        oam_warning_log0(0, 0, "{hmac_config_vap_info::this is config vap! can't get info.}");
        return HI_FAIL;
    }

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, 0, "{hmac_config_vap_info::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    oam_warning_log4(0, 0, "vap id: %d, vap state: %d, vap mode: %d, P2P mode:%d", mac_vap->vap_id, mac_vap->vap_state,
        mac_vap->vap_mode, mac_vap->p2p_mode);
    /* AP/STA信息显示 */
    mac_user = mac_user_get_user_stru(mac_vap->assoc_vap_id);
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) && (mac_user != HI_NULL)) {
        oam_warning_log3(0, 0, "available protocol: %d, current protocol: %d, channel number:%d.", mac_vap->protocol,
            mac_user->cur_protocol_mode, mac_vap->channel.chan_number);
    } else if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)

#endif
    ) {
        oam_warning_log4(0, 0, "protocol:%d, channel number:%d, associated user number:%d, beacon interval:%d.",
            mac_vap->protocol, mac_vap->channel.chan_number, mac_vap->user_nums,
            mac_vap->mib_info->wlan_mib_sta_config.dot11_beacon_period);
        oam_warning_log1(0, 0, "hide_ssid :%d", mac_vap->cap_flag.hide_ssid);
    } else {
        oam_warning_log1(0, 0, "protocol: %d.", mac_vap->protocol);
    }
    hi_u32 bandwidth = hmac_config_bw2string(mac_vap->channel.en_bandwidth);
    hi_unref_param(bandwidth);
    oam_warning_log0(0, 0, "0-11a, 1-11b, 3-11bg, 4-11g, 5-11bgn, 6-11ac, 7-11nonly, 8-11aconly, 9-11ng, other-error.");
    oam_warning_log2(0, 0, "band: %x G, bandwidth: %x M[80A=80+,80B=80-,80AB=80+-]",
        (mac_vap->channel.band == WLAN_BAND_2G) ? 2 : 0xFF, bandwidth); /* 只支持2G,其他返回异常值0XFF */
    oam_warning_log4(0, 0, "amsdu=%d, uapsd=%d, wpa=%d, wpa2=%d.", hmac_vap->amsdu_active, mac_vap->cap_flag.uapsd,
        mac_vap->cap_flag.wpa, mac_vap->cap_flag.wpa2);
    oam_warning_log4(0, 0, "wps=%d, keepalive=%d, shortgi=%d, tx power=%d.", hmac_vap->wps_active,
        mac_vap->cap_flag.keepalive, mac_vap->mib_info->phy_ht.dot11_short_gi_option_in_twenty_implemented,
        mac_vap->tx_power);
    /* APP IE 信息 */
    for (loop = 0; loop < OAL_APP_IE_NUM; loop++) {
        oam_warning_log3(0, 0, "APP IE:type= %d, addr = %p, len = %d.", loop,
            (uintptr_t)mac_vap->ast_app_ie[loop].puc_ie, mac_vap->ast_app_ie[loop].ie_len);
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 开启AMPDU配置命令
**************************************************************************** */
hi_u32 hmac_config_ampdu_start(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_ampdu_start_param_stru *ampdu_start = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    mac_action_mgmt_args_stru action_args;
    hi_u8 ret;
    hi_u32 result;

    hi_unref_param(us_len);

    ampdu_start = (mac_cfg_ampdu_start_param_stru *)puc_param;
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    /* 获取用户对应的索引 */
    hmac_user = mac_vap_get_hmac_user_by_addr(mac_vap, ampdu_start->auc_mac_addr, WLAN_MAC_ADDR_LEN);
    if (hmac_vap == HI_NULL || hmac_user == HI_NULL) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_ampdu_start::hmac_vap/hmac_user null! hmac_vap=%p, hmac_user=%p}", (uintptr_t)hmac_vap,
            (uintptr_t)hmac_user);
        return HI_ERR_CODE_PTR_NULL;
    }

    ret = hmac_tid_need_ba_session(hmac_vap, hmac_user, ampdu_start->tidno, HI_NULL);
    if (ret == HI_TRUE) {
        /*
           建立BA会话时，st_action_args结构各个成员意义如下
           (1)uc_category:action的类别
           (2)uc_action:BA action下的类别
           (3)ul_arg1:BA会话对应的TID
           (4)ul_arg2:BUFFER SIZE大小
           (5)ul_arg3:BA会话的确认策略
           (6)ul_arg4:TIMEOUT时间
         */
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_ampdu_start::uc_tidno=%d.}", ampdu_start->tidno);
        action_args.category = MAC_ACTION_CATEGORY_BA;
        action_args.action = MAC_BA_ACTION_ADDBA_REQ;
        action_args.arg1 = ampdu_start->tidno;         /* 该数据帧对应的TID号 */
        action_args.arg2 = WLAN_AMPDU_TX_MAX_BUF_SIZE; /* ADDBA_REQ中，buffer_size的默认大小 */
        action_args.arg3 = MAC_BA_POLICY_IMMEDIATE;    /* BA会话的确认策略 */
        action_args.arg4 = 0;                          /* BA会话的超时时间设置为0 */

        /* 建立BA会话 */
        result = hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args);
        if (result != HI_SUCCESS) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "hmac_mgmt_tx_action return NON SUCCESS. ");
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置amsdu+ampdu联合聚合的开关
**************************************************************************** */
hi_u32 hmac_config_amsdu_ampdu_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    hi_s32 l_value;

    hi_unref_param(us_len);

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_amsdu_ampdu_switch::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    l_value = *((hi_s32 *)puc_param);
    hmac_vap->amsdu_ampdu_active = (hi_u8)l_value;
    oam_warning_log1(0, 0, "hmac_config_amsdu_ampdu_switch:: switch_value[%d]", hmac_vap->amsdu_ampdu_active);
    return HI_SUCCESS;
}

#endif

/* ****************************************************************************
 功能描述  : 打印user信息
**************************************************************************** */
hi_u32 hmac_config_user_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_USER_INFO, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_user_info::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : 设置发送描述符信息
**************************************************************************** */
hi_u32 hmac_config_set_dscr_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_DSCR, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_set_dscr_param:: send_event failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : 删除hmac ba超时定时器，调用函数超50行拆分
 **************************************************************************** */
hi_u32 hmac_proc_dev_sleep_req_del_ba_timer(const hmac_vap_stru *hmac_vap, hi_u32 pm_wlan_state)
{
    hi_list *entry = HI_NULL;
    hi_list *user_list_head = HI_NULL;
    mac_user_stru *mac_user = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;
    hi_u8 tid_num;

    /* 遍历该 VAP 下所有用户 */
    if (hmac_vap->base_vap == HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_proc_dev_sleep_req_del_ba_timer::hmac_vap->base_vap is null.}");
        return HI_FAIL;
    }

    user_list_head = &(hmac_vap->base_vap->mac_user_list_head);
    for (entry = user_list_head->next; entry != user_list_head; entry = entry->next) {
        mac_user = hi_list_entry(entry, mac_user_stru, user_dlist);
        /*lint -e774*/
        if (mac_user == HI_NULL) {
            oam_warning_log0(0, OAM_SF_ANY, "{hmac_proc_dev_sleep_req_del_ba_timer::mac user is null.}");
            return HI_FAIL;
        }
        /*lint +e774*/
        hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(mac_user->us_assoc_id);
        if (hmac_user == HI_NULL) {
            oam_warning_log0(0, OAM_SF_ANY, "{hmac_proc_dev_sleep_req_del_ba_timer::hmac user is null.}");
            return HI_FAIL;
        }

        for (tid_num = 0; tid_num < WLAN_TID_MAX_NUM; tid_num++) {
            hmac_ba_rx_stru *rx_ba = hmac_user->ast_tid_info[tid_num].ba_rx_info;
            if ((rx_ba == HI_NULL) || (rx_ba->ba_timer.is_registerd == HI_FALSE)) {
                continue;
            }
            if ((pm_wlan_state == WLAN_PM_WORK) && (rx_ba->ba_timer.func != HI_NULL)) {
                if (rx_ba->ba_timer.timeout_arg == HI_NULL) {
                    continue;
                }
                /* work中直接调用超时回调函数 */
                rx_ba->ba_timer.func(rx_ba->ba_timer.timeout_arg);
            } else if (pm_wlan_state == WLAN_PM_DEEP_SLEEP) {
                /* 深睡下直接禁用定时器 */
                frw_timer_stop_timer(&(rx_ba->ba_timer));
            }
            /* 其他场景不需要处理 */
        }
    }

    return HI_SUCCESS;
}

hi_u32 hmac_proc_dev_sleep_req(const frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    hi_u32 sleep_type;
    hi_u32 *data = HI_NULL;
    hi_u32 pm_wlan_state;
    hi_u8 *pm_wlan_need_stop_ba = mac_get_pm_wlan_need_stop_ba();
    hi_u32 ret;

    event = (frw_event_stru *)(event_mem->puc_data);
    data = (hi_u32 *)(event->auc_event_data);
    sleep_type = *data;
    hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_proc_dev_sleep_req::hmac vap is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (sleep_type == WLAN_PM_LIGHT_SLEEP) {
        pm_wlan_state = WLAN_PM_LIGHT_SLEEP;
    } else if (sleep_type == WLAN_PM_DEEP_SLEEP) {
        *pm_wlan_need_stop_ba = HI_TRUE;
        pm_wlan_state = WLAN_PM_DEEP_SLEEP;
    } else if (sleep_type == WLAN_PM_WORK) {
        *pm_wlan_need_stop_ba = HI_FALSE;
        pm_wlan_state = WLAN_PM_WORK;
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_proc_dev_sleep_req::state is wrong.}");
        return HI_FAIL;
    }

    /* 删除hmac ba超时定时器 */
    ret = hmac_proc_dev_sleep_req_del_ba_timer(hmac_vap, pm_wlan_state);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{hmac_proc_dev_sleep_req::del ba timeout timer not succ[%d]}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理协议模式同步事件
 修改历史      :
  1.日    期   : 2015年4月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_syn_info_event(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u32 relt;
    dmac_to_hmac_syn_info_event_stru *syn_info_event = HI_NULL;

    event = (frw_event_stru *)event_mem->puc_data;
    syn_info_event = (dmac_to_hmac_syn_info_event_stru *)event->auc_event_data;
    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(syn_info_event->user_index);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hmac_syn_info_event: pst_hmac_user null,user_idx=%d.}",
            syn_info_event->user_index);
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_vap = mac_vap_get_vap_stru(hmac_user->base_user->vap_id);
    if (mac_vap == HI_NULL) {
        oam_warning_log2(0, OAM_SF_ANY, "{hmac_syn_info_event: pst_mac_vap null! vap_idx=%d, user_idx=%d.}",
            hmac_user->base_user->vap_id, syn_info_event->user_index);
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_user->base_user->cur_protocol_mode = syn_info_event->cur_protocol;
    hmac_user->base_user->cur_bandwidth = syn_info_event->cur_bandwidth;
    relt = hmac_config_user_info_syn(mac_vap, hmac_user->base_user);
    return relt;
}

/* ****************************************************************************
 功能描述  : 处理Voice聚合同步事件
 修改历史      :
  1.日    期   : 2015年4月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_voice_aggr_event(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    dmac_to_hmac_voice_aggr_event_stru *voice_aggr_event = HI_NULL;
    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_P2P, "{hmac_voice_aggr_event::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    event = (frw_event_stru *)event_mem->puc_data;
    voice_aggr_event = (dmac_to_hmac_voice_aggr_event_stru *)event->auc_event_data;

    mac_vap = mac_vap_get_vap_stru(voice_aggr_event->vap_id);
    if (mac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{hmac_voice_aggr_event: pst_mac_vap null! vap_idx=%d}",
            voice_aggr_event->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }
    mac_vap->voice_aggr = voice_aggr_event->voice_aggr;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 分配事件内存
 输入参数  : pst_mac_vap: 指向vap
             en_syn_type: 事件的subtype, 即同步消息类型
             ppst_syn_msg  : 指向同步消息payload的指针
             ppst_event_mem: 指向事件内存的指针
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_config_alloc_event(const mac_vap_stru *mac_vap, hmac_to_dmac_syn_type_enum_uint8 syn_type,
    hmac_to_dmac_cfg_msg_stru **syn_msg, frw_event_mem_stru **event_mem, hi_u16 us_len)
{
    frw_event_mem_stru *event_mem_value = HI_NULL;
    frw_event_stru *event = HI_NULL;

    event_mem_value = frw_event_alloc(us_len + sizeof(hmac_to_dmac_cfg_msg_stru) - 4); /* 4 用于计算 */
    if (oal_unlikely(event_mem_value == HI_NULL)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_alloc_event::event_mem null, us_len = %d }", us_len);
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    event = (frw_event_stru *)event_mem_value->puc_data;

    /* 填充事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CRX, syn_type,
        (us_len + sizeof(hmac_to_dmac_cfg_msg_stru) - 4), /* 4 用于计算 */
        FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);
    /* 出参赋值 */
    *event_mem = event_mem_value;
    *syn_msg = (hmac_to_dmac_cfg_msg_stru *)event->auc_event_data;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 抛事件到DMAC层, 同步DMAC数据
 输入参数  : pst_mac_vap  : VAP
             en_cfg_id: 配置id
             us_len: 消息长度
             puc_param: 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_send_event(const mac_vap_stru *mac_vap, wlan_cfgid_enum_uint16 cfg_id, hi_u16 us_len,
    const hi_u8 *puc_param)
{
    hi_u32 ret;
    frw_event_mem_stru *event_mem = HI_NULL;
    hmac_to_dmac_cfg_msg_stru *syn_msg = HI_NULL;

    ret = hmac_config_alloc_event(mac_vap, HMAC_TO_DMAC_SYN_CFG, &syn_msg, &event_mem, us_len);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_send_event::hmac_config_alloc_event failed[%d].}", ret);
        return ret;
    }
    syn_msg->syn_id = cfg_id;
    syn_msg->us_len = us_len;
    /* 填写配置同步消息内容 */
    if (puc_param != HI_NULL) {
        if (memcpy_s(syn_msg->auc_msg_body, syn_msg->us_len, puc_param, (hi_u32)us_len) != EOK) {
            frw_event_free(event_mem);
            oam_error_log0(0, OAM_SF_CFG, "hmac_config_send_event:: puc_param memcpy_s fail.");
            return HI_FAIL;
        }
    }
    /* 抛出事件 */
    ret = hcc_hmac_tx_control_event(event_mem,
        us_len + (hi_u16)oal_offset_of(hmac_to_dmac_cfg_msg_stru, auc_msg_body));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_send_event::frw_event_dispatch_event failed[%d].}", ret);
        frw_event_free(event_mem);
        return ret;
    }

    frw_event_free(event_mem);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 抛start vap事件
 修改历史      :
  1.日    期   : 2015年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_start_vap_event(const mac_vap_stru *mac_vap, hi_u8 mgmt_rate_init_flag)
{
    hi_u32 ret;
    mac_cfg_start_vap_param_stru start_vap_param;

    /* DMAC不使用netdev成员 */
    start_vap_param.net_dev = HI_NULL;
    start_vap_param.mgmt_rate_init_flag = mgmt_rate_init_flag;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    start_vap_param.protocol = mac_vap->protocol;
    start_vap_param.band = mac_vap->channel.band;
    start_vap_param.uc_bandwidth = mac_vap->channel.en_bandwidth;
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    start_vap_param.p2p_mode = mac_vap->p2p_mode;
#endif

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_START_VAP, sizeof(mac_cfg_start_vap_param_stru),
        (hi_u8 *)&start_vap_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap_event::Start_vap failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : 获取g_wlan_pm_switch结构
**************************************************************************** */
hi_bool hmac_get_wlan_pm_switch(hi_void)
{
    return g_wlan_pm_on;
}

/* ****************************************************************************
 功能描述  : 设置g_wlan_pm_switch结构
**************************************************************************** */
hi_void hmac_set_wlan_pm_switch(hi_bool wlan_pm_switch)
{
    g_wlan_pm_on = wlan_pm_switch;
}

/* ****************************************************************************
 功能描述  : 设置模式事件，抛事件给dmac侧
 修改历史      :
  1.日    期   : 2015年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_set_mode_event(const mac_vap_stru *mac_vap)
{
    hi_u32 ret;
    mac_cfg_mode_param_stru prot_param;

    /* 设置带宽模式，直接抛事件到DMAC配置寄存器 */
    prot_param.protocol = mac_vap->protocol;
    prot_param.band = mac_vap->channel.band;
    prot_param.en_bandwidth = mac_vap->channel.en_bandwidth;

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_MODE, sizeof(mac_cfg_mode_param_stru), (hi_u8 *)&prot_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log4(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_set_mode_event::mode_set failed[%d],protocol[%d], band[%d], bandwidth[%d].}", ret,
                         mac_vap->protocol, mac_vap->channel.band,
                         mac_vap->channel.en_bandwidth);
    }
    return ret;
}

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
/* ****************************************************************************
 功能描述  : 同步模式通知相关信息
 返 回 值  : hi_u32
 修改历史      :
  1.日    期   : 2015年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_update_opmode_event(mac_vap_stru *mac_vap, mac_user_stru *mac_user, hi_u8 mgmt_frm_type)
{
    hi_u32 relt;
    mac_user_opmode_stru user_opmode;
    /* opmode息同步dmac */
    user_opmode.avail_num_spatial_stream = mac_user->avail_num_spatial_stream;
    user_opmode.avail_bf_num_spatial_stream = mac_user->avail_bf_num_spatial_stream;
    user_opmode.avail_bandwidth = mac_user->avail_bandwidth;
    user_opmode.cur_bandwidth = mac_user->cur_bandwidth;
    user_opmode.user_idx = (hi_u8)mac_user->us_assoc_id;
    user_opmode.frame_type = mgmt_frm_type;

    relt = hmac_config_send_event(mac_vap, WLAN_CFGID_UPDATE_OPMODE, sizeof(mac_user_opmode_stru),
        (hi_u8 *)(&user_opmode));
    if (oal_unlikely(relt != HI_SUCCESS)) {
        oam_warning_log1(mac_user->vap_id, OAM_SF_CFG,
            "{hmac_config_update_opmode_event::opmode_event send failed[%d].}", relt);
    }
    return relt;
}
#endif

/* ****************************************************************************
 功能描述  : 通用的从hmac同步命令到dmac函数
 修改历史      :
  1.日    期   : 2013年5月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_sync_cmd_common(const mac_vap_stru *mac_vap, wlan_cfgid_enum_uint16 cfg_id, hi_u16 us_len,
    const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, cfg_id, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_sync_cmd_common::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :
 修改历史      :
  1.日    期   : 2014年5月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_config_normal_check_vap_num(const mac_device_stru *mac_dev, const mac_cfg_add_vap_param_stru *param)
{
    if (param == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_config_normal_check_vap_num::param is null.}");
        return HI_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    if (param->p2p_mode != WLAN_LEGACY_VAP_MODE) {
        /* P2P VAP数量校验 */
        return hmac_p2p_check_vap_num(mac_dev, param->p2p_mode);
    }
#endif

    if ((param->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        || (param->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        /* AP数量不超过1 */
        if ((mac_dev->vap_num - mac_dev->sta_num) >= WLAN_AP_NUM_PER_DEVICE) {
            oam_warning_log0(0, OAM_SF_CFG,
                "{hmac_config_normal_check_vap_num::create vap fail, because at least 1 ap exist.}");
            return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
        }
    } else if (param->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        if (mac_dev->sta_num >= WLAN_STA_NUM_PER_DEVICE) {
            /* 已创建的STA个数达到最大值 */
            oam_warning_log1(0, OAM_SF_CFG,
                "{hmac_config_normal_check_vap_num::create vap fail, because sta num [%d] is more than 2.}",
                mac_dev->sta_num);
            return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
        }
    }

    return HI_SUCCESS;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/* ****************************************************************************
 功能描述  : 创建配置vap抛事件
 修改历史      :
  1.日    期   : 2013年5月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_cfg_vap_send_event(const mac_device_stru *mac_dev)
{
    hi_unref_param(mac_dev);
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hi_u32 ret;

    /* 抛事件给DMAC,让DMAC完成配置VAP创建 */
    event_mem = frw_event_alloc(0);
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_cfg_vap_send_event::event_mem null.}");
        return HI_FAIL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CRX, HMAC_TO_DMAC_SYN_CREATE_CFG_VAP, 0,
        FRW_EVENT_PIPELINE_STAGE_1, WLAN_CFG_VAP_ID);

    ret = hcc_hmac_tx_control_event(event_mem, sizeof(hi_u16));
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{hmac_cfg_vap_send_event::frw_event_dispatch_event failed[%d].}", ret);
    }

    /* 释放事件 */
    frw_event_free(event_mem);

    return ret;
}
#endif

/* ****************************************************************************
 功能描述      : 配置系统低功耗开关
 修改历史      :
  1.日    期   : 2018年12月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_pm_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    hi_u32 pm_cfg;
    hmac_vap_stru *hmac_vap = HI_NULL;
    mac_vap_stru *vap = HI_NULL; /* 业务vap */
    mac_device_stru *mac_dev = mac_res_get_dev();
    hi_u8 vap_idx;

    pm_cfg = *(hi_u32 *)puc_param;
    /* 寻找STA */
    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (vap == HI_NULL) {
            continue;
        }
        if (vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
            break;
        }
    }

    if (vap != HI_NULL) {
        hmac_vap = hmac_vap_get_vap_stru(vap->vap_id);
        if (hmac_vap == HI_NULL) {
            oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_pm_switch::pst_hmac_vap null.}");
            return HI_ERR_CODE_PTR_NULL;
        }

        if ((pm_cfg & BIT0) == MAC_STA_PM_SWITCH_OFF) {
            if (hmac_vap->ps_sw_timer.is_registerd == HI_TRUE) {
                frw_timer_immediate_destroy_timer(&(hmac_vap->ps_sw_timer));
            }
        }
    }

    /*
     * PM_SWITCH 和 DTIM_TIMES 复用参数
     * PM_SWITCH BIT[0]
     * DTIM_TIMES BIT[31:1]
     */
    if ((pm_cfg & BIT0) == MAC_STA_PM_SWITCH_ON) {
        hmac_set_wlan_pm_switch(HI_TRUE);
    } else {
        hmac_set_wlan_pm_switch(HI_FALSE);
        if (g_pm_apdown_timer.is_registerd == HI_TRUE) {
            frw_timer_immediate_destroy_timer(&g_pm_apdown_timer);
        }
    }

    /* **************************************************************************
    抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_PM_SWITCH, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_pm_switch::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}

static hi_u32 hmac_config_init_hmac_vap(hi_u8 vap_id, hmac_vap_stru *hmac_vap, mac_device_stru *mac_dev,
    mac_cfg_add_vap_param_stru *param)
{
    hi_u32 ret;
    param->vap_id = vap_id;
    /* 初始化HMAC VAP */
    ret = hmac_vap_init(hmac_vap, vap_id, param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_init_hmac_vap::hmac_vap_init failed[%d].}", ret);
        if (hmac_vap->base_vap->mib_info != HI_NULL) {
            oal_mem_free(hmac_vap->base_vap->mib_info);
        }

        /* 异常处理，释放内存 */
        mac_vap_free_vap_res(vap_id);
        return ret;
    }

    /* 设置反挂的net_device指针 */
#ifdef _PRE_WLAN_FEATURE_P2P
    if (param->p2p_mode == WLAN_P2P_DEV_MODE) {
        /* p2p0 DEV 模式vap，采用pst_p2p0_net_device 成员指向对应的net_device */
        hmac_vap->p2p0_net_device = param->net_dev;
        mac_dev->p2p_info.p2p0_vap_idx = hmac_vap->base_vap->vap_id;
    }
#endif
    hmac_vap->net_device = param->net_dev;

    /* 将申请到的mac_vap空间挂到net_device ml_priv指针上去 */
    oal_net_dev_priv(param->net_dev) = hmac_vap->base_vap;
    /* 申请hmac组播用户 */
    hmac_user_add_multi_user(hmac_vap->base_vap, &param->muti_user_id);
    mac_vap_set_multi_user_idx(hmac_vap->base_vap, param->muti_user_id);
    mac_device_set_vap_id(mac_dev, hmac_vap->base_vap, param, vap_id, HI_TRUE);

    if (param->vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || param->vap_mode == WLAN_VAP_MODE_MESH
#endif
    ) {
        param->uapsd_enable = hmac_vap->base_vap->cap_flag.uapsd;
    }
    return HI_SUCCESS;
}

static hi_u32 hmac_config_set_station_id(const hmac_vap_stru *hmac_vap, mac_device_stru *mac_dev, hi_u8 vap_id,
    const mac_cfg_add_vap_param_stru *param)
{
    mac_cfg_staion_id_param_stru station_id_param = { 0 };
    hi_u32 ret;
    /* 设置mac地址 */
    if (memcpy_s(station_id_param.auc_station_id, WLAN_MAC_ADDR_LEN,
        param->net_dev->macAddr, WLAN_MAC_ADDR_LEN) != EOK) {
        /* 此处回退有误，需要对应mac_device_set_vap_id，做回退操作 */
        mac_device_set_vap_id(mac_dev, hmac_vap->base_vap, param, vap_id, HI_FALSE);

        /* 异常处理，释放内存 */
        oal_mem_free(hmac_vap->base_vap->mib_info);

        mac_vap_free_vap_res(vap_id);
        oam_error_log0(0, OAM_SF_CFG, "{hmac_config_set_station_id::mem safe function err!}");
        return HI_FAIL;
    }
    station_id_param.p2p_mode = param->p2p_mode;
    ret = hmac_config_set_mac_addr(hmac_vap->base_vap, sizeof(mac_cfg_staion_id_param_stru),
        (hi_u8 *)(&station_id_param));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        /* 此处回退有误，需要对应mac_device_set_vap_id，做回退操作 */
        mac_device_set_vap_id(mac_dev, hmac_vap->base_vap, param, vap_id, HI_FALSE);

        /* 异常处理，释放内存 */
        oal_mem_free(hmac_vap->base_vap->mib_info);

        mac_vap_free_vap_res(vap_id);
        oam_error_log1(0, OAM_SF_CFG, "{hmac_config_set_station_id::hmac_config_set_mac_addr failed[%d].}", ret);
        return ret;
    }

    return HI_SUCCESS;
}
/* ****************************************************************************
 功能描述  : 创建HMAC 业务VAP
 输入参数  : pst_vap   : 指向配置vap
             us_len    : 参数长度
             puc_param : 参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_add_vap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    mac_device_stru *mac_dev = mac_res_get_dev();

    if (oal_unlikely((mac_vap == HI_NULL) || (puc_param == HI_NULL))) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_add_vap::param null,pst_vap=%p puc_param=%p.}",
                       (uintptr_t)mac_vap, (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }
    mac_cfg_add_vap_param_stru *param = (mac_cfg_add_vap_param_stru *)puc_param;
#ifdef _PRE_WLAN_FEATURE_P2P
    if (param->p2p_mode == WLAN_P2P_CL_MODE) {
        return hmac_p2p_add_gc_vap(mac_dev, us_len, puc_param);
    }
#endif
    /* VAP个数判断 */
    ret = hmac_config_normal_check_vap_num(mac_dev, param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* 从资源池申请 vap id */
    hi_u8 vap_id = mac_vap_alloc_vap_res();
    if (oal_unlikely(vap_id == MAC_VAP_RES_ID_INVALID)) {
        return HI_FAIL;
    }
    /* 从资源池获取新申请到的hmac vap */
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(vap_id);
    if (hmac_vap == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    ret = hmac_config_init_hmac_vap(vap_id, hmac_vap, mac_dev, param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(hmac_vap->base_vap, WLAN_CFGID_ADD_VAP, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        /* 此处回退有误，需要对应mac_device_set_vap_id，做回退操作 */
        mac_device_set_vap_id(mac_dev, hmac_vap->base_vap, param, vap_id, HI_FALSE);

        /* 异常处理，释放内存 */
        oal_mem_free(hmac_vap->base_vap->mib_info);

        mac_vap_free_vap_res(vap_id);

        oam_error_log1(0, OAM_SF_CFG, "{hmac_config_add_vap::hmac_config_alloc_event failed[%d].}", ret);
        return ret;
    }
    /* 设置帧过滤 */
    hmac_set_rx_filter_value(hmac_vap->base_vap);

    /* 设置station id */
    ret = hmac_config_set_station_id(hmac_vap, mac_dev, vap_id, param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    oam_warning_log2(vap_id, OAM_SF_ANY, "{hmac_config_add_vap::add vap [%d] success! vap_id is %d", param->vap_mode,
        param->vap_id);

    return HI_SUCCESS;
}

hi_void hmac_config_del_timer_user_vap(hmac_vap_stru *hmac_vap)
{
    /* 清理所有的timer */
    if (hmac_vap->mgmt_timer.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(hmac_vap->mgmt_timer));
        hmac_vap->mgmt_timer.is_registerd = HI_FALSE;
    }
    if (hmac_vap->scan_timeout.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(hmac_vap->scan_timeout));
        hmac_vap->scan_timeout.is_registerd = HI_FALSE;
    }
    if (hmac_vap->scanresult_clean_timeout.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(hmac_vap->scanresult_clean_timeout));
        hmac_vap->scanresult_clean_timeout.is_registerd = HI_FALSE;
    }
#ifdef _PRE_WLAN_FEATURE_STA_PM
    if (hmac_vap->ps_sw_timer.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(hmac_vap->ps_sw_timer));
        hmac_vap->ps_sw_timer.is_registerd = HI_FALSE;
    }
#endif
}

hi_u32 hmac_config_del_timer_user(mac_vap_stru *mac_vap, hmac_vap_stru *hmac_vap)
{
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || (mac_vap->vap_mode == (hi_u8)WLAN_VAP_MODE_MESH)
#endif
    ) {
        hmac_vap->edca_opt_flag_ap = 0;
        frw_timer_immediate_destroy_timer(&(hmac_vap->edca_opt_timer));
    } else if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        hmac_vap->edca_opt_flag_sta = 0;
    }
#endif

    /* 如果是配置VAP, 去注册配置vap对应的net_device, 释放，返回 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_CONFIG) {
        /* 在注销netdevice之前先将指针赋为空 */
        oal_net_device_stru *netdev = hmac_vap->net_device;
        hmac_vap->net_device = HI_NULL;
        oal_net_unregister_netdev(netdev);
        oal_net_free_netdev(netdev);
        mac_vap_free_vap_res(mac_vap->vap_id);
        return HI_SUCCESS;
    }

    /* 业务vap net_device已在WAL释放，此处置为null */
#ifdef _PRE_WLAN_FEATURE_P2P
    /* 针对p2p0,需要删除hmac 中对应的p2p0 netdevice 指针 */
    hmac_vap->p2p0_net_device = (mac_vap->p2p_mode == WLAN_P2P_DEV_MODE) ? HI_NULL : hmac_vap->p2p0_net_device;
#endif
    hmac_vap->net_device = HI_NULL;
    if (hmac_vap->puc_asoc_req_ie_buff != HI_NULL) {
        oal_mem_free(hmac_vap->puc_asoc_req_ie_buff);
        hmac_vap->puc_asoc_req_ie_buff = HI_NULL;
    }
    hmac_config_del_timer_user_vap(hmac_vap);

    mac_vap_exit(mac_vap);

    return HI_CONTINUE;
}

/* ****************************************************************************
 功能描述  : 删除vap
 输入参数  : pst_vap   : 指向vap的指针
             us_len    : 参数长度
             puc_param : 参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2013年5月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_del_vap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *param)
{
    if (oal_unlikely((mac_vap == HI_NULL) || (param == HI_NULL))) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_del_vap:vap=%p,param=%p}", (uintptr_t)mac_vap, (uintptr_t)param);
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_del_vap::hmac_vap_get_vap_stru failed.}");
        return HI_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    if (mac_vap->p2p_mode == WLAN_P2P_CL_MODE) {
        return hmac_p2p_del_gc_vap(mac_vap, us_len, param);
    }
#endif

    if (mac_vap->vap_state != MAC_VAP_STATE_INIT) {
        oam_warning_log2(0, 0, "{hmac_config_del_vap:state=%d,mode=%d}", mac_vap->vap_state, mac_vap->vap_mode);
        return HI_FAIL;
    }

    hi_u32 ret = hmac_config_del_timer_user(mac_vap, hmac_vap);
    if (ret != HI_CONTINUE) {
        return ret;
    }
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /* liteos需要超时下发睡眠指令到device侧 */
    if (((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)) &&
        g_wlan_pm_on == HI_TRUE) {
        if (g_pm_apdown_timer.is_registerd == HI_TRUE) {
            frw_timer_immediate_destroy_timer(&g_pm_apdown_timer);
        }
        frw_timer_create_timer(&g_pm_apdown_timer, hmac_set_psm_timeout,
                               PM_APDOWN_ENTERY_TIME, (hi_void *)hmac_vap, HI_FALSE);
    }
#endif
    mac_vap_free_vap_res(mac_vap->vap_id);

    /* **************************************************************************
                          抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DEL_VAP, us_len, param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        /* 不退出，保证Devce挂掉的情况下可以下电 */
        oam_error_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_del_vap::hmac_config_send_event failed[%d].}", ret);
    }

    oam_warning_log3(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_del_vap:Del succ.vap_mode=%d,p2p_mode=%d,user idx[%d]}",
        mac_vap->vap_mode, mac_vap->p2p_mode, mac_vap->multi_user_idx);
    /* 等dmac清理完组播用户数据之后再释放组播用户 */
    hmac_user_del_multi_user(mac_vap->multi_user_idx);

    /* 业务vap已删除，从device上去掉 */
    mac_device_stru *mac_dev = mac_res_get_dev();
    oam_warning_log1(0, OAM_SF_ANY, "uc_vap_num = %d", mac_dev->vap_num);
    if (mac_dev->vap_num == 0) {
        hmac_config_host_dev_exit(mac_vap, 0, HI_NULL);
        wlan_pm_close();
    }

    return ret;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/* ****************************************************************************
 功能描述  : 配置默认频带，信道，带宽
 输入参数  : pst_mac_vap : 指向vap
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2015年3月37日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_def_chan(mac_vap_stru *mac_vap)
{
    hi_u8 channel;
    mac_cfg_mode_param_stru param;

    if (((mac_vap->channel.band == WLAN_BAND_BUTT) ||
         (mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_BUTT) ||
         (mac_vap->protocol == WLAN_PROTOCOL_BUTT)) &&
        (!is_p2p_go(mac_vap))) {
        param.band = WLAN_BAND_2G;
        param.en_bandwidth = WLAN_BAND_WIDTH_20M;
        param.protocol = WLAN_HT_MODE;
        hmac_config_set_mode(mac_vap, sizeof(param), (hi_u8 *)&param);
    }

    if ((mac_vap->channel.chan_number == 0) && (!is_p2p_go(mac_vap))) {
        mac_vap->channel.chan_number = 6; /* number 赋值为 6 */
        channel = mac_vap->channel.chan_number;
        hmac_config_set_freq(mac_vap, sizeof(hi_u32), &channel);
    }

    return HI_SUCCESS;
}

#endif /* #if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) */

hi_u32 hmac_config_ap_mesh_start(mac_vap_stru *mac_vap, hmac_vap_stru *hmac_vap)
{
    /* P2P GO 创建后，未设置ssid 信息，设置为up 状态不需要检查ssid 参数 */
    hi_u8 *puc_ssid = mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_desired_ssid;
    if ((strlen((const hi_char *)puc_ssid) == 0) && (!is_p2p_go(mac_vap))) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::ssid length=0.}");
        return HI_FAIL; /* 没设置SSID，则不启动VAP */
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    hi_u8 *puc_meshid = mac_vap->mib_info->wlan_mib_mesh_sta_cfg.auc_dot11_mesh_id;
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_MESH) && (strlen((const hi_char *)puc_meshid) == 0)) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::mesh vap meshid length=0.}");
        return HI_FAIL; /* 没设置Meshid，则不启动VAP */
    }
#endif

    /* 设置AP侧状态机为 WAIT_START */
    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_AP_WAIT_START);

    if (is_legacy_vap(hmac_vap->base_vap)) {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        hmac_config_def_chan(mac_vap);
#endif
    }

    /* 这里 en_status 等于 MAC_CHNL_AV_CHK_NOT_REQ(无需检测) 或者 MAC_CHNL_AV_CHK_COMPLETE(检测完成) */
    /* 检查协议 频段 带宽是否设置 */
    if ((mac_vap->channel.band == WLAN_BAND_BUTT) || (mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_BUTT) ||
        (mac_vap->protocol == WLAN_PROTOCOL_BUTT)) {
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_INIT);

        if (is_p2p_go(mac_vap)) {
            /* wpa_supplicant 会先设置vap up， 此时并未给vap 配置信道、带宽和协议模式信息，
               wpa_supplicant 在cfg80211_start_ap 接口配置GO 信道、带宽和协议模式信息，
               故此处如果没有设置信道、带宽和协议模式，直接返回成功，不返回失败。 */
            oam_warning_log3(mac_vap->vap_id, OAM_SF_CFG,
                "{hmac_config_start_vap::set band bandwidth protocol first.band[%d], bw[%d], protocol[%d]}",
                mac_vap->channel.band, mac_vap->channel.en_bandwidth, mac_vap->protocol);
            return HI_SUCCESS;
        } else {
            oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::set bandwidth protocol first}");
            return HI_FAIL;
        }
    }

    /* 检查信道号是否设置 */
    if ((mac_vap->channel.chan_number == 0) && (!is_p2p_go(mac_vap))) {
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_INIT);
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::set channel number first.}");
        return HI_FAIL;
    }

    /* 设置bssid */
    mac_vap_set_bssid(mac_vap, mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN);

    /* 入网优化，不同频段下的能力不一样 */
    if (mac_vap->channel.band == WLAN_BAND_2G) {
        mac_mib_set_short_preamble_option_implemented(mac_vap, WLAN_LEGACY_11B_MIB_SHORT_PREAMBLE);
        mac_mib_set_spectrum_management_required(mac_vap, HI_FALSE);
    } else {
        mac_mib_set_short_preamble_option_implemented(mac_vap, WLAN_LEGACY_11B_MIB_LONG_PREAMBLE);
        mac_mib_set_spectrum_management_required(mac_vap, HI_TRUE);
    }

    /* 设置AP侧状态机为 UP */
    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_UP);

    /* allow sleep when ap up */
    wlan_pm_add_vote(HI_PM_ID_AP);

    return HI_CONTINUE;
}

/* ****************************************************************************
 函 数 名  : hmac_config_start_vap
 功能描述  : hmac启用VAP
 输入参数  : pst_mac_vap : 指向vap
             us_len      : 参数长度
             puc_param   : 参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2012年12月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_start_vap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *param)
{
    mac_cfg_start_vap_param_stru *start_vap_param = (mac_cfg_start_vap_param_stru *)param;

    if (oal_unlikely((mac_vap == HI_NULL) || (param == HI_NULL) || (us_len != us_len))) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_start_vap:vap=%p param=%p}", (uintptr_t)mac_vap, (uintptr_t)param);
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_vap_state_enum_uint8 state = mac_vap->vap_state;
    if (state == MAC_VAP_STATE_BUTT) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::the vap has been deleted.}");
        return HI_FAIL;
    }

    /* 如果已经在up状态，则返回成功 */
    if ((state == MAC_VAP_STATE_UP) || (state == MAC_VAP_STATE_AP_WAIT_START) || (state == MAC_VAP_STATE_STA_FAKE_UP)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::state=%d,duplicate start}", state);
        return HI_SUCCESS;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        hi_u32 ap_ret = hmac_config_ap_mesh_start(mac_vap, hmac_vap);
        if (ap_ret != HI_CONTINUE) {
            return ap_ret;
        }
    } else if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
#ifdef _PRE_WLAN_FEATURE_P2P
        /* p2p0和p2p-p2p0 共VAP 结构，对于p2p cl不用修改vap 状态 */
        if (start_vap_param->p2p_mode != WLAN_P2P_CL_MODE) {
            hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);
        }
#else
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);
#endif
    } else {
        /* 其它分支 暂不支持 待开发 */
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::mode[%d]Err}", mac_vap->vap_mode);
    }

    mac_vap_init_rates(mac_vap);

    hi_u32 ret = hmac_config_start_vap_event(mac_vap, start_vap_param->mgmt_rate_init_flag);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap::hmac_config_send_event Err=%d}", ret);
        return ret;
    }

    oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_start_vap:host start vap ok}");

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 按照指定的协议模式更新VAP速率集
 输入参数  : pst_mac_vap : 指向vap
             pst_cfg_mode: 协议模式相关参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2014年8月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_sta_update_rates(mac_vap_stru *mac_vap, const mac_cfg_mode_param_stru *cfg_mode)
{
    hi_u32 ret;
    hmac_vap_stru *hmac_vap = HI_NULL;

    if (mac_vap->vap_state == MAC_VAP_STATE_BUTT) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_sta_update_rates::the vap has been deleted.}");

        return HI_FAIL;
    }

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_sta_update_rates::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (cfg_mode->protocol >= WLAN_HT_MODE) {
        hmac_vap->tx_aggr_on = HI_TRUE;
    } else {
        hmac_vap->tx_aggr_on = HI_FALSE;
    }

    mac_vap_init_by_protocol(mac_vap, cfg_mode->protocol);
    mac_vap->channel.band = cfg_mode->band;
    mac_vap->channel.en_bandwidth = cfg_mode->en_bandwidth;
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_start_vap_event(mac_vap, HI_FALSE);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_sta_update_rates::hmac_config_send_event failed[%d].}", ret);
        mac_vap_init_by_protocol(mac_vap, hmac_vap->preset_para.protocol);
        mac_vap->channel.band = hmac_vap->preset_para.band;
        mac_vap->channel.en_bandwidth = hmac_vap->preset_para.en_bandwidth;
        return ret;
    }

    return HI_SUCCESS;
}

hi_u32 hmac_config_del_user(mac_vap_stru *mac_vap, const hmac_vap_stru *hmac_vap)
{
    hi_list *user_list_head = &(mac_vap->mac_user_list_head);
    hi_list *entry = HI_NULL;

    for (entry = user_list_head->next; entry != user_list_head;) {
        mac_user_stru *user_tmp = hi_list_entry(entry, mac_user_stru, user_dlist);
        hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru((hi_u8)user_tmp->us_assoc_id);
        if (hmac_user == HI_NULL) {
            continue;
        }

        /* 指向双向链表下一个 */
        entry = entry->next;

        /* 管理帧加密是否开启 */
        hi_u8 is_protected = user_tmp->cap_info.pmf_active;
#ifdef _PRE_WLAN_FEATURE_MESH
        if (user_tmp->is_mesh_user == HI_TRUE) {
            hmac_handle_close_peer_mesh(hmac_vap, hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN,
                HMAC_REPORT_DISASSOC, MAC_DISAS_LV_SS);
            /* 删除用户 (先在驱动删除) */
            hmac_user_del(mac_vap, hmac_user);
        } else {
#endif
            /* 发去关联帧 */
            hmac_mgmt_send_disassoc_frame(mac_vap, user_tmp->user_mac_addr, MAC_DISAS_LV_SS, is_protected);

            /* 删除用户事件上报给上层 */
            if (is_ap(mac_vap)) {
                hmac_handle_disconnect_rsp_ap(hmac_vap, hmac_user);
            } else if (is_sta(mac_vap)) {
                hmac_sta_disassoc_rsp(hmac_vap, MAC_DISAS_LV_SS, DMAC_DISASOC_MISC_KICKUSER);
            }

            /* 删除用户 */
            hmac_user_del(mac_vap, hmac_user);
#ifdef _PRE_WLAN_FEATURE_MESH
        }
#endif
    }

    /* VAP下user链表应该为空 */
    if (hi_is_list_empty_optimize(&mac_vap->mac_user_list_head) == HI_FALSE) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_down_vap::st_mac_user_list_head is not empty.}");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 停用vap
 输入参数  : pst_mac_vap : 指向vap
             us_len      : 参数长度
             puc_param   : 参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2013年5月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_down_vap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *param)
{
    if (oal_unlikely((mac_vap == HI_NULL) || (param == HI_NULL))) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_down_vap:vap=%p param=%p}", (uintptr_t)mac_vap, (uintptr_t)param);
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_cfg_down_vap_param_stru *param_tmp = (mac_cfg_down_vap_param_stru *)param;

    /* 如果vap已经在down的状态，直接返回 */
    if (mac_vap->vap_state == MAC_VAP_STATE_INIT) {
        /* 设置net_device里flags标志 */
        if ((param_tmp->net_dev != HI_NULL) && (oal_netdevice_flags(param_tmp->net_dev) & OAL_IFF_RUNNING)) {
            oal_netdevice_flags(param_tmp->net_dev) &= (~OAL_IFF_RUNNING);
        }

        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_down_vap::vap already down.}");
        return HI_SUCCESS;
    }

    mac_device_stru *mac_dev    = mac_res_get_dev();
    hmac_vap_stru   *hmac_vap   = hmac_vap_get_vap_stru(mac_vap->vap_id);
    mac_user_stru   *multi_user = mac_user_get_user_stru(mac_vap->multi_user_idx);
    if ((hmac_vap == HI_NULL) || (multi_user == HI_NULL)) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_down_vap::hmac_vap[%p]/multi_user[%p] null}",
            (uintptr_t)hmac_vap, (uintptr_t)multi_user);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 设置net_device里flags标志 */
    if (param_tmp->net_dev != HI_NULL) {
        oal_netdevice_flags(param_tmp->net_dev) &= (~OAL_IFF_RUNNING);
    }

    /* 遍历vap下所有用户, 删除用户 */
    if (hmac_config_del_user(mac_vap, hmac_vap) != HI_SUCCESS) {
        return HI_FAIL;
    }

    /* 初始化组播用户的安全信息 */
    mac_user_init_key(multi_user);
    multi_user->user_tx_info.security.cipher_key_type = WLAN_KEY_TYPE_TX_GTK;

    /* sta模式时 将desired ssid MIB项置空 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        if (memset_s(mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_desired_ssid, WLAN_SSID_MAX_LEN, 0,
            WLAN_SSID_MAX_LEN) != EOK) {
            return HI_FAIL;
        }
    } else if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) { /* AP down状态需要投sleep票 */
        wlan_pm_remove_vote(HI_PM_ID_AP);
    }

    /* **************************************************************************
                         抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DOWN_VAP, us_len, param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_down_vap::hmac_config_send_event Err[%d].}", ret);
        return ret;
    }

    mac_vap_state_enum_uint8 vap_state = MAC_VAP_STATE_INIT;
#ifdef _PRE_WLAN_FEATURE_P2P
    vap_state = (param_tmp->p2p_mode == WLAN_P2P_CL_MODE) ? MAC_VAP_STATE_STA_SCAN_COMP : vap_state;
#endif
    mac_vap_state_change(mac_vap, vap_state);

    hmac_vap->auth_mode = WLAN_WITP_AUTH_OPEN_SYSTEM;
    hmac_set_rx_filter_value(mac_vap);

    oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_down_vap::SUCC!Now remaining%d vap}", mac_dev->vap_num);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取模式 包括协议 频段 带宽
 输入参数  : pst_mac_vap: 指向vap的指针
 输出参数  : pus_len    : 参数长度
             puc_param  : 参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2012年12月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_mode(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    mac_cfg_mode_param_stru *prot_param = HI_NULL;

    prot_param = (mac_cfg_mode_param_stru *)puc_param;

    prot_param->protocol = mac_vap->protocol;
    prot_param->band = mac_vap->channel.band;
    prot_param->en_bandwidth = mac_vap->channel.en_bandwidth;

    *pus_len = sizeof(mac_cfg_mode_param_stru);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置mode时，协议 频段 带宽参数检查
 输入参数  : pst_mac_device: device结构体
             pst_prot_param: pst_prot_param配置命令下发的参数
 修改历史      :
  1.日    期   : 2013年7月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_config_device_check_param(const mac_device_stru *mac_dev, wlan_protocol_enum_uint8 protocol)
{
    switch (protocol) {
        case WLAN_LEGACY_11A_MODE:
        case WLAN_LEGACY_11B_MODE:
        case WLAN_LEGACY_11G_MODE:
        case WLAN_MIXED_ONE_11G_MODE:
        case WLAN_MIXED_TWO_11G_MODE:
            break;

        case WLAN_HT_MODE:
        case WLAN_HT_ONLY_MODE:
        case WLAN_HT_11G_MODE:
            if (mac_dev->protocol_cap < WLAN_PROTOCOL_CAP_HT) {
                /* 设置11n协议，但device不支持HT模式 */
                oam_warning_log2(0, OAM_SF_CFG,
                    "{hmac_config_device_check_param::not support HT mode,en_protocol=%d en_protocol_cap=%d.}",
                    protocol, mac_dev->protocol_cap);
                return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
            }
            break;

        case WLAN_VHT_MODE:
        case WLAN_VHT_ONLY_MODE:
            if (mac_dev->protocol_cap < WLAN_PROTOCOL_CAP_VHT) {
                /* 设置11ac协议，但device不支持VHT模式 */
                oam_warning_log2(0, OAM_SF_CFG,
                    "{hmac_config_device_check_param::not support VHT mode,en_protocol=%d en_protocol_cap=%d.}",
                    protocol, mac_dev->protocol_cap);
                return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
            }
            break;

        default:
            oam_warning_log0(0, OAM_SF_CFG, "{hmac_config_device_check_param::mode param does not in the list.}");
            break;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置mode时，协议 频段 带宽参数检查
 输入参数  : pst_mac_device: device结构体
             pst_prot_param: pst_prot_param配置命令下发的参数
 修改历史      :
  1.日    期   : 2013年7月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_config_check_mode_param(const mac_device_stru *mac_dev, const mac_cfg_mode_param_stru *prot_param)
{
    /* 根据device能力对参数进行检查 */
    hi_u32 ret = hmac_config_device_check_param(mac_dev, prot_param->protocol);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_check_mode_param::check_param Err=%d}", ret);
        return ret;
    }

    if ((prot_param->en_bandwidth > WLAN_BAND_WIDTH_40MINUS) && (mac_dev->bandwidth_cap < WLAN_BW_CAP_80M)) {
        /* 设置80M带宽，但device能力不支持80M，返回错误码 */
        oam_warning_log2(0, OAM_SF_CFG,
            "{hmac_config_check_mode_param::not support 80MHz bandwidth,en_protocol=%d en_protocol_cap=%d.}",
            prot_param->en_bandwidth, mac_dev->bandwidth_cap);
        return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
    }

    if ((WLAN_BAND_2G != prot_param->band) || (WLAN_BAND_CAP_2G != mac_dev->band_cap)) {
        /* 设置2G频带，但device不支持2G */
        oam_warning_log2(0, OAM_SF_CFG,
            "{hmac_config_check_mode_param::not support 5GHz band,en_protocol=%d en_protocol_cap=%d.}",
            prot_param->band, mac_dev->band_cap);
        return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 非首次设置带宽时根据已配置带宽检查新配置带宽参数
 输入参数  : en_bw_device: 首次配置的带宽
             en_bw_config: 本次配置命令配置的带宽
 修改历史      :
  1.日    期   : 2013年11月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_config_set_mode_check_bandwith(wlan_channel_bandwidth_enum_uint8 bw_device,
    wlan_channel_bandwidth_enum_uint8 bw_config)
{
    /* 要配置带宽是20M */
    if (WLAN_BAND_WIDTH_20M == bw_config) {
        return HI_SUCCESS;
    }

    /* 要配置带宽与首次配置带宽相同 */
    if (bw_device == bw_config) {
        return HI_SUCCESS;
    }

    switch (bw_device) {
        case WLAN_BAND_WIDTH_80PLUSPLUS:
        case WLAN_BAND_WIDTH_80PLUSMINUS:
            if (WLAN_BAND_WIDTH_40PLUS == bw_config) {
                return HI_SUCCESS;
            }
            break;

        case WLAN_BAND_WIDTH_80MINUSPLUS:
        case WLAN_BAND_WIDTH_80MINUSMINUS:
            if (WLAN_BAND_WIDTH_40MINUS == bw_config) {
                return HI_SUCCESS;
            }
            break;

        default:
            break;
    }

    return HI_FAIL;
}

hi_u32 hmac_config_mac_vap_dev(mac_vap_stru *mac_vap, mac_device_stru *mac_dev,
    const mac_cfg_mode_param_stru *prot_param)
{
    /* 根据协议更新vap能力 */
    mac_vap_init_by_protocol(mac_vap, prot_param->protocol);
    mac_vap_init_rates(mac_vap);

    /* 根据带宽信息更新Mib */
    mac_vap_change_mib_by_bandwidth(mac_vap, prot_param->en_bandwidth);

    /* 更新device的频段及最大带宽信息 */
    if (mac_dev->max_bandwidth == WLAN_BAND_WIDTH_BUTT) {
        mac_dev->max_bandwidth = prot_param->en_bandwidth;
        mac_dev->max_band      = prot_param->band;
    }

    /* **************************************************************************
     抛事件到DMAC层, 配置寄存器
    ************************************************************************** */
    hi_u32 ret = hmac_set_mode_event(mac_vap);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_mode::hmac_config_send_event failed[%d]}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 设置模式 包括协议 频段 带宽
 输入参数  : pst_mac_vap: 指向VAP的指针
             us_len     : 参数长度
             puc_param  : 参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2012年12月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年5月5日
    作    者   : HiSilicon
    修改内容   : 函数整改，hmac配置流程应该是
                           1、参数校验并设置hmac vap下的信息
                           2、设置mac vap下的信息及mib信息
                           3、设置mac device下的信息
                           4、抛配置事件到dmac
**************************************************************************** */
hi_u32 hmac_config_set_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_unref_param(us_len);

    /* 获取device ,设置模式时，device下必须至少有一个vap */
    mac_device_stru *mac_dev = mac_res_get_dev();
    if (mac_dev->vap_num == 0) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_mode::no vap in device.}");
        return HI_ERR_CODE_MAC_DEVICE_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_mode::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 检查配置参数是否在device能力内 */
    mac_cfg_mode_param_stru *prot_param = (mac_cfg_mode_param_stru *)puc_param;
    hi_u32 ret = hmac_config_check_mode_param(mac_dev, prot_param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* device已经配置时，需要校验下频段、带宽是否一致 */
    if ((mac_dev->max_bandwidth != WLAN_BAND_WIDTH_BUTT) && (!mac_dev->dbac_enabled)) {
        if (mac_dev->max_band != prot_param->band) {
            oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_mode::previous vapBand=%d,mismatchWith=%d}",
                mac_dev->max_band, prot_param->band);
            return HI_FAIL;
        }

        ret = hmac_config_set_mode_check_bandwith(mac_dev->max_bandwidth, prot_param->en_bandwidth);
        if (ret != HI_SUCCESS) {
            oam_warning_log3(mac_vap->vap_id, OAM_SF_CFG,
                "{hmac_config_set_mode::config_set_mode_check_bandwith Err=%d,previous vap bandwidth[%d,current=%d]}",
                ret, mac_dev->max_bandwidth, prot_param->en_bandwidth);
            return ret;
        }
    }

    hmac_vap->tx_aggr_on = (prot_param->protocol >= WLAN_HT_MODE) ? HI_TRUE : HI_FALSE;

    wlan_channel_bandwidth_enum_uint8 cur_bw = mac_vap->channel.en_bandwidth;
    /* 更新STA协议配置标志位 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        hmac_vap->preset_para.protocol = prot_param->protocol;
        if ((cur_bw != WLAN_BAND_WIDTH_5M) && (cur_bw != WLAN_BAND_WIDTH_10M)) {
            hmac_vap->preset_para.en_bandwidth = prot_param->en_bandwidth;
        }
        hmac_vap->preset_para.band = prot_param->band;
    }

    /* 记录协议模式, band, bandwidth到mac_vap下 */
    mac_vap->protocol     = prot_param->protocol;
    mac_vap->channel.band = prot_param->band;

    mac_vap->channel.en_bandwidth = ((cur_bw != WLAN_BAND_WIDTH_5M) && (cur_bw != WLAN_BAND_WIDTH_10M)) ?
        prot_param->en_bandwidth : cur_bw;

    oam_info_log3(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_mode::protocol=%d, band=%d, bandwidth=%d.}",
        mac_vap->protocol, mac_vap->channel.band, mac_vap->channel.en_bandwidth);

    ret = hmac_config_mac_vap_dev(mac_vap, mac_dev, prot_param);
    return ret;
}

/* ****************************************************************************
 功能描述  : 设置stationID值，即MAC地址
 输入参数  : event_hdr:事件头
             pst_param    :参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2012年12月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_mac_addr(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
#ifdef _PRE_WLAN_FEATURE_P2P
    mac_cfg_staion_id_param_stru *station_id_param = HI_NULL;
    wlan_p2p_mode_enum_uint8 p2p_mode;
#endif
    hi_u32 ret;

#ifdef _PRE_WLAN_FEATURE_P2P
    /* P2P 设置MAC 地址mib 值需要区分P2P DEV 或P2P_CL/P2P_GO,P2P_DEV MAC 地址设置到p2p0 MIB 中 */
    station_id_param = (mac_cfg_staion_id_param_stru *)puc_param;
    p2p_mode = station_id_param->p2p_mode;
    if (p2p_mode == WLAN_P2P_DEV_MODE) {
        /* 如果是p2p0 device，则配置MAC 地址到auc_p2p0_dot11StationID 成员中 */
        if (memcpy_s(mac_vap->mib_info->wlan_mib_sta_config.auc_p2p0_dot11_station_id, WLAN_MAC_ADDR_LEN,
            station_id_param->auc_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, 0, "{hmac_config_set_mac_addr::memcpy_s fail.}");
            return HI_FAIL;
        }
    } else {
        /* 设置mib值, Station_ID */
        mac_mib_set_station_id(mac_vap, (hi_u8)us_len, puc_param);
    }
#else
    /* 设置mib值, Station_ID */
    mac_mib_set_station_id(mac_vap, (hi_u8)us_len, puc_param);
#endif

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_STATION_ID, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_mac_addr::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : hmac读SSID
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_ssid(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    /* 读取mib值 */
    return mac_mib_get_ssid(mac_vap, (hi_u8 *)pus_len, puc_param);
}

/* ****************************************************************************
 功能描述  : hmac设SSID
 输入参数  : event_hdr: 事件头
             us_len       : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_ssid(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 设置mib值 */
    mac_mib_set_ssid(mac_vap, (hi_u8)us_len, puc_param);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    return hmac_config_send_event(mac_vap, WLAN_CFGID_SSID, us_len, puc_param);
#else
    return HI_SUCCESS;
#endif
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 设置短前导码能力位
 修改历史      :
  1.日    期   : 2013年1月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_shpreamble(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 设置mib值 */
    mac_mib_set_shpreamble(mac_vap, (hi_u8)us_len, puc_param);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SHORT_PREAMBLE, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_shpreamble::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
#else
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : 读前导码能力位
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_shpreamble(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    /* 读mib值 */
    return mac_mib_get_shpreamble(mac_vap, (hi_u8 *)pus_len, puc_param);
}
#endif

/* ****************************************************************************
 功能描述  : 20M short gi能力设置
 输入参数  : event_hdr: 事件头
             us_len       : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_shortgi20(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_s32 l_value;
    hi_unref_param(us_len);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    shortgi_cfg_stru shortgi_cfg;

    shortgi_cfg.type = SHORTGI_20_CFG_ENUM;
#endif
    l_value = *((hi_s32 *)puc_param);

    if (l_value != 0) {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        shortgi_cfg.enable = HI_TRUE;
#endif
        mac_vap->mib_info->phy_ht.dot11_short_gi_option_in_twenty_implemented = HI_TRUE;
    } else {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        shortgi_cfg.enable = HI_FALSE;
#endif
        mac_vap->mib_info->phy_ht.dot11_short_gi_option_in_twenty_implemented = HI_FALSE;
    }

    /* hi1131-cb : Need to send to Dmac via sdio */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* 配置事件的子事件 WLAN_CFGID_SHORTGI 通过新加的接口函数取出关键数据存入skb后通过sdio发出 */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SHORTGI, SHORTGI_CFG_STRU_LEN, (hi_u8 *)&shortgi_cfg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_set_shortgi20::hmac_config_send_event failed[%u].}", ret);
    }
    return ret;
#else
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : 读取20M short gi
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_shortgi20(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    hi_s32 l_value;

    l_value = mac_vap->mib_info->phy_ht.dot11_short_gi_option_in_twenty_implemented;

    *((hi_s32 *)puc_param) = l_value;

    *pus_len = sizeof(l_value);

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 设置保护模式
 输入参数  : event_hdr: 事件头
             us_len       : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_prot_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_s32 l_value;

    l_value = *((hi_s32 *)puc_param);
    if (oal_unlikely(l_value >= WLAN_PROT_BUTT)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_prot_mode::invalid value[%d].}", l_value);
        return HI_ERR_CODE_INVALID_CONFIG;
    }
    mac_vap->protection.protection_mode = (hi_u8)l_value;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_PROT_MODE, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_prot_mode::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
#else
    hi_unref_param(us_len);
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : 读取保护模式
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_prot_mode(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    *((hi_s32 *)puc_param) = mac_vap->protection.protection_mode;
    *pus_len = sizeof(hi_s32);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置认证模式
 输入参数  : event_hdr: 事件头
             us_len       : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_auth_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 value;
    hmac_vap_stru *hmac_vap = HI_NULL;

    hi_unref_param(us_len);

    value = *((hi_u32 *)puc_param);
    /* 默认OPEN */
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_auth_mode::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap->auth_mode = WLAN_WITP_AUTH_OPEN_SYSTEM;
    if (value & BIT1) {
        hmac_vap->auth_mode = WLAN_WITP_AUTH_SHARED_KEY;
    }
    /* 支持OPEN跟SHARE KEY */
    if ((value & BIT0) && (value & BIT1)) {
        hmac_vap->auth_mode = WLAN_WITP_ALG_AUTH_BUTT;
    }

    oam_info_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_auth_mode::set auth mode[%d] succ.}",
        hmac_vap->auth_mode);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 读取认证模式
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_auth_mode(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    hmac_vap_stru *hmac_vap = HI_NULL;

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_get_auth_mode::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    *((hi_s32 *)puc_param) = hmac_vap->auth_mode;
    *pus_len = sizeof(hi_s32);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置beacon interval
 输入参数  : event_hdr: 事件头
             us_len       : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_bintval(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    mac_device_stru *mac_dev = HI_NULL;
    hi_u8 vap_idx;
    mac_vap_stru *mac_vap_tmp = HI_NULL;

    mac_dev = mac_res_get_dev();
    /* 设置device下的值 */
    mac_dev->beacon_interval = *((hi_u32 *)puc_param);
    /* 遍历device下所有vap */
    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap_tmp = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (mac_vap_tmp == HI_NULL) {
            continue;
        }

        /* 只有AP VAP需要beacon interval */
        if ((mac_vap_tmp->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
            || (mac_vap_tmp->vap_mode == WLAN_VAP_MODE_MESH)
#endif
        ) {
            /* 设置mib值 */
            mac_mib_set_beacon_period(mac_vap_tmp, (hi_u8)us_len, puc_param);
        }
    }

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_BEACON_INTERVAL, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_bintval::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 读取beacon interval
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_bintval(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    /* 读取mib值 */
    return mac_mib_get_beacon_period(mac_vap, (hi_u8 *)pus_len, puc_param);
}

/* ****************************************************************************
 功能描述  : 设置dtim period
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_dtimperiod(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 设置mib值 */
    mac_mib_set_dtim_period(mac_vap, (hi_u8)us_len, puc_param);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DTIM_PERIOD, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_bintval::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
#else
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : 读取dtim period
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_dtimperiod(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    /* 读取mib值 */
    return mac_mib_get_dtim_period(mac_vap, (hi_u8 *)pus_len, puc_param);
}
#endif

/* ****************************************************************************
 功能描述  : 设置发送功率
 输入参数  : event_hdr: 事件头
             us_len       : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_txpower(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_s32 l_value;
    hi_u32 ret;

    l_value = *((hi_s32 *)puc_param);

    mac_vap_set_tx_power(mac_vap, (hi_u8)l_value);

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_TX_POWER, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_txpower::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 读取发送功率
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_txpower(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    *((hi_s32 *)puc_param) = mac_vap->tx_power;
    *pus_len = sizeof(hi_s32);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置频率
 输入参数  : event_hdr: 事件头
             us_len       : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_freq(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *channel)
{
    mac_cfg_channel_param_stru  l_channel_param;
    mac_device_stru            *mac_dev = mac_res_get_dev();

    hi_u32 ret = mac_is_channel_num_valid(mac_vap->channel.band, (*channel));
    if (ret != HI_SUCCESS) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_freq::channel=%d,Err=%d}", (*channel), ret);
        return HI_ERR_CODE_INVALID_CONFIG;
    }

#ifdef _PRE_WLAN_FEATURE_11D
    /* 信道14特殊处理，只在11b协议模式下有效 */
    if (((*channel) == 14) && (mac_vap->protocol != WLAN_LEGACY_11B_MODE)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_freq::protocol=%d}", mac_vap->protocol);
        return HI_ERR_CODE_INVALID_CONFIG;
    }
#endif

    mac_vap->channel.chan_number = (*channel);
    ret = mac_get_channel_idx_from_num(mac_vap->channel.band, (*channel), &(mac_vap->channel.idx));
    if (ret != HI_SUCCESS) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_freq::get Channel Err.band=%u,channel=%u}",
            mac_vap->channel.band, mac_vap->channel.idx);
        return ret;
    }

    /* 非DBAC时，首次配置信道时设置到硬件 */
    if ((mac_dev->vap_num == 1) || (mac_dev->max_channel == 0)) {
        mac_device_get_channel(mac_dev, &l_channel_param);
        l_channel_param.channel = (*channel);
        mac_device_set_channel(mac_dev, &l_channel_param);

        /* **************************************************************************
            抛事件到DMAC层, 同步DMAC数据
        ************************************************************************** */
        ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CURRENT_CHANEL, us_len, channel);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_freq::hmac_config_send_event Err=%d}", ret);
            return ret;
        }
#ifdef _PRE_WLAN_FEATURE_DBAC
    } else if (mac_dev->dbac_enabled) {
        /* **************************************************************************
            抛事件到DMAC层, 同步DMAC数据
        ************************************************************************** */
        ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CURRENT_CHANEL, us_len, channel);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_freq::hmac_config_send_event Err=%d}", ret);
            return ret;
        }

        oam_info_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_freq::do not check channel while DBAC enabled.}");
#endif
    } else if (mac_dev->max_channel != (*channel)) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_freq::previous vap channel=%d,mismatch=%d}",
            mac_dev->max_channel, (*channel));

        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 读取频率
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_freq(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    *((hi_u32 *)puc_param) = mac_vap->channel.chan_number;

    *pus_len = sizeof(hi_u32);

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 设置WMM参数
 输入参数  : event_hdr: 事件头
             us_len       : 参数长度
             puc_param    : 参数
 修改历史      :
  1.日    期   : 2013年5月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* 规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 功能内聚，且被包含的宏没有打开, 建议屏蔽 */
hi_u32 hmac_config_set_wmm_params(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret = HI_SUCCESS;
    hi_u8 syn_flag; /* 默认不需要同步到dmac */

    syn_flag = HI_FALSE;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* offload模式下均需要同步到dmac */
    syn_flag = HI_TRUE;
#endif

    hmac_config_wmm_para_stru *cfg_stru = (hmac_config_wmm_para_stru *)puc_param;

    hi_u32 ac = cfg_stru->ac;
    hi_u32 value = cfg_stru->value;
    wlan_cfgid_enum_uint16 cfg_id = (hi_u16)cfg_stru->cfg_id;

    if (ac >= WLAN_WME_AC_BUTT) {
        oam_warning_log3(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_wmm_params::invalid param,en_cfg_id=%d, ul_ac=%d, ul_value=%d.}", cfg_id, ac, value);
        return HI_FAIL;
    }

    /* 根据sub-ioctl id填写WID */
    switch (cfg_id) {
        case WLAN_CFGID_EDCA_TABLE_CWMIN:
            if ((value > WLAN_QEDCA_TABLE_CWMIN_MAX) || (value < WLAN_QEDCA_TABLE_CWMIN_MIN)) {
                return HI_FAIL;
            }
            mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_c_wmin = value;
            break;
        case WLAN_CFGID_EDCA_TABLE_CWMAX:
            if ((value > WLAN_QEDCA_TABLE_CWMAX_MAX) || (value < WLAN_QEDCA_TABLE_CWMAX_MIN)) {
                return HI_FAIL;
            }
            mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_c_wmax = value;
            break;
        case WLAN_CFGID_EDCA_TABLE_AIFSN:
            if ((value < WLAN_QEDCA_TABLE_AIFSN_MIN) || (value > WLAN_QEDCA_TABLE_AIFSN_MAX)) {
                return HI_FAIL;
            }
            mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_aifsn = value;
            break;
        case WLAN_CFGID_EDCA_TABLE_TXOP_LIMIT:
            if (value > WLAN_QEDCA_TABLE_TXOP_LIMIT_MAX) {
                return HI_FAIL;
            }
            mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_txop_limit = value;
            break;
        case WLAN_CFGID_EDCA_TABLE_MSDU_LIFETIME:
            if (value > WLAN_QEDCA_TABLE_MSDU_LIFETIME_MAX) {
                return HI_FAIL;
            }
            mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_msdu_lifetime = value;
            break;
        case WLAN_CFGID_EDCA_TABLE_MANDATORY:
            if ((value != HI_TRUE) && (value != HI_FALSE)) {
                return HI_FAIL;
            }
            mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_mandatory = (hi_u8)value;
            break;
        case WLAN_CFGID_QEDCA_TABLE_CWMIN:
            if ((value > WLAN_QEDCA_TABLE_CWMIN_MAX) || (value < WLAN_QEDCA_TABLE_CWMIN_MIN)) {
                return HI_FAIL;
            }
            mac_vap->mib_info->wlan_mib_qap_edac[ac].dot11_qapedca_table_c_wmin = value;
            syn_flag = HI_TRUE;
            break;
        case WLAN_CFGID_QEDCA_TABLE_CWMAX:
            if ((value > WLAN_QEDCA_TABLE_CWMAX_MAX) || (value < WLAN_QEDCA_TABLE_CWMAX_MIN)) {
                return HI_FAIL;
            }
            mac_vap->mib_info->wlan_mib_qap_edac[ac].dot11_qapedca_table_c_wmax = value;
            syn_flag = HI_TRUE;
            break;
        case WLAN_CFGID_QEDCA_TABLE_AIFSN:
            if ((value < WLAN_QEDCA_TABLE_AIFSN_MIN) || (value > WLAN_QEDCA_TABLE_AIFSN_MAX)) {
                return HI_FAIL;
            }
            mac_vap->mib_info->wlan_mib_qap_edac[ac].dot11_qapedca_table_aifsn = value;
            syn_flag = HI_TRUE;
            break;
        case WLAN_CFGID_QEDCA_TABLE_TXOP_LIMIT:
            if (value > WLAN_QEDCA_TABLE_TXOP_LIMIT_MAX) {
                return HI_FAIL;
            }
            mac_vap->mib_info->wlan_mib_qap_edac[ac].dot11_qapedca_table_txop_limit = value;
            syn_flag = HI_TRUE;
            break;
        case WLAN_CFGID_QEDCA_TABLE_MSDU_LIFETIME:
            if (value > WLAN_QEDCA_TABLE_MSDU_LIFETIME_MAX) {
                return HI_FAIL;
            }
            mac_vap->mib_info->wlan_mib_qap_edac[ac].dot11_qapedca_table_msdu_lifetime = value;
            syn_flag = HI_TRUE;
            break;
        case WLAN_CFGID_QEDCA_TABLE_MANDATORY:
            /* offload模式下 才需要同步到dmac */
            if ((value != HI_TRUE) && (value != HI_FALSE)) {
                return HI_FAIL;
            }
            mac_vap->mib_info->wlan_mib_qap_edac[ac].dot11_qapedca_table_mandatory = (hi_u8)value;
            break;
        default:
            return HI_FAIL;
    }

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    if (syn_flag) {
        ret = hmac_config_send_event(mac_vap, cfg_id, us_len, puc_param);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_wmm_params::send_event failed[%d].}", ret);
        }
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : 读取EDCA参数
 输入参数  : event_hdr: 事件头
 输出参数  : pus_len      : 参数长度
             puc_param    : 参数
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_wmm_params(mac_vap_stru *mac_vap, hi_u16 *us_len, hi_u8 *puc_param)
{
    hmac_config_wmm_para_stru *cfg_stru = (hmac_config_wmm_para_stru *)puc_param;
    hi_u32                     value    = 0xFFFFFFFF;

    *us_len = sizeof(hmac_config_wmm_para_stru);

    if (cfg_stru->ac >= WLAN_WME_AC_BUTT) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_get_wmm_params::cfg_id=%d,ac=%d}", cfg_stru->cfg_id, cfg_stru->ac);
        return HI_FALSE;
    }

    /* 根据sub-ioctl id填写WID */
    if (cfg_stru->cfg_id == WLAN_CFGID_EDCA_TABLE_CWMIN) {
        value = mac_vap->mib_info->ast_wlan_mib_edca[cfg_stru->ac].dot11_edca_table_c_wmin;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_EDCA_TABLE_CWMAX) {
        value = mac_vap->mib_info->ast_wlan_mib_edca[cfg_stru->ac].dot11_edca_table_c_wmax;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_EDCA_TABLE_AIFSN) {
        value = mac_vap->mib_info->ast_wlan_mib_edca[cfg_stru->ac].dot11_edca_table_aifsn;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_EDCA_TABLE_TXOP_LIMIT) {
        value = mac_vap->mib_info->ast_wlan_mib_edca[cfg_stru->ac].dot11_edca_table_txop_limit;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_EDCA_TABLE_MSDU_LIFETIME) {
        value = mac_vap->mib_info->ast_wlan_mib_edca[cfg_stru->ac].dot11_edca_table_msdu_lifetime;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_EDCA_TABLE_MANDATORY) {
        value = mac_vap->mib_info->ast_wlan_mib_edca[cfg_stru->ac].dot11_edca_table_mandatory;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_QEDCA_TABLE_CWMIN) {
        value = mac_vap->mib_info->wlan_mib_qap_edac[cfg_stru->ac].dot11_qapedca_table_c_wmin;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_QEDCA_TABLE_CWMAX) {
        value = mac_vap->mib_info->wlan_mib_qap_edac[cfg_stru->ac].dot11_qapedca_table_c_wmax;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_QEDCA_TABLE_AIFSN) {
        value = mac_vap->mib_info->wlan_mib_qap_edac[cfg_stru->ac].dot11_qapedca_table_aifsn;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_QEDCA_TABLE_TXOP_LIMIT) {
        value = mac_vap->mib_info->wlan_mib_qap_edac[cfg_stru->ac].dot11_qapedca_table_txop_limit;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_QEDCA_TABLE_MSDU_LIFETIME) {
        value = mac_vap->mib_info->wlan_mib_qap_edac[cfg_stru->ac].dot11_qapedca_table_msdu_lifetime;
    } else if (cfg_stru->cfg_id == WLAN_CFGID_QEDCA_TABLE_MANDATORY) {
        value = mac_vap->mib_info->wlan_mib_qap_edac[cfg_stru->ac].dot11_qapedca_table_mandatory;
    }

    cfg_stru->value = value;
    return HI_SUCCESS;
}
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
hi_u32 hmac_config_set_reset_state(mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param)
{
    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_config_set_reset_state::pst_mac_vap[%p] NULL or pst_puc_param[%p] NULL!}",
            (uintptr_t)mac_vap, (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }
    hi_unref_param(mac_vap);
    hi_unref_param(us_len);
    hi_u32 ret = HI_SUCCESS;
    mac_reset_sys_stru *reset_sys = HI_NULL;
    mac_device_stru *mac_dev = HI_NULL;

    reset_sys = (mac_reset_sys_stru *)puc_param;
    mac_dev = mac_res_get_dev();
    mac_dev->reset_in_progress = reset_sys->value;
    return ret;
}
#endif

/* ****************************************************************************
 功能描述  : 设置Channnel时，协议 频段 带宽参数检查
 输入参数  : pst_mac_device: device结构体
             pst_prot_param: pst_prot_param配置命令下发的参数
 被调函数  :hmac_config_set_channel
 修改历史      :
  1.日    期   : 2014年8月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_config_set_channel_check_param(const mac_device_stru *mac_dev,
    const mac_cfg_channel_param_stru *prot_param)
{
    /* 根据device能力对参数进行检查 */
    if ((prot_param->en_bandwidth > WLAN_BAND_WIDTH_40MINUS) && (mac_dev->bandwidth_cap < WLAN_BW_CAP_80M)) {
        /* 设置80M带宽，但device能力不支持80M，返回错误码 */
        oam_warning_log2(0, OAM_SF_CFG,
            "{hmac_config_set_channel_check_param::not support 80MHz bandwidth,en_protocol=%d en_protocol_cap=%d.}",
            prot_param->en_bandwidth, mac_dev->bandwidth_cap);
        return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
    }

    if ((WLAN_BAND_2G != prot_param->band) || (WLAN_BAND_CAP_2G != mac_dev->band_cap)) {
        oam_warning_log2(0, OAM_SF_CFG,
            "{hmac_config_set_channel_check_param::not support 5GHz band,en_protocol=%d en_protocol_cap=%d.}",
            prot_param->band, mac_dev->band_cap);
        return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
    }
    return HI_SUCCESS;
}

hi_u32 hmac_config_vap_set_channel(mac_vap_stru *mac_vap, const mac_cfg_channel_param_stru *channel_param,
    const mac_device_stru *mac_dev, hi_u8 *set_reg)
{
    hi_u32 ret;
    hi_u8 vap_idx;

#ifdef _PRE_WLAN_FEATURE_DBAC
    if (mac_dev->dbac_enabled) {
        mac_vap->channel.chan_number  = channel_param->channel;
        mac_vap->channel.band         = channel_param->band;
        mac_vap->channel.en_bandwidth = channel_param->en_bandwidth;
        ret = mac_get_channel_idx_from_num(channel_param->band, channel_param->channel, &(mac_vap->channel.idx));
        if (ret != HI_SUCCESS) {
            oam_warning_log3(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_channel:get chl Err=%d,band=%d,channel=%d}",
                ret, channel_param->band, channel_param->channel);
            return HI_FAIL;
        }

        /* 根据带宽信息更新Mib */
        mac_vap_change_mib_by_bandwidth(mac_vap, channel_param->en_bandwidth);

        *set_reg = HI_TRUE;
    } else {
#endif /* _PRE_WLAN_FEATURE_DBAC */
        for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
            mac_vap_stru *mac_vap_tmp = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
            if (mac_vap_tmp == HI_NULL) {
                continue;
            }
            mac_vap_tmp->channel.chan_number  = channel_param->channel;
            mac_vap_tmp->channel.band         = channel_param->band;
            mac_vap_tmp->channel.en_bandwidth = channel_param->en_bandwidth;

            ret = mac_get_channel_idx_from_num(channel_param->band, channel_param->channel,
                                               &(mac_vap_tmp->channel.idx));
            if (ret != HI_SUCCESS) {
                oam_warning_log3(mac_vap_tmp->vap_id, OAM_SF_CFG, "{hmac_config_set_channel:Err=%d,band=%d,channel=%d}",
                    ret, channel_param->band, channel_param->channel);
                continue;
            }

            /* 根据带宽信息更新Mib */
            mac_vap_change_mib_by_bandwidth(mac_vap_tmp, channel_param->en_bandwidth);
        }
#ifdef _PRE_WLAN_FEATURE_DBAC
    }
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : HMAC 层设置信道信息
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_channel(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 set_reg = HI_FALSE;
    mac_cfg_channel_param_stru *channel_param = (mac_cfg_channel_param_stru *)puc_param;
    mac_device_stru            *mac_dev       = mac_res_get_dev();

    /* 检查配置参数是否在device能力内 */
    hi_u32 ret = hmac_config_set_channel_check_param(mac_dev, channel_param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* 仅在没有VAP up的情况下，配置硬件频带、带宽寄存器 */
    hi_u32 up_vap_cnt = hmac_calc_up_vap_num(mac_dev);
    if (up_vap_cnt <= 1) {
        /* 记录首次配置的带宽值 */
        mac_device_set_channel(mac_dev, channel_param);

        /* **************************************************************************
         抛事件到DMAC层, 配置寄存器  置标志位
        ************************************************************************** */
        set_reg = HI_TRUE;
#ifdef _PRE_WLAN_FEATURE_DBAC
    } else if (mac_dev->dbac_enabled) {
        /* 开启DBAC不进行信道判断 */
        /* 信道设置只针对AP模式，非AP模式则跳出 */
#endif /* _PRE_WLAN_FEATURE_DBAC */
    } else {
        /* 信道不是当前信道 */
        if (mac_dev->max_channel != channel_param->channel) {
            oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_channel::previous channel=%d mismatch[%d]}",
                mac_dev->max_channel, channel_param->channel);

            return HI_FAIL;
        }

        /* 带宽不能超出已配置的带宽 */
        ret = hmac_config_set_mode_check_bandwith(mac_dev->max_bandwidth, channel_param->en_bandwidth);
        if (ret != HI_SUCCESS) {
            oam_warning_log3(mac_vap->vap_id, OAM_SF_CFG,
                "{hmac_config_set_channel:hmac_config_set_mode_check_bandwith Err=%d,previous bandwidth=%d,current=%d}",
                ret, mac_dev->max_bandwidth, channel_param->en_bandwidth);
            return HI_FAIL;
        }
    }

    ret = hmac_config_vap_set_channel(mac_vap, channel_param, mac_dev, &set_reg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* **************************************************************************
     抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    if (set_reg == HI_TRUE) {
        ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CFG80211_SET_CHANNEL, us_len, puc_param);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_channel::send_event failed[%d]}", ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置mode时，协议 频段 带宽参数检查
 输入参数  : pst_mac_device: device结构体
             pst_prot_param: pst_prot_param配置命令下发的参数
 修改历史      :
  1.日    期   : 2015年6月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_config_set_beacon_check_param(const mac_device_stru *mac_dev,
    const mac_beacon_param_stru *prot_param)
{
    /* 根据device能力对参数进行检查 */
    return hmac_config_device_check_param(mac_dev, prot_param->protocol);
}

/* ****************************************************************************
 功能描述  : HMAC 层设置AP 信息
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_beacon(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 获取device */
    mac_device_stru *mac_dev = mac_res_get_dev();
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (puc_param == HI_NULL || hmac_vap == HI_NULL) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_beacon::puc_param/hmac_vap null! puc_param=%p, hmac_vap=%p.}", (uintptr_t)puc_param,
            (uintptr_t)hmac_vap);
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_beacon_param_stru *beacon_param = (mac_beacon_param_stru *)puc_param;

    /* 检查协议配置参数是否在device能力内 */
    hi_u32 ret = hmac_config_set_beacon_check_param(mac_dev, beacon_param);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_beacon::hmac_config_add_beacon_check_param failed[%d].}", ret);
        return ret;
    }
    hmac_vap->tx_aggr_on = (beacon_param->protocol >= WLAN_HT_MODE) ? HI_TRUE : HI_FALSE;

    /* 设置协议模式 */
    if ((beacon_param->privacy == HI_TRUE) && (beacon_param->crypto_mode & (WLAN_WPA_BIT | WLAN_WPA2_BIT))) {
        hmac_vap->auth_mode = WLAN_WITP_AUTH_OPEN_SYSTEM; /* 强制设置VAP 认证方式为OPEN */
    }
    mac_vap_set_hide_ssid(mac_vap, beacon_param->hidden_ssid);

    /* 1102适配新内核start ap和change beacon接口复用此接口，不同的是change beacon时，不再设置beacon周期
       和dtim周期，因此，change beacon时，interval和dtim period参数为全零，此时不应该被设置到mib中 */
    /* 设置VAP beacon interval， dtim_period */
    if ((beacon_param->l_dtim_period != 0) || (beacon_param->l_interval != 0)) {
        mac_vap->mib_info->wlan_mib_sta_config.dot11_dtim_period = (hi_u32)beacon_param->l_dtim_period;
        mac_vap->mib_info->wlan_mib_sta_config.dot11_beacon_period = (hi_u32)beacon_param->l_interval;
    }

    /* 设置short gi */
    mac_vap->mib_info->phy_ht.dot11_short_gi_option_in_twenty_implemented = beacon_param->shortgi_20;
    mac_mib_set_shortgi_option_in_forty_implemented(mac_vap, beacon_param->shortgi_40);

    if (beacon_param->operation_type == MAC_ADD_BEACON) {
        mac_vap_add_beacon(mac_vap, beacon_param);
    } else {
        mac_vap_set_beacon(mac_vap, beacon_param);
    }

    mac_vap_init_by_protocol(mac_vap, beacon_param->protocol);

    mac_vap_init_rates(mac_vap);

#ifdef _PRE_WLAN_FEATURE_MESH
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        mac_vap_set_mib_mesh(mac_vap, beacon_param->mesh_auth_protocol);
    }
#endif

    /* **************************************************************************
     抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CFG80211_CONFIG_BEACON, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_beacon::hmac_config_send_event fail[%d]}", ret);
    }

    return ret;
}

#ifdef _PRE_WLAN_FEATURE_BTCOEX
hi_u32 hmac_config_set_btcoex_en(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32           ret;
    mac_device_stru *mac_dev = HI_NULL;

    /* BT共存不支持多VAP和AP模式 */
    mac_dev = mac_res_get_dev();
    if ((hmac_calc_up_vap_num(mac_dev) > 1) || hmac_find_is_ap_up(mac_dev)) {
        hi_diag_log_msg_w0(0, "hmac_config_set_btcoex_en:: there is a up ap, don't support btcoex.");
        return HI_FAIL;
    }
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_BTCOEX_ENABLE, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        hi_diag_log_msg_w1(0, "{hmac_config_set_btcoex_en::send event return err code [%d].}", ret);
    }

    return ret;
}
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 函 数 名  : hmac_config_report_vap_info
 功能描述  : 根据flags位上报对应的vap信息
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_report_vap_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_REPORT_VAP_INFO, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_report_vap_info::hmac_config_send_event fail[%d].", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
/* ****************************************************************************
 功能描述  : rekey offload信息下发，抛事件到DMAC
 输入参数  : mac_vap_stru *pst_mac_vap, hi_u16 us_len, hi_u8 *puc_param
 输出参数  : hi_u32
 返 回 值  : 0:成功,其他:失败
 修改历史      :
  1.日    期   : 2016年8月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_rekey_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    /* **************************************************************************
    抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_REKEY, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_config_set_rekey_info::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}
#endif

#ifdef _PRE_WLAN_RF_110X_CALI_DPD
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_start_dpd(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
       抛事件到DMAC层, 同步DMAC数据
     ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_START_DPD, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_start_dpd::hmac_config_send_event fail[%d].", ret);
    }

    return ret;
}
#endif

hi_u32 hmac_dpd_data_processed_send(mac_vap_stru *mac_vap, hi_void *param)
{
    if (oal_unlikely((mac_vap == HI_NULL) || (param == HI_NULL))) {
        oam_error_log2(0, OAM_SF_CALIBRATE, "{hmac_dpd_data_processed_send::param null, %p %p.}", mac_vap, param);
        return HI_ERR_CODE_PTR_NULL;
    }

    frw_event_mem_stru *event_mem = frw_event_alloc(sizeof(dmac_tx_event_stru));
    if (event_mem == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CALIBRATE, "{hmac_scan_proc_scan_req_event::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    oal_netbuf_stru *netbuf_dpd_data = oal_netbuf_alloc(WLAN_LARGE_NETBUF_SIZE, 0, 4); /* align 4 */
    if (netbuf_dpd_data == HI_NULL) {
        frw_event_free(event_mem);
        oam_error_log0(0, OAM_SF_CALIBRATE, "{hmac_dpd_data_processed_send::pst_netbuf_scan_result null.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_DPD_DATA_PROCESSED,
        sizeof(dmac_tx_event_stru), FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    if (memset_s(oal_netbuf_cb(netbuf_dpd_data), OAL_TX_CB_LEN, 0, OAL_TX_CB_LEN) != EOK) {
        oal_netbuf_free(netbuf_dpd_data);
        frw_event_free(event_mem);
        return HI_FALSE;
    }

    hmac_tx_ctl_stru *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf_dpd_data);
    tx_ctl->frame_header_length = 0;
    tx_ctl->mac_head_type = 1;
    tx_ctl->frame_header = HI_NULL;

    dpd_cali_data_stru *dpd_cali_data = (dpd_cali_data_stru *)(oal_netbuf_data(netbuf_dpd_data));
    if (memcpy_s(dpd_cali_data, sizeof(dpd_cali_data_stru), param, sizeof(dpd_cali_data_stru)) != EOK) {
        oal_netbuf_free(netbuf_dpd_data);
        frw_event_free(event_mem);
        oam_error_log0(0, OAM_SF_CFG, "hmac_dpd_data_processed_send::p_param memcpy_s fail.");
        return HI_FALSE;
    }

    dmac_tx_event_stru *dpd_event = (dmac_tx_event_stru *)event->auc_event_data;
    dpd_event->netbuf = netbuf_dpd_data;
    dpd_event->us_frame_len = sizeof(dpd_cali_data_stru);
    netbuf_dpd_data->data_len = sizeof(dpd_cali_data_stru);

    hi_u32 ret = hcc_hmac_tx_data_event(event_mem, netbuf_dpd_data, HI_FALSE);
    if (ret != HI_SUCCESS) {
        oal_netbuf_free(netbuf_dpd_data);
        oam_error_log1(mac_vap->vap_id, OAM_SF_BA, "{hmac_mgmt_tx_delba::frw_event_dispatch_event failed[%d].}", ret);
        frw_event_free(event_mem);
        return ret;
    }

    frw_event_free(event_mem);
    return HI_SUCCESS;
}

hi_u32 hmac_dpd_cali_data_recv(const frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    oal_netbuf_stru *dpd_data_netbuf = HI_NULL;
    dpd_cali_data_stru *dpd_cali_data_read = HI_NULL;
    dpd_cali_data_stru dpd_cali_data_calc;
    mac_vap_stru *mac_vap = HI_NULL;
    dmac_tx_event_stru *dtx_event = HI_NULL;
    hi_u32 ret;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_CALIBRATE, "{hmac_dpd_cali_data_recv::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    dtx_event = (dmac_tx_event_stru *)event->auc_event_data;
    dpd_data_netbuf = dtx_event->netbuf;

    mac_vap = mac_vap_get_vap_stru(event->event_hdr.vap_id);

    dpd_cali_data_read = (dpd_cali_data_stru *)oal_netbuf_data(dpd_data_netbuf);

    oam_warning_log0(0, OAM_SF_CALIBRATE, "{hmac_dpd_cali_data_recv DPD cali data}\r\n");

    oal_print_hex_dump((hi_u8 *)dpd_cali_data_read->us_dpd_data, DPD_CALI_LUT_LENGTH, 32, "  "); /* size为32 */

    /* dpd_cali_data_calc.us_dpd_data是函数入参，未初始化dpd_cali_data_calc -g- lin_t !e603 */
    if (HI_SUCCESS != hmac_rf_cali_dpd_corr_calc(dpd_cali_data_read->us_dpd_data, dpd_cali_data_calc.us_dpd_data)) {
        dpd_cali_data_calc.us_data_len = 0;
    } else {
        dpd_cali_data_calc.us_data_len = DPD_CALI_LUT_LENGTH;
    }

    ret = hmac_dpd_data_processed_send(mac_vap, &dpd_cali_data_calc);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_CALIBRATE, "hmac_dpd_data_processed_send return NON SUCCESS. ");
    }

    oal_netbuf_free(dpd_data_netbuf);

    return HI_SUCCESS;
}

#endif

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 设置随机mac addr扫描开关, 0关闭，1打开
 修改历史      :
  1.日    期   : 2015年5月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_random_mac_addr_scan(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hmac_device_stru *hmac_dev = HI_NULL;
    hi_u8 random_mac_addr_scan_switch;

    hi_unref_param(mac_vap);
    hi_unref_param(us_len);

    random_mac_addr_scan_switch = *((hi_u8 *)puc_param);

    /* 获取hmac device结构体 */
    hmac_dev = hmac_get_device_stru();
    hmac_dev->scan_mgmt.is_random_mac_addr_scan = random_mac_addr_scan_switch;
    oam_info_log1(0, OAM_SF_SCAN, "{set set_random_mac_addr_scan SUCC[%d]!}",
        hmac_dev->scan_mgmt.is_random_mac_addr_scan);
    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
/* ****************************************************************************
 功能描述  : 设置rekey offload开关
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2019年10月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_rekey_offload_set_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_REKEY_OFFLOAD_SET_SWITCH, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_rekey_offload_set_switch::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

hi_void hmac_config_kick_user_disassoc(mac_vap_stru *mac_vap, const mac_cfg_kick_user_param_stru *kick_user_param,
    hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user)
{
#if defined(_PRE_WLAN_FEATURE_ARP_OFFLOAD) || defined(_PRE_WLAN_FEATURE_DHCP_OFFLOAD)
    mac_ip_addr_config_stru ip_addr_cfg = {
        .type = MAC_CONFIG_IPV4,
        .oper = MAC_IP_ADDR_DEL
    };
#endif

    /* 发去认证帧 */
    hmac_mgmt_send_disassoc_frame(mac_vap, hmac_user->base_user->user_mac_addr, kick_user_param->us_reason_code,
        (hi_u8)hmac_user->base_user->cap_info.pmf_active);

    /* 修改 state & 删除 user */
    hmac_handle_disconnect_rsp(hmac_vap, hmac_user, HMAC_REPORT_DISASSOC);
    /* 删除用户 */
    hmac_user_del(mac_vap, hmac_user);

    /* 关闭arp offload功能 */
#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
    hmac_config_arp_offload_setting(mac_vap, sizeof(mac_ip_addr_config_stru), (const hi_u8 *)&ip_addr_cfg);
#endif
    /* 关闭dhcp offload功能 */
#ifdef _PRE_WLAN_FEATURE_DHCP_OFFLOAD
    hmac_config_dhcp_offload_setting(mac_vap, sizeof(mac_ip_addr_config_stru), (const hi_u8 *)&ip_addr_cfg);
#endif
    /* 关闭rekey offload功能 */
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
    hi_u8 rekey_offload = HI_FALSE;
    hmac_config_rekey_offload_set_switch(mac_vap, sizeof(hi_u8), (const hi_u8 *)&rekey_offload);
#endif
}

static hi_u32 hmac_config_kick_user_vap(mac_vap_stru *mac_vap, const mac_cfg_kick_user_param_stru *kick_user_param,
    hmac_vap_stru *hmac_vap)
{
    hi_u8 uidx = 0;

    if (mac_vap_find_user_by_macaddr(mac_vap, kick_user_param->auc_mac_addr, WLAN_MAC_ADDR_LEN, &uidx) != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_kick_user::mac_vap_find_user_by_macaddr}");
        if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
            hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);
        }
        return HI_FAIL;
    }

    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(uidx);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_kick_user::hmac_user null,user_idx:%d}", uidx);
        if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
            hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);
        }
        return HI_ERR_CODE_PTR_NULL;
    }

    if (hmac_user->base_user->user_asoc_state != MAC_USER_STATE_ASSOC) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_kick_user:user unassociate,user_idx:%d}", uidx);
    }
#ifdef _PRE_WLAN_FEATURE_MESH
    if (hmac_user->base_user->is_mesh_user == HI_TRUE) {
        /* 如果是由wpa发起的删除用户，则直接将用户删掉即可 */
        if (kick_user_param->us_reason_code == MAC_WPA_KICK_MESH_USER) {
            /* 删除用户 */
            hmac_user_del(mac_vap, hmac_user);
            oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_kick_user::the mesh user is del}");
            return HI_SUCCESS;
        }
        hmac_handle_close_peer_mesh(hmac_vap, hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN,
            HMAC_REPORT_DISASSOC, DMAC_DISASOC_MISC_KICKUSER);

        return HI_SUCCESS;
    }
#endif

    hmac_config_kick_user_disassoc(mac_vap, kick_user_param, hmac_vap, hmac_user);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 配置命令去关联1个用户
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2014年5月29日
    作    者   : HiSilicon
    修改内容   : 增加踢掉全部user的功能
**************************************************************************** */
hi_u32 hmac_config_kick_user(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_user_stru                *user_tmp = HI_NULL;
    hmac_user_stru               *hmac_user_tmp = HI_NULL;
    hi_list                      *entry = HI_NULL;

    hi_unref_param(us_len);

    if (oal_unlikely(mac_vap == HI_NULL || puc_param == HI_NULL)) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_kick_user:vap=%p pa=%p}", (uintptr_t)mac_vap, (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_vap->vap_mode == WLAN_VAP_MODE_CONFIG) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_kick_user::en_vap_mode is WLAN_VAP_MODE_CONFIG.}");
        return HI_ERR_CODE_INVALID_CONFIG;
    }

    mac_cfg_kick_user_param_stru *kick_user_param = (mac_cfg_kick_user_param_stru *)puc_param;
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log1(0, OAM_SF_CFG, "{hmac_config_kick_user::null param,pst_hmac_vap[%d].}", mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    oam_warning_log4(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_kick_user: user mac[XX:XX:XX:%02X:%02X:%02X]code %d}",
        kick_user_param->auc_mac_addr[3], kick_user_param->auc_mac_addr[4], /* 3 4 元素索引 */
        kick_user_param->auc_mac_addr[5], kick_user_param->us_reason_code); /* 5 元素索引 */

    /* 踢掉全部user (mesh不支持) */
    if (oal_is_broadcast_ether_addr(kick_user_param->auc_mac_addr)
#ifdef _PRE_WLAN_FEATURE_MESH
        && (mac_vap->vap_mode != WLAN_VAP_MODE_MESH)
#endif
    ) {
        hmac_mgmt_send_disassoc_frame(mac_vap, kick_user_param->auc_mac_addr,
                                      kick_user_param->us_reason_code, HI_FALSE);

        /* 遍历vap下所有用户, 删除用户 */
        hi_list *user_list_head = &(mac_vap->mac_user_list_head);
        for (entry = user_list_head->next; entry != user_list_head;) {
            user_tmp = hi_list_entry(entry, mac_user_stru, user_dlist);
            hmac_user_tmp = (hmac_user_stru *)hmac_user_get_user_stru((hi_u8)user_tmp->us_assoc_id);
            if (oal_unlikely(hmac_user_tmp == HI_NULL)) {
                oam_error_log1(0, OAM_SF_CFG, "{hmac_config_kick_user::null param,user_tmp %d}", user_tmp->us_assoc_id);
                continue;
            }

            /* 指向双向链表下一个 */
            entry = entry->next;

            /* 修改 state & 删除 user */
            hmac_handle_disconnect_rsp(hmac_vap, hmac_user_tmp, HMAC_REPORT_DISASSOC);

            /* 删除用户 */
            hmac_user_del(mac_vap, hmac_user_tmp);
        }

        /* VAP下user头指针不应该为空 */
        if (hi_is_list_empty_optimize(&mac_vap->mac_user_list_head) == HI_FALSE) {
            oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_kick_user::st_mac_user_list_head is not empty.}");
        }
        return HI_SUCCESS;
    }

    return hmac_config_kick_user_vap(mac_vap, kick_user_param, hmac_vap);
}

/* ****************************************************************************
 功能描述  : 发送设置non-HT速率命令到dmac
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_rate(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_RATE, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_rate::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 发送设置HT速率命令到dmac
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_mcs(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_MCS, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_mcs::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 发送设置带宽命令到dmac
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_bw(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32                              ret;
    mac_cfg_tx_comp_stru                *event_set_bw = HI_NULL;
    wlan_channel_bandwidth_enum_uint8   bandwidth      = WLAN_BAND_ASSEMBLE_20M;
    hmac_vap_stru                       *hmac_vap     = HI_NULL;

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_bw::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    event_set_bw = (mac_cfg_tx_comp_stru *)puc_param;
    if (event_set_bw->param == WLAN_BAND_ASSEMBLE_5M) {
        bandwidth = WLAN_BAND_WIDTH_5M;
    } else if (event_set_bw->param == WLAN_BAND_ASSEMBLE_10M) {
        bandwidth = WLAN_BAND_WIDTH_10M;
    } else if (event_set_bw->param == WLAN_BAND_ASSEMBLE_20M) {
        bandwidth = WLAN_BAND_WIDTH_20M;
    }
    hmac_vap->preset_para.en_bandwidth = bandwidth;
    mac_vap->channel.en_bandwidth = bandwidth;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_BW, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_bw::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 获取带宽信息
**************************************************************************** */
hi_u32 hmac_config_get_bw(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    mac_cfg_tx_comp_stru *set_bw_param = HI_NULL;

    set_bw_param = (mac_cfg_tx_comp_stru *)puc_param;
    if (mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_5M) {
        set_bw_param->param = WLAN_BAND_ASSEMBLE_5M;
    } else if (mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_10M) {
        set_bw_param->param = WLAN_BAND_ASSEMBLE_10M;
    } else if (mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_20M) {
        set_bw_param->param = WLAN_BAND_ASSEMBLE_20M;
    }

    *pus_len = sizeof(mac_cfg_tx_comp_stru);

    return HI_SUCCESS;
}


/* ****************************************************************************
 功能描述  : 发送设置常发模式命令到dmac
 修改历史      :
  1.日    期   : 2015年1月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_always_tx(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    mac_cfg_tx_comp_stru *event_set_bcast = HI_NULL;

    /* 使能常发 */
    event_set_bcast = (mac_cfg_tx_comp_stru *)puc_param;
    mac_vap->al_tx_flag = (event_set_bcast->param == HI_SWITCH_OFF) ? HI_SWITCH_OFF : HI_SWITCH_ON;
    if (mac_vap->al_tx_flag) {
        mac_vap->cap_flag.keepalive = HI_FALSE;
        /* 常发打开时关闭低功耗 */
        hmac_set_wlan_pm_switch(HI_FALSE);
    } else {
        mac_vap_set_al_tx_first_run(mac_vap, HI_FALSE);
    }
    mac_vap_set_al_tx_payload_flag(mac_vap, event_set_bcast->payload_flag);

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_ALWAYS_TX, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_always_tx::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}

#ifdef _PRE_WLAN_FEATURE_CSI
/* ***************************************************************************
 功能描述  : hmac接收wal抛下来的CSI开关设置事件,并将事件继续抛到dmac
 修改历史      :
  1.日    期   : 2019年1月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_csi_set_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 send_event_ret;

    /* **************************************************************************
     抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    send_event_ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CSI_SWITCH, us_len, puc_param);
    if (send_event_ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CSI,
            "{hmac_config_csi_set_switch::hmac_config_send_event failed[%d].}", send_event_ret);
        return send_event_ret;
    }
    return HI_SUCCESS;
}

/* ***************************************************************************
 功能描述  : hmac接收wal抛下来的CSI参数配置事件,并将事件继续抛到dmac
 修改历史      :
  1.日    期   : 2019年1月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_csi_set_config(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 send_event_ret;

    /* **************************************************************************
     抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    send_event_ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CSI_SET_CONFIG, us_len, puc_param);
    if (send_event_ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CSI,
            "{hmac_config_csi_set_config::hmac_config_send_event failed[%d].}", send_event_ret);
        return send_event_ret;
    }
    return HI_SUCCESS;
}

/* ***************************************************************************
 功能描述  : HMAC层处理DMAC模块产生CSI上报事件函数
 修改历史      :
  1.日    期   : 2019年1月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_csi_data_report_event(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u32 send_event_ret;

    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_CSI, "{hmac_csi_data_report_event::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* **************************************************************************
     抛事件到WAL层, 同步WAL数据
    ************************************************************************** */
    /* 获得事件指针 */
    event = (frw_event_stru *)event_mem->puc_data;
    mac_vap = mac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CSI, "{hmac_csi_data_report_event::pst_mac_vap null.}");
        return HI_FAIL;
    }

    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CTX, HMAC_HOST_CTX_EVENT_SUB_TYPE_CSI_REPORT,
        sizeof(mac_csi_data_stru), FRW_EVENT_PIPELINE_STAGE_0, mac_vap->vap_id);

    /* 分发事件到WAL层 */
    send_event_ret = frw_event_dispatch_event(event_mem);
    if (send_event_ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CSI,
            "{hmac_csi_data_report_event::frw_event_dispatch_event fail[%d].}", send_event_ret);
        return send_event_ret;
    }
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 发送设置常收模式命令到dmac
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_always_rx(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_always_rx::hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap->hmac_al_rx_flag = puc_param[0];
    hmac_vap->mac_filter_flag = puc_param[1];

    if (hmac_vap->hmac_al_rx_flag == HI_SWITCH_ON) {
        /* 常收打开时关闭低功耗 */
        hmac_set_wlan_pm_switch(HI_FALSE);
    }

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_ALWAYS_RX, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_always_rx::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    /* 设置帧过滤 */
    ret = hmac_set_rx_filter_value(mac_vap);
#endif
    return ret;
}

#ifdef _PRE_DEBUG_MODE
/* ****************************************************************************
 功能描述  : 发送设置动态功率校准命令到dmac
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_dync_txpower(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DYNC_TXPOWER, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_dync_txpower::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 上报某一个vap下的收发包统计
 修改历史      :
  1.日    期   : 2014年7月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_vap_pkt_stat(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_unref_param(mac_vap);
    hi_unref_param(us_len);
    hi_unref_param(puc_param);
    return HI_SUCCESS;
}
#endif
#endif

/* ****************************************************************************
 功能描述  : hmac设置国家码
 修改历史      :
  1.日    期   : 2013年10月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_country(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_country_stru      *country_param = HI_NULL;
    mac_regdomain_info_stru   *mac_regdom = HI_NULL;
    mac_regdomain_info_stru   *regdomain_info = HI_NULL;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hi_u32 ret;
#endif
    hi_u32 size;

    hi_unref_param(us_len);

    country_param = (mac_cfg_country_stru *)puc_param;
    mac_regdom = (mac_regdomain_info_stru *)country_param->mac_regdom;
    size = sizeof(mac_regdomain_info_stru);
    /* 获取管制域全局变量 */
    regdomain_info = mac_get_regdomain_info();
    /* 更新管制域信息 */
    if (mac_regdom != HI_NULL) {
        if (memcpy_s(regdomain_info, sizeof(mac_regdomain_info_stru), mac_regdom, size) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "hmac_config_set_country::pst_mac_regdom memcpy_s fail.");
            return HI_FAIL;
        }
    }
    /* 更新信道的管制域信息 */
    mac_init_channel_list();
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_COUNTRY, (hi_u16)size, (hi_u8 *)mac_regdom);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oal_mem_free(mac_regdom);
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_country::hmac_config_send_event failed[%d].}",
            ret);
        return ret;
    }
#else
    hi_unref_param(mac_vap);
#endif
    /* WAL层抛内存下来，此处释放 */
    oal_mem_free(mac_regdom);
    return HI_SUCCESS;
}

#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
/* ****************************************************************************
 功能描述  : hmac设置ampdu tx 开关
 修改历史      :
  1.日    期   : 2015年5月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_ampdu_tx_on_sub(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_ampdu_tx_on_param_stru *ampdu_tx_on_param = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;

    hi_unref_param(us_len);

    if (oal_unlikely(mac_vap == HI_NULL || puc_param == HI_NULL)) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_config_set_ampdu_tx_on:: param null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_ampdu_tx_on::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    ampdu_tx_on_param = (mac_cfg_ampdu_tx_on_param_stru *)puc_param;
#if defined(_PRE_WLAN_FEATURE_HIPRIV) && defined(_PRE_WLAN_FEATURE_INTRF_MODE)
    if (ampdu_tx_on_param->aggr_tx_on == 2) { /* 2:恢复历史值 */
        hmac_vap->ampdu_tx_on_switch = g_hist_ampdu_tx_on;
    } else { /* 设置并保存旧值 */
        g_hist_ampdu_tx_on = (hi_bool)hmac_vap->ampdu_tx_on_switch;
        hmac_vap->ampdu_tx_on_switch = ampdu_tx_on_param->aggr_tx_on;
    }
#else
    hmac_vap->ampdu_tx_on_switch = ampdu_tx_on_param->aggr_tx_on;
#endif
    oam_info_log1(0, OAM_SF_CFG, "{hmac_config_set_ampdu_tx_on:: ampdu_tx_on_switch[%d]!}\r\n",
        hmac_vap->ampdu_tx_on_switch);

    return HI_SUCCESS;
}

hi_u32 hmac_config_set_ampdu_tx_on(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    return hmac_config_set_ampdu_tx_on_sub(mac_vap, us_len, puc_param);
}

hi_u32 hmac_config_set_ampdu_tx_on_from_dmac(mac_vap_stru *mac_vap, hi_u8 us_len, const hi_u8 *puc_param)
{
    return hmac_config_set_ampdu_tx_on_sub(mac_vap, us_len, puc_param);
}
#endif

/* ****************************************************************************
 功能描述  : 获取对端RSSI
**************************************************************************** */
hi_u32 hmac_config_query_rssi(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_RSSI, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_query_rssi::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 发送用户定制报文
 输入参数  : pst_mac_vap:  MAC VAP
             puc_param   : 上层参数信息
             us_len      : 上层参数长度
 返 回 值  : HI_SUCCESS 上报成功，其它错误码 上报失败
**************************************************************************** */
hi_u32 hmac_send_custom_pkt(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    oal_netbuf_stru *pkt_buf = HI_NULL;
    hi_u16 us_pkt_len;
    hi_u8 *puc_data = HI_NULL;
    hmac_tx_ctl_stru *tx_ctl = HI_NULL;
    wlan_custom_pkt_stru *pkt_param = (wlan_custom_pkt_stru *)puc_param;

    hi_unref_param(us_len);

    if (pkt_param->puc_data == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 复制用户数据到帧中 */
    if (pkt_param->us_len > WLAN_LARGE_NETBUF_SIZE) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_send_custom_pkt::pkt_param is null or pkt len too long.}");
        hi_free(HI_MOD_ID_WIFI_DRV, pkt_param->puc_data);
        return HI_FAIL;
    }

    /* 申请报文内存 */
    pkt_buf = (oal_netbuf_stru *)oal_netbuf_alloc(pkt_param->us_len, 0, 4); /* align 4 */
    if (pkt_buf == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_send_custom_pkt::alloc netbuf failed.}");
        hi_free(HI_MOD_ID_WIFI_DRV, pkt_param->puc_data);
        return HI_ERR_CODE_PTR_NULL;
    }

    if (memset_s(oal_netbuf_cb(pkt_buf), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size()) != EOK) {
        hi_free(HI_MOD_ID_WIFI_DRV, pkt_param->puc_data);
        oal_netbuf_free(pkt_buf);
        return HI_FAIL;
    }
    puc_data = (hi_u8 *)oal_netbuf_header(pkt_buf);
    tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(pkt_buf);

    if (memcpy_s(puc_data, (hi_u32)pkt_param->us_len, pkt_param->puc_data, (hi_u32)pkt_param->us_len) != EOK) {
        oal_netbuf_free(pkt_buf);
        hi_free(HI_MOD_ID_WIFI_DRV, pkt_param->puc_data);
        return HI_FAIL;
    }
    us_pkt_len = (hi_u16)pkt_param->us_len;
    hi_free(HI_MOD_ID_WIFI_DRV, pkt_param->puc_data);

    tx_ctl->frame_header_length = MAC_80211_FRAME_LEN;
    tx_ctl->frame_header = (mac_ieee80211_frame_stru *)oal_netbuf_header(pkt_buf);
    tx_ctl->mac_head_type = 1;
    tx_ctl->us_tx_user_idx = 0xF;
    tx_ctl->us_mpdu_len = us_pkt_len;
    oal_netbuf_put(pkt_buf, (hi_u32)us_pkt_len);

    /* 调用发送管理帧接口 */
    if (hmac_tx_mgmt_send_event(mac_vap, pkt_buf, us_pkt_len) != HI_SUCCESS) {
        oal_netbuf_free(pkt_buf);
        oam_warning_log0(0, OAM_SF_CFG, "{hmac_send_custom_pkt::hmac_tx_mgmt_send_event failed.}");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 降SAR
 修改历史      :
  1.日    期   : 2014年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_reduce_sar(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_REDUCE_SAR, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "hmac_config_reduce_sar::hmac_config_send_event failed, error no[%d]!", ret);
        return ret;
    }
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : hmac读取国际码
 修改历史      :
  1.日    期   : 2013年10月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_country(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    hi_unref_param(pus_len);
    hi_unref_param(mac_vap);
    mac_cfg_get_country_stru *param = (mac_cfg_get_country_stru *)puc_param;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    mac_regdomain_info_stru *regdomain_info = mac_get_regdomain_info();
    param->ac_country[0] = regdomain_info->ac_country[0];
    param->ac_country[1] = regdomain_info->ac_country[1]; /* 1 元素索引 */
    param->ac_country[2] = regdomain_info->ac_country[2]; /* 2 元素索引 */
#else
    hi_char *pc_curr_cntry = mac_regdomain_get_country();
    param->ac_country[0] = pc_curr_cntry[0];
    param->ac_country[1] = pc_curr_cntry[1]; /* 1 元素索引 */
    param->ac_country[2] = pc_curr_cntry[2]; /* 2 元素索引 */
#endif

    oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG, "hmac_config_get_country:: country[%C%C]\r\n", param->ac_country[0],
        param->ac_country[1]);
    *pus_len = sizeof(mac_cfg_get_country_stru);
    return HI_SUCCESS;
}

static hi_u32 hmac_config_connect_ie(mac_vap_stru *mac_vap, hmac_scanned_bss_info *scanned_bss_info,
    const mac_cfg80211_connect_param_stru *connect_param, const mac_bss_dscr_stru *bss_dscr,
    mac_cfg80211_connect_security_stru *conn_sec)
{
    hi_unref_param(scanned_bss_info);

    oal_app_ie_stru *app_ie = (oal_app_ie_stru *)oal_memalloc(sizeof(oal_app_ie_stru));
    if (app_ie == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_connect:: failed alloc app_ie}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }
    app_ie->ie_len = connect_param->ie_len;
    if ((connect_param->puc_ie != HI_NULL) &&
        (memcpy_s(app_ie->auc_ie, WLAN_WPS_IE_MAX_SIZE, connect_param->puc_ie, app_ie->ie_len) != EOK)) {
        oam_warning_log0(0, 0, "hmac_config_connect:puc_ie mem error");
        oal_free(app_ie);
        return HI_FAIL;
    }
    app_ie->app_ie_type = OAL_APP_ASSOC_REQ_IE;
    if (hmac_config_set_app_ie_to_vap(mac_vap, app_ie, app_ie->app_ie_type) != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "hmac_config_set_app_ie_to_vap return NON SUCCESS. ");
    }
    oal_free(app_ie);

    /* 设置有效 dot11DTIMPeriod */
    if (bss_dscr->dtim_period > 0) {
        mac_vap->mib_info->wlan_mib_sta_config.dot11_dtim_period = bss_dscr->dtim_period;
    }
    /* 设置关联用户的能力信息 */
    mac_vap->us_assoc_user_cap_info = bss_dscr->us_cap_info;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* 入网选择的热点rssi，同步到dmac传给tpc算法做管理报文tpc */
    conn_sec->rssi = scanned_bss_info->bss_dscr_info.rssi;
#endif /* _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE */

    hi_u32 ret = mac_vap_init_privacy(mac_vap, conn_sec);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_connect::mac_11i_init_privacy failed[%d]!}\r\n", ret);
        return ret;
    }

    if (!conn_sec->privacy) {
        mac_vap->user_pmf_cap = HI_FALSE;
    }

    return HI_SUCCESS;
}

static hi_u32 hmac_config_connect_dev(const mac_vap_stru *mac_vap, mac_cfg80211_connect_param_stru *connect_param,
    hmac_vap_stru *hmac_vap, hmac_bss_mgmt_stru *bss_mgmt)
{
    hi_unref_param(mac_vap);
    hi_unref_param(connect_param);
    hi_unref_param(bss_mgmt);

    /* 解决重关联请求问题后打开 */
    hmac_vap->reassoc_flag = HI_FALSE;
#ifdef _PRE_WLAN_FEATURE_WAPI
    bss_dscr->wapi = connect_param->wapi;
    if (bss_dscr->wapi) {
        mac_device_stru *mac_dev = mac_res_get_dev();
        if (mac_device_is_p2p_connected(mac_dev) == HI_SUCCESS) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{wapi connect failed for p2p having been connected!.}");
            oal_spin_unlock(&(bss_mgmt->st_lock));
            return HI_FAIL;
        }
    }
#endif

    /* 解锁 */
    oal_spin_unlock(&(bss_mgmt->st_lock));

#ifdef _PRE_WLAN_FEATURE_P2P
    /* 设置P2P/WPS IE 信息到 vap 结构体中 */
    if (is_legacy_vap(mac_vap)) {
        hmac_p2p_del_ie(connect_param->puc_ie, &(connect_param->ie_len));
    }
#endif

    return HI_SUCCESS;
}

static hi_u32 hmac_config_connect_conn_init(const mac_cfg80211_connect_param_stru *connect_param,
    mac_cfg80211_connect_security_stru *conn_sec)
{
    /* 根据内核下发的关联能力，赋值加密相关的mib 值 */
    /* 根据下发的join,提取出安全相关的内容 */
    conn_sec->wep_key_len = connect_param->wep_key_len;
    conn_sec->auth_type = connect_param->auth_type;
    conn_sec->privacy = connect_param->privacy;
    conn_sec->crypto = connect_param->crypto;
    conn_sec->wep_key_index = connect_param->wep_key_index;
    conn_sec->mgmt_proteced = connect_param->mfp;
    if (conn_sec->wep_key_len > WLAN_WEP104_KEY_LEN) {
        oam_error_log1(0, OAM_SF_ANY, "{hmac_config_connect:key_len[%d] > WLAN_WEP104_KEY_LEN}\r\n",
            conn_sec->wep_key_len);
        conn_sec->wep_key_len = WLAN_WEP104_KEY_LEN;
    }
    if ((connect_param->puc_wep_key != HI_NULL) && (memcpy_s(conn_sec->auc_wep_key, WLAN_WEP104_KEY_LEN,
        connect_param->puc_wep_key, conn_sec->wep_key_len) != EOK)) {
        oam_warning_log1(0, 0, "hmac_config_connect:mem error :: %p", (uintptr_t)connect_param->puc_wep_key);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

hi_u32 hmac_config_connect_hmac(mac_vap_stru *mac_vap, mac_cfg80211_connect_security_stru *conn_sec,
    mac_cfg80211_connect_param_stru *connect_param, hmac_vap_stru *hmac_vap)
{
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    /* 获取管理扫描的bss结果的结构体 */
    hmac_bss_mgmt_stru *bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt); /* 管理扫描的bss结果的结构体 */
    /* 对链表删操作前加锁 */
    oal_spin_lock(&(bss_mgmt->st_lock));
    hmac_scanned_bss_info *scanned_bss_info = hmac_scan_find_scanned_bss_by_bssid(bss_mgmt, connect_param->puc_bssid);
    if (scanned_bss_info == HI_NULL) {
        oam_warning_log3(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_connect:find bss fail bssid::XX:XX:%02X:%02X:%02X}",
            connect_param->puc_bssid[3], connect_param->puc_bssid[4], connect_param->puc_bssid[5]); /* 3 4 5 元素索引 */

        /* 解锁 */
        oal_spin_unlock(&(bss_mgmt->st_lock));
        return HI_FAIL;
    }

    if (memcmp(connect_param->puc_ssid, scanned_bss_info->bss_dscr_info.ac_ssid, (hi_u32)connect_param->ssid_len)) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_connect::find the bss failed by ssid.}");
        /* 解锁 */
        oal_spin_unlock(&(bss_mgmt->st_lock));
        return HI_FAIL;
    }

    mac_bss_dscr_stru *bss_dscr = &(scanned_bss_info->bss_dscr_info);

    if (hmac_config_connect_dev(mac_vap, connect_param, hmac_vap, bss_mgmt) != HI_SUCCESS) {
        return HI_FAIL;
    }

    hi_u32 ret = hmac_config_connect_ie(mac_vap, scanned_bss_info, connect_param, bss_dscr, conn_sec);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    hmac_vap->wps_active = conn_sec->wps_enable;
    ret = hmac_check_capability_mac_phy_supplicant(mac_vap, bss_dscr);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_connect:check mac phy capability fail[%d]}\n", ret);
    }

    /* **************************************************************************
    抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CONNECT_REQ, sizeof(mac_cfg80211_connect_security_stru),
        (hi_u8 *)conn_sec);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_connect::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }

    return hmac_sta_initiate_join(mac_vap, bss_dscr);
}

/* ****************************************************************************
 功能描述  : hmac连接
 修改历史      :
  1.日    期   : 2015年5月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_connect(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg80211_connect_security_stru conn_sec;

    if (oal_unlikely(mac_vap == HI_NULL) || oal_unlikely(puc_param == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_config_connect:: connect failed, null ptr!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (us_len != sizeof(mac_cfg80211_connect_param_stru)) {
        oam_error_log1(0, OAM_SF_ANY, "{hmac_config_connect:: connect failed,unexpected param len![%x]!}\r\n", us_len);
        return HI_ERR_CODE_INVALID_CONFIG;
    }

    /* 先判断其他VAP 的状态是否允许本VAP 入网连接 */
    /* 如果允许本VAP 入网，则返回设备忙状态           */
    mac_cfg80211_connect_param_stru *connect_param = (mac_cfg80211_connect_param_stru *)puc_param;

    if (hmac_config_connect_conn_init(connect_param, &conn_sec) != HI_SUCCESS) {
        return HI_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_PMF
    conn_sec.pmf_cap = mac_get_pmf_cap(connect_param->puc_ie, connect_param->ie_len);
#endif
    conn_sec.wps_enable = HI_FALSE;
    if (mac_find_vendor_ie(MAC_WLAN_OUI_MICROSOFT, MAC_WLAN_OUI_TYPE_MICROSOFT_WPS, connect_param->puc_ie,
        (hi_s32)(connect_param->ie_len))) {
        conn_sec.wps_enable = HI_TRUE;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_CFG, "{hmac_config_connect:connect failed,hmac_vap null.vap_id[%d]}", mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap->auth_mode = conn_sec.auth_type;

    return hmac_config_connect_hmac(mac_vap, &conn_sec, connect_param, hmac_vap);
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 获取最新数据帧的tid
 修改历史      :
  1.日    期   : 2013年10月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_tid(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    mac_device_stru *mac_dev = HI_NULL;
    mac_cfg_get_tid_stru *tid = HI_NULL;
    hi_unref_param(mac_vap);

    tid = (mac_cfg_get_tid_stru *)puc_param;
    mac_dev = mac_res_get_dev();
    tid->tid = mac_dev->tid;
    *pus_len = sizeof(tid->tid);

    oam_info_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_get_tid::en_tid=%d.}", tid->tid);
    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 输出设备支持的信道列表
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_list_channel(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 chan_num;
    hi_u8 chan_idx;
    hi_u32 ret;
    hi_unref_param(us_len);

    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_list_channel::null param,pst_mac_vap=%p puc_param=%p.}",
            (uintptr_t)mac_vap, (uintptr_t)puc_param);
        return HI_FAIL;
    }

    for (chan_idx = 0; chan_idx < MAC_CHANNEL_FREQ_2_BUTT; chan_idx++) {
        ret = mac_is_channel_idx_valid(MAC_RC_START_FREQ_2, chan_idx, HI_NULL);
        if (ret == HI_SUCCESS) {
            mac_get_channel_num_from_idx(MAC_RC_START_FREQ_2, chan_idx, &chan_num);

            /* 输出2G信道号 */
            oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_list_channel::2gCHA.NO=%d}\n", chan_num);
        }
    }

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 将用户态 IE 信息拷贝到内核态中
 输入参数  : mac_vap_stru *pst_mac_vap
             oal_net_dev_ioctl_data_stru *pst_ioctl_data
             enum WPS_IE_TYPE en_type
 返 回 值  : static hi_u8*
 修改历史      :
  1.日    期   : 2014年4月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_app_ie_to_vap(mac_vap_stru *mac_vap, oal_app_ie_stru *app_ie, en_app_ie_type_uint8 type)
{
    hi_u32 ret;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_app_ie_stru tmp_app_ie;
#endif
    hi_u8 *puc_ie = HI_NULL;
    hi_u32 remain_len;

    if ((mac_vap == HI_NULL) || (app_ie == HI_NULL)) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_set_app_ie_to_vap::scan failed, set ie null ptr, %p, %p.}",
            (uintptr_t)mac_vap, (uintptr_t)app_ie);

        return HI_ERR_CODE_PTR_NULL;
    }

    /* 移除驱动侧重复MAC_EID_EXT_CAPS */
    puc_ie = mac_find_ie(MAC_EID_EXT_CAPS, app_ie->auc_ie, app_ie->ie_len);
    if (puc_ie != HI_NULL) {
        app_ie->ie_len -= (hi_u32)(puc_ie[1] + MAC_IE_HDR_LEN);
        remain_len = app_ie->ie_len - (hi_u32)(puc_ie - app_ie->auc_ie);
        if (memmove_s(puc_ie, remain_len, puc_ie + (hi_u32)(puc_ie[1] + MAC_IE_HDR_LEN), remain_len) != EOK) {
            return HI_FAIL;
        }
    }

    ret = mac_vap_save_app_ie(mac_vap, app_ie, type);
    if (ret != HI_SUCCESS) {
        oam_error_log3(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_app_ie_to_vap::mac_vap_save_app_ie failed[%d], en_type[%d], len[%d].}", ret, type,
            app_ie->ie_len);
        return ret;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if (app_ie->app_ie_type >= OAL_APP_ASSOC_REQ_IE) {
        /* 只有OAL_APP_BEACON_IE、OAL_APP_PROBE_REQ_IE、OAL_APP_PROBE_RSP_IE 才需要保存到device */
        return HI_SUCCESS;
    }
    tmp_app_ie.app_ie_type = app_ie->app_ie_type;
    tmp_app_ie.ie_len = app_ie->ie_len;

    /* 将下发的ie类型和长度保存到auc_buffer 中，再下抛事件下发给DMAC */
    if (memcpy_s(tmp_app_ie.auc_ie, WLAN_WPS_IE_MAX_SIZE, app_ie->auc_ie, app_ie->ie_len) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_config_set_app_ie_to_vap::pst_app_ie->auc_ie memcpy_s fail.");
        return HI_FAIL;
    }
    /* **************************************************************************
     抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_WPS_P2P_IE, sizeof(oal_app_ie_stru), (hi_u8 *)&tmp_app_ie);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log2(0, OAM_SF_CFG,
            "{hmac_config_set_app_ie_to_vap::hmac_config_send_event failed[%d], vap id[%d].}", ret, mac_vap->vap_id);
    }
#endif

    return ret; /* app_ie在本函数中不涉及释放，误报告警，lin_t e429告警屏蔽 */
}

/* ****************************************************************************
 功能描述  : 设置分片门限
 修改历史      :
  1.日    期   : 2014年8月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_rts_threshold(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_rts_threshold_stru *rts_threshold = HI_NULL;
    hi_unref_param(us_len);

    if (oal_unlikely(mac_vap == HI_NULL || puc_param == HI_NULL || mac_vap->mib_info == HI_NULL)) {
        oam_error_log2(0, OAM_SF_CFG,
            "{hmac_config_rts_threshold:: mac_vap/puc_param/mib_info is null ptr %p, %p!}\r\n", (uintptr_t)mac_vap,
            (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }

    rts_threshold = (mac_cfg_rts_threshold_stru *)puc_param;
    mac_mib_set_rts_threshold(mac_vap, rts_threshold->rts_threshold);

    oam_info_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_rts_threshold: mib rts %d!}\r\n",
        mac_vap->mib_info->wlan_mib_operation.dot11_rts_threshold);

    return HI_SUCCESS;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 读取寄存器值
 修改历史      :
  1.日    期   : 2013年5月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_reg_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_REG_INFO, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_reg_info::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_WOW
/* ****************************************************************************
 功能描述  :
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_wow_set_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    hi_unref_param(us_len);
    hi_unref_param(puc_param);

    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_config_wow_set_param:oal_net_dev_priv(pst_net_dev) is null ptr!}\r\n");
        return HI_ERR_WIFI_HMAC_INVALID_PARAMETER;
    }

    ret = hmac_wow_set_dmac_cfg();
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_wow_set_dmac_cfg return NON SUCCESS. ");
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : hmac set FW no send any frame to driver
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_host_sleep_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 is_host_sleep;
    hi_unref_param(us_len);
    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_config_host_sleep_switch::param null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    is_host_sleep = *(hi_u32 *)puc_param;

    hmac_wow_host_sleep_cmd(mac_vap, is_host_sleep);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : Hmac Enable/disable WOW events
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_wow(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 wow_event;
    hi_unref_param(us_len);

    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_config_set_wow::param null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    wow_event = *(hi_u32 *)puc_param;

    hisi_wlan_set_wow_event(wow_event);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : Hmac activate/deactivate wow hipriv
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_wow_activate_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 wow_en;
    mac_vap_stru *vap = HI_NULL;
    mac_device_stru *mac_dev = mac_res_get_dev();
    hi_unref_param(us_len);
    hi_u8 vap_idx;

    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_config_wow_en::param null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (vap == HI_NULL) {
            continue;
        }
        if ((vap->vap_mode == WLAN_VAP_MODE_BSS_AP) && (vap->vap_state == MAC_VAP_STATE_UP)) {
            oam_warning_log0(0, 0, "hmac_config_wow_activate_switch:: AP EXIST, don't support wowEn");
            return HI_FAIL;
        }
    }

    wow_en = *(hi_u32 *)puc_param;

    hmac_wow_set_wow_en_cmd(wow_en);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : Hmac set wow pattern
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_wow_pattern(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hmac_cfg_wow_pattern_param_stru *pattern = HI_NULL;
    hi_u32 ret;

    hi_unref_param(us_len);

    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_config_set_wow_pattern::param null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    pattern = (hmac_cfg_wow_pattern_param_stru *)puc_param;

    if (pattern->us_pattern_option == MAC_WOW_PATTERN_PARAM_OPTION_ADD) {
        ret = hisi_wlan_add_netpattern((hi_u32)pattern->us_pattern_index, &pattern->auc_pattern_value[0],
            pattern->pattern_len);
        if (ret != HI_SUCCESS) {
            oam_warning_log0(0, OAM_SF_CFG, "hisi_wlan_add_netpattern return NON SUCCESS. ");
        }
    } else if (pattern->us_pattern_option == MAC_WOW_PATTERN_PARAM_OPTION_DEL) {
        hisi_wlan_del_netpattern((hi_u32)pattern->us_pattern_index);
    } else {
        hmac_wow_set_pattern_cmd(pattern);
    }

    return HI_SUCCESS;
}
#endif /* end of _PRE_WLAN_FEATURE_WOW */
#endif

#ifdef _PRE_WLAN_FEATURE_PROMIS
/* ****************************************************************************
 功能描述  :
 修改历史      :
  1.日    期   : 2016年3月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_monitor_switch(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hi_u8 value = puc_param[0];

    mac_device_stru *mac_dev = mac_res_get_dev();
    if (value == 0) {
        mac_dev->promis_switch = HI_FALSE;
    } else {
        mac_dev->promis_switch = HI_TRUE;
    }
#endif

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_MONITOR_EN, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_monitor_switch::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}
#endif

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 指定用户的指定tid发送bar
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_send_bar(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SEND_BAR, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_send_bar::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_reg_write(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_REG_WRITE, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_reg_write::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

/* ****************************************************************************
 功能描述  : hmac, 算法配置命令示例
 修改历史      :
  1.日    期   : 2013年10月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_alg_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    frw_event_mem_stru *event_mem = HI_NULL;
    hmac_to_dmac_cfg_msg_stru *syn_msg = HI_NULL;

    ret = hmac_config_alloc_event(mac_vap, HMAC_TO_DMAC_SYN_ALG, &syn_msg, &event_mem, us_len);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_alg_send_event::hmac_config_alloc_event failed[%d].}", ret);
        return ret;
    }
    syn_msg->syn_id = WLAN_CFGID_ALG_PARAM;
    syn_msg->us_len = us_len;
    /* 填写配置同步消息内容 */
    if (puc_param != HI_NULL) {
        if (memcpy_s(syn_msg->auc_msg_body, us_len, puc_param, us_len) != EOK) {
            frw_event_free(event_mem);
            oam_error_log0(0, OAM_SF_CFG, "dmac_join_set_reg_event_process:: hmac_config_alloc_event memcpy_s fail.");
            return HI_FAIL;
        }
    }
    /* 抛出事件 */
    hcc_hmac_tx_control_event(event_mem, us_len + (hi_u16)oal_offset_of(hmac_to_dmac_cfg_msg_stru, auc_msg_body));
    frw_event_free(event_mem);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  :
 修改历史      :
  1.日    期   : 2014年6月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_forty_mhz_intolerant(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_config_set_forty_mhz_intolerant::mac_vap[%p] NULL or puc_param[%p] NULL!}",
            (uintptr_t)mac_vap, (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }
    hi_unref_param(us_len);

    if ((*puc_param != 0) && (*puc_param != 1)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_FortyMHzIntolerant::invalid param[%d].",
            *puc_param);
        return HI_ERR_CODE_INVALID_CONFIG;
    }

    mac_mib_set_forty_mhz_intolerant(mac_vap, (hi_u8)(*puc_param));

    oam_info_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_FortyMHzIntolerant::end func,puc_param=%d.}",
        *puc_param);
    return HI_SUCCESS;
}

#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
/* ****************************************************************************
 功能描述  : 设置分片门限
 修改历史      :
  1.日    期   : 2014年2月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_frag_threshold(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_frag_threshold_stru *frag_threshold = HI_NULL;

    hi_unref_param(us_len);

    if (oal_unlikely(mac_vap == HI_NULL || puc_param == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_config_frag_threshold:: pst_mac_vap/puc_param is null ptr %p, %p!}\r\n",
            (uintptr_t)mac_vap, (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        oam_warning_log0(0, OAM_SF_ANY,
            "{hmac_config_frag_threshold::[MESH]pst_mac_vap is mesh,not support set frag threshold}\r\n");
        return HI_FAIL;
    }
#endif

    frag_threshold = (mac_cfg_frag_threshold_stru *)puc_param;

    if (mac_vap->mib_info == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_config_frag_threshold:pst_mib_info is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    mac_mib_set_frag_threshold(mac_vap, frag_threshold->frag_threshold);
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 打印接收帧的FCS信息
 修改历史      :
  1.日    期   : 2014年3月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_rx_fcs_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_RX_FCS_INFO, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_rx_fcs_info::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP_DEBUG
#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 打开edca参数调数开关
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2014年12月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_edca_opt_switch_sta(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 flag;
    hi_u32 ret;
    hmac_vap_stru *hmac_vap = HI_NULL;

    /* 获取hmac_vap */
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_edca_opt_switch_sta, hmac_vap_get_vap_stru fail.vap_id = %u",
            mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取配置参数 */
    flag = *puc_param;

    /* 参数没有更改，不需要重新配置 */
    if (flag == hmac_vap->edca_opt_flag_sta) {
        oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_edca_opt_switch_sta, change nothing to flag:%d",
            hmac_vap->edca_opt_flag_sta);
        return HI_SUCCESS;
    }

    /* 设置参数，并启动或者停止edca参数调整定时器 */
    hmac_vap->edca_opt_flag_sta = flag;

    if (hmac_vap->edca_opt_flag_sta == 0) {
        mac_vap_init_wme_param(mac_vap);
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_edca_opt_adj_param_sta succ");
    }

    /* 更新EDCA相关的MAC寄存器 */
    ret = hmac_sta_up_update_edca_params_machw(hmac_vap, MAC_WMM_SET_PARAM_TYPE_UPDATE_EDCA);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY,
            "hmac_config_set_edca_opt_switch_sta: hmac_sta_up_update_edca_params_machw failed");
        return ret;
    }

    oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_edca_opt_switch_sta,config sucess, %d",
        hmac_vap->edca_opt_flag_sta);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 打开edca参数调数开关
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2014年12月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_edca_opt_switch_ap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 flag;
    hmac_vap_stru *hmac_vap = HI_NULL;

    /* 获取hmac_vap */
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_edca_opt_switch_ap, hmac_vap_get_vap_stru fail.vap_id = %u",
            mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取配置参数 */
    flag = *puc_param;

    /* 参数没有更改，不需要重新配置 */
    if (flag == hmac_vap->edca_opt_flag_ap) {
        oam_warning_log1(0, OAM_SF_ANY, "wal_hipriv_set_edca_opt_switch_ap, change nothing to flag:%d",
            hmac_vap->edca_opt_flag_ap);
        return HI_SUCCESS;
    }

    /* 设置参数，并启动或者停止edca参数调整定时器 */
    if (flag == 1) {
        hmac_vap->edca_opt_flag_ap = 1;
        frw_timer_restart_timer(&(hmac_vap->edca_opt_timer), hmac_vap->us_edca_opt_time_ms, HI_TRUE);
    } else {
        hmac_vap->edca_opt_flag_ap = 0;
        frw_timer_stop_timer(&(hmac_vap->edca_opt_timer));
    }

    oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_edca_opt_switch_ap succ, flag = %d", hmac_vap->edca_opt_flag_ap);

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 设置edca调整周期
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2014年12月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_edca_opt_cycle_ap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u16 us_cycle_ms;
    hmac_vap_stru *hmac_vap = HI_NULL;

    /* 获取hmac_vap */
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_edca_opt_cycle_ap, hmac_vap_get_vap_stru fail.vap_id = %u",
            mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    us_cycle_ms = *((hi_u16 *)puc_param);

    /* 判断edca调整周期是否有更新 */
    if (us_cycle_ms == hmac_vap->us_edca_opt_time_ms) {
        oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_edca_opt_cycle_ap, change nothing to cycle:%d",
            hmac_vap->us_edca_opt_time_ms);
        return HI_SUCCESS;
    }

    /* 如果edca调整定时器正在运行，则需要先停止后，再根据新的参数restart */
    hmac_vap->us_edca_opt_time_ms = us_cycle_ms;
    if (hmac_vap->edca_opt_flag_ap == 1) {
        frw_timer_stop_timer(&(hmac_vap->edca_opt_timer));
        FRW_TIMER_RESTART_TIMER(&(hmac_vap->edca_opt_timer), hmac_vap->us_edca_opt_time_ms, HI_TRUE);
    }

    oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_edca_opt_cycle_ap succ, cycle = %d",
        hmac_vap->us_edca_opt_time_ms);

    return HI_SUCCESS;
}

#endif

#ifdef _PRE_WLAN_FEATURE_STA_PM
/* ****************************************************************************
 功能描述  : 120S低功耗定时器超时处理函数
**************************************************************************** */
hi_u32 hmac_set_psm_timeout(hi_void *puc_para)
{
    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)puc_para;
    hi_u32 auto_powersave_val = 0;

    auto_powersave_val |= PM_SWITCH_ON;
    auto_powersave_val |= PM_SWITCH_AUTO_FLAG << 16; /* 左移16位 */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    wlan_pm_set_timeout(WLAN_SLEEP_DEFAULT_CHECK_CNT);
#endif

    if (g_wlan_pm_on == HI_FALSE) {
        oam_warning_log0(0, OAM_SF_CFG, "{hmac_set_psm_timeout::pm off.}");
        return HI_FALSE;
    }
    oam_warning_log0(0, OAM_SF_CFG, "{hmac_set_psm_timeout::set pm}");
    /* **************************************************************************
    抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    return hmac_config_send_event(hmac_vap->base_vap, WLAN_CFGID_SET_PM_SWITCH, sizeof(auto_powersave_val),
        (hi_u8 *)&auto_powersave_val);
}

/* ****************************************************************************
 功能描述  : 低功耗控制接口
 输入参数  : [1]mac_vap
             [2]pm_ctrl_type
             [3]pm_enable
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_pm_by_module(mac_vap_stru *mac_vap, mac_pm_ctrl_type_enum pm_ctrl_type,
    mac_pm_switch_enum pm_enable)
{
    hi_u32 ret;
    mac_cfg_ps_open_stru ps_open = { 0 };

    if (pm_enable >= MAC_STA_PM_SWITCH_BUTT || pm_ctrl_type >= MAC_STA_PM_CTRL_TYPE_BUTT || mac_vap == HI_NULL) {
        oam_error_log3(0, OAM_SF_ANY,
            "hmac_config_set_pm_by_module, PARAM ERROR! pst_mac_vap = %p, pm_ctrl_type = %d, pm_enable = %d ",
            (uintptr_t)mac_vap, pm_ctrl_type, pm_enable);
        return HI_FAIL;
    }

    ps_open.pm_enable = pm_enable;
    ps_open.pm_ctrl_type = pm_ctrl_type;

    ret = hmac_config_set_sta_pm_on(mac_vap, sizeof(mac_cfg_ps_open_stru), (hi_u8 *)&ps_open);
    oam_warning_log3(0, OAM_SF_PWR, "hmac_config_set_pm_by_module, pm_module = %d, pm_enable = %d, cfg ret = %d ",
        pm_ctrl_type, pm_enable, ret);

    return ret;
}
#endif

#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
/* ****************************************************************************
 函 数 名  : hmac_config_set_obss_scan_param
 功能描述  : 整体同步obss scan相关的mib值
 输入参数  : mac_vap_stru *pst_mac_vap
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年3月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_obss_scan_param(const mac_vap_stru *mac_vap)
{
    hi_u32 ret;

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_OBSS_MIB, sizeof(wlan_mib_dot11_operation_entry_stru),
        (hi_u8 *)&mac_vap->mib_info->wlan_mib_operation);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_obss_scan_param::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : dmac_offload架构下同步user关联状态到device侧
 修改历史      :
  1.日    期   : 2014年12月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_user_asoc_state_syn(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user)
{
    hi_u32 ret;
    mac_h2d_user_asoc_state_stru h2d_user_asoc_state_stru;

    h2d_user_asoc_state_stru.user_idx = (hi_u8)mac_user->us_assoc_id;
    h2d_user_asoc_state_stru.asoc_state = mac_user->user_asoc_state;
    /* **************************************************************************
        抛事件到DMAC层, 同步user关联状态到device侧
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_USER_ASOC_STATE_SYN, sizeof(mac_h2d_user_asoc_state_stru),
        (hi_u8 *)(&h2d_user_asoc_state_stru));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_user_asoc_state_syn::send_event failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : dmac_offload架构下同步user速率信息到device侧
 修改历史      :
  1.日    期   : 2015年3月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_user_rate_info_syn(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user)
{
    hi_u32 ret;
    mac_h2d_usr_rate_info_stru mac_h2d_usr_rate_info;

    mac_h2d_usr_rate_info.user_idx = (hi_u8)mac_user->us_assoc_id;
    mac_h2d_usr_rate_info.protocol_mode = mac_user->protocol_mode;
    /* legacy速率集信息，同步到dmac */
    mac_h2d_usr_rate_info.avail_rs_nrates = mac_user->avail_op_rates.rs_nrates;
    if (memcpy_s(mac_h2d_usr_rate_info.auc_avail_rs_rates, WLAN_MAX_SUPP_RATES, mac_user->avail_op_rates.auc_rs_rates,
        WLAN_MAX_SUPP_RATES) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_config_user_rate_info_syn:: auc_rs_rates memcpy_s fail.");
        return HI_FAIL;
    }
    /* ht速率集信息，同步到dmac */
    mac_user_get_ht_hdl(mac_user, &mac_h2d_usr_rate_info.ht_hdl);
    /* **************************************************************************
        抛事件到DMAC层, 同步user关联状态到device侧
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_USER_RATE_SYN, sizeof(mac_h2d_usr_rate_info_stru),
        (hi_u8 *)(&mac_h2d_usr_rate_info));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_user_rate_info_syn::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : dmac_offload架构下同步sta vap信息到 dmac
 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_sta_vap_info_syn(const mac_vap_stru *mac_vap)
{
    hi_u32 ret;
    mac_h2d_vap_info_stru mac_h2d_vap_info;

    mac_h2d_vap_info.us_sta_aid = mac_vap->us_sta_aid;
    mac_h2d_vap_info.uapsd_cap = mac_vap->uapsd_cap;
    /* **************************************************************************
        抛事件到DMAC层, 同步VAP最新状态到DMAC
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_STA_VAP_INFO_SYN, sizeof(mac_h2d_vap_info_stru),
        (hi_u8 *)(&mac_h2d_vap_info));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_sta_vap_info_syn::hmac_config_sta_vap_info_syn failed[%d].}", ret);
    }
    return ret;
}

#endif /* #if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) */

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 设置VAP mib值
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 _hmac_config_set_mib(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 如果是配置VAP, 直接返回 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_CONFIG) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{_hmac_config_set_mib::this is config vap! can't set.}");
        return HI_FAIL;
    }

    mac_config_set_mib(mac_vap, us_len, puc_param);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_MIB, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{_hmac_config_set_mib::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
#else
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : 获取VAP mib值
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 _hmac_config_get_mib(const mac_vap_stru *mac_vap, const hi_u8 *puc_param)
{
    hi_u32 mib_value;
    hi_u32 mib_idx = *((hi_u32 *)puc_param);

    switch (mib_idx) {
        case WLAN_MIB_INDEX_SPEC_MGMT_IMPLEMENT:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_implemented;
            break;

        case WLAN_MIB_INDEX_FORTY_MHZ_OPERN_IMPLEMENT:
            mib_value = (hi_u32)mac_mib_get_forty_mhz_operation_implemented(mac_vap);
            break;

        case WLAN_MIB_INDEX_2040_COEXT_MGMT_SUPPORT:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot112040_bss_coexistence_management_support;
            break;

        case WLAN_MIB_INDEX_FORTY_MHZ_INTOL:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot11_forty_m_hz_intolerant;
            break;

        case WLAN_MIB_INDEX_OBSSSCAN_TRIGGER_INTERVAL:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot11_bss_width_trigger_scan_interval;
            break;

        case WLAN_MIB_INDEX_OBSSSCAN_TRANSITION_DELAY_FACTOR:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot11_bss_width_channel_transition_delay_factor;
            break;

        case WLAN_MIB_INDEX_OBSSSCAN_PASSIVE_DWELL:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_passive_dwell;
            break;

        case WLAN_MIB_INDEX_OBSSSCAN_ACTIVE_DWELL:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_active_dwell;
            break;

        case WLAN_MIB_INDEX_OBSSSCAN_PASSIVE_TOTAL_PER_CHANNEL:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_passive_total_per_channel;
            break;

        case WLAN_MIB_INDEX_OBSSSCAN_ACTIVE_TOTAL_PER_CHANNEL:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_active_total_per_channel;
            break;

        case WLAN_MIB_INDEX_OBSSSCAN_ACTIVITY_THRESHOLD:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_operation.dot11_obss_scan_activity_threshold;
            break;

#ifdef _PRE_WLAN_FEATURE_MESH
        case WLAN_MIB_INDEX_MESH_ACCEPTING_PEER:
            mib_value = (hi_u32)mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_accepting_additional_peerings;
            break;
#endif
        default:
            oam_error_log1(mac_vap->vap_id, OAM_SF_CFG, "{_hmac_config_get_mib::invalid ul_mib_idx[%d].}", mib_idx);
            return HI_FAIL;
    }

    oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{_hmac_config_get_mib::mib value=%d.}", mib_value);

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 用户改变同步保护机制
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_protection_update_from_user(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_PROTECTION_UPDATE_STA_USER, us_len, puc_param);
    if (ret != HI_SUCCESS) {
        hi_diag_log_msg_w0(0, "{hmac_config_set_protection::hmac_config_send_event_etc failed.}");
    }
    return ret;
}

/* ****************************************************************************
 函 数 名  : hmac_config_vap_state_syn
 功能描述  : HMAC同步vap状态到DMAC
    作    者   : HiSilicon
**************************************************************************** */
hi_u32 hmac_config_vap_state_syn(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步VAP最新状态到DMAC
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_VAP_STATE_SYN, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_vap_state_syn::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : dmac offload模式下hmac向dmac同步user cap info的所有内容
 修改历史      :
  1.日    期   : 2015年5月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_user_cap_syn(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user)
{
    hi_u32 ret;
    mac_h2d_usr_cap_stru mac_h2d_usr_cap;

    mac_h2d_usr_cap.user_idx = (hi_u8)mac_user->us_assoc_id;
    if (memcpy_s((hi_u8 *)(&mac_h2d_usr_cap.user_cap_info), sizeof(mac_user_cap_info_stru),
        (hi_u8 *)(&mac_user->cap_info), sizeof(mac_user_cap_info_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_config_user_cap_syn:: st_cap_info memcpy_s fail.");
        return HI_FAIL;
    }
    /* **************************************************************************
        抛事件到DMAC层, 同步VAP最新状态到DMAC
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_USER_CAP_SYN, sizeof(mac_h2d_usr_cap_stru),
        (hi_u8 *)(&mac_h2d_usr_cap));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_user_cap_syn::send_event failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : dmac_offload架构下同步sta usr的状态到dmac
 修改历史      :
  1.日    期   : 2014年12月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_user_info_syn(const mac_vap_stru *mac_vap, const mac_user_stru *mac_user)
{
    hi_u32 ret;
    mac_h2d_usr_info_stru mac_h2d_usr_info;

    mac_h2d_usr_info.avail_bandwidth = mac_user->avail_bandwidth;
    mac_h2d_usr_info.cur_bandwidth = mac_user->cur_bandwidth;
    mac_h2d_usr_info.user_idx = (hi_u8)mac_user->us_assoc_id;
    mac_h2d_usr_info.user_pmf = mac_user->cap_info.pmf_active;
    mac_h2d_usr_info.arg1 = mac_user->ht_hdl.max_rx_ampdu_factor;
    mac_h2d_usr_info.arg2 = mac_user->ht_hdl.min_mpdu_start_spacing;
    mac_h2d_usr_info.user_asoc_state = mac_user->user_asoc_state;

    /* 协议模式信息同步到dmac */
    mac_h2d_usr_info.avail_protocol_mode = mac_user->avail_protocol_mode;

    mac_h2d_usr_info.cur_protocol_mode = mac_user->cur_protocol_mode;
    mac_h2d_usr_info.protocol_mode = mac_user->protocol_mode;
    mac_h2d_usr_info.bandwidth_cap = mac_user->bandwidth_cap;

    /* **************************************************************************
        抛事件到DMAC层, 同步VAP最新状态到DMAC
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_USR_INFO_SYN, sizeof(mac_h2d_usr_info),
        (hi_u8 *)(&mac_h2d_usr_info));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_user_info_syn::hmac_config_send_event failed[%d],user_id[%d].}", ret, mac_user->us_assoc_id);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 初始化用户的加密端口标志
 修改历史      :
  1.日    期   : 2015年5月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_init_user_security_port(const mac_vap_stru *mac_vap, mac_user_stru *mac_user)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hi_u32 ret;
    mac_cfg80211_init_port_stru init_port;
#endif
    /* 初始化认证端口信息 */
    mac_vap_init_user_security_port(mac_vap, mac_user);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* dmac offload模式下同步至device侧 */
    if (memcpy_s(init_port.auc_mac_addr, OAL_MAC_ADDR_LEN, mac_user->user_mac_addr, OAL_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_init_user_security_port:: auc_user_mac_addr memcpy_s fail.");
        return HI_FAIL;
    }
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_INIT_SECURTIY_PORT, OAL_MAC_ADDR_LEN, (hi_u8 *)&init_port);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_config_user_security_port::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 配置用户关联状态，offload模式下同步信息到dmac
 修改历史      :
  1.日    期   : 2015年5月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_user_set_asoc_state(const mac_vap_stru *mac_vap, mac_user_stru *mac_user,
    mac_user_asoc_state_enum_uint8 value)
{
    mac_user_set_asoc_state(mac_user, value);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* dmac offload架构下，同步user关联状态信息到dmac */
    hi_u32 ret = hmac_config_user_asoc_state_syn(mac_vap, mac_user);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_CFG,
                       "{hmac_user_set_asoc_state::user_asoc_state_syn failed[%d].}", ret);
    }
    return ret;
#else
    hi_unref_param(mac_vap);
    return HI_SUCCESS;
#endif
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 设置聚合最大个数
 修改历史      :
  1.日    期   : 2014年10月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_ampdu_aggr_num(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_AGGR_NUM, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_ampdu_aggr_num::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

/* ****************************************************************************
 功能描述  : 通过设置mib值, 设置AP的STBC能力
 修改历史      :
  1.日    期   : 2014年11月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_stbc_cap(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 value = *puc_param;

    hi_unref_param(us_len);

    if (oal_unlikely(mac_vap->mib_info == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_stbc_cap::pst_mac_vap->pst_mib_info null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (value == 0) {
        mac_vap->mib_info->phy_ht.dot11_tx_stbc_option_implemented = HI_FALSE;
        mac_vap->mib_info->phy_ht.dot11_rx_stbc_option_implemented = HI_FALSE;
        mac_vap->mib_info->phy_ht.dot11_tx_stbc_option_activated = HI_FALSE;
    } else {
        mac_vap->mib_info->phy_ht.dot11_tx_stbc_option_implemented = HI_TRUE;
        mac_vap->mib_info->phy_ht.dot11_rx_stbc_option_implemented = HI_TRUE;
        mac_vap->mib_info->phy_ht.dot11_tx_stbc_option_activated = HI_TRUE;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) /* hi1131-cb set at both side (HMAC to DMAC) */
    return hmac_config_send_event(mac_vap, WLAN_CFGID_SET_STBC_CAP, us_len, puc_param);
#else
    return HI_SUCCESS;
#endif
}


#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
hi_u32 hmac_vap_start_xmit_check(oal_net_device_stru *netdev)
{
    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_vap_start_xmit::pst_dev = HI_NULL!}\r\n");
        oal_netbuf_free(netbuf);
        return HI_SUCCESS;
    }

    /* 获取VAP结构体 */
    mac_vap_stru *mac_vap = (mac_vap_stru *)oal_net_dev_priv(netdev);
    /* 如果VAP结构体不存在，则丢弃报文 */
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_vap_start_xmit::pst_vap = HI_NULL!}\r\n");
        oal_netbuf_free(netbuf);
        return HI_SUCCESS;
    }

    return HI_CONTINUE;
}

/* ****************************************************************************
 功能描述  : 挂接到VAP对应net_device结构体下的发送函数
 输入参数  : pst_buf: SKB结构体,其中data指针指向以太网头
             pst_dev: net_device结构体
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2012年11月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
oal_net_dev_tx_enum hmac_vap_start_xmit(oal_netbuf_stru *netbuf, oal_net_device_stru *netdev)
{
    hi_u32 ret = hmac_vap_start_xmit_check(netdev);
    if (ret == HI_SUCCESS) {
        return OAL_NETDEV_TX_OK;
    }

    mac_vap_stru *mac_vap = (mac_vap_stru *)oal_net_dev_priv(netdev);

    netbuf = oal_netbuf_unshare(netbuf);
    if (netbuf == HI_NULL) {
        oam_error_log0(0, OAM_SF_TX, "{wal_vap_start_xmit::the unshare netbuf = HI_NULL!}\r\n");
        return OAL_NETDEV_TX_OK;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log1(0, OAM_SF_TX, "{wal_vap_start_xmit::pst_hmac_vap[%d] = HI_NULL!}", mac_vap->vap_id);
        oal_netbuf_free(netbuf);
        return OAL_NETDEV_TX_OK;
    }

    /* 防止下行来包太多，造成软件处理来不及，造成软件积累包太多，skb内存不能及时释放，入队限制修改为300，
       MIPS降低后，这个值可以抬高 */
    if (oal_netbuf_list_num(&hmac_vap->tx_queue_head[hmac_vap->in_queue_id]) >= 300) { /* 300 用于判断 */
        /* 关键帧做100个缓存，保证关键帧的正常发送 */
        if (oal_netbuf_list_num(&hmac_vap->tx_queue_head[hmac_vap->in_queue_id]) < 400) { /* 400 用于判断 */
            hi_u8 data_type;

            data_type = mac_get_data_type_from_8023((hi_u8 *)oal_netbuf_payload(netbuf), MAC_NETBUFF_PAYLOAD_ETH);
            if ((data_type == MAC_DATA_EAPOL) ||
                (data_type >= MAC_DATA_DHCP_DISCOVER && data_type <= MAC_DATA_DHCP_ACK) ||
                (data_type == MAC_DATA_ARP_REQ) || (data_type == MAC_DATA_ARP_RSP)) {
                hi_task_lock();
                OAL_NETBUF_QUEUE_TAIL(&(hmac_vap->tx_queue_head[hmac_vap->in_queue_id]), netbuf);
                hi_task_unlock();
            } else {
                oal_netbuf_free(netbuf);
            }
        } else {
            oal_netbuf_free(netbuf);
        }

        if (g_tx_debug) {
            /* 增加维测信息，把tx_event_num的值打印出来，用户关连不上，或者一直ping不通，打开g_tx_debug开关，
               如果此时的值不为1，就属于异常 */
            oam_error_log1(mac_vap->vap_id, OAM_SF_TX, "{wal_vap_start_xmit::tx_event_num value is [%d].}",
                (hi_s32)hi_atomic_read(&(hmac_vap->tx_event_num)));
            oal_io_print("wal_vap_start_xmit too fast\n");
        }
    } else {
        if (g_tx_debug) {
            oal_io_print("wal_vap_start_xmit enqueue and post event\n");
        }

        hi_task_lock();
        OAL_NETBUF_QUEUE_TAIL(&(hmac_vap->tx_queue_head[hmac_vap->in_queue_id]), netbuf);
        hi_task_unlock();
    }

    hmac_tx_post_event(mac_vap);

    return OAL_NETDEV_TX_OK;
}

#endif

hi_u32 hmac_bridge_vap_xmit_check(oal_netbuf_stru *netbuf, const oal_net_device_stru *netdev)
{
    if (oal_unlikely(netbuf == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{hmac_bridge_vap_xmit::pst_buf = HI_NULL!}\r\n");
        return HI_FAIL;
    }

    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{hmac_bridge_vap_xmit::pst_dev = HI_NULL!}\r\n");
        oal_netbuf_free(netbuf);
        return HI_FAIL;
    }

    /* 获取VAP结构体, 如果VAP结构体不存在，则丢弃报文 */
    mac_vap_stru *mac_vap = (mac_vap_stru *)oal_net_dev_priv(netdev);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{hmac_bridge_vap_xmit::pst_vap = HI_NULL!}\r\n");
        oal_netbuf_free(netbuf);
        return HI_FAIL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_bridge_vap_xmit::pst_hmac_vap null.}");
        oal_netbuf_free(netbuf);
        return HI_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    if (mac_vap->al_tx_flag == HI_SWITCH_ON) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_TX, "{hmac_bridge_vap_xmit::the vap alway tx/rx!}");
        oal_netbuf_free(netbuf);
        return HI_FAIL;
    }
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 挂接到VAP对应net_device结构体下的发送函数
 输入参数  : pst_buf: SKB结构体,其中data指针指向以太网头
             pst_dev: net_device结构体
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2012年11月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
oal_net_dev_tx_enum hmac_bridge_vap_xmit(oal_net_device_stru *netdev, oal_netbuf_stru *netbuf)
{
    if (hmac_bridge_vap_xmit_check(netbuf, netdev) == HI_FAIL) {
        return OAL_NETDEV_TX_OK;
    }

    mac_vap_stru *mac_vap = (mac_vap_stru *)oal_net_dev_priv(netdev);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    netbuf = oal_netbuf_unshare(netbuf);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    netbuf = oal_netbuf_unshare(netbuf, GFP_ATOMIC);
#endif
    if (oal_unlikely(netbuf == HI_NULL)) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_TX, "{hmac_bridge_vap_xmit::the unshare netbuf = HI_NULL!}");
        return OAL_NETDEV_TX_OK;
    }

#ifdef _PRE_WLAN_FEATURE_WOW
    /* wow 过滤开关 */
    if (HI_TRUE == hmac_wow_tx_check_filter_switch()) {
        oal_netbuf_free(netbuf);
        return OAL_NETDEV_TX_OK;
    }
#endif

    /* 判断VAP的状态，如果没有UP/PAUSE，则丢弃报文 */
    if (oal_unlikely(!((mac_vap->vap_state == MAC_VAP_STATE_UP) || (mac_vap->vap_state == MAC_VAP_STATE_PAUSE)))) {
        oam_info_log1(mac_vap->vap_id, OAM_SF_TX, "{hmac_bridge_vap_xmit::vap state[%d] != MAC_VAP_STATE_{UP|PAUSE}}",
            mac_vap->vap_state);
        oal_netbuf_free(netbuf);
        return OAL_NETDEV_TX_OK;
    }

#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
    /* 在将cb字段清零前，将extflag取出 */
    hi_u16 us_pbuf_flags = *((hi_u16 *)(netbuf->cb));
#endif

    set_oal_netbuf_next(netbuf, HI_NULL);
    set_oal_netbuf_prev(netbuf, HI_NULL);

    /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(oal_netbuf_cb(netbuf), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
    /* 对cb 字段中的extflag赋值 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        hmac_tx_ctl_stru *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(netbuf);
        /*
        us_pbuf_flags:
        |BIT13       |BIT12           |
        |Ctrl PKT    |Compressed 6lo  |
        */
        tx_ctl->pbuf_flags = (us_pbuf_flags & (PBUF_FLAG_6LO_PKT | PBUF_FLAG_CTRL_PKT)) >> 12; /* 右移12位 */
    }
#endif

    oal_net_dev_tx_enum ret_value = (hmac_tx_lan_to_wlan(mac_vap, netbuf) != HI_SUCCESS) ?
        OAL_NETDEV_TX_BUSY : OAL_NETDEV_TX_OK;
    if (oal_unlikely(ret_value != OAL_NETDEV_TX_OK)) {
        hmac_free_netbuf_list(netbuf);
    }

    return OAL_NETDEV_TX_OK;
}

/* ****************************************************************************
 功能描述  : 扫描终止
 修改历史      :
  1.日    期   : 2015年6月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_scan_abort(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    hmac_device_stru *hmac_dev = HI_NULL;
    hi_u32 ret;

    oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN,
                     "{hmac_config_scan_abort::vap_id[%d] scan abort.}", mac_vap->vap_id);

    hmac_dev = hmac_get_device_stru();

    /* 若下发的是ANY扫描，结束扫描的时候恢复标志为非ANY扫描，若下发的非ANY扫描，这里赋该值无影响 */
    hmac_dev->scan_mgmt.scan_record_mgmt.is_any_scan = HI_FALSE;

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN, "{hmac_config_scan_abort::pst_hmac_vap is null, vap_id[%d].}",
            mac_vap->vap_id);
        return HI_ERR_CODE_MAC_DEVICE_NULL;
    }

    if (((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
        ) && (hmac_dev->scan_mgmt.scan_record_mgmt.vap_last_state != MAC_VAP_STATE_BUTT)) {
        oam_warning_log1(0, OAM_SF_SCAN,
                         "{hmac_config_scan_abort::en_vap_last_state:%d}",
                         hmac_dev->scan_mgmt.scan_record_mgmt.vap_last_state);
        hmac_fsm_change_state(hmac_vap, hmac_dev->scan_mgmt.scan_record_mgmt.vap_last_state);
        hmac_dev->scan_mgmt.scan_record_mgmt.vap_last_state = MAC_VAP_STATE_BUTT;
    }
    /* 清除扫描结果上报的回调函数，无需上报 */
    if (hmac_dev->scan_mgmt.scan_record_mgmt.vap_id == mac_vap->vap_id) {
        hmac_dev->scan_mgmt.scan_record_mgmt.fn_cb = HI_NULL;
        hmac_dev->scan_mgmt.is_scanning = HI_FALSE;
    }

    /* **************************************************************************
                         抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SCAN_ABORT, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN,
                         "{hmac_config_scan_abort::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  :host侧查询station info
 输入参数  : pst_mac_vap: mac_vap_stru
             us_len       : 参数长度
             puc_param    : 参数
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2014年11月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_query_station_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_QUERY_STATION_STATS, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_phy_stat_info::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

#ifdef _PRE_WLAN_FEATURE_BTCOEX
/* ****************************************************************************
 功能描述  : 共存删除BA会话
 修改历史      :
  1.日    期   : 2015年7月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_btcoex_delba_foreach_tid(mac_vap_stru *mac_vap, const mac_user_stru *mac_user,
    mac_cfg_delba_req_param_stru *mac_cfg_delba_param)
{
    hi_u32 ret = 0;

    if (memcpy_s(mac_cfg_delba_param->auc_mac_addr, WLAN_MAC_ADDR_LEN,
                 mac_user->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_btcoex_delba_foreach_tid::mem safe function err!}");
        return HI_FAIL;
    }

    for (mac_cfg_delba_param->tidno = 0; mac_cfg_delba_param->tidno < WLAN_TID_MAX_NUM; mac_cfg_delba_param->tidno++) {
        ret = hmac_config_delba_req(mac_vap, 0, (hi_u8 *)mac_cfg_delba_param);
        if (ret != HI_SUCCESS) {
            oam_warning_log2(mac_vap->vap_id, OAM_SF_COEX, "{hmac_btcoex_delba_foreach_tid::ul_ret: %d, tid: %d}", ret,
                mac_cfg_delba_param->tidno);
            return ret;
        }
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : hmac删除BA
 修改历史      :
  1.日    期   : 2015年7月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_btcoex_delba_foreach_user(mac_vap_stru *mac_vap)
{
    hi_u32 ret = 0;
    mac_cfg_delba_req_param_stru mac_cfg_delba_param;
    mac_user_stru *mac_user = HI_NULL;
    hi_u8 user_idx;
    hi_list *entry = HI_NULL;
    mac_res_user_hash_stru *res_hash = HI_NULL;

    mac_cfg_delba_param.direction = MAC_RECIPIENT_DELBA;

    for (user_idx = 0; user_idx < MAC_VAP_USER_HASH_MAX_VALUE; user_idx++) {
        hi_list_for_each(entry, &(mac_vap->ast_user_hash[user_idx])) {
            res_hash = (mac_res_user_hash_stru *)entry;

            mac_user = mac_user_get_user_stru(res_hash->user_idx);
            if (mac_user == HI_NULL) {
                oam_warning_log1(mac_vap->vap_id, OAM_SF_COEX,
                    "{hmac_btcoex_delba_foreach_user::pst_mac_user null, user_idx: %d.}", res_hash->user_idx);
                entry = res_hash->entry.next;
                continue;
            }
            ret = hmac_btcoex_delba_foreach_tid(mac_vap, mac_user, &mac_cfg_delba_param);
            if (ret != HI_SUCCESS) {
                return ret;
            }
        }
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : hmac删除BA
 修改历史      :
  1.日    期   : 2015年7月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_btcoex_rx_delba_trigger(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hmac_device_stru *hmac_dev = HI_NULL;
    d2h_btcoex_delba_event_stru *d2h_btcoex_delba = HI_NULL;
    hi_u32 ret;

    hi_unref_param(len);

    hmac_dev = hmac_get_device_stru();
    d2h_btcoex_delba = (d2h_btcoex_delba_event_stru *)puc_param;

    hmac_dev->d2h_btcoex_delba.need_delba = d2h_btcoex_delba->need_delba;
    hmac_dev->d2h_btcoex_delba.ba_size = d2h_btcoex_delba->ba_size;
    if (hmac_dev->d2h_btcoex_delba.need_delba) {
        ret = hmac_btcoex_delba_foreach_user(mac_vap);
        if (ret != HI_SUCCESS) {
            oam_warning_log1(mac_vap->vap_id, OAM_SF_COEX, "{hmac_btcoex_syn:delba send failed:ul_ret: %d}", ret);
            return ret;
        }
    }
    return HI_SUCCESS;
}
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : reg_info test
 修改历史      :
  1.日    期   : 2015年7月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_wifitest_get_reg_info(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hmac_reg_info_receive_event *dmac_reg_info_response_event = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;

    hi_unref_param(len);

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_wifitest_get_reg_info::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    mac_vap = hmac_vap->base_vap;
    dmac_reg_info_response_event = (hmac_reg_info_receive_event *)puc_param;
    g_hmac_reg_info_receive_event.reg_info_num = dmac_reg_info_response_event->reg_info_num;
    if (g_hmac_reg_info_receive_event.reg_info_num > REG_INFO_MAX_NUM) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "hmac_wifitest_get_reg_info:: reg numb is out of range[%d]",
            g_hmac_reg_info_receive_event.reg_info_num);
        return HI_FAIL;
    }
    while (dmac_reg_info_response_event->reg_info_num) {
        g_hmac_reg_info_receive_event.val[dmac_reg_info_response_event->reg_info_num - 1] =
            dmac_reg_info_response_event->val[dmac_reg_info_response_event->reg_info_num - 1];
        dmac_reg_info_response_event->reg_info_num--;
    }

    g_hmac_reg_info_receive_event.flag = HI_TRUE;
    hi_wait_queue_wake_up_interrupt(&(hmac_vap->query_wait_q));

    return HI_SUCCESS;
}
#endif
#endif

/* ****************************************************************************
 功能描述  : 输出常收结果
 修改历史      :
  1.日    期   : 2019年6月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_get_rx_fcs_info(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param)
{
    hi_u32 succ_num;
    if (mac_vap == HI_NULL || param == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_get_rx_fcs_info::pst_mac_vap[%p] NULL or pst_param[%p] NULL!}",
            (uintptr_t)mac_vap, (uintptr_t)param);
        return HI_ERR_CODE_PTR_NULL;
    }
    hi_unref_param(len);
    hi_unref_param(mac_vap);

    succ_num = *((hi_u32 *)param);
    /* 勿删,自动化测试需要用 */
#ifdef CUSTOM_AT_COMMAND
    hi_at_printf("+RXINFO:%d\r\n", succ_num);
    hi_at_printf("OK\r\n");
#endif
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    printk("+RXINFO:%d\r\n", succ_num);
    printk("OK\r\n");
#endif
    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
hi_u32 hmac_print_ate_paras(const oal_at_rsp_stru *rsp, const hi_u8 *param, hi_u8 len)
{
    if ((len < ((OAL_AT_ATE_PARAS_BUTT + 1) << 2)) || (rsp->data_num != OAL_AT_ATE_PARAS_BUTT)) { /* 2:每个字段4字节 */
        printk("ERROR\r\nReport data len invalid,len %d, data_num %d\r\n", len, rsp->data_num);
        return HI_FAIL;
    }
    hi_u32 *data = (hi_u32 *)(param + sizeof(oal_at_rsp_stru));
    printk("+RCALDATA:Efuse cali chance(s) left:%d times.\r\n", *((hi_s32 *)&data[OAL_AT_ATE_PARAS_USED_CNT]));
    printk("+RCALDATA:freq_offset %d\r\n", *((hi_s32 *)&data[OAL_AT_ATE_PARAS_FREQ_OFFSET]));
    printk("+RCALDATA:band_pwr_offset_0 %d\r\n", *((hi_s32 *)&data[OAL_AT_ATE_PARAS_BPWR_OFFSET_0]));
    printk("+RCALDATA:band_pwr_offset_1 %d\r\n", *((hi_s32 *)&data[OAL_AT_ATE_PARAS_BPWR_OFFSET_1]));
    printk("+RCALDATA:band_pwr_offset_2 %d\r\n", *((hi_s32 *)&data[OAL_AT_ATE_PARAS_BPWR_OFFSET_2]));

    printk("+RCALDATA:rate_pwr_offset_11n 0x%x\r\n", data[OAL_AT_ATE_PARAS_DBB_OFFSET_11N]);
    printk("+RCALDATA:rate_pwr_offset_11g 0x%x\r\n", data[OAL_AT_ATE_PARAS_DBB_OFFSET_11G]);
    printk("+RCALDATA:rate_pwr_offset_11b 0x%x\r\n", data[OAL_AT_ATE_PARAS_DBB_OFFSET_11B]);
    printk("+RCALDATA:dbb_scale_0 0x%x\r\n", data[OAL_AT_ATE_PARAS_DBB_SCALE_0]);
    printk("+RCALDATA:dbb_scale_1 0x%x\r\n", data[OAL_AT_ATE_PARAS_DBB_SCALE_1]);
    printk("+RCALDATA:dbb_scale_2 0x%x\r\n", data[OAL_AT_ATE_PARAS_DBB_SCALE_2]);
    printk("+RCALDATA:dbb_scale_3 0x%x\r\n", data[OAL_AT_ATE_PARAS_DBB_SCALE_3]);
    printk("+RCALDATA:dbb_scale_4 0x%x\r\n", data[OAL_AT_ATE_PARAS_DBB_SCALE_4]);
    printk("+RCALDATA:freq_and_band_pwr_hybrid_offset 0x%x\r\n", data[OAL_AT_ATE_PARAS_HYBRID_DATA]);

    return HI_SUCCESS;
}

hi_u32 hmac_print_ate_mac(const oal_at_rsp_stru *rsp, const hi_u8 *param, hi_u8 len)
{
    if ((len < ((OAL_AT_ATE_MAC_BUTT + 1) << 2)) || (rsp->data_num != OAL_AT_ATE_MAC_BUTT)) { /* 2:每个字段4字节 */
        printk("ERROR\r\nReport data len invalid,len %d, data_num %d\r\n", len, rsp->data_num);
        return HI_FAIL;
    }
    hi_u8 *mac = (hi_u8 *)(param + sizeof(oal_at_rsp_stru));

    printk("+EFUSEMAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); /* 0 1 2 3 4 5：mac地址偏移 */
    hi_u32 *times_left = (hi_u32 *)(param + sizeof(oal_at_rsp_stru) + AT_ATE_MAC_SIZE);
    printk("+EFUSEMAC:Efuse mac chance(s) left:%d times.\r\n", *times_left); /* 剩余写机会总数 */

    return HI_SUCCESS;
}

hi_u32 hmac_report_mfg_test(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param)
{
    if (mac_vap == HI_NULL || param == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_config_set_pm_switch::pst_mac_vap[%p] NULL or pst_param[%p] NULL!}",
            (uintptr_t)mac_vap, (uintptr_t)param);
        return HI_ERR_CODE_PTR_NULL;
    }
    hi_unref_param(mac_vap);

    if (len < sizeof(oal_at_rsp_stru)) {
        printk("ERROR\r\nRsp format error\r\n");
        return HI_SUCCESS;
    }
    oal_at_rsp_stru *rsp = (oal_at_rsp_stru *)param;
    if (rsp->result != 0) { /* 失败 */
        printk("ERROR\r\n");
        return HI_SUCCESS;
    } else if (rsp->num != 0) { /* 需打印内容 */
        switch (rsp->num) {
            case AT_RSP_ATE_PARAS:
                if (hmac_print_ate_paras(rsp, param, len) != HI_SUCCESS) {
                    return HI_SUCCESS;
                }
                break;
            case AT_RSP_ATE_MAC:
                if (hmac_print_ate_mac(rsp, param, len) != HI_SUCCESS) {
                    return HI_SUCCESS;
                }
                break;
            default:
                printk("ERROR\r\nInvalid rsp num %d\r\n", rsp->result);
                return HI_SUCCESS;
        }
    }
    printk("OK\r\n");
    return HI_SUCCESS;
}
#endif

hi_u32 hmac_report_mac_from_efuse(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param)
{
    frw_event_mem_stru         *event_mem = HI_NULL;
    frw_event_stru             *event = HI_NULL;
    hi_u32                      ret;
    hi_unref_param(len);

    if (param == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_report_mac_from_efuse::puc_param is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 抛事件到WAL */
    event_mem = frw_event_alloc(ETHER_ADDR_LEN);
    if (event_mem == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_report_mac_from_efuse::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CTX, HMAC_HOST_CTX_EVENT_GET_MAC_FROM_EFUSE,
        WLAN_MAC_ADDR_LEN, FRW_EVENT_PIPELINE_STAGE_0, mac_vap->vap_id);

    if (memcpy_s(event->auc_event_data, WLAN_MAC_ADDR_LEN, param, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_report_mac_from_efuse:: param memcpy fail.");
        return HI_FAIL;
    }
    /* 分发事件到WAL层 */
    ret = frw_event_dispatch_event(event_mem);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_report_mac_from_efuse::frw_event_dispatch_event fail[%d].}", ret);
        frw_event_free(event_mem);
        return ret;
    }
    frw_event_free(event_mem);
    return HI_SUCCESS;
}

hi_u32 hmac_config_report_tx_params(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param)
{
    frw_event_mem_stru         *event_mem = HI_NULL;
    frw_event_stru             *event = HI_NULL;
    hi_u32                      ret;
    hi_unref_param(len);

    if (param == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_config_report_theory_goodput::puc_param is null!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 抛事件到WAL */
    event_mem = frw_event_alloc(sizeof(hamc_config_report_tx_params_stru) * WLAN_WME_AC_BUTT);
    if (event_mem == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_report_theory_goodput::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CTX, HMAC_HOST_CTX_EVENT_REPORT_TX_PARAMS,
        sizeof(hamc_config_report_tx_params_stru) * WLAN_WME_AC_BUTT, FRW_EVENT_PIPELINE_STAGE_0, mac_vap->vap_id);

    if (memcpy_s(event->auc_event_data, sizeof(hamc_config_report_tx_params_stru) * WLAN_WME_AC_BUTT, param,
        sizeof(hamc_config_report_tx_params_stru) * WLAN_WME_AC_BUTT) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_config_report_theory_goodput:: param memcpy fail.");
        return HI_FAIL;
    }

    /* 分发事件到WAL层 */
    ret = frw_event_dispatch_event(event_mem);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_report_theory_goodput::frw_event_dispatch_event fail[%d].}", ret);
        frw_event_free(event_mem);
        return ret;
    }

    frw_event_free(event_mem);
    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
hi_u32 hmac_report_dbg_cal_data_from_dev(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *param)
{
    frw_event_mem_stru         *event_mem = HI_NULL;
    frw_event_stru             *event = HI_NULL;
    hi_u32                      ret;
    hi_unref_param(len);
    const hi_u8 data_size = 28; /* 28:7个补偿值，每个4字节 */
    if (mac_vap == HI_NULL || param == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_report_dbg_cal_data_from_dev::mac_vap[%p] NULL or param[%p] NULL!}",
            (uintptr_t)mac_vap, (uintptr_t)param);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 抛事件到WAL */
    event_mem = frw_event_alloc(data_size);
    if (event_mem == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_report_dbg_cal_data_from_dev::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CTX, HMAC_HOST_CTX_EVENT_GET_DBG_CAL_DATA,
        WLAN_MAC_ADDR_LEN, FRW_EVENT_PIPELINE_STAGE_0, mac_vap->vap_id);

    if (memcpy_s(event->auc_event_data, data_size, param, data_size) != EOK) {
        oam_error_log0(0, 0, "hmac_report_dbg_cal_data_from_dev:: memcpy_s fail.");
        return HI_FAIL;
    }
    /* 分发事件到WAL层 */
    ret = frw_event_dispatch_event(event_mem);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_report_dbg_cal_data_from_dev::frw_event_dispatch_event fail[%d].}", ret);
        frw_event_free(event_mem);
        return ret;
    }
    frw_event_free(event_mem);
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : hmac接收dmac抛回来的查询RSSI应答
 修改历史      :
  1.日    期   : 2019年6月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_proc_query_rssi_response(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    wlan_rssi_stru *rssi_param = HI_NULL;

    hi_unref_param(len);
    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_proc_query_rssi_response::mac_vap[%p] NULL or pst_puc_param[%p] NULL!}",
            (uintptr_t)mac_vap, (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_query_response::pst_hmac_vap null.}");
        return HI_FAIL;
    }

    rssi_param = (wlan_rssi_stru *)puc_param;
    hmac_vap->ap_rssi = rssi_param->rssi;
    hmac_vap->query_ap_rssi_flag = HI_TRUE;
    hi_wait_queue_wake_up_interrupt(&(hmac_vap->query_wait_q));

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
/* ****************************************************************************
 功能描述  : sta uspad 配置命令
 修改历史      :
  1.日    期   : 2015年2月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_uapsd_para(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_uapsd_sta_stru *uapsd_param = HI_NULL;
    mac_device_stru *mac_dev = HI_NULL;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hi_u32 ret;
#endif

    /* wmm */
    mac_dev = mac_res_get_dev();
    if (mac_dev->wmm == HI_FALSE) {
        oam_warning_log0(0, OAM_SF_UM, "{hmac_config_set_uapsd_para::wmm is off, not support uapsd mode}");
        return HI_FAIL;
    }

    /* mesh */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_PWR, "{hmac_config_set_uapsd_para::vap mode mesh,not support uapsd!}");
        return HI_FAIL;
    }

    /* 窄带 */
    if ((mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_5M) ||
        (mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_10M)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_PWR,
            "{hmac_config_set_uapsd_para::narrow band[%dM] mode,not support uapsd!}", mac_vap->channel.en_bandwidth);
        return HI_FAIL;
    }

    uapsd_param = (mac_cfg_uapsd_sta_stru *)puc_param;
    if (uapsd_param->max_sp_len >= MAC_APSD_SP_LEN_BUTT) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_PWR, "{hmac_config_set_uapsd_para::uc_max_sp_len[%d] > 6!}",
            uapsd_param->max_sp_len);
        return HI_FAIL;
    }
    mac_vap_set_uapsd_para(mac_vap, uapsd_param);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* **************************************************************************
        抛事件到DMAC层, 同步VAP最新状态到DMAC
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_UAPSD_PARA, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_uapsd_para::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }
#else
    hi_unref_param(us_len);
#endif
    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_STA_PM
/* ****************************************************************************
 功能描述  : 配置staut低功耗模式
 修改历史      :
  1.日    期   : 2015年10月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_sta_pm_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    return hmac_config_sync_cmd_common(mac_vap, WLAN_CFGID_SET_PS_MODE, us_len, puc_param);
}

/* ****************************************************************************
 功能描述  : 打开staut低功耗
 修改历史      :
  1.日    期   : 2015年10月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 _hmac_config_set_sta_pm_on(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hmac_vap_stru *hmac_vap = HI_NULL;

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_PWR, "{_hmac_config_set_sta_pm_on::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 再下发打开低功耗 */
    return hmac_config_sync_cmd_common(mac_vap, WLAN_CFGID_SET_STA_PM_ON, us_len, puc_param);
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_psm_offset(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_PSM_OFFSET, len, param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_psm_offset::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

hi_u32 hmac_config_set_sta_hw_ps_mode(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_STA_HW_PS_MODE, len, param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_sta_hw_ps_mode::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif
#endif

#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
/* ****************************************************************************
 功能描述  : 获取吞吐量，打印到host侧，便于自动化脚本获取结果
 修改历史      :
  1.日    期   : 2015年2月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_get_thruput_info(mac_vap_stru *mac_vap, hi_u8 len, const hi_u8 *puc_param)
{
    hi_unref_param(mac_vap);
    hi_unref_param(len);
    dmac_thruput_info_sync_stru *thruput_info = HI_NULL;

    thruput_info = (dmac_thruput_info_sync_stru *)puc_param;

    if (thruput_info != HI_NULL) {
        oal_io_print1("interval cycles: %u \n", thruput_info->cycles);
        oal_io_print1("sw tx succ num: %u \n", thruput_info->sw_tx_succ_num);
        oal_io_print1("sw tx fail num: %u \n", thruput_info->sw_tx_fail_num);
        oal_io_print1("sw rx ampdu succ num: %u \n", thruput_info->sw_rx_ampdu_succ_num);
        oal_io_print1("sw rx mpdu succ num: %u \n", thruput_info->sw_rx_mpdu_succ_num);
        oal_io_print1("sw rx fail num: %u \n", thruput_info->sw_rx_ppdu_fail_num);
        oal_io_print1("hw rx ampdu fcs fail num: %u \n", thruput_info->hw_rx_ampdu_fcs_fail_num);
        oal_io_print1("hw rx mpdu fcs fail num: %u \n", thruput_info->hw_rx_mpdu_fcs_fail_num);
        return HI_SUCCESS;
    } else {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_get_thruput_info::pst_thruput_info null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
}
#endif

#ifdef _PRE_WLAN_FEATURE_PMF
/* ****************************************************************************
 功能描述  : chip test强制配置pmf能力，且对关联后的vap也生效
 修改历史      :
  1.日    期   : 2015年1月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_enable_pmf(mac_vap_stru *mac_vap, hi_u8 *puc_param)
{
    hi_u8 pmf_active;
    hi_list *entry = HI_NULL;
    hi_list *user_list_head = HI_NULL;
    mac_user_stru *user_tmp = HI_NULL;

    oal_io_print("hmac_enable_pmf: func start!");
    if (mac_vap == HI_NULL || puc_param == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "hmac_enable_pmf:: pointer is null: pst_mac_vap[%p],puc_param[%p]",
            (uintptr_t)mac_vap, (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }

    wlan_pmf_cap_status_uint8 *puc_pmf_cap = (wlan_pmf_cap_status_uint8 *)puc_param;

    switch (*puc_pmf_cap) {
        case MAC_PMF_DISABLED:
            mac_mib_set_dot11_rsnamfpr(mac_vap, HI_FALSE);
            mac_mib_set_dot11_rsnamfpc(mac_vap, HI_FALSE);
            mac_mib_set_dot11_rsnaactivated(mac_vap, HI_FALSE);
            pmf_active = HI_FALSE;
            break;
        case MAC_PMF_ENABLED:
            mac_mib_set_dot11_rsnamfpr(mac_vap, HI_FALSE);
            mac_mib_set_dot11_rsnamfpc(mac_vap, HI_TRUE);
            mac_mib_set_dot11_rsnaactivated(mac_vap, HI_TRUE);
            return HI_SUCCESS;
        case MAC_PME_REQUIRED:
            mac_mib_set_dot11_rsnamfpr(mac_vap, HI_TRUE);
            mac_mib_set_dot11_rsnamfpc(mac_vap, HI_TRUE);
            mac_mib_set_dot11_rsnaactivated(mac_vap, HI_TRUE);
            pmf_active = HI_TRUE;
            break;
        default:
            oal_io_print("hmac_enable_pmf: commend error!");
            return HI_FALSE;
    }

    if (mac_vap->vap_state == MAC_VAP_STATE_UP) {
        user_list_head = &(mac_vap->mac_user_list_head);

        for (entry = user_list_head->next; entry != user_list_head;) {
            user_tmp = hi_list_entry(entry, mac_user_stru, user_dlist);

            /* 指向双向链表下一个节点 */
            entry = entry->next;
            if (user_tmp == HI_NULL) { // user_tmp有为空的可能，误报告警，lin_t e774告警屏蔽
                oam_error_log0(0, OAM_SF_ANY, "hmac_enable_pmf:: pst_user_tmp is null");
                return HI_ERR_CODE_PTR_NULL;
            }
            mac_user_set_pmf_active(user_tmp, pmf_active);
        }
    }

    oal_io_print("hmac_enable_pmf: func end!");

    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
/* ****************************************************************************
 功能描述      : 配置ARP offload信息

 修改历史      :
  1.日    期   : 2015年5月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_arp_offload_setting(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 配置DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_ARP_OFFLOAD_SETTING, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_arp_offload_setting::hmac_config_send_event fail[%d].", ret);
    }

    return ret;
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述      : 显示Device侧记录的IP地址

 修改历史      :
  1.日    期   : 2015年8月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_arp_offload_show_info(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 配置DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_ARP_OFFLOAD_SHOW_INFO, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_arp_offload_show_info::hmac_config_send_event fail[%d].", ret);
    }

    return ret;
}
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_DHCP_OFFLOAD
/* ****************************************************************************
 功能描述      : 配置IP地址信息

 修改历史      :
  1.日    期   : 2019年10月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_dhcp_offload_setting(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 配置DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DHCP_OFFLOAD_SETTING, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_config_dhcp_offload_setting::hmac_config_send_event fail[%d].", ret);
    }

    return ret;
}
#endif

#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
/* ****************************************************************************
 功能描述  : cfg vap h2d
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_cfg_vap_h2d(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_unref_param(us_len);
    hi_u32 ret;
    mac_device_stru *mac_dev = HI_NULL;

    if (oal_unlikely((mac_vap == HI_NULL) || (puc_param == HI_NULL))) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_add_vap::param null,pst_vap=%d puc_param=%d.}", (uintptr_t)mac_vap,
            (uintptr_t)puc_param);
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_dev = mac_res_get_dev();
    /* **************************************************************************
    抛事件到DMAC层, 创建dmac cfg vap
    ************************************************************************** */
    ret = hmac_cfg_vap_send_event(mac_dev);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_CFG, "{hmac_cfg_vap_send_event::hmac_config_send_event fail[%d].", ret);
    }

    return ret;
}
#endif

/* ****************************************************************************
 功能描述  :
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_pm_cfg_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_PM_CFG_PARAM, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_pm_cfg_param::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_cus_rf(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_CUS_RF, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_cus_rf::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_cus_dts_cali(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_CUS_DTS_CALI, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_cus_dts_cali::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2015年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_cus_nvram_params(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_CUS_NVRAM_PARAM, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_cus_nvram_params::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

hi_u32 hmac_config_set_cus_fcc_tx_params(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_CUS_FCC_TX_PWR, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_cus_fcc_tx_params::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

#ifdef _PRE_XTAL_FREQUENCY_COMPESATION_ENABLE
hi_u32 hmac_config_set_freq_comp(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_FREQ_COMP, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_freq_comp::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
/* ****************************************************************************
 功能描述  : 设置业务识别功能开关
 返 回 值  : 错误码
 修改历史      :
  1.日    期   : 2015年11月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_tx_classify_switch(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 flag;
    hmac_vap_stru *hmac_vap = HI_NULL;

    /* 获取hmac_vap */
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(0, OAM_SF_ANY, "{hmac_config_set_tx_classify_switch::hmac_vap_get_vap_stru fail.vap_id[%u]}",
            mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取配置参数 */
    flag = *puc_param;

    /* 参数没有更改，不需要重新配置 */
    if (flag == hmac_vap->tx_traffic_classify_flag) {
        oam_warning_log1(0, OAM_SF_ANY, "hmac_config_set_tx_classify_switch::change nothing to flag:%d",
            hmac_vap->tx_traffic_classify_flag);
        return HI_SUCCESS;
    }

    /* 设置参数开关 */
    hmac_vap->tx_traffic_classify_flag = flag;

    if (hmac_vap->tx_traffic_classify_flag == HI_SWITCH_OFF) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_config_set_tx_classify_switch::flag = HI_SWITCH_OFF(0)");
        return HI_SUCCESS;
    } else if (hmac_vap->tx_traffic_classify_flag == HI_SWITCH_ON) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_config_set_tx_classify_switch::flag = HI_SWITCH_ON(1)");
        return HI_SUCCESS;
    }

    return HI_FAIL;
}
#endif /* _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN */

/* ****************************************************************************
 功能描述  : 打开sta device 低功耗
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_sta_pm_on(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    mac_cfg_ps_open_stru *sta_pm_open = HI_NULL;

    if (oal_unlikely((mac_vap == HI_NULL) || (puc_param == HI_NULL))) {
        oam_warning_log0(0, OAM_SF_PWR, "{wal_config_set_sta_pm_on::pst_mac_vap / puc_param null}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_PWR, "{wal_config_set_sta_pm_on::pst_hmac_vap null,vap state[%d].}",
            mac_vap->vap_state);
        return HI_ERR_CODE_PTR_NULL;
    }
    sta_pm_open = (mac_cfg_ps_open_stru *)puc_param;

    /* 如果上层主动dhcp成功此时取消超时开低功耗的定时器 */
    if ((hmac_vap->ps_sw_timer.is_registerd == HI_TRUE) && (sta_pm_open->pm_enable > MAC_STA_PM_SWITCH_OFF)) {
        frw_timer_immediate_destroy_timer(&(hmac_vap->ps_sw_timer));
    }

    return _hmac_config_set_sta_pm_on(mac_vap, us_len, puc_param);
}

#ifdef _PRE_WLAN_FEATURE_STA_PM
#ifdef _PRE_WLAN_FEATURE_HIPRIV
hi_u32 hmac_config_set_pm_param(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 规避逻辑问题: 配置bcn timeout时需要vap up,否则会导致bcn timeout中断无法清除 */
    if (mac_vap->vap_state != MAC_VAP_STATE_UP) {
        oam_warning_log0(0, OAM_SF_PWR, "hmac_config_set_pm_param: need up vap first.");
        return HI_FAIL;
    }
    return hmac_config_sync_cmd_common(mac_vap, WLAN_CFGID_SET_PSM_PARAM, us_len, puc_param);
}
#endif
#endif

hi_u32 hmac_config_open_wmm(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 wmm = *(hi_u8 *)puc_param;
#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    mac_device_stru *mac_dev = mac_res_get_dev();
    mac_dev->wmm = wmm;
    mac_vap->mib_info->wlan_mib_sta_config.dot11_qos_option_implemented = wmm;
    mac_vap->voice_aggr = !wmm;
#endif

    if ((wmm == HI_FALSE) && (mac_vap->cap_flag.uapsd == HI_TRUE)) {
        oam_warning_log0(0, OAM_SF_UM, "{hmac_config_open_wmm::config uapsd mode, not support turn off wmm}");
        return HI_FAIL;
    }

    return hmac_config_sync_cmd_common(mac_vap, WLAN_CFGID_WMM_SWITCH, us_len, puc_param);
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 设置VAP mib值
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_mib(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    return _hmac_config_set_mib(mac_vap, us_len, puc_param);
}

/* ****************************************************************************
 函 数 名  : wal_config_get_mib
 功能描述  : 获取VAP mib值
 输出参数  : 无
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_get_mib(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_unref_param(us_len);
    return _hmac_config_get_mib(mac_vap, puc_param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : Mesh 设置用户状态
 修改历史      :
  1.日    期   : 2019年1月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_mesh_user(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 user_index = 0;
    mac_cfg_set_mesh_user_param_stru *set_mesh = (mac_cfg_set_mesh_user_param_stru *)puc_param;
    hi_unref_param(us_len);

    /* 判断命令为设置用户参数/新增用户 */
    if (set_mesh->set == HI_SWITCH_OFF) {
        oam_warning_log0(0, OAM_SF_UM, "{hmac_config_set_mesh_user::not support add user.}");
        return HI_FAIL;
    } else {
        if (mac_vap_find_user_by_macaddr(mac_vap, set_mesh->auc_addr, WLAN_MAC_ADDR_LEN, &user_index) != HI_SUCCESS) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_config_set_mesh_user::find_user failed}");
            return HI_FAIL;
        }
    }

    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_index);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_config_set_mesh_user::pst_hmac_user null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (hmac_user->base_user->is_mesh_user == HI_FALSE) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_config_set_mesh_user::isNot meshUser,unsupport set state}");
        return HI_FAIL;
    }

    /* 获取上层下发的设置状态值 */
    switch (set_mesh->plink_sta) {
        case HISI_PLINK_ESTAB:
            /* 设置用户携带在beacon和probe rsp中的优先级 */
            hmac_user->has_rx_mesh_confirm = HI_FALSE;
            hmac_user->base_user->bcn_prio = set_mesh->bcn_prio;
            hmac_user->base_user->is_mesh_mbr = set_mesh->is_mbr;
            /* 标志该用户在关联阶段中是否为发起角色 */
            hmac_user->base_user->mesh_initiative_role = set_mesh->mesh_initiative_peering;
            /* Mesh下ESTAB状态即为关联成功 */
            mac_user_set_asoc_state(hmac_user->base_user, MAC_USER_STATE_ASSOC);
            /* 通知算法 */
            hmac_user_add_notify_alg(mac_vap, user_index);

            /* 通知Dmac添加白名单 */
            hi_u32 ret = hmac_set_multicast_user_whitelist(mac_vap, set_mesh->auc_addr, WLAN_MAC_ADDR_LEN);
            if (ret != HI_SUCCESS) {
                oam_warning_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_config_set_mesh_user::set allowlist fail=%d}", ret);
                return ret;
            }
            oam_warning_log1(hmac_user->base_user->vap_id, OAM_SF_ASSOC,
                "{hmac_config_set_mesh_user::mesh assoc mesh HI_SUCCESS! user_indx=%d.}", user_index);
            break;
        default:
            break;
    }
    return HI_SUCCESS;
}

hi_u32 hmac_config_encap_mesh_frame(mac_vap_stru *mac_vap, mac_action_data_stru *action_data, oal_netbuf_stru *data,
    hi_u8 us_action_code, hi_u32 *len)
{
    if (memset_s(oal_netbuf_cb(data), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size()) != EOK) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_send_mesh_action::[MESH]memset_s fail.}");
        oal_netbuf_free(data);
        goto error_handle;
    }

    if (memset_s((hi_u8 *)oal_netbuf_header(data), MAC_80211_FRAME_LEN, 0, MAC_80211_FRAME_LEN) != EOK) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_send_mesh_action::[MESH]memset_s fail.}");
        oal_netbuf_free(data);
        goto error_handle;
    }

    if (us_action_code == MAC_SP_ACTION_MESH_PEERING_OPEN) {
        *len = hmac_encap_mesh_peering_open_frame(mac_vap, (oal_netbuf_header(data)), action_data);
    } else if (us_action_code == MAC_SP_ACTION_MESH_PEERING_CONFIRM) {
        *len = hmac_encap_mesh_peering_confirm_frame(mac_vap, (oal_netbuf_header(data)), action_data);
    } else if (us_action_code == MAC_SP_ACTION_MESH_PEERING_CLOSE) {
        *len = hmac_encap_mesh_peering_close_frame(mac_vap, (oal_netbuf_header(data)), action_data);
    } else if (us_action_code == MAC_SP_ACTION_MESH_GROUP_KEY_INFORM) {
        *len = hmac_encap_mesh_group_key_inform_frame(mac_vap, (oal_netbuf_header(data)), action_data);
    } else if (us_action_code == MAC_SP_ACTION_MESH_GROUP_KEY_ACK) {
        *len = hmac_encap_mesh_group_key_ack_frame(mac_vap, (oal_netbuf_header(data)), action_data);
    } else {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
            "{hmac_config_send_mesh_action::[MESH]unsupported self-protected action code:%d}", us_action_code);
        goto error_handle;
    }

    if ((*len) == 0) {
        /* 组帧失败 */
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_send_mesh_action::[MESH]encap_mesh_frame Err}");
        oal_netbuf_free(data);
        goto error_handle;
    }
    oal_netbuf_put(data, *len);

    /* 为填写发送描述符准备参数 */
    hmac_tx_ctl_stru *tx_ctl    = (hmac_tx_ctl_stru *)oal_netbuf_cb(data);
    tx_ctl->us_mpdu_len         = *len; /* dmac发送需要的mpdu长度 */
    tx_ctl->frame_header_length = MAC_80211_FRAME_LEN;
    tx_ctl->frame_header        = (mac_ieee80211_frame_stru *)oal_netbuf_header(data);
    tx_ctl->mac_head_type       = 1;

    mac_vap_set_cb_tx_user_idx(mac_vap, tx_ctl, action_data->puc_dst);

    oam_warning_log3(mac_vap->vap_id, OAM_SF_ANY,
        "{hmac_config_send_mesh_action::[MESH]Send Mesh Self-Protected Action[%d] to user [X:X:X:X:%x:%x].}",
        us_action_code, action_data->puc_dst[4], action_data->puc_dst[5]); /* 4 5 元素索引 */

    return HI_SUCCESS;

error_handle:
    /* 释放上层申请的puc_data空间 */
    if (action_data->data_len > 0) {
        oal_free(action_data->puc_data);
        action_data->puc_data = HI_NULL;
    }

    return HI_FAIL;
}

hi_u32 hmac_config_mesh_send_event(mac_vap_stru *mac_vap, hmac_user_stru *hmac_user, mac_action_data_stru *action_data,
    oal_netbuf_stru *data, hi_u32 len)
{
    hi_u8 us_action_code;
    hi_u32 ret;

    us_action_code = mac_get_action_code(action_data->puc_data);
    ret = hmac_tx_mgmt_send_event(mac_vap, data, len);
    if (ret != HI_SUCCESS) {
        oal_netbuf_free(data);
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_send_mesh_action::[MESH]send_event Err%d}", ret);

        /* 释放上层申请的puc_data空间 */
        if (action_data->data_len > 0) {
            oal_free(action_data->puc_data);
            action_data->puc_data = HI_NULL;
        }

        return HI_FAIL;
    }

    /* 更改状态 */
    if ((us_action_code == MAC_SP_ACTION_MESH_PEERING_OPEN) && (hmac_user != HI_NULL)) {
        hmac_user_set_asoc_state(mac_vap, hmac_user->base_user, MAC_USER_STATE_AUTH_COMPLETE);
    }

    /* 释放上层申请的puc_data空间 */
    if (action_data->data_len > 0) {
        oal_free(action_data->puc_data);
        action_data->puc_data = HI_NULL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : Mesh 发送Action 帧
 修改历史      :
  1.日    期   : 2019年2月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_send_mesh_action(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8           user_index = 0;
    hi_u32          len = 0;
    hmac_user_stru *hmac_user = HI_NULL;

    hi_unref_param(us_len);

    mac_action_data_stru *action_data = (mac_action_data_stru *)puc_param;
    hmac_vap_stru        *hmac_vap    = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_warning_log0(0, OAM_SF_UM, "{hmac_config_send_mesh_action::pst_hmac_vap null.}");
        goto error_handle;
    }

    /* 获取Action category 和code */
    hi_u8 us_category    = mac_get_action_category(action_data->puc_data);
    hi_u8 us_action_code = mac_get_action_code(action_data->puc_data);
    if (us_category != MAC_ACTION_CATEGORY_SELF_PROTECTED) {
        oam_warning_log0(0, OAM_SF_UM, "{hmac_config_send_mesh_action::action category is wrong.}");
        goto error_handle;
    }

    hi_u32 ret = mac_vap_find_user_by_macaddr(mac_vap, action_data->puc_dst, WLAN_MAC_ADDR_LEN, &user_index);
    /* Mesh Peering Close帧存在无用户发送的情况，收到一个从未关联得节点发来的帧 */
    if (us_action_code != MAC_SP_ACTION_MESH_PEERING_CLOSE) {
        /* 找到用户 */
        if (ret != HI_SUCCESS) {
            oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_send_mesh_action::[MESH]cannot find user}");
            goto error_handle;
        }

        hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_index);
        if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
            oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_send_mesh_action::[MESH]hmac/base user null}");
            goto error_handle;
        }

        if (hmac_user->base_user->is_mesh_user == HI_FALSE) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_config_send_mesh_action::[MESH]is_mesh_user Err}");
            goto error_handle;
        }
    }

    /* 申请空间 */
    oal_netbuf_stru *data = (oal_netbuf_stru *)oal_netbuf_alloc(WLAN_MGMT_NETBUF_SIZE, 0, 4); /* align 4 */
    if (data == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_send_mesh_action::[MESH]puc_data null.}");
        goto error_handle;
    }

    if (hmac_config_encap_mesh_frame(mac_vap, action_data, data, us_action_code, &len) != HI_SUCCESS) {
        return HI_FAIL;
    }

    return hmac_config_mesh_send_event(mac_vap, hmac_user, action_data, data, len);

error_handle:
    /* 释放上层申请的puc_data空间 */
    if (action_data->data_len > 0) {
        oal_free(action_data->puc_data);
    }

    return HI_FAIL;
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 设置发送次数上报限制参数(调试使用)
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_report_times_limit(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    if (mac_vap->vap_mode != WLAN_VAP_MODE_MESH) {
        return HI_FALSE;
    }

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_REPORT_TIMES_LIMIT, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_config_set_report_times_limit::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 设置发送次数上报限制参数(调试使用)
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_report_cnt_limit(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    if (mac_vap->vap_mode != WLAN_VAP_MODE_MESH) {
        return HI_FALSE;
    }

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_REPORT_CNT_LIMIT, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_config_set_report_cnt_limit::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

/* ****************************************************************************
 功能描述  : hmac获取meshid
 输入参数  : event_hdr: 事件头
                            us_len       : 参数长度
                            puc_param    : 参数
 返 回 值  : hi_u32 错误码
 修改历史      :
  1.日    期   : 2019年3月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_meshid(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    /* 读取mib值 */
    return mac_mib_get_meshid(mac_vap, (hi_u8 *)pus_len, puc_param);
}

/* ****************************************************************************
 功能描述  : hmac设meshid
 输入参数  : event_hdr: 事件头
                            us_len       : 参数长度
                            puc_param    : 参数
 返 回 值  : hi_u32 错误码
 修改历史      :
  1.日    期   : 2019年3月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_meshid(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    return hmac_config_send_event(mac_vap, WLAN_CFGID_MESHID, us_len, puc_param);
#else
    /* 设置mib值 */
    mac_mib_set_meshid(mac_vap, (hi_u8)us_len, puc_param);
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : 设置mesh Hisi-optimization字段
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_beacon_priority(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    hi_u8 priority;

    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_config_set_beacon_priority::pst_mac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    if (mac_vap->vap_mode != WLAN_VAP_MODE_MESH) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_beacon_priority::not Mesh vap!.}");
        return HI_FAIL;
    }
    priority = *puc_param;

    oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_beacon_priority:: lwip set prioriy: %d}", priority);

    mac_vap->priority = priority;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_BEACON_PRIORITY, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_config_set_beacon_priority::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 设置mesh vap 的mnid
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_mnid(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_MNID, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
                         "{hmac_config_set_mnid::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

#ifdef _PRE_WLAN_FEATURE_HIPRIV
/* ****************************************************************************
 功能描述  : 设置mesh vap 为mbr
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_en_mbr(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_MBR_EN, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
                         "{hmac_config_set_en_mbr::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

/* ****************************************************************************
 功能描述  : mesh上报new peer candidate事件到wpa
 输入参数  : [1]event_mem
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_mesh_report_new_peer_candidate(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    frw_event_hdr_stru *event_hdr = HI_NULL;
    dmac_tx_event_stru *dtx_event = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u32 us_payload_len;
    hi_u32 ret;
    oal_netbuf_stru *netbuf = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_RX, "{hmac_mesh_report_new_peer_candidate::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取事件头和事件结构体指针 */
    event = (frw_event_stru *)event_mem->puc_data;
    event_hdr = &(event->event_hdr);
    dtx_event = (dmac_tx_event_stru *)event->auc_event_data;
    netbuf = dtx_event->netbuf;

    mac_vap = mac_vap_get_vap_stru(event_hdr->vap_id);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_mesh_report_new_peer_candidate::pst_mac_vap null.}");

        /* 释放上报的beacon的内存 */
        oal_netbuf_free(netbuf);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 处理Mesh Beacon参数 */
    us_payload_len = dtx_event->us_frame_len;
    ret = hmac_config_new_peer_candidate_event(mac_vap, netbuf, us_payload_len);
    if (ret != HI_SUCCESS) {
        /* 释放上报的beacon的内存 */
        oal_netbuf_free(netbuf);
        oam_warning_log1(0, 0, "hmac_mesh_report_new_peer_candidate:report to wpa fail,ret = %d!", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : mesh通过wpa添加mesh用户
 输入参数  : mac_vap_stru *pst_mac_vap, hi_u8 uc_len, hi_u8 *puc_param
 修改历史      :
  1.日    期   : 2019年5月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_add_mesh_user(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *puc_param)
{
    mac_cfg_set_mesh_user_param_stru *add_mesh_user = HI_NULL;
    hi_u8                             user_index;
    hmac_user_stru                   *hmac_user = HI_NULL;
    hi_u32                            ret;
    mac_vap_stru                     *mac_vap_tmp = HI_NULL;
    hi_unref_param(len);

    add_mesh_user = (mac_cfg_set_mesh_user_param_stru *)puc_param;

    /* 判断命令为设置用户参数/新增用户 */
    if (add_mesh_user->set == HI_SWITCH_ON) {
        oam_warning_log0(0, OAM_SF_UM, "{hmac_config_add_mesh_user::not support set user state.}");
        return HI_FAIL;
    }
    /* mesh 下判断accepting peer的值，为false不接受远端接入 */
    if (mac_mib_get_accepting_peer(mac_vap) == HI_FALSE) {
        hi_diag_log_msg_w0(0, "{hmac_config_add_mesh_user::Mesh is not ready to accept peer connect.}");
        return HI_FAIL;
    }
    /* 若在同一device下的其他VAP下找到给用户，删除之。否则导致业务不通。在DBAC下尤其常见 */
    if (mac_device_find_user_by_macaddr(mac_vap, add_mesh_user->auc_addr, WLAN_MAC_ADDR_LEN,
        &user_index) == HI_SUCCESS) {
        hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_index);
        if ((hmac_user != HI_NULL) && (hmac_user->base_user != HI_NULL)) {
            mac_vap_tmp = mac_vap_get_vap_stru(hmac_user->base_user->vap_id);
            if (mac_vap_tmp != HI_NULL) {
                hmac_user_del(mac_vap_tmp, hmac_user);
            }
        }
    }
    ret = hmac_user_add(mac_vap, add_mesh_user->auc_addr, WLAN_MAC_ADDR_LEN, &user_index);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_config_add_mesh_user::hmac_user_add failed[%d].}", ret);
        return ret;
    }

    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_index);
    if (hmac_user == HI_NULL) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_config_add_mesh_user::pst_hmac_user null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_user->base_user->is_mesh_user = HI_TRUE;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : mesh设置new peer candidate的使能
 输入参数  : mac_vap_stru *pst_mac_vap, hi_u8 uc_len, hi_u8 *puc_param
 修改历史      :
  1.日    期   : 2019年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_new_peer_candidate_en(mac_vap_stru *mac_vap, hi_u16 len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    mac_device_stru *mac_dev = mac_res_get_dev();
    mac_vap_stru *mac_vap_first = HI_NULL;
    mac_vap_stru *mac_vap_second = HI_NULL;

    if (mac_vap->vap_mode != WLAN_VAP_MODE_MESH) {
        return HI_FALSE;
    }

    ret = mac_device_find_2up_vap(mac_dev, &mac_vap_first, &mac_vap_second);
    if (ret == HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_config_new_peer_candidate_en::current mode is mbr,unsupport en auto-peer.}");
        return HI_FAIL;
    }
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_NEW_PEER_CONFIG_EN, len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_config_new_peer_candidate_en::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 设置mesh uc_accept_sta
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_mesh_accept_sta(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 accept_sta;
    hi_unref_param(us_len);

    accept_sta = *puc_param;

    mac_vap->mesh_accept_sta = accept_sta;

    return HI_SUCCESS;
}

/* ***************************************************************************
 功能描述  : 设置mesh用户的gtk
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_mesh_user_gtk(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_MESH_USER_GTK, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_config_set_mesh_user_gtk::[MESH]hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : 设置mesh accept peer
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_accept_peer(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 accept_peer;
    hi_unref_param(us_len);

    accept_peer = *puc_param;

    mac_mib_set_accepting_peer(mac_vap, accept_peer);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 组new peer candidate事件上报wpa
 输入参数  : mac_vap_stru *mac_vap, oal_netbuf_stru *beacon_netbuf, hi_u16 us_frame_len
 返 回 值  : hi_u32
 修改历史      :
  1.日    期   : 2019年8月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_new_peer_candidate_event(const mac_vap_stru *mac_vap, oal_netbuf_stru *netbuf, hi_u32 payload_len)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hi_u8 *puc_ie_payload = HI_NULL;
    hi_u8 *puc_payload = HI_NULL;
    hmac_rx_ctl_stru *cb = HI_NULL;
    mac_mesh_conf_ie_stru *mesh_conf_ie = HI_NULL;
    mac_ieee80211_frame_stru *frame_header = HI_NULL;
    hmac_report_new_peer_candidate_stru *wal_new_peer = HI_NULL;
    hi_u32 ret;

    puc_payload = (hi_u8 *)oal_netbuf_data(netbuf);
    cb = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    frame_header = (mac_ieee80211_frame_stru *)puc_payload;

    /* Probe Rsp和Beacon帧中前面为Timestamp,beacon interval,capability字段，非tlv结构，不能直接用于mac_find_ie函数，
      此处加上偏移，以Element ID为0的SSID做为起始地址查找指定IE */
    puc_ie_payload = puc_payload + MAC_80211_FRAME_LEN;

    if (payload_len > MAC_SSID_OFFSET) {
        /* 新增Mesh Configuration Element解析获取Accepting Peer字段值 */
        mesh_conf_ie = (mac_mesh_conf_ie_stru *)mac_find_ie(MAC_EID_MESH_CONF,
            puc_ie_payload + MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN,
            payload_len - MAC_SSID_OFFSET);
    }
    if (mesh_conf_ie == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    event_mem = frw_event_alloc(sizeof(hmac_report_new_peer_candidate_stru));
    if (event_mem == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_mesh_report_new_peer_candidate::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;

    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CTX, HMAC_HOST_CTX_EVENT_SUB_TYPE_NEW_PEER_CANDIDATE,
        sizeof(hmac_report_new_peer_candidate_stru), FRW_EVENT_PIPELINE_STAGE_0, mac_vap->vap_id);

    wal_new_peer = (hmac_report_new_peer_candidate_stru *)(event->auc_event_data);
    wal_new_peer->bcn_prio = mac_get_hisi_beacon_prio(puc_ie_payload, (hi_s32)payload_len);
    wal_new_peer->accept_sta = mac_get_hisi_accept_sta(puc_ie_payload, (hi_s32)payload_len);
    wal_new_peer->is_mbr = mac_get_hisi_en_is_mbr(puc_ie_payload, (hi_s32)payload_len);
    wal_new_peer->link_num = mesh_conf_ie->mesh_formation_info.number_of_peerings;

    /* 抛事件，无法传递有符号数，下面的左值rssi需要定义为无符号数，后面使用的时候再强转 -g- */
    wal_new_peer->rssi = (hi_u8)(cb->rssi_dbm);

    if (memcpy_s(wal_new_peer->auc_mac_addr, OAL_MAC_ADDR_LEN, frame_header->auc_address2, OAL_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_mesh_report_new_peer_candidate:: auc_mac_addr memcpy_s fail.");
        frw_event_free(event_mem);
        return HI_FAIL;
    }

    /* 分发事件 */
    ret = frw_event_dispatch_event(event_mem);
    frw_event_free(event_mem);

    return ret;
}

/* ****************************************************************************
 功能描述  : hmac读取mesh节点信息
 修改历史      :
  1.日    期   : 2019年11月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_mesh_nodeinfo(mac_vap_stru *mac_vap, hi_u16 *pus_len, hi_u8 *puc_param)
{
    mac_cfg_mesh_nodeinfo_stru *param = HI_NULL;

    param = (mac_cfg_mesh_nodeinfo_stru *)puc_param;

    if (memset_s(param, sizeof(mac_cfg_mesh_nodeinfo_stru), 0, sizeof(mac_cfg_mesh_nodeinfo_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_config_get_mesh_nodeinfo:: memset_s fail.");
        return HI_FAIL;
    }
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) && (mac_vap->is_conn_to_mesh == HI_TRUE)) {
        param->node_type = MAC_HISI_MESH_STA;
    } else if ((mac_vap->vap_mode == WLAN_VAP_MODE_MESH) && (mac_vap->is_mbr == HI_FALSE)) {
        param->node_type = MAC_HISI_MESH_MG;
    } else if ((mac_vap->vap_mode == WLAN_VAP_MODE_MESH) && (mac_vap->is_mbr == HI_TRUE)) {
        param->node_type = MAC_HISI_MESH_MBR;
    } else {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_get_mesh_nodeinfo::unspec mesh node type.}");
        param->node_type = MAC_HISI_MESH_UNSPEC;
        *pus_len = sizeof(mac_cfg_mesh_nodeinfo_stru);
        return HI_SUCCESS;
    }
    param->privacy = mac_vap->mib_info->wlan_mib_privacy.dot11_privacy_invoked;
    param->mesh_accept_sta = mac_vap->mesh_accept_sta;
    param->priority = mac_vap->priority;
    param->user_num = mac_vap->user_nums;
    param->chan = mac_vap->channel.chan_number;

    *pus_len = sizeof(mac_cfg_mesh_nodeinfo_stru);

    oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_get_mesh_nodeinfo::node type=%d, chan=%d.}",
        param->node_type, param->chan);

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 设置vap 重传限制数
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_config_set_retry_limit(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_RETRY_LIMIT, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_config_set_retry_limit::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

#ifdef FEATURE_DAQ
/* ****************************************************************************
 功能描述  : 获取数据采集结果
 修改历史      :
  1.日    期   : 2019年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_data_acq_result(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DATA_ACQ_REPORT, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_data_acq_result::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 返回数据采集结果
 修改历史      :
  1.日    期   : 2019年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_get_data_acq_result(mac_vap_stru *mac_vap, hi_u8 us_len, const hi_u8 *puc_param)
{
    wlan_acq_result_addr_stru *data_result_addr = HI_NULL;

    hi_unref_param(us_len);

    data_result_addr = (wlan_acq_result_addr_stru *)puc_param;

    oam_unrom_w_log4(mac_vap->vap_id, 0, "{hmac_get_data_acq_result::0x%x,0x%x,0x%x,0x%x}",
        data_result_addr->start_addr, data_result_addr->middle_addr1, data_result_addr->middle_addr2,
        data_result_addr->end_addr);

    return hmac_send_event_to_host(mac_vap, (const hi_u8 *)data_result_addr, sizeof(wlan_acq_result_addr_stru),
        HMAC_HOST_CTX_EVENT_SUB_TYPE_ACQ_RESULT);
}

/* ****************************************************************************
 功能描述  : 查询数据采集状态
 修改历史      :
  1.日    期   : 2019年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_data_acq_status(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    hi_unref_param(us_len);
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DATA_ACQ_STATUS, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_data_acq_status::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 返回数据采集状态
 修改历史      :
  1.日    期   : 2019年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_get_data_acq_status(mac_vap_stru *mac_vap, hi_u8 us_len, const hi_u8 *puc_param)
{
    hi_u8 value;

    hi_unref_param(us_len);

    value = *((hi_u8 *)puc_param);
    oam_warning_log1(mac_vap->vap_id, 0, "{hmac_get_data_acq_status::en_value[%d]}", value);

    return hmac_send_event_to_host(mac_vap, (const hi_u8 *)(&value), sizeof(hi_u8),
        HMAC_HOST_CTX_EVENT_SUB_TYPE_ACQ_STATUS);
}

/* ****************************************************************************
 功能描述  : 启动数据采集信息
 修改历史      :
  1.日    期   : 2019年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_data_acq_start(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DATA_ACQ_START, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_data_acq_start::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

#ifdef _PRE_WLAN_FEATURE_BW_HIEX
/* ****************************************************************************
 功能描述  : 设置窄带切换到宽带发送selfcts的参数
 修改历史      :
  1.日    期   : 2019年7月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_hiex_set_selfcts(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    wlan_selfcts_param_stru *param = HI_NULL;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hi_u32 ret;
#endif

    param = (wlan_selfcts_param_stru *)puc_param;
    mac_vap->selfcts = param->selfcts;
    mac_vap->duration = param->duration;
    mac_vap->us_per = param->us_per;
    oam_warning_log3(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_hiex_set_selfcts::enable[%d] duration[%d] per[%d].}",
        mac_vap->selfcts, mac_vap->duration, mac_vap->us_per);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_SELFCTS, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_COEX,
            "{hmac_config_hiex_set_selfcts::send event return err code [%d].}", ret);
        return ret;
    }
#else
    hi_unref_param(us_len);
#endif

    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
/* ****************************************************************************
 功能描述  : 对各band做平均功率补偿，传递补偿事件给dmac
 输入参数  : [1]mac_vap
             [2]len 事件传递的数据长度
             [3]puc_param 事件传递的数据指针
 输出参数  : 无
 返 回 值  : 事件传递是否成功的结果
**************************************************************************** */
hi_u32 hmac_config_set_cal_band_power(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 抛事件到DMAC层, 同步DMAC数据 */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CAL_BAND_POWER, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_cal_band_power::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 对不同协议场景、不用速率分别做功率补偿，传递补偿事件给dmac
 输入参数  : [1]mac_vap
             [2]len 事件传递的数据长度
             [3]puc_param 事件传递的数据指针
 输出参数  : 无
 返 回 值  : 事件传递是否成功的结果
**************************************************************************** */
hi_u32 hmac_config_set_cal_rate_power(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 抛事件到DMAC层, 同步DMAC数据 */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CAL_RATE_POWER, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_cal_rate_power::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 进行常温频偏功率补偿，传递补偿事件给dmac
 输入参数  : [1]mac_vap
             [2]len 事件传递的数据长度
             [3]puc_param 事件传递的数据指针
 输出参数  : 无
 返 回 值  : 事件传递是否成功的结果
**************************************************************************** */
hi_u32 hmac_config_set_cal_freq(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* 抛事件到DMAC层, 同步DMAC数据 */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_CAL_FREQ, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_cal_freq::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :MAC 设入 EFUSE
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2015年10月22日
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_efuse_mac(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_GET_EFUSE_MAC, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_get_efuse_mac::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  :MAC 设入 EFUSE
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2015年10月22日
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_efuse_mac(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_EFUSE_MAC, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_efuse_mac::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :将校准值写入EFUSE
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2015年10月22日
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_dataefuse(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_DATAEFUSE, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_dataefuse::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :获取校准数据
 修改历史      :
  1.日    期   : 2020年03月09日
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_get_cal_data(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_GET_CAL_DATA, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_get_cal_data::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  :设置发送功率偏移
 修改历史      :
  1.日    期   : 2020年3月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_tx_pwr_offset(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_TX_PWR_OFFSET, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_tx_pwr_offset::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

/* ****************************************************************************
 功能描述  :设置cca阈值
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2020年1月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_set_cca_th(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_CCA_TH, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
                         "{hmac_config_set_cca_th::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

hi_u32 hmac_config_get_efuse_mac_addr(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_GET_EFUSE_MAC_ADDR, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_get_efuse_mac_addr::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

#if defined(_PRE_WLAN_FEATURE_HIPRIV) && defined(_PRE_WLAN_FEATURE_INTRF_MODE)
hi_u32 hmac_config_set_intrf_mode(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_INTRF_MODE_ON, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_set_intrf_mode::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}
#endif

hi_u32 hmac_config_notify_get_tx_params(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret;

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_NOTIFY_GET_TX_PARAMS, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_notify_get_goodput::hmac_config_send_event failed[%d].}", ret);
    }

    return ret;
}

/* ****************************************************************************
 功能描述  :降低开机电流(注意:会牺牲一部分射频性能)
 修改历史      :
  1.日    期   : 2020年07月11日
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_minimize_boot_current(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    hi_u32 ret = hmac_config_send_event(mac_vap, WLAN_CFGID_MINIMIZE_BOOT_CURRET, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_config_minimize_boot_current::hmac_config_send_event failed[%d].}", ret);
    }
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
