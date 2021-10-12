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

#include "mac_ie.h"
#include "mac_frame.h"
#include "mac_device.h"
#include "dmac_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  函数实现
**************************************************************************** */
#define __WIFI_ROM_SECTION__ /* 代码ROM段起始位置 */

/* ****************************************************************************
 功能描述  : 设置VAP mib值
 输入参数  : [1]mac_vap
             [2]us_len
             [3]puc_param
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_config_set_mib(const mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_cfg_set_mib_stru *set_mib = (mac_cfg_set_mib_stru *)puc_param;

    hi_unref_param(us_len);

    if (set_mib->mib_idx == WLAN_MIB_INDEX_SPEC_MGMT_IMPLEMENT) {
        mac_mib_set_spectrum_management_implemented(mac_vap, (hi_u8)(set_mib->mib_value));
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_FORTY_MHZ_OPERN_IMPLEMENT) {
        mac_mib_set_forty_mhz_operation_implemented(mac_vap, (hi_u8)(set_mib->mib_value));
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_2040_COEXT_MGMT_SUPPORT) {
        mac_mib_set_2040bss_coexistence_management_support(mac_vap, (hi_u8)(set_mib->mib_value));
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_FORTY_MHZ_INTOL) {
        mac_mib_set_forty_mhz_intolerant(mac_vap, (hi_u8)(set_mib->mib_value));
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_OBSSSCAN_TRIGGER_INTERVAL) {
        mac_mib_set_bsswidth_trigger_scan_interval(mac_vap, set_mib->mib_value);
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_OBSSSCAN_TRANSITION_DELAY_FACTOR) {
        mac_mib_set_bsswidth_channel_transition_delay_factor(mac_vap, set_mib->mib_value);
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_OBSSSCAN_PASSIVE_DWELL) {
        mac_mib_set_obssscan_passive_dwell(mac_vap, set_mib->mib_value);
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_OBSSSCAN_ACTIVE_DWELL) {
        mac_mib_set_obssscan_active_dwell(mac_vap, set_mib->mib_value);
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_OBSSSCAN_PASSIVE_TOTAL_PER_CHANNEL) {
        mac_mib_set_obssscan_passive_total_per_channel(mac_vap, set_mib->mib_value);
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_OBSSSCAN_ACTIVE_TOTAL_PER_CHANNEL) {
        mac_mib_set_obssscan_active_total_per_channel(mac_vap, set_mib->mib_value);
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_OBSSSCAN_ACTIVITY_THRESHOLD) {
        mac_mib_set_obssscan_activity_threshold(mac_vap, set_mib->mib_value);
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_MESH_ACCEPTING_PEER) {
        mac_mib_set_accepting_peer(mac_vap, (hi_u8)set_mib->mib_value);
    } else if (set_mib->mib_idx == WLAN_MIB_INDEX_MESH_SECURITY_ACTIVATED) {
        mac_mib_set_mesh_security(mac_vap, (hi_u8)set_mib->mib_value);
#endif
    } else {
        oam_error_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_config_set_mib::invalid ul_mib_idx[%d].}",
            set_mib->mib_idx);
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理HT Operation IE
 输入参数  : pst_mac_vap : MAC VAP结构体指针，指向STA
             puc_payload : 指向HT Operation IE的指针
             pst_mac_user: MAC VAP结构体指针，指向AP
 修改历史      :
  1.日    期   : 2014年3月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_proc_ht_opern_ie(mac_vap_stru *mac_vap, const hi_u8 *puc_payload, mac_user_stru *mac_user)
{
    mac_ht_opern_stru *ht_opern = HI_NULL;
    mac_user_ht_hdl_stru ht_hdl;
    wlan_bw_cap_enum_uint8 bwcap_vap;
    const hi_u32 change = MAC_NO_CHANGE;

    if (oal_unlikely((mac_vap == HI_NULL) || (puc_payload == HI_NULL) || (mac_user == HI_NULL))) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ie_proc_ht_opern_ie::param null.}");
        return change;
    }

    if (puc_payload[1] < 6) { /* 长度校验，此处仅用到前6字节，后面Basic MCS Set未涉及 */
        oam_warning_log1(0, OAM_SF_ANY, "{mac_proc_ht_opern_ie::invalid ht opern ie len[%d].}", puc_payload[1]);
        return change;
    }
    mac_user_get_ht_hdl(mac_user, &ht_hdl);

    /* *********************** HT Operation Element *************************************
      ----------------------------------------------------------------------
      |EID |Length |PrimaryChannel |HT Operation Information |Basic MCS Set|
      ----------------------------------------------------------------------
      |1   |1      |1              |5                        |16           |
      ----------------------------------------------------------------------
    ************************************************************************** */
    /* *********************** HT Information Field ****************************
     |--------------------------------------------------------------------|
     | Primary | Seconday  | STA Ch | RIFS |           reserved           |
     | Channel | Ch Offset | Width  | Mode |                              |
     |--------------------------------------------------------------------|
     |    1    | B0     B1 |   B2   |  B3  |    B4                     B7 |
     |--------------------------------------------------------------------|

     |----------------------------------------------------------------|
     |     HT     | Non-GF STAs | resv      | OBSS Non-HT  | Reserved |
     | Protection |   Present   |           | STAs Present |          |
     |----------------------------------------------------------------|
     | B0     B1  |     B2      |    B3     |     B4       | B5   B15 |
     |----------------------------------------------------------------|

     |-------------------------------------------------------------|
     | Reserved |  Dual  |  Dual CTS  | Seconday | LSIG TXOP Protn |
     |          | Beacon | Protection |  Beacon  | Full Support    |
     |-------------------------------------------------------------|
     | B0    B5 |   B6   |     B7     |     B8   |       B9        |
     |-------------------------------------------------------------|

     |---------------------------------------|
     |  PCO   |  PCO  | Reserved | Basic MCS |
     | Active | Phase |          |    Set    |
     |---------------------------------------|
     |  B10   |  B11  | B12  B15 |    16     |
     |---------------------------------------|
    ************************************************************************* */
    ht_opern = (mac_ht_opern_stru *)(&puc_payload[MAC_IE_HDR_LEN]);

    /* 提取HT Operation IE中的"STA Channel Width" */
    mac_user_set_bandwidth_info(mac_user, ht_opern->sta_chan_width, mac_user->cur_bandwidth);

    /* 提取HT Operation IE中的"Secondary Channel Offset" */
    ht_hdl.secondary_chan_offset = ht_opern->secondary_chan_offset;

    /* 为了防止5G下用户声称20M，但发送80M数据的情况，在5G情况下该变量不切换 */
    if ((mac_user->avail_bandwidth == 0) && (mac_vap->channel.band == WLAN_BAND_2G)) {
        ht_hdl.secondary_chan_offset = MAC_SCN;
    }

    /* 用户与VAP带宽能力取交集 */
    mac_vap_get_bandwidth_cap(mac_vap, &bwcap_vap);
    bwcap_vap = oal_min(mac_user->bandwidth_cap, bwcap_vap);
    bwcap_vap = oal_min(mac_user->avail_bandwidth, bwcap_vap);
    mac_user_set_bandwidth_info(mac_user, bwcap_vap, bwcap_vap);

    /* 保护相关 */
    ht_hdl.rifs_mode = ht_opern->rifs_mode; /* 发送描述符填写时候需要此值 */
    ht_hdl.ht_protection = ht_opern->ht_protection;
    ht_hdl.nongf_sta_present = ht_opern->nongf_sta_present;
    ht_hdl.obss_nonht_sta_present = ht_opern->obss_nonht_sta_present;
    ht_hdl.lsig_txop_protection_full_support = ht_opern->lsig_txop_protection_full_support;

    mac_user_set_ht_hdl(mac_user, &ht_hdl);

    return change;
}

