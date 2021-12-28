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

#ifndef __HMAC_USER_H__
#define __HMAC_USER_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "mac_user.h"
#include "mac_resource.h"
#include "dmac_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define HMAC_ADDBA_EXCHANGE_ATTEMPTS 10 /* 试图建立ADDBA会话的最大允许次数 */

#define hmac_user_is_amsdu_support(_user, _tid)         (((_user)->amsdu_supported) & (0x01 << ((_tid) & 0x07)))
#define hmac_user_set_amsdu_support(_user, _tid)        (((_user)->amsdu_supported) |= (0x01 << ((_tid) & 0x07)))
#define hmac_user_set_amsdu_not_support(_user, _tid)    (((_user)->amsdu_supported) &= \
    (hi_u8)(~(0x01 << ((_tid) & 0x07))))

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
#define MAX_JUDGE_CACHE_LENGTH 20 /* 业务识别-用户待识别队列长度 */
#define MAX_CONFIRMED_FLOW_NUM 2  /* 业务识别-用户已识别业务总数 */
#endif

#define hmac_user_stats_pkt_incr(_member, _cnt) ((_member) += (_cnt))

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
typedef enum {
    WLAN_TX_TCP_DATA = 0, /* 发送TCP data */
    WLAN_RX_TCP_DATA = 1, /* 接收TCP data */
    WLAN_TX_UDP_DATA = 2, /* 发送UDP data */
    WLAN_RX_UDP_DATA = 3, /* 接收UDP data */

    WLAN_TXRX_DATA_BUTT = 4,
} wlan_txrx_data_type_enum;
typedef hi_u8 wlan_txrx_data_enum_uint8;
/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
typedef struct {
    oal_netbuf_head_stru            msdu_head;         /* msdu链表头 */
    frw_timeout_stru                amsdu_timer;
    oal_spin_lock_stru              st_amsdu_lock;        /* amsdu task lock */
    hi_u8                       last_pad_len;      /* 最后一个msdu的pad长度 */
    hi_u8                       msdu_num     : 4;  /* Number of sub-MSDUs accumulated */
    hi_u8                       amsdu_maxnum : 4;  /* 最大聚合个数 value = 12个 */
    hi_u16                      us_amsdu_size;        /* Present size of the AMSDU */

    hi_u8                       auc_eth_da[WLAN_MAC_ADDR_LEN];
    hi_u8                       auc_eth_sa[WLAN_MAC_ADDR_LEN];
} hmac_amsdu_stru;
/* TID对应的发送BA会话的状态 */
typedef struct {
    dmac_ba_conn_status_enum_uint8  ba_status;       /* 该TID对应的BA会话的状态 */
    hi_u8                       dialog_token;    /* 随机标记数 */
    hi_u8                       uc_resv[2]; /* 2 保留字节 */
    frw_timeout_stru                addba_timer;
    dmac_ba_alarm_stru              alarm_data;
} hmac_ba_tx_stru;

typedef struct {
    hi_u8 in_use;                     /* 缓存BUF是否被使用 */
    hi_u8 num_buf;                    /* MPDU占用的netbuf(接收描述符)个数 */
    hi_u16 us_seq_num;                /* MPDU对应的序列号 */
    oal_netbuf_head_stru netbuf_head; /* MPDU对应的描述符首地址 */
    hi_u32 rx_time;                   /* 报文被缓存的时间戳 */
} hmac_rx_buf_stru;

typedef struct {
    hi_void                             *ba;
    hi_u8                               tid;
    hi_u8                               vap_id;
    hi_u16                              us_timeout_times;
} hmac_ba_alarm_stru;

