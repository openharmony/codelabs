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
#include "frw_event.h"
#include "hmac_edca_opt.h"
#include "hmac_ext_if.h"
#include "hmac_vap.h"
#include "hcc_hmac_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  3 宏定义
**************************************************************************** */
#define HMAC_EDCA_OPT_ADJ_STEP 2

/* (3-a)/3*X + a/3*Y */
#define wlan_edca_opt_mod(x, y, a) \
    (((x) * (WLAN_EDCA_OPT_MAX_WEIGHT_STA - (a)) + (y) * (a)) / WLAN_EDCA_OPT_MAX_WEIGHT_STA);

/* ****************************************************************************
  4 内部静态函数声明
**************************************************************************** */
static hi_void hmac_edca_opt_stat_traffic_num(const hmac_vap_stru *hmac_vap,
    hi_u8 (*ppuc_traffic_num)[WLAN_TXRX_DATA_BUTT]);
/* ****************************************************************************
  5 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : edca调整统计上/下行，TCP/UDP流数目
 修改历史      :
  1.日    期   : 2014年12月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static hi_void hmac_edca_opt_stat_traffic_num(const hmac_vap_stru *hmac_vap,
    hi_u8 (*ppuc_traffic_num)[WLAN_TXRX_DATA_BUTT])
{
    mac_user_stru *user = HI_NULL;
    hmac_user_stru *hmac_user = HI_NULL;
    hi_u8 ac_idx;
    mac_vap_stru *mac_vap = hmac_vap->base_vap;
    hi_list *list_pos = HI_NULL;

    list_pos = mac_vap->mac_user_list_head.next;

    for (; list_pos != &(mac_vap->mac_user_list_head); list_pos = list_pos->next) {
        user = hi_list_entry(list_pos, mac_user_stru, user_dlist);
        hmac_user = (hmac_user_stru *)hmac_user_get_user_stru((hi_u8)user->us_assoc_id);
        if (hmac_user == HI_NULL) {
            continue;
        }

        for (ac_idx = 0; ac_idx < WLAN_WME_AC_BUTT; ac_idx++) {
            /* 遍历所有的数据类型 嵌套超4层整改删除for循环 */
            if (hmac_user->txrx_data_stat[ac_idx][WLAN_TX_TCP_DATA] > HMAC_EDCA_OPT_PKT_NUM) {
                ppuc_traffic_num[ac_idx][WLAN_TX_TCP_DATA]++;
            }
            if (hmac_user->txrx_data_stat[ac_idx][WLAN_RX_TCP_DATA] > HMAC_EDCA_OPT_PKT_NUM) {
                ppuc_traffic_num[ac_idx][WLAN_RX_TCP_DATA]++;
            }
            if (hmac_user->txrx_data_stat[ac_idx][WLAN_TX_UDP_DATA] > HMAC_EDCA_OPT_PKT_NUM) {
                ppuc_traffic_num[ac_idx][WLAN_TX_UDP_DATA]++;
            }
            if (hmac_user->txrx_data_stat[ac_idx][WLAN_RX_UDP_DATA] > HMAC_EDCA_OPT_PKT_NUM) {
                ppuc_traffic_num[ac_idx][WLAN_RX_UDP_DATA]++;
            }
            /* 统计完毕置0 */
            hmac_user->txrx_data_stat[ac_idx][WLAN_TX_TCP_DATA] = 0;
            hmac_user->txrx_data_stat[ac_idx][WLAN_RX_TCP_DATA] = 0;
            hmac_user->txrx_data_stat[ac_idx][WLAN_TX_UDP_DATA] = 0;
            hmac_user->txrx_data_stat[ac_idx][WLAN_RX_UDP_DATA] = 0;
        }
    }
}

