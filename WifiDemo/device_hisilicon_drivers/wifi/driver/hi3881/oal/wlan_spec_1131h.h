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

#ifndef __WLAN_SPEC_1131_H__
#define __WLAN_SPEC_1131_H__

/* ****************************************************************************
  其他头文件包含
**************************************************************************** */
#include "hi_types.h"
#include "oal_err_wifi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  1 版本spec
**************************************************************************** */
/* 待efuse实现后切换 TBD */
#define WLAN_CHIP_VERSION_HI1131HV100 0x11310101
#define WLAN_COMPILE_VERSION PRODUCT_CFG_SOFT_VER_STR

/* ****************************************************************************
  2 多Core对应spec
**************************************************************************** */
#define WLAN_FRW_MAX_NUM_CORES            1             /* WiFi对应Linux系统CORE的数量 为1 */
#define WLAN_AMSDU_MAX_NUM                12            /* 一个amsdu下允许拥有的msdu的最大个数 */
/* ****************************************************************************
  3 STA入网功能
**************************************************************************** */
#define WLAN_ASSOC_AP_MAX_NUM              2            /* STA可同时关联的最大AP个数 */
#define WLAN_JOIN_START_TIMEOUT            10000
#define WLAN_AUTH_TIMEOUT                  512
#ifdef _PRE_WLAN_FEATURE_PMF
#define WLAN_ASSOC_REJECT_TIMEOUT          2000
#endif
#define WLAN_ASSOC_TIMEOUT                 600
#define WLAN_SCAN_REQ_MAX_BSS              2        /* 一次可以扫描的BSS个数，PROBE REQ帧中最大可携带的BSSID SSID数量 */
#define WLAN_MAX_SCAN_BSS_PER_CH           8            /* 一个信道下记录扫描到的最大BSS个数 */
#define WLAN_SSID_MAX_LEN                  (32 + 1)     /* SSID最大长度, +1为\0预留空间 */
#define WLAN_MESHID_MAX_LEN                (32 + 1)     /* Mesh ID最大长度, +1为\0预留空间 */
#define WLAN_BG_SCAN_CNT_PER_CHANNEL       1            /* 背景扫描每信道扫描次数 */
#define WLAN_SCAN_REQ_CNT_PER_CHANNEL      1            /* 每次信道扫描发送probe req帧的次数 */
#define WLAN_MAX_TIME_PER_SCAN             4500         /* 扫描的默认的最大执行时间ms，超过做超时处理 */
#define WLAN_ACTIVE_SCAN_TIME              30           /* 主动扫描每个信道停留时间 时间结束后切信道 */
#define WLAN_PASSIVE_SCAN_TIME             360          /* 被动扫描每个信道停留时间 时间结束后切信道 */
#define WLAN_SCANRESULT_CLEAN_TIME         90000        /* 扫描结果老化时间90s */
#ifdef _PRE_WLAN_FEATURE_MESH
#define WLAN_MESH_CHL_SCAN_TIME            40           /* Mesh指定信道扫描在信道的停留时间 */
#define WLAN_MESH_SCAN_TIME                60           /* Mesh扫描每个信道停留时间           时间结束后切信道 */
#endif
/* ****************************************************************************
  4 热点入网功能
**************************************************************************** */
#define WLAN_USER_ACTIVE_TRIGGER_TIME      1000         /* 活跃定时器触发周期 */
#define WLAN_USER_AGING_TRIGGER_TIME       5000         /* 老化定时器触发周期 */
#define WLAN_USER_ACTIVE_TO_INACTIVE_TIME  5000         /* 用户由活跃到非活跃的转化时间 ms */
#define WLAN_AP_USER_AGING_TIME            (300 * 1000) /* AP 用户老化时间 300S */
#define WLAN_P2PGO_USER_AGING_TIME         (60 * 1000)  /* GO 用户老化时间 60S */
#define WLAN_AP_KEEPALIVE_TRIGGER_TIME     (15 * 1000)  /* keepalive定时器触发周期 15s */
#define WLAN_AP_KEEPALIVE_INTERVAL         (WLAN_AP_KEEPALIVE_TRIGGER_TIME * 4)   /* ap发送keepalive null帧间隔 */
#define WLAN_GO_KEEPALIVE_INTERVAL         (25*1000)    /* P2P GO发送keepalive null帧间隔  */
#define WLAN_STA_KEEPALIVE_TIME            (25*1000)    /* sta发送keepalive null帧间隔, 25s */
#define WLAN_CL_KEEPALIVE_TIME             (20*1000)    /* CL发送keepalive null帧间隔,避免CL被GO pvb唤醒,20s */
#ifdef _PRE_WLAN_FEATURE_MESH_ROM
#define WLAN_MESH_USER_AGING_TIME          (90 * 1000) /* Mesh 用户老化时间 60S */
#define WLAN_MESH_KEEPALIVE_INTERVAL       (30 * 1000)    /* Mesh 发送keepalive null帧间隔  */
#endif
/* ****************************************************************************
  5 STA断网功能
**************************************************************************** */
#define WLAN_LINKLOSS_MIN_THRESHOLD        20           /* linkloss门限最小最低值 */
#define WLAN_BEACON_INTVAL_MAX             3500         /* AP最大beacon周期, ms */
#define WLAN_BEACON_INTVAL_MIN             40           /* AP最小beacon周期, ms */
#define WLAN_BEACON_INTVAL_DEFAULT         100          /* AP默认beacon周期, ms */
#define WLAN_BEACON_INTVAL_IDLE            1000         /* AP IDLE状态下beacon interval值 */
/* ****************************************************************************
  6 保护模式功能
**************************************************************************** */
#define WLAN_RTS_MIN                        1           /* RTS开启门限最小值 */
#define WLAN_RTS_MAX                        2346        /* RTS开启门限最大值 */
/* ****************************************************************************
  7 分片功能
**************************************************************************** */
#define WLAN_FRAG_THRESHOLD_MIN             512       /* 最小分片门限 */
#define WLAN_FRAG_THRESHOLD_MAX             2346      /* 最大分片门限 */
/* ****************************************************************************
  8 数据速率功能
**************************************************************************** */
#define WLAN_MAX_SUPP_RATES                 12          /* 记录扫描到的ap支持的速率最大个数 */
#define WLAN_TX_RATE_MAX_NUM                4           /* 每个用户支持的最大速率集个数 */
/* ****************************************************************************
  9 国家码功能
**************************************************************************** */
/* 2.4G下目前最大支持2个管制类信息 见wal_regdb.c */
#define WLAN_MAX_RC_NUM                      2          /* 管制类最大个数 仅JP为2 */
#define WLAN_MAX_CHANNEL_NUM                 14         /* wifi 5G 2.4G全部信道个数 */

