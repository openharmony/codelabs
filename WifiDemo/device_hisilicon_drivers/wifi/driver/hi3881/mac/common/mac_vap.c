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

#include "oal_mem.h"
#include "wlan_types.h"
#include "mac_vap.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "mac_regdomain.h"
#include "dmac_ext_if.h"
#include "hi_isr.h"
#include "hmac_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  全局变量定义
**************************************************************************** */
#define __WIFI_ROM_SECTION__          /* 代码ROM段起始位置 */
WIFI_ROM_BSS hi_u8 g_vap_res_map = 0; /* vap 资源map表 */
WIFI_ROM_BSS hi_u8 *g_puc_mac_vap_res = HI_NULL;
/* WME初始参数定义，按照OFDM初始化 AP模式 值来自于TGn 9 Appendix D: Default WMM AC Parameters */
WIFI_ROM_RODATA static const mac_wme_param_stru g_ast_wmm_initial_params_ap[WLAN_WME_AC_BUTT] = {
    /* AIFS, cwmin, cwmax, txop */
    /* BE */
    {  3,    4,     6,     0, },
    /* BK */
    {  7,    4,     10,    0, },
    /* VI */
    {  1,    3,     4,     3008, },
    /* VO */
    {  1,    2,     3,     1504, },
};

/* WMM初始参数定义，按照OFDM初始化 STA模式 */
WIFI_ROM_RODATA static const mac_wme_param_stru g_ast_wmm_initial_params_sta[WLAN_WME_AC_BUTT] = {
    /* AIFS, cwmin, cwmax, txop */
    /* BE */
    {  3,    3,     10,     0, },
    /* BK */
    {  7,    4,     10,     0, },
    /* VI */
    {  2,    3,     4,     3008, },
    /* VO */
    {  2,    2,     3,     1504, },
};

/* WMM初始参数定义，aput建立的bss中STA的使用的EDCA参数 */
WIFI_ROM_RODATA static const mac_wme_param_stru g_ast_wmm_initial_params_bss[WLAN_WME_AC_BUTT] = {
    /* AIFS, cwmin, cwmax, txop */
    /* BE */
    {  3,    4,     10,     0, },
    /* BK */
    {  7,    4,     10,     0, },
    /* VI */
    {  2,    3,     4,     3008, },
    /* VO */
    {  2,    2,     3,     1504, },
};

/* ****************************************************************************
 功能描述  : 分配一个未使用vap资源
 返 回 值  : 未使用的vap资源id
 修改历史      :
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_vap_alloc_vap_res(hi_void)
{
    hi_u8 vap_res_idx;
    hi_u8 vap_res_num = oal_mem_get_vap_res_num();
    vap_res_idx = oal_bit_find_first_zero((hi_u32)g_vap_res_map, vap_res_num);
    if (vap_res_idx >= vap_res_num) {
        oam_error_log1(0, OAM_SF_ANY, "{mac_vap_alloc_vap_res:: alloc vap res fail. res[%x].}", g_vap_res_map);
        vap_res_idx = MAC_VAP_RES_ID_INVALID;
    } else {
        /* 将对应的res标志位置1 */
        g_vap_res_map |= (hi_u8)(BIT0 << vap_res_idx);
    }
    return vap_res_idx;
}

/* ****************************************************************************
 功能描述  : 删除一个已使用vap资源map标志位,内存不需要释放(驱动卸载时释放)和清零(创建初始化时清零)
             OFFLOAD模式下dmac hmac各自释放，非OFFLOAD模式下统一由hmac申请和释放
 输入参数  : vap资源id
 修改历史      :
  1.日    期   : 2019年5月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_free_vap_res(hi_u8 idx)
{
    g_vap_res_map &= (~((hi_u8)(BIT0 << idx)));
}

/* ****************************************************************************
 功能描述  : mac vap资源初始化,根据vap数量申请内存
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_res_init(const hi_u8 vap_num)
{
    hi_u32 vap_size = sizeof(mac_vap_stru) * vap_num;
    /* mac user 在非offload模式下存在两次申请的情况,已经申请过则跳过 */
    if (g_puc_mac_vap_res != HI_NULL) {
        return HI_SUCCESS;
    }
    g_puc_mac_vap_res = hi_malloc(HI_MOD_ID_WIFI_DRV, vap_size);
    if (g_puc_mac_vap_res == HI_NULL) {
        hi_diag_log_msg_e1(0, "{mac_vap_res_init::mem alloc vap res null. size = %d.}", vap_size);
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }
    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(g_puc_mac_vap_res, vap_size, 0, vap_size);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : mac vap资源去初始化
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_res_exit(hi_void)
{
    if (g_puc_mac_vap_res != HI_NULL) {
        hi_free(HI_MOD_ID_WIFI_DRV, g_puc_mac_vap_res);
        g_puc_mac_vap_res = HI_NULL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取mac vap结构体
 输入参数  : vap资源id
 修改历史      :
  1.日    期   : 2019年5月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT mac_vap_stru *mac_vap_get_vap_stru(hi_u8 idx)
{
    hi_u8 vap_res_num = oal_mem_get_vap_res_num();
    if (oal_unlikely(idx >= vap_res_num)) {
        return HI_NULL;
    }
    return (mac_vap_stru *)(g_puc_mac_vap_res + idx * sizeof(mac_vap_stru));
}

/* ****************************************************************************
 功能描述  : 判断vap是否存在
 输入参数  : vap资源id
 修改历史      :
  1.日    期   : 2019年5月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_is_valid(hi_u8 idx)
{
    if (oal_unlikely(idx >= oal_mem_get_vap_res_num())) {
        return HI_FALSE;
    }
    if (g_vap_res_map & (BIT0 << idx)) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 获取ap或者sta 的配置指针
 输入参数  : en_vap_mode: en_vap_mode当前模式
 返 回 值  : wmm配置指针
 修改历史      :
  1.日    期   : 2014年1月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT mac_wme_param_stru *mac_get_wmm_cfg(wlan_vap_mode_enum_uint8 vap_mode)
{
    /* 参考认证项配置，没有按照协议配置，WLAN_VAP_MODE_BUTT表示是ap广播给sta的edca参数 */
    if (vap_mode == WLAN_VAP_MODE_BUTT) {
        return (mac_wme_param_stru *)g_ast_wmm_initial_params_bss;
    } else if (vap_mode == WLAN_VAP_MODE_BSS_AP) {
        return (mac_wme_param_stru *)g_ast_wmm_initial_params_ap;
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    } else if (vap_mode == WLAN_VAP_MODE_MESH) {
        return (mac_wme_param_stru *)g_ast_wmm_initial_params_ap;
#endif
    }

    return (mac_wme_param_stru *)g_ast_wmm_initial_params_sta;
}

/* ****************************************************************************
 功能描述  : 初始化wme参数, 除sta之外的模式
 输出参数  : pst_wme_param: wme参数
 返 回 值  : HI_SUCCESS 或其它错误码
 修改历史      :
  1.日    期   : 2012年12月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_init_wme_param(const mac_vap_stru *mac_vap)
{
    const mac_wme_param_stru   *wmm_param = HI_NULL;
    const mac_wme_param_stru   *wmm_param_sta = HI_NULL;
    hi_u8                       ac_type;

    wmm_param = mac_get_wmm_cfg(mac_vap->vap_mode);
    for (ac_type = 0; ac_type < WLAN_WME_AC_BUTT; ac_type++) {
        /* VAP自身的EDCA参数 */
        mac_vap->mib_info->wlan_mib_qap_edac[ac_type].dot11_qapedca_table_aifsn = wmm_param[ac_type].aifsn;
        mac_vap->mib_info->wlan_mib_qap_edac[ac_type].dot11_qapedca_table_c_wmin = wmm_param[ac_type].logcwmin;
        mac_vap->mib_info->wlan_mib_qap_edac[ac_type].dot11_qapedca_table_c_wmax = wmm_param[ac_type].us_logcwmax;
        mac_vap->mib_info->wlan_mib_qap_edac[ac_type].dot11_qapedca_table_txop_limit = wmm_param[ac_type].txop_limit;
    }

    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        /* AP模式时广播给STA的EDCA参数，只在AP模式需要初始化此值，使用WLAN_VAP_MODE_BUTT， */
        wmm_param_sta = mac_get_wmm_cfg(WLAN_VAP_MODE_BUTT);

        for (ac_type = 0; ac_type < WLAN_WME_AC_BUTT; ac_type++) {
            mac_vap->mib_info->ast_wlan_mib_edca[ac_type].dot11_edca_table_aifsn = wmm_param_sta[ac_type].aifsn;
            mac_vap->mib_info->ast_wlan_mib_edca[ac_type].dot11_edca_table_c_wmin = wmm_param_sta[ac_type].logcwmin;
            mac_vap->mib_info->ast_wlan_mib_edca[ac_type].dot11_edca_table_c_wmax = wmm_param_sta[ac_type].us_logcwmax;
            mac_vap->mib_info->ast_wlan_mib_edca[ac_type].dot11_edca_table_txop_limit =
                wmm_param_sta[ac_type].txop_limit;
        }
    }
}

/* ****************************************************************************
 功能描述  : 增加关联用户，获取hash值并加入hash表中
 输入参数  : vap对象内存指针，以及user对象指针(user对象需要在调用此函数前申请并赋值)
 修改历史      :
  1.日    期   : 2012年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_add_assoc_user(mac_vap_stru *mac_vap, hi_u8 user_idx)
{
    mac_user_stru              *user = HI_NULL;
    mac_res_user_hash_stru     *hash = HI_NULL;
    hi_u32                  rslt;
    hi_u8                   hash_idx;
    hi_list                    *dlist_head = HI_NULL;
    unsigned long            irq_save;
    user = mac_user_get_user_stru(user_idx);
    if (oal_unlikely(user == HI_NULL)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_add_assoc_user::pst_user[%d] null.}", user_idx);
        return HI_ERR_CODE_PTR_NULL;
    }
    user->user_hash_idx = mac_calculate_hash_value(user->user_mac_addr);
    rslt = mac_res_alloc_hash(&hash_idx);
    if (rslt != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_add_assoc_user::alloc hash failed[%d].}", rslt);
        return rslt;
    }
    hash = mac_res_get_hash(hash_idx);

    if (mac_vap_user_exist(&(user->user_dlist), &(mac_vap->mac_user_list_head)) == HI_TRUE) {
        mac_res_free_hash(hash_idx);
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{mac_vap_add_assoc_user::user[%d] already exist.}", user_idx);
        return HI_ERR_CODE_PTR_NULL;
    }
    hash->us_hash_res_idx = hash_idx; /* 记录HASH对应的资源池索引值 */
    hash->user_idx = user_idx;        /* 记录对应的用户索引值 */
    dlist_head = &(mac_vap->ast_user_hash[user->user_hash_idx]);
    hi_list_head_insert(&(hash->entry), dlist_head);
    dlist_head = &(mac_vap->mac_user_list_head); /* 加入双向链表表头 */
    hi_list_head_insert(&(user->user_dlist), dlist_head);

    oal_spin_lock_irq_save(&mac_vap->cache_user_lock, &irq_save);

    if (memcpy_s(mac_vap->auc_cache_user_mac_addr, WLAN_MAC_ADDR_LEN, /* 更新cache user */
        user->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ASSOC, "{mac_vap_add_assoc_user::mem safe func err!}");
        oal_spin_unlock_irq_restore(&mac_vap->cache_user_lock, &irq_save);
        return HI_FAIL;
    }
    mac_vap->cache_user_id = user_idx;

    oal_spin_unlock_irq_restore(&mac_vap->cache_user_lock, &irq_save);

    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) { /* 记录STA模式下的与之关联的VAP的id */
        mac_vap_set_assoc_id(mac_vap, user_idx);
    }
    mac_vap->user_nums++; /* vap已关联 user个数++ */
    return HI_SUCCESS;
}

