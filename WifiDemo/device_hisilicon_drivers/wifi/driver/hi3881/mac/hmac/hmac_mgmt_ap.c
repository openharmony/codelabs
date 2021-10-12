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
#include "hmac_mgmt_ap.h"
#include "hmac_encap_frame.h"
#include "hmac_encap_frame_ap.h"
#include "hmac_event.h"
#include "hmac_mgmt_bss_comm.h"
#include "mac_frame.h"
#include "hmac_rx_data.h"
#include "hmac_uapsd.h"
#include "mac_ie.h"
#include "mac_user.h"
#include "hmac_user.h"
#include "hmac_11i.h"
#include "hmac_protection.h"
#include "hmac_chan_mgmt.h"
#include "hmac_fsm.h"
#include "hmac_ext_if.h"
#include "hmac_config.h"
#ifdef _PRE_WLAN_FEATURE_P2P
#include "hmac_p2p.h"
#endif
#include "hmac_blockack.h"
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#include "hmac_ext_if.h"
#endif
#include "oal_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
static hi_u32 hmac_ap_up_update_sta_sup_rates(hi_u8 *puc_payload, hmac_user_stru *hmac_user,
    mac_status_code_enum_uint16 *pen_status_code, const hmac_sup_rates_info_stru *sup_rates_info);
static hi_u32 hmac_ap_prepare_assoc_req(hmac_user_stru *hmac_user, const hi_u8 *puc_payload, hi_u32 payload_len,
    hi_u8 mgmt_frm_type);
#ifdef _PRE_WLAN_FEATURE_MESH
static hi_u32 hmac_mesh_up_rx_confirm_action(const hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user,
    hi_u8 *puc_mac_hdr, hi_u8 *puc_payload, hi_u32 payload_len);
static hi_u32 hmac_check_open_action_sec_authenticator(const hmac_vap_stru *hmac_vap, hi_u8 *puc_payload,
    hi_u32 msg_len, const hmac_user_stru *hmac_user, mac_status_code_enum_uint16 *pen_status_code);
static hi_u32 hmac_chk_open_action_sec_cap(const mac_vap_stru *mac_vap, const hi_u8 *puc_ie, hi_u8 is_80211i_mode,
    hi_u8 offset, mac_status_code_enum_uint16 *pen_status_code);
#endif

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_PMF
/* ****************************************************************************
 功能描述  : 根据ap sta两端的pmf能力设置这条连接pmf使能状态
 输入参数  : pst_mac_vap
             pst_mac_user
             puc_rsn_ie   :存储user rsn能力的指针
 修改历史      :
  1.日    期   : 2014年5月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_set_pmf_cap(mac_user_stru *mac_user, const mac_vap_stru *mac_vap, const hi_u8 *puc_rsn_ie)
{
    hi_u16 us_rsn_capability;
    hi_u8 ap_mfpc;
    hi_u8 ap_dot11_rsna_activated;
    hi_u8 sta_mfpc;

    if (mac_vap->mib_info == HI_NULL) {
        oam_error_log0(0, 0, "hmac_set_pmf_cap::mib_info null");
        return;
    }

    ap_dot11_rsna_activated = mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated;
    us_rsn_capability = mac_get_rsn_capability(puc_rsn_ie);
    ap_mfpc = mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpc;
    sta_mfpc = (us_rsn_capability & BIT7) ? HI_TRUE : HI_FALSE;

    if ((ap_mfpc == HI_FALSE) || (sta_mfpc == HI_FALSE) || (ap_dot11_rsna_activated == HI_FALSE)) {
        mac_user_set_pmf_active(mac_user, HI_FALSE);
        return;
    } else {
        mac_user_set_pmf_active(mac_user, HI_TRUE);
        return;
    }
}
#endif

/* ****************************************************************************
 功能描述  : AP(驱动)上报去关联某个STA
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_handle_disconnect_rsp_ap(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user)
{
    hmac_send_event_to_host(hmac_vap->base_vap, (const hi_u8*)(hmac_user->base_user->user_mac_addr),
        WLAN_MAC_ADDR_LEN, HMAC_HOST_CTX_EVENT_SUB_TYPE_STA_DISCONNECT_AP);
    return;
}

/* ****************************************************************************
 功能描述  : AP(驱动)上报新关联上某个STA
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_handle_connect_rsp_ap(const hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user)
{
    hmac_asoc_user_req_ie_stru asoc_user_req_info;

    /* 上报内核的关联sta发送的关联请求帧ie信息 */
    asoc_user_req_info.puc_assoc_req_ie_buff = hmac_user->puc_assoc_req_ie_buff;
    asoc_user_req_info.assoc_req_ie_len = hmac_user->assoc_req_ie_len;

    /* 关联的STA mac地址 */
    if (memcpy_s((hi_u8 *)asoc_user_req_info.auc_user_mac_addr, WLAN_MAC_ADDR_LEN, hmac_user->base_user->user_mac_addr,
        WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_handle_connect_rsp_ap:: auc_user_mac_addr memcpy_s fail.");
        return;
    }
    hmac_send_event_to_host(hmac_vap->base_vap, (const hi_u8 *)(&asoc_user_req_info),
        sizeof(hmac_asoc_user_req_ie_stru), HMAC_HOST_CTX_EVENT_SUB_TYPE_STA_CONNECT_AP);
}

#ifdef _PRE_WLAN_FEATURE_MESH
hi_void hmac_mesh_sae_auth_proc(const hmac_vap_stru *hmac_vap, oal_netbuf_stru *auth_req)
{
    hi_u8  mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    hi_u16 auth_seq = mac_get_auth_seq_num(oal_netbuf_header(auth_req)); /* 解析auth transaction number */
    if (auth_seq > WLAN_AUTH_TRASACTION_NUM_TWO) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
            "{hmac_ap_rx_auth_req::[MESH]auth receive invalid seq, auth seq [%d]}", auth_seq);
        return;
    }
    oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
        "{hmac_ap_rx_auth_req::[MESH]auth receive valid seq, auth seq [%d]}", auth_seq);

    /* 获取STA的地址 */
    mac_get_address2(oal_netbuf_header(auth_req), WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN);
    if (mac_addr_is_zero(mac_addr)) {
        oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
            "{hmac_ap_rx_auth_req::[MESH]user mac:XX:XX:XX:%02X:%02X:%02X is all 0 and invalid!}", mac_addr[3],
            mac_addr[4], mac_addr[5]); /* 3 4 5 元素索引 */
        return;
    }

    hmac_rx_ctl_stru *rx_ctl = (hmac_rx_ctl_stru *)oal_netbuf_cb(auth_req);
    hmac_user_stru *hmac_user = mac_vap_get_hmac_user_by_addr(hmac_vap->base_vap, mac_addr, WLAN_MAC_ADDR_LEN);

    if ((hmac_user != HI_NULL) && (hmac_user->base_user->user_asoc_state != MAC_USER_STATE_ASSOC) &&
        (hmac_user->base_user->is_mesh_user == HI_TRUE)) {
        /* 更新用户的RSSI统计信息 */
        hmac_user->base_user->rx_conn_rssi = wlan_rssi_lpf(hmac_user->base_user->rx_conn_rssi, rx_ctl->rssi_dbm);
    }

    hmac_rx_mgmt_send_to_host(hmac_vap, auth_req);

    oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
        "{hmac_ap_rx_auth_req::[MESH] report Auth Req to host,user mac:XX:XX:XX:%02X:%02X:%02X.}", mac_addr[3],
        mac_addr[4], mac_addr[5]); /* 3 4 5 元素索引 */
}
#endif

/* ****************************************************************************
 功能描述  : 处理认证请求帧
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ap_rx_auth_req(const hmac_vap_stru *hmac_vap, const oal_netbuf_stru *auth_req)
{
    hi_u8  auc_chtxt[WLAN_CHTXT_SIZE] = {0};
    hi_u16 auth_alg = mac_get_auth_alg(oal_netbuf_header(auth_req));
    hi_u8 chtxt_index;

    if (auth_alg == WLAN_WITP_AUTH_SHARED_KEY) {
        /* 获取challenge text */
        for (chtxt_index = 0; chtxt_index < WLAN_CHTXT_SIZE; chtxt_index++) {
            auc_chtxt[chtxt_index] = oal_get_random();
        }
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    /* MESH VAP时收到认证请求帧且认证类型为SAE 时直接发送给WPA_SUPPLICANT处理,不在此处组Auth Response帧 */
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) && (auth_alg == WLAN_WITP_AUTH_SAE)) {
        hmac_mesh_sae_auth_proc(hmac_vap, auth_req);
        return;
    }
#endif

    /* AP接收到STA发来的认证请求帧组相应的认证响应帧 */
    oal_netbuf_stru *auth_rsp = (oal_netbuf_stru *)oal_netbuf_alloc(WLAN_MGMT_NETBUF_SIZE, 0, 4); /* align 4 */
    if (auth_rsp == HI_NULL) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_ap_rx_auth_req::pst_auth_rsp null.}");
        return;
    }

    /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(oal_netbuf_cb(auth_rsp), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    hi_u16 us_auth_rsp_len = hmac_encap_auth_rsp(hmac_vap->base_vap, auth_rsp, auth_req, auc_chtxt, WLAN_CHTXT_SIZE);
    if (us_auth_rsp_len == 0) {
        oal_netbuf_free(auth_rsp);
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_ap_rx_auth_req::auth_rsp_len is 0.}");
        return;
    }

    oal_netbuf_put(auth_rsp, us_auth_rsp_len);

    /* 发送认证响应帧之前，将用户的节能状态复位 */
    hmac_tx_ctl_stru *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(auth_rsp);
    if (hmac_user_get_user_stru(tx_ctl->us_tx_user_idx) != HI_NULL) {
        hmac_mgmt_reset_psm(hmac_vap->base_vap, tx_ctl->us_tx_user_idx);
    }

    /* 抛事件给dmac发送认证帧 */
    hi_u32 ret = hmac_tx_mgmt_send_event(hmac_vap->base_vap, auth_rsp, us_auth_rsp_len);
    if (ret != HI_SUCCESS) {
        oal_netbuf_free(auth_rsp);
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
            "{hmac_ap_rx_auth_req::hmac_tx_mgmt_send_event failed[%d].}", ret);
    }
}

/* ****************************************************************************
 功能描述  : 处理接收去认证帧
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ap_rx_deauth_req(const hmac_vap_stru *hmac_vap, hi_u8 *puc_mac_hdr, hi_u8 is_protected)
{
    hi_u8 *sa_mac_addr = HI_NULL;
    hi_u8 *da_mac_addr = HI_NULL;
    hi_u16 us_err_code = *((hi_u16 *)(puc_mac_hdr + MAC_80211_FRAME_LEN));

    mac_rx_get_sa((mac_ieee80211_frame_stru *)puc_mac_hdr, &sa_mac_addr);

    /* 增加接收到去认证帧时的维测信息 */
    oam_warning_log3(0, OAM_SF_AUTH, "{aput rx deauth frame, reason code = %d, sa[XX:XX:XX:XX:%X:%X]}", us_err_code,
        sa_mac_addr[4], sa_mac_addr[5]); /* 4 5MAC地址最后2字节  */

    hmac_user_stru *hmac_user = mac_vap_get_hmac_user_by_addr(hmac_vap->base_vap, sa_mac_addr, WLAN_MAC_ADDR_LEN);
    if (hmac_user == HI_NULL) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{aput rx deauth frame, pst_hmac_user null.}");
        return HI_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_PMF
    /* 检查是否需要发送SA query request */
    if ((hmac_user->base_user->user_asoc_state == MAC_USER_STATE_ASSOC) &&
        (hmac_pmf_check_err_code(hmac_user->base_user, is_protected, puc_mac_hdr) == HI_SUCCESS)) {
        /* 在关联状态下收到未加密的ReasonCode 6/7需要开启SA Query流程 */
        hi_u32 ret = hmac_start_sa_query(hmac_vap->base_vap, hmac_user, hmac_user->base_user->cap_info.pmf_active);
        if (ret != HI_SUCCESS) {
            return HI_ERR_CODE_PMF_SA_QUERY_START_FAIL;
        }
        return HI_SUCCESS;
    }
#endif

    /* 如果该用户的管理帧加密属性不一致，丢弃该报文 */
    mac_rx_get_da((mac_ieee80211_frame_stru *)puc_mac_hdr, &da_mac_addr);
    if ((HI_TRUE != ether_is_multicast(da_mac_addr)) && (is_protected != hmac_user->base_user->cap_info.pmf_active)) {
        oam_error_log2(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_ap_rx_deauth_req::PMF check failed %d %d.}",
            is_protected, hmac_user->base_user->cap_info.pmf_active);
        return HI_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_MESH
    /* 当源地址指向一个Mesh类型的User时，有可能收到Deauth帧，同样上报并删除用户即可 */
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) && (hmac_user->base_user->is_mesh_user == HI_TRUE)) {
        oam_warning_log4(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
            "{meshut rx deauth frame [mesh user], reason code = %d, sa[XX:XX:XX:%X:%X:%X]}", us_err_code,
            sa_mac_addr[3], sa_mac_addr[4], sa_mac_addr[5]); /* 3 4 5 元素索引 */

        hmac_handle_close_peer_mesh(hmac_vap, sa_mac_addr, WLAN_MAC_ADDR_LEN, HMAC_REPORT_DEAUTH,
            DMAC_DISASOC_MISC_KICKUSER);
    } else {
#endif
        /* 抛事件上报内核，已经去关联某个STA */
        hmac_handle_disconnect_rsp_ap(hmac_vap, hmac_user);
        hmac_user_del(hmac_vap->base_vap, hmac_user);
#ifdef _PRE_WLAN_FEATURE_MESH
    }
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 按一定顺序将user中速率重新排序
 修改历史      :
  1.日    期   : 2013年7月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_user_sort_op_rates(hmac_user_stru *hmac_user)
{
    hi_u8 loop;
    hi_u8 num_rates;
    hi_u8 min_rate;
    hi_u8 temp_rate;     /* 临时速率，用于数据交换 */
    hi_u8 index;
    hi_u8 temp_index;    /* 临时索引，用于数据交换 */

    num_rates = hmac_user->op_rates.rs_nrates;

    for (loop = 0; loop < num_rates; loop++) {
        /* 记录当前速率为最小速率 */
        min_rate = (hmac_user->op_rates.auc_rs_rates[loop] & 0x7F);
        temp_index = loop;

        /* 依次查找最小速率 */
        for (index = loop + 1; index < num_rates; index++) {
            /* 记录的最小速率大于如果当前速率 */
            if (min_rate > (hmac_user->op_rates.auc_rs_rates[index] & 0x7F)) {
                /* 更新最小速率 */
                min_rate = (hmac_user->op_rates.auc_rs_rates[index] & 0x7F);
                temp_index = index;
            }
        }

        temp_rate = hmac_user->op_rates.auc_rs_rates[loop];
        hmac_user->op_rates.auc_rs_rates[loop] = hmac_user->op_rates.auc_rs_rates[temp_index];
        hmac_user->op_rates.auc_rs_rates[temp_index] = temp_rate;
    }
}

