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
#include "mac_frame.h"
#include "mac_user.h"
#include "mac_vap.h"
#include "mac_mib.h"
#include "dmac_ext_if.h"
#include "hmac_config.h"
#include "hmac_encap_frame_ap.h"
#include "hmac_main.h"
#include "hmac_tx_data.h"
#include "hmac_mgmt_ap.h"
#include "hmac_11i.h"
#include "hmac_blockack.h"
#include "frw_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 封装chtxt
 修改历史      :
  1.日    期   : 2013年7月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_mgmt_encap_chtxt(hi_u8 *puc_frame, const hi_u8 *puc_chtxt, hi_u16 *pus_auth_rsp_len,
    hmac_user_stru *hmac_user)
{
    /* Challenge Text Element                  */
    /* --------------------------------------- */
    /* |Element ID | Length | Challenge Text | */
    /* --------------------------------------- */
    /* | 1         |1       |1 - 253         | */
    /* --------------------------------------- */
    puc_frame[6] = MAC_EID_CHALLENGE; /* 6 元素索引 */
    puc_frame[7] = WLAN_CHTXT_SIZE;   /* 7 元素索引 */

    /* 认证帧长度增加Challenge Text Element的长度 */
    *pus_auth_rsp_len += (WLAN_CHTXT_SIZE + MAC_IE_HDR_LEN);
    /* 将challenge text拷贝到帧体中去 */
    hi_u32 ret = (hi_u32)memcpy_s(&puc_frame[8], WLAN_CHTXT_SIZE, puc_chtxt, WLAN_CHTXT_SIZE); /* 8 挑战字段起始地址 */
    if (ret != EOK) {
        oam_error_log1(0, OAM_SF_CFG, "hmac_mgmt_encap_chtxt:: challenge text memcpy fail, ret[%d].", ret);
        return;
    }
    /* 保存明文的challenge text */
    if (hmac_user->ch_text == HI_NULL) {
        /* 此处只负责申请由认证成功或者超时定时器释放 */
        hmac_user->ch_text = (hi_u8 *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, WLAN_CHTXT_SIZE);
    }

    if (hmac_user->ch_text != HI_NULL) {
        ret = (hi_u32)memcpy_s(hmac_user->ch_text, WLAN_CHTXT_SIZE, puc_chtxt, WLAN_CHTXT_SIZE);
        if (ret != EOK) {
            oam_error_log1(0, OAM_SF_CFG, "hmac_mgmt_encap_chtxt:: save challenge text fail, ret[%d].", ret);
            return;
        }
    }
}

#ifdef _PRE_WLAN_FEATURE_PMF
/* ****************************************************************************
 功能描述  : 计算得到 assoc rsp 中所需的assoc comeback time
 输入参数  : pst_mac_vap   : mac vap 指针
             pst_hmac_user : hamc user 指针
 返 回 值  : 计算得到的assoc comeback time值
 修改历史      :
  1.日    期   : 2014年4月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_get_assoc_comeback_time(const mac_vap_stru *mac_vap, const hmac_user_stru *hmac_user)
{
    hi_u32 timeout = 0;

    hi_u32 now_time;
    hi_u32 passed_time;
    hi_u32 sa_query_maxtimeout;

    /* 获取现在时间 */
    now_time = (hi_u32)hi_get_milli_seconds();

    /* 设置ASSOCIATION_COMEBACK_TIME，使STA在AP完成SA Query流程之后再发送关联请求 */
    /* 获得sa Query Max timeout值 */
    sa_query_maxtimeout = mac_mib_get_dot11_association_saquery_maximum_timeout(mac_vap);

    /* 是否现在有sa Query流程正在进行 */
    if ((hmac_user->sa_query_info.sa_query_interval_timer.is_enabled == HI_TRUE) &&
        (now_time >= hmac_user->sa_query_info.sa_query_start_time)) {
        /* 待现有SA Query流程结束后才可以接受STA发过来的关联帧 */
        passed_time = now_time - hmac_user->sa_query_info.sa_query_start_time;
        timeout = sa_query_maxtimeout - passed_time;
    } else {
        /* 给接下来的SA Query流程预留时间 */
        timeout = sa_query_maxtimeout;
    }

    return timeout;
}
#endif

hi_void hmac_set_supported_rates_ie_asoc_rsp(const mac_user_stru *mac_user, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    hi_u8 nrates;
    hi_u8 idx;
    /* *************************************************************************
                        ---------------------------------------
                        |Element ID | Length | Supported Rates|
                        ---------------------------------------
             Octets:    |1          | 1      | 1~8            |
                        ---------------------------------------
    The Information field is encoded as 1 to 8 octets, where each octet describes a single Supported
    Rate or BSS membership selector.
    ************************************************************************* */
    puc_buffer[0] = MAC_EID_RATES;
    nrates = mac_user->avail_op_rates.rs_nrates;

    if (nrates > MAC_MAX_SUPRATES) {
        nrates = MAC_MAX_SUPRATES;
    }

    for (idx = 0; idx < nrates; idx++) {
        puc_buffer[MAC_IE_HDR_LEN + idx] = mac_user->avail_op_rates.auc_rs_rates[idx];
    }

    puc_buffer[1] = nrates;
    *puc_ie_len = MAC_IE_HDR_LEN + nrates;
}

hi_void hmac_set_exsup_rates_ie_asoc_rsp(const mac_user_stru *mac_user, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    hi_u8 nrates;
    hi_u8 idx;

    /* **************************************************************************
                   -----------------------------------------------
                   |ElementID | Length | Extended Supported Rates|
                   -----------------------------------------------
       Octets:     |1         | 1      | 1-255                   |
                   -----------------------------------------------
    ************************************************************************** */
    if (mac_user->avail_op_rates.rs_nrates <= MAC_MAX_SUPRATES) {
        *puc_ie_len = 0;
        return;
    }

    puc_buffer[0] = MAC_EID_XRATES;
    nrates = mac_user->avail_op_rates.rs_nrates - MAC_MAX_SUPRATES;
    puc_buffer[1] = nrates;

    for (idx = 0; idx < nrates; idx++) {
        puc_buffer[MAC_IE_HDR_LEN + idx] = mac_user->avail_op_rates.auc_rs_rates[idx + MAC_MAX_SUPRATES];
    }

    *puc_ie_len = MAC_IE_HDR_LEN + nrates;
}


