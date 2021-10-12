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
#include "hmac_rx_data.h"
#include "hmac_mgmt_bss_comm.h"
#include "hmac_mgmt_classifier.h"
#include "hmac_fsm.h"
#include "hmac_sme_sta.h"
#include "hmac_mgmt_sta.h"
#include "hmac_mgmt_ap.h"
#include "hcc_hmac_if.h"
#include "wal_cfg80211_apt.h"
#ifdef _PRE_WLAN_FEATURE_ANY
#include "hmac_any.h"
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
#include "hmac_p2p.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : ACTION帧发送
 修改历史      :
  1.日    期   : 2013年4月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_mgmt_tx_action(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, mac_action_mgmt_args_stru *action_args)
{
    hi_u32 ret;

    if ((hmac_vap == HI_NULL) || (hmac_user == HI_NULL) || (action_args == HI_NULL)) {
        oam_error_log3(0, OAM_SF_TX, "{hmac_mgmt_tx_action::param null, %p %p %p.}", (uintptr_t)hmac_vap,
            (uintptr_t)hmac_user, (uintptr_t)action_args);
        return HI_ERR_CODE_PTR_NULL;
    }
    if (action_args->category == MAC_ACTION_CATEGORY_BA) {
        switch (action_args->action) {
            case MAC_BA_ACTION_ADDBA_REQ:
                oam_info_log0(hmac_vap->base_vap->vap_id, OAM_SF_TX,
                              "{hmac_mgmt_tx_action::MAC_BA_ACTION_ADDBA_REQ.}");
                hmac_mgmt_tx_addba_req(hmac_vap, hmac_user, action_args);
                break;

            case MAC_BA_ACTION_DELBA:
                oam_info_log0(hmac_vap->base_vap->vap_id, OAM_SF_TX, "{hmac_mgmt_tx_action::MAC_BA_ACTION_DELBA.}");
                ret = hmac_mgmt_tx_delba(hmac_vap, hmac_user, action_args);
                if (ret != HI_SUCCESS) {
                    oam_warning_log0(hmac_user->base_user->vap_id, OAM_SF_BA,
                        "hmac_mgmt_tx_delba return NON SUCCESS. ");
                    return ret;
                }
                break;

            default:
                oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_TX, "{hmac_mgmt_tx_action::invalid ba type[%d].}",
                    action_args->action);
                return HI_FAIL;
        }
    } else {
        oam_info_log1(hmac_vap->base_vap->vap_id, OAM_SF_TX, "{hmac_mgmt_tx_action::invalid ba type[%d].}",
            action_args->category);
    }
    return HI_SUCCESS;
}