/* ****************************************************************************
  10 WMM功能
**************************************************************************** */
#define WLAN_QEDCA_TABLE_CWMIN_MIN           1
#define WLAN_QEDCA_TABLE_CWMIN_MAX           10
#define WLAN_QEDCA_TABLE_CWMAX_MIN           1
#define WLAN_QEDCA_TABLE_CWMAX_MAX           10
#define WLAN_QEDCA_TABLE_AIFSN_MIN           2
#define WLAN_QEDCA_TABLE_AIFSN_MAX           15
#define WLAN_QEDCA_TABLE_TXOP_LIMIT_MIN      1
#define WLAN_QEDCA_TABLE_TXOP_LIMIT_MAX      65535
#define WLAN_QEDCA_TABLE_MSDU_LIFETIME_MAX   500
/* ****************************************************************************
  11 协议节能STA侧功能
**************************************************************************** */
#define WLAN_DTIM_DEFAULT                    3          /* default DTIM period */
#define WLAN_DTIM_PERIOD_MAX                 255        /* 最大的DTIM周期 */
#define WLAN_DTIM_PERIOD_MIN                 1          /* 最小的DTIM周期 */
/* ****************************************************************************
  12 安全协议类别的spec
**************************************************************************** */
#define WLAN_NUM_TK                          4
#define WLAN_NUM_IGTK                        2
#define WLAN_MAX_IGTK_KEY_INDEX              5
#define WLAN_MAX_WEP_KEY_COUNT               4
/* ****************************************************************************
  13 PMF STA功能
**************************************************************************** */
#define WLAN_SA_QUERY_RETRY_TIME             (WLAN_AP_USER_AGING_TIME / 3)   /* SA Query间隔时间,老化时间的三分之一 */
#define WLAN_SA_QUERY_MAXIMUM_TIME           (WLAN_SA_QUERY_RETRY_TIME * 3)  /* SA Query超时时间,小于老化时间 */
/* ****************************************************************************
  14 WPA功能
**************************************************************************** */
#define HAL_CE_LUT_UPDATE_TIMEOUT            4           /* 硬件MAC 最多等待32us， 软件等待40us */
/* ****************************************************************************
  15 块确认功能
**************************************************************************** */
#define WLAN_ADDBA_TIMEOUT                   500
#define WLAN_MAX_RX_BA                       16          /* 支持的接收ba窗的最大个数 mac lut = 16 */
#define WLAN_MAX_TX_BA                       8           /* 支持的发送ba窗的最大个数 */
/* ****************************************************************************
  16 AMPDU功能
**************************************************************************** */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#define WLAN_AMPDU_RX_BUFFER_SIZE            32         /* AMPDU接收端接收缓冲区的buffer size的大小 */
#define WLAN_AMPDU_RX_BA_LUT_WSIZE           32         /* AMPDU接收端用于填写BA RX LUT表的win size,
                                                           要求大于等于WLAN_AMPDU_RX_BUFFER_SIZE */
