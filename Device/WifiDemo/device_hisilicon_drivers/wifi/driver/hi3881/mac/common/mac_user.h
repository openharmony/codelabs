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

#ifndef __MAC_USER_H__
#define __MAC_USER_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "oal_ext_if.h"
#include "frw_timer.h"
#include "wlan_types.h"
#include "wlan_mib.h"
#include "mac_frame.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
*****************************************************************************/
#define MAC_USER_INIT_STREAM        1
#define MAC_INVALID_USER_ID         0xff         /* 非法用户ID芯片最大规格,防止不同版本用户数量变更 */
#define MAC_INVALID_USER_ID2        0xf
#define MAC_USER_FREED              0            /* USER资源未申请 */
#define MAC_USER_ALLOCED            1            /* USER已被申请 */

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
#define MAC_REKEY_OFFLOAD_KCK_LEN              16
#define MAC_REKEY_OFFLOAD_KEK_LEN              16
#define MAC_REKEY_OFFLOAD_REPLAY_LEN           8
#endif

/* ****************************************************************************
  STRUCT定义
**************************************************************************** */
typedef struct {
    wlan_security_txop_params_stru security;
} mac_user_tx_param_stru;

typedef struct {
    hi_u8        rs_nrates;                          /* 个数 */
    hi_u8        auc_resv[3];                        /* 3 BYTE 保留字段 */
    hi_u8        auc_rs_rates[WLAN_MAX_SUPP_RATES];  /* 速率 */
} mac_rate_stru;

typedef struct {
    hi_u32  spectrum_mgmt           : 1,        /* 频谱管理: 0=不支持, 1=支持 */
                qos                 : 1,        /* QOS: 0=非QOS站点, 1=QOS站点 */
                barker_preamble_mode: 1,        /* 供STA保存BSS中站点是否都支持short preamble， 0=支持， 1=不支持 */
                /* 自动节能: 0=不支持, 1=支持 目前bit_apsd只有写没有读，wifi联盟已自己定义了WMM节能IE代替
                    cap apsd功能 ,此处预留为后续可能出的兼容性问题提供接口 */
                apsd                : 1,
                pmf_active          : 1,        /* 管理帧加密使能开关 */
                erp_use_protect     : 1,        /* 供STA保存AP是否启用了ERP保护 */
                ntxbf               : 1,
                bit_resv            : 25;
} mac_user_cap_info_stru;