hi_u32 hmac_mgmt_encap_asoc_rsp_ap_add_copy(const mac_vap_stru *mac_ap,
    const hmac_asoc_rsp_ap_info_stru *asoc_rsp_ap_info)
{
    if (mac_ap->mib_info == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 设置 Frame Control field */
    mac_hdr_set_frame_control(asoc_rsp_ap_info->puc_asoc_rsp, asoc_rsp_ap_info->us_type);
    /* 设置 DA address1: STA MAC地址 设置 SA address2: dot11MACAddress 设置 DA address3: AP MAC地址 (BSSID) */
    if ((memcpy_s(asoc_rsp_ap_info->puc_asoc_rsp + WLAN_HDR_ADDR1_OFFSET, WLAN_MAC_ADDR_LEN,
        asoc_rsp_ap_info->puc_sta_addr, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(asoc_rsp_ap_info->puc_asoc_rsp + WLAN_HDR_ADDR2_OFFSET, WLAN_MAC_ADDR_LEN,
                  mac_ap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(asoc_rsp_ap_info->puc_asoc_rsp + WLAN_HDR_ADDR3_OFFSET, WLAN_MAC_ADDR_LEN, mac_ap->auc_bssid,
                  WLAN_MAC_ADDR_LEN) != EOK)) {
        oam_error_log0(0, 0, "{hmac_mgmt_encap_asoc_rsp_ap::memcpy_s fail.}");
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 组关联响应帧
 修改历史      :
  1.日    期   : 2013年7月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_mgmt_encap_asoc_rsp_ap(mac_vap_stru *mac_ap, hmac_asoc_rsp_ap_info_stru *asoc_rsp_ap_info)
{
    hi_u8 ie_len = 0;
    hi_u16 us_app_ie_len;

    /* 保存起始地址，方便计算长度 */
    hi_u8 *puc_asoc_rsp_original = asoc_rsp_ap_info->puc_asoc_rsp;

    /* 获取user */
    mac_user_stru *mac_user = mac_user_get_user_stru(asoc_rsp_ap_info->assoc_id);
    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(asoc_rsp_ap_info->assoc_id);
    if (mac_user == HI_NULL || hmac_user == HI_NULL) {
        oam_error_log3(0, OAM_SF_ASSOC, "{hmac_mgmt_encap_asoc_rsp_ap::mac_user/hmac_user(%d) is null mac=%p, hmac=%p}",
            asoc_rsp_ap_info->assoc_id, (uintptr_t)mac_user, (uintptr_t)hmac_user);
        return 0;
    }
#ifdef _PRE_WLAN_FEATURE_PMF
    mac_timeout_interval_type_enum tie_type =
        (asoc_rsp_ap_info->status_code == MAC_REJECT_TEMP) ? MAC_TIE_ASSOCIATION_COMEBACK_TIME : MAC_TIE_BUTT;
#endif
    /* *********************************************************************** */
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                Set the fields in the frame header                     */
    /* *********************************************************************** */
    if (hmac_mgmt_encap_asoc_rsp_ap_add_copy(mac_ap, asoc_rsp_ap_info) != HI_SUCCESS) {
        return 0;
    }

    asoc_rsp_ap_info->puc_asoc_rsp += MAC_80211_FRAME_LEN;

    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*              Association Response Frame - Frame Body                  */
    /* --------------------------------------------------------------------- */
    /* | Capability Information |   Status Code   | AID | Supported  Rates | */
    /* --------------------------------------------------------------------- */
    /* |2                       |2                |2    |3-10              | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* 设置 capability information field */
    mac_set_cap_info_ap((hi_void *)mac_ap, asoc_rsp_ap_info->puc_asoc_rsp);
    asoc_rsp_ap_info->puc_asoc_rsp += MAC_CAP_INFO_LEN;
    /* 设置 Status Code */
    mac_set_status_code_ie(asoc_rsp_ap_info->puc_asoc_rsp, asoc_rsp_ap_info->status_code);
    asoc_rsp_ap_info->puc_asoc_rsp += MAC_STATUS_CODE_LEN;
    /* 设置 Association ID */
    mac_set_aid_ie(asoc_rsp_ap_info->puc_asoc_rsp, (hi_u16)asoc_rsp_ap_info->assoc_id);
    asoc_rsp_ap_info->puc_asoc_rsp += MAC_AID_LEN;
    /* 设置 Supported Rates IE */
    hmac_set_supported_rates_ie_asoc_rsp(mac_user, asoc_rsp_ap_info->puc_asoc_rsp, &ie_len);
    asoc_rsp_ap_info->puc_asoc_rsp += ie_len;
    /* 设置 Extended Supported Rates IE */
    hmac_set_exsup_rates_ie_asoc_rsp(mac_user, asoc_rsp_ap_info->puc_asoc_rsp, &ie_len);
    asoc_rsp_ap_info->puc_asoc_rsp += ie_len;
    /* 设置 EDCA IE */
    mac_set_wmm_params_ie((hi_void *)mac_ap, asoc_rsp_ap_info->puc_asoc_rsp, mac_user->cap_info.qos, &ie_len);
    asoc_rsp_ap_info->puc_asoc_rsp += ie_len;
#ifdef _PRE_WLAN_FEATURE_PMF
    /* 设置 Timeout Interval (Association Comeback time) IE */
    hi_u32 timeout = hmac_get_assoc_comeback_time(mac_ap, hmac_user);
    mac_set_timeout_interval_ie(asoc_rsp_ap_info->puc_asoc_rsp, &ie_len, tie_type, timeout);
    asoc_rsp_ap_info->puc_asoc_rsp += ie_len;
#endif
    if (mac_user->ht_hdl.ht_capable == HI_TRUE) {
        /* 设置 HT-Capabilities Information IE */
        mac_set_ht_capabilities_ie((hi_void *)mac_ap, asoc_rsp_ap_info->puc_asoc_rsp, &ie_len);
        asoc_rsp_ap_info->puc_asoc_rsp += ie_len;
        /* 设置 HT-Operation Information IE */
        mac_set_ht_opern_ie((hi_void *)mac_ap, asoc_rsp_ap_info->puc_asoc_rsp, &ie_len);
        asoc_rsp_ap_info->puc_asoc_rsp += ie_len;
        /* 设置 Extended Capabilities Information IE */
        mac_set_ext_capabilities_ie((hi_void *)mac_ap, asoc_rsp_ap_info->puc_asoc_rsp, &ie_len);
        asoc_rsp_ap_info->puc_asoc_rsp += ie_len;
    }

    /* 填充WPS信息 */
    mac_add_app_ie((hi_void *)mac_ap, asoc_rsp_ap_info->puc_asoc_rsp, &us_app_ie_len, OAL_APP_ASSOC_RSP_IE);
    asoc_rsp_ap_info->puc_asoc_rsp += us_app_ie_len;

    return (hi_u32)(asoc_rsp_ap_info->puc_asoc_rsp - puc_asoc_rsp_original);
}

/* ****************************************************************************
 功能描述  : 判断两个challenge txt是否相等
 修改历史      :
  1.日    期   : 2013年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u8 hmac_mgmt_is_challenge_txt_equal(hi_u8 *puc_data, const hi_u8 *puc_chtxt)
{
    hi_u8 *puc_ch_text = 0;
    hi_u16 us_idx = 0;
    hi_u8 ch_text_len;

    if (puc_chtxt == HI_NULL) {
        return HI_FALSE;
    }
    /* Challenge Text Element                  */
    /* --------------------------------------- */
    /* |Element ID | Length | Challenge Text | */
    /* --------------------------------------- */
    /* | 1         |1       |1 - 253         | */
    /* --------------------------------------- */
    ch_text_len = puc_data[1];
    puc_ch_text = puc_data + 2; /* 加2 */

    for (us_idx = 0; us_idx < ch_text_len; us_idx++) {
        /* Return false on mismatch */
        if (puc_ch_text[us_idx] != puc_chtxt[us_idx]) {
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}


hi_u32 hmac_encap_auth_rsp_get_user_idx_seq(mac_vap_stru *mac_vap, hi_u8 is_seq1, hi_u8 *mac_addr, hi_u8 addr_len,
    hi_u8 *puc_user_index)
{
    /* 在收到第一个认证帧时用户已创建 */
    if (!is_seq1) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_encap_auth_rsp_get_user_idx::user have been add at seq1}");
        return HI_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_MESH
    /* Accepting Peer值控制Mesh关联，STA关联由Mesh Accepting STA控制 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        if (mac_vap->mesh_accept_sta == HI_FALSE) {
            oam_warning_log0(0, OAM_SF_ANY, "{hmac_encap_auth_rsp_get_user_idx:mesh vap not accept sta connect!}");
            return HI_ERR_CODE_MESH_NOT_ACCEPT_PEER;
        }
    }
#endif
    hi_u32 ret = hmac_user_add(mac_vap, mac_addr, addr_len, puc_user_index);
    if (ret != HI_SUCCESS) {
        if (ret == HI_ERR_CODE_CONFIG_EXCEED_SPEC) {
            oam_warning_log0(0, OAM_SF_ANY, "{hmac_encap_auth_rsp_get_user_idx:add_assoc_user fail,users config spec}");
            return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
        } else {
            oam_error_log1(0, OAM_SF_ANY, "{hmac_encap_auth_rsp_get_user_idx:add_assoc_user fail %d}", *puc_user_index);
            return HI_FAIL;
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取user idx,如果用户不存在，且resend置位的话，将sta加入ap
 输入参数  : 1.vap指针
             2.sta的mac地址
             3.是否为seq1标志位.如果为真，表示如果用户不存在,需要将sta加入ap
 输出参数  : 1. puc_auth_resend 用户存在的情况下收到seq1,seq1判定为重传帧，
                置位此标志
             2. puc_user_index 返回获取到的user idx
 返 回 值  :获取正常或者失败
 修改历史      :
  1.日    期   : 2014年1月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* puc_user_index作为参数传入mac_vap_find_user_by_macaddr，在其中对其内容进行了修改，lin_t e818告警屏蔽 */
hi_u32 hmac_encap_auth_rsp_get_user_idx(mac_vap_stru *mac_vap, hmac_mac_addr_stru auth_mac_addr, hi_u8 is_seq1,
    hi_u8 *puc_auth_resend, hi_u8 *puc_user_index)
{
    hi_u8 user_idx;
    hi_u8 *mac_addr = auth_mac_addr.mac_addr;
    hi_u8 addr_len = auth_mac_addr.addr_len;

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_encap_auth_rsp_get_user_idx:hmac_vap_get_vap_stru failed!}");
        return HI_FAIL;
    }

    *puc_auth_resend = HI_FALSE;
    /* 找到用户 */
    if (mac_vap_find_user_by_macaddr(hmac_vap->base_vap, mac_addr, addr_len, puc_user_index) == HI_SUCCESS) {
        /* 获取hmac用户的状态，如果不是0，说明是重复帧 */
        hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(*puc_user_index);
        if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
            oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_encap_auth_rsp_get_user_idx::hmac_user_get_user null}");
            return HI_FAIL;
        }
        /* en_user_asoc_state为枚举变量，取值为1~4，初始化为MAC_USER_STATE_BUTT，
         * 应该使用!=MAC_USER_STATE_BUTT作为判断，否则会导致WEP share加密关联不上问题
         */
        if (hmac_user->base_user->user_asoc_state != MAC_USER_STATE_BUTT) {
            *puc_auth_resend = HI_TRUE;
        }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        if (hmac_user->base_user->user_asoc_state == MAC_USER_STATE_ASSOC) {
            oal_net_device_stru *netdev = hmac_vap_get_net_device(mac_vap->vap_id);
            if (netdev != HI_NULL) {
                oal_kobject_uevent_env_sta_leave(netdev, mac_addr);
            }
        }
#endif
        return HI_SUCCESS;
    }

    /* 若在同一device下的其他VAP下找到该用户，删除之。否则导致业务不通。在DBAC下尤其常见 */
    if (mac_device_find_user_by_macaddr(hmac_vap->base_vap, mac_addr, addr_len, &user_idx) == HI_SUCCESS) {
        hmac_user_stru *hmac_user_tmp = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
        if ((hmac_user_tmp != HI_NULL) && (hmac_user_tmp->base_user != HI_NULL)) {
            mac_vap_stru *mac_vap_tmp = mac_vap_get_vap_stru(hmac_user_tmp->base_user->vap_id);
            if (mac_vap_tmp != HI_NULL) {
                hmac_user_del(mac_vap_tmp, hmac_user_tmp);
            }
        }
    }

    return hmac_encap_auth_rsp_get_user_idx_seq(mac_vap, is_seq1, mac_addr, addr_len, puc_user_index);
}

/* ****************************************************************************
 功能描述  : 处理seq1的auth req
 输入参数  : 1.auth_rsp_param 处理auth rsp所需的参数
 输出参数  : 1.puc_code 错误码
             2.pst_usr_ass_stat auth处理完成之后，置相应的user状态
 返 回 值  :获取正常或者失败
 修改历史      :
  1.日    期   : 2014年1月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hmac_ap_auth_process_code_enum_uint8 hmac_encap_auth_rsp_seq1(const hmac_auth_rsp_param_stru *auth_rsp_param,
    hi_u8 *puc_code, mac_user_asoc_state_enum_uint8 *usr_ass_stat)
{
    *puc_code = MAC_SUCCESSFUL_STATUSCODE;
    *usr_ass_stat = MAC_USER_STATE_BUTT;
    /* 如果不是重传 */
    if (auth_rsp_param->auth_resend != HI_TRUE) {
        if (auth_rsp_param->us_auth_type == WLAN_WITP_AUTH_OPEN_SYSTEM) {
            *usr_ass_stat = MAC_USER_STATE_AUTH_COMPLETE;

            return HMAC_AP_AUTH_SEQ1_OPEN_ANY;
        }

        if (auth_rsp_param->is_wep_allowed == HI_TRUE) {
            *usr_ass_stat = MAC_USER_STATE_AUTH_KEY_SEQ1;
            /* 此处返回后需要wep后操作 */
            return HMAC_AP_AUTH_SEQ1_WEP_NOT_RESEND;
        }

        /* 不支持算法 */
        *puc_code = MAC_UNSUPT_ALG;
        return HMAC_AP_AUTH_BUTT;
    }

    /* 检查用户状态 */
    if ((auth_rsp_param->user_asoc_state == MAC_USER_STATE_ASSOC) &&
        (auth_rsp_param->us_auth_type == WLAN_WITP_AUTH_OPEN_SYSTEM)) {
        /* 用户已经关联上了不需要任何操作 */
        *usr_ass_stat = MAC_USER_STATE_AUTH_COMPLETE;
        return HMAC_AP_AUTH_DUMMY;
    }

    if (auth_rsp_param->us_auth_type == WLAN_WITP_AUTH_OPEN_SYSTEM) {
        *usr_ass_stat = MAC_USER_STATE_AUTH_COMPLETE;

        return HMAC_AP_AUTH_SEQ1_OPEN_ANY;
    }

    if (auth_rsp_param->is_wep_allowed == HI_TRUE) {
        /* seq为1 的认证帧重传 */
        *usr_ass_stat = MAC_USER_STATE_AUTH_COMPLETE;
        return HMAC_AP_AUTH_SEQ1_WEP_RESEND;
    }
    /* 不支持算法 */
    *puc_code = MAC_UNSUPT_ALG;
    return HMAC_AP_AUTH_BUTT;
}

/* ****************************************************************************
 功能描述  : 处理seq3的auth req
 输入参数  : 1.auth_rsp_param 处理auth rsp所需的参数

 输出参数  : 1.puc_code 错误码
             2.pst_usr_ass_stat auth处理完成之后，置相应的user状态

 返 回 值  :获取正常或者失败
 修改历史      :
  1.日    期   : 2014年1月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hmac_ap_auth_process_code_enum_uint8 hmac_encap_auth_rsp_seq3(const hmac_auth_rsp_param_stru *auth_rsp_param,
    hi_u8 *puc_code, mac_user_asoc_state_enum_uint8 *usr_ass_stat)
{
    /* 如果不存在，返回错误 */
    if (auth_rsp_param->auth_resend == HI_FALSE) {
        *usr_ass_stat = MAC_USER_STATE_BUTT;
        *puc_code = MAC_SUCCESSFUL_STATUSCODE;
        return HMAC_AP_AUTH_BUTT;
    }
    /* 检查用户状态 */
    if ((auth_rsp_param->user_asoc_state == MAC_USER_STATE_ASSOC) &&
        (auth_rsp_param->us_auth_type == WLAN_WITP_AUTH_OPEN_SYSTEM)) {
        /* 用户已经关联上了不需要任何操作 */
        *usr_ass_stat = MAC_USER_STATE_AUTH_COMPLETE;
        *puc_code = MAC_SUCCESSFUL_STATUSCODE;
        return HMAC_AP_AUTH_DUMMY;
    }

    if (auth_rsp_param->us_auth_type == WLAN_WITP_AUTH_OPEN_SYSTEM) {
        *usr_ass_stat = MAC_USER_STATE_AUTH_COMPLETE;
        *puc_code = MAC_SUCCESSFUL_STATUSCODE;
        return HMAC_AP_AUTH_SEQ3_OPEN_ANY;
    }

    if (auth_rsp_param->user_asoc_state == MAC_USER_STATE_AUTH_KEY_SEQ1) {
        *usr_ass_stat = MAC_USER_STATE_AUTH_COMPLETE;
        *puc_code = MAC_SUCCESSFUL_STATUSCODE;
        return HMAC_AP_AUTH_SEQ3_WEP_COMPLETE;
    }

    if (auth_rsp_param->user_asoc_state == MAC_USER_STATE_AUTH_COMPLETE) {
        *usr_ass_stat = MAC_USER_STATE_AUTH_COMPLETE;
        *puc_code = MAC_SUCCESSFUL_STATUSCODE;
        return HMAC_AP_AUTH_SEQ3_WEP_COMPLETE;
    }

    if (auth_rsp_param->user_asoc_state == MAC_USER_STATE_ASSOC) {
        *usr_ass_stat = MAC_USER_STATE_AUTH_KEY_SEQ1;
        *puc_code = MAC_SUCCESSFUL_STATUSCODE;
        return HMAC_AP_AUTH_SEQ3_WEP_ASSOC;
    }

    /* 不支持算法 */
    *usr_ass_stat = MAC_USER_STATE_BUTT;
    *puc_code = MAC_UNSUPT_ALG;
    return HMAC_AP_AUTH_BUTT;
}

