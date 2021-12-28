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
  1头文件包含
**************************************************************************** */

#include "hmac_traffic_classify.h"
#include "hmac_user.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2宏定义
**************************************************************************** */
#define RTP_VERSION                 2           /* RTP协议版本号，占2位，当前协议版本号为2 */
#define RTP_VER_SHIFT               6           /* RTP协议版本号位移量 */
#define RTP_CSRC_MASK               0x0f        /* CSRC计数器，占4位，指示CSRC标识符的个数 */
#define RTP_CSRC_LEN_BYTE           4           /* 每个CSRC标识符占32位，一共4字节 */
#define RTP_HDR_LEN_BYTE            12          /* RTP帧头固定字节数(不包含CSRC字段) */
#define TCP_HTTP_VI_LEN_THR         1000        /* HTTP视频流报文长度阈值 */
#define JUDGE_CACHE_LIFETIME        1           /* 待识别队列失效时间: 1s */
#define IP_FRAGMENT_MASK            0x1FFF      /* IP分片Fragment Offset字段 */
/* RTP Payload_Type 编号:RFC3551 */
#define RTP_PT_VO_G729              18          /* RTP载荷类型:18-Audio-G729 */
#define RTP_PT_VI_CELB              25          /* RTP载荷类型:25-Video-CelB */
#define RTP_PT_VI_JPEG              26          /* RTP载荷类型:26-Video-JPEG */
#define RTP_PT_VI_NV                28          /* RTP载荷类型:28-Video-nv */
#define RTP_PT_VI_H261              31          /* RTP载荷类型:31-Video-H261 */
#define RTP_PT_VI_MPV               32          /* RTP载荷类型:32-Video-MPV */
#define RTP_PT_VI_MP2T              33          /* RTP载荷类型:33-Video-MP2T */
#define RTP_PT_VI_H263              34          /* RTP载荷类型:34-Video-H263 */
/* HTTP流媒体端口 */
#define HTTP_PORT_80                80          /* HTTP协议默认端口号80 */
#define HTTP_PORT_8080              8080        /* HTTP协议默认端口号8080 */

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : hmac_tx_add_cfm_traffic
 功能描述  : 将用户已识别业务加入已识别记录表
 输入参数  : hmac用户结构体指针，TID指针，hmac_tx_major_flow_stru结构体指针
 输出参数  :
 返 回 值  : 成功返回HI_SUCCESS,失败返回HI_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日期: 2015.09.16
    修改内容: 生成新函数
