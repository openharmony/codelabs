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
#ifndef __HI_CONFIG_H__
#define __HI_CONFIG_H__
/* *************************************************************************** */
#include <hi_types.h>

/*
 * 全局模块/子模块ID设置
 * 注意增减模块个数后，需要增减 HI_MOD_MAX_NUM
 * SAL : [200, 300)
 * BSP : [300, 400)   -- [12c, 190)
 * MAC : [500, 600)   -- [1F4, 258)
 * APP : [800, 900)   -- [320, 384)
 */
#ifndef PRODUCT_CFG_DIAG_MODULE_ID_MAX_NUM
#define PRODUCT_CFG_DIAG_MODULE_ID_MAX_NUM 60
#endif
#define HI_MOD_MAX_NUM (PRODUCT_CFG_DIAG_MODULE_ID_MAX_NUM) /* 模块个数 */

/* SAL: [200,  300) */
#define HI_MOD_ID_SAL_SYS    201
#define HI_MOD_ID_SAL_NVM    203
#define HI_MOD_ID_SAL_DIAG   204
#define HI_MOD_ID_SAL_DFX    205
#define HI_MOD_ID_CPUP_DFX   210
#define HI_MOD_ID_CIPHER     211

/* BSP: [300, 400) */
#define HI_MOD_ID_DRV        300 /* 0x12C */
#define HI_MOD_ID_DRV_SPI    307 /* 0x133 */
#define HI_MOD_ID_DRV_DMA    316 /* 0x13c */
#define HI_MOD_ID_UPG        317
#define HI_MOD_ID_WIFI_DRV       400 /* 0x190 */

/* APP : [800, 900) */
#define HI_MOD_ID_APP_COMMON 812 /* 32C */
#define HI_MOD_ID_APP_AT     813 /* 32C */

/* Module name size in bytes */
#define HI_SYS_MOD_NAME_LEN 16
#ifdef __NEW_FILE_ID__
#define HI_DIAG_LOG_MSG_MOD_ID __NEW_MOD_ID__
#define HI_DIAG_LOG_MSG_FILE_ID __NEW_FILE_ID__
#else
#define HI_DIAG_LOG_MSG_MOD_ID  0
#define HI_DIAG_LOG_MSG_FILE_ID 0
#endif

/* DIAG包缓存单包最大大小设置 */
#define HI_DIAG_BUFFERING_MSG_MAX_LEN 100
#define HI_DIAG_BUFFERING_CMD_MAX_LEN 100

/* DAIG 通道 中 缓存队列 大小设置 */
#define HI_DIAG_BUFFERING_MSG_NUM 5 /* 消息 大于该门限则动态分配内存 */
#define HI_DIAG_BUFFERING_CMD_NUM 2 /* 命令 大于该门限则动态分配内存 */


/* ****************************************************************************
 全局静态设置
 **************************************************************************** */
/* HI_SYS_MSG_PARAM_NUM_MAX-1,由于HISO不支持宏-1，用HI_SYS_MSG_PARAM_NUM_MIN替代HI_SYS_MSG_PARAM_NUM_MIN-1 */
#define HI_SYS_MSG_PARAM_NUM_MAX_AULDATA 2

#define HI_SYS_MSG_PARAM_NUM_MIN 4
#define HI_PLC_MAC_ADDR_LEN      6
#define HI_IPV4_ADDR_LEN         4
#define HI_PLC_RELAY_NUM_MAX     3

/* 接收DIAG包最大大小，包括通道帧头和payload */
#define HI_DIAG_PACKET_REQ_CACHE_MAX_SIZE 320  /* 诊断子系统缓存大小，上位机传输数据大于该大小，将被丢弃 */
#define HI_DIAG_PACKET_ACK_MAX_SIZE       1024 /* 限制ACK单包回复数据长度 */
#define HI_DIAG_PACKET_REQ_DATA_MAX_SIZE  320  /* 由于升级传输包需要，必须大于312字节 */

