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
#include "oal_ext_if.h"
#include "mac_resource.h"
#include "mac_frame.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "mac_vap.h"
#include "hmac_11i.h"
#include "hmac_main.h"
#include "hmac_ext_if.h"
#include "hmac_crypto_tkip.h"
#include "hmac_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define cipher_suite_no_encrpy(i) (((i) & WLAN_ENCRYPT_BIT) ? 0 : 1)
#define cipher_suite_is_wep104(i) ((((i) & WLAN_WEP104_BIT) == WLAN_WEP104_BIT) ? 1 : 0)
#define cipher_suite_is_wep40(i)  ((((i) & WLAN_WEP104_BIT) == WLAN_WEP_BIT) ? 1 : 0)
#define cipher_suite_is_wpa(i)    ((((i) & WLAN_WPA_BIT) == WLAN_WPA_BIT) ? 1 : 0)
#define cipher_suite_is_wpa2(i)   ((((i) & WLAN_WPA2_BIT) == WLAN_WPA2_BIT) ? 1 : 0)
#define cipher_suite_is_tkip(i)   ((((i) & WLAN_TKIP_BIT) == WLAN_TKIP_BIT) ? 1 : 0)
#define cipher_suite_is_ccmp(i)   ((((i) & WLAN_CCMP_BIT) == WLAN_CCMP_BIT) ? 1 : 0)

/* ****************************************************************************
 功能描述  : 获取key
 修改历史      :
  1.日    期   : 2013年11月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static wlan_priv_key_param_stru *hmac_get_key_info(mac_vap_stru *mac_vap, hi_u8 *mac_addr, hi_u8 pairwise,
    hi_u8 key_index, hi_u8 *pus_user_idx)
{
    hi_u32 ret;
    mac_user_stru *mac_user = HI_NULL;
    hi_u8 macaddr_is_zero;

    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_WPA, "{hmac_get_key_info::mac_vap is null}");
        return HI_NULL;
    }

    /* 1.1 根据mac addr 找到对应sta索引号 */
    macaddr_is_zero = mac_addr_is_zero(mac_addr);
    if (!mac_11i_is_ptk(macaddr_is_zero, pairwise)) {
        /* 如果是组播用户，不能使用mac地址来查找,根据索引找到组播user内存区域 */
        *pus_user_idx = mac_vap->multi_user_idx;
    } else { /* 单播用户 */
        ret = mac_vap_find_user_by_macaddr(mac_vap, mac_addr, OAL_MAC_ADDR_LEN, pus_user_idx);
        if (ret != HI_SUCCESS) {
            return HI_NULL;
        }
    }

    mac_user = mac_user_get_user_stru(*pus_user_idx);
    if (mac_user == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_get_key_info::mac_res_get_mutil_mac_user null.}");
        return HI_NULL;
    }

    oam_info_log2(mac_vap->vap_id, OAM_SF_WPA, "{hmac_get_key_info::key_index=%d,pairwise=%d.}", key_index, pairwise);

    if (mac_addr != HI_NULL) {
        oam_info_log4(mac_vap->vap_id, OAM_SF_WPA, "{hmac_get_key_info::mac_addr[%d] = XX:XX:XX:%02X:%02X:%02X.}",
            *pus_user_idx, mac_addr[3], mac_addr[4], mac_addr[5]); /* 3 4 5 元素索引 */
    }

    return mac_user_get_key(mac_user, key_index);
}

#ifdef _PRE_WLAN_FEATURE_WAPI
/* ****************************************************************************
 功能描述  : 配置wapi key
 修改历史      :
  1.日    期   : 2015年2月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_wapi_add_key(mac_vap_stru *mac_vap, mac_addkey_param_stru *payload_addkey_params)
{
    hi_u8 key_index;
    hi_u8 pairwise;
    hi_u8 *mac_addr = HI_NULL;
    mac_key_params_stru *key_param = HI_NULL;
    hmac_wapi_stru *wapi = HI_NULL;
    hi_u32 ret;
    hi_u8 user_index = 0;
    mac_device_stru *mac_dev = HI_NULL;

    key_index = payload_addkey_params->key_index;
    if (key_index >= HMAC_WAPI_MAX_KEYID) {
        oam_error_log1(0, OAM_SF_WPA, "{hmac_config_wapi_add_key::keyid==%u Err!.}", key_index);
        return HI_FAIL;
    }

    pairwise = payload_addkey_params->pairwise;
    mac_addr = (hi_u8 *)payload_addkey_params->auc_mac_addr;
    key_param = &payload_addkey_params->key;

    if (key_param->key_len != (WAPI_KEY_LEN * 2))       /* 扩大2倍 */
        if (key_param->key_len != (WAPI_KEY_LEN * 2)) { /* 扩大2倍 */
            oam_error_log1(0, OAM_SF_WPA, "{hmac_config_wapi_add_key:: key_len %d Err!.}", key_param->key_len);
            return HI_FAIL;
        }

    if (pairwise == HI_TRUE) {
        ret = mac_vap_find_user_by_macaddr(mac_vap, mac_addr, OAL_MAC_ADDR_LEN, &user_index);
        if (ret != HI_SUCCESS) {
            oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
                "{hmac_config_wapi_add_key::mac_vap_find_user_by_macaddr failed. %u}", ret);
            return HI_FAIL;
        }
    }

    wapi = hmac_user_get_wapi_ptr(mac_vap, pairwise, user_index);
    if (wapi == HI_NULL) {
        oam_error_log0(0, OAM_SF_WPA, "{hmac_config_wapi_add_key:: get pst_wapi  Err!.}");
        return HI_FAIL;
    }

    hmac_wapi_add_key(wapi, key_index, key_param->auc_key);
    mac_dev = mac_res_get_dev();
    mac_dev->wapi = HI_TRUE;

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 保存wapi key并且同步
 修改历史      :
  1.日    期   : 2015年2月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
/* 用同一个事件回调表来定义的，改动一个就要改动所有的。有些需要const 有些不需要。现在没有办法改，lint_t告警屏蔽 */
hi_u32 hmac_config_wapi_add_key_and_sync(const mac_vap_stru *mac_vap, mac_addkey_param_stru *payload_addkey_params)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    hi_u32 ret;

    oam_warning_log2(0, OAM_SF_WPA, "{hmac_config_wapi_add_key_and_sync:: key idx==%u, pairwise==%u}",
        payload_addkey_params->key_index, payload_addkey_params->pairwise);

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_config_wapi_add_key_and_sync::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    ret = hmac_config_wapi_add_key(hmac_vap->base_vap, payload_addkey_params);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_config_wapi_add_key_and_sync::hmac_config_wapi_add_key fail[%d].}", ret);
        return ret;
    }

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_ADD_WAPI_KEY, 0, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_config_wapi_add_key_and_sync::WLAN_CFGID_ADD_WAPI_KEY send fail[%d].}", ret);
        return ret;
    }

    return ret;
}
#endif /* #ifdef _PRE_WLAN_FEATURE_WAPI */