WIFI_ROM_TEXT hi_u32 mac_vap_user_remove_list(const hi_list *hash_head, mac_user_stru *user, hi_u8 user_idx)
{
    mac_user_stru          *user_temp = HI_NULL;
    hi_list                *entry = HI_NULL;
    mac_res_user_hash_stru *user_hash = HI_NULL;
    hi_list                *dlist_tmp = HI_NULL;
    hi_u32                  ret = HI_FAIL;

    hi_list_for_each_safe(entry, dlist_tmp, hash_head) {
        user_hash = (mac_res_user_hash_stru *)entry;
        user_temp = mac_user_get_user_stru(user_hash->user_idx);
        if (user_temp == HI_NULL) {
            continue;
        }
        if (!oal_compare_mac_addr(user->user_mac_addr, user_temp->user_mac_addr, WLAN_MAC_ADDR_LEN)) {
            hi_list_delete(entry);
            /* 从双向链表中拆掉 */
            hi_list_delete(&(user->user_dlist));
            mac_res_free_hash(user_hash->us_hash_res_idx);

            /* 初始化相应成员 */
            user->user_hash_idx = 0xff;
            user->us_assoc_id   = user_idx;
            user->is_multi_user = HI_FALSE;
            /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
            memset_s(user->user_mac_addr, WLAN_MAC_ADDR_LEN, 0, WLAN_MAC_ADDR_LEN);
            user->vap_id          = 0x0f;
            user->user_asoc_state = MAC_USER_STATE_BUTT;

            ret = HI_SUCCESS;
        }
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 删除用户，将用户从双向链表中删除，并从hash表中删除
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_del_user(mac_vap_stru *mac_vap, hi_u8 user_idx)
{
    mac_user_stru          *user = HI_NULL;
    hi_list                *hash_head = HI_NULL;
    hi_u32                 ret;
    unsigned long          irq_save;

    oal_spin_lock_irq_save(&mac_vap->cache_user_lock, &irq_save);

    if (user_idx == mac_vap->cache_user_id) { /* 与cache user id对比 , 相等则清空cache user */
        oal_set_mac_addr_zero(mac_vap->auc_cache_user_mac_addr);
        mac_vap->cache_user_id = MAC_INVALID_USER_ID;
    }

    oal_spin_unlock_irq_restore(&mac_vap->cache_user_lock, &irq_save);

    user = mac_user_get_user_stru(user_idx);
    if (oal_unlikely(user == HI_NULL)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{mac_vap_del_user::pst_user null, user_idx is %d}", user_idx);
        return HI_ERR_CODE_PTR_NULL;
    }
    mac_user_set_asoc_state(user, MAC_USER_STATE_BUTT);
    if (user->user_hash_idx >= MAC_VAP_USER_HASH_MAX_VALUE) {
        /* ADD USER命令丢失，或者重复删除User都可能进入此分支 */
        oam_error_log1(mac_vap->vap_id, OAM_SF_ASSOC, "{mac_vap_del_user::hash idx invalid %u}", user->user_hash_idx);
        return HI_FAIL;
    }

    hash_head = &(mac_vap->ast_user_hash[user->user_hash_idx]);
    ret = mac_vap_user_remove_list(hash_head, user, user_idx);
    if (ret == HI_SUCCESS) {
        /* vap已关联 user个数-- */
        if (mac_vap->user_nums) {
            mac_vap->user_nums--;
        }
        /* STA模式下将关联的VAP的id置为非法值 */
        if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
            mac_vap_set_assoc_id(mac_vap, 0xff);
        }
        return HI_SUCCESS;
    }

    oam_warning_log1(mac_vap->vap_id, OAM_SF_ASSOC,
        "{mac_vap_del_user::delete user failed,user idx is %d.}", user_idx);

    return HI_FAIL;
}

/* ****************************************************************************
 功能描述  : 根据user MAC地址查找user对象
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_find_user_by_macaddr(mac_vap_stru *mac_vap, const hi_u8 *sta_mac_addr, hi_u8 mac_addr_len,
    hi_u8 *puc_user_idx)
{
    mac_user_stru              *mac_user = HI_NULL;
    hi_u32                      user_hash_value;
    mac_res_user_hash_stru     *hash = HI_NULL;
    hi_list                    *entry = HI_NULL;
    unsigned long               irq_save;

    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        mac_user = mac_user_get_user_stru(mac_vap->assoc_vap_id);
        if (mac_user == HI_NULL) {
            return HI_FAIL;
        }
        if (!oal_compare_mac_addr(mac_user->user_mac_addr, sta_mac_addr, mac_addr_len)) {
            *puc_user_idx = mac_vap->assoc_vap_id;
            return (hi_u32)((*puc_user_idx != MAC_INVALID_USER_ID) ? HI_SUCCESS : HI_FAIL);
        }
        return HI_FAIL;
    }

    oal_spin_lock_irq_save(&mac_vap->cache_user_lock, &irq_save);

    /* 与cache user对比 , 相等则直接返回cache user id */
    if (!oal_compare_mac_addr(mac_vap->auc_cache_user_mac_addr, sta_mac_addr, mac_addr_len)) {
        *puc_user_idx = mac_vap->cache_user_id;
        oal_spin_unlock_irq_restore(&mac_vap->cache_user_lock, &irq_save);
        return (hi_u32)((*puc_user_idx != MAC_INVALID_USER_ID) ? HI_SUCCESS : HI_FAIL);
    }

    user_hash_value = mac_calculate_hash_value(sta_mac_addr);
    hi_list_for_each(entry, &(mac_vap->ast_user_hash[user_hash_value])) {
        hash = (mac_res_user_hash_stru *)entry;

        mac_user = mac_user_get_user_stru(hash->user_idx);
        if (mac_user == HI_NULL) {
            oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
                           "{mac_vap_find_user_by_macaddr::pst_mac_user null.user idx %d}",
                           hash->user_idx);
            continue;
        }

        /* 相同的MAC地址 */
        if (!oal_compare_mac_addr(mac_user->user_mac_addr, sta_mac_addr, mac_addr_len)) {
            *puc_user_idx = hash->user_idx;
            /* 更新cache user */
            if (memcpy_s(mac_vap->auc_cache_user_mac_addr, WLAN_MAC_ADDR_LEN, mac_user->user_mac_addr,
                WLAN_MAC_ADDR_LEN) != EOK) {
                oam_error_log0(0, 0, "{mac_vap_find_user_by_macaddr::mem safe func err!}");
                continue;
            }
            mac_vap->cache_user_id = (hi_u8)hash->user_idx;
            oal_spin_unlock_irq_restore(&mac_vap->cache_user_lock, &irq_save);
            return (hi_u32)((*puc_user_idx != MAC_INVALID_USER_ID) ? HI_SUCCESS : HI_FAIL);
        }
    }
    oal_spin_unlock_irq_restore(&mac_vap->cache_user_lock, &irq_save);

    return HI_FAIL;
}

/* ****************************************************************************
 功能描述  : 初始化11n的mib
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_vap_init_mib_11n(const mac_vap_stru *mac_vap)
{
    wlan_mib_ieee802dot11_stru    *mib_info = HI_NULL;
    mac_device_stru               *mac_dev = HI_NULL;
    mac_dev = mac_res_get_dev();
    mib_info = mac_vap->mib_info;
    mib_info->wlan_mib_sta_config.dot11_high_throughput_option_implemented = HI_FALSE;
    mib_info->phy_ht.dot11_ldpc_coding_option_implemented         = HI_FALSE;
    mib_info->phy_ht.dot11_ldpc_coding_option_activated           = HI_FALSE;
    mib_info->phy_ht.dot11_tx_stbc_option_activated               = HI_FALSE;
    mib_info->phy_ht.dot112_g_forty_m_hz_operation_implemented    = HI_FALSE;
    mib_info->phy_ht.dot11_short_gi_option_in_twenty_implemented  = HI_TRUE;
    mib_info->phy_ht.dot112_g_short_gi_option_in_forty_implemented = HI_FALSE;
    mib_info->phy_ht.dot11_tx_stbc_option_implemented = mac_dev->tx_stbc;
    mib_info->phy_ht.dot11_rx_stbc_option_implemented = mac_dev->rx_stbc;
    mib_info->wlan_mib_operation.dot11_obss_scan_passive_dwell = 20; /* obss被动扫描时每个信道的扫描时间 20 TUs */
    mib_info->wlan_mib_operation.dot11_obss_scan_active_dwell  = 10; /* 主动扫描时间 10TUs */
    /* 防止host默认参数与关联ap下发参数一致，不执行扫描,特将300s改小
        host配置obss 扫描间隔不低于300s故此处只要小于300即可 */
    mib_info->wlan_mib_operation.dot11_bss_width_trigger_scan_interval           = 299; /* 设置为299 */
    mib_info->wlan_mib_operation.dot11_obss_scan_passive_total_per_channel       = 200; /* 设置为200 */
    mib_info->wlan_mib_operation.dot11_obss_scan_active_total_per_channel        = 20;  /* 设置为20 */
    mib_info->wlan_mib_operation.dot11_bss_width_channel_transition_delay_factor = 5;   /* 设置为5 */
    mib_info->wlan_mib_operation.dot11_obss_scan_activity_threshold              = 25;  /* 设置为25 */
    mac_mib_init_2040(mac_vap);
}

/* ****************************************************************************
 功能描述  : 初始化11i mib信息
 输入参数  : pst_vap 指向VAP用户的指针
 修改历史      :
  1.日    期   : 2013年6月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_vap_init_mib_11i(const mac_vap_stru *mac_vap)
{
    /* WEP、WAPI加密方式时、IBSS下需要去使能 */
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated = HI_FALSE;
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpr = HI_FALSE;
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpc = HI_FALSE;
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_preauthentication_activated = HI_FALSE;
    mac_vap->mib_info->wlan_mib_privacy.dot11_privacy_invoked = HI_FALSE;
    /* Version信息为1 */
    /* see <80211-2012> chapter 8.4.2.27 RSN elemet  */
    /* RSN Version 1 is defined in this standard */
    mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_version = MAC_RSN_IE_VERSION;
    if (mac_vap->cap_flag.wpa2) {
        mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher = WLAN_80211_CIPHER_SUITE_CCMP;
        /* CCMP加密方式下GTK的bit位为256 */
        mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher_size = WLAN_CCMP_KEY_LEN * 8; /* 乘8转bit */
    }
    if (mac_vap->cap_flag.wpa) {
        mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher = WLAN_80211_CIPHER_SUITE_TKIP;
        /* TKIP加密方式下GTK的bit位为256 */
        mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher_size = WLAN_TKIP_KEY_LEN * 8; /* 乘8转bit */
    }
    mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_pairwise_cipher_requested       = 0;
    mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_group_cipher_requested          = 0;
    /* wpa PairwiseCipher CCMP */
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[0].dot11_rsna_config_pairwise_cipher_implemented =
        WLAN_80211_CIPHER_SUITE_CCMP;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[0].dot11_rsna_config_pairwise_cipher_activated =
        HI_FALSE;
    /* wpa PairwiseCipher TKIP */
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[1].dot11_rsna_config_pairwise_cipher_implemented =
        WLAN_80211_CIPHER_SUITE_TKIP;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[1].dot11_rsna_config_pairwise_cipher_activated =
        HI_FALSE;
    /* wpa2 PairwiseCipher CCMP */
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[0].dot11_rsna_config_pairwise_cipher_implemented =
        WLAN_80211_CIPHER_SUITE_CCMP;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[0].dot11_rsna_config_pairwise_cipher_activated =
        HI_FALSE;
    /* wpa2 PairwiseCipher TKIP */
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[1].dot11_rsna_config_pairwise_cipher_implemented =
        WLAN_80211_CIPHER_SUITE_TKIP;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[1].dot11_rsna_config_pairwise_cipher_activated =
        HI_FALSE;
    /* AuthenticationSuite */
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[0].dot11_rsna_config_authentication_suite_implemented =
        WLAN_AUTH_SUITE_PSK;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[0].dot11_rsna_config_authentication_suite_activated = HI_FALSE;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[1].dot11_rsna_config_authentication_suite_implemented =
        WLAN_AUTH_SUITE_PSK_SHA256;
    mac_vap->mib_info->ast_wlan_mib_rsna_cfg_auth_suite[1].dot11_rsna_config_authentication_suite_activated = HI_FALSE;
    mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_number_of_ptksa_replay_counters_implemented = 0;
    mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_number_of_gtksa_replay_counters_implemented = 0;
}

