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
#include "mac_ie.h"
#include "mac_regdomain.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "hmac_fsm.h"
#include "hmac_sme_sta.h"
#include "hmac_device.h"
#include "hmac_scan.h"
#include "hmac_mgmt_sta.h"
#include "hmac_mgmt_ap.h"
#include "frw_timer.h"
#include "hmac_chan_mgmt.h"
#include "hmac_event.h"
#include "hcc_hmac_if.h"
#include "wal_customize.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 申请内存，存储扫描到的bss信息
 输入参数  : hi_u32 ul_mgmt_len, 上报的管理帧的长度
 修改历史      :
  1.日    期   : 2015年2月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hmac_scanned_bss_info *hmac_scan_alloc_scanned_bss(hi_u32 mgmt_len)
{
    hmac_scanned_bss_info *scanned_bss = HI_NULL;

    /* 申请内存，存储扫描到的bss信息 */
    scanned_bss = oal_memalloc(sizeof(hmac_scanned_bss_info) + mgmt_len -
                                   sizeof(scanned_bss->bss_dscr_info.auc_mgmt_buff));
    if (oal_unlikely(scanned_bss == HI_NULL)) {
        oam_warning_log0(0, OAM_SF_SCAN,
            "{hmac_scan_alloc_scanned_bss::alloc memory failed for storing scanned result.}");
        return HI_NULL;
    }

    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(scanned_bss, sizeof(hmac_scanned_bss_info) + mgmt_len - sizeof(scanned_bss->bss_dscr_info.auc_mgmt_buff),
        0, sizeof(hmac_scanned_bss_info) + mgmt_len - sizeof(scanned_bss->bss_dscr_info.auc_mgmt_buff));

    /* 初始化链表头节点指针 */
    hi_list_init(&(scanned_bss->dlist_head));

    return scanned_bss;
}

/* ****************************************************************************
 功能描述  : 将扫描到的bss添加到链表
 输入参数  : hmac_scanned_bss_info *pst_scanned_bss,        待添加到链表上的扫描到的bss节点
             hmac_device_stru *pst_hmac_device,             hmac device结构体
 修改历史      :
  1.日    期   : 2015年2月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_scan_add_bss_to_list(hmac_scanned_bss_info *scanned_bss, hmac_device_stru *hmac_dev)
{
    hmac_bss_mgmt_stru *bss_mgmt = HI_NULL; /* 管理扫描结果的结构体 */

    bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt);
    scanned_bss->bss_dscr_info.new_scan_bss = HI_TRUE;

    /* 对链表写操作前加锁 */
    oal_spin_lock(&(bss_mgmt->st_lock));

    /* 添加扫描结果到链表中，并更新扫描到的bss计数 */
    hi_list_tail_insert_optimize(&(scanned_bss->dlist_head), &(bss_mgmt->bss_list_head));

    bss_mgmt->bss_num++;
    /* 解锁 */
    oal_spin_unlock(&(bss_mgmt->st_lock));
}

/* ****************************************************************************
 功能描述  : 删除扫描结果链表中的bss节点
 输入参数  : hmac_scanned_bss_info *pst_scanned_bss,        待删除的扫描到的bss节点
             hmac_device_stru *pst_hmac_device,             hmac device结构体
 修改历史      :
  1.日    期   : 2015年2月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_scan_del_bss_from_list_nolock(hmac_scanned_bss_info *scanned_bss, hmac_device_stru *hmac_dev)
{
    hmac_bss_mgmt_stru *bss_mgmt = HI_NULL; /* 管理扫描结果的结构体 */

    bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt);

    /* 从链表中删除节点，并更新扫描到的bss计数 */
    hi_list_delete_optimize(&(scanned_bss->dlist_head));

    bss_mgmt->bss_num--;
}

/* ****************************************************************************
 功能描述  : 清除上次扫描请求相关的扫描记录信息: 包括扫描到的bss信息，并释放内存空间、以及其他信息清零
 输入参数  : hmac_scan_record_stru  *pst_scan_record
 修改历史      :
  1.日    期   : 2015年2月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_scan_clean_scan_record(hmac_scan_record_stru *scan_record)
{
    hi_list *entry = HI_NULL;
    hmac_scanned_bss_info *scanned_bss = HI_NULL;
    hmac_bss_mgmt_stru *bss_mgmt = HI_NULL;

    /* 参数合法性检查 */
    if (scan_record == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_clean_scan_record::pst_scan_record is null.}");
        return;
    }

    /* 1.一定要先清除扫描到的bss信息，再进行清零处理 */
    bss_mgmt = &(scan_record->bss_mgmt);

    /* 对链表写操作前加锁 */
    oal_spin_lock(&(bss_mgmt->st_lock));

    /* 遍历链表，删除扫描到的bss信息 */
    while (HI_FALSE == hi_is_list_empty_optimize(&(bss_mgmt->bss_list_head))) {
        entry = hi_list_delete_head_optimize(&(bss_mgmt->bss_list_head));
        scanned_bss = hi_list_entry(entry, hmac_scanned_bss_info, dlist_head);

        bss_mgmt->bss_num--;

        /* 释放扫描队列里的内存 */
        oal_free(scanned_bss);
    }

    /* 对链表写操作前加锁 */
    oal_spin_unlock(&(bss_mgmt->st_lock));

    /* 2.其它信息清零 */
    if (memset_s(scan_record, sizeof(hmac_scan_record_stru), 0, sizeof(hmac_scan_record_stru)) != EOK) {
        return;
    }
    scan_record->scan_rsp_status = MAC_SCAN_STATUS_BUTT; /* 初始化扫描完成时状态码为无效值 */

    /* 3.重新初始化bss管理结果链表和锁 */
    bss_mgmt = &(scan_record->bss_mgmt);
    hi_list_init(&(bss_mgmt->bss_list_head));

    oam_info_log0(0, OAM_SF_SCAN, "{hmac_scan_clean_scan_record::cleaned scan record success.}");

    return;
}

/* ****************************************************************************
 功能描述  : 判断输入bssid参数是否是关联的AP的bssid,用于不老化已经关联的AP
 输入参数  : hi_u8 auc_bssid[WLAN_MAC_ADDR_LEN]
 返 回 值  : HI_TRUE:是,HI_FALSE:否
 修改历史      :
  1.日    期   : 2016年1月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_s32 hmac_is_connected_ap_bssid(const hi_u8 auc_bssid[WLAN_MAC_ADDR_LEN])
{
    hi_u8 vap_idx;
    mac_vap_stru *mac_vap = HI_NULL;
    mac_device_stru *mac_dev = HI_NULL;

    mac_dev = mac_res_get_dev();
    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (oal_unlikely(mac_vap == HI_NULL)) {
            oam_warning_log1(0, OAM_SF_P2P, "{hmac_is_connected_ap_bssid::mac_vap_get_vap_stru fail! vap id is %d}",
                mac_dev->auc_vap_id[vap_idx]);
            continue;
        }
        if ((is_legacy_vap(mac_vap) && (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA)) &&
            ((mac_vap->vap_state == MAC_VAP_STATE_UP) || (mac_vap->vap_state == MAC_VAP_STATE_PAUSE))) {
            if (0 == memcmp(auc_bssid, mac_vap->auc_bssid, WLAN_MAC_ADDR_LEN)) {
                /* 不老化当前关联的AP */
                oam_info_log3(mac_vap->vap_id, OAM_SF_SCAN,
                    "{hmac_is_connected_ap_bssid::connected AP bssid:XX:XX:XX:%02X:%02X:%02X}", auc_bssid[3],
                    auc_bssid[4], auc_bssid[5]); /* 3 4 5 元素索引 */

                return HI_TRUE;
            }
        }
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 本次扫描请求发起时，清除上次扫描结果中到期的bss信息
 输入参数  : hmac_scan_record_stru  *pst_scan_record
 修改历史      :
  1.日    期   : 2015年8月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_scan_clean_expire_scanned_bss(hmac_scan_record_stru *scan_record, hi_u8 clean_flag)
{
    hi_list *entry = HI_NULL;
    hi_list *entry_tmp = HI_NULL;
    hmac_bss_mgmt_stru *bss_mgmt = HI_NULL;
    hmac_scanned_bss_info *scanned_bss = HI_NULL;
    mac_bss_dscr_stru *bss_dscr = HI_NULL;
    hi_u32 curr_time_stamp = 0;

    /* 参数合法性检查 */
    if (scan_record == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_clean_expire_scanned_bss::scan record is null.}");
        return;
    }

    /* 管理扫描的bss结果的结构体 */
    bss_mgmt = &(scan_record->bss_mgmt);

    if (clean_flag != HI_TRUE) {
        curr_time_stamp = (hi_u32)hi_get_milli_seconds();
    }
    /* 对链表写操作前加锁 */
    oal_spin_lock(&(bss_mgmt->st_lock));

    /* 遍历链表，删除上一次扫描结果中到期的bss信息 */
    hi_list_for_each_safe(entry, entry_tmp, &(bss_mgmt->bss_list_head)) {
        scanned_bss = hi_list_entry(entry, hmac_scanned_bss_info, dlist_head);
        bss_dscr = &(scanned_bss->bss_dscr_info);
        if (clean_flag != HI_TRUE) {
            if (curr_time_stamp - bss_dscr->timestamp < HMAC_SCAN_MAX_SCANNED_BSS_EXPIRE) {
                continue;
            }
        }
        /* 不老化当前正在关联的AP */
        if (hmac_is_connected_ap_bssid(bss_dscr->auc_bssid)) {
            continue;
        }

        /* 从链表中删除节点，并更新扫描到的bss计数 */
        hi_list_delete_optimize(&(scanned_bss->dlist_head));
        bss_mgmt->bss_num--;
        /* 释放对应内存 */
        oal_free(scanned_bss);
    }
    /* 对链表写操作前加锁 */
    oal_spin_unlock(&(bss_mgmt->st_lock));

    return;
}

#ifdef _PRE_DEBUG_MODE
/* ****************************************************************************
 功能描述  : 根据bss index查找对应的bss dscr结构信息
 输入参数  : hi_u32 ul_bss_index,        bss    index
 修改历史      :
  1.日    期   : 2015年2月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
mac_bss_dscr_stru *hmac_scan_find_scanned_bss_dscr_by_index(hi_u32 bss_index)
{
    hi_list *entry = HI_NULL;
    hmac_scanned_bss_info *scanned_bss = HI_NULL;
    hmac_device_stru *hmac_dev = HI_NULL;
    hmac_bss_mgmt_stru *bss_mgmt = HI_NULL;
    hi_u8 loop;

    /* 获取hmac device 结构 */
    hmac_dev = hmac_get_device_stru();
    bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt);

    /* 对链表删操作前加锁 */
    oal_spin_lock(&(bss_mgmt->st_lock));

    /* 如果索引大于总共扫描的bss个数，返回异常 */
    if (bss_index >= bss_mgmt->bss_num) {
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_find_scanned_bss_by_index::no such bss in bss list!}");

        /* 解锁 */
        oal_spin_unlock(&(bss_mgmt->st_lock));
        return HI_NULL;
    }

    loop = 0;
    /* 遍历链表，返回对应index的bss dscr信息 */
    hi_list_for_each(entry, &(bss_mgmt->bss_list_head)) {
        scanned_bss = hi_list_entry(entry, hmac_scanned_bss_info, dlist_head);

        /* 相同的bss index返回 */
        if (bss_index == loop) {
            /* 解锁 */
            oal_spin_unlock(&(bss_mgmt->st_lock));
            return &(scanned_bss->bss_dscr_info);
        }

        loop++;
    }
    /* 解锁 */
    oal_spin_unlock(&(bss_mgmt->st_lock));

    return HI_NULL;
}
#endif

