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
#include "mac_regdomain.h"
#include "mac_device.h"
#include "hmac_mgmt_sta.h"
#include "hmac_sme_sta.h"
#include "hmac_fsm.h"
#include "hmac_chan_mgmt.h"
#include "hcc_hmac_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* 2.4G频段 信道与中心频率映射 */
const mac_freq_channel_map_stru g_ast_freq_map_2g[MAC_CHANNEL_FREQ_2_BUTT] = {
    {2412, 1, 0},
    {2417, 2, 1},
    {2422, 3, 2},
    {2427, 4, 3},
    {2432, 5, 4},
    {2437, 6, 5},
    {2442, 7, 6},
    {2447, 8, 7},
    {2452, 9, 8},
    {2457, 10, 9},
    {2462, 11, 10},
    {2467, 12, 11},
    {2472, 13, 12},
    {2484, 14, 13},
};

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
mac_freq_channel_map_stru get_ast_freq_map_2g_elem(hi_u32 index)
{
    return g_ast_freq_map_2g[index];
}

/* ****************************************************************************
 功能描述  : 设置VAP信道参数，准备切换至新信道运行
 输入参数  : pst_mac_vap : MAC VAP结构体指针
             uc_channel  : 新信道号(准备切换到的20MHz主信道号)
             en_bandwidth: 新带宽模式
 修改历史      :
  1.日    期   : 2014年2月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_chan_initiate_switch_to_new_channel(mac_vap_stru *mac_vap, hi_u8 channel,
    wlan_channel_bandwidth_enum_uint8 bandwidth)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hi_u32 ret;
    dmac_set_ch_switch_info_stru *ch_switch_info = HI_NULL;

    /* AP准备切换信道 */
    mac_vap->ch_switch_info.ch_switch_status = WLAN_CH_SWITCH_STATUS_1;
    mac_vap->ch_switch_info.announced_channel = channel;
    mac_vap->ch_switch_info.announced_bandwidth = bandwidth;

    /* 在Beacon帧中添加Channel Switch Announcement IE */
    mac_vap->ch_switch_info.csa_present_in_bcn = HI_TRUE;

    oam_info_log2(mac_vap->vap_id, OAM_SF_2040,
        "{hmac_chan_initiate_switch_to_new_channel::uc_announced_channel=%d,en_announced_bandwidth=%d}",
        channel, bandwidth);

    /* 申请事件内存 */
    event_mem = frw_event_alloc(sizeof(dmac_set_ch_switch_info_stru));
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_SCAN, "{hmac_chan_initiate_switch_to_new_channel::event_mem null.}");
        return;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_SWITCH_TO_NEW_CHAN,
        sizeof(dmac_set_ch_switch_info_stru), FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    /* 填写事件payload */
    ch_switch_info = (dmac_set_ch_switch_info_stru *)event->auc_event_data;
    ch_switch_info->ch_switch_status = WLAN_CH_SWITCH_STATUS_1;
    ch_switch_info->announced_channel = channel;
    ch_switch_info->announced_bandwidth = bandwidth;
    ch_switch_info->ch_switch_cnt = mac_vap->ch_switch_info.ch_switch_cnt;
    ch_switch_info->csa_present_in_bcn = HI_TRUE;

    /* 分发事件 */
    ret = hcc_hmac_tx_control_event(event_mem, sizeof(dmac_set_ch_switch_info_stru));
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_SCAN,
            "{hmac_chan_initiate_switch_to_new_channel::frw_event_dispatch_event failed[%d].}", ret);
        frw_event_free(event_mem);
        return;
    }

    /* 释放事件 */
    frw_event_free(event_mem);
}