#else       /* 31H IOT下受限PKT B只有8个长帧描述, rx ampdu聚合限制为4个 否则跑流波动较大 */
#define WLAN_AMPDU_RX_BUFFER_SIZE            4          /* AMPDU接收端接收缓冲区的buffer size的大小 */
#define WLAN_AMPDU_RX_BA_LUT_WSIZE           4          /* AMPDU接收端用于填写BA RX LUT表的win size,
                                                           要求大于等于WLAN_AMPDU_RX_BUFFER_SIZE */
#endif
#define WLAN_AMPDU_TX_MAX_NUM                32         /* AMPDU发送端最大聚合子MPDU个数 */
#define WLAN_AMPDU_TX_MAX_BUF_SIZE           64         /* 发送端的buffer size */
#define WLAN_AMPDU_TX_SCHD_STRATEGY          2          /* 最大聚合设置为窗口大小的一半 */
#define HAL_MAX_BA_LUT_SIZE                  16         /* 31H mac 16个lut表 */
/* ****************************************************************************
  17 AMSDU功能
**************************************************************************** */
#define AMSDU_ENABLE_ALL_TID                 0xFF
#define WLAN_MSDU_MAX_LEN                    128        /* amsdu下子msdu的最大长度 */
#define WLAN_AMSDU_FRAME_MAX_LEN             7935       /* amsdu最大长度，受制于一个buffer长度 */
#define WLAN_DSCR_SUBTABEL_MAX_NUM           1
/* ****************************************************************************
  18 小包优化
**************************************************************************** */
#define WLAN_SMALL_RX_MAX_BUFFS              12        /* 小包数据接收描述符队列中描述符最大个数 */
#define WLAN_NORMAL_RX_MAX_BUFFS             8         /* 普通接收描述符最大个数 */
#define WLAN_HIGH_RX_MAX_BUFFS               12        /* 高优先级接收描述符最大个数 */
#define WLAN_SMALL_RX_MAX_BUFFS_PATCH        16        /* 小包数据接收描述符队列中描述符最大个数 */
#define WLAN_NORMAL_RX_MAX_BUFFS_PATCH       12        /* 普通接收描述符最大个数 */
#define WLAN_HIGH_RX_MAX_BUFFS_PATCH         12        /* 高优先级接收描述符最大个数 */
#define WLAN_NORMAL_RX_MAX_RX_OPT_BUFFS      8          /* 普通优先级描述符优化规格 */
/* ****************************************************************************
  19 TPC功能
**************************************************************************** */
#define WLAN_MAX_TXPOWER                     30         /* 最大传输功率，单位dBm */
/* ****************************************************************************
  20 DBAC功能
**************************************************************************** */
/* 虚假队列个数，用于切离一个信道时，将原信道上放到硬件队列里的帧保存起来 */
#define WLAN_TX_FAKE_QUEUE_NUM               3
#define WLAN_TX_FAKE_QUEUE_BGSCAN_ID         2
#define WLAN_FCS_PROT_MAX_FRAME_LEN          24
#define WLAN_FCS_NOA_MAX_FRAME_LEN           32         /* 最大的NOA保护帧长,不含FCS */
/* ****************************************************************************
  21 芯片适配规格
**************************************************************************** */
#define WLAN_RF_CHANNEL_NUMS                 1          /* 双通道 */
#define WLAN_TX_CHAIN_DOUBLE                 3          /* 双通道掩码 11 */
#define WLAN_TX_CHAIN_ZERO                   1          /* 通道0 掩码 01 */
#define WLAN_TX_CHAIN_ONE                    2          /* 通道1 掩码 10 */
/* ****************************************************************************
  22 STA AP规格
**************************************************************************** */
#define WLAN_OHTER_BSS_BCAST_ID              14         /* 来自其他BSS的广播帧ID */
#define WLAN_OTHER_BSS_OTHER_ID              15         /* 来自其他BSS的其他帧(组播、单播) */
#define WLAN_USER_NUM_SPEC                   8          /* 31H最大用户数量规格8个 */
#define WLAN_MESHAP_ASSOC_USER_MAX_NUM       6          /* MESHAP关联的最大用户个数:6 */
#define WLAN_SOFTAP_ASSOC_USER_MAX_NUM       4          /* AP关联的最大用户个数:4 */
#define WLAN_ACTIVE_USER_MAX_NUM             7          /* 活跃用户,DBAC关联user之和:MAX=7+1, IOT=6+1 */
#define WLAN_ASSOC_USER_IDX_BMAP_LEN         1          /* 关联用户索引位图长度 8 >> 3 */
#define WLAN_ACTIVE_USER_IDX_BMAP_LEN        1          /* 活跃用户索引位图长度 8 >> 3 */
#define WLAN_AP_NUM_PER_DEVICE               1          /* AP的规格 1131H: 1个AP */
#define WLAN_STA_NUM_PER_DEVICE              2          /* STA的规格 1131H: 2个STA */
#define WLAN_CFG_VAP_NUM_PER_DEVICE          1          /* 配置VAP个数 1个芯片1个 */
#define WLAN_SERVICE_VAP_NUM_PER_DEVICE  (WLAN_AP_NUM_PER_DEVICE + WLAN_STA_NUM_PER_DEVICE)  /* AP+STA */
#define WLAN_VAP_NUM_PER_DEVICE    (WLAN_AP_NUM_PER_DEVICE + WLAN_STA_NUM_PER_DEVICE + WLAN_CFG_VAP_NUM_PER_DEVICE)
#define WLAN_CHIP_NUM_PER_BOARD              1          /* 每个board支持chip的最大个数，总数不会超过8个 */
#define WLAN_DEVICE_NUM_PER_CHIP             1          /* 每个chip支持device的最大个数，总数不会超过8个 */
#define WLAN_DEVICE_NUM_PER_BOARD  (WLAN_CHIP_NUM_PER_BOARD * WLAN_DEVICE_NUM_PER_CHIP)  /* 单板总device数量 */
#define WLAN_VAP_NUM_PER_BOARD     (WLAN_DEVICE_NUM_PER_BOARD * WLAN_VAP_NUM_PER_DEVICE) /* 单板总vap数量 */
#define WLAN_SERVICE_VAP_START_ID            1          /* 单芯片下，每个board的业务vap id从1开始 */
#define WLAN_CFG_VAP_ID                      0          /* 单device下 配置vap的ID为0 */
#define WLAN_CFG_VAP_NAME                    "Hisilicon0"   /* 单device下 配置vap的名称 */
#define WLAN_ASSOC_MAX_ID   (WLAN_ACTIVE_USER_MAX_NUM + WLAN_SERVICE_VAP_NUM_PER_DEVICE)  /* ASSOC_ID最大值 */