/* ****************************************************************************
 功能描述  : 构建从20M信道偏移IE
 修改历史      :
  1.日    期   : 2013年12月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_set_second_channel_offset_ie(wlan_channel_bandwidth_enum_uint8 bw, hi_u8 *pauc_buffer,
    hi_u8 *puc_output_len)
{
    /* 默认输出为空 */
    *pauc_buffer    = '\0';
    *puc_output_len = 0;

    /* 11n 设置Secondary Channel Offset Element */
    /* **************************************************************** */
    /* -------------------------------------------------------------- */
    /* |Ele. ID |Length |Secondary channel offset |                   */
    /* -------------------------------------------------------------- */
    /* |1       |1      |1                        |                   */
    /*                                                                */
    /* **************************************************************** */
    pauc_buffer[0] = 62; /* 固定设置为62 */
    pauc_buffer[1] = 1;
    switch (bw) {
        case WLAN_BAND_WIDTH_20M:
            pauc_buffer[2] = 0; /* no secondary channel(pauc_buffer[2]) */
            break;
        case WLAN_BAND_WIDTH_5M:
            pauc_buffer[2] = MAC_BW_5M; /* pauc_buffer[2] : 自定义，窄带5M */
            break;
        case WLAN_BAND_WIDTH_10M:
            pauc_buffer[2] = MAC_BW_10M; /* pauc_buffer[2] : 自定义，窄带10M */
            break;
        default:
            oam_error_log1(0, OAM_SF_SCAN, "{mac_set_second_channel_offset_ie::invalid bandwidth[%d].}", bw);
            return HI_FAIL;
    }
    *puc_output_len = 3; /* output_len固定赋值为3 */
    return HI_SUCCESS;
}