/* ****************************************************************************
 函 数 名  : hmac_config_11i_add_key
 功能描述  : add key 逻辑，抛事件到DMAC
 输入参数  : frw_event_mem_stru *event_mem
 返 回 值  : 0:成功,其他:失败
 修改历史      :
  1.日    期   : 2013年12月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_11i_add_key(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u8 user_idx = 0;

    /* 2.1 获取参数 */
    mac_addkey_param_stru *payload_addkey_params = (mac_addkey_param_stru *)puc_param;
    hi_u8 key_index = payload_addkey_params->key_index;
    hi_u8 pairwise = payload_addkey_params->pairwise;
    hi_u8 *mac_addr = (hi_u8 *)payload_addkey_params->auc_mac_addr;
    mac_key_params_stru *key = &(payload_addkey_params->key);

#ifdef _PRE_WLAN_FEATURE_WAPI
    if (oal_unlikely(key->cipher == WLAN_CIPHER_SUITE_SMS4)) {
        return hmac_config_wapi_add_key_and_sync(mac_vap, payload_addkey_params);
    }
#endif

    /* 2.2 索引值最大值检查 */
    if (key_index >= WLAN_NUM_TK + WLAN_NUM_IGTK) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_key::invalid uc_key_index[%d].}", key_index);
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }

    oam_info_log3(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_key::mac addr=XX:XX:XX:%02X:%02X:%02X}",
        mac_addr[3], mac_addr[4], mac_addr[5]); /* 3 4 5 元素索引 */

    if (pairwise == HI_TRUE) {
        /* 单播密钥存放在单播用户中 */
        if (mac_vap_find_user_by_macaddr(mac_vap, mac_addr, OAL_MAC_ADDR_LEN, &user_idx) != HI_SUCCESS) {
            oam_error_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_key::find_user_by_macaddr fail.}");
            return HI_FAIL;
        }
    } else {
        /* 组播密钥存放在组播用户中 */
        user_idx = mac_vap->multi_user_idx;
    }

    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_key::get_mac_user null.idx:%u}", user_idx);
        return HI_ERR_CODE_SECURITY_USER_INVAILD;
    }
#ifdef _PRE_WLAN_FEATURE_WAPI
    /* 11i的情况下，关掉wapi端口 */
    hmac_wapi_reset_port(&hmac_user->wapi);
    mac_device_stru *mac_dev = mac_res_get_dev();
    mac_dev->wapi = HI_FALSE;
#endif

    /* 3.1 将加密属性更新到用户中 */
    hi_u32 ret = mac_vap_add_key(mac_vap, hmac_user->base_user, key_index, key);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_key::mac_11i_add_key fail[%d].}", ret);
        return ret;
    }
    /* 设置用户8021x端口合法性的状态为合法 */
    hmac_user->base_user->port_valid = HI_TRUE;

    /* **************************************************************************
    抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_ADD_KEY, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_key:hmac_config_send_event fail[%d]}", ret);
    }

    return ret;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
功能描述  : 获取密钥，不需要抛事件到DMAC,直接从hmac数据区中拿数据
输入参数  : frw_event_mem_stru *event_mem
返 回 值  : 0:成功,其他:失败
修改历史      :
1.日    期   : 2013年8月16日
    作    者   : HiSilicon
  修改内容   : 新生成函数
2.日    期   : 2014年1月4日
    作    者   : HiSilicon
  修改内容   : 使用局部变量替代malloc，以减少释放内存的复杂度
**************************************************************************** */
hi_u32 hmac_config_11i_get_key(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    wlan_priv_key_param_stru *priv_key = HI_NULL;
    oal_key_params_stru key;
    hi_u8 key_index;
    hi_u8 pairwise;
    hi_u8 *mac_addr = HI_NULL;
    hi_void *cookie = HI_NULL;
    mac_getkey_param_stru *payload_getkey_params = HI_NULL;
    hi_u8 us_user_idx = MAC_INVALID_USER_ID;
    hi_unref_param(us_len);

    /* 2.1 获取参数 */
    payload_getkey_params = (mac_getkey_param_stru *)puc_param;
    key_index = payload_getkey_params->key_index;
    pairwise = payload_getkey_params->pairwise;
    mac_addr = payload_getkey_params->puc_mac_addr;
    cookie = payload_getkey_params->cookie;

    /* 2.2 索引值最大值检查 */
    if (key_index >= WLAN_NUM_TK + WLAN_NUM_IGTK) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_get_key::uc_key_index invalid[%d].}", key_index);
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }

    /* 3.1 获取密钥 */
    priv_key = hmac_get_key_info(mac_vap, mac_addr, pairwise, key_index, &us_user_idx);
    if (priv_key == HI_NULL) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_get_key::key is null.pairwise[%d], key_idx[%d]}",
            pairwise, key_index);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 四次握手获取密钥信息长度为0 是正常值,不应该为error 级别打印 */
    if (priv_key->key_len == 0) {
        oam_info_log2(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_get_key::key len = 0.pairwise[%d], key_idx[%d]}",
            pairwise, key_index);
        return HI_ERR_CODE_SECURITY_KEY_LEN;
    }

    /* 4.1 密钥赋值转换 */
    /* 安全编程规则6.6例外(1) 对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(&key, sizeof(oal_key_params_stru), 0, sizeof(key));
    key.key = priv_key->auc_key;
    key.key_len = (hi_s32)priv_key->key_len;
    key.seq = priv_key->auc_seq;
    key.seq_len = (hi_s32)priv_key->seq_len;
    key.cipher = priv_key->cipher;

    /* 5.1 调用回调函数 */
    if (payload_getkey_params->callback != HI_NULL) {
        payload_getkey_params->callback(cookie, &key);
    }

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 处理remove key事件，抛事件到DMAC
 输入参数  : mac_vap_stru *pst_mac_vap, hi_u16 us_len, hi_u8 *puc_param
 返 回 值  : 0:成功,其他:失败
 修改历史      :
  1.日    期   : 2014年1月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_11i_remove_key(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *param)
{
    mac_removekey_param_stru *removekey = (mac_removekey_param_stru *)param;
    mac_user_stru            *mac_user  = HI_NULL;
    wlan_cfgid_enum_uint16    cfgid     = WLAN_CFGID_REMOVE_WEP_KEY;
    hi_u8                     user_idx  = MAC_INVALID_USER_ID;

    /* 2.1 获取参数 */
    hi_u8 key_index = removekey->key_index;
    hi_u8 pairwise  = removekey->pairwise;

    oam_info_log2(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_remove_key::key%d,pairwise%d}", key_index, pairwise);

    /* 2.2 索引值最大值检查 */
    if ((key_index >= WLAN_NUM_TK + WLAN_NUM_IGTK) || (key_index >= WLAN_NUM_DOT11WEPDEFAULTKEYVALUE)) {
        /* 内核会下发删除6 个组播密钥，驱动现有6个组播密钥保存空间 */
        oam_info_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_remove_key::invalid key_index%d}", key_index);
        return HI_SUCCESS;
    }

    /* 3.1 获取本地密钥信息 */
    wlan_priv_key_param_stru *key = hmac_get_key_info(mac_vap, removekey->auc_mac_addr, pairwise, key_index, &user_idx);
    if (key == HI_NULL) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_remove_key::user_idx=%d}", user_idx);
        return ((user_idx == MAC_INVALID_USER_ID) ? HI_SUCCESS : HI_ERR_CODE_SECURITY_USER_INVAILD);
    }

    if (key->key_len == 0) {
        /* 如果检测到密钥没有使用， 则直接返回正确 */
        oam_info_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_remove_key::ul_key_len=0.}");
        return HI_SUCCESS;
    }

    /* 4.1 区分是wep还是wpa */
    if ((key->cipher == WLAN_CIPHER_SUITE_WEP40) || (key->cipher == WLAN_CIPHER_SUITE_WEP104)) {
        mac_mib_set_wep(mac_vap, key_index);
    } else {
        hi_u8 macaddr_is_zero = mac_addr_is_zero(removekey->auc_mac_addr);
        if (mac_11i_is_ptk(macaddr_is_zero, pairwise)) {
            mac_user = mac_vap_get_user_by_addr(mac_vap, removekey->auc_mac_addr);
            if (mac_user == HI_NULL) {
                return HI_ERR_CODE_SECURITY_USER_INVAILD;
            }
            mac_user->user_tx_info.security.cipher_key_type = HAL_KEY_TYPE_BUTT;
        } else {
            mac_user = mac_user_get_user_stru(mac_vap->multi_user_idx);
            if (mac_user == HI_NULL) {
                return HI_ERR_CODE_SECURITY_USER_INVAILD;
            }
        }
        cfgid = WLAN_CFGID_REMOVE_KEY;
        mac_user->port_valid = HI_FALSE;
    }

    /* 4.2 抛事件到dmac层处理 */
    hi_u32 ret = hmac_config_send_event(mac_vap, cfgid, us_len, param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_remove_key:SendEvent Err%d,cfgid%d}", ret, cfgid);
        return ret;
    }

    /* 5.1 删除密钥成功，设置密钥长度为0 */
    key->key_len = 0;

    return ret;
}