/* ****************************************************************************
  23 低成本约束
**************************************************************************** */
#define WLAN_TID_FOR_DATA                    0          /* 默认的数据类型业务的TID */
#define WLAN_RX_QUEUE_NUM                    3          /* 接收队列的个数 与HAL_RX_DSCR_QUEUE_ID_BUTT相等 */
#define WLAN_TX_QUEUE_NUM                    5          /* 发送队列的个数 */
#define WLAN_RX_DSCR_LIST_NUM                2          /* 存储硬件接收上报的描述符链表数目(ping pong使用) */
#define WLAN_RX_ISR_MAX_COUNT                30         /* 接收完成中断最大个数 */
#define WLAN_DOWM_PART_RX_TRACK_MEM          200
#define WLAN_DEBUG_RX_DSCR_LINE              (12 + 2)   /* 接收描述符软件可见为第14行，用于打时间戳，调试用 */
#define WLAN_RX_FRAME_MAX_LEN                8000       /* 接收缓存最大过滤长度(超过此值，硬件将其丢弃) */
/* ****************************************************************************
  24 RX描述符动态调整
**************************************************************************** */
#define WLAN_PKT_MEM_PKT_OPT_LIMIT           2000
#define WLAN_PKT_MEM_PKT_RESET_LIMIT         500
#define WLAN_PKT_MEM_OPT_TIME_MS             1000
/* ****************************************************************************
  25 P2P特性
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_P2P
#define WLAN_MAX_SERVICE_P2P_DEV_NUM         1          /* P2P DEV数量 =1 */
#define WLAN_MAX_SERVICE_P2P_GOCLIENT_NUM    1          /* P2P GO/GC数量 =1 */
#endif
/* ****************************************************************************
  27 RSSI
**************************************************************************** */
#define WLAN_NEAR_DISTANCE_RSSI            (-35)        /* 默认近距离信号门限-35dBm */
#define WLAN_CLOSE_DISTANCE_RSSI           (-25)        /* 关联前距离判断门限-25dBm */
#define WLAN_FAR_DISTANCE_RSSI             (-73)        /* 默认远距离信号门限-73dBm */
#define WLAN_NORMAL_DISTANCE_RSSI_UP       (-42)        /* 信号强度小于-42dBm时，才认为非超近距离 */
#define WLAN_NORMAL_DISTANCE_RSSI_DOWN     (-66)        /* 信号强度大于-66dBm时，才认为是非超远距离 */
#define WLAN_NEAR_DISTANCE_IMPROVE_RSSI_UP (-40)        /* improve 1*1问题规避,要求近距离判断上门限为-44dBm */
#define WLAN_NEAR_DISTANCE_IMPROVE_RSSI_DOWN    (-48)   /* improve 1*1问题规避,要求近距离判断下门限为-50dBm */
/* ****************************************************************************
  28 COEX FEATURE
**************************************************************************** */
#define BTCOEX_RSSI_THRESHOLD               (WLAN_FAR_DISTANCE_RSSI)
#define BTCOEX_RX_WINDOW_SIZE_INDEX_0       0
#define BTCOEX_RX_WINDOW_SIZE_INDEX_1       1
#define BTCOEX_RX_WINDOW_SIZE_INDEX_2       2
#define BTCOEX_RX_WINDOW_SIZE_INDEX_3       3
#define BTCOEX_RX_WINDOW_SIZE_INDEX_MAX     4
#define BTCOEX_MAC_HDR                      32
#define BT_POSTPREEMPT_MAX_TIMES            1
#define BT_PREEMPT_MAX_TIMES                1
#define BT_POSTPREEMPT_TIMEOUT_US           150
#define BT_ABORT_RETRY_TIMES_MAX            10
#define BT_PREEMPT_TIMEOUT_US               150
#define BLE_PREEMPT_TIMEOUT_US              10
#define BTCOEX_BT_DEFAULT_DURATION          0xFF