/* user下ht相关信息 */
typedef struct {
    hi_u8           ht_capable                   : 1,              /* HT capable              */
                        max_rx_ampdu_factor      : 2,              /* Max AMPDU Rx Factor 2bits    */
                        min_mpdu_start_spacing   : 3,              /* Min AMPDU Start Spacing 3bits */
                        htc_support              : 1,              /* HTC 域支持              */
                        uc_resv                  : 1;
    hi_u8           primary_channel;

    mac_frame_ht_cap_stru ht_capinfo;
    hi_u8           rx_mcs_bitmask[WLAN_HT_MCS_BITMASK_LEN];   /* Rx MCS bitmask */
    hi_u8               secondary_chan_offset             : 2,
                        sta_chan_width                    : 1,
                        rifs_mode                         : 1,
                        ht_protection                     : 2,
                        nongf_sta_present                 : 1,
                        obss_nonht_sta_present            : 1;
    hi_u8               dual_beacon                       : 1,
                        dual_cts_protection               : 1,
                        secondary_beacon                  : 1,
                        lsig_txop_protection_full_support : 1,
                        pco_active                        : 1,
                        pco_phase                         : 1,
                        resv6                             : 2;

    hi_u32              imbf_receive_cap                :   1,  /* 隐式TxBf接收能力 */
                        receive_staggered_sounding_cap  :   1,  /* 接收交错探测帧的能力 */
                        transmit_staggered_sounding_cap :   1,  /* 发送交错探测帧的能力 */
                        receive_ndp_cap                 :   1,  /* 接收NDP能力 */
                        transmit_ndp_cap                :   1,  /* 发送NDP能力 */
                        imbf_cap                        :   1,  /* 隐式TxBf能力 */
                        /* 0=不支持，1=站点可以用CSI报告相应校准请求，但不能发起校准，2=保留，3=站点可以发起，
                            也可以相应校准请求 */
                        calibration                     :   2,
                        exp_csi_txbf_cap                :   1,  /* 应用CSI反馈进行TxBf的能力 */
                        exp_noncomp_txbf_cap            :   1,  /* 应用非压缩矩阵进行TxBf的能力 */
                        exp_comp_txbf_cap               :   1,  /* 应用压缩矩阵进行TxBf的能力 */
                        exp_csi_feedback                :   2,  /* 0=不支持，1=延迟反馈，2=立即反馈，3=延迟和立即反馈 */
                        exp_noncomp_feedback            :   2,  /* 0=不支持，1=延迟反馈，2=立即反馈，3=延迟和立即反馈 */
                        exp_comp_feedback               :   2,  /* 0=不支持，1=延迟反馈，2=立即反馈，3=延迟和立即反馈 */
                        min_grouping                    :   2,  /* 0=不分组，1=1,2分组，2=1,4分组，3=1,2,4分组 */
                        /* CSI反馈时，bfee最多支持的beamformer天线数，0=1Tx天线探测，1=2Tx天线探测，2=3Tx天线探测，
                            3=4Tx天线探测 */
                        csi_bfer_ant_number             :   2,
                        /* 非压缩矩阵反馈时，bfee最多支持的beamformer天线数，0=1Tx天线探测，1=2Tx天线探测，
                            2=3Tx天线探测，3=4Tx天线探测 */
                        noncomp_bfer_ant_number         :   2,
                        /* 压缩矩阵反馈时，bfee最多支持的beamformer天线数，0=1Tx天线探测，1=2Tx天线探测，
                            2=3Tx天线探测，3=4Tx天线探测 */
                        comp_bfer_ant_number            :   2,
                        csi_bfee_max_rows               :   2,  /* bfer支持的来自bfee的CSI显示反馈的最大行数 */
                        channel_est_cap                 :   2,  /* 信道估计的能力，0=1空时流，依次递增 */
                        reserved                        :   3;
}mac_user_ht_hdl_stru;

typedef struct {
    hi_u16              us_max_mcs_1ss : 2,                             /* 一个空间流的MCS最大支持MAP */
                        us_max_mcs_2ss : 2,                             /* 一个空间流的MCS最大支持MAP */
                        us_max_mcs_3ss : 2,                             /* 一个空间流的MCS最大支持MAP */
                        us_max_mcs_4ss : 2,                             /* 一个空间流的MCS最大支持MAP */
                        us_max_mcs_5ss : 2,                             /* 一个空间流的MCS最大支持MAP */
                        us_max_mcs_6ss : 2,                             /* 一个空间流的MCS最大支持MAP */
                        us_max_mcs_7ss : 2,                             /* 一个空间流的MCS最大支持MAP */
                        us_max_mcs_8ss : 2;                             /* 一个空间流的MCS最大支持MAP */
} mac_max_mcs_map_stru;

typedef mac_max_mcs_map_stru mac_tx_max_mcs_map_stru;
typedef mac_max_mcs_map_stru mac_rx_max_mcs_map_stru;

typedef struct {
    hi_u16 us_max_mpdu_length;
    hi_u16 us_basic_mcs_set;

    mac_vht_cap_info_stru       vht_cap_info;

    mac_tx_max_mcs_map_stru     tx_max_mcs_map;
    mac_rx_max_mcs_map_stru     rx_max_mcs_map;

    hi_u16  rx_highest_rate : 13,
                resv2           : 3;
    hi_u16  tx_highest_rate : 13,
                resv3           : 3;                 /* 解析vht Capabilities IE: VHT Supported MCS Set field */

    hi_u8 vht_capable; /* VHT capable */

    /* vht operation只有是ap的情况下有 */
    hi_u8 channel_width; /* 解析VHT Operation IE */
                         /* uc_channel_width的取值，0 -- 20/40M, 1 -- 80M, 2 -- 160M */
    hi_u8 channel_center_freq_seg0;
    hi_u8 channel_center_freq_seg1;
} mac_vht_hdl_stru;