/* ****************************************************************************
 功能描述  : 处理seq3的auth req
 输入参数  : 1.auth_rsp_param 处理auth rsp所需的参数数组

 输出参数  : 1.puc_code 错误码
             2.pst_usr_ass_stat auth处理完成之后，置相应的user状态

 返 回 值  :获取正常或者失败
 修改历史      :
  1.日    期   : 2014年1月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hmac_auth_rsp_fun hmac_encap_auth_rsp_get_func(hi_u16 us_auth_seq)
{
    hmac_auth_rsp_fun auth_rsp_fun = HI_NULL;
    switch (us_auth_seq) {
        case WLAN_AUTH_TRASACTION_NUM_ONE:
            auth_rsp_fun = hmac_encap_auth_rsp_seq1;
            break;
        case WLAN_AUTH_TRASACTION_NUM_THREE:
            auth_rsp_fun = hmac_encap_auth_rsp_seq3;
            break;
        default:
            auth_rsp_fun = HI_NULL;
            break;
    }
    return auth_rsp_fun;
}

/* ****************************************************************************
 功能描述  : 判断认证类型是否支持
 输入参数  : 1.pst_hmac_vap vap指针
             2. us_auth_type 认证类型
 返 回 值  :HI_SUCCESS-支持，其他-不支持
 修改历史      :
  1.日    期   : 2014年1月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_encap_auth_rsp_support(const hmac_vap_stru *hmac_vap, hi_u16 us_auth_type)
{
    /* 检测认证类型是否支持 不支持的话状态位置成UNSUPT_ALG */
    if ((hmac_vap->auth_mode) != us_auth_type && (hmac_vap->auth_mode != WLAN_WITP_ALG_AUTH_BUTT)) {
        return HI_ERR_CODE_CONFIG_UNSUPPORT;
    }
    return HI_SUCCESS;
}