/* ****************************************************************************
 功能描述  : 遍历device下所有ap，设置VAP信道参数，准备切换至新信道运行
 输入参数  : pst_mac_vap : MAC VAP结构体指针
             uc_channel  : 新信道号(准备切换到的20MHz主信道号)
             en_bandwidth: 新带宽模式
 修改历史      :
  1.日    期   : 2014年4月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_chan_multi_switch_to_new_channel(hi_u8 channel, wlan_channel_bandwidth_enum_uint8 bandwidth)
{
    hi_u8 vap_idx;
    mac_device_stru *mac_dev = HI_NULL;
    mac_vap_stru *ap = HI_NULL;

    oam_info_log2(0, OAM_SF_2040, "{hmac_chan_multi_switch_to_new_channel::uc_channel=%d,en_bandwidth=%d}",
        channel, bandwidth);

    mac_dev = mac_res_get_dev();
    if (mac_dev->vap_num == 0) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_chan_multi_switch_to_new_channel::none vap.}");
        return;
    }

    /* 遍历device下所有ap，设置ap信道参数，准备切换至新信道运行 */
    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        ap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (ap == HI_NULL) {
            oam_error_log1(0, OAM_SF_SCAN, "{hmac_chan_multi_switch_to_new_channel::pst_ap null.}",
                mac_dev->auc_vap_id[vap_idx]);
            continue;
        }

        /* 只更新AP侧的信道切换信息 */
        if (ap->vap_mode != WLAN_VAP_MODE_BSS_AP) {
            continue;
        }

        hmac_chan_initiate_switch_to_new_channel(ap, channel, bandwidth);
    }
}

hi_void hmac_chan_sync_init(const mac_vap_stru *mac_vap, dmac_set_chan_stru *set_chan)
{
    if (memset_s(set_chan, sizeof(dmac_set_chan_stru), 0, sizeof(dmac_set_chan_stru)) != EOK) {
        return;
    }
    if (memcpy_s(&set_chan->channel, sizeof(mac_channel_stru), &mac_vap->channel, sizeof(mac_channel_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_chan_sync_init::hmac_chan_sync_init memcpy_s fail.");
        return;
    }
    if (memcpy_s(&set_chan->ch_switch_info, sizeof(mac_ch_switch_info_stru), &mac_vap->ch_switch_info,
        sizeof(mac_ch_switch_info_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_chan_sync_init::hmac_chan_sync_init memcpy_s fail.");
        return;
    }
}

/* ****************************************************************************
 功能描述  : HMAC模块抛事件到DMAC模块，设置SW/MAC/PHY/RF中的信道和带宽，
             使VAP工作在新信道上
 修改历史      :
  1.日    期   : 2014年2月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_chan_do_sync(mac_vap_stru *mac_vap, dmac_set_chan_stru *set_chan)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hi_u32 ret;
    hi_u8 idx;

    if (mac_vap == HI_NULL || set_chan == HI_NULL) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_chan_do_sync::pst_mac_vap[%p] or pst_set_chan[%p] null!}",
            (uintptr_t)mac_vap, (uintptr_t)set_chan);
        return;
    }

    /* 更新VAP下的主20MHz信道号、带宽模式、信道索引 */
    ret = mac_get_channel_idx_from_num(mac_vap->channel.band, set_chan->channel.chan_number, &idx);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY,
                         "{hmac_chan_sync::mac_get_channel_idx_from_num failed[%d].}", ret);
        return;
    }

    mac_vap->channel.chan_number = set_chan->channel.chan_number;
    mac_vap->channel.en_bandwidth = set_chan->channel.en_bandwidth;
    mac_vap->channel.idx = idx;

    /* 申请事件内存 */
    event_mem = frw_event_alloc(sizeof(dmac_set_chan_stru));
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_SCAN, "{hmac_chan_sync::event_mem null.}");
        return;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WALN_CTX_EVENT_SUB_TYPR_SELECT_CHAN,
        sizeof(dmac_set_chan_stru), FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);
    /* event->auc_event_data, 可变数组 */
    if (memcpy_s(frw_get_event_payload(event_mem), sizeof(dmac_set_chan_stru), (hi_u8 *)set_chan,
        sizeof(dmac_set_chan_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_chan_do_sync::pst_set_chan memcpy_s fail.");
        frw_event_free(event_mem);
        return;
    }

    /* 分发事件 */
    ret = hcc_hmac_tx_control_event(event_mem, sizeof(dmac_set_chan_stru));
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN, "{hmac_chan_sync::frw_event_dispatch_event failed[%d].}", ret);
        frw_event_free(event_mem);
        return;
    }

    /* 释放事件 */
    frw_event_free(event_mem);
}

