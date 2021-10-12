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
#include "hmac_rx_filter.h"
#include "wlan_types.h"
#include "mac_device.h"
#include "dmac_ext_if.h"
#include "hcc_hmac_if.h"
#include "frw_event.h"
#include "hmac_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 查找是否有已经UP的STA
 修改历史      :
  1.日    期   : 2015年8月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_find_is_sta_up(const mac_device_stru *mac_dev)
{
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u8 vap_idx;

    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (mac_vap == HI_NULL) {
            continue;
        }
        if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) && (mac_vap->vap_state == MAC_VAP_STATE_UP)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 查找是否有已经UP的AP
 修改历史      :
  1.日    期   : 2015年8月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_find_is_ap_up(const mac_device_stru *mac_dev)
{
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u8 vap_idx;

    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (mac_vap == HI_NULL) {
            continue;
        }
        if ((mac_vap->vap_state != MAC_VAP_STATE_INIT) && (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)) {
            return HI_TRUE;
        }
#ifdef _PRE_WLAN_FEATURE_MESH
        if ((mac_vap->vap_state != MAC_VAP_STATE_INIT) && (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)) {
            return HI_TRUE;
        }
#endif
    }
    return HI_FALSE;
}


/* ****************************************************************************
 功能描述  : 查找是否有已有AP
**************************************************************************** */
hi_u8 hmac_find_is_ap(const mac_device_stru *mac_device)
{
    mac_vap_stru *vap = HI_NULL;
    hi_u8 vap_idx;

    for (vap_idx = 0; vap_idx < mac_device->vap_num; vap_idx++) {
        vap = mac_vap_get_vap_stru(mac_device->auc_vap_id[vap_idx]);
        if (vap == HI_NULL) {
            continue;
        }
        if (vap->vap_mode == WLAN_VAP_MODE_BSS_AP) {
            return HI_TRUE;
        }
#ifdef _PRE_WLAN_FEATURE_MESH
        if (vap->vap_mode == WLAN_VAP_MODE_MESH) {
            return HI_TRUE;
        }
#endif
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 计算不处于inti状态的VAP个数
 修改历史      :
  1.日    期   : 2014年7月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_calc_up_vap_num(const mac_device_stru *mac_dev)
{
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u8 vap_idx;
    hi_u8 up_ap_num = 0;

    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (mac_vap == HI_NULL) {
            continue;
        }

        if ((mac_vap->vap_state != MAC_VAP_STATE_INIT) && ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
            || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
            )) {
            up_ap_num++;
        } else if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) && (mac_vap->vap_state == MAC_VAP_STATE_UP)) {
            up_ap_num++;
        }
    }

    return up_ap_num;
}

/* ****************************************************************************
 功能描述  : 单vap模式下根据模式和状态获取对应的接收过滤值
 修改历史      :
  1.日    期   : 2019年6月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
 **************************************************************************** */