/* ****************************************************************************
功能描述  : 初始化11a 11g速率
修改历史      :
  1.日    期   : 2013年7月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_vap_init_legacy_rates(mac_vap_stru *mac_vap, mac_data_rate_stru *rates)
{
    hi_u8 rate_index;
    hi_u8 curr_rate_index = 0;
    mac_data_rate_stru *puc_orig_rate = HI_NULL;
    mac_data_rate_stru *puc_curr_rate = HI_NULL;

    /* 初始化速率个数，基本速率个数，非基本速率个数 */
    mac_vap->curr_sup_rates.rate.rs_nrates = MAC_NUM_DR_802_11G;
    mac_vap->curr_sup_rates.br_rate_num       = MAC_NUM_BR_802_11G;
    mac_vap->curr_sup_rates.nbr_rate_num      = MAC_NUM_NBR_802_11G;
    mac_vap->curr_sup_rates.min_rate          = 6;  /* 最小基本速率设置为6 */
    mac_vap->curr_sup_rates.max_rate          = 24; /* 最大基本速率设置为24 */
    /* 将速率拷贝到VAP结构体下的速率集中 */
    for (rate_index = 0; rate_index < MAC_DATARATES_PHY_80211G_NUM; rate_index++) {
        puc_orig_rate = &rates[rate_index];
        puc_curr_rate = &(mac_vap->curr_sup_rates.rate.ast_rs_rates[curr_rate_index]);
        /* Basic Rates */
        if ((puc_orig_rate->mbps == 6) || (puc_orig_rate->mbps == 12) || /* 判断mbps 6/12 */
            (puc_orig_rate->mbps == 24)) {                               /* 判断mbps 24 */
            if (memcpy_s(puc_curr_rate, sizeof(mac_data_rate_stru), puc_orig_rate,
                sizeof(mac_data_rate_stru)) != EOK) {
                continue;
            }
            puc_curr_rate->mac_rate |= 0x80;
            curr_rate_index++;
        } else if ((puc_orig_rate->mbps == 9) || (puc_orig_rate->mbps == 18) || /* 判断mbps 9/18 */
            (puc_orig_rate->mbps == 36) || (puc_orig_rate->mbps == 48) ||       /* 判断mbps 36/48 */
            (puc_orig_rate->mbps == 54)) {                                      /* 判断mbps 54 */
            /* Non-basic rates */
            if (memcpy_s(puc_curr_rate, sizeof(mac_data_rate_stru), puc_orig_rate,
                sizeof(mac_data_rate_stru)) != EOK) {
                return;
            }
            curr_rate_index++;
        }
        if (curr_rate_index == mac_vap->curr_sup_rates.rate.rs_nrates) {
            break;
        }
    }
}

/* ****************************************************************************
功能描述  : 初始化11b速率
修改历史      :
  1.日    期   : 2013年7月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_vap_init_11b_rates(mac_vap_stru *mac_vap, mac_data_rate_stru *rates)
{
    hi_u8 rate_index;
    hi_u8 curr_rate_index = 0;
    mac_data_rate_stru *puc_orig_rate = HI_NULL;
    mac_data_rate_stru *puc_curr_rate = HI_NULL;

    /* 初始化速率个数，基本速率个数，非基本速率个数 */
    mac_vap->curr_sup_rates.rate.rs_nrates = MAC_NUM_DR_802_11B;
    mac_vap->curr_sup_rates.br_rate_num       = 0;
    mac_vap->curr_sup_rates.nbr_rate_num      = MAC_NUM_NBR_802_11B;
    mac_vap->curr_sup_rates.min_rate          = 1;
    mac_vap->curr_sup_rates.max_rate          = 2; /* 最大基本速率设置为2 */
    /* 将速率拷贝到VAP结构体下的速率集中 */
    for (rate_index = 0; rate_index < MAC_DATARATES_PHY_80211G_NUM; rate_index++) {
        puc_orig_rate = &rates[rate_index];
        puc_curr_rate = &(mac_vap->curr_sup_rates.rate.ast_rs_rates[curr_rate_index]);
        /*  Basic Rates  */
        if ((puc_orig_rate->mbps == 1) || (puc_orig_rate->mbps == 2) || /* mbps 1/2 为Basic Rates */
            ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) &&
            ((puc_orig_rate->mbps == 5) || (puc_orig_rate->mbps == 11)))) { /* mbps 5/11 为Basic Rates */
            mac_vap->curr_sup_rates.br_rate_num++;
            if (memcpy_s(puc_curr_rate, sizeof(mac_data_rate_stru), puc_orig_rate, sizeof(mac_data_rate_stru)) != EOK) {
                oam_error_log0(0, 0, "{mac_vap_init_11b_rates::mem safe func err!}");
                continue;
            }
            puc_curr_rate->mac_rate |= 0x80;
            curr_rate_index++;
        } else if (((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) /* Non-basic rates */
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
            || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
        ) && ((puc_orig_rate->mbps == 5) || (puc_orig_rate->mbps == 11))) { /* 判断mbps 5/11 */
            if (memcpy_s(puc_curr_rate, sizeof(mac_data_rate_stru), puc_orig_rate, sizeof(mac_data_rate_stru)) != EOK) {
                oam_error_log0(0, 0, "{mac_vap_init_11b_rates::mem safe func err!}");
                continue;
            }
            /* 初始化11b速率时，将高位置0 */
            puc_curr_rate->mac_rate &= 0x7f;
            curr_rate_index++;
        } else {
            continue;
        }
        if (curr_rate_index == mac_vap->curr_sup_rates.rate.rs_nrates) {
            break;
        }
    }
}

/* ****************************************************************************
功能描述  : 初始化11g 混合1速率
修改历史      :
  1.日    期   : 2013年7月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_vap_init_11g_mixed_one_rates(mac_vap_stru *mac_vap, mac_data_rate_stru *rates)
{
    hi_u8 rate_index;
    mac_data_rate_stru *puc_orig_rate = HI_NULL;
    mac_data_rate_stru *puc_curr_rate = HI_NULL;
    hi_u8 rates_num;

    /* 初始化速率集 */
    rates_num = MAC_DATARATES_PHY_80211G_NUM;
    /* 初始化速率个数，基本速率个数，非基本速率个数 */
    mac_vap->curr_sup_rates.rate.rs_nrates = MAC_NUM_DR_802_11G_MIXED;
    mac_vap->curr_sup_rates.br_rate_num       = MAC_NUM_BR_802_11G_MIXED_ONE;
    mac_vap->curr_sup_rates.nbr_rate_num      = MAC_NUM_NBR_802_11G_MIXED_ONE;
    mac_vap->curr_sup_rates.min_rate          = 1;
    mac_vap->curr_sup_rates.max_rate          = 11; /* max_rate 设置为11 */
    /* 将速率拷贝到VAP结构体下的速率集中 */
    for (rate_index = 0; rate_index < rates_num; rate_index++) {
        puc_orig_rate = &rates[rate_index];
        puc_curr_rate = &(mac_vap->curr_sup_rates.rate.ast_rs_rates[rate_index]);
        if (memcpy_s(puc_curr_rate, sizeof(mac_data_rate_stru), puc_orig_rate, sizeof(mac_data_rate_stru)) != EOK) {
            continue;
        }
        /* Basic Rates */
        if ((puc_orig_rate->mbps == 1) || (puc_orig_rate->mbps == 2)        /* mbps 1/2 为Basic Rates */
            || (puc_orig_rate->mbps == 5) || (puc_orig_rate->mbps == 11)) { /* mbps 5/11 为Basic Rates */
            puc_curr_rate->mac_rate |= 0x80;
        }
    }
}

/* ****************************************************************************
功能描述  : 初始化11g mixed two速率
修改历史      :
  1.日    期   : 2013年7月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_vap_init_11g_mixed_two_rates(mac_vap_stru *mac_vap, mac_data_rate_stru *rates)
{
    hi_u8 rate_index;
    mac_data_rate_stru *puc_orig_rate = HI_NULL;
    mac_data_rate_stru *puc_curr_rate = HI_NULL;

    /* 初始化速率个数，基本速率个数，非基本速率个数 */
    mac_vap->curr_sup_rates.rate.rs_nrates = MAC_NUM_DR_802_11G_MIXED;
    mac_vap->curr_sup_rates.br_rate_num       = MAC_NUM_BR_802_11G_MIXED_TWO;
    mac_vap->curr_sup_rates.nbr_rate_num      = MAC_NUM_NBR_802_11G_MIXED_TWO;
    mac_vap->curr_sup_rates.min_rate          = 1;
    mac_vap->curr_sup_rates.max_rate          = 24; /* max_rate设置为24 */
    /* 将速率拷贝到VAP结构体下的速率集中 */
    for (rate_index = 0; rate_index < MAC_DATARATES_PHY_80211G_NUM; rate_index++) {
        puc_orig_rate = &rates[rate_index];
        puc_curr_rate = &(mac_vap->curr_sup_rates.rate.ast_rs_rates[rate_index]);
        if (memcpy_s(puc_curr_rate, sizeof(mac_data_rate_stru), puc_orig_rate, sizeof(mac_data_rate_stru)) != EOK) {
            continue;
        }
        /* Basic Rates */
        if ((puc_orig_rate->mbps == 1) || (puc_orig_rate->mbps == 2) ||  /* mbps 1/2 为Basic Rates */
            (puc_orig_rate->mbps == 5) || (puc_orig_rate->mbps == 11) || /* mbps 5/11 为Basic Rates */
            (puc_orig_rate->mbps == 6) || (puc_orig_rate->mbps == 12)    /* mbps 6/12 为Basic Rates */
            || (puc_orig_rate->mbps == 24)) {                            /* mbps 24 为Basic Rates */
            puc_curr_rate->mac_rate |= 0x80;
        }
    }
}

/* ****************************************************************************
功能描述  : 初始化11n速率
修改历史      :
  1.日    期   : 2013年7月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_vap_init_11n_rates(const mac_vap_stru *mac_vap)
{
    wlan_mib_ieee802dot11_stru *mib_info = HI_NULL;

    mib_info = mac_vap->mib_info;
    /* MCS相关MIB值初始化 */
    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(mib_info->supported_mcsrx.auc_dot11_supported_mcs_rx_value, WLAN_HT_MCS_BITMASK_LEN, 0,
        WLAN_HT_MCS_BITMASK_LEN);
    mib_info->supported_mcsrx.auc_dot11_supported_mcs_rx_value[0] = 0xFF; /* 支持 RX MCS 0-7，8位全置为1 */
    mib_info->supported_mcstx.auc_dot11_supported_mcs_tx_value[0] = 0xFF; /* 支持 TX MCS 0-7，8位全置为1 */
}

/* ****************************************************************************
 功能描述  : 初始化p2p vap的速率集
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_init_p2p_rates(mac_vap_stru *mac_vap, mac_data_rate_stru *rates)
{
    mac_vap_init_legacy_rates(mac_vap, rates);
    mac_vap_init_11n_rates(mac_vap);
}

WIFI_ROM_TEXT hi_void mac_vap_init_rates_by_protocol(mac_vap_stru *mac_vap, wlan_protocol_enum_uint8 vap_protocol,
    mac_data_rate_stru *rates)
{
    /* STA模式默认协议模式是11n，初始化速率集为所有速率集 */
#ifdef _PRE_WLAN_FEATURE_P2P_ROM
    if (!is_legacy_vap(mac_vap)) {
        mac_vap_init_p2p_rates(mac_vap, rates);
        return;
    }
#endif
    if ((vap_protocol == WLAN_HT_ONLY_MODE) || (vap_protocol == WLAN_HT_MODE)) {
        mac_vap_init_11g_mixed_one_rates(mac_vap, rates);
        mac_vap_init_11n_rates(mac_vap);
    } else if (vap_protocol == WLAN_LEGACY_11G_MODE) {
        mac_vap_init_legacy_rates(mac_vap, rates);
    } else if (vap_protocol == WLAN_LEGACY_11B_MODE) {
        mac_vap_init_11b_rates(mac_vap, rates);
    } else if (vap_protocol == WLAN_MIXED_ONE_11G_MODE) {
        mac_vap_init_11g_mixed_one_rates(mac_vap, rates);
    } else if (vap_protocol == WLAN_MIXED_TWO_11G_MODE) {
        mac_vap_init_11g_mixed_two_rates(mac_vap, rates);
    } else {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_init_rates_by_protocol::protocol[%d] isn't supportted.}",
            vap_protocol);
    }
}

/* ****************************************************************************
功能描述  : 初始化速率集
修改历史      :
 1.日    期   : 2013年7月30日
    作    者   : HiSilicon
   修改内容   : 新生成函数
 2.日    期   : 2013年11月6日
    作    者   : HiSilicon
   修改内容   : 修改不同协议模式速率初始化

**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_init_rates(mac_vap_stru *mac_vap)
{
    mac_device_stru *mac_dev = HI_NULL;
    wlan_protocol_enum_uint8 vap_protocol;
    mac_data_rate_stru *rates = HI_NULL;

    mac_dev = mac_res_get_dev();
    /* 初始化速率集 */
    rates = &mac_dev->mac_rates_11g[0];
    vap_protocol = mac_vap->protocol;
    mac_vap_init_rates_by_protocol(mac_vap, vap_protocol, rates);
}

#ifdef _PRE_WLAN_FEATURE_MESH_ROM

