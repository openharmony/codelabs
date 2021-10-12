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
#include "hmac_fsm.h"
#include "hmac_scan.h"
#include "hmac_mgmt_sta.h"
#include "hmac_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 改变状态机状态
 输入参数  : pst_hmac_vap: HMAC VAP
             en_vap_state: 要切换到的状态
 修改历史      :
  1.日    期   : 2013年6月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_fsm_change_state(hmac_vap_stru *hmac_vap, mac_vap_state_enum_uint8 vap_state)
{
    mac_cfg_mode_param_stru cfg_mode;
    mac_vap_state_enum_uint8 old_state;
    hi_u32 ret;
    old_state = hmac_vap->base_vap->vap_state;

    /* 将vap状态改变信息上报 */
    mac_vap_state_change(hmac_vap->base_vap, vap_state);

    ret = hmac_config_vap_state_syn(hmac_vap->base_vap, sizeof(vap_state), (hi_u8 *)(&vap_state));
    if (ret != HI_SUCCESS) {
        oam_error_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_fsm_change_state::hmac_syn_vap_state failed[%d], old_state=%d, new_state=%d.}", ret, old_state,
            vap_state);
    }
#ifdef _PRE_WLAN_FEATURE_STA_PM
    /*
     * sta start未关联, HMAC_SWITCH_STA_PSM_PERIOD后投休眠票
     * sta start关联成功, HMAC_SWITCH_STA_PSM_PERIOD开启节能定时器
     */
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) &&
        ((vap_state == MAC_VAP_STATE_STA_FAKE_UP) || (vap_state == MAC_VAP_STATE_UP))) {
        if (hmac_vap->ps_sw_timer.is_registerd == HI_TRUE) {
            frw_timer_immediate_destroy_timer(&(hmac_vap->ps_sw_timer));
        }
        frw_timer_create_timer(&(hmac_vap->ps_sw_timer), hmac_set_psm_timeout, HMAC_SWITCH_STA_PSM_PERIOD,
            (hi_void *)hmac_vap, HI_FALSE);
    }
#endif

    /* 设置帧过滤寄存器 */
    hmac_set_rx_filter_value(hmac_vap->base_vap);

    if ((vap_state == MAC_VAP_STATE_STA_FAKE_UP) && (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_STA)) {
        cfg_mode.protocol = hmac_vap->preset_para.protocol;
        cfg_mode.band = hmac_vap->preset_para.band;
        cfg_mode.en_bandwidth = hmac_vap->preset_para.en_bandwidth;

        hmac_config_sta_update_rates(hmac_vap->base_vap, &cfg_mode);
    }
}

/* ****************************************************************************
 功能描述  : 根据STA状态机处理扫描请求
 输入参数  : pst_hmac_vap: hmac vap
             pst_scan_params: 扫描请求参数
 修改历史      :
  1.日    期   : 2019年6月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_fsm_handle_scan_req(const mac_vap_stru *mac_vap, mac_scan_req_stru *scan_params)
{
    hmac_vap_stru *hmac_vap = HI_NULL;

    /* 入参在调用处已判空，这里仅对使用的指针判空 */
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_fsm_handle_scan_req::mac vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_fsm_handle_scan_req::hmac_vap is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_vap->vap_state == MAC_VAP_STATE_PAUSE) {
        /* 切换vap的状态为UP状态 */
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_UP);
    }

    switch (mac_vap->vap_state) {
        case MAC_VAP_STATE_INIT:
        case MAC_VAP_STATE_STA_FAKE_UP:
        case MAC_VAP_STATE_STA_SCAN_COMP:
        case MAC_VAP_STATE_STA_JOIN_COMP:
        case MAC_VAP_STATE_STA_AUTH_COMP:
        case MAC_VAP_STATE_UP:
        case MAC_VAP_STATE_STA_LISTEN:
            return hmac_scan_proc_scan_req_event(hmac_vap, scan_params);

        default:
            return hmac_scan_proc_scan_req_event_exception(hmac_vap);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