/* ****************************************************************************
 功能描述  : edca调整定时器到期处理函数
 修改历史      :
  1.日    期   : 2014年12月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
  2.日    期   : 2015年5月5日
    作    者   : HiSilicon
    修改内容   : 增加抛事件，将参数调整下调到alg

**************************************************************************** */
hi_u32 hmac_edca_opt_timeout_fn(hi_void *arg)
{
    hi_u8        aast_uc_traffic_num[WLAN_WME_AC_BUTT][WLAN_TXRX_DATA_BUTT] = {{0}};
    hmac_vap_stru *hmac_vap       = HI_NULL;

    frw_event_mem_stru *event_mem = HI_NULL;
    frw_event_stru     *event = HI_NULL;

    hmac_vap = (hmac_vap_stru *)arg;

    /* 安全编程规则6.6例外(1)对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    memset_s(aast_uc_traffic_num, sizeof(aast_uc_traffic_num), 0, sizeof(aast_uc_traffic_num));

    /* 统计device下所有用户上/下行 TPC/UDP条数目 */
    hmac_edca_opt_stat_traffic_num(hmac_vap, aast_uc_traffic_num);

    /* **************************************************************************
        抛事件到dmac模块,将统计信息报给dmac
    ************************************************************************** */
    event_mem = frw_event_alloc(sizeof(aast_uc_traffic_num));
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANTI_INTF, "{hmac_edca_opt_timeout_fn::event_mem null.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    /* 填写事件头 */
    frw_event_hdr_init(&(event->event_hdr), FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPR_EDCA_OPT,
        sizeof(aast_uc_traffic_num), FRW_EVENT_PIPELINE_STAGE_1, hmac_vap->base_vap->vap_id);

    /* 拷贝参数 */
    /* event->auc_event_data, 可变数组 */
    if (memcpy_s(frw_get_event_payload(event_mem), sizeof(aast_uc_traffic_num), (hi_u8 *)aast_uc_traffic_num,
        sizeof(aast_uc_traffic_num)) != EOK) {
        frw_event_free(event_mem);
        oam_error_log0(0, OAM_SF_CFG, "hmac_edca_opt_timeout_fn:: aast_uc_traffic_num memcpy_s fail.");
        return HI_FAIL;
    }

    /* 分发事件 */
    hcc_hmac_tx_control_event(event_mem, sizeof(aast_uc_traffic_num));
    frw_event_free(event_mem);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : edca调整特性统计接收报文数量
 修改历史      :
  1.日    期   : 2014年12月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_edca_opt_rx_pkts_stat(hmac_user_stru *hmac_user, hi_u8 tidno, const mac_ip_header_stru *ip)
{
    /* 过滤IP_LEN 小于 HMAC_EDCA_OPT_MIN_PKT_LEN的报文 */
    if (oal_net2host_short(ip->us_tot_len) < HMAC_EDCA_OPT_MIN_PKT_LEN) {
        return;
    }

    if (ip->protocol == MAC_UDP_PROTOCAL) {
        hmac_user->txrx_data_stat[wlan_wme_tid_to_ac(tidno)][WLAN_RX_UDP_DATA]++;
    } else if (ip->protocol == MAC_TCP_PROTOCAL) {
        hmac_user->txrx_data_stat[wlan_wme_tid_to_ac(tidno)][WLAN_RX_TCP_DATA]++;
    }
}

/* ****************************************************************************
 功能描述  : edca调整特性统计发送报文数量
 修改历史      :
  1.日    期   : 2014年12月1日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_void hmac_edca_opt_tx_pkts_stat(const hmac_tx_ctl_stru *tx_ctl, hi_u8 tidno, const mac_ip_header_stru *ip)
{
    hmac_user_stru *hmac_user = HI_NULL;
    hmac_vap_stru  *hmac_vap = HI_NULL;

    hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(tx_ctl->us_tx_user_idx);
    if (oal_unlikely(hmac_user == HI_NULL)) {
        oam_error_log1(0, OAM_SF_CFG, "{hmac_edca_opt_rx_pkts_stat::hmac_user is null[%d].}", tx_ctl->us_tx_user_idx);
        return;
    }
    hmac_vap = hmac_vap_get_vap_stru(tx_ctl->tx_vap_index);
    if (hmac_vap == HI_NULL || hmac_vap->base_vap == HI_NULL) {
        oam_warning_log1(0, OAM_SF_TX, "{hmac_tx_classify_ipv4_data::get hmac_vap[%d] fail.}", tx_ctl->tx_vap_index);
        return;
    }
    /* AP模式且开启EDCA优化方进行维测统计 */
    if ((hmac_vap->edca_opt_flag_ap != HI_TRUE) || (hmac_vap->base_vap->vap_mode != WLAN_VAP_MODE_BSS_AP)) {
        return;
    }

    /* mips优化:解决开启业务统计性能差10M问题 */
    if (((ip->protocol == MAC_UDP_PROTOCAL) &&
        (hmac_user->txrx_data_stat[wlan_wme_tid_to_ac(tidno)][WLAN_TX_UDP_DATA] < (HMAC_EDCA_OPT_PKT_NUM + 10))) ||
        ((ip->protocol == MAC_TCP_PROTOCAL) &&                                    /* 10：偏移位 */
        (hmac_user->txrx_data_stat[wlan_wme_tid_to_ac(tidno)][WLAN_TX_TCP_DATA] < /* 10：偏移位 */
        (HMAC_EDCA_OPT_PKT_NUM + 10)))) {
        /* 过滤IP_LEN 小于 HMAC_EDCA_OPT_MIN_PKT_LEN的报文 */
        if (oal_net2host_short(ip->us_tot_len) < HMAC_EDCA_OPT_MIN_PKT_LEN) {
            return;
        }
        if (ip->protocol == MAC_UDP_PROTOCAL) {
            hmac_user->txrx_data_stat[wlan_wme_tid_to_ac(tidno)][WLAN_TX_UDP_DATA]++;
        } else if (ip->protocol == MAC_TCP_PROTOCAL) {
            hmac_user->txrx_data_stat[wlan_wme_tid_to_ac(tidno)][WLAN_TX_TCP_DATA]++;
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