/* ********************************************* SINGLE VAP ******************************************************
    WLAN_VAP_MODE_CONFIG  配置模式                        WLAN_VAP_MODE_BSS_STA              BSS STA模式
  +-----------------------------+-----------------+      +----------------------------------+-----------------+
  | FSM State                   | RX FILTER VALUE |      | FSM State                        | RX FILTER VALUE |
  +-----------------------------+-----------------+      +----------------------------------+-----------------+
  | All states                  | 0x37BDEEFA      |      | MAC_VAP_STATE_INIT               | 0x37BDEEFA      |
  +-----------------------------+----- -----------+      | MAC_VAP_STATE_UP                 | 0x37BDEADA      |
                                                         | MAC_VAP_STATE_STA_FAKE_UP        | 0x37BDEEFA      |
   WLAN_VAP_MODE_BSS_AP             BSS AP模式           | MAC_VAP_STATE_STA_WAIT_SCAN      | 0x37BDCEEA      |
  +-----------------------------+-----------------+      | MAC_VAP_STATE_STA_SCAN_COMP      | 0x37BDEEDA      |
  | FSM State                   | RX FILTER VALUE |      | MAC_VAP_STATE_STA_WAIT_JOIN      | 0x37BDEEDA      |
  +-----------------------------+-----------------+      | MAC_VAP_STATE_STA_JOIN_COMP      | 0x37BDEEDA      |
  | MAC_VAP_STATE_INIT          | 0xF7B9EEFA      |      | MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2 | 0x37BDEEDA      |
  | MAC_VAP_STATE_UP            | 0x73B9EAEA      |      | MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4 | 0x37BDEEDA      |
  | MAC_VAP_STATE_PAUSE         | 0x73B9EAEA      |      | MAC_VAP_STATE_STA_AUTH_COMP      | 0x37BDEEDA      |
  | MAC_VAP_STATE_AP_WAIT_START | 0x73B9EAEA      |      | MAC_VAP_STATE_STA_WAIT_ASOC      | 0x37BDEEDA      |
  +-----------------------------+-----------------+      | MAC_VAP_STATE_STA_OBSS_SCAN      | 0x37BDCEEA      |
                                                         | MAC_VAP_STATE_STA_BG_SCAN        | 0x37BDCEEA      |
   WLAN_VAP_MODE_MONITOER           侦听模式             | MAC_VAP_STATE_STA_LISTEN         | 0x33A9EECA      |
  +-----------------------------+-----------------+      +----------------------------------+-----------------+
  | FSM State                   | RX FILTER VALUE |
  +-----------------------------+-----------------+
  | all status                  | 0x1             |
  +-----------------------------+-----------------+
*********************************************** MULTI  VAP **************************************************** */
hi_u32 hmac_get_single_vap_rx_filter(const mac_vap_stru *mac_vap)
{
    hi_u32 def_value = (BIT0 << 21); /* 默认开启FCS ERROR过滤 21: 左移21位 */

    if (mac_vap->vap_mode == WLAN_VAP_MODE_CONFIG) {
        return 0x37B9FEFA; /* 配置vap均使用0x37B9FEFA */
    } else if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        switch (mac_vap->vap_state) { /* STA各状态下的过滤寄存器设置 */
            case MAC_VAP_STATE_INIT:
            case MAC_VAP_STATE_STA_FAKE_UP:
                return 0xF7B9FEFA;
            case MAC_VAP_STATE_STA_WAIT_SCAN:
                return 0x37B9DECA;
            case MAC_VAP_STATE_STA_OBSS_SCAN:
            case MAC_VAP_STATE_STA_BG_SCAN:
                return 0x37B9DEEA;
            case MAC_VAP_STATE_STA_LISTEN:
                return 0x33A9FECA;
            default:
                return 0x73B9FADA;
        }
    } else if (is_ap(mac_vap)) {
        switch (mac_vap->vap_state) { /* AP以及MESH AP各状态下的过滤寄存器设置 */
            case MAC_VAP_STATE_INIT:
                return 0xF7B9FEFA;
            case MAC_VAP_STATE_UP:
                return 0xF7B9FAEA;
            case MAC_VAP_STATE_PAUSE:
            case MAC_VAP_STATE_AP_WAIT_START:
                return 0x73B9FAEA;
            default: /* AP下异常状态返回默认值 */
                return def_value;
        }
    } else if (mac_vap->vap_mode == WLAN_VAP_MODE_MONITOER) {
        return def_value | BIT0; /* 混杂模式需要置BIT0 和FCS ERROR */
    } else {
        return def_value; /* 其他模式下均返回默认值 */
    }
}

