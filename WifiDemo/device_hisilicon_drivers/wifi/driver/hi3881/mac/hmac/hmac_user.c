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
#include "frw_timer.h"
#include "dmac_ext_if.h"
#include "hmac_user.h"
#include "hmac_main.h"
#include "hmac_protection.h"
#include "hmac_ext_if.h"
#include "hmac_config.h"
#include "hmac_chan_mgmt.h"
#include "hmac_event.h"
#include "hcc_hmac_if.h"
#ifdef _PRE_WLAN_FEATURE_MESH
#include "wlan_types.h"
#include "hmac_vap.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* hmac user资源指针 根据配置的user数量在hmac main init时申请，exit时释放 */
hi_u8 *g_puc_hmac_user_res = HI_NULL;

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : hmac 用户资源池初始化
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_user_res_init(hi_void)
{
    hi_u8       index;
    hi_u8       user_num = mac_user_get_user_num();
    hi_u32      ret;
    /* 有逻辑保证size不可能为0或者溢出 此处不做判断 */
    hi_u32      user_size = sizeof(hmac_user_stru) * user_num;
    hmac_user_stru  *hmac_user = HI_NULL;
    mac_user_stru   *mac_user  = HI_NULL;

    ret = mac_user_res_init(user_num);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_user_res_init::mac_user_res_init failed.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }
    /* 不为空,重复调用初始化函数,不允许,失败 */
    if (g_puc_hmac_user_res != HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_user_res_init::re-mem alloc user res.}");
        return HI_FAIL;
    }
    /* 动态申请用户资源池相关内存 */
    g_puc_hmac_user_res = (hi_u8 *)oal_memalloc(user_size);
    if (g_puc_hmac_user_res == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_user_res_init::mem alloc user res null.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }
    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(g_puc_hmac_user_res, user_size, 0, user_size);
    /* 将mac user资源一一匹配到hmac user */
    for (index = 0; index < user_num; index++) {
        hmac_user = ((hmac_user_stru *)g_puc_hmac_user_res) + index;
        mac_user  = (mac_user_stru *)mac_user_init_get_user_stru(index);
        hmac_user->base_user = mac_user;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : hmac 用户资源池去初始化
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_user_res_exit(hi_void)
{
    mac_user_res_exit(); /* 先释放mac user资源 */
    if (g_puc_hmac_user_res != HI_NULL) {
        oal_free(g_puc_hmac_user_res);
        g_puc_hmac_user_res = HI_NULL;
    }
}

/* ****************************************************************************
 功能描述  : 获取对应HMAC USER索引的内存 由HMAC层强转为自己的内存解析
 输入参数  : 对应HMAC USER内存索引
 返 回 值  : 对应内存地址
 修改历史      :
  1.日    期   : 2019年5月30日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 *hmac_user_get_user_stru(hi_u8 idx)
{
    hi_u8 user_num = mac_user_get_user_num();
    if ((g_puc_hmac_user_res == HI_NULL) || (idx >= user_num)) {
        return HI_NULL;
    }
    return (g_puc_hmac_user_res + sizeof(hmac_user_stru) * idx);
}

/* ****************************************************************************
 功能描述  : 创建HMAC用户实体
 输出参数  : puc_user_idx-用户内存索引
 返 回 值  : 成功或失败原因
 修改历史      :
  1.日    期   : 2012年12月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_user_alloc(hi_u8 *puc_user_idx)
{
    mac_user_stru   *mac_user  = HI_NULL;
    hmac_user_stru  *hmac_user = HI_NULL;
    hi_u8            user_idx;

    /* 申请hmac user内存 */
    user_idx = mac_user_alloc_user_res();
    if (user_idx == MAC_INVALID_USER_ID) {
        oam_error_log0(0, OAM_SF_UM, "{hmac_user_alloc::alloc user id failed.}");
        return HI_FAIL;
    }
    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if (hmac_user == HI_NULL) {
        mac_user_free_user_res(user_idx);
        oam_error_log1(0, OAM_SF_UM, "{hmac_user_alloc::pst_hmac_user null,user_idx=%d.}", user_idx);
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 重复申请异常,避免影响业务，暂时打印error但正常申请 */
    if ((hmac_user->base_user) && (hmac_user->base_user->is_user_alloced == MAC_USER_ALLOCED)) {
        oam_warning_log1(0, OAM_SF_UM, "{hmac_user_get_user_stru::[E]user has been alloced,user_idx=%d.}", user_idx);
    }
    /* 保存mac user指针,保持对应关系 */
    mac_user = hmac_user->base_user;
    if (memset_s(hmac_user, sizeof(hmac_user_stru), 0, sizeof(hmac_user_stru) - sizeof(uintptr_t)) != EOK) {
        mac_user_free_user_res(user_idx);
        return HI_FAIL;
    }
    hmac_user->base_user = mac_user;
    /* 重新获取user指针并hmac与mac的一一匹配关系 匹配关系被修改返回失败 */
    mac_user = (mac_user_stru *)mac_user_init_get_user_stru(user_idx);
    if (hmac_user->base_user != mac_user) {
        mac_user_free_user_res(user_idx);
        oam_error_log1(0, OAM_SF_UM, "{hmac_user_alloc::idx mismatch, user_idx=%d.}", user_idx);
        return HI_FAIL;
    }
    /* 标记user资源已被alloc */
    mac_user->is_user_alloced = MAC_USER_ALLOCED;
    *puc_user_idx = user_idx;
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 释放对应HMAC USER的内存
 输入参数  : 对应HMAC USER内存索引
 修改历史      :
  1.日    期   : 2015年11月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_user_free(hi_u8 idx)
{
    hmac_user_stru *hmac_user = HI_NULL;

    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(idx);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_error_log1(0, OAM_SF_UM, "{hmac_user_free::user point null,user_idx=%d.}", idx);
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 重复释放异常, 继续释放不返回 */
    if (hmac_user->base_user->is_user_alloced == MAC_USER_FREED) {
        oam_warning_log1(0, OAM_SF_UM, "{hmac_user_free::[E]user has been freed,user_idx=%d.}", idx);
    }
    hmac_user->base_user->is_user_alloced = MAC_USER_FREED;
    mac_user_free_user_res(idx);
    oam_warning_log1(0, OAM_SF_ANY, "{hmac_user_free::user_idx=%d.}", idx);
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hmac_user_set_avail_num_space_stream
 功能描述  : 获取用户和VAP公共可用的空间流数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年10月16日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_user_set_avail_num_space_stream(mac_user_stru *mac_user, wlan_nss_enum_uint8 vap_nss)
{
    hi_u32                        ret = HI_SUCCESS;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    mac_vap_stru                 *mac_vap = HI_NULL;
    mac_user_nss_stru             user_nss;
#endif
    hi_unref_param(vap_nss);
    mac_user_set_num_spatial_stream(mac_user, WLAN_SINGLE_NSS);
    mac_user_set_avail_num_spatial_stream(mac_user, WLAN_SINGLE_NSS);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* 空间流信息同步dmac */
    mac_vap = mac_vap_get_vap_stru(mac_user->vap_id);
    if (mac_vap == HI_NULL) {
        oam_error_log1(mac_user->vap_id, OAM_SF_CFG, "hmac_user_set_avail_num_space_stream::mac vap(idx=%d) is null!",
            mac_user->vap_id);
        return ret;
    }
    user_nss.avail_num_spatial_stream = mac_user->avail_num_spatial_stream;
    user_nss.num_spatial_stream       = mac_user->num_spatial_stream;
    user_nss.user_idx = (hi_u8)mac_user->us_assoc_id;
    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_NSS, sizeof(mac_user_nss_stru), (hi_u8 *)(&user_nss));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_user->vap_id, OAM_SF_CFG,
            "{hmac_user_set_avail_num_space_stream::hmac_config_send_event failed[%d].}", ret);
    }
#endif
    return ret;
}

