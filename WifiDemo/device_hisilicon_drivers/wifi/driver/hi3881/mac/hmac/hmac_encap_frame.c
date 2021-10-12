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
#include "hmac_encap_frame.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
功能描述  : 组sa query 请求帧
输入参数  : pst_mac_vap :mac vap结构体
            puc_data    :netbuf data指针
            puc_da      :目标用户的mac地址
            trans_id    :sa query ie,用于辨别response和request是否一致
返 回 值  : 帧头+帧体的长度
修改历史      :
 1.日    期   : 2014年4月19日
   作    者   : HiSilicon
   修改内容   : 新生成函数
**************************************************************************** */
hi_u16 hmac_encap_sa_query_req(const mac_vap_stru *mac_vap, hi_u8 *puc_data, const hi_u8 *da_mac_addr,
    hi_u16 us_trans_id)
{
    hi_u16 us_len;

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
    /* All the fields of the Frame Control Field are set to zero. Only the   */
    /* Type/Subtype field is set.                                            */
    mac_hdr_set_frame_control(puc_data, WLAN_FC0_SUBTYPE_ACTION);
    /*  Set DA  */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address1, WLAN_MAC_ADDR_LEN, da_mac_addr,
        WLAN_MAC_ADDR_LEN) != EOK) {
        return 0;
    }
    /*  Set SA  */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address2, WLAN_MAC_ADDR_LEN,
        mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
        return 0;
    }
    /*  Set SSID  */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address3, WLAN_MAC_ADDR_LEN, mac_vap->auc_bssid,
        WLAN_MAC_ADDR_LEN) != EOK) {
        return 0;
    }
    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                  SA Query Frame - Frame Body                          */
    /* --------------------------------------------------------------------- */
    /* |   Category   |SA Query Action |  Transaction Identifier           | */
    /* --------------------------------------------------------------------- */
    /* |1             |1               |2 Byte                             | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    puc_data[MAC_80211_FRAME_LEN] = MAC_ACTION_CATEGORY_SA_QUERY;
    puc_data[MAC_80211_FRAME_LEN + 1] = MAC_SA_QUERY_ACTION_REQUEST; /* 1:偏移1 */
    puc_data[MAC_80211_FRAME_LEN + 2] = (us_trans_id & 0x00FF);      /* 2:偏移2 */
    puc_data[MAC_80211_FRAME_LEN + 3] = (us_trans_id & 0xFF00) >> 8; /* 3:偏移1，8:向右移动8位 */

    us_len = MAC_80211_FRAME_LEN + MAC_SA_QUERY_LEN;
    return us_len;
}

/* ****************************************************************************
功能描述  : 组sa query 反馈帧
输入参数  : pst_hdr:sa query request frame
            puc_data:sa query response frame
返 回 值  : 帧头+帧体的长度
修改历史      :
 1.日    期   : 2014年4月19日
   作    者   : HiSilicon
   修改内容   : 新生成函数
**************************************************************************** */
hi_u16 hmac_encap_sa_query_rsp(const mac_vap_stru *mac_vap, const hi_u8 *hdr, hi_u8 *puc_data)
{
    hi_u16 us_len;

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
    /* All the fields of the Frame Control Field are set to zero. Only the   */
    /* Type/Subtype field is set.                                            */
    mac_hdr_set_frame_control(puc_data, WLAN_FC0_SUBTYPE_ACTION);
    /* Set DA  */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address1, WLAN_MAC_ADDR_LEN,
        ((mac_ieee80211_frame_stru *)hdr)->auc_address2, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_sa_query_rsp::mem safe function err!}");
        return 0;
    }
    /*  Set SA  */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address2, WLAN_MAC_ADDR_LEN,
        mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_sa_query_rsp::mem safe function err!}");
        return 0;
    }
    /*  Set SSID  */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address3, WLAN_MAC_ADDR_LEN, mac_vap->auc_bssid,
        WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_encap_sa_query_rsp::mem safe function err!}");
        return 0;
    }
    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                  SA Query Frame - Frame Body                          */
    /* --------------------------------------------------------------------- */
    /* |   Category   |SA Query Action |  Transaction Identifier           | */
    /* --------------------------------------------------------------------- */
    /* |1             |1               |2 Byte                             | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    puc_data[MAC_80211_FRAME_LEN] = hdr[MAC_80211_FRAME_LEN];
    puc_data[MAC_80211_FRAME_LEN + 1] = MAC_SA_QUERY_ACTION_RESPONSE; /* 1:偏移1 */
    puc_data[MAC_80211_FRAME_LEN + 2] = hdr[MAC_80211_FRAME_LEN + 2]; /* 2:偏移2 */
    puc_data[MAC_80211_FRAME_LEN + 3] = hdr[MAC_80211_FRAME_LEN + 3]; /* 3:偏移3 */

    us_len = MAC_80211_FRAME_LEN + MAC_SA_QUERY_LEN;
    return us_len;
}