/* 任务等待异常 */
#define HI_SYS_TASK_WAIT_EXCEPTION_TIME_OUT 10000 /* ms task wait 异常等待 Sleep */
// *****************************************************************************
#if !defined(HI_HAVE_CROSS_COMPILER_DIAB_AS)
#if defined(PRODUCT_CFG_HSO)
#undef PRODUCT_CFG_MCORE_RAM_LOW_ADDR
#define PRODUCT_CFG_MCORE_RAM_LOW_ADDR 0x0
#endif

// *****************************************************************************
#define HI_SYS_RAM_MAX_ADDR   (PRODUCT_CFG_MCORE_RAM_HIGH_ADDR)
#define HI_SYS_RAM_ADDR_LIMIT (PRODUCT_CFG_MCORE_RAM_LOW_ADDR)
#define HI_ADDR_RAM_BASE      ((hi_u32)(HI_SYS_RAM_ADDR_LIMIT))

// 可读写的起始地址
#if defined(PRODUCT_CFG_PLATFORM_WINDOWS) || defined(PRODUCT_CFG_FEATURE_UT)
#define HI_ADDR_USER_WR (0x00001000)
#else
#define HI_ADDR_USER_WR 0x10000000
#endif

#if defined(PRODUCT_CFG_PLATFORM_WINDOWS) || defined(PRODUCT_CFG_FEATURE_UT)
#define HI_ADDR_USER_RAM_START (0x00001000)
#undef HI_ADDR_RAM_BASE
#define HI_ADDR_RAM_BASE (0x00001000)
#define HI_ADDR_USER_RAM_MAX 0xFFFFFFFF
#define set_ram_addr(x) if (x == 0)         \
                            x = 0x02000000
#else
#if defined(PRODUCT_CFG_HSO)
#define HI_ADDR_USER_RAM_START (0x00001000)
#define HI_ADDR_USER_RAM_MAX 0xFFFFFFFF
#define set_ram_addr(x) if (x == 0)         \
                            x = 0x02000000
#else
#if defined(HI_HAVE_CROSS_COMPILER_DIAB)
HI_EXTERN hi_u32 g_wrs_kernel_text_end;
#define HI_ADDR_USER_RAM_START (hi_u32)((hi_u32 *)(&g_wrs_kernel_text_end))
#else
#define HI_ADDR_USER_RAM_START ((hi_u32)(HI_ADDR_RAM_BASE))
#endif
#define HI_ADDR_USER_RAM_MAX (PRODUCT_CFG_MCORE_RAM_MEM_MAX_ADDR)
#define set_ram_addr(x)
#endif
#endif

#define HI_ADDR_USER_RAM HI_ADDR_USER_RAM_START
#define HI_ADDR_CODE_RAM HI_SYS_RAM_ADDR_LIMIT

/* The total size of the RAM */
HI_EXTERN hi_u32 g_halSectorsRamSize;
/* The start address of RAM */
HI_EXTERN hi_void *g_halSectorsRamStart;

#if !defined(PRODUCT_CFG_FEATURE_UT)
#if defined(HAVE_PCLINT_CHECK) /* 底层接口适配, 不参与PCLINT检查 */
#define HI_SYS_ADDR_USER_RAM (HI_SYS_RAM_ADDR_LIMIT)
#else
#define HI_SYS_ADDR_USER_RAM (((HI_ADDR_USER_RAM) < HI_SYS_RAM_ADDR_LIMIT) ? \
                             (HI_SYS_RAM_ADDR_LIMIT) : (HI_ADDR_USER_RAM))
#endif
#else
#define HI_SYS_ADDR_USER_RAM (HI_ADDR_USER_RAM_START)
#endif

#define hi_is_valid_ram_addr(_x) (((hi_u32)(_x) >= (hi_u32)(uintptr_t)g_halSectorsRamStart) && \
                                 ((hi_u32)(_x) < ((hi_u32)(uintptr_t)g_halSectorsRamStart + g_halSectorsRamSize)))

#define hi_is_valid_code_addr(_x) (_x)