/* ****************************************************************************
 功能描述  : 查找相同的bssid的bss是否出现过
 输入参数  : hi_u8 *puc_bssid,          bssid信息
 修改历史      :
  1.日    期   : 2015年2月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hmac_scanned_bss_info *hmac_scan_find_scanned_bss_by_bssid(const hmac_bss_mgmt_stru *bss_mgmt, const hi_u8 *puc_bssid)
{
    hi_list *entry = HI_NULL;
    hmac_scanned_bss_info *scanned_bss = HI_NULL;

    /* 遍历链表，查找链表中是否已经存在相同bssid的bss信息 */
    hi_list_for_each(entry, &(bss_mgmt->bss_list_head)) {
        scanned_bss = hi_list_entry(entry, hmac_scanned_bss_info, dlist_head);
        /* 相同的bssid地址 */
        if (0 == oal_compare_mac_addr(scanned_bss->bss_dscr_info.auc_bssid, puc_bssid, WLAN_MAC_ADDR_LEN)) {
            return scanned_bss;
        }
    }

    return HI_NULL;
}

#if defined(_PRE_WLAN_FEATURE_WPA2)
/* ****************************************************************************
 功能描述  : STA 更新从 scan， probe response 帧接收到的AP RSN安全信息
 输入参数  : [1]bss_dscr
             [2]puc_ie
 返 回 值  : static hi_u8
**************************************************************************** */
static hi_u8 hmac_scan_update_bss_list_rsn(mac_bss_dscr_stru *bss_dscr, const hi_u8 *puc_ie)
{
    hi_u8 auc_oui[MAC_OUI_LEN] = {MAC_WLAN_OUI_RSN0, MAC_WLAN_OUI_RSN1, MAC_WLAN_OUI_RSN2};
    hi_u16 suite_temp, us_temp;

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
    /* 规则6.6：禁止使用内存操作类危险函数 例外(1)对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(bss_dscr->bss_sec_info.auc_rsn_pairwise_policy, MAC_PAIRWISE_CIPHER_SUITES_NUM, 0xFF,
        MAC_PAIRWISE_CIPHER_SUITES_NUM);
    memset_s(bss_dscr->bss_sec_info.auc_rsn_auth_policy, MAC_AUTHENTICATION_SUITE_NUM, 0xFF,
        MAC_AUTHENTICATION_SUITE_NUM);

    /* 忽略 RSN IE 和 IE 长度 */
    hi_u8 index = MAC_IE_HDR_LEN;

    /* 获取RSN 版本号 */
    hi_u16 us_ver = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    if (us_ver != MAC_RSN_IE_VERSION) {
        oam_warning_log1(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_rsn::invalid us_ver[%d].}", us_ver);
        return HI_FALSE;
    }

    /* 忽略 RSN 版本号长度 */
    index += 2; /* 2 忽略 RSN 版本号长度 */

    /* 获取组播密钥套件 */
    if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_rsn::invalid RSN group OUI.}");
        return HI_FALSE;
    }
    bss_dscr->bss_sec_info.rsn_grp_policy = puc_ie[index + MAC_OUI_LEN];

    /* 忽略 组播密钥套件 长度 */
    index += 4; /* 4 忽略组播密钥套件长度 */

    /* 获取成对密钥套件 */
    hi_u16 us_suite_count = 0;
    hi_u16 us_pcip_num = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    index += 2; /* 索引自增2 */
    for (suite_temp = 0; suite_temp < us_pcip_num; suite_temp++, index += 4) { /* 4 对于不识别的成对密钥套件，忽略保存 */
        if (memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN) != 0) {
            oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_rsn:invalid RSN paerwise OUI,ignore this ie}");
            /* 对于不识别的成对密钥套件，忽略保存 */
            continue;
        }

        if (us_suite_count >= MAC_PAIRWISE_CIPHER_SUITES_NUM) {
            oam_warning_log1(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_wpa:ignore this ie,pcip_num:%d}", us_pcip_num);
        } else {
            /* 成对密钥套件个数驱动最大为2，超过则不再继续保存 */
            bss_dscr->bss_sec_info.auc_rsn_pairwise_policy[us_suite_count++] = puc_ie[index + MAC_OUI_LEN];
        }
    }

    us_suite_count = 0;
    /* 获取认证套件计数 */
    hi_u16 us_auth_num = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    index += 2; /* 索引自增2 */
    /* 获取认证类型 */
    for (us_temp = 0; us_temp < us_auth_num; us_temp++, index += 4) { /* 4 对于不识别的AKM套件，忽略保存 */
        if (0 != memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN)) {
            oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_rsn::invalid RSN auth OUI,ignore this ie.}");
            /* 对于不识别的AKM套件，忽略保存 */
            continue;
        } else if (us_suite_count < WLAN_AUTHENTICATION_SUITES) {
            /* AKM套件个数驱动最大为2，超过则不再继续保存 */
            bss_dscr->bss_sec_info.auc_rsn_auth_policy[us_suite_count++] = puc_ie[index + MAC_OUI_LEN];
        } else {
            oam_warning_log1(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_wpa:ignore this ie,auth_num:%d}", us_auth_num);
        }
    }

    /* 获取 RSN 能力 */
    bss_dscr->bss_sec_info.auc_rsn_cap[0] = *(puc_ie + index++);
    bss_dscr->bss_sec_info.auc_rsn_cap[1] = *(puc_ie + index++);

    /* 设置 RSNA */
    bss_dscr->bss_sec_info.bss_80211i_mode |= DMAC_RSNA_802_11I;
    return HI_TRUE;
}
#endif

#if defined(_PRE_WLAN_FEATURE_WPA)
/* ****************************************************************************
 功能描述  : STA 更新从 scan， probe response 帧接收到的AP WPA 安全信息
 输入参数  : [1]bss_dscr
             [2]puc_ie
 返回值  : static hi_u8
**************************************************************************** */
static hi_u8 hmac_scan_update_bss_list_wpa(mac_bss_dscr_stru *bss_dscr, const hi_u8 *puc_ie)
{
    hi_u16 us_suite_count = 0;
    hi_u8 auc_oui[MAC_OUI_LEN] = {(hi_u8)MAC_WLAN_OUI_MICRO0, (hi_u8)MAC_WLAN_OUI_MICRO1, (hi_u8)MAC_WLAN_OUI_MICRO2};
    mac_bss_80211i_info_stru *bss_80211i_info = &(bss_dscr->bss_sec_info);
    hi_u16 suite_temp, us_temp;

    /* *********************************************************************** */
    /*                  WPA Element Format                                   */
    /* --------------------------------------------------------------------- */
    /* |Element ID | Length |    WPA OUI    |  Version |  Group Cipher Suite */
    /* --------------------------------------------------------------------- */
    /* |     1     |   1    |        4      |     2    |         4           */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* Pairwise Cipher |  Pairwise Cipher   |                 |              */
    /* Suite Count     |    Suite List      | AKM Suite Count |AKM Suite List */
    /* --------------------------------------------------------------------- */
    /*        2        |          4*m       |         2       |     4*n      */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* 忽略 WPA IE(1 字节) ，IE 长度(1 字节) ，WPA OUI(4 字节)  */
    hi_u8 index = 2 + 4; /* 2 4 忽略 WPA IE(1 字节) ，IE 长度(1 字节) ，WPA OUI(4 字节)  */

    /* 对比WPA 版本信息 */
    if (hi_makeu16(puc_ie[index], puc_ie[index + 1]) != MAC_WPA_IE_VERSION) {
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_wpa::invalid WPA version.}");
        return HI_FALSE;
    }

    /* 忽略 版本号 长度 */
    index += 2; /* 2 忽略 版本号 长度 */

    /* 获取组播密钥套件 */
    if (0 != memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN)) {
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_wpa::invalid WPA group OUI.}");
        return HI_FALSE;
    }
    bss_80211i_info->wpa_grp_policy = puc_ie[index + MAC_OUI_LEN];

    /* 忽略组播密钥套件长度 */
    index += 4; /* 4 忽略组播密钥套件长度 */

    /* 获取成对密钥套件 */
    hi_u16 us_pcip_num = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    index += 2; /* 索引自增2 */
    for (suite_temp = 0; suite_temp < us_pcip_num; suite_temp++) {
        if (0 != memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN)) {
            oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_wpa::invalid WPA pairwise OUI,ignore ie.}");
            /* 对于不识别的成对密钥套件，忽略保存 */
            index += 4; /* 4 对于不识别的成对密钥套件，忽略保存 */
            continue;
        }
        if (us_suite_count < MAC_PAIRWISE_CIPHER_SUITES_NUM) {
            /* 成对密钥套件个数驱动最大为2，超过则不再继续保存 */
            bss_80211i_info->auc_wpa_pairwise_policy[us_suite_count++] = puc_ie[index + MAC_OUI_LEN];
        } else {
            oam_warning_log1(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_wpa::ignore ie,pcip_num:%d.}", us_pcip_num);
        }

        index += 4; /* 索引自增4 */
    }

    /* 获取认证套件计数 */
    hi_u16 us_auth_num = hi_makeu16(puc_ie[index], puc_ie[index + 1]);
    index += 2; /* 索引自增2 */
    /* 获取认证类型 */
    us_suite_count = 0;
    for (us_temp = 0; us_temp < us_auth_num; us_temp++) {
        if (0 != memcmp(auc_oui, puc_ie + index, MAC_OUI_LEN)) {
            oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_wpa::invalid WPA auth OUI,ignore this ie.}");
            /* 对于不识别的AKM套件，忽略保存 */
            index += 4; /* 4 对于不识别的AKM套件，忽略保存 */
            continue;
        } else if (us_suite_count >= WLAN_AUTHENTICATION_SUITES) {
            oam_warning_log1(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_wpa::ignore ie,us_auth_num:%d.}", us_auth_num);
        } else {
            /* AKM套件个数驱动最大为2，超过则不再继续保存 */
            bss_80211i_info->auc_wpa_auth_policy[us_suite_count++] = puc_ie[index + MAC_OUI_LEN];
        }
        index += 4; /* 索引自增4 */
    }

    /* 设置 WPA */
    bss_dscr->bss_sec_info.bss_80211i_mode |= DMAC_WPA_802_11I;

    return HI_TRUE;
}
#endif

#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
/* ****************************************************************************
 功能描述  : STA 更新从 scan， probe response 帧接收到的AP 安全信息
 输入参数  : [1]bss_dscr
             [2]puc_frame_body
             [3]us_frame_len
             [4]us_offset
 返 回 值  : 无
**************************************************************************** */
static hi_void hmac_scan_update_bss_list_security(mac_bss_dscr_stru *bss_dscr, hi_u8 *puc_frame_body,
    hi_u16 us_frame_len, hi_u16 us_offset)
{
    hi_u8 *puc_ie = HI_NULL;

    /* 安全相关信息元素 */
    /* 清空当前 bss_info 结构中的安全信息 */
    if (memset_s(&(bss_dscr->bss_sec_info), sizeof(mac_bss_80211i_info_stru), 0xff,
                 sizeof(mac_bss_80211i_info_stru)) != EOK) {
        return;
    }
    bss_dscr->bss_sec_info.bss_80211i_mode = 0;
    bss_dscr->bss_sec_info.auc_rsn_cap[0] = 0;
    bss_dscr->bss_sec_info.auc_rsn_cap[1] = 0;

#if defined(_PRE_WLAN_FEATURE_WPA2)
    if (us_frame_len > us_offset) {
        puc_ie = mac_find_ie(MAC_EID_RSN, puc_frame_body + us_offset, (us_frame_len - us_offset));
        if (puc_ie != HI_NULL) {
            /* 更新从beacon 中收到的 RSN 安全相关信息到 pst_bss_dscr 中 */
            hmac_scan_update_bss_list_rsn(bss_dscr, puc_ie);
        }
    }
#endif

#if defined(_PRE_WLAN_FEATURE_WPA)
    puc_ie = mac_find_vendor_ie(MAC_WLAN_OUI_MICROSOFT, MAC_OUITYPE_WPA, puc_frame_body + us_offset,
        (hi_s32)(us_frame_len - us_offset));
    if (puc_ie != HI_NULL) {
        /* 更新从beacon 中收到的 WPA 安全相关信息到 pst_bss_dscr 中 */
        hmac_scan_update_bss_list_wpa(bss_dscr, puc_ie);
    }
#endif
}
#endif /* defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2) */

