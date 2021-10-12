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

#ifndef __OAL_UTIL_H__
#define __OAL_UTIL_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include <linux/kernel.h>
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <sys/time.h>
#include <string.h>
#include <ctype.h>
#include <linux/delay.h>
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/time.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/delay.h>
#include <asm/delay.h>
#endif

#include "hi_atomic.h"
#include "hi_stdlib.h"
#include "oal_err_wifi.h"
#include "oam_log.h"
#include "wlan_spec_1131h.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#ifndef CURRENT
#define CURRENT 0
#endif
#ifndef EOK
#define EOK 0
#endif

#define __OAL_DECLARE_PACKED    __attribute__((__packed__))
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#define WIFI_ROM_TEXT
#define WIFI_ROM_RODATA
#define WIFI_ROM_DATA
#define WIFI_ROM_BSS
#else
#define WIFI_ROM_TEXT           __attribute__ ((section(".wifi.rom.text")))   /* ROM代码段 */
#define WIFI_ROM_RODATA         __attribute__ ((section(".wifi.rom.rodata"))) /* ROM const全局变量段 可与text段复用 */
#define WIFI_ROM_DATA           __attribute__ ((section(".wifi.rom.data")))   /* ROM 初值非0全局变量段 不能与bss混用 */
#define WIFI_ROM_BSS            __attribute__ ((section(".wifi.rom.bss")))    /* ROM 初值0或未赋值全局变量段
                                                                                 可以data段混用 只会影响RAM段大小 */
#endif

extern hi_u32 g_level_log;

#define _HI113X_PRINTK_STDOUT

static inline hi_char *hi_strrchr(const hi_char *file, hi_char c)
{
    hi_char *p = HI_NULL;
    return (((p = strrchr(file, c)) != HI_NULL) ? (p + 1) : (HI_NULL));
}


#if defined(HAVE_PCLINT_CHECK)
#define oal_likely(_expr)       (_expr)
#define oal_unlikely(_expr)     (_expr)
#else
#define oal_likely(_expr)       likely(_expr)
#define oal_unlikely(_expr)     unlikely(_expr)
#endif

#define OAL_BUG_ON(_con)        BUG_ON(_con)
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define OAL_WARN_ON(condition)  ({int __ret = !!(condition); __ret;})
#else
#define OAL_WARN_ON(condition)  WARN_ON(condition)
#endif

/* 数学计算 */
#define oal_max(a, b)                   (((a) > (b)) ? (a) : (b))
#define oal_min(a, b)                   (((a) < (b)) ? (a) : (b))
#define oal_sub(a, b)                   (((a) > (b)) ? ((a) - (b)) : 0)
#define oal_abs(a)                      (((a) > 0) ? (a) : (-(a)))
#define oal_abs_ab(a, b)                ((a) >= 0 ? (b) : (-(b)))
#define oal_abs_sub(a, b)               (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* Works only for digits and letters, but small and fast */
#define TOLOWER(x) ((x) | 0x20)

#define isdigit(c) ('0' <= (c) && (c) <= '9')

#define _isxdigit(c) (('0' <= (c) && (c) <= '9') || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))
#endif

/* 入参判空 */
#define oal_any_null_ptr1(_a)           (oal_unlikely(HI_NULL == (_a)))
#define oal_any_null_ptr2(_a, _b)       (oal_unlikely((HI_NULL == (_a)) || (HI_NULL == (_b))))
#define oal_any_null_ptr3(_a, _b, _c)   (oal_unlikely((HI_NULL == (_a)) || (HI_NULL == (_b)) || (HI_NULL == (_c))))

/* 字节操作 */
#define rotl_w(val, bits, width)        (((val) << (bits)) | ((val) >> ((width) - (bits))))
#define rotr_w(val, bits, width)        (((val) >> (bits)) | ((val) << ((width) - (bits))))
#define rotl32(val, bits)               rotl_w(val, bits, 32) /* 32 bits word */
#define rotr32(val, bits)               rotr_w(val, bits, 32) /* 32 bits word */

