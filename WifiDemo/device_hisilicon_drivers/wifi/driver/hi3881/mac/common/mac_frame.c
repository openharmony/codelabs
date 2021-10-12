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
#include "mac_frame.h"
#include "mac_vap.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "mac_regdomain.h"
#include "mac_data.h"
#include "frw_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* ****************************************************************************
  全局变量定义
**************************************************************************** */
#define __WIFI_ROM_SECTION__ /* 代码ROM段起始位置 */
/* ****************************************************************************
  函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 查找指定的IE
 输入参数  : [1]eid
             [2]puc_ies
             [3]l_len
 返 回 值  : const hi_u8 *
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 *mac_find_ie(hi_u8 eid, hi_u8 *puc_ies, hi_u32 l_len)
{
    if (puc_ies == HI_NULL) {
        return HI_NULL;
    }
    while (l_len > MAC_IE_HDR_LEN && puc_ies[0] != eid) {
        if (l_len < (hi_u32)(puc_ies[1] + MAC_IE_HDR_LEN)) {
            break;
        }
        l_len -= puc_ies[1] + MAC_IE_HDR_LEN;
        puc_ies += puc_ies[1] + MAC_IE_HDR_LEN;
    }
    if ((l_len < MAC_IE_HDR_LEN) || (l_len < (hi_u32)(MAC_IE_HDR_LEN + puc_ies[1])) ||
        ((l_len == MAC_IE_HDR_LEN) && (puc_ies[0] != eid))) {
        return HI_NULL;
    }
    return puc_ies;
}

/* ****************************************************************************
 功能描述  : 查找厂家自定义 IE
 输入参数  : [1]oui
             [2]oui_type
             [3]puc_ies
             [4]l_len
 返 回 值  : const hi_u8 * 摘自linux 内核
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 *mac_find_vendor_ie(hi_u32 oui, hi_u8 oui_type, hi_u8 *puc_ies, hi_s32 l_len)
{
    mac_ieee80211_vendor_ie_stru *ie = HI_NULL;
    hi_u8 *puc_pos = HI_NULL;
    hi_u8 *puc_end = HI_NULL;
    hi_u32 ie_oui;

    if (puc_ies == HI_NULL) {
        return HI_NULL;
    }
    puc_pos = puc_ies;
    puc_end = puc_ies + l_len;
    while (puc_pos < puc_end) {
        puc_pos = mac_find_ie(MAC_EID_VENDOR, puc_pos, (hi_u32)(puc_end - puc_pos));
        if (puc_pos == HI_NULL) {
            return HI_NULL;
        }
        ie = (mac_ieee80211_vendor_ie_stru *)puc_pos;
        if (ie->len >= (sizeof(mac_ieee80211_vendor_ie_stru) - MAC_IE_HDR_LEN)) {
            ie_oui = (ie->auc_oui[0] << 16) |           /* auc_oui[0]存于最高16bit */
                (ie->auc_oui[1] << 8) | ie->auc_oui[2]; /* auc_oui[1]存于次高8bit，auc_oui[2]存于最低8bit */
            if ((ie_oui == oui) && (ie->oui_type == oui_type)) {
                return puc_pos;
            }
        }
        puc_pos += 2 + ie->len; /* 每次循环偏移(2 + ie->len)byte */
    }
    return HI_NULL;
}

/* ****************************************************************************
 功能描述  : 作为ap时，根据mib值，设置cap info
 输入参数  : pst_vap      : 指向vap
             puc_cap_info : 指向存储能力位信息的buffer
 修改历史      :
  1.日    期   : 2013年4月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_cap_info_ap(hi_void *vap, hi_u8 *puc_cap_info)
{
    mac_cap_info_stru  *cap_info = (mac_cap_info_stru *)puc_cap_info;
    mac_vap_stru       *mac_vap  = (mac_vap_stru *)vap;

    wlan_mib_ieee802dot11_stru *mib = mac_vap->mib_info;
    /* *************************************************************************
         -------------------------------------------------------------------
         |B0 |B1  |B2        |B3    |B4     |B5      |B6  |B7     |B8      |
         -------------------------------------------------------------------
         |ESS|IBSS|CFPollable|CFPReq|Privacy|Preamble|PBCC|Agility|SpecMgmt|
         -------------------------------------------------------------------
         |B9 |B10      |B11 |B12     |B13      |B14        |B15            |
         -------------------------------------------------------------------
         |QoS|ShortSlot|APSD|RM      |DSSS-OFDM|Delayed BA |Immediate BA   |
         -------------------------------------------------------------------
    ************************************************************************** */
    /* 初始清零 */
    puc_cap_info[0] = 0;
    puc_cap_info[1] = 0;

    if (WLAN_MIB_DESIRED_BSSTYPE_INDEPENDENT == mib->wlan_mib_sta_config.dot11_desired_bss_type) {
        cap_info->ibss = 1;
    } else if (WLAN_MIB_DESIRED_BSSTYPE_INFRA == mib->wlan_mib_sta_config.dot11_desired_bss_type) {
        cap_info->ess = 1;
    }

    /* The Privacy bit is set if WEP is enabled */
    cap_info->privacy = mib->wlan_mib_privacy.dot11_privacy_invoked;
    /* preamble */
    cap_info->short_preamble = mac_mib_get_short_preamble_option_implemented(mac_vap);
    /* packet binary convolutional code (PBCC) modulation */
    cap_info->pbcc = mib->phy_hrdsss.dot11_pbcc_option_implemented;
    /* Channel Agility */
    cap_info->channel_agility = mib->phy_hrdsss.dot11_channel_agility_present;
    /* Spectrum Management */
    cap_info->spectrum_mgmt = mib->wlan_mib_sta_config.dot11_spectrum_management_required;
    /* QoS subfield */
    cap_info->qos = mib->wlan_mib_sta_config.dot11_qos_option_implemented;
    /* short slot */
    cap_info->short_slot_time = 1;
    /* APSD */
    cap_info->apsd = mib->wlan_mib_sta_config.dot11_apsd_option_implemented;
    /* Radio Measurement */
    cap_info->radio_measurement = mib->wlan_mib_sta_config.dot11_radio_measurement_activated;
    /* DSSS-OFDM */
    cap_info->dsss_ofdm = HI_FALSE;
    /* Delayed BA */
    cap_info->delayed_block_ack = mib->wlan_mib_sta_config.dot11_delayed_block_ack_option_implemented;
    /* Immediate Block Ack 参考STA及AP标杆，此能力一直为0,实际通过addba协商。此处修改为标杆一致。mib值不修改 */
    cap_info->immediate_block_ack = 0;
}

WIFI_ROM_RODATA const hi_char g_dmac_p2p_wildcard_ssid[MAC_P2P_WILDCARD_SSID] = "DIRECT-";
/* ****************************************************************************
 功能描述  : 设置ssid ie
 输入参数  : pst_vap: 指向vap
             puc_buffer : 指向buffer
 输出参数  : puc_ie_len : element的长度
 修改历史      :
  1.日    期   : 2013年4月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u16 mac_set_ssid_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len, hi_u16 us_frm_type)
{
    hi_u8 *puc_ssid = HI_NULL;
    hi_u8 ssid_len;
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;

    /* **************************************************************************
                    ----------------------------
                    |Element ID | Length | SSID|
                    ----------------------------
           Octets:  |1          | 1      | 0~32|
                    ----------------------------
    ************************************************************************** */
    /* **************************************************************************
      A SSID  field  of length 0 is  used  within Probe
      Request management frames to indicate the wildcard SSID.
    ************************************************************************** */
    *puc_buffer = MAC_EID_SSID;

    /* 只有beacon会隐藏ssid */
    if ((mac_vap->cap_flag.hide_ssid) && (WLAN_FC0_SUBTYPE_BEACON == us_frm_type)) {
        /* ssid len */
        *(puc_buffer + 1) = 0;
        *puc_ie_len = MAC_IE_HDR_LEN;
        return 0;
    }

    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA && mac_vap->p2p_mode == WLAN_P2P_DEV_MODE) {
        ssid_len = (hi_u8)sizeof(g_dmac_p2p_wildcard_ssid) - 1; /* 不包含'\0' */
        *(puc_buffer + 1) = ssid_len;

        if (memcpy_s(puc_buffer + MAC_IE_HDR_LEN, ssid_len, g_dmac_p2p_wildcard_ssid, ssid_len) != EOK) {
            oam_error_log0(0, OAM_SF_P2P, "{mac_set_ssid_ie::mem safe func err!}");
            /* ssid len */
            *(puc_buffer + 1) = 0;
            *puc_ie_len = MAC_IE_HDR_LEN;
            return 0;
        }
    } else {
        puc_ssid = mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_desired_ssid;
        ssid_len = (hi_u8)strlen((hi_char *)puc_ssid); /* 不包含'\0' */
        *(puc_buffer + 1) = ssid_len;
        if (memcpy_s(puc_buffer + MAC_IE_HDR_LEN, ssid_len, puc_ssid, ssid_len) != EOK) {
            oam_error_log0(0, 0, "{mac_set_ssid_ie::mem safe func err!}");
            /* ssid len */
            *(puc_buffer + 1) = 0;
            *puc_ie_len = MAC_IE_HDR_LEN;
            return 0;
        }
    }
    *puc_ie_len = ssid_len + MAC_IE_HDR_LEN;
    return ssid_len;
}

/* ****************************************************************************
 功能描述  : 设置速率集
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_supported_rates_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru     *mac_vap   = (mac_vap_stru *)vap;
    mac_rateset_stru *rates_set = HI_NULL;
    hi_u8 nrates;
    hi_u8 idx;

    rates_set = &(mac_vap->curr_sup_rates.rate);
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
    nrates = rates_set->rs_nrates;
    if (nrates > MAC_MAX_SUPRATES) {
        nrates = MAC_MAX_SUPRATES;
    }
    for (idx = 0; idx < nrates; idx++) {
        puc_buffer[MAC_IE_HDR_LEN + idx] = rates_set->ast_rs_rates[idx].mac_rate;
    }
    puc_buffer[1] = nrates;
    *puc_ie_len = MAC_IE_HDR_LEN + nrates;
}

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* ****************************************************************************
 功能描述  : Mesh填充回复给Mesh节点的rsn信息
 输入参数  : pst_vap   : 指向vap
             puc_buffer: 指向buffer
             hi_u16 us_frm_type
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2019年1月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_rsn_mesh_cap(mac_rsn_cap_stru *rsn_cap, const mac_vap_stru *mac_vap)
{
    rsn_cap->mfpr        = mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpr;
    rsn_cap->mfpc        = mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpc;
    rsn_cap->pre_auth    =
        mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_preauthentication_activated;
    rsn_cap->no_pairwise = 0;
    rsn_cap->ptska_relay_counter =
        mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_number_of_ptksa_replay_counters_implemented;
    rsn_cap->gtska_relay_counter =
        mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_number_of_gtksa_replay_counters_implemented;
}

WIFI_ROM_TEXT static hi_void mac_set_rsn_group_cipher_suite_field(const mac_vap_stru *mac_vap,
    hi_u8 *puc_buffer, hi_u16 us_frm_type, hi_u8 *puc_ie_len)
{
    hi_u8 auc_oui[MAC_OUI_LEN];
    hi_u8 index = 0;

    if (us_frm_type == WLAN_FC0_SUBTYPE_PROBE_RSP) {
        index = MAC_IE_HDR_LEN;
        /* 设置RSN ie的EID */
        puc_buffer[0] = MAC_EID_RSN;
    } else if (us_frm_type == WLAN_FC0_SUBTYPE_BEACON) {
        index = MAC_IE_VENDOR_SPEC_MESH_HDR_LEN;
        puc_buffer[0] = MAC_EID_VENDOR;
        auc_oui[0] = (hi_u8)MAC_WLAN_OUI_VENDOR0;
        auc_oui[1] = (hi_u8)MAC_WLAN_OUI_VENDOR1;
        auc_oui[2] = (hi_u8)MAC_WLAN_OUI_VENDOR2; /* auc_oui[2]赋值为MAC_WLAN_OUI_VENDOR2 */
        if (memcpy_s(&puc_buffer[2], MAC_OUI_LEN, auc_oui, MAC_OUI_LEN) != EOK) { /* puc_buffer[2]赋值 */
            return;
        }
        puc_buffer[5] = MAC_OUITYPE_MESH;             /* puc_buffer[5]赋值为MAC_OUITYPE_MESH */
        puc_buffer[6] = MAC_OUISUBTYPE_MESH_HISI_RSN; /* puc_buffer[6]赋值为MAC_OUISUBTYPE_MESH_HISI_RSN */
    }
    auc_oui[0] = (hi_u8)MAC_WLAN_OUI_RSN0;
    auc_oui[1] = (hi_u8)MAC_WLAN_OUI_RSN1;
    auc_oui[2] = (hi_u8)MAC_WLAN_OUI_RSN2; /* auc_oui[2]赋值为MAC_WLAN_OUI_RSN2 */
    /* 设置version字段 */
    puc_buffer[index++] = MAC_RSN_IE_VERSION;
    puc_buffer[index++] = 0;

    /* 设置Group Cipher Suite */
    /* *********************************************************************** */
    /*                  Group Cipher Suite                                   */
    /* --------------------------------------------------------------------- */
    /*                  | OUI | Suite type |                                 */
    /* --------------------------------------------------------------------- */
    /*          Octets: |  3  |     1      |                                 */
    /* --------------------------------------------------------------------- */
    /* *********************************************************************** */
    if (memcpy_s(&puc_buffer[index], MAC_OUI_LEN, auc_oui, MAC_OUI_LEN) != EOK) {
        return;
    }
    index += MAC_OUI_LEN;
    puc_buffer[index++] = mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher;
    /* 设置成对加密套件数量 */
    puc_buffer[index++] = 1;
    puc_buffer[index++] = 0;
    if (memcpy_s(&puc_buffer[index], MAC_OUI_LEN, auc_oui, MAC_OUI_LEN) != EOK) {
        return;
    }
    index += MAC_OUI_LEN;
    puc_buffer[index++] = WLAN_80211_CIPHER_SUITE_CCMP; /* Mesh 下只使用CCMP-128 ,暂不考虑其他 */
    /* 设置认证套件数 (Mesh下只支持SAE) */
    puc_buffer[index++] = 1;
    puc_buffer[index++] = 0;
    /* 根据MIB 值，设置认证套件内容 */
    if (memcpy_s(&puc_buffer[index], MAC_OUI_LEN, auc_oui, MAC_OUI_LEN) != EOK) {
        return;
    }
    index += MAC_OUI_LEN;
    puc_buffer[index++] = WLAN_AUTH_SUITE_SAE_SHA256; /* MESH 下只支持SAE */

    *puc_ie_len = index;
}

