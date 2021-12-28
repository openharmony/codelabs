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

/* *****************************************************************************
  1 头文件包含
***************************************************************************** */
#include "oal_util.h"
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/etherdevice.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
1 全局变量定义
**************************************************************************** */
hi_u32 g_level_log = 1;

/* ****************************************************************************
2 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 字符串转MAC地址
 输入参数  : param: MAC地址字符串, 格式 xx:xx:xx:xx:xx:xx  分隔符支持':'与'-'
 输出参数  : mac_addr: 转换成16进制后的MAC地址
 返 回 值  :
**************************************************************************** */
WIFI_ROM_TEXT hi_void oal_strtoaddr(const hi_char *param, hi_u8 *mac_addr, hi_u8 mac_addr_len)
{
    hi_u8 index;

    /* 获取mac地址,16进制转换 */
    for (index = 0; index < 12; index++) { /* 循环12次 */
        if ((*param == ':') || (*param == '-')) {
            param++;
            if (index != 0) {
                index--;
            }
            continue;
        }
        if ((index / 2) >= mac_addr_len) { /* 除2 以找到正确的MAC地址 */
            break;                         /* 防止mac_addr 数组越界 */
        }
        mac_addr[index / 2] = /* 除2 以找到正确的MAC地址 */
            (hi_u8)(mac_addr[index / 2] * 16 * (index % 2) + oal_strtohex(param)); /* 除2 乘16以找到正确的MAC地址 */
        param++;
    }
}

/* ****************************************************************************
 功能描述  : 找到1字节右数第一个是1的位数
 输入参数  : pbyte: 要查找的字节
 输出参数  : 无
 返 回 值  : 右数第一个是1的位数
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 oal_bit_find_first_bit_four_byte(hi_u32 word)
{
    hi_u8 ret = 0;

    if (word == 0) {
        return ret;
    }

    if (!(word & 0xffff)) {
        word >>= 16; /* 右移16bit */
        ret += 16;   /* bit数加16 */
    }

    if (!(word & 0xff)) {
        word >>= 8; /* 右移8bit */
        ret += 8;   /* bit数加8 */
    }

    if (!(word & 0xf)) {
        word >>= 4; /* 右移4bit */
        ret += 4;   /* bit数加4 */
    }

    if (!(word & 0x3)) {
        word >>= 2; /* 右移2bit */
        ret += 2;   /* bit数加2 */
    }

    if (!(word & 1)) {
        ret += 1;
    }

    return ret;
}

/* ****************************************************************************
 功能描述  : 从LUT index bitmap表中，获取一个没有被使用的索引，没有找到的话，
             返回不可用的索引标识(非关键路径，未进行优化，有时间可以优化)
**************************************************************************** */
WIFI_ROM_TEXT hi_u8 oal_get_lut_index(hi_u8 *lut_index_table, hi_u8 bitmap_len, hi_u16 max_lut_size)
{
    hi_u8       byte;
    hi_u8       bit_idx;
    hi_u8       temp;
    hi_u16      index;

    for (byte = 0; byte < bitmap_len; byte++) {
        temp = lut_index_table[byte];

        if (temp == 0xFF) {
            continue;
        }

        for (bit_idx = 0; bit_idx < 8; bit_idx++) { /* 8 bit循环处理 */
            if ((temp & (1 << bit_idx)) != 0) {
                continue;
            }

            index = (byte * 8 + bit_idx); /* 乘8转成bit index */

            if (index < max_lut_size) {
                lut_index_table[byte] |= (hi_u8)(1 << bit_idx);

                return (hi_u8)index;
            } else {
                return (hi_u8)max_lut_size;
            }
        }
    }

    return (hi_u8)max_lut_size;
}