/* ****************************************************************************
 功能描述  : 组去认证帧
**************************************************************************** */
hi_u16 hmac_mgmt_encap_deauth(const mac_vap_stru *mac_vap, hi_u8 *puc_data, const hi_u8 *da_mac_addr, hi_u16 err_code)
{
    hi_u8 auc_bssid[WLAN_MAC_ADDR_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    mac_ieee80211_frame_stru *mac_hdr = (mac_ieee80211_frame_stru *)puc_data;

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
    /* All the fields of the Frame Control Field are set to zero. Only the   */
    /* Type/Subtype field is set.                                            */
    mac_hdr_set_frame_control(puc_data, WLAN_FC0_SUBTYPE_DEAUTH);
    /* Set DA to address of unauthenticated STA */
    if (memcpy_s(mac_hdr->auc_address1, WLAN_MAC_ADDR_LEN, da_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        return 0;
    }
#ifdef _PRE_WLAN_FEATURE_P2P
    if (err_code & MAC_SEND_TWO_DEAUTH_FLAG) {
        mac_vap_stru *up_vap1 = HI_NULL;
        mac_vap_stru *up_vap2 = HI_NULL;
        err_code = err_code & ~MAC_SEND_TWO_DEAUTH_FLAG;

        mac_device_stru *mac_dev = mac_res_get_dev();
        if (mac_device_find_2up_vap(mac_dev, &up_vap1, &up_vap2) == HI_SUCCESS) {
            /* 获取另外一个VAP，组帧时修改地址2为另外1个VAP的MAC地址 */
            up_vap2 = (mac_vap->vap_id != up_vap1->vap_id) ? up_vap1 : up_vap2;
            if (up_vap2->mib_info == HI_NULL) {
                return 0;
            }
            if ((memcpy_s(mac_hdr->auc_address2, WLAN_MAC_ADDR_LEN,
                up_vap2->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) ||
                (memcpy_s(mac_hdr->auc_address3, WLAN_MAC_ADDR_LEN, up_vap2->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK)) {
                return 0;
            }
        }
    } else {
#endif
        if (mac_vap->mib_info == HI_NULL) {
            return 0;
        }
        /* SA is the dot11MACAddress */
        if (memcpy_s(mac_hdr->auc_address2, WLAN_MAC_ADDR_LEN,
            mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
            return 0;
        }
        /*
         * 针对50行整改,先填写BSSID后对BSSID进行全0比较
         * STA处于FAKE_UP状态，mac vap中bssid为全0，deauth帧对端不处理,改为填写da。
         */
        if (memcpy_s(mac_hdr->auc_address3, WLAN_MAC_ADDR_LEN, mac_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
            return 0;
        }
        if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) &&
            (memcmp(mac_vap->auc_bssid, auc_bssid, WLAN_MAC_ADDR_LEN) == 0)) {
            if (memcpy_s(mac_hdr->auc_address3, WLAN_MAC_ADDR_LEN, da_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
                return 0;
            }
        }
#ifdef _PRE_WLAN_FEATURE_P2P
    }
#endif
    /* *********************************************************************** */
    /*                Set the contents of the frame body                     */
    /* *********************************************************************** */
    /* *********************************************************************** */
    /*                  Deauthentication Frame - Frame Body                  */
    /* --------------------------------------------------------------------- */
    /* |                           Reason Code                             | */
    /* --------------------------------------------------------------------- */
    /* |2 Byte                                                             | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* Set Reason Code to 'Class2 error' */
    puc_data[MAC_80211_FRAME_LEN] = (err_code & 0x00FF);
    puc_data[MAC_80211_FRAME_LEN + 1] = (err_code & 0xFF00) >> 8; /* 1:偏移1，8:向右位移8 */

    return (MAC_80211_FRAME_LEN + WLAN_REASON_CODE_LEN);
}

/* ****************************************************************************
 功能描述  : 组去关联帧
 输入参数  : vap指针,DA,ErrCode
 输出参数  : 帧缓冲区
 返 回 值  : 帧长度
 修改历史      :
  1.日    期   : 2013年12月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u16 hmac_mgmt_encap_disassoc(const mac_vap_stru *mac_vap, hi_u8 *puc_data, const hi_u8 *da_mac_addr,
    hi_u16 us_err_code)
{
    hi_u16 us_disassoc_len = 0;

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
    /*                            设置帧头                                   */
    /* *********************************************************************** */
    /* 设置subtype   */
    mac_hdr_set_frame_control(puc_data, WLAN_FC0_SUBTYPE_DISASSOC);

    if (mac_vap->mib_info == HI_NULL) {
        us_disassoc_len = 0;
        oam_error_log0(mac_vap->vap_id, OAM_SF_AUTH, "hmac_mgmt_encap_disassoc: pst_mac_vap mib ptr null.");
        return us_disassoc_len;
    }
    /* 设置DA */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address1, WLAN_MAC_ADDR_LEN, da_mac_addr,
        WLAN_MAC_ADDR_LEN) != EOK) {
        return 0;
    }
    /* 设置SA */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address2, WLAN_MAC_ADDR_LEN,
        mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
        return 0;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    /* Mesh 下直接填充本设备mac 地址，不判断 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address3, WLAN_MAC_ADDR_LEN,
            mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
            return 0;
        }
    } else {
        /* 设置bssid */
        if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address3, WLAN_MAC_ADDR_LEN,
            mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP ? mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id :
            mac_vap->auc_bssid, WLAN_MAC_ADDR_LEN) != EOK) {
            return 0;
        }
    }