/*****************************************************************************
 功能描述  : 删除hmac tid相关的信息
 修改历史      :
  1.日    期   : 2014年8月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_tid_clear(mac_vap_stru *mac_vap, hmac_user_stru *hmac_user)
{
    hi_u8 loop;
    hmac_tid_stru *tid = HI_NULL;
#if defined(_PRE_WLAN_FEATURE_AMPDU_VAP)
    hmac_vap_stru *hmac_vap = HI_NULL;
#endif

#if defined(_PRE_WLAN_FEATURE_AMPDU_VAP)
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_BA, "{hmac_tid_clear::pst_hmac_vap null.}");
        return;
    }
#else
    hi_unref_param(mac_vap);
#endif
    for (loop = 0; loop < WLAN_WME_MAX_TID_NUM; loop++) {
        tid = &(hmac_user->ast_tid_info[loop]);
        tid->tid_no = (hi_u8)loop;
        /* 清除接收方向会话句柄 */
        if (tid->ba_rx_info != HI_NULL) {
            hmac_ba_reset_rx_handle(&tid->ba_rx_info, loop);
        }
        /* 清除发送方向会话句柄 */
        if (tid->ba_tx_info != HI_NULL) {
            hmac_ba_reset_tx_handle(&tid->ba_tx_info);
        }
        hmac_user->ast_tid_info[loop].ba_flag = 0;
    }
}

/* ****************************************************************************
 功能描述  : 封装auth rsp帧的帧体，CODE默认填写为SUCCESS，下一个函数刷新
**************************************************************************** */
hi_u16 hmac_encap_auth_rsp_body(const mac_vap_stru *mac_vap, oal_netbuf_stru *auth_rsp, const oal_netbuf_stru *auth_req)
{
    hi_u8            *puc_data = HI_NULL;
    hi_u8            *puc_frame = HI_NULL;
    hmac_tx_ctl_stru *tx_ctl = HI_NULL;
    hi_u8            mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    hi_u16           auth_rsp_len;
    hi_u16           auth_type;
    hi_u16           auth_seq;

    puc_data = (hi_u8 *)oal_netbuf_header(auth_rsp);
    tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(auth_rsp);
    /* *********************************************************************** */
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                Set the fields in the frame header                     */
    /* *********************************************************************** */
    /* 设置函数头的frame control字段 */
    mac_hdr_set_frame_control(puc_data, WLAN_FC0_SUBTYPE_AUTH);
    /* 获取STA的地址 */
    mac_get_address2(oal_netbuf_header(auth_req), WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN);
    /* 将DA设置为STA的地址 */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address1, WLAN_MAC_ADDR_LEN, mac_addr,
        WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_auth_rsp_body::copy address1 failed!}");
        return 0;
    }
    /* 将SA设置为dot11MacAddress */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address2, WLAN_MAC_ADDR_LEN,
        mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_auth_rsp_body::copy address2 failed!}");
        return 0;
    }
    /* 设置BSSID */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address3, WLAN_MAC_ADDR_LEN, mac_vap->auc_bssid,
        WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_auth_rsp_body::copy address3 failed!}");
        return 0;
    }

    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*              Authentication Frame - Frame Body                        */
    /* --------------------------------------------------------------------- */
    /* |Auth Algo Number|Auth Trans Seq Number|Status Code| Challenge Text | */
    /* --------------------------------------------------------------------- */
    /* | 2              |2                    |2          | 3 - 256        | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* 解析认证类型 */
    auth_type = mac_get_auth_alg(oal_netbuf_header(auth_req));
    /* 解析auth transaction number */
    auth_seq = mac_get_auth_seq_num(oal_netbuf_header(auth_req));
    if (auth_seq > WLAN_AUTH_TRASACTION_NUM_FOUR) {
        oam_warning_log1(0, OAM_SF_AUTH, "{hmac_encap_auth_rsp_body::invalid auth seq [%d]}", auth_seq);
        return 0;
    }

    puc_frame = (hi_u8 *)(puc_data + MAC_80211_FRAME_LEN);
    /* 计算认证相应帧的长度 */
    auth_rsp_len = MAC_80211_FRAME_LEN + MAC_AUTH_ALG_LEN + MAC_AUTH_TRANS_SEQ_NUM_LEN + MAC_STATUS_CODE_LEN;
    tx_ctl->frame_header_length = MAC_80211_FRAME_LEN;
    tx_ctl->frame_header = (mac_ieee80211_frame_stru *)oal_netbuf_header(auth_rsp);
    tx_ctl->mac_head_type = 1;
    /* 设置认证类型IE */
    puc_frame[0] = (auth_type & 0x00FF);
    puc_frame[1] = (auth_type & 0xFF00) >> 8; /* 右移8位 */
    /* 将收到的transaction number + 1后复制给新的认证响应帧 */
    puc_frame[2] = ((auth_seq + 1) & 0x00FF);      /* 2 元素索引 */
    puc_frame[3] = ((auth_seq + 1) & 0xFF00) >> 8; /* 3 元素索引 右移8位 */
    /* 状态为初始化为成功 */
    puc_frame[4] = MAC_SUCCESSFUL_STATUSCODE; /* 4 元素索引 */
    puc_frame[5] = 0;                         /* 5 元素索引 */

    return auth_rsp_len;
}

