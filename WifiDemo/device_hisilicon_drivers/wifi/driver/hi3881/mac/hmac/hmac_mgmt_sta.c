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
#include "wlan_mib.h"
#include "mac_frame.h"
#include "mac_ie.h"
#include "mac_regdomain.h"
#include "mac_user.h"
#include "mac_vap.h"
#include "mac_device.h"
#include "hmac_device.h"
#include "hmac_user.h"
#include "hmac_mgmt_sta.h"
#include "hmac_fsm.h"
#include "hmac_rx_data.h"
#include "hmac_chan_mgmt.h"
#include "hmac_mgmt_bss_comm.h"
#include "hmac_encap_frame_sta.h"
#include "hmac_sme_sta.h"
#include "hmac_scan.h"
#include "hmac_11i.h"
#include "hmac_config.h"
#include "hmac_ext_if.h"
#include "hmac_event.h"
#include "hmac_blockack.h"
#include "hcc_hmac_if.h"
#include "frw_timer.h"
#ifdef _PRE_WLAN_FEATURE_WAPI
#include "hmac_wapi.h"
#endif
#include "wal_customize.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : sta等待管理帧超时处理函数
 修改历史      :
  1.日    期   : 2013年7月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_mgmt_timeout_sta(hi_void *arg)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    hmac_mgmt_timeout_param_stru *timeout_param = HI_NULL;

    timeout_param = (hmac_mgmt_timeout_param_stru *)arg;
    hmac_vap = hmac_vap_get_vap_stru(timeout_param->vap_id);
    if ((hmac_vap == HI_NULL) || (hmac_vap->base_vap == HI_NULL)) {
        return HI_ERR_CODE_PTR_NULL;
    }

    frw_timer_immediate_destroy_timer(&(hmac_vap->mgmt_timer));

    switch (hmac_vap->base_vap->vap_state) {
        case MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2:
        case MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4:
            return hmac_sta_auth_timeout(hmac_vap);
        case MAC_VAP_STATE_STA_WAIT_ASOC:
            return hmac_sta_wait_asoc_timeout(hmac_vap);
        default:
            return HI_SUCCESS;
    }
}

/* ****************************************************************************
 功能描述  : 在join之前更新协议相关的参数
 修改历史      :
  1.日    期   : 2013年10月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_update_join_req_params_prot_sta(hmac_vap_stru *hmac_vap, const hmac_join_req_stru *join_req)
{
    if (hmac_vap->base_vap->mib_info->wlan_mib_sta_config.dot11_desired_bss_type == WLAN_MIB_DESIRED_BSSTYPE_INFRA) {
#ifdef _PRE_WLAN_FEATURE_MESH
        if (join_req->bss_dscr.is_hisi_mesh == HI_TRUE) {
            hmac_vap->wmm_cap = HI_TRUE;
        } else {
            hmac_vap->wmm_cap = join_req->bss_dscr.wmm_cap;
            mac_vap_set_uapsd_cap(hmac_vap->base_vap, join_req->bss_dscr.uapsd_cap);
        }
#else
        hmac_vap->wmm_cap = join_req->bss_dscr.wmm_cap;
        mac_vap_set_uapsd_cap(hmac_vap->base_vap, join_req->bss_dscr.uapsd_cap);
#endif
    }
}

/* ****************************************************************************
 功能描述  : 判断是否支持某种速率
 修改历史      :
  1.日    期   : 2016年3月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_is_rate_support(const hi_u8 *puc_rates, hi_u8 rate_num, hi_u8 rate)
{
    hi_u8 rate_is_supp = HI_FALSE;
    hi_u8 loop;

    if (puc_rates == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_is_rate_support::puc_rates null}");
        return HI_ERR_CODE_PTR_NULL;
    }

    for (loop = 0; loop < rate_num; loop++) {
        if ((puc_rates[loop] & 0x7F) == rate) {
            rate_is_supp = HI_TRUE;
            break;
        }
    }

    return rate_is_supp;
}

/* ****************************************************************************
 功能描述  : 是否支持11g速率
 修改历史      :
  1.日    期   : 2016年3月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_is_support_11grate(const hi_u8 *puc_rates, hi_u8 rate_num)
{
    if (puc_rates == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_is_rate_support::puc_rates null}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if ((HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x0C)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x12)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x18)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x24)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x30)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x48)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x60)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x6C))) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 是否支持11b速率
 修改历史      :
  1.日    期   : 2016年3月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_is_support_11brate(const hi_u8 *puc_rates, hi_u8 rate_num)
{
    if (puc_rates == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_is_support_11brate::puc_rates null}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if ((HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x02)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x04)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x0B)) ||
        (HI_TRUE == hmac_is_rate_support(puc_rates, rate_num, 0x16))) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 获取用户的协议模式
 修改历史      :
  1.日    期   : 2014年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_get_user_protocol(mac_bss_dscr_stru *bss_dscr, wlan_protocol_enum_uint8 *protocol_mode)
{
    /* 入参保护 */
    if (bss_dscr == HI_NULL || protocol_mode == HI_NULL) {
        oam_error_log2(0, OAM_SF_SCAN, "{hmac_sta_get_user_protocol::param null,%p %p.}", (uintptr_t)bss_dscr,
            (uintptr_t)protocol_mode);
        return HI_ERR_CODE_PTR_NULL;
    }

    if (bss_dscr->ht_capable == HI_TRUE) {
        *protocol_mode = WLAN_HT_MODE;
    } else {
        if (hmac_is_support_11grate(bss_dscr->auc_supp_rates, bss_dscr->num_supp_rates) == HI_TRUE) {
            *protocol_mode = WLAN_LEGACY_11G_MODE;
            if (hmac_is_support_11brate(bss_dscr->auc_supp_rates, bss_dscr->num_supp_rates) == HI_TRUE) {
                *protocol_mode = WLAN_MIXED_ONE_11G_MODE;
            }
        } else if (hmac_is_support_11brate(bss_dscr->auc_supp_rates, bss_dscr->num_supp_rates) == HI_TRUE) {
            *protocol_mode = WLAN_LEGACY_11B_MODE;
        } else {
            oam_warning_log0(0, OAM_SF_ANY, "{hmac_sta_get_user_protocol::get user protocol failed.}");
            return HI_FAIL;
        }
    }

    return HI_SUCCESS;
}

hi_u8 hmac_sta_need_update_protocol(wlan_protocol_enum_uint8 vap_protocol, wlan_protocol_enum_uint8 user_protocol)
{
    if (((vap_protocol == WLAN_MIXED_ONE_11G_MODE) && (user_protocol == WLAN_LEGACY_11B_MODE)) ||
        ((vap_protocol == WLAN_HT_MODE) &&
        ((user_protocol == WLAN_LEGACY_11B_MODE) || (user_protocol == WLAN_MIXED_ONE_11G_MODE)))) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 根据带宽获取转换到us的放大倍数(转换为移位数)
 修改历史      :
  1.日    期   : 2019年7月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_dbac_get_scale_by_bw(hi_u8 bandwidth)
{
    hi_u8 scale = 0;

    if (bandwidth == WLAN_BAND_WIDTH_5M) {
        scale = 2; /* 5M是4倍，对应位移数2 */
    } else if (bandwidth == WLAN_BAND_WIDTH_10M) {
        scale = 1; /* 10M是2倍，对应位移数1 */
    } else {
        scale = 0;
    }
    return scale;
}

/* ****************************************************************************
 功能描述  : 根据join_request帧更新sta关联的信道相关信息
**************************************************************************** */
static hi_u32 hmac_sta_update_join_channel(mac_vap_stru *mac_vap, const hmac_join_req_stru *join_req)
{
    mac_device_stru *mac_dev = HI_NULL;
    hi_u8 bcn_scale;

    mac_dev = mac_res_get_dev();
    /* 设置BSSID */
    mac_vap_set_bssid(mac_vap, join_req->bss_dscr.auc_bssid, WLAN_MAC_ADDR_LEN);
    /* 更新mib库对应的ssid */
    if (memcpy_s(mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_desired_ssid, WLAN_SSID_MAX_LEN,
        join_req->bss_dscr.ac_ssid, WLAN_SSID_MAX_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_sta_update_join_channel:: ac_ssid memcpy_s fail.");
        return HI_FAIL;
    }
    mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_desired_ssid[WLAN_SSID_MAX_LEN - 1] = '\0';

    /* 根据带宽转换beacon周期为单位TU */
    bcn_scale = hmac_dbac_get_scale_by_bw(mac_vap->channel.en_bandwidth);
    /* 更新mib库对应的dot11BeaconPeriod值 */
    mac_vap->mib_info->wlan_mib_sta_config.dot11_beacon_period =
        ((hi_u32)(join_req->bss_dscr.us_beacon_period)) << bcn_scale;
    /* 更新mib库对应的ul_dot11CurrentChannel值 */
    mac_vap_set_current_channel(mac_vap, join_req->bss_dscr.channel.band, join_req->bss_dscr.channel.chan_number);

    /* 更新频带、主20MHz信道号，与AP通信 DMAC切换信道时直接调用 */
    if ((mac_vap->channel.en_bandwidth != WLAN_BAND_WIDTH_5M) &&
        (mac_vap->channel.en_bandwidth != WLAN_BAND_WIDTH_10M)) {  /* 仅在非窄带时更新 */
        mac_vap->channel.en_bandwidth =
            hmac_sta_get_band(mac_dev->bandwidth_cap, join_req->bss_dscr.channel_bandwidth);
    }
    mac_vap->channel.chan_number = join_req->bss_dscr.channel.chan_number;
    mac_vap->channel.band = join_req->bss_dscr.channel.band;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 根据join_request帧更新sta关联的协议模式相关信息
**************************************************************************** */
static hi_u32 hmac_sta_update_join_protocol(const hmac_vap_stru *hmac_vap, hmac_join_req_stru *join_req)
{
    mac_cfg_mode_param_stru cfg_mode = { 0 };
    mac_vap_stru *mac_vap = hmac_vap->base_vap;

    /* Mesh beacon帧中不带速率集，无法获取对应协议模式，规格默认Mesh为11BGN，不会出现STA协议模式比关联AP高的情形 */
    if (join_req->bss_dscr.is_hisi_mesh == HI_FALSE) {
        if (hmac_sta_get_user_protocol(&join_req->bss_dscr, &cfg_mode.protocol) != HI_SUCCESS) {
            oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_update_join_req_params::hmac_sta_get_user_protocol fail.}");
            return HI_FAIL;
        }
        /* STA的协议模式比要关联的AP高，则更新mib库中对应的相关能力 */
        if (hmac_sta_need_update_protocol(mac_vap->protocol, cfg_mode.protocol) == HI_TRUE) {
            /* 关联前先将STA模式恢复成设置前，防止之前关联跟随AP时模式降低不恢复 */
            mac_vap->protocol = hmac_vap->preset_para.protocol;
            mac_vap->mib_info->wlan_mib_sta_config.dot11_high_throughput_option_implemented =
                join_req->bss_dscr.ht_capable;
            mac_vap->mib_info->phy_ht.dot11_ldpc_coding_option_implemented = (join_req->bss_dscr.ht_ldpc &&
                mac_vap->mib_info->phy_ht.dot11_ldpc_coding_option_activated);
            mac_vap->mib_info->phy_ht.dot11_tx_stbc_option_implemented = (join_req->bss_dscr.ht_stbc &&
                mac_vap->mib_info->phy_ht.dot11_tx_stbc_option_activated);

            /* 关联2G AP，且2ght40禁止位为1时，不学习AP的HT 40能力 */
            mac_mib_set_forty_mhz_operation_implemented(mac_vap, HI_FALSE);
            if (!(mac_vap->channel.band == WLAN_BAND_2G && mac_vap->cap_flag.disable_2ght40) &&
                (join_req->bss_dscr.bw_cap != WLAN_BW_CAP_20M)) {
                mac_mib_set_forty_mhz_operation_implemented(mac_vap, HI_TRUE);
            }

            /* 根据要加入AP的协议模式更新STA侧速率集 */
            cfg_mode.band = join_req->bss_dscr.channel.band;
            cfg_mode.en_bandwidth = mac_vap->channel.en_bandwidth;
            cfg_mode.channel_idx = join_req->bss_dscr.channel.chan_number;
            if (hmac_config_sta_update_rates(mac_vap, &cfg_mode) != HI_SUCCESS) {
                oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_update_join_protocol::hmac_config_sta_update_rates fail.}");
                return HI_FAIL;
            }
        }
    }

    /* wapi 需要降协议 */
    if (join_req->bss_dscr.wapi) {
        hmac_update_pcip_policy_prot_supplicant(mac_vap, WLAN_80211_CIPHER_SUITE_WAPI);
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_sta_update_join_protocol::wapi prot fall!}");
    }

    /* 更新mib库对应的加密相关值 */
    if (hmac_update_current_join_req_parms_11i(mac_vap, &join_req->bss_dscr.bss_sec_info) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_update_join_protocol::update security parameter failed.}");
        return HI_FAIL;
    }

    cfg_mode.protocol = mac_vap->protocol;
    cfg_mode.band = mac_vap->channel.band;
    cfg_mode.en_bandwidth = mac_vap->channel.en_bandwidth;
    cfg_mode.channel_idx = join_req->bss_dscr.channel.chan_number;

    return hmac_config_sta_update_rates(mac_vap, &cfg_mode);
}