WIFI_ROM_TEXT hi_void mac_set_rsn_mesh_ie_authenticator(hi_void *vap, hi_u8 *puc_buffer, hi_u16 us_frm_type,
    hi_u8 *puc_ie_len)
{
    mac_vap_stru        *mac_vap = (mac_vap_stru *)vap;
    mac_rsn_cap_stru    *rsn_cap = HI_NULL;
    hi_u8               index = 0;
    hi_u8               ie_len = 0;

    if (mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated != HI_TRUE) {
        *puc_ie_len = 0;
        return;
    }
    if (mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_security_activated != HI_TRUE) {
        *puc_ie_len = 0;
        return;
    }
    if (mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_active_authentication_protocol !=
        MAC_MIB_AUTH_PROTOCOL_SAE) {
        *puc_ie_len = 0;
        return;
    }
    /* *********************************************************************** */
    /*                  RSN Element Format              */
    /* --------------------------------------------------------------------- */
    /* |Element ID | Length | Version | Group Cipher Suite | Pairwise Cipher */
    /* --------------------------------------------------------------------- */
    /* | 1         | 1      | 2       |      4             |     2           */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* Suite | Pairwise Cipher Suite List | AKM Suite Count | AKM Suite List */
    /* --------------------------------------------------------------------- */
    /*       | 4-m                        |     2          | 4-n             */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* |RSN Capabilities|PMKID Count|PMKID List|Group Management Cipher Suite */
    /* --------------------------------------------------------------------- */
    /* |    2           |    2      |16 -s     |         4                 | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    mac_set_rsn_group_cipher_suite_field(mac_vap, &puc_buffer[index], us_frm_type, &ie_len);
    index += ie_len;
    /* 设置 RSN Capabilities字段 */
    /* *********************************************************************** */
    /* --------------------------------------------------------------------- */
    /* | B15 - B6  |  B5 - B4      | B3 - B2     |       B1    |     B0    | */
    /* --------------------------------------------------------------------- */
    /* | Reserved  |  GTSKA Replay | PTSKA Replay| No Pairwise | Pre - Auth| */
    /* |           |    Counter    |   Counter   |             |           | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* 设置RSN Capabilities 值，包括Pre_Auth, no_pairwise */
    /* Replay counters (PTKSA and GTKSA)                    */
    rsn_cap = (mac_rsn_cap_stru *)(puc_buffer + index);
    if (memset_s(rsn_cap, sizeof(mac_rsn_cap_stru), 0, sizeof(mac_rsn_cap_stru)) != EOK) {
        return;
    }
    index += MAC_RSN_CAP_LEN;

    mac_set_rsn_mesh_cap(rsn_cap, mac_vap);

    /* 设置RSN element的长度 */
    puc_buffer[1] = index - MAC_IE_HDR_LEN;

    *puc_ie_len = index;
}

/* ****************************************************************************
 功能描述  : 查找mesh自定义 IE(精确到subtype)
 输入参数  : [1]oui_sub_type,
             [2]puc_ies,
             [3]l_len
 返 回 值  : hi_u8 *
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 *mac_find_mesh_vendor_ie(hi_u8 oui_sub_type, hi_u8 *puc_ies, hi_u32 l_len)
{
    mac_ieee80211_vendor_ie_stru *ie = HI_NULL;
    hi_u8 *puc_pos = HI_NULL;
    hi_u8 *puc_end = HI_NULL;
    hi_u32 ie_oui;

    puc_pos = puc_ies;
    puc_end = puc_ies + l_len;
    while (puc_pos < puc_end) {
        puc_pos = mac_find_ie(MAC_EID_VENDOR, puc_pos, (hi_u32)(puc_end - puc_pos));
        if (puc_pos == HI_NULL) {
            return HI_NULL;
        }
        ie = (mac_ieee80211_vendor_ie_stru *)puc_pos;
        if (ie->len > (sizeof(mac_ieee80211_vendor_ie_stru) - MAC_IE_HDR_LEN)) {
            ie_oui = (ie->auc_oui[0] << 16) |           /* auc_oui[0]存于最高16bit */
                (ie->auc_oui[1] << 8) | ie->auc_oui[2]; /* auc_oui[1]存于次高8bit，auc_oui[2]存于最低8bit */
            if ((ie_oui == MAC_WLAN_OUI_VENDOR) && (ie->oui_type == MAC_OUITYPE_MESH) &&
                (puc_pos[MAC_IE_VENDOR_SPEC_MESH_SUBTYPE_POS] == oui_sub_type)) {
                return puc_pos;
            }
        }
        puc_pos += 2 + ie->len; /* 每次偏移(2 + ie->len) byte */
    }
    return HI_NULL;
}

/* ****************************************************************************
 功能描述  : 接收时检查私有字段，判断包的发送端是否为Mesh VAP
 输入参数  :
             [1]puc_buffer
             [2]puc_ie_len
 返 回 值  : hi_u8
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_check_is_mesh_vap(hi_u8 *puc_buffer, hi_u8 puc_ie_len)
{
    hi_u8 *hisi_beacon_ie = HI_NULL;
    hi_u8 *hisi_prb_rsp_ie = HI_NULL;

    hisi_beacon_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_HISI_BEACON, puc_buffer, puc_ie_len);
    hisi_prb_rsp_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_HISI_RSP, puc_buffer, puc_ie_len);
    if ((hisi_beacon_ie == HI_NULL) && (hisi_prb_rsp_ie == HI_NULL)) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

/* ****************************************************************************
 功能描述  : 获取beacon帧中的meshid(私有字段中)
 输入参数  : puc_beacon_body:Beacon or probe rsp帧体
                            hi_s32 l_frame_body_len:帧体长度
 输出参数  : puc_meshid_len:meshid长度
 返 回 值  : 指向meshid
 修改历史      :
  1.日    期   : 2019年4月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
WIFI_ROM_TEXT hi_u8 *mac_get_meshid(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len, hi_u8 *puc_meshid_len)
{
    const hi_u8 *puc_meshid_ie = HI_NULL;
    hi_u16 us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    /* *********************************************************************** */
    /*                       Beacon Frame - Frame Body                       */
    /* --------------------------------------------------------------------- */
    /* |Timestamp |BeaconInt |CapInfo |SSID |SupRates |DSParSet |TIM elm   | */
    /* --------------------------------------------------------------------- */
    /* |8         |2         |2       |2-34 |3-10     |3        |4-256     | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* **************************************************************************
         Vendor Spec |Length| OUI|TYPE|SUBTYPE|MESHID LEN|MESHID|
           1                |1        |3    |1     |1           |  1               |Var       |
    ************************************************************************** */
    /* meshid的长度初始赋值为0 */
    *puc_meshid_len = 0;

    /* 检测beacon帧或者probe rsp帧的长度的合法性 */
    if (l_frame_body_len <= us_offset) {
        oam_warning_log0(0, OAM_SF_ANY, "{mac_get_meshid:: the length of beacon/probe rsp frame body is invalid.}");
        return HI_NULL;
    }

    puc_meshid_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_HISI_MESHID, (puc_beacon_body + us_offset),
        (hi_u32)(l_frame_body_len - us_offset));
    /* 查找meshid的ie */
    if ((puc_meshid_ie != HI_NULL) && (puc_meshid_ie[MAC_MESH_MESHID_LEN_POS] < WLAN_MESHID_MAX_LEN)) {
        /* 获取ssid ie的长度 */
        *puc_meshid_len = puc_meshid_ie[MAC_MESH_MESHID_LEN_POS];

        return (hi_u8 *)(puc_meshid_ie + MAC_MESH_MESHID_OFFSET);
    }

    return HI_NULL;
}

/* ****************************************************************************
 功能描述  : 获取beacon帧中的beacon priority字段，上报new peer candidate时使用(私有字段中)
 输入参数  : puc_beacon_body:Beacon or probe rsp帧体
                            hi_s32 l_frame_body_len:帧体长度
 返 回 值  : uc_bcn_prio:beacon priority
 修改历史      :
  1.日    期   : 2019年5月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_get_hisi_beacon_prio(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len)
{
    hi_u8 bcn_prio = 0;
    hi_u8 *puc_bcn_prio_ie = HI_NULL;
    hi_u16 us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    /* *********************************************************************** */
    /*                       Beacon Frame - Frame Body                       */
    /* --------------------------------------------------------------------- */
    /* |Timestamp |BeaconInt |CapInfo |SSID |SupRates |DSParSet |TIM elm   | */
    /* --------------------------------------------------------------------- */
    /* |8         |2         |2       |2-34 |3-10     |3        |4-256     | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* **************************************************************************
         Vendor Spec |Length| OUI|TYPE|SUBTYPE|Beacon Priority|
           1                |1        |3    |1     |1            |  1                  |
    ************************************************************************** */
    /* 检测beacon帧或者probe rsp帧的长度的合法性 */
    if (l_frame_body_len <= us_offset) {
        oam_warning_log0(0, OAM_SF_ANY,
            "{mac_get_hisi_mesh_optimization_ie:: the length of beacon/probe rsp frame body is invalid.}");
        return 0;
    }

    puc_bcn_prio_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_HISI_OPTIMIZATION, (puc_beacon_body + us_offset),
        (hi_u32)(l_frame_body_len - us_offset));
    /* 查找beacon prio的ie */
    if (puc_bcn_prio_ie != HI_NULL) {
        /* 获取beacon prio */
        bcn_prio = puc_bcn_prio_ie[MAC_MESH_HISI_BEACON_PRIO_POS];
    }

    return bcn_prio;
}

/* ****************************************************************************
 功能描述  : 获取beacon帧中的is mbr标识字段，上报new peer candidate时使用(私有字段中)
 输入参数  : puc_beacon_body:Beacon or probe rsp帧体
                            hi_s32 l_frame_body_len:帧体长度
 返 回 值  : hi_u8 en_is_mbr
 修改历史      :
  1.日    期   : 2019年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_get_hisi_en_is_mbr(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len)
{
    hi_u8 is_mbr = 0;
    hi_u8 *puc_hisi_optimization_ie = HI_NULL;
    hi_u16 us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    /* *********************************************************************** */
    /*                       Beacon Frame - Frame Body                       */
    /* --------------------------------------------------------------------- */
    /* |Timestamp |BeaconInt |CapInfo |SSID |SupRates |DSParSet |TIM elm   | */
    /* --------------------------------------------------------------------- */
    /* |8         |2         |2       |2-34 |3-10     |3        |4-256     | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* **************************************************************************
         Vendor Spec |Length| OUI|TYPE|SUBTYPE|Beacon Priority|
           1                |1        |3    |1     |1            |  1                  |
    ************************************************************************** */
    /* 检测beacon帧或者probe rsp帧的长度的合法性 */
    if (l_frame_body_len <= us_offset) {
        oam_warning_log0(0, OAM_SF_ANY,
            "{mac_get_hisi_en_is_mbr:: the length of beacon/probe rsp frame body is invalid.}");
        return 0;
    }

    puc_hisi_optimization_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_HISI_OPTIMIZATION,
        (puc_beacon_body + us_offset), (hi_u32)(l_frame_body_len - us_offset));
    /* 查找beacon prio的ie */
    if (puc_hisi_optimization_ie != HI_NULL) {
        /* 获取beacon prio */
        is_mbr = puc_hisi_optimization_ie[MAC_MESH_HISI_IS_MBR_POS];
    }

    return is_mbr;
}

/* ****************************************************************************
 功能描述  : 获取beacon帧中的Accept sta标识字段，上报new peer candidate时使用(私有字段中)
 输入参数  : puc_beacon_body:Beacon or probe rsp帧体
                            hi_s32 l_frame_body_len:帧体长度
 返 回 值  : hi_u8 accept_sta
 修改历史      :
  1.日    期   : 2019年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_get_hisi_accept_sta(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len)
{
    hi_u8 accept_sta = 0;
    hi_u8 *puc_hisi_optimization_ie = HI_NULL;
    hi_u16 us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    /* *********************************************************************** */
    /*                       Beacon Frame - Frame Body                       */
    /* --------------------------------------------------------------------- */
    /* |Timestamp |BeaconInt |CapInfo |SSID |SupRates |DSParSet |TIM elm   | */
    /* --------------------------------------------------------------------- */
    /* |8         |2         |2       |2-34 |3-10     |3        |4-256     | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* **************************************************************************
         Vendor Spec |Length| OUI|TYPE|SUBTYPE|Beacon Priority|
           1                |1        |3    |1     |1            |  1                  |
    ************************************************************************** */
    /* 检测beacon帧或者probe rsp帧的长度的合法性 */
    if (l_frame_body_len <= us_offset) {
        oam_warning_log0(0, OAM_SF_ANY,
            "{mac_get_hisi_accept_sta:: the length of beacon/probe rsp frame body is invalid.}");
        return 0;
    }

    puc_hisi_optimization_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_HISI_OPTIMIZATION,
        (puc_beacon_body + us_offset), (hi_u32)(l_frame_body_len - us_offset));
    /* 查找beacon prio的ie */
    if (puc_hisi_optimization_ie != HI_NULL) {
        /* 获取beacon prio */
        accept_sta = puc_hisi_optimization_ie[MAC_MESH_HISI_ACCEPT_STA_POS];
    }

    return accept_sta;
}