/* 代码ROM段结束位置 新增ROM代码请放在SECTION中 */
#undef __WIFI_ROM_SECTION__

/* ****************************************************************************
 功能描述  : 处理ht cap ie中的 supported channel width
 输入参数  : pst_mac_user_sta           : user结构体指针
             pst_mac_vap                : vap结构体指针
             uc_supported_channel_width : 是否支持40Mhz带宽  0: 不支持， 1: 支持
             en_prev_asoc_ht            : user之前是否已ht站点身份关联到ap  0: 之前未关联， 1: 之前关联过
 返 回 值  :用户中共享区域的st_ht_hdl.bit_supported_channel_width信息
 修改历史      :
  1.日    期   : 2014年1月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_ie_proc_ht_supported_channel_width(const mac_user_stru *mac_user_sta, mac_vap_stru *mac_vap,
    hi_u8 supported_channel_width, hi_bool prev_asoc_ht)
{
    /* 不支持20/40Mhz频宽 */
    if (supported_channel_width == 0) {
        /*  如果STA之前没有作为HT站点与AP关联， 或者STA之前已经作为支持20/40Mhz频宽的HT站点与AP关联 */
        if ((prev_asoc_ht == HI_FALSE) || (mac_user_sta->ht_hdl.ht_capinfo.supported_channel_width == HI_TRUE)) {
            mac_vap->protection.sta_20_m_only_num++;
        }
        return HI_FALSE;
    } else { /* 支持20/40Mhz频宽 */
        /*  如果STA之前已经作为不支持20/40Mhz频宽的HT站点与AP关联 */
        if ((prev_asoc_ht == HI_TRUE) && (mac_user_sta->ht_hdl.ht_capinfo.supported_channel_width == HI_FALSE)) {
            mac_vap->protection.sta_20_m_only_num--;
        }
        return HI_TRUE;
    }
}