/* ****************************************************************************
 功能描述  : AP侧设置STA的能力信息
 修改历史      :
  1.日    期   : 2013年7月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2013年7月8日
    作    者   : HiSilicon
    修改内容   : 添加函数检查内容
**************************************************************************** */
static hi_u8 hmac_ap_up_update_sta_cap_info(const hmac_vap_stru *hmac_vap, hi_u16 us_cap_info,
    const hmac_user_stru *hmac_user, mac_status_code_enum_uint16 *pen_status_code)
{
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u8 ret;
    mac_cap_info_stru *cap_info = (mac_cap_info_stru *)(&us_cap_info);

    mac_vap = hmac_vap->base_vap;
    /* check bss capability info MAC,忽略MAC能力不匹配的STA */
    ret = hmac_check_bss_cap_info(us_cap_info, mac_vap);
    if (ret != HI_TRUE) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ANY,
            "{hmac_ap_up_update_sta_cap_info::hmac_check_bss_cap_info failed[%d].}", ret);
        *pen_status_code = MAC_UNSUP_CAP;
        return HI_FALSE;
    }

    /* 如果以上各能力信息均满足关联要求，则继续处理其他能力信息 */
    mac_vap_check_bss_cap_info_phy_ap(us_cap_info, mac_vap);

    if ((0 == cap_info->privacy) &&
        (hmac_user->base_user->key_info.cipher_type != WLAN_80211_CIPHER_SUITE_NO_ENCRYP)) {
        oam_warning_log2(hmac_vap->base_vap->vap_id, OAM_SF_ANY,
            "{hmac_ap_up_update_sta_cap_info::Encrypt not match,cap privacy[%d],user_cipher_type[%d].}",
            cap_info->privacy, hmac_user->base_user->key_info.cipher_type);
        *pen_status_code = MAC_UNSPEC_FAIL;
        return HI_FALSE;
    }

    return HI_TRUE;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : 检查更新关联STA的速率信息(mesh add sta接口适配)
 返 回 值  : hi_u32
 修改历史      :
  1.日    期   : 2019年1月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_mesh_update_sta_sup_rates(hi_u8 *puc_sup_rates, hi_u8 sup_rates_len, hmac_user_stru *hmac_user,
    mac_status_code_enum_uint16 *pen_status_code)
{
    mac_user_stru *mac_user = HI_NULL;
    mac_vap_stru  *mac_vap  = HI_NULL;
    hi_u8            puc_sup_rates_set[MAC_MAX_SUPRATES] = {0};
    hi_u32 loop;

    mac_user = hmac_user->base_user;
    mac_vap = mac_vap_get_vap_stru(mac_user->vap_id);
    if (mac_vap == HI_NULL) {
        *pen_status_code = MAC_UNSUP_RATE;
        oam_error_log0(mac_user->vap_id, OAM_SF_ANY, "{hmac_mesh_update_sta_sup_rates::pst_mac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (sup_rates_len == 0) {
        *pen_status_code = MAC_UNSUP_RATE;
        oam_error_log0(mac_user->vap_id, OAM_SF_ANY,
            "{hmac_mesh_update_sta_sup_rates::the sta's rates are not supported.}");
        return HI_FAIL;
    }

    if (puc_sup_rates != HI_NULL) {
        if (memcpy_s(puc_sup_rates_set, sup_rates_len, puc_sup_rates, sup_rates_len) != EOK) {
            oam_error_log2(mac_user->vap_id, OAM_SF_ANY,
                "{hmac_mesh_update_sta_sup_rates::memcpy_s fail, puc_sup_rates_set[%p], puc_sup_rates[%p].}",
                (uintptr_t)puc_sup_rates_set, (uintptr_t)puc_sup_rates);
            return HI_FAIL;
        }
    }

    if (sup_rates_len > MAC_MAX_SUPRATES) {
        oam_warning_log1(0, OAM_SF_ANY, "hmac_mesh_update_sta_sup_rates: ul_sup_rates_len error: %d", sup_rates_len);
        sup_rates_len = MAC_MAX_SUPRATES;
    }

    for (loop = 0; loop < sup_rates_len; loop++) {
        /* 保存对应的速率到USER中 */
        hmac_user->op_rates.auc_rs_rates[loop] = puc_sup_rates_set[loop] & 0x7F;
    }
    hmac_user->op_rates.rs_nrates = sup_rates_len;

    /* 按一定顺序重新排列速率 */
    hmac_user_sort_op_rates(hmac_user);

    /* ******************************************************************
    如果STA不支持所有基本速率返回不支持速率的错误码
    ****************************************************************** */
    hmac_check_sta_base_rate((hi_u8 *)hmac_user, pen_status_code);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  :通知WPA发送Mesh Close Peer，与对端断开连接。
 输入参数  : [1]hmac_vap
             [2]puc_mac_addr,
             [3]us_disasoc_reason_code
             [4]us_dmac_reason_code
 返 回 值  :hi_u32
**************************************************************************** */
hi_u32 hmac_handle_close_peer_mesh(const hmac_vap_stru *hmac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len,
    hi_u16 us_disasoc_reason_code, hi_u16 us_dmac_reason_code)
{
    hi_u8 data[WLAN_MAC_ADDR_LEN + WLAN_MAC_ADDR_LEN] = {0};

    /* 去关联的STA mac地址 */
    if (mac_addr != HI_NULL) {
        if (memcpy_s((hi_u8 *)data, WLAN_MAC_ADDR_LEN, mac_addr, mac_addr_len) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "hmac_handle_close_peer_mesh:: puc_mac_addr memcpy_s fail.");
            return HI_FAIL;
        }
    }
    /* 填充断联原因 */
    /* 事件payload填写的是错误码 */
    *((hi_u32 *)(data + WLAN_MAC_ADDR_LEN)) =
        ((us_disasoc_reason_code & 0x0000ffff) | ((us_dmac_reason_code << 16) & 0xffff0000)); /* 16:左移 */

    return hmac_send_event_to_host(hmac_vap->base_vap, (const hi_u8 *)data, WLAN_MAC_ADDR_LEN + sizeof(hi_u32),
        HMAC_HOST_CTX_EVENT_SUB_TYPE_PEER_CLOSE_MESH);
}
#endif

hi_void hmac_resort_rates_info_cyc(hmac_user_stru *hmac_user, const hi_u8 *puc_ext_sup_rates_ie, hi_u8 uc_num_rates,
    hi_u8 *uc_num_ex_rates, hi_u16 us_msg_idx_ex)
{
    hi_u32 loop;

    /* 超出支持速率总个数,扩展速率集进行修正 */
    if (uc_num_rates + *uc_num_ex_rates > WLAN_MAX_SUPP_RATES) {
        *uc_num_ex_rates = WLAN_MAX_SUPP_RATES - uc_num_rates;
    }

    if (uc_num_rates < WLAN_MAX_SUPP_RATES) {
        for (loop = 0; loop < *uc_num_ex_rates; loop++) {
            hmac_user->op_rates.auc_rs_rates[uc_num_rates + loop] = puc_ext_sup_rates_ie[us_msg_idx_ex + loop] & 0x7F;
        }
    }
}

hi_void hmac_resort_rates_info(hmac_user_stru *hmac_user, const hi_u8 *puc_ext_sup_rates_ie, hi_u8 uc_num_rates)
{
    hi_u16 us_msg_idx_ex = 0;
    hi_u8 temp_rate;
    hi_u8 uc_num_ex_rates = 0;
    mac_vap_stru *mac_vap = HI_NULL;

    mac_vap = mac_vap_get_vap_stru(hmac_user->base_user->vap_id);
    if (mac_vap == HI_NULL) {
        return;
    }
    if (puc_ext_sup_rates_ie == HI_NULL) {
        if (mac_vap->channel.band == WLAN_BAND_2G) {
            oam_warning_log0(0, OAM_SF_ANY, "{hmac_ap_up_update_sta_user::puc_ext_sup_rates_ie null.}");
        }
    } else {
        uc_num_ex_rates = puc_ext_sup_rates_ie[1];
        us_msg_idx_ex += MAC_IE_HDR_LEN;
        /* 只有11g混合模式或者更高协议版本才支持ERP */
        if ((mac_vap->protocol > WLAN_LEGACY_11G_MODE) &&
            (uc_num_ex_rates <= WLAN_MAX_SUPP_RATES && uc_num_ex_rates >= MAC_MIN_XRATE_LEN)) {
            hmac_resort_rates_info_cyc(hmac_user, puc_ext_sup_rates_ie, uc_num_rates, &uc_num_ex_rates, us_msg_idx_ex);
        } else {
            uc_num_ex_rates = 0;
        }
    }

    /* 更新STA支持的速率个数 */
    hmac_user->op_rates.rs_nrates = uc_num_rates + uc_num_ex_rates;

    /* 按一定顺序重新排列速率 */
    hmac_user_sort_op_rates(hmac_user);

    /* ******************************************************************
      重排11g模式的可操作速率，使11b速率都聚集在11a之前
      802.11a 速率:6、9、12、18、24、36、48、54Mbps
      802.11b 速率:1、2、5.5、11Mbps
      由于按由小到大排序后802.11b中的速率11Mbps在802.11a中，下标为5
      所以从第五位进行检验并排序。
    ****************************************************************** */
    if (hmac_user->op_rates.rs_nrates == MAC_DATARATES_PHY_80211G_NUM) { /* 11g_compatibility mode */
        if ((hmac_user->op_rates.auc_rs_rates[5] & 0x7F) == 0x16) {      /* 5 元素索引 11Mbps */
            temp_rate = hmac_user->op_rates.auc_rs_rates[5];
            hmac_user->op_rates.auc_rs_rates[5] = hmac_user->op_rates.auc_rs_rates[4]; /* 5 4 元素索引 */
            hmac_user->op_rates.auc_rs_rates[4] = hmac_user->op_rates.auc_rs_rates[3]; /* 4 3 元素索引 */
            hmac_user->op_rates.auc_rs_rates[3] = temp_rate;                           /* 3 元素索引 */
        }
    }
}

/* ****************************************************************************
 功能描述  : 检查更新AP侧关联STA的速率信息
 输入参数  : hi_u8                     *puc_payload    -- 关联帧帧体内容指针
             hi_u32                     ul_msg_len     -- 关联帧帧体长度
             hmac_user_stru                *pst_hmac_user  -- 关联用户的结构体指针
             hi_u16                    *pus_msg_idx    -- 关联帧帧体偏移索引
 输出参数  : wlan_status_code_enum_uint16  *pen_status_code-- 关联状态码
             hi_u8                     *puc_num_rates  -- 关联用户的速率个数
             hi_u16                    *pus_msg_idx    -- 关联帧帧体偏移索引
 返 回 值  : hi_u32
 修改历史      :
  1.日    期   : 2013年7月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ap_up_update_sta_sup_rates(hi_u8 *puc_payload, hmac_user_stru *hmac_user,
    mac_status_code_enum_uint16 *pen_status_code, const hmac_sup_rates_info_stru *sup_rates_info)
{
    hi_u8 uc_num_rates = 0;
    hi_u32 loop;
    mac_user_stru *mac_user = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u8 *puc_sup_rates_ie = HI_NULL;
    hi_u8 *puc_ext_sup_rates_ie = HI_NULL;
    hi_u16 us_msg_idx = 0;

    mac_user = hmac_user->base_user;
    mac_vap = mac_vap_get_vap_stru(mac_user->vap_id);
    if (mac_vap == HI_NULL) {
        *pen_status_code = MAC_UNSUP_RATE;
        oam_error_log0(mac_user->vap_id, OAM_SF_ANY, "{hmac_ap_up_update_sta_sup_rates::pst_mac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (sup_rates_info->msg_len > sup_rates_info->us_offset) {
        puc_sup_rates_ie = mac_find_ie(MAC_EID_RATES, puc_payload + sup_rates_info->us_offset,
            (sup_rates_info->msg_len - sup_rates_info->us_offset));
    }
    if (puc_sup_rates_ie == HI_NULL) {
        *pen_status_code = MAC_UNSUP_RATE;
        oam_error_log0(mac_user->vap_id, OAM_SF_ANY, "{hmac_ap_up_update_sta_user::puc_ie null.}");
        return HI_ERR_CODE_PTR_NULL;
    } else {
        uc_num_rates = puc_sup_rates_ie[1];

        /* 如果速率个数为0或超过最大支持速率 ，直接返回失败 */
        if (uc_num_rates == 0 || uc_num_rates > WLAN_MAX_SUPP_RATES) {
            *pen_status_code = MAC_UNSUP_RATE;
            *(sup_rates_info->pus_msg_idx) = us_msg_idx;
            oam_error_log0(mac_user->vap_id, OAM_SF_ANY,
                "{hmac_ap_up_update_sta_sup_rates::the sta's rates are not supported.}");
            return HI_FAIL;
        }

        us_msg_idx += MAC_IE_HDR_LEN;

        for (loop = 0; loop < uc_num_rates; loop++) {
            /* 保存对应的速率到USER中 */
            hmac_user->op_rates.auc_rs_rates[loop] = puc_sup_rates_ie[us_msg_idx + loop] & 0x7F;
        }

        us_msg_idx += uc_num_rates;
        hmac_user->op_rates.rs_nrates = uc_num_rates;
    }

    if (sup_rates_info->msg_len > sup_rates_info->us_offset) {
        /* 如果存在扩展速率 */
        puc_ext_sup_rates_ie = mac_find_ie(MAC_EID_XRATES, puc_payload + sup_rates_info->us_offset,
            (sup_rates_info->msg_len - sup_rates_info->us_offset));
    }

    hmac_resort_rates_info(hmac_user, puc_ext_sup_rates_ie, uc_num_rates);

    /* ******************************************************************
      如果STA不支持所有基本速率返回不支持速率的错误码
    **************************************************************** */
    hmac_check_sta_base_rate((hi_u8 *)hmac_user, pen_status_code);

    *(sup_rates_info->pus_msg_idx) = us_msg_idx;
    return HI_SUCCESS;
}