/* ****************************************************************************
 功能描述  : 发送关联请求事件到dmac
**************************************************************************** */
static hi_u32 hmac_sta_dispatch_join_req(const mac_vap_stru *mac_vap, const hmac_join_req_stru *join_req)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    dmac_ctx_join_req_set_reg_stru *reg_params = HI_NULL;

    /* 抛事件到DMAC, 申请事件内存 */
    event_mem = frw_event_alloc(sizeof(dmac_ctx_join_req_set_reg_stru));
    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_update_join_req_params::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_SET_REG,
        sizeof(dmac_ctx_join_req_set_reg_stru), FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    reg_params = (dmac_ctx_join_req_set_reg_stru *)event->auc_event_data;
    /* 设置需要写入寄存器的BSSID信息 */
    if (memcpy_s(reg_params->auc_bssid, WLAN_MAC_ADDR_LEN, join_req->bss_dscr.auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(0, 0, "{alg_autorate_init_rate_policy::copy bssid failed!}");
        return HI_FAIL;
    }

    /* 填写信道相关信息 */
    reg_params->current_channel.chan_number = mac_vap->channel.chan_number;
    reg_params->current_channel.band = mac_vap->channel.band;
    reg_params->current_channel.en_bandwidth = mac_vap->channel.en_bandwidth;
    reg_params->current_channel.idx = mac_vap->channel.idx;

    /* 设置beaocn period信息 */
    reg_params->us_beacon_period = (join_req->bss_dscr.us_beacon_period);
    /* 同步FortyMHzOperationImplemented */
    reg_params->dot11_forty_m_hz_operation_implemented = mac_mib_get_forty_mhz_operation_implemented(mac_vap);
    /* 设置beacon filter关闭 */
    reg_params->beacon_filter = HI_FALSE;
    /* 设置no frame filter打开 */
    reg_params->non_frame_filter = HI_TRUE;
    /* 下发ssid */
    if (memcpy_s(reg_params->auc_ssid, WLAN_SSID_MAX_LEN, join_req->bss_dscr.ac_ssid, WLAN_SSID_MAX_LEN) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(0, OAM_SF_CFG, "hmac_sta_update_join_req_params:: ac_ssid memcpy_s fail.");
        return HI_FAIL;
    }
    reg_params->auc_ssid[WLAN_SSID_MAX_LEN - 1] = '\0';

    /* 分发事件 */
    hcc_hmac_tx_control_event(event_mem, sizeof(dmac_ctx_join_req_set_reg_stru));
    frw_event_free(event_mem);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 根据join_request帧更新mib信息和填写相应寄存器
 输入参数  : hmac_vap_stru      *pst_hmac_vap,
             hmac_join_req_stru *pst_join_req
 修改历史      :
  1.日    期   : 2013年7月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数

 2.日    期   : 2014年4月7日
    作    者   : HiSilicon
   修改内容   : 删除打印日志，整理错误码
**************************************************************************** */
static hi_u32 hmac_sta_update_join_req_params(hmac_vap_stru *hmac_vap, hmac_join_req_stru *join_req)
{
    mac_vap_stru *mac_vap = hmac_vap->base_vap;
    hi_u32 ret;
    mac_device_stru *mac_dev = HI_NULL;
    wlan_mib_ieee802dot11_stru *mib_info = mac_vap->mib_info;

    if (mib_info == HI_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    }
    mac_dev = mac_res_get_dev();
    /* 关联前根据sta是否支持wmm重新刷新mib值,防止之前关联不支持wmm的ap,mib恢复不过来 */
    mib_info->wlan_mib_sta_config.dot11_qos_option_implemented = mac_dev->wmm;
    /* 根据join_request帧更新sta关联的信道和带宽相关信息 */
    if (hmac_sta_update_join_channel(mac_vap, join_req) != HI_SUCCESS) {
        return HI_FAIL;
    }
    /* 根据join_request帧更新sta关联的协议模式相关信息 */
    if (hmac_sta_update_join_protocol(hmac_vap, join_req) != HI_SUCCESS) {
        return HI_FAIL;
    }

    /* STA首先以20MHz运行，如果要切换到40 or 80MHz运行，需要满足一下条件: */
    /* (1) 用户支持40 or 80MHz运行 */
    /* (2) AP支持40 or 80MHz运行(HT Supported Channel Width Set = 1 && VHT Supported Channel Width Set = 0) */
    /* (3) AP在40 or 80MHz运行(SCO = SCA or SCB && VHT Channel Width = 1) */
    ret = mac_get_channel_idx_from_num(mac_vap->channel.band, mac_vap->channel.chan_number, &(mac_vap->channel.idx));
    if (ret != HI_SUCCESS) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_SCAN,
            "{hmac_sta_update_join_req_params::band and channel_num are not compatible.band[%d], channel_num[%d]}",
            mac_vap->channel.band, mac_vap->channel.chan_number);
        return ret;
    }

    /* 更新协议相关信息，包括WMM P2P 11I 20/40M等 */
    hmac_update_join_req_params_prot_sta(hmac_vap, join_req);
    /* 入网优化，不同频段下的能力不一样 */
    if (WLAN_BAND_2G == mac_vap->channel.band) {
        mac_mib_set_short_preamble_option_implemented(mac_vap, WLAN_LEGACY_11B_MIB_SHORT_PREAMBLE);
        mac_mib_set_spectrum_management_required(mac_vap, HI_FALSE);
    } else {
        mac_mib_set_short_preamble_option_implemented(mac_vap, WLAN_LEGACY_11B_MIB_LONG_PREAMBLE);
        mac_mib_set_spectrum_management_required(mac_vap, HI_TRUE);
    }

    if (0 == hmac_calc_up_vap_num(mac_dev)) {
        mac_dev->max_channel = mac_vap->channel.chan_number;
        mac_dev->max_band = mac_vap->channel.band;
        mac_dev->max_bandwidth = mac_vap->channel.en_bandwidth;
    }
    /* 发送关联请求事件到dmac */
    return hmac_sta_dispatch_join_req(mac_vap, join_req);
}

/* ****************************************************************************
 功能描述  : 处理SME发送过来的JOIN_REQ命令，启动JOIN流程，将STA状态设置为WAIT_JOIN
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年4月7日
    作    者   : HiSilicon
    修改内容   : 删除等待beacon及tbtt中断的操作
**************************************************************************** */
hi_u32 hmac_sta_wait_join(hmac_vap_stru *hmac_vap, hmac_join_req_stru *join_req)
{
#ifdef _PRE_WLAN_FEATURE_P2P
    /* 1102 P2PSTA共存 用于 更新参数失败的话需要返回而不是继续下发Join动作 */
    if (hmac_p2p_check_can_enter_state(hmac_vap->base_vap, HMAC_FSM_INPUT_ASOC_REQ) != HI_SUCCESS) {
        /* 不能进入监听状态，返回设备忙 */
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_join fail}\r\n");
        return HI_ERR_CODE_CONFIG_BUSY;
    }
#endif

    /* 更新JOIN REG params 到MIB及MAC寄存器 */
    hi_u32 ret = hmac_sta_update_join_req_params(hmac_vap, join_req);
    if (ret != HI_SUCCESS) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_join::params fail[%d]!}", ret);
        return ret;
    }
    oam_info_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_join::chn=%d bcnPeriod=%d DTIMPeriod=%d.}",
        join_req->bss_dscr.channel.chan_number, join_req->bss_dscr.us_beacon_period, join_req->bss_dscr.dtim_period);

    /* 非proxy sta模式时，需要将dtim参数配置到dmac */
    /* 抛事件到DMAC, 申请事件内存 */
    frw_event_mem_stru *event_mem = frw_event_alloc(sizeof(dmac_ctx_set_dtim_tsf_reg_stru));
    if (event_mem == HI_NULL) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_join::alloc null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;

    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG,
        sizeof(dmac_ctx_set_dtim_tsf_reg_stru), FRW_EVENT_PIPELINE_STAGE_1, hmac_vap->base_vap->vap_id);

    dmac_ctx_set_dtim_tsf_reg_stru *set_dtim_tsf_reg_params = (dmac_ctx_set_dtim_tsf_reg_stru *)event->auc_event_data;

    /* 将Ap bssid和tsf REG 设置值保存在事件payload中 */
    set_dtim_tsf_reg_params->dtim_cnt = join_req->bss_dscr.dtim_cnt;
    set_dtim_tsf_reg_params->dtim_period = join_req->bss_dscr.dtim_period;
    set_dtim_tsf_reg_params->us_tsf_bit0 = BIT0;
    if (memcpy_s(set_dtim_tsf_reg_params->auc_bssid, WLAN_MAC_ADDR_LEN, hmac_vap->base_vap->auc_bssid,
        WLAN_MAC_ADDR_LEN) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(0, OAM_SF_CFG, "hmac_sta_wait_join:: auc_bssid memcpy_s fail.");
        return HI_FAIL;
    }

    /* 分发事件 */
    hcc_hmac_tx_control_event(event_mem, sizeof(dmac_ctx_set_dtim_tsf_reg_stru));
    frw_event_free(event_mem);

    hmac_mgmt_status_enum_uint8 join_result_code = HMAC_MGMT_SUCCESS;
    /* 切换STA状态到JOIN_COMP */
    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_JOIN_COMP);

    /* 发送JOIN成功消息给SME */
    hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_JOIN_RSP, &join_result_code);

    oam_info_log4(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
        "{hmac_sta_wait_join::Join AP[XX:XX:XX:%02X:%02X:%02X] HT=%d VHT=%d HI_SUCCESS.}",
        join_req->bss_dscr.auc_bssid[3], join_req->bss_dscr.auc_bssid[4], /* 3 4 元素索引 */
        join_req->bss_dscr.auc_bssid[5], join_req->bss_dscr.ht_capable);  /* 5 元素索引 */

    oam_info_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
        "{hmac_sta_wait_join::Join AP channel=%d bandwidth=%d Beacon Period=%d HI_SUCCESS.}",
        join_req->bss_dscr.channel.chan_number, hmac_vap->base_vap->channel.en_bandwidth,
        join_req->bss_dscr.us_beacon_period);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理sme发来的auth req请求。将状态置为WAIT_AUTH_SEQ2 抛事件到dmac发送
 修改历史      :
  1.日    期   : 2013年6月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_wait_auth(hmac_vap_stru *hmac_vap, hi_u16 auth_timeout)
{
    /* 申请认证帧空间 */
    oal_netbuf_stru *auth_frame = oal_netbuf_alloc(WLAN_MGMT_NETBUF_SIZE, 0, 4); /* align 4 */
    if (auth_frame == HI_NULL) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::puc_auth_frame null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(oal_netbuf_cb(auth_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    if (memset_s((hi_u8 *)oal_netbuf_header(auth_frame), MAC_80211_FRAME_LEN, 0, MAC_80211_FRAME_LEN) != EOK) {
        oal_netbuf_free(auth_frame);
        return HI_FAIL;
    }

    /* 组认证请求帧 */
    hi_u16 us_auth_len = hmac_mgmt_encap_auth_req(hmac_vap, (hi_u8 *)(oal_netbuf_header(auth_frame)));
    if (us_auth_len == 0) {
        /* 组帧失败 */
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta:hmac_mgmt_encap_auth_req fail}");

        oal_netbuf_free(auth_frame);
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);
        /* 报系统错误，reset MAC 之类的 */
        return HI_FAIL;
    }

    oal_netbuf_put(auth_frame, us_auth_len);
    hmac_user_stru *hmac_user_ap = (hmac_user_stru *)hmac_user_get_user_stru(hmac_vap->base_vap->assoc_vap_id);
    if ((hmac_user_ap == HI_NULL) || (hmac_user_ap->base_user == HI_NULL)) {
        oal_netbuf_free(auth_frame);
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::pst_hmac_user_ap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 为填写发送描述符准备参数 */
    hmac_tx_ctl_stru *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(auth_frame); /* 获取cb结构体 */
    tx_ctl->us_mpdu_len = us_auth_len;                                        /* dmac发送需要的mpdu长度 */
    tx_ctl->us_tx_user_idx = hmac_user_ap->base_user->us_assoc_id; /* 发送完成需要获取user结构体 */
    tx_ctl->frame_header_length = MAC_80211_FRAME_LEN;
    tx_ctl->frame_header = (mac_ieee80211_frame_stru *)oal_netbuf_header(auth_frame);
    tx_ctl->mac_head_type = 1;

    /* 如果是WEP，需要将ap的mac地址写入lut */
    hi_u32 ret = hmac_init_security(hmac_vap->base_vap, hmac_user_ap->base_user->user_mac_addr, WLAN_MAC_ADDR_LEN);
    if (ret != HI_SUCCESS) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_auth::security failed[%d].}", ret);
    }

    /* 抛事件让dmac将该帧发送 */
    ret = hmac_tx_mgmt_send_event(hmac_vap->base_vap, auth_frame, us_auth_len);
    if (ret != HI_SUCCESS) {
        oal_netbuf_free(auth_frame);
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
            "{hmac_wait_auth_sta::hmac_tx_mgmt_send_event failed[%d].}", ret);
        return ret;
    }

    /* 更改状态 */
    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2);

    /* 启动认证超时定时器 */
    hmac_vap->mgmt_timetout_param.state = MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2;
    hmac_vap->mgmt_timetout_param.user_index = (hi_u8)hmac_user_ap->base_user->us_assoc_id;
    hmac_vap->mgmt_timetout_param.vap_id = hmac_vap->base_vap->vap_id;
    frw_timer_create_timer(&hmac_vap->mgmt_timer, hmac_mgmt_timeout_sta, auth_timeout, &hmac_vap->mgmt_timetout_param,
        HI_FALSE);

    return HI_SUCCESS;
}