/* ****************************************************************************
 功能描述  : 更新wmm相关信息
 修改历史      :
  1.日    期   : 2013年10月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_scan_update_bss_list_wmm(mac_bss_dscr_stru *bss_dscr, hi_u8 *puc_frame_body, hi_u16 us_frame_len)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 offset;

    offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    bss_dscr->wmm_cap = HI_FALSE;
    bss_dscr->uapsd_cap = HI_FALSE;

    puc_ie = mac_get_wmm_ie(puc_frame_body, us_frame_len, offset);
    if (puc_ie != HI_NULL) {
        bss_dscr->wmm_cap = HI_TRUE;

        /* Check if Bit 7 is set indicating U-APSD capability */
        if (puc_ie[8] & BIT7) { /* wmm ie的第8个字节是QoS info字节 */
            bss_dscr->uapsd_cap = HI_TRUE;
        }
    } else {
        if (us_frame_len > offset) {
            puc_ie = mac_find_ie(MAC_EID_HT_CAP, puc_frame_body + offset, us_frame_len - offset);
            if (puc_ie != HI_NULL) {
                bss_dscr->wmm_cap = HI_TRUE;
            }
        }
    }
}

#ifdef _PRE_WLAN_FEATURE_11D
/* ****************************************************************************
 功能描述  : 解析country IE
 修改历史      :
  1.日    期   : 2013年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_scan_update_bss_list_country(mac_bss_dscr_stru *bss_dscr, hi_u8 *puc_frame_body,
    hi_u16 us_frame_len)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 offset;

    offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    /* 国家码默认标记为0 */
    bss_dscr->ac_country[0] = 0;
    bss_dscr->ac_country[1] = 0;
    bss_dscr->ac_country[2] = 0; /* 第2个字节 */

    if (us_frame_len > offset) {
        puc_ie = mac_find_ie(MAC_EID_COUNTRY, puc_frame_body + offset, us_frame_len - offset);
        if (puc_ie != HI_NULL) {
            bss_dscr->ac_country[0] = (hi_s8)puc_ie[MAC_IE_HDR_LEN];
            bss_dscr->ac_country[1] = (hi_s8)puc_ie[MAC_IE_HDR_LEN + 1];
            bss_dscr->ac_country[2] = 0; /* 第2个字节 */
        }
    }
}
#endif

/* ****************************************************************************
 功能描述  : 更新11n相关信息
 修改历史      :
  1.日    期   : 2013年10月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_scan_update_bss_list_11n(mac_bss_dscr_stru *bss_dscr, hi_u8 *puc_frame_body, hi_u16 us_frame_len,
    hi_u16 us_offset)
{
    hi_u8 *puc_ie = HI_NULL;
    mac_ht_opern_stru *ht_op = HI_NULL;
    hi_u8 sec_chan_offset;
    wlan_bw_cap_enum_uint8 ht_cap_bw = WLAN_BW_CAP_20M;
    wlan_bw_cap_enum_uint8 ht_op_bw = WLAN_BW_CAP_20M;

    /* 11n */
    if (us_frame_len > us_offset) {
        puc_ie = mac_find_ie(MAC_EID_HT_CAP, puc_frame_body + us_offset, us_frame_len - us_offset);
        if ((puc_ie != HI_NULL) && (puc_ie[1] >= 2)) { /* 增加ie长度异常检查 2: 与2比较 */
            /* puc_ie[2]是HT Capabilities Info的第1个字节 */
            bss_dscr->ht_capable = HI_TRUE;                /* 支持ht */
            bss_dscr->ht_ldpc = (puc_ie[2] & BIT0);        /* 支持ldpc 2: 数组下标 */
            ht_cap_bw = ((puc_ie[2] & BIT1) >> 1);         /* 取出支持的带宽 2: 数组下标 */
            bss_dscr->ht_stbc = ((puc_ie[2] & BIT7) >> 7); /* 支持stbc 2: 数组下标,右移7位 */
        }
    }

    /* 默认20M,如果帧内容未携带HT_OPERATION则可以直接采用默认值 */
    bss_dscr->channel_bandwidth = WLAN_BAND_WIDTH_20M;

    if (us_frame_len > us_offset) {
        puc_ie = mac_find_ie(MAC_EID_HT_OPERATION, puc_frame_body + us_offset, us_frame_len - us_offset);
    }
    if ((puc_ie != HI_NULL) && (puc_ie[1] >= 2)) { /* 增加ie长度异常检查 2: 与2比较 */
        ht_op = (mac_ht_opern_stru *)(puc_ie + MAC_IE_HDR_LEN);

        /* 提取次信道偏移 */
        sec_chan_offset = ht_op->secondary_chan_offset;

        /* 防止ap的channel width=0, 但channel offset = 1或者3 此时以channel width为主 */
        /* ht cap 20/40 enabled && ht operation 40 enabled */
        if ((ht_op->sta_chan_width != 0) && (ht_cap_bw > WLAN_BW_CAP_20M)) { /* cap > 20M才取channel bw */
            if (sec_chan_offset == MAC_SCB) {
                bss_dscr->channel_bandwidth = WLAN_BAND_WIDTH_40MINUS;
                ht_op_bw = WLAN_BW_CAP_40M;
            } else if (sec_chan_offset == MAC_SCA) {
                bss_dscr->channel_bandwidth = WLAN_BAND_WIDTH_40PLUS;
                ht_op_bw = WLAN_BW_CAP_40M;
            }
        }
    }

    /* 将AP带宽能力取声明能力的最小值，防止AP异常发送超过带宽能力数据，造成数据不通 */
    bss_dscr->bw_cap = oal_min(ht_cap_bw, ht_op_bw);

    if (us_frame_len > us_offset) {
        puc_ie = mac_find_ie(MAC_EID_EXT_CAPS, puc_frame_body + us_offset, us_frame_len - us_offset);
        if ((puc_ie != HI_NULL) && (puc_ie[1] >= 1)) {
            /* Extract 20/40 BSS Coexistence Management Support */
            bss_dscr->coex_mgmt_supp = (puc_ie[2] & BIT0);
        }
    }
}

/* ****************************************************************************
 功能描述  : 更新协议类 bss信息
 修改历史      :
  1.日    期   : 2013年6月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2013年8月21日
    作    者   : HiSilicon
    修改内容   : 补充11i 加密信息
**************************************************************************** */
static hi_void hmac_scan_update_bss_list_protocol(mac_bss_dscr_stru *bss_dscr, hi_u8 *puc_frame_body,
    hi_u16 us_frame_len)
{
    hi_u16 us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    /* *********************************************************************** */
    /*                       Beacon Frame - Frame Body                       */
    /* ---------------------------------------------------------------------- */
    /* |Timestamp|BcnInt|CapInfo|SSID|SupRates|DSParamSet|TIM  |CountryElem | */
    /* ---------------------------------------------------------------------- */
    /* |8        |2     |2      |2-34|3-10    |3         |6-256|8-256       | */
    /* ---------------------------------------------------------------------- */
    /* |PowerConstraint |Quiet|TPC Report|ERP |RSN  |WMM |Extended Sup Rates| */
    /* ---------------------------------------------------------------------- */
    /* |3               |8    |4         |3   |4-255|26  | 3-257            | */
    /* ---------------------------------------------------------------------- */
    /* |BSS Load |HT Capabilities |HT Operation |Overlapping BSS Scan       | */
    /* ---------------------------------------------------------------------- */
    /* |7        |28              |24           |16                         | */
    /* ---------------------------------------------------------------------- */
    /* |Extended Capabilities |                                              */
    /* ---------------------------------------------------------------------- */
    /* |3-8                   |                                              */
    /* *********************************************************************** */
    /* wmm */
    hmac_scan_update_bss_list_wmm(bss_dscr, puc_frame_body, us_frame_len);

#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
    /* 11i */
    hmac_scan_update_bss_list_security(bss_dscr, puc_frame_body, us_frame_len, us_offset);
#endif
#ifdef _PRE_WLAN_FEATURE_11D
    /* 11d */
    hmac_scan_update_bss_list_country(bss_dscr, puc_frame_body, us_frame_len);
#endif
    /* 11n */
    hmac_scan_update_bss_list_11n(bss_dscr, puc_frame_body, us_frame_len, us_offset);
}

/* ****************************************************************************
 功能描述  : 检查速率
 修改历史      :
  1.日    期   : 2016年04月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_scan_check_bss_supp_rates(mac_device_stru *mac_dev, const hi_u8 *puc_rate, hi_u8 bss_rate_num,
    hi_u8 *puc_update_rate, hi_u8 rate_len)
{
    mac_data_rate_stru *rates = HI_NULL;
    hi_u32 i, j;
    hi_u8 rate_num = 0;

    rates = &mac_dev->mac_rates_11g[0];

    if (puc_rate == HI_NULL) {
        return rate_num;
    }

    for (i = 0; i < bss_rate_num; i++) {
        for (j = 0; j < rate_len; j++) {
            if (((rates[j].mac_rate & 0x7f) == (puc_rate[i] & 0x7f)) && (rate_num < MAC_DATARATES_PHY_80211G_NUM)) {
                puc_update_rate[rate_num] = puc_rate[i];
                rate_num++;
                break;
            }
        }
    }

    return rate_num;
}

/* ****************************************************************************
 功能描述  : 更新扫描到bss的速率集
 修改历史      :
  1.日    期   : 2013年7月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_scan_update_bss_list_rates(mac_bss_dscr_stru *bss_dscr, hi_u8 *puc_frame_body, hi_u16 us_frame_len,
    mac_device_stru *mac_dev)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u8 num_rates = 0;
    hi_u8 num_ex_rates;
    hi_u8 us_offset;
    hi_u8 auc_rates[MAC_DATARATES_PHY_80211G_NUM] = {0};

    /* 设置Beacon帧的field偏移量 */
    us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    if (us_frame_len <= us_offset) {
        oam_warning_log1(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_rates::frame_len[%d].}", us_frame_len);
        return HI_FAIL;
    }

    puc_ie = mac_find_ie(MAC_EID_RATES, puc_frame_body + us_offset, us_frame_len - us_offset);
    if (puc_ie != HI_NULL) {
        num_rates = hmac_scan_check_bss_supp_rates(mac_dev, puc_ie + MAC_IE_HDR_LEN, puc_ie[1], auc_rates,
            MAC_DATARATES_PHY_80211G_NUM);
        if (num_rates > WLAN_MAX_SUPP_RATES) {
            oam_warning_log1(0, OAM_SF_SCAN, "{hmac_scan_update_bss_list_rates::uc_num_rates=%d.}", num_rates);
            num_rates = WLAN_MAX_SUPP_RATES;
        }

        if (memcpy_s(bss_dscr->auc_supp_rates, WLAN_MAX_SUPP_RATES, auc_rates, num_rates) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "hmac_scan_update_bss_list_rates:: auc_rates memcpy_s fail.");
            return HI_FAIL;
        }

        bss_dscr->num_supp_rates = num_rates;
    }
    puc_ie = mac_find_ie(MAC_EID_XRATES, puc_frame_body + us_offset, us_frame_len - us_offset);
    if (puc_ie != HI_NULL) {
        num_ex_rates = hmac_scan_check_bss_supp_rates(mac_dev, puc_ie + MAC_IE_HDR_LEN, puc_ie[1], auc_rates,
            MAC_DATARATES_PHY_80211G_NUM);
        if (num_rates + num_ex_rates > WLAN_MAX_SUPP_RATES) { /* 超出支持速率个数 */
            oam_warning_log2(0, OAM_SF_SCAN,
                "{hmac_scan_update_bss_list_rates::number of rates too large, num_rates=%d, num_ex_rates=%d.}",
                num_rates, num_ex_rates);

            num_ex_rates = WLAN_MAX_SUPP_RATES - num_rates;
        }

        if (num_ex_rates > 0) {
            if (memcpy_s(&(bss_dscr->auc_supp_rates[num_rates]), WLAN_MAX_SUPP_RATES, auc_rates,
                         num_ex_rates) != EOK) {
                oam_error_log0(0, OAM_SF_CFG, "hmac_scan_update_bss_list_rates:: auc_rates memcpy_s fail.");
                return HI_FAIL;
            }
        }
        bss_dscr->num_supp_rates += num_ex_rates;
    }

    return HI_SUCCESS;
}

