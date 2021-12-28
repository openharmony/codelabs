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
#include "hmac_user.h"
#include "hmac_vap.h"
#include "hmac_protection.h"
#include "mac_vap.h"
#include "hmac_config.h"
#include "frw_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 同步保护相关的参数到Dmac
 输入参数  : pst_hmac_vap : hmac vap结构体指针
 修改历史      :
  1.日    期   : 2017年1月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
************************************************************************** */
hi_u32 hmac_user_protection_sync_data(const mac_vap_stru *mac_vap)
{
    mac_h2d_protection_stru h2d_prot;
    hi_u8 lsig_txop_full_protection_activated;
    hi_u8 non_gf_entities_present;
    hi_u8 rifs_mode;
    hi_u8 ht_protection;

    /* 安全编程规则6.6例外(1) 对固定长度的结构体进行内存初始化 */
    memset_s(&h2d_prot, sizeof(mac_h2d_protection_stru), 0x00, sizeof(h2d_prot));

    /* 更新vap的en_dot11NonGFEntitiesPresent字段 */
    non_gf_entities_present = (0 != mac_vap->protection.sta_non_gf_num) ? HI_TRUE : HI_FALSE;
    mac_mib_set_non_gfentities_present(mac_vap, non_gf_entities_present);

    /* 更新vap的en_dot11LSIGTXOPFullProtectionActivated字段 */
    lsig_txop_full_protection_activated = (0 == mac_vap->protection.sta_no_lsig_txop_num) ? HI_TRUE : HI_FALSE;
    mac_mib_set_lsig_txop_full_protection_activated(mac_vap, lsig_txop_full_protection_activated);

    /* 更新vap的en_dot11HTProtection和en_dot11RIFSMode字段 */
    if (mac_vap->protection.sta_non_ht_num != 0) {
        ht_protection = WLAN_MIB_HT_NON_HT_MIXED;
        rifs_mode = HI_FALSE;
    } else if (mac_vap->protection.obss_non_ht_present == HI_TRUE) {
        ht_protection = WLAN_MIB_HT_NONMEMBER_PROTECTION;
        rifs_mode     = HI_FALSE;
    } else if ((WLAN_BAND_WIDTH_20M != mac_vap->channel.en_bandwidth)
                && (mac_vap->protection.sta_20_m_only_num != 0)) {
        ht_protection = WLAN_MIB_HT_20MHZ_PROTECTION;
        rifs_mode     = HI_TRUE;
    } else {
        ht_protection = WLAN_MIB_HT_NO_PROTECTION;
        rifs_mode     = HI_TRUE;
    }

    mac_mib_set_ht_protection(mac_vap, ht_protection);
    mac_mib_set_rifs_mode(mac_vap, rifs_mode);

    if (memcpy_s((hi_u8 *)&h2d_prot.protection, sizeof(mac_protection_stru), (hi_u8 *)&mac_vap->protection,
        sizeof(mac_protection_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_user_protection_sync_data:: st_protection memcpy_s fail.");
        return HI_FAIL;
    }

    h2d_prot.dot11_ht_protection         = mac_mib_get_ht_protection(mac_vap);
    h2d_prot.dot11_rifs_mode             = mac_mib_get_rifs_mode(mac_vap);
    h2d_prot.dot11_lsigtxop_full_protection_activated = mac_mib_get_lsig_txop_full_protection_activated(mac_vap);
    h2d_prot.dot11_non_gf_entities_present = mac_mib_get_non_gfentities_present(mac_vap);

    return hmac_protection_update_from_user(mac_vap, sizeof(h2d_prot), (hi_u8 *)&h2d_prot);
}

/* ****************************************************************************
 功能描述  : 删除保护模式相关user统计(legacy)
 输入参数  : pst_mac_vap  : mac vap结构体指针
             pst_mac_user : mac user结构体指针
 修改历史      :
  1.日    期   : 2014年1月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_protection_del_user_stat_legacy_ap(mac_vap_stru *mac_vap, const mac_user_stru *mac_user)
{
    mac_protection_stru *protection = &(mac_vap->protection);
    hmac_user_stru *hmac_user = HI_NULL;

    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru((hi_u8)mac_user->us_assoc_id);
    if (hmac_user == HI_NULL) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
            "hmac_protection_del_user_stat_legacy_ap::Get Hmac_user(idx=%d) NULL POINT!", mac_user->us_assoc_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 如果去关联的站点不支持ERP */
    if ((hmac_user->hmac_cap_info.erp == HI_FALSE) && (hmac_user->user_stats_flag.no_erp_stats_flag == HI_TRUE) &&
        (protection->sta_non_erp_num != 0)) {
        protection->sta_non_erp_num--;
    }

    /* 如果去关联的站点不支持short preamble */
    if ((hmac_user->hmac_cap_info.short_preamble == HI_FALSE) &&
        (hmac_user->user_stats_flag.no_short_preamble_stats_flag == HI_TRUE) &&
        (protection->sta_no_short_preamble_num != 0)) {
        protection->sta_no_short_preamble_num--;
    }

    /* 如果去关联的站点不支持short slot */
    if ((hmac_user->hmac_cap_info.short_slot_time == HI_FALSE) &&
        (hmac_user->user_stats_flag.no_short_slot_stats_flag == HI_TRUE) && (protection->sta_no_short_slot_num != 0)) {
        protection->sta_no_short_slot_num--;
    }

    hmac_user->user_stats_flag.no_short_slot_stats_flag = HI_FALSE;
    hmac_user->user_stats_flag.no_short_preamble_stats_flag = HI_FALSE;
    hmac_user->user_stats_flag.no_erp_stats_flag = HI_FALSE;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 删除保护模式相关user统计(ht)
 输入参数  : pst_mac_vap  : mac vap结构体指针
             pst_mac_user : mac user结构体指针
 修改历史      :
  1.日    期   : 2014年1月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_protection_del_user_stat_ht_ap(mac_vap_stru *mac_vap, mac_user_stru *mac_user)
{
    mac_user_ht_hdl_stru *ht_hdl = &(mac_user->ht_hdl);
    mac_protection_stru *protection = &(mac_vap->protection);
    hmac_user_stru *hmac_user = HI_NULL;

    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru((hi_u8)mac_user->us_assoc_id);
    if (hmac_user == HI_NULL) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
            "hmac_protection_del_user_stat_ht_ap::Get Hmac_user(idx=%d) NULL POINT!", mac_user->us_assoc_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 如果去关联的站点不支持HT */
    if ((ht_hdl->ht_capable == HI_FALSE) && (hmac_user->user_stats_flag.no_ht_stats_flag == HI_TRUE) &&
        (protection->sta_non_ht_num != 0)) {
        protection->sta_non_ht_num--;
    } else { /* 支持HT */
        /* 如果去关联的站点不支持20/40Mhz频宽 */
        if ((ht_hdl->ht_capinfo.supported_channel_width == HI_FALSE) &&
            (hmac_user->user_stats_flag.m_only_stats_flag == HI_TRUE) && (protection->sta_20_m_only_num != 0)) {
            protection->sta_20_m_only_num--;
        }

        /* 如果去关联的站点不支持GF */
        if ((ht_hdl->ht_capinfo.ht_green_field == HI_FALSE) &&
            (hmac_user->user_stats_flag.no_gf_stats_flag == HI_TRUE) && (protection->sta_non_gf_num != 0)) {
            protection->sta_non_gf_num--;
        }

        /* 如果去关联的站点不支持L-SIG TXOP Protection */
        if ((ht_hdl->ht_capinfo.lsig_txop_protection == HI_FALSE) &&
            (hmac_user->user_stats_flag.no_lsig_txop_stats_flag == HI_TRUE) &&
            (protection->sta_no_lsig_txop_num != 0)) {
            protection->sta_no_lsig_txop_num--;
        }

        /* 如果去关联的站点不支持40Mhz cck */
        if ((ht_hdl->ht_capinfo.dsss_cck_mode_40mhz == HI_FALSE) &&
            (ht_hdl->ht_capinfo.supported_channel_width == HI_TRUE) &&
            (hmac_user->user_stats_flag.no_40dsss_stats_flag == HI_TRUE) && (protection->sta_no_40dsss_cck_num != 0)) {
            protection->sta_no_40dsss_cck_num--;
        }
    }

    hmac_user->user_stats_flag.no_ht_stats_flag = HI_FALSE;
    hmac_user->user_stats_flag.no_gf_stats_flag = HI_FALSE;
    hmac_user->user_stats_flag.m_only_stats_flag = HI_FALSE;
    hmac_user->user_stats_flag.no_40dsss_stats_flag = HI_FALSE;
    hmac_user->user_stats_flag.no_lsig_txop_stats_flag = HI_FALSE;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 删除保护模式相关user统计
 输入参数  : pst_mac_vap  : mac vap结构体指针
             pst_mac_user : mac user结构体指针
 修改历史      :
  1.日    期   : 2014年1月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_protection_del_user_stat_ap(mac_vap_stru *mac_vap, mac_user_stru *mac_user)
{
    hi_u32 ret;

    ret = hmac_protection_del_user_stat_legacy_ap(mac_vap, mac_user);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "hmac_protection_del_user_stat_legacy_ap return NON SUCCESS. ");
    }

    ret = hmac_protection_del_user_stat_ht_ap(mac_vap, mac_user);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "hmac_protection_del_user_stat_ht_ap return NON SUCCESS. ");
    }
}