#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
/* ****************************************************************************
 功能描述  : AP 检测STA 关联请求帧中的安全相关公共信息
 输入参数  : [1]mac_vap
             [2]puc_ie
             [3]is_80211i_mode
             [4]offset
             [5]pen_status_code
 返 回 值  : static hi_u32
**************************************************************************** */
static hi_u32 hmac_check_assoc_req_security_cap_common(const mac_vap_stru *mac_vap, const hi_u8 *ie, hi_u8 is_80211i,
    hi_u8 offset, mac_status_code_enum_uint16 *pen_status_code)
{
    wlan_mib_ieee802dot11_stru *mib_info = mac_vap->mib_info;
    hi_u8 len = 0;
    hi_u8 auc_oui[MAC_OUI_LEN];

    if (hmac_get_security_oui(is_80211i, auc_oui) != HI_SUCCESS) {
        *pen_status_code = MAC_UNSUP_RSN_INFO_VER;
        return HI_FAIL;
    }
    /* 检测版本信息 */
    if (mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_version != hi_makeu16(ie[offset], ie[offset + 1])) {
        *pen_status_code = MAC_UNSUP_RSN_INFO_VER;
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_check_assoc_req_security_cap_common::unsup rsn ver=%d.}",
            hi_makeu16(ie[offset], ie[offset + 1]));
        return HI_FAIL;
    }
    /* 忽略版本信息 */
    hi_u8 index = offset + 2; /* 2 忽略版本信息 */

    if (memcmp(auc_oui, ie + index, MAC_OUI_LEN) != 0) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_check_assoc_req_security_cap_common::invalid OUI.}");
    }
    index += MAC_OUI_LEN;
    hi_u8 grp_policy = ie[index++]; /* 获取组播密钥 */

    /* 获取单播加密套件 */
    hi_u8 pcip_policy = hmac_get_pcip_policy_auth(ie + index, &len);
    index += len;

    /* 获取认证套件信息 */
    hi_u8 auth_policy = hmac_get_auth_policy_auth(ie + index, &len);

    /* 成对密钥套件是否为组播密钥套件 */
    pcip_policy = (pcip_policy == WLAN_80211_CIPHER_SUITE_GROUP_CIPHER) ? grp_policy : pcip_policy;

    /* 检测组播套件 */
    if (mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher != grp_policy) {
        *pen_status_code = MAC_INVALID_GRP_CIPHER;
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_check_assoc_req_security_cap_common::grp %d}", grp_policy);
        return HI_FAIL;
    }
    /* 检测单播密钥套件 */
    if (hmac_check_pcip_policy(mib_info, pcip_policy, is_80211i) != HI_SUCCESS) {
        *pen_status_code = MAC_INVALID_PW_CIPHER;
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_check_assoc_req_security_cap_common:pcip%d}", pcip_policy);
        return HI_FAIL;
    }
    /* 检测认证套件 */
    if (mac_check_auth_policy(mib_info, auth_policy) == HI_FALSE) {
        *pen_status_code = MAC_INVALID_AKMP_CIPHER;
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_check_assoc_req_security_cap_common:auth%d}", auth_policy);
        return HI_FAIL;
    }
    /* 禁止单播套件为TKIP, 组播套件为CCMP */
    if ((grp_policy == WLAN_80211_CIPHER_SUITE_CCMP) && (pcip_policy == WLAN_80211_CIPHER_SUITE_TKIP)) {
        *pen_status_code = MAC_CIPHER_REJ;
        oam_error_log0(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_check_assoc_req_security_cap_common::conflict.}");
        return HI_FAIL;
    }
    /* 更新mib 参数 */
    mib_info->wlan_mib_rsna_cfg.dot11_rsna_group_cipher_requested = grp_policy;
    mib_info->wlan_mib_rsna_cfg.dot11_rsna_pairwise_cipher_requested = pcip_policy;

    return HI_SUCCESS;
}

static hi_u32 hmac_chk_assoc_req_sec_cap_auth_encry(const mac_vap_stru *mac_vap,
    mac_status_code_enum_uint16 *pen_status_code, hi_u8 *puc_rsn_ie, hi_u8 *puc_wpa_ie)
{
    hi_u8 is_80211i_mode = 0;
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 index = 0;
    hi_u8 rsn_found = HI_FALSE; /* AP 接收到的ASSOC REQ帧IE 是否匹配 */

    /* RSNA 或 WPA加密方式处理 */
    if (mac_vap->cap_flag.wpa2 == HI_TRUE) {
        if (puc_rsn_ie != HI_NULL) {
            rsn_found = HI_TRUE;
            is_80211i_mode = DMAC_RSNA_802_11I;

            /* 略过 IE + LEN */
            index += 2; /* 2 略过 IE + LEN */

            puc_ie = puc_rsn_ie;
        }
    }

    if ((rsn_found == HI_FALSE) && (mac_vap->cap_flag.wpa == HI_TRUE)) {
        if (puc_wpa_ie != HI_NULL) {
            rsn_found = HI_TRUE;
            is_80211i_mode = DMAC_WPA_802_11I;

            /* 略过 IE(1字节) + LEN(1字节) + WPA OUI(4字节) */
            index += 6; /* 共6个字节 */

            puc_ie = puc_wpa_ie;
        } else {
            *pen_status_code = MAC_CIPHER_REJ;
            return HI_FAIL;
        }
    }

    if (((rsn_found == HI_TRUE) && (puc_ie != HI_NULL)) && (hmac_check_assoc_req_security_cap_common(mac_vap, puc_ie,
        is_80211i_mode, index, pen_status_code) != HI_SUCCESS)) {
        return HI_FAIL;
    }

    oam_info_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_assoc_req_sec_cap_auth_encry::is_80211i_mode = %d}",
        is_80211i_mode);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : AP 检查接收到的ASOC REQ消息中的SECURITY参数.如出错,则返回对应的错
             误码
 输入参数  : [1]hmac_vap
             [2]puc_mac_hdr
             [3]puc_payload
             [4]msg_len
             [5]hmac_user
             [6]pen_status_code
 返 回 值  : static hi_u8
**************************************************************************** */
static hi_u32 hmac_chk_assoc_req_sec_cap_auth(const hmac_vap_stru *vap, const hi_u8 *hdr, hi_u8 *payload, hi_u32 len,
    const hmac_user_stru *user, mac_status_code_enum_uint16 *status)
{
    hi_u8 pcip_policy = WLAN_80211_CIPHER_SUITE_NO_ENCRYP;
    hi_u8 grp_policy = WLAN_80211_CIPHER_SUITE_NO_ENCRYP;
    hi_u8 *puc_rsn_ie = HI_NULL;
    hi_u8 *puc_wpa_ie = HI_NULL;

    mac_vap_stru *mac_vap = vap->base_vap;
    wlan_mib_ieee802dot11_stru *mib_info = mac_vap->mib_info;

    mib_info->wlan_mib_rsna_cfg.dot11_rsna_group_cipher_requested = 0;
    mib_info->wlan_mib_rsna_cfg.dot11_rsna_pairwise_cipher_requested = 0;

    if (mib_info->wlan_mib_privacy.dot11_rsna_activated != HI_TRUE) {
        oam_info_log0(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_assoc_req_sec_cap_auth::dot11RSNAActivated = FALSE}");
        return HI_SUCCESS;
    }

    hi_u8 offset = MAC_CAP_INFO_LEN + MAC_LIS_INTERVAL_IE_LEN;
    if (mac_get_frame_sub_type(hdr) == WLAN_FC0_SUBTYPE_REASSOC_REQ) {
        offset += OAL_MAC_ADDR_LEN;
    }
    if (len > offset) {
        /* 获取RSNA和WPA IE信息 */
        puc_rsn_ie = mac_find_ie(MAC_EID_RSN, payload + offset, (len - offset));
        puc_wpa_ie = mac_find_vendor_ie(MAC_WLAN_OUI_MICROSOFT, MAC_OUITYPE_WPA, payload + offset,
            (hi_s32)(len - offset));
    }

    if ((puc_rsn_ie == HI_NULL) && (puc_wpa_ie == HI_NULL)) {
        /* 加密方式不是WPA/WPA2， 则判断是否为WEP */
        grp_policy = mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher;
        if (is_wep_cipher(grp_policy)) {
            mib_info->wlan_mib_rsna_cfg.dot11_rsna_pairwise_cipher_requested = grp_policy;
        } else if (vap->wps_active == HI_TRUE) {
            mac_user_init_key(user->base_user);
            return HI_SUCCESS;
        } else {
            *status = MAC_UNSPEC_FAIL;
            oam_error_log0(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_assoc_req_sec_cap_auth:WPA/WPA2}");
            return HI_FAIL;
        }
    } else {
        if (hmac_chk_assoc_req_sec_cap_auth_encry(mac_vap, status, puc_rsn_ie, puc_wpa_ie) != HI_SUCCESS) {
            return HI_FAIL;
        }
    }

    if (*status == MAC_SUCCESSFUL_STATUSCODE) {
        /* 检测出来的这个硬件加密方式应该保存到 mac_user -> en_cipher_type 中 */
        mac_user_stru *mac_user = user->base_user;
        /* 保存用户的加密方式 */
        mac_user->key_info.cipher_type = pcip_policy;
    }
    oam_info_log2(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_assoc_req_sec_cap_auth::"
                  "group=%d, pairwise=%d, auth=%d}", grp_policy, pcip_policy);

    return HI_SUCCESS;
}
#endif /* (_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2) */

/* ****************************************************************************
 功能描述  : 查看sta是否属于erp站点
 输入参数  : mac_user_stru               pst_mac_user
 返 回 值  : HI_TRUE: sta为erp站点
            HI_FALSE:sta为非erp站点(11b站点)
 修改历史      :
  1.日    期   : 2014年3月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u8 hmac_is_erp_sta(const hmac_user_stru *hmac_user)
{
    hi_u32 loop;
    hi_u8 is_erp_sta;

    /* 确认是否是erp 站点 */
    if (hmac_user->op_rates.rs_nrates <= MAC_NUM_DR_802_11B) {
        is_erp_sta = HI_FALSE;
        for (loop = 0; loop < hmac_user->op_rates.rs_nrates; loop++) {
            /* 如果支持速率不在11b的1M, 2M, 5.5M, 11M范围内，则说明站点为支持ERP的站点 */
            if (((hmac_user->op_rates.auc_rs_rates[loop] & 0x7F) != 0x2) &&
                ((hmac_user->op_rates.auc_rs_rates[loop] & 0x7F) != 0x4) &&
                ((hmac_user->op_rates.auc_rs_rates[loop] & 0x7F) != 0xb) &&
                ((hmac_user->op_rates.auc_rs_rates[loop] & 0x7F) != 0x16)) {
                is_erp_sta = HI_TRUE;
                break;
            }
        }
    } else {
        is_erp_sta = HI_TRUE;
    }

    return is_erp_sta;
}