#else
    /* 设置bssid */
    if (memcpy_s(((mac_ieee80211_frame_stru *)puc_data)->auc_address3, WLAN_MAC_ADDR_LEN,
        mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP ?
        mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id : mac_vap->auc_bssid,
        WLAN_MAC_ADDR_LEN) != EOK) {
        return 0;
    }
#endif

    /* *********************************************************************** */
    /*                  Disassociation 帧 - 帧体                  */
    /* --------------------------------------------------------------------- */
    /* |                           Reason Code                             | */
    /* --------------------------------------------------------------------- */
    /* |2 Byte                                                             | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* 设置reason code */
    puc_data[MAC_80211_FRAME_LEN] = (us_err_code & 0x00FF);
    puc_data[MAC_80211_FRAME_LEN + 1] = (us_err_code & 0xFF00) >> 8; /* 1:偏移1，8:向右位移8 */

    us_disassoc_len = MAC_80211_FRAME_LEN + WLAN_REASON_CODE_LEN;

    return us_disassoc_len;
}

/* ****************************************************************************
 功能描述  : 检查当前STA是否支持AP的基本速率
 修改历史      :
  1.日    期   : 2013年7月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_check_sta_base_rate(hi_u8 *user, mac_status_code_enum_uint16 *pen_status_code)
{
    hi_u8 num_basic_rates;
    hi_u8 loop;
    hi_u8 index;
    hi_u8 found;
    hi_u8 num_rates;
    hi_u8 ap_base_rate;
    mac_vap_stru *mac_vap = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;

    hmac_user = (hmac_user_stru *)user;

    /* 获取VAP */
    mac_vap = mac_vap_get_vap_stru(hmac_user->base_user->vap_id);
    if (mac_vap == HI_NULL) {
        return;
    }

    num_basic_rates = mac_vap->curr_sup_rates.br_rate_num;
    num_rates = hmac_user->op_rates.rs_nrates;

    for (loop = 0; loop < num_basic_rates; loop++) {
        found = HI_FALSE;
        ap_base_rate = mac_vap_get_curr_baserate(mac_vap, loop);

        for (index = 0; index < num_rates; index++) {
            if ((hmac_user->op_rates.auc_rs_rates[index] & 0x7F) == (ap_base_rate & 0x7F)) {
                found = HI_TRUE;
                break;
            }
        }

        /* 不支持某基本速率，返回false  */
        if (found == HI_FALSE) {
            *pen_status_code = MAC_UNSUP_RATE;
            return;
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