hi_u32 hmac_sta_shared_key_auth_proc(hmac_vap_stru *hmac_vap, hi_u8 *mac_hdr)
{
    oal_netbuf_stru *auth_frame = oal_netbuf_alloc(WLAN_MGMT_NETBUF_SIZE, 0, 4); /* align 4 */
    if (auth_frame == HI_NULL) {
        /* 复位mac */
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::pst_auth_frame null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化 */
    memset_s(oal_netbuf_cb(auth_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    hi_u16 auth_frame_len = hmac_mgmt_encap_auth_req_seq3(hmac_vap, (hi_u8 *)oal_netbuf_header(auth_frame), mac_hdr);
    if (auth_frame_len == 0) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::auth_frame_len is 0.}");
        oal_netbuf_free(auth_frame);
        return HI_FAIL;
    }
    oal_netbuf_put(auth_frame, auth_frame_len);

    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru((hi_u16)hmac_vap->base_vap->assoc_vap_id);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oal_netbuf_free(auth_frame);
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::pst_hmac_user_ap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写发送和发送完成需要的参数 */
    hmac_tx_ctl_stru *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(auth_frame);
    tx_ctl->us_mpdu_len         = auth_frame_len;                    /* 发送需要帧长度 */
    tx_ctl->us_tx_user_idx      = hmac_user->base_user->us_assoc_id; /* 发送完成要获取用户 */
    tx_ctl->frame_header_length = MAC_80211_FRAME_LEN;
    tx_ctl->frame_header        = (mac_ieee80211_frame_stru *)oal_netbuf_header(auth_frame);
    tx_ctl->mac_head_type       = 1;

    /* 抛事件给dmac发送 */
    hi_u32 ret = hmac_tx_mgmt_send_event(hmac_vap->base_vap, auth_frame, auth_frame_len);
    if (ret != HI_SUCCESS) {
        oal_netbuf_free(auth_frame);
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::send_event Err=%d}", ret);
        return ret;
    }

    frw_timer_immediate_destroy_timer(&hmac_vap->mgmt_timer);

    /* 更改状态为MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4，并启动定时器 */
    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4);

    frw_timer_create_timer(&hmac_vap->mgmt_timer, hmac_mgmt_timeout_sta, hmac_vap->mgmt_timer.timeout,
        &hmac_vap->mgmt_timetout_param, HI_FALSE);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理接收到seq num 等于2 的认证帧
 修改历史      :
  1.日    期   : 2013年6月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_wait_auth_seq2_rx(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *crx_event)
{
    hmac_auth_rsp_stru auth_rsp = { 0 };

    /* 每一个MPDU的控制信息 */
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb((oal_netbuf_stru *)crx_event->netbuf);
    hi_u8            *mac_hdr = (hi_u8 *)rx_ctrl->pul_mac_hdr_start_addr;

    if ((mac_get_frame_sub_type(mac_hdr) != WLAN_FC0_SUBTYPE_AUTH) ||
        (mac_get_auth_seq_num(mac_hdr) != WLAN_AUTH_TRASACTION_NUM_TWO)) {
        return HI_SUCCESS;
    }

    /* AUTH alg CHECK */
    hi_u16 auth_alg = mac_get_auth_alg(mac_hdr);
    if ((hmac_vap->auth_mode != auth_alg) && (hmac_vap->auth_mode != WLAN_WITP_AUTH_AUTOMATIC)) {
        oam_warning_log2(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
            "{hmac_sta_wait_auth_seq2_rx::rcv unexpected auth alg[%d/%d].}", auth_alg, hmac_vap->auth_mode);
    }

    if (mac_get_auth_status(mac_hdr) != MAC_SUCCESSFUL_STATUSCODE) {
        frw_timer_immediate_destroy_timer(&hmac_vap->mgmt_timer);

        auth_rsp.us_status_code = mac_get_auth_status(mac_hdr);

        /* 上报给SME认证成功 */
        hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_AUTH_RSP, (hi_u8 *)&auth_rsp);
        return HI_SUCCESS;
    }

    if (auth_alg == WLAN_WITP_AUTH_OPEN_SYSTEM) {
        frw_timer_immediate_destroy_timer(&hmac_vap->mgmt_timer);

        /* 将状态更改为AUTH_COMP */
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_AUTH_COMP);
        auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* 上报给SME认证成功 */
        hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_AUTH_RSP, (hi_u8 *)&auth_rsp);

        return HI_SUCCESS;
    } else if (auth_alg == WLAN_WITP_AUTH_SHARED_KEY) {
        /* 准备seq等于3的认证帧 */
        hi_u32 ret = hmac_sta_shared_key_auth_proc(hmac_vap, mac_hdr);
        return ret;
    } else {
        frw_timer_immediate_destroy_timer(&hmac_vap->mgmt_timer);

        /* 接收到AP 回复的auth response 中支持认证算法当前不支持的情况下，status code 却是SUCC,
           认为认证成功，并且继续出发关联 */
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq2_rx::auth_alg[%d]Err}",
            auth_alg);

        /* 将状态更改为AUTH_COMP */
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_AUTH_COMP);
        auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* 上报给SME认证成功 */
        hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_AUTH_RSP, (hi_u8 *)&auth_rsp);

        return HI_SUCCESS;
    }
}

/* ****************************************************************************
 功能描述  : 处理收到seq = 4 的认证帧
 修改历史      :
  1.日    期   : 2013年6月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_wait_auth_seq4_rx(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *crx_event)
{
    hmac_rx_ctl_stru    *rx_ctrl = HI_NULL;
    hi_u8               *puc_mac_hdr = HI_NULL;
    hi_u16              us_auth_status;
    hmac_auth_rsp_stru  auth_rsp = {{0}, 0};

    /* 每一个MPDU的控制信息 */
    rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb((oal_netbuf_stru *)crx_event->netbuf);
    puc_mac_hdr = (hi_u8 *)rx_ctrl->pul_mac_hdr_start_addr;

    if (WLAN_FC0_SUBTYPE_AUTH == mac_get_frame_sub_type(puc_mac_hdr)) {
        us_auth_status = mac_get_auth_status(puc_mac_hdr);
        if ((WLAN_AUTH_TRASACTION_NUM_FOUR == mac_get_auth_seq_num(puc_mac_hdr)) &&
            (us_auth_status == MAC_SUCCESSFUL_STATUSCODE)) {
            /* 接收到seq = 4 且状态位为succ 取消定时器 */
            frw_timer_immediate_destroy_timer(&hmac_vap->mgmt_timer);

            auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

            /* 更改sta状态为MAC_VAP_STATE_STA_AUTH_COMP */
            hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_AUTH_COMP);
            oam_info_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq4_rx::auth succ.}");
            /* 将认证结果上报SME */
            hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_AUTH_RSP, (hi_u8 *)&auth_rsp);
        } else {
            oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
                "{hmac_sta_wait_auth_seq4_rx::transaction num.status[%d]}", us_auth_status);
            /* 等待定时器超时 */
        }
    }

    return HI_SUCCESS;
}

static hi_u32 hmac_sta_encap_asoc_req_frame(hmac_vap_stru *hmac_vap, oal_netbuf_stru *asoc_req_frame,
    hi_u32 *asoc_frame_len)
{
    /* 组帧 (Re)Assoc_req_Frame */
    hi_u32 asoc_frame_len_local = hmac_mgmt_encap_asoc_req_sta(hmac_vap, (hi_u8 *)(oal_netbuf_header(asoc_req_frame)));
    if (asoc_frame_len_local == 0) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::get asoc_frame_len fail.}");
        return HI_FAIL;
    }
    oal_netbuf_put(asoc_req_frame, asoc_frame_len_local);

    if (hmac_vap->puc_asoc_req_ie_buff != HI_NULL) {
        oal_mem_free(hmac_vap->puc_asoc_req_ie_buff);
        hmac_vap->puc_asoc_req_ie_buff = HI_NULL;
    }

    if (oal_unlikely(asoc_frame_len_local < OAL_ASSOC_REQ_IE_OFFSET)) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_wait_asoc::invalid ul_asoc_req_ie_len[%u].}", asoc_frame_len_local);
        return HI_FAIL;
    }
    *asoc_frame_len = asoc_frame_len_local;

    return HI_SUCCESS;
}

static hi_u32 hmac_sta_fill_asoc_req_ie_buff(hmac_vap_stru *hmac_vap, const oal_netbuf_stru *asoc_req_frame,
    hi_u32 asoc_frame_len)
{
    /* Should we change the ie buff from local mem to netbuf ?  */
    /* 此处申请的内存，只在上报给内核后释放 */
    hmac_vap->us_asoc_req_ie_len = (hi_u16)((hmac_vap->reassoc_flag) ?
        (asoc_frame_len - OAL_ASSOC_REQ_IE_OFFSET - OAL_MAC_ADDR_LEN) : (asoc_frame_len - OAL_ASSOC_REQ_IE_OFFSET));
    hmac_vap->puc_asoc_req_ie_buff = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, hmac_vap->us_asoc_req_ie_len);
    if (hmac_vap->puc_asoc_req_ie_buff == HI_NULL) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_fill_asoc_req_ie_buff::alloc %u bytes failed}", hmac_vap->us_asoc_req_ie_len);
        return HI_FAIL;
    }

    if (hmac_vap->reassoc_flag) {
        if (memcpy_s(hmac_vap->puc_asoc_req_ie_buff, hmac_vap->us_asoc_req_ie_len,
            oal_netbuf_header(asoc_req_frame) + OAL_ASSOC_REQ_IE_OFFSET + OAL_MAC_ADDR_LEN,
            hmac_vap->us_asoc_req_ie_len) != EOK) {
            oal_mem_free(hmac_vap->puc_asoc_req_ie_buff);
            hmac_vap->puc_asoc_req_ie_buff = HI_NULL;
            oam_error_log0(0, OAM_SF_CFG, "hmac_sta_fill_asoc_req_ie_buff:: pst_asoc_req_frame memcpy_s fail.");
            return HI_FAIL;
        }
    } else {
        if (memcpy_s(hmac_vap->puc_asoc_req_ie_buff, hmac_vap->us_asoc_req_ie_len,
            oal_netbuf_header(asoc_req_frame) + OAL_ASSOC_REQ_IE_OFFSET, hmac_vap->us_asoc_req_ie_len) != EOK) {
            oal_mem_free(hmac_vap->puc_asoc_req_ie_buff);
            hmac_vap->puc_asoc_req_ie_buff = HI_NULL;
            oam_error_log0(0, OAM_SF_CFG, "hmac_sta_fill_asoc_req_ie_buff:: pst_asoc_req_frame memcpy_s fail.");
            return HI_FAIL;
        }
    }

    return HI_SUCCESS;
}

static hi_void hmac_sta_fill_tx_ctl_stru(oal_netbuf_stru *asoc_req_frame, hi_u32 asoc_frame_len,
    const hmac_user_stru *hmac_user_ap)
{
    hmac_tx_ctl_stru *tx_ctl = (hmac_tx_ctl_stru *)oal_netbuf_cb(asoc_req_frame);

    tx_ctl->us_mpdu_len = (hi_u16)asoc_frame_len;
    tx_ctl->us_tx_user_idx = hmac_user_ap->base_user->us_assoc_id;
    tx_ctl->frame_header_length = MAC_80211_FRAME_LEN;
    tx_ctl->frame_header = (mac_ieee80211_frame_stru *)oal_netbuf_header(asoc_req_frame);
    tx_ctl->mac_head_type = 1;
}
/* ****************************************************************************
 功能描述  : 在AUTH_COMP状态接收到SME发过来的ASOC_REQ请求，将STA状态设置为WAIT_ASOC,
             抛事件给DMAC，发送Asoc_req_frame
 修改历史      :
  1.日    期   : 2013年6月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_wait_asoc(hmac_vap_stru *hmac_vap, hi_u16 us_assoc_timeout)
{
    hi_u32 asoc_frame_len = 0;
    oal_netbuf_stru *asoc_req_frame = oal_netbuf_alloc(WLAN_MGMT_NETBUF_SIZE, 0, 4); /* align 4 */

    if (asoc_req_frame == HI_NULL) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::pst_asoc_req_frame null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(oal_netbuf_cb(asoc_req_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    /* 将mac header清零 */
    if (memset_s((hi_u8 *)oal_netbuf_header(asoc_req_frame), MAC_80211_FRAME_LEN, 0, MAC_80211_FRAME_LEN) != EOK) {
        oal_netbuf_free(asoc_req_frame);
        return HI_FAIL;
    }

    /* 组帧 (Re)Assoc_req_Frame */
    if (hmac_sta_encap_asoc_req_frame(hmac_vap, asoc_req_frame, &asoc_frame_len) != HI_SUCCESS) {
        oal_netbuf_free(asoc_req_frame);
        return HI_FAIL;
    }

    /* 申请hmac_vap->puc_asoc_req_ie_buff 内存，只在上报给内核后释放 */
    if (hmac_sta_fill_asoc_req_ie_buff(hmac_vap, asoc_req_frame, asoc_frame_len) != HI_SUCCESS) {
        oal_netbuf_free(asoc_req_frame);
        return HI_FAIL;
    }

    hmac_user_stru *hmac_user_ap = (hmac_user_stru *)hmac_user_get_user_stru(hmac_vap->base_vap->assoc_vap_id);
    if ((hmac_user_ap == HI_NULL) || (hmac_user_ap->base_user == HI_NULL)) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::pst_hmac_user_ap null.}");
        oal_netbuf_free(asoc_req_frame);
        oal_mem_free(hmac_vap->puc_asoc_req_ie_buff);
        hmac_vap->puc_asoc_req_ie_buff = HI_NULL;
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 填充tx_ctl 信息 */
    hmac_sta_fill_tx_ctl_stru(asoc_req_frame, asoc_frame_len, hmac_user_ap);

    /* 抛事件让DMAC将该帧发送 */
    hi_u32 ret = hmac_tx_mgmt_send_event(hmac_vap->base_vap, asoc_req_frame, (hi_u16)asoc_frame_len);
    if (ret != HI_SUCCESS) {
        oal_netbuf_free(asoc_req_frame);
        oal_mem_free(hmac_vap->puc_asoc_req_ie_buff);
        hmac_vap->puc_asoc_req_ie_buff = HI_NULL;

        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_wait_asoc::hmac_tx_mgmt_send_event failed[%d].}", ret);
        return ret;
    }

    /* 更改状态 */
    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_WAIT_ASOC);

    /* 启动关联超时定时器, 为对端ap分配一个定时器，如果超时ap没回asoc rsp则启动超时处理 */
    hmac_vap->mgmt_timetout_param.state = MAC_VAP_STATE_STA_WAIT_ASOC;
    hmac_vap->mgmt_timetout_param.user_index = (hi_u8)hmac_user_ap->base_user->us_assoc_id;
    hmac_vap->mgmt_timetout_param.vap_id = hmac_vap->base_vap->vap_id;

    frw_timer_create_timer(&(hmac_vap->mgmt_timer), hmac_mgmt_timeout_sta, us_assoc_timeout,
        &(hmac_vap->mgmt_timetout_param), HI_FALSE);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 当与STA关联的AP不是QoS的时候，STA默认采用VO策略发送数据
 修改历史      :
  1.日    期   : 2013年10月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_up_update_edca_params_machw(const hmac_vap_stru *hmac_vap, mac_wmm_set_param_type_enum_uint8 type)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    dmac_ctx_sta_asoc_set_edca_reg_stru asoc_set_edca_reg_param = { 0 };

    /* 抛事件到dmac写寄存器 */
    /* 申请事件内存 */
    event_mem = frw_event_alloc(sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru));
    if (event_mem == HI_NULL) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_up_update_edca_params_machw::event_mem alloc null, size[%d].}",
            sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru));
        return HI_ERR_CODE_PTR_NULL;
    }

    asoc_set_edca_reg_param.vap_id = hmac_vap->base_vap->vap_id;
    asoc_set_edca_reg_param.set_param_type = type;

    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_EDCA_REG,
        sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru), FRW_EVENT_PIPELINE_STAGE_1, hmac_vap->base_vap->vap_id);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if (type != MAC_WMM_SET_PARAM_TYPE_DEFAULT) {
        if (memcpy_s((hi_u8 *)&asoc_set_edca_reg_param.ast_wlan_mib_qap_edac,
            (sizeof(wlan_mib_dot11_qapedca_entry_stru) * WLAN_WME_AC_BUTT),
            (hi_u8 *)&hmac_vap->base_vap->mib_info->wlan_mib_qap_edac,
            (sizeof(wlan_mib_dot11_qapedca_entry_stru) * WLAN_WME_AC_BUTT)) != EOK) {
            frw_event_free(event_mem);
            oam_error_log0(0, OAM_SF_CFG, "hmac_sta_up_update_edca_params_machw:: st_wlan_mib_qap_edac memcpy_s fail.");
            return HI_FAIL;
        }
    }