/* ****************************************************************************
 功能描述  : 根据用户信息更新auth rsp帧的status code
**************************************************************************** */
hi_u32 hmac_update_status_code_by_user(const mac_vap_stru *mac_vap, hmac_tx_ctl_stru *tx_ctl, hi_u8 *puc_frame,
    hi_u16 auth_type, hi_u8 user_index)
{
    hmac_user_stru  *hmac_user = HI_NULL;
    hmac_vap_stru   *hmac_vap  = HI_NULL;

    /* 获取hmac user指针 */
    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_index);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_AUTH, "{hmac_update_status_code_by_user::hmac_user is NULL}");
        puc_frame[4] = MAC_UNSPEC_FAIL; /* 4 元素索引 */
        return HI_FAIL;
    }
    /* 获取hmac vap指针 */
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if ((hmac_vap == HI_NULL) || (hmac_vap->base_vap != mac_vap)) {
        oam_error_log1(0, OAM_SF_AUTH,
            "{hmac_update_status_code_by_user::vap is error, change user[idx=%d] state to BUTT!}",
            hmac_user->base_user->us_assoc_id);
        puc_frame[4] = MAC_UNSPEC_FAIL; /* 4 元素索引 */
        mac_user_set_asoc_state(hmac_user->base_user, MAC_USER_STATE_BUTT);
        return HI_FAIL;
    }
    tx_ctl->us_tx_user_idx = user_index;
    /* 判断算法是否支持 */
    if (hmac_encap_auth_rsp_support(hmac_vap, auth_type) != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_AUTH, "{hmac_update_status_code_by_user::auth type[%d] not support!}", auth_type);
        puc_frame[4] = MAC_UNSUPT_ALG; /* 4 元素索引 */
        hmac_user_set_asoc_state(hmac_vap->base_vap, hmac_user->base_user, MAC_USER_STATE_BUTT);
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 更新auth rsp帧的status code
**************************************************************************** */
hi_u32 hmac_update_auth_rsp_status_code(mac_vap_stru *mac_vap, oal_netbuf_stru *auth_rsp,
    const oal_netbuf_stru *auth_req, hi_u16 auth_rsp_len, hmac_auth_rsp_handle_stru *auth_rsp_handle)
{
    hmac_tx_ctl_stru *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(auth_rsp);
    hi_u8            *puc_data = (hi_u8 *)oal_netbuf_header(auth_rsp);
    hi_u8            *puc_frame = (hi_u8 *)(puc_data + MAC_80211_FRAME_LEN); /* 除MAC HDR的帧体起始位置 */
    hmac_mac_addr_stru auth_mac_addr = {0};
    hi_u32             ret;
    hi_u16             auth_type = mac_get_auth_alg(oal_netbuf_header(auth_req));
    hi_u16             auth_seq = mac_get_auth_seq_num(oal_netbuf_header(auth_req)); /* auth transaction number */
    hi_u8              zero_mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    hi_u8              mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    hi_u8              user_index = 0xff;   /* 默认为无效用户id 0xff */

    /* 获取STA的地址 */
    mac_get_address2(oal_netbuf_header(auth_req), WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN);
    if (memcmp(zero_mac_addr, mac_addr, WLAN_MAC_ADDR_LEN) == 0) {
        oam_warning_log0(0, OAM_SF_AUTH, "{hmac_update_auth_rsp_status_code::user mac is all 0 !}");
        puc_frame[4] = MAC_UNSPEC_FAIL; /* 4 元素索引 */
        tx_ctl->us_tx_user_idx = MAC_INVALID_USER_ID;
        tx_ctl->us_mpdu_len = auth_rsp_len;
        return HI_FAIL;
    }

    /* 获取用户idx */
    auth_mac_addr.mac_addr = mac_addr;
    auth_mac_addr.addr_len = WLAN_MAC_ADDR_LEN;
    ret = hmac_encap_auth_rsp_get_user_idx(mac_vap, auth_mac_addr, (WLAN_AUTH_TRASACTION_NUM_ONE == auth_seq),
        &auth_rsp_handle->auth_rsp_param.auth_resend, &user_index);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_AUTH, "{hmac_encap_auth_rsp::get_user_idx fail, error code[%d]!}", ret);
        puc_frame[4] = MAC_UNSPEC_FAIL; /* 4 元素索引 */
#ifdef _PRE_WLAN_FEATURE_MESH
        if (ret == HI_ERR_CODE_MESH_NOT_ACCEPT_PEER) {
            /* mesh用户满使用MAC_AP_FULL status code */
            puc_frame[4] = MAC_AP_FULL; /* 4 元素索引 */
        }
#endif
        tx_ctl->us_tx_user_idx = MAC_INVALID_USER_ID;
        tx_ctl->us_mpdu_len = auth_rsp_len;
        return HI_FAIL;
    }
    /* 申请user并刷新status code */
    tx_ctl->us_tx_user_idx = MAC_INVALID_USER_ID; /* 默认设置为无效ID */
    tx_ctl->us_mpdu_len = auth_rsp_len;
    /* 赋值出参 */
    auth_rsp_handle->auth_rsp_param.us_auth_type = auth_type;
    auth_rsp_handle->auth_rsp_fun = hmac_encap_auth_rsp_get_func(auth_seq);
    return hmac_update_status_code_by_user(mac_vap, tx_ctl, puc_frame, auth_type, user_index);
}

/* ****************************************************************************
 功能描述  : 根据auth回调的返回值进行auth后续处理
**************************************************************************** */
hi_u16 hmac_auth_rsp_handle_result(const hmac_vap_stru *hmac_vap, hmac_tx_ctl_stru *tx_ctl,
    hmac_ap_auth_process_code_enum_uint8 auth_proc_rst, hi_u8 *puc_frame, hi_u8 *puc_chtxt)
{
    hi_u16 auth_rsp_len = (hi_u16)tx_ctl->us_mpdu_len;

    /* 可直接获取hmac vap以及hmac user hmac_update_status_code_by_user已判空处理 */
    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(tx_ctl->us_tx_user_idx);
    if (hmac_user == HI_NULL) {
        return 0;
    }
    /*  根据返回的code进行后续处理 */
    switch (auth_proc_rst) {
        case HMAC_AP_AUTH_SEQ1_OPEN_ANY:
        case HMAC_AP_AUTH_SEQ3_OPEN_ANY:
            mac_user_init_key(hmac_user->base_user);
            break;

        case HMAC_AP_AUTH_SEQ1_WEP_NOT_RESEND:
            hmac_config_11i_add_wep_entry(hmac_vap->base_vap, WLAN_MAC_ADDR_LEN, hmac_user->base_user->user_mac_addr);
            hmac_mgmt_encap_chtxt(puc_frame, puc_chtxt, &auth_rsp_len, hmac_user);
            /* 为该用户启动一个定时器，超时认证失败 */
            frw_timer_create_timer(&hmac_user->mgmt_timer, hmac_mgmt_timeout_ap,
                (hi_u16)hmac_vap->base_vap->mib_info->wlan_mib_sta_config.dot11_authentication_response_time_out,
                hmac_user, HI_FALSE);
            hmac_user->base_user->key_info.cipher_type =
                mac_get_wep_type(hmac_vap->base_vap, mac_mib_get_wep_default_keyid(hmac_vap->base_vap));
            break;

        case HMAC_AP_AUTH_SEQ1_WEP_RESEND:
            /* seq为1 的认证帧重传 */
            hmac_mgmt_encap_chtxt(puc_frame, puc_chtxt, &auth_rsp_len, hmac_user);
            /* 重启超时定时器 */
            frw_timer_restart_timer(&hmac_user->mgmt_timer, hmac_user->mgmt_timer.timeout, HI_FALSE);
            break;

        case HMAC_AP_AUTH_SEQ3_WEP_COMPLETE:
            if (hmac_mgmt_is_challenge_txt_equal(puc_chtxt, hmac_user->ch_text) == HI_TRUE) {
                mac_user_set_asoc_state(hmac_user->base_user, MAC_USER_STATE_AUTH_COMPLETE);
                oal_mem_free(hmac_user->ch_text);
                hmac_user->ch_text = HI_NULL;
                /* cancel timer for auth */
                frw_timer_immediate_destroy_timer(&hmac_user->mgmt_timer);
            } else {
                puc_frame[4] = MAC_CHLNG_FAIL; /* 4 元素索引 */
                mac_user_set_asoc_state(hmac_user->base_user, MAC_USER_STATE_BUTT);
            }
            break;

        case HMAC_AP_AUTH_SEQ3_WEP_ASSOC:
            hmac_mgmt_encap_chtxt(puc_frame, puc_chtxt, &auth_rsp_len, hmac_user);
            /* 开启超时定时器 */
            frw_timer_create_timer(&hmac_user->mgmt_timer, hmac_mgmt_timeout_ap,
                (hi_u16)hmac_vap->base_vap->mib_info->wlan_mib_sta_config.dot11_authentication_response_time_out,
                hmac_user, HI_FALSE);
            break;

        case HMAC_AP_AUTH_DUMMY:
            break;

        default:
            mac_user_init_key(hmac_user->base_user);
            hmac_user->base_user->user_asoc_state = MAC_USER_STATE_BUTT;
            break;
    }

    tx_ctl->us_mpdu_len = (hi_u32)auth_rsp_len; /* 长度可能变更,重新刷新CB字段长度信息 */
    return auth_rsp_len;
}