/* ****************************************************************************
 功能描述  : HMAC将信道/带宽信息同步到DMAC
 输入参数  : pst_mac_vap : MAC VAP结构体指针
             uc_channel  : 将要被设置的信道号
             en_bandwidth: 将要被设置的带宽模式
             en_switch_immediately: DMAC侧收到同步事件之后是否立即切换
 修改历史      :
  1.日    期   : 2014年2月26日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_chan_sync(mac_vap_stru *mac_vap, hi_u8 channel, wlan_channel_bandwidth_enum_uint8 bandwidth,
    hi_u8 switch_immediately)
{
    dmac_set_chan_stru set_chan;

    hmac_chan_sync_init(mac_vap, &set_chan);
    set_chan.channel.chan_number = channel;
    set_chan.channel.en_bandwidth = bandwidth;
    set_chan.switch_immediately = switch_immediately;
    hmac_chan_do_sync(mac_vap, &set_chan);
}

/* ****************************************************************************
 功能描述  : 遍历device下所有VAP，设置SW/MAC/PHY/RF中的信道和带宽，使VAP工作在新信道上
 输入参数  : pst_mac_vap : MAC VAP结构体指针
             uc_channel  : 将要被设置的信道号
             en_bandwidth: 将要被设置的带宽模式
 修改历史      :
  1.日    期   : 2014年4月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_chan_multi_select_channel_mac(mac_vap_stru *mac_vap, hi_u8 channel,
    wlan_channel_bandwidth_enum_uint8 bandwidth)
{
    hi_u8 vap_idx;
    mac_device_stru *mac_dev = HI_NULL;
    mac_vap_stru *mac_vap_value = HI_NULL;

    oam_warning_log2(mac_vap->vap_id, OAM_SF_2040,
        "{hmac_chan_multi_select_channel_mac::uc_channel=%d,en_bandwidth=%d}", channel, bandwidth);

    mac_dev = mac_res_get_dev();
    if (mac_dev->vap_num == 0) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_2040, "{hmac_chan_multi_select_channel_mac::none vap.}");
        return;
    }

    if (mac_is_dbac_running(mac_dev)) {
        hmac_chan_sync(mac_vap, channel, bandwidth, HI_TRUE);
        return;
    }

    /* 遍历device下所有vap， */
    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap_value = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (mac_vap_value == HI_NULL) {
            oam_error_log1(mac_vap->vap_id, OAM_SF_SCAN,
                "{hmac_chan_multi_select_channel_mac::mac_vap_value null,vap_id=%d.}", mac_dev->auc_vap_id[vap_idx]);
            continue;
        }

        hmac_chan_sync(mac_vap_value, channel, bandwidth, HI_TRUE);
    }
}

/* ****************************************************************************
 功能描述  : 控制硬件是否发送(数据帧、ACK、RTS)
 输入参数  : pst_mac_vap: MAC VAP结构体指针
             uc_sub_type: 事件子类型
 修改历史      :
  1.日    期   : 2014年7月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_chan_ctrl_machw_tx(const mac_vap_stru *mac_vap, hi_u8 sub_type)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hi_u32 ret;

    /* 申请事件内存 */
    event_mem = frw_event_alloc(0);
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_SCAN, "{hmac_chan_ctrl_machw_tx::event_mem null.}");
        return;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, sub_type, 0, FRW_EVENT_PIPELINE_STAGE_1,
        mac_vap->vap_id);

    /* 分发事件 */
    ret = hcc_hmac_tx_control_event(event_mem, 0);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN,
            "{hmac_chan_ctrl_machw_tx::frw_event_dispatch_event failed[%d].}", ret);
        frw_event_free(event_mem);
        return;
    }

    /* 释放事件 */
    frw_event_free(event_mem);
}