/* user结构体，对SA Query流程信息的保存结构 */
typedef struct {
    hi_u16 us_sa_query_count;                 /* number of pending SA Query requests, 0 = no SA Query in progress */
    hi_u16 us_sa_query_trans_id;              /* trans id */
    hi_u32 sa_query_start_time;               /* sa_query 流程开始时间,单位ms */
    frw_timeout_stru sa_query_interval_timer; /* SA Query 间隔定时器，相关dot11AssociationSAQueryRetryTimeout */
} mac_sa_query_stru;

typedef struct {
    hi_u8 qos_info;                         /* 关联请求中的WMM IE的QOS info field */
    hi_u8 max_sp_len;                       /* 从qos info字段中提取翻译的最大服务长度 */
    hi_u8 auc_resv[2];                      /* 2 byte 保留字段 */
    hi_u8 ac_trigger_ena[WLAN_WME_AC_BUTT]; /* 4个AC的trigger能力 */
    hi_u8 ac_delievy_ena[WLAN_WME_AC_BUTT]; /* 4个AC的delivery能力 */
} mac_user_uapsd_status_stru;

/* 用户与AP的关联状态枚举 */
typedef enum {
    MAC_USER_STATE_AUTH_COMPLETE   = 1,
    MAC_USER_STATE_AUTH_KEY_SEQ1   = 2,
    MAC_USER_STATE_ASSOC           = 3,

    MAC_USER_STATE_BUTT            = 4
} hmac_user_asoc_state_enum;
typedef hi_u8 mac_user_asoc_state_enum_uint8;
typedef struct {
    hi_u8 auc_user_addr[WLAN_MAC_ADDR_LEN]; /* 用户mac地址 */
    hi_u8 conn_rx_rssi;
    mac_user_asoc_state_enum_uint8 assoc_state;
    hi_u8 bcn_prio;
    hi_u8 is_mesh_user;
    hi_u8 is_initiative_role;
} mac_user_assoc_info_stru;