/* ****************************************************************************
 功能描述  : 处理ht cap ie中的 ht green field  BIT4
 输入参数  : pst_mac_user_sta  : user结构体指针
             pst_mac_vap       : vap结构体指针
             uc_ht_green_field : 是否支持gf， 0: 不支持， 1: 支持
             en_prev_asoc_ht   : user之前是否已ht站点身份关联到ap  0: 之前未关联， 1: 之前关联过
 返 回 值  :用户公共区域的st_ht_hdl.bit_ht_green_field信息
 修改历史      :
  1.日    期   : 2013年12月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_ie_proc_ht_green_field(const mac_user_stru *mac_user_sta, mac_vap_stru *mac_vap, hi_u8 ht_green_field,
    hi_bool prev_asoc_ht)
{
    /* 不支持Greenfield */
    if (ht_green_field == 0) {
        /*  如果STA之前没有作为HT站点与AP关联， 或者STA之前已经作为支持GF的HT站点与AP关联 */
        if ((prev_asoc_ht == HI_FALSE) || (mac_user_sta->ht_hdl.ht_capinfo.ht_green_field == HI_TRUE)) {
            mac_vap->protection.sta_non_gf_num++;
        }
        return HI_FALSE;
    } else { /* 支持Greenfield */
        /*  如果STA之前已经作为不支持GF的HT站点与AP关联 */
        if ((prev_asoc_ht == HI_TRUE) && (mac_user_sta->ht_hdl.ht_capinfo.ht_green_field == HI_FALSE)) {
            mac_vap->protection.sta_non_gf_num--;
        }
        return HI_TRUE;
    }
}

/* ****************************************************************************
 函 数 名  : mac_ie_proc_lsig_txop_protection_support
 功能描述  : 处理ht cap ie中的 lsig_txop_protection_support
 输入参数  : pst_mac_user_sta                : user结构体指针
             pst_mac_vap                     : vap结构体指针
             uc_lsig_txop_protection_support : 是否支持lsig txop保护， 0: 不支持， 1: 支持
             en_prev_asoc_ht                 : user之前是否已ht站点身份关联到ap  0: 之前未关联， 1: 之前关联过
 返 回 值  :用户公共区域的st_ht_hdl.bit_lsig_txop_protection信息
 修改历史      :
  1.日    期   : 2014年1月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_ie_proc_lsig_txop_protection_support(const mac_user_stru *mac_user_sta, mac_vap_stru *mac_vap,
    hi_u8 lsig_txop_protection_support, hi_bool prev_asoc_ht)
{
    /* 不支持L-sig txop protection */
    if (lsig_txop_protection_support == 0) {
        /*  如果STA之前没有作为HT站点与AP关联， 或者STA之前已经作为支持Lsig txop protection的HT站点与AP关联 */
        if ((prev_asoc_ht == HI_FALSE) || (mac_user_sta->ht_hdl.ht_capinfo.lsig_txop_protection == HI_TRUE)) {
            mac_vap->protection.sta_no_lsig_txop_num++;
        }
        return HI_FALSE;
    } else { /* 支持L-sig txop protection */
        /*  如果STA之前已经作为不支持Lsig txop protection的HT站点与AP关联 */
        if ((prev_asoc_ht == HI_TRUE) && (mac_user_sta->ht_hdl.ht_capinfo.lsig_txop_protection == HI_FALSE)) {
            mac_vap->protection.sta_no_lsig_txop_num--;
        }
        return HI_TRUE;
    }
}

/* ****************************************************************************
 * 函 数 名  :   mac_ie_txbf_set_ht_hdl
 * 功能描述  : 设置txbf feild结构体
 * 输入参数  :
 * 输出参数  :
 * 返 回 值  : hi_void
 * *************************************************************************** */