/* ****************************************************************************
 功能描述  :
 修改历史      :
  1.日    期   : 2013年4月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_mgmt_tx_priv_req(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, mac_priv_req_args_stru *priv_req)
{
    mac_priv_req_11n_enum_uint8 req_type;

    if ((hmac_vap == HI_NULL) || (hmac_user == HI_NULL) || (priv_req == HI_NULL)) {
        oam_error_log3(0, OAM_SF_TX, "{hmac_mgmt_tx_priv_req::param null, %p %p %p.}", (uintptr_t)hmac_vap,
            (uintptr_t)hmac_user, (uintptr_t)priv_req);
        return HI_ERR_CODE_PTR_NULL;
    }

    req_type = priv_req->type;

    switch (req_type) {
        case MAC_A_MPDU_START:

            hmac_mgmt_tx_ampdu_start(hmac_vap, hmac_user, priv_req);
            break;

        case MAC_A_MPDU_END:
            hmac_mgmt_tx_ampdu_end(hmac_vap, hmac_user, priv_req);
            break;

        default:

            oam_info_log1(hmac_vap->base_vap->vap_id, OAM_SF_TX,
                          "{hmac_mgmt_tx_priv_req::invalid en_req_type[%d].}", req_type);
            break;
    };

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 自身DMAC模块产生DELBA的处理函数
 修改历史      :
  1.日    期   : 2013年4月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_mgmt_rx_delba_event(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    frw_event_hdr_stru *event_hdr = HI_NULL;
    dmac_ctx_action_event_stru *delba_event = HI_NULL;
    hi_u8 *da_mac_addr = HI_NULL;      /* 保存用户目的地址的指针 */
    hmac_vap_stru *hmac_vap = HI_NULL; /* vap指针 */
    hmac_user_stru *hmac_user = HI_NULL;
    mac_action_mgmt_args_stru action_args;
    hi_u32 ret;

    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_BA, "{hmac_mgmt_rx_delba_event::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取事件头和事件结构体指针 */
    event = (frw_event_stru *)event_mem->puc_data;
    event_hdr = &(event->event_hdr);
    delba_event = (dmac_ctx_action_event_stru *)(event->auc_event_data);

    /* 获取vap结构信息 */
    hmac_vap = hmac_vap_get_vap_stru(event_hdr->vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(event_hdr->vap_id, OAM_SF_BA, "{hmac_mgmt_rx_delba_event::pst_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取目的用户的MAC ADDR */
    da_mac_addr = delba_event->auc_mac_addr;

    /* 获取发送端的用户指针 */
    hmac_user = mac_vap_get_hmac_user_by_addr(hmac_vap->base_vap, da_mac_addr, WLAN_MAC_ADDR_LEN);
    if (hmac_user == HI_NULL) {
        oam_warning_log0(event_hdr->vap_id, OAM_SF_BA,
            "{hmac_mgmt_rx_delba_event::mac_vap_find_user_by_macaddr failed.}");
        return HI_FAIL;
    }

    action_args.category = MAC_ACTION_CATEGORY_BA;
    action_args.action = MAC_BA_ACTION_DELBA;
    action_args.arg1 = delba_event->tidno;     /* 该数据帧对应的TID号 */
    action_args.arg2 = delba_event->initiator; /* DELBA中，触发删除BA会话的发起端 */
    action_args.arg3 = delba_event->stauts;    /* DELBA中代表删除reason */
    action_args.puc_arg5 = da_mac_addr;        /* DELBA中代表目的地址 */

    ret = hmac_mgmt_tx_action(hmac_vap, hmac_user, &action_args);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(event_hdr->vap_id, OAM_SF_BA, "hmac_mgmt_tx_action return NON SUCCESS. ");
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_PROMIS
hi_u32 hmac_rx_process_others_bss_management(const oal_netbuf_stru *netbuf)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hi_u32 ret;

    /* 抛事件到wal层会出现由于事件队列满导致抛事件失败,直接调用可以提高效率 */
    ret = hwal_send_others_bss_data(netbuf);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_RX, "{hmac_rx_process_others_bss_data::hwal_send_others_bss_data fail,ul_ret=%d.}",
            ret);
        return HI_FAIL;
    }
#endif
    return HI_SUCCESS;
}
#endif

hi_void hmac_rx_vap_state_proc(const dmac_wlan_crx_event_stru *crx_event, hmac_vap_stru *hmac_vap, hi_u32 *ret)
{
    if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        *ret = hmac_ap_up_rx_mgmt(hmac_vap, crx_event);
    } else if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        mac_vap_state_enum_uint8 vap_state = hmac_vap->base_vap->vap_state;

        if (vap_state == MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2) {
            *ret = hmac_sta_wait_auth_seq2_rx(hmac_vap, crx_event);
        } else if (vap_state == MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4) {
            *ret = hmac_sta_wait_auth_seq4_rx(hmac_vap, crx_event);
        } else if (vap_state == MAC_VAP_STATE_STA_WAIT_ASOC) {
            *ret = hmac_sta_wait_asoc_rx(hmac_vap, crx_event);
        } else if (vap_state == MAC_VAP_STATE_UP) {
            *ret = hmac_sta_up_rx_mgmt(hmac_vap, crx_event);
#ifdef _PRE_WLAN_FEATURE_P2P
        } else if ((vap_state == MAC_VAP_STATE_STA_SCAN_COMP) || (vap_state == MAC_VAP_STATE_STA_WAIT_SCAN) ||
            (vap_state == MAC_VAP_STATE_STA_FAKE_UP) || (vap_state == MAC_VAP_STATE_STA_LISTEN)) {
            *ret = hmac_p2p_sta_not_up_rx_mgmt(hmac_vap, crx_event);
#endif
        }
    }
}