#define BT_WLAN_COEX_UNAVAIL_PAYLOAD_THRES  8
#define BT_WLAN_COEX_SMALL_PKT_THRES        200
#define BT_WLAN_COEX_SMALL_FIFO_THRES       1023
#define BTCOEX_OCCUPY_DATA_TIMEOUT_MS       60
#define BTCOEX_OCCUPY_MGMT_TIMEOUT_MS       10
#define COEX_LINKLOSS_OCCUP_PERIOD_MS       20
#define BTCOEX_ARP_PROTECT_TIMEOUT_MS       1000
#define BTCOEX_DHCP_STEP1_PROTECT_TIMEOUT_MS 6000
#define BTCOEX_DHCP_STEP3_PROTECT_TIMEOUT_MS 8000
#define BTCOEX_DHCP_EAPOL_PROTECT_TIMEOUT_MS 3000

/* ****************************************************************************
  29 WiFi关键信息检测
**************************************************************************** */
#define WLAN_MAX_MAC_HDR_LEN                36            /* 最大的mac头长度 oal_mem.h里面引用该宏 */
#define WLAN_MIN_MAC_HDR_LEN                10            /* ack与cts的帧头长度为10 */
#define WLAN_MGMT_MAC_HDR_LEN               24            /* 管理帧的MAC帧头长度 */
#define WLAN_MEM_MAX_SUBPOOL_NUM            6             /* 内存池中最大子内存池个数 */
#define WLAN_MEM_MAX_USERS_NUM              4             /* 共享同一块内存的最大用户数 */
#define WLAN_MAC_ADDR_LEN                   6             /* MAC地址长度宏 */
#define WLAN_MAC_ADDR_BYTE_LEN              17            /* MAC地址占的字符长度 */
#define WLAN_TID_MPDU_NUM_BIT               9
#define WLAN_TID_MPDU_NUM_LIMIT             (1 << WLAN_TID_MPDU_NUM_BIT)
/* ****************************************************************************
  31 描述符内存池配置信息
**************************************************************************** */
#define WLAN_MEM_SHARE_DSCR_SUBPOOL_CNT    2               /* 共享描述符内存池子池个数 */
/* hal_rx_dscr_stru(共用4) + hi1131_rx_buffer_addr_stru(共用4) - 4 + hi1131_rx_status_dscr_stru +
   hi1131_rx_debug_dscr_stru */
