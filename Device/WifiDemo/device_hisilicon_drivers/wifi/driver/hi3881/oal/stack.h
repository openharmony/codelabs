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

#ifndef __STACK_H__
#define __STACK_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  1 Other Header File Including
**************************************************************************** */
/* ****************************************************************************
  2 Macro Definition
**************************************************************************** */
#define VECTOR_NUMBER           48
/* ROM for code 16k */
#define ROM_BASEADDR            0x00000000
#define ROM_LEN                 0x00059000      /* 1131C-debug */
#define ROM_MPU_LEN             0x00080000      /* ROM_LEN向上取2的整数次方倍) */

#define PATCH_TAB_BASEADDR      0x00100000      /* RAMx2 0/1 belong to CPU_RAM */

#define PATCH_FP_MAX_NUM        256
#define PATCH_FP_REG_SIZE       4
#define PATCH_HEAD_SIZE         (PATCH_FP_MAX_NUM * PATCH_FP_REG_SIZE * 2)

#define PATCH_BASEADDR          RAM_BASEADDR

#define RAM_VECTOR_LEN          (VECTOR_NUMBER * 4)

/* the following region will not be downloaded from host */
#define RAM_NODL_DATA_LEN       0x3000

/* stack for normal */
#define USER_STACK_BASEADDR     (RAM_BASEADDR + RAM_LEN - RAM_NODL_DATA_LEN)
#define USER_STACK_LEN          0x1800

#define RAM_NODL_DATA_BASEADDR  USER_STACK_BASEADDR
#define RAM_VECTOR_BASEADDR     RAM_NODL_DATA_BASEADDR

#define PKT_H_RAM_BASEADDR      0x30400000
#define PKT_H_RAM_LEN           0x00006000

#define PKT_B_RAM_BASEADDR      0x40100000

#define PSM_SAVE_BASEADDR       0x4011C000
#define PSM_SAVE_PHY_OFFSET     0x400

/* PKT A 规格和地址空间需要再规划 */
#ifdef SW_DEBUG
#define OML_DATA_BUFF_LEN       0x00000C00         /* 3k */
#else
#define OML_DATA_BUFF_LEN       4
#endif
#define OML_DATA_BUFF_ADDR      (PKT_B_RAM_BASEADDR + PKT_B_RAM_LEN - OML_DATA_BUFF_LEN)

/* 总线生成时，一个slave口的基地址应该是0x3000_0000，
   但实际错误的配置为了0x0300_0000,需由MPU保护0x2000_0000~0x2FFF_FFFF 地址被改写 */
#define ROM_PROTECT_BASEADDR    0x20000000
#define ROM_PROTECT_LEN         0x10000000

/* 以下为需要MPU保护的RAM空间，如果不保护这部分空间，发生写操作后会修改正常空间的内容 */
#define ROM_PROTECT_BASEADDR_1  0x200000
#define ROM_PROTECT_LEN_1       0x200000

#define ROM_PROTECT_BASEADDR_2  0x30200000
#define ROM_PROTECT_LEN_2       0x100000

#define ROM_PROTECT_BASEADDR_3  0x30500000
#define ROM_PROTECT_LEN_3       0x100000

#define ROM_PROTECT_BASEADDR_4  0x30600000
#define ROM_PROTECT_LEN_4       0x100000

/* ****************************************************************************
  3 Enum Type Definition
**************************************************************************** */
/* ****************************************************************************
  4 Global Variable Declaring
**************************************************************************** */
/* ****************************************************************************
  5 Message Header Definition
**************************************************************************** */
/* ****************************************************************************
  6 Message Definition
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT Type Definition
**************************************************************************** */
/* ****************************************************************************
  8 UNION Type Definition
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS Definition
**************************************************************************** */
/* ****************************************************************************
  10 Function Declare
**************************************************************************** */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of stack_r4.h */