#define OAL_RET_ADDR __builtin_return_address(0)

/* RSSI统计滤波，RSSI范围是-128~127, 一般不会等于127这么大，所以将127设置为MARKER,即初始值 */
#define WLAN_RSSI_DUMMY_MARKER 0x7F
/* 将几个字符串按照指定格式合成一个字符串 */
#define OAL_PAGE_SIZE PAGE_SIZE

#define kernel_version(a, b, c)      (((a)<<16) | ((b)<<8) | (c))
#ifndef _PRE_HDF_LINUX
#define LINUX_VERSION_CODE          kernel_version(4, 9, 0)
#endif
#ifndef HW_LITEOS_OPEN_VERSION_NUM
#define HW_LITEOS_OPEN_VERSION_NUM  kernel_version(1, 3, 2)
#endif

#define oal_warn_on(condition)      (condition)
#define PLATFORM_NAME_SIZE          20

/* 虚拟地址转物理地址 */
#define oal_virt_to_phy_addr(_virt_addr) ((uintptr_t)(_virt_addr))
/* 物理地址转虚拟地址 */
#define oal_phy_to_virt_addr(_phy_addr) ((hi_u32 *)(_phy_addr))

#if (_PRE_BIG_CPU_ENDIAN == _PRE_CPU_ENDIAN) /* BIG_ENDIAN */
#define oal_byteorder_to_le64(_val) hi_swap_byteorder_64(_val)
#define oal_byteorder_to_le32(_val) hi_swap_byteorder_32(_val)
#define oal_byteorder_to_le16(_val) hi_swap_byteorder_16(_val)

#define oal_byteorder_to_be64(_val) (_val)
#define oal_byteorder_to_be32(_val) (_val)
#define oal_byteorder_to_be16(_val) (_val)

#define oal_mask_inverse(_len, _offset) ((hi_u32)(hi_swap_byteorder_32(~((hi_u32)((1UL << (_len)) - 1) << (_offset)))))
#define oal_mask(_len, _offset) ((hi_u32)(hi_swap_byteorder_32((hi_u32)((1UL << (_len)) - 1) << (_offset))))
#define oal_ntoh_16(_val) (_val)
#define oal_ntoh_32(_val) (_val)
#define oal_hton_16(_val) (_val)
#define oal_hton_32(_val) (_val)

#elif (_PRE_LITTLE_CPU_ENDIAN == _PRE_CPU_ENDIAN) /* LITTLE_ENDIAN */
#define oal_byteorder_to_le64(_val) (_val)
#define oal_byteorder_to_le32(_val) (_val)
#define oal_byteorder_to_le16(_val) (_val)

#ifdef HAVE_PCLINT_CHECK
#define oal_byteorder_to_be64(_val) (_val)
#define oal_byteorder_to_be32(_val) (_val)
#define oal_byteorder_to_be16(_val) (_val)

#define oal_ntoh_16(_val) (_val)
#define oal_ntoh_32(_val) (_val)
#define oal_hton_16(_val) (_val)
#define oal_hton_32(_val) (_val)
#else
#define oal_byteorder_to_be64(_val) hi_swap_byteorder_64(_val)
#define oal_byteorder_to_be32(_val) hi_swap_byteorder_32(_val)
#define oal_byteorder_to_be16(_val) hi_swap_byteorder_16(_val)

#define oal_ntoh_16(_val) hi_swap_byteorder_16(_val)
#define oal_ntoh_32(_val) hi_swap_byteorder_32(_val)
#define oal_hton_16(_val) hi_swap_byteorder_16(_val)
#define oal_hton_32(_val) hi_swap_byteorder_32(_val)
#endif /* HAVE_PCLINT_CHECK */

#define oal_mask_inverse(_len, _offset)    ((hi_u32)(~((hi_u32)((1UL << (_len)) - 1) << (_offset))))
#define oal_mask(_len, _offset)            ((hi_u32)((hi_u32)((1UL << (_len)) - 1) << (_offset)))
#endif /* _PRE_CPU_ENDIAN */


