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

#ifndef __HCC_COMM_H
#define __HCC_COMM_H

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hi_types.h"
#include "frw_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/* ****************************************************************************
  2 宏定义
*****************************************************************************/
#define hcc_get_event_hdr(pst_event)        ((pst_event)->event_hdr)
#define hcc_get_event_payload(pst_event)    ((hi_u8*)((pst_event)->auc_event_data))
#define hcc_get_event_len(pst_event)        ((pst_event)->event_hdr.us_length)

#define hcc_field_setup(_p, _m, _v)         ((_p)->_m = (_v))

/* 事件头初始化宏 */
#define hcc_mac_event_hdr_init(_pst_event_hdr, _en_type, _uc_sub_type, _uc_chip_id, _uc_device_id, _uc_vap_id, \
    _us_length)                                                                                                \
    do {                                                                                                       \
        hcc_field_setup((_pst_event_hdr), en_type, (_en_type));                                                \
        hcc_field_setup((_pst_event_hdr), uc_sub_type, (_uc_sub_type));                                        \
        hcc_field_setup((_pst_event_hdr), uc_chip_id, (_uc_chip_id));                                          \
        hcc_field_setup((_pst_event_hdr), uc_device_id, (_uc_device_id));                                      \
        hcc_field_setup((_pst_event_hdr), uc_vap_id, (_uc_vap_id));                                            \
        hcc_field_setup((_pst_event_hdr), us_length, (_us_length));                                            \
    } while (0)

#define HCC_TEST_CMD_START_TX          0
#define HCC_TEST_CMD_START_RX          1
#define HCC_TEST_CMD_START_LOOP        2
#define HCC_TEST_CMD_STOP_TEST         3

/* hcc tx transfer flow control */
#define HCC_FC_NONE 0x0 /* 无流控 */
#define HCC_FC_WAIT 0x1 /* 阻塞等待，如果是在中断上下文调用，该标记被自动清除,非中断上下文生效 */
#define HCC_FC_NET  0x2 /* 对于网络层的流控 */
#define HCC_FC_DROP 0x4 /* 流控采用丢包方式,流控时返回成功 */
#define HCC_FC_ALL  (HCC_FC_WAIT | HCC_FC_NET | HCC_FC_DROP)

#define HCC_CONFIG_FRAME 0x80

#undef CONFIG_HCC_DEBUG
#undef CONFIG_HCC_TX_MULTI_BUF_CHECK
#define CONFIG_HCC_HEADER_CHECK_SUM

/* ****************************************************************************
  2 结构体定义
**************************************************************************** */
typedef enum {
    HCC_NETBUF_NORMAL_QUEUE = 0, /* netbuf is shared with others */
    HCC_NETBUF_HIGH_QUEUE = 1,   /* netbuf is special for high pri */
    HCC_NETBUF_QUEUE_BUTT
} hcc_netbuf_queue_type_enum;

/* ****************************************************************************
  结构名  : frw_event_hdr_stru
  结构说明: 事件头结构体,
  备注    : uc_length的值为(payload长度 + 事件头长度 - 2)
**************************************************************************** */
typedef struct {
    hi_u8           type;         /* 事件类型 */
    hi_u8           sub_type;     /* 事件子类型 */
    hi_u8           chip_id;      /* 芯片ID */
    hi_u8           device_id;    /* 设备ID */
    hi_u8           vap_id;       /* VAP ID */
    hi_u8           resever;      /* 保留 */
    hi_u16          us_length;    /* payload length */
} hcc_mac_event_hdr_stru;

typedef enum {
    HCC_ACTION_TYPE_WIFI = 0, /* data from wifi */
    HCC_ACTION_TYPE_OAM = 1,  /* data from oam,sdt etc. */
    HCC_ACTION_TYPE_TEST = 2, /* used for hcc transfer test */
    HCC_ACTION_TYPE_BUTT
} hcc_action_type_enum;

typedef enum _wifi_sub_type_ {
    WIFI_CONTROL_TYPE       = 0,
    WIFI_DATA_TYPE          = 1,
    WIFI_SUB_TYPE_BUTT,
} wifi_sub_type;

typedef enum _oam_sub_type_ {
    DEAULT_VALUE        = 0,
    DUMP_REG            = 1,
    DUMP_MEM            = 2,
} oam_sub_type;

typedef enum {
    HCC_WIFI_SUB_TYPE_CONTROL  = 0,
    HCC_WIFI_SUB_TYPE_DATA     = 1,
    HCC_WIFI_SUB_TYPE_BUTT,
} hcc_event_wifi_sub_type;

typedef struct {
    hi_u32 main_type;
    hi_u32 sub_type;
    hi_u8  extend_len;
    hi_u32 fc_flag;
    hi_u32 queue_id;
    hi_u8 resv[3]; /* reserved bytes: 3. */
} hcc_transfer_param;