/* ****************************************************************************
 功能描述  : 封装auth rsp帧
 输入参数  : [1]mac_vap
             [2]puc_chtxt
             [3]auth_req
 输出参数  : [1]auth_rsp
 返 回 值  : hi_u16
**************************************************************************** */
hi_u16 hmac_encap_auth_rsp(mac_vap_stru *mac_vap, oal_netbuf_stru *auth_rsp, const oal_netbuf_stru *auth_req,
    hi_u8 *puc_chtxt, hi_u16 chtxt_len)
{
    hmac_tx_ctl_stru *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(auth_rsp); /* rsp帧的tx cb字段 */
    hmac_user_stru   *hmac_user = HI_NULL;
    hmac_vap_stru    *hmac_vap  = HI_NULL;
    hi_u8            *puc_frame = HI_NULL;
    hi_u16           auth_rsp_len;
    hmac_ap_auth_process_code_enum_uint8 auth_proc_rst; /* 认证方法 */
    hmac_auth_rsp_handle_stru auth_rsp_handle;

    hi_unref_param(chtxt_len);
    /* 组auth响应帧初值 状态码为SUCCESS */
    auth_rsp_len = hmac_encap_auth_rsp_body(mac_vap, auth_rsp, auth_req);
    if (auth_rsp_len == 0) {
        return auth_rsp_len;
    }
    /* 刷新auth响应帧状态码的值 失败后不继续处理直接返回当前帧长 */
    if (hmac_update_auth_rsp_status_code(mac_vap, auth_rsp, auth_req, auth_rsp_len, &auth_rsp_handle) != HI_SUCCESS) {
        return auth_rsp_len;
    }
    /* 执行成功后 可直接获取hmac vap以及hmac user hmac_update_status_code_by_user已判空处理 */
    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(tx_ctl->us_tx_user_idx);
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_user == HI_NULL || hmac_vap == HI_NULL) {
        return 0;
    }
    /*  初始化处理参数 */
    auth_rsp_handle.auth_rsp_param.is_wep_allowed = mac_is_wep_allowed(mac_vap);
    auth_rsp_handle.auth_rsp_param.user_asoc_state = hmac_user->base_user->user_asoc_state;

    /*  处理seq1或者seq3 */
    puc_frame = (hi_u8 *)oal_netbuf_header(auth_rsp) + MAC_80211_FRAME_LEN; /* 除MAC HDR的帧体起始位置 */
    if (auth_rsp_handle.auth_rsp_fun != HI_NULL) {
        auth_proc_rst = auth_rsp_handle.auth_rsp_fun(&auth_rsp_handle.auth_rsp_param, &puc_frame[4], /* 4 元素索引 */
            &hmac_user->base_user->user_asoc_state);
        /* 清空 HMAC层TID信息 */
        hmac_tid_clear(mac_vap, hmac_user);
    } else {
        auth_proc_rst = HMAC_AP_AUTH_BUTT;
        mac_user_set_asoc_state(hmac_user->base_user, MAC_USER_STATE_BUTT);
        puc_frame[4] = MAC_AUTH_SEQ_FAIL; /* 4 元素索引 */
    }
    oam_warning_log1(mac_vap->vap_id, OAM_SF_AUTH, "{hmac_encap_auth_rsp::ul_auth_proc_rst:%d}", auth_proc_rst);

    /*  根据返回的code进行后续处理 */
    if (auth_proc_rst == HMAC_AP_AUTH_SEQ3_WEP_COMPLETE) {
        puc_chtxt = mac_get_auth_ch_text(oal_netbuf_header(auth_req)); /* seq3提取req的挑战字符串 */
    }
    puc_frame = (hi_u8 *)oal_netbuf_header(auth_rsp) + MAC_80211_FRAME_LEN; /* 取帧体起始指针 */
    auth_rsp_len = hmac_auth_rsp_handle_result(hmac_vap, tx_ctl, auth_proc_rst, puc_frame, puc_chtxt);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* dmac offload架构下，同步user关联状态信息到dmac */
    if (hmac_config_user_asoc_state_syn(hmac_vap->base_vap, hmac_user->base_user) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_AUTH, "{hmac_ap_rx_auth_req::user_asoc_state_syn failed.}");
    }