/* ****************************************************************************
 功能描述  : AP处理assoc req 中的capability info
 输入参数  : pst_hmac_vap   : hmac vap结构体指针
             pst_hmac_user  : hmac user结构体指针
             us_cap_info    : 帧体中cap info信息
 返 回 值  : hi_u32
 修改历史      :
  1.日    期   : 2013年7月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ap_up_update_legacy_capability(const hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user,
    hi_u16 us_cap_info)
{
    mac_protection_stru *protection = &(hmac_vap->base_vap->protection);
    mac_user_stru *mac_user = hmac_user->base_user;

    /* 如果STA不支持short slot */
    if ((us_cap_info & MAC_CAP_SHORT_SLOT) != MAC_CAP_SHORT_SLOT) {
        /* 如果STA之前没有关联， 或者之前以支持short slot站点身份关联，需要update处理 */
        if ((mac_user->user_asoc_state != MAC_USER_STATE_ASSOC) ||
            (hmac_user->hmac_cap_info.short_slot_time == HI_TRUE)) {
            protection->sta_no_short_slot_num++;
        }

        hmac_user->hmac_cap_info.short_slot_time = HI_FALSE;
    } else {
        /* 如果STA以不支持short slot站点身份关联，需要update处理 */
        if ((mac_user->user_asoc_state == MAC_USER_STATE_ASSOC) &&
            (hmac_user->hmac_cap_info.short_slot_time == HI_FALSE) && (protection->sta_no_short_slot_num != 0)) {
            protection->sta_no_short_slot_num--;
        }

        hmac_user->hmac_cap_info.short_slot_time = HI_TRUE;
    }

    hmac_user->user_stats_flag.no_short_slot_stats_flag = HI_TRUE;

    /* 如果STA不支持short preamble */
    if ((us_cap_info & MAC_CAP_SHORT_PREAMBLE) != MAC_CAP_SHORT_PREAMBLE) {
        /* 如果STA之前没有关联， 或者之前以支持short preamble站点身份关联，需要update处理 */
        if ((mac_user->user_asoc_state != MAC_USER_STATE_ASSOC) ||
            (hmac_user->hmac_cap_info.short_preamble == HI_TRUE)) {
            protection->sta_no_short_preamble_num++;
        }

        hmac_user->hmac_cap_info.short_preamble = HI_FALSE;
    } else {
        /* 如果STA之前以不支持short preamble站点身份关联，需要update处理 */
        if ((mac_user->user_asoc_state == MAC_USER_STATE_ASSOC) &&
            (hmac_user->hmac_cap_info.short_preamble == HI_FALSE) && (protection->sta_no_short_preamble_num != 0)) {
            protection->sta_no_short_preamble_num--;
        }

        hmac_user->hmac_cap_info.short_preamble = HI_TRUE;
    }

    hmac_user->user_stats_flag.no_short_preamble_stats_flag = HI_TRUE;

    /* 确定user是否是erp站点 */
    hi_u8 is_erp_sta = hmac_is_erp_sta(hmac_user);
    if (is_erp_sta == HI_FALSE) {
        /* 如果STA之前没有关联， 或者之前以支持ERP站点身份关联，需要update处理 */
        if ((mac_user->user_asoc_state != MAC_USER_STATE_ASSOC) || (hmac_user->hmac_cap_info.erp == HI_TRUE)) {
            protection->sta_non_erp_num++;
        }

        hmac_user->hmac_cap_info.erp = HI_FALSE;
    } else {
        /* 如果STA之前以不支持ERP身份站点关联，需要update处理 */
        if ((mac_user->user_asoc_state == MAC_USER_STATE_ASSOC) && (hmac_user->hmac_cap_info.erp == HI_FALSE) &&
            (protection->sta_non_erp_num != 0)) {
            protection->sta_non_erp_num--;
        }

        hmac_user->hmac_cap_info.erp = HI_TRUE;
    }

    hmac_user->user_stats_flag.no_erp_stats_flag = HI_TRUE;

    hi_u8 spectrum_mgmt = ((us_cap_info & MAC_CAP_SPECTRUM_MGMT) != MAC_CAP_SPECTRUM_MGMT) ? HI_FALSE : HI_TRUE;
    mac_user_set_spectrum_mgmt(hmac_user->base_user, spectrum_mgmt);
}

#ifdef _PRE_WLAN_FEATURE_UAPSD
/* ****************************************************************************
 功能描述  : 更新ASOC关联实体中的UAPSD信息
 修改历史      :
  1.日    期   : 2013年7月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ap_up_update_asoc_entry_prot(const hi_u8 *puc_mac_hdr, hi_u8 sub_type, hi_u32 msg_len,
    const hmac_user_stru *hmac_user)
{
    /* WMM */
    hmac_uapsd_update_user_para(puc_mac_hdr, sub_type, msg_len, hmac_user);
}
#endif

hi_u32 hmac_sta_ssid_and_sup_rate_proc(const hmac_update_sta_user_info_stru *update_sta_user,
    hmac_user_stru *hmac_user, mac_status_code_enum_uint16 *status_code, mac_vap_stru *mac_vap, hi_u16 *msg_idx)
{
    mac_cfg_ssid_param_stru cfg_ssid;
    hmac_sup_rates_info_stru sup_rates;
    hi_u16 us_rate_len = 0;

#ifdef _PRE_WLAN_FEATURE_MESH
    /* Mesh关连帧不带SSID */
    if ((mac_vap->vap_mode != WLAN_VAP_MODE_MESH) ||
        (mac_get_frame_sub_type(update_sta_user->puc_mac_hdr) != WLAN_FC0_SUBTYPE_ACTION)) {
#endif
        /* 判断SSID,长度或内容不一致时,认为是SSID不一致 */
        if (update_sta_user->puc_payload[*msg_idx] == MAC_EID_SSID) {
            hi_u16 us_ssid_len = 0;

            cfg_ssid.ssid_len = 0;

            hmac_config_get_ssid(mac_vap, &us_ssid_len, (hi_u8 *)(&cfg_ssid));

            if (cfg_ssid.ssid_len != update_sta_user->puc_payload[(*msg_idx) + 1]) {
                *status_code = MAC_UNSPEC_FAIL;
                oam_warning_log1(mac_vap->vap_id, OAM_SF_ASSOC,
                    "{hmac_ap_up_update_sta_user:AP refuse STA assoc,ssid len Err,status_code=%d}", *status_code);
                return HI_FAIL;
            }

            if (memcmp(&(update_sta_user->puc_payload[(*msg_idx) + 2]), cfg_ssid.ac_ssid, /* 2 索引偏置 */
                cfg_ssid.ssid_len) != 0) {
                *status_code = MAC_UNSPEC_FAIL;
                oam_warning_log1(mac_vap->vap_id, OAM_SF_ASSOC,
                    "{hmac_ap_up_update_sta_user::AP refuse STA assoc,ssid mismatch,status_code=%d}", *status_code);
                return HI_FAIL;
            }
        }

        (*msg_idx) += update_sta_user->puc_payload[(*msg_idx) + 1] + MAC_IE_HDR_LEN;
#ifdef _PRE_WLAN_FEATURE_MESH
    }
#endif

    /* 当前用户已关联 */
    sup_rates.msg_len     = update_sta_user->msg_len;
    sup_rates.us_offset   = *msg_idx;
    sup_rates.pus_msg_idx = &us_rate_len;
    hi_u32 rslt = hmac_ap_up_update_sta_sup_rates(update_sta_user->puc_payload, hmac_user, status_code, &sup_rates);
    if (rslt != HI_SUCCESS) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_update_sta_user::AP refuse STA assoc,update rates Err=%d,status_code=%d}", rslt, *status_code);
        return rslt;
    }

    (*msg_idx) += us_rate_len;

    return HI_SUCCESS;
}

hi_u32 hmac_sta_several_param_proc(const hmac_vap_stru *hmac_vap, const hmac_update_sta_user_info_stru *sta_user,
    hmac_user_stru *hmac_user, mac_status_code_enum_uint16 *status_code, const hmac_sta_check_info *sta_check)
{
    const hi_u8   *ie       = HI_NULL;
    hi_u8          sub_type = mac_get_frame_sub_type(sta_user->puc_mac_hdr);
#ifdef _PRE_WLAN_FEATURE_MESH
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) && (sub_type == WLAN_FC0_SUBTYPE_ACTION)) {
        /* 检查接收到的Mesh Peering Open消息中的SECURITY参数.如出错,则返回对应的错误码 */
        hi_u32 sec_rslt = hmac_check_open_action_sec_authenticator(hmac_vap, sta_user->puc_payload, sta_user->msg_len,
            hmac_user, status_code);
        if (sec_rslt != HI_SUCCESS) {
            return HI_FAIL;
        }
    } else {
#endif
        /* 检查接收到的ASOC REQ消息中的SECURITY参数.如出错,则返回对应的错误码 */
        hi_u32 ret = hmac_chk_assoc_req_sec_cap_auth(hmac_vap, sta_user->puc_mac_hdr, sta_user->puc_payload,
            sta_user->msg_len, hmac_user, status_code);
        if (ret != HI_SUCCESS) {
            oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user::Err=%d}", ret);
            return HI_FAIL;
        }
#ifdef _PRE_WLAN_FEATURE_MESH
    }
#endif

    /* 更新对应STA的legacy协议能力 */
    hmac_ap_up_update_legacy_capability(hmac_vap, hmac_user, sta_check->cap);

    /* 检查HT capability是否匹配，并进行处理  */
    hi_u16 ret_val = hmac_vap_check_ht_capabilities_ap(hmac_vap, sta_user->puc_payload, sta_check->msg_idx,
        sta_user->msg_len, hmac_user);
    if (ret_val != MAC_SUCCESSFUL_STATUSCODE) {
        *status_code = ret_val;
        return ret_val;
    }

    /* 更新AP中保护相关mib量 */
    hi_u32 rslt = hmac_user_protection_sync_data(hmac_vap->base_vap);
    if (rslt != HI_SUCCESS) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user::Err=%d}", rslt);
    }

#ifdef _PRE_WLAN_FEATURE_UAPSD
    /* 更新对应STA的协议能力 */
    hmac_ap_up_update_asoc_entry_prot(sta_user->puc_payload, sub_type, sta_user->msg_len, hmac_user);
#endif

    /* 更新QoS能力 */
    hmac_mgmt_update_assoc_user_qos(sta_user->puc_payload, (hi_u16)sta_user->msg_len, sta_check->msg_idx, hmac_user);

    if (sta_user->msg_len > sta_check->offset) {
        /* 查找RSN信息元素,如果没有RSN信息元素,则按照不支持处理 */
        ie = mac_find_ie(MAC_EID_RSN, sta_user->puc_payload + sta_check->offset,
            (sta_user->msg_len - sta_check->offset));
        /* 根据RSN信息元素, 判断RSN能力是否匹配 */
        if (hmac_check_rsn_capability(hmac_vap->base_vap, ie, status_code) != HI_SUCCESS) {
            return HI_FAIL;
        }
#ifdef _PRE_WLAN_FEATURE_PMF
        /* 配置这条连接的pmf能力开关 */
        hmac_set_pmf_cap(hmac_user->base_user, hmac_vap->base_vap, ie);
#endif
    }
    return HI_SUCCESS;
}

hi_u32 hmac_sta_protocol_bandwidth_proc(const hmac_vap_stru *hmac_vap, const hmac_update_sta_user_info_stru *update,
    hmac_user_stru *hmac_user, mac_status_code_enum_uint16 *status_code, hi_u16 msg_idx)
{
    mac_vap_stru  *mac_vap  = hmac_vap->base_vap;
    mac_user_stru *mac_user = hmac_user->base_user;
    wlan_bw_cap_enum_uint8 bandwidth_cap;
    wlan_bw_cap_enum_uint8 bwcap_ap; /* ap自身带宽能力 */

    hi_unref_param(update);
    hi_unref_param(msg_idx);

    /* 获取用户的协议模式 */
    hmac_set_user_protocol_mode(mac_vap, hmac_user);
    if (hmac_get_auc_avail_protocol_mode(mac_vap->protocol, mac_user->protocol_mode) == WLAN_PROTOCOL_BUTT) {
        oam_warning_log3(mac_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_update_sta_user:user not allowed:no valid protocol:vapMode=%d,userMode=%d,userAvailMode=%d}",
            mac_vap->protocol, mac_user->protocol_mode, mac_user->avail_protocol_mode);
        *status_code = MAC_UNSUP_CAP;
        return HI_FAIL;
    }
    /* 获取用户与VAP协议模式交集 */
    mac_user->avail_protocol_mode = hmac_get_auc_avail_protocol_mode(mac_vap->protocol, mac_user->protocol_mode);
    mac_user->cur_protocol_mode   = mac_user->avail_protocol_mode;
    oam_warning_log3(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user:protocol:%d,mode:%d,avail_mode:%d}",
        mac_vap->protocol, mac_user->protocol_mode, mac_user->avail_protocol_mode);
    /* 获取用户和VAP 可支持的11a/b/g 速率交集 */
    hmac_vap_set_user_avail_rates(mac_vap, hmac_user);
    /* 获取用户的带宽能力 */
    mac_user_get_sta_cap_bandwidth(mac_user, &bandwidth_cap);
    /* 获取vap带宽能力与用户带宽能力的交集 */
    mac_vap_get_bandwidth_cap(hmac_vap->base_vap, &bwcap_ap);
    mac_user_set_bandwidth_info(mac_user, oal_min(bwcap_ap, bandwidth_cap), oal_min(bwcap_ap, bandwidth_cap));

    oam_warning_log3(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user::bwcap_ap:%d,bw_cap:%d,curbw:%d.}",
        bwcap_ap, bandwidth_cap, mac_user->cur_bandwidth);

    hi_u32 rslt = hmac_config_user_cap_syn(mac_vap, mac_user);
    if (rslt != HI_SUCCESS) {
        oam_error_log1(mac_user->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user::config_usr_cap_syn Err=%d}", rslt);
    }
    /* 根据用户支持带宽能力，协商出当前带宽，dmac offload架构下，同步带宽信息到device */
    rslt = hmac_config_user_info_syn(mac_vap, mac_user);
    if (rslt != HI_SUCCESS) {
        oam_error_log1(mac_user->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user::usr_info_syn failed[%d].}", rslt);
    }
    /* 获取用户与VAP空间流交集 */
    rslt = hmac_user_set_avail_num_space_stream(mac_user, WLAN_SINGLE_NSS);
    if (rslt != HI_SUCCESS) {
        *status_code = MAC_UNSPEC_FAIL;
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user:set_avail_num_spaceErr=%d}", rslt);
    }
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    /* 处理Operating Mode Notification 信息元素 */
    rslt = hmac_check_opmode_notify(hmac_vap, update_sta_user->puc_mac_hdr, update_sta_user->puc_payload, msg_idx,
        update_sta_user->msg_len, hmac_user);
    if (rslt != HI_SUCCESS) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user:check Err%d}", rslt);
    }
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : AP侧更新STA信息
  1.日    期   : 2013年7月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2013年8月27日
    作    者   : HiSilicon
    修改内容   : 修改函数，添加安全相关的信息