#ifdef _PRE_WLAN_FEATURE_PMF
/* ****************************************************************************
 函 数 名  : hmac_stop_sa_query_timer
 功能描述  : 删除sa query timer
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年2月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static hi_void hmac_stop_sa_query_timer(hmac_user_stru *hmac_user)
{
    frw_timeout_stru *sa_query_interval_timer = HI_NULL;

    sa_query_interval_timer = &(hmac_user->sa_query_info.sa_query_interval_timer);
    if (sa_query_interval_timer->is_registerd != HI_FALSE) {
        frw_timer_immediate_destroy_timer(sa_query_interval_timer);
    }

    /* 删除timers的入参存储空间 */
    if (sa_query_interval_timer->timeout_arg != HI_NULL) {
        oal_mem_free((hi_void *)sa_query_interval_timer->timeout_arg);
        sa_query_interval_timer->timeout_arg = HI_NULL;
    }
}
#endif

/* ****************************************************************************
 函 数 名  : hmac_user_get_wapi_ptr
 功能描述  : 获取用户的wapi对象指针
 输入参数  :
 输出参数  : hi_void
 返 回 值  : 无
 调用函数  : 无
 被调函数  : 无

 修改历史      :
  1.日    期   : 2015年5月29日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_WAPI
hmac_wapi_stru *hmac_user_get_wapi_ptr(const mac_vap_stru *mac_vap, hi_bool pairwise, hi_u8 pairwise_idx)
{
    hmac_user_stru             *hmac_user = HI_NULL;
    hi_u8                       user_index;

    if (pairwise == HI_TRUE) {
        user_index = pairwise_idx;
    } else {
        user_index = mac_vap->multi_user_idx;
    }

    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_index);
    if (hmac_user == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_user_get_wapi_ptr::pst_hmac_user null.}");
        return HI_NULL;
    }

    return &hmac_user->wapi;
}
#endif

hi_u32 hmac_user_del_wapi_sta_mesh_proc(hmac_user_stru *hmac_user, mac_vap_stru *mac_vap, const mac_user_stru *mac_user)
{
#ifdef _PRE_WLAN_FEATURE_WAPI
    mac_device_stru *mac_dev = mac_res_get_dev();

    if (hmac_wapi_deinit(&hmac_user->wapi) != HI_SUCCESS) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "hmac_wapi_deinit return NON SUCCESS. ");
    }

    /* STA模式下，清组播wapi加密端口 */
    hmac_user_stru *hmac_user_multi = (hmac_user_stru *)hmac_user_get_user_stru(mac_vap->multi_user_idx);
    if (hmac_user_multi == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{hmac_user_del::hmac_user_get_user_stru fail! user_idx[%u]}",
            mac_vap->multi_user_idx);
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_wapi_reset_port(&hmac_user_multi->wapi);
    mac_dev->wapi = HI_FALSE;