hi_void mac_ie_txbf_set_ht_hdl(mac_user_ht_hdl_stru *ht_hdl, hi_u32 info_elem)
{
    hi_u32 tmp_txbf_elem = info_elem;

    ht_hdl->imbf_receive_cap                = (tmp_txbf_elem & BIT0);
    ht_hdl->receive_staggered_sounding_cap  = ((tmp_txbf_elem & BIT1) >> 1);
    ht_hdl->transmit_staggered_sounding_cap = ((tmp_txbf_elem & BIT2) >> 2); /* 右移2 bit获取transmit sounding_cap */
    ht_hdl->receive_ndp_cap                 = ((tmp_txbf_elem & BIT3) >> 3); /* 右移3 bit获取receive_ndp_cap */
    ht_hdl->transmit_ndp_cap                = ((tmp_txbf_elem & BIT4) >> 4); /* 右移4 bit获取transmit_ndp_cap */
    ht_hdl->imbf_cap                        = ((tmp_txbf_elem & BIT5) >> 5); /* 右移5 bit获取imbf_cap */
    ht_hdl->calibration                     = ((tmp_txbf_elem & 0x000000C0) >> 6);  /* 右移6 bit获取calibration */
    ht_hdl->exp_csi_txbf_cap                = ((tmp_txbf_elem & BIT8) >> 8);   /* 右移8 bit获取exp_csi_txbf_cap */
    ht_hdl->exp_noncomp_txbf_cap            = ((tmp_txbf_elem & BIT9) >> 9);   /* 右移9 bit获取exp_noncomp_txbf_cap */
    ht_hdl->exp_comp_txbf_cap               = ((tmp_txbf_elem & BIT10) >> 10); /* 右移10 bit获取exp_comp_txbf_cap */
    ht_hdl->exp_csi_feedback                = ((tmp_txbf_elem & 0x00001800) >> 11); /* 右移11 bit获取exp_csi_feedback */
    ht_hdl->exp_noncomp_feedback            = ((tmp_txbf_elem & 0x00006000) >> 13); /* 右移13 bit获取noncomp_feedback */

    ht_hdl->exp_comp_feedback               = ((tmp_txbf_elem & 0x0001C000) >> 15); /* 右移15 bit获取comp_feedback */
    ht_hdl->min_grouping                    = ((tmp_txbf_elem & 0x00060000) >> 17); /* 右移17 bit获取min_grouping */
    ht_hdl->csi_bfer_ant_number             = ((tmp_txbf_elem & 0x001C0000) >> 19); /* 右移19 bit获取bfer_ant_number */
    ht_hdl->noncomp_bfer_ant_number         = ((tmp_txbf_elem & 0x00600000) >> 21); /* 右移21 bit获取bfer_ant_number */
    ht_hdl->comp_bfer_ant_number            = ((tmp_txbf_elem & 0x01C00000) >> 23); /* 右移23 bit获取bfer_ant_number */
    ht_hdl->csi_bfee_max_rows               = ((tmp_txbf_elem & 0x06000000) >> 25); /* 右移25 bit获取bfee_max_rows */
    ht_hdl->channel_est_cap                 = ((tmp_txbf_elem & 0x18000000) >> 27); /* 右移27 bit获取channel_est_cap */
}