#endif

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
/* ****************************************************************************
 功能描述  : 设置sta qos info字段
 输入参数  : pst_mac_vap: 指向vap
             puc_buffer : 指向buffer
 修改历史      :
  1.日    期   : 2015年2月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_set_qos_info_wmm_sta(const mac_vap_stru *mac_vap, hi_u8 *puc_buffer)
{
    hi_u8 qos_info = 0;
    hi_u8 max_sp_bits;
    hi_u8 max_sp_length;
    /* QoS Information field                                          */
    /* -------------------------------------------------------------- */
    /* | B0    | B1    | B2    | B3    | B4      | B5:B6 | B7       | */
    /* -------------------------------------------------------------- */
    /* | AC_VO | AC_VI | AC_BK | AC_BE |         | Max SP|          | */
    /* | U-APSD| U-APSD| U-APSD| U-APSD| Reserved| Length| Reserved | */
    /* | Flag  | Flag  | Flag  | Flag  |         |       |          | */
    /* -------------------------------------------------------------- */
    /* Set the UAPSD configuration information in the QoS info field if the  */
    /* BSS type is Infrastructure and the AP supports UAPSD.                 */
    if (mac_vap->uapsd_cap) {
        max_sp_length = mac_vap->sta_uapsd_cfg.max_sp_len;
        qos_info = mac_vap->sta_uapsd_cfg.trigger_map;
        if (max_sp_length <= 6) { /* sp最大长度为6 byte */
            max_sp_bits = max_sp_length >> 1;
            qos_info |= ((max_sp_bits & 0x03) << 5); /* sp bit数从qos_info的第5bit开始存储 */
        }
    }
    puc_buffer[0] = qos_info;
}
#endif

/* ****************************************************************************
 功能描述  : 设置qos info字段
 输入参数  : pst_mac_vap: 指向vap
             puc_buffer : 指向buffer
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_set_qos_info_field(const mac_vap_stru *mac_vap, hi_u8 *puc_buffer)
{
    mac_qos_info_stru *qos_info = (mac_qos_info_stru *)puc_buffer;

    /* QoS Information field  (AP MODE)            */
    /* ------------------------------------------- */
    /* | B0:B3               | B4:B6    | B7     | */
    /* ------------------------------------------- */
    /* | Parameter Set Count | Reserved | U-APSD | */
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        qos_info->params_count = mac_vap->wmm_params_update_count;
        qos_info->uapsd        = mac_vap->cap_flag.uapsd;
        qos_info->bit_resv     = 0;
    }

    /* QoS Information field  (STA MODE)           */
    /* ---------------------------------------------------------------------------------------------------------- */
    /* | B0              | B1              | B2              | B3              | B4      |B5   B6      | B7     | */
    /* ---------------------------------------------------------------------------------------------------------- */
    /* |AC_VO U-APSD Flag|AC_VI U-APSD Flag|AC_BK U-APSD Flag|AC_BE U-APSD Flag|Reserved |Max SP Length|Reserved| */
    /* ---------------------------------------------------------------------------------------------------------- */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
        mac_set_qos_info_wmm_sta(mac_vap, puc_buffer);
#else
        puc_buffer[0] = 0;
        puc_buffer[0] |= 0x0;
#endif
    }
}

/* ****************************************************************************
 功能描述  : 设置一个ac的参数
 输入参数  : pst_mac_vap: 指向vap
             puc_buffer : 指向buffer
             en_ac      : AC类型
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_set_wmm_ac_params(const mac_vap_stru *mac_vap, hi_u8 *puc_buffer, hi_u8 ac)
{
    mac_wmm_ac_params_stru *ac_params = (mac_wmm_ac_params_stru *)puc_buffer;

    /* AC_** Parameter Record field               */
    /* ------------------------------------------ */
    /* | Byte 1    | Byte 2        | Byte 3:4   | */
    /* ------------------------------------------ */
    /* | ACI/AIFSN | ECWmin/ECWmax | TXOP Limit | */
    /* ------------------------------------------ */
    /* ACI/AIFSN Field                    */
    /* ---------------------------------- */
    /* | B0:B3 | B4  | B5:B6 | B7       | */
    /* ---------------------------------- */
    /* | AIFSN | ACM | ACI   | Reserved | */
    /* ---------------------------------- */
    /* AIFSN */
    ac_params->aifsn = mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_aifsn;
    /* ACM */
    ac_params->acm = mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_mandatory;
    /* ACI */
    ac_params->aci = ac;
    ac_params->bit_resv = 0;

    /* ECWmin/ECWmax Field */
    /* ------------------- */
    /* | B0:B3  | B4:B7  | */
    /* ------------------- */
    /* | ECWmin | ECWmax | */
    /* ------------------- */
    /* ECWmin */
    ac_params->ecwmin = mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_c_wmin;
    /* ECWmax */
    ac_params->ecwmax = mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_c_wmax;
    /* TXOP Limit. The value saved in MIB is in usec while the value to be   */
    /* set in this element should be in multiple of 32us                     */
    ac_params->us_txop =
        (hi_u16)((mac_vap->mib_info->ast_wlan_mib_edca[ac].dot11_edca_table_txop_limit) >> 5); /* 右移5bit获取txop */
}

/* ****************************************************************************
 功能描述  : 设置wmm信息元素
 输入参数  : pst_vap   : 指向vap
             puc_buffer: 指向buffer
             en_is_qos : 是否支持QOS。如果是BEACON/Probe Req/Probe Rsp/ASSOC Req帧，则取AP/STA自身的QOS能力；
             如果ASSOC RSP，则需要根据对方STA的QOS能力，来判断是否带WMM IE。
 输出参数  : puc_ie_len: ie的总长度
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_wmm_params_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 is_qos, hi_u8 *puc_ie_len)
{
    hi_u8 index;
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    hi_u8     auc_oui[MAC_OUI_LEN] = {(hi_u8)MAC_WLAN_OUI_MICRO0,
                                      (hi_u8)MAC_WLAN_OUI_MICRO1, (hi_u8)MAC_WLAN_OUI_MICRO2};

    if (!is_qos) {
        *puc_ie_len = 0;
        return;
    }
    /* WMM Parameter Element Format                                          */
    /* --------------------------------------------------------------------- */
    /* | 3Byte | 1        | 1           | 1             | 1        | 1     | */
    /* --------------------------------------------------------------------- */
    /* | OUI   | OUI Type | OUI Subtype | Version field | QoS Info | Resvd | */
    /* --------------------------------------------------------------------- */
    /* | 4              | 4              | 4              | 4              | */
    /* --------------------------------------------------------------------- */
    /* | AC_BE ParamRec | AC_BK ParamRec | AC_VI ParamRec | AC_VO ParamRec | */
    /* --------------------------------------------------------------------- */
    puc_buffer[0] = MAC_EID_WMM;
    puc_buffer[1] = MAC_WMM_PARAM_LEN;
    index = MAC_IE_HDR_LEN;
    /* OUI */
    if (memcpy_s(&puc_buffer[index], MAC_OUI_LEN, auc_oui, MAC_OUI_LEN) != EOK) {
        return;
    }
    index += MAC_OUI_LEN;
    /* OUI Type */
    puc_buffer[index++] = MAC_OUITYPE_WMM;
    /* OUI Subtype */
    puc_buffer[index++] = MAC_OUISUBTYPE_WMM_PARAM;
    /* Version field */
    puc_buffer[index++] = MAC_OUI_WMM_VERSION;
    /* QoS Information Field */
    mac_set_qos_info_field(mac_vap, &puc_buffer[index]);
    index += MAC_QOS_INFO_LEN;
    /* Reserved */
    puc_buffer[index++] = 0;
    /* Set the AC_BE, AC_BK, AC_VI, AC_VO Parameter Record fields */
    mac_set_wmm_ac_params(mac_vap, &puc_buffer[index], WLAN_WME_AC_BE);
    index += MAC_AC_PARAM_LEN;
    mac_set_wmm_ac_params(mac_vap, &puc_buffer[index], WLAN_WME_AC_BK);
    index += MAC_AC_PARAM_LEN;
    mac_set_wmm_ac_params(mac_vap, &puc_buffer[index], WLAN_WME_AC_VI);
    index += MAC_AC_PARAM_LEN;
    mac_set_wmm_ac_params(mac_vap, &puc_buffer[index], WLAN_WME_AC_VO);
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_WMM_PARAM_LEN;
}

/* ****************************************************************************
 功能描述  : 填充extended supported rates信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_exsup_rates_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru     *mac_vap   = (mac_vap_stru *)vap;
    mac_rateset_stru *rates_set = HI_NULL;
    hi_u8 nrates;
    hi_u8 idx;

    rates_set = &(mac_vap->curr_sup_rates.rate);
    /* **************************************************************************
                   -----------------------------------------------
                   |ElementID | Length | Extended Supported Rates|
                   -----------------------------------------------
       Octets:     |1         | 1      | 1-255                   |
                   -----------------------------------------------
    ************************************************************************** */
    if (rates_set->rs_nrates <= MAC_MAX_SUPRATES) {
        *puc_ie_len = 0;
        return;
    }
    puc_buffer[0] = MAC_EID_XRATES;
    nrates = rates_set->rs_nrates - MAC_MAX_SUPRATES;
    puc_buffer[1] = nrates;
    for (idx = 0; idx < nrates; idx++) {
        puc_buffer[MAC_IE_HDR_LEN + idx] = rates_set->ast_rs_rates[idx + MAC_MAX_SUPRATES].mac_rate;
    }
    *puc_ie_len = MAC_IE_HDR_LEN + nrates;
}

/* ****************************************************************************
 功能描述  : 填写ht capabilities info域
 输入参数  : pst_vap :指向vap
             puc_buffer :指向buffer
 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_set_ht_capinfo_field(hi_void *vap, hi_u8 *puc_buffer)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    mac_frame_ht_cap_stru *ht_capinfo = (mac_frame_ht_cap_stru *)puc_buffer;
    /* ********************** HT Capabilities Info field*************************
    ----------------------------------------------------------------------------
     |-------------------------------------------------------------------|
     | LDPC   | Supp    | SM    | Green- | Short  | Short  |  Tx  |  Rx  |
     | Coding | Channel | Power | field  | GI for | GI for | STBC | STBC |
     | Cap    | Wth Set | Save  |        | 20 MHz | 40 MHz |      |      |
     |-------------------------------------------------------------------|
     |   B0   |    B1   |B2   B3|   B4   |   B5   |    B6  |  B7  |B8  B9|
     |-------------------------------------------------------------------|
     |-------------------------------------------------------------------|
     |    HT     |  Max   | DSS/CCK | Reserved | 40 MHz     | L-SIG TXOP |
     |  Delayed  | AMSDU  | Mode in |          | Intolerant | Protection |
     | Block-Ack | Length | 40MHz   |          |            | Support    |
     |-------------------------------------------------------------------|
     |    B10    |   B11  |   B12   |   B13    |    B14     |    B15     |
     |-------------------------------------------------------------------|
    ************************************************************************** */
    /* 初始清0 */
    puc_buffer[0] = 0;
    puc_buffer[1] = 0;
    ht_capinfo->ldpc_coding_cap = mac_vap->mib_info->phy_ht.dot11_ldpc_coding_option_implemented;
    /* 设置所支持的信道宽度集"，0:仅20MHz运行; 1:20MHz与40MHz运行 */
    ht_capinfo->supported_channel_width = mac_mib_get_forty_mhz_operation_implemented(mac_vap);
    ht_capinfo->sm_power_save = MAC_SMPS_MIMO_MODE;
    ht_capinfo->ht_green_field = 0;
    ht_capinfo->short_gi_20mhz = mac_vap->mib_info->phy_ht.dot11_short_gi_option_in_twenty_implemented;
    ht_capinfo->short_gi_40mhz = mac_mib_get_shortgi_option_in_forty_implemented(mac_vap);
    ht_capinfo->tx_stbc = mac_vap->mib_info->phy_ht.dot11_tx_stbc_option_implemented;
    ht_capinfo->rx_stbc = (HI_TRUE == mac_vap->mib_info->phy_ht.dot11_rx_stbc_option_implemented) ? 1 : 0;
    ht_capinfo->ht_delayed_block_ack =
        mac_vap->mib_info->wlan_mib_sta_config.dot11_delayed_block_ack_option_implemented;
    ht_capinfo->max_amsdu_length = 0; /* 0表示最大amsdu长度为3839bytes */
    /* 1131H只支持2.4g 20M 配置为默认值 */
    ht_capinfo->dsss_cck_mode_40mhz = 0;
    ht_capinfo->forty_mhz_intolerant = mac_mib_get_forty_mhz_intolerant(mac_vap);
    ht_capinfo->lsig_txop_protection = HI_TRUE;
}

/* ****************************************************************************
 功能描述  : 填充ht a-mpdu parameters域信息
 输入参数  : pst_vap :指向vap
             puc_buffer :指向buffer
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_set_ampdu_params_field(hi_u8 *puc_buffer)
{
    mac_ampdu_params_stru *ampdu_params = (mac_ampdu_params_stru *)puc_buffer;

    /* ******************* AMPDU Parameters Field ******************************
     |-----------------------------------------------------------------------|
     | Maximum AMPDU Length Exponent | Minimum MPDU Start Spacing | Reserved |
     |-----------------------------------------------------------------------|
     | B0                         B1 | B2                      B4 | B5     B7|
     |-----------------------------------------------------------------------|
    ************************************************************************* */
    /* 初始清0 */
    puc_buffer[0] = 0;
    if (frw_get_offload_mode()) {
        ampdu_params->max_ampdu_len_exponent = 2; /* IPC 最大接收的ampdu长度 32k=(2^(13+2))-1 */
    } else {
        /* IOT 31H受限PACKET B大小,最大接收的ampdu长度 8k=(2^(13))-1 */
        ampdu_params->max_ampdu_len_exponent = 0;
    }
    ampdu_params->min_mpdu_start_spacing = 5; /* AMPDU中两个mpdu的最小间隔,取值5(= 4ms) */
}