/* ****************************************************************************
 功能描述  : HMAC模块接收WLAN_CRX事件的处理函数
 输入参数  : event_mem: 事件内存结构体指针
 返 回 值  : 成功或者失败原因
 修改历史      :
  1.日    期   : 2012年11月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_rx_process_mgmt_event(frw_event_mem_stru *event_mem)
{
    hi_u32 ret = HI_SUCCESS;

    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_RX, "{hmac_rx_process_mgmt_event::param null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取事件头和事件结构体指针 */
    frw_event_stru           *event     = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_stru       *event_hdr = &(event->event_hdr);
    dmac_wlan_crx_event_stru *crx_event = (dmac_wlan_crx_event_stru *)(event->auc_event_data);
    oal_netbuf_stru          *netbuf    = (oal_netbuf_stru *)crx_event->netbuf;

    if (netbuf == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_rx_process_mgmt_event netbuf NULL.");
        return HI_FAIL;
    }

    /* 获取vap结构信息 */
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event_hdr->vap_id);
    if (oal_unlikely((hmac_vap == HI_NULL) || (hmac_vap->base_vap == HI_NULL))) {
        oam_warning_log0(event_hdr->vap_id, OAM_SF_BA, "{hmac_mgmt_rx_delba_event::pst_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

#ifdef _PRE_WLAN_FEATURE_ANY
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    hi_u8            *data    = (hi_u8 *)(rx_ctrl->pul_mac_hdr_start_addr) + rx_ctrl->mac_header_len; /* 获取帧体指针 */

    /* 查找OUI-OUI type值为 00 E0 FC - 01 (ANY华为自定义的帧格式)  */
    /* 并用hmac_any_proc_rx_mgmt进行处理 */
    if (mac_find_vendor_action(MAC_WLAN_OUI_VENDOR, MAC_OUITYPE_ANY, data,
        rx_ctrl->us_frame_len - rx_ctrl->mac_header_len) != HI_FALSE) {
        /* 处理接收到的ANY管理帧 */
        ret = hmac_any_proc_rx_mgmt(hmac_vap, rx_ctrl->channel_number, netbuf, rx_ctrl->us_frame_len);
        oal_netbuf_free(netbuf);
        return ret;
    }
#endif

    /* 接收管理帧是状态机的一个输入，调用状态机接口 */
    hmac_rx_vap_state_proc(crx_event, hmac_vap, &ret);

    /* 管理帧统一释放接口 */
    oal_netbuf_free(netbuf);
    return ret;
}

#ifdef _PRE_WLAN_FEATURE_PROMIS
hi_u32 hmac_rx_process_mgmt_promis(frw_event_mem_stru *event_mem)
{
    hi_u32 ret = HI_SUCCESS;
    /* 获取事件头和事件结构体指针 */
    frw_event_stru           *event     = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_stru       *event_hdr = &(event->event_hdr);
    dmac_wlan_crx_event_stru *crx_event = (dmac_wlan_crx_event_stru *)(event->auc_event_data);
    oal_netbuf_stru          *netbuf    = (oal_netbuf_stru *)crx_event->netbuf;

    if (netbuf == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_rx_process_mgmt_event netbuf NULL.");
        return HI_FAIL;
    }

    /* 获取vap结构信息 */
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event_hdr->vap_id);
    if (oal_unlikely((hmac_vap == HI_NULL) || (hmac_vap->base_vap == HI_NULL))) {
        oam_warning_log0(event_hdr->vap_id, OAM_SF_BA, "{hmac_mgmt_rx_delba_event::pst_vap null.}");
        oal_netbuf_free(netbuf);
        return HI_ERR_CODE_PTR_NULL;
    }

    if ((mac_res_get_dev())->promis_switch) {
        /* 处理上报的其他BSS 管理帧 */
        ret = hmac_rx_process_others_bss_management(netbuf);
    }
    /* 管理帧统一释放接口 */
    oal_netbuf_free(netbuf);
    return ret;
}
#endif