#endif

    /* 拷贝参数 */
    if (memcpy_s(frw_get_event_payload(event_mem), sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru),
        (hi_u8 *)&asoc_set_edca_reg_param, sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru)) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(0, 0, "hmac_sta_up_update_edca_params_machw:: st_asoc_set_edca_reg_param memcpy_s fail.");
        return HI_FAIL;
    }

    /* 分发事件 */
    hcc_hmac_tx_control_event(event_mem, sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru));
    frw_event_free(event_mem);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : STA更新每一个AC的参数
 输入参数  : pst_hmac_sta:处于sta模式的vap
             puc_payload :帧体
 修改历史      :
  1.日    期   : 2013年10月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_sta_up_update_edca_params_mib(const hmac_vap_stru *hmac_vap, const hi_u8 *puc_payload)
{
    hi_u8 aifsn;
    hi_u8 aci;
    hi_u8 ecwmin;
    hi_u8 ecwmax;
    hi_u16 us_txop_limit;
    hi_u8 acm;
    /*        AC Parameters Record Format         */
    /* ------------------------------------------ */
    /* |     1     |       1       |      2     | */
    /* ------------------------------------------ */
    /* | ACI/AIFSN | ECWmin/ECWmax | TXOP Limit | */
    /* ------------------------------------------ */
    /* ************ ACI/AIFSN Field ************** */
    /*     ---------------------------------- */
    /* bit |   4   |  1  |  2  |    1     |   */
    /*     ---------------------------------- */
    /*     | AIFSN | ACM | ACI | Reserved |   */
    /*     ---------------------------------- */
    aifsn = puc_payload[0] & MAC_WMM_QOS_PARAM_AIFSN_MASK;
    acm = (puc_payload[0] & BIT4) ? HI_TRUE : HI_FALSE;
    aci = (puc_payload[0] >> MAC_WMM_QOS_PARAM_ACI_BIT_OFFSET) & MAC_WMM_QOS_PARAM_ACI_MASK;

    /* ECWmin/ECWmax Field */
    /*     ------------------- */
    /* bit |   4    |   4    | */
    /*     ------------------- */
    /*     | ECWmin | ECWmax | */
    /*     ------------------- */
    ecwmin = (puc_payload[1] & MAC_WMM_QOS_PARAM_ECWMIN_MASK);
    ecwmax = ((puc_payload[1] & MAC_WMM_QOS_PARAM_ECWMAX_MASK) >> MAC_WMM_QOS_PARAM_ECWMAX_BIT_OFFSET);

    /* 在mib库中和寄存器里保存的TXOP值都是以us为单位的，但是传输的时候是以32us为
       单位进行传输的，因此在解析的时候需要将解析到的值乘以32
     */
    us_txop_limit = puc_payload[2] | /* 2 元素索引 */
        ((puc_payload[3] & MAC_WMM_QOS_PARAM_TXOPLIMIT_MASK) << MAC_WMM_QOS_PARAM_BIT_NUMS_OF_ONE_BYTE); /* 3元素索引 */
    us_txop_limit = (hi_u16)(us_txop_limit << MAC_WMM_QOS_PARAM_TXOPLIMIT_SAVE_TO_TRANS_TIMES);

    /* 更新相应的MIB库信息 */
    if (aci < WLAN_WME_AC_BUTT) {
        hmac_vap->base_vap->mib_info->wlan_mib_qap_edac[aci].dot11_qapedca_table_c_wmin = ecwmin;
        hmac_vap->base_vap->mib_info->wlan_mib_qap_edac[aci].dot11_qapedca_table_c_wmax = ecwmax;
        hmac_vap->base_vap->mib_info->wlan_mib_qap_edac[aci].dot11_qapedca_table_aifsn = aifsn;
        hmac_vap->base_vap->mib_info->wlan_mib_qap_edac[aci].dot11_qapedca_table_txop_limit = us_txop_limit;
        hmac_vap->base_vap->mib_info->wlan_mib_qap_edac[aci].dot11_qapedca_table_mandatory = acm;
    }
}

/* ****************************************************************************
 功能描述  : STA接收到beacon帧或者关联响应帧更新自身的EDCA参数，涉及到mib值
             和寄存器
 输入参数  : puc_payload :帧体
             ul_msg_len  :帧长度
             us_info_elem_offset :当前指向的帧体位置
             pst_hmac_sta        :指向hmac_vap的指针，vap是sta模式
             uc_frame_sub_type   :帧的次类型
 修改历史      :
  1.日    期   : 2013年10月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_sta_up_update_edca_params(const hmac_edca_params_info_stru *edca_params_info,
    const hmac_vap_stru *hmac_vap, hi_u8 frame_sub_type, const hmac_user_stru *hmac_user)
{
    hi_u8 param_set_cnt, edca_param_set, apsd, ac_num_loop;
    hi_u16 us_msg_offset = edca_params_info->us_info_elem_offset; /* 检查帧中是否有WMM信息元素 */
    mac_device_stru *mac_dev = (mac_device_stru *)mac_res_get_dev();

    /* *********************** WMM Parameter Element ************************** */
    /* ------------------------------------------------------------------------------ */
    /* | EID | LEN | OUI |OUI Type |OUI Subtype |Version |QoS Info |Resd |AC Params | */
    /* ------------------------------------------------------------------------------ */
    /* |  1  |  1  |  3  |    1    |     1      |    1   |    1    |  1  |    16    | */
    /* ------------------------------------------------------------------------------ */
    /* ****************** QoS Info field when sent from WMM AP **************** */
    /*        --------------------------------------------                    */
    /*          | Parameter Set Count | Reserved | U-APSD |                   */
    /*          --------------------------------------------                  */
    /*   bit    |        0~3          |   4~6    |   7    |                   */
    /*          --------------------------------------------                  */
    /* ************************************************************************ */
    while (us_msg_offset < edca_params_info->us_msg_len) {
        /* 判断当前的ie是否是wmm ie，如果不是，继续检查下一个ie，如果是，更新WMM参数 */
        if (HI_TRUE == mac_is_wmm_ie(&(edca_params_info->puc_payload[us_msg_offset]))) {
            /* 解析wmm ie是否携带EDCA参数 */
            edca_param_set = edca_params_info->puc_payload[us_msg_offset + MAC_OUISUBTYPE_WMM_PARAM_OFFSET];

            us_msg_offset += HMAC_WMM_QOS_PARAMS_HDR_LEN;
            param_set_cnt = edca_params_info->puc_payload[us_msg_offset] & 0x0F;

            /* 如果收到的是beacon帧，并且param_set_count没有改变，说明AP的WMM参数没有变
               则STA也不用做任何改变，直接返回即可.
             */
            if ((frame_sub_type == WLAN_FC0_SUBTYPE_BEACON) &&
                (param_set_cnt == hmac_vap->base_vap->wmm_params_update_count)) {
                return;
            }

            mac_dev->wmm = HI_TRUE;

            if (frame_sub_type == WLAN_FC0_SUBTYPE_BEACON) {
                /* 保存QoS Info */
                mac_vap_set_wmm_params_update_count(hmac_vap->base_vap, param_set_cnt);
            }

            apsd = (edca_params_info->puc_payload[us_msg_offset] & BIT7) ? HI_TRUE : HI_FALSE;
            mac_user_set_apsd(hmac_user->base_user, apsd);

            us_msg_offset += HMAC_WMM_QOSINFO_AND_RESV_LEN;

            /* wmm ie中不携带edca参数 直接返回 */
            if (edca_param_set != MAC_OUISUBTYPE_WMM_PARAM) {
                return;
            }

            /* 针对每一个AC，更新EDCA参数 */
            for (ac_num_loop = 0; ac_num_loop < WLAN_WME_AC_BUTT; ac_num_loop++) {
                hmac_sta_up_update_edca_params_mib(hmac_vap, &(edca_params_info->puc_payload[us_msg_offset]));
                us_msg_offset += HMAC_WMM_AC_PARAMS_RECORD_LEN;
            }
            /* 更新EDCA相关的MAC寄存器 */
            hmac_sta_up_update_edca_params_machw(hmac_vap, MAC_WMM_SET_PARAM_TYPE_UPDATE_EDCA);
            return;
        }

        us_msg_offset += (edca_params_info->puc_payload[us_msg_offset + 1] + MAC_IE_HDR_LEN);
    }

    if (frame_sub_type == WLAN_FC0_SUBTYPE_ASSOC_RSP) {
        /* 当与STA关联的AP不是QoS的，STA会去使能EDCA寄存器，并默认利用VO级别发送数据 */
        hi_u32 ret = hmac_sta_up_update_edca_params_machw(hmac_vap, MAC_WMM_SET_PARAM_TYPE_DEFAULT);
        if (ret != HI_SUCCESS) {
            oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
                "{hmac_sta_up_update_edca_params::hmac_sta_up_update_edca_params_machw failed[%d].}", ret);
        }
    }
}

/* ****************************************************************************
 功能描述  : 在beacon HT IE状态变化下更新mac user info到device
 修改历史      :
  1.日    期   : 2015年03月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_sta_update_mac_user_info(const hmac_user_stru *hmac_user_ap, hi_u8 user_idx)
{
    mac_vap_stru *mac_vap = HI_NULL;
    mac_user_stru *mac_user_ap = HI_NULL;
    hi_u32 ret;

    if (hmac_user_ap == HI_NULL) {
        oam_error_log0(0, OAM_SF_RX, "{hmac_sta_update_mac_user_info::param null.}");
        return;
    }

    mac_vap = mac_vap_get_vap_stru(hmac_user_ap->base_user->vap_id);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log1(0, OAM_SF_RX, "{hmac_sta_update_mac_user_info::get mac_vap [vap_id:%d] null.}",
            hmac_user_ap->base_user->vap_id);
        return;
    }

    mac_user_ap = hmac_user_ap->base_user;
    oam_warning_log3(mac_vap->vap_id, OAM_SF_RX,
        "{hmac_sta_update_mac_user_info::user_idx:%d,en_avail_bandwidth:%d,en_cur_bandwidth:%d}", user_idx,
        mac_user_ap->avail_bandwidth, mac_user_ap->cur_bandwidth);

    ret = hmac_config_user_info_syn(mac_vap, mac_user_ap);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_RX,
            "{hmac_sta_update_mac_user_info::hmac_config_user_info_syn failed[%d].}", ret);
    }
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    ret = hmac_config_user_rate_info_syn(mac_vap, mac_user_ap);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_RX, "{hmac_sta_wait_asoc_rx::hmac_syn_rate_info failed[%d].}", ret);
    }
#endif
    return;
}

/* ****************************************************************************
 功能描述  : 在已保存的probe rsp中寻找指定IE，一般用于在asoc rsp中寻找IE失败时
             再在probe rsp做进一步查找
 输入参数  : pst_mac_vap : mac vap结构体
             uc_eid: 要查找的EID
 输出参数  : puc_payload: probe rsp帧体，以帧内第一个IE开头
             us_index:目标IE在payload中的相对位置
 修改历史      :
  1.日    期   : 2016年5月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 *hmac_sta_find_ie_in_probe_rsp(const mac_vap_stru *mac_vap, hi_u8 eid, hi_u16 *pus_index)
{
    hmac_scanned_bss_info *scanned_bss_info = HI_NULL;
    hmac_bss_mgmt_stru *bss_mgmt = HI_NULL;
    hmac_device_stru *hmac_dev = HI_NULL;
    mac_bss_dscr_stru *bss_dscr = HI_NULL;
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 *puc_payload = HI_NULL;
    hi_u8 us_offset;

    if (mac_vap == HI_NULL) {
        oam_warning_log0(0, OAM_SF_SCAN, "{find ie fail, pst_mac_vap is null.}");
        return HI_NULL;
    }

    /* 获取hmac device 结构 */
    hmac_dev = hmac_get_device_stru();
    /* 获取管理扫描的bss结果的结构体 */
    bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt);

    oal_spin_lock(&(bss_mgmt->st_lock));

    scanned_bss_info = hmac_scan_find_scanned_bss_by_bssid(bss_mgmt, mac_vap->auc_bssid);
    if (scanned_bss_info == HI_NULL) {
        oam_warning_log3(mac_vap->vap_id, OAM_SF_CFG, "{find the bss failed by bssid:XX:XX:XX:%02X:%02X:%02X}",
            mac_vap->auc_bssid[3], mac_vap->auc_bssid[4], mac_vap->auc_bssid[5]); /* 3 4 5 元素索引 */

        /* 解锁 */
        oal_spin_unlock(&(bss_mgmt->st_lock));
        return HI_NULL;
    }

    bss_dscr = &(scanned_bss_info->bss_dscr_info);
    /* 解锁 */
    oal_spin_unlock(&(bss_mgmt->st_lock));

    /* 以IE开头的payload，返回供调用者使用 */
    us_offset = MAC_80211_FRAME_LEN + MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    /* 可变数组用法，lin_t e416告警屏蔽 */
    puc_payload = (hi_u8 *)(bss_dscr->auc_mgmt_buff + us_offset);
    if (bss_dscr->mgmt_len < us_offset) {
        return HI_NULL;
    }

    puc_ie = mac_find_ie(eid, puc_payload, (bss_dscr->mgmt_len - us_offset));
    if (puc_ie == HI_NULL) {
        return HI_NULL;
    }

    /* IE长度初步校验 */
    if (*(puc_ie + 1) == 0) {
        oam_warning_log1(0, OAM_SF_ANY, "{IE[%d] len in probe rsp is 0, find ie fail.}", eid);
        return HI_NULL;
    }

    *pus_index = (hi_u16)(puc_ie - puc_payload);

    oam_warning_log1(0, OAM_SF_ANY, "{found ie[%d] in probe rsp.}", eid);

    return puc_payload;
}

