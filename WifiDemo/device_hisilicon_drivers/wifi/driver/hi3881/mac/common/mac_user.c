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
/* 1 头文件包含 */
#include "oam_ext_if.h"
#include "mac_resource.h"
#include "mac_device.h"
#include "mac_user.h"
#include "hmac_11i.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#define __WIFI_ROM_SECTION__               /* 代码ROM段起始位置 */
WIFI_ROM_BSS hi_u16 g_us_user_res_map = 0; /* user资源map表 最大支持16个用户 */
WIFI_ROM_BSS hi_u8 *g_puc_mac_user_res = HI_NULL;

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : mac user资源初始化,根据user数量申请内存
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_user_res_init(const hi_u8 user_num)
{
    hi_u32 user_size = sizeof(mac_user_stru) * user_num;
    /* mac user 在非offload模式下存在两次申请的情况,已经申请过则跳过 */
    if (g_puc_mac_user_res != HI_NULL) {
        return HI_SUCCESS;
    }
    g_puc_mac_user_res = hi_malloc(HI_MOD_ID_WIFI_DRV, user_size);
    if (g_puc_mac_user_res == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{mac_user_res_init::mem alloc user res null.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }
    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(g_puc_mac_user_res, user_size, 0, user_size);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : mac 用户资源池去初始化
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_user_res_exit(hi_void)
{
    if (g_puc_mac_user_res != HI_NULL) {
        hi_free(HI_MOD_ID_WIFI_DRV, g_puc_mac_user_res);
        g_puc_mac_user_res = HI_NULL;
    }
}

/* ****************************************************************************
 功能描述  : 获取全局用户数量 用于申请用户内存 user数量=配置的单播用户 + vap数量(组播用户)
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_user_get_user_num(hi_void)
{
    hi_u8 ucast_user_num = oal_mem_get_user_res_num(); /* 单播用户 = 用户配置数 */
    /* 组播用户 = vap数-1(配置vap无组播用户) */
    hi_u8 mcast_user_num = oal_mem_get_vap_res_num() - WLAN_CFG_VAP_NUM_PER_DEVICE;
    hi_u8 user_num = ucast_user_num + mcast_user_num;
    /* USER资源设置时已做规格校验此处不再校验 */
    return user_num;
}

/* ****************************************************************************
 功能描述  : 分配一个未使用user资源
 返 回 值  : 未使用的user资源id
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_user_alloc_user_res(hi_void)
{
    hi_u8 user_idx;
    hi_u8 user_num = mac_user_get_user_num();
    user_idx = oal_bit_find_first_zero((hi_u32)g_us_user_res_map, user_num);
    if (user_idx >= user_num) {
        oam_error_log2(0, OAM_SF_ANY, "{mac_user_alloc_user_res::alloc user res fail. res mac[%x], max user spec[%d].}",
            g_us_user_res_map, user_num);
        user_idx = MAC_INVALID_USER_ID;
    } else {
        /* 将对应的res标志位置1 */
        g_us_user_res_map |= (hi_u16)(BIT0 << user_idx);
    }
    return user_idx;
}

/* ****************************************************************************
 功能描述  : user初始化时 获取对应MAC USER索引的内存
             仅用于与hmac dmac建立匹配关系 入参调用处已校验
 输入参数  : 对应MAC USER内存索引
 返 回 值  : 对应内存地址
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 *mac_user_init_get_user_stru(hi_u8 idx)
{
    return (g_puc_mac_user_res + sizeof(mac_user_stru) * idx);
}

/* ****************************************************************************
 功能描述  : 获取对应MAC USER索引的内存
 输入参数  : 对应MAC USER内存索引
 返 回 值  : 对应内存地址
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT mac_user_stru *mac_user_get_user_stru(hi_u8 idx)
{
    hi_u8 user_num = mac_user_get_user_num();
    mac_user_stru *mac_user = HI_NULL;
    if ((g_puc_mac_user_res == HI_NULL) || (idx >= user_num)) {
        return HI_NULL;
    }
    mac_user = (mac_user_stru *)(g_puc_mac_user_res + sizeof(mac_user_stru) * idx);
    /* user id=0为特殊user,单播不为0 */
    if ((mac_user->is_user_alloced != MAC_USER_ALLOCED) && (idx != 0)) {
        /* 获取用户时用户已经释放属于正常，直接返回空指针即可 */
        return HI_NULL;
    }
    return mac_user;
}