static hi_u32 hmac_scan_update_bss_ssid(mac_bss_dscr_stru *bss_dscr, hmac_scanned_bss_info *scanned_bss,
    hi_u8 *puc_frame_body, hi_u16 us_frame_body_len)
{
    hi_unref_param(scanned_bss);
    hi_u8 ssid_len;
    /* 解析并保存ssid */
    hi_u8 *puc_ssid = mac_get_ssid(puc_frame_body, (hi_s32)us_frame_body_len, &ssid_len);
    if ((puc_ssid != HI_NULL) && (ssid_len != 0)) {
        /* 将查找到的ssid保存到bss描述结构体中 */
        if (memcpy_s(bss_dscr->ac_ssid, WLAN_SSID_MAX_LEN, puc_ssid, ssid_len) != EOK) {
            oam_warning_log1(0, OAM_SF_SCAN, "hmac_scan_update_bss_ssid:memcpy_s fail, ssid=[%p]", (uintptr_t)puc_ssid);
            return HI_FAIL;
        }
        bss_dscr->ac_ssid[ssid_len] = '\0';
#ifdef _PRE_WLAN_FEATURE_MESH
    } else {
        /* 同WPA，Mesh的beacon和probe rsp会将ssid回复在私有meshid字段中，获取出来将meshid更新填充到ssid中 */
        /* mac_get_meshid()函数返回值类型就是hi_u8*,误报lin_t64告警，申请屏蔽 */
        puc_ssid = mac_get_meshid(puc_frame_body, (hi_s32)us_frame_body_len, &ssid_len);
        /* 将查找到的ssid保存到bss描述结构体中 */
        if (puc_ssid != HI_NULL) {
            if (memcpy_s(bss_dscr->ac_ssid, WLAN_SSID_MAX_LEN, puc_ssid, ssid_len) != EOK) {
                oam_warning_log1(0, OAM_SF_SCAN, "hmac_scan_update_bss_ssid:memcpy_s fail, ssid=[%p]",
                    (uintptr_t)puc_ssid);
                return HI_FAIL;
            }
        } else {
            ssid_len = 0;
        }
        bss_dscr->ac_ssid[ssid_len] = '\0';
#endif
    }
#ifdef _PRE_WLAN_FEATURE_SCAN_BY_SSID
    /* 检查本次扫描请求是否为指定ssid扫描，判断是否需要驱动过滤非指定ssid的扫描信息 */
    hmac_scan_proc_check_ssid(scanned_bss, puc_ssid, ssid_len);
#endif
    return HI_SUCCESS;
}

static hi_u32 hmac_scan_update_bss_bssid(mac_bss_dscr_stru *bss_dscr, const mac_ieee80211_frame_stru *frame_header)
{
    if (memcpy_s(bss_dscr->auc_mac_addr, WLAN_MAC_ADDR_LEN, frame_header->auc_address2, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_scan_update_bss_bssid::mem safe function err!}");
        return HI_FAIL;
    }
    if (memcpy_s(bss_dscr->auc_bssid, WLAN_MAC_ADDR_LEN, frame_header->auc_address3, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{hmac_scan_update_bss_bssid::mem safe function err!}");
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

static hi_void hmac_scan_update_bss_base(hmac_vap_stru *hmac_vap, const dmac_tx_event_stru *dtx_event,
    mac_bss_dscr_stru *bss_dscr, hi_u8 frame_channel)
{
    hi_unref_param(hmac_vap);

    oal_netbuf_stru *netbuf = dtx_event->netbuf;
    mac_scanned_result_extend_info_stru *scan_result_extend_info = HI_NULL;
    mac_device_stru *mac_dev = mac_res_get_dev();
    hi_u16 us_netbuf_len = (hi_u16)(dtx_event->us_frame_len + MAC_80211_FRAME_LEN);
    /* 获取device上报的扫描结果信息，并将其更新到bss描述结构体中 */
    hi_u16 us_frame_len = us_netbuf_len - sizeof(mac_scanned_result_extend_info_stru);
    hi_u8 *puc_mgmt_frame = (hi_u8 *)oal_netbuf_data(netbuf);
    /* 指向netbuf中的上报的扫描结果的扩展信息的位置 */
    scan_result_extend_info = (mac_scanned_result_extend_info_stru *)(puc_mgmt_frame + us_frame_len);
    /* 获取管理帧的帧头和帧体指针 */
    mac_ieee80211_frame_stru *frame_header = (mac_ieee80211_frame_stru *)puc_mgmt_frame;
    hi_u8 *puc_frame_body = (hi_u8 *)(puc_mgmt_frame + MAC_80211_FRAME_LEN);
    hi_u16 us_frame_body_len = us_frame_len - MAC_80211_FRAME_LEN;

    /* bss基本信息 */
    bss_dscr->bss_type = scan_result_extend_info->bss_type;

    bss_dscr->us_cap_info = *((hi_u16 *)(puc_frame_body + MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN));

    bss_dscr->rssi = (hi_s8)scan_result_extend_info->l_rssi;

    /* 解析beacon周期 */
    bss_dscr->us_beacon_period = mac_get_beacon_period(puc_frame_body);

    /* 解析 TIM 周期，仅 Beacon 帧有该元素 */
    if (frame_header->frame_control.sub_type == WLAN_BEACON) {
        bss_dscr->dtim_period = mac_get_dtim_period(puc_frame_body, us_frame_body_len);
        bss_dscr->dtim_cnt = mac_get_dtim_cnt(puc_frame_body, us_frame_body_len);
    }

    /* 信道 */
    bss_dscr->channel.chan_number = frame_channel;
    bss_dscr->channel.band = mac_get_band_by_channel_num(frame_channel);

    /* 记录速率集 */
    if (hmac_scan_update_bss_list_rates(bss_dscr, puc_frame_body, us_frame_body_len, mac_dev) != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_SCAN, "hmac_scan_update_bss_list_rates return NON SUCCESS. ");
    }

    /* 协议类相关信息元素的获取 */
    hmac_scan_update_bss_list_protocol(bss_dscr, puc_frame_body, us_frame_body_len);

#ifdef _PRE_WLAN_FEATURE_MESH
    /* Probe Rsp和Beacon帧中前面为Timestamp,beacon interval,capability字段，非tlv结构，不能直接用于mac_find_ie函数，
    此处加上偏移，以Element ID为0的SSID做为起始地址查找指定IE */
    hi_u8 *puc_frame_ie_body = puc_frame_body + MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        /* 新增Mesh Configuration Element解析获取Accepting Peer字段值 */
        if (us_frame_body_len > MAC_SSID_OFFSET) {
            mac_mesh_conf_ie_stru *puc_mesh_conf_ie = (mac_mesh_conf_ie_stru *)mac_find_ie(MAC_EID_MESH_CONF,
                puc_frame_ie_body, us_frame_body_len - MAC_SSID_OFFSET);
            if (puc_mesh_conf_ie != HI_NULL) {
                bss_dscr->is_mesh_accepting_peer =
                    (puc_mesh_conf_ie->mesh_capa.accepting_add_mesh_peerings == 1) ? HI_TRUE : HI_FALSE;
            }
        } else {
            bss_dscr->is_mesh_accepting_peer = HI_FALSE;
        }
    }
    /* 查找Hisi-Mesh私有IE字段,过滤使用 */
    bss_dscr->is_hisi_mesh = mac_check_is_mesh_vap(puc_frame_ie_body, (hi_u8)(us_frame_body_len - MAC_SSID_OFFSET));
    if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        oam_info_log4(0, OAM_SF_SCAN, "hmac_scan_update_bss_base:mac address: 0x%x::0x%x, peer = %d, mesh = %d",
            bss_dscr->auc_mac_addr[4], bss_dscr->auc_mac_addr[5], /* 4 5 元素索引 */
            bss_dscr->is_mesh_accepting_peer, bss_dscr->is_hisi_mesh);
    }
#endif
}

static hi_void hmac_scan_update_bss_any(mac_bss_dscr_stru *bss_dscr, hi_u8 *puc_frame_body, hi_u16 us_frame_body_len)
{
#ifdef _PRE_WLAN_FEATURE_ANY
    /* Probe Rsp和Beacon帧中前面为Timestamp,beacon interval,capability字段，非tlv结构，不能直接用于mac_find_ie函数，
    此处加上偏移，以Element ID为0的SSID做为起始地址查找指定IE */
    hi_u8 *puc_frame_ie_start = puc_frame_body + MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    /* 查找对应的ANY IE */
    hi_u8 *puc_any_ie = mac_find_vendor_ie(MAC_WLAN_OUI_VENDOR, MAC_OUITYPE_ANY, puc_frame_ie_start,
        us_frame_body_len - MAC_SSID_OFFSET);
    if ((puc_any_ie != HI_NULL) && ((puc_any_ie[6] == MAC_ANY_STA_TYPE) || /* 6元素索引 */
        (puc_any_ie[6] == MAC_ANY_AP_TYPE))) {                             /* 6元素索引 */
        bss_dscr->supp_any = HI_TRUE;
        bss_dscr->is_any_sta = HI_FALSE;
        if (puc_any_ie[6] == MAC_ANY_STA_TYPE) { /* 6 元素索引 */
            bss_dscr->is_any_sta = HI_TRUE;
        }
    }
#else
    hi_unref_param(bss_dscr);
    hi_unref_param(puc_frame_body);
    hi_unref_param(us_frame_body_len);
#endif
    return;
}