/* ****************************************************************************
 功能描述  : 搜索asoc rsp frame帧中的HT cap IE
 修改历史      :
  1.日    期   : 2013年7月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_ie_proc_ht_sta(const mac_vap_stru *mac_vap, const hi_u8 *puc_payload, const hi_u16 *pus_index,
    mac_user_stru *mac_user_ap, hi_u16 *pus_ht_cap_info, hi_u16 *pus_amsdu_max)
{
    hi_u16                us_offset = *pus_index;
    mac_user_stru        *mac_user = mac_user_ap;
    mac_user_ht_hdl_stru  ht_hdl_value;
    mac_user_ht_hdl_stru *ht_hdl = &ht_hdl_value;
    hi_u8 mcs_bmp_index;

    mac_user_get_ht_hdl(mac_user, ht_hdl);
    /* 带有 HT Capability Element 的 AP，标示它具有HT capable. */
    ht_hdl->ht_capable = HI_TRUE;
    us_offset += MAC_IE_HDR_LEN;
    /* ****************************************** */
    /*     解析 HT Capabilities Info Field      */
    /* ****************************************** */
    *pus_ht_cap_info = hi_makeu16(puc_payload[us_offset], puc_payload[us_offset + 1]);
    /* 检查STA所支持的LDPC编码能力 B0，0:不支持，1:支持 */
    ht_hdl->ht_capinfo.ldpc_coding_cap = (*pus_ht_cap_info & BIT0);
    /* 提取AP所支持的带宽能力  */
    ht_hdl->ht_capinfo.supported_channel_width = ((*pus_ht_cap_info & BIT1) >> 1);
    /* 检查空间复用节能模式 B2~B3 */
    hi_u8 smps = (*pus_ht_cap_info & (BIT2 | BIT3));
    ht_hdl->ht_capinfo.sm_power_save = mac_ie_proc_sm_power_save_field(smps);

    ht_hdl->ht_capinfo.ht_green_field = ((*pus_ht_cap_info & BIT4) >> 4); /* 提取AP支持Greenfield情况,右移4bit */

    ht_hdl->ht_capinfo.short_gi_20mhz = ((*pus_ht_cap_info & BIT5) >> 5); /* 提取AP支持20MHz Short-GI情况,右移5bit */

    ht_hdl->ht_capinfo.short_gi_40mhz = ((*pus_ht_cap_info & BIT6) >> 6); /* 提取AP支持40MHz Short-GI情况,右移6bit */

    ht_hdl->ht_capinfo.rx_stbc = (hi_u8)((*pus_ht_cap_info & 0x30) >> 4); /* 提取AP支持STBC PPDU情况,右移4bit */
    /* 提取AP支持最大A-MSDU长度情况 */
    *pus_amsdu_max =
        ((*pus_ht_cap_info & BIT11) == 0) ? WLAN_MIB_MAX_AMSDU_LENGTH_SHORT : WLAN_MIB_MAX_AMSDU_LENGTH_LONG;

    ht_hdl->ht_capinfo.dsss_cck_mode_40mhz = ((*pus_ht_cap_info & BIT12) >> 12); /* 12:提取40M上DSSS/CCK的支持情况 */

    ht_hdl->ht_capinfo.lsig_txop_protection = ((*pus_ht_cap_info & BIT15) >> 15); /* 15:提取L-SIG TXOP 保护的支持情况 */
    us_offset += MAC_HT_CAPINFO_LEN;

    /* ****************************************** */
    /*     解析 A-MPDU Parameters Field         */
    /* ****************************************** */
    /* 提取 Maximum Rx A-MPDU factor (B1 - B0) */
    ht_hdl->max_rx_ampdu_factor = (puc_payload[us_offset] & 0x03);

    ht_hdl->min_mpdu_start_spacing = (puc_payload[us_offset] >> 2) & 0x07; /* 提取 Minmum Rx A-MPDU factor (B3 - B2) */
    us_offset += MAC_HT_AMPDU_PARAMS_LEN;

    /* ****************************************** */
    /*     解析 Supported MCS Set Field         */
    /* ****************************************** */
    for (mcs_bmp_index = 0; mcs_bmp_index < WLAN_HT_MCS_BITMASK_LEN; mcs_bmp_index++) {
        ht_hdl->rx_mcs_bitmask[mcs_bmp_index] =
            (mac_vap->mib_info->supported_mcstx.auc_dot11_supported_mcs_tx_value[mcs_bmp_index]) &
            (*(hi_u8 *)(puc_payload + us_offset + mcs_bmp_index));
    }
    ht_hdl->rx_mcs_bitmask[WLAN_HT_MCS_BITMASK_LEN - 1] &= 0x1F;
    us_offset += MAC_HT_SUP_MCS_SET_LEN;

    /* ****************************************** */
    /* 解析 HT Extended Capabilities Info Field */
    /* ****************************************** */
    *pus_ht_cap_info = hi_makeu16(puc_payload[us_offset], puc_payload[us_offset + 1]);
    /* 提取 HTC support Information */
    if ((*pus_ht_cap_info & BIT10) != 0) {
        ht_hdl->htc_support = 1;
    }
    us_offset += MAC_HT_EXT_CAP_LEN;

    /* ****************************************** */
    /*  解析 Tx Beamforming Field               */
    /* ****************************************** */
    hi_u16 us_tmp_info_elem = hi_makeu16(puc_payload[us_offset], puc_payload[us_offset + 1]);
    hi_u16 us_tmp_txbf_low  = hi_makeu16(puc_payload[us_offset + 2], puc_payload[us_offset + 3]); /* 2/3:偏移 */
    hi_u32 tmp_txbf_elem    = hi_makeu32(us_tmp_info_elem, us_tmp_txbf_low);

    mac_ie_txbf_set_ht_hdl(ht_hdl, tmp_txbf_elem);

    mac_user_set_ht_hdl(mac_user, ht_hdl);
}