**************************************************************************** */
static hi_u32 hmac_ap_up_update_sta_user(const hmac_vap_stru *hmac_vap, const hmac_update_sta_user_info_stru *update,
    hmac_user_stru *hmac_user, mac_status_code_enum_uint16 *status_code)
{
    hi_u16 msg_idx = 0;
    hi_u16 offset = MAC_CAP_INFO_LEN + MAC_LISTEN_INT_LEN;
    mac_vap_stru *mac_vap = hmac_vap->base_vap;

    *status_code = MAC_SUCCESSFUL_STATUSCODE;

#ifdef _PRE_WLAN_FEATURE_MESH
    if (mac_get_frame_sub_type(update->puc_mac_hdr) == WLAN_FC0_SUBTYPE_ACTION) {
        /* Mesh关联采用Self_protected Action帧，帧结构不同 */
        offset = MAC_ACTION_CATEGORY_LEN + MAC_ACTION_CODE_LEN + MAC_CAP_INFO_LEN;
        msg_idx += MAC_ACTION_CATEGORY_LEN + MAC_ACTION_CODE_LEN;
    }
#endif

    /* **************************************************************************
        检查AP是否支持当前正在关联的STA的所有能力
        |ESS|IBSS|CFPollable|CFPReq|Privacy|Preamble|PBCC|Agility|Reserved|
    ************************************************************************** */
    hi_u16 cap = update->puc_payload[msg_idx] | (update->puc_payload[(msg_idx + 1)] << 8); /* 8 */

    if (hmac_ap_up_update_sta_cap_info(hmac_vap, cap, hmac_user, status_code) != HI_TRUE) {
        return HI_FAIL;
    }
    msg_idx += MAC_CAP_INFO_LEN;

#ifdef _PRE_WLAN_FEATURE_MESH
    msg_idx += (mac_get_frame_sub_type(update->puc_mac_hdr) != WLAN_FC0_SUBTYPE_ACTION) ? MAC_LIS_INTERVAL_IE_LEN : 0;
    if ((mac_get_frame_sub_type(update->puc_mac_hdr) == WLAN_FC0_SUBTYPE_ACTION) &&
        (mac_get_action_category(update->puc_payload) == MAC_ACTION_CATEGORY_SELF_PROTECTED) &&
        (mac_get_action_code(update->puc_payload) == MAC_SP_ACTION_MESH_PEERING_CONFIRM)) {
        /* Mesh Peering Confirm帧中Cap后为AID */
        msg_idx += MAC_AID_LEN;
        offset += MAC_AID_LEN;
    }
#else
    msg_idx += MAC_LIS_INTERVAL_IE_LEN;
#endif

    if (mac_get_frame_sub_type(update->puc_mac_hdr) == WLAN_FC0_SUBTYPE_REASSOC_REQ) {
        /* 重关联比关联请求帧头多了AP的MAC地址  */
        msg_idx += WLAN_MAC_ADDR_LEN;
        offset += WLAN_MAC_ADDR_LEN;
    }

    hi_u32 rslt = hmac_sta_ssid_and_sup_rate_proc(update, hmac_user, status_code, mac_vap, &msg_idx);
    if (rslt != HI_SUCCESS) {
        return rslt;
    }
    hmac_sta_check_info sta_check = { cap, msg_idx, offset, { 0, 0 } };
    rslt = hmac_sta_several_param_proc(hmac_vap, update, hmac_user, status_code, &sta_check);
    if (rslt != HI_SUCCESS) {
        return rslt;
    }

    rslt = hmac_sta_protocol_bandwidth_proc(hmac_vap, update, hmac_user, status_code, msg_idx);
    return rslt;
}

/* ****************************************************************************
 功能描述  : AP 保存STA 的关联请求帧信息，以备上报内核
 输入参数  : [1]hmac_user
             [2]puc_payload
             [3]payload_len
             [4]mgmt_frm_type
 返 回 值  : static hi_u32
**************************************************************************** */
static hi_u32 hmac_ap_prepare_assoc_req(hmac_user_stru *hmac_user, const hi_u8 *puc_payload, hi_u32 payload_len,
    hi_u8 mgmt_frm_type)
{
    hi_u32 payload_size;

    /* AP 保存STA 的关联请求帧信息，以备上报内核 */
    if (hmac_user->puc_assoc_req_ie_buff != HI_NULL) {
        oal_mem_free(hmac_user->puc_assoc_req_ie_buff);
        hmac_user->puc_assoc_req_ie_buff = HI_NULL;
        hmac_user->assoc_req_ie_len = 0;
    }
    payload_size = payload_len;

    /* 目前11r没有实现，所以处理重关联帧的流程和关联帧一样，11r实现后此处需要修改 */
    if (WLAN_FC0_SUBTYPE_REASSOC_REQ == mgmt_frm_type) {
        /* 重关联比关联请求帧头多了AP的MAC地址  */
        puc_payload += (WLAN_MAC_ADDR_LEN + MAC_CAP_INFO_LEN + MAC_LIS_INTERVAL_IE_LEN);
        if (payload_len < (WLAN_MAC_ADDR_LEN + MAC_CAP_INFO_LEN + MAC_LIS_INTERVAL_IE_LEN)) {
            oam_warning_log0(0, OAM_SF_CFG, "hmac_ap_prepare_assoc_req:: reassoc frame err.");
            return HI_FAIL;
        }
        payload_len -= (WLAN_MAC_ADDR_LEN + MAC_CAP_INFO_LEN + MAC_LIS_INTERVAL_IE_LEN);
    } else {
        if (payload_len < (MAC_CAP_INFO_LEN + MAC_LIS_INTERVAL_IE_LEN)) {
            oam_warning_log0(0, OAM_SF_CFG, "hmac_ap_prepare_assoc_req:: resoc frame err.");
            return HI_FAIL;
        }
        puc_payload += (MAC_CAP_INFO_LEN + MAC_LIS_INTERVAL_IE_LEN);
        payload_len -= (MAC_CAP_INFO_LEN + MAC_LIS_INTERVAL_IE_LEN);
    }

    hmac_user->puc_assoc_req_ie_buff = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, (hi_u16)payload_size);
    if (hmac_user->puc_assoc_req_ie_buff == HI_NULL) {
        oam_error_log0(hmac_user->base_user->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_prepare_assoc_req::Alloc Assoc Req for kernel failed.}");
        hmac_user->assoc_req_ie_len = 0;
        return HI_FAIL;
    } else {
        if (memcpy_s(hmac_user->puc_assoc_req_ie_buff, (hi_u16)payload_size, puc_payload, payload_len) != EOK) {
            oal_mem_free(hmac_user->puc_assoc_req_ie_buff);
            oam_error_log0(0, OAM_SF_CFG, "hmac_ap_prepare_assoc_req:: puc_payload memcpy_s fail.");
            return HI_FAIL;
        }
        hmac_user->assoc_req_ie_len = payload_len;
        return HI_SUCCESS;
    }
}

static hi_u32 hmac_ap_get_mac_and_user(const hmac_vap_stru *hmac_vap, const hi_u8 *puc_mac_hdr, hi_u8 *sta_mac_addr,
    hmac_user_stru **hmac_user, hi_u8 *user_idx)
{
    hi_u32 rslt;

    mac_get_address2(puc_mac_hdr, WLAN_MAC_ADDR_LEN, sta_mac_addr, WLAN_MAC_ADDR_LEN);

    rslt = mac_vap_find_user_by_macaddr(hmac_vap->base_vap, sta_mac_addr, WLAN_MAC_ADDR_LEN, user_idx);
    if (rslt != HI_SUCCESS) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_asoc_req::mac_vap_find_user_by_macaddr failed[%d].}", rslt);
        oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_asoc_req::user mac:%XX:XX:XX:%02X:%02X:%02X.}", sta_mac_addr[3], sta_mac_addr[4],
            sta_mac_addr[5]); /* 3 4 5 元素索引 */
        hmac_mgmt_send_deauth_frame(hmac_vap->base_vap, sta_mac_addr, WLAN_MAC_ADDR_LEN, MAC_ASOC_NOT_AUTH);

        return rslt;
    }

    oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_get_mac_and_user::uc_user_idx[%d].}",
        *user_idx);

    *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(*user_idx);
    if ((*hmac_user == HI_NULL) || ((*hmac_user)->base_user == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{hmac_ap_up_rx_asoc_req::pst_hmac_user null.}");
        /* 没有查到对应的USER,发送去认证消息 */
        hmac_mgmt_send_deauth_frame(hmac_vap->base_vap, sta_mac_addr, WLAN_MAC_ADDR_LEN, MAC_ASOC_NOT_AUTH);
        return HI_ERR_CODE_PTR_NULL;
    }
#ifdef _PRE_WLAN_FEATURE_MESH
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) && ((*hmac_user)->base_user->is_mesh_user == HI_TRUE)) {
        /* 不应该收到Mesh User发来的Assoc Req */
        oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_asoc_req::[MESH] receive assoc req from mesh user, user mac:XX:XX:XX:%02X:%02X:%02X.}",
            sta_mac_addr[3], sta_mac_addr[4], sta_mac_addr[5]); /* 3 4 5 元素索引 */
        return HI_SUCCESS;
    }
#endif
    return HI_CONTINUE;
}

static hi_u32 hmac_ap_asoc_pmf_process(const hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user,
    mac_status_code_enum_uint16 *status_code)
{
    /* 是否符合触发SA query流程的条件 */
#ifdef _PRE_WLAN_FEATURE_PMF
    if ((hmac_user->sa_query_info.sa_query_interval_timer.is_enabled != HI_TRUE) &&
        (hmac_user->base_user->user_asoc_state == MAC_USER_STATE_ASSOC) &&
        (hmac_user->base_user->cap_info.pmf_active == HI_TRUE)) {
        oam_info_log0(hmac_user->base_user->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_asoc_req::AP rx STA assoc req ,and start sa query process.}");
        hi_u32 rslt = hmac_start_sa_query(hmac_vap->base_vap, hmac_user, hmac_user->base_user->cap_info.pmf_active);
        if (rslt != HI_SUCCESS) {
            oam_error_log1(hmac_user->base_user->vap_id, OAM_SF_ASSOC,
                "{hmac_ap_up_rx_asoc_req::hmac_start_sa_query failed[%d].}", rslt);
            return rslt;
        }
        oam_info_log0(hmac_user->base_user->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_asoc_req::set en_status_code is MAC_REJECT_TEMP.}");
        *status_code = MAC_REJECT_TEMP;
    }
#endif
    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_P2P
static hi_void hmac_ap_asoc_p2p_process(const hmac_vap_stru *hmac_vap, hi_u8 mgmt_frm_type, hi_u32 payload_len,
    hi_u8 *puc_payload, const hmac_user_stru *hmac_user)
{
        hi_s32 l_len = ((mgmt_frm_type == WLAN_FC0_SUBTYPE_REASSOC_REQ) ? (MAC_CAP_INFO_LEN + MAC_LISTEN_INT_LEN +
                                               WLAN_MAC_ADDR_LEN) : (MAC_CAP_INFO_LEN + MAC_LISTEN_INT_LEN));
        if (is_p2p_go(hmac_vap->base_vap) &&
            (HI_NULL == mac_find_vendor_ie(MAC_WLAN_OUI_WFA, MAC_WLAN_OUI_TYPE_WFA_P2P, puc_payload + l_len,
                (hi_s32) payload_len - l_len))) {
            oam_info_log1(hmac_user->base_user->vap_id, OAM_SF_ASSOC,
                          "{hmac_ap_up_rx_asoc_req::GO got assoc request from legacy device, length = [%d]}",
                          payload_len);
            hmac_p2p_disable_pm(hmac_vap);
        }
}
#endif

static hi_u32 hmac_ap_asoc_req_process(hmac_ap_asoc_req_stru asoc_req, hmac_ap_asoc_info_stru asoc_info)
{
    hi_u32 rslt;
    hmac_update_sta_user_info_stru update_sta_user_info = {
        .puc_mac_hdr = asoc_req.puc_mac_hdr,
        .puc_payload = asoc_req.puc_payload,
        .msg_len = asoc_req.payload_len
    };

    rslt = hmac_ap_asoc_pmf_process(asoc_req.hmac_vap, asoc_info.hmac_user, asoc_info.status_code);
    if (rslt != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ASSOC, "hmac_ap_asoc_pmf_process fail");
        return rslt;
    }

    if (*(asoc_info.status_code) != MAC_REJECT_TEMP) {
        /* 当可以查找到用户时,说明当前USER的状态为已关联或已认证完成 */
        rslt = hmac_ap_up_update_sta_user(asoc_req.hmac_vap, &update_sta_user_info, asoc_info.hmac_user,
            asoc_info.status_code);
        if ((rslt != HI_SUCCESS) || (*(asoc_info.status_code) != MAC_SUCCESSFUL_STATUSCODE)) {
            oam_warning_log1(0, OAM_SF_ASSOC, "{hmac_ap_up_update_sta_user failed[%d].}", *(asoc_info.status_code));
            hmac_user_set_asoc_state(asoc_req.hmac_vap->base_vap, asoc_info.hmac_user->base_user,
                MAC_USER_STATE_AUTH_COMPLETE);
        }
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        /* 同步ap带宽，能力等信息到dmac */
        hmac_chan_sync(asoc_req.hmac_vap->base_vap, asoc_req.hmac_vap->base_vap->channel.chan_number,
            asoc_req.hmac_vap->base_vap->channel.en_bandwidth, HI_FALSE);
#endif

        /* 根据用户支持带宽能力，协商出当前带宽，dmac offload架构下，同步带宽信息到device */
        rslt = hmac_config_user_info_syn(asoc_req.hmac_vap->base_vap, asoc_info.hmac_user->base_user);
        if (rslt != HI_SUCCESS) {
            oam_error_log1(0, OAM_SF_ASSOC, "{hmac_ap_up_rx_asoc_req::usr_info_syn failed[%d].}", rslt);
        }

        if (*(asoc_info.status_code) == MAC_SUCCESSFUL_STATUSCODE) {
            rslt = hmac_init_security(asoc_req.hmac_vap->base_vap, asoc_info.sta_mac_addr, WLAN_MAC_ADDR_LEN);
            if (rslt != HI_SUCCESS) {
                oam_error_log2(0, 0, "{hmac_init_security failed[%d] status_code[%d].}", rslt, MAC_UNSPEC_FAIL);
                *(asoc_info.status_code) = MAC_UNSPEC_FAIL;
            }
#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
            rslt = hmac_init_user_security_port(asoc_req.hmac_vap->base_vap, asoc_info.hmac_user->base_user);
            if (rslt != HI_SUCCESS) {
                oam_error_log1(0, OAM_SF_ASSOC, "{hmac_init_user_security_port failed[%d].}", rslt);
            }
#endif /* defined(_PRE_WLAN_FEATURE_WPA) ||　defined(_PRE_WLAN_FEATURE_WPA2) */
        }

        if ((rslt != HI_SUCCESS) || (*(asoc_info.status_code) != MAC_SUCCESSFUL_STATUSCODE)) {
            oam_warning_log2(0, OAM_SF_CFG, "hmac_ap_up_update_sta_user fail rslt[%d] status_code[%d].", rslt,
                *(asoc_info.status_code));
            hmac_user_set_asoc_state(asoc_req.hmac_vap->base_vap, asoc_info.hmac_user->base_user,
                MAC_USER_STATE_AUTH_COMPLETE);
        }
#ifdef _PRE_WLAN_FEATURE_P2P
        hmac_ap_asoc_p2p_process(asoc_req.hmac_vap, asoc_req.mgmt_frm_type, asoc_req.payload_len,
            asoc_req.puc_payload, asoc_info.hmac_user);
#endif
    }

    return HI_SUCCESS;
}