#else
    hi_unref_param(hmac_user);
    hi_unref_param(mac_user);
#endif

    if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
#ifdef _PRE_WLAN_FEATURE_STA_PM
        mac_vap_set_aid(mac_vap, 0);
#endif
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        if (mac_user->is_mesh_user == HI_TRUE) {
            /* 通知dmac从白名单中删除该用户 */
            if (hmac_del_multicast_user_whitelist(mac_vap, mac_user->user_mac_addr, WLAN_MAC_ADDR_LEN) != HI_SUCCESS) {
                return HI_FAIL;
            }
        }
    }
#endif

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : hmac抛事件删除dmac用户
 修改历史      :
  1.日    期   : 2015年8月6日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_send_del_user_event(const mac_vap_stru *mac_vap, const hi_u8 *da_mac_addr, hi_u8 user_idx)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    dmac_ctx_del_user_stru *del_user_payload = HI_NULL;

    event_mem = frw_event_alloc(sizeof(dmac_ctx_del_user_stru));
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_send_del_user_event::cannot alloc event,size[%d].}",
            sizeof(dmac_ctx_del_user_stru));
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    del_user_payload = (dmac_ctx_del_user_stru *)event->auc_event_data;
    del_user_payload->user_idx = user_idx;
    if (memcpy_s(del_user_payload->auc_user_mac_addr, WLAN_MAC_ADDR_LEN, da_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(0, OAM_SF_CFG, "hmac_send_del_user_event:: puc_da memcpy_s fail.");
        return HI_FAIL;
    }

    /* 填充事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_DEL_USER,
        sizeof(dmac_ctx_del_user_stru), FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    hcc_hmac_tx_control_event(event_mem, sizeof(dmac_ctx_del_user_stru));
    frw_event_free(event_mem);

    return HI_SUCCESS;
}

hi_u32 hmac_sync_del_user(mac_vap_stru *mac_vap, hmac_user_stru *hmac_user, const mac_user_stru *mac_user,
    hi_u8 user_index)
{
    mac_device_stru *mac_dev = mac_res_get_dev();

    if (hmac_send_del_user_event(mac_vap, mac_user->user_mac_addr, user_index) != HI_SUCCESS) {
        return HI_FAIL;
    }
    hmac_tid_clear(mac_vap, hmac_user);
    if (hmac_user->mgmt_timer.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&hmac_user->mgmt_timer);
    }
    if (hmac_user->ch_text != HI_NULL) {
        oal_mem_free(hmac_user->ch_text);
        hmac_user->ch_text = HI_NULL;
    }
    if (hmac_user->defrag_timer.is_registerd == HI_TRUE) {
        frw_timer_immediate_destroy_timer(&hmac_user->defrag_timer);
    }
    /* 从vap中删除用户 */
    mac_vap_del_user(mac_vap, user_index);

    /* 释放用户内存 */
    hi_u32 ret = hmac_user_free(user_index);
    if (ret == HI_SUCCESS) {
        if (mac_dev->asoc_user_cnt > 0) {
            /* device下已关联user个数-- */
            mac_dev->asoc_user_cnt--;
        }
    } else {
        oam_error_log1(0, OAM_SF_UM, "{hmac_user_del::mac_user_free_user_res fail[%d].}", ret);
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hmac_del_user
 功能描述  : 删除user
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_user_del(mac_vap_stru *mac_vap, hmac_user_stru *hmac_user)
{
    if ((mac_vap == HI_NULL) || (hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_error_log2(0, OAM_SF_UM, "{hmac_user_del::param null,%p %p.}", (uintptr_t)mac_vap, (uintptr_t)hmac_user);
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_user_stru *mac_user = hmac_user->base_user;
    oam_warning_log4(mac_vap->vap_id, OAM_SF_UM,
        "{hmac_user_del::del user[%d] start,is multi user[%d], user mac:XX:XX:XX:XX:%02X:%02X}", mac_user->us_assoc_id,
        mac_user->is_multi_user, mac_user->user_mac_addr[4], mac_user->user_mac_addr[5]); /* 4 5 元素索引 */

    /* 删除user时候，需要更新保护机制 */
    if (hmac_protection_del_user(mac_vap, mac_user) != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_UM, "{hmac_user_del::hmac_protection_del_user return Err}");
    }

    /* 获取用户对应的索引 */
    hi_u8 user_index = (hi_u8)hmac_user->base_user->us_assoc_id;

    /* 删除hmac user 的关联请求帧空间 */
    if (hmac_user->puc_assoc_req_ie_buff != HI_NULL) {
        oal_mem_free(hmac_user->puc_assoc_req_ie_buff);
        hmac_user->puc_assoc_req_ie_buff = HI_NULL;
        hmac_user->assoc_req_ie_len      = 0;
    }

#ifdef _PRE_WLAN_FEATURE_PMF
    hmac_stop_sa_query_timer(hmac_user);
#endif

    hi_u32 ret = hmac_user_del_wapi_sta_mesh_proc(hmac_user, mac_vap, mac_user);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* 抛事件到DMAC层, 删除dmac用户 */
    return hmac_sync_del_user(mac_vap, hmac_user, mac_user, user_index);
}

hi_u32 hmac_user_add_check(mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len)
{
    hi_u8 user_res_num = oal_mem_get_user_res_num();
    hi_u8 user_idx = 0;
    hi_u8 user_spec;

    if (oal_unlikely((mac_vap == HI_NULL) || (mac_addr == HI_NULL))) {
        hi_diag_log_msg_e2(0, "{hmac_user_add::param null, %p %p}", (uintptr_t)mac_vap, (uintptr_t)mac_addr);
        return HI_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        hi_diag_log_msg_e1(0, "{hmac_user_add:: hmac vap is null, id = %d.}", mac_vap->vap_id);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 判断用户数量是否超出规格 */
    mac_device_stru *mac_dev = mac_res_get_dev();
    /* _PRE_WLAN_FEATURE_MESH + */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        user_spec = (hi_u8)WLAN_MESHAP_ASSOC_USER_MAX_NUM;
    } else if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) {
        user_spec = WLAN_SOFTAP_ASSOC_USER_MAX_NUM;
#ifdef _PRE_WLAN_FEATURE_P2P
    } else if (is_p2p_cl(mac_vap)) {
        user_spec = 2; /* P2P作为CL时可以添加2个用户，一个用于发送管理帧，一个用户发送数据帧 */
#endif
    } else {
        user_spec = 1; /* STA 1个用户 */
    }
    if ((mac_dev->asoc_user_cnt >= user_res_num) || (mac_vap->user_nums >= user_spec)) {
        hi_diag_log_msg_w3(0, "{hmac_user_add_check::user cnt Err.asoc_user_cnt=%d, user_res_num=%d, user_nums=%d}",
            mac_dev->asoc_user_cnt, user_res_num, mac_vap->user_nums);
        return HI_ERR_CODE_CONFIG_EXCEED_SPEC;
    }

    /* 如果此用户已经创建，则返回失败 */
    hi_u32 ret = mac_vap_find_user_by_macaddr(mac_vap, mac_addr, mac_addr_len, &user_idx);
    if (ret == HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add::mac_vap_find_user_by_macaddr failed[%d].}", ret);
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

hi_u32 hmac_user_init_proc(const mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 *puc_user_index, hi_u8 *user_idx)
{
    /* 申请hmac用户内存，并初始清0 */
    hi_u32 ret = hmac_user_alloc(user_idx);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add::hmac_user_alloc failed[%d].}", ret);
        return ret;
    }

    /* 单播用户不能使用userid为0，需重新申请一个。将userid作为aid分配给对端，处理psm时会出错 */
    if ((*user_idx) == 0) {
        /* 重新申请一个新的userid, 此处不可能再申请到0 */
        ret = hmac_user_alloc(user_idx);
        if (hmac_user_free(0) != HI_SUCCESS) { /* 0不可作为单播用户的userid，将0还回用户资源池 先申请后释放 */
            oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "hmac_user_free return NON SUCCESS. ");
        }
        if (ret != HI_SUCCESS) {
            oam_warning_log2(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add::Err=%d, idx=%p}", ret, (uintptr_t)user_idx);
            return ret;
        }
    }

    *puc_user_index = *user_idx; /* 出参赋值 */
    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(*user_idx);
    if ((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL)) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add::pst_hmac_user null.}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 清空hmac user结构体 不清base_user保持与mac user的匹配关系 */
    if (memset_s(((hi_u8 *)hmac_user), hi_offset_of_member(hmac_user_stru, base_user), 0,
        hi_offset_of_member(hmac_user_stru, base_user)) != EOK) {
        oam_warning_log0(0, 0, "hmac_user_init_proc::memset_s 0 fail!");
        return HI_FAIL;
    }

    /* 初始化mac_user_stru */
    mac_user_init(hmac_user->base_user, (*user_idx), mac_addr, mac_vap->vap_id);

    /* mesh不支持wpai */
    if (mac_vap->vap_mode != WLAN_VAP_MODE_MESH) {
#ifdef _PRE_WLAN_FEATURE_WAPI
        mac_device_stru *mac_dev = mac_res_get_dev();
        /* 初始化单播wapi对象 */
        hmac_wapi_init(&hmac_user->wapi, HI_TRUE);
        mac_dev->wapi = HI_FALSE;
#endif
    }
    /* 设置amsdu域 */
    hmac_user->us_amsdu_maxsize = WLAN_AMSDU_FRAME_MAX_LEN_LONG;
    hmac_user->amsdu_supported  = AMSDU_ENABLE_ALL_TID;

    return HI_SUCCESS;
}