/* ****************************************************************************
 功能描述  : 检查action帧是不是p2p帧
 修改历史      :
  1.日    期   : 2014年12月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_ie_check_p2p_action(const hi_u8 *puc_payload)
{
    /* 找到WFA OUI */
    hi_u8 auc_oui[MAC_OUI_LEN] = {(hi_u8)MAC_WLAN_OUI_P2P0, (hi_u8)MAC_WLAN_OUI_P2P1, (hi_u8)MAC_WLAN_OUI_P2P2};

    if ((memcmp(puc_payload, auc_oui, MAC_OUI_LEN) == 0) && (puc_payload[MAC_OUI_LEN] == MAC_OUITYPE_P2P)) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 更新ht cap ie中的 sm power save field B2~B3
 输入参数  : pst_mac_user_sta --用户结构体指针，uc_smps--用户smps模式
 返 回 值  : 用户信息中st_ht_hdl.bit_sm_power_save的信息
 修改历史      :
  1.日    期   : 2013年12月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
wlan_mib_mimo_power_save_enum_uint8 mac_ie_proc_sm_power_save_field(hi_u8 smps)
{
    if (smps == MAC_SMPS_STATIC_MODE) {
        return WLAN_MIB_MIMO_POWER_SAVE_STATIC;
    } else if (smps == MAC_SMPS_DYNAMIC_MODE) {
        return WLAN_MIB_MIMO_POWER_SAVE_DYNAMIC;
    } else {
        return WLAN_MIB_MIMO_POWER_SAVE_MIMO;
    }
}

/* ****************************************************************************
 功能描述  : 从帧体中解析ie中的chan信息，先在HT operation IE中找chan信息，如果找到就返回，如找不到，
             再在DSSS Param set ie中寻找
 修改历史      :
  1.日    期   : 2014年2月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_ie_get_chan_num(hi_u8 *puc_frame_body, hi_u16 us_frame_len, hi_u16 us_offset, hi_u8 curr_chan)
{
    hi_u8 chan_num;
    hi_u8 *puc_ie_start_addr = HI_NULL;

    if (us_frame_len > us_offset) {
        /* 在DSSS Param set ie中解析chan num */
        puc_ie_start_addr = mac_find_ie(MAC_EID_DSPARMS, puc_frame_body + us_offset, us_frame_len - us_offset);
        if ((puc_ie_start_addr != HI_NULL) && (puc_ie_start_addr[1] == MAC_DSPARMS_LEN)) {
            chan_num = puc_ie_start_addr[2]; /* 从ie_start_addr第2 byte获取信道数 */
            if (mac_is_channel_num_valid(mac_get_band_by_channel_num(chan_num), chan_num) == HI_SUCCESS) {
                return chan_num;
            }
        }
        /* 在HT operation ie中解析 chan num */
        puc_ie_start_addr = mac_find_ie(MAC_EID_HT_OPERATION, puc_frame_body + us_offset, us_frame_len - us_offset);
        if ((puc_ie_start_addr != HI_NULL) && (puc_ie_start_addr[1] >= 1)) {
            chan_num = puc_ie_start_addr[2]; /* 从ie_start_addr第2 byte获取信道数 */
            if (mac_is_channel_num_valid(mac_get_band_by_channel_num(chan_num), chan_num) == HI_SUCCESS) {
                return chan_num;
            }
        }
    }
    chan_num = curr_chan;
    return chan_num;
}

