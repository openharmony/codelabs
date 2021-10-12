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

#ifndef __HMAC_VAP_H__
#define __HMAC_VAP_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "mac_vap.h"
#include "hmac_user.h"
#include "hmac_main.h"
#ifdef FEATURE_DAQ
#include "oal_data_collect.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define hmac_vap_dft_stats_pkt_incr(_member, _cnt)
#define hmac_vap_stats_pkt_incr(_member, _cnt) ((_member) += (_cnt))

#define HMAC_RSP_MSG_MAX_LEN 64 /* get wid返回消息最大长度 */
/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
    初始化vap特性枚举
**************************************************************************** */
typedef enum {
    HMAC_ADDBA_MODE_AUTO,
    HMAC_ADDBA_MODE_MANUAL,

    HMAC_ADDBA_MODE_BUTT
} hmac_addba_mode_enum;
typedef hi_u8 hmac_addba_mode_enum_uint8;

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
/* hmac配置私有结构 */
typedef struct {
    /* 用于wal_config层线程等待(wal_config-->hmac),给SDT下发读寄存器命令时用 */
    oal_wait_queue_head_stru  wait_queue_for_sdt_reg;
    hi_u8               wait_ack_for_sdt_reg;
    hi_u8               auc_resv2[3]; /* 3 保留字节 */
    hi_s8               ac_rsp_msg[HMAC_RSP_MSG_MAX_LEN];     /* get wid返回消息内存空间 */
    hi_u32              dog_tag;
} hmac_vap_cfg_priv_stru;

typedef struct {
    oal_wait_queue_head_stru  wait_queue;
    hi_u8                mgmt_tx_status;
    hi_u8                mgmt_tx_complete;
    hi_u8                mgmt_frame_id;
    hi_u8                uc_resv;
} oal_mgmt_tx_stru;

typedef enum {
    HMAC_REPORT_DISASSOC = 0, /* Disassociation帧 */
    HMAC_REPORT_DEAUTH = 1,   /* Deauthentication帧 */
    HMAC_REPORT_ACTION = 2,   /* ACTION帧(目前只有SA Query Action帧) */

    DEVICE_REPORT_PROTECTED_BUTT
} hmac_report_disasoc_reason;
typedef hi_u16 hmac_report_disasoc_reason_uint16;

/* end add */
typedef struct {
    hi_list timeout_head;
} hmac_mgmt_timeout_stru;

typedef struct {
    hi_u8 user_index;
    mac_vap_state_enum_uint8 state;
    hi_u8 vap_id;
    hi_u8 uc_resv;
} hmac_mgmt_timeout_param_stru;

/* 修改此结构体需要同步通知SDT，否则上报无法解析 */
typedef struct {
    /* **************************************************************************
                                收送包统计
    ************************************************************************** */
    /* 发往lan的数据包统计 */
    hi_u32 rx_pkt_to_lan;   /* 接收流程发往以太网的数据包数目，MSDU */
    hi_u32 rx_bytes_to_lan; /* 接收流程发往以太网的字节数 */

    /* **************************************************************************
                                发送包统计
    ************************************************************************** */
    /* 从lan接收到的数据包统计 */
    hi_u32 tx_pkt_num_from_lan; /* 从lan过来的包数目,MSDU */
    hi_u32 tx_bytes_from_lan;   /* 从lan过来的字节数 */
} hmac_vap_query_stats_stru;
/* 装备测试 */
typedef struct {
    hi_u32 rx_pkct_succ_num;    /* 接收数据包数 */
    hi_u32 dbb_num;             /* DBB版本号 */
    hi_u32 check_fem_pa_status; /* fem和pa是否烧毁标志 */
    hi_s16 s_rx_rssi;
    hi_u8 get_dbb_completed_flag; /* 获取DBB版本号成功上报标志 */
    hi_u8 check_fem_pa_flag;      /* fem和pa是否烧毁上报标志 */
    hi_u8 get_rx_pkct_flag;       /* 接收数据包上报标志位 */
    hi_u8 lte_gpio_check_flag;    /* 接收数据包上报标志位 */
    hi_u8 reserved[2];            /* 2 保留字节 */
} hmac_atcmdsrv_get_stats_stru;

typedef enum _hmac_tcp_opt_queue_ {
    HMAC_TCP_ACK_QUEUE = 0,
    HMAC_TCP_OPT_QUEUE_BUTT
} hmac_tcp_opt_queue;