/* ****************************************************************************
 功能描述  : 填充supported mcs set域信息
 输入参数  : pst_vap :指向vap
             puc_buffer :指向buffer
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_set_sup_mcs_set_field(hi_void *vap, hi_u8 *puc_buffer)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    mac_sup_mcs_set_stru *sup_mcs_set = (mac_sup_mcs_set_stru *)puc_buffer;

    /* ************************ Supported MCS Set Field **********************
    |-------------------------------------------------------------------|
    | Rx MCS Bitmask | Reserved | Rx Highest    | Reserved |  Tx MCS    |
    |                |          | Supp Data Rate|          |Set Defined |
    |-------------------------------------------------------------------|
    | B0         B76 | B77  B79 | B80       B89 | B90  B95 |    B96     |
    |-------------------------------------------------------------------|
    | Tx Rx MCS Set  | Tx Max Number     |   Tx Unequal     | Reserved  |
    |  Not Equal     | Spat Stream Supp  | Modulation Supp  |           |
    |-------------------------------------------------------------------|
    |      B97       | B98           B99 |       B100       | B101 B127 |
    |-------------------------------------------------------------------|
    ************************************************************************ */
    /* 初始清零 */
    if (memset_s(puc_buffer, sizeof(mac_sup_mcs_set_stru), 0, sizeof(mac_sup_mcs_set_stru)) != EOK) {
        return;
    }
    if (memcpy_s(sup_mcs_set->auc_rx_mcs, sizeof(sup_mcs_set->auc_rx_mcs),
        mac_vap->mib_info->supported_mcsrx.auc_dot11_supported_mcs_rx_value, WLAN_HT_MCS_BITMASK_LEN) != EOK) {
        return;
    }
    sup_mcs_set->rx_highest_rate = MAC_MAX_RATE_SINGLE_NSS_20M_11N;
    sup_mcs_set->tx_mcs_set_def = 1;
    /* reserve位清0 */
    sup_mcs_set->resv1 = 0;
    sup_mcs_set->resv2 = 0;
}

/* ****************************************************************************
 函 数 名  : mac_set_ht_extcap_field
 功能描述  : 填充ht extended capabilities field信息
 输入参数  : pst_vap :指向vap
             puc_buffer :指向buffer
 修改历史      :
  1.日    期   : 2013年4月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_set_ht_extcap_field(hi_u8 *puc_buffer)
{
    mac_ext_cap_stru *ext_cap = (mac_ext_cap_stru *)puc_buffer;

    /* **************** HT Extended Capabilities Field **********************
      |-----------------------------------------------------------------|
      | PCO | PCO Trans | Reserved | MCS  |  +HTC   |  RD    | Reserved |
      |     |   Time    |          | Fdbk | Support | Resp   |          |
      |-----------------------------------------------------------------|
      | B0  | B1     B2 | B3    B7 | B8 B9|   B10   |  B11   | B12  B15 |
      |-----------------------------------------------------------------|
    ********************************************************************** */
    /* 初始清0 */
    puc_buffer[0] = 0;
    puc_buffer[1] = 0;
    ext_cap->mcs_fdbk = (hi_u16)WLAN_MIB_MCS_FEEDBACK_OPT_IMPLT_NONE;
    ext_cap->htc_sup = HI_FALSE;
    ext_cap->rd_resp = HI_FALSE;
}


/* ****************************************************************************
 功能描述  : 填充ht capabilities信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_ht_capabilities_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;

    if (!mac_vap->mib_info->wlan_mib_sta_config.dot11_high_throughput_option_implemented) {
        *puc_ie_len = 0;
        return;
    }
    /* **************************************************************************
    -------------------------------------------------------------------------
    |EID |Length |HT Capa. Info |A-MPDU Parameters |Supported MCS Set|
    -------------------------------------------------------------------------
    |1   |1      |2             |1                 |16               |
    -------------------------------------------------------------------------
    |HT Extended Cap. |Transmit Beamforming Cap. |ASEL Cap.          |
    -------------------------------------------------------------------------
    |2                |4                         |1                  |
    -------------------------------------------------------------------------
    ************************************************************************** */
    *puc_buffer = MAC_EID_HT_CAP;
    *(puc_buffer + 1) = MAC_HT_CAP_LEN;
    puc_buffer += MAC_IE_HDR_LEN;
    /* 填充ht capabilities information域信息 */
    mac_set_ht_capinfo_field(vap, puc_buffer);
    puc_buffer += MAC_HT_CAPINFO_LEN;
    /* 填充A-MPDU parameters域信息 */
    mac_set_ampdu_params_field(puc_buffer);
    puc_buffer += MAC_HT_AMPDU_PARAMS_LEN;
    /* 填充supported MCS set域信息 */
    mac_set_sup_mcs_set_field(vap, puc_buffer);
    puc_buffer += MAC_HT_SUP_MCS_SET_LEN;
    /* 填充ht extended capabilities域信息 */
    mac_set_ht_extcap_field(puc_buffer);
    puc_buffer += MAC_HT_EXT_CAP_LEN;
    /* 填充 transmit beamforming capabilities域信息 */
    mac_set_txbf_cap_field(puc_buffer);
    puc_buffer += MAC_HT_TXBF_CAP_LEN;
    /* 填充asel(antenna selection) capabilities域信息 */
    mac_set_asel_cap_field(puc_buffer);
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_HT_CAP_LEN;
}

/* ****************************************************************************
 功能描述  : 填充ht operation信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_ht_opern_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru      *mac_vap = (mac_vap_stru *)vap;
    mac_ht_opern_stru *ht_opern = HI_NULL;
    hi_u8              obss_non_ht = 0;

    if (!mac_mib_get_high_throughput_option_implemented(mac_vap)) {
        *puc_ie_len = 0;
        return;
    }
    /* **************************************************************************
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
    *puc_buffer = MAC_EID_HT_OPERATION;
    *(puc_buffer + 1) = MAC_HT_OPERN_LEN;
    ht_opern = (mac_ht_opern_stru *)(puc_buffer + MAC_IE_HDR_LEN);
    /* 主信道编号 */
    ht_opern->primary_channel = mac_vap->channel.chan_number;
    ht_opern->secondary_chan_offset = MAC_SCN;
    /* 设置"STA信道宽度"，当BSS运行信道宽度 >= 40MHz时，需要将此field设置为1 */
    ht_opern->sta_chan_width = (mac_vap->channel.en_bandwidth > WLAN_BAND_WIDTH_20M) ? 1 : 0;
    /* 指示基本服务集里是否允许使用减小的帧间距 */
    ht_opern->rifs_mode = mac_mib_get_rifs_mode(mac_vap);
    /* B4-B7保留 */
    ht_opern->resv1 = 0;
    /* 指示ht传输的保护要求 */
    ht_opern->ht_protection = mac_mib_get_ht_protection(mac_vap);
    /* Non-GF STAs */
    ht_opern->nongf_sta_present = mac_mib_get_non_gfentities_present(mac_vap);
    /* B3 resv */
    ht_opern->resv2 = 0;
    /* B4  obss_nonht_sta_present */
    if ((mac_vap->protection.obss_non_ht_present != 0) || (mac_vap->protection.sta_non_ht_num != 0)) {
        obss_non_ht = 1;
    }
    ht_opern->obss_nonht_sta_present = obss_non_ht;
    /* B5-B15 保留 */
    ht_opern->resv3 = 0;
    ht_opern->resv4 = 0;
    /* B0-B5 保留 */
    ht_opern->resv5 = 0;
    /* B6  dual_beacon */
    ht_opern->dual_beacon = 0;
    /* Dual CTS protection */
    ht_opern->dual_cts_protection = 0;
    /* secondary_beacon: Set to 0 in a primary beacon */
    ht_opern->secondary_beacon = 0;
    /* BSS support L-SIG TXOP Protection */
    ht_opern->lsig_txop_protection_full_support = mac_mib_get_lsig_txop_full_protection_activated(mac_vap);
    /* PCO active */
    ht_opern->pco_active = 0;
    /* PCO phase */
    ht_opern->pco_phase = 0;
    /* B12-B15  保留 */
    ht_opern->resv6 = 0;
    /* Basic MCS Set: set all bit zero,Indicates the MCS values that are supported by all HT STAs in the BSS. */
    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(ht_opern->auc_basic_mcs_set, MAC_HT_BASIC_MCS_SET_LEN, 0, MAC_HT_BASIC_MCS_SET_LEN);
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_HT_OPERN_LEN;
}

/* ****************************************************************************
 功能描述  : 填充extended capabilities element信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_ext_capabilities_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru        *mac_vap = (mac_vap_stru *)vap;
    mac_ext_cap_ie_stru *ext_cap = HI_NULL;

    if (!mac_mib_get_high_throughput_option_implemented(mac_vap)) {
        *puc_ie_len = 0;
        return;
    }
    /* **************************************************************************
                         ----------------------------------
                         |Element ID |Length |Capabilities|
                         ----------------------------------
          Octets:        |1          |1      |n           |
                         ----------------------------------
    ------------------------------------------------------------------------------------------
    |  B0       | B1 | B2             | B3   | B4   |  B5  |  B6    |  B7   | ...|  B38    |   B39      |
    ----------------------------------------------------------------------------
    |20/40 coex |resv|extended channel| resv | PSMP | resv | S-PSMP | Event |    |TDLS Pro-  TDLS Channel
                                                                                             Switching
    |mgmt supp  |    |switching       |      |      |      |        |       | ...| hibited | Prohibited |
    --------------------------------------------------------------------------------------------
    ************************************************************************** */
    puc_buffer[0] = MAC_EID_EXT_CAPS;
    puc_buffer[1] = MAC_XCAPS_EX_LEN;
    /* 初始清零 */
    if (memset_s(puc_buffer + MAC_IE_HDR_LEN, sizeof(mac_ext_cap_ie_stru), 0, sizeof(mac_ext_cap_ie_stru)) != EOK) {
        return;
    }
    ext_cap = (mac_ext_cap_ie_stru *)(puc_buffer + MAC_IE_HDR_LEN);
    /* 设置20/40 BSS Coexistence Management Support fieid */
    if ((HI_TRUE == mac_mib_get_2040bss_coexistence_management_support(mac_vap)) &&
        (WLAN_BAND_2G == mac_vap->channel.band)) {
        ext_cap->coexistence_mgmt = 1;
    }
    /* 设置TDLS prohibited */
    ext_cap->tdls_prhibited = mac_vap->cap_flag.tdls_prohibited;
    /* 设置TDLS channel switch prohibited */
    ext_cap->tdls_channel_switch_prhibited = mac_vap->cap_flag.tdls_channel_switch_prohibited;
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    /* 如果是11ac 站点 设置OPMODE NOTIFY标志 */
    if (mac_mib_get_VHTOptionImplemented(mac_vap)) {
        ext_cap->operating_mode_notification = mac_mib_get_operating_mode_notification_implemented(mac_vap);
    }
#endif
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_XCAPS_EX_LEN;
}

/* ****************************************************************************
 功能描述  : 获取beacon帧中的ssid
 输入参数  : puc_beacon_body,               Beacon or probe rsp帧体
             hi_s32 l_frame_body_len,    帧体长度
 输出参数  : puc_ssid_len,                  ssid 长度
 返 回 值  : 指向ssid
 修改历史      :
  1.日    期   : 2013年6月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 *mac_get_ssid(hi_u8 *puc_beacon_body, hi_s32 l_frame_body_len, hi_u8 *puc_ssid_len)
{
    const hi_u8 *puc_ssid_ie = HI_NULL;
    const hi_u16 us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    /* *********************************************************************** */
    /*                       Beacon Frame - Frame Body                       */
    /* --------------------------------------------------------------------- */
    /* |Timestamp |BeaconInt |CapInfo |SSID |SupRates |DSParSet |TIM elm   | */
    /* --------------------------------------------------------------------- */
    /* |8         |2         |2       |2-34 |3-10     |3        |4-256     | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* **************************************************************************
                    ----------------------------
                    |Element ID | Length | SSID|
                    ----------------------------
           Octets:  |1          | 1      | 0~32|
                    ----------------------------
    ************************************************************************** */
    /* ssid的长度初始赋值为0 */
    *puc_ssid_len = 0;
    /* 检测beacon帧或者probe rsp帧的长度的合法性 */
    if (l_frame_body_len <= us_offset) {
        oam_warning_log0(0, OAM_SF_ANY, "{mac_get_ssid:: the length of beacon/probe rsp frame body is invalid.}");
        return HI_NULL;
    }
    /* 查找ssid的ie */
    puc_ssid_ie = mac_find_ie(MAC_EID_SSID, (puc_beacon_body + us_offset), (hi_u32)(l_frame_body_len - us_offset));
    if ((puc_ssid_ie != HI_NULL) && (puc_ssid_ie[1] < WLAN_SSID_MAX_LEN)) {
        /* 获取ssid ie的长度 */
        *puc_ssid_len = puc_ssid_ie[1];
        return (hi_u8 *)(puc_ssid_ie + MAC_IE_HDR_LEN);
    }
    return HI_NULL;
}

/* ****************************************************************************
 功能描述  : 填充用户态下发的信息元素到管理帧中
 输入参数  : [1]vap
             [2]puc_buffer
             [3]puc_ie_len
             [4]type
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_add_app_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u16 *pus_ie_len, en_app_ie_type_uint8 type)
{
    mac_vap_stru *mac_vap    = HI_NULL;
    hi_u8        *puc_app_ie = HI_NULL;
    hi_u32        app_ie_len;

    mac_vap = (mac_vap_stru *)vap;
    puc_app_ie = mac_vap->ast_app_ie[type].puc_ie;
    app_ie_len = mac_vap->ast_app_ie[type].ie_len;
    if (app_ie_len == 0) {
        *pus_ie_len = 0;
        return;
    } else {
        if (memcpy_s(puc_buffer, app_ie_len, puc_app_ie, app_ie_len) != EOK) {
            return;
        }
        *pus_ie_len = (hi_u16)app_ie_len;
    }
}

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* ****************************************************************************
 功能描述  : 设置mesh ssid ie
 输入参数  : pst_vap: 指向vap
             puc_buffer : 指向buffer
 输出参数  : puc_ie_len : element的长度
 修改历史      :
  1.日    期   : 2019年1月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_mesh_ssid_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len, hi_u8 is_mesh_req)
{
    hi_u8 ssid_len;
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;

    /* **************************************************************************
                    ----------------------------
                    |Element ID | Length | SSID|
                    ----------------------------
           Octets:  |1          | 1      | 0~32|
                    ----------------------------
    ************************************************************************** */
    /* Mesh中Probe req节点若为非Mesh 节点，回复非空SSID，若为Mesh节点，回复空SSID */
    if (is_mesh_req == HI_TRUE) {
        /* ssid ie */
        *puc_buffer = MAC_EID_SSID;
        /* ssid len */
        *(puc_buffer + 1) = 0;
        *puc_ie_len = MAC_IE_HDR_LEN;
        return;
    } else {
        hi_u8 *puc_ssid = HI_NULL;
        *puc_buffer = MAC_EID_SSID;
        puc_ssid = mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_desired_ssid;
        ssid_len = (hi_u8)strlen((hi_char *)puc_ssid); /* 不包含'\0' */
        *(puc_buffer + 1) = ssid_len;
        if (memcpy_s(puc_buffer + MAC_IE_HDR_LEN, ssid_len, puc_ssid, ssid_len) != EOK) {
            return;
        }
        *puc_ie_len = ssid_len + MAC_IE_HDR_LEN;
        return;
    }
}