/* ****************************************************************************
 功能描述  : 更新描述扫描结构的bss dscr结构体
 输入参数  : hmac_scanned_bss_info   *pst_scanned_bss,
             dmac_tx_event_stru      *pst_dtx_event,
             hi_u8                uc_vap_id
 修改历史      :
  1.日    期   : 2015年2月2日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_scan_update_bss_dscr(hmac_scanned_bss_info *scanned_bss, const dmac_tx_event_stru *dtx_event, hi_u8 vap_id)
{
    oal_netbuf_stru *netbuf = dtx_event->netbuf;
    mac_scanned_result_extend_info_stru *scan_result_extend_info = HI_NULL;
    hi_u16 us_netbuf_len = (hi_u16)(dtx_event->us_frame_len + MAC_80211_FRAME_LEN);
    hi_u16 us_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    /* 获取hmac vap */
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_update_bss_dscr::pst_hmac_vap is null.}");
        return HI_FAIL;
    }

    /* 获取device上报的扫描结果信息，并将其更新到bss描述结构体中 */
    hi_u16 us_frame_len = us_netbuf_len - sizeof(mac_scanned_result_extend_info_stru);
    hi_u8 *puc_mgmt_frame = (hi_u8 *)oal_netbuf_data(netbuf);
    if (puc_mgmt_frame == HI_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_scan_update_bss_dscr:: puc_mgmt_frame fail.");
        return HI_FAIL;
    }

    /* 指向netbuf中的上报的扫描结果的扩展信息的位置 */
    scan_result_extend_info = (mac_scanned_result_extend_info_stru *)(puc_mgmt_frame + us_frame_len);
    /* 获取管理帧的帧头和帧体指针 */
    mac_ieee80211_frame_stru *frame_header = (mac_ieee80211_frame_stru *)puc_mgmt_frame;
    hi_u8 *puc_frame_body = (hi_u8 *)(puc_mgmt_frame + MAC_80211_FRAME_LEN);
    hi_u16 us_frame_body_len = us_frame_len - MAC_80211_FRAME_LEN;

    /* 获取管理帧中的信道 */
    hi_u8 frame_channel = mac_ie_get_chan_num(puc_frame_body, us_frame_body_len, us_offset, 0);
    /* 如果信道非法，直接返回 */
    if (frame_channel == 0) {
        oam_info_log0(0, OAM_SF_SCAN, "hmac_scan_update_bss_dscr:: Received a frame from unregulated domain.");
        return HI_FAIL;
    }

    /* 如果收到的帧信道和当前扫描信道不一致，则检查帧信道是否在扫描信道列表中, 若不在则过滤 */
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    hmac_scan_stru *scan_mgmt = &(hmac_dev->scan_mgmt);
    if ((frame_channel != scan_result_extend_info->channel) &&
        (scan_mgmt->scan_2g_ch_list_map & (BIT0 << frame_channel)) == HI_FALSE) {
        scanned_bss->bss_dscr_info.need_drop = HI_TRUE;
    }

    /* 更新bss信息 */
    mac_bss_dscr_stru *bss_dscr = &(scanned_bss->bss_dscr_info);

    /* ****************************************************************************
        解析beacon/probe rsp帧，记录到pst_bss_dscr
    **************************************************************************** */
    /* 解析并保存ssid 和 bssid */
    if ((hmac_scan_update_bss_ssid(bss_dscr, scanned_bss, puc_frame_body, us_frame_body_len) != HI_SUCCESS) ||
        (hmac_scan_update_bss_bssid(bss_dscr, frame_header) != HI_SUCCESS)) {
        return HI_FAIL;
    }

    /* bss基本信息 */
    hmac_scan_update_bss_base(hmac_vap, dtx_event, bss_dscr, frame_channel);

    /* 更新时间戳 */
    bss_dscr->timestamp = (hi_u32)hi_get_milli_seconds();
    bss_dscr->mgmt_len = us_frame_len;
    /* 更新any相关信息 */
    hmac_scan_update_bss_any(bss_dscr, puc_frame_body, us_frame_body_len);

    /* 拷贝管理帧内容 */
    if (memcpy_s((hi_u8 *)scanned_bss->bss_dscr_info.auc_mgmt_buff, (hi_u32)us_frame_len, puc_mgmt_frame,
        (hi_u32)us_frame_len) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_scan_update_bss_dscr:: puc_mgmt_frame memcpy_s fail.");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

static hi_u32 hmac_scan_proc_scanned_bss_mgmt(hmac_device_stru *hmac_dev, hmac_scanned_bss_info *new_scanned_bss,
    oal_netbuf_stru *bss_mgmt_netbuf)
{
    /* 获取管理扫描的bss结果的结构体 */
    hmac_bss_mgmt_stru *bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt);
    /* 对链表删操作前加锁 */
    oal_spin_lock(&(bss_mgmt->st_lock));

    /* 判断相同bssid的bss是否已经扫描到 */
    hmac_scanned_bss_info *old_scanned_bss =
        hmac_scan_find_scanned_bss_by_bssid(bss_mgmt, new_scanned_bss->bss_dscr_info.auc_bssid);
    if (old_scanned_bss == HI_NULL) {
        /* 解锁 */
        oal_spin_unlock(&(bss_mgmt->st_lock));

        goto add_bss; /* goto语句使用，lin_t e801告警屏蔽 */
    }

    /* 如果老的扫描的bss的信号强度大于当前扫描到的bss的信号强度，更新当前扫描到的信号强度为最强的信号强度 */
    if (old_scanned_bss->bss_dscr_info.rssi > new_scanned_bss->bss_dscr_info.rssi) {
        /* 1s中以内就采用之前的BSS保存的RSSI信息，否则就采用新的RSSI信息 */
        if ((hi_get_milli_seconds() - old_scanned_bss->bss_dscr_info.timestamp) < HMAC_SCAN_MAX_SCANNED_RSSI_EXPIRE) {
            new_scanned_bss->bss_dscr_info.rssi = old_scanned_bss->bss_dscr_info.rssi;
        }
    }

    if ((new_scanned_bss->bss_dscr_info.ac_ssid[0] == '\0') && (old_scanned_bss->bss_dscr_info.ac_ssid[0] != '\0')) {
        /* 隐藏SSID，如果保存过此AP信息,且ssid不为空，此次通过BEACON帧扫描到此AP信息,且SSID为空，则不进行更新 */
        oam_warning_log3(0, OAM_SF_SCAN, "{hmac_scan_proc_scanned_bss::ssid:%.2x:%.2x:%.2x}",
            new_scanned_bss->bss_dscr_info.auc_bssid[3], new_scanned_bss->bss_dscr_info.auc_bssid[4], /* 3 4 元素索引 */
            new_scanned_bss->bss_dscr_info.auc_bssid[5]); /* 5 元素索引 */

        old_scanned_bss->bss_dscr_info.timestamp = (hi_u32)hi_get_milli_seconds();
        old_scanned_bss->bss_dscr_info.rssi = new_scanned_bss->bss_dscr_info.rssi;

        /* 解锁 */
        oal_spin_unlock(&(bss_mgmt->st_lock));

        /* 释放申请的存储bss信息的内存 */
        oal_free(new_scanned_bss);

        /* 释放上报的bss信息和beacon或者probe rsp帧的内存 */
        oal_netbuf_free(bss_mgmt_netbuf);

        return HI_SUCCESS;
    }

    /* 从链表中将原先扫描到的相同bssid的bss节点删除 */
    hmac_scan_del_bss_from_list_nolock(old_scanned_bss, hmac_dev);
    /* 解锁 */
    oal_spin_unlock(&(bss_mgmt->st_lock));

    /* 释放内存 */
    oal_free(old_scanned_bss);

add_bss: /* lint_t e801告警屏蔽 */
#ifdef _PRE_WLAN_FEATURE_QUICK_START
    hi_s8 *pc_ssid = new_scanned_bss->bss_dscr_info.ac_ssid;
    databk_quick_start_stru *quick_start_param = hisi_get_quick_start_param();
    if ((quick_start_param->ssid_len != 0) && (quick_start_param->ssid_len == strlen(pc_ssid)) &&
        (SSID_MAX_LEN >= strlen(pc_ssid)) && (memcmp(quick_start_param->auc_ssid, pc_ssid, strlen(pc_ssid)) == 0)) {
        hi_u16 bsslen = sizeof(hmac_scanned_bss_info) + mgmt_len - sizeof(new_scanned_bss->bss_dscr_info.auc_mgmt_buff);
        /* 备份扫描结果 */
        if (memcpy_s(quick_start_param->auc_bss_frame, bsslen, new_scanned_bss, bsslen) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "hmac_scan_proc_scanned_bss:: pst_new_scanned_bss memcpy_s fail.");
            oal_free(new_scanned_bss);
            return HI_FAIL;
        }
        quick_start_param->us_bss_frame_len = mgmt_len;
        quick_start_param->uc_update_flag = PARAM_NEED_UPDATE;
    }
#endif

    /* 将扫描结果添加到链表中 */
    hmac_scan_add_bss_to_list(new_scanned_bss, hmac_dev);

    /* 释放上报的bss信息和beacon或者probe rsp帧的内存 */
    oal_netbuf_free(bss_mgmt_netbuf);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 接收每个信道的扫描结果到host侧进行处理
 修改历史      :
  1.日    期   : 2015年2月7日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_scan_proc_scanned_bss(frw_event_mem_stru *event_mem)
{
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_scanned_bss::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取事件头和事件结构体指针 */
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_stru *event_hdr = &(event->event_hdr);
    dmac_tx_event_stru *dtx_event = (dmac_tx_event_stru *)event->auc_event_data;
    oal_netbuf_stru *bss_mgmt_netbuf = dtx_event->netbuf;

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event_hdr->vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_scanned_bss::pst_hmac_vap null.}");

        /* 释放上报的bss信息和beacon或者probe rsp帧的内存 */
        oal_netbuf_free(bss_mgmt_netbuf);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取hmac device 结构 */
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    /* 对dmac上报的netbuf内容进行解析，内容如下所示 */
    /* ********************************************************************************************* */
    /*            netbuf data域的上报的扫描结果的字段的分布                                        */
    /* ------------------------------------------------------------------------------------------  */
    /* beacon/probe rsp body  |     帧体后面附加字段(mac_scanned_result_extend_info_stru)          */
    /* -----------------------------------------------------------------------------------------   */
    /* 收到的beacon/rsp的body | rssi(4字节) | channel num(1字节)| band(1字节)|bss_tye(1字节)|填充  */
    /* ------------------------------------------------------------------------------------------  */
    /*                                                                                             */
    /* ********************************************************************************************* */
    /* 管理帧的长度等于上报的netbuf的长度减去上报的扫描结果的扩展字段的长度 */
    hi_u16 us_mgmt_len =
        (hi_u16)(dtx_event->us_frame_len + MAC_80211_FRAME_LEN - sizeof(mac_scanned_result_extend_info_stru));

    /* 申请存储扫描结果的内存 */
    hmac_scanned_bss_info *new_scanned_bss = hmac_scan_alloc_scanned_bss(us_mgmt_len);
    if (oal_unlikely(new_scanned_bss == HI_NULL)) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_SCAN, "{hmac_scan_proc_scanned_bss::alloc memory failed }");

        /* 释放上报的bss信息和beacon或者probe rsp帧的内存 */
        oal_netbuf_free(bss_mgmt_netbuf);
        return HI_FAIL;
    }

    /* 更新描述扫描结果的bss dscr结构体 */
    if (oal_unlikely(hmac_scan_update_bss_dscr(new_scanned_bss, dtx_event, event_hdr->vap_id) != HI_SUCCESS)) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_SCAN, "{hmac_scan_proc_scanned_bss::hmac_scan_update fail}");

        /* 释放上报的bss信息和beacon或者probe rsp帧的内存 */
        oal_netbuf_free(bss_mgmt_netbuf);

        /* 释放申请的存储bss信息的内存 */
        oal_free(new_scanned_bss);
        return HI_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    /* MESH VAP过滤非MESH VAP发出的Beacon/Probe Rsp
       过滤规则:1.en_is_hisi_mesh为FALSE 2.en_is_mesh_accepting_peer为FALSE,
       如果扫描是ANY下发的扫描，则这里不过滤 */
    if ((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH) &&
        (((new_scanned_bss->bss_dscr_info.is_hisi_mesh == HI_FALSE) ||
        (new_scanned_bss->bss_dscr_info.is_mesh_accepting_peer == HI_FALSE)) &&
        (hmac_dev->scan_mgmt.scan_record_mgmt.is_any_scan == HI_FALSE))) {
        /* 释放上报的bss信息和beacon或者probe rsp帧的内存 */
        oal_netbuf_free(bss_mgmt_netbuf);

        /* 释放申请的存储bss信息的内存 */
        oal_free(new_scanned_bss);

        return HI_SUCCESS;
    }
#endif

    /* 如果之前的判断需要过滤 */
    if (new_scanned_bss->bss_dscr_info.need_drop == HI_TRUE) {
        /* 释放上报的bss信息和beacon或者probe rsp帧的内存 */
        oal_netbuf_free(bss_mgmt_netbuf);

        /* 释放申请的存储bss信息的内存 */
        oal_free(new_scanned_bss);

        return HI_SUCCESS;
    }

    return hmac_scan_proc_scanned_bss_mgmt(hmac_dev, new_scanned_bss, bss_mgmt_netbuf);
}