/* ****************************************************************************
 功能描述  : RSSI低通滤波，注RSSI一定是小于0的数
 输入参数  : c_old, 老的RSSI；c_new，新的RSSI
 输出参数  : 滤波后的RSSI
**************************************************************************** */
WIFI_ROM_TEXT hi_s8 wlan_rssi_lpf(hi_s8 old, hi_s8 new)
{
    hi_u8   oldval;
    hi_u8   newval;
    hi_u16  us_sum;

    /* 如果c_new是正数或0，则说明该RSSI有问题，不需要往下计算 */
    if (new >= 0) {
        return old;
    }

    /* 如果是第一次，则直接返回新的RSSI */
    if (old == WLAN_RSSI_DUMMY_MARKER) {
        return new;
    }

    /* 先获取绝对值，变成正数 */
    oldval = (hi_u8)oal_abs(old);
    newval = (hi_u8)oal_abs(new);

    /* 公式: (uc_old x 7/8 + uc_new x 1/8) */
    us_sum = (((oldval) << 3) + (newval) - (oldval));
    newval = (us_sum >> 3) & 0xFF;

    /* 返回相反数 */
    return -(newval & WLAN_RSSI_DUMMY_MARKER);
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 oal_atoi(const hi_char *c_string)
{
    hi_s32 l_ret = 0;
    hi_s32 flag = 0;

    for (;; c_string++) {
        switch (*c_string) {
            case '0' ... '9':
                l_ret = 10 * l_ret + (*c_string - '0'); /* 10:十进制数 */
                break;
            case '-':
                flag = 1;
                break;
            case ' ':
                continue;
            default:
                return ((flag == 0) ? l_ret : (-l_ret));
        }
    }
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_void oal_random_ether_addr(hi_u8 *mac_addr, hi_u8 mac_addr_len)
{
    struct timeval tv1;
    struct timeval tv2;

    hi_unref_param(mac_addr_len);
    /* 获取随机种子 */
    gettimeofday(&tv1, NULL);

    /* 防止秒级种子为0 */
    tv1.tv_sec += 2; /* 加2 */

    tv2.tv_sec = (hi_u32)((hi_u32)((hi_u64)tv1.tv_sec * tv1.tv_sec) * (hi_u64)tv1.tv_usec);
    tv2.tv_usec = (hi_u32)((hi_u32)((hi_u64)tv1.tv_sec * tv1.tv_usec) * (hi_u64)tv1.tv_usec);

    /* 生成随机的mac地址 */
    mac_addr[0] = ((hi_u32)tv2.tv_sec & 0xff) & 0xfe;
    mac_addr[1] = (hi_u32)tv2.tv_usec & 0xff;
    mac_addr[2] = ((hi_u32)tv2.tv_sec & 0xff0) >> 4;   /* mac_addr[2]右移4 bit */
    mac_addr[3] = ((hi_u32)tv2.tv_usec & 0xff0) >> 4;  /* mac_addr[3]右移4 bit */
    mac_addr[4] = ((hi_u32)tv2.tv_sec & 0xff00) >> 8;  /* mac_addr[4]右移8 bit */
    mac_addr[5] = ((hi_u32)tv2.tv_usec & 0xff00) >> 8; /* mac_addr[5]右移8 bit */
}
#else
hi_void oal_random_ether_addr(hi_u8 *mac_addr, hi_u8 mac_addr_len)
{
    hi_unref_param(mac_addr_len);
    random_ether_addr(mac_addr);
}
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */

/* ****************************************************************************
 功能描述  : 打印对应的内存值
**************************************************************************** */
hi_void oal_print_hex_dump(const hi_u8 *addr, hi_s32 len, hi_s32 group_size, hi_char *pre_str)
{
#ifdef CONFIG_PRINTK
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hi_unref_param(group_size);
    printk(KERN_DEBUG "buf %p,len:%d\n", addr, len);
    print_hex_dump(KERN_DEBUG, pre_str, DUMP_PREFIX_ADDRESS, 16, 1, /* 16 */
        addr, len, true);
    printk(KERN_DEBUG "\n");
#else
    hi_unref_param(addr);
    hi_unref_param(group_size);
    hi_unref_param(pre_str);
#endif
#endif
    hi_diag_log_msg_i0(0, "---start--\n");
    hi_s32 i = 0;
    for (i = 0; i < len; i++) {
        hi_diag_log_msg_i2(0, "netbuf[%d]=%02x\n", (hi_u32)i, addr[i]);
    }
    hi_diag_log_msg_i0(0, "---end---\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