#ifdef _PRE_WLAN_FEATURE_MESH_ROM

/* ****************************************************************************
 功能描述  : 填充mesh相关的Vendor Specific IE字段域头部信息
 输入参数  : [1]puc_buffer
 输出参数  : [1]puc_len
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_mesh_vendor_ie_hdr(hi_u8 *puc_buffer, hi_u8 *puc_len)
{
    mac_ieee80211_vendor_ie_stru *vendor_ie = HI_NULL;
    hi_u8  auc_oui[MAC_OUI_LEN] = { (hi_u8)MAC_WLAN_OUI_VENDOR0, (hi_u8)MAC_WLAN_OUI_VENDOR1,
                                    (hi_u8)MAC_WLAN_OUI_VENDOR2 };

    vendor_ie = (mac_ieee80211_vendor_ie_stru *)puc_buffer;
    vendor_ie->element_id = MAC_EID_VENDOR;
    vendor_ie->len = sizeof(mac_ieee80211_vendor_ie_stru) - MAC_IE_HDR_LEN;
    if (memcpy_s(vendor_ie->auc_oui, MAC_OUI_LEN, auc_oui, MAC_OUI_LEN) != EOK) {
        puc_len = 0;
        return;
    }
    vendor_ie->oui_type = MAC_OUITYPE_MESH;
    *puc_len = sizeof(mac_ieee80211_vendor_ie_stru);
}

#endif

/* ****************************************************************************
 功能描述  : 填充meshid字段(Hi1131Hmesh 方案meshid携带在私有字段)
 输入参数  : [1]vap
             [2]puc_buffer
 输出参数  : [3]puc_ie_len
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_meshid_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    hi_u8 mesh_vendor_ie_hdr_len = 0;
    hi_u8 meshid_len;

    meshid_len = (hi_u8)strlen((const hi_char *)mac_vap->mib_info->wlan_mib_mesh_sta_cfg.auc_dot11_mesh_id);

    /* Vendor Spec |Length| OUI|TYPE|SUBTYPE|MESHID LEN|MESHID|
           1                |1        |3    |1     |1           |  1               |Var       | */
    mac_set_mesh_vendor_ie_hdr(puc_buffer, &mesh_vendor_ie_hdr_len);
    *(puc_buffer + mesh_vendor_ie_hdr_len) = MAC_OUISUBTYPE_MESH_HISI_MESHID;
    *(puc_buffer + mesh_vendor_ie_hdr_len + MAC_OUISUBTYPE_LEN) = meshid_len;
    if (memcpy_s(puc_buffer + mesh_vendor_ie_hdr_len + MAC_OUISUBTYPE_LEN + 1, meshid_len,
        mac_vap->mib_info->wlan_mib_mesh_sta_cfg.auc_dot11_mesh_id, meshid_len) != EOK) {
        return;
    }

    /* 更新Element 长度 */
    *(puc_buffer + 1) += MAC_OUISUBTYPE_LEN + 1 + meshid_len;
    *(puc_ie_len) = mesh_vendor_ie_hdr_len + MAC_OUISUBTYPE_LEN + 1 + meshid_len;

    return;
}

/* ****************************************************************************
 功能描述  : 填充Mesh Formation Info域信息
 输入参数  : [1]vap
             [2]puc_buffer
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_mesh_formation_info_field(hi_void *vap, hi_u8 *puc_buffer)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    mesh_formation_info_stru *mesh_formation_info = (mesh_formation_info_stru *)puc_buffer;
    /* *********** mesh formation info field******************
    --------------------------------------------------
    |Connected to Mesh Gate |Number of Peerings|Connected to AS|
    --------------------------------------------------
    |BIT0                                |BIT1-BIT6             |BIT7                   |
    --------------------------------------------------
    ************************************************* */
    /* 初始清零 */
    if (memset_s(puc_buffer, sizeof(mesh_formation_info_stru), 0, sizeof(mesh_formation_info_stru)) != EOK) {
        return;
    }
    mesh_formation_info->connected_to_mesh_gate = 0;
    mesh_formation_info->number_of_peerings = mac_vap->user_nums;
    mesh_formation_info->connected_to_as = 0;
}

/* ****************************************************************************
 功能描述  : 填充Mesh Capability域信息
 输入参数  : [1]vap
             [2]puc_buffer
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_mesh_capability_field(hi_void *vap, hi_u8 *puc_buffer)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    mesh_capability_stru *mesh_capability = (mesh_capability_stru *)puc_buffer;
    /* *********** mesh capability field*********************************
    -------------------------------------------------------------
    |Accepting Additional Mesh Peering|MCCA Supported|MCCA Enabled|Forwarding|
    -------------------------------------------------------------
    |BIT0                                           |BIT1                   |BIT2              |BIT3          |
    -------------------------------------------------------------
    |MBCA Enabled|TBTT Adjusting|Mesh Power Save Level|Reserved|
    |BIT4               |BIT5               |BIT6                           |BIT7         |
    -------------------------------------------------------------
    ************************************************************* */
    /* 初始清零 */
    if (memset_s(mesh_capability, sizeof(mesh_capability_stru), 0, sizeof(mesh_capability_stru)) != EOK) {
        return;
    }
    mesh_capability->accepting_add_mesh_peerings =
        mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_accepting_additional_peerings;
    mesh_capability->mbca_enabled = mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mbca_activated;
    mesh_capability->tbtt_adjusting = mac_vap->mesh_tbtt_adjusting;
}

/* ****************************************************************************
 功能描述  : 填充mesh configuration字段
 输入参数  : [1]vap
             [2]puc_buffer
 输出参数  : [1]puc_ie_len
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_mesh_configuration_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;

    if (mac_vap->vap_mode != WLAN_VAP_MODE_MESH) {
        *puc_ie_len = 0;
        return;
    }

    if (mac_vap->mib_info->wlan_mib_sta_config.dot11_mesh_activated != HI_TRUE) {
        *puc_ie_len = 0;
        return;
    }

    /* ********************** Mesh Configuration Element*************************
    -------------------------------------------------------------------------
    |EID |Length |Active Path Selection Protocol Id |Active Path Selection Metric Id |Congestion Control Mode Id|
    -------------------------------------------------------------------------
    |1   |1      |1                                 |1                               |1                         |
    -------------------------------------------------------------------------
    |Sync Method ID |Auth Protocol ID |Mesh Formation Info|Mesh Capability|
    -------------------------------------------------------------------------
    |1              |1                |1                  |1              |
    -------------------------------------------------------------------------
    ************************************************************************** */
    *puc_buffer = MAC_EID_MESH_CONF;
    *(puc_buffer + 1) = MAC_MESH_CONF_LEN;

    puc_buffer += MAC_IE_HDR_LEN;

    /* 填充Active Path Selection Protocol Id域信息 */
    *puc_buffer = MAC_MIB_MESH_VENDOR_SPECIFIC; /* Vendor Specific */
    /* 填充Active Path Selection Metric Id域信息 */
    *(puc_buffer + 1) = MAC_MIB_MESH_VENDOR_SPECIFIC; /* Vendor Specific: byte 1 */
    /* 填充Congestion Control Mode Id域信息 */
    *(puc_buffer + 2) = 0; /* Not activated: byte 2 */
    /* 填充Sync Method ID域信息 */
    *(puc_buffer + 3) = MAC_MIB_MESH_VENDOR_SPECIFIC; /* Neighbor offset synchronization Method: byte 3 */
    /* 填充Auth Protocol ID域信息 */
    *(puc_buffer + 4) = /* SAE(1): byte 4 */
        mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_active_authentication_protocol;
    puc_buffer += 5; /* Active Path Selection Protocol Id域长度为5 byte */

    /* 填充Mesh Formation Info域信息 */
    mac_set_mesh_formation_info_field(vap, puc_buffer);
    puc_buffer += MAC_MESH_FORMATION_LEN;
    /* 填充Mesh Capability Field 域信息 */
    mac_set_mesh_capability_field(vap, puc_buffer);
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_MESH_CONF_LEN;

    return;
}

/* ****************************************************************************
 功能描述  : 填充HISI Mesh Optimization域信息
 输入参数  : [1]pst_vap
             [2]puc_buffer
 输出参数  : [1]puc_ie_len
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_hisi_mesh_optimization_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    hi_u8 mesh_vendor_ie_hdr_len = 0;
    if (mac_vap->vap_mode != WLAN_VAP_MODE_MESH) {
        *puc_ie_len = 0;
        return;
    }
    /* Mesh Optimization: Node Priority, En is MBR, Mesh Accept Sta */
    mac_set_mesh_vendor_ie_hdr(puc_buffer, &mesh_vendor_ie_hdr_len);
    *(puc_buffer + mesh_vendor_ie_hdr_len) = MAC_OUISUBTYPE_MESH_HISI_OPTIMIZATION;
    *(puc_buffer + mesh_vendor_ie_hdr_len + MAC_OUISUBTYPE_LEN) = mac_vap->priority;
    *(puc_buffer + mesh_vendor_ie_hdr_len + MAC_OUISUBTYPE_LEN + 1) = mac_vap->is_mbr;
    *(puc_buffer + mesh_vendor_ie_hdr_len + MAC_OUISUBTYPE_LEN + 2) = /* accept_sta标志,在IE头部信息后的byte(1+2)位填写 */
        mac_vap->mesh_accept_sta;

    /* 更新Element 长度 */
    *(puc_buffer + 1) += MAC_MESH_HISI_OPTIMIZATION_LEN + MAC_OUISUBTYPE_LEN;
    *(puc_ie_len) = mesh_vendor_ie_hdr_len + MAC_OUISUBTYPE_LEN + MAC_MESH_HISI_OPTIMIZATION_LEN;
    return;
}

/* ****************************************************************************
 功能描述  : 填充HISI Mesh Vendor字段(Subtype)
 输入参数  : [1]puc_buffer
             [2]subtype
 输出参数  : [1]puc_ie_len
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_mesh_vendor_subtype(hi_u8 *puc_buffer, hi_u8 subtype, hi_u8 *puc_ie_len)
{
    hi_u8 mesh_vendor_ie_hdr_len = 0;

    mac_set_mesh_vendor_ie_hdr(puc_buffer, &mesh_vendor_ie_hdr_len);
    *(puc_buffer + mesh_vendor_ie_hdr_len) = subtype;
    mesh_vendor_ie_hdr_len += MAC_OUISUBTYPE_LEN;
    *(puc_ie_len) = mesh_vendor_ie_hdr_len;
    /* 更新IE 长度 */
    *(puc_buffer + 1) += MAC_OUISUBTYPE_LEN;
}
#endif

/* ****************************************************************************
 功能描述  : 填充DS参数集
 输入参数  : [1]vap
             [2]us_frm_type
             [3]puc_buffer
 输出参数  : [1]puc_ie_len
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_dsss_params(hi_void *vap, hi_u16 us_frm_type, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    hi_u8 chan_num;
    mac_device_stru *mac_dev = HI_NULL;
#ifndef _PRE_WLAN_FEATURE_MESH_ROM
    hi_unref_param(us_frm_type);
#endif
    /* **************************************************************************
                        ----------------------------------------
                        | Element ID  | Length |Current Channel|
                        ----------------------------------------
              Octets:   | 1           | 1      | 1             |
                        ----------------------------------------
    The DSSS Parameter Set element contains information to allow channel number identification for STAs.
    ************************************************************************** */
    mac_dev = mac_res_get_dev();
    chan_num = mac_vap->channel.chan_number;
    if ((is_sta(mac_vap)
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
        || ((mac_vap->vap_mode == WLAN_VAP_MODE_MESH) && (us_frm_type == WLAN_FC0_SUBTYPE_PROBE_REQ))
#endif
        ) && (mac_dev->curr_scan_state == MAC_SCAN_STATE_RUNNING)) {
        chan_num = mac_dev->scan_params.ast_channel_list[mac_dev->scan_chan_idx].chan_number;
    }
    puc_buffer[0] = MAC_EID_DSPARMS;
    puc_buffer[1] = MAC_DSPARMS_LEN;
    puc_buffer[2] = chan_num; /* DS参数集 byte 2 指示为信道个数 */
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_DSPARMS_LEN;
}

/* ****************************************************************************
 功能描述  : 填充Country信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_country_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru                *mac_vap = (mac_vap_stru *)vap;
    mac_regdomain_info_stru     *rd_info = HI_NULL;
    hi_u8                    band;
    hi_u8                    index;
    hi_u8                    len = 0;

    if (mac_vap->mib_info->wlan_mib_sta_config.dot11_multi_domain_capability_activated != HI_TRUE &&
        mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_required != HI_TRUE &&
        mac_vap->mib_info->wlan_mib_sta_config.dot11_radio_measurement_activated != HI_TRUE) {
        /* 没有使能管制域ie */
        *puc_ie_len = 0;
        return;
    }
    /* **************************************************************************
                               |....These three fields are repeated...|
    -------------------------------------------------------------------------------
    |EID | Len | CountryString | First Channel |Number of |Maximum    | Pad       |
    |    |     |               |   Number/     |Channels/ |Transmit   |(if needed)|
    |    |     |               |   Operating   | Operating|Power Level|           |
    |    |     |               |   Extension   | Class    |/Coverage  |           |
    |    |     |               |   Identifier  |          |Class      |           |
    -------------------------------------------------------------------------------
    |1   |1    |3              |1              |1         |1          |0 or 1     |
    -------------------------------------------------------------------------------
    ************************************************************************** */
    /* 读取管制域信息 */
    rd_info = mac_get_regdomain_info();
    /* 获取当前工作频段 */
    band = mac_vap->channel.band;
    /* 填写EID, 长度最后填 */
    puc_buffer[0] = MAC_EID_COUNTRY;
    /* 初始化填写buffer的位置 */
    index = MAC_IE_HDR_LEN;
    /* 国家码 */
    puc_buffer[index++] = (hi_u8)(rd_info->ac_country[0]);
    puc_buffer[index++] = (hi_u8)(rd_info->ac_country[1]);
    puc_buffer[index++] = ' '; /* 0表示室内室外规定相同 */
    if (WLAN_BAND_2G == band) {
        mac_set_country_ie_2g(rd_info, &(puc_buffer[index]), &len);
    }

    if (len == 0) {
        /* 无管制域内容 */
        *puc_ie_len = 0;
        return;
    }
    index += len;
    /* 如果总长度为奇数，则补1字节pad */
    if (index & BIT0) {
        puc_buffer[index] = 0;
        index += 1;
    }
    /* 设置信息元素长度 */
    puc_buffer[1] = index - MAC_IE_HDR_LEN;
    *puc_ie_len = index;
}