static hi_void hmac_scan_proc_scan_comp_event_vap(const mac_device_stru *mac_dev, hmac_scan_stru *scan_mgmt,
    hmac_vap_stru *hmac_vap)
{
    if ((mac_is_dbac_running(mac_dev) != HI_TRUE) || (mac_dev->dbac_same_ch == HI_TRUE)) {
        /* 根据当前扫描的类型和当前vap的状态，决定切换vap的状态，如果是前景扫描，才需要切换vap的状态 */
        if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
            if (hmac_vap->base_vap->vap_state == MAC_VAP_STATE_STA_WAIT_SCAN) {
                /* 改变vap状态到SCAN_COMP */
                hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_SCAN_COMP);
            } else if (hmac_vap->base_vap->vap_state == MAC_VAP_STATE_UP) {
                /* 背景扫描时需要进行帧过滤的配置 */
                hmac_set_rx_filter_value(hmac_vap->base_vap);
            }
        }

        if (((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) ||
            (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH)) &&
            (scan_mgmt->scan_record_mgmt.vap_last_state != MAC_VAP_STATE_BUTT)) {
            hmac_fsm_change_state(hmac_vap, scan_mgmt->scan_record_mgmt.vap_last_state);
            scan_mgmt->scan_record_mgmt.vap_last_state = MAC_VAP_STATE_BUTT;
        }
    }
}

/* ****************************************************************************
 功能描述  : DMAC扫描完成事件处理
 修改历史      :
  1.日    期   : 2013年6月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_scan_proc_scan_comp_event(frw_event_mem_stru *event_mem)
{
    /* 获取事件头和事件结构体指针 */
    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_stru *event_hdr = &(event->event_hdr);

    /* 获取hmac device */
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    mac_device_stru *mac_dev = mac_res_get_dev();
    mac_scan_rsp_stru *d2h_scan_rsp_info = (mac_scan_rsp_stru *)(event->auc_event_data);
    hmac_scan_stru *scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 如果下发的是ANY扫描，结束扫描的时候设置为非ANY扫描，下发的若非ANY扫描，这里赋该值无影响 */
    hmac_dev->scan_mgmt.scan_record_mgmt.is_any_scan = HI_FALSE;

    if ((event_hdr->vap_id != scan_mgmt->scan_record_mgmt.vap_id) ||
        (d2h_scan_rsp_info->ull_cookie != scan_mgmt->scan_record_mgmt.ull_cookie)) {
        oam_warning_log4(event_hdr->vap_id, OAM_SF_SCAN,
            "{hmac_scan_proc_scan_comp_event::vap(%d) Scancomplete(cookie %d), anoter vap(%d) scanning(cookie %d) !}",
            event_hdr->vap_id, d2h_scan_rsp_info->ull_cookie, scan_mgmt->scan_record_mgmt.vap_id,
            scan_mgmt->scan_record_mgmt.ull_cookie);
        return HI_SUCCESS;
    }

    /* 删除扫描超时保护定时器 */
    if (scan_mgmt->scan_timeout.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(scan_mgmt->scan_timeout));
    }

    /* 获取hmac vap */
    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(event_hdr->vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(event_hdr->vap_id, OAM_SF_SCAN, "{hmac_scan_proc_scan_comp_event::pst_hmac_vap null.}");

        /* 设置当前处于非扫描状态 */
        scan_mgmt->is_scanning = HI_FALSE;
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_scan_proc_scan_comp_event_vap(mac_dev, scan_mgmt, hmac_vap);

#ifdef _PRE_WLAN_FEATURE_ANY
    if ((hmac_vap->base_vap->support_any == HI_TRUE) && (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) &&
        (mac_dev->vap_num == 1) && (mac_dev->scan_params.scan_mode == WLAN_SCAN_MODE_FOREGROUND)) {
        oam_warning_log1(0, OAM_SF_SCAN, "{[ANY]switch to original channel %d.}",
            hmac_vap->base_vap->channel.chan_number);
        hmac_config_set_freq(hmac_vap->base_vap, 1, &hmac_vap->base_vap->channel.chan_number);
    }
#endif
    /* 1102 作为ap ，40M 带宽下执行扫描，扫描完成后VAP 状态修改为扫描前的状态 */
    /* 根据device上报的扫描结果，上报sme */
    /* 将扫描执行情况(扫描执行成功、还是失败等返回结果)记录到扫描运行记录结构体中 */
    scan_mgmt->scan_record_mgmt.scan_rsp_status = d2h_scan_rsp_info->scan_rsp_status;
    scan_mgmt->scan_record_mgmt.ull_cookie = d2h_scan_rsp_info->ull_cookie;

    /* 上报扫描结果前，清除下到期的扫描bss，防止上报过多到期的bss */
    hmac_scan_clean_expire_scanned_bss(&(scan_mgmt->scan_record_mgmt), HI_FALSE);
    /* 如果扫描回调函数不为空，则调用回调函数 */
    if (scan_mgmt->scan_record_mgmt.fn_cb != HI_NULL) {
        scan_mgmt->scan_record_mgmt.fn_cb(&(scan_mgmt->scan_record_mgmt));
    }

    /* 设置当前处于非扫描状态 */
    scan_mgmt->is_scanning = HI_FALSE;

#ifdef _PRE_WLAN_FEATURE_P2P
    if (hmac_vap->base_vap->vap_state == MAC_VAP_STATE_STA_LISTEN) {
        hmac_p2p_listen_timeout(hmac_vap->base_vap);
    }
    if (hmac_vap->en_wait_roc_end == HI_TRUE) {
        oam_warning_log1(event_hdr->vap_id, OAM_SF_SCAN, "{hmac_scan_proc_scan_comp_event::scan rsp status[%d]}",
            d2h_scan_rsp_info->scan_rsp_status);
        OAL_COMPLETE(&(hmac_vap->st_roc_end_ready));
        hmac_vap->en_wait_roc_end = HI_FALSE;
    }
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 异常扫描请求,抛事件到wal 层，执行扫描完成
 输入参数  : pst_mac_device: 指向device结构体
             p_params: 本次扫描请求的参数
 修改历史      :
  1.日    期   : 2013年12月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_scan_proc_scan_req_event_exception(hmac_vap_stru *hmac_vap)
{
    hmac_scan_rsp_stru scan_rsp;

    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log1(0, OAM_SF_SCAN, "{hmac_mgmt_scan_req_exception::param null, %p.}", (uintptr_t)hmac_vap);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 不支持发起扫描的状态发起了扫描 */
    oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_SCAN, "{hmac_mgmt_scan_req_exception::vap state is=%x.}",
        hmac_vap->base_vap->vap_state);

    if (memset_s(&scan_rsp, sizeof(hmac_scan_rsp_stru), 0, sizeof(hmac_scan_rsp_stru)) != EOK) {
        return HI_FAIL;
    }

    scan_rsp.result_code = HMAC_MGMT_REFUSED;
    scan_rsp.num_dscr = 0;

    return hmac_send_event_to_host(hmac_vap->base_vap, (const hi_u8 *)(&scan_rsp), sizeof(hmac_scan_rsp_stru),
        HMAC_HOST_CTX_EVENT_SUB_TYPE_SCAN_COMP_STA);
}

/* ****************************************************************************
 功能描述  : 设置probe req帧中携带的源mac addr，如果随机mac addr特性开启，则携带随机mac addr
 输入参数  : [1]pst_hmac_vap
             [2]puc_sour_mac_addr
             [3]is_rand_mac_addr_scan
             [4]is_p2p0_scan
 返 回 值  : 无
**************************************************************************** */
hi_void hmac_scan_set_sour_mac_addr_in_probe_req(const hmac_vap_stru *hmac_vap, hi_u8 *sa_mac_addr, hi_u8 mac_addr_len,
    hi_u8 is_rand_mac_addr_scan, hi_u8 is_p2p0_scan)
{
#ifdef _PRE_WLAN_FEATURE_P2P
    /* WLAN/P2P 特性情况下，p2p0 和p2p-p2p0 cl 扫描时候，需要使用不同设备 */
    if (is_p2p0_scan == HI_TRUE) {
        if (memcpy_s(sa_mac_addr, mac_addr_len,
            hmac_vap->base_vap->mib_info->wlan_mib_sta_config.auc_p2p0_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
            return;
        }
    } else
#else
    hi_unref_param(is_p2p0_scan);
#endif /* _PRE_WLAN_FEATURE_P2P */
    {
        /* 如果随机mac addr扫描特性开启且非P2P场景，设置随机mac addr到probe req帧中 */
        if ((is_rand_mac_addr_scan == HI_TRUE) && (is_legacy_vap(hmac_vap->base_vap))) {
            oal_random_ether_addr(sa_mac_addr, mac_addr_len);
            sa_mac_addr[0] &= (~0x02); /* wlan0 MAC[0] bit1 需要设置为0 */
            sa_mac_addr[1] = 0x11;
            sa_mac_addr[2] = 0x02; /* 2 元素索引 */

            oam_warning_log3(hmac_vap->base_vap->vap_id, OAM_SF_SCAN,
                "{hmac_scan_set_sour_mac_addr_in_probe_req::rand_mac_addr[XX:XX:XX:%02X:%02X:%02X].}",
                sa_mac_addr[3],                  /* 3 元素索引 */
                sa_mac_addr[4], sa_mac_addr[5]); /* 4 5 元素索引 */
        } else {
            /* 设置地址为自己的MAC地址 */
            if (memcpy_s(sa_mac_addr, mac_addr_len,
                hmac_vap->base_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN) != EOK) {
                return;
            }
        }
    }
    return;
}

/* ****************************************************************************
 功能描述  : 根据device下所有的vap状态以及其它信息，更新扫描参数:
             包括发起扫描者的vap id、扫描模式、每信道扫描次数、probe req帧携带的源mac addr
 输入参数  : hmac_vap_stru *pst_hmac_vap,
             mac_scan_req_stru *pst_scan_params,
             hi_u8   en_is_random_mac_addr_scan,      是否为随机mac addr扫描的标记
 修改历史      :
  1.日    期   : 2015年2月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_scan_update_scan_params(const hmac_vap_stru *hmac_vap, mac_scan_req_stru *scan_params,
    hi_u8 is_random_mac_addr_scan)
{
    mac_device_stru *mac_dev = HI_NULL;
    mac_vap_stru *mac_vap_temp = HI_NULL;
    wlan_vap_mode_enum_uint8 vap_mode;

    /* 获取mac device */
    mac_dev = mac_res_get_dev();
    /* 1.记录发起扫描的vap id到扫描参数 */
    scan_params->vap_id = hmac_vap->base_vap->vap_id;
    scan_params->scan_mode = WLAN_SCAN_MODE_FOREGROUND;
    scan_params->need_switch_back_home_channel = HI_FALSE;

    /* 2.修改扫描模式和信道扫描次数: 根据是否存在up状态下的vap，如果是，则是背景扫描，如果不是，则是前景扫描 */
    mac_device_find_up_vap(mac_dev, &mac_vap_temp);
    if (mac_vap_temp != HI_NULL) {
        /* 判断vap的类型，如果是sta则为sta的背景扫描，如果是ap，则是ap的背景扫描，其它类型的vap暂不支持背景扫描 */
        vap_mode = hmac_vap->base_vap->vap_mode;
        if (vap_mode == WLAN_VAP_MODE_BSS_STA) {
            /* 修改扫描参数为sta的背景扫描 */
            scan_params->scan_mode = WLAN_SCAN_MODE_BACKGROUND_STA;
        } else if (vap_mode == WLAN_VAP_MODE_BSS_AP
#ifdef _PRE_WLAN_FEATURE_MESH
            || (vap_mode == WLAN_VAP_MODE_MESH)
#endif
        ) {
            /* 修改扫描参数为sta的背景扫描 */
            scan_params->scan_mode = WLAN_SCAN_MODE_BACKGROUND_AP;
        } else {
            oam_error_log1(0, OAM_SF_SCAN, "{hmac_scan_update_scan_params::vap mode[%d], not support bg scan.}",
                vap_mode);
            return HI_FAIL;
        }
        scan_params->need_switch_back_home_channel = HI_TRUE;
    }
    /* 3.设置发送的probe req帧中源mac addr */
    scan_params->is_random_mac_addr_scan = is_random_mac_addr_scan;
    hmac_scan_set_sour_mac_addr_in_probe_req(hmac_vap, scan_params->auc_sour_mac_addr, WLAN_MAC_ADDR_LEN,
        is_random_mac_addr_scan, scan_params->is_p2p0_scan);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 检测是否能够发起扫描，如果可以，则记录扫描请求者的信息，并清空上一次扫描结果
 输入参数  : hmac_vap_stru       *pst_hmac_vap,
             hmac_device_stru    *pst_hmac_device
 修改历史      :
  1.日    期   : 2015年5月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_scan_check_is_dispatch_scan_req(const hmac_vap_stru *hmac_vap, const hmac_device_stru *hmac_dev)
{
#ifdef _PRE_WLAN_FEATURE_P2P
    hi_u32 ret;
    /* 1.先检测其它vap的状态从而判断是否可进入扫描状态，使得扫描尽量不打断其它的入网流程 */
    ret = hmac_p2p_check_can_enter_state(hmac_vap->base_vap, HMAC_FSM_INPUT_SCAN_REQ);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_SCAN,
            "{hmac_scan_check_is_dispatch_scan_req::Because of err_code[%d], can't enter into scan state.}", ret);
        return ret;
    }