/* Hmac侧接收侧BA会话句柄 */
typedef struct {
    hi_u16 us_baw_start; /* 第一个未收到的MPDU的序列号 */
    hi_u16 us_baw_end;   /* 最后一个可以接收的MPDU的序列号 */
    hi_u16 us_baw_tail;  /* 目前Re-Order队列中，最大的序列号 */
    hi_u16 us_baw_size;  /* Block_Ack会话的buffer size大小 */

    oal_spin_lock_stru st_ba_lock; /* 02用于hcc线程和事件线程并发 */

    hi_u8 timer_triggered;                    /* 上一次上报是否为定时器上报 */
    hi_u8 is_ba;                              /* Session Valid Flag */
    dmac_ba_conn_status_enum_uint8 ba_status; /* 该TID对应的BA会话的状态 */
    hi_u8 mpdu_cnt;                           /* 当前Re-Order队列中，MPDU的数目 */

    hmac_rx_buf_stru                ast_re_order_list[WLAN_AMPDU_RX_BUFFER_SIZE];  /* Re-Order队列 */
    hmac_ba_alarm_stru              alarm_data;
    frw_timeout_stru                ba_timer;                /* 接收重排序缓冲超时 */

    /* 以下action帧相关 */
    mac_back_variant_enum_uint8     back_var;        /* BA会话的变体 */
    hi_u8                       dialog_token;        /* ADDBA交互帧的dialog token */
    hi_u8                       ba_policy;           /* Immediate=1 Delayed=0 */
    hi_u8                       lut_index;           /* 接收端Session H/w LUT Index */
    hi_u16                      us_status_code;      /* 返回状态码 */
    hi_u16                      us_ba_timeout;       /* BA会话交互超时时间 */
    hi_u8                      *puc_transmit_addr;   /* BA会话发送端地址 */
    hi_u8                       amsdu_supp;      /* BLOCK ACK支持AMSDU的标识 */
    hi_u8                       auc_resv1[1];
    hi_u16                      us_baw_head;         /* bitmap的起始序列号 */
    hi_u32                      aul_rx_buf_bitmap[2]; /* 元素个数为2 */
} hmac_ba_rx_stru;

/* user结构中，TID对应的BA信息的保存结构 */
typedef struct {
    hi_u8               tid_no;
    hi_u8               ampdu_start;            /* 标识该tid下的AMPDU是否已经被设置 */
    hi_u8               tx_ba_attemps;
    hi_u8               ba_flag;                /* 大于等于5时表示是否可以建立BA会话。用户初始化、删除BA会话时清零 */
    hmac_ba_tx_stru        *ba_tx_info;
    hmac_ba_rx_stru        *ba_rx_info;             /* 由于部分处理上移，这部分内存到LocalMem中申请 */
} hmac_tid_stru;

typedef struct {
    hi_u32  short_preamble          : 1,        /* 是否支持802.11b短前导码 0=不支持， 1=支持 */
                erp                 : 1,        /* AP保存STA能力使用,指示user是否有ERP能力， 0=不支持，1=支持 */
                short_slot_time     : 1,        /* 短时隙: 0=不支持, 1=支持 */
                ac2g                : 1,
                bit_resv            : 28;
} hmac_user_cap_info_stru;

#ifdef _PRE_WLAN_FEATURE_WAPI
#define WAPI_KEY_LEN                    16
#define WAPI_PN_LEN                     16
#define HMAC_WAPI_MAX_KEYID             2

typedef struct {
    hi_u32 ulrx_mic_calc_fail; /* 由于参数错误导致mic计算错误 */
    hi_u32 ultx_ucast_drop;    /* 由于协议没有完成，将帧drop掉 */
    hi_u32 ultx_wai;
    hi_u32 ultx_port_valid;      /* 协商完成的情况下，发送的帧个数 */
    hi_u32 ulrx_port_valid;      /* 协商完成的情况下，接收的帧个数 */
    hi_u32 ulrx_idx_err;         /* 接收idx错误错误 */
    hi_u32 ulrx_netbuff_len_err; /* 接收netbuff长度错误 */
    hi_u32 ulrx_idx_update_err;  /* 密钥更新错误 */
    hi_u32 ulrx_key_en_err;      /* 密钥没有使能 */
    hi_u32 ulrx_pn_odd_err;      /* PN奇偶校验出错 */
    hi_u32 ulrx_pn_replay_err;   /* PN重放 */
    hi_u32 ulrx_memalloc_err;    /* rx内存申请失败 */
    hi_u32 ulrx_decrypt_ok;      /* 解密成功的次数 */

    hi_u32 ultx_memalloc_err;    /* 内存分配失败 */
    hi_u32 ultx_mic_calc_fail;   /* 由于参数错误导致mic计算错误 */
    hi_u32 ultx_encrypt_ok;      /* 加密成功的次数 */
    hi_u8 aucrx_pn[WAPI_PN_LEN]; /* 问题发生时，记录接收方向帧的PN,此pn会随时被刷新 */
} hmac_wapi_debug;

