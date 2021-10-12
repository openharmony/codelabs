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

#ifndef __OAL_MEM_H__
#define __OAL_MEM_H__

#include "oal_net.h"
#include "oal_util.h"
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/kallsyms.h>
#endif
#include "oal_spinlock.h"
#include "oal_mm.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  宏定义
**************************************************************************** */
#define OAL_MEM_MAX_WORD_ALIGNMENT_BUFFER 3
#define hi_malloc(_id, _size) oal_memalloc((_size) + OAL_MEM_MAX_WORD_ALIGNMENT_BUFFER)
#define hi_free(_id, _ptr) oal_free(_ptr)

#define PACKET_H_MEM __attribute__((section("pkt_head_mem_section"))) __attribute__((zero_init))
#define PACKETMEM __attribute__((section("pkt_mem_section"))) __attribute__((zero_init))
#define PACKET_DC_MEM __attribute__((section("pkt_dc_mem_section"))) __attribute__((zero_init))

/* host侧向device侧发送数据，sdio传输为了使payload部分四字节对齐，在真正的payload前面会有pad长度扩充，
   因此，device侧需要获得真正payload部分，需要偏移pay的长度，此接口只在device侧rx adapt时使用，将pad
   部分剥离掉，后续通过OAL_NETBUF_DATA(_pst_buf)获取的即为真正的payload内容 */
#define oal_dev_netbuf_hcc_payload(_pst_buf) \
    (oal_dev_netbuf_get_payload(_pst_buf) + ((struct hcc_header *)oal_dev_netbuf_hcchdr(_pst_buf))->pad_payload)
#define oal_dev_netbuf_hcchdr(_pst_buf) oal_netbuf_get_hcc_hdr_addr(_pst_buf)

#ifdef _PRE_DEBUG_MODE
#define oal_mem_tracing(_p_data, _uc_lock) oal_mem_trace(0, __LINE__, _p_data, _uc_lock)
#define OAL_DOG_TAG_SIZE 4 /* 狗牌大小(4字节) */
#else
#define oal_mem_tracing(_p_data, _uc_lock)
#define OAL_DOG_TAG_SIZE 0
#endif

/* 对于enhanced类型的申请接口与释放接口，每一个内存块都包含一个4字节的头部， */
/* 用来指向内存块管理结构体oal_mem_struc，整个内存块的结构如下所示。           */
/*                                                                           */
/* +-------------------+------------------------------------------+---------+ */
/* | oal_mem_stru addr |                    payload               | dog tag | */
/* +-------------------+------------------------------------------+---------+ */
/* |      4/8 byte       |                                          | 4 byte  | */
/* +-------------------+------------------------------------------+---------+ */
#define OAL_MEM_INFO_SIZE 4
#define OAL_DOG_TAG 0x5b3a293e /* 狗牌，用于检测内存越界 */

#define OAL_NETBUF_MACHDR_BYTE_LEN 64 /* netbuf mac head */
#define MAC_HEAD_OFFSET             (OAL_NETBUF_MACHDR_BYTE_LEN - WLAN_MAX_MAC_HDR_LEN)
#define CB_OFFSET                   (OAL_NETBUF_MACHDR_BYTE_LEN - WLAN_MAX_MAC_HDR_LEN - HI_MAX_DEV_CB_LEN)
/* HCC PAD LEN (64 - hcc - cb - mac) */
#define OAL_PAD_HDR_LEN             (OAL_NETBUF_MACHDR_BYTE_LEN - WLAN_MAX_MAC_HDR_LEN - HI_MAX_DEV_CB_LEN - \
                                     OAL_HCC_HDR_LEN)
#define OAL_NETBUF_CONTROL_COUNT  10

#define OAL_HCC_HDR_LEN         8
#define OAL_TX_CB_LEN           48
#define HI_MAX_DEV_CB_LEN      20      /* device侧cb字段结构体长度。原值19， 四字节不对齐。 */

/* 对于enhanced类型的申请接口与释放接口，每一个内存块都包含一个4字节的头部， */
/* 用来指向内存块管理结构体oal_mem_struc，整个内存块的结构如下所示。           */
/*                                                                           */
/* +-------------------+------------------------------------------+---------+ */
/* | oal_mem_stru addr |                    payload               | dog tag | */
/* +-------------------+------------------------------------------+---------+ */
/* |      4 byte       |                                          | 4 byte  | */
/* +-------------------+------------------------------------------+---------+ */
/* ****************************************************************************
  枚举定义
**************************************************************************** */
typedef enum {
    OAL_MEM_STATE_FREE = 0, /* 该内存空闲 */
    OAL_MEM_STATE_ALLOC,    /* 该内存已分配 */
    OAL_MEM_STATE_BUTT
} oal_mem_state_enum;
typedef hi_u8 oal_mem_state_enum_uint8;