/* ****************************************************************************
 函 数 名  : hmac_config_11i_set_default_key
 功能描述  : 处理set default key事件，抛事件到DMAC
 输入参数  : mac_vap_stru *pst_mac_vap, hi_u16 us_len, hi_u8 *puc_param
 输出参数  : hi_u32
 返 回 值  : 0:成功,其他:失败
 修改历史      :
  1.日    期   : 2014年1月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2014年7月31日
    作    者   : HiSilicon
    修改内容   : 合并设置数据帧默认密钥和设置管理帧默认密钥函数
**************************************************************************** */
/* 用同一个事件回调表来定义的，改动一个就要改动所有的。有些需要const 有些不需要。现在没有办法改，lint_t告警屏蔽 */
hi_u32 hmac_config_11i_set_default_key(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    hi_u32 ret = HI_SUCCESS;
    hi_u8 key_index;
    hi_u8 unicast;
    hi_u8 multicast;
    mac_setdefaultkey_param_stru *payload_setdefaultkey_params = HI_NULL;

    /* 2.1 获取参数 */
    payload_setdefaultkey_params = (mac_setdefaultkey_param_stru *)puc_param;
    key_index = payload_setdefaultkey_params->key_index;
    unicast = payload_setdefaultkey_params->unicast;
    multicast = payload_setdefaultkey_params->multicast;

    /* 2.2 索引值最大值检查 */
    if (key_index >= (WLAN_NUM_TK + WLAN_NUM_IGTK)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_set_default_key::invalid uc_key_index[%d].}",
            key_index);
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }

    /* 2.3 参数有效性检查 */
    if ((multicast == HI_FALSE) && (unicast == HI_FALSE)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_set_default_key::not ptk or gtk,invalid mode.}");
        return HI_ERR_CODE_SECURITY_PARAMETERS;
    }

    if (key_index >= WLAN_NUM_TK) {
        /* 3.1 设置default mgmt key属性 */
        ret = mac_vap_set_default_mgmt_key(mac_vap, key_index);
    } else {
        ret = mac_vap_set_default_key(mac_vap, key_index);
    }

    if (ret != HI_SUCCESS) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_set_default_key::set key[%d] failed[%d].}",
            key_index, ret);
        return ret;
    }

    /* **************************************************************************
    抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_DEFAULT_KEY, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_config_11i_set_default_key::hmac_config_send_event failed[%d].}", ret);
    }
    oam_info_log3(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_set_default_key::key_id[%d] un[%d] mu[%d] OK}",
        key_index, unicast, multicast);
    return ret;
}

/* ****************************************************************************
 功能描述  : add wep加密，抛事件到DMAC
 输入参数  : mac_vap_stru *pst_mac_vap, hi_u16 us_len, hi_u8 *puc_param
 返 回 值  : 0:成功,其他:失败
 修改历史      :
  1.日    期   : 2013年11月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_config_11i_add_wep_entry(mac_vap_stru *mac_vap, hi_u16 us_len, const hi_u8 *puc_param)
{
    mac_user_stru *mac_user = HI_NULL;
    hi_u32 ret;

    mac_user = mac_vap_get_user_by_addr(mac_vap, puc_param);
    if (mac_user == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_wep_entry::mac_user NULL}");
        return HI_ERR_CODE_PTR_NULL;
    }

    ret = mac_user_update_wep_key(mac_user, mac_vap->multi_user_idx);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_wep_entry::mac_wep_add_usr_key failed[%d].}",
            ret);
        return ret;
    }

    /* **************************************************************************
    抛事件到DMAC层, 同步DMAC数据
    ************************************************************************** */
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_ADD_WEP_ENTRY, us_len, puc_param);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_config_11i_add_wep_entry::hmac_config_send_event failed[%d].}", ret);
    }

    /* 设置用户的发送加密套件 */
    oam_info_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_config_11i_add_wep_entry:: usridx[%d] OK.}",
        mac_user->us_assoc_id);

    return ret;
}