/* ****************************************************************************
 功能描述  : 删除一个已使用使用user资源
             OFFLOAD模式下dmac hmac各自释放，非OFFLOAD模式下统一由hmac申请和释放
 输入参数  : vap资源id
 修改历史      :
  1.日    期   : 2019年5月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_user_free_user_res(hi_u8 idx)
{
    g_us_user_res_map &= (~((hi_u16)BIT0 << idx)); // ~操作符表达式中所有变量都是无符号数,误报告警，lin_t e502告警屏蔽
}

/* ****************************************************************************
 功能描述  : 添加wep密钥到指定的密钥槽
 修改历史      :
  1.日    期   : 2015年5月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_user_add_key_common(mac_user_stru *mac_user, hi_u8 key_index, const mac_key_params_stru *key)
{
    hi_s32 key_max_len;

    if (key_index >= WLAN_NUM_TK) {
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }

    if (((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_WEP_40) ||
        ((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_WEP_104)) {
        key_max_len = WLAN_WEP104_KEY_LEN;
    } else if (((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_TKIP) ||
        ((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_CCMP)) {
        key_max_len = WLAN_WPA_KEY_LEN;
    } else {
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }

    if (key->key_len > key_max_len) {
        return HI_ERR_CODE_SECURITY_KEY_LEN;
    }

    if ((hi_u32)key->seq_len > WLAN_WPA_SEQ_LEN) {
        return HI_ERR_CODE_SECURITY_KEY_LEN;
    }

    mac_user->key_info.ast_key[key_index].cipher = key->cipher;
    mac_user->key_info.ast_key[key_index].key_len = (hi_u32)key->key_len;
    mac_user->key_info.ast_key[key_index].seq_len = (hi_u32)key->seq_len;

    if (memcpy_s(mac_user->key_info.ast_key[key_index].auc_key, WLAN_WPA_KEY_LEN, key->auc_key, (hi_u32)key->key_len) !=
        EOK) {
        return HI_FAIL;
    }
    if (memcpy_s(mac_user->key_info.ast_key[key_index].auc_seq, WLAN_WPA_SEQ_LEN, key->auc_seq, (hi_u32)key->seq_len) !=
        EOK) {
        return HI_FAIL;
    }

    if (((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_WEP_40) ||
        ((hi_u8)key->cipher == WLAN_80211_CIPHER_SUITE_WEP_104)) {
        mac_user->user_tx_info.security.cipher_key_type = WLAN_KEY_TYPE_TX_GTK;
    } else {
        mac_user->key_info.cipher_type = (hi_u8)key->cipher;
        mac_user->key_info.default_index = key_index;
    }

    return HI_SUCCESS;
}

WIFI_ROM_TEXT hi_u32 mac_user_add_wep_key(mac_user_stru *mac_user, hi_u8 key_index, const mac_key_params_stru *key)
{
    return mac_user_add_key_common(mac_user, key_index, key);
}

/* ****************************************************************************
 函 数 名  : mac_user_add_rsn_key
 功能描述  : 更新rsn单播信息
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_user_add_rsn_key(mac_user_stru *mac_user, hi_u8 key_index, const mac_key_params_stru *key)
{
    return mac_user_add_key_common(mac_user, key_index, key);
}

/* ****************************************************************************
 功能描述  : 更新bip信息
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_user_add_bip_key(mac_user_stru *mac_user, hi_u8 key_index, const mac_key_params_stru *key)
{
    /* keyid校验 */
    if (key_index < WLAN_NUM_TK || key_index > WLAN_MAX_IGTK_KEY_INDEX) {
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }

    if (memcpy_s(mac_user->key_info.ast_key[key_index].auc_key, WLAN_WPA_KEY_LEN, key->auc_key,
        (hi_u32)key->key_len) != EOK) {
        return HI_FAIL;
    }
    if (memcpy_s(mac_user->key_info.ast_key[key_index].auc_seq, WLAN_WPA_SEQ_LEN, key->auc_seq,
        (hi_u32)key->seq_len) != EOK) {
        return HI_FAIL;
    }
    mac_user->key_info.ast_key[key_index].cipher = key->cipher;
    mac_user->key_info.ast_key[key_index].key_len = (hi_u32)key->key_len;
    mac_user->key_info.ast_key[key_index].seq_len = (hi_u32)key->seq_len;

    mac_user->key_info.igtk_key_index = key_index;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 初始化用户的密钥信息
 修改历史      :
  1.日    期   : 2015年5月14日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_user_init_key(mac_user_stru *mac_user)
{
    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(&mac_user->key_info, sizeof(mac_key_mgmt_stru), 0, sizeof(mac_key_mgmt_stru));
    mac_user->key_info.cipher_type = WLAN_80211_CIPHER_SUITE_NO_ENCRYP;
    mac_user->key_info.last_gtk_key_idx = 0xFF;
}