#else
    hi_unref_param(hmac_vap);
#endif
    /* 2.判断当前扫描是否正在执行 */
    if (hmac_dev->scan_mgmt.is_scanning == HI_TRUE) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_SCAN,
            "{hmac_scan_check_is_dispatch_scan_req::the scan request is rejected.}");
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理上一次的扫描记录，接口封装，从而便于可扩展(未来可能使用老化机制判断是否清除)
 输入参数  : hmac_vap_stru       *pst_hmac_vap,
             hmac_device_stru    *pst_hmac_device
 修改历史      :
  1.日    期   : 2015年5月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_scan_proc_last_scan_record(hmac_device_stru *hmac_dev)
{
    oam_info_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_scan_req_event:: start clean last scan record.}");

    /* 本次扫描请求发起时，清除上一次扫描结果中过期的bss信息 */
    hmac_scan_clean_expire_scanned_bss(&(hmac_dev->scan_mgmt.scan_record_mgmt), HI_FALSE);
    return;
}

/* ****************************************************************************
 功能描述  : host侧抛扫描请求时间到device侧，防止因核间通信、抛事件等异常情况，host侧接收不到
             扫描响应的超时回调函数处理，属于扫描模块内的超时保护
 修改历史      :
  1.日    期   : 2015年5月19日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_scan_proc_scan_timeout_fn(hi_void *arg)
{
    hmac_device_stru *hmac_dev = (hmac_device_stru *)arg;
    hmac_vap_stru *hmac_vap = HI_NULL;
    hmac_scan_record_stru *scan_record = HI_NULL;
    hi_u32 pedding_data = 0;

    /* 获取扫描记录信息 */
    scan_record = &(hmac_dev->scan_mgmt.scan_record_mgmt);

    /* 若下发的是ANY扫描，结束扫描的时候恢复标志为非ANY扫描，若下发的非ANY扫描，这里赋该值无影响 */
    scan_record->is_any_scan = HI_FALSE;

    /* 获取hmac vap */
    hmac_vap = hmac_vap_get_vap_stru(scan_record->vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(scan_record->vap_id, OAM_SF_SCAN, "{hmac_scan_proc_scan_timeout_fn::pst_hmac_vap null.}");

        /* 扫描状态恢复为未在执行的状态 */
        hmac_dev->scan_mgmt.is_scanning = HI_FALSE;
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 根据当前扫描的类型和当前vap的状态，决定切换vap的状态，如果是前景扫描，才需要切换vap的状态 */
    if (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        if (hmac_vap->base_vap->vap_state == MAC_VAP_STATE_STA_WAIT_SCAN) {
            /* 改变vap状态到SCAN_COMP */
            hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_SCAN_COMP);
        } else if (hmac_vap->base_vap->vap_state == MAC_VAP_STATE_UP) {
            /* 背景扫描时需要进行帧过滤的配置 */
            hmac_set_rx_filter_value(hmac_vap->base_vap);
        }
    }

    /* 1102 作为ap ，40M 带宽下执行扫描，扫描完成后VAP 状态修改为扫描前的状态 */
    if (((hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        || (hmac_vap->base_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
        ) && (scan_record->vap_last_state != MAC_VAP_STATE_BUTT)) {
        hmac_fsm_change_state(hmac_vap, scan_record->vap_last_state);
        scan_record->vap_last_state = MAC_VAP_STATE_BUTT;
    }
    /* 1102 作为ap ，40M 带宽下执行扫描，扫描完成后VAP 状态修改为扫描前的状态 */
    /* 设置扫描响应状态为超时 */
    scan_record->scan_rsp_status = MAC_SCAN_TIMEOUT;
    oam_warning_log1(scan_record->vap_id, OAM_SF_SCAN, "{hmac_scan_proc_scan_timeout_fn::scan time out cookie [%x].}",
        scan_record->ull_cookie);

    /* 如果扫描回调函数不为空，则调用回调函数 */
    if (scan_record->fn_cb != HI_NULL) {
        oam_warning_log0(scan_record->vap_id, OAM_SF_SCAN,
            "{hmac_scan_proc_scan_timeout_fn::scan callback func proc.}");
        scan_record->fn_cb(scan_record);
    }

    /* DMAC 超时未上报扫描完成，HMAC 下发扫描结束命令，停止DMAC 扫描 */
    hmac_config_scan_abort(hmac_vap->base_vap, sizeof(hi_u32), (hi_u8 *)&pedding_data);

    /* 扫描状态恢复为未在执行的状态 */
    hmac_dev->scan_mgmt.is_scanning = HI_FALSE;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 填写扫描请求事件并通知dmac进行扫描
**************************************************************************** */
hi_u32 hmac_scan_dispatch_req_event(const hmac_vap_stru *hmac_vap, const mac_scan_req_stru *scan_params)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    mac_scan_req_stru *h2d_scan_req_params = HI_NULL; /* hmac发送到dmac的扫描请求参数 */
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    hi_u32 scan_timeout;

    /* 抛扫描请求事件到DMAC, 申请事件内存 */
    event_mem = frw_event_alloc(sizeof(mac_scan_req_stru));
    if (event_mem == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_scan_req_event::event_mem null.}");
        /* 恢复扫描状态为非运行状态 */
        hmac_dev->scan_mgmt.is_scanning = HI_FALSE;
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_SCAN_REQ,
        sizeof(mac_scan_req_stru), FRW_EVENT_PIPELINE_STAGE_1, hmac_vap->base_vap->vap_id);
    h2d_scan_req_params = (mac_scan_req_stru *)(event->auc_event_data);
    /* 拷贝扫描请求参数到事件data区域 */
    /* h2d_scan_req_params: event->auc_event_data, 可变数组 */
    if (memcpy_s(h2d_scan_req_params, sizeof(mac_scan_req_stru), scan_params, sizeof(mac_scan_req_stru)) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(0, OAM_SF_CFG, "hmac_scan_proc_scan_req_event:: pst_scan_params memcpy_s fail.");
        return HI_FAIL;
    }

    /* 如果是P2P 发起监听，则设置HMAC 扫描超时时间为P2P 监听时间 */
    if (MAC_SCAN_FUNC_P2P_LISTEN == scan_params->scan_func) {
        scan_timeout = scan_params->us_scan_time * 2; /* 2倍超出监听时间 */
    } else {
        scan_timeout = WLAN_MAX_TIME_PER_SCAN;
    }

    oam_warning_log4(scan_params->vap_id, OAM_SF_SCAN,
        "Scan_params::Now Scan channel_num[%d] p2p_scan[%d],scan_cnt_per_ch[%d],need back home_ch[%d]!",
        scan_params->channel_nums, scan_params->is_p2p0_scan, scan_params->max_scan_cnt_per_channel,
        scan_params->need_switch_back_home_channel);

    /* 启动扫描保护定时器，防止因拋事件、核间通信失败等情况下的异常保护，定时器初步的超时时间为4.5秒 */
    frw_timer_create_timer(&(hmac_dev->scan_mgmt.scan_timeout), hmac_scan_proc_scan_timeout_fn, scan_timeout, hmac_dev,
        HI_FALSE);
    /* 如果是p2p listen 记录下listen的信道 */
    if (MAC_SCAN_FUNC_P2P_LISTEN == scan_params->scan_func) {
        hmac_dev->scan_mgmt.p2p_listen_channel = scan_params->ast_channel_list[0];
    }
    /* 分发事件 */
    hcc_hmac_tx_control_event(event_mem, sizeof(mac_scan_req_stru));
    frw_event_free(event_mem);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理扫描请求的总入口
 输入参数  : pst_mac_device: 指向device结构体
             p_params: 本次扫描请求的参数
 修改历史      :
  1.日    期   : 2015年2月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_scan_proc_scan_req_event(hmac_vap_stru *hmac_vap, mac_scan_req_stru *scan_params)
{
    hmac_device_stru *hmac_dev = hmac_get_device_stru();
    mac_device_stru *mac_dev = mac_res_get_dev();
    hmac_scan_record_stru *scan_record = &(hmac_dev->scan_mgmt.scan_record_mgmt);
    hi_u8 is_random_mac_addr_scan;

    /* 异常判断: 扫描的信道个数为0 */
    if (scan_params->channel_nums == 0) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_scan_req_event::channel_nums=0.}");
        return HI_FAIL;
    }

    /* 更新此次扫描请求的扫描参数 */
    is_random_mac_addr_scan = hmac_dev->scan_mgmt.is_random_mac_addr_scan;
    if (scan_params->scan_func == MAC_SCAN_FUNC_P2P_LISTEN) {
        is_random_mac_addr_scan = HI_FALSE;
    }
    if (hmac_scan_update_scan_params(hmac_vap, scan_params, is_random_mac_addr_scan) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_scan_req_event::update scan mode failed.}");
        return HI_FAIL;
    }

    /* 检测是否符合发起扫描请求的条件，如果不符合，直接返回 */
    if (hmac_scan_check_is_dispatch_scan_req(hmac_vap, hmac_dev) != HI_SUCCESS) {
        if (scan_params->scan_func == MAC_SCAN_FUNC_P2P_LISTEN) {
            mac_vap_state_change(hmac_vap->base_vap, mac_dev->p2p_info.last_vap_state);
        }
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_scan_req_event:: can't dispatch scan req.}");
        return HI_FAIL;
    }

    /* 设置扫描模块处于扫描状态，其它扫描请求将丢弃 */
    hmac_dev->scan_mgmt.is_scanning = HI_TRUE;
    /* 处理上一次扫描记录，目前直接清楚上一次结果，后续可能需要老化时间处理 */
    hmac_scan_proc_last_scan_record(hmac_dev);
    /* 记录扫描发起者的信息，某些模块回调函数使用 */
    scan_record->vap_id = scan_params->vap_id;
    scan_record->chan_numbers = scan_params->channel_nums;
    scan_record->fn_cb = scan_params->fn_cb;

    if (is_ap(hmac_vap->base_vap)) {
        oam_warning_log1(0, 0, "{hmac_scan_proc_scan_req_event::save vap_state:%d}", hmac_vap->base_vap->vap_state);
        scan_record->vap_last_state = hmac_vap->base_vap->vap_state;
    }

    scan_record->ull_cookie = scan_params->ull_cookie;
    /* 如果发起扫描的vap的模式为sta，并且，其关联状态为非up状态，且非p2p监听状态，则切换其扫描状态 */
    if (is_sta(hmac_vap->base_vap) && (scan_params->scan_func != MAC_SCAN_FUNC_P2P_LISTEN)) {
        if (hmac_vap->base_vap->vap_state != MAC_VAP_STATE_UP) {
            /* 切换vap的状态为WAIT_SCAN状态 */
            hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_STA_WAIT_SCAN);
        } else {
            /* 背景扫描时需要进行帧过滤的配置 */
            hmac_vap->base_vap->vap_state = MAC_VAP_STATE_STA_WAIT_SCAN;
            hmac_set_rx_filter_value(hmac_vap->base_vap);
            hmac_vap->base_vap->vap_state = MAC_VAP_STATE_UP;
        }
    }

    /* AP的启动扫描做特殊处理，当hostapd下发扫描请求时，VAP还处于INIT状态 */
    if (is_ap(hmac_vap->base_vap) && (hmac_vap->base_vap->vap_state == MAC_VAP_STATE_INIT)) {
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_AP_WAIT_START);
    }

    return hmac_scan_dispatch_req_event(hmac_vap, scan_params);
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 处理pno调度扫描请求的入口
 输入参数  : pst_mac_device: 指向device结构体
             p_params: 本次扫描请求的参数
 修改历史      :
  1.日    期   : 2015年6月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_scan_proc_sched_scan_req_event(const hmac_vap_stru *hmac_vap, const mac_pno_scan_stru *pno_scan_params)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hmac_device_stru *hmac_dev = HI_NULL;
    hmac_scan_record_stru *scan_record = HI_NULL;
    hi_u32 ret;

    /* 判断PNO调度扫描下发的过滤的ssid个数小于等于0 */
    if (pno_scan_params->l_ssid_count <= 0) {
        oam_warning_log0(hmac_vap->base_vap->vap_id, OAM_SF_SCAN,
            "{hmac_scan_proc_sched_scan_req_event::ssid_count <=0.}");
        return HI_FAIL;
    }

    /* 获取hmac device */
    hmac_dev = hmac_get_device_stru();
    /* 检测是否符合发起扫描请求的条件，如果不符合，直接返回 */
    ret = hmac_scan_check_is_dispatch_scan_req(hmac_vap, hmac_dev);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(hmac_vap->base_vap->vap_id, OAM_SF_SCAN,
            "{hmac_scan_proc_sched_scan_req_event::Because of error[%d], can't dispatch scan req.}", ret);
        return ret;
    }

    /* 清空上一次的扫描结果 */
    hmac_scan_proc_last_scan_record(hmac_dev);

    /* 记录扫描发起者的信息，某些模块回调函数使用 */
    scan_record = &(hmac_dev->scan_mgmt.scan_record_mgmt);
    scan_record->vap_id = hmac_vap->base_vap->vap_id;
    scan_record->fn_cb = pno_scan_params->fn_cb;

    /* 抛扫描请求事件到DMAC, 申请事件内存 */
    event_mem = frw_event_alloc(sizeof(uintptr_t));
    if (event_mem == HI_NULL) {
        oam_error_log0(hmac_vap->base_vap->vap_id, OAM_SF_SCAN,
            "{hmac_scan_proc_sched_scan_req_event::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    event = (frw_event_stru *)event_mem->puc_data;

    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_SCHED_SCAN_REQ,
        sizeof(uintptr_t), FRW_EVENT_PIPELINE_STAGE_1, hmac_vap->base_vap->vap_id);

    /* 事件data域内携带PNO扫描请求参数 */
    if (memcpy_s(frw_get_event_payload(event_mem), sizeof(mac_pno_scan_stru *), (hi_u8 *)&pno_scan_params,
        sizeof(mac_pno_scan_stru *)) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(0, OAM_SF_CFG, "hmac_scan_proc_sched_scan_req_event:: pst_pno_scan_params memcpy_s fail.");
        return HI_FAIL;
    }

    /* 分发事件 */
    hcc_hmac_tx_control_event(event_mem, sizeof(uintptr_t));
    frw_event_free(event_mem);

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 扫描模块初始化
 修改历史      :
  1.日    期   : 2015年5月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_scan_init(hmac_device_stru *hmac_dev)
{
    hmac_scan_stru *scan_mgmt = HI_NULL;
    hmac_bss_mgmt_stru *bss_mgmt = HI_NULL;

    /* 初始化扫描管理结构体信息 */
    scan_mgmt = &(hmac_dev->scan_mgmt);
    scan_mgmt->is_scanning = HI_FALSE;
    scan_mgmt->request = HI_NULL;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    scan_mgmt->sched_scan_req = HI_NULL;
#endif
    scan_mgmt->complete = HI_TRUE;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    scan_mgmt->sched_scan_complete = HI_TRUE;
    oal_spin_lock_init(&scan_mgmt->st_scan_request_spinlock);
#endif
    scan_mgmt->scan_record_mgmt.vap_last_state = MAC_VAP_STATE_BUTT;
    scan_mgmt->is_random_mac_addr_scan = HI_FALSE; /* 随机mac 扫描开关，默认关闭 */
    scan_mgmt->scan_record_mgmt.is_any_scan = HI_FALSE;
    /* 初始化bss管理结果链表和锁 */
    bss_mgmt = &(scan_mgmt->scan_record_mgmt.bss_mgmt);
    hi_list_init(&(bss_mgmt->bss_list_head));
    oal_spin_lock_init(&bss_mgmt->st_lock);

    /* 初始化内核下发扫描request资源锁 */
    return;
}

/* ****************************************************************************
 功能描述  : hmac扫描模块退出
 修改历史      :
  1.日    期   : 2015年5月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_scan_exit(hmac_device_stru *hmac_dev)
{
    hmac_scan_stru *scan_mgmt = HI_NULL;

    scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 清空扫描记录信息 */
    hmac_scan_clean_scan_record(&scan_mgmt->scan_record_mgmt);

    /* 删除扫描超时保护定时器 */
    if (scan_mgmt->scan_timeout.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&(scan_mgmt->scan_timeout));
    }

    /* 清除扫描管理结构体信息 */
    if (scan_mgmt->request == HI_NULL) {
        return;
    }
    if (scan_mgmt->request->ie != HI_NULL) {
        oal_free(scan_mgmt->request->ie);
        scan_mgmt->request->ie = HI_NULL;
    }
    if (scan_mgmt->request->ssids != HI_NULL) {
        oal_free(scan_mgmt->request->ssids);
        scan_mgmt->request->ssids = HI_NULL;
    }
    oal_free(scan_mgmt->request);

    scan_mgmt->request = HI_NULL;
    scan_mgmt->is_scanning = HI_FALSE;
    scan_mgmt->complete = HI_TRUE;
}
#ifdef _PRE_WLAN_FEATURE_SCAN_BY_SSID
static hi_void hmac_scan_proc_check_prefix_ssid(hmac_scanned_bss_info *scanned_bss,
    const oal_cfg80211_ssid_stru *req_ssid, const hi_u8 *puc_ssid, hi_u8 ssid_len)
{
    hi_u8 req_ssid_len;
    hi_u8 loop;

    if (oal_unlikely(puc_ssid == HI_NULL)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_check_prefix_ssid::puc_ssid null.}");
        return;
    }

    if (oal_unlikely(req_ssid == HI_NULL)) {
        return;
    }
    req_ssid_len = req_ssid[0].ssid_len;
    if (req_ssid_len <= ssid_len) {
        for (loop = 0; loop < req_ssid_len; loop++) {
            if (req_ssid[0].ssid[loop] != puc_ssid[loop]) {
                break;
            }
        }
        if (loop == req_ssid_len) {
            return;
        }
    }

    scanned_bss->bss_dscr_info.need_drop = HI_TRUE;
}