/* hmac vap结构体 */
/* 在向此结构体中增加成员的时候，请保持整个结构体8字节对齐 */
typedef struct hmac_vap_tag {
    /* ap sta公共字段 */
    oal_net_device_stru            *net_device;                   /* VAP对应的net_devices */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hmac_atcmdsrv_get_stats_stru     st_atcmdsrv_get_status;
    oal_spin_lock_stru               st_lock_state;               /* 数据面和控制面对VAP状态进行互斥 */
#endif
    oal_mgmt_tx_stru                mgmt_tx;
    frw_timeout_stru                mgmt_timer;
    hmac_mgmt_timeout_param_stru    mgmt_timetout_param;
    frw_timeout_stru                scan_timeout;                  /* vap发起扫描时，会启动定时器，做超时保护处理 */
    frw_timeout_stru                scanresult_clean_timeout;      /* vap扫描结束时，会启动定时器，做扫描结果老化处理 */

    wlan_auth_alg_enum_uint8        auth_mode;           /* 认证算法 */
    hi_u8                           is80211i_mode;       /* 指示当前的方式时WPA还是WPA2, bit0 = 1,WPA; bit1 = 1, RSN */
    hi_u8                           ba_dialog_token;     /* BA会话创建伪随机值 */
#ifdef _PRE_WLAN_FEATURE_PMF
    hi_u8                           pre_assoc_status;
#else
    hi_u8                           resv;
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    oal_net_device_stru            *p2p0_net_device;              /* 指向p2p0 net device */
    oal_net_device_stru            *del_net_device;               /* 指向需要通过cfg80211 接口删除的 net device */
    oal_work_stru                   del_virtual_inf_worker;    /* 删除net_device 工作队列 */
    hi_u8                           en_wait_roc_end;
    hi_u8                           auc_resv0[3];
    oal_completion                  st_roc_end_ready;         /* roc end completion */
#endif
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
    oal_netbuf_head_stru            tx_queue_head[2];              /* 2个发送队列，2个线程pinpon操作 */
    hi_u8                           in_queue_id;
    hi_u8                           out_queue_id;
    hi_u8                           auc_resv1[2];                  /* 2 保留字节 */
    hi_atomic                       tx_event_num;                  /* frw发包事件的个数 */
    hi_u32                          tx_quata;                      /* 发包配额，方便配置 */
#endif
    hi_u16                          us_asoc_req_ie_len;
    hi_u16                          us_del_timeout;                     /* 多长时间超时删除ba会话 如果是0则不删除 */

    hi_u8                           protocol_fall       : 1,        /* 降协议标志位 */
                                    reassoc_flag        : 1,        /* 关联过程中判断是否为重关联动作 */
                                    init_flag           : 1,        /* 常发关闭再次打开标志 */
                                    ack_policy          : 1,        /* ack policy: 0:normal ack 1:normal ack */
                                    wmm_cap             : 1,        /* 保存与STA关联的AP是否支持wmm能力信息 */
                                    cfg_sta_pm_manual   : 1,        /* 手动设置sta pm mode的标志 */
                                    query_wait_q_flag   : 2;        /* 查询标志，HI_TRUE查询结束，HI_FALSE未结束 */

    hi_u8                           addr_filter          : 1,
                                    amsdu_active         : 1,
                                    amsdu_ampdu_active   : 1,
                                    wps_active           : 1,
                                    tx_aggr_on           : 1,
                                    ampdu_tx_on_switch   : 1,
                                    pm_status_with_csi   : 1,        /* CSI打开时保存当前低功耗状态 */
                                    is_csi_open          : 1;        /* CSI打开标志，以供判断当前是否可打开低功耗 */
    hi_u8                           auth_cnt             : 4,        /* STA认证重试次数,最大值=3 */
                                    asoc_cnt             : 4;        /* 关联重试次数,最大值=5 */
    hi_u8                           rs_nrates;                       /* 速率个数 */

    hi_u8                           auc_supp_rates[WLAN_MAX_SUPP_RATES]; /* 支持的速率集 */
    hi_u8                          *puc_asoc_req_ie_buff;
    mac_cfg_mode_param_stru         preset_para;                         /* STA协议变更时变更前的协议模式 */
    oal_wait_queue_head_stru        query_wait_q;
#ifdef FEATURE_DAQ
    wlan_acq_result_addr_stru       acq_result_addr;
    hi_u8                           station_info_query_completed_flag;
    hi_u8                           acq_status_filter;
    hi_u8                           auc_resv3[2];                         /* 2 保留字节 */
#endif
    oal_station_info_stru           station_info;
    oal_spin_lock_stru              cache_user_lock;                        /* cache_user lock */
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    frw_timeout_stru                edca_opt_timer;                       /* edca参数调整定时器 */
    hi_u16                          us_edca_opt_time_ms;                  /* edca参数调整计时器周期 ms */
    hi_u8                           edca_opt_flag_ap;                     /* ap模式下是否使能edca优化特性 */
    hi_u8                           edca_opt_flag_sta;                    /* sta模式下是否使能edca优化特性 */
#endif

#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    hi_u8                           rx_ba_session_num;                    /* 该vap下，rx BA会话的数目 */
    hi_u8                           tx_ba_session_num;                    /* 该vap下，tx BA会话的数目 */
    hi_u8                           auc_resv4[2];                         /* 2 保留字节 */
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    mac_h2d_protection_stru         prot;
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
    frw_timeout_stru                ps_sw_timer;                          /* 低功耗开关 */
#endif

    hi_s8                           ap_rssi;

    hi_u8                           query_ap_rssi_flag : 1;     /* 取值范围：HI_TRUE、HI_FALSE */
    hi_u8                           hmac_al_rx_flag    : 1;     /* hmac常收使能标志 */
    hi_u8                           mac_filter_flag    : 1;     /* 常收mac地址过滤使能标志 */
    hi_u8                           resv5              : 5;     /* 5 bit保留字段 */

    hi_u8                           max_ampdu_num;              /* ADDBA_REQ中，buffer_size的默认大小 */
    hi_u8                           tx_traffic_classify_flag;   /* 业务识别功能开关 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hi_u8                           resv[4];                    /* resv 4 bytes */
#endif
    mac_vap_stru                    *base_vap;              /* MAC VAP指针,与mac资源序号对应,必须放最后,且不能修改 */
} hmac_vap_stru;

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_u32 hmac_vap_res_exit(hi_void);
hi_u32 hmac_vap_res_init(hi_void);
oal_net_device_stru *hmac_vap_get_net_device(hi_u8 vap_id);
hmac_vap_stru *hmac_vap_get_vap_stru(hi_u8 idx);
hi_u32 hmac_vap_destroy(hmac_vap_stru *hmac_vap);
hi_u32 hmac_vap_init(hmac_vap_stru *hmac_vap, hi_u8 vap_id, const mac_cfg_add_vap_param_stru *param);
hi_u32 hmac_vap_creat_netdev(hmac_vap_stru *hmac_vap, hi_char *puc_netdev_name, const hi_s8 *mac_addr,
    hi_u8 mac_addr_len);