static hi_u32 hmac_ap_prepare_asoc_rsp(hmac_ap_asoc_req_stru asoc_req, hmac_ap_asoc_info_stru asoc_info,
    oal_netbuf_stru **asoc_rsp, hi_u32 *asoc_rsp_len)
{
    hmac_asoc_rsp_ap_info_stru asoc_rsp_ap_info;
    hmac_tx_ctl_stru *tx_ctl = HI_NULL;

    *asoc_rsp = (oal_netbuf_stru *)oal_netbuf_alloc(WLAN_MGMT_NETBUF_SIZE, 0, 4); /* align 4 */
    if (*asoc_rsp == HI_NULL) {
        oam_error_log0(asoc_info.hmac_user->base_user->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_asoc_req::pst_asoc_rsp null.}");
        /* 异常返回之前删除user */
        hmac_user_del(asoc_req.hmac_vap->base_vap, asoc_info.hmac_user);
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }
    tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(*asoc_rsp);
    if (memset_s(tx_ctl, oal_netbuf_cb_size(), 0, sizeof(hmac_tx_ctl_stru)) != EOK) {
        oal_netbuf_free(*asoc_rsp);
        return HI_FAIL;
    }

    asoc_rsp_ap_info.puc_sta_addr = asoc_info.sta_mac_addr;
    asoc_rsp_ap_info.assoc_id = asoc_info.hmac_user->base_user->us_assoc_id;
    asoc_rsp_ap_info.status_code = *(asoc_info.status_code);
    asoc_rsp_ap_info.puc_asoc_rsp = oal_netbuf_header(*asoc_rsp);
    asoc_rsp_ap_info.us_type = (asoc_req.mgmt_frm_type == WLAN_FC0_SUBTYPE_ASSOC_REQ) ?
                                WLAN_FC0_SUBTYPE_ASSOC_RSP : WLAN_FC0_SUBTYPE_REASSOC_RSP;
    *asoc_rsp_len = hmac_mgmt_encap_asoc_rsp_ap(asoc_req.hmac_vap->base_vap, &asoc_rsp_ap_info);
    if (*asoc_rsp_len == 0) {
        oam_warning_log0(asoc_info.hmac_user->base_user->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_asoc_req::hmac_mgmt_encap_asoc_rsp_ap encap msg fail.}");
        oal_netbuf_free(*asoc_rsp);
        /* 异常返回之前删除user */
        hmac_user_del(asoc_req.hmac_vap->base_vap, asoc_info.hmac_user);
        return HI_FAIL;
    }

    oal_netbuf_put(*asoc_rsp, *asoc_rsp_len);

    tx_ctl->us_tx_user_idx = asoc_info.hmac_user->base_user->us_assoc_id;
    tx_ctl->us_mpdu_len = (hi_u16)(*asoc_rsp_len);
    tx_ctl->frame_header_length = MAC_80211_FRAME_LEN;
    tx_ctl->frame_header = (mac_ieee80211_frame_stru *)oal_netbuf_header(*asoc_rsp);
    tx_ctl->mac_head_type = 1;

    return HI_SUCCESS;
}

static hi_u32 hmac_ap_tx_asoc_rsp(const hmac_vap_stru *hmac_vap, hi_u8 mgmt_frm_type, hmac_ap_asoc_info_stru asoc_info)
{
    hi_u32                      rslt;
    oal_net_device_stru         *netdev = HI_NULL;
    oal_netbuf_stru             *asoc_rsp   = HI_NULL;
    hi_u32                      asoc_rsp_len;
    hmac_tx_ctl_stru            *tx_ctl     = HI_NULL;
    hmac_ap_asoc_req_stru       asoc_req = {
        .hmac_vap = hmac_vap,
        .mgmt_frm_type = mgmt_frm_type
    };

    rslt = hmac_ap_prepare_asoc_rsp(asoc_req, asoc_info, &asoc_rsp, &asoc_rsp_len);
    if (rslt != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ASSOC, "hmac_ap_prepare_asoc_rsp fail");
        return rslt;
    }

    /* 判断当前状态，如果用户已经关联成功则向上报用户离开信息 */
    if (asoc_info.hmac_user->base_user->user_asoc_state == MAC_USER_STATE_ASSOC) {
        netdev = hmac_vap_get_net_device(hmac_vap->base_vap->vap_id);
        if (netdev != HI_NULL) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
            oal_kobject_uevent_env_sta_leave(netdev, asoc_info.sta_mac_addr);
#endif
        }
    }

    if (*(asoc_info.status_code) == MAC_SUCCESSFUL_STATUSCODE) {
        hmac_user_set_asoc_state(hmac_vap->base_vap, asoc_info.hmac_user->base_user, MAC_USER_STATE_ASSOC);
    }

    /* 发送关联响应帧之前，将用户的节能状态复位 */
    tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(asoc_rsp);
    hmac_mgmt_reset_psm(hmac_vap->base_vap, tx_ctl->us_tx_user_idx);

    rslt = hmac_tx_mgmt_send_event(hmac_vap->base_vap, asoc_rsp, (hi_u16)asoc_rsp_len);
    if (rslt != HI_SUCCESS) {
        oal_netbuf_free(asoc_rsp);
        oam_warning_log1(asoc_info.hmac_user->base_user->vap_id, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_asoc_req::hmac_tx_mgmt_send_event failed[%d].}", rslt);
        /* 异常返回之前删除user */
        hmac_user_del(hmac_vap->base_vap, asoc_info.hmac_user);
        return rslt;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : AP侧接收到对应STA的关联请求消息
 修改历史      :
  1.日    期   : 2013年7月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ap_up_rx_asoc_req(const hmac_vap_stru *hmac_vap, hi_u8 mgmt_frm_type,
    hi_u8 *puc_mac_hdr, hi_u8 *puc_payload, hi_u32 payload_len)
{
    hmac_user_stru              *hmac_user  = HI_NULL;
    hi_u8                       user_idx     = 0;
    mac_status_code_enum_uint16 status_code = MAC_SUCCESSFUL_STATUSCODE;
    hi_u8                       sta_mac_addr[WLAN_MAC_ADDR_LEN] = { 0 };
    hi_u8                       *sta_mac_addr_ptr = sta_mac_addr;
    hmac_ap_asoc_info_stru      asoc_info = {.sta_mac_addr = sta_mac_addr, .status_code = &status_code};
    hmac_ap_asoc_req_stru       asoc_req = { .hmac_vap = hmac_vap, .mgmt_frm_type = mgmt_frm_type,
        .puc_mac_hdr = puc_mac_hdr, .puc_payload = puc_payload, .payload_len =payload_len };

    hi_u32 rslt = hmac_ap_get_mac_and_user(hmac_vap, puc_mac_hdr, sta_mac_addr_ptr, &hmac_user, &user_idx);
    if (rslt != HI_CONTINUE) {
        oam_error_log0(0, OAM_SF_ASSOC, "hmac_ap_get_mac_and_user fail");
        return rslt;
    }
    asoc_info.hmac_user = hmac_user;

    rslt = hmac_ap_asoc_req_process(asoc_req, asoc_info);
    if (rslt != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ASSOC, "hmac_ap_asoc_req_process fail");
        return rslt;
    }

    rslt = hmac_ap_tx_asoc_rsp(hmac_vap, mgmt_frm_type, asoc_info);
    if (rslt != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ASSOC, "hmac_ap_tx_asoc_rsp fail");
        return rslt;
    }
    hi_u8 vap_id = hmac_user->base_user->vap_id;
    hi_unref_param(vap_id);
    if (status_code == MAC_SUCCESSFUL_STATUSCODE) {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        rslt = hmac_config_user_rate_info_syn(hmac_vap->base_vap, hmac_user->base_user);
        if (rslt != HI_SUCCESS) {
            oam_error_log1(vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc_rx::user_rate_info_syn failed[%d].}", rslt);
        }
#endif
        /*  user已经关联上，抛事件给DMAC，在DMAC层挂用户算法钩子 */
        hmac_user_add_notify_alg(hmac_vap->base_vap, user_idx);

        /* AP 保存STA 的关联请求帧信息，以备上报内核 */
        hmac_ap_prepare_assoc_req(hmac_user, puc_payload, payload_len, mgmt_frm_type);

        /* 上报WAL层(WAL上报内核) AP关联上了一个新的STA */
        hmac_handle_connect_rsp_ap(hmac_vap, hmac_user);
        oam_warning_log1(vap_id, OAM_SF_ASSOC, "{hmac_ap_up_rx_asoc_req::STA assoc success! idx=%d.}", user_idx);
    } else {
        /* AP检测STA失败，将其删除 */
        if (status_code != MAC_REJECT_TEMP) {
            hmac_user_del(hmac_vap->base_vap, hmac_user);
        }
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : AP侧接收到对应STA的去关联请求消息
 修改历史      :
  1.日    期   : 2013年7月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ap_up_rx_disasoc(const hmac_vap_stru *hmac_vap, hi_u8 *puc_mac_hdr, hi_u8 is_protected)
{
    hi_u8 *da_mac_addr = HI_NULL;
    hi_u8 *sa_mac_addr = HI_NULL;
    hi_u8  sta_mac_addr[WLAN_MAC_ADDR_LEN];

    mac_get_address2(puc_mac_hdr, WLAN_MAC_ADDR_LEN, sta_mac_addr, WLAN_MAC_ADDR_LEN);

    mac_rx_get_sa((mac_ieee80211_frame_stru *)puc_mac_hdr, &sa_mac_addr);

    /* 增加接收到去关联帧时的维测信息 */
    oam_warning_log4(0, OAM_SF_ASSOC,
        "{hmac_ap_up_rx_disasoc::Because of err_code[%d], disassoc frame from source addr xx:xx:xx:%02x:%02x:%02x.}",
        *((hi_u16 *)(puc_mac_hdr + MAC_80211_FRAME_LEN)), sa_mac_addr[3], sa_mac_addr[4], sa_mac_addr[5]); /* 3 4 5 */

    hmac_user_stru *hmac_user = mac_vap_get_hmac_user_by_addr(hmac_vap->base_vap, sta_mac_addr, WLAN_MAC_ADDR_LEN);
    if (hmac_user == HI_NULL) {
        oam_warning_log0(0, OAM_SF_ASSOC, "{hmac_ap_up_rx_disasoc::pst_hmac_user null.}");
        /* 没有查到对应的USER,发送去认证消息 */
        hmac_mgmt_send_deauth_frame(hmac_vap->base_vap, sta_mac_addr, WLAN_MAC_ADDR_LEN, MAC_NOT_ASSOCED);

        return HI_ERR_CODE_PTR_NULL;
    }
#ifdef _PRE_WLAN_FEATURE_MESH
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) && (hmac_user->base_user->is_mesh_user == HI_TRUE)) {
        /* 不应该收到Mesh User发来的DisAssoc Req */
        oam_warning_log3(0, OAM_SF_ASSOC,
            "{hmac_ap_up_rx_disasoc::[MESH] receive disassoc req from mesh user, user mac:XX:XX:XX:%02X:%02X:%02X.}",
            sta_mac_addr[3], sta_mac_addr[4], sta_mac_addr[5]); /* 3 4 5 元素索引 */

        return HI_SUCCESS;
    }