typedef struct {
    hi_u8 auc_wpi_ek[WAPI_KEY_LEN];
    hi_u8 auc_wpi_ck[WAPI_KEY_LEN];
    hi_u8 auc_pn_rx[WAPI_PN_LEN];
    hi_u8 auc_pn_tx[WAPI_PN_LEN];
    hi_u8 key_en;
    hi_u8 auc_rsv[3]; /* 3 保留字节 */
} hmac_wapi_key_stru;

typedef struct tag_hmac_wapi_stru {
    hi_u8 port_valid; /* wapi控制端口 */
    hi_u8 keyidx;
    hi_u8 keyupdate_flg; /* key更新标志 */
    hi_u8 pn_inc;        /* pn步进值 */

    hmac_wapi_key_stru ast_wapi_key[HMAC_WAPI_MAX_KEYID]; /* keyed: 0~1 */

#ifdef _PRE_WAPI_DEBUG
    hmac_wapi_debug debug; /* 维侧 */
#endif

    hi_u8 (*wapi_filter_frame)(struct tag_hmac_wapi_stru *pst_wapi, oal_netbuf_stru *pst_netbuff);
    hi_u8 (*wapi_is_pn_odd)(const hi_u8 *puc_pn); /* 判断pn是否为奇数 */
    hi_u32 (*wapi_decrypt)(struct tag_hmac_wapi_stru *pst_wapi, oal_netbuf_stru *pst_netbuff);
    hi_u32 (*wapi_encrypt)(struct tag_hmac_wapi_stru *pst_wapi, oal_netbuf_stru *pst_netbuf);
    oal_netbuf_stru *(*wapi_netbuff_txhandle)(struct tag_hmac_wapi_stru *pst_wapi, oal_netbuf_stru *pst_netbuf);
    oal_netbuf_stru *(*wapi_netbuff_rxhandle)(struct tag_hmac_wapi_stru *pst_wapi, oal_netbuf_stru *pst_netbuf);
} hmac_wapi_stru;

#endif

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
/* 业务识别-五元组结构体: 用于唯一地标识业务流 */
typedef struct {
    hi_u32 sip; /* ip */
    hi_u32 dip;

    hi_u16 us_sport; /* 端口 */
    hi_u16 us_dport;

    hi_u32 proto; /* 协议 */
} hmac_tx_flow_info_stru;

/* 业务识别-待识别队列结构体: */
typedef struct {
    hmac_tx_flow_info_stru flow_info;

    hi_u32 len; /* 来包长度 */
    hi_u8  flag; /* 有效位，用于计数 */

    hi_u8  udp_flag; /* udp flag为1即为UDP帧 */
    hi_u8  tcp_flag; /* tcp flag为1即为TCP帧 */

    hi_u8  rtpver;        /* RTP version */
    hi_u32 rtpssrc;      /* RTP SSRC */
    hi_u32 payload_type; /* RTP:标记1bit、有效载荷类型(PT)7bit */
} hmac_tx_judge_info_stru;

/* 业务识别-待识别队列主要业务结构体: */
typedef struct {
    hmac_tx_flow_info_stru flow_info;

    hi_u32 average_len; /* 业务来包平均长度 */
    hi_u8  flag;         /* 有效位 */

    hi_u8  udp_flag; /* udp flag为1即为UDP帧 */
    hi_u8  tcp_flag; /* tcp flag为1即为TCP帧 */

    hi_u8  rtpver;        /* RTP version */
    hi_u32 rtpssrc;      /* RTP SSRC */
    hi_u32 payload_type; /* 标记1bit、有效载荷类型(PT)7bit */

    hi_u32 wait_check_num; /* 待检测列表中此业务包个数 */
} hmac_tx_major_flow_stru;