/* ****************************************************************************
 功能描述  : 初始化mac user公共部分
 输入参数  : pst_mac_user: 指向user的结构体
             uc_user_idx : 用户索引
             puc_mac_addr: MAC地址
             uc_vap_id   :
 修改历史      :
  1.日    期   : 2013年8月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_user_init(mac_user_stru *mac_user, hi_u8 user_idx, const hi_u8 *mac_addr, hi_u8 vap_id)
{
    /* 初始清0 */
    if (memset_s(mac_user, sizeof(mac_user_stru), 0, sizeof(mac_user_stru)) != EOK) {
        return;
    }
    mac_user->is_user_alloced = MAC_USER_ALLOCED;
    mac_user->vap_id = vap_id;
    mac_user->us_assoc_id = user_idx;
    /* 初始化密钥 */
    mac_user->user_tx_info.security.cipher_key_type = WLAN_KEY_TYPE_PTK;
    mac_user->user_tx_info.security.cipher_protocol_type = WLAN_80211_CIPHER_SUITE_NO_ENCRYP;
    /* 初始化安全加密信息 */
    mac_user_init_key(mac_user);
    mac_user_set_key(mac_user, WLAN_KEY_TYPE_PTK, WLAN_80211_CIPHER_SUITE_NO_ENCRYP, 0);
    mac_user->port_valid = HI_FALSE;
    mac_user->user_asoc_state = MAC_USER_STATE_BUTT;

    if (mac_addr == HI_NULL) {
        mac_user->is_multi_user = HI_TRUE;
        mac_user->user_asoc_state = MAC_USER_STATE_ASSOC;
    } else {
        /* 初始化一个用户是否是组播用户的标志，组播用户初始化时不会调用本函数 */
        mac_user->is_multi_user = HI_FALSE;
        /* 设置mac地址 */
        if (memcpy_s(mac_user->user_mac_addr, WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            return;
        }
    }
    mac_user->mesh_user_leave = HI_FALSE;
    /* 初始化能力 */
    mac_user_set_pmf_active(mac_user, HI_FALSE);
    mac_user_set_avail_num_spatial_stream(mac_user, MAC_USER_INIT_STREAM);
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
    /* RSSI统计量初始化 */
    mac_user->rx_conn_rssi = WLAN_RSSI_DUMMY_MARKER;
#endif
}