/* ****************************************************************************
 功能描述  : AP:删除user统计， 并更新保护模式
             STA: 更新为无保护模式
 输入参数  : pst_mac_vap  : mac vap结构体指针
             pst_mac_user : mac user结构体指针
 修改历史      :
  1.日    期   : 2014年1月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_protection_del_user(mac_vap_stru *mac_vap, mac_user_stru *mac_user)
{
    hi_u32 ret = HI_SUCCESS;

    /* AP 更新VAP结构体统计量，更新保护机制 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        /* 删除保护模式相关user统计 */
        hmac_protection_del_user_stat_ap(mac_vap, mac_user);
        /* 更新AP中保护相关mib量 */
        ret = hmac_user_protection_sync_data(mac_vap);
        if (ret != HI_SUCCESS) {
            oam_warning_log0(0, OAM_SF_ANY, "{hmac_protection_del_user::protection update failed}");
            return ret;
        }
    }

    return ret;
}

#if (_PRE_MULTI_CORE_MODE == _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
/* ****************************************************************************
 功能描述  : 处理保护模式同步事件
 输入参数  :
 修改历史      :
  1.日    期   : 2016年12月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_protection_info_sync_event(frw_event_mem_stru *event_mem)
{
    frw_event_stru           *event = HI_NULL;
    frw_event_hdr_stru       *event_hdr = HI_NULL;
    mac_h2d_protection_stru  *h2d_prot = HI_NULL;
    mac_vap_stru             *mac_vap = HI_NULL;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_protection_info_syn_event::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取事件、事件头以及事件payload结构体 */
    event     = frw_get_event_stru(event_mem);
    event_hdr = &(event->event_hdr);
    h2d_prot  = (mac_h2d_protection_stru *)event->auc_event_data;

    mac_vap   = (mac_vap_stru *)mac_vap_get_vap_stru(event_hdr->vap_id);
    if (mac_vap == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{hmac_protection_info_syn_event::mac_res_get_mac_vap fail.vap_id:%u}",
            event_hdr->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_vap->mib_info == HI_NULL) {
        return HI_SUCCESS;
    }
    if (h2d_prot == HI_NULL) {
        return HI_SUCCESS;
    }
    if (memcpy_s((hi_u8 *)&mac_vap->protection, sizeof(mac_protection_stru), (hi_u8 *)&h2d_prot->protection,
        sizeof(mac_protection_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_protection_info_sync_event:: st_protection memcpy_s fail.");
        return HI_FAIL;
    }

    mac_mib_set_ht_protection(mac_vap, h2d_prot->dot11_ht_protection);
    mac_mib_set_rifs_mode(mac_vap, h2d_prot->dot11_rifs_mode);
    mac_mib_set_lsig_txop_full_protection_activated(mac_vap, h2d_prot->dot11_lsigtxop_full_protection_activated);
    mac_mib_set_non_gfentities_present(mac_vap, h2d_prot->dot11_non_gf_entities_present);

    return HI_SUCCESS;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