/* hcc protocol header
|-------hcc total(64B)-----|-----------package mem--------------|
|hcc hdr|pad hdr|hcc extend|pad_payload|--------payload---------|
*/
#pragma pack(push, 1)
/* 4bytes */
struct hcc_header {
    hi_u16      sub_type: 4;        /* sub type to hcc type,refer to hcc_action_type */
    hi_u16      main_type: 3;       /* main type to hcc type,refer to hcc_action_type */
    hi_u16      pad_hdr: 6;         /* pad_hdr only need 6 bits, pad_hdr used to algin hcc tcm hdr(64B) */
    hi_u16      pad_payload: 2;     /* the pad to algin the payload addr */
    hi_u16      more: 1;            /* for hcc aggregation */
    hi_u16      seq: 4;             /* seq num for debug */
    hi_u16      pay_len: 12;        /* the payload length, did't contain the extend hcc hdr area */
} __OAL_DECLARE_PACKED;
#pragma pack(pop)

typedef struct hcc_header hcc_header_stru;

struct hcc_extend_hdr {
    frw_event_type_enum_uint8 nest_type;
    hi_u8 nest_sub_type;
    hi_u8 chip_id : 2;
    hi_u8 device_id : 2;
    hi_u8 vap_id : 4;
    hi_u8 config_frame;
} __OAL_DECLARE_PACKED;

typedef struct hcc_extend_hdr frw_hcc_extend_hdr_stru;

#define HCC_HDR_TOTAL_LEN 64
#define HCC_HDR_LEN (sizeof(hcc_header_stru))
#define HCC_HDR_RESERVED_MAX_LEN (HCC_HDR_TOTAL_LEN - HCC_HDR_LEN)
#define HCC_EXTEND_TOTAL_SIZE (WLAN_MAX_MAC_HDR_LEN + HI_MAX_DEV_CB_LEN + (hi_u32)sizeof(frw_hcc_extend_hdr_stru))

typedef enum _hcc_test_case_ {
    HCC_TEST_CASE_TX = 0,
    HCC_TEST_CASE_RX = 1,
    HCC_TEST_CASE_LOOP = 2,
    HCC_TEST_CASE_COUNT
} hcc_test_case;

/* less than 16,4bits for sub_type */
typedef enum _hcc_test_subtype_ {
    HCC_TEST_SUBTYPE_CMD = 0, /* command mode */
    HCC_TEST_SUBTYPE_DATA = 1,
    HCC_TEST_SUBTYPE_BUTT
} hcc_test_subtype;

typedef struct _hcc_test_cmd_stru_ {
    hi_u16 cmd_type;
    hi_u16 cmd_len;
} hcc_test_cmd_stru;
#define hcc_get_test_cmd_data(base) (((hi_u8 *)(base)) + sizeof(hcc_test_cmd_stru))

typedef struct _hsdio_trans_test_info_ {
    hi_u32 actual_tx_pkts;
    hi_u8  resv[4];                 /* 4 byte保留字段 */
    hi_u64 total_h2d_trans_bytes;   /* total bytes trans by from host to device */
    hi_u64 total_d2h_trans_bytes;   /* total bytes trans by from device to host */
} hsdio_trans_test_info;

typedef struct _hsdio_trans_test_rx_info_ {
    hi_u32 total_trans_pkts;
    hi_u32 pkt_len;
    hi_u8  pkt_value;
    hi_u8  resv[3]; /* 3 byte保留字段，word对齐 */
} hsdio_trans_test_rx_info;

enum _hcc_descr_type_ {
    HCC_DESCR_ASSEM_RESET = 0,
    HCC_NETBUF_QUEUE_SWITCH = 1,
    HCC_DESCR_TYPE_BUTT
};

typedef hi_u32 hcc_descr_type;
/* This descr buff is reversed in device,
   the callback function can't cost much time,
   just for transfer sdio buff message */
struct hcc_descr_header {
    hcc_descr_type descr_type;
};

typedef struct exception_bcpu_dump_header {
    hi_u32 align_type;
    hi_u32 start_addr;
    hi_u32 men_len;
} exception_bcpu_dump_header;

enum dump_h2d_cmd {
    READ_MEM = 0,
    READ_REG = 1,

    DUMP_H2D_BUTT,
};

enum dump_d2h_cmd {
    DATA = 0,

    DUMP_D2H_BUTT,
};
typedef hi_u8 hcc_netbuf_queue_type;

typedef enum {
    DATA_HI_QUEUE = 0,
    DATA_LO_QUEUE = 1,
    HCC_QUEUE_COUNT
} hcc_queue_type_enum;

typedef struct {
    frw_event_mem_stru *(*rx_adapt_func)(frw_event_mem_stru *);
} hcc_mac_event_table_stru;

/* ****************************************************************************
  3 函数声明
**************************************************************************** */
typedef struct {
    oal_netbuf_stru *pst_netbuf;
    hi_s32 len; /* for hcc transfer */
} hcc_netbuf_stru;

typedef struct {
    hi_u8                  nest_type;
    hi_u8                  nest_sub_type;
    hi_u16                 buf_len;
    hi_void                *netbuf;
} hcc_event_stru;

/* ****************************************************************************
  外部声明
**************************************************************************** */
extern hi_u8 g_sdio_txpkt_index;

/* ****************************************************************************
  inline
**************************************************************************** */
/* the macro to set hcc hdr */
static inline hi_void hcc_hdr_param_init(hcc_transfer_param *param, hi_u32 main_type, hi_u32 sub_type,
    hi_u8 extend_len, hi_u32 fc_type, hi_u32 queue_id)
{
    param->main_type = main_type;
    param->sub_type = sub_type;
    param->extend_len = extend_len;
    param->fc_flag = fc_type;
    param->queue_id = queue_id;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