/* ****************************************************************************
 功能描述  : 设置可用带宽的信息
 修改历史      :
  1.日    期   : 2015年4月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  1.日    期   : 2015年5月27日
    作    者   : HiSilicon
    修改内容   : 针对协议切换, 对带宽更新进行特殊判断
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_user_set_bandwidth_info(mac_user_stru *mac_user, wlan_bw_cap_enum_uint8 avail_bandwidth,
    wlan_bw_cap_enum_uint8 cur_bandwidth)
{
    mac_user->avail_bandwidth = avail_bandwidth;
    mac_user->cur_bandwidth = cur_bandwidth;
    /* Autorate将协议从11n切换成11b后, cur_bandwidth会变为20M
       此时如果软件将带宽改为40M, cur_bandwidth仍需要保持20M */
    if ((WLAN_LEGACY_11B_MODE == mac_user->cur_protocol_mode) && (WLAN_BW_CAP_20M != mac_user->cur_bandwidth)) {
        mac_user->cur_bandwidth = WLAN_BW_CAP_20M;
    }
}

/* ****************************************************************************
 功能描述  : 获取用户的带宽
 修改历史      :
  1.日    期   : 2013年10月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
*****************************************************************************/
WIFI_ROM_TEXT hi_void mac_user_get_sta_cap_bandwidth(mac_user_stru *mac_user,
                                                     wlan_bw_cap_enum_uint8 *pen_bandwidth_cap)
{
    mac_user_ht_hdl_stru *mac_ht_hdl = HI_NULL;

    *pen_bandwidth_cap = WLAN_BW_CAP_20M;
    /* 获取HT和VHT结构体指针 */
    mac_ht_hdl = &(mac_user->ht_hdl);

    if (mac_ht_hdl->ht_capable) {
        if (mac_ht_hdl->ht_capinfo.supported_channel_width == HI_TRUE) {
            *pen_bandwidth_cap = WLAN_BW_CAP_40M;
        }
    } else {
        /* else分支不需要处理 使用默认值20M */
    }
    mac_user_set_bandwidth_cap(mac_user, *pen_bandwidth_cap);
}

/* ****************************************************************************
 功能描述  : 设置en_user_asoc_state 的统一接口
 修改历史      :
  1.日    期   : 2015年5月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_user_set_asoc_state(mac_user_stru *mac_user, mac_user_asoc_state_enum_uint8 value)
{
    mac_user->user_asoc_state = value;
}

/* ****************************************************************************
 功能描述  : 对用户的ht信息进行设置
 修改历史      :
  1.日    期   : 2015年5月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_user_set_ht_hdl(mac_user_stru *mac_user, const mac_user_ht_hdl_stru *ht_hdl)
{
    if (memcpy_s((hi_u8 *)(&mac_user->ht_hdl), sizeof(mac_user_ht_hdl_stru), (hi_u8 *)ht_hdl,
        sizeof(mac_user_ht_hdl_stru)) != EOK) {
        return;
    }
}

/* ****************************************************************************
 功能描述  : 获取用户的ht信息
 修改历史      :
  1.日    期   : 2015年5月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
WIFI_ROM_TEXT hi_void mac_user_get_ht_hdl(const mac_user_stru *mac_user, mac_user_ht_hdl_stru *ht_hdl)
{
    if (memcpy_s((hi_u8 *)ht_hdl, sizeof(mac_user_ht_hdl_stru), (hi_u8 *)(&mac_user->ht_hdl),
        sizeof(mac_user_ht_hdl_stru)) != EOK) {
        return;
    }
}

/* ****************************************************************************
 功能描述  : 设置用户wep加密密钥信息
 修改历史      :
  1.日    期   : 2014年1月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u32 mac_user_update_wep_key(mac_user_stru *mac_usr, hi_u8 multi_user_idx)
{
    mac_user_stru *multi_user = HI_NULL;

    multi_user = mac_user_get_user_stru(multi_user_idx);
    if (multi_user == HI_NULL) {
        return HI_ERR_CODE_SECURITY_USER_INVAILD;
    }
    if (multi_user->key_info.cipher_type != WLAN_80211_CIPHER_SUITE_WEP_104 &&
        multi_user->key_info.cipher_type != WLAN_80211_CIPHER_SUITE_WEP_40) {
        oam_error_log1(0, OAM_SF_WPA, "{mac_wep_add_usr_key::en_cipher_type==%d}", multi_user->key_info.cipher_type);
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }
    if (multi_user->key_info.default_index >= WLAN_MAX_WEP_KEY_COUNT) {
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }
    /* wep加密下，拷贝组播用户的密钥信息到单播用户 */
    /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memcpy_s(&mac_usr->key_info, sizeof(mac_key_mgmt_stru), &multi_user->key_info, sizeof(mac_key_mgmt_stru));
    mac_usr->user_tx_info.security.cipher_key_type = mac_usr->key_info.default_index + HAL_KEY_TYPE_PTK;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 判断mac地址是否全0
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 mac_addr_is_zero(const hi_u8 *mac_addr)
{
    hi_u8 zero_mac_addr[OAL_MAC_ADDR_LEN] = {0};

    if (mac_addr == HI_NULL) {
        return HI_TRUE;
    }

    return (0 == memcmp(zero_mac_addr, mac_addr, OAL_MAC_ADDR_LEN));
}