hi_u16 hmac_vap_check_ht_capabilities_ap(const hmac_vap_stru *hmac_vap, hi_u8 *puc_payload, hi_u16 us_info_elem_offset,
    hi_u32 msg_len, hmac_user_stru *hmac_user);
hi_u32 hmac_search_ht_cap_ie_ap(const hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, hi_u8 *puc_payload,
    hi_u16 us_current_offset, hi_bool prev_asoc_ht);
hi_void hmac_vap_net_startall(hi_void);

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
hi_u8 hmac_flowctl_check_device_is_sta_mode(hi_void);
hi_void hmac_vap_net_start_subqueue(hi_u16 us_queue_idx);
hi_void hmac_vap_net_stop_subqueue(hi_u16 us_queue_idx);
#endif

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
hi_u32 hmac_check_opmode_notify(hmac_vap_stru *hmac_vap, hi_u8 *puc_mac_hdr, hi_u8 *puc_payload,
    hi_u16 us_info_elem_offset, hi_u32 msg_len, hmac_user_stru *hmac_user);
#endif
hi_void hmac_handle_disconnect_rsp(hmac_vap_stru *hmac_vap, const hmac_user_stru *hmac_user,
    hmac_report_disasoc_reason_uint16 disasoc_reason);
#ifdef _PRE_WLAN_FEATURE_MESH
hi_u32 hmac_handle_close_peer_mesh(const hmac_vap_stru *hmac_vap, const hi_u8 *mac_addr, hi_u8 mac_addr_len,
    hi_u16 us_disasoc_reason_code, hi_u16 us_dmac_reason_code);
#endif
hi_u32 hmac_tx_get_mac_vap(hi_u8 vap_id, mac_vap_stru **mac_vap);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HMAC_VAP_H__ */