**************************************************************************** */
static hi_u32 hmac_tx_add_cfm_traffic(hmac_user_stru *hmac_user, hi_u8 tid, const hmac_tx_major_flow_stru *max)
{
    hi_u32  time_stamp;
    hi_u8   mark         = 0;
    hi_u8   traffic_idx  = 0;

    hmac_tx_cfm_flow_stru *cfm_info = HI_NULL;

    if (hmac_user->cfm_num == MAX_CONFIRMED_FLOW_NUM) {
        /* 已识别列表已满，将列表中最长时间没有来包的业务进行替换 */
        time_stamp = hmac_user->ast_cfm_flow_list[traffic_idx].last_jiffies;

        for (traffic_idx = 1; traffic_idx < MAX_CONFIRMED_FLOW_NUM; traffic_idx++) {
            cfm_info = (hmac_user->ast_cfm_flow_list + traffic_idx);
            if (time_stamp > cfm_info->last_jiffies) {
                time_stamp = cfm_info->last_jiffies;
                mark = traffic_idx;
            }
        }
    } else {
        /* 已识别列表不满，找到可记录的index */
        for (traffic_idx = 0; traffic_idx < MAX_CONFIRMED_FLOW_NUM; traffic_idx++) {
            cfm_info = (hmac_user->ast_cfm_flow_list + traffic_idx);
            if (cfm_info->us_cfm_flag == HI_FALSE) {
                mark = traffic_idx;
                hmac_user->cfm_num++;
                cfm_info->us_cfm_flag = HI_TRUE;
                break;
            }
        }
    }

    /* 更新列表 */
    cfm_info = (hmac_user->ast_cfm_flow_list + mark);

    if (memcpy_s(&cfm_info->cfm_flow_info, sizeof(hmac_tx_flow_info_stru), &max->flow_info,
        sizeof(hmac_tx_flow_info_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_tx_add_cfm_traffic:: st_flow_info memcpy_s fail.");
        return HI_FALSE;
    }

    cfm_info->us_cfm_tid   = tid;
    cfm_info->last_jiffies = hi_get_tick();

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_traffic_judge
 功能描述  : 对主要业务进行业务识别处理
 输入参数  : hmac用户结构体指针，主要业务结构体指针，TID指针
 输出参数  : TID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日期: 2015.11.26
    修改内容: 生成新函数
  2.日期: 2015.12.26
    修改内容: TCP识别功能裁剪
**************************************************************************** */
static hi_u32 hmac_tx_traffic_judge(hmac_user_stru *hmac_user, const hmac_tx_major_flow_stru *major_flow, hi_u8 *puc_tid)
{
    hi_u32 ret = HI_FAIL;
    hi_u8  cache_idx;
    hi_u32 pt;

    hmac_tx_judge_list_stru *judge_list = &(hmac_user->judge_list);
    hmac_tx_judge_info_stru *judge_info = HI_NULL;

    /* 主要业务帧为UDP帧，进行RTP帧检测 */
    for (cache_idx = 0; cache_idx < MAX_JUDGE_CACHE_LENGTH; cache_idx++) {
        judge_info = (hmac_tx_judge_info_stru *)(judge_list->ast_judge_cache + cache_idx);

        if (!memcmp(&judge_info->flow_info, &major_flow->flow_info, sizeof(hmac_tx_flow_info_stru))) {
            /* RTP帧判断标准:version位保持为2，SSRC、PT保持不变，且帧长度大于RTP包头长度 */
            if (((judge_info->rtpver >> RTP_VER_SHIFT) != RTP_VERSION) ||
                (major_flow->rtpssrc      != judge_info->rtpssrc) ||
                (major_flow->payload_type != judge_info->payload_type) ||
                (major_flow->average_len <
                (hi_u32)(judge_info->rtpver & RTP_CSRC_MASK) * RTP_CSRC_LEN_BYTE + RTP_HDR_LEN_BYTE)) {
                hmac_user->judge_list.to_judge_num = 0; /* 识别失败，清空队列 */
                return HI_FAIL;
            }
        }
    }

    pt = (major_flow->payload_type & (~BIT7));
    if (pt <= RTP_PT_VO_G729) { /* 依据PayloadType判断RTP载荷类型 */
        *puc_tid = WLAN_TIDNO_VOICE;
    } else if ((pt == RTP_PT_VI_CELB) || (pt == RTP_PT_VI_JPEG) || (pt == RTP_PT_VI_NV) ||
        ((pt >= RTP_PT_VI_H261) && (pt <= RTP_PT_VI_H263))) {
        *puc_tid = WLAN_TIDNO_VIDEO;
    }

    /* 识别成功，更新用户已识别流列表 */
    if ((*puc_tid == WLAN_TIDNO_VOICE) || (*puc_tid == WLAN_TIDNO_VIDEO)) {
        ret = hmac_tx_add_cfm_traffic(hmac_user, *puc_tid, major_flow);
    }
    hmac_user->judge_list.to_judge_num = 0; /* 识别完成，清空队列 */

    return ret;
}

/* ****************************************************************************
 函 数 名  : hmac_tx_find_major_traffic
 功能描述  : 找到待识别队列中主要业务
 输入参数  : hmac用户结构体指针，TID指针
 输出参数  :
 返 回 值  : 成功返回HI_SUCCESS，失败返回HI_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日期: 2015.11.26
    修改内容: 生成新函数
**************************************************************************** */
static hi_u32 hmac_tx_find_major_traffic(hmac_user_stru *hmac_user, hi_u8 *puc_tid)
{
    hmac_tx_major_flow_stru mark = { 0 };
    hmac_tx_major_flow_stru max = { 0 };
    hmac_tx_judge_list_stru *judge_list = &(hmac_user->judge_list);
    hmac_tx_judge_info_stru *judge_info = HI_NULL;

    /* 队列超时，清空队列记录 */
    if (((hi_s32)judge_list->jiffies_end - (hi_s32)judge_list->jiffies_st) > (hi_s32)(JUDGE_CACHE_LIFETIME * HZ)) {
        /* 强制转换为long防止jiffies溢出 */
        hmac_user->judge_list.to_judge_num = 0; /* 清空队列 */
        return HI_FAIL;
    }

    /* 队列已满且未超时 */
    for (hi_u8 cache_idx_i = 0; cache_idx_i < (MAX_JUDGE_CACHE_LENGTH >> 1); cache_idx_i++) {
        judge_info = (hmac_tx_judge_info_stru *)(judge_list->ast_judge_cache + cache_idx_i);

        if (judge_info->flag == HI_FALSE) {
            continue;
        }

        judge_info->flag = HI_FALSE;
        if (memcpy_s(&mark, sizeof(hmac_tx_judge_info_stru), judge_info, sizeof(hmac_tx_judge_info_stru)) != EOK) {
            oam_error_log0(0, OAM_SF_CFG, "hmac_tx_find_major_traffic:: pst_judge_info memcpy_s fail.");
            continue;
        }
        mark.wait_check_num = 1;

        for (hi_u8 cache_idx_j = 0; cache_idx_j < MAX_JUDGE_CACHE_LENGTH; cache_idx_j++) {
            judge_info = (hmac_tx_judge_info_stru *)(judge_list->ast_judge_cache + cache_idx_j);

            if ((judge_info->flag == HI_TRUE) &&
                !memcmp(&judge_info->flow_info, &mark.flow_info, sizeof(hmac_tx_flow_info_stru))) {
                judge_info->flag    = HI_FALSE;
                mark.average_len    += judge_info->len;
                mark.wait_check_num += 1;
            }

            if (mark.wait_check_num <= max.wait_check_num) {
                continue;
            }
            if (memcpy_s(&max, sizeof(hmac_tx_major_flow_stru), &mark, sizeof(hmac_tx_major_flow_stru)) != EOK) {
                oam_error_log0(0, OAM_SF_CFG, "hmac_tx_find_major_traffic:: st_mark memcpy_s fail.");
                continue;
            }
            if (max.wait_check_num >= (MAX_JUDGE_CACHE_LENGTH >> 1)) {
                /* 已找到主要业务流，不必继续搜索 */
                max.average_len = max.average_len / max.wait_check_num;
                return hmac_tx_traffic_judge(hmac_user, &max, puc_tid);
            }
        }
    }

    if (max.wait_check_num < (MAX_JUDGE_CACHE_LENGTH >> 2)) { /* 2:右移2位 */
        /* 认为没有主要业务流 */
        hmac_user->judge_list.to_judge_num = 0; /* 清空队列 */
        return HI_FAIL;
    }

    max.average_len = max.average_len / max.wait_check_num;
    return hmac_tx_traffic_judge(hmac_user, &max, puc_tid);
}

hi_void hmac_tx_traffic_classify_list_proc(const mac_ip_header_stru *ip, hi_u8 *puc_tid,
    const hmac_tx_flow_info_stru *flow_info, hmac_user_stru *hmac_user, udp_hdr_stru *udp_hdr)
{
    /* 来包尚未识别，存入用户待识别队列 */
    hmac_tx_judge_list_stru *judge_list = &(hmac_user->judge_list);
    hmac_tx_judge_info_stru *judge_info = (hmac_tx_judge_info_stru *)(judge_list->ast_judge_cache +
        judge_list->to_judge_num);

    if (judge_list->to_judge_num >= MAX_JUDGE_CACHE_LENGTH) { /* 队列已满，识别过程中的来包不作记录 */
        return;
    }

    judge_list->jiffies_end = hi_get_tick();    /* 更新最新来包时间 */
    if (judge_list->to_judge_num == 0) {        /* 若队列为空 */
        judge_list->jiffies_st = hi_get_tick(); /* 更新队列产生时间 */
    }
    judge_list->to_judge_num += 1; /* 更新队列长度 */

    if (memset_s(judge_info, sizeof(hmac_tx_judge_info_stru), 0, sizeof(hmac_tx_judge_info_stru)) != EOK) {
        return;
    }
    if (memcpy_s(&(judge_info->flow_info), sizeof(hmac_tx_flow_info_stru), flow_info,
                 sizeof(hmac_tx_flow_info_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_tx_traffic_classify:: st_flow_info memcpy_s fail.");
        return;
    }

    hmac_tx_rtp_hdr *rtp_hdr = (hmac_tx_rtp_hdr *)(udp_hdr + 1);                 /* 偏移一个UDP头，取RTP头 */

    judge_info->flag         = HI_TRUE;
    judge_info->len          = oal_net2host_short(ip->us_tot_len) - sizeof(mac_ip_header_stru) - sizeof(udp_hdr_stru);
    judge_info->rtpver       = rtp_hdr->version_and_csrc;
    judge_info->payload_type = (hi_u32)(rtp_hdr->payload_type);

    /* 此处由于2字节对齐指针引用4字节对齐变量，在liteos上会崩溃，故用linux的宏括起来 */
    /* pst_rtp_hdr:为2字节对齐地址 */
    if (memcpy_s(&(judge_info->rtpssrc), sizeof(hi_u32), &(rtp_hdr->ssrc), sizeof(hi_u32)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "hmac_tx_traffic_classify:: ul_SSRC memcpy_s fail.");
        return;
    }

    /* 若待识别队列已满，尝试提取队列主要业务并进行业务识别 */ /* "<=":防止进程并发使此值大于待识别队列长度而踩内存 */
    if ((judge_list->to_judge_num >= MAX_JUDGE_CACHE_LENGTH) &&
        (hmac_tx_find_major_traffic(hmac_user, puc_tid) != HI_SUCCESS)) {
        oam_info_log0(0, OAM_SF_TX, "hmac_tx_traffic_classify::the classify process failed.");
    }
}

/* ****************************************************************************
 函 数 名  : hmac_tx_traffic_classify
 功能描述  : 对下行数据包进行处理:
                若业务已被识别，直接返回TID, 若否，提取包头信息并进入待检测队列
 输入参数  : netbuff CB字段指针，ip头指针，TID指针
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日期: 2015.11.26
    修改内容: 生成新函数
  2.日期: 2015.12.26
    修改内容: TCP业务识别功能裁剪
**************************************************************************** */
hi_void hmac_tx_traffic_classify(const hmac_tx_ctl_stru *tx_ctl, mac_ip_header_stru *ip, hi_u8 *puc_tid)
{
    hmac_tx_flow_info_stru  flow_info;
    hmac_user_stru         *hmac_user = (hmac_user_stru *)hmac_user_get_user_stru(tx_ctl->us_tx_user_idx);

    if (hmac_user == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_tx_traffic_classify::cannot find hmac_user_stru!");
        return;
    }

    /* 功能裁剪，只处理UDP报文，以及识别WifiDisplay RTSP业务为VI */
    if (ip->protocol != MAC_UDP_PROTOCAL) {
        if (ip->protocol == MAC_TCP_PROTOCAL) {
            mac_tcp_header_stru *tcp_hdr = (mac_tcp_header_stru *)(ip + 1);

            /* 识别WifiDisplay RTSP业务为VI */
            if (oal_ntoh_16(MAC_WFD_RTSP_PORT) == tcp_hdr->us_sport) {
                *puc_tid = WLAN_TIDNO_VIDEO;
                return;
            }
        }
        return;
    }

    /* 若为IP分片帧，没有端口号，直接返回 */
    if ((oal_ntoh_16(ip->us_frag_off) & IP_FRAGMENT_MASK) != 0) {
        return;
    }

    udp_hdr_stru *udp_hdr = (udp_hdr_stru *)(ip + 1); /* 偏移一个IP头，取UDP头 */

    /* 提取五元组 */
    flow_info.us_dport = udp_hdr->us_des_port;
    flow_info.us_sport = udp_hdr->us_src_port;
    flow_info.dip      = ip->daddr;
    flow_info.sip      = ip->saddr;
    flow_info.proto    = (hi_u32)(ip->protocol);

    /* 若来包属于用户已识别业务，直接返回TID */
    for (hi_u8 loop = 0; loop < hmac_user->cfm_num; loop++) {
        hmac_tx_cfm_flow_stru *cfm_info = (hmac_tx_cfm_flow_stru *)(hmac_user->ast_cfm_flow_list + loop);
        if (!memcmp(&cfm_info->cfm_flow_info, &flow_info, sizeof(hmac_tx_flow_info_stru))) {
            *puc_tid = (hi_u8)(cfm_info->us_cfm_tid);
            cfm_info->last_jiffies = hi_get_tick(); /* 更新业务最新来包时间 */
            return;
        }
    }

    hmac_tx_traffic_classify_list_proc(ip, puc_tid, &flow_info, hmac_user, udp_hdr);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