/* 代码ROM段结束位置 新增ROM代码请放在SECTION中 */
#undef __WIFI_ROM_SECTION__

/* ****************************************************************************
 功能描述  : 根据en_key_type，调用相应的函数，更新vap信息
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
wlan_priv_key_param_stru *mac_user_get_key(mac_user_stru *mac_user, hi_u8 key_id)
{
    if (key_id >= WLAN_NUM_TK + WLAN_NUM_IGTK) {
        return HI_NULL;
    }
    return &mac_user->key_info.ast_key[key_id];
}

/* ****************************************************************************
 功能描述  : 根据AP的operation ie获取ap的工作带宽
 修改历史      :
  1.日    期   : 2014年4月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_user_get_ap_opern_bandwidth(mac_user_stru *mac_user, wlan_bw_cap_enum_uint8 *pen_bandwidth_cap)
{
    mac_user_ht_hdl_stru *mac_ht_hdl = HI_NULL;
    wlan_bw_cap_enum_uint8 bandwidth_cap;

    bandwidth_cap = WLAN_BW_CAP_20M;
    /* 获取HT和VHT结构体指针 */
    mac_ht_hdl = &(mac_user->ht_hdl);

    if (mac_ht_hdl->ht_capable) {
        if (mac_ht_hdl->secondary_chan_offset != MAC_SCN) {
            bandwidth_cap = WLAN_BW_CAP_40M;
        }
    }

    *pen_bandwidth_cap = bandwidth_cap;
    mac_user_set_bandwidth_cap(mac_user, bandwidth_cap);
}

/* ****************************************************************************
 功能描述  : 设置ht cap信息
 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_user_set_ht_capable(mac_user_stru *mac_user, hi_u8 ht_capable)
{
    mac_user->ht_hdl.ht_capable = ht_capable;
}

/* ****************************************************************************
 功能描述  : 设置更新用户bit_spectrum_mgmt能力的信息
 修改历史      :
  1.日    期   : 2015年5月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_user_set_spectrum_mgmt(mac_user_stru *mac_user, hi_u8 spectrum_mgmt)
{
    mac_user->cap_info.spectrum_mgmt = spectrum_mgmt;
}

/* ****************************************************************************
 功能描述  : 设置用户公共区域的bit_apsd能力
 修改历史      :
  1.日    期   : 2015年5月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void mac_user_set_apsd(mac_user_stru *mac_user, hi_u8 apsd)
{
    mac_user->cap_info.apsd = apsd;
}

hi_u8 mac_user_is_user_valid(hi_u8 idx)
{
    return (g_us_user_res_map & ((hi_u16)BIT0 << idx)) ? HI_TRUE : HI_FALSE;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