#endif
    return auth_rsp_len;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : 封装mesh peering open帧
 输入参数  : 1. vap指针2.hi_u8 *puc_data 3.mac_mesh_action_data_stru *st_action_data
 返 回 值  :    帧长度
  1.日    期   : 2019年2月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_encap_mesh_peering_open_frame(mac_vap_stru *mac_vap, hi_u8 *data, const mac_action_data_stru *action)
{
    hi_u8 ie_len = 0;
    hi_u8 *puc_frame_origin = data; /* 保存起始地址，便于计算长度 */

    if (mac_vap->mib_info == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    /* *********************************************************************** */
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                Set the fields in the frame header                     */
    /* *********************************************************************** */
    /* 设置 Frame Control field */
    mac_hdr_set_frame_control(data, WLAN_FC0_SUBTYPE_ACTION);

    /* 设置 DA address1: STA MAC地址 */
    /* 设置 SA address2: dot11MACAddress */
    /* 设置 DA address3::BSSID */
    if ((memcpy_s(data + WLAN_HDR_ADDR1_OFFSET, WLAN_MAC_ADDR_LEN, action->puc_dst, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(data + WLAN_HDR_ADDR2_OFFSET, WLAN_MAC_ADDR_LEN,
                  mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(data + WLAN_HDR_ADDR3_OFFSET, WLAN_MAC_ADDR_LEN, action->puc_bssid, WLAN_MAC_ADDR_LEN) != EOK)) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_peering_open_frame::memcpy_s fail.}");
        return 0;
    }

    data += MAC_80211_FRAME_LEN;
    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*              Mesh peering open Frame - Frame Body                     */
    /* --------------------------------------------------------------------- */
    /* |Category|action code | Capability Information | Supported Rates |    */
    /* --------------------------------------------------------------------- */
    /* |1             |1               |2                |3-10             | */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* |Externed Surpported rates|RSN | HT Capabilities | Extended Capabilities */
    /* --------------------------------------------------------------------- */
    /* |3-257                    |4-256                 | */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* | RSN   | HT Capabilities | Extended Capabilities  | */
    /* --------------------------------------------------------------------- */
    /* |36-256 |3               |28               |3-8                     | */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* | MESH Element| MIC |Authenticated Mesh Peering Exchange| */
    /* --------------------------------------------------------------------- */
    /* |7-257  |X    |                                                       */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* 设置Category 和Action Code */
    *data = MAC_ACTION_CATEGORY_SELF_PROTECTED;
    *(data + 1) = MAC_SP_ACTION_MESH_PEERING_OPEN;
    data += 2; /* 自增2 */

    /* 设置Capability Info Field */
    mac_set_cap_info_ap((hi_void *)mac_vap, data);
    data += MAC_CAP_INFO_LEN;

    /* 设置 Supported Rates IE */
    mac_set_supported_rates_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置 Extended Supported Rates IE */
    mac_set_exsup_rates_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置RSN IE */
    mac_set_rsn_mesh_ie_authenticator((hi_void *)mac_vap, data, WLAN_FC0_SUBTYPE_PROBE_RSP, &ie_len);
    data += ie_len;

    /* 设置 HT-Capabilities Information IE */
    mac_set_ht_capabilities_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置 HT-Operation Information IE */
    mac_set_ht_opern_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 填充wmm信息 */
    mac_set_wmm_params_ie(mac_vap, data, mac_vap->mib_info->wlan_mib_sta_config.dot11_qos_option_implemented, &ie_len);
    data += ie_len;

    /* 填充bss load信息 */
    mac_set_bssload_ie(mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置 Extended Capabilities Information IE */
    mac_set_ext_capabilities_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置Hisi-Mesh私有信息 */
    mac_set_hisi_mesh_optimization_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 填充WPS信息 */
    if (action->data_len > 0) {
        /* wpa带下来的action puc data携带category和Action code，共两个字节,用于驱动判断下发的帧类型，无需填充 */
        /* Mesh peering open action frame 字段 */
        /* Bytes |1      |1     |...| */
        /* ie    |action category|action code|...| */
        if (memcpy_s(data, action->data_len - 2, action->puc_data + 2, action->data_len - 2) != EOK) { /* 2 计算偏差 */
            oam_error_log0(0, 0, "hmac_encap_mesh_peering_open_frame:: st_action_data->puc_data memcpy_s fail.");
            return HI_FAIL;
        }
        data += (action->data_len - 2); /* 减去2 */
    }

    return (hi_u32)(data - puc_frame_origin);
}

/* ****************************************************************************
 功能描述  : 封装mesh peering confirm帧
 输入参数  : 1. vap指针2.hmac_user指针3.hi_u8 *puc_data 4. mac_mesh_action_data_stru *st_action_data
 返 回 值  :帧长度
 修改历史      :
  1.日    期   : 2019年2月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_encap_mesh_peering_confirm_frame(mac_vap_stru *mac_vap, hi_u8 *data,
    const mac_action_data_stru *action_data)
{
    hi_u8 ie_len = 0;

    /* 保存起始地址，便于计算长度 */
    hi_u8 *puc_frame_origin = data;

    if (mac_vap->mib_info == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    /* *********************************************************************** */
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                Set the fields in the frame header                     */
    /* *********************************************************************** */
    /* 设置 Frame Control field */
    mac_hdr_set_frame_control(data, WLAN_FC0_SUBTYPE_ACTION);

    /* 设置 DA address1: 远端节点MAC地址 */
    /* 设置 SA address2: dot11MACAddress */
    /* 设置 DA address3::BSSID */
    if ((memcpy_s(data + WLAN_HDR_ADDR1_OFFSET, WLAN_MAC_ADDR_LEN, action_data->puc_dst, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(data + WLAN_HDR_ADDR2_OFFSET, WLAN_MAC_ADDR_LEN,
                  mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(data + WLAN_HDR_ADDR3_OFFSET, WLAN_MAC_ADDR_LEN, action_data->puc_bssid, WLAN_MAC_ADDR_LEN) != EOK)) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_peering_confirm_frame::memcpy_s fail.}");
        return 0;
    }
    data += MAC_80211_FRAME_LEN;
    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*              Mesh peering confirm Frame - Frame Body                   */
    /* --------------------------------------------------------------------- */
    /* |Category|action code | Capability Information |AID| Supported Rates | */
    /* --------------------------------------------------------------------- */
    /* */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* |Externed Surpported rates|RSN | HT Capabilities | Extended Capabilities */
    /* --------------------------------------------------------------------- */
    /* */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* | MESH Element| MIC |Authenticated Mesh Peering Exchange| */
    /* --------------------------------------------------------------------- */
    /* *********************************************************************** */
    /* 设置Category 和Action Code */
    *data = MAC_ACTION_CATEGORY_SELF_PROTECTED;
    *(data + 1) = MAC_SP_ACTION_MESH_PEERING_CONFIRM;
    data += 2; /* 自增2 */

    /* 设置Capability Info Field */
    mac_set_cap_info_ap((hi_void *)mac_vap, data);
    data += MAC_CAP_INFO_LEN;

    /* 设置Mesh AID ，直接由wpa 下发 */
    if (memcpy_s(data, MAC_AID_LEN, action_data->puc_data + 2, MAC_AID_LEN) != EOK) { /* 2 用于计算偏差 */
        oam_error_log0(0, OAM_SF_CFG, "hmac_encap_mesh_peering_confirm_frame::st_action_data->puc_data memcpy_s fail.");
        return HI_FAIL;
    }
    data += MAC_AID_LEN;

    /* 设置 Supported Rates IE */
    mac_set_supported_rates_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置 Extended Supported Rates IE */
    mac_set_exsup_rates_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置RSN IE */
    mac_set_rsn_mesh_ie_authenticator((hi_void *)mac_vap, data, WLAN_FC0_SUBTYPE_PROBE_RSP, &ie_len);
    data += ie_len;

    /* 设置 HT-Capabilities Information IE */
    mac_set_ht_capabilities_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置 HT-Operation Information IE */
    mac_set_ht_opern_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 填充wmm信息 */
    mac_set_wmm_params_ie(mac_vap, data, mac_vap->mib_info->wlan_mib_sta_config.dot11_qos_option_implemented, &ie_len);
    data += ie_len;

    /* 填充bss load信息 */
    mac_set_bssload_ie(mac_vap, data, &ie_len);
    data += ie_len;

    /* 设置 Extended Capabilities Information IE */
    mac_set_ext_capabilities_ie((hi_void *)mac_vap, data, &ie_len);
    data += ie_len;

    /* 填充WPS信息 */
    /* Mesh peering confirm action frame 字段 */
    /* Bytes |1       |1          |2|...| */
    /* ie    |action category|action code|aid|...| */
    if (action_data->data_len > 0) {
        if (memcpy_s(data, action_data->data_len - 4, action_data->puc_data + 4, /* 4 用于计算偏差 */
            action_data->data_len - 4) != EOK) {                                 /* 4 用于计算偏差 */
            oam_error_log0(0, 0, "hmac_encap_mesh_peering_confirm_frame::action_data->puc_data memcpy_s fail.");
            return HI_FAIL;
        }
        data += action_data->data_len - 4; /* 4 用于计算偏差 */
    }

    return (hi_u32)(data - puc_frame_origin);
}

/* ****************************************************************************
 功能描述  : 封装mesh peering close帧
 输入参数  : 1. vap指针2.hi_u8 *puc_data 3.hisi_action_data_stru *st_action_data
 返 回 值  :    帧长度
  1.日    期   : 2019年2月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_encap_mesh_peering_close_frame(const mac_vap_stru *mac_vap, hi_u8 *puc_data,
    const mac_action_data_stru *action_data)
{
    hi_u8 *puc_frame_origin = HI_NULL;
    hi_u32 us_frame_len;

    /* 保存起始地址，便于计算长度 */
    puc_frame_origin = puc_data;

    if (mac_vap->mib_info == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }
    /* *********************************************************************** */
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                Set the fields in the frame header                     */
    /* *********************************************************************** */
    /* 设置 Frame Control field */
    mac_hdr_set_frame_control(puc_data, WLAN_FC0_SUBTYPE_ACTION);

    /* 设置 DA address1: STA MAC地址 */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR1_OFFSET, WLAN_MAC_ADDR_LEN,
                 action_data->puc_dst, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_peering_close_frame::memcpy_s fail.}");
        return 0;
    }
    /* 设置 SA address2: dot11MACAddress */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR2_OFFSET, WLAN_MAC_ADDR_LEN,
        mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_peering_close_frame::memcpy_s fail.}");
        return 0;
    }
    /* 设置 DA address3::BSSID */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR3_OFFSET, WLAN_MAC_ADDR_LEN, action_data->puc_bssid, WLAN_MAC_ADDR_LEN) !=
        EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_peering_close_frame::memcpy_s fail.}");
        return 0;
    }
    puc_data += MAC_80211_FRAME_LEN;
    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*              Mesh peering close Frame - Frame Body                    */
    /* --------------------------------------------------------------------- */
    /* |Category|action code | Mesh Element | MIC Element |Authenticated Mesh Peering Exchange */
    /* --------------------------------------------------------------------- */
    /* *********************************************************************** */
    /* 设置Category 和Action Code */
    *puc_data = MAC_ACTION_CATEGORY_SELF_PROTECTED;
    *(puc_data + 1) = MAC_SP_ACTION_MESH_PEERING_CLOSE;
    puc_data += 2; /* 自增2 */

    /* 填充WPS信息 */
    if (action_data->data_len > 0) {
        if (memcpy_s(puc_data, action_data->data_len - 2, action_data->puc_data + 2, /* 2 用于计算偏差 */
            action_data->data_len - 2) != EOK) {                                     /* 2 用于计算偏差 */
            oam_error_log0(0, OAM_SF_CFG, "hmac_encap_mesh_peering_close_frame:: puc_data memcpy_s fail.");
            return HI_FAIL;
        }
        puc_data += action_data->data_len - 2; /* 2 用于计算偏差 */
    }

    us_frame_len = (hi_u32)(puc_data - puc_frame_origin);

    return us_frame_len;
}