/* ****************************************************************************
 功能描述  : 在STA为WAIT_ASOC状态时，解析ht cap IE，分别在asoc rsp和probe rsp
             中查找
 修改历史      :
  1.日    期   : 2016年5月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_sta_check_ht_cap_ie(const mac_vap_stru *mac_sta, hi_u8 *puc_payload, mac_user_stru *mac_user_ap,
    hi_u16 *pus_amsdu_maxsize, hi_u16 us_payload_len)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 *puc_payload_for_ht_cap_chk = HI_NULL;
    hi_u16 us_ht_cap_index;
    hi_u16 us_ht_cap_info = 0;

    if ((mac_sta == HI_NULL) || (puc_payload == HI_NULL) || (mac_user_ap == HI_NULL)) {
        return;
    }

    puc_ie = mac_find_ie(MAC_EID_HT_CAP, puc_payload, us_payload_len);
    if (puc_ie == HI_NULL || puc_ie[1] < MAC_HT_CAP_LEN) {
        puc_payload_for_ht_cap_chk = hmac_sta_find_ie_in_probe_rsp(mac_sta, MAC_EID_HT_CAP, &us_ht_cap_index);
        if (puc_payload_for_ht_cap_chk == HI_NULL) {
            oam_warning_log0(0, OAM_SF_ANY, "{hmac_sta_check_ht_cap_ie::puc_payload_for_ht_cap_chk is null.}");
            return;
        }

        if (puc_payload_for_ht_cap_chk[us_ht_cap_index + 1] < MAC_HT_CAP_LEN) {
            oam_warning_log1(0, OAM_SF_ANY, "{hmac_sta_check_ht_cap_ie::invalid ht cap len[%d].}",
                puc_payload_for_ht_cap_chk[us_ht_cap_index + 1]);
            return;
        }
    } else {
        if (puc_ie < puc_payload) {
            return;
        }
        us_ht_cap_index = (hi_u16)(puc_ie - puc_payload);
        puc_payload_for_ht_cap_chk = puc_payload;
    }

    mac_user_set_ht_capable(mac_user_ap, HI_TRUE);
    /* 根据协议值设置特性，必须在hmac_amsdu_init_user后面调用 */
    mac_ie_proc_ht_sta(mac_sta, puc_payload_for_ht_cap_chk, &us_ht_cap_index, mac_user_ap, &us_ht_cap_info,
        pus_amsdu_maxsize);

    if ((mac_user_ap->ht_hdl.rx_mcs_bitmask[3] == 0) && (mac_user_ap->ht_hdl.rx_mcs_bitmask[2] == 0) &&
        (mac_user_ap->ht_hdl.rx_mcs_bitmask[1] == 0) && (mac_user_ap->ht_hdl.rx_mcs_bitmask[0]) == 0) {
        oam_warning_log0(0, OAM_SF_ANY,
            "{hmac_sta_check_ht_cap_ie::AP support ht capability but support none space_stream.}");
        /* 对端ht能力置为不支持 */
        mac_user_set_ht_capable(mac_user_ap, HI_FALSE);
    }
}

/* ****************************************************************************
 功能描述  : 在STA为WAIT_ASOC状态时，解析ext cap IE，分别在asoc rsp和probe rsp
             中查找
 修改历史      :
  1.日    期   : 2016年5月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_sta_check_ext_cap_ie(const mac_vap_stru *mac_sta, hi_u8 *puc_payload, hi_u16 us_rx_len)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 *puc_payload_proc = HI_NULL;
    hi_u16 us_index;

    puc_ie = mac_find_ie(MAC_EID_EXT_CAPS, puc_payload, us_rx_len);
    if (puc_ie == HI_NULL || puc_ie[1] < MAC_XCAPS_LEN) {
        puc_payload_proc = hmac_sta_find_ie_in_probe_rsp(mac_sta, MAC_EID_EXT_CAPS, &us_index);
        if (puc_payload_proc == HI_NULL) {
            return;
        }

        if (puc_payload_proc[us_index + 1] < MAC_XCAPS_LEN) {
            oam_warning_log1(0, OAM_SF_ANY, "{hmac_sta_check_ext_cap_ie::invalid ext cap len[%d].}",
                puc_payload_proc[us_index + 1]);
            return;
        }
    } else {
        if (puc_ie < puc_payload) {
            return;
        }

        us_index = (hi_u16)(puc_ie - puc_payload);
    }
}

/* ****************************************************************************
 功能描述  : 在STA为WAIT_ASOC状态时，解析OBSS IE，分别在asoc rsp和probe rsp
             中查找
 修改历史      :
  1.日    期   : 2016年5月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_sta_check_obss_scan_ie(const mac_vap_stru *mac_sta, hi_u8 *puc_payload, hi_u16 us_rx_len)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 *puc_payload_proc = HI_NULL;
    hi_u16 us_index;
    hi_u32 ret;

    puc_ie = mac_find_ie(MAC_EID_OBSS_SCAN, puc_payload, us_rx_len);
    if (puc_ie == HI_NULL || puc_ie[1] < MAC_OBSS_SCAN_IE_LEN) {
        puc_payload_proc = hmac_sta_find_ie_in_probe_rsp(mac_sta, MAC_EID_OBSS_SCAN, &us_index);
        if (puc_payload_proc == HI_NULL) {
            return;
        }

        if (puc_payload_proc[us_index + 1] < MAC_OBSS_SCAN_IE_LEN) {
            oam_warning_log1(0, OAM_SF_ANY, "{hmac_sta_check_obss_scan_ie::invalid obss scan len[%d].}",
                puc_payload_proc[us_index + 1]);
            return;
        }
    } else {
        puc_payload_proc = puc_payload;
        if (puc_ie < puc_payload) {
            return;
        }

        us_index = (hi_u16)(puc_ie - puc_payload);
    }

    /* 处理 obss scan IE */
    ret = hmac_ie_proc_obss_scan_ie(mac_sta, &puc_payload_proc[us_index]);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_ie_proc_obss_scan_ie return NON SUCCESS. ");
    }
}

/* ****************************************************************************
 功能描述  : 在STA为WAIT_ASOC状态时，解析ht opern IE，分别在asoc rsp和probe rsp
             中查找
 修改历史      :
  1.日    期   : 2016年5月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_check_ht_opern_ie(mac_vap_stru *mac_sta, mac_user_stru *mac_user_ap, hi_u8 *puc_payload,
    hi_u16 us_rx_len)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 *puc_payload_proc = HI_NULL;
    hi_u16 us_index;
    hi_u32 change = MAC_NO_CHANGE;

    puc_ie = mac_find_ie(MAC_EID_HT_OPERATION, puc_payload, us_rx_len);
    if (puc_ie == HI_NULL || puc_ie[1] < MAC_HT_OPERN_LEN) {
        puc_payload_proc = hmac_sta_find_ie_in_probe_rsp(mac_sta, MAC_EID_HT_OPERATION, &us_index);
        if (puc_payload_proc == HI_NULL) {
            return change;
        }

        if (puc_payload_proc[us_index + 1] < MAC_HT_OPERN_LEN) {
            oam_warning_log1(0, OAM_SF_ANY, "{hmac_sta_check_ht_opern_ie::invalid ht cap len[%d].}",
                puc_payload_proc[us_index + 1]);
            return change;
        }
    } else {
        puc_payload_proc = puc_payload;
        if (puc_ie < puc_payload) {
            return change;
        }

        us_index = (hi_u16)(puc_ie - puc_payload);
    }
    change |= mac_proc_ht_opern_ie(mac_sta, &puc_payload_proc[us_index], mac_user_ap);

    return change;
}

/* ****************************************************************************
 功能描述  : 在STA为WAIT_ASOC状态时，解析asoc rsp 或者reasoc rsp frame，更新相关参数
 修改历史      :
  1.日    期   : 2013年7月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_ie_check_ht_sta(mac_vap_stru *mac_sta, const hmac_check_ht_sta_info_stru *check_ht_sta_info,
    mac_user_stru *mac_user_ap, hi_u16 *pus_amsdu_maxsize)
{
    hi_u32 change = MAC_NO_CHANGE;
    hi_u8 *puc_ie_payload_start = HI_NULL;
    hi_u16 us_ie_payload_len;

    if ((mac_sta == HI_NULL) || (check_ht_sta_info->puc_payload == HI_NULL) || (mac_user_ap == HI_NULL)) {
        return change;
    }

    /* 初始化HT cap为FALSE，入网时会把本地能力跟随AP能力 */
    mac_user_set_ht_capable(mac_user_ap, HI_FALSE);

    /* 至少支持11n才进行后续的处理 */
    if (mac_mib_get_high_throughput_option_implemented(mac_sta) == HI_FALSE) {
        return change;
    }

    puc_ie_payload_start = check_ht_sta_info->puc_payload + check_ht_sta_info->us_offset;
    if (check_ht_sta_info->us_rx_len <= check_ht_sta_info->us_offset) {
        oam_warning_log2(0, OAM_SF_ANY, "{hmac_ie_check_ht_sta::rx_len[%d] less offset[%d].}",
            check_ht_sta_info->us_rx_len, check_ht_sta_info->us_offset);
        return change;
    }
    us_ie_payload_len = check_ht_sta_info->us_rx_len - check_ht_sta_info->us_offset;

    hmac_sta_check_ht_cap_ie(mac_sta, puc_ie_payload_start, mac_user_ap, pus_amsdu_maxsize, us_ie_payload_len);

    hmac_sta_check_ext_cap_ie(mac_sta, puc_ie_payload_start, us_ie_payload_len);

    change = hmac_sta_check_ht_opern_ie(mac_sta, mac_user_ap, puc_ie_payload_start, us_ie_payload_len);

    return change;
}

/* ****************************************************************************
 功能描述  : 处理Overlapping BSS Scan Parameters IE，并更新STA相应MIB项
 输入参数  : pst_mac_vap: MAC VAP结构体指针
             puc_payload: 指向Overlapping BSS Scan Parameters IE的指针
 调用函数  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2014年2月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_ie_proc_obss_scan_ie(const mac_vap_stru *mac_vap, const hi_u8 *puc_payload)
{
    hi_u16 us_trigger_scan_interval;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    wlan_mib_dot11_operation_entry_stru old_mib;
#endif
    if (oal_unlikely((mac_vap == HI_NULL) || (puc_payload == HI_NULL))) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_ie_proc_obss_scan_ie::param null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* *******************Overlapping BSS Scan Parameters element******************
     |ElementID |Length |OBSS    |OBSS   |BSS Channel   |OBSS Scan  |OBSS Scan   |
     |          |       |Scan    |Scan   |Width Trigger |Passive    |Active Total|
     |          |       |Passive |Active |Scan Interval |Total Per  |Per         |
     |          |       |Dwell   |Dwell  |              |Channel    |Channel     |
     ----------------------------------------------------------------------------
     |1         |1      |2       |2      |2             |2          |2           |
     ----------------------------------------------------------------------------
     |BSS Width   |OBSS Scan|
     |Channel     |Activity |
     |Transition  |Threshold|
     |Delay Factor|         |
     ------------------------
     |2           |2        |
    ************************************************************************** */
    if (puc_payload[1] < MAC_OBSS_SCAN_IE_LEN) {
        oam_warning_log1(0, OAM_SF_SCAN, "{mac_ie_proc_obss_scan_ie::invalid obss scan ie len[%d].}", puc_payload[1]);
        return HI_FAIL;
    }

    us_trigger_scan_interval = hi_makeu16(puc_payload[6], puc_payload[7]); /* 6 7 元素索引 */
    if (us_trigger_scan_interval == 0) {
        return HI_ERR_CODE_INVALID_CONFIG;
    }
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if (memset_s(&old_mib, sizeof(wlan_mib_dot11_operation_entry_stru), 0,
        sizeof(wlan_mib_dot11_operation_entry_stru)) != EOK) {
        return HI_FAIL;
    }
    if (memcpy_s(&old_mib, sizeof(old_mib), &mac_vap->mib_info->wlan_mib_operation, sizeof(old_mib)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_ie_proc_obss_scan_ie:: hmac_ie_proc_obss_scan_ie memcpy_s fail.");
        return HI_FAIL;
    }
#endif
    mac_mib_set_obssscan_passive_dwell(mac_vap, hi_makeu16(puc_payload[2], puc_payload[3])); /* 数组2,3,8 */
    mac_mib_set_obssscan_active_dwell(mac_vap, hi_makeu16(puc_payload[4], puc_payload[5]));  /* 数组4,5,8 */
    /* obss扫描周期最小300秒,最大600S, 初始化默认为300秒 */
    mac_mib_set_bsswidth_trigger_scan_interval(mac_vap,
        oal_min(oal_max(us_trigger_scan_interval, 300), 600)); /* min:max 300:600 */
    mac_mib_set_obssscan_passive_total_per_channel(mac_vap, hi_makeu16(puc_payload[8], puc_payload[9])); /* 8 9索引 */
    mac_mib_set_obssscan_active_total_per_channel(mac_vap,
        hi_makeu16(puc_payload[10], puc_payload[11])); /* 10 11索引 */
    mac_mib_set_bsswidth_channel_transition_delay_factor(mac_vap,
        hi_makeu16(puc_payload[12], puc_payload[13])); /* 12 13元素索引 */
    mac_mib_set_obssscan_activity_threshold(mac_vap, hi_makeu16(puc_payload[14], puc_payload[15])); /* 14 15索引 */

    if (0 != memcmp(&old_mib, &mac_vap->mib_info->wlan_mib_operation, sizeof(old_mib))) {
        oam_info_log0(mac_vap->vap_id, OAM_SF_2040, "hmac_ie_proc_obss_scan_ie::sync obss mib to dmac");
        hmac_config_set_obss_scan_param(mac_vap);
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 解析帧中legacy 速率集，更新到user的结构体速率变量成员中
 修改历史      :
  1.日    期   : 2013年11月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_ie_proc_assoc_user_legacy_rate(hi_u8 *puc_payload, hi_u16 us_offset, hi_u16 us_rx_len,
    hmac_user_stru *hmac_user)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 num_rates = 0;
    hi_u8 num_ex_rates = 0;

    if (us_rx_len > us_offset) {
        puc_ie = mac_find_ie(MAC_EID_RATES, puc_payload + us_offset, us_rx_len - us_offset);
        if (puc_ie != HI_NULL) {
            num_rates = puc_ie[1];

            if (num_rates > WLAN_MAX_SUPP_RATES || num_rates < MAC_MIN_XRATE_LEN) {
                oam_warning_log1(0, OAM_SF_ANY, "{hmac_ie_proc_assoc_user_legacy_rate:: invalid rates:%d}", num_rates);
                return HI_FAIL;
            }
            if (memcpy_s(hmac_user->op_rates.auc_rs_rates, num_rates, puc_ie + MAC_IE_HDR_LEN, num_rates) != EOK) {
                oam_error_log0(0, OAM_SF_CFG, "hmac_ie_proc_assoc_user_legacy_rate:: puc_ie memcpy_s fail.");
                return HI_FAIL;
            }
        }
        puc_ie = mac_find_ie(MAC_EID_XRATES, puc_payload + us_offset, us_rx_len - us_offset);
        if (puc_ie != HI_NULL) {
            num_ex_rates = puc_ie[1];

            if (num_ex_rates < MAC_MIN_XRATE_LEN) {
                oam_warning_log1(0, OAM_SF_ANY, "{hmac_ie_proc_assoc_user_legacy_rate:: invalid xrates:%d}",
                    num_ex_rates);
                return HI_FAIL;
            }

            if (num_rates + num_ex_rates > WLAN_MAX_SUPP_RATES) { /* 超出支持速率个数 */
                num_ex_rates = WLAN_MAX_SUPP_RATES - num_rates;
            }

            if (memcpy_s(&(hmac_user->op_rates.auc_rs_rates[num_rates]), WLAN_MAX_SUPP_RATES, puc_ie + MAC_IE_HDR_LEN,
                num_ex_rates) != EOK) {
                oam_error_log0(0, OAM_SF_CFG, "hmac_ie_proc_assoc_user_legacy_rate:: puc_ie memcpy_s fail.");
                return HI_FAIL;
            }
        }
    }

    hmac_user->op_rates.rs_nrates = num_rates + num_ex_rates;

    return HI_SUCCESS;
}

hi_void hmac_sta_wait_asoc_rx_handle_for_pmf(hmac_vap_stru *hmac_vap, mac_status_code_enum_uint16 asoc_status)
{
    mac_vap_stru *mac_vap = hmac_vap->base_vap;
    hmac_vap->pre_assoc_status = asoc_status;

    if (asoc_status == MAC_REJECT_TEMP) {
        mac_vap->mib_info->wlan_mib_sta_config.dot11_association_response_time_out = WLAN_ASSOC_REJECT_TIMEOUT;
    } else {
        mac_vap->mib_info->wlan_mib_sta_config.dot11_association_response_time_out = WLAN_ASSOC_TIMEOUT;
    }
}

hi_u32 hmac_sta_check_protocol_bandwidth(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user_ap,
    hmac_asoc_rsp_stru asoc_rsp)
{
    wlan_bw_cap_enum_uint8 bwcap;
    wlan_bw_cap_enum_uint8 bandwidth_cap;
    mac_vap_stru *mac_vap = hmac_vap->base_vap;

    /* 获取用户的协议模式 */
    hmac_set_user_protocol_mode(mac_vap, hmac_user_ap);
    /* 将协议模式更新到STA */
    hi_u8 avail_mode = hmac_get_auc_avail_protocol_mode(mac_vap->protocol, hmac_user_ap->base_user->protocol_mode);
    /* STA和AP的协议模式不兼容，STA直接去关联 */
    if (avail_mode == WLAN_PROTOCOL_BUTT) {
        oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_check_protocol_bandwidth::no valid protocol:vap mode=%d, user mode=%d,user avail mode=%d.}",
            mac_vap->protocol, hmac_user_ap->base_user->protocol_mode, hmac_user_ap->base_user->avail_protocol_mode);

        asoc_rsp.result_code = HMAC_MGMT_REFUSED;
        asoc_rsp.status_code = MAC_UNSUP_RATE;

        /* 将重连次数直接置为max,不进行再次关联 */
        hmac_vap->asoc_cnt = MAX_ASOC_CNT;

        /* 发送关联结果给SME */
        hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_ASOC_RSP, (hi_u8 *)&asoc_rsp);

        return HI_FAIL;
    }
    /* 获取用户与VAP协议模式交集 */
    hmac_user_ap->base_user->avail_protocol_mode =
        hmac_get_auc_avail_protocol_mode(mac_vap->protocol, hmac_user_ap->base_user->protocol_mode);
    hmac_user_ap->base_user->cur_protocol_mode = hmac_user_ap->base_user->avail_protocol_mode;
    oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
        "{hmac_sta_check_protocol_bandwidth::user avail_protocol:%d,user cur_protocol:%d,vap protocol:%d}",
        hmac_user_ap->base_user->avail_protocol_mode, hmac_user_ap->base_user->cur_protocol_mode, mac_vap->protocol);
    /* 获取用户和VAP 可支持的11a/b/g 速率交集 */
    hmac_vap_set_user_avail_rates(hmac_vap->base_vap, hmac_user_ap);

    /* 获取用户与VAP带宽能力交集 */
    /* 获取用户的带宽能力 */
    mac_user_get_ap_opern_bandwidth(hmac_user_ap->base_user, &bandwidth_cap);

    mac_vap_get_bandwidth_cap(mac_vap, &bwcap);
    bwcap = oal_min(bwcap, bandwidth_cap);
    mac_user_set_bandwidth_info(hmac_user_ap->base_user, bwcap, bwcap);

    oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
        "{hmac_sta_check_protocol_bandwidth::mac user[%d] en_bandwidth_cap:%d,en_avail_bandwidth:%d}",
        hmac_user_ap->base_user->us_assoc_id, bandwidth_cap, hmac_user_ap->base_user->avail_bandwidth);
    return HI_SUCCESS;
}