/* ****************************************************************************
功能描述  : 根据wpa下发的mesh配置参数设置Mesh相关Mib值
                            当前只支持设置auth protocol
输入参数  :mac_vap_stru *pst_vap
                            hi_u8 uc_mesh_formation_info
                            hi_u8 uc_mesh_capability
修改历史      :
 1.日    期   : 2019年3月19日
    作    者   : HiSilicon
   修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_set_mib_mesh(const mac_vap_stru *mac_vap, hi_u8 mesh_auth_protocol)
{
    /* 先清空之前的信息 */
    mac_mib_set_mesh_security(mac_vap, HI_FALSE);
    mac_mib_clear_mesh_auth_protocol(mac_vap);

    if (mesh_auth_protocol == 0) {
        mac_mib_set_mesh_security(mac_vap, HI_FALSE);
        mac_mib_set_mesh_auth_protocol(mac_vap, 0);
    } else {
        mac_mib_set_mesh_security(mac_vap, HI_TRUE);
        mac_mib_set_mesh_auth_protocol(mac_vap, mesh_auth_protocol);
    }

    oam_warning_log2(mac_vap->vap_id, OAM_SF_ANY,
        "{mac_vap_set_mib_mesh::meshSecurityActivated = %d , meshactiveAuthenticationProtocol = %d}",
        mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_security_activated,
        mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_active_authentication_protocol);
}
#endif

/* ****************************************************************************
 功能描述  : 删除P2P vap num的功能函数
 修改历史      :
  1.日    期   : 2014年11月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_dec_p2p_num(const mac_vap_stru *mac_vap)
{
    mac_device_stru *mac_dev = HI_NULL;

    mac_dev = mac_res_get_dev();
    if (is_p2p_dev(mac_vap)) {
        mac_dev->p2p_info.p2p_device_num--;
    } else if (is_p2p_go(mac_vap) || is_p2p_cl(mac_vap)) {
        mac_dev->p2p_info.p2p_goclient_num--;
    }
}

/* ****************************************************************************
 功能描述  : add p2p vap时同步增加p2p设备的计数器
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_inc_p2p_num(const mac_vap_stru *mac_vap)
{
    mac_device_stru *mac_dev = HI_NULL;

    mac_dev = mac_res_get_dev();
    if (is_p2p_dev(mac_vap)) {
        /* device下sta个数加1 */
        mac_dev->sta_num++;
        mac_dev->p2p_info.p2p_device_num++;
    } else if (is_p2p_go(mac_vap)) {
        mac_dev->p2p_info.p2p_goclient_num++;
    } else if (is_p2p_cl(mac_vap)) {
        mac_dev->p2p_info.p2p_goclient_num++;
    }
}

/* ****************************************************************************
功能描述  : 将用户态 IE 信息拷贝到内核态中
输入参数  : mac_vap_stru *pst_mac_vap
          oal_net_dev_ioctl_data_stru *pst_ioctl_data
          enum WPS_IE_TYPE en_type
修改历史      :
 1.日    期   : 2015年4月28日
    作    者   : HiSilicon
  修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_save_app_ie(mac_vap_stru *mac_vap, const oal_app_ie_stru *app_ie,
    en_app_ie_type_uint8 type)
{
    hi_u8 *puc_ie = HI_NULL;
    hi_u32 ie_len;

    if (type >= OAL_APP_IE_NUM) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{mac_vap_save_app_ie::invalid en_type[%d].}", type);
        return HI_ERR_CODE_INVALID_CONFIG;
    }

    ie_len = app_ie->ie_len;
    /* 如果输入WPS 长度为0， 则直接释放VAP 中资源 */
    if (ie_len == 0) {
        if (mac_vap->ast_app_ie[type].puc_ie != HI_NULL) {
            oal_mem_free(mac_vap->ast_app_ie[type].puc_ie);
        }
        mac_vap->ast_app_ie[type].puc_ie = HI_NULL;
        mac_vap->ast_app_ie[type].ie_len = 0;
        return HI_SUCCESS;
    }
    /* 检查该类型的IE是否需要申请内存 */
    if ((mac_vap->ast_app_ie[type].ie_len < ie_len) || (mac_vap->ast_app_ie[type].puc_ie == HI_NULL)) {
        /* 如果以前的内存空间小于新信息元素需要的长度，则需要重新申请内存 */
        puc_ie = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, (hi_u16)(ie_len));
        if (puc_ie == HI_NULL) {
            oam_warning_log2(mac_vap->vap_id, OAM_SF_CFG,
                "{mac_vap_set_app_ie::LOCAL_MEM_POOL is empty!,len[%d], en_type[%d].}", app_ie->ie_len, type);
            return HI_ERR_CODE_ALLOC_MEM_FAIL;
        }
        if (mac_vap->ast_app_ie[type].puc_ie != HI_NULL) {
            oal_mem_free(mac_vap->ast_app_ie[type].puc_ie);
        }
        mac_vap->ast_app_ie[type].puc_ie = puc_ie;
    }
    if (memcpy_s((hi_void *)mac_vap->ast_app_ie[type].puc_ie, ie_len, (hi_void *)app_ie->auc_ie, ie_len) != EOK) {
        if (puc_ie != HI_NULL) {
            oal_mem_free(puc_ie);
            mac_vap->ast_app_ie[type].puc_ie = HI_NULL;
        }
        return HI_FAIL;
    }
    mac_vap->ast_app_ie[type].ie_len = ie_len;
    return HI_SUCCESS;
}

/* ****************************************************************************
功能描述  : mac vap exit
修改历史      :
 1.日    期   : 2013年5月29日
   作    者   : HiSilicon
   修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_exit(mac_vap_stru *mac_vap)
{
    mac_device_stru *mac_dev = HI_NULL;
    hi_u8 index;

    /* 释放与hmac有关的内存 */
    mac_vap_free_mib(mac_vap);
    /* 释放WPS信息元素内存 */
    for (index = 0; index < OAL_APP_IE_NUM; index++) {
        mac_vap_clear_app_ie(mac_vap, index);
    }
    /* 业务vap已删除，从device上去掉 */
    mac_dev = mac_res_get_dev();
    /* 业务vap已经删除，从device中去掉 */
    for (index = 0; index < mac_dev->vap_num; index++) {
        /* 从device中找到vap id */
        if (mac_dev->auc_vap_id[index] == mac_vap->vap_id) {
            /* 如果不是最后一个vap，则把最后一个vap id移动到这个位置，使得该数组是紧凑的 */
            if (index < (mac_dev->vap_num - 1)) {
                mac_dev->auc_vap_id[index] = mac_dev->auc_vap_id[mac_dev->vap_num - 1];
                break;
            }
        }
    }
    /* device下的vap总数减1 */
    mac_dev->vap_num--;
    /* 清除数组中已删除的vap id，保证非零数组元素均为未删除vap */
    mac_dev->auc_vap_id[mac_dev->vap_num] = 0;
    /* device下sta个数减1 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        mac_dev->sta_num--;
    }
#ifdef _PRE_WLAN_FEATURE_P2P_ROM
    mac_dec_p2p_num(mac_vap);
#endif
    mac_vap->protocol = WLAN_PROTOCOL_BUTT;
    /* 最后1个vap删除时，清除device级带宽信息 */
    if (mac_dev->vap_num == 0) {
        mac_dev->max_channel   = 0;
        mac_dev->max_band      = WLAN_BAND_BUTT;
        mac_dev->max_bandwidth = WLAN_BAND_WIDTH_BUTT;
    }
    /* 删除之后将vap的状态置位非法 */
    mac_vap_state_change(mac_vap, MAC_VAP_STATE_BUTT);
}

/* ****************************************************************************
 功能描述  : mib初始化函数
 修改历史      :
  1.日    期   : 2013年7月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2013年11月6日
    作    者   : HiSilicon
    修改内容   : 增加HT only和VHT only协议模式的初始化
**************************************************************************** */
WIFI_ROM_TEXT static hi_void mac_init_mib(const mac_vap_stru *mac_vap)
{
    wlan_mib_ieee802dot11_stru *mib_info = HI_NULL;
    hi_u8 idx;

    mib_info = mac_vap->mib_info;
    /* 公共特性mib值初始化 */
    mib_info->wlan_mib_sta_config.dot11_dtim_period             = WLAN_DTIM_DEFAULT;
    mib_info->wlan_mib_operation.dot11_rts_threshold            = WLAN_RTS_MAX;
    mib_info->wlan_mib_operation.dot11_fragmentation_threshold  = WLAN_FRAG_THRESHOLD_MAX;
    mib_info->wlan_mib_sta_config.dot11_desired_bss_type         = WLAN_MIB_DESIRED_BSSTYPE_INFRA;
    mib_info->wlan_mib_sta_config.dot11_beacon_period           = WLAN_BEACON_INTVAL_DEFAULT;
    mib_info->phy_hrdsss.dot11_short_preamble_option_implemented  = WLAN_LEGACY_11B_MIB_SHORT_PREAMBLE;
    mib_info->phy_hrdsss.dot11_pbcc_option_implemented           = HI_FALSE;
    mib_info->phy_hrdsss.dot11_channel_agility_present           = HI_FALSE;
    mib_info->wlan_mib_sta_config.dot11_multi_domain_capability_activated = HI_TRUE;
    mib_info->wlan_mib_sta_config.dot11_spectrum_management_required = HI_TRUE;
    mib_info->wlan_mib_sta_config.dot11_extended_channel_switch_activated = HI_FALSE;
    mib_info->wlan_mib_sta_config.dot11_qos_option_implemented   = HI_TRUE;
    mib_info->wlan_mib_sta_config.dot11_apsd_option_implemented  = HI_FALSE;
    mib_info->wlan_mib_sta_config.dot11_qbss_load_implemented    = HI_TRUE;
    mib_info->wlan_mib_sta_config.dot11_radio_measurement_activated = HI_FALSE;
    mib_info->wlan_mib_sta_config.dot11_immediate_block_ack_option_implemented = HI_TRUE;
    mib_info->wlan_mib_sta_config.dot11_delayed_block_ack_option_implemented   = HI_FALSE;
    mib_info->wlan_mib_sta_config.dot11_authentication_response_time_out = WLAN_AUTH_TIMEOUT;
    mib_info->wlan_mib_operation.dot11_ht_protection = WLAN_MIB_HT_NO_PROTECTION;
    mib_info->wlan_mib_operation.dot11_rifs_mode = HI_TRUE;
    mib_info->wlan_mib_operation.dot11_non_gf_entities_present = HI_FALSE;
    mib_info->wlan_mib_operation.dot11_lsigtxop_full_protection_activated = HI_TRUE;
    mib_info->wlan_mib_sta_config.dot11_association_response_time_out = WLAN_ASSOC_TIMEOUT;
    mib_info->wlan_mib_sta_config.dot11_association_sa_query_maximum_timeout = WLAN_SA_QUERY_RETRY_TIME;
    mib_info->wlan_mib_sta_config.dot11_association_sa_query_retry_timeout   = WLAN_SA_QUERY_MAXIMUM_TIME;
    /* 认证算法表初始化 */
    mib_info->wlan_mib_auth_alg.dot11_authentication_algorithm = WLAN_WITP_AUTH_OPEN_SYSTEM;
    mib_info->wlan_mib_auth_alg.dot11_authentication_algorithms_activated = HI_FALSE;
    /* WEP 缺省Key表初始化 */
    for (idx = 0; idx < WLAN_NUM_DOT11WEPDEFAULTKEYVALUE; idx++) {
        mib_info->ast_wlan_mib_wep_dflt_key[idx].auc_dot11_wep_default_key_value[WLAN_WEP_SIZE_OFFSET] =
            40; /* 大小初始化为 WEP-40  */
    }
    /*    相关私有表初始化  */
    mib_info->wlan_mib_privacy.dot11_privacy_invoked = HI_FALSE;
    mib_info->wlan_mib_privacy.dot11_wep_default_key_id = 0;
    mac_vap_init_wme_param(mac_vap);
    mac_vap_init_mib_11i(mac_vap);
    /* 默认11n 11ac使能关闭，配置协议模式时打开 */
    mac_vap_init_mib_11n(mac_vap);
    mib_info->wlan_mib_sta_config.dot11_power_management_mode = WLAN_MIB_PWR_MGMT_MODE_ACTIVE;
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    mac_vap_init_mib_mesh(mac_vap);
#endif
}