/* ****************************************************************************
 功能描述  : 根据"带宽模式"获取对应的"次信道偏移量"
 输入参数  : en_bandwidth: 带宽模式
 返 回 值  : 次信道偏移量
**************************************************************************** */
WIFI_ROM_TEXT mac_sec_ch_off_enum_uint8 mac_get_sco_from_bandwidth(wlan_channel_bandwidth_enum_uint8 bandwidth)
{
    switch (bandwidth) {
        case WLAN_BAND_WIDTH_40PLUS:
        case WLAN_BAND_WIDTH_80PLUSPLUS:
        case WLAN_BAND_WIDTH_80PLUSMINUS:
            return MAC_SCA;
        case WLAN_BAND_WIDTH_40MINUS:
        case WLAN_BAND_WIDTH_80MINUSPLUS:
        case WLAN_BAND_WIDTH_80MINUSMINUS:
            return MAC_SCB;
        default:
            return MAC_SCN;
    }
}

/* ****************************************************************************
 功能描述  : 根据信道中心频点获取对应的"带宽模式"
 输入参数  : uc_channel         : 信道号
             uc_chan_center_freq: 信道中心频点
 修改历史      :
  1.日    期   : 2014年2月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
wlan_channel_bandwidth_enum_uint8 mac_get_bandwith_from_center_freq_seg0(hi_u8 channel, hi_u8 chan_center_freq)
{
    switch (chan_center_freq - channel) {
        case 6: /* 6: 从20信道+1, 从40信道+1 场景 */
            /* **********************************************************************
            | 主20 | 从20 | 从40       |
                          |
                          |中心频率相对于主20偏6个信道
            *********************************************************************** */
            return WLAN_BAND_WIDTH_80PLUSPLUS;
        case -2: /* -2: 从20信道+1, 从40信道-1 场景 */
            /* **********************************************************************
            | 从40        | 主20 | 从20 |
                          |
                          |中心频率相对于主20偏-2个信道
            *********************************************************************** */
            return WLAN_BAND_WIDTH_80PLUSMINUS;
        case 2: /* 2: 从20信道-1, 从40信道+1 场景 */
            /* **********************************************************************
            | 从20 | 主20 | 从40       |
                          |
                          |中心频率相对于主20偏2个信道
            *********************************************************************** */
            return WLAN_BAND_WIDTH_80MINUSPLUS;
        case -6: /* -6: 从20信道-1, 从40信道-1 场景 */
            /* **********************************************************************
            | 从40        | 从20 | 主20 |
                          |
                          |中心频率相对于主20偏-6个信道
            *********************************************************************** */
            return WLAN_BAND_WIDTH_80MINUSMINUS;
        default:
            return 0;
    }
}

/* ****************************************************************************
 功能描述  : 根据"次信道偏移量"获取对应的带宽模式
 输入参数  : en_sec_chan_offset: 次信道偏移量
 返 回 值  : 带宽模式
**************************************************************************** */
wlan_channel_bandwidth_enum_uint8 mac_get_bandwidth_from_sco(mac_sec_ch_off_enum_uint8 sec_chan_offset)
{
    switch (sec_chan_offset) {
        case MAC_SCA: /* Secondary Channel Above */
            return WLAN_BAND_WIDTH_40PLUS;
        case MAC_SCB: /* Secondary Channel Below */
            return WLAN_BAND_WIDTH_40MINUS;
        case MAC_BW_5M:
            return WLAN_BAND_WIDTH_5M;
        case MAC_BW_10M:
            return WLAN_BAND_WIDTH_10M;
        default: /* No Secondary Channel    */
            return WLAN_BAND_WIDTH_20M;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