hi_void hmac_sta_wait_asoc_rx_complete_handle(hmac_vap_stru *hmac_vap, hi_u8 user_idx,
    const hmac_user_stru *hmac_user_ap, hmac_asoc_rsp_stru asoc_rsp, const hmac_rx_ctl_stru *rx_ctrl)
{
    hi_u32 rslt;
    mac_vap_stru *mac_vap = hmac_vap->base_vap;
    mac_user_stru *mac_user_ap = hmac_user_ap->base_user;

    hi_u8 *puc_mac_hdr = (hi_u8 *)(rx_ctrl->pul_mac_hdr_start_addr);
    hi_u16 us_msg_len = rx_ctrl->us_frame_len - rx_ctrl->mac_header_len;

    /* STA切换到UP状态 */
    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_UP);
    /* 将用户(AP)在本地的状态信息设置为已关联状态 */
    mac_user_set_asoc_state(hmac_user_ap->base_user, MAC_USER_STATE_ASSOC);

    /* dmac offload架构下，同步STA USR信息到dmac */
    rslt = hmac_config_user_cap_syn(hmac_vap->base_vap, mac_user_ap);
    if (rslt != HI_SUCCESS) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_wait_asoc_rx_complete_handle::hmac_config_usr_cap_syn failed[%d].}", rslt);
    }

    rslt = hmac_config_user_info_syn(hmac_vap->base_vap, mac_user_ap);
    if (rslt != HI_SUCCESS) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_wait_asoc_rx_complete_handle::hmac_syn_vap_state failed[%d].}", rslt);
    }
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    rslt = hmac_config_user_rate_info_syn(hmac_vap->base_vap, mac_user_ap);
    if (rslt != HI_SUCCESS) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_wait_asoc_rx_complete_handle::hmac_syn_rate_info failed[%d].}", rslt);
    }
#endif

    /* user已经关联上，抛事件给DMAC，在DMAC层挂用户算法钩子 */
    hmac_user_add_notify_alg(hmac_vap->base_vap, user_idx);

    /* 准备消息，上报给APP */
    asoc_rsp.result_code = HMAC_MGMT_SUCCESS;
    asoc_rsp.status_code = MAC_SUCCESSFUL_STATUSCODE;

    /* 记录关联响应帧的部分内容，用于上报给内核 */
    asoc_rsp.asoc_rsp_ie_len = us_msg_len - OAL_ASSOC_RSP_FIXED_OFFSET; /* 除去MAC帧头24字节和FIXED部分6字节 */
    asoc_rsp.puc_asoc_rsp_ie_buff = puc_mac_hdr + OAL_ASSOC_RSP_IE_OFFSET;

    /* 获取AP的mac地址 */
    mac_get_bssid(puc_mac_hdr, asoc_rsp.auc_addr_ap, WLAN_MAC_ADDR_LEN);

    /* 获取关联请求帧信息 */
    asoc_rsp.puc_asoc_req_ie_buff = hmac_vap->puc_asoc_req_ie_buff;
    asoc_rsp.asoc_req_ie_len = hmac_vap->us_asoc_req_ie_len;

    /* 获取信道中心频率 */
    hi_u16 us_freq = (hi_u16)oal_ieee80211_channel_to_frequency(mac_vap->channel.chan_number, mac_vap->channel.band);
    asoc_rsp.us_freq = us_freq;

    hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_ASOC_RSP, (hi_u8 *)(&asoc_rsp));
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /* 上报给Lwip */
    hmac_report_assoc_state_sta(hmac_vap, asoc_rsp.auc_addr_ap, HI_TRUE);
#endif
#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    /* dmac offload架构下，同步STA USR信息到dmac */
    rslt = hmac_config_sta_vap_info_syn(hmac_vap->base_vap);
    if (rslt != HI_SUCCESS) {
        oam_error_log1(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_wait_asoc_rx::hmac_syn_vap_state failed[%d].}", rslt);
    }
#endif
}

hi_void hmac_sta_wait_asoc_rx_complete_update_param(const hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user_ap,
    const hmac_rx_ctl_stru *rx_ctrl, hi_u16 us_offset)
{
    hi_u8 *puc_mac_hdr = (hi_u8 *)(rx_ctrl->pul_mac_hdr_start_addr);
    hi_u8 *puc_payload = (hi_u8 *)(puc_mac_hdr) + rx_ctrl->mac_header_len;
    hi_u16 us_msg_len = rx_ctrl->us_frame_len - rx_ctrl->mac_header_len;
    hi_u8 frame_sub_type = mac_get_frame_sub_type(puc_mac_hdr);
    mac_vap_stru *mac_vap = hmac_vap->base_vap;
    hmac_edca_params_info_stru edca_params_info;
    hmac_check_ht_sta_info_stru check_ht_sta_info;
    /* sta更新自身的edca parameters */
    edca_params_info.puc_payload = puc_payload;
    edca_params_info.us_msg_len = us_msg_len;
    edca_params_info.us_info_elem_offset = us_offset;
    hmac_sta_up_update_edca_params(&edca_params_info, hmac_vap, frame_sub_type, hmac_user_ap);

    /* 更新关联用户的 QoS protocol table */
    hmac_mgmt_update_assoc_user_qos(puc_payload, us_msg_len, us_offset, hmac_user_ap);

    /* 更新关联用户的legacy速率集合 */
    hi_u32 rslt = hmac_ie_proc_assoc_user_legacy_rate(puc_payload, us_offset, us_msg_len, hmac_user_ap);
    if (rslt != HI_SUCCESS) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC, "hmac_ie_proc_assoc_user_legacy_rate fail");
    }

    /* 更新 HT 参数  */
    check_ht_sta_info.puc_payload = puc_payload;
    check_ht_sta_info.us_offset = us_offset;
    check_ht_sta_info.us_rx_len = us_msg_len;
    hi_u32 change = hmac_ie_check_ht_sta(hmac_vap->base_vap, &check_ht_sta_info, hmac_user_ap->base_user,
        &hmac_user_ap->us_amsdu_maxsize);
    if (MAC_BW_CHANGE & change) {
        oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_ASSOC,
            "{hmac_sta_wait_asoc_rx::change BW. ul_change[0x%x], uc_channel[%d], en_bandwidth[%d].}", change,
            mac_vap->channel.chan_number, mac_vap->channel.en_bandwidth);
        hmac_chan_sync(mac_vap, mac_vap->channel.chan_number, mac_vap->channel.en_bandwidth, HI_TRUE);
    }
}

/* ****************************************************************************
 功能描述  : 在WAIT_ASOC状态下接收到Asoc_rsp_frame的处理函数
 修改历史      :
  1.日    期   : 2013年6月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* 规则5.1 避免函数过长，函数不超过50行（非空非注释），申请例外: 功能内聚，建议屏蔽 */