#define WLAN_MEM_RX_DSCR_SIZE              48              /* 接收描述符结构体大小 */
/* 接收描述符数量 */
#define WLAN_MEM_RX_DSCR_CNT               (WLAN_SMALL_RX_MAX_BUFFS + WLAN_NORMAL_RX_MAX_BUFFS + WLAN_HIGH_RX_MAX_BUFFS)
#define WLAN_MEM_RX_DSCR_CNT_PATCH         (WLAN_SMALL_RX_MAX_BUFFS_PATCH + WLAN_NORMAL_RX_MAX_BUFFS_PATCH \
                                            + WLAN_HIGH_RX_MAX_BUFFS_PATCH)

/* hal_tx_dscr_stru(共用4) + hi1131_tx_ctrl_dscr_one_stru(共用4) - 4 + hi1131_tx_ctrl_dscr_two_stru +
   hi1131_tx_ctrl_dscr_three_stru */
#define WLAN_MEM_TX_DSCR_SIZE              76              /* 发送描述符结构体大小 hal_tx_dscr_stru有4字节公用 */
#define WLAN_MEM_TX_DSCR_CNT               14              /* 发送描述符 netbuff总数48-接收32-netbuff裕量2 */
#define WLAN_MEM_TX_DSCR_CNT_PATCH         58              /* 发送描述符 netbuff总数48-接收32-netbuff裕量2 */
/* ****************************************************************************
  34 本地内存池配置信息
**************************************************************************** */
#define WLAN_MEM_LOCAL_SUBPOOL_CNT          6               /* 本地变量内存池子池个数 */
#define WLAN_MEM_LOCAL_SIZE1                32
#define WLAN_MEM_LOCAL_SIZE2                64
#define WLAN_MEM_LOCAL_SIZE3                128
#define WLAN_MEM_LOCAL_SIZE4                256
#define WLAN_MEM_LOCAL_SIZE5                600
#define WLAN_MEM_LOCAL_SIZE6                1600            /* 自动速率算法使用 956 * 8 users */

/* ****************************************************************************
  35 netbuff内存池配置信息
**************************************************************************** */
#define WLAN_SHORT_NETBUF_SIZE              256     /* 短帧netbufpayload长度 */
#define WLAN_MGMT_NETBUF_SIZE               640     /* 管理帧netbufpayload长度 */
/* 长帧netbufpayload长度 1500+36(HDR)+4(FCS)+20(解密失败20个调试信息)+8(SNAP LLC) */
#define WLAN_LARGE_PAYLOAD_SIZE             1500    /* 允许发送的最大帧数据长度 */
#define WLAN_LARGE_NETBUF_SIZE              1600    /* (WLAN_LARGE_PAYLOAD_SIZE + 100) */
/* netbuf最大帧长，帧头 + payload */
#define WLAN_MAX_NETBUF_SIZE                (WLAN_LARGE_NETBUF_SIZE + WLAN_MAX_MAC_HDR_LEN)
#define WLAN_MEM_NETBUF_ALIGN               4       /* netbuf对齐 */
/* ****************************************************************************
  36 事件结构体内存池
**************************************************************************** */
#define WLAN_MEM_EVENT_SUBPOOL_CNT          2               /* 本地事件内存池子池个数 */
#define WLAN_MEM_EVENT_SIZE1                64              /* 注意: 事件内存长度 */
#define WLAN_MEM_EVENT_SIZE2                512             /* 注意: 事件内存长度 */
#define WLAN_MEM_EVENT_MULTI_USER_CNT1      96              /* 多用户下事件池子池1数量 */
#define WLAN_MEM_EVENT_CNT2                 6               /* 单用户下事件池子池2数量 */
#define WLAN_MEM_EVENT_MULTI_USER_CNT2      8               /* 多用户下事件池子池2数量 */
#define WLAN_MEM_EVENT_CNT1                 40              /* 单用户下事件池子池1数量 */
#define WLAN_WPS_IE_MAX_SIZE                352 // (WLAN_MEM_EVENT_SIZE2 - 32)   /* 32表示事件自身占用的空间 */
/* ****************************************************************************
  37 MIB内存池
**************************************************************************** */
#define WLAN_MEM_MIB_SUBPOOL_CNT            1               /* 本地MIB内存池子池个数 */
#define WLAN_MEM_MIB_SIZE                   476             /* 当前(wlan_mib_ieee802dot11_stru)=444+4 预留28bytes */
/* ****************************************************************************
  38 TCP ACK优化
**************************************************************************** */
#define DEFAULT_TX_TCP_ACK_THRESHOLD        1       /* 丢弃发送ack 的门限 */
#define DEFAULT_RX_TCP_ACK_THRESHOLD        1       /* 丢弃接收ack 的门限 */
/* ****************************************************************************
  39 frw相关的spec
**************************************************************************** */
/* *****************************************************************************
    事件队列配置信息表
    注意: 每个队列所能容纳的最大事件个数必须是2的整数次幂
****************************************************************************** */
#define FRW_EVENT_MAX_NUM_QUEUES (FRW_EVENT_TYPE_BUTT * WLAN_VAP_NUM_PER_BOARD)
/* 配置VAP事件队列配置 */
#define WLAN_FRW_EVENT_CFG_TABLE_CFG_VAP \
         /* 事件类型                 队列权重   队列最大事件个数   队列调度策略 */  \