hi_u32 hmac_mgmt_send_disasoc_deauth_event_frame(const dmac_diasoc_deauth_event *disasoc_deauth_event,
    mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u16 us_err_code, hmac_vap_stru *hmac_vap)
{
    hi_unref_param(hmac_vap);

    /* 发送去认证, 未关联状态收到第三类帧 */
    if (disasoc_deauth_event->event == DMAC_WLAN_CRX_DEAUTH) {
        hmac_mgmt_send_deauth_frame(mac_vap, mac_addr, WLAN_MAC_ADDR_LEN, us_err_code); /* 非PMF */
#ifdef _PRE_WLAN_FEATURE_MESH
        /* Mesh下这种情况下无法区分是mesh用户还是sta用户，因此发两个帧 */
        if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
            hmac_handle_close_peer_mesh(hmac_vap, mac_addr, WLAN_MAC_ADDR_LEN, HMAC_REPORT_DISASSOC, MAC_NOT_ASSOCED);
        }
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
        mac_vap_stru *up_vap1 = HI_NULL;
        mac_vap_stru *up_vap2 = HI_NULL;

        mac_device_stru *mac_dev = mac_res_get_dev();
        /* 判断是异频DBAC模式时，无法判断是哪个信道收到的数据帧，两个信道都需要发去认证 */
        if (mac_device_find_2up_vap(mac_dev, &up_vap1, &up_vap2) != HI_SUCCESS) {
            return HI_SUCCESS;
        }

        if ((up_vap1->channel.chan_number == up_vap2->channel.chan_number) &&
            (up_vap1->channel.en_bandwidth == up_vap2->channel.en_bandwidth)) {
            return HI_SUCCESS;
        }

        /* 获取另一个VAP */
        if (mac_vap->vap_id != up_vap1->vap_id) {
            up_vap2 = up_vap1;
        }

        /* 另外一个VAP也发去认证帧。error code加上特殊标记，组去认证帧时要修改源地址 */
        hmac_mgmt_send_deauth_frame(up_vap2, mac_addr, WLAN_MAC_ADDR_LEN, (us_err_code | MAC_SEND_TWO_DEAUTH_FLAG));
#endif

        return HI_SUCCESS;
    }
    return HI_CONTINUE;
}

/* ****************************************************************************
 功能描述  : 去关联/去认证事件处理
 修改历史      :
  1.日    期   : 2014年4月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_mgmt_send_disasoc_deauth_event(frw_event_mem_stru *event_mem)
{
    hi_u8 user_idx = 0;

    /* 获取事件头和事件结构体指针 */
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    dmac_diasoc_deauth_event *deauth_event = (dmac_diasoc_deauth_event *)(event->auc_event_data);

    /* 获取vap结构信息 */
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id); /* vap指针 */
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ASSOC, "{hmac_mgmt_send_disasoc_deauth_event::pst_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    mac_vap_stru *mac_vap = hmac_vap->base_vap;

    /* 获取目的用户的MAC ADDR */
    hi_u8 *mac_addr = deauth_event->auc_des_addr; /* 保存用户目的地址的指针 */
    hi_u16 err_code = deauth_event->reason;

    if (hmac_mgmt_send_disasoc_deauth_event_frame(deauth_event, mac_vap, mac_addr, err_code, hmac_vap) == HI_SUCCESS) {
        return HI_SUCCESS;
    }
    /* 获取发送端的用户指针 */
    if (mac_vap_find_user_by_macaddr(mac_vap, mac_addr, WLAN_MAC_ADDR_LEN, &user_idx) != HI_SUCCESS) {
        oam_warning_log3(0, OAM_SF_RX,
            "{hmac_mgmt_send_disasoc_deauth_event:cannot find USER by addr[XX:XX:XX:%02X:%02X:%02X]just del DMAC user}",
            mac_addr[3], mac_addr[4], mac_addr[5]); /* 3 4 5 数组索引 */

        return hmac_send_del_user_event(mac_vap, mac_addr, (hi_u8)MAC_INVALID_USER_ID);
    }

    /* 获取到hmac user,使用protected标志 */
    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        return HI_ERR_CODE_PTR_NULL;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_MESH) && (hmac_user->base_user->is_mesh_user == HI_TRUE)) {
        /* Mesh VAP下的Mesh User需要上报WPA发送Mesh Peering Close Frame 来断联，不发Assoc帧
           删除用户也由WPA调用STA remove 来删除 */
        hmac_handle_close_peer_mesh(hmac_vap, hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN,
            HMAC_REPORT_DISASSOC, err_code);
        return HI_SUCCESS;
    } else {
        hmac_mgmt_send_disassoc_frame(mac_vap, mac_addr, err_code,
            ((hmac_user == HI_NULL) ? HI_FALSE : hmac_user->base_user->cap_info.pmf_active));
    }