hi_u32 hmac_sta_wait_asoc_rx(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *crx_event)
{
    hmac_asoc_rsp_stru          asoc_rsp;
    hi_u8                       sa_mac_addr[WLAN_MAC_ADDR_LEN] = { 0 };
    hi_u8                       user_idx = 0;
    mac_vap_stru *mac_vap = hmac_vap->base_vap;
    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb((oal_netbuf_stru *)crx_event->netbuf);
    hi_u8 *puc_mac_hdr = (hi_u8 *)(rx_ctrl->pul_mac_hdr_start_addr);
    hi_u8 *puc_payload = (hi_u8 *)(puc_mac_hdr) + rx_ctrl->mac_header_len;
    hi_u16 us_msg_len = rx_ctrl->us_frame_len - rx_ctrl->mac_header_len; /* 消息总长度,不包括FCS */

    hi_u16 us_offset = 0;
    hi_u8 frame_sub_type = mac_get_frame_sub_type(puc_mac_hdr);

    if (memset_s(&asoc_rsp, sizeof(hmac_asoc_rsp_stru), 0, sizeof(hmac_asoc_rsp_stru)) != EOK) {
        return HI_FAIL;
    }

    /* 设置初始关联状态为成功 */
    asoc_rsp.result_code = HMAC_MGMT_SUCCESS;
    switch (frame_sub_type) {
        case WLAN_FC0_SUBTYPE_ASSOC_RSP:
        case WLAN_FC0_SUBTYPE_REASSOC_RSP:
            break;
        default:
            /* do nothing,wait for time out */
            return HI_SUCCESS;
    }

    us_offset += MAC_CAP_INFO_LEN;

    mac_status_code_enum_uint16 asoc_status = mac_get_asoc_status(puc_payload);

    us_offset += MAC_STATUS_CODE_LEN;

#ifdef _PRE_WLAN_FEATURE_PMF
    hmac_sta_wait_asoc_rx_handle_for_pmf(hmac_vap, asoc_status);
#endif

    if ((asoc_status != MAC_SUCCESSFUL_STATUSCODE) || (us_msg_len < OAL_ASSOC_RSP_FIXED_OFFSET)) {
        oam_warning_log2(0, 0, "{hmac_sta_wait_asoc_rx fail:: asoc_status[%d], msg_len[%d].}", asoc_status, us_msg_len);
        return HI_FAIL;
    }

    /* 获取SA 地址 */
    mac_get_address2(puc_mac_hdr, WLAN_MAC_ADDR_LEN, sa_mac_addr, WLAN_MAC_ADDR_LEN);

    /* 根据SA 地地找到对应AP USER结构 */
    hi_u32 rslt = mac_vap_find_user_by_macaddr(hmac_vap->base_vap, sa_mac_addr, WLAN_MAC_ADDR_LEN, &user_idx);
    if (rslt != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{hmac_sta_wait_asoc_rx:: mac_vap_find_user_by_macaddr failed[%d].}", rslt);

        return rslt;
    }

    /* 获取STA关联的AP的用户指针 */
    hmac_user_stru *hmac_user_ap = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if ((hmac_user_ap == HI_NULL) || (hmac_user_ap->base_user == HI_NULL)) {
        return HI_FAIL;
    }

    /* 取消定时器 */
    frw_timer_immediate_destroy_timer(&(hmac_vap->mgmt_timer));

    /* 更新关联ID */
    hi_u16 us_aid = mac_get_asoc_id(puc_payload);
    if ((us_aid > 0) && (us_aid <= 2007)) { /* id小于2007 */
        mac_vap_set_aid(hmac_vap->base_vap, us_aid);
    } else {
        oam_warning_log1(0, 0, "{hmac_sta_wait_asoc_rx::invalid us_sta_aid[%d].}", us_aid);
    }
    us_offset += MAC_AID_LEN;

    /* 初始化安全端口过滤参数 */
#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
    rslt = hmac_init_user_security_port(hmac_vap->base_vap, hmac_user_ap->base_user);
    if (rslt != HI_SUCCESS) {
        oam_error_log1(0, 0, "{hmac_sta_wait_asoc_rx::hmac_init_user_security_port failed[%d].}", rslt);
    }
#endif

#ifdef _PRE_WLAN_FEATURE_PMF
    /* STA模式下的pmf能力来源于WPA_supplicant，只有启动pmf和不启动pmf两种类型 */
    mac_user_set_pmf_active(hmac_user_ap->base_user, mac_vap->user_pmf_cap);
#endif

    hmac_sta_wait_asoc_rx_complete_update_param(hmac_vap, hmac_user_ap, rx_ctrl, us_offset);

    rslt = hmac_sta_check_protocol_bandwidth(hmac_vap, hmac_user_ap, asoc_rsp);
    if (rslt != HI_SUCCESS) {
        return HI_SUCCESS;
    }

    /* 获取用户与VAP空间流交集 */
    rslt = hmac_user_set_avail_num_space_stream(hmac_user_ap->base_user, WLAN_SINGLE_NSS);
    if (rslt != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{hmac_sta_wait_asoc_rx::mac_user_set_avail_num_space_stream failed[%d].}", rslt);
    }
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY

    /* 处理Operating Mode Notification 信息元素 */
    rslt = hmac_check_opmode_notify(hmac_vap, puc_mac_hdr, puc_payload, us_offset, us_msg_len, hmac_user_ap);
    if (rslt != HI_SUCCESS) {
        oam_warning_log1(0, 0, "{hmac_sta_wait_asoc_rx::hmac_check_opmode_notify failed[%d].}", rslt);
    }
#endif

    hmac_sta_wait_asoc_rx_complete_handle(hmac_vap, user_idx, hmac_user_ap, asoc_rsp, rx_ctrl);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 认证超时处理
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_auth_timeout(hmac_vap_stru *hmac_vap)
{
    hmac_auth_rsp_stru auth_rsp = { { 0, }, 0 };

    /* and send it to the host.                                          */
    auth_rsp.us_status_code = HMAC_MGMT_TIMEOUT;

    /* Send the response to host now. */
    hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_AUTH_RSP, (hi_u8 *)&auth_rsp);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 根据初始化的dev带宽能力和bss的带宽能力决定当前需要使用的带宽
 输入参数  :    en_dev_cap, en_bss_cap
 返 回 值  : hi_u32
 修改历史      :
  1.日    期   : 2015年2月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
wlan_channel_bandwidth_enum_uint8 hmac_sta_get_band(wlan_bw_cap_enum_uint8 dev_cap,
    wlan_channel_bandwidth_enum_uint8 bss_cap)
{
    wlan_channel_bandwidth_enum_uint8 band;

    band = WLAN_BAND_WIDTH_20M;

    if ((dev_cap == WLAN_BW_CAP_80M) && (bss_cap >= WLAN_BAND_WIDTH_80PLUSPLUS)) {
        /* 如果AP和STAUT都支持80M，则设置为AP一样 */
        band = bss_cap;
        return band;
    }

    switch (bss_cap) {
        case WLAN_BAND_WIDTH_40PLUS:
        case WLAN_BAND_WIDTH_80PLUSPLUS:
        case WLAN_BAND_WIDTH_80PLUSMINUS:
            if (WLAN_BW_CAP_40M <= dev_cap) {
                band = WLAN_BAND_WIDTH_40PLUS;
            }
            break;

        case WLAN_BAND_WIDTH_40MINUS:
        case WLAN_BAND_WIDTH_80MINUSPLUS:
        case WLAN_BAND_WIDTH_80MINUSMINUS:
            if (WLAN_BW_CAP_40M <= dev_cap) {
                band = WLAN_BAND_WIDTH_40MINUS;
            }
            break;

        default:
            band = WLAN_BAND_WIDTH_20M;
            break;
    }

    return band;
}

/* ****************************************************************************
 功能描述  : 关联超时处理函数
 输入参数  : hmac_vap_stru *pst_hmac_sta, hi_void *p_param
 修改历史      :
  1.日    期   : 2013年7月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_wait_asoc_timeout(hmac_vap_stru *hmac_vap)
{
    hmac_asoc_rsp_stru asoc_rsp = { 0 };

    /* 填写关联结果 */
    asoc_rsp.result_code = HMAC_MGMT_TIMEOUT;

    /* 关联超时失败,原因码上报wpa_supplicant */
#ifdef _PRE_WLAN_FEATURE_PMF
    if (hmac_vap->pre_assoc_status == MAC_REJECT_TEMP) {
        asoc_rsp.status_code = MAC_REJECT_TEMP;
    } else {
        asoc_rsp.status_code = MAC_AUTH_TIMEOUT;
    }
#else
    asoc_rsp.status_code = MAC_AUTH_TIMEOUT;
#endif
    /* 发送关联结果给SME */
    hmac_send_rsp_to_sme_sta(hmac_vap, HMAC_SME_ASOC_RSP, (hi_u8 *)&asoc_rsp);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 上报内核sta已经和某个ap去关联
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_sta_disassoc_rsp(const hmac_vap_stru *hmac_vap, hi_u16 us_disasoc_reason_code, hi_u16 us_dmac_reason_code)
{
    hi_u32 reason_code = ((us_disasoc_reason_code & 0x0000ffff) |
        ((us_dmac_reason_code << 16) & 0xffff0000)); /* 16 移位bit数，高2字节和低2字节错开 */
    hmac_send_event_to_host(hmac_vap->base_vap, (const hi_u8 *)(&reason_code), sizeof(hi_u32),
        HMAC_HOST_CTX_EVENT_SUB_TYPE_DISASOC_COMP_STA);

    const hi_u8 *bcast_mac_addr = mac_get_mac_bcast_addr();
    /* 上报给Lwip */
    hmac_report_assoc_state_sta(hmac_vap, (hi_u8 *)bcast_mac_addr, HI_FALSE);
    return;
}

/* ****************************************************************************
功能描述  : 处理接收去认证帧
修改历史      :
1.日    期   : 2013年7月1日
    作    者   : HiSilicon
修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_sta_rx_deauth_req(hmac_vap_stru *hmac_vap, hi_u8 *mac_hdr, hi_u8 protected)
{
    hi_u8  auc_bssid[WLAN_MAC_ADDR_LEN] = {0}; /* 元素个数为6 */
    hi_u8 user_idx = 0xff;
    hi_u8 *da_mac_addr = HI_NULL;
    hi_u8 *sa_mac_addr = HI_NULL;

    /* 增加接收到去认证帧或者去关联帧时的维测信息 */
    mac_rx_get_sa((mac_ieee80211_frame_stru *)mac_hdr, &sa_mac_addr);
    oam_warning_log4(hmac_vap->base_vap->vap_id, OAM_SF_AUTH,
        "{hmac_sta_rx_deauth_req::Because of err_code[%d], received deauth/disassoc frame, sa xx:xx:xx:%2x:%2x:%2x.}",
        *((hi_u16 *)(mac_hdr + MAC_80211_FRAME_LEN)), sa_mac_addr[3], sa_mac_addr[4], sa_mac_addr[5]); /* 3 4 5 */

    mac_get_address2(mac_hdr, WLAN_MAC_ADDR_LEN, auc_bssid, WLAN_MAC_ADDR_LEN);

    hi_u32 ret = mac_vap_find_user_by_macaddr(hmac_vap->base_vap, auc_bssid, WLAN_MAC_ADDR_LEN, &user_idx);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_sta_rx_deauth_req::find user failed=%d}", ret);
        return ret;
    }

    hmac_user_stru *hmac_user_vap = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if ((hmac_user_vap == HI_NULL) || (hmac_user_vap->base_user == HI_NULL)) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_sta_rx_deauth_req::pst_hmac_user_vap null.}");

        /* 没有查到对应的USER,发送去认证消息 */
        hmac_mgmt_send_deauth_frame(hmac_vap->base_vap, auc_bssid, WLAN_MAC_ADDR_LEN, MAC_NOT_AUTHED);

        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);

        /* 上报内核sta已经和某个ap去关联 */
        hmac_sta_disassoc_rsp(hmac_vap, *((hi_u16 *)(mac_hdr + MAC_80211_FRAME_LEN)), DMAC_DISASOC_MISC_WOW_RX_DEAUTH);
        return HI_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_PMF
    /* 检查是否需要发送SA query request */
    if ((hmac_user_vap->base_user->user_asoc_state == MAC_USER_STATE_ASSOC) &&
        (hmac_pmf_check_err_code(hmac_user_vap->base_user, protected, mac_hdr) == HI_SUCCESS)) {
        /* 在关联状态下收到未加密的ReasonCode 6/7需要启动SA Query流程 */
        ret = hmac_start_sa_query(hmac_vap->base_vap, hmac_user_vap, hmac_user_vap->base_user->cap_info.pmf_active);
        if (ret != HI_SUCCESS) {
            return HI_ERR_CODE_PMF_SA_QUERY_START_FAIL;
        }

        return HI_SUCCESS;
    }
#endif

    /* 如果该用户的管理帧加密属性不一致，丢弃该报文 */
    mac_rx_get_da((mac_ieee80211_frame_stru *)mac_hdr, &da_mac_addr);
    if ((ether_is_multicast(da_mac_addr) != HI_TRUE) && (protected != hmac_user_vap->base_user->cap_info.pmf_active)) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_sta_rx_deauth_req::PMF check failed.}");

        return HI_FAIL;
    }

    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);

    /* 上报system error 复位 mac */
    /* 删除user */
    if (hmac_user_del(hmac_vap->base_vap, hmac_user_vap) != HI_SUCCESS) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_AUTH, "{hmac_sta_rx_deauth_req::hmac_user_del failed.}");

        /* 上报内核sta已经和某个ap去关联 */
        hmac_sta_disassoc_rsp(hmac_vap, *((hi_u16 *)(mac_hdr + MAC_80211_FRAME_LEN)), DMAC_DISASOC_MISC_WOW_RX_DEAUTH);
        return HI_FAIL;
    }

    /* 上报内核sta已经和某个ap去关联 */
    hmac_sta_disassoc_rsp(hmac_vap, *((hi_u16 *)(mac_hdr + MAC_80211_FRAME_LEN)), DMAC_DISASOC_MISC_WOW_RX_DEAUTH);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : STA收到Beacon帧后，处理HT相关信息元素
 输入参数  : pst_mac_vap    : MAC VAP结构体指针，指向STA
             puc_payload    : 指向Beacon帧体的指针
             us_frame_len   : Beacon帧体的长度(不包括帧头)
             us_frame_offset: Beacon帧中第一个IE相对帧体地址的偏移
 输出参数  : pst_mac_user   : MAC USER结构体指针，指向AP
 返 回 值  : hi_u8:相关信息是否有改变，是否需要同步?
 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_sta_up_update_ht_params(mac_vap_stru *mac_vap, const hi_u8 *puc_payload, hi_u16 us_frame_len,
    hi_u16 us_frame_offset, mac_user_stru *mac_user)
{
    hi_u16 us_index = us_frame_offset;
    mac_user_ht_hdl_stru ht_hdl;
    hi_u32 change = MAC_NO_CHANGE;

    if (memset_s(&ht_hdl, sizeof(mac_user_ht_hdl_stru), 0, sizeof(mac_user_ht_hdl_stru)) != EOK) {
        return HI_FAIL;
    }
    mac_user_get_ht_hdl(mac_user, &ht_hdl);

    while (us_index < us_frame_len) {
        if (puc_payload[us_index] == MAC_EID_HT_OPERATION) {
            change |= mac_proc_ht_opern_ie(mac_vap, &puc_payload[us_index], mac_user);
        }
        us_index += puc_payload[us_index + 1] + MAC_IE_HDR_LEN;
    }

    if (memcmp((hi_u8 *)(&ht_hdl), (hi_u8 *)(&mac_user->ht_hdl), sizeof(mac_user_ht_hdl_stru)) != 0) {
        return (change | MAC_HT_CHANGE);
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : sta up状态接收beacon帧处理
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_sta_up_rx_beacon(const hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf)
{
    hi_u8 sa_mac_addr[WLAN_MAC_ADDR_LEN] = { 0 };
    hi_u8 user_idx = 0;

    hmac_rx_ctl_stru *rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    mac_ieee80211_frame_stru *mac_hdr = (mac_ieee80211_frame_stru *)(rx_ctrl->pul_mac_hdr_start_addr);
    hi_u8 *frame_body = (hi_u8 *)mac_hdr + rx_ctrl->mac_header_len;
    hi_u16 frame_len = rx_ctrl->us_frame_len - rx_ctrl->mac_header_len; /* 帧体长度 */

    hi_u16 frame_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    hi_u8 frame_sub_type = mac_get_frame_sub_type((hi_u8 *)mac_hdr);

    /* 来自其它bss的Beacon不做处理 */
    if (oal_compare_mac_addr(hmac_vap->base_vap->auc_bssid, mac_hdr->auc_address3, WLAN_MAC_ADDR_LEN) != 0) {
        return HI_SUCCESS;
    }

    /* 获取管理帧的源地址SA */
    mac_get_address2((hi_u8 *)mac_hdr, WLAN_MAC_ADDR_LEN, sa_mac_addr, WLAN_MAC_ADDR_LEN);

    /* 根据SA 地地找到对应AP USER结构 */
    if (mac_vap_find_user_by_macaddr(hmac_vap->base_vap, sa_mac_addr, WLAN_MAC_ADDR_LEN, &user_idx) != HI_SUCCESS) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_sta_up_rx_beacon:mac_vap_find failed}");
        return HI_FAIL;
    }
    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_sta_up_rx_beacon::pst_hmac_user null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 处理HT 相关信息元素 */
    hi_u32 change_flag = MAC_NO_CHANGE |
        hmac_sta_up_update_ht_params(hmac_vap->base_vap, frame_body, frame_len, frame_offset, hmac_user->base_user);

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    /* 处理Operating Mode Notification 信息元素 */
    if (hmac_check_opmode_notify(hmac_vap, (hi_u8 *)mac_hdr, frame_body, frame_offset, frame_len, hmac_user) !=
        HI_SUCCESS) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_sta_up_rx_beacon::hmac_check failed.}");
    }
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if (((MAC_HT_CHANGE & change_flag) || (MAC_VHT_CHANGE & change_flag)) &&
        (hmac_config_user_rate_info_syn(hmac_vap->base_vap, hmac_user->base_user) != HI_SUCCESS)) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_sta_up_rx_beacon::user_rate failed.}");
    }