#endif
    if (hmac_user->base_user->user_asoc_state == MAC_USER_STATE_ASSOC) {
#ifdef _PRE_WLAN_FEATURE_PMF
        /* 检查是否需要发送SA query request */
        if (HI_SUCCESS == hmac_pmf_check_err_code(hmac_user->base_user, is_protected, puc_mac_hdr)) {
            /* 在关联状态下收到未加密的ReasonCode 6/7需要启动SA Query流程 */
            hi_u32 ret = hmac_start_sa_query(hmac_vap->base_vap, hmac_user, hmac_user->base_user->cap_info.pmf_active);
            if (ret != HI_SUCCESS) {
                oam_error_log1(0, OAM_SF_ASSOC, "{hmac_ap_up_rx_disasoc::hmac_start_sa_query Err=%d}", ret);
                return HI_ERR_CODE_PMF_SA_QUERY_START_FAIL;
            }
            return HI_SUCCESS;
        }
#endif

        /* 如果该用户的管理帧加密属性不一致，丢弃该报文 */
        mac_rx_get_da((mac_ieee80211_frame_stru *)puc_mac_hdr, &da_mac_addr);
        if (!ether_is_multicast(da_mac_addr) && (is_protected != hmac_user->base_user->cap_info.pmf_active)) {
            oam_error_log1(0, OAM_SF_ASSOC, "{hmac_ap_up_rx_disasoc::PMF check fail,is_protected=%d}", is_protected);
            return HI_FAIL;
        }

        mac_user_set_asoc_state(hmac_user->base_user, MAC_USER_STATE_AUTH_COMPLETE);
        /* 抛事件上报内核，已经去关联某个STA */
        hmac_handle_disconnect_rsp_ap(hmac_vap, hmac_user);

        /* 有些网卡去关联时只发送DISASOC,也将删除其在AP内部的数据结构 */
        hi_u32 hmac_user_del_ret = hmac_user_del(hmac_vap->base_vap, hmac_user);
        if (hmac_user_del_ret != HI_SUCCESS) {
            oam_warning_log1(0, OAM_SF_ASSOC, "{hmac_ap_up_rx_disasoc::hmac_user_del err=%d}", hmac_user_del_ret);
        }
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MESH
static hi_u32 hmac_ap_up_get_mesh_hmac_user(const hmac_vap_stru *hmac_vap, const hi_u8 *puc_data)
{
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) &&
        (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_SELF_PROTECTED) &&
        (puc_data[MAC_ACTION_OFFSET_ACTION] == MAC_SP_ACTION_MESH_PEERING_OPEN)) {
        /* Mesh Peering Open收到时可能无用户，统一由wpa添加用户，该处直接上报 */
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX,
            "{hmac_ap_up_get_mesh_hmac_user::rx open frame but no user.}");
        return HI_SUCCESS;
    } else {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_ap_up_get_mesh_hmac_user::find_user failed.}");
        return HI_FAIL;
    }
}
#endif

static hi_void hmac_ap_up_rx_action_category_public(const hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf,
    const hi_u8 *puc_data)
{
    switch (puc_data[MAC_ACTION_OFFSET_ACTION]) {
        case MAC_PUB_VENDOR_SPECIFIC: {
#ifdef _PRE_WLAN_FEATURE_P2P
            /* 查找OUI-OUI type值为 50 6F 9A - 09 (WFA P2P v1.0)  */
            /* 并用hmac_rx_mgmt_send_to_host接口上报 */
            if (HI_TRUE == mac_ie_check_p2p_action(puc_data + MAC_ACTION_OFFSET_ACTION)) {
                hmac_rx_mgmt_send_to_host(hmac_vap, netbuf);
            }
#else
            hi_unref_param(hmac_vap);
            hi_unref_param(netbuf);
#endif
            break;
        }
        default:
            break;
    }
}

static hi_void hmac_ap_up_rx_action_category_vht(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf,
    const hi_u8 *puc_data)
{
    switch (puc_data[MAC_ACTION_OFFSET_ACTION]) {
        case MAC_VHT_ACTION_OPREATE_MODE_NOTIFY:
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
            hmac_mgmt_rx_opmode_notify_frame(hmac_vap, netbuf);
            break;
#else
            hi_unref_param(hmac_vap);
            hi_unref_param(netbuf);
            break;
#endif
        case MAC_VHT_ACTION_BUTT:
            break;
        default:
            break;
    }
}

#ifdef _PRE_WLAN_FEATURE_PMF
static hi_void hmac_ap_up_rx_action_category_sa_query(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf,
    const hi_u8 *puc_data, hi_u8 is_protected)
{
    switch (puc_data[MAC_ACTION_OFFSET_ACTION]) {
        case MAC_SA_QUERY_ACTION_RESPONSE:
            hmac_rx_sa_query_rsp(hmac_vap, netbuf, is_protected);
            break;
        case MAC_SA_QUERY_ACTION_REQUEST:
            hmac_rx_sa_query_req(hmac_vap, netbuf, is_protected);
            break;
        default:
            break;
    }
}
#endif

#ifdef _PRE_WLAN_FEATURE_MESH
static hi_void hmac_ap_up_rx_action_self_protected(const hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user,
    mac_ieee80211_frame_stru *frame_hdr, oal_netbuf_stru *netbuf, hi_u8 *puc_data)
{
    hi_u32 ret;
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    switch (puc_data[MAC_ACTION_OFFSET_ACTION]) {
        case MAC_SP_ACTION_MESH_PEERING_CONFIRM:
            if (hmac_user == HI_NULL) {
                return;
            }
            /* 上报wpa_supplicant之前，预先处理与驱动User相关的速率/RSN等信息 */
            ret = hmac_mesh_up_rx_confirm_action(hmac_vap, hmac_user, (hi_u8 *)frame_hdr, puc_data,
                rx_ctrl->us_frame_len - rx_ctrl->mac_header_len);
            if (ret != HI_SUCCESS) {
                hmac_handle_close_peer_mesh(hmac_vap, hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN,
                    (hi_u16)ret, MAC_UNSPEC_REASON);
                break;
            }
            /* fall-through */
        case MAC_SP_ACTION_MESH_PEERING_OPEN:
            /* confirm消息内容与open消息关于user信息的部分一致 */
        case MAC_SP_ACTION_MESH_PEERING_CLOSE:
        case MAC_SP_ACTION_MESH_GROUP_KEY_INFORM:
        case MAC_SP_ACTION_MESH_GROUP_KEY_ACK:
            if ((hmac_user != HI_NULL) && (hmac_user->base_user->user_asoc_state != MAC_USER_STATE_ASSOC) &&
                (hmac_user->base_user->is_mesh_user == HI_TRUE)) {
                /* 更新用户的RSSI统计信息 */
                hmac_user->base_user->rx_conn_rssi =
                    wlan_rssi_lpf(hmac_user->base_user->rx_conn_rssi, rx_ctrl->rssi_dbm);
            }
            /* 收到SELF_PROTECTED类型ACTION帧发送到WPA_SUPPLICANT处理 */
            hmac_rx_mgmt_send_to_host(hmac_vap, netbuf);
            break;
        default:
            oam_warning_log0(hmac_user->base_user->vap_id, OAM_SF_ASSOC,
                "{hmac_ap_up_rx_action_self_protected::unsupported self protected action subtype.}");
            break;
    }
}

static hi_void hmac_ap_up_rx_action_category_mesh(const hi_u8 *puc_data)
{
    switch (puc_data[MAC_ACTION_OFFSET_ACTION]) {
        case MAC_MESH_ACTION_TBTT_ADJ_REQ:
        case MAC_MESH_ACTION_TBTT_ADJ_RSP:
            oam_info_log0(0, OAM_SF_ASSOC, "{hmac_ap_up_rx_action_category_mesh::[MESH]rx tbtt adj req/rsp.}");
            break;
        default:
            oam_warning_log0(0, OAM_SF_ASSOC,
                "{hmac_ap_up_rx_action_category_mesh::unsupported mesh action subtype.}");
            break;
    }
}
#endif

/* ****************************************************************************
 功能描述  : AP在UP状态下的接收ACTION帧处理
 输入参数  : pst_hmac_vap: HMAC VAP指针
             pst_netbuf  : 管理帧所在的sk_buff
 修改历史      :
  1.日    期   : 2014年2月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ap_up_rx_action(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf, hi_u8 is_protected)
{
    hmac_user_stru *hmac_user = HI_NULL;
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    /* 获取帧头信息 */
    mac_ieee80211_frame_stru *frame_hdr = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;
    /* 获取帧体指针 */
    hi_u8 *puc_data = (hi_u8 *)rx_ctrl->pul_mac_hdr_start_addr + rx_ctrl->mac_header_len;
    /* 获取发送端的用户指针 */
    hmac_user = mac_vap_get_hmac_user_by_addr(hmac_vap->base_vap, frame_hdr->auc_address2, WLAN_MAC_ADDR_LEN);
    if (hmac_user == HI_NULL) {
#ifdef _PRE_WLAN_FEATURE_MESH
        if (hmac_ap_up_get_mesh_hmac_user(hmac_vap, puc_data) != HI_SUCCESS) {
            return;
        }
#else
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_ap_up_rx_action::find_user_by_macaddr failed.}");
        return;
#endif
    }

    /* Category */
    if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_BA) {
        hmac_mgmt_rx_action_ba(hmac_vap, hmac_user, puc_data);
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_PUBLIC) {
        hmac_ap_up_rx_action_category_public(hmac_vap, netbuf, puc_data);
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_VHT) {
        hmac_ap_up_rx_action_category_vht(hmac_vap, netbuf, puc_data);
#ifdef _PRE_WLAN_FEATURE_PMF
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_SA_QUERY) {
        hmac_ap_up_rx_action_category_sa_query(hmac_vap, netbuf, puc_data, is_protected);
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_SELF_PROTECTED) {
        hmac_ap_up_rx_action_self_protected(hmac_vap, hmac_user, frame_hdr, netbuf, puc_data);
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_MESH) {
        hmac_ap_up_rx_action_category_mesh(puc_data);
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_VENDOR) {
        /* 查找OUI-OUI type值为 50 6F 9A - 09 (WFA P2P v1.0)  */
        /* 并用hmac_rx_mgmt_send_to_host接口上报 */
        if (HI_TRUE == mac_ie_check_p2p_action(puc_data + MAC_ACTION_OFFSET_CATEGORY)) {
            hmac_rx_mgmt_send_to_host(hmac_vap, netbuf);
        }
#endif
    }
}

/* ****************************************************************************
 功能描述  : AP在UP状态下的接收WPS probe req帧处理
 输入参数  : pst_hmac_vap: HMAC VAP指针
             pst_netbuf  : 管理帧所在的sk_buff
 修改历史      :
  1.日    期   : 2014年5月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_ap_up_rx_probe_req(const hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf)
{
    hmac_rx_ctl_stru *rx_ctrl = HI_NULL;
    enum ieee80211_band band;
    hi_s32 l_freq;

    rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    /* 获取AP 当前信道 */
    if (WLAN_BAND_2G == hmac_vap->base_vap->channel.band) {
        band = IEEE80211_BAND_2GHZ;
    } else {
        band = IEEE80211_NUM_BANDS;
    }
    l_freq = oal_ieee80211_channel_to_frequency(hmac_vap->base_vap->channel.chan_number, band);

    /* 上报接收到的probe req 管理帧 */
    hmac_send_mgmt_to_host(hmac_vap, netbuf, rx_ctrl->us_frame_len, l_freq);
}

/* ****************************************************************************
 功能描述  : AP在UP状态下的接收管理帧处理
 修改历史      :
  1.日    期   : 2013年6月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_ap_up_rx_mgmt(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *mgmt_rx_event)
{
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb((oal_netbuf_stru *)mgmt_rx_event->netbuf);
    hi_u8  *puc_mac_hdr   = (hi_u8 *)(rx_ctrl->pul_mac_hdr_start_addr);
    hi_u32  mac_hdr_len   = rx_ctrl->mac_header_len;    /* MAC头长度 */
    hi_u8  *puc_payload   = (hi_u8 *)(puc_mac_hdr) + mac_hdr_len;
    hi_u32  msg_len       = rx_ctrl->us_frame_len;     /* 消息总长度,不包括FCS */
    hi_u8   is_protected  = (hi_u8)mac_get_protectedframe(puc_mac_hdr);
    hi_u8   mgmt_type     = mac_get_frame_sub_type(puc_mac_hdr);

    if (hmac_vap->base_vap->vap_state != MAC_VAP_STATE_UP) {
        return HI_SUCCESS;
    }

    /* Bar frame proc here */
    if ((mac_get_frame_type(puc_mac_hdr) == WLAN_FC0_TYPE_CTL) && ((mgmt_type >> 4) == WLAN_BLOCKACK_REQ)) { /* 移4 */
        hmac_up_rx_bar(hmac_vap, rx_ctrl);
    }

    switch (mgmt_type) {
        case WLAN_FC0_SUBTYPE_AUTH:
            if (msg_len - mac_hdr_len < WLAN_FC0_AUTH_MIN_LEN) {
                oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_ap_up_rx_mgmt::auth frame err.}");
            } else {
                hmac_ap_rx_auth_req(hmac_vap, (oal_netbuf_stru *)mgmt_rx_event->netbuf);
            }
            break;

        case WLAN_FC0_SUBTYPE_DEAUTH:
            if (msg_len - mac_hdr_len < WLAN_FC0_DEAUTH_MIN_LEN) {
                oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_ap_up_rx_mgmt::deauth frame err.}");
            } else {
                hmac_ap_rx_deauth_req(hmac_vap, puc_mac_hdr, is_protected);
            }
            break;

        case WLAN_FC0_SUBTYPE_ASSOC_REQ:
        case WLAN_FC0_SUBTYPE_REASSOC_REQ:
            hmac_ap_up_rx_asoc_req(hmac_vap, mgmt_type, puc_mac_hdr, puc_payload, (msg_len - mac_hdr_len));
            break;

        case WLAN_FC0_SUBTYPE_DISASSOC:
            if (msg_len - mac_hdr_len < WLAN_FC0_DISASSOC_MIN_LEN) {
                oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_rx_mgmt::disassoc frame err.}");
            } else {
                hmac_ap_up_rx_disasoc(hmac_vap, puc_mac_hdr, is_protected);
            }
            break;

        case WLAN_FC0_SUBTYPE_ACTION:
            if (msg_len - mac_hdr_len < WLAN_FC0_ACTION_MIN_LEN) {
                oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_ap_up_rx_mgmt::action frame err.}");
            } else {
                hmac_ap_up_rx_action(hmac_vap, (oal_netbuf_stru *)mgmt_rx_event->netbuf, is_protected);
            }
            break;

        case WLAN_FC0_SUBTYPE_PROBE_REQ:
            hmac_ap_up_rx_probe_req(hmac_vap, (oal_netbuf_stru *)mgmt_rx_event->netbuf);
            break;

        default:
            break;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 管理帧超时处理函数
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_mgmt_timeout_ap(hi_void *param)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;
    hi_u32 ret;

    hmac_user = (hmac_user_stru *)param;

    hmac_vap = hmac_vap_get_vap_stru(hmac_user->base_user->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(hmac_user->base_user->vap_id, OAM_SF_AUTH, "{hmac_mgmt_timeout_ap::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    oam_warning_log1(hmac_user->base_user->vap_id, OAM_SF_AUTH,
        "{hmac_mgmt_timeout_ap::Wait AUTH timeout!! After %d ms.}", WLAN_AUTH_TIMEOUT);

    /* 发送去关联帧消息给STA */
    hmac_mgmt_send_deauth_frame(hmac_vap->base_vap, hmac_user->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN,
        MAC_AUTH_NOT_VALID);

    ret = hmac_user_del(hmac_vap->base_vap, hmac_user);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(hmac_user->base_user->vap_id, OAM_SF_AUTH,
                         "{hmac_mgmt_timeout_ap::hmac_user_del failed[%d].}", ret);
    }
    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : Mesh 侧接收到mesh peering confirm frame的处理入口
 输入参数  : [1]hmac_vap
             [2]puc_mac_hdr
             [3]puc_payload
             [4]payload_len
 返 回 值  : 处理正常或失败