/* ****************************************************************************
 功能描述  : 依据VAP 协议模式，初始化vap HT/VHT 相应 MIB 能力
 输入参数  : pst_mac_vap: 指向vap
 修改历史      :
  1.日    期   : 2013年12月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_config_vht_ht_mib_by_protocol(const mac_vap_stru *mac_vap)
{
    if (mac_vap->mib_info == HI_NULL) {
        oam_error_log3(mac_vap->vap_id, OAM_SF_ASSOC,
            "{mac_vap_config_vht_ht_mib_by_protocol::pst_mib_info null,vap mode[%d] state[%d] user num[%d].}",
            mac_vap->vap_mode, mac_vap->vap_state, mac_vap->user_nums);
        return HI_FAIL;
    }
    if ((mac_vap->protocol == WLAN_HT_MODE) || (mac_vap->protocol == WLAN_HT_ONLY_MODE)) {
        mac_vap->mib_info->wlan_mib_sta_config.dot11_high_throughput_option_implemented = HI_TRUE;
    } else {
        mac_vap->mib_info->wlan_mib_sta_config.dot11_high_throughput_option_implemented = HI_FALSE;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 依据协议初始化vap相应能力
 输入参数  : pst_mac_vap: 指向vap
             en_protocol: 协议枚举 调用函数  :
 修改历史      :
  1.日    期   : 2013年11月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_init_by_protocol(mac_vap_stru *mac_vap, wlan_protocol_enum_uint8 protocol)
{
    mac_vap->protocol = protocol;
    if (protocol < WLAN_HT_MODE) {
        mac_vap_cap_init_legacy(mac_vap);
    } else {
        mac_vap_cap_init_htvht(mac_vap);
    }
    /* 根据协议模式更新mib值 */
    if (mac_vap_config_vht_ht_mib_by_protocol(mac_vap) != HI_SUCCESS) {
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 根据带宽更改vap的mib值
 修改历史      :
  1.日    期   : 2014年5月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_change_mib_by_bandwidth(const mac_vap_stru *mac_vap,
    wlan_channel_bandwidth_enum_uint8 bandwidth)
{
    wlan_mib_ieee802dot11_stru *mib_info = HI_NULL;

    mib_info = mac_vap->mib_info;
    if (mib_info == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_change_mib_by_bandwidth::pst_mib_info null.}");
        return;
    }

    /* 更新short gi使能mib, 默认全使能，根据带宽信息更新 */
    mib_info->phy_ht.dot11_short_gi_option_in_twenty_implemented = HI_TRUE;
    if (WLAN_BAND_WIDTH_20M == bandwidth) {
        mac_mib_set_forty_mhz_operation_implemented(mac_vap, HI_FALSE);
        mac_mib_set_shortgi_option_in_forty_implemented(mac_vap, HI_FALSE);
    }
}

WIFI_ROM_TEXT static hi_void mac_vap_stru_mesh_info(mac_vap_stru *mac_vap)
{
    mac_vap->cap_flag.hide_meshid = HI_TRUE;
    mac_vap->report_times_limit = MAC_MAX_REPORT_TIME;
    mac_vap->report_tx_cnt_limit = MAC_MAX_REPORT_TX_CNT;
    mac_vap->priority = 0x0;             /* 初始化为最低优先级 */
    mac_vap->mnid = MAC_MESH_INVALID_ID; /* 初始化为无效值 */
    mac_vap->is_mbr = HI_FALSE;
    mac_vap->mesh_accept_sta = HI_FALSE;     /* 初始Mesh未加入组网，不支持STA加入 */
    mac_vap->mesh_tbtt_adjusting = HI_FALSE; /* 初始Mesh未在调整tbtt */
}

WIFI_ROM_TEXT static hi_u32 mac_vap_init_mode(mac_vap_stru *mac_vap, const mac_cfg_add_vap_param_stru *param)
{
    switch (mac_vap->vap_mode) {
        case WLAN_VAP_MODE_CONFIG:
            return HI_SUCCESS;
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
        case WLAN_VAP_MODE_MESH:
            mac_vap_stru_mesh_info(mac_vap);
#endif
        /* fall-through */
        case WLAN_VAP_MODE_BSS_STA:
        case WLAN_VAP_MODE_BSS_AP:
            mac_vap->assoc_vap_id = 0xff;
            mac_vap->tx_power = WLAN_MAX_TXPOWER;
            mac_vap->protection.protection_mode = WLAN_PROT_NO;
            mac_vap->cap_flag.dsss_cck_mode_40mhz = HI_FALSE;
            mac_vap->cap_flag.uapsd = HI_FALSE;
#ifdef _PRE_WLAN_FEATURE_UAPSD
            if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
                || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
            ) {
                mac_vap->cap_flag.uapsd = param->uapsd_enable;
            }
#endif
            /* 初始化dpd能力 */
            mac_vap->cap_flag.dpd_enbale = HI_TRUE;
            mac_vap->cap_flag.keepalive  = HI_TRUE;
            mac_vap->channel.band         = WLAN_BAND_BUTT;
            mac_vap->channel.en_bandwidth    = WLAN_BAND_WIDTH_BUTT;
            mac_vap->protocol  = WLAN_PROTOCOL_BUTT;
            /* 设置自动保护开启 */
            mac_vap->protection.auto_protection = HI_SWITCH_ON;
            /* 设置VAP状态为初始状态INIT */
            mac_vap_state_change(mac_vap, MAC_VAP_STATE_INIT);
            /* 清mac vap下的uapsd的状态,否则状态会有残留，导致host device uapsd信息不同步 */
#ifdef _PRE_WLAN_FEATURE_STA_PM
            /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
            memset_s(&(mac_vap->sta_uapsd_cfg), sizeof(mac_cfg_uapsd_sta_stru), 0, sizeof(mac_cfg_uapsd_sta_stru));
#endif
            break;
        default:
            oam_warning_log1(0, OAM_SF_ANY, "{mac_vap_init_mode::invalid vap mode[%d].}", mac_vap->vap_mode);
            return HI_ERR_CODE_INVALID_CONFIG;
    }
    return HI_SUCCESS;
}

WIFI_ROM_TEXT static hi_u32 mac_vap_init_mib(mac_vap_stru *mac_vap, hi_u8 vap_id)
{
    mac_device_stru *mac_dev = mac_res_get_dev();
    wlan_mib_ieee802dot11_stru *mib_info = HI_NULL;

    /* 申请MIB内存空间，配置VAP没有MIB */
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) || (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) ||
        (mac_vap->vap_mode == WLAN_VAP_MODE_WDS)
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        mac_vap->mib_info = oal_mem_alloc(OAL_MEM_POOL_ID_MIB, sizeof(wlan_mib_ieee802dot11_stru));
        if (mac_vap->mib_info == HI_NULL) {
            oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_init_mib::pst_mib_info alloc null, size[%d].}",
                sizeof(wlan_mib_ieee802dot11_stru));
            return HI_ERR_CODE_ALLOC_MEM_FAIL;
        }
        mib_info = mac_vap->mib_info;
        /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
        memset_s(mib_info, sizeof(wlan_mib_ieee802dot11_stru), 0, sizeof(wlan_mib_ieee802dot11_stru));
        if (memcpy_s(mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN, mac_dev->auc_hw_addr,
            WLAN_MAC_ADDR_LEN) != EOK) {
            mac_vap_free_mib(mac_vap);
            return HI_FAIL;
        }
        /* VAP的mac地址为device地址的最后一个值+vap id */
        mib_info->wlan_mib_sta_config.auc_dot11_station_id[WLAN_MAC_ADDR_LEN - 1] += vap_id;
        mac_init_mib(mac_vap);
        /* sta以最大能力启用 */
        if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
            mac_vap->protocol = WLAN_HT_MODE;
            mac_vap->channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
            mac_vap->channel.band = WLAN_BAND_2G;
            if (HI_SUCCESS != mac_vap_init_by_protocol(mac_vap, WLAN_HT_MODE)) {
                mac_vap_free_mib(mac_vap);
                return HI_ERR_CODE_INVALID_CONFIG;
            }
            mac_vap_init_rates(mac_vap);
        }
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : mac vap init
 修改历史      :
  1.日    期   : 2013年5月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_init(mac_vap_stru *mac_vap, hi_u8 vap_id, const mac_cfg_add_vap_param_stru *param)
{
    hi_u32 loop;
    hi_u32 ret;

    if (memset_s(mac_vap, sizeof(mac_vap_stru), 0, sizeof(mac_vap_stru)) != EOK) {
        return HI_FAIL;
    }

    mac_vap->vap_id      = vap_id;
    mac_vap->vap_mode    = param->vap_mode;
    mac_vap->cap_flag.disable_2ght40 = HI_TRUE;
    mac_vap->ch_switch_info.new_bandwidth = WLAN_BAND_WIDTH_BUTT;

    for (loop = 0; loop < MAC_VAP_USER_HASH_MAX_VALUE; loop++) {
        hi_list_init(&(mac_vap->ast_user_hash[loop]));
    }
    hi_list_init(&mac_vap->mac_user_list_head);

    ret = mac_vap_init_mode(mac_vap, param);
    if (ret != HI_SUCCESS) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_init_mode fail.}");
        return ret;
    }

    ret = mac_vap_init_mib(mac_vap, vap_id);
    if (ret != HI_SUCCESS) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_init_mib fail.}");
        return ret;
    }
    oal_spin_lock_init(&mac_vap->cache_user_lock);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : add wep key逻辑，抛事件到DMAC
 输入参数  : mac_vap_stru *pst_mac_vap, hi_u16 us_len, hi_u8 *puc_param
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年5月12日
    作    者   : HiSilicon
    修改内容   : wep密钥保存在组播用户中
**************************************************************************** */
WIFI_ROM_TEXT static hi_u32 mac_vap_add_wep_key(const mac_vap_stru *mac_vap, hi_u16 us_len, hi_u8 *puc_param)
{
    mac_wep_key_param_stru    *wep_addkey_params = HI_NULL;
    mac_user_stru             *multi_user        = HI_NULL;
    wlan_priv_key_param_stru  *wep_key           = HI_NULL;
    hi_u32                     cipher_type        = WLAN_CIPHER_SUITE_WEP40;
    hi_u8                      wep_cipher_type    = WLAN_80211_CIPHER_SUITE_WEP_40;

    hi_unref_param(us_len);

    wep_addkey_params = (mac_wep_key_param_stru *)puc_param;
    /* wep 密钥最大为4个 */
    if (wep_addkey_params->key_index >= WLAN_MAX_WEP_KEY_COUNT) {
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }
    switch (wep_addkey_params->key_len) {
        case WLAN_WEP40_KEY_LEN:
            wep_cipher_type = WLAN_80211_CIPHER_SUITE_WEP_40;
            cipher_type = WLAN_CIPHER_SUITE_WEP40;
            break;
        case WLAN_WEP104_KEY_LEN:
            wep_cipher_type = WLAN_80211_CIPHER_SUITE_WEP_104;
            cipher_type = WLAN_CIPHER_SUITE_WEP104;
            break;
        default:
            return HI_ERR_CODE_SECURITY_KEY_LEN;
    }
    /* WEP密钥信息记录到组播用户中 */
    multi_user = mac_user_get_user_stru(mac_vap->multi_user_idx);
    if (multi_user == HI_NULL) {
        return HI_ERR_CODE_SECURITY_USER_INVAILD;
    }
    mac_mib_set_privacyinvoked(mac_vap, HI_TRUE);
    /* 初始化组播用户的安全信息 */
    if (wep_addkey_params->default_key) {
        multi_user->key_info.cipher_type     = wep_cipher_type;
        multi_user->key_info.default_index   = wep_addkey_params->key_index;
        multi_user->key_info.igtk_key_index  = 0xff; /* wep时设置为无效 */
        multi_user->key_info.gtk            = 0;
    }

    wep_key   = &multi_user->key_info.ast_key[wep_addkey_params->key_index];
    wep_key->cipher        = cipher_type;
    wep_key->key_len       = (hi_u32)wep_addkey_params->key_len;

    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(wep_key->auc_key, WLAN_WPA_KEY_LEN, 0, WLAN_WPA_KEY_LEN);
    if (memcpy_s(wep_key->auc_key, WLAN_WPA_KEY_LEN, wep_addkey_params->auc_wep_key,
                 wep_addkey_params->key_len) != EOK) {
        return HI_FAIL;
    }
    multi_user->user_tx_info.security.cipher_key_type = wep_addkey_params->key_index + HAL_KEY_TYPE_PTK;
    multi_user->user_tx_info.security.cipher_protocol_type = wep_cipher_type;
    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_PMF
/* ****************************************************************************
 功能描述  : 获取pmf的配置能力
 修改历史      :
  1.日    期   : 2015年2月7日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_u32 mac_vap_init_pmf(mac_vap_stru *mac_vap,
    const mac_cfg80211_connect_security_stru *mac_security_param)
{
    if (!mac_mib_get_rsnaactivated(mac_vap)) {
        return HI_TRUE;
    }
    switch (mac_security_param->pmf_cap) {
        case MAC_PMF_DISABLED: {
            mac_mib_set_dot11_rsnamfpc(mac_vap, HI_FALSE);
            mac_mib_set_dot11_rsnamfpr(mac_vap, HI_FALSE);
        } break;
        case MAC_PMF_ENABLED: {
            mac_mib_set_dot11_rsnamfpc(mac_vap, HI_TRUE);
            mac_mib_set_dot11_rsnamfpr(mac_vap, HI_FALSE);
        } break;
        case MAC_PME_REQUIRED: {
            mac_mib_set_dot11_rsnamfpc(mac_vap, HI_TRUE);
            mac_mib_set_dot11_rsnamfpr(mac_vap, HI_TRUE);
        } break;
        default: {
            return HI_FALSE;
        }
    }

    if (MAC_NL80211_MFP_REQUIRED == mac_security_param->mgmt_proteced) {
        mac_vap->user_pmf_cap = HI_TRUE;
    } else {
        mac_vap->user_pmf_cap = HI_FALSE;
    }
    return HI_SUCCESS;
}
#endif

WIFI_ROM_TEXT hi_void mac_vap_init_crypto_suites(mac_vap_stru *mac_vap, const mac_cfg80211_crypto_settings_stru *crypto)
{
    hi_u8 loop = 0;

    /* 初始化单播密钥套件 */
    if (crypto->wpa_versions == WITP_WPA_VERSION_1) {
        mac_vap->cap_flag.wpa = HI_TRUE;
        for (loop = 0; loop < crypto->n_ciphers_pairwise; loop++) {
            mac_mib_set_rsnaconfig_wpa_pairwise_cipher_implemented(mac_vap, crypto->ciphers_pairwise[loop]);
        }
    } else if (crypto->wpa_versions == WITP_WPA_VERSION_2) {
        mac_vap->cap_flag.wpa2 = HI_TRUE;
        for (loop = 0; loop < crypto->n_ciphers_pairwise; loop++) {
            mac_mib_set_rsnaconfig_wpa2_pairwise_cipher_implemented(mac_vap, crypto->ciphers_pairwise[loop]);
        }
    }

    /* 初始化认证套件 */
    for (loop = 0; loop < crypto->n_akm_suites; loop++) {
        mac_mib_set_rsnaconfig_authentication_suite_implemented(mac_vap, crypto->akm_suites[loop], loop);
    }
}