/* FRW_EVENT_TYPE_HIGH_PRIO        */    { 1,          32,              0, 0}, \
/* FRW_EVENT_TYPE_HOST_CRX         */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_HOST_DRX         */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_HOST_CTX         */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_DMAC_TO_HMAC_CFG */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_WLAN_CRX         */    { 1,          32,              0, 0}, \
/* FRW_EVENT_TYPE_WLAN_DRX         */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_WLAN_CTX         */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_WLAN_DTX         */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_WLAN_TX_COMP     */    { 1,          32,              1, 0}, \
/* FRW_EVENT_TYPE_TBTT             */    { 1,           0,              1, 0}, \
/* FRW_EVENT_TYPE_TIMEOUT          */    { 1,          32,              1, 0}, \
/* FRW_EVENT_TYPE_DMAC_MISC        */    { 1,          32,              0, 0}, \
/* FRW_EVENT_TYPE_HCC              */    { 1,          32,              1, 0},

/* 业务VAP事件队列配置 */
/* FRW_EVENT_TYPE_HIGH_PRIO             HAL层只通过配置VAP分发事件,业务VAP队列置0
   FRW_EVENT_TYPE_WLAN_TX_COMP          HAL层只通过配置VAP分发事件,业务VAP队列置0
   FRW_EVENT_TYPE_TIMEOUT               FRW层只通过配置VAP分发事件,业务VAP队列置0
   FRW_EVENT_TYPE_TBTT                  配置VAP没有TBTT中断
*/
#define WLAN_FRW_EVENT_CFG_TABLE_SERVIC_VAP \
    /* 事件类型       队列权重   队列所能容纳的最大事件个数   队列所属调度策略 */  \
         /* 事件类型                 队列权重   队列最大事件个数   队列调度策略 */  \
/* FRW_EVENT_TYPE_HIGH_PRIO        */    { 1,          0 ,              0, 0}, \
/* FRW_EVENT_TYPE_HOST_CRX         */    { 1,          32,              1, 0}, \
/* FRW_EVENT_TYPE_HOST_DRX         */    { 1,          32,              1, 0}, \
/* FRW_EVENT_TYPE_HOST_CTX         */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_DMAC_TO_HMAC_CFG */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_WLAN_CRX         */    { 1,          64,              0, 0}, \
/* FRW_EVENT_TYPE_WLAN_DRX         */    { 1,          64,              1, 0}, \
/* FRW_EVENT_TYPE_WLAN_CTX         */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_WLAN_DTX         */    { 1,          32,              1, 0}, \
/* FRW_EVENT_TYPE_WLAN_TX_COMP     */    { 1,          0 ,              1, 0}, \
/* FRW_EVENT_TYPE_TBTT             */    { 1,          16,              1, 0}, \
/* FRW_EVENT_TYPE_TIMEOUT          */    { 1,          0 ,              1, 0}, \
/* DMAC FRW_EVENT_TYPE_DMAC_MISC   */    { 1,          32,              0, 0}, \
/* FRW_EVENT_TYPE_HCC              */    { 1,          32,              1, 0},

#define WLAN_FRW_EVENT_CFG_TABLE {                                       \
        WLAN_FRW_EVENT_CFG_TABLE_CFG_VAP    \
        WLAN_FRW_EVENT_CFG_TABLE_SERVIC_VAP \
        WLAN_FRW_EVENT_CFG_TABLE_SERVIC_VAP \
        WLAN_FRW_EVENT_CFG_TABLE_SERVIC_VAP \
    }