/* ****************************************************************************
  枚举名  : oal_mem_pool_id_enum_uint8
  协议表格:
  枚举说明: HOST侧内存池ID
**************************************************************************** */
typedef enum {
    OAL_MEM_POOL_ID_EVENT = 0, /* 事件内存池 */
    OAL_MEM_POOL_ID_LOCAL,     /* 本地变量内存池  */
    OAL_MEM_POOL_ID_MIB,       /* MIB内存池 */
    OAL_MEM_POOL_ID_BUTT,
} oal_mem_pool_id_enum;
typedef hi_u8 oal_mem_pool_id_enum_uint8;

/* ****************************************************************************
  枚举名  : oal_netbuf_priority_enum_uint8
  协议表格:
  枚举说明: netbuf优先级
**************************************************************************** */
typedef enum {
    OAL_NETBUF_PRIORITY_LOW     = 0,  /* 最低优先级,不能跨池申请 */
    OAL_NETBUF_PRIORITY_MID     = 1,  /* 中间优先级，可以跨池申请，但不能申请大包的最后N片 */
    OAL_NETBUF_PRIORITY_HIGH    = 2,  /* 最高优先级，可以跨池申请，且可以申请大包的最后N片 */

    OAL_NETBUF_PRIORITY_BUTT
} oal_netbuf_priority_enum;
typedef hi_u8 oal_netbuf_priority_enum_uint8;

/* ****************************************************************************
  枚举名  : oal_netbuf_id_enum_uint8
  协议表格:
  枚举说明: 对外提供netbuf内存池ID
**************************************************************************** */
typedef enum {
    OAL_NORMAL_NETBUF = 0,                /* 包括短帧、长帧内存池 */
    OAL_MGMT_NETBUF   = 1,                /* 管理帧内存池 */

    OAL_NETBUF_POOL_ID_BUTT
} oal_netbuf_id_enum;
typedef hi_u8 oal_netbuf_id_enum_uint8;

/* ****************************************************************************
  枚举名  : oal_mem_netbuf_pool_id_enum_uint8
  协议表格:
  枚举说明: netbuf内存池ID
**************************************************************************** */
typedef enum {
    OAL_MEM_NETBUF_POOL_ID_SHORT_PKT = 0, /* 短帧内存池 */
    OAL_MEM_NETBUF_POOL_ID_MGMT_PKT,      /* 管理帧 */
    OAL_MEM_NETBUF_POOL_ID_LARGE_PKT,     /* 长帧内存池 */

    OAL_MEM_NETBUF_POOL_ID_BUTT
} oal_mem_netbuf_pool_id_enum;
typedef hi_u8 oal_mem_netbuf_pool_id_enum_uint8;

/* ****************************************************************************
  结构说明: PKT内存配置结构体
**************************************************************************** */
typedef struct {
    uintptr_t start_addr; /* PKT内存块的首地址 */
    hi_u32 length;        /* 内存块长度 */
} oal_mem_pkt_cfg_stru;

/* ****************************************************************************
  结构名  : oal_netbuf_machdr_stru
  结构说明: 裸系统下mac结构体大小
**************************************************************************** */
typedef struct {
    hi_u8 auc_mac_hdr[OAL_NETBUF_MACHDR_BYTE_LEN + OAL_DOG_TAG_SIZE];
} oal_netbuf_machdr_stru;

/* ****************************************************************************
  结构名  : oal_mem_pool_info_stru
  结构说明: 内存池信息结构体
**************************************************************************** */
typedef struct {
    uintptr_t   buff_base;    /* 本内存池内存块基地址 */
    hi_u32      buff_len;     /* 本内存池内存块总长度 */
} oal_mem_pool_info_stru;

/* ****************************************************************************
  STRUCT定义
**************************************************************************** */
/* ****************************************************************************
  结构名  : oal_mem_ctrl_blk_stru
  结构说明: 将内存块所用空间封装成一个结构体
**************************************************************************** */
typedef struct {
    hi_u8 *puc_base_addr;
    hi_u32 idx;
    hi_u32 max_size;
} oal_mem_ctrl_blk_stru;

/* ****************************************************************************
  结构名  : oal_mem_netbuf_info_stru
  结构说明: netbuf内存块维测结构体
**************************************************************************** */
typedef struct {
    hi_u32 dog_tag;          /* 狗牌，用于内存写越界检查 */
    hi_u32 alloc_file_id;    /* 申请netbuf内存时的文件ID */
    hi_u32 alloc_line_num;   /* 申请netbuf内存时的行号 */
    hi_u32 alloc_time_stamp; /* 申请netbuf内存时的时间戳 */
    hi_u32 trace_file_id;    /* netbuf内存在关键路径上的文件ID */
    hi_u32 trace_line_num;   /* netbuf内存在关键路径上的行号 */
    hi_u32 trace_time_stamp; /* netbuf内存在关键路径上的时间戳 */
} oal_mem_netbuf_info_stru;