/* 802.1X-port 状态结构体 */
/* 1X端口状态说明:                                                  */
/* 1) portvalid && keydone 都是 TRUE: 表示端口处于合法状态          */
/* 2) portvalid == TRUE && keydone == FALSE:表示端口处于未知状态    */
/*                                     密钥还未生效                 */
/* 3) portValid == FALSE && keydone== TRUE:表示端口处于非法状态     */
/*                                      密钥获取失败                */
/* 4) portValid && keyDone are FALSE: 表示端口处于合法状态          */
/*                                          密钥还未生效            */
typedef struct {
    hi_u8 keydone;      /* 端口合法性是否允许测试 */
    hi_u8 portvalid;    /* 端口合法性标识 */
    hi_u8 auc_resv0[2]; /* 2 BYTE 保留字段 */
} mac_8021x_port_status_stru;
/* 供AP查看STA是否被统计到对应项 */
typedef struct {
    /* 指示user是否被统计到no short slot num中, 0表示未被统计， 1表示已被统计 */
    hi_u8             no_short_slot_stats_flag     :1;
    hi_u8             no_short_preamble_stats_flag :1;      /* 指示user是否被统计到no short preamble num中 */
    hi_u8             no_erp_stats_flag            :1;      /* 指示user是否被统计到no erp num中 */
    hi_u8             no_ht_stats_flag             :1;      /* 指示user是否被统计到no ht num中 */
    hi_u8             no_gf_stats_flag             :1;      /* 指示user是否被统计到no gf num中 */
    hi_u8             m_only_stats_flag            :1;      /* 指示user是否被统计到no 20M only num中 */
    hi_u8             no_40dsss_stats_flag         :1;      /* 指示user是否被统计到no 40dsss num中 */
    hi_u8             no_lsig_txop_stats_flag      :1;      /* 指示user是否被统计到no lsig txop num中 */
} mac_user_stats_flag_stru;
/* AP侧keepalive活动的控制结构体 */
typedef struct {
    hi_u8 keepalive_count_ap; /* AP定时保活检测操作计数器 */
    /* 复用aging timer/STA省电模式定时清醒策略，定时发送keepalive帧的计数器 */
    hi_u8 timer_to_keepalive_count;
    hi_u8 delay_flag; /* 标志用户进入睡眠状态或者其他不能马上收帧反馈的状态 */

    hi_u8 auc_resv[1];
} mac_user_keepalive;
/* 密钥管理结构体 */
typedef struct {
    wlan_ciper_protocol_type_enum_uint8 cipher_type : 7;
    hi_u8 gtk : 1;          /* 指示RX GTK的槽位，02使用 */
    hi_u8 default_index;    /* 默认索引 */
    hi_u8 igtk_key_index;   /* igtk索引 */
    hi_u8 last_gtk_key_idx; /* igtk索引 */

    wlan_priv_key_param_stru ast_key[WLAN_NUM_TK + WLAN_NUM_IGTK]; /* key缓存 */
} mac_key_mgmt_stru;
/* 空间流信息结构体 */
typedef struct {
    hi_u8 user_idx;
    hi_u8 uc_resv;
    hi_u8 avail_num_spatial_stream; /* Tx和Rx支持Nss的交集,供算法调用 */
    hi_u8 num_spatial_stream;       /* 用户支持的空间流个数 */
} mac_user_nss_stru;
/* opmode信息结构体 */
typedef struct {
    hi_u8 user_idx;
    hi_u8 avail_num_spatial_stream;    /* Tx和Rx支持Nss的交集,供算法调用 */
    hi_u8 avail_bf_num_spatial_stream; /* 用户支持的Beamforming空间流个数 */
    hi_u8 frame_type;

    wlan_bw_cap_enum_uint8 avail_bandwidth; /* 用户和VAP带宽能力交集,供算法调用 */
    wlan_bw_cap_enum_uint8 cur_bandwidth;   /* 默认值与en_avail_bandwidth相同,供算法调用修改 */
    hi_u8 auc_resv[2];                      /* 2 BYTE 保留字段 */
} mac_user_opmode_stru;
typedef struct mac_key_params_tag {
    hi_u8 auc_key[OAL_WPA_KEY_LEN];
    hi_u8 auc_seq[OAL_WPA_SEQ_LEN];
    hi_s32 key_len;
    hi_s32 seq_len;
    hi_u32 cipher;
} mac_key_params_stru;
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
typedef struct _mac_rekey_offload_stru {
    hi_u8 auc_kck[MAC_REKEY_OFFLOAD_KCK_LEN];
    hi_u8 auc_kek[MAC_REKEY_OFFLOAD_KEK_LEN];
    hi_u8 auc_replay_ctr[MAC_REKEY_OFFLOAD_REPLAY_LEN];
} mac_rekey_offload_stru;
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct _mac_set_mesh_user_gtk_stru {
    hi_u8 auc_addr[WLAN_MAC_ADDR_LEN];
    hi_u8 auc_gtk[WLAN_CCMP_KEY_LEN];
} mac_set_mesh_user_gtk_stru;
#endif
/* mac user结构体, hmac_user_stru和dmac_user_stru公共部分 */
typedef struct {
    /* 当前VAP工作在AP或STA模式，以下字段为user是STA或AP时公共字段，新添加字段请注意!!! */
    hi_list                             user_dlist;                          /* 用于hash双向链中的双向链表 */
    hi_u8                               vap_id           : 4,                /* vap ID 0-7 */
                                        is_mesh_user     : 1,                /* 标志该User用户是否为Mesh骨干网用户 */
                                        port_valid       : 1,                /* 802.1X端口合法性标识 */
                                        is_multi_user    : 1,
                                        is_mesh_mbr      : 1;                /* 标识关联节点是否为MBR节点 */
    hi_u8                               user_hash_idx;                       /* 索引值(关联) */
    hi_u8                               user_mac_addr[WLAN_MAC_ADDR_LEN];   /* user对应的MAC地址 */

    hi_u8                               is_user_alloced  : 1,                /* 标志此user资源是否已经被申请 */
                                        mesh_user_leave  : 1,                /* 标志此user离开(收到user的切离保护帧) */
                                        mesh_initiative_role: 1,             /* Mesh关联发起端 */
                                        uc_resv             : 5;
    mac_user_asoc_state_enum_uint8      user_asoc_state;                     /* 用户关联状态  */
    /* user对应资源池索引值; user为STA时，表示填在管理帧中的AID，值为用户的资源池索引值1~32(协议规定范围为1~2007) */
    hi_u8                               us_assoc_id;
    wlan_protocol_enum_uint8            protocol_mode        : 4;            /* 用户工作协议 */
    wlan_protocol_enum_uint8            avail_protocol_mode  : 4;            /* 用户和VAP协议模式交集, 供算法调用 */

    /* 默认值与en_avail_protocol_mode值相同, 供算法调用修改 */
    wlan_protocol_enum_uint8            cur_protocol_mode    : 4;
    wlan_bw_cap_enum_uint8              bandwidth_cap        : 4;            /* 用户带宽能力信息 */
    wlan_bw_cap_enum_uint8              avail_bandwidth      : 4;            /* 用户和VAP带宽能力交集,供算法调用 */
    /* 默认值与en_avail_bandwidth相同,供算法调用修改 */
    wlan_bw_cap_enum_uint8              cur_bandwidth        : 4;
    hi_u8                               avail_num_spatial_stream : 4;        /* Tx和Rx支持Nss的交集,供算法调用 */
    hi_u8                               num_spatial_stream   : 4;            /* 用户支持的空间流个数 */
    /* 表示该mesh用户的节点优先级,beacon和probe rsp中携带，仅mesh用户有效 */
    hi_u8                               bcn_prio;
    mac_rate_stru                       avail_op_rates;                /* 用户和VAP可用的11a/b/g速率交集，供算法调用 */
    mac_user_tx_param_stru              user_tx_info;                  /* TX相关参数 */
    mac_user_cap_info_stru              cap_info;                      /* user基本能力信息位 */
    mac_user_ht_hdl_stru                ht_hdl;                        /* HT capability IE和 operation IE的解析信息 */
    mac_key_mgmt_stru                   key_info;

    hi_s8                               rx_conn_rssi;                  /* 用户关联过程接收管理帧RSSI统计量 MESH */
    hi_u8                               resv[3];                       /* reserve 3byte */
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    hi_u8                               avail_bf_num_spatial_stream;         /* 用户支持的Beamforming空间流个数 */
#endif
} mac_user_stru;