/* ****************************************************************************
 功能描述  : 初始化加密数据。
 修改历史      :
  1.日    期   : 2013年10月18日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_init_security(mac_vap_stru *mac_vap, hi_u8 *mac_addr, hi_u16 addr_len)
{
    hi_u32 ret = HI_SUCCESS;
    hi_u16 us_len;
    hi_u8 *puc_param = HI_NULL;

    if (mac_is_wep_enabled(mac_vap) == HI_TRUE) {
        puc_param = mac_addr;
        us_len = addr_len;
        ret = hmac_config_11i_add_wep_entry(mac_vap, us_len, puc_param);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : 检测单播wpa密钥是否匹配
 输入参数  : wlan_mib_ieee802dot11_stru *pst_mib_info
             hi_u8 uc_policy
 返 回 值  : hi_u8    HI_TRUE:匹配成功
                                    HI_FALSE:匹配失败
 修改历史      :
  1.日    期   : 2013年8月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_check_pcip_wpa_policy(const wlan_mib_ieee802dot11_stru *mib_info, hi_u8 policy)
{
    hi_u8 loop = 0;
    for (loop = 0; loop < MAC_PAIRWISE_CIPHER_SUITES_NUM; loop++) {
        /* 检测单播密钥是否使能和匹配 */
        if ((mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_activated == HI_TRUE) &&
            (mib_info->ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_implemented == policy)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 检测单播wpa2密钥是否匹配
 输入参数  : wlan_mib_ieee802dot11_stru *pst_mib_info
             hi_u8 uc_policy
 返 回 值  : hi_u8    HI_TRUE:匹配成功
                                    HI_FALSE:匹配失败
 修改历史      :
  1.日    期   : 2013年8月31日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_check_pcip_wpa2_policy(const wlan_mib_ieee802dot11_stru *mib_info, hi_u8 policy)
{
    hi_u8 loop = 0;
    for (loop = 0; loop < MAC_PAIRWISE_CIPHER_SUITES_NUM; loop++) {
        /* 检测单播密钥是否使能和匹配 */
        if ((mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_activated == HI_TRUE) &&
            (mib_info->ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[loop].dot11_rsna_config_pairwise_cipher_implemented == policy)) {
            return HI_TRUE;
        }
    }
    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 检测单播密钥是否匹配
 输入参数  : wlan_mib_ieee802dot11_stru *pst_mib_info
             hi_u8 uc_policy
             hi_u8 uc_80211i_mode
 返 回 值  : hi_u32    HI_SUCCESS:匹配成功
                           HI_FAIL:匹配失败
 修改历史      :
  1.日    期   : 2013年12月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_check_pcip_policy(const wlan_mib_ieee802dot11_stru *mib_info, hi_u8 policy, hi_u8 is_80211i_mode)
{
    hi_u8 ret = HI_FALSE;

    if (is_80211i_mode == DMAC_WPA_802_11I) {
        ret = hmac_check_pcip_wpa_policy(mib_info, policy);
    } else if (is_80211i_mode == DMAC_RSNA_802_11I) {
        ret = hmac_check_pcip_wpa2_policy(mib_info, policy);
    } else {
        ret = HI_FALSE;
    }

    if (ret == HI_TRUE) {
        return HI_SUCCESS;
    } else {
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }
}

/* ****************************************************************************
 功能描述  : 检查RSN能力是否匹配
 输入参数  : [1]mac_vap
             [2]puc_rsn_ie
             [3]pen_status_code
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_check_rsn_capability(const mac_vap_stru *mac_vap, const hi_u8 *puc_rsn_ie,
    mac_status_code_enum_uint16 *pen_status_code)
{
    wlan_mib_ieee802dot11_stru *mib_info = HI_NULL;
    hi_u16 us_rsn_capability;
    hi_u8 preauth_activated;
    hi_u8 dot11_rsnamfpr;
    hi_u8 dot11_rsnamfpc;

    mib_info = mac_vap->mib_info;
    if (mib_info == HI_NULL) {
        *pen_status_code = MAC_INVALID_RSN_INFO_CAP;
        oam_error_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_check_rsn_capability::pst_mib_info null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    us_rsn_capability = mac_get_rsn_capability(puc_rsn_ie);

    /* 2.1 预认证能力检查 */
    preauth_activated = us_rsn_capability & BIT0;
    if (preauth_activated != mib_info->wlan_mib_privacy.dot11_rsna_preauthentication_activated) {
        *pen_status_code = MAC_INVALID_RSN_INFO_CAP;
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_check_rsn_capability::VAP not supported PreauthActivated[%d].}", preauth_activated);
        return HI_ERR_CODE_SECURITY_AUTH_TYPE;
    }

    /* 3.1 管理帧加密(80211w)能力检查 */
    dot11_rsnamfpr = (us_rsn_capability & BIT6) ? HI_TRUE : HI_FALSE;
    dot11_rsnamfpc = (us_rsn_capability & BIT7) ? HI_TRUE : HI_FALSE;
    /* 3.1.1 本地强制，对端没有MFP能力 */
    if ((mib_info->wlan_mib_privacy.dot11_rsnamfpr == HI_TRUE) && (dot11_rsnamfpc == HI_FALSE)) {
        *pen_status_code = MAC_MFP_VIOLATION;
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_check_rsn_capability::refuse with NON MFP[%d].}",
            preauth_activated);
        return HI_ERR_CODE_SECURITY_CAP_MFP;
    }
    /* 3.1.2 对端强制，本地没有MFP能力 */
    if ((mib_info->wlan_mib_privacy.dot11_rsnamfpc == HI_FALSE) && (dot11_rsnamfpr == HI_TRUE)) {
        *pen_status_code = MAC_MFP_VIOLATION;
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA, "{hmac_check_rsn_capability::VAP not supported RSNA MFP[%d].}",
            preauth_activated);
        return HI_ERR_CODE_SECURITY_CAP_MFP;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 根据WPA/WPA2模式获取WPA/WPA2 oui
 修改历史      :
  1.日    期   : 2013年12月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_get_security_oui(hi_u8 is_80211i_mode, hi_u8 *auc_oui)
{
    if (is_80211i_mode == DMAC_WPA_802_11I) {
        auc_oui[0] = (hi_u8)MAC_WLAN_OUI_MICRO0;
        auc_oui[1] = (hi_u8)MAC_WLAN_OUI_MICRO1;
        auc_oui[2] = (hi_u8)MAC_WLAN_OUI_MICRO2; /* 2 元素索引 */
    } else if (is_80211i_mode == DMAC_RSNA_802_11I) {
        auc_oui[0] = (hi_u8)MAC_WLAN_OUI_RSN0;
        auc_oui[1] = (hi_u8)MAC_WLAN_OUI_RSN1;
        auc_oui[2] = (hi_u8)MAC_WLAN_OUI_RSN2; /* 2 元素索引 */
    } else {
        return HI_ERR_WIFI_HMAC_INVALID_PARAMETER;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 从数据包中提取出单播加密套件信息
 输入参数  : hi_u8 *puc_frame   WPA/WPA2 信息元素中，保存的单播起始地址
             hi_u8 *puc_len     加密信息长度
             hi_u8 *puc_oui     WPA/WPA2 信息元素OUI
 返 回 值  : hi_u8              获取的单播套件信息
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hmac_get_pcip_policy_auth(const hi_u8 *puc_frame, hi_u8 *puc_len)
{
    hi_u8 indext;
    hi_u8 pcip_policy = 0;

    /* *********************************************************************** */
    /*                  RSN Element Pairwise Ciper Format                    */
    /* --------------------------------------------------------------------- */
    /* | Pairwise Cipher Count | Pairwise Cipher Suite List |                */
    /* --------------------------------------------------------------------- */
    /* |         2             |           4*m              |                */
    /* --------------------------------------------------------------------- */
    /* *********************************************************************** */
    /* 获取数据包中，成对密钥套件总的字节数 */
    *puc_len = (hi_u8)(puc_frame[0] * 4) + 2; /* 4 2 用于计算 */

    /* ASSOC REQ 中的单播套件个数只能为1个 */
    if (puc_frame[0] == 1) {
        indext = 2;                      /* 忽略单播套件的2 字节 */
        indext += MAC_OUI_LEN;           /* 忽略OUI 长度 */
        pcip_policy = puc_frame[indext]; /* 获取单播加密套件 */
    } else {
        pcip_policy = 0xFF;
    }

    return pcip_policy;
}

/* ****************************************************************************
 功能描述  : 从数据包中提取出认证套件信息
 输入参数  : [1]puc_frame
             [2]len
 返 回 值  : hi_u8
**************************************************************************** */
hi_u8 hmac_get_auth_policy_auth(const hi_u8 *puc_frame, hi_u8 *len)
{
    hi_u8 index;
    hi_u8 auth_policy = 0;

    /* *********************************************************************** */
    /*                  RSN Element AKM Suite Format                         */
    /* --------------------------------------------------------------------- */
    /* |    AKM Cipher Count   |   AKM Cipher Suite List    |                */
    /* --------------------------------------------------------------------- */
    /* |         2             |           4*s              |                */
    /* --------------------------------------------------------------------- */
    /* *********************************************************************** */
    /* 获取数据包中，认证套件总的字节数 */
    *len = (hi_u8)(puc_frame[0] * 4) + 2; /* 4 2 用于计算 */

    /* ASSOC REQ 中的单播套件个数只能为1个 */
    if (puc_frame[0] == 1) {
        index = 2;                      /* 忽略AKM 套件数的2字节 */
        index += MAC_OUI_LEN;           /* 忽略OUI 长度 */
        auth_policy = puc_frame[index]; /* 获取认证套件 */
    } else {
        auth_policy = 0xFF;
    }
    return auth_policy;
}

/* ****************************************************************************
 功能描述  : STA 检测扫描到的单播加密能力和设备能力是否匹配。
 输入参数  : [1]mib_info
             [2]puc_pcip_policy_match
             [3]is_802_11i_mode
             [4]puc_pcip_policy
 返 回 值  : hi_u32
**************************************************************************** */
hi_u32 hmac_check_join_req_parewise_cipher_supplicant(const wlan_mib_ieee802dot11_stru *mib_info,
    hi_u8 *puc_pcip_policy_match, hi_u8 is_802_11i_mode, const hi_u8 *puc_pcip_policy)
{
    hi_u8 loop = 0;
    hi_u8 ret = HI_FALSE;
    if (puc_pcip_policy == HI_NULL || puc_pcip_policy_match == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY,
            "{hmac_check_join_req_parewise_cipher_supplicant::puc_pcip_policy/puc_pcip_policy_match is NULL!}\r\n");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 初始设置单播匹配为无效数据 */
    *puc_pcip_policy_match = 0xFF;

    /* 从STA mib 中查找和 AP 能力匹配的单播加密方式 */
    for (loop = 0; loop < MAC_PAIRWISE_CIPHER_SUITES_NUM; loop++) {
        if (puc_pcip_policy[loop] == WLAN_80211_CIPHER_SUITE_GROUP_CIPHER) {
            /* 成对密钥套件选择组播加密套件 */
            *puc_pcip_policy_match = WLAN_80211_CIPHER_SUITE_GROUP_CIPHER;
            break;
        } else if (puc_pcip_policy[loop] == 0xFF) {
            /* 如果没有找到，继续查找成对加密套件 */
            continue;
        }

        /* 检测成对密钥套件 */
        /* 检测单播密钥套件 */
        if (is_802_11i_mode == DMAC_WPA_802_11I) {
            ret = hmac_check_pcip_wpa_policy(mib_info, puc_pcip_policy[loop]);
        } else {
            ret = hmac_check_pcip_wpa2_policy(mib_info, puc_pcip_policy[loop]);
        }
        if (ret == HI_TRUE) {
            *puc_pcip_policy_match = puc_pcip_policy[loop];
            if (WLAN_80211_CIPHER_SUITE_CCMP == puc_pcip_policy[loop]) {
                break;
            }
        }
    }

    /* 检测单播密钥是否匹配成功 */
    if (*puc_pcip_policy_match == 0xFF) {
        oam_error_log0(0, OAM_SF_WPA, "{hmac_check_join_req_parewise_cipher_supplicant::pariwise not match.}");
        for (loop = 0; loop < MAC_PAIRWISE_CIPHER_SUITES_NUM; loop++) {
            oam_error_log2(0, OAM_SF_WPA, "{hmac_check_join_req_parewise_cipher_supplicant::user pairwise[%d]=%d.}",
                loop, puc_pcip_policy[loop]);
        }
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : STA 检测扫描到的认证套件和设备能力是否匹配。
 输入参数  : [1]pst_mib_info
             [2]puc_auth_policy_match
             [3]puc_auth_policy
 返 回 值  : static hi_u32
**************************************************************************** */
static hi_u32 hmac_check_join_req_auth_suite_supplicant(const wlan_mib_ieee802dot11_stru *mib_info,
    hi_u8 *puc_auth_policy_match, const hi_u8 *puc_auth_policy)
{
    hi_u8 loop = 0;

    /* 设置认证匹配为无效数据 */
    *puc_auth_policy_match = 0xFF;

    /* 查找STA 和 AP 能力匹配的认证方式 */
    for (loop = 0; loop < MAC_AUTHENTICATION_SUITE_NUM; loop++) {
        /* 如果没有找到对应的认证套件，则继续 */
        if (puc_auth_policy[loop] == 0xFF) {
            continue;
        }

        /* 如果找到对应认证套件，则和本地认证套件比较 */
        if (mac_check_auth_policy(mib_info, puc_auth_policy[loop]) == HI_TRUE) {
            *puc_auth_policy_match = puc_auth_policy[loop];
        }
    }

    if (*puc_auth_policy_match == 0xFF) {
        oam_error_log1(0, OAM_SF_WPA, "{hmac_check_join_req_security_cap_supplicant::user auth=%d.}",
            puc_auth_policy[0]);
        return HI_ERR_CODE_SECURITY_AUTH_TYPE;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : STA 检测扫描到的 RSN/WPA 加密能力和设备能力是否匹配。
 输入参数  : mac_bss_dscr_stru *st_bss_dscr     AP  的BSS 结构
             hi_u8  uc_802_11i_mode         STA 支持的安全加密模式
             hi_u8 *puc_grp_policy_match
             hi_u8 *puc_pcip_policy_match
             hi_u8 *puc_auth_policy_match
             hi_u8  uc_802_11i_mode         WPA/WPA2
 返 回 值  : hi_u32 HI_SUCCESS  匹配成功
                        HI_FAIL  匹配失败
 修改历史      :
  1.日    期   : 2013年8月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_check_join_req_security_cap_supplicant(mac_bss_dscr_stru *bss_dscr,
    const wlan_mib_ieee802dot11_stru *mib_info, const hmac_cap_supplicant_info_stru *cap_supplicant_info,
    hi_u8 is_802_11i_mode)
{
    hi_u8 *puc_pcip_policy = HI_NULL;
    hi_u8 *puc_auth_policy = HI_NULL;
    hi_u8 grp_policy = 0;
    hi_u32 check_status;

    if ((bss_dscr->bss_sec_info.bss_80211i_mode & is_802_11i_mode) != is_802_11i_mode) {
        oam_error_log1(0, OAM_SF_WPA, "{hmac_check_join_req_security_cap_supplicant::80211i modeh=%d.}",
            is_802_11i_mode);
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }

    if (is_802_11i_mode == DMAC_RSNA_802_11I) {
        puc_pcip_policy = bss_dscr->bss_sec_info.auc_rsn_pairwise_policy;
        puc_auth_policy = bss_dscr->bss_sec_info.auc_rsn_auth_policy;
        grp_policy = bss_dscr->bss_sec_info.rsn_grp_policy;
    } else {
        puc_pcip_policy = bss_dscr->bss_sec_info.auc_wpa_pairwise_policy;
        puc_auth_policy = bss_dscr->bss_sec_info.auc_wpa_auth_policy;
        grp_policy = bss_dscr->bss_sec_info.wpa_grp_policy;
    }

    /* 组播密钥 */
    *(cap_supplicant_info->puc_grp_policy_match) = grp_policy;

    /* 检查单播密钥套件 */
    check_status = hmac_check_join_req_parewise_cipher_supplicant(mib_info, cap_supplicant_info->puc_pcip_policy_match,
        is_802_11i_mode, puc_pcip_policy);
    if (check_status != HI_SUCCESS) {
        return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
    }

    /* 检查认证密钥套件 */
    check_status = hmac_check_join_req_auth_suite_supplicant(mib_info, cap_supplicant_info->puc_auth_policy_match,
        puc_auth_policy);
    if (check_status != HI_SUCCESS) {
        return HI_ERR_CODE_SECURITY_AUTH_TYPE;
    }

    oam_info_log3(0, OAM_SF_WPA, "{hmac_check_join_req_security_cap_supplicant::group=%d, pairwise=%d, auth=%d.}",
        *(cap_supplicant_info->puc_grp_policy_match), *(cap_supplicant_info->puc_pcip_policy_match),
        *(cap_supplicant_info->puc_auth_policy_match));

    return HI_SUCCESS;
}

static hi_u32 hmac_check_security_capability_supplicant_check(const mac_vap_stru *mac_vap,
    const mac_bss_dscr_stru *bss_dscr)
{
    oam_info_log4(mac_vap->vap_id, OAM_SF_WPA,
        "{hmac_check_security_capability_supplicant_check :: mode %d,active %d,wpa %d,wpa2 %d}",
        bss_dscr->bss_sec_info.bss_80211i_mode, mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated,
        mac_vap->cap_flag.wpa, mac_vap->cap_flag.wpa2);

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG,
            "{hmac_check_security_capability_supplicant_check::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (hmac_vap->wps_active == HI_TRUE) {
        oam_info_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_check_security_capability_supplicant_check::WPS enable.}");
        return HI_CONTINUE;
    }
#ifdef _PRE_WLAN_FEATURE_MESH
    if (bss_dscr->is_hisi_mesh == HI_TRUE) {
        oam_info_log0(0, OAM_SF_WPA,
            "{hmac_check_security_capability_supplicant_check::Connect Hisi-Mesh,no need check!.}");
        return HI_CONTINUE;
    }
#endif

    return HI_SUCCESS;
}


/* ****************************************************************************
 功能描述  : STA 在JOIN 前检测加密能力是否匹配
 输入参数  : mac_vap_stru pst_mac_vap
             mac_bss_dscr_stru *pst_bss_dscr
 返 回 值  : hi_u32 HI_SUCCESS 匹配成功
                        HI_FAIL 匹配失败
 修改历史      :
  1.日    期   : 2013年9月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_u32 hmac_check_security_capability_supplicant(const mac_vap_stru *mac_vap, mac_bss_dscr_stru *bss_dscr)
{
    hi_u8 grp_policy_match = 0xFF;
    hi_u8 pcip_policy_match = 0xFF;
    hi_u8 auth_policy_match = 0xFF;
    hi_u8 is_80211i_mode = 0x00;
    hi_u32 ret = HI_FAIL;
    hmac_cap_supplicant_info_stru supplicant_info;
    wlan_mib_ieee802dot11_stru *mib_info = mac_vap->mib_info; /* 本机的 MIB 值 */

    hi_u32 retval = hmac_check_security_capability_supplicant_check(mac_vap, bss_dscr);
    if (retval == HI_ERR_CODE_PTR_NULL) {
        return HI_ERR_CODE_PTR_NULL;
    } else if (retval == HI_CONTINUE) {
        return HI_SUCCESS;
    }

    if (mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated == HI_TRUE) {
        /* 检查STA 和 AP 加密能力是否匹配, 并更新到对应的match 中 */
        /* 在WPA/WPA2 混合模式下，优先选择WPA2 */
        supplicant_info.puc_grp_policy_match = &grp_policy_match;
        supplicant_info.puc_pcip_policy_match = &pcip_policy_match;
        supplicant_info.puc_auth_policy_match = &auth_policy_match;
        if ((bss_dscr->bss_sec_info.bss_80211i_mode & DMAC_RSNA_802_11I) && (mac_vap->cap_flag.wpa2 == HI_TRUE)) {
            ret = hmac_check_join_req_security_cap_supplicant(bss_dscr, mib_info, &supplicant_info, DMAC_RSNA_802_11I);
            if (ret == HI_SUCCESS) {
                is_80211i_mode = DMAC_RSNA_802_11I;
            }
        }

        if ((ret == HI_FAIL) && (bss_dscr->bss_sec_info.bss_80211i_mode & DMAC_WPA_802_11I) &&
            (mac_vap->cap_flag.wpa == HI_TRUE)) {
            ret = hmac_check_join_req_security_cap_supplicant(bss_dscr, mib_info, &supplicant_info, DMAC_WPA_802_11I);
            if (ret == HI_SUCCESS) {
                is_80211i_mode = DMAC_WPA_802_11I;
            }
        }

        if (ret != HI_SUCCESS) {
            oam_error_log1(0, OAM_SF_WPA, "{hmac_check_security_capability_supplicant::WPA & WPA2 not match[%d]}", ret);
            return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
        }

        bss_dscr->bss_sec_info.bss_80211i_mode = is_80211i_mode;
        bss_dscr->bss_sec_info.grp_policy_match = grp_policy_match;
        bss_dscr->bss_sec_info.pairwise_policy_match = pcip_policy_match;
        bss_dscr->bss_sec_info.auth_policy_match = auth_policy_match;
    } else { /* 本vap不支持rsn,对端支持rsn, 返回失败 */
        if (bss_dscr->bss_sec_info.bss_80211i_mode & (DMAC_RSNA_802_11I | DMAC_WPA_802_11I)) {
            oam_error_log1(0, OAM_SF_WPA, "{hmac_check_security_capability_supplicant::WPA/WPA2 not support! mode=%d}",
                bss_dscr->bss_sec_info.bss_80211i_mode);
            return HI_ERR_CODE_SECURITY_CHIPER_TYPE;
        }
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 检测期望join 的AP能力信息是否匹配
 输入参数  : mac_vap_stru pst_mac_vap         STA 自己
             mac_bss_dscr_stru *pst_bss_dscr  AP bss 信息
 返 回 值  : hi_u32 HI_SUCCESS 匹配成功
                        HI_FAIL 匹配失败
 修改历史      :
  1.日    期   : 2013年9月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_check_capability_mac_phy_supplicant(mac_vap_stru *mac_vap, mac_bss_dscr_stru *bss_dscr)
{
    hi_u32 ret;

    /* 根据协议模式重新初始化STA HT/VHT mib值 */
    mac_vap_config_vht_ht_mib_by_protocol(mac_vap);

    hi_u8 check_bss_ret = hmac_check_bss_cap_info(bss_dscr->us_cap_info, mac_vap);
    if (check_bss_ret != HI_TRUE) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_check_capability_mac_phy_supplicant::hmac_check_bss_cap_info Err[%d]}", check_bss_ret);
    }

    /* check bss capability info PHY,忽略PHY能力不匹配的AP */
    mac_vap_check_bss_cap_info_phy_ap(bss_dscr->us_cap_info, mac_vap);

    ret = hmac_check_security_capability_supplicant(mac_vap, bss_dscr);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA,
            "{hmac_check_capability_mac_phy_supplicant::hmac_check_security_capability_supplicant failed[%d].}", ret);
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 根据STA 的单播加密方式和保护模式，更新STA 工作速率
             在WEP / TKIP 加密模式下，不能工作在HT MODE
 输入参数  : mac_vap_stru *pst_mac_vap
 修改历史      :
  1.日    期   : 2013年9月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_update_pcip_policy_prot_supplicant(mac_vap_stru *mac_vap, hi_u8 pcip_policy_match)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    mac_cfg_mode_param_stru cfg_mode;
    hi_u8 protocol_fall_flag = HI_FALSE;

    cfg_mode.protocol = mac_vap->protocol;

    if ((pcip_policy_match == WLAN_80211_CIPHER_SUITE_TKIP) ||
        (pcip_policy_match == WLAN_80211_CIPHER_SUITE_WEP_104) ||
        (pcip_policy_match == WLAN_80211_CIPHER_SUITE_WEP_40) ||
        (pcip_policy_match == WLAN_80211_CIPHER_SUITE_WAPI)) {
        if ((mac_vap->protocol >= WLAN_HT_MODE) && (mac_vap->protocol < WLAN_PROTOCOL_BUTT)) {
            if (mac_vap->channel.band == WLAN_BAND_2G) {
                cfg_mode.protocol = WLAN_MIXED_ONE_11G_MODE;
                mac_vap->channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
                protocol_fall_flag = HI_TRUE;
            }
        }
    }
    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_update_pcip_policy_prot_supplicant::pst_hmac_vap null.}");
        return;
    }
    /* 降协议或恢复后需要对bit_protocol_fall更新 */
    hmac_vap->protocol_fall = protocol_fall_flag;

    if (cfg_mode.protocol >= WLAN_HT_MODE) {
        hmac_vap->tx_aggr_on = HI_TRUE;
    } else {
        hmac_vap->tx_aggr_on = HI_FALSE;
    }

    mac_vap_init_by_protocol(mac_vap, cfg_mode.protocol);

    oam_info_log2(mac_vap->vap_id, OAM_SF_WPA,
        "{hmac_update_pcip_policy_prot_supplicant::en_protocol=%d, bandwidth=%d.}", mac_vap->protocol,
        mac_vap->channel.en_bandwidth);
}