/* ****************************************************************************
 功能描述  : 存在已经UP的STA，当前的vap模式为AP/MESH时，根据其状态获取对应的接收过滤值
       WLAN_VAP_MODE_BSS_AP/MESH               BSS AP模式              BSS MESH模式
     +----------------------------------+--------------------------+--------------------------+
     | FSM State                        | RX FILTER VALUE          | RX FILTER VALUE          |
     +----------------------------------+--------------------------+--------------------------+
     | MAC_VAP_STATE_INIT               | 保持原有值不配置         | 保持原有值不配置         |
     | MAC_VAP_STATE_UP                 | 0x73B9EACA               | 0x73B9EADA               |
     | MAC_VAP_STATE_PAUSE              | 0x73B9EACA               | 0x73B9EADA               |
     | MAC_VAP_STATE_AP_WAIT_START      | 0x73B9EACA               | 0x73B9EADA               |
     +----------------------------------+--------------------------+--------------------------+
 修改历史      :
  1.日    期   : 2019年6月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_get_staup_ap_rx_filter(const mac_vap_stru *mac_vap)
{
    switch (mac_vap->vap_state) {
        case MAC_VAP_STATE_INIT:
            return 0;
        default:
            return 0x73B9FACA;
    }
}

/* ****************************************************************************
 功能描述  : 存在已经UP的STA，当前的vap模式为STA时，根据其状态获取对应的接收过滤值
    /  多STA模式    WLAN_VAP_MODE_BSS_STA          BSS STA模式        /
    / +----------------------------------+--------------------------+ /
    / | FSM State                        | RX FILTER VALUE          | /
    / +----------------------------------+--------------------------+ /
    / | MAC_VAP_STATE_STA_WAIT_SCAN      | 0x33B9CACA               | /
    / | MAC_VAP_STATE_STA_OBSS_SCAN      | 0x33B9CACA               | /
    / | MAC_VAP_STATE_STA_BG_SCAN        | 0x33B9CACA               | /
    / | MAC_VAP_STATE_STA_LISTEN         | 0x33A9EACA               | /
    / | ALL OTHER STATE                  | 0x73B9EADA               | /
    / +----------------------------------+--------------------------+ /
 修改历史:
  1.日    期   : 2019年6月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_get_staup_sta_rx_filter(const mac_vap_stru *mac_vap)
{
    switch (mac_vap->vap_state) {
        case MAC_VAP_STATE_STA_WAIT_SCAN:
        case MAC_VAP_STATE_STA_OBSS_SCAN:
        case MAC_VAP_STATE_STA_BG_SCAN:
            return 0x33B9DACA;
        case MAC_VAP_STATE_STA_LISTEN:
            return 0x33A9FACA;
        default:
            return 0x73B9FADA;
    }
}

/* ****************************************************************************
 功能描述  : 存在已经UP的AP，当前的vap模式为STA时，根据其状态获取对应的接收过滤值
    / 多AP UP时,STA配置场景:  WLAN_VAP_MODE_BSS_STA   BSS STA模式     /
    / +----------------------------------+--------------------------+ /
    / | FSM State                        | RX FILTER VALUE          | /
    / +----------------------------------+--------------------------+ /
    / | MAC_VAP_STATE_INIT               | 保持原有值不配置         | /
    / | MAC_VAP_STATE_UP                 | 0x73B9FACA               | /
    / | MAC_VAP_STATE_STA_FAKE_UP        | 保持原有值不配置         | /
    / | MAC_VAP_STATE_STA_WAIT_SCAN      | 0x33B9CACA               | /
    / | MAC_VAP_STATE_STA_SCAN_COMP      | 0x73B9FACA               | /
    / | MAC_VAP_STATE_STA_WAIT_JOIN      | 0x73B9FACA               | /
    / | MAC_VAP_STATE_STA_JOIN_COMP      | 0x73B9FACA               | /
    / | MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2 | 0x73B9FACA               | /
    / | MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4 | 0x73B9FACA               | /
    / | MAC_VAP_STATE_STA_AUTH_COMP      | 0x73B9FACA               | /
    / | MAC_VAP_STATE_STA_WAIT_ASOC      | 0x73B9FACA               | /
    / | MAC_VAP_STATE_STA_OBSS_SCAN      | 0x33B9CACA               | /
    / | MAC_VAP_STATE_STA_BG_SCAN        | 0x33B9CACA               | /
    / | MAC_VAP_STATE_STA_LISTEN         | 0x33A9EACA               | /
    / +----------------------------------+--------------------------+ /
 修改历史:
  1.日    期   : 2019年6月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_get_apup_sta_rx_filter(const mac_vap_stru *mac_vap)
{
    switch (mac_vap->vap_state) {
        case MAC_VAP_STATE_STA_WAIT_SCAN:
        case MAC_VAP_STATE_STA_OBSS_SCAN:
        case MAC_VAP_STATE_STA_BG_SCAN:
            return 0x33B9DACA;
        case MAC_VAP_STATE_STA_LISTEN:
            return 0x33A9FACA;
        case MAC_VAP_STATE_INIT:
        case MAC_VAP_STATE_STA_FAKE_UP:
            return 0;
        default:
            return 0x73B9FACA;
    }
}

#ifndef _PRE_WLAN_PHY_PERFORMANCE
hi_u32 hmac_send_rx_filter_event(const mac_vap_stru *mac_vap, hi_u32 rx_filter_val)
{
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_set_rx_filter_value::hmac_vap is null}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if ((hmac_vap->hmac_al_rx_flag == HI_TRUE) && (hmac_vap->mac_filter_flag == HI_FALSE)) {
        /* 常收关闭过滤 */
        rx_filter_val |= (BIT0);
    }

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_FEATURE_PROMIS
    if (mac_res_get_dev()->promis_switch) {
        hi_u32 filter = hwal_get_promis_filter();
        if ((filter & 0x1) == HI_TRUE) { /* bit 0 :上报组播(广播)数据帧使能标志 */
            rx_filter_val = rx_filter_val & (~BIT3) & (~BIT12);
        }
        if (((filter >> 1) & 0x1) == HI_TRUE) { /* bit 1 :上报单播数据包使能标志 */
            rx_filter_val = rx_filter_val & (~BIT11);
        }
        if (((filter >> 2) & 0x1) == HI_TRUE) { /* bit 2 :上报组播(广播)管理帧使能标志 */
            rx_filter_val = rx_filter_val & (~BIT4);
        }
        if (((filter >> 3) & 0x1) == HI_TRUE) { /* bit 3 :上报单播管理帧使能标志 */
            rx_filter_val = rx_filter_val & (~BIT13);
        }
    }