/* ****************************************************************************
  结构名  : oal_mem_stru
  结构说明: 内存块结构体
**************************************************************************** */
#pragma pack(push, 1) /* 涉及位域操作，按1字节对齐 */
struct oal_mem_stru_tag {
    hi_u8                  *puc_data;                                   /* 存放数据的指针 */
    hi_u8                  *puc_origin_data;                            /* 记录数据的原始指针 */
    hi_u16                  us_len;                                     /* 内存块的长度 */
    hi_u8                   user_cnt       :4;                          /* 申请本内存块的用户计数 */
    oal_mem_state_enum_uint8    mem_state_flag :4;                      /* 内存块状态 */
    oal_mem_pool_id_enum_uint8  pool_id        :4;                      /* 本内存属于哪一个内存池 */
    hi_u8                   subpool_id     :4;                          /* 本内存是属于哪一级子内存池 */
};
typedef struct oal_mem_stru_tag oal_mem_stru;
/* 取消实际结构对齐，恢复原有对齐方式 */
#pragma pack(pop)
/* ****************************************************************************
  结构名  : oal_mem_subpool_stru
  结构说明: 子内存池结构体
**************************************************************************** */
typedef struct {
    oal_spin_lock_stru st_spinlock;
    hi_u16 us_len;      /* 本子内存池的内存块长度 */
    hi_u16 us_free_cnt; /* 本子内存池可用内存块数 */

    /* 记录oal_mem_stru可用内存索引表的栈顶元素，其内容为oal_mem_stru指针 */
    hi_void **free_stack;

    hi_u16 us_total_cnt; /* 本子内存池内存块总数 */
    hi_u8  auc_resv[2];   /* 2: bytes保留字段 */
} oal_mem_subpool_stru;

/* ****************************************************************************
  结构名  : oal_mem_pool_stru
  结构说明: 内存池结构体
**************************************************************************** */
typedef struct {
    hi_u16              us_max_byte_len;        /* 本内存池可分配内存块最大长度 */
    hi_u8               subpool_cnt;         /* 本内存池一共有多少子内存池 */
    hi_u8               uc_resv;
    /* 子内存池索引表数组 */
    oal_mem_subpool_stru ast_subpool_table[WLAN_MEM_MAX_SUBPOOL_NUM];

    hi_u16 us_mem_used_cnt;  /* 本内存池已用内存块 */
    hi_u16 us_mem_total_cnt; /* 本内存池一共有多少内存块 */
    oal_mem_stru *mem_start_addr;
} oal_mem_pool_stru;

/* ****************************************************************************
  结构名  : oal_mem_subpool_cfg_stru
  结构说明: 子内存池配置结构体
**************************************************************************** */
typedef struct {
    hi_u16 us_size; /* 本子内存池内存块大小 */
    hi_u16 us_cnt;  /* 本子内存池内存块个数 */
} oal_mem_subpool_cfg_stru;

/* ****************************************************************************
  结构名  : oal_mem_pool_cfg_stru
  结构说明: 内存池配置结构体
**************************************************************************** */
typedef struct {
    oal_mem_pool_id_enum_uint8 pool_id; /* 内存池ID */
    hi_u8 subpool_cnt;                  /* 本内存池子内存池个数 */

    hi_u8 auc_resv[2]; /* 2: bytes保留字段 */

    /* 指向每一子内存池具体配置信息 */
    oal_mem_subpool_cfg_stru *subpool_cfg_info;
} oal_mem_pool_cfg_stru;

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_void oal_mem_exit(hi_void);
hi_void *oal_mem_alloc(oal_mem_pool_id_enum_uint8 pool_id, hi_u16 us_len);
hi_u32 oal_mem_free(const hi_void *data);
hi_u32 oal_mem_free_enhanced(oal_mem_stru *mem);
hi_u32 oal_mem_init_pool(hi_void);
oal_mem_stru *oal_mem_alloc_enhanced(oal_mem_pool_id_enum_uint8 pool_id, hi_u16 us_len);
hi_u8 *oal_dev_netbuf_get_payload(const oal_dev_netbuf_stru *dev_netbuf);
hi_u8 *oal_dev_netbuf_cb(hi_u16 us_netbuf_index);
hi_u8 *oal_dev_netbuf_get_mac_hdr(const oal_dev_netbuf_stru *dev_netbuf);
hi_u32 oal_mem_dev_netbuf_free(oal_dev_netbuf_stru *dev_netbuf);
hi_u16 oal_dev_netbuf_get_len(const oal_dev_netbuf_stru *dev_netbuf);
hi_u32 oal_mem_trace(hi_u32 file_id, hi_u32 line_num, hi_void *data, hi_u8 lock);
hi_u8 oal_mem_get_vap_res_num(hi_void);
hi_u8 oal_mem_get_user_res_num(hi_void);
hi_u32 oal_mem_set_vap_res_num(const hi_u8 vap_res_num, const hi_u8 vap_spec);
hi_u32 oal_mem_set_user_res_num(const hi_u8 user_res_num, const hi_u8 user_spec);
hi_u32 oal_mem_set_subpool_config(const oal_mem_subpool_cfg_stru *subpool_cfg, oal_mem_pool_id_enum_uint8 pool_id,
    hi_u8 subpool_num);
hi_u16 oal_mem_get_dev_netbuff_cnt(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* end of oal_mm.h */