/* ****************************************************************************
 功能描述  : 封装MESH_GROUP_KEY_INFORM帧
 输入参数  : 1. vap指针2.hi_u8 *puc_data 3.hisi_action_data_stru *st_action_data
 返 回 值  :    帧长度
  1.日    期   : 2019年6月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_encap_mesh_group_key_inform_frame(const mac_vap_stru *mac_vap, hi_u8 *puc_data,
    const mac_action_data_stru *action_data)
{
    hi_u8 *puc_frame_origin = HI_NULL;
    hi_u32 us_frame_len;

    /* 保存起始地址，便于计算长度 */
    puc_frame_origin = puc_data;

    if (mac_vap->mib_info == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }

    /* *********************************************************************** */
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                Set the fields in the frame header                     */
    /* *********************************************************************** */
    /* 设置 Frame Control field */
    mac_hdr_set_frame_control(puc_data, WLAN_FC0_SUBTYPE_ACTION);

    /* 设置 DA address1: STA MAC地址 */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR1_OFFSET, WLAN_MAC_ADDR_LEN,
                 action_data->puc_dst, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_group_key_inform_frame::memcpy_s fail.}");
        return 0;
    }
    /* 设置 SA address2: dot11MACAddress */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR2_OFFSET, WLAN_MAC_ADDR_LEN,
        mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_group_key_inform_frame::memcpy_s fail.}");
        return 0;
    }
    /* 设置 DA address3::BSSID */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR3_OFFSET, WLAN_MAC_ADDR_LEN, action_data->puc_bssid, WLAN_MAC_ADDR_LEN) !=
        EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_group_key_inform_frame::memcpy_s fail.}");
        return 0;
    }
    puc_data += MAC_80211_FRAME_LEN;
    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*              Mesh GROUP KEY INFORM Frame - Frame Body                 */
    /* --------------------------------------------------------------------- */
    /* |Category|action code | MIC Element |Authenticated Mesh Peering Exchange */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* *********************************************************************** */
    /* 设置Category 和Action Code */
    *puc_data = MAC_ACTION_CATEGORY_SELF_PROTECTED;
    *(puc_data + 1) = MAC_SP_ACTION_MESH_GROUP_KEY_INFORM;
    puc_data += 2; /* 自增2 */

    /* 填充WPS信息 */
    if (action_data->data_len > 0) {
        if (memcpy_s(puc_data, action_data->data_len - 2, action_data->puc_data + 2, /* 2 用于计算偏差 */
            action_data->data_len - 2) != EOK) {                                     /* 2 用于计算偏差 */
            oam_error_log0(0, OAM_SF_CFG, "hmac_encap_mesh_group_key_inform_frame:: puc_data memcpy_s fail.");
            return HI_FAIL;
        }
        puc_data += action_data->data_len - 2; /* 2 用于计算偏差 */
    }

    us_frame_len = (hi_u32)(puc_data - puc_frame_origin);

    return us_frame_len;
}

/* ****************************************************************************
 功能描述  : 封装MESH_GROUP_KEY_ACK帧
 输入参数  : 1. vap指针2.hi_u8 *puc_data 3.hisi_action_data_stru *st_action_data
 返 回 值  :    帧长度
  1.日    期   : 2019年6月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_encap_mesh_group_key_ack_frame(const mac_vap_stru *mac_vap, hi_u8 *puc_data,
    const mac_action_data_stru *action_data)
{
    hi_u8 *puc_frame_origin = HI_NULL;
    hi_u32 us_frame_len;

    /* 保存起始地址，便于计算长度 */
    puc_frame_origin = puc_data;

    if (mac_vap->mib_info == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }
    /* *********************************************************************** */
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                Set the fields in the frame header                     */
    /* *********************************************************************** */
    /* 设置 Frame Control field */
    mac_hdr_set_frame_control(puc_data, WLAN_FC0_SUBTYPE_ACTION);

    /* 设置 DA address1: STA MAC地址 */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR1_OFFSET, WLAN_MAC_ADDR_LEN,
        action_data->puc_dst, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_group_key_ack_frame::memcpy_s fail.}");
        return 0;
    }
    /* 设置 SA address2: dot11MACAddress */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR2_OFFSET, WLAN_MAC_ADDR_LEN,
        mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_group_key_ack_frame::memcpy_s fail.}");
        return 0;
    }
    /* 设置 DA address3::BSSID */
    if (memcpy_s(puc_data + WLAN_HDR_ADDR3_OFFSET, WLAN_MAC_ADDR_LEN, action_data->puc_bssid, WLAN_MAC_ADDR_LEN) !=
        EOK) {
        oam_error_log0(0, 0, "{hmac_encap_mesh_group_key_ack_frame::memcpy_s fail.}");
        return 0;
    }
    puc_data += MAC_80211_FRAME_LEN;
    /* ************************************************************************** */
    /*                Set the contents of the frame body                        */
    /* ************************************************************************** */
    /* ************************************************************************** */
    /*              Mesh GROUP KEY ACK Frame - Frame Body                       */
    /* ---------------------------------------------------------------------    */
    /* |Category|action code | MIC Element |Authenticated Mesh Peering Exchange */
    /* ---------------------------------------------------------------------    */
    /* ---------------------------------------------------------------------    */
    /* ************************************************************************** */
    /* 设置Category 和Action Code */
    *puc_data = MAC_ACTION_CATEGORY_SELF_PROTECTED;
    *(puc_data + 1) = MAC_SP_ACTION_MESH_GROUP_KEY_ACK;
    puc_data += 2; /* 自增2 */

    /* 填充WPS信息 */
    if (action_data->data_len > 0) {
        if (memcpy_s(puc_data, action_data->data_len - 2, action_data->puc_data + 2, /* 2 用于计算偏差 */
            action_data->data_len - 2) != EOK) {                                     /* 2 用于计算偏差 */
            oam_error_log0(0, OAM_SF_CFG, "hmac_encap_mesh_group_key_ack_frame:: puc_data memcpy_s fail.");
            return HI_FAIL;
        }
        puc_data += action_data->data_len - 2; /* 2 用于计算偏差 */
    }

    us_frame_len = (hi_u32)(puc_data - puc_frame_origin);

    return us_frame_len;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