/* ****************************************************************************
 功能描述  : 填充power constraint信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_pwrconstraint_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;

    /* **************************************************************************
                   -------------------------------------------
                   |ElementID | Length | LocalPowerConstraint|
                   -------------------------------------------
       Octets:     |1         | 1      | 1                   |
                   -------------------------------------------

    向工作站描述其所允许的最大传输功率，此信息元素记录规定最大值
    减去实际使用时的最大值
    ************************************************************************** */
    if (mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_required == HI_FALSE) {
        *puc_ie_len = 0;
        return;
    }
    *puc_buffer = MAC_EID_PWRCNSTR;
    *(puc_buffer + 1) = MAC_PWR_CONSTRAINT_LEN;
    /* Note that this field is always set to 0 currently. Ideally            */
    /* this field can be updated by having an algorithm to decide transmit   */
    /* power to be used in the BSS by the AP.                                */
    *(puc_buffer + MAC_IE_HDR_LEN) = 0;
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_PWR_CONSTRAINT_LEN;
}

/* ****************************************************************************
 功能描述  : 填充quiet信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
             uc_qcount  : Quiet Count
             uc_qperiod : Quiet Period
             us_qdur    : Quiet Duration
             us_qoffset : Quiet Offset
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_quiet_ie(hi_void *vap, hi_u8 *puc_buffer,
    const mac_set_quiet_ie_info_stru *mac_set_quiet_ie_info, hi_u8 *puc_ie_len)
{
    mac_quiet_ie_stru *quiet = HI_NULL;
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    if ((mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_required != HI_TRUE) &&
        (mac_vap->mib_info->wlan_mib_sta_config.dot11_radio_measurement_activated != HI_TRUE)) {
        *puc_ie_len = 0;
        return;
    }
    /* **************************************************************************
    -----------------------------------------------------------------------------
    |ElementID | Length | QuietCount | QuietPeriod | QuietDuration | QuietOffset|
    -----------------------------------------------------------------------------
    |1         | 1      | 1          | 1           | 2             | 2          |
    -----------------------------------------------------------------------------
    ************************************************************************** */
    if (mac_set_quiet_ie_info->us_qduration == 0 || mac_set_quiet_ie_info->qcount == 0) {
        *puc_ie_len = 0;
        return;
    }
    *puc_buffer = MAC_EID_QUIET;
    *(puc_buffer + 1) = MAC_QUIET_IE_LEN;
    quiet = (mac_quiet_ie_stru *)(puc_buffer + MAC_IE_HDR_LEN);
    quiet->quiet_count    = mac_set_quiet_ie_info->qcount;
    quiet->quiet_period   = mac_set_quiet_ie_info->qperiod;
    quiet->quiet_duration = oal_byteorder_to_le16(mac_set_quiet_ie_info->us_qduration);
    quiet->quiet_offset   = oal_byteorder_to_le16(mac_set_quiet_ie_info->us_qoffset);
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_QUIET_IE_LEN;
}

/* ****************************************************************************
 功能描述  : 填充tpc report信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_tpc_report_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    /* **************************************************************************
                -------------------------------------------------
                |ElementID  |Length  |TransmitPower  |LinkMargin|
                -------------------------------------------------
       Octets:  |1          |1       |1              |1         |
                -------------------------------------------------

    TransimitPower, 此帧的传送功率，以dBm为单位
    ************************************************************************** */
    if (mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_required == HI_FALSE &&
        mac_vap->mib_info->wlan_mib_sta_config.dot11_radio_measurement_activated == HI_FALSE) {
        *puc_ie_len = 0;
        return;
    }
    *puc_buffer = MAC_EID_TPCREP;
    *(puc_buffer + 1) = MAC_TPCREP_IE_LEN;
    *(puc_buffer + 2) = mac_vap->tx_power; /* tpc report byte 2 存储tx_power */
    *(puc_buffer + 3) = 0;                 /* tpc report byte 3 字段管理帧中不用 */
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_TPCREP_IE_LEN;
}

/* ****************************************************************************
 功能描述  : 填充Quiet信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_erp_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru         *mac_vap    = (mac_vap_stru *)vap;
    mac_erp_params_stru  *erp_params = HI_NULL;
    /***************************************************************************
    --------------------------------------------------------------------------
    |EID  |Len  |NonERP_Present|Use_Protection|Barker_Preamble_Mode|Reserved|
    --------------------------------------------------------------------------
    |B0-B7|B0-B7|B0            |B1            |B2                  |B3-B7   |
    --------------------------------------------------------------------------
    ************************************************************************** */
    if (WLAN_LEGACY_11B_MODE == mac_vap->protocol) {
        *puc_ie_len = 0;
        return; /* 5G频段和11b协议模式 没有erp信息 */
    }
    *puc_buffer       = MAC_EID_ERP;
    *(puc_buffer + 1) = MAC_ERP_IE_LEN;
    *(puc_buffer + 2) = 0;  /* Quiet信息byte 2 初始清0 */
    erp_params = (mac_erp_params_stru *)(puc_buffer + MAC_IE_HDR_LEN);
    /* 如果存在non erp站点与ap关联， 或者obss中存在non erp站点 */
    if ((mac_vap->protection.sta_non_erp_num != 0) || (mac_vap->protection.obss_non_erp_present)) {
        erp_params->non_erp = 1;
    }
    /* 如果ap已经启用erp保护 */
    if (mac_vap->protection.protection_mode == WLAN_PROT_ERP) {
        erp_params->use_protection = 1;
    }
    /* 如果存在不支持short preamble的站点与ap关联， 或者ap自身不支持short preamble */
    if ((mac_vap->protection.sta_no_short_preamble_num != 0) ||
        (mac_mib_get_short_preamble_option_implemented(mac_vap) == HI_FALSE)) {
        erp_params->preamble_mode = 1;
    }
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_ERP_IE_LEN;
}

/* ****************************************************************************
 功能描述  : 填充bss load信息
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2013年4月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_set_bssload_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_bss_load_stru  *bss_load = HI_NULL;
    mac_vap_stru       *mac_vap  = (mac_vap_stru *)vap;
    if (mac_vap->mib_info->wlan_mib_sta_config.dot11_qos_option_implemented == HI_FALSE ||
        mac_vap->mib_info->wlan_mib_sta_config.dot11_qbss_load_implemented == HI_FALSE) {
        *puc_ie_len = 0;
        return;
    }
    /* **************************************************************************
    ------------------------------------------------------------------------
    |EID |Len |StationCount |ChannelUtilization |AvailableAdmissionCapacity|
    ------------------------------------------------------------------------
    |1   |1   |2            |1                  |2                         |
    ------------------------------------------------------------------------
    ************************************************************************** */
    puc_buffer[0] = MAC_EID_QBSS_LOAD;
    puc_buffer[1] = MAC_BSS_LOAD_IE_LEN;
    bss_load = (mac_bss_load_stru *)(puc_buffer + MAC_IE_HDR_LEN);
    bss_load->us_sta_count = oal_byteorder_to_le16(mac_vap->user_nums);
    bss_load->chan_utilization = 0;
    bss_load->us_aac = 0;
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_BSS_LOAD_IE_LEN;
}

/* ****************************************************************************
 功能描述  : 填充用户态下发的信息元素到管理帧中
 输入参数  : [1]vap
             [2]puc_buffer
             [3]puc_ie_len
             [4]type
 返 回 值  : 无
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_add_wps_ie(hi_void *vap, hi_u8 *puc_buffer, hi_u16 *pus_ie_len, en_app_ie_type_uint8 type)
{
    mac_vap_stru    *mac_vap = HI_NULL;
    hi_u8           *puc_app_ie  = HI_NULL;
    hi_u8           *puc_wps_ie  = HI_NULL;
    hi_u32          app_ie_len;

    mac_vap    = (mac_vap_stru *)vap;
    puc_app_ie = mac_vap->ast_app_ie[type].puc_ie;
    app_ie_len = mac_vap->ast_app_ie[type].ie_len;

    if (app_ie_len == 0) {
        *pus_ie_len = 0;
        return;
    }
    puc_wps_ie =
        mac_find_vendor_ie(MAC_WLAN_OUI_MICROSOFT, MAC_WLAN_OUI_TYPE_MICROSOFT_WPS, puc_app_ie, (hi_s32)app_ie_len);
    if ((puc_wps_ie == HI_NULL) || (puc_wps_ie[1] < MAC_MIN_WPS_IE_LEN)) {
        *pus_ie_len = 0;
        return;
    }
    /* 将WPS ie 信息拷贝到buffer 中 */
    if (memcpy_s(puc_buffer, puc_wps_ie[1] + MAC_IE_HDR_LEN, puc_wps_ie, puc_wps_ie[1] + MAC_IE_HDR_LEN) != EOK) {
        oam_warning_log0(0, 0, "{mac_add_wps_ie::memcpy_s fail!}");
        *pus_ie_len = 0;
        return;
    }

    *pus_ie_len = puc_wps_ie[1] + MAC_IE_HDR_LEN;
}