/* ****************************************************************************
 功能描述  : 根据内核下发的关联能力，赋值加密相关的mib 值
 输入参数  : mac_vap_stru                        *pst_mac_vap
             mac_cfg80211_connect_param_stru     *pst_mac_connect_param
 修改历史      :
  1.日    期   : 2014年1月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_init_privacy(mac_vap_stru *mac_vap, mac_cfg80211_connect_security_stru *mac_sec_param)
{
    mac_wep_key_param_stru wep_key = { 0 };
    mac_cfg80211_crypto_settings_stru *crypto = HI_NULL;
    hi_u32 ret;

    mac_mib_set_privacyinvoked(mac_vap, HI_FALSE);
#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
    /* 初始化 RSNActive 为FALSE */
    mac_mib_set_rsnaactivated(mac_vap, HI_FALSE);
#endif
    /* 清除加密套件信息 */
    mac_mib_set_rsnaclear_wpa_pairwise_cipher_implemented(mac_vap);
    mac_mib_set_rsnaclear_wpa2_pairwise_cipher_implemented(mac_vap);
    mac_vap->cap_flag.wpa = HI_FALSE;
    mac_vap->cap_flag.wpa2 = HI_FALSE;

    /* 不加密 */
    if (!mac_sec_param->privacy) {
        return HI_SUCCESS;
    }

    /* WEP加密 */
    if (mac_sec_param->wep_key_len != 0) {
        wep_key.key_len = mac_sec_param->wep_key_len;
        wep_key.key_index = mac_sec_param->wep_key_index;
        wep_key.default_key = HI_TRUE;
        if (memcpy_s(wep_key.auc_wep_key, WLAN_WEP104_KEY_LEN, mac_sec_param->auc_wep_key, WLAN_WEP104_KEY_LEN) !=
            EOK) {
            return HI_FAIL;
        }
        ret = mac_vap_add_wep_key(mac_vap, sizeof(mac_wep_key_param_stru), (hi_u8 *)&wep_key);
        if (ret != HI_SUCCESS) {
            oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{mac_vap_init_privacy::wep_key failed[%d].}", ret);
        }
        return ret;
    }

    /* WPA/WPA2加密 */
    crypto = &(mac_sec_param->crypto);
    if ((crypto->n_ciphers_pairwise > WLAN_PAIRWISE_CIPHER_SUITES) ||
        (crypto->n_akm_suites > MAC_AUTHENTICATION_SUITE_NUM)) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_WPA, "{mac_vap_init_privacy::cipher[%d] akm[%d] unexpected.}",
            crypto->n_ciphers_pairwise, crypto->n_akm_suites);
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }

    /* 初始化RSNA mib 为 TRUR */
    mac_mib_set_privacyinvoked(mac_vap, HI_TRUE);
    mac_mib_set_rsnaactivated(mac_vap, HI_TRUE);

    /* 初始化组播密钥套件 */
    mac_mib_set_rsnacfggroupcipher(mac_vap, (hi_u8)(crypto->cipher_group));

#ifdef _PRE_WLAN_FEATURE_PMF
    ret = mac_vap_init_pmf(mac_vap, mac_sec_param);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{mac_11i_init_privacy::mac_11w_init_privacy failed[%d].}", ret);
        return ret;
    }
#endif
    /* 初始化单播密钥套件和认证套件 */
    mac_vap_init_crypto_suites(mac_vap, crypto);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 根据mac地址获取mac_user指针
**************************************************************************** */
WIFI_ROM_TEXT mac_user_stru *mac_vap_get_user_by_addr(mac_vap_stru *mac_vap, const hi_u8 *mac_addr)
{
    hi_u32 ret;
    hi_u8 user_idx = 0xff;
    mac_user_stru *mac_user = HI_NULL;

    /* 根据mac addr找到sta索引 */
    ret = mac_vap_find_user_by_macaddr(mac_vap, mac_addr, WLAN_MAC_ADDR_LEN, &user_idx);
    if (ret != HI_SUCCESS) {
        oam_info_log1(0, OAM_SF_ANY, "{mac_vap_get_user_by_addr::find_user_by_macaddr failed[%d].}", ret);
        if (mac_addr != HI_NULL) {
            oam_info_log3(0, OAM_SF_ANY, "{mac_vap_get_user_by_addr::mac[%x:XX:XX:XX:%x:%x] cant be found!}",
                mac_addr[0], mac_addr[4], mac_addr[5]); /* 维测记录[0]、[4]、[5] */
        }
        return HI_NULL;
    }
    /* 根据sta索引找到user内存区域 */
    mac_user = mac_user_get_user_stru(user_idx);
    if (mac_user == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{mac_vap_get_user_by_addr::user ptr null.}");
    }
    return mac_user;
}

WIFI_ROM_TEXT static hi_u32 mac_vap_config_group_cipher(const mac_vap_stru *mac_vap,
    const mac_beacon_param_stru *beacon_param, wlan_mib_dot11_rsna_config_entry_stru *mib_rsna_cfg)
{
    switch (beacon_param->group_crypto) {
        case WLAN_80211_CIPHER_SUITE_TKIP:
            mib_rsna_cfg->dot11_rsna_config_group_cipher_size = WLAN_TKIP_KEY_LEN * 8; /* 乘8转成bit数 */
            break;
        case WLAN_80211_CIPHER_SUITE_CCMP:
            mib_rsna_cfg->dot11_rsna_config_group_cipher_size = WLAN_CCMP_KEY_LEN * 8; /* 乘8转成bit数 */
            break;
        default:
            if (mac_vap != HI_NULL) {
                oam_error_log1(mac_vap->vap_id, OAM_SF_CFG,
                    "{mac_vap_config_group_cipher::UNEXPECTED group_crypto[%d].}", beacon_param->group_crypto);
            }

            return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }
    mib_rsna_cfg->dot11_rsna_config_group_cipher = beacon_param->group_crypto;

    return HI_SUCCESS;
}

/* 清除之前的加密配置信息
 */
WIFI_ROM_TEXT static hi_void mac_vap_clear_auth_suite(mac_vap_stru *mac_vap)
{
    mac_mib_set_rsnaclear_wpa_pairwise_cipher_implemented(mac_vap);
    mac_mib_set_rsnaclear_wpa2_pairwise_cipher_implemented(mac_vap);
    mac_mib_set_privacyinvoked(mac_vap, HI_FALSE);
    mac_mib_set_rsnaactivated(mac_vap, HI_FALSE);
    mac_vap->cap_flag.wpa = HI_FALSE;
    mac_vap->cap_flag.wpa2 = HI_FALSE;
    mac_mib_clear_rsna_auth_suite(mac_vap);
}

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
WIFI_ROM_TEXT static hi_u32 mac_vap_config_mesh_group_cipher(const mac_vap_stru *mac_vap,
    const mac_beacon_param_stru *beacon_param, wlan_mib_dot11_rsna_config_entry_stru *mib_rsna_cfg)
{
    if (mac_vap->mib_info->wlan_mib_sta_config.dot11_mesh_activated == HI_TRUE) {
        if (beacon_param->group_crypto != WLAN_80211_CIPHER_SUITE_CCMP) {
            oam_error_log1(mac_vap->vap_id, OAM_SF_CFG,
                "{mac_vap_config_mesh_group_cipher::[MESH]UNEXPECTED group_crypto[%d].}", beacon_param->group_crypto);
            return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
        }
        mib_rsna_cfg->dot11_rsna_config_group_cipher = beacon_param->group_crypto;
        mib_rsna_cfg->dot11_rsna_config_group_cipher_size = WLAN_CCMP_KEY_LEN * 8; /* 乘8 转成bit数 */
    }

    return HI_SUCCESS;
}

WIFI_ROM_TEXT static hi_u32 mac_mesh_vap_config_beacon(const mac_vap_stru *mac_vap,
    mac_beacon_operation_type_uint8 operation_type, const mac_beacon_param_stru *beacon_param,
    wlan_mib_dot11_rsna_config_entry_stru *mib_rsna_cfg)
{
    hi_u32 ret;
    if (operation_type == MAC_SET_BEACON) {
        ret = mac_vap_config_mesh_group_cipher(mac_vap, beacon_param, mib_rsna_cfg);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    } else {
        ret = mac_vap_config_group_cipher(mac_vap, beacon_param, mib_rsna_cfg);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    return ret;
}
#endif

WIFI_ROM_TEXT static hi_void mac_vap_authentication_suite_config(mac_vap_stru *mac_vap,
    wlan_mib_dot11_rsna_config_entry_stru *mib_rsna_cfg, const mac_beacon_param_stru *beacon_param)
{
    hi_u32 loop;

    /* 配置认证套件 */
    for (loop = 0; loop < MAC_AUTHENTICATION_SUITE_NUM; loop++) {
        if (beacon_param->auc_auth_type[loop] == 0xff) {
            continue;
        }
        mac_mib_set_rsna_auth_suite(mac_vap, beacon_param->auc_auth_type[loop]);
    }

    if (beacon_param->crypto_mode & WLAN_WPA_BIT) {
        mac_vap->cap_flag.wpa = HI_TRUE;
        /* 配置WPA单播密钥套件 */
        for (loop = 0; loop < MAC_PAIRWISE_CIPHER_SUITES_NUM; loop++) {
            if (beacon_param->auc_pairwise_crypto_wpa[loop] == 0xff) {
                continue;
            }
            mac_mib_set_rsnaconfig_wpa_pairwise_cipher_implemented(mac_vap,
                beacon_param->auc_pairwise_crypto_wpa[loop]);
        }
    }

    if (beacon_param->crypto_mode & WLAN_WPA2_BIT) {
        mac_vap->cap_flag.wpa2 = HI_TRUE;
        /* 配置WPA2单播密钥套件 */
        for (loop = 0; loop < MAC_PAIRWISE_CIPHER_SUITES_NUM; loop++) {
            if (beacon_param->auc_pairwise_crypto_wpa2[loop] == 0xff) {
                continue;
            }
            mac_mib_set_rsnaconfig_wpa2_pairwise_cipher_implemented(mac_vap,
                beacon_param->auc_pairwise_crypto_wpa2[loop]);
        }

        /* RSN 能力 */
        mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpr =
            (beacon_param->us_rsn_capability & BIT6) ? HI_TRUE : HI_FALSE;
        mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpc =
            (beacon_param->us_rsn_capability & BIT7) ? HI_TRUE : HI_FALSE;
        mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_preauthentication_activated =
            beacon_param->us_rsn_capability & BIT0;
        mib_rsna_cfg->dot11_rsna_config_number_of_ptksa_replay_counters_implemented =
            (beacon_param->us_rsn_capability & 0x0C) >> 2; /* 右移2 bit */
        mib_rsna_cfg->dot11_rsna_config_number_of_gtksa_replay_counters_implemented =
            (beacon_param->us_rsn_capability & 0x30) >> 4; /* 右移4 bit */
    }
}

/* ****************************************************************************
 功能描述  : 复位11X端口
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT static hi_u32 mac_vap_config_beacon(mac_vap_stru *mac_vap,
    mac_beacon_operation_type_uint8 operation_type, const mac_beacon_param_stru *beacon_param)
{
    mac_user_stru *multi_user = HI_NULL;
    wlan_mib_dot11_rsna_config_entry_stru *mib_rsna_cfg = HI_NULL;
    hi_u32 ret;

    if (operation_type >= MAC_BEACON_OPERATION_BUTT) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_CFG, "{mac_vap_add_beacon::en_operation_type:%d error [idx:%d]}",
            operation_type, mac_vap->multi_user_idx);
    }

    /* 清除之前的加密配置信息 */
    mac_vap_clear_auth_suite(mac_vap);

    mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpr = HI_FALSE;
    mac_vap->mib_info->wlan_mib_privacy.dot11_rsnamfpc = HI_FALSE;

    if (operation_type == MAC_ADD_BEACON) {
        /* 清除组播密钥信息 */
        multi_user = mac_user_get_user_stru(mac_vap->multi_user_idx);
        if (multi_user == HI_NULL) {
            oam_error_log1(mac_vap->vap_id, OAM_SF_CFG, "{mac_vap_add_beacon::pst_multi_user null [idx:%d] .}",
                mac_vap->multi_user_idx);
            return HI_ERR_CODE_PTR_NULL;
        }
    }

    if (!beacon_param->privacy) {
        return HI_SUCCESS;
    }
    /* 使能加密 */
    mac_mib_set_privacyinvoked(mac_vap, HI_TRUE);
    mib_rsna_cfg = &mac_vap->mib_info->wlan_mib_rsna_cfg;
    if ((beacon_param->crypto_mode & (WLAN_WPA_BIT | WLAN_WPA2_BIT)) == 0) {
        return HI_SUCCESS;
    }
    /* WEP加密时，在addbeacon流程之前，已经通过add key配置了密钥。 */
    if (operation_type == MAC_ADD_BEACON) {
        mac_user_init_key(multi_user);
        /* 本函数前面的if语句已经判断了multi_user非空，误报告警，lin_t e613告警屏蔽 */
        multi_user->user_tx_info.security.cipher_key_type = WLAN_KEY_TYPE_TX_GTK;
    }
    mac_mib_set_rsnaactivated(mac_vap, HI_TRUE);

    /* 配置组播套件 */
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    ret = mac_mesh_vap_config_beacon(mac_vap, operation_type, beacon_param, mib_rsna_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }
#else
    ret = mac_vap_config_group_cipher(mac_vap, beacon_param, mib_rsna_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }
#endif
    /* 配置认证套件 */
    mac_vap_authentication_suite_config(mac_vap, mib_rsna_cfg, beacon_param);
    return HI_SUCCESS;
}
/* mac_vap_add_beacon->mac_vap_config_beacon->mac_vap_clear_auth_suite，作了修改，lin_t e818告警屏蔽 */
WIFI_ROM_TEXT hi_u32 mac_vap_add_beacon(mac_vap_stru *mac_vap, const mac_beacon_param_stru *beacon_param)
{
    return mac_vap_config_beacon(mac_vap, MAC_ADD_BEACON, beacon_param);
}