/* 业务识别-用户已识别结构体: */
typedef struct {
    hmac_tx_flow_info_stru cfm_flow_info; /* 已识别业务的五元组信息 */

    hi_u32 last_jiffies; /* 记录已识别业务的最新来包时间 */
    hi_u16 us_cfm_tid;   /* 已识别业务tid */

    hi_u16 us_cfm_flag; /* 有效位 */
} hmac_tx_cfm_flow_stru;

/* 业务识别-用户待识别业务队列: */
typedef struct {
    hi_u32 jiffies_st; /* 记录待识别业务队列的起始时间与最新来包时间 */
    hi_u32 jiffies_end;
    hi_u32 to_judge_num; /* 用户待识别业务队列长度 */

    hmac_tx_judge_info_stru ast_judge_cache[MAX_JUDGE_CACHE_LENGTH]; /* 待识别流队列 */
} hmac_tx_judge_list_stru;
#endif

typedef struct {
    hi_u8                       amsdu_supported;             /* 每个位代表某个TID是否支持AMSDU */
    mac_user_stats_flag_stru    user_stats_flag;             /* 1byte 当user是sta时候，指示user是否被统计到对应项 */
    hi_u16                      us_amsdu_maxsize;            /* amsdu最大长度 */

    hmac_amsdu_stru            *past_hmac_amsdu[WLAN_WME_MAX_TID_NUM];     /* amsdu指针数组 */
    hmac_tid_stru               ast_tid_info[WLAN_TID_MAX_NUM];            /* 保存与TID相关的信息 */
    hi_u8                      *ch_text;                                  /* WEP用的挑战明文 */
    frw_timeout_stru            mgmt_timer;                              /* 认证关联用定时器 */
    frw_timeout_stru            defrag_timer;                            /* 去分片超时定时器 */
    oal_netbuf_stru            *defrag_netbuf;
#ifdef _PRE_WLAN_FEATURE_PMF
    mac_sa_query_stru           sa_query_info;                      /* sa query流程的控制信息 */
#endif
    mac_rate_stru               op_rates;                           /* user可选速率 AP侧保存STA速率；STA侧保存AP速率 */
    hmac_user_cap_info_stru     hmac_cap_info;                      /* hmac侧用户能力标志位 */
    hi_u32                      assoc_req_ie_len;
    hi_u8                      *puc_assoc_req_ie_buff;

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    /* 发送/接收 tcp/udp be,bk,vi,vo报文 计数到HMAC_EDCA_OPT_PKT_NUM u16足够 */
    hi_u16                      txrx_data_stat[WLAN_WME_AC_BUTT][WLAN_TXRX_DATA_BUTT];
#endif
#ifdef _PRE_WLAN_FEATURE_WAPI
    hmac_wapi_stru              wapi;
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hi_u32                      rssi_last_timestamp;                /* 获取user rssi所用时间戳, 1s最多更新一次rssi */
#endif
#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
    hi_u8                       cfm_num;                            /* 用户已被识别业务个数 */
    hi_u8                       auc_resv2[3]; /* 3 保留字节 */
    hmac_tx_cfm_flow_stru       ast_cfm_flow_list[MAX_CONFIRMED_FLOW_NUM];  /* 已识别业务 */
    hmac_tx_judge_list_stru     judge_list;                                 /* 待识别流队列 */
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
    /* 标志当前Mesh用户AMPE关联阶段是否已经收到并上报Confirm帧,关联成功时重置 */
    hi_u8                       has_rx_mesh_confirm;
    hi_u8                       mesh_resv[3];   /* 3:预留 4字节对齐 */
#endif
    /* 此项变量仅能处于HMAC USER结构体内的最后一项 且不能修改和释放 由模块初始化时与mac user指针一一对应 */
    mac_user_stru              *base_user;
} hmac_user_stru;