/* ****************************************************************************
 功能描述  : 封装空桢
 输入参数  : header－80211头部指针
             us_fc frame control类型
             puc_da: 目的mac地址
             puc_sa: 源mac地址
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_null_data_encap(hi_u8 *header, hi_u16 us_fc, const hi_u8 *da_mac_addr,
    const hi_u8 *sa_mac_addr)
{
    mac_hdr_set_frame_control(header, us_fc);
    /* 设置ADDR1为DA|BSSID */
    if (memcpy_s((header + WLAN_HDR_ADDR1_OFFSET), WLAN_MAC_ADDR_LEN, da_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_warning_log0(0, 0, "{mac_null_data_encap::memcpy_s fail!}");
        return;
    }
    /* 设置ADDR2为BSSID|SA */
    if (memcpy_s((header + WLAN_HDR_ADDR2_OFFSET), WLAN_MAC_ADDR_LEN, sa_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_warning_log0(0, 0, "{mac_null_data_encap::memcpy_s fail!}");
        return;
    }
    if ((us_fc & WLAN_FRAME_FROME_AP) && !(us_fc & WLAN_FRAME_TO_AP)) {
        /* 设置ADDR3为SA */
        if (memcpy_s((header + WLAN_HDR_ADDR3_OFFSET), WLAN_MAC_ADDR_LEN, sa_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_warning_log0(0, 0, "{mac_null_data_encap::memcpy_s fail!}");
            return;
        }
    } else if (!(us_fc & WLAN_FRAME_FROME_AP) && (us_fc & WLAN_FRAME_TO_AP)) {
        /* 设置ADDR3为DA */
        if (memcpy_s((header + WLAN_HDR_ADDR3_OFFSET), WLAN_MAC_ADDR_LEN, da_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_warning_log0(0, 0, "{mac_null_data_encap::memcpy_s fail!}");
            return;
        }
    }
    /* NULL帧不存在其他DS位 不处理 */
}

/* ****************************************************************************
 功能描述  : 封装action帧头
 输入参数  : header－80211头部指针
             puc_da: 目的mac地址
             puc_sa: 源mac地址
 修改历史      :
  1.日    期   : 2019年03月08日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_prepare_action_frame_head(hi_u8 *puc_header, const hi_u8 *da_mac_addr,
    const hi_u8 *sa_mac_addr)
{
    /* 帧控制字段全为0，除了type和subtype */
    mac_hdr_set_frame_control(puc_header, WLAN_PROTOCOL_VERSION | WLAN_FC0_TYPE_MGT | WLAN_FC0_SUBTYPE_ACTION);
    /* 设置分片序号为0 */
    mac_hdr_set_fragment_number(puc_header, 0);
    /* 设置地址1，一般是广播地址 */
    if (memcpy_s(puc_header + WLAN_HDR_ADDR1_OFFSET, WLAN_MAC_ADDR_LEN, da_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{mac_prepare_action_frame_head::mem safe func err!}");
        return HI_FAIL;
    }
    /* 设置地址2为自己的MAC地址 */
    if (memcpy_s(puc_header + WLAN_HDR_ADDR2_OFFSET, WLAN_MAC_ADDR_LEN, sa_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{mac_prepare_action_frame_head::mem safe func err!}");
        return HI_FAIL;
    }
    /* 地址3，为VAP自己的MAC地址 */
    if (memcpy_s(puc_header + WLAN_HDR_ADDR3_OFFSET, WLAN_MAC_ADDR_LEN, sa_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{mac_prepare_action_frame_head::mem safe func err!}");
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 封装action帧体
 修改历史      :
  1.日    期   : 2019年03月08日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_prepare_action_frame_body(hi_u8 *puc_body, hi_u8 body_len, hi_u8 category,
    const hi_u8 *puc_elements, hi_u8 element_len)
{
    if (body_len < (WLAN_ACTION_BODY_ELEMENT_OFFSET + element_len)) {
        return HI_FALSE;
    }

    /* category :127,表示是厂商自定义帧 */
    puc_body[WLAN_ACTION_BODY_CATEGORY_OFFSET] = category;
    /* 初始化vendor OUI */
    puc_body[MAC_ACTION_OUI_POS]     = MAC_WLAN_OUI_VENDOR0;
    puc_body[MAC_ACTION_OUI_POS + 1] = MAC_WLAN_OUI_VENDOR1; /* vendor OUI byte(1+1)设置 */
    puc_body[MAC_ACTION_OUI_POS + 2] = MAC_WLAN_OUI_VENDOR2; /* vendor OUI byte(1+2)设置 */
    /* 设置action类型和子类型 */
    puc_body[MAC_ACTION_VENDOR_TYPE_POS] = MAC_OUITYPE_DBAC;
    puc_body[MAC_ACTION_VENDOR_SUBTYPE_POS] = MAC_OUISUBTYPE_DBAC_NOA;
    /* 设置IE字段 */
    if (0 != memcpy_s(puc_body + WLAN_ACTION_BODY_ELEMENT_OFFSET, body_len, puc_elements, element_len)) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

/* 代码ROM段结束位置 新增ROM代码请放在SECTION中 */
#undef __WIFI_ROM_SECTION__

/* ****************************************************************************
 功能描述  : STA根据关联用户的能力信息，设置关联请求帧中的cap info
 输入参数  : pst_vap      : 指向vap
             puc_cap_info : 指向存储能力位信息的buffer
 修改历史      :
  1.日    期   : 2015年9月7日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_set_cap_info_sta(hi_void *vap, hi_u8 *puc_cap_info)
{
    mac_cap_info_stru   *cap_info = (mac_cap_info_stru *)puc_cap_info;
    mac_vap_stru        *mac_vap  = (mac_vap_stru *)vap;

    /* *************************************************************************
         -------------------------------------------------------------------
         |B0 |B1  |B2        |B3    |B4     |B5      |B6  |B7     |B8      |
         -------------------------------------------------------------------
         |ESS|IBSS|CFPollable|CFPReq|Privacy|Preamble|PBCC|Agility|SpecMgmt|
         -------------------------------------------------------------------
         |B9 |B10      |B11 |B12     |B13      |B14        |B15            |
         -------------------------------------------------------------------
         |QoS|ShortSlot|APSD|RM      |DSSS-OFDM|Delayed BA |Immediate BA   |
         -------------------------------------------------------------------
    ************************************************************************** */
    /* 学习对端的能力信息 */
    if (memcpy_s(puc_cap_info, sizeof(mac_cap_info_stru), (hi_u8 *)(&mac_vap->us_assoc_user_cap_info),
        sizeof(mac_cap_info_stru)) != EOK) {
        return;
    }
    /* 以下能力位不学习，保持默认值 */
    cap_info->ibss              = 0;
    cap_info->cf_pollable       = 0;
    cap_info->cf_poll_request   = 0;
    cap_info->radio_measurement =
        mac_vap->mib_info->wlan_mib_sta_config.dot11_radio_measurement_activated;
}

#ifdef _PRE_WLAN_FEATURE_PMF
/* ****************************************************************************
 功能描述  : 设置Timeout_Interval信息元素
 输入参数  : pst_mac_vap   : 指向vap
             puc_buffer: 指向buffer
             ul_type: Timeout_Interval的类型
             puc_sta_addr: ap发送带Timeout_Interval的assoc rsp帧中的DA
             puc_ie_len: ie的总长度
             pst_sa_query_info :组ASSOCIATION_COMEBACK_TIME时需要吧
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_set_timeout_interval_ie(hi_u8 *puc_buffer, hi_u8 *puc_ie_len, hi_u32 type, hi_u32 timeout)
{
    mac_timeout_interval_type_enum tie_type;

    tie_type = (mac_timeout_interval_type_enum)type;
    *puc_ie_len = 0;
    /* 判断是否需要设置timeout_interval IE */
    if (tie_type >= MAC_TIE_BUTT) {
        return;
    }
    /* Timeout Interval Parameter Element Format
    -----------------------------------------------------------------------
    |ElementID | Length | Timeout Interval Type| Timeout Interval Value  |
    -----------------------------------------------------------------------
    |1         | 1      | 1                    |  4                      |
    ----------------------------------------------------------------------- */
    puc_buffer[0] = MAC_EID_TIMEOUT_INTERVAL;
    puc_buffer[1] = MAC_TIMEOUT_INTERVAL_INFO_LEN;
    puc_buffer[2] = tie_type; /* Timeout_Interval byte2 为tie_type */
    /* 设置Timeout Interval Value */
    puc_buffer[3] = timeout & 0x000000FF;       /* Timeout_Interval byte3 为timeout最低8byte */
    puc_buffer[4] = (timeout & 0x0000FF00)>>8;  /* Timeout_Interval byte4 为timeout的bit8 ~ 15 */
    puc_buffer[5] = (timeout & 0x00FF0000)>>16; /* Timeout_Interval byte5 为timeout的bit16 ~ 23 */
    puc_buffer[6] = (timeout & 0xFF000000)>>24; /* Timeout_Interval byte6 为timeout的bit24 ~ 31 */
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_TIMEOUT_INTERVAL_INFO_LEN;
}
#endif /* #ifdef HI_ON_FLASH */

/* ****************************************************************************
 功能描述  : 从RSN ie中获取pmf能力信息
 修改历史      :
  1.日    期   : 2015年2月7日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
wlan_pmf_cap_status_uint8 mac_get_pmf_cap(hi_u8 *puc_ie, hi_u32 ie_len)
{
    hi_u8 *puc_rsn_ie = HI_NULL;
    hi_u16 us_rsn_cap;

    if (oal_unlikely(puc_ie == HI_NULL)) {
        return MAC_PMF_DISABLED;
    }

    /* 查找RSN信息元素,如果没有RSN信息元素,则按照不支持处理 */
    puc_rsn_ie = mac_find_ie(MAC_EID_RSN, puc_ie, ie_len);
    /* 根据RSN信息元素, 判断RSN能力是否匹配 */
    us_rsn_cap = mac_get_rsn_capability(puc_rsn_ie);
    if ((us_rsn_cap & BIT6) && (us_rsn_cap & BIT7)) {
        return MAC_PME_REQUIRED;
    }
    if (us_rsn_cap & BIT7) {
        return MAC_PMF_ENABLED;
    }
    return MAC_PMF_DISABLED;
}

/* ****************************************************************************
 功能描述  : 从beacon帧中获得beacon period
 修改历史      :
  1.日    期   : 2013年6月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u16 mac_get_beacon_period(const hi_u8 *puc_beacon_body)
{
    /* *********************************************************************** */
    /*                       Beacon Frame - Frame Body                       */
    /* --------------------------------------------------------------------- */
    /* |Timestamp |BeaconInt |CapInfo |SSID |SupRates |DSParSet |TIM elm   | */
    /* --------------------------------------------------------------------- */
    /* |8         |2         |2       |2-34 |3-10     |3        |4-256     | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    return *((hi_u16 *)(puc_beacon_body + MAC_TIME_STAMP_LEN));
}

/* ****************************************************************************
 功能描述  : 获取dtim period值
 修改历史      :
  1.日    期   : 2013年10月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_get_dtim_period(hi_u8 *puc_frame_body, hi_u16 us_frame_body_len)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u16 us_offset;

    us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    if (us_frame_body_len > us_offset) {
        puc_ie = mac_find_ie(MAC_EID_TIM, puc_frame_body + us_offset, us_frame_body_len - us_offset);
        if ((puc_ie != HI_NULL) && (puc_ie[1] >= MAC_MIN_TIM_LEN)) {
            return puc_ie[3]; /* byte 3 为dtim period值 */
        }
    }
    return 0;
}

/* ****************************************************************************
 功能描述  : 获取dtim cnt值
 修改历史      :
  1.日    期   : 2013年10月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_get_dtim_cnt(hi_u8 *puc_frame_body, hi_u16 us_frame_body_len)
{
    hi_u8 *puc_ie = HI_NULL;
    const hi_u16 us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    if (us_frame_body_len > us_offset) {
        puc_ie = mac_find_ie(MAC_EID_TIM, puc_frame_body + us_offset, us_frame_body_len - us_offset);
        if ((puc_ie != HI_NULL) && (puc_ie[1] >= MAC_MIN_TIM_LEN)) {
            return puc_ie[2]; /* byte 2 为dtim cnt值 */
        }
    }
    return 0;
}

/* ****************************************************************************
 功能描述  : 从管理帧中获取wmm ie
 修改历史      :
  1.日    期   : 2013年6月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 *mac_get_wmm_ie(hi_u8 *puc_beacon_body, hi_u16 us_frame_len, hi_u16 us_offset)
{
    hi_u16 us_index = us_offset;

    /* 寻找TIM信息元素 */
    while (us_index < us_frame_len) {
        if (mac_is_wmm_ie(puc_beacon_body + us_index) == HI_TRUE) {
            return (&puc_beacon_body[us_index]);
        } else {
            us_index += (MAC_IE_HDR_LEN + puc_beacon_body[us_index + 1]);
        }
    }
    return HI_NULL;
}

/* ****************************************************************************
 功能描述  : 根据rsn_ie获取rsn能力
 输入参数  : [1]puc_rsn_ie
 返 回 值  : 无
***************************************************************************** */
hi_u16 mac_get_rsn_capability(const hi_u8 *puc_rsn_ie)
{
    hi_u16 us_pairwise_count;
    hi_u16 us_akm_count;
    hi_u16 us_rsn_capability;
    hi_u16 us_index = 0;

    if (puc_rsn_ie == HI_NULL) {
        return 0;
    }
    /* *********************************************************************** */
    /*                  RSN Element Format                                   */
    /* --------------------------------------------------------------------- */
    /* |Element ID | Length | Version | Group Cipher Suite | Pairwise Cipher */
    /* --------------------------------------------------------------------- */
    /* |     1     |    1   |    2    |         4          |       2         */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* Suite Count| Pairwise Cipher Suite List | AKM Suite Count | AKM Suite List */
    /* --------------------------------------------------------------------- */
    /*            |         4*m                |     2           |   4*n     */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* |RSN Capabilities|PMKID Count|PMKID List|Group Management Cipher Suite */
    /* --------------------------------------------------------------------- */
    /* |        2       |    2      |   16 *s  |               4           | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    if (puc_rsn_ie[1] < MAC_MIN_RSN_LEN) {
        oam_warning_log1(0, OAM_SF_WPA, "{hmac_get_rsn_capability::invalid rsn ie len[%d].}", puc_rsn_ie[1]);
        return 0;
    }
    us_index += 8; /* 偏移8 byte，获取pairwise_count */
    us_pairwise_count = hi_makeu16(puc_rsn_ie[us_index], puc_rsn_ie[us_index + 1]);
    if (us_pairwise_count > MAC_PAIRWISE_CIPHER_SUITES_NUM) {
        oam_warning_log1(0, OAM_SF_WPA, "{hmac_get_rsn_capability::invalid us_pairwise_count[%d].}", us_pairwise_count);
        return 0;
    }
    us_index += 2 + 4 * (hi_u8)us_pairwise_count; /* 再偏移(2 + 4 * pairwise_count) byte，获取akm_count */
    us_akm_count = hi_makeu16(puc_rsn_ie[us_index], puc_rsn_ie[us_index + 1]);
    if (us_akm_count > MAC_AUTHENTICATION_SUITE_NUM) {
        oam_warning_log1(0, OAM_SF_WPA, "{hmac_get_rsn_capability::invalid us_akm_count[%d].}", us_akm_count);
        return 0;
    }
    us_index += 2 + 4 * (hi_u8)us_akm_count; /* 再偏移(2 + 4 * akm_count) byte，获取rsn_capability */
    us_rsn_capability = hi_makeu16(puc_rsn_ie[us_index], puc_rsn_ie[us_index + 1]);
    return us_rsn_capability;
}

/* ****************************************************************************
 功能描述  : 设置power capability信息元素
 输入参数  : mac_vap_stru *pst_vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len
 修改历史      :
  1.日    期   : 2013年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_set_power_cap_ie(hi_u8 *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_vap_stru           *mac_vap       = (mac_vap_stru *)vap;
    mac_regclass_info_stru *regclass_info = HI_NULL;

    /* *******************************************************************************************
            ------------------------------------------------------------------------------------
            |ElementID | Length | MinimumTransmitPowerCapability| MaximumTransmitPowerCapability|
            ------------------------------------------------------------------------------------
    Octets: |1         | 1      | 1                             | 1                             |
            -------------------------------------------------------------------------------------

    ******************************************************************************************** */
    *puc_buffer       = MAC_EID_PWRCAP;
    *(puc_buffer + 1) = MAC_PWR_CAP_LEN;
    /* 成功获取管制域信息则根据国家码和TPC设置最大和最小发射功率，否则默认为0 */
    regclass_info = mac_get_channel_num_rc_info(mac_vap->channel.band, mac_vap->channel.chan_number);
    if (regclass_info != HI_NULL) {
        *(puc_buffer + 2) = /* puc_buffer第2 byte 表示最大发射功率 */
            (hi_u8)((mac_vap->channel.band == WLAN_BAND_2G) ? 4 : 3); /* 2G场景下最大发射功率为4，否则为3 */
        *(puc_buffer + 3) = /* puc_buffer第3 byte 表示最小发射功率 */
            oal_min(regclass_info->max_reg_tx_pwr, regclass_info->max_tx_pwr);
    } else {
        *(puc_buffer + 2) = 0; /* 未获取管制域信息, 将puc_buffer第2 byte(最大发射功率)置为0 */
        *(puc_buffer + 3) = 0; /* 未获取管制域信息, 将puc_buffer第3 byte(最小发射功率)置为0 */
    }
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_PWR_CAP_LEN;
}