**************************************************************************** */
static hi_u32 hmac_mesh_up_rx_confirm_action(const hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user,
    hi_u8 *puc_mac_hdr, hi_u8 *puc_payload, hi_u32 payload_len)
{
    hmac_update_sta_user_info_stru update_sta_user_info;
    hi_u32 rslt;
    mac_user_stru *mac_user = hmac_user->base_user;

    if (mac_user->is_mesh_user != HI_TRUE) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_mesh_up_rx_confirm_action::request user[%d] is not mesh user}", mac_user->us_assoc_id);
        return HI_ERR_CODE_MESH_NOT_MESH_USER;
    }

    /* 关联状态下收到confirm Action帧不刷新驱动用户属性 */
    if (mac_user->user_asoc_state == MAC_USER_STATE_ASSOC) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
                         "{hmac_mesh_up_rx_confirm_action::user[%d] assoc succ yet,dont' update user info!}",
                         mac_user->us_assoc_id);
        return HI_SUCCESS;
    }

    /* Mesh 和Mesh 之间无需开启pmf，因此不会触发SA query流程 */
    update_sta_user_info.puc_mac_hdr = puc_mac_hdr;
    update_sta_user_info.puc_payload = puc_payload;
    update_sta_user_info.msg_len = payload_len;

    mac_status_code_enum_uint16 status_code = MAC_SUCCESSFUL_STATUSCODE;
    rslt = hmac_ap_up_update_sta_user(hmac_vap, &update_sta_user_info, hmac_user, &status_code);
    if ((rslt != HI_SUCCESS) || (status_code != MAC_SUCCESSFUL_STATUSCODE)) {
        oam_warning_log1(hmac_user->base_user->vap_id, OAM_SF_ASSOC,
            "{hmac_mesh_up_rx_confirm_action::hmac_ap_up_update_sta_user failed[%d].}", status_code);
        hmac_user_set_asoc_state(hmac_vap->base_vap, hmac_user->base_user, MAC_USER_STATE_AUTH_COMPLETE);
    }

    /* 根据用户支持带宽能力，协商出当前带宽，dmac offload架构下，同步带宽信息到device */
    rslt = hmac_config_user_info_syn(hmac_vap->base_vap, hmac_user->base_user);
    if (rslt != HI_SUCCESS) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_mesh_up_rx_confirm_action::usr_info_syn failed[%d].}", rslt);
    }

    /* Mesh和Mesh之间没有802.1X */
    if (status_code == MAC_SUCCESSFUL_STATUSCODE) {
        rslt = hmac_init_user_security_port(hmac_vap->base_vap, hmac_user->base_user);
        if (rslt != HI_SUCCESS) {
            oam_error_log1(0, OAM_SF_ASSOC, "{hmac_mesh_up_rx_confirm_action::hmac_init_user_security_port Err=%d}",
                rslt);
            status_code = MAC_UNSPEC_FAIL;
        }
    }

    /* 处理失败，上报更新用户失败发送踢用户事件到Wpa_supplicant */
    if ((rslt != HI_SUCCESS) || (status_code != MAC_SUCCESSFUL_STATUSCODE)) {
        oam_warning_log2(hmac_vap->base_vap->vap_id, OAM_SF_CFG,
            "{hmac_mesh_up_rx_confirm_action:: fail rslt=%d,status_code=%d", rslt, status_code);
        hmac_user_set_asoc_state(hmac_vap->base_vap, hmac_user->base_user, MAC_USER_STATE_AUTH_COMPLETE);
        return HI_FAIL;
    }

    oam_warning_log1(mac_user->vap_id, OAM_SF_ASSOC, "{hmac_mesh_up_rx_confirm_action::update_sta_user[%d] succ}",
        mac_user->us_assoc_id);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : Mesh检查接收到的Mesh Peering Open消息中的SECURITY参数.如出错,则返回对应的错
             误码
 输入参数  : [1]hmac_vap
             [2]puc_payload
             [3]msg_len
             [4]hmac_user
 输出参数  : [1]pen_status_code
 返 回 值  : hi_u32
**************************************************************************** */
static hi_u32 hmac_check_open_action_sec_authenticator(const hmac_vap_stru *hmac_vap, hi_u8 *puc_payload,
    hi_u32 msg_len, const hmac_user_stru *hmac_user, mac_status_code_enum_uint16 *pen_status_code)
{
    hi_u8 pcip_policy = WLAN_80211_CIPHER_SUITE_NO_ENCRYP;
    mac_user_stru *mac_user = NULL;
    hi_u8 *puc_rsn_ie = HI_NULL;
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 index = 0;
    hi_u32 ret;

    mac_vap_stru *mac_vap = hmac_vap->base_vap;
    wlan_mib_ieee802dot11_stru *mib_info = mac_vap->mib_info;

    mib_info->wlan_mib_rsna_cfg.dot11_rsna_group_cipher_requested = 0;
    mib_info->wlan_mib_rsna_cfg.dot11_rsna_pairwise_cipher_requested = 0;

    if (mib_info->wlan_mib_privacy.dot11_rsna_activated != HI_TRUE) {
        oam_info_log0(mac_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_check_open_action_sec_authenticator::dot11RSNAActivated = FALSE.}");
        return HI_SUCCESS;
    }

    if (mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_security_activated != HI_TRUE) {
        oam_info_log0(mac_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_check_open_action_sec_authenticator::dot11MeshSecurityActivated = FALSE.}");
        return HI_SUCCESS;
    }

    hi_u8 offset = MAC_ACTION_CATEGORY_LEN + MAC_ACTION_CODE_LEN + MAC_CAP_INFO_LEN;

    if (msg_len > offset) {
        /* 获取RSNA信息 */
        puc_rsn_ie = mac_find_ie(MAC_EID_RSN, puc_payload + offset, (msg_len - offset));
    }

    /* 获取RSNA信息 */
    puc_rsn_ie = mac_find_ie(MAC_EID_RSN, puc_payload + offset, (msg_len - offset));
    if (puc_rsn_ie == HI_NULL) {
        *pen_status_code = MAC_CIPHER_REJ;
        oam_info_log0(mac_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_check_open_action_sec_authenticator::can not find RSNA ie.}");
        return HI_FAIL;
    }

    /* RSNA加密方式处理 */
    /* 略过 IE + LEN */
    index += 2; /* 2 略过 IE + LEN */
    puc_ie = puc_rsn_ie;

    ret = hmac_chk_open_action_sec_cap(mac_vap, puc_ie, DMAC_RSNA_802_11I, index, pen_status_code);
    if (ret != HI_SUCCESS) {
        return HI_FAIL;
    }

    if (*pen_status_code == MAC_SUCCESSFUL_STATUSCODE) {
        /* 检测出来的这个硬件加密方式应该保存到 mac_user -> en_cipher_type 中 */
        mac_user = hmac_user->base_user;
        /* 保存用户的加密方式 */
        mac_user->key_info.cipher_type = pcip_policy;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : Mesh检测Mesh Open Peering 帧中的安全相关公共信息
 输入参数  : [1]mac_vap
             [2]puc_ie
             [3]hi_u8 uc_80211i_mode
             [4]offset
 输出参数  : [1]pen_status_code
 返 回 值  : hi_u32
**************************************************************************** */
static hi_u32 hmac_chk_open_action_sec_cap(const mac_vap_stru *mac_vap, const hi_u8 *puc_ie, hi_u8 is_80211i_mode,
    hi_u8 offset, mac_status_code_enum_uint16 *pen_status_code)
{
    hi_u8 auc_oui[MAC_OUI_LEN] = {0};
    hi_u8 index                = offset;
    hi_u8 len                  = 0;
    wlan_mib_ieee802dot11_stru *mib_info = mac_vap->mib_info;

    if (hmac_get_security_oui(is_80211i_mode, auc_oui) != HI_SUCCESS) {
        *pen_status_code = MAC_UNSUP_RSN_INFO_VER;
        oam_error_log0(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_open_action_sec_cap:getSecurityOui Er}");
        return HI_FAIL;
    }

    /* 检测版本信息 */
    if (mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_version != hi_makeu16(puc_ie[index], puc_ie[index + 1])) {
        *pen_status_code = MAC_UNSUP_RSN_INFO_VER;
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_open_action_sec_cap:unsup rsn ver=%d}",
            hi_makeu16(puc_ie[index], puc_ie[index + 1]));
        return HI_FAIL;
    }

    /* 忽略版本信息 */
    index += 2; /* 2 忽略版本信息 */

    /* 检测组播OUI 信息 */
    if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_open_action_sec_cap::invalid OUI.}");
    }
    index += MAC_OUI_LEN;

    hi_u8 grp_policy = puc_ie[index++];                                  /* 获取组播密钥 */
    hi_u8 pcip_policy = hmac_get_pcip_policy_auth(puc_ie + index, &len); /* 获取单播加密套件 */

    index += len;

    /* 获取认证套件信息 */
    hi_u8 auth_policy = hmac_get_auth_policy_auth(puc_ie + index, &len);

    /* 成对密钥套件是否为组播密钥套件 */
    pcip_policy = (pcip_policy == WLAN_80211_CIPHER_SUITE_GROUP_CIPHER) ? grp_policy : pcip_policy;
    /* 检测组播套件 */
    if (mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher != grp_policy) {
        *pen_status_code = MAC_INVALID_GRP_CIPHER;
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_chk_open_action_sec_cap::invalid group[%d].}", grp_policy);
        return HI_FAIL;
    }

    /* 检测单播密钥套件 (只有CCMP) */
    if (hmac_mesh_check_pcip_policy(pcip_policy) != HI_TRUE) {
        *pen_status_code = MAC_INVALID_PW_CIPHER;
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_open_action_sec_cap::invalid pcip[%d].}", pcip_policy);
        return HI_FAIL;
    }

    /* 检测认证套件 (只有SAE) */
    if (hmac_mesh_check_auth_policy(auth_policy) == HI_FALSE) {
        *pen_status_code = MAC_INVALID_AKMP_CIPHER;
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{hmac_chk_open_action_sec_cap::invalid auth[%d].}", auth_policy);
        return HI_FAIL;
    }

    /* 针对RSN 情况下，做单独处理 RSN capability */
    /* RSN Capabilities Information                          */
    /* ----------------------------------------------------- */
    /* | B15 - B6    | B5 - B4          | B3 - B2          | */
    /* ----------------------------------------------------- */
    /* | Reserved    | GTSKA Replay Ctr | PTSKA Replay Ctr | */
    /* ----------------------------------------------------- */
    /* | B1             | B0                               | */
    /* ----------------------------------------------------- */
    /* | No Pairwise    | Pre-Authentication               | */
    /* ----------------------------------------------------- */
    /* 检测预认证能力 */
    /* 更新mib 参数 */
    mib_info->wlan_mib_rsna_cfg.dot11_rsna_group_cipher_requested = grp_policy;
    mib_info->wlan_mib_rsna_cfg.dot11_rsna_pairwise_cipher_requested = pcip_policy;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 检测MESH单播密钥是否匹配
 输入参数  :hi_u8 uc_policy
 返 回 值      : hi_u8
                           HI_TRUE:匹配成功
                           HI_FALSE:匹配失败
 修改历史      :
  1.日    期   : 2019年2月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_mesh_check_pcip_policy(hi_u8 policy)
{
    /* 检测单播密钥是否使能和匹配 */
    if (policy != WLAN_80211_CIPHER_SUITE_CCMP) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

/* ****************************************************************************
 功能描述  : 检测认证方式是否匹配
 输入参数  : hi_u8 uc_policy
 返 回 值  : hi_u8    HI_TRUE:匹配成功
                                    HI_FALSE:匹配失败
 修改历史      :
  1.日    期   : 2019年2月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_mesh_check_auth_policy(hi_u8 policy)
{
    /* 检测认证方式是否使能和匹配 */
    if (policy != WLAN_AUTH_SUITE_SAE_SHA256) {
        return HI_FALSE;
    }
    return HI_TRUE;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