/* ****************************************************************************
 功能描述  : 根据en_key_type，调用相应的函数，更新vap信息
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_add_key(const mac_vap_stru *mac_vap, mac_user_stru *mac_user, hi_u8 key_id,
    const mac_key_params_stru *key)
{
    hi_u32 ret;

    if (((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_WEP_40) ||
        ((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_WEP_104)) {
        /* 设置mib */
        mac_mib_set_privacyinvoked(mac_vap, HI_TRUE);
        mac_mib_set_rsnaactivated(mac_vap, HI_FALSE);
        mac_mib_set_rsnacfggroupcipher(mac_vap, (hi_u8)key->cipher);
        ret = mac_user_add_wep_key(mac_user, key_id, key);
    } else if (((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_TKIP) ||
        ((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_CCMP)) {
        ret = mac_user_add_rsn_key(mac_user, key_id, key);
    } else if ((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_BIP) {
        ret = mac_user_add_bip_key(mac_user, key_id, key);
    } else {
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 获取mac加密属性default_key_id
 修改历史      :
  1.日    期   : 2015年5月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_vap_get_default_key_id(const mac_vap_stru *mac_vap)
{
    mac_user_stru *multi_user = HI_NULL;
    hi_u8          default_key_id;

    /* 根据索引，从组播用户密钥信息中查找密钥 */
    multi_user = mac_user_get_user_stru(mac_vap->multi_user_idx);
    if (multi_user == HI_NULL) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{mac_vap_get_default_key_id::multi_user[%d] NULL}",
            mac_vap->multi_user_idx);
        return 0;
    }

    if ((multi_user->key_info.cipher_type != WLAN_80211_CIPHER_SUITE_WEP_40) &&
        (multi_user->key_info.cipher_type != WLAN_80211_CIPHER_SUITE_WEP_104)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{mac_vap_get_default_key_id::unexpected cipher_type[%d]}",
            multi_user->key_info.cipher_type);
        return 0;
    }
    default_key_id = multi_user->key_info.default_index;
    if (default_key_id >= WLAN_NUM_TK) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{mac_vap_get_default_key_id::unexpected keyid[%d]}",
            default_key_id);
        return 0;
    }
    return default_key_id;
}

/* ****************************************************************************
 功能描述  : 更新hmac加密属性default_key
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_set_default_key(const mac_vap_stru *mac_vap, hi_u8 key_index)
{
    wlan_priv_key_param_stru *wep_key = HI_NULL;
    mac_user_stru            *multi_user = HI_NULL;

    /* 1.1 如果非wep 加密，则直接返回 */
    if (!mac_is_wep_enabled(mac_vap)) {
        return HI_SUCCESS;
    }

    /* 2.1 根据索引，从组播用户密钥信息中查找密钥 */
    multi_user = mac_user_get_user_stru(mac_vap->multi_user_idx);
    if (multi_user == HI_NULL) {
        return HI_ERR_CODE_SECURITY_USER_INVAILD;
    }
    wep_key = &multi_user->key_info.ast_key[key_index];
    if (wep_key->cipher != WLAN_CIPHER_SUITE_WEP40 && wep_key->cipher != WLAN_CIPHER_SUITE_WEP104) {
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }

    /* 3.1 更新密钥类型及default id */
    multi_user->key_info.cipher_type = (hi_u8)(wep_key->cipher);
    multi_user->key_info.default_index = key_index;
    /* 4.1 设置mib属性 */
    mac_mib_set_wep_default_keyid(mac_vap, key_index);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 设置管理帧密钥
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_vap_set_default_mgmt_key(const mac_vap_stru *mac_vap, hi_u8 key_index)
{
    mac_user_stru *multi_user = HI_NULL;

    /* 管理帧加密信息保存在组播用户中 */
    multi_user = mac_user_get_user_stru(mac_vap->multi_user_idx);
    if (multi_user == HI_NULL) {
        return HI_ERR_CODE_SECURITY_USER_INVAILD;
    }
    /* keyid校验 */
    if (key_index < WLAN_NUM_TK || key_index > WLAN_MAX_IGTK_KEY_INDEX) {
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }
    if ((hi_u8)multi_user->key_info.ast_key[key_index].cipher != WLAN_80211_CIPHER_SUITE_BIP) {
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }
    /* 更新IGTK的keyid */
    multi_user->key_info.igtk_key_index = key_index;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 初始化STA 加密情况下，数据过滤的参数。
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_init_user_security_port(const mac_vap_stru *mac_vap, mac_user_stru *mac_user)
{
    mac_user->port_valid = HI_TRUE;
    if (!mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated) {
        return;
    }
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_MESH) && (mac_user->is_mesh_user == HI_TRUE)) {
        if (mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_security_activated != HI_TRUE) {
            oam_warning_log0(0, OAM_SF_WPA, "{mac_vap_init_user_security_port::dot11MeshSecurityActivated is FALSE!.}");
            return;
        }
        if (mac_vap->mib_info->wlan_mib_mesh_sta_cfg.dot11_mesh_active_authentication_protocol !=
            MAC_MESH_AUTH_PROTO_SAE) {
            oam_warning_log0(0, OAM_SF_WPA,
                "{mac_vap_init_user_security_port::dot11MeshActiveAuthenticationProtocol isn't SAE!.}");
            return;
        }
        mac_user->port_valid = HI_FALSE;
        mac_user_init_key(mac_user);
        return;
    }
#endif

    /* 是否采用WPA 或WPA2 加密方式 */
    if (!(mac_vap->cap_flag.wpa) && !(mac_vap->cap_flag.wpa2)) {
        return;
    }
    /* STA需要检测是否采用802.1X 认证方式 */
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA &&
        mac_check_auth_policy(mac_vap->mib_info, WLAN_AUTH_SUITE_1X) == HI_TRUE) ||
        ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
        || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    )) {
        mac_user->port_valid = HI_FALSE;
    }
    mac_user_init_key(mac_user);
}

/* ****************************************************************************
 功能描述  : 复位11X端口
 修改历史      :
  1.日    期   : 2015年6月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* mac_vap_set_beacon->mac_vap_config_beacon->mac_vap_clear_auth_suite，作了修改，lin_t e818告警屏蔽 */
WIFI_ROM_TEXT hi_u32 mac_vap_set_beacon(mac_vap_stru *mac_vap, const mac_beacon_param_stru *beacon_param)
{
    return mac_vap_config_beacon(mac_vap, MAC_SET_BEACON, beacon_param);
}

/* ****************************************************************************
 功能描述  : 获取vap的带宽运行信息获取vap带宽能力
 修改历史      :
  1.日    期   : 2014年4月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_vap_get_bandwidth_cap(mac_vap_stru *mac_vap, wlan_bw_cap_enum_uint8 *pen_cap)
{
    mac_channel_stru      *channel = HI_NULL;
    wlan_bw_cap_enum_uint8 band_cap = WLAN_BW_CAP_20M;

    channel = &(mac_vap->channel);
    if (WLAN_BAND_WIDTH_40PLUS == channel->en_bandwidth || WLAN_BAND_WIDTH_40MINUS == channel->en_bandwidth) {
        band_cap = WLAN_BW_CAP_40M;
    } else if (channel->en_bandwidth >= WLAN_BAND_WIDTH_80PLUSPLUS) {
        band_cap = WLAN_BW_CAP_80M;
    }
    *pen_cap = band_cap;
}

/* ****************************************************************************
 功能描述  : 判断是否允许WEP加密
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_is_wep_allowed(const mac_vap_stru *mac_vap)
{
    hi_u8 grp_policy;

    if (mac_mib_get_rsnaactivated(mac_vap) != 0) {
        grp_policy = mac_mib_get_rsnacfggroupcipher(mac_vap);
        if ((grp_policy == WLAN_80211_CIPHER_SUITE_WEP_40) || (grp_policy == WLAN_80211_CIPHER_SUITE_WEP_104)) {
            return HI_TRUE;
        }
        return HI_FALSE;
    } else {
        return mac_is_wep_enabled(mac_vap);
    }
}

/* ****************************************************************************
 功能描述  : 获取保护模式类型
 输入参数  : mac_vap_stru        : mac VAP结构体， 保存sta信息
             pst_mac_sta         : mac user结构体， 保存ap信息
 修改历史      :
  1.日    期   : 2014年1月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT wlan_prot_mode_enum_uint8 mac_vap_get_user_protection_mode(const mac_vap_stru *mac_vap,
    const mac_user_stru *mac_user)
{
    wlan_prot_mode_enum_uint8 protection_mode = WLAN_PROT_NO;

    if ((mac_vap == HI_NULL) || (mac_user == HI_NULL)) {
        return protection_mode;
    }

    /* 在2G频段下，如果AP发送的beacon帧ERP ie中Use Protection bit置为1，则将保护级别设置为ERP保护 */
    if ((WLAN_BAND_2G == mac_vap->channel.band) && (mac_user->cap_info.erp_use_protect == HI_TRUE)) {
        protection_mode = WLAN_PROT_ERP;
    } else if ((mac_user->ht_hdl.ht_protection == WLAN_MIB_HT_NON_HT_MIXED) ||
        (mac_user->ht_hdl.ht_protection == WLAN_MIB_HT_NONMEMBER_PROTECTION)) {
        /* 如果AP发送的beacon帧ht operation ie中ht protection字段为mixed或non-member，则将保护级别设置为HT保护 */
        protection_mode = WLAN_PROT_HT;
    } else if (mac_user->ht_hdl.nongf_sta_present == HI_TRUE) {
        /* 如果AP发送的beacon帧ht operation ie中non-gf sta present字段为1，则将保护级别设置为GF保护 */
        protection_mode = WLAN_PROT_GF;
    } else {
        /* 剩下的情况不做保护 */
        protection_mode = WLAN_PROT_NO;
    }

    return protection_mode;
}