/* ****************************************************************************
 功能描述  : 设置支持信道信息元素
 输入参数  : [1]vap,
             [2]puc_buffer
             [3]puc_ie_len
 返 回 值  : 无
**************************************************************************** */
hi_void mac_set_supported_channel_ie(hi_u8 *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    hi_u8 channel_max_num;
    hi_u8 channel_idx;
    hi_u8 us_channel_ie_len = 0;
    hi_u8 *puc_ie_len_buffer = 0;
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    hi_u8 channel_idx_cnt = 0;
    if (mac_vap->mib_info->wlan_mib_sta_config.dot11_spectrum_management_required == HI_FALSE ||
        mac_vap->mib_info->wlan_mib_sta_config.dot11_extended_channel_switch_activated == HI_TRUE) {
        *puc_ie_len = 0;
        return;
    }

    /* *******************************************************************************************
            长度不定，信道号与信道数成对出现
            ------------------------------------------------------------------------------------
            |ElementID | Length | Fisrt Channel Number| Number of Channels|
            ------------------------------------------------------------------------------------
    Octets: |1         | 1      | 1                   | 1                 |
            -------------------------------------------------------------------------------------

    ******************************************************************************************** */
    /* 根据支持的频段获取最大信道个数 */
    if (WLAN_BAND_2G == mac_vap->channel.band) {
        channel_max_num = (hi_u8)MAC_CHANNEL_FREQ_2_BUTT;
    } else {
        *puc_ie_len = 0;
        return;
    }

    *puc_buffer = MAC_EID_SUPPCHAN;
    puc_buffer++;
    puc_ie_len_buffer = puc_buffer;
    /* 填写信道信息 */
    for (channel_idx = 0; channel_idx < channel_max_num; channel_idx++) {
        /* 修改管制域结构体后，需要增加该是否支持信号的判断 */
        if (mac_is_channel_idx_valid(mac_vap->channel.band, channel_idx, HI_NULL) == HI_SUCCESS) {
            channel_idx_cnt++;
            /* uc_channel_idx_cnt为1的时候表示是第一个可用信道，需要写到Fisrt Channel Number */
            if (channel_idx_cnt == 1) {
                puc_buffer++;
                mac_get_channel_num_from_idx(mac_vap->channel.band, channel_idx, puc_buffer);
            } else if ((channel_max_num - 1) == channel_idx) {
                /* 将Number of Channels写入帧体中 */
                puc_buffer++;
                *puc_buffer = channel_idx_cnt;
                us_channel_ie_len += 2; /* 信道IE长度每次增加2 byte */
            }
        } else {
            /* uc_channel_idx_cnt不为0的时候表示之前有可用信道，需要将可用信道的长度写到帧体中 */
            if (channel_idx_cnt != 0) {
                /* 将Number of Channels写入帧体中 */
                puc_buffer++;
                *puc_buffer = channel_idx_cnt;
                us_channel_ie_len += 2; /* 信道IE长度每次增加2 byte */
            }
            /* 将Number of Channels统计清零 */
            channel_idx_cnt = 0;
        }
    }
    *puc_ie_len_buffer = us_channel_ie_len;
    *puc_ie_len = us_channel_ie_len + MAC_IE_HDR_LEN;
}

/* ****************************************************************************
 功能描述  : 设置WMM info element
 输入参数  : mac_vap_stru  *pst_vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2013年10月23日
    作    者   : HiSilicon
    修改内容   : 修改函数名和填充内容
**************************************************************************** */
hi_void mac_set_wmm_ie_sta(hi_u8 *vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    hi_u8 index;
    mac_vap_stru *mac_vap = (mac_vap_stru *)vap;
    hi_u8     auc_oui[MAC_OUI_LEN] = {(hi_u8)MAC_WLAN_OUI_MICRO0,
                                      (hi_u8)MAC_WLAN_OUI_MICRO1, (hi_u8)MAC_WLAN_OUI_MICRO2};

    /* WMM Information Element Format                                */
    /* ------------------------------------------------------------- */
    /* | 3     | 1        | 1           | 1             | 1        | */
    /* ------------------------------------------------------------- */
    /* | OUI   | OUI Type | OUI Subtype | Version field | QoS Info | */
    /* ------------------------------------------------------------- */
    /* 判断STA是否支持WMM */
    if (!mac_vap->mib_info->wlan_mib_sta_config.dot11_qos_option_implemented) {
        *puc_ie_len = 0;
        return;
    }

    puc_buffer[0] = MAC_EID_WMM;
    puc_buffer[1] = MAC_WMM_INFO_LEN;
    index = MAC_IE_HDR_LEN;
    /* OUI */
    if (memcpy_s(&puc_buffer[index], MAC_OUI_LEN, auc_oui, MAC_OUI_LEN) != EOK) {
        return;
    }
    index += MAC_OUI_LEN;
    /* OUI Type */
    puc_buffer[index++] = MAC_OUITYPE_WMM;
    /* OUI Subtype */
    puc_buffer[index++] = MAC_OUISUBTYPE_WMM_INFO;
    /* Version field */
    puc_buffer[index++] = MAC_OUI_WMM_VERSION;
    /* QoS Information Field */
    mac_set_qos_info_field(mac_vap, &puc_buffer[index]);
    index += MAC_QOS_INFO_LEN;
    /* Reserved */
    puc_buffer[index++] = 0;
    *puc_ie_len = MAC_IE_HDR_LEN + MAC_WMM_INFO_LEN;
}

/* ****************************************************************************
 功能描述  : 设置listen interval信息元素
 输入参数  : mac_vap_stru *pst_vap, hi_u8 *puc_buffer, hi_u8 *puc_ie_len
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_set_listen_interval_ie(hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    puc_buffer[0] = 0x03;
    puc_buffer[1] = 0x00;
    *puc_ie_len = MAC_LIS_INTERVAL_IE_LEN;
}

/* ****************************************************************************
 功能描述  : 设置状态码信息元素
 返 回 值  : hi_void
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_set_status_code_ie(hi_u8 *puc_buffer, mac_status_code_enum_uint16 status_code)
{
    puc_buffer[0] = (hi_u8)(status_code & 0x00FF);
    puc_buffer[1] = (hi_u8)((status_code & 0xFF00) >> 8); /* 状态码信息元素byte 1,赋值为status_code高8 byte */
}

/* ****************************************************************************
 功能描述  : 设置AID（扫描ID）信息元素
 输入参数  : hi_u8 *puc_buffer, hi_u16 uc_status_code
 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_set_aid_ie(hi_u8 *puc_buffer, hi_u16 aid)
{
    /* The 2 MSB bits of Association ID is set to 1 as required by the standard. */
    aid |= 0xC000;
    puc_buffer[0] = (aid & 0x00FF);
    puc_buffer[1] = (aid & 0xFF00) >> 8; /* 扫描ID信息元素byte 1,赋值为aid高8 byte */
}

/* ****************************************************************************
 函 数 名  : mac_get_bss_type
 功能描述  : 获取BSS的类型
 修改历史      :
  1.日    期   : 2013年7月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_get_bss_type(hi_u16 us_cap_info)
{
    mac_cap_info_stru *cap_info = (mac_cap_info_stru *)&us_cap_info;

    if (cap_info->ess != 0) {
        return (hi_u8)WLAN_MIB_DESIRED_BSSTYPE_INFRA;
    }
    if (cap_info->ibss != 0) {
        return (hi_u8)WLAN_MIB_DESIRED_BSSTYPE_INDEPENDENT;
    }
    return (hi_u8)WLAN_MIB_DESIRED_BSSTYPE_ANY;
}

/* ****************************************************************************
 功能描述  : 检查CAP INFO中privacy 是否加密
 修改历史      :
  1.日    期   : 2013年7月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_check_mac_privacy(hi_u16 us_cap_info, hi_u8 *vap)
{
    mac_vap_stru       *mac_vap = HI_NULL;
    mac_cap_info_stru  *cap_info = (mac_cap_info_stru *)&us_cap_info;

    mac_vap = (mac_vap_stru *)vap;
    if (mac_vap->mib_info->wlan_mib_privacy.dot11_privacy_invoked) {
        /* 该VAP有Privacy invoked但其他VAP没有 */
        if (cap_info->privacy == 0) {
            return HI_FALSE;
        }
    }
    /* 考虑兼容性，当vap不支持加密时，不检查用户的能力 */
    return HI_TRUE;
}

/* ****************************************************************************
 功能描述  : 设置LLC SNAP, TX流程上调用
 输入参数  : pst_buf netbuf结构体 us_ether_type 以太网类型
**************************************************************************** */
hi_void mac_set_snap(oal_netbuf_stru *netbuf, hi_u16 us_ether_type, hi_u8 offset)
{
    mac_llc_snap_stru *llc = HI_NULL;
    hi_u16 use_btep1;
    hi_u16 use_btep2;

    /* LLC */
    llc = (mac_llc_snap_stru *)(oal_netbuf_data(netbuf) + offset);
    llc->llc_dsap = SNAP_LLC_LSAP;
    llc->llc_ssap = SNAP_LLC_LSAP;
    llc->control  = LLC_UI;

    use_btep1 = hi_swap_byteorder_16(ETHER_TYPE_AARP);
    use_btep2 = hi_swap_byteorder_16(ETHER_TYPE_IPX);
    if (oal_unlikely((use_btep1 == us_ether_type) || (use_btep2 == us_ether_type))) {
        llc->auc_org_code[0] = SNAP_BTEP_ORGCODE_0; /* org_code[0]:0x0 */
        llc->auc_org_code[1] = SNAP_BTEP_ORGCODE_1; /* org_code[1]:0x0 */
        llc->auc_org_code[2] = SNAP_BTEP_ORGCODE_2; /* org_code[2]:0xf8 */
    } else {
        llc->auc_org_code[0] = SNAP_RFC1042_ORGCODE_0; /* org_code[0]:0x0 */
        llc->auc_org_code[1] = SNAP_RFC1042_ORGCODE_1; /* org_code[1]:0x0 */
        llc->auc_org_code[2] = SNAP_RFC1042_ORGCODE_2; /* org_code[2]:0x0 */
    }
    llc->us_ether_type = us_ether_type;
    oal_netbuf_pull(netbuf, offset);
}

/* ****************************************************************************
 功能描述  : 获取mac头中的qos ctrl字段
**************************************************************************** */
hi_void mac_get_qos_ctrl(const hi_u8 *puc_mac_hdr, hi_u8 *puc_qos_ctrl)
{
    if (!mac_is_4addr(puc_mac_hdr)) {
        if (memcpy_s(puc_qos_ctrl, MAC_QOS_CTL_LEN, puc_mac_hdr + MAC_QOS_CTRL_FIELD_OFFSET, MAC_QOS_CTL_LEN) != EOK) {
            oam_error_log0(0, 0, "{mac_get_qos_ctrl::memcpy_s fail.}");
            return;
        }
        return;
    }
    if (memcpy_s(puc_qos_ctrl, MAC_QOS_CTL_LEN, puc_mac_hdr + MAC_QOS_CTRL_FIELD_OFFSET_4ADDR, MAC_QOS_CTL_LEN) !=  EOK) {
        oam_error_log0(0, 0, "{mac_get_qos_ctrl::memcpy_s fail.}");
        return;
    }
}

/* ****************************************************************************
 功能描述  : 填充厂商自定义ie
 输入参数  : pst_vap: 指向vap
             puc_buffer: 指向buffer
 输出参数  : puc_ie_len: element的长度
 修改历史      :
  1.日    期   : 2014年6月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年11月18日
    作    者   : HiSilicon
    修改内容   : 分配2.4G 11ac私有增强OUI和Type
**************************************************************************** */
hi_void mac_set_vendor_hisi_ie(hi_u8 *puc_buffer, hi_u8 *puc_ie_len)
{
    mac_ieee80211_vendor_ie_stru *vendor_ie;

    vendor_ie = (mac_ieee80211_vendor_ie_stru *)puc_buffer;
    vendor_ie->element_id = MAC_EID_VENDOR;
    vendor_ie->len = sizeof(mac_ieee80211_vendor_ie_stru) - MAC_IE_HDR_LEN;

    /* 此值参照a公司所写 */
    vendor_ie->oui_type = MAC_EID_VHT_TYPE;
    vendor_ie->auc_oui[0] = (hi_u8)((MAC_VENDER_IE >> 16) & 0xff); /* oui[0]由最低HW IE次最高16 bit获取 */
    vendor_ie->auc_oui[1] = (hi_u8)((MAC_VENDER_IE >> 8) & 0xff);  /* oui[1]由最低HW IE次低8 bit获取 */
    vendor_ie->auc_oui[2] = (hi_u8)((MAC_VENDER_IE) & 0xff);       /* oui[2]由最低HW IE最低8 bit获取 */
    *puc_ie_len = sizeof(mac_ieee80211_vendor_ie_stru);
}


/* ****************************************************************************
 功能描述  : 判断是否是厂家自定义Action类型
 修改历史      :
  1.日    期   : 2019年4月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_check_is_vendor_action(hi_u32 oui, const hi_u8 *puc_ies, hi_u16 us_len)
{
    hi_u32 ie_oui;

    if (us_len < 4) { /* puc_ies大小不小于4字节 */
        return HI_FALSE;
    }

    ie_oui = (puc_ies[1] << 16) | (puc_ies[2] << 8) | puc_ies[3]; /* 获OUI类型,byte 1在高16bit,byte 2在bit8~15,byte 3 */
    if (ie_oui == oui) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 判断是否是厂家自定义Action类型帧
 输入参数  : [1]oui
             [2]oui_type
             [3]puc_ies
             [4]l_len
 返 回 值  : hi_u8 *类型指针，HI_NULL表示没有找到对应vendor action位置
**************************************************************************** */
hi_u8 mac_find_vendor_action(hi_u32 oui, hi_u8 oui_type, const hi_u8 *puc_ies, hi_s32 l_len)
{
    hi_u32 ie_oui;
    hi_u8 type;

    if ((puc_ies == HI_NULL) || (l_len <= MAC_ACTION_VENDOR_TYPE_POS)) {
        return HI_FALSE;
    }

    if (puc_ies[0] != MAC_ACTION_CATEGORY_VENDOR) {
        return HI_FALSE;
    }

    type = puc_ies[MAC_ACTION_VENDOR_TYPE_POS];
    ie_oui = (puc_ies[1] << 16) | (puc_ies[2] << 8) | puc_ies[3]; /* 16:左移位数；2:数组位数；8:左移位数；3:数组位数 */
    if ((ie_oui == oui) && (type == oui_type)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
 * 功能描述  : 设置管理帧头
 * 输入参数  : mac_header MAC 头指针，frame_type : Frame Control，
 * addr1，addr2，addr3: mac addrsss
 * *************************************************************************** */
WIFI_ROM_TEXT hi_u16 mac_set_mgmt_frame_header(hi_u8 *mac_header, hi_u16 frame_type, const hi_u8 *addr1,
    const hi_u8 *addr2, const hi_u8 *addr3)
{
    /* *********************************************************************** */
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /* *********************************************************************** */
    mac_hdr_set_frame_control(mac_header, frame_type);

    /* 设置duration为0，由硬件填 */
    mac_hdr_set_duration(mac_header, frame_type, 0);

    /* 设置地址1, 2, 3  */
    if (mac_hdr_set_mac_addrsss(mac_header, addr1, addr2, addr3) == 0) {
        return 0;
    }

    /* 设置分片序号, 管理帧为0 */
    mac_hdr_set_fragment_number(mac_header, 0);

    return MAC_80211_FRAME_LEN;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