/* ****************************************************************************
 功能描述  : hmac扫描模块上报前判断beacon/probe rsp中的ssid与指定ssid的扫描请求是否一致
 输入参数  : hmac_scanned_bss_info *pst_scanned_bss :扫描信息结构体
                            hi_u8 *puc_ssid: 帧中的ssid
                            hi_u8 uc_ssid_len:帧中ssid的长度
 修改历史      :
  1.日    期   : 2019年2月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_scan_proc_check_ssid(hmac_scanned_bss_info *scanned_bss, const hi_u8 *puc_ssid, hi_u8 ssid_len)
{
    hmac_scan_stru *scan_mgmt = HI_NULL;
    hmac_device_stru *hmac_dev = HI_NULL;
    hi_u32 l_index;
    hi_u8 req_ssid_len;
    oal_cfg80211_ssid_stru *ssids = HI_NULL;

    /* 获取hmac device 结构 */
    hmac_dev = hmac_get_device_stru();
    scan_mgmt = &(hmac_dev->scan_mgmt);
    if (scan_mgmt->request == HI_NULL) {
        return;
    }

    /* 当下发的ssid中有通配ssid时，不进行任何过滤 */
    for (l_index = 0; l_index < scan_mgmt->request->n_ssids; l_index++) {
        if (scan_mgmt->request->ssids[l_index].ssid[0] == '\0') {
            return;
        }
    }

    if (puc_ssid == HI_NULL) {
        scanned_bss->bss_dscr_info.need_drop = HI_TRUE;
        return;
    }

    ssids = scan_mgmt->request->ssids;
    hi_u32 l_ssid_num = scan_mgmt->request->n_ssids;

    /* 如果是前缀ssid扫描,则执行前缀过滤规则 */
    if (scan_mgmt->request->prefix_ssid_scan_flag == HI_TRUE) {
        hmac_scan_proc_check_prefix_ssid(scanned_bss, ssids, puc_ssid, ssid_len);
        return;
    }
    /* pst_request 在驱动判断指定ssid扫描最大个数之前赋值，需在这里进行检查 */
    /* 如果未指定ssid,则直接返回 */
    if (l_ssid_num == 0) {
        return;
    }

    if (l_ssid_num > WLAN_SCAN_REQ_MAX_BSS) {
        /* 如果用户下发的指定ssid的个数大于驱动支持的最大个数，则取驱动支持的指定ssid的最大个数 */
        l_ssid_num = WLAN_SCAN_REQ_MAX_BSS;
    }

    for (l_index = 0; l_index < l_ssid_num; l_index++) {
        req_ssid_len = ssids[l_index].ssid_len;
        if (req_ssid_len > OAL_IEEE80211_MAX_SSID_LEN) {
            req_ssid_len = OAL_IEEE80211_MAX_SSID_LEN;
        }

        if (req_ssid_len != ssid_len) {
            continue;
        }

        if (memcmp(puc_ssid, ssids[l_index].ssid, req_ssid_len) != 0) {
            continue;
        } else {
            return;
        }
    }

    scanned_bss->bss_dscr_info.need_drop = HI_TRUE;
    return;
}
#endif /* #ifdef _PRE_WLAN_FEATURE_SCAN_BY_SSID */

hi_u32 hmac_scan_clean_result(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hmac_device_stru *hmac_dev = hmac_get_device_stru();

    hi_unref_param(mac_vap);
    hi_unref_param(us_len);
    hi_unref_param(puc_param);

    if (hmac_dev == HI_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_scan_clean_result::hmac_dev null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    oam_info_log0(0, OAM_SF_SCAN, "{hmac_scan_clean_result::clean driver scan results.}");
    hmac_scan_clean_expire_scanned_bss(&(hmac_dev->scan_mgmt.scan_record_mgmt), HI_TRUE);
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