/* ****************************************************************************
 功能描述  : 禁止硬件发送(数据帧、ACK、RTS)
 输入参数  : pst_mac_vap: MAC VAP结构体指针
 修改历史      :
  1.日    期   : 2014年3月13日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_chan_disable_machw_tx(const mac_vap_stru *mac_vap)
{
    hmac_chan_ctrl_machw_tx(mac_vap, DMAC_WALN_CTX_EVENT_SUB_TYPR_DISABLE_TX);
}

/* ****************************************************************************
 功能描述  : 在指定(可用)信道上启动BSS
 输入参数  : pst_hmac_vap: HMAC VAP指针
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2014年10月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_start_bss_in_available_channel(hmac_vap_stru *hmac_vap)
{
    hi_u32 ret;

    /* 调用hmac_config_start_vap_event，启动BSS */
    ret = hmac_config_start_vap_event(hmac_vap->base_vap, HI_TRUE);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_INIT);
        oam_warning_log1(0, OAM_SF_SCAN,
                         "{hmac_start_bss_in_available_channel::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }

    /* 设置bssid */
    mac_vap_set_bssid(hmac_vap->base_vap, hmac_vap->base_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id,
        WLAN_MAC_ADDR_LEN);

    /* 入网优化，不同频段下的能力不一样 */
    if (hmac_vap->base_vap->channel.band == WLAN_BAND_2G) {
        mac_mib_set_short_preamble_option_implemented(hmac_vap->base_vap, WLAN_LEGACY_11B_MIB_SHORT_PREAMBLE);
        mac_mib_set_spectrum_management_required(hmac_vap->base_vap, HI_FALSE);
    } else {
        mac_mib_set_short_preamble_option_implemented(hmac_vap->base_vap, WLAN_LEGACY_11B_MIB_LONG_PREAMBLE);
        mac_mib_set_spectrum_management_required(hmac_vap->base_vap, HI_TRUE);
    }

    /* 设置AP侧状态机为 UP */
    hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_UP);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 挑选一条信道(对)，并启动BSS
 输入参数  : pst_mac_vap: MAC VAP结构体指针
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2014年6月3日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_chan_start_bss(hmac_vap_stru *hmac_vap)
{
    mac_device_stru *mac_dev = HI_NULL;
    mac_vap_stru *mac_vap = hmac_vap->base_vap;
    hi_u8 channel = 0;
    wlan_channel_bandwidth_enum_uint8 bandwidth = WLAN_BAND_WIDTH_BUTT;
    hi_u32 ret;

    /* 设置bssid */
    mac_vap_set_bssid(mac_vap, mac_vap->mib_info->wlan_mib_sta_config.auc_dot11_station_id, WLAN_MAC_ADDR_LEN);

    /* 初始化AP速率集 */
    mac_vap_init_rates(mac_vap);

    /* 获取mac device指针 */
    mac_dev = mac_res_get_dev();
    /* 挑选一条信道(对) */
    ret = mac_is_channel_num_valid(mac_vap->channel.band, channel);
    if (ret != HI_SUCCESS) {
        hmac_fsm_change_state(hmac_vap, MAC_VAP_STATE_INIT);
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN, "{hmac_chan_start_bss::mac_is_channel_num_valid failed[%d].}", ret);
        return ret;
    }

    oam_info_log2(mac_vap->vap_id, OAM_SF_SCAN,
        "{hmac_chan_start_bss::AP: Starting network in Channel: %d, bandwidth: %d.}", channel, bandwidth);

    /* 更新带宽模式 */
    mac_vap->channel.en_bandwidth = bandwidth;

    /* 设置信道号 */
#ifdef _PRE_WLAN_FEATURE_DBAC
    /* 同时更改多个VAP的信道，此时需要强制清除记录 */
    /* 若启动了DBAC，则按照原始流程进行 */
    if (!mac_dev->dbac_enabled) {
        mac_dev->max_channel = 0;
    }
#else
    mac_dev->max_channel = 0;
#endif

    ret = hmac_config_set_freq(mac_vap, sizeof(hi_u32), &channel);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN, "{hmac_chan_start_bss::hmac_config_set_freq failed[%d].}", ret);
        return ret;
    }

    /* 设置带宽模式，直接抛事件到DMAC配置寄存器 */
    ret = hmac_set_mode_event(mac_vap);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_CFG, "{hmac_chan_start_bss::hmac_config_send_event failed[%d].}", ret);
        return ret;
    }
    /* 否则，直接启动BSS */
    return hmac_start_bss_in_available_channel(hmac_vap);
}