/* ****************************************************************************
 功能描述  : 更新STA 加密的mib 信息
 输入参数  : mac_vap_stru          *pst_mac_vap     STA 本机信息
             hmac_join_req_stru    *pst_join_req    STA join 的AP bss 信息
 修改历史      :
  1.日    期   : 2013年8月21日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_update_current_join_req_parms_11i(mac_vap_stru *mac_vap, const mac_bss_80211i_info_stru *is_11i)
{
    hmac_vap_stru *hmac_vap = HI_NULL;
    hi_u8 grp_policy_match;  /* STA 和 AP 能力匹配的组播加密套件 */
    hi_u8 pcip_policy_match; /* STA 和 AP 能力匹配的单播加密套件 */
    hi_u8 auth_policy_match; /* STA 和 AP 能力匹配的认证模式 */
    hi_u8 is_80211i_mode;    /* STA 自己支持的安全模式 */
    hi_u16 ciphersize = 0;

    /* 根据匹配值来设置join 时候STA 的mib */
    grp_policy_match = is_11i->grp_policy_match;
    pcip_policy_match = is_11i->pairwise_policy_match;
    auth_policy_match = is_11i->auth_policy_match;
    is_80211i_mode = is_11i->bss_80211i_mode;

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_CFG, "{hmac_update_current_join_req_parms_11i::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_vap->mib_info->wlan_mib_privacy.dot11_privacy_invoked != HI_TRUE) {
        if (hmac_vap->protocol_fall == HI_TRUE) {
            hmac_update_pcip_policy_prot_supplicant(mac_vap, WLAN_80211_CIPHER_SUITE_NO_ENCRYP);
        }
        return HI_SUCCESS;
    }

    /* 使能RSN */
    if (mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated != HI_TRUE) {
        /* 在WEP / TKIP 加密模式下，不能工作在HT MODE */
        hmac_update_pcip_policy_prot_supplicant(mac_vap, WLAN_80211_CIPHER_SUITE_WEP_40);
        return HI_SUCCESS;
    }

    /* 在WEP / TKIP 加密模式下，不能工作在HT MODE */
    hmac_update_pcip_policy_prot_supplicant(mac_vap, pcip_policy_match);

    /* 配置STA 的MIB 信息 */
    /* 配置组播mib 值，组播信息来自于AP */
    if (mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher != grp_policy_match) {
        mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher = grp_policy_match;
        if (grp_policy_match == WLAN_80211_CIPHER_SUITE_CCMP) {
            ciphersize = WLAN_CCMP_KEY_LEN * 8; /* CCMP 加密方式的密钥长度(BITS) 8: 8位 */
        } else if (grp_policy_match == WLAN_80211_CIPHER_SUITE_TKIP) {
            ciphersize = WLAN_TKIP_KEY_LEN * 8; /* TKIP 加密方式下的密钥长度(BITS) 8: 8位 */
        }
        mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_config_group_cipher_size = ciphersize;
    }

    /* 更新MIB 值 */
    mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_pairwise_cipher_requested = pcip_policy_match;
    mac_vap->mib_info->wlan_mib_rsna_cfg.dot11_rsna_group_cipher_requested = grp_policy_match;

    /* STA 保存本机 80211i_mode 指定为WPA 或者 WPA2 */
    hmac_vap->is80211i_mode = is_80211i_mode;

    oam_info_log4(mac_vap->vap_id, OAM_SF_WPA,
        "{hmac_update_current_join_req_parms_11i::mode=%d group=%d pairwise=%d auth=%d.}", is_80211i_mode,
        grp_policy_match, pcip_policy_match, auth_policy_match);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 增加mic校验码
 输出参数  : iv头的长度
 修改历史      :
  1.日    期   : 2014年1月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_en_mic(const hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf, hi_u8 *puc_iv_len)
{
    wlan_priv_key_param_stru *key = HI_NULL;
    hi_u32 ret = HI_SUCCESS;
    wlan_ciper_protocol_type_enum_uint8 cipher_type;
    wlan_cipher_key_type_enum_uint8 key_type;

    *puc_iv_len = 0;
    key_type = hmac_user->base_user->user_tx_info.security.cipher_key_type;
    cipher_type = hmac_user->base_user->key_info.cipher_type;
    key = mac_user_get_key(hmac_user->base_user, key_type - 1);
    if (key == HI_NULL) {
        oam_error_log1(0, OAM_SF_WPA, "{hmac_en_mic::mac_user_get_key FAIL. en_key_type[%d]}", key_type);
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }

    switch (cipher_type) {
        case WLAN_80211_CIPHER_SUITE_TKIP:
            if (key_type == 0 || key_type > 5) { /* 5 边界 */
                return HI_ERR_CODE_SECURITY_KEY_TYPE;
            }

            ret = hmac_crypto_tkip_enmic(key, netbuf);
            if (ret != HI_SUCCESS) {
                oam_error_log1(0, OAM_SF_WPA, "{hmac_en_mic::hmac_crypto_tkip_enmic failed[%d].}", ret);
                return ret;
            }

            *puc_iv_len = WEP_IV_FIELD_SIZE + EXT_IV_FIELD_SIZE;
            break;
        case WLAN_80211_CIPHER_SUITE_CCMP:
            *puc_iv_len = WEP_IV_FIELD_SIZE + EXT_IV_FIELD_SIZE;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : mic码校验
 修改历史      :
  1.日    期   : 2014年1月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_de_mic(const hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf)
{
    wlan_priv_key_param_stru *key = HI_NULL;
    hi_u32 ret = HI_SUCCESS;
    wlan_ciper_protocol_type_enum_uint8 cipher_type;
    wlan_cipher_key_type_enum_uint8 key_type;

    key_type = hmac_user->base_user->user_tx_info.security.cipher_key_type;
    cipher_type = hmac_user->base_user->key_info.cipher_type;
    key = mac_user_get_key(hmac_user->base_user, key_type - 1);
    if (key == HI_NULL) {
        oam_error_log1(0, OAM_SF_WPA, "{hmac_de_mic::mac_user_get_key FAIL. en_key_type[%d]}", key_type);
        return HI_ERR_CODE_SECURITY_KEY_ID;
    }

    switch (cipher_type) {
        case WLAN_80211_CIPHER_SUITE_TKIP:
            if (key_type == 0 || key_type > 5) { /* 5 边界 */
                oam_error_log0(hmac_user->base_user->vap_id, OAM_SF_WPA,
                    "{hmac_de_mic::key_type is err code security key type.}");
                return HI_ERR_CODE_SECURITY_KEY_TYPE;
            }
            ret = hmac_crypto_tkip_demic(key, netbuf);
            if (ret != HI_SUCCESS) {
                oam_error_log1(hmac_user->base_user->vap_id, OAM_SF_WPA,
                    "{hmac_de_mic::hmac_crypto_tkip_demic failed[%d].}", ret);
                return ret;
            }
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : ap 和 sta 接收到MIC faile 事件处理
 输入参数  :
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_rx_tkip_mic_failure_process(frw_event_mem_stru *event_mem)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    frw_event_stru *event = HI_NULL;
    frw_event_mem_stru *hmac_event_mem = HI_NULL;
    frw_event_hdr_stru *event_hdr = HI_NULL;
    dmac_to_hmac_mic_event_stru *mic_event = HI_NULL;

    /* 获取事件头和事件结构体指针 */
    event = (frw_event_stru *)event_mem->puc_data;
    event_hdr = &(event->event_hdr);
    mic_event = (dmac_to_hmac_mic_event_stru *)(event->auc_event_data);

    /* 将mic事件抛到WAL */
    hmac_event_mem = frw_event_alloc(sizeof(dmac_to_hmac_mic_event_stru));
    if (hmac_event_mem == HI_NULL) {
        oam_error_log0(event_hdr->vap_id, OAM_SF_WPA, "{hmac_rx_tkip_mic_failure_process::pst_hmac_event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 填写事件 */
    event = (frw_event_stru *)hmac_event_mem->puc_data;

    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_HOST_CTX, HMAC_HOST_CTX_EVENT_SUB_TYPE_MIC_FAILURE,
        sizeof(dmac_to_hmac_mic_event_stru), FRW_EVENT_PIPELINE_STAGE_0, event_hdr->vap_id);

    /* 去关联的STA mac地址 */
    if (memcpy_s((hi_u8 *)frw_get_event_payload(event_mem), sizeof(dmac_to_hmac_mic_event_stru), (hi_u8 *)mic_event,
        sizeof(dmac_to_hmac_mic_event_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_rx_tkip_mic_failure_process::pst_mic_event memcpy_s fail.");
        frw_event_free(hmac_event_mem);
        return HI_FAIL;
    }

    /* 分发事件 */
    frw_event_dispatch_event(hmac_event_mem);
    frw_event_free(hmac_event_mem);
    return HI_SUCCESS;
#else
    hi_unref_param(event_mem);
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : 接收数据，安全相关过滤
 输入参数  : (1)vap
             (2)mac地址
             (3)接收数据类型
 返 回 值  : 成功或者失败
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_11i_ether_type_filter(const hmac_vap_stru *hmac_vap, const hi_u8 *mac_addr, hi_u16 us_ether_type)
{
    mac_user_stru *mac_user = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    hi_u32 ret = HI_SUCCESS;

    mac_vap = hmac_vap->base_vap;
    if (mac_vap->mib_info->wlan_mib_privacy.dot11_rsna_activated == HI_TRUE) { /* 判断是否使能WPA/WPA2 */
        mac_user = mac_vap_get_user_by_addr(hmac_vap->base_vap, mac_addr);
        if (mac_user == HI_NULL) {
            oam_info_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_11i_ether_type_filter:: user filterd.}");
            return HI_ERR_CODE_SECURITY_USER_INVAILD;
        }

        if (mac_user->port_valid != HI_TRUE) { /* 判断端口是否打开 */
            /* 接收数据时，针对非EAPOL 的数据帧做过滤 */
            if (hi_swap_byteorder_16(ETHER_TYPE_PAE) != us_ether_type) {
                oam_warning_log1(mac_vap->vap_id, OAM_SF_WPA,
                    "{hmac_11i_ether_type_filter::TYPE 0x%04x not permission.}", us_ether_type);
                ret = HI_ERR_CODE_SECURITY_PORT_INVALID;
            }
        } else if (hi_swap_byteorder_16(ETHER_TYPE_PAE) == us_ether_type) { /* EAPOL收发维测信息 */
            oam_info_log0(mac_vap->vap_id, OAM_SF_WPA, "{hmac_11i_ether_type_filter::rx EAPOL.}");
        }
    }
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