#if defined(PRODUCT_CFG_MCORE_FLH_LOW_ADDR)
#define hi_is_valid_flh_addr(_x) (((hi_u32)(_x) >= (hi_u32)PRODUCT_CFG_MCORE_FLH_LOW_ADDR) && \
                                 ((hi_u32)(_x) < (hi_u32)HI_ADDR_USER_RAM_MAX))
#else
#define hi_is_valid_flh_addr(_x) (_x)
#endif

/* 必须等于 HI_SYS_MTSK_MAX_NUM + HI_SYS_ATSK_MAX_NUM 之和, 由于HSO解析问题,这里不使用四则运算表达式 */
#define HI_SYS_TSK_MAX_NUM  20
#define HI_SYS_HTSK_MAX_NUM 4
#define HI_SYS_MQ_MAX_NUM   10
#define HI_SYS_AQ_MAX_NUM   1

#define HI_SYS_MTSK_MAX_NUM       18
#define HI_SYS_ATSK_MAX_NUM       2
#define HI_DIAG_DBG_SYNC_SIZE_MAX 3


// *****************************************************************************
// NV范围
// *****************************************************************************
/* *
 * @ingroup  iot_nv
 *
 * Factory NV area start ID (including this ID).CNcomment:工厂NV区起始ID（包含此ID）。CNend
 */
#define HI_NV_FACTORY_ID_START 0x0

/* *
 * @ingroup  iot_nv
 *
 * Factory NV area end ID (not included).CNcomment:工厂NV区结束ID（不包含此ID）。CNend
 */
#define HI_NV_FACTORY_ID_END 0x16

/* *
 * @ingroup  iot_nv
 *
 * Factory NV user area start ID (including this ID).CNcomment:工厂NV区用户区起始ID（包含此ID）。CNend
 */
#define HI_NV_FACTORY_USR_ID_START HI_NV_FACTORY_ID_END

/* *
 * @ingroup  iot_nv
 *
 * Factory NV user area end ID (not included).CNcomment:工厂NV区用户区结束ID（不包含此ID）。CNend
 */
#define HI_NV_FACTORY_USR_ID_END 0x20

/* *
 * @ingroup  iot_nv
 *
 * Non factory NV area start ID (including this ID).CNcomment:非工厂NV区起始ID（包含此ID）。CNend
 */
#define HI_NV_NORMAL_ID_START HI_NV_FACTORY_USR_ID_END

/* *
 * @ingroup  iot_nv
 *
 * Non factory NV user area end ID (not included).CNcomment:非工厂NV区结束ID（不包含此ID）。CNend
 */
#define HI_NV_NORMAL_ID_END 0x80

/* ****************************************************************************
      通道设置
**************************************************************************** */
#define HI_DMS_CHL_UART_PORT_DIAG 1 /* DIAG通道支持的UART端口号, 必须小于 HI_DMS_CHL_UART_PORT_NUM_MAX */
#define HI_DMS_CHL_UART_PORT_MAX  1 /* max. */

#define HI_DMS_CHL_UART_PORT_NUM_MAX  3
#define HI_DMS_CHL_UART_PORT_NUM_MAX2 3

#define HI_DMS_CHL_MAC_PORT_MIN 0x10

#if defined(PRODUCT_CFG_SUPPORT_UPG_SEPARATE)
#define HI_DMS_CHL_MAC_PORT_UPG 0x12
#define HI_DMS_CHL_MAC_PORT_MAX 0x12
#else
#define HI_DMS_CHL_MAC_PORT_MAX 0x11
#endif

#define HI_DMS_CHL_PORT_ANY 0xFFF0