#endif
    if (MAC_BW_CHANGE & change_flag) {
        hmac_sta_update_mac_user_info(hmac_user, user_idx);

        oam_warning_log3(0, OAM_SF_ASSOC, "{hmac_sta_up_rx_beacon::change BW.change[0x%x],channel[%d],bandwidth[%d].}",
            change_flag, hmac_vap->base_vap->channel.chan_number, hmac_vap->base_vap->channel.en_bandwidth);
        hmac_chan_sync(hmac_vap->base_vap, hmac_vap->base_vap->channel.chan_number,
            hmac_vap->base_vap->channel.en_bandwidth, HI_TRUE);
    }

    /* 更新edca参数 */
    hmac_edca_params_info_stru edca_params_info = { frame_body, frame_len, frame_offset };
    hmac_sta_up_update_edca_params(&edca_params_info, hmac_vap, frame_sub_type, hmac_user);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : STA up状态接收Channel Switch Announcement帧处理
 输入参数  : pst_mac_vap: MAC VAP结构体指针
             pst_netbuf : 包含Channel Switch Announcement帧的netbuf
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_sta_up_rx_ch_switch(mac_vap_stru *mac_vap, oal_netbuf_stru *netbuf)
{
    hmac_rx_ctl_stru *rx_ctrl = HI_NULL;
    hi_u16 us_index;
    hi_u8 *puc_data = HI_NULL;
    hi_u16 us_framebody_len;

    if (HI_FALSE == mac_mib_get_spectrum_management_implemented(mac_vap)) {
        oam_info_log0(mac_vap->vap_id, OAM_SF_BA, "{hmac_sta_up_rx_ch_switch::Ignoring Spectrum Management frames.}");
        return;
    }

    rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    us_framebody_len = rx_ctrl->us_frame_len - rx_ctrl->mac_header_len;

    /* 获取帧体指针 */
    puc_data = (hi_u8 *)rx_ctrl->pul_mac_hdr_start_addr + rx_ctrl->mac_header_len;

    us_index = MAC_ACTION_OFFSET_ACTION + 1;

    while (us_index < us_framebody_len) {
        if (puc_data[us_index] == MAC_EID_CHANSWITCHANN) {
            hmac_ie_proc_ch_switch_ie(mac_vap, &puc_data[us_index], MAC_EID_CHANSWITCHANN);
        } else if (puc_data[us_index] == MAC_EID_SEC_CH_OFFSET) {
            if (puc_data[us_index + 1] < MAC_SEC_CH_OFFSET_IE_LEN) {
                oam_warning_log1(0, OAM_SF_ANY, "{dmac_sta_up_rx_ch_switch::invalid sec chan offset ie len[%d]}",
                    puc_data[us_index + 1]);
                us_index += MAC_IE_HDR_LEN + puc_data[us_index + 1];
                continue;
            }
            /* 如果通道发生改变了，不需要切信道么? */
            mac_vap->ch_switch_info.new_bandwidth = mac_get_bandwidth_from_sco(puc_data[us_index + MAC_IE_HDR_LEN]);
            oam_warning_log1(0, OAM_SF_ANY, "{hmac_sta_up_rx_sca:new_bw[%d]}", mac_vap->ch_switch_info.new_bandwidth);
        }
        us_index += MAC_IE_HDR_LEN + puc_data[us_index + 1];
    }
}

/* ****************************************************************************
 功能描述  : STA up状态接收Extended Channel Switch Announcement帧处理
 输入参数  : pst_mac_vap: MAC VAP结构体指针
             pst_netbuf : 包含Extended Channel Switch Announcement帧的netbuf
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_sta_up_rx_ext_ch_switch(mac_vap_stru *mac_vap, oal_netbuf_stru *netbuf)
{
    hmac_rx_ctl_stru *rx_ctrl = HI_NULL;
    hi_u8 *puc_data = HI_NULL;

    if (HI_FALSE == mac_mib_get_spectrum_management_implemented(mac_vap)) {
        oam_info_log0(mac_vap->vap_id, OAM_SF_BA,
            "{hmac_sta_up_rx_ext_ch_switch::Ignoring Spectrum Management frames.}");
        return;
    }

    rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    /* 获取帧体指针 */
    puc_data = (hi_u8 *)rx_ctrl->pul_mac_hdr_start_addr + rx_ctrl->mac_header_len;

    hmac_ie_proc_ch_switch_ie(mac_vap, puc_data, MAC_EID_EXTCHANSWITCHANN);
}

/* ****************************************************************************
 功能描述  : STA在UP状态下的接收ACTION帧处理
 输入参数  : pst_hmac_vap: HMAC VAP结构体指针
             pst_netbuf  : Action帧所在的netbuf
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_sta_up_rx_action(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf, hi_u8 is_protected)
{
    hmac_user_stru   *hmac_user = HI_NULL;
    hmac_rx_ctl_stru *rx_ctrl   = (hmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);

    /* 获取帧头信息 */
    mac_ieee80211_frame_stru *frame_hdr = (mac_ieee80211_frame_stru *)rx_ctrl->pul_mac_hdr_start_addr;
#ifdef _PRE_WLAN_FEATURE_P2P
    /* P2P0设备所接受的action全部上报 */
    hi_u8 *puc_p2p0_mac_addr = hmac_vap->base_vap->mib_info->wlan_mib_sta_config.auc_p2p0_dot11_station_id;
    if (oal_compare_mac_addr(frame_hdr->auc_address1, puc_p2p0_mac_addr, WLAN_MAC_ADDR_LEN) == 0) {
        hmac_rx_mgmt_send_to_host(hmac_vap, netbuf);
    }
#endif

    /* 获取发送端的用户指针 */
    hmac_user = mac_vap_get_hmac_user_by_addr(hmac_vap->base_vap, frame_hdr->auc_address2, WLAN_MAC_ADDR_LEN);
    if (hmac_user == HI_NULL) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_RX, "{hmac_sta_up_rx_action::mac_vap_find_user failed.}");
        return;
    }

    /* 获取帧体指针 */
    hi_u8 *puc_data = (hi_u8 *)rx_ctrl->pul_mac_hdr_start_addr + rx_ctrl->mac_header_len;

    /* Category */
    if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_BA) {
        hmac_mgmt_rx_action_ba(hmac_vap, hmac_user, puc_data);
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_SPECMGMT) {
        if (puc_data[MAC_ACTION_OFFSET_ACTION] == MAC_SPEC_CH_SWITCH_ANNOUNCE) {
            hmac_sta_up_rx_ch_switch(hmac_vap->base_vap, netbuf);
        }
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_PUBLIC) {
        if (puc_data[MAC_ACTION_OFFSET_ACTION] == MAC_PUB_EX_CH_SWITCH_ANNOUNCE) {
            hmac_sta_up_rx_ext_ch_switch(hmac_vap->base_vap, netbuf);
#ifdef _PRE_WLAN_FEATURE_P2P
        } else if (puc_data[MAC_ACTION_OFFSET_ACTION] == MAC_PUB_VENDOR_SPECIFIC) {
            /* 查找OUI-OUI type值为 50 6F 9A - 09 (WFA P2P v1.0)  */
            /* 并用hmac_rx_mgmt_send_to_host接口上报 */
            if (mac_ie_check_p2p_action(puc_data + MAC_ACTION_OFFSET_ACTION) == HI_TRUE) {
                hmac_rx_mgmt_send_to_host(hmac_vap, netbuf);
            }
#endif
        }
#ifdef _PRE_WLAN_FEATURE_PMF
    } else if (puc_data[MAC_ACTION_OFFSET_CATEGORY] == MAC_ACTION_CATEGORY_SA_QUERY) {
        if (puc_data[MAC_ACTION_OFFSET_ACTION] == MAC_SA_QUERY_ACTION_REQUEST) {
            hmac_rx_sa_query_req(hmac_vap, netbuf, is_protected);
        } else if (puc_data[MAC_ACTION_OFFSET_ACTION] == MAC_SA_QUERY_ACTION_RESPONSE) {
            hmac_rx_sa_query_rsp(hmac_vap, netbuf, is_protected);
        }
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
 功能描述  : AP在UP状态下的接收管理帧处理
 修改历史      :
  1.日    期   : 2013年6月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_sta_up_rx_mgmt(hmac_vap_stru *hmac_vap, const dmac_wlan_crx_event_stru *crx_event)
{
    hmac_rx_ctl_stru    *rx_ctrl = HI_NULL;
    hi_u8               *puc_mac_hdr = HI_NULL;
    hi_u8               mgmt_frm_type;
    hi_u8               is_protected;
    if (crx_event == HI_NULL || crx_event->netbuf == HI_NULL) {
        oam_error_log0(0, OAM_SF_AUTH, "{hmac_sta_up_rx_mgmt::crx_event/crx_event->netbuf  is NULL}");
        return HI_ERR_CODE_PTR_NULL;
    }

    rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb((oal_netbuf_stru *)crx_event->netbuf);

    puc_mac_hdr = (hi_u8 *)(rx_ctrl->pul_mac_hdr_start_addr);
    if (puc_mac_hdr == HI_NULL) {
        oam_error_log0(0, OAM_SF_AUTH, "{hmac_sta_up_rx_mgmt::puc_mac_hdr is NULL}");
        return HI_ERR_CODE_PTR_NULL;
    }

    is_protected = mac_get_protectedframe(puc_mac_hdr);

    /* Bar frame proc here */
    if (WLAN_FC0_TYPE_CTL == mac_get_frame_type(puc_mac_hdr)) {
        mgmt_frm_type = mac_get_frame_sub_type(puc_mac_hdr);
        if ((mgmt_frm_type >> 4) == WLAN_BLOCKACK_REQ) { /* 右移4位 */
            hmac_up_rx_bar(hmac_vap, rx_ctrl);
        }
    }

    /* AP在UP状态下 接收到的各种管理帧处理 */
    mgmt_frm_type = mac_get_frame_sub_type(puc_mac_hdr);

    switch (mgmt_frm_type) {
        case WLAN_FC0_SUBTYPE_DEAUTH:
        case WLAN_FC0_SUBTYPE_DISASSOC:
            hmac_sta_rx_deauth_req(hmac_vap, puc_mac_hdr, is_protected);
            break;

        case WLAN_FC0_SUBTYPE_BEACON:
            hmac_sta_up_rx_beacon(hmac_vap, (oal_netbuf_stru *)crx_event->netbuf);
            break;

        case WLAN_FC0_SUBTYPE_ACTION:
            hmac_sta_up_rx_action(hmac_vap, (oal_netbuf_stru *)crx_event->netbuf, is_protected);
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 开启25ms重传策略
 修改历史      :
  1.日    期   : 2019年9月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_set_retry_time_en(const mac_vap_stru *mac_vap, hi_u8 retry_time, hi_u8 retry_frame_type)
{
    mac_cfg_retry_param_stru set_retry_first;
    mac_cfg_retry_param_stru set_retry_second;
    hi_u32 ret;

    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_TX, "{hmac_set_retry_time_en::mac vap is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 将当前重传次数置0 */
    set_retry_first.limit = 0;
    set_retry_first.type = retry_frame_type;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_RETRY_LIMIT, sizeof(mac_cfg_retry_param_stru),
        (hi_u8 *)&set_retry_first);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_set_retry_time_en::hmac_config_send_event failed[%d].}",
            ret);
        return ret;
    }

    /* 使能时间重传策略 */
    set_retry_second.limit = retry_time;
    set_retry_second.type = MAC_CFG_RETRY_TIMEOUT;

    /* **************************************************************************
            抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_RETRY_LIMIT, sizeof(mac_cfg_retry_param_stru),
        (hi_u8 *)&set_retry_second);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_set_retry_time_en::hmac_config_send_event failed[%d].}",
            ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 关闭25ms重传策略
 修改历史      :
  1.日    期   : 2019年9月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_set_retry_time_close(const mac_vap_stru *mac_vap)
{
    mac_cfg_retry_param_stru set_retry_first;
    mac_cfg_retry_param_stru set_retry_second;
    mac_cfg_retry_param_stru set_retry_third;
    hi_u32 ret;

    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_TX, "{hmac_set_retry_time_close::mac vap is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 恢复数据帧重传次数 */
    set_retry_first.limit = 5; /* DMAC_MAX_SW_RETRIES: 5 */
    set_retry_first.type = MAC_CFG_RETRY_DATA;

    /* **************************************************************************
        抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_RETRY_LIMIT, sizeof(mac_cfg_retry_param_stru),
        (hi_u8 *)&set_retry_first);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
                         "{hmac_set_retry_time_close::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }

    /* 恢复管理帧重传次数 */
    set_retry_second.limit = DMAC_MGMT_MAX_SW_RETRIES;
    set_retry_second.type = MAC_CFG_RETRY_MGMT;

    /* **************************************************************************
            抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_RETRY_LIMIT, sizeof(mac_cfg_retry_param_stru),
        (hi_u8 *)&set_retry_second);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
                         "{hmac_set_retry_time_close::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }

    /* 关闭时间重传策略 */
    set_retry_third.limit = 0;
    set_retry_third.type = MAC_CFG_RETRY_TIMEOUT;

    /* **************************************************************************
            抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_SET_RETRY_LIMIT, sizeof(mac_cfg_retry_param_stru),
        (hi_u8 *)&set_retry_third);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
                         "{hmac_set_retry_time_close::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