hi_u32 hmac_user_add_send_event(const mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len, hi_u8 user_idx)
{
    frw_event_mem_stru *event_mem = frw_event_alloc(sizeof(dmac_ctx_add_user_stru));

    if (oal_unlikely(event_mem == HI_NULL) || (event_mem->puc_data == HI_NULL)) {
        /* 异常处理，释放内存，device下关联用户数还没有++，这里不需要判断返回值做--操作 */
        if (hmac_user_free(user_idx) != HI_SUCCESS) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "hmac_user_free return NON SUCCESS. ");
        }
        oam_error_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add::event_mem null.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    frw_event_stru         *event            = (frw_event_stru *)event_mem->puc_data;
    dmac_ctx_add_user_stru *add_user_payload = (dmac_ctx_add_user_stru *)event->auc_event_data;
    add_user_payload->user_idx = user_idx;
    if (memcpy_s(add_user_payload->auc_user_mac_addr, WLAN_MAC_ADDR_LEN, mac_addr, mac_addr_len) != EOK) {
        frw_event_free(event_mem);
        return HI_FAIL;
    }

    /* 填充事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_ADD_USER,
        sizeof(dmac_ctx_add_user_stru), FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    hi_u32 ret = hcc_hmac_tx_control_event(event_mem, sizeof(dmac_ctx_add_user_stru));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        /* 异常处理，释放内存，device下关联用户数还没有++，这里不需要判断返回值做--操作 */
        if (hmac_user_free(user_idx) != HI_SUCCESS) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "hmac_user_free return NON SUCCESS. ");
        }
        frw_event_free(event_mem);

        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add::frw_event_dispatch_event failed[%d].}", ret);
        return ret;
    }

    frw_event_free(event_mem);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 添加用户配置命令
 修改历史      :
  1.日    期   : 2013年6月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_user_add(mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len, hi_u8 *puc_user_index)
{
    hi_u8                   user_idx;
    mac_device_stru        *mac_dev  = mac_res_get_dev();
    hi_u8                   tid_loop;

    hi_u32 ret = hmac_user_add_check(mac_vap, mac_addr, mac_addr_len);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hmac_user_init_proc(mac_vap, mac_addr, puc_user_index, &user_idx);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    hmac_user_stru *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if (hmac_user == HI_NULL) {
        return HI_FAIL;
    }

    /* 抛事件到DMAC层, 创建dmac用户 */
    ret = hmac_user_add_send_event(mac_vap, mac_addr, mac_addr_len, user_idx);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* 添加用户到MAC VAP */
    ret = mac_vap_add_assoc_user(mac_vap, user_idx);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add::mac_vap_add_assoc_user failed[%d].}", ret);

        /* 异常处理，释放内存，device下关联用户数还没有++，这里不需要判断返回值做--操作 */
        if (hmac_user_free(user_idx) != HI_SUCCESS) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "hmac_user_free return NON SUCCESS. ");
        }
        return HI_FAIL;
    }

    /* 初始化tid信息 统一由申请时清0,此处仅进行非零初始化 */
    for (tid_loop = 0; tid_loop < WLAN_TID_MAX_NUM; tid_loop++) {
        hmac_user->ast_tid_info[tid_loop].tid_no = (hi_u8)tid_loop;
    }
    mac_dev->asoc_user_cnt++;
    oam_warning_log4(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add::user[%d] mac:XX:XX:XX:%02X:%02X:%02X}", user_idx,
        mac_addr[3], mac_addr[4], mac_addr[5]); /* 3 4 5 元素索引 */

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : hmac层创建组播用户
 修改历史      :
  1.日    期   : 2013年8月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_user_add_multi_user(const mac_vap_stru *mac_vap, hi_u8 *puc_user_index)
{
    hi_u32      ret;
    hi_u8       user_index;
    mac_user_stru  *mac_user  = HI_NULL;
#ifdef _PRE_WLAN_FEATURE_WAPI
    hmac_user_stru *hmac_user = HI_NULL;
#endif

    ret = hmac_user_alloc(&user_index);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add_multi_user::hmac_user_alloc failed[%d].}", ret);
        return ret;
    }

    /* 初始化组播用户基本信息 */
    mac_user = mac_user_get_user_stru(user_index);
    if (mac_user == HI_NULL) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_user_add_multi_user::pst_mac_user null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_user_init(mac_user, user_index, HI_NULL, mac_vap->vap_id);
    *puc_user_index = user_index;