#define HI_DIAG_CHL_UART_BLOCK_WRITE_TIMEOUT 10    /* ms block写超时 */
#define HI_DMS_CHL_UART_BLOCK_WRITE_TIMEOUT  10    /* ms block写超时 */
#define HI_DMS_CHL_EXCEPTION_POLLING_WAIT    5000  /* ms task read dev 异常等待Sleep */
#define HI_DMS_CHL_FLASH_BLOCK_WRITE_TIMEOUT 10    /* ms block写超时 */
#define HI_DMS_UART_EXCEPTION_POLLING_WAIT   1000  /* ms UART异常等待Sleep */
#define HI_DMS_CHL_CONNECT_DETECT_NUM        3     /* 通道连接后心跳方式检测客户端的失败的次数 */
#define HI_DMS_CHL_REPORT_PACKET_TIMEOUT     10    /* 批量上报延时 */
#define HI_DIAG_MODULE_SYS                   5     /* MSG SYS可配置的类别个数 */
#define HI_DIAG_MODULE_DEV                   30    /* MSG DEV可配置的Module个数 */
#define HI_DIAG_MODULE_USR                   5     /* MSG USR可配置的类别个数 */

#ifndef PRODUCT_CFG_DIAG_MSG_CFG_MAX_NUM
#define PRODUCT_CFG_DIAG_MSG_CFG_MAX_NUM     10
#endif
#define HI_DIAG_MSG_ID_NUM (PRODUCT_CFG_DIAG_MSG_CFG_MAX_NUM)  /* 配置的消息个数 */

#define HI_DIAG_WAIT_MESSAGE_TIMEOUT         50  /* ms */
#define HI_DIAG_WAI_DIAG_MESSAGE_TIMEOUT     100 /* 上报 DIAG包的最小周期 ms */

/* 系统诊断ID定义范围 */
#define HI_DFX_STAT_ID_BASE_SYS 0xA000  /* SYS STAT的包ID范围: [0xA000, 0xF000) */
#define HI_DFX_STAT_ID_MAX_SYS  0xF000

/* MSG SYS和USR子类别定义(16bit) */
#define HI_MSG_SYS_L0 0xff10 // MSG SYS Level0
#define HI_MSG_SYS_L1 0xff20 // MSG SYS Level1
#define HI_MSG_SYS_L2 0xff01 // MSG SYS Level2

#define HI_MSG_USR_L0 0xf3e8 // MSG USR Level0
#define HI_MSG_USR_L1 0xf3e9 // MSG USR Level1
#define HI_MSG_USR_L2 0xf3ea // MSG USR Level2
#define HI_MSG_USR_L3 0xf3eb // MSG USR Level3
#define HI_MSG_USR_L4 0xf3ec // MSG USR Level4
#define hi_sys_inf_ver(_v, _r, _c, _b, _spc) ((((_v) & 0x07) << 29) | (((_r) & 0x07) << 26) | (((_c) & 0x3F) << 20)

#endif /* HI_HAVE_CROSS_COMPILER_DIAB_AS */

#define DIAG_PROC_TASK_STACK_SIZE    2048
#define DIAG_UART_TASK_STACK_SIZE    1536
#define FLASH_PROTECT_TASK_STACK_SIZE 1024
#define DATA_COLLECT_TASK_STACK_SIZE 2048    /* this statck will destroy after dc finish */
#ifdef HI_BOARD_ASIC
#define DIAG_QUEUE_MSG_NUM           48
#else
#define DIAG_QUEUE_MSG_NUM           100
#endif
#define DIAG_PROC_TASK_PRIO          25
#define DIAG_UART_TASK_PRIO          20
#define FLASH_PROTECT_TASK_PRIO      1
#define DATA_COLLECT_TASK_PRIO       21

#define HI_MILLISECOND_PER_TICK 10

#define PRODUCT_CFG_TASK_PRIO_SC_EXPMONITOR      30
#define PRODUCT_CFG_TASK_STACK_SIZE_SC_EXPMONITOR  2048
#define PRODUCT_CFG_FLASH_BLOCK_SIZE             0x1000
#define PRODUCT_CFG_AUTO_RESET_SYSTEM_TIMEOUT    30000
#define PRODUCT_CFG_DEEP_SLEEP_DEFAULT_MAX_MS    30000

#endif /* __HI_CONFIG_H__ */