#endif
#endif

    /* 抛事件到DMAC, 申请事件内存 */
    frw_event_mem_stru *event_mem = frw_event_alloc(sizeof(hi_u32));
    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_set_rx_filter_value::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_SET_RX_FILTER,
        sizeof(hi_u32), FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);
    /* 拷贝参数 */
    hi_u32 *event_data = (hi_u32 *)((hi_void *)event->auc_event_data);
    *event_data = rx_filter_val;

    if (hcc_hmac_tx_control_event(event_mem, sizeof(hi_u32)) != HI_SUCCESS) {
        frw_event_free(event_mem);
        return HI_FAIL;
    }

    frw_event_free(event_mem);

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : AP侧接收到对应STA的关联请求消息
             待修订为dmac_set_rx_filter_value
 修改历史      :
  1.日    期   : 2014年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_set_rx_filter_value(const mac_vap_stru *mac_vap)
{
#ifndef _PRE_WLAN_PHY_PERFORMANCE
    hi_u32           rx_filter_val;
    mac_device_stru *mac_dev = mac_res_get_dev();

    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_set_rx_filter_value::pst_mac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_vap->vap_state == MAC_VAP_STATE_PAUSE) {
        return HI_SUCCESS;
    }

    if (hmac_find_is_ap_up(mac_dev) == HI_TRUE) { /* 多VAP模式，AP已经UP */
        if ((mac_vap->vap_state == MAC_VAP_STATE_INIT) || (mac_vap->vap_state == MAC_VAP_STATE_STA_FAKE_UP)) {
            return HI_SUCCESS;
        }

        /* 多VAP模式，STA配置(在STA已经up和没有STA up的情况下，配置AP模式都使用该配置) */
        /* 已有AP UP,再起AP时使用单vap配置值 */
        rx_filter_val = (!is_ap(mac_vap)) ? hmac_get_apup_sta_rx_filter(mac_vap) :
            ((hmac_find_is_sta_up(mac_dev) && mac_vap->vap_state < MAC_VAP_AP_STATE_BUTT) ?
            hmac_get_staup_ap_rx_filter(mac_vap) : hmac_get_single_vap_rx_filter(mac_vap));
    } else if (hmac_find_is_sta_up(mac_dev) == HI_TRUE) {   /* 多VAP模式，STA已经UP */
        /* 多VAP模式，STA配置(在STA已经up和没有STA up的情况下，配置AP模式都使用该配置) */
        if ((mac_vap->vap_state == MAC_VAP_STATE_INIT) || (mac_vap->vap_state == MAC_VAP_STATE_STA_FAKE_UP)) {
            return HI_SUCCESS;
        }

        /* STA已经UP的状态下，STA的配置 */
        if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
            rx_filter_val = hmac_get_staup_sta_rx_filter(mac_vap);
        } else if (is_ap(mac_vap)) {
            rx_filter_val = hmac_get_staup_ap_rx_filter(mac_vap);
        } else {
            return HI_SUCCESS;
        }
    } else {
        /* 没有任何设备处于UP状态，即单VAP存在 */
        rx_filter_val = hmac_get_single_vap_rx_filter(mac_vap);
    }

    /* 支持ANY的设备(STA & AP等)需要能够收到来自其他BSS的广播管理帧，尤其是probe req, 这里设置不过滤，清零BIT4 */
    rx_filter_val = (mac_vap->support_any == HI_TRUE) ? (rx_filter_val & (~BIT4)) : rx_filter_val;

    /* 过滤值为0时 表示维持原有值不变 不需要配置 */
    if (rx_filter_val == 0) {
        return HI_SUCCESS;
    }

    hi_u32 ret = hmac_send_rx_filter_event(mac_vap, rx_filter_val);
    if (ret != HI_SUCCESS) {
        return ret;
    }

#endif /* #ifndef _PRE_WLAN_PHY_PERFORMANCE */

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