#ifdef _PRE_WLAN_FEATURE_WAPI
    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_index);
    if (hmac_user == HI_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_user_add_multi_user::get hmac_user fail.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 初始化wapi对象 */
    hmac_wapi_init(&hmac_user->wapi, HI_FALSE);
#endif

    oam_info_log1(mac_vap->vap_id, OAM_SF_ANY, "{hmac_user_add_multi_user:: user index[%d].}", user_index);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hmac_user_del_multi_user
 功能描述  : hmac层删除multiuser
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年7月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_user_del_multi_user(hi_u8 idx)
{
#ifdef _PRE_WLAN_FEATURE_WAPI
    hmac_user_stru *hmac_user = HI_NULL;
#endif
    hi_u32 ret;

#ifdef _PRE_WLAN_FEATURE_WAPI
    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(idx);
    if (hmac_user == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_user_add_multi_user::get hmac_user fail.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    ret = hmac_wapi_deinit(&hmac_user->wapi);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_wapi_deinit return NON SUCCESS. ");
    }
#endif

    ret = hmac_user_free(idx);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_user_free return NON SUCCESS. ");
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 判断wapi设备是否关连
 修改历史      :
  1.日    期   : 2015年12月23日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_WAPI
hi_u8 hmac_user_is_wapi_connected(hi_void)
{
    hi_u8                  vap_idx;
    hmac_user_stru         *hmac_user = HI_NULL;
    mac_device_stru        *mac_dev      = HI_NULL;
    mac_vap_stru           *mac_vap         = HI_NULL;

    mac_dev = mac_res_get_dev();
    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (oal_unlikely(mac_vap == HI_NULL)) {
            oam_warning_log1(0, OAM_SF_CFG, "vap is null! vap id is %d", mac_dev->auc_vap_id[vap_idx]);
            continue;
        }

        if (!is_sta(mac_vap)) {
            continue;
        }

        hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(mac_vap->multi_user_idx);
        if ((hmac_user != HI_NULL) && (hmac_user->wapi.port_valid == HI_TRUE)) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}
#endif /* #ifdef _PRE_WLAN_FEATURE_WAPI */

/* ****************************************************************************
 函 数 名  : hmac_user_add_notify_alg
 功能描述  : 抛事件给dmac，让其在dmac挂算法钩子
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年11月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_u32 hmac_user_add_notify_alg(const mac_vap_stru *mac_vap, hi_u8 user_idx)
{
    frw_event_mem_stru             *event_mem        = HI_NULL;
    frw_event_stru                 *event            = HI_NULL;
    dmac_ctx_add_user_stru         *add_user_payload = HI_NULL;
    hi_u32                         ret;
    hmac_user_stru                 *hmac_user        = HI_NULL;

    /* 抛事件给Dmac，在dmac层挂用户算法钩子 */
    event_mem = frw_event_alloc(sizeof(dmac_ctx_add_user_stru));
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{hmac_user_add_notify_alg::event_mem null.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    add_user_payload = (dmac_ctx_add_user_stru *)event->auc_event_data;
    add_user_payload->user_idx = user_idx;
    add_user_payload->us_sta_aid = mac_vap->us_sta_aid;
    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if (oal_unlikely((hmac_user == HI_NULL) || (hmac_user->base_user == HI_NULL))) {
        oam_error_log1(0, OAM_SF_CFG, "{hmac_user_add_notify_alg::null param,pst_hmac_user[%d].}", user_idx);
        frw_event_free(event_mem);
        return HI_ERR_CODE_PTR_NULL;
    }
    mac_user_get_ht_hdl(hmac_user->base_user, &add_user_payload->ht_hdl);
    /* 填充事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_NOTIFY_ALG_ADD_USER,
        sizeof(dmac_ctx_add_user_stru), FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    ret = hcc_hmac_tx_control_event(event_mem, sizeof(dmac_ctx_add_user_stru));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        /* 异常处理，释放内存 */
        frw_event_free(event_mem);

        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY,
            "{hmac_user_add_notify_alg::frw_event_dispatch_event failed[%d].}", ret);
        return ret;
    }
    frw_event_free(event_mem);

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : mac_vap_get_hmac_user_by_addr
 功能描述  : 根据mac地址获取mac_user指针
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hmac_user_stru *mac_vap_get_hmac_user_by_addr(mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 addr_len)
{
    hi_u32              ret;
    hi_u8               user_idx   = 0xff;
    hmac_user_stru         *hmac_user = HI_NULL;

    /* 根据mac addr找sta索引 */
    ret = mac_vap_find_user_by_macaddr(mac_vap, mac_addr, addr_len, &user_idx);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{mac_vap_get_hmac_user_by_addr::find_user_by_macaddr failed[%d].}", ret);
        if (mac_addr != HI_NULL) {
            oam_warning_log3(0, OAM_SF_ANY, "{mac_vap_get_hmac_user_by_addr:: mac_addr[XX:XX:XX:XX:%02x:%02x:%02x]!.}",
                mac_addr[3], mac_addr[4], mac_addr[5]); /* 3 4 5 元素索引 */
        }
        return HI_NULL;
    }

    /* 根据sta索引找到user内存区域 */
    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(user_idx);
    if (hmac_user == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{mac_vap_get_hmac_user_by_addr::user ptr null.}");
    }
    return hmac_user;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 功能描述  : 配置Mesh用户的Mac地址到白名单中，接收广播/组播数据帧
 输入参数  :  mac_vap_stru *pst_mac_vap, hi_u8 *puc_mac_addr
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2019年7月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_set_multicast_user_whitelist(const mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    dmac_ctx_mesh_mac_addr_whitelist_stru *mesh_wl = HI_NULL;
    hi_u32 ret;

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, 0, "{hmac_set_multicast_user_whitelist::pst_hmac_vap null!}");
        return HI_FAIL;
    }
    /* **************************************************************************
        抛事件到DMAC层, 添加白名单
    ************************************************************************** */
    event_mem = frw_event_alloc(sizeof(dmac_ctx_mesh_mac_addr_whitelist_stru));
    if (oal_unlikely(event_mem == HI_NULL)) {
        /* 异常处理，释放内存，无法添加到硬件，该远端无法正常通信 */
        hmac_handle_close_peer_mesh(hmac_vap, mac_addr, mac_addr_len, HMAC_REPORT_DISASSOC, DMAC_DISASOC_MISC_KICKUSER);
        oam_error_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_set_multicast_user_whitelist::event_mem null.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    mesh_wl = (dmac_ctx_mesh_mac_addr_whitelist_stru *)event->auc_event_data;
    mesh_wl->set = HI_TRUE;
    if (memcpy_s(mesh_wl->auc_addr, WLAN_MAC_ADDR_LEN, mac_addr, mac_addr_len) != EOK) {
        frw_event_free(event_mem);
        return HI_FAIL;
    }

    /* 填充事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX,
        DMAC_WLAN_CTX_EVENT_SUB_TYPE_SET_MESH_USER_WHITELIST, sizeof(dmac_ctx_mesh_mac_addr_whitelist_stru),
        FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    ret = hcc_hmac_tx_control_event(event_mem, sizeof(dmac_ctx_mesh_mac_addr_whitelist_stru));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        /* 异常处理，释放内存，无法添加到硬件，该远端无法正常通信 */
        hmac_handle_close_peer_mesh(hmac_vap, mac_addr, mac_addr_len, HMAC_REPORT_DISASSOC, DMAC_DISASOC_MISC_KICKUSER);
        frw_event_free(event_mem);
        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_set_multicast_user_whitelist::frw_event_dispatch_event failed[%d].}", ret);
        return ret;
    }

    frw_event_free(event_mem);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 删除白名单中的某个Mesh用户的Mac地址
 输入参数  :  mac_vap_stru *pst_mac_vap, hi_u8 *puc_mac_addr
 返 回 值  : HI_SUCCESS 或 失败错误码
 修改历史      :
  1.日    期   : 2019年7月4日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_del_multicast_user_whitelist(const mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    dmac_ctx_mesh_mac_addr_whitelist_stru *mesh_wl = HI_NULL;
    hi_u32 ret;

    /* **************************************************************************
        抛事件到DMAC层, 删除白名单
    ************************************************************************** */
    event_mem = frw_event_alloc(sizeof(dmac_ctx_mesh_mac_addr_whitelist_stru));
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_unset_multicast_user_whitelist::event_mem null.}");
        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    mesh_wl = (dmac_ctx_mesh_mac_addr_whitelist_stru *)event->auc_event_data;
    mesh_wl->set = HI_FALSE;
    if (memcpy_s(mesh_wl->auc_addr, WLAN_MAC_ADDR_LEN, mac_addr, mac_addr_len) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(mac_vap->vap_id, OAM_SF_UM, "{hmac_unset_multicast_user_whitelist::memcpy_s Err.}");
        return HI_FAIL;
    }

    /* 填充事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX,
        DMAC_WLAN_CTX_EVENT_SUB_TYPE_UNSET_MESH_USER_WHITELIST, sizeof(dmac_ctx_mesh_mac_addr_whitelist_stru),
        FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    ret = hcc_hmac_tx_control_event(event_mem, sizeof(dmac_ctx_mesh_mac_addr_whitelist_stru));
    if (oal_unlikely(ret != HI_SUCCESS)) {
        frw_event_free(event_mem);

        oam_warning_log1(mac_vap->vap_id, OAM_SF_UM,
            "{hmac_unset_multicast_user_whitelist::frw_event_dispatch_event failed[%d].}", ret);
        return ret;
    }

    frw_event_free(event_mem);
    return HI_SUCCESS;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