/* ****************************************************************************
 功能描述  : 查询是否可以使用lsigtxop保护
 输入参数  : pst_mac_vap : mac vap结构体指针
 输出参数  : 无
 返 回 值  : hi_bool : 0: 不可以使用lsig txop保护
                             1: 可以使用lsig txop保护
 修改历史      :
  1.日    期   : 2014年4月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_bool mac_protection_lsigtxop_check(const mac_vap_stru *mac_vap)
{
    mac_user_stru *mac_user = HI_NULL;

    /* 如果不是11n站点，则不支持lsigtxop保护 */
    if ((mac_vap->protocol != WLAN_HT_MODE) && (mac_vap->protocol != WLAN_HT_ONLY_MODE) &&
        (mac_vap->protocol != WLAN_HT_11G_MODE)) {
        return HI_FALSE;
    }

    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
        mac_user = (mac_user_stru *)mac_user_get_user_stru(mac_vap->assoc_vap_id); /* user保存的是AP的信息 */
        if ((mac_user == HI_NULL) || (mac_user->ht_hdl.lsig_txop_protection_full_support == HI_FALSE)) {
            return HI_FALSE;
        } else {
            return HI_TRUE;
        }
    }
    /* BSS 中所有站点都支持Lsig txop protection, 则使用Lsig txop protection机制，开销小, AP和STA采用不同的判断 */
    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) &&
        mac_mib_get_lsig_txop_full_protection_activated(mac_vap) == HI_TRUE) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

/* ****************************************************************************
 功能描述  : 设置rts 发送参数,host,device共用,
 输入参数  : pst_hmac_vap : hmac vap结构体指针
             en_flag      : 0:关闭lsig txop保护机制   / 1: 打开lsig txop保护机制
 修改历史      :
  1.日    期   : 2014年1月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_protection_set_rts_tx_param(mac_vap_stru *mac_vap, hi_u8 flag,
    wlan_prot_mode_enum_uint8 prot_mode, mac_cfg_rts_tx_param_stru *rts_tx_param)
{
    if ((mac_vap == HI_NULL) || (rts_tx_param == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ASSOC,
            "{mac_protection_set_rts_tx_param_etc::param null,pst_mac_vap[%p] pst_rts_tx_param[%p]}",
            (uintptr_t)mac_vap, (uintptr_t)rts_tx_param);
        return;
    }
    /* 设置窄带的RTS速率参数 */
    if ((mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_5M) ||
        (mac_vap->channel.en_bandwidth == WLAN_BAND_WIDTH_10M)) {
        rts_tx_param->band = WLAN_BAND_2G;
        rts_tx_param->auc_protocol_mode[0]    = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE;
        rts_tx_param->auc_rate[0]             = WLAN_LEGACY_OFDM_24M_BPS;
        rts_tx_param->auc_protocol_mode[1]    = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE;
        rts_tx_param->auc_rate[1]             = WLAN_LEGACY_OFDM_6M_BPS;
        rts_tx_param->auc_protocol_mode[2]    = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE; /* 设置auc_protocol_mode[2] */
        rts_tx_param->auc_rate[2]             = WLAN_LEGACY_OFDM_6M_BPS;            /* 设置auc_rate[2] */
        rts_tx_param->auc_protocol_mode[3]    = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE; /* 设置auc_protocol_mode[3] */
        rts_tx_param->auc_rate[3]             = WLAN_LEGACY_OFDM_6M_BPS;            /* 设置auc_rate[3] */
        return;
    }

    /* 只有启用erp保护时候，RTS[0~2]速率才设为5.5Mpbs(11b), 其余时候都为24Mpbs(leagcy ofdm) */
    if ((prot_mode == WLAN_PROT_ERP) && (flag == HI_SWITCH_ON)) {
        rts_tx_param->band = WLAN_BAND_2G;

        /* RTS[0~2]设为5.5Mbps, RTS[3]设为1Mbps */
        rts_tx_param->auc_protocol_mode[0]    = WLAN_11B_PHY_PROTOCOL_MODE;
        rts_tx_param->auc_rate[0]             = WLAN_LONG_11B_5_HALF_M_BPS;
        rts_tx_param->auc_protocol_mode[1]    = WLAN_11B_PHY_PROTOCOL_MODE;
        rts_tx_param->auc_rate[1]             = WLAN_LONG_11B_5_HALF_M_BPS;
        rts_tx_param->auc_protocol_mode[2]    = WLAN_11B_PHY_PROTOCOL_MODE; /* 设置auc_protocol_mode[2] */
        rts_tx_param->auc_rate[2]             = WLAN_LONG_11B_5_HALF_M_BPS; /* 设置auc_rate[2] */
        rts_tx_param->auc_protocol_mode[3]    = WLAN_11B_PHY_PROTOCOL_MODE; /* 设置auc_protocol_mode[3] */
        rts_tx_param->auc_rate[3]             = WLAN_LONG_11B_1_M_BPS;      /* 设置auc_rate[3] */
    } else {
        rts_tx_param->band = mac_vap->channel.band;

        /* RTS[0~2]设为24Mbps */
        rts_tx_param->auc_protocol_mode[0]    = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE;
        rts_tx_param->auc_rate[0]             = WLAN_LEGACY_OFDM_24M_BPS;
        rts_tx_param->auc_protocol_mode[1]    = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE;
        rts_tx_param->auc_rate[1]             = WLAN_LEGACY_OFDM_24M_BPS;
        rts_tx_param->auc_protocol_mode[2]    = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE; /* 设置auc_protocol_mode[2] */
        rts_tx_param->auc_rate[2]             = WLAN_LEGACY_OFDM_24M_BPS;           /* 设置auc_rate[2] */

        /* 2G的RTS[3]设为1Mbps */
        if (WLAN_BAND_2G == rts_tx_param->band) {
            rts_tx_param->auc_protocol_mode[3]    = WLAN_11B_PHY_PROTOCOL_MODE; /* 设置auc_protocol_mode[3] */
            rts_tx_param->auc_rate[3]             = WLAN_LONG_11B_1_M_BPS;      /* 设置auc_rate[3] */
        } else {
            /* 5G的RTS[3]设为24Mbps */
            rts_tx_param->auc_protocol_mode[3]    = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE; /* 设置auc_protocol_mode[3] */
            rts_tx_param->auc_rate[3]             = WLAN_LEGACY_OFDM_24M_BPS;           /* 设置auc_rate[3] */
        }
    }
}


/* 代码ROM段结束位置 新增ROM代码请放在SECTION中 */
#undef __WIFI_ROM_SECTION__

/* ****************************************************************************
 功能描述  : 根据user MAC地址查找user对象
 输入参数  : vap对象指针，以及user MAC地址
 输出参数  : user对象指针
 修改历史      :
  1.日    期   : 2012年10月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 mac_device_find_user_by_macaddr(const mac_vap_stru *mac_vap, const hi_u8 *sta_mac_addr, hi_u8 addr_len,
    hi_u8 *puc_user_idx)
{
    mac_device_stru        *mac_dev  = HI_NULL;
    mac_vap_stru           *mac_vap_temp = HI_NULL;
    hi_u8                   vap_id;
    hi_u8                   vap_idx;
    hi_u32                  ret;

    /* 获取device */
    mac_dev = mac_res_get_dev();
    /* 对device下的所有vap进行遍历 */
    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        vap_id = mac_dev->auc_vap_id[vap_idx];
        /* 配置vap不需要处理 */
        if (vap_id == WLAN_CFG_VAP_ID) {
            continue;
        }
        /* 本vap不需要处理 */
        if (vap_id == mac_vap->vap_id) {
            continue;
        }
        mac_vap_temp = mac_vap_get_vap_stru(vap_id);
        if (mac_vap_temp == HI_NULL) {
            continue;
        }
        /* 只处理AP模式 */
        if ((mac_vap_temp->vap_mode != WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
            && (mac_vap_temp->vap_mode != WLAN_VAP_MODE_MESH)
#endif
        ) {
            continue;
        }
        ret = mac_vap_find_user_by_macaddr(mac_vap_temp, sta_mac_addr, addr_len, puc_user_idx);
        if (ret == HI_SUCCESS) {
            return HI_SUCCESS;
        }
    }

    return HI_FAIL;
}

/* ****************************************************************************
 功能描述  : 设置mib信息中当前信道
 输入参数  : 频段:wlan_channel_band_enum_uint8 en_band,
             信道:hi_u8 uc_channel
 修改历史      :
  1.日    期   : 2013年7月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 mac_vap_set_current_channel(mac_vap_stru *mac_vap, wlan_channel_band_enum_uint8 band, hi_u8 channel)
{
    hi_u8 channel_idx = 0;
    hi_u32 ret;

    /* 检查信道号 */
    ret = mac_is_channel_num_valid(band, channel);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* 根据信道号找到索引号 */
    ret = mac_get_channel_idx_from_num(band, channel, &channel_idx);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    mac_vap->channel.chan_number = channel;
    mac_vap->channel.band        = band;
    mac_vap->channel.idx         = channel_idx;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取某个基本速率
 修改历史      :
  1.日    期   : 2013年7月9日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 mac_vap_get_curr_baserate(mac_vap_stru *mac_vap, hi_u8 br_idx)
{
    hi_u8          loop;
    hi_u8          found_br_num = 0;
    hi_u8          rate_num;
    mac_rateset_stru  *rate = HI_NULL;

    rate = &(mac_vap->curr_sup_rates.rate);
    rate_num = rate->rs_nrates;
    /* 查找base rate 并记录查找到的个数，与所以比较并返回 */
    for (loop = 0; loop < rate_num; loop++) {
        if (((rate->ast_rs_rates[loop].mac_rate) & 0x80) != 0) {
            if (br_idx == found_br_num) {
                return rate->ast_rs_rates[loop].mac_rate;
            }
            found_br_num++;
        }
    }
    /* 未找到，返回错误 */
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 检查能力信息中与PHY相关的信息
 输出参数  : 无check_bss_capability_phy
 修改历史      :
  1.日    期   : 2013年7月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
 修改历史      :
  2.日    期   : 2013年12月6日
    作    者   : HiSilicon
    修改内容   : 修改函数名，转换为AP和STA公共函数
**************************************************************************** */
hi_void mac_vap_check_bss_cap_info_phy_ap(hi_u16 us_cap_info, const mac_vap_stru *mac_vap)
{
    mac_cap_info_stru *cap_info = (mac_cap_info_stru *)(&us_cap_info);

    if (mac_vap->channel.band != WLAN_BAND_2G) {
        return;
    }
    /* PBCC */
    if ((mac_vap->mib_info->phy_hrdsss.dot11_pbcc_option_implemented == HI_FALSE) && (cap_info->pbcc == 1)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{mac_vap_check_bss_cap_info_phy_ap::PBCC is different.}");
    }
    /* Channel Agility */
    if ((mac_vap->mib_info->phy_hrdsss.dot11_channel_agility_present == HI_FALSE) &&
        (cap_info->channel_agility == 1)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY,
            "{mac_vap_check_bss_cap_info_phy_ap::Channel Agility is different.}");
    }
    /* DSSS-OFDM Capabilities 31h不支持 对方支持则报错 */
    if (cap_info->dsss_ofdm == 1) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY,
            "{mac_vap_check_bss_cap_info_phy_ap::DSSS-OFDM Capabilities is different.}");
    }
}

/* ****************************************************************************
 功能描述  : 设置发送描述符的user_idx
 输入参数  : pst_mac_vap :mac vap结构指针
             pst_cb_ctrl:接收帧的控制字段
             puc_data:表明是目的MAC地址
 修改历史      :
  1.日    期   : 2017年8月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_vap_set_cb_tx_user_idx(mac_vap_stru *mac_vap, hi_void *tx_ctl, const hi_u8 *mac_addr)
{
    hmac_tx_ctl_stru *tx_ctl_temp = (hmac_tx_ctl_stru *)tx_ctl;
    hi_u8 user_idx = MAC_INVALID_USER_ID;
    hi_u32 ret;

    ret = mac_vap_find_user_by_macaddr(mac_vap, mac_addr, WLAN_MAC_ADDR_LEN, &user_idx);
    if (ret != HI_SUCCESS) {
        oam_warning_log4(mac_vap->vap_id, OAM_SF_ANY,
            "{mac_vap_set_cb_tx_user_idx:: cannot find user_idx from xx:xx:xx:%x:%x:%x, set TX_USER_IDX %d.}",
            mac_addr[3], mac_addr[4], mac_addr[5], MAC_INVALID_USER_ID); /* 维测记录[3]、[4]、[5] */
        tx_ctl_temp->us_tx_user_idx = MAC_INVALID_USER_ID;
        return;
    }

    tx_ctl_temp->us_tx_user_idx = user_idx;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