/* ****************************************************************************
  40 虚拟OS适配
**************************************************************************** */
#define WLAN_WPA_KEY_LEN 32 /* WPA 密钥长度 */
#define WLAN_WPA_SEQ_LEN 16 /* WPA 序号长度 */

/* 管理帧子类型 */
typedef enum {
    WLAN_ASSOC_REQ              = 0,    /* 0000 */
    WLAN_ASSOC_RSP              = 1,    /* 0001 */
    WLAN_REASSOC_REQ            = 2,    /* 0010 */
    WLAN_REASSOC_RSP            = 3,    /* 0011 */
    WLAN_PROBE_REQ              = 4,    /* 0100 */
    WLAN_PROBE_RSP              = 5,    /* 0101 */
    WLAN_TIMING_AD              = 6,    /* 0110 */
    WLAN_MGMT_SUBTYPE_RESV1     = 7,    /* 0111 */
    WLAN_BEACON                 = 8,    /* 1000 */
    WLAN_ATIM                   = 9,    /* 1001 */
    WLAN_DISASOC                = 10,   /* 1010 */
    WLAN_AUTH                   = 11,   /* 1011 */
    WLAN_DEAUTH                 = 12,   /* 1100 */
    WLAN_ACTION                 = 13,   /* 1101 */
    WLAN_ACTION_NO_ACK          = 14,   /* 1110 */
    WLAN_MGMT_SUBTYPE_RESV2     = 15,   /* 1111 */

    WLAN_MGMT_SUBTYPE_BUTT      = 16,   /* 一共16种管理帧子类型 */
} wlan_frame_mgmt_subtype_enum;

typedef enum {
    WLAN_WME_AC_BE = 0,    /* best effort */
    WLAN_WME_AC_BK = 1,    /* background */
    WLAN_WME_AC_VI = 2,    /* video */
    WLAN_WME_AC_VO = 3,    /* voice */

    WLAN_WME_AC_BUTT = 4,
    WLAN_WME_AC_MGMT = WLAN_WME_AC_BUTT /* 管理AC，协议没有,对应硬件高优先级队列 */
} wlan_wme_ac_type_enum;
typedef hi_u8 wlan_wme_ac_type_enum_uint8;

/* TID编号类别 */
typedef enum {
    WLAN_TIDNO_BEST_EFFORT              = 0, /* BE业务 */
    WLAN_TIDNO_BACKGROUND               = 1, /* BK业务 */
    WLAN_TIDNO_UAPSD                    = 2, /* U-APSD */
    WLAN_TIDNO_ANT_TRAINING_LOW_PRIO    = 3, /* 智能天线低优先级训练帧 */
    WLAN_TIDNO_ANT_TRAINING_HIGH_PRIO   = 4, /* 智能天线高优先级训练帧 */
    WLAN_TIDNO_VIDEO                    = 5, /* VI业务 */
    WLAN_TIDNO_VOICE                    = 6, /* VO业务 */
    WLAN_TIDNO_BCAST                    = 7, /* 广播用户的广播或者组播报文 */

    WLAN_TIDNO_BUTT
} wlan_tidno_enum;
typedef hi_u8 wlan_tidno_enum_uint8;

enum wlan_serv_id {
    WLAN_STA0_ID = 0x00,
    WLAN_STA1_ID = 0x01,
    WLAN_STA2_ID = 0x02,
    WLAN_AP0_ID = 0x03,
    WLAN_ID_NUM = 0xff /* 用于非关联状态或初始状态向平台投票进入或退出低功耗 */
};
#define WLAN_TID_MAX_NUM WLAN_TIDNO_BUTT /* TID个数为8 */

/* ****************************************************************************
  41 Mesh相关
**************************************************************************** */
#define WLAN_MESH_PASSIVE_SCAN_PERIOD   500     /* mesh节点周期性进入被动扫描的时间 */
#define WLAN_MESH_BEACON_PRIO_MAX       255     /* mesh节点beacon prio的最大值 */
#define WLAN_MESH_BEACON_PRIO_MIN       0       /* mesh节点beacon prio的最小值 */
#define WLAN_MESH_BEACON_TIMEOUT_VAL    20      /* Mesh节点Beacon发送超时时间(ms) */
#define WLAN_MESH_6LO_PKT_LIMIT         500     /* Mesh节点6lo包头压缩帧长判断 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* #ifndef __WLAN_SPEC_1131_H__ */