/* SA Query 超时定时器 和 间隔定时器 的超时函数入参结构 */
typedef struct {
    mac_vap_stru     *mac_vap;                  /* 发送SA Query request的mac vap信息 */
    hmac_user_stru   *hmac_user;                /* 目标user */
} hmac_maxtimeout_timer_stru;
typedef struct {
    mac_vap_stru       *mac_vap;                  /* 发送SA Query request的mac vap信息 */
    hmac_user_stru     *hmac_user;                /* 目标user */
    hi_u16              us_trans_id;              /* SA Query request帧的trans id */
    hi_u8               is_protected;             /* SA Query管理帧加密的使能开关 */
    hi_u8               resv;
} hmac_interval_timer_stru;

/* 存储AP关联请求帧的ie信息，用于上报内核 */
typedef struct {
    hi_u32 assoc_req_ie_len;
    hi_u8 *puc_assoc_req_ie_buff;
    hi_u8  auc_user_mac_addr[WLAN_MAC_ADDR_LEN];
    hi_u8  uc_resv[2]; /* 2 保留字节 */
} hmac_asoc_user_req_ie_stru;

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : hmac_user_ht_support
 功能描述  : 是否为HT用户
 输入参数  : 无
 输出参数  : 无
 返 回 值  : HI_TRUE是，HI_FALSE不是
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_u8 hmac_user_ht_support(const hmac_user_stru *hmac_user)
{
    if (hmac_user->base_user->ht_hdl.ht_capable == HI_TRUE) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 是否支持ht/vht聚合
 输入参数  : hmac_user_stru *pst_hmac_user
 返 回 值  : static inline hi_u8
 修改历史      :
  1.日    期   : 2013年12月12日,星期四
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_u8 hmac_user_xht_support(const hmac_user_stru *hmac_user)
{
    if ((hmac_user->base_user->cur_protocol_mode >= WLAN_HT_MODE) &&
        (hmac_user->base_user->cur_protocol_mode < WLAN_PROTOCOL_BUTT)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_void hmac_user_res_exit(hi_void);
hi_u32 hmac_user_res_init(hi_void);
hi_u32 hmac_user_alloc(hi_u8 *puc_user_idx);
hi_u8 *hmac_user_get_user_stru(hi_u8 idx);
hi_u32 hmac_user_free(hi_u8 idx);
hi_u32 hmac_user_set_avail_num_space_stream(mac_user_stru *mac_user, wlan_nss_enum_uint8 vap_nss);
hi_u32 hmac_send_del_user_event(const mac_vap_stru *mac_vap, const hi_u8 *da_mac_addr, hi_u8 user_idx);
hi_u32 hmac_user_del(mac_vap_stru *mac_vap, hmac_user_stru *hmac_user);
hi_u32 hmac_user_add(mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len, hi_u8 *puc_user_index);
hi_u32 hmac_user_add_multi_user(const mac_vap_stru *mac_vap, hi_u8 *puc_user_index);
hi_u32 hmac_user_del_multi_user(hi_u8 idx);
hi_u32 hmac_user_add_notify_alg(const mac_vap_stru *mac_vap, hi_u8 user_idx);
hi_u32 hmac_update_user_last_active_time(mac_vap_stru *mac_vap, hi_u8 len, hi_u8 *puc_param);
hi_void hmac_tid_clear(mac_vap_stru *mac_vap, hmac_user_stru *hmac_user);
hmac_user_stru *mac_vap_get_hmac_user_by_addr(mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 addr_len);

#ifdef _PRE_WLAN_FEATURE_WAPI
hmac_wapi_stru *hmac_user_get_wapi_ptr(const mac_vap_stru *mac_vap, hi_bool pairwise, hi_u8 pairwise_idx);
hi_u8 hmac_user_is_wapi_connected(hi_void);
#endif
hi_u32 hmac_user_asoc_info_report(mac_vap_stru *mac_vap, const mac_user_stru *mac_user, hi_u8 asoc_state);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 hmac_set_multicast_user_whitelist(const mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len);
hi_u32 hmac_del_multicast_user_whitelist(const mac_vap_stru *mac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _HMAC_USER_H__ */