/* 日志级别 */
typedef enum {
    OAM_LOG_LEVEL_ERROR = 0, /* ERROR级别打印 */
    OAM_LOG_LEVEL_WARNING,   /* WARNING级别打印 */
    OAM_LOG_LEVEL_INFO,      /* INFO级别打印 */
    OAM_LOG_LEVEL_DEBUG,     /* INFO级别打印 */
    OAM_LOG_LEVEL_VERBOSE,   /* INFO级别打印 */

    OAM_LOG_LEVEL_BUTT
} oam_log_level_enum;

#define oam_logn(vid, eid, level, fmt, args...) \
    oal_print_nlogs(__FILE__, __FUNCTION__, (hi_u16)__LINE__, OAL_RET_ADDR, vid, eid, level, 0, fmt, ##args)

#define oam_log(vid, eid, level, fmt, cnt, p1, p2, p3, p4) \
    oal_print_nlogs(__FILE__, __FUNCTION__, (hi_u16)__LINE__, OAL_RET_ADDR, vid, eid, level, cnt, fmt, p1, p2, p3, p4)

#define hi_diag_log_msg_e0(id, sz)                  oam_log(0, id, OAM_LOG_LEVEL_ERROR, sz, 0, 0, 0, 0, 0)
#define hi_diag_log_msg_e1(id, sz, p1)              oam_log(0, id, OAM_LOG_LEVEL_ERROR, sz, 1, p1,0, 0, 0)
#define hi_diag_log_msg_e2(id, sz, p1, p2)          oam_log(0, id, OAM_LOG_LEVEL_ERROR, sz, 2, p1, p2, 0, 0)
#define hi_diag_log_msg_e3(id, sz, p1, p2, p3)      oam_log(0, id, OAM_LOG_LEVEL_ERROR, sz, 3, p1, p2, p3, 0)
#define hi_diag_log_msg_e4(id, sz, p1, p2, p3, p4)  oam_log(0, id, OAM_LOG_LEVEL_ERROR, sz, 4, p1, p2, p3, p4)
#define hi_diag_log_buf_e(id, sz, buffer, size)

#define hi_diag_log_msg_w0(id, sz)                  oam_log(0, id, OAM_LOG_LEVEL_WARNING, sz, 0, 0, 0, 0, 0)
#define hi_diag_log_msg_w1(id, sz, p1)              oam_log(0, id, OAM_LOG_LEVEL_WARNING, sz, 1, p1,0, 0, 0)
#define hi_diag_log_msg_w2(id, sz, p1, p2)          oam_log(0, id, OAM_LOG_LEVEL_WARNING, sz, 2, p1, p2, 0, 0)
#define hi_diag_log_msg_w3(id, sz, p1, p2, p3)      oam_log(0, id, OAM_LOG_LEVEL_WARNING, sz, 3, p1, p2, p3, 0)
#define hi_diag_log_msg_w4(id, sz, p1, p2, p3, p4)  oam_log(0, id, OAM_LOG_LEVEL_WARNING, sz, 4, p1, p2, p3, p4)
#define hi_diag_log_buf_w(id, sz, buffer, size)

#define hi_diag_log_msg_i0(id, sz)                  oam_log(0, id, OAM_LOG_LEVEL_INFO, sz, 0, 0, 0, 0, 0)
#define hi_diag_log_msg_i1(id, sz, p1)              oam_log(0, id, OAM_LOG_LEVEL_INFO, sz, 1, p1,0, 0, 0)
#define hi_diag_log_msg_i2(id, sz, p1, p2)          oam_log(0, id, OAM_LOG_LEVEL_INFO, sz, 2, p1, p2, 0, 0)
#define hi_diag_log_msg_i3(id, sz, p1, p2, p3)      oam_log(0, id, OAM_LOG_LEVEL_INFO, sz, 3, p1, p2, p3, 0)
#define hi_diag_log_msg_i4(id, sz, p1, p2, p3, p4)  oam_log(0, id, OAM_LOG_LEVEL_INFO, sz, 4, p1, p2, p3, p4)
#define hi_diag_log_bug(id, sz, buffer, size)

#define oal_io_print_err(sz)                        oam_log(0, 0, OAM_LOG_LEVEL_ERROR, sz, 0, 0, 0, 0, 0)
#define oal_io_print(sz)                            oam_print(sz)
#define oal_io_print0(sz)                           oam_log(0, 0, OAM_LOG_LEVEL_INFO, sz, 0, 0,0, 0, 0)
#define oal_io_print1(sz, p1)                       oam_log(0, 0, OAM_LOG_LEVEL_INFO, sz, 1, p1,0, 0, 0)
#define oal_io_print2(sz, p1, p2)                   oam_log(0, 0, OAM_LOG_LEVEL_INFO, sz, 2, p1, p2, 0, 0)
#define oal_io_print3(sz, p1, p2, p3)               oam_log(0, 0, OAM_LOG_LEVEL_INFO, sz, 3, p1, p2, p3, 0)
#define oal_io_print4(sz, p1, p2, p3, p4)           oam_log(0, 0, OAM_LOG_LEVEL_INFO, sz, 4, p1, p2, p3, p4)

#define hi_at_printf dprintf

#ifndef IS_ALIGNED
#define oal_is_aligned(val, align) (((hi_u32)(val) & ((align)-1)) == 0)
#else
#define oal_is_aligned IS_ALIGNED
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define oal_round_up        round_up
#define oal_round_down      round_down
#define oal_offset_of       offsetof
#else
#define oal_round_up(_old_len, _align)   ((((_old_len) + ((_align) - 1)) / (_align)) * (_align))
#define oal_round_down(value, boundary)  ((value) & (~((boundary) - 1)))
#define oal_offset_of(type, member)      ((long) &((type *) 0)->member)
#endif

/* ****************************************************************************
  函数声明
**************************************************************************** */
hi_void oal_strtoaddr(const hi_char *param, hi_u8 *mac_addr, hi_u8 mac_addr_len);
hi_u8 oal_bit_find_first_bit_four_byte(hi_u32 word);
hi_u8 oal_get_lut_index(hi_u8 *lut_index_table, hi_u8 bitmap_len, hi_u16 max_lut_size);
hi_void oal_random_ether_addr(hi_u8 *mac_addr, hi_u8 mac_addr_len);
hi_void oal_print_hex_dump(const hi_u8 *addr, hi_s32 len, hi_s32 group_size, hi_char *pre_str);
hi_s8 wlan_rssi_lpf(hi_s8 old, hi_s8 new);

/* ****************************************************************************
  函数定义
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define oal_atoi atoi
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 oal_atoi(const hi_char *c_string);
#endif


#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define oal_strtok strtok
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_char *oal_strtok(hi_char *pc_token, const hi_char *pc_delemit)
{
    hi_char *pc_str = HI_NULL;
    const hi_char *pc_ctrl = pc_delemit;

    hi_char uc_map[32]; /* 32个元素数组  */
    hi_s32 l_count;

    static hi_char *pc_nextoken;

    /* Clear control map */
    for (l_count = 0; l_count < 32; l_count++) { /* 32个元素数组  */
        uc_map[l_count] = 0;
    }

    /* Set bits in delimiter table */
    do {
        uc_map[*pc_ctrl >> 3] |= (1 << (*pc_ctrl & 7)); /* 7:取低3位，左移1位 */
    } while (*pc_ctrl++);

    /* Initialize str. If string is NULL, set str to the saved
     * pointer (i.e., continue breaking tokens out of the string
     * from the last strtok call) */
    if (pc_token != HI_NULL) {
        pc_str = pc_token;
    } else {
        pc_str = pc_nextoken;
    }

    /* Find beginning of token (skip over leading delimiters). Note that
     * there is no token iff this loop sets str to point to the terminal
     * null (*str == '\0') */
    while ((uc_map[*pc_str >> 3] & (1 << (*pc_str & 7))) && *pc_str) { /* 7:取低3位，左移1位 */
        pc_str++;
    }

    pc_token = pc_str;

    /* Find the end of the token. If it is not the end of the string,
     * put a null there. */
    for (; *pc_str; pc_str++) {
        if (uc_map[*pc_str >> 3] & (1 << (*pc_str & 7))) { /* 7:取低3位，左移1位 */
            *pc_str++ = '\0';
            break;
        }
    }

    /* Update nextoken (or the corresponding field in the per-thread data
     * structure */
    pc_nextoken = pc_str;

    /* Determine if a token has been found. */
    if (pc_token == pc_str) {
        return NULL;
    } else {
        return pc_token;
    }
}
#endif