/* ****************************************************************************
 功能描述  : 切换信道后重启BSS
 修改历史      :
  1.日    期   : 2014年11月7日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_chan_restart_network_after_switch(const mac_vap_stru *mac_vap)
{
    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru *event = HI_NULL;
    hi_u32 ret;

    /* 申请事件内存 */
    event_mem = frw_event_alloc(0);
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_SCAN, "{hmac_chan_restart_network_after_switch::event_mem null.}");

        return HI_ERR_CODE_ALLOC_MEM_FAIL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPR_RESTART_NETWORK, 0,
        FRW_EVENT_PIPELINE_STAGE_1, mac_vap->vap_id);

    /* 分发事件 */
    ret = hcc_hmac_tx_control_event(event_mem, 0);
    if (ret != HI_SUCCESS) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_SCAN,
            "{hmac_chan_restart_network_after_switch::frw_event_dispatch_event failed[%d].}", ret);
        frw_event_free(event_mem);

        return ret;
    }
    frw_event_free(event_mem);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理信道/带宽切换完成事件
 输入参数  :
 修改历史      :
  1.日    期   : 2014年5月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_chan_switch_to_new_chan_complete(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    hmac_vap_stru *hmac_vap = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    dmac_set_chan_stru *set_chan = HI_NULL;
    hi_u32 ret;
    hi_u8 idx;
    hi_s32 l_freq = 0;
    hi_unref_param(l_freq);

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_2040, "{hmac_switch_to_new_chan_complete::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    set_chan = (dmac_set_chan_stru *)event->auc_event_data;
    hmac_vap = hmac_vap_get_vap_stru(event->event_hdr.vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_2040, "{hmac_switch_to_new_chan_complete::pst_hmac_vap null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    mac_vap = hmac_vap->base_vap;
    ret = mac_get_channel_idx_from_num(mac_vap->channel.band, set_chan->channel.chan_number, &idx);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_2040,
            "{hmac_switch_to_new_chan_complete::mac_get_channel_idx_from_num failed[%d].}", ret);
        return HI_FAIL;
    }

    mac_vap->channel.chan_number = set_chan->channel.chan_number;
    mac_vap->channel.en_bandwidth = set_chan->channel.en_bandwidth;
    mac_vap->channel.idx = idx;

    mac_vap->ch_switch_info.waiting_to_shift_channel = set_chan->ch_switch_info.waiting_to_shift_channel;

    mac_vap->ch_switch_info.ch_switch_status = set_chan->ch_switch_info.ch_switch_status;
    mac_vap->ch_switch_info.bw_switch_status = set_chan->ch_switch_info.bw_switch_status;

    l_freq = oal_ieee80211_channel_to_frequency(mac_vap->channel.chan_number, mac_vap->channel.band);
    hi_unref_param(l_freq);

#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
    /* 将信道切换信息上报到wpa_supplicant */
    hmac_channel_switch_report_event(hmac_vap, l_freq);
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理dbac status event
 输入参数  :
 修改历史      :
  1.日    期   : 2014年5月8日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u32 hmac_dbac_status_notify(frw_event_mem_stru *event_mem)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    frw_event_stru  *event   = HI_NULL;
    mac_device_stru *mac_dev = HI_NULL;
    mac_vap_stru    *mac_vap = HI_NULL;
    hi_u8            vap_index = 0;

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_DBAC, "{hmac_dbac_status_notify::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;
    mac_dev = mac_res_get_dev();
    mac_dev->dbac_running = event->auc_event_data[0];
    mac_dev->dbac_same_ch = event->auc_event_data[1];
    oam_warning_log2(0, OAM_SF_DBAC, "{hmac_dbac_status_notify::sync dbac status, running[%d], same ch[%d].}",
        mac_dev->dbac_running, mac_dev->dbac_same_ch);
    /* 设置device下所有STA的KEEPALIVE */
    for (; vap_index < mac_dev->vap_num; vap_index++) {
        mac_vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_index]);
        if ((mac_vap != HI_NULL) && (is_sta(mac_vap))) {
            /* 开启时关闭 keepalive,关闭时开启 */
            mac_vap->cap_flag.keepalive = (mac_dev->dbac_running == HI_TRUE) ? HI_FALSE : HI_TRUE;
        }
    }
#else
    hi_unref_param(event_mem);
#endif
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 处理(Extended) Channel Switch Announcement IE
 输入参数  : pst_mac_vap: MAC VAP结构体指针
             puc_payload: 指向(Extended) Channel Switch Announcement IE的指针
             en_eid_type: Element ID
 返 回 值  : HI_SUCCESS或其它错误码
 修改历史      :
  1.日    期   : 2014年3月12日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年1月20日
    作    者   : HiSilicon
    修改内容   : 上移HMAC