#ifdef _PRE_WLAN_FEATURE_UAPSD
#define MAC_USR_UAPSD_EN     0x01  /* U-APSD使能 */
#define MAC_USR_UAPSD_TRIG   0x02  /* U-APSD可以被trigger */
#define MAC_USR_UAPSD_SP     0x04  /* u-APSD在一个Service Period处理中 */

/* AC是trigge_en的且用户当前处于trigger状态，can be trigger */
#define mac_usr_uapsd_ac_can_tigger(_ac, _dmac_usr) \
    (((_dmac_usr)->uapsd_status.ac_trigger_ena[_ac]) && ((_dmac_usr)->uapsd_flag & MAC_USR_UAPSD_TRIG))

/* AC是delivery_en的且用户当前处于trigger状态,can be delivery */
#define mac_usr_uapsd_ac_can_delivery(_ac, _dmac_usr) \
    (((_dmac_usr)->uapsd_status.ac_delievy_ena[_ac]) && ((_dmac_usr)->uapsd_flag & MAC_USR_UAPSD_TRIG))

#define mac_usr_uapsd_use_tim(_dmac_usr) (mac_usr_uapsd_ac_can_delivery(WLAN_WME_AC_BK, _dmac_usr) &&    \
        mac_usr_uapsd_ac_can_delivery(WLAN_WME_AC_BE, _dmac_usr) && \
        mac_usr_uapsd_ac_can_delivery(WLAN_WME_AC_VI, _dmac_usr) && \
        mac_usr_uapsd_ac_can_delivery(WLAN_WME_AC_VO, _dmac_usr))