/* ****************************************************************************
 功能描述  : 判断是否是大写字母
 输入参数  : c_letter: 字符串字母
**************************************************************************** */
static inline hi_u8 oal_is_alpha_upper(hi_char letter)
{
    if (letter >= 'A' && letter <= 'Z') {
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 将一个字符转化成16进制数
**************************************************************************** */
static inline hi_u8 oal_strtohex(const hi_char *str)
{
    if (*str >= '0' && *str <= '9') {
        return (hi_u8)(*str - '0');
    } else if (*str >= 'A' && *str <= 'F') {
        return (hi_u8)(*str - 'A' + 10); /* 10: 加10 */
    } else if (*str >= 'a' && *str <= 'f') {
        return (hi_u8)(*str - 'a' + 10); /* 10: 加10 */
    }

    return 0;
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define oal_strtol strtol
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static inline hi_u32 oal_simple_guess_base(const char *cp)
{
    if (cp[0] == '0') {
        if (TOLOWER(cp[1]) == 'x' && _isxdigit(cp[2])) {
            return 16; /* 16 hex */
        } else {
            return 8; /* 8 oct */
        }
    } else {
        return 10; /* 10 dec */
    }
}

static inline unsigned long long oal_simple_strtoull(const hi_char *cp, hi_char **endp, hi_u32 base)
{
    unsigned long long result = 0;

    if (!base) {
        base = oal_simple_guess_base(cp);
    }

    if (base == 16 && cp[0] == '0' && TOLOWER(cp[1]) == 'x') { /* base equal 16 */
        cp += 2;                                               /* add 2 */
    }

    while (_isxdigit(*cp)) {
        unsigned int value;

        value = isdigit(*cp) ? *cp - '0' : TOLOWER(*cp) - 'a' + 10; /* add 10 */
        if (value >= base) {
            break;
        }
        result = result * base + value;
        cp++;
    }
    if (endp != HI_NULL) {
        *endp = (hi_char *)cp;
    }

    return result;
}

static inline hi_s32 oal_strtol(const hi_char *pc_nptr, hi_char **ppc_endptr, hi_s32 l_base)
{
    /* 跳过空格 */
    while ((*pc_nptr) == ' ') {
        pc_nptr++;
    }

    if (*pc_nptr == '-') {
        return -oal_simple_strtoull(pc_nptr + 1, ppc_endptr, l_base);
    }

    return oal_simple_strtoull(pc_nptr, ppc_endptr, l_base);
}
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

/* ****************************************************************************
 功能描述  : 产生随机数
**************************************************************************** */
static inline hi_u8 oal_get_random(hi_void)
{
    return 1;
}

/* ****************************************************************************
 功能描述  : 产生随机数
 输入参数  : value:随机种子   rst_flag:0:不更新随机种子，非0:更新随机种子
**************************************************************************** */
static inline hi_u8 oal_gen_random(hi_u32 value, hi_u8 rst_flag)
{
    static hi_u32 rand_val = 0; /* 本inline函数被ROM化代码调用，lin_t e1534告警屏蔽 */

    if (rst_flag != 0) {
        rand_val = value;
    }

    rand_val = rand_val * 1664525L + 1013904223L;
    return (hi_u8)(rand_val >> 24); /* 24: 右移24位 */
}

/* ****************************************************************************
 功能描述  : 获取单字节中的bit1的个数
 输入参数  : byte:需要操作的字节
**************************************************************************** */
static inline hi_u8 oal_bit_get_num_one_byte(hi_u8 byte)
{
    byte = (byte & 0x55) + ((byte >> 1) & 0x55);
    byte = (byte & 0x33) + ((byte >> 2) & 0x33); /* 2: 右移2位 */
    byte = (byte & 0x0F) + ((byte >> 4) & 0x0F); /* 4: 右移4位 */

    return byte;
}

/* ****************************************************************************
 功能描述  : 获取4字节中bit1的个数
 输入参数  : word:需要操作的字节
**************************************************************************** */
static inline hi_u32 oal_bit_get_num_four_byte(hi_u32 word)
{
    word = (word & 0x55555555) + ((word >> 1) & 0x55555555);
    word = (word & 0x33333333) + ((word >> 2) & 0x33333333);  /* 2: 右移2位 */
    word = (word & 0x0F0F0F0F) + ((word >> 4) & 0x0F0F0F0F);  /* 4: 右移4位 */
    word = (word & 0x00FF00FF) + ((word >> 8) & 0x00FF00FF);  /* 8: 右移8位 */
    word = (word & 0x0000FFFF) + ((word >> 16) & 0x0000FFFF); /* 16: 右移16位 */

    return word;
}

/* ****************************************************************************
 功能描述  : 对1字节的指定位置一
 输入参数  : byte: 要进行位操作的变量地址
             nr: 置位的位数
**************************************************************************** */
static inline hi_void oal_bit_set_bit_one_byte(hi_u8 *byte, hi_u32 nr)
{
    *byte |= ((hi_u8)(1 << nr));
}

/* ****************************************************************************
 功能描述  : 对1字节的指定位置零
 输入参数  : byte: 要进行位操作的变量地址
             nr: 置零的位数
**************************************************************************** */
static inline hi_void oal_bit_clear_bit_one_byte(hi_u8 *byte, hi_u32 nr)
{
    *byte &= (~((hi_u8)(1 << nr)));
}

/* ****************************************************************************
 功能描述  : 对8字节的指定位置一
 输入参数  : [1]double_word
             [2]nr
 输出参数  : 无
 返 回 值  : static inline hi_void
**************************************************************************** */
static inline hi_void oal_bit_set_bit_eight_byte(hi_u64 *double_word, hi_u32 nr)
{
    *double_word |= ((hi_u64)1 << nr);
}

/* ****************************************************************************
 功能描述  : 对8字节的指定位置0
 输入参数  : [1]double_word
             [2]nr
**************************************************************************** */
static inline hi_void oal_bit_clear_bit_eight_byte(hi_u64 *double_word, hi_u32 nr)
{
    *double_word &= ~((hi_u64)1 << nr);
}

/* ****************************************************************************
 功能描述  : 找到字节数第一个是0的位数
 输入参数  : value: 要查找的字节, max_cnt: 查找字节中可用位数
 返 回 值  : 右数第一个是0的位数
**************************************************************************** */
static inline hi_u8 oal_bit_find_first_zero(hi_u32 value, hi_u8 max_cnt)
{
    hi_u8 index;

    for (index = 0; index < max_cnt; index++) {
        if ((value & (hi_u32)(1 << index)) == 0) {
            break;
        }
    }
    return index;
}

/* ****************************************************************************
 功能描述  : mac地址置零
 输入参数  : mac_addr: 需清零的mac地址的指针
**************************************************************************** */
static inline hi_void oal_set_mac_addr_zero(hi_u8 *mac_addr)
{
    mac_addr[0] = 0;
    mac_addr[1] = 0;
    mac_addr[2] = 0; /* 2: mac_addr 下标 */
    mac_addr[3] = 0; /* 3: mac_addr 下标 */
    mac_addr[4] = 0; /* 4: mac_addr 下标 */
    mac_addr[5] = 0; /* 5: mac_addr 下标 */
}

/* ****************************************************************************
 功能描述  : 比较两个mac地址是否相等
 输入参数  : mac_addr1: 第一个mac地址
             mac_addr2: 第二个mac地址
 输出参数  : 无
 返 回 值  : 不等返回1 ； 相等返回0
**************************************************************************** */
static inline hi_u32 oal_compare_mac_addr(const hi_u8 *mac_addr1, const hi_u8 *mac_addr2, hi_u8 mac_addr_len)
{
    if (mac_addr_len != WLAN_MAC_ADDR_LEN) {
        return 1;
    }

    return (mac_addr1[0] ^ mac_addr2[0]) | (mac_addr1[1] ^ mac_addr2[1])
           | (mac_addr1[2] ^ mac_addr2[2]) | (mac_addr1[3] ^ mac_addr2[3])  /* 2,3: mac_addr 下标 */
           | (mac_addr1[4] ^ mac_addr2[4]) | (mac_addr1[5] ^ mac_addr2[5]); /* 4,5: mac_addr 下标 */
}

/* ****************************************************************************
 功能描述  : 将第二个ip地址 赋值给第一个ip地址
 输入参数  : ip_addr1: 第一个mac地址
             ip_addr2: 第二个mac地址
**************************************************************************** */
static inline hi_void oal_set_ip_addr(hi_u8 *ip_addr1, const hi_u8 *ip_addr2)
{
    ip_addr1[0] = ip_addr2[0];
    ip_addr1[1] = ip_addr2[1];
    ip_addr1[2] = ip_addr2[2]; /* 2: ip_addr1 下标 */
    ip_addr1[3] = ip_addr2[3]; /* 3: ip_addr1 下标 */
}

/* ****************************************************************************
 功能描述  : 比较两个接收描述符的中断顺序号的大小，seq_num1大于seq_num2返回真
 输入参数  : (1)数值1
             (2)数值2
             (3)两个数值间的最大差值
 输出参数  : 无
 返 回 值  : HI_TRUE或者HI_FALSE
**************************************************************************** */
static inline hi_u8 oal_cmp_seq_num(hi_u32 seq_num1, hi_u32 seq_num2, hi_u32 diff_value)
{
    if (((seq_num1 < seq_num2) && ((seq_num2 - seq_num1) < diff_value)) ||
        ((seq_num1 > seq_num2) && ((seq_num1 - seq_num2) > diff_value))) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

/* ****************************************************************************
 功能描述  : 初始化传入的LUT BITMAP表
**************************************************************************** */
static inline hi_u32 oal_init_lut(hi_u8 *lut_index_table, hi_u8 bitmap_len)
{
    hi_u8 index;

    for (index = 0; index < bitmap_len; index++) {
        lut_index_table[index] = 0;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 在LUT index bitmap表中，删除一个LUT index (注:%可以作为优化项)
**************************************************************************** */
static inline hi_void oal_del_lut_index(hi_u8 *lut_index_table, hi_u8 idx)
{
    hi_u8 byte = idx / 8;       /* 8: 除以8 */
    hi_u8 bit_offset = idx % 8; /* 8: 对8取余 */

    lut_index_table[byte] &= ~(hi_u8)(1 << bit_offset);
}

/* ****************************************************************************
 功能描述  : 物理地址转化为虚拟地址
**************************************************************************** */
static inline hi_u32 *oal_get_virt_addr(hi_u32 *phy_addr)
{
    /* 空指针无需转化 */
    if (phy_addr == HI_NULL) {
        return phy_addr;
    }

    return (hi_u32 *)oal_phy_to_virt_addr(phy_addr);
}

/* ****************************************************************************
 功能描述  : 获取当前时间
**************************************************************************** */
static inline hi_u32 oal_get_curr_time_ms(hi_void)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 0,
    };

    gettimeofday(&tv, HI_NULL);
    return (hi_u32)(tv.tv_sec * 1000 + tv.tv_usec / 1000); /* 1000 时间单位转换 */
#elif defined(_PRE_HDF_LINUX)
    return 0;
#endif
}

static inline unsigned long oal_simple_strtoul(const hi_char *string, char **result, hi_u32 base)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return strtoul(string, result, base);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return simple_strtoul(string, result, base);
#endif
}

static inline hi_void oal_msleep(hi_u32 ul_usecs)
{
    msleep(ul_usecs);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_util.h */