**************************************************************************** */
hi_u32 hmac_ie_proc_ch_switch_ie(mac_vap_stru *mac_vap, const hi_u8 *puc_payload, mac_eid_enum_uint8 eid_type)
{
    if (oal_unlikely((mac_vap == HI_NULL) || (puc_payload == HI_NULL))) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ie_proc_ch_switch_ie::param null.}");

        return HI_ERR_CODE_PTR_NULL;
    }

    /* *********************************************************************** */
    /*                    Channel Switch Announcement element                */
    /* --------------------------------------------------------------------- */
    /* |Element ID|Length |Channel switch Mode|New Channel| Ch switch count| */
    /* --------------------------------------------------------------------- */
    /* |1         |1      |1                  |1          |1               | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /*                Extended Channel Switch Announcement element           */
    /* --------------------------------------------------------------------- */
    /* |Elem ID|Length|Ch Switch Mode|New Reg Class|New Ch| Ch switch count| */
    /* --------------------------------------------------------------------- */
    /* |1      |1     |1             |1            |1     |1               | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /* *********************************************************************** */
    /* Extended Channel Switch Announcement element */
    hi_u8 ch_sw_mode = puc_payload[MAC_IE_HDR_LEN];
    hi_u8 new_chan   = puc_payload[MAC_IE_HDR_LEN + 1];
    hi_u8 sw_cnt     = puc_payload[MAC_IE_HDR_LEN + 2]; /* 2:偏移2 */

    if (eid_type == MAC_EID_CHANSWITCHANN) {
        if (puc_payload[1] < MAC_CHANSWITCHANN_IE_LEN) {
            oam_warning_log1(0, 0, "{hmac_ie_proc_ch_switch_ie::invalid chan switch ann ie len[%d]}", puc_payload[1]);
            return HI_FAIL;
        }
    } else if (eid_type == MAC_EID_EXTCHANSWITCHANN) {
        if (puc_payload[1] < MAC_EXT_CHANSWITCHANN_IE_LEN) {
            oam_warning_log1(0, 0, "{hmac_ie_proc_ch_switch_ie::invalid ExtChan switch ann ie len=%d}", puc_payload[1]);
            return HI_FAIL;
        }

        /* Skip New Operating Class = puc_payload[MAC_IE_HDR_LEN + 1] */
        new_chan = puc_payload[MAC_IE_HDR_LEN + 2]; /* 2:偏移2 */
        sw_cnt = puc_payload[MAC_IE_HDR_LEN + 3];   /* 3:偏移3 */
    } else {
        return HI_FAIL;
    }

    /* 检查当前管制域是否支持该信道，如果不支持，则直接返回 */
    hi_u32 check = mac_is_channel_num_valid(mac_vap->channel.band, new_chan);
    if (check != HI_SUCCESS) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_ANY,
            "{hmac_ie_proc_ch_switch_ie::mac_is_channel_num_valid failed[%d], uc_new_chan=%d.}", check, new_chan);
        return check;
    }

    /* 如果STA已经准备进行信道切换，则不做任何事情，直接返回 */
    if (mac_vap->ch_switch_info.waiting_to_shift_channel == HI_TRUE) {
        if (sw_cnt < mac_vap->ch_switch_info.ch_swt_cnt) {
            return HI_SUCCESS;
        }
    } else if (ch_sw_mode == 1) { /* STA在信道切换完成前应该停止传输 */
        hmac_chan_disable_machw_tx(mac_vap);
    } /* 后无else */

    mac_vap->ch_switch_info.new_channel    = new_chan;
    mac_vap->ch_switch_info.new_ch_swt_cnt = sw_cnt;
    mac_vap->ch_switch_info.ch_swt_cnt     = sw_cnt;
    mac_vap->ch_switch_info.waiting_to_shift_channel = (hi_u8)HI_TRUE;

    /* 如果"信道切换计数"等于0，则立即切换信道 */
    if (mac_vap->ch_switch_info.new_ch_swt_cnt == 0) {
        mac_vap->ch_switch_info.channel_swt_cnt_zero = (hi_u8)HI_TRUE;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_chan_sync(mac_vap, mac_vap->channel.chan_number, mac_vap->channel.en_bandwidth, HI_FALSE);
#endif

    return HI_SUCCESS;
}

#if (_PRE_MULTI_CORE_MODE != _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC)
/* ****************************************************************************
 功能描述  : 将CSA切换信道的结果上报到WPA
**************************************************************************** */
hi_void hmac_channel_switch_report_event(const hmac_vap_stru *hmac_vap, hi_s32 l_freq)
{
    hmac_send_event_to_host(hmac_vap->base_vap, (const hi_u8 *)(&l_freq), sizeof(hi_s32),
        HMAC_HOST_CTX_EVENT_SUB_TYPE_CHANNEL_SWITCH);
    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