#else
    hmac_mgmt_send_disassoc_frame(mac_vap, mac_addr, err_code,
        ((hmac_user == HI_NULL) ? HI_FALSE : hmac_user->base_user->cap_info.pmf_active));
#endif
    /* AP删除用户之前抛事件上报内核，刷新hostapd，删除可能存在的对应STA记录 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) {
        hmac_handle_disconnect_rsp_ap(hmac_vap, hmac_user);
    }
    /* 删除用户 */
    hmac_user_del(mac_vap, hmac_user);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : HMAC去关联用户处理入口函数
 输入参数  : event_mem: 事件内存指针
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2015年1月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_proc_disasoc_misc_event(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    dmac_disasoc_misc_stru *pdmac_disasoc_misc_stru = (dmac_disasoc_misc_stru *)event->auc_event_data;
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{hmac_proc_disasoc_misc_event::pst_hmac_vap is null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    oam_warning_log2(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_proc_disasoc_misc_event::disas user[%d]in[%d]!}",
        pdmac_disasoc_misc_stru->user_idx, pdmac_disasoc_misc_stru->disasoc_reason);

    if (is_ap(hmac_vap->base_vap)) {
        hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(pdmac_disasoc_misc_stru->user_idx);
        if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
            oam_error_log0(0, OAM_SF_ASSOC, "{hmac_proc_disasoc_misc_event ap::pst_hmac_user is null.}");
            return HI_ERR_CODE_PTR_NULL;
        }

#ifdef _PRE_WLAN_FEATURE_MESH
        if (hmac_user->base_user->is_mesh_user == HI_TRUE) {
            hmac_handle_close_peer_mesh(hmac_vap, hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN,
                HMAC_REPORT_DISASSOC, pdmac_disasoc_misc_stru->disasoc_reason);
        } else {
#endif
            hi_u8 is_protected = hmac_user->base_user->cap_info.pmf_active;
            /* 抛事件上报内核，已经去关联某个STA */
            hmac_handle_disconnect_rsp_ap(hmac_vap, hmac_user);
            /* 发去关联帧 */
            hmac_mgmt_send_disassoc_frame(hmac_vap->base_vap, hmac_user->base_user->user_mac_addr, MAC_DISAS_LV_SS,
                is_protected);
            /* 删除用户 */
            hmac_user_del(hmac_vap->base_vap, hmac_user);
#ifdef _PRE_WLAN_FEATURE_MESH
        }
#endif
    } else {
        /* 获用户 */
        hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(hmac_vap->base_vap->assoc_vap_id);
        if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
            oam_warning_log0(0, OAM_SF_ASSOC, "{hmac_proc_disasoc_misc_event::pst_hmac_user is null.}");
            return HI_ERR_CODE_PTR_NULL;
        }

        if (pdmac_disasoc_misc_stru->disasoc_reason != DMAC_DISASOC_MISC_CHANNEL_MISMATCH) {
            /* 发送去认证帧到AP */
            hmac_mgmt_send_disassoc_frame(hmac_vap->base_vap, hmac_user->base_user->user_mac_addr, MAC_UNSPEC_REASON,
                (hi_u8)hmac_user->base_user->cap_info.pmf_active);
        }
        /* 删除对应用户 */
        hmac_user_del(hmac_vap->base_vap, hmac_user);
        /* 设置状态为FAKE UP */
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);
        /* hmac_proc_disasoc_misc_event, dmac_reason_code is 0~4 from dmac */
        hmac_sta_disassoc_rsp(hmac_vap, HMAC_REPORT_DEAUTH, pdmac_disasoc_misc_stru->disasoc_reason);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