#endif

#define mac_11i_is_ptk(macaddr_is_zero, pairwise) ((HI_TRUE != (macaddr_is_zero)) && (HI_TRUE == (pairwise)))

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
#define MAC_USER_STATE_DEL 0
#endif

/* ****************************************************************************
  inline函数定义
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 设置用户的安全信息
 返 回 值  : 0:成功,其他:失败
 其    他  : 截止此函数编写时，只有keyid得到使用，multiuser下的安全信息可以优化一下
 修改历史      :
  1.日    期   : 2015年03月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_key(mac_user_stru *multiuser, wlan_cipher_key_type_enum_uint8 keytype,
    wlan_ciper_protocol_type_enum_uint8 ciphertype, hi_u8 keyid)
{
    multiuser->user_tx_info.security.cipher_key_type = keytype;
    multiuser->user_tx_info.security.cipher_protocol_type = ciphertype;
    multiuser->user_tx_info.security.cipher_key_id = keyid;
    oam_warning_log4(0, OAM_SF_WPA, "{mac_user_set_key::keytpe==%u, ciphertype==%u, keyid==%u, usridx==%u}", keytype,
        ciphertype, keyid, multiuser->us_assoc_id);
}

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
/* ****************************************************************************
 功能描述  : 设置user公共区域uc_avail_num_spatial_stream Tx和Rx支持Nss的交集,供算法调用的值
 返 回 值  : hi_void
 修改历史      :
  1.日    期   : 2015年4月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_avail_bf_num_spatial_stream(mac_user_stru *mac_user, hi_u8 value)
{
    mac_user->avail_bf_num_spatial_stream = value;
}
#endif

/* ****************************************************************************
 功能描述  : 设置user公共区域uc_avail_num_spatial_stream Tx和Rx支持Nss的交集,供算法调用的值
 修改历史      :
  1.日    期   : 2015年4月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_avail_num_spatial_stream(mac_user_stru *mac_user, hi_u8 value)
{
    mac_user->avail_num_spatial_stream = value;
}

/* ****************************************************************************
 功能描述  : 设置user公共区域uc_num_spatial_stream用户支持空间流个数的值
 修改历史      :
  1.日    期   : 2015年4月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_num_spatial_stream(mac_user_stru *mac_user, hi_u8 value)
{
    mac_user->num_spatial_stream = value;
}

/* ****************************************************************************
 功能描述  : 设置用户的bandwidth能力
 修改历史      :
  1.日    期   : 2015年4月27日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_bandwidth_cap(mac_user_stru *mac_user, wlan_bw_cap_enum_uint8 bandwidth_value)
{
    mac_user->bandwidth_cap = bandwidth_value;
}

/* ****************************************************************************
 功能描述  : 这只mac user中的user与vap协议模式的交集模式
 修改历史      :
  1.日    期   : 2015年4月20日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_protocol_mode(mac_user_stru *mac_user, wlan_protocol_enum_uint8 protocol_mode)
{
    mac_user->protocol_mode = protocol_mode;
}

/* ****************************************************************************
 功能描述  : 设置user下可用速率集
 修改历史      :
  1.日    期   : 2015年5月5日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_avail_op_rates(mac_user_stru *mac_user, hi_u8 rs_nrates, const hi_u8 *puc_rs_rates)
{
    mac_user->avail_op_rates.rs_nrates = rs_nrates;
    if (memcpy_s(mac_user->avail_op_rates.auc_rs_rates, WLAN_MAX_SUPP_RATES, puc_rs_rates, WLAN_MAX_SUPP_RATES) !=
        EOK) {
        mac_user->avail_op_rates.rs_nrates = 0;
        return;
    }
}

/* ****************************************************************************
 功能描述  : 设置与用户pmf能力协商的结果
 修改历史      :
  1.日    期   : 2015年5月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_pmf_active(mac_user_stru *mac_user, hi_u8 pmf_active)
{
    mac_user->cap_info.pmf_active = pmf_active;
}

/* ****************************************************************************
 功能描述  : 设置用户barker_preamble的模式
 修改历史      :
  1.日    期   : 2015年5月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_barker_preamble_mode(mac_user_stru *mac_user, hi_u8 barker_preamble_mode)
{
    mac_user->cap_info.barker_preamble_mode = barker_preamble_mode;
}

/* ****************************************************************************
 功能描述  : 设置与用户的qos使能结果‘
 修改历史      :
  1.日    期   : 2015年5月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
static inline hi_void mac_user_set_qos(mac_user_stru *mac_user, hi_u8 qos_mode)
{
    mac_user->cap_info.qos = qos_mode;
}

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_u32 mac_user_res_init(const hi_u8 user_num);
hi_void mac_user_res_exit(hi_void);
hi_u8 mac_user_get_user_num(hi_void);
hi_u8 mac_user_alloc_user_res(hi_void);
hi_void mac_user_free_user_res(hi_u8 idx);
hi_u8 *mac_user_init_get_user_stru(hi_u8 idx);
mac_user_stru *mac_user_get_user_stru(hi_u8 idx);
hi_u32 mac_user_add_wep_key(mac_user_stru *mac_user, hi_u8 key_index, const mac_key_params_stru *key);
hi_u32 mac_user_add_rsn_key(mac_user_stru *mac_user, hi_u8 key_index, const mac_key_params_stru *key);
hi_u32 mac_user_add_bip_key(mac_user_stru *mac_user, hi_u8 key_index, const mac_key_params_stru *key);
wlan_priv_key_param_stru *mac_user_get_key(mac_user_stru *mac_user, hi_u8 key_id);
hi_void mac_user_init(mac_user_stru *mac_user, hi_u8 user_idx, const hi_u8 *mac_addr, hi_u8 vap_id);
hi_void mac_user_set_bandwidth_info(mac_user_stru *mac_user, wlan_bw_cap_enum_uint8 avail_bandwidth,
    wlan_bw_cap_enum_uint8 cur_bandwidth);
hi_void mac_user_get_sta_cap_bandwidth(mac_user_stru *mac_user, wlan_bw_cap_enum_uint8 *pen_bandwidth_cap);
hi_void mac_user_get_ap_opern_bandwidth(mac_user_stru *mac_user, wlan_bw_cap_enum_uint8 *pen_bandwidth_cap);
hi_u32 mac_user_update_bandwidth(mac_user_stru *mac_user, wlan_bw_cap_enum_uint8 bwcap);
hi_void mac_user_set_asoc_state(mac_user_stru *mac_user, mac_user_asoc_state_enum_uint8 value);
hi_void mac_user_get_vht_hdl(mac_user_stru *mac_user, mac_vht_hdl_stru *ht_hdl);
hi_void mac_user_set_ht_hdl(mac_user_stru *mac_user, const mac_user_ht_hdl_stru *ht_hdl);
hi_void mac_user_get_ht_hdl(const mac_user_stru *mac_user, mac_user_ht_hdl_stru *ht_hdl);
hi_void mac_user_set_ht_capable(mac_user_stru *mac_user, hi_u8 ht_capable);
hi_void mac_user_set_spectrum_mgmt(mac_user_stru *mac_user, hi_u8 spectrum_mgmt);
hi_void mac_user_set_apsd(mac_user_stru *mac_user, hi_u8 apsd);
hi_void mac_user_init_key(mac_user_stru *mac_user);
hi_u32 mac_user_update_wep_key(mac_user_stru *mac_usr, hi_u8 multi_user_idx);
hi_u8 mac_addr_is_zero(const hi_u8 *mac_addr);
hi_u8 mac_user_is_user_valid(hi_u8 idx);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __MAC_USER_H__ */
