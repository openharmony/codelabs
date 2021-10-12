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

/* ****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "plat_firmware.h"
#include "oal_file.h"
#include "oal_sdio_host_if.h"
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_pm.h"
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "plat_sdio.h"
#include "los_event.h"
#include "los_typedef.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "los_exc.h"
#include "oal_util.h"
#include "oal_time.h"
#ifdef _PRE_HI113X_FS_DISABLE
#include "plat_wifi_cfg.h"
#ifndef _PRE_WLAN_FEATURE_MFG_FW
#include "plat_rw.h"
#else
#include "plat_rw_mfg.h"
#endif
#endif
#endif
#include "oal_channel_host_if.h"
#include "wal_customize.h"
#include "wal_hipriv.h"
#include "oam_ext_if.h"
#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
#include "data_process.h"
#include "plat_data_backup.h"
#endif

/* ****************************************************************************
  2 全局变量
**************************************************************************** */
#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
extern unsigned int g_ul_wlan_resume_wifi_init_flag;
#endif

#ifdef _PRE_HI113X_FS_DISABLE
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_u8 g_firmware_array_wifi_cfg_c01[] = {
    #include "c01/plat_wifi_cfg.h"
};

static hi_u8 g_firmware_array_wifi_cfg_c02[] = {
    #include "c02/plat_wifi_cfg.h"
};

static hi_u8 g_firmware_array_rw_bin_c01[] = {
    #include "c01/plat_rw.h"
};

static hi_u8 g_firmware_array_rw_bin_c02[] = {
    #include "c02/plat_rw.h"
};
#endif

DECLARE_FIRMWARE_FILE(wifi_cfg_c01);
DECLARE_FIRMWARE_FILE(wifi_cfg_c02);
DECLARE_FIRMWARE_FILE(rw_bin_c01);
DECLARE_FIRMWARE_FILE(rw_bin_c02);

static firmware_file_stru *g_st_wifi_cfg[SOFT_VER_BUTT] = {
    &firmware_file_wifi_cfg_c01,
    &firmware_file_wifi_cfg_c02
};

static firmware_file_stru *g_st_rw_bin[SOFT_VER_BUTT] = {
    &firmware_file_rw_bin_c01,
    &firmware_file_rw_bin_c02
};
#endif

#define WIFI_FIRMWARE_FILE_BIN "/vendor/firmware/hisilicon/hi1131h_demo_non_rom.bin"

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define WIFI_CFG_C01_PATH   "/vendor/firmware/hi3881/wifi_cfg"
#define WIFI_CFG_C02_PATH   HI_NULL
#define RAM_CHECK_CFG_PATH  HI_NULL
#define STORE_WIFI_MEM      HI_NULL

#define FILE_COUNT_PER_SEND             1
#define MIN_FIRMWARE_FILE_TX_BUF_LEN    4096
#define MAX_FIRMWARE_FILE_TX_BUF_LEN    (4*1024*1024)  /* 大小4M */

#define DEVICE_EFUSE_ADDR               0x50000764
#define DEVICE_EFUSE_LENGTH             16

#define CFG_CMD_NUM_MAX                 10    /* 支持配置的最大命令（参数）个数 */

/* ****************************************************************************
  3 全局变量定义
**************************************************************************** */
hi_u8 *g_auc_cfg_path[SOFT_VER_BUTT] = {
    (hi_u8 *)WIFI_CFG_C01_PATH,
    WIFI_CFG_C02_PATH,
};

/* 存储cfg文件信息，解析cfg文件时赋值，加载的时候使用该变量 */
firmware_globals_struct g_st_cfg_info;
cfg_cmd_struct g_cus_cfg_cmd[CFG_CMD_NUM_MAX]; /* 存储每个cfg文件的产测校准命令 */
hi_u32 g_ul_jump_cmd_result = CMD_JUMP_EXEC_RESULT_SUCC;
efuse_info_stru g_st_efuse_info = {
    .soft_ver = SOFT_VER_CO1,
    .mac_h = 0x0,
    .mac_m = 0x0,
    .mac_l = 0x0,
};

/* ****************************************************************************
  4 函数实现
**************************************************************************** */
extern hi_u32 usb_max_req_size(void);
static void firmware_mem_free(firmware_mem_stru *firmware_mem);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
extern int usleep(unsigned useconds);
extern int32_t plat_usb_init(void);
extern void plat_usb_destroy(void);
#endif

/* ****************************************************************************
 功能描述  : host接收device发来的消息
 输入参数  : data: 接收消息的buffer
             len : 接收buffer的长度
 输出参数  : 无
 返 回 值  : -1表示失败，否则返回实际接收的长度
**************************************************************************** */
static hi_s32 firmware_read_msg(hi_u8 *data, hi_s32 len)
{
    hi_s32 l_len;

    if (oal_unlikely((data == HI_NULL))) {
        oam_error_log0(0, 0, "data is HI_NULL\n ");
        return -OAL_EFAIL;
    }

    l_len = oal_bus_patch_readsb(data, len, READ_MEG_TIMEOUT);

    return l_len;
}

static hi_s32 firmware_read_msg_timeout(hi_u8 *data, hi_s32 len, hi_u32 timeout)
{
    hi_s32 l_len;

    if (oal_unlikely((data == HI_NULL))) {
        oam_error_log0(0, 0, "data is HI_NULL\n ");
        return -OAL_EFAIL;
    }

    l_len = oal_bus_patch_readsb(data, len, timeout);

    return l_len;
}

/* ****************************************************************************
 功能描述  : host往device发送消息
 输入参数  : data: 发送buffer
             len : 发送数据的长度
 输出参数  : 无
 返 回 值  : -1表示失败，否则返回实际发送的长度
**************************************************************************** */
static hi_s32 firmware_send_msg(hi_u8 *data, hi_s32 len)
{
    hi_s32 l_ret;

#ifdef HW_DEBUG
    print_hex_dump_bytes("firmware_send_msg :", DUMP_PREFIX_ADDRESS, data, (len < 128 ? len : 128)); /* len 128 */
#endif

    l_ret = oal_bus_patch_writesb(data, len);
    return l_ret;
}

static firmware_mem_stru *firmware_mem_request(void)
{
    firmware_mem_stru *firmware_mem = oal_kzalloc(sizeof(firmware_mem_stru), OAL_GFP_KERNEL);
    if (firmware_mem == HI_NULL) {
        oam_error_log0(0, 0, "g_st_firmware_mem KMALLOC failed\n");
        goto nomem;
    }

#if (_PRE_FEATURE_USB == _PRE_FEATURE_CHANNEL_TYPE)
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (plat_usb_init()) {
        oam_warning_log0(0, 0, "plat_usb_init failed\n");
        goto nomem;
    }
#endif
    firmware_mem->ul_data_buf_len = usb_max_req_size();
#elif (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    firmware_mem->ul_data_buf_len = oal_sdio_func_max_req_size(oal_get_bus_default_handler());
    oam_warning_log1(0, 0, "sdio max transmit size is [%d]\n", firmware_mem->ul_data_buf_len);
    if (firmware_mem->ul_data_buf_len < HISDIO_BLOCK_SIZE) {
        oam_warning_log1(0, 0, "sdio max transmit size [%d] is error!\n", firmware_mem->ul_data_buf_len);
        goto nomem;
    }
#endif

    do {
        firmware_mem->puc_data_buf = (hi_u8 *)OS_KMALLOC_GFP(firmware_mem->ul_data_buf_len);
        if (firmware_mem->puc_data_buf == HI_NULL) {
            oam_warning_log1(0, 0, "malloc mem len [%d] fail, continue to try in a smaller size\n",
                firmware_mem->ul_data_buf_len);
            firmware_mem->ul_data_buf_len = firmware_mem->ul_data_buf_len >> 1;
        }
    } while ((firmware_mem->puc_data_buf == HI_NULL) &&
        (firmware_mem->ul_data_buf_len >= MIN_FIRMWARE_FILE_TX_BUF_LEN) &&
        (firmware_mem->ul_data_buf_len <= MAX_FIRMWARE_FILE_TX_BUF_LEN));

    if (firmware_mem->puc_data_buf == HI_NULL) {
        oam_info_log0(0, 0, "puc_data_buf KMALLOC failed\n");
        goto nomem;
    }

    firmware_mem->puc_recv_cmd_buff = (hi_u8 *)OS_KMALLOC_GFP(CMD_BUFF_LEN);
    if (firmware_mem->puc_recv_cmd_buff == HI_NULL) {
        oam_info_log0(0, 0, "puc_recv_cmd_buff KMALLOC failed\n");
        goto nomem;
    }

    firmware_mem->puc_send_cmd_buff = (hi_u8 *)OS_KMALLOC_GFP(CMD_BUFF_LEN);
    if (firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_info_log0(0, 0, "puc_recv_cmd_buff KMALLOC failed\n");
        goto nomem;
    }

    return firmware_mem;

nomem:
    firmware_mem_free(firmware_mem);
    return HI_NULL;
}

static void firmware_mem_free(firmware_mem_stru *firmware_mem)
{
    if (firmware_mem == HI_NULL) {
        oam_error_log0(0, 0, "g_firmware_mem_mutex is null\n");
        return;
    }
    if (firmware_mem->puc_send_cmd_buff != HI_NULL) {
        oal_free(firmware_mem->puc_send_cmd_buff);
    }
    if (firmware_mem->puc_recv_cmd_buff != HI_NULL) {
        oal_free(firmware_mem->puc_recv_cmd_buff);
    }
    if (firmware_mem->puc_data_buf != HI_NULL) {
        oal_free(firmware_mem->puc_data_buf);
    }
    oal_free(firmware_mem);
    firmware_mem = HI_NULL;
#if (_PRE_FEATURE_USB == _PRE_FEATURE_CHANNEL_TYPE) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    plat_usb_destroy();
#endif
}

/* ****************************************************************************
 功能描述  : 接收host期望device正确返回的内容
 输入参数  : expect: 期望device正确返回的内容
 输出参数  : 无
 返 回 值  : 0表示成功，-1表示失败
**************************************************************************** */
static hi_s32 recv_expect_result(const hi_u8 *expect, const firmware_mem_stru *firmware_mem)
{
    hi_s32 l_len;
    hi_s32 i;
    if (expect == HI_NULL) {
        oam_error_log0(0, 0, "recv_expect_result:expect = HI_NULL \n");
        return -OAL_EFAIL;
    }

    if (!strlen((const hi_char *)expect)) {
        oam_info_log0(0, 0, "not wait device to respond!\n");
        return HI_SUCCESS;
    }
    if (firmware_mem == HI_NULL || firmware_mem->puc_recv_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "puc_recv_cmd_buff = HI_NULL \n");
        return -OAL_EFAIL;
    }
    memset_s(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN);
    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        l_len = firmware_read_msg(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN);
        if (l_len < 0) {
            oam_error_log0(0, 0, "recv result fail\n");
            return -OAL_EFAIL;
        }
        if (!memcmp(firmware_mem->puc_recv_cmd_buff, expect, strlen((const hi_char *)expect))) {
            return HI_SUCCESS;
        } else {
            oam_error_log2(0, 0, " error result[%s], expect [%s], read result again",
                (uintptr_t)firmware_mem->puc_recv_cmd_buff, (uintptr_t)expect);
        }
    }

    return -OAL_EFAIL;
}

static hi_s32 recv_expect_result_timeout(const hi_u8 *expect, const firmware_mem_stru *firmware_mem, hi_u32 timeout)
{
    hi_s32 l_len;

    if (expect == HI_NULL) {
        oam_error_log0(0, 0, "recv_expect_result_timeout:expect = HI_NULL \n");
        return -OAL_EFAIL;
    }

    if (!strlen((const hi_char *)expect)) {
        oam_info_log0(0, 0, "not wait device to respond!\n");
        return HI_SUCCESS;
    }

    memset_s(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN);
    l_len = firmware_read_msg_timeout(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, timeout);
    if (l_len < 0) {
        oam_error_log0(0, 0, "recv result fail\n");
        return -OAL_EFAIL;
    }

    if (!memcmp(firmware_mem->puc_recv_cmd_buff, expect, strlen((const hi_char *)expect))) {
        oam_info_log1(0, 0, " send HI_SUCCESS, expect [%s] ok\n", (uintptr_t)expect);
        return HI_SUCCESS;
    } else {
        oam_error_log2(0, 0, " error result[%s], expect [%s], read result again\n", firmware_mem->puc_recv_cmd_buff,
            expect);
    }
    return -OAL_EFAIL;
}

/* ****************************************************************************
 功能描述  : host向device发送消息并等待device返回消息
 输入参数  : data  : 发送buffer
             len   : 发送内容的长度
             expect: 期望device回复的内容
 输出参数  : 无
 返 回 值  : -1表示失败，0表示成功
**************************************************************************** */
static hi_s32 msg_send_and_recv_except(hi_u8 *data, hi_s32 len, const hi_u8 *expect,
    const firmware_mem_stru *firmware_mem)
{
    hi_s32 i;
    hi_s32 l_ret;

    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        l_ret = firmware_send_msg(data, len);
        if (l_ret < 0) {
            continue;
        }

        l_ret = recv_expect_result(expect, firmware_mem);
        if (l_ret == 0) {
            return HI_SUCCESS;
        }
    }
    return -OAL_EFAIL;
}

/* ****************************************************************************
 功能描述  : 解析cfg文件，将解析的结果保存在g_st_cfg_info全局变量中
 输入参数  : puc_cfg_info_buf: 保存有cfg文件内容的buffer
             ul_index        : 保存解析结果的数组索引值
 输出参数  : 无
 返 回 值  : NULL表示分配内存失败，否则返回指向保存解析cfg文件命令数组的首地址
**************************************************************************** */
static void *malloc_cmd_buf(hi_u8 *puc_cfg_info_buf, hi_u32 ul_index)
{
    hi_u32           l_len;
    hi_u8          *flag = HI_NULL;
    hi_u8          *p_buf = HI_NULL;

    if (puc_cfg_info_buf == HI_NULL) {
        oam_error_log0(0, 0, "malloc_cmd_buf: buf is HI_NULL!\n");
        return HI_NULL;
    }

    /* 统计命令个数 */
    flag = puc_cfg_info_buf;
    g_st_cfg_info.al_count[ul_index] = 0;
    while (flag != HI_NULL) {
        /* 一个正确的命令行结束符为 ; */
        flag = (hi_u8 *)strchr((const hi_char *)flag, CMD_LINE_SIGN);
        if (flag == HI_NULL) {
            break;
        }
        g_st_cfg_info.al_count[ul_index]++;
        flag++;
    }
    oam_info_log2(0, 0, "cfg file cmd count: al_count[%d] = %d", ul_index, g_st_cfg_info.al_count[ul_index]);

    /* 申请存储命令空间 */
    l_len = ((g_st_cfg_info.al_count[ul_index]) + CFG_INFO_RESERVE_LEN) * sizeof(struct cmd_type_st);

    p_buf = OS_KMALLOC_GFP(l_len);
    if (p_buf == HI_NULL) {
        oam_error_log0(0, 0, "kmalloc cmd_type_st fail\n");
        return HI_NULL;
    }
    memset_s((void *)p_buf, l_len, 0, l_len);

    return p_buf;
}

/* ****************************************************************************
 功能描述  : 删除字符串两边多余的空格
 输入参数  : string: 原始字符串
             len   : 字符串的长度
 输出参数  : 无
 返 回 值  : 错误返回NULL，否则返回删除两边空格以后字符串的首地址
**************************************************************************** */
static hi_u8 *delete_space(hi_u8 *string, hi_s32 *len)
{
    int i;

    if ((string == HI_NULL) || (len == HI_NULL)) {
        return HI_NULL;
    }

    /* 删除尾部的空格 */
    for (i = *len - 1; i >= 0; i--) {
        if (string[i] != COMPART_KEYWORD) {
            break;
        }
        string[i] = '\0';
    }
    /* 出错 */
    if (i < 0) {
        oam_error_log0(0, 0, " string is Space bar\n");
        return HI_NULL;
    }
    /* 在for语句中减去1，这里加上1 */
    *len = i + 1;

    /* 删除头部的空格 */
    for (i = 0; i < *len; i++) {
        if (string[i] != COMPART_KEYWORD) {
            /* 减去空格的个数 */
            *len = *len - i;
            return &string[i];
        }
    }

    return HI_NULL;
}

/* ************************************************************************************
 功能描述  : 打开文件，保存read mem读上来的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 返回打开文件的描述符
************************************************************************************ */
static oal_file_stru *open_file_to_readm(hi_u8 *name)
{
    oal_file_stru *fp = HI_NULL;
    hi_u8 *file_name = HI_NULL;

    if (OAL_WARN_ON(name == HI_NULL)) {
        file_name = (hi_u8 *)"/data/memdump/readm_wifi";
    } else {
        file_name = name;
    }
    fp = oal_file_open((const hi_char *)file_name, (OAL_O_CREAT | OAL_O_RDWR | OAL_O_TRUNC), 0);

    return fp;
}

/* ************************************************************************************
 功能描述  : 接收device发送上来的内存，保存到指定的文件中
 输入参数  : fp : 保存内存的文件指针
             len: 需要保存的内存的长度
 输出参数  : 无
 返 回 值  : -1表示失败，否则返回实际保存的内存的长度
************************************************************************************ */
static hi_s32 recv_device_mem(oal_file_stru *fp, hi_u8 *puc_data_buf, hi_s32 len)
{
    hi_s32 l_ret = 0;
    hi_u8 retry = 3;
    hi_s32 lenbuf = 0;

    if (IS_ERR_OR_NULL(fp)) {
        oam_error_log1(0, 0, "fp is error,fp = 0x%p\n", (uintptr_t)fp);
        return -OAL_EFAIL;
    }

    if (puc_data_buf == HI_NULL) {
        oam_error_log0(0, 0, "puc_data_buf is HI_NULL\n");
        return -OAL_EFAIL;
    }

    oam_info_log1(0, 0, "expect recv len is [%d]\n", len);

    while (len > lenbuf) {
        l_ret = firmware_read_msg(puc_data_buf + lenbuf, len - lenbuf);
        if (l_ret > 0) {
            lenbuf += l_ret;
        } else {
            retry--;
            lenbuf = 0;
            if (0 == retry) {
                l_ret = -OAL_EFAIL;
                oam_error_log0(0, 0, "time out\n");
                break;
            }
        }
    }

    if (len <= lenbuf) {
        oal_file_write(fp, (hi_char *)puc_data_buf, len);
    }

    return l_ret;
}

/* ************************************************************************************
 功能描述  : 发送命令读device版本号，并检查device上报的版本号和host的版本号是否匹配
 输入参数  : 无
 输出参数  : 无
 返 回 值  : -1表示失败，0表示成功
************************************************************************************ */
static hi_s32 check_version(const firmware_mem_stru *firmware_mem)
{
    hi_s32 l_ret;
    size_t l_len;
    hi_s32 i;

    if (firmware_mem == HI_NULL || firmware_mem->puc_recv_cmd_buff == HI_NULL ||
        firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "MEM IS HI_NULL \n");
        return -OAL_EFAIL;
    }

    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        if ((memset_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN) != EOK) ||
            (memcpy_s(firmware_mem->puc_send_cmd_buff, strlen(VER_CMD_KEYWORD), (hi_u8 *)VER_CMD_KEYWORD,
                strlen(VER_CMD_KEYWORD)) != EOK)) {
            continue;
        }
        l_len = strlen(VER_CMD_KEYWORD);

        firmware_mem->puc_send_cmd_buff[l_len] = COMPART_KEYWORD;
        l_len++;

        l_len = HISDIO_ALIGN_4_OR_BLK(l_len + 1);

        l_ret = firmware_send_msg(firmware_mem->puc_send_cmd_buff, l_len);
        if (l_ret < 0) {
            oam_warning_log1(0, 0, "send version fail![%d]\n", i);
            continue;
        }
        if (memset_s(g_st_cfg_info.auc_DevVersion, VERSION_LEN, 0, VERSION_LEN) != EOK ||
            memset_s(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN) != EOK) {
            continue;
        }

        l_ret = firmware_read_msg(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN);
        if (l_ret < 0) {
            oam_warning_log1(0, 0, "read version fail![%d]\n", i);
            continue;
        }
        if (memcpy_s(g_st_cfg_info.auc_DevVersion, VERSION_LEN, firmware_mem->puc_recv_cmd_buff, VERSION_LEN) != EOK) {
            continue;
        }
        if (!memcmp((hi_char *)g_st_cfg_info.auc_DevVersion, (hi_char *)g_st_cfg_info.auc_CfgVersion,
            strlen((const hi_char *)g_st_cfg_info.auc_CfgVersion))) {
            oam_warning_log2(0, 0, "HI_SUCCESS: Device Version=[%s], CfgVersion=[%s].",
                (uintptr_t)g_st_cfg_info.auc_DevVersion, (uintptr_t)g_st_cfg_info.auc_CfgVersion);
            return HI_SUCCESS;
        } else {
            oam_error_log2(0, 0, "ERROR version,Device Version=[%s], CfgVersion=[%s].", g_st_cfg_info.auc_DevVersion,
                g_st_cfg_info.auc_CfgVersion);
            return -OAL_EFAIL;
        }
    }

    return -OAL_EFAIL;
}

/* ****************************************************************************
 功能描述  : 处理number类型的命令，并发送到device
 输入参数  : key  : 命令的关键字
             val: 命令的参数
 输出参数  : 无
 返 回 值  : -1表示失败，非零表示成功
**************************************************************************** */
static hi_s32 number_type_cmd_send(hi_u8 *key, hi_u8 *val, const firmware_mem_stru *firmware_mem)
{
    hi_s32 l_ret;
    hi_u32 data_len;
    hi_u32 value_len;
    hi_u32 i;
    hi_u32 n;
#define RESERVED_BYTES 10

    if (firmware_mem == HI_NULL || firmware_mem->puc_recv_cmd_buff == HI_NULL ||
        firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "MEM IS HI_NULL \n");
        return -OAL_EFAIL;
    }

    if (CMD_BUFF_LEN < strlen((const hi_char *)key) + strlen((const hi_char *)val) + RESERVED_BYTES) {
        oam_error_log2(0, 0, "the cmd string must be error, key=%s, vlaue=%s \n", (uintptr_t)key, (uintptr_t)val);
        return -OAL_EFAIL;
    }

    value_len = strlen((hi_char *)val);

    if (memset_s(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN) != EOK ||
        memset_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN) != EOK) {
        return -OAL_EFAIL;
    }

    data_len = (hi_s32)strlen((const hi_char *)key);
    if (memcpy_s(firmware_mem->puc_send_cmd_buff, data_len, key, data_len) != EOK) {
        return -OAL_EFAIL;
    }

    firmware_mem->puc_send_cmd_buff[data_len] = COMPART_KEYWORD;
    data_len = data_len + 1;

    for (i = 0, n = 0; (i <= value_len) && (n < INT32_STR_LEN); i++) {
        if ((val[i] == ',') || (i == value_len)) {
            if (n == 0) {
                continue;
            }
            if (memcpy_s((hi_u8 *)&firmware_mem->puc_send_cmd_buff[data_len], n,
                firmware_mem->puc_recv_cmd_buff, n) != EOK) {
                return -OAL_EFAIL;
            }
            data_len = data_len + n;

            firmware_mem->puc_send_cmd_buff[data_len] = COMPART_KEYWORD;
            data_len = data_len + 1;

            memset_s(firmware_mem->puc_recv_cmd_buff, INT32_STR_LEN, 0, INT32_STR_LEN);
            n = 0;
        } else if (val[i] == COMPART_KEYWORD) {
            continue;
        } else {
            firmware_mem->puc_recv_cmd_buff[n] = val[i];
            n++;
        }
    }
    firmware_mem->puc_send_cmd_buff[data_len + 1] = '\0';
    l_ret = firmware_send_msg(firmware_mem->puc_send_cmd_buff, data_len);

    return l_ret;
}

/* ****************************************************************************
 功能描述  : 解析file命令参数
 输入参数  : string   : file命令的参数
             addr     : 发送的数据地址
             file_path: 发送文件的路径
 输出参数  : 无
 返 回 值  : -1表示失败，0表示成功
**************************************************************************** */
static hi_s32 parse_file_cmd(hi_u8 *string, unsigned long *addr, hi_char **file_path)
{
    hi_char *tmp = HI_NULL;
    hi_size_t count;
    hi_char *after = HI_NULL;
#define DECIMAL_ALGORITHM 10

    if (string == HI_NULL || addr == HI_NULL || file_path == HI_NULL) {
        oam_error_log0(0, 0, "param is error!\n");
        return -OAL_EFAIL;
    }

    /* 获得发送的文件的个数，此处必须为1，string字符串的格式必须是"1,0xXXXXX,file_path" */
    tmp = (hi_char *)string;
    hi_u32 cnt = 0;
    while ((cnt < DOWNLOAD_CMD_PARA_LEN) && (*tmp == COMPART_KEYWORD)) {
        tmp++;
        cnt++;
    }
    count = oal_simple_strtoul(tmp, HI_NULL, DECIMAL_ALGORITHM);
    if (count != FILE_COUNT_PER_SEND) {
        oam_error_log1(0, 0, "the count of send file must be 1, count = [%d]\n", count);
        return -OAL_EFAIL;
    }

    /* 让tmp指向地址的首字母 */
    tmp = strchr((const hi_char *)string, ',');
    if (tmp == HI_NULL) {
        oam_error_log0(0, 0, "param string is err!\n");
        return -OAL_EFAIL;
    } else {
        tmp++;
        while (((tmp - (hi_char *)string) < DOWNLOAD_CMD_PARA_LEN) && (*tmp == COMPART_KEYWORD)) {
            tmp++;
        }
    }

    *addr = oal_simple_strtoul((hi_char *)tmp, &after, 16); /* base 16 */
    oam_info_log1(0, 0, "file to send addr:[0x%lx]", *addr);

    /* "1,0xXXXX,file_path" */
    /*         ^          */
    /*       after        */
    after++;
    while (((after - (hi_char *)string) < DOWNLOAD_CMD_PARA_LEN) && (*after == COMPART_KEYWORD)) {
        after++;
    }

    *file_path = after;

    return HI_SUCCESS;
}


/* ****************************************************************************
 功能描述  : 当device处于bootloader时从DEVICE读取内存
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 小于0表示失败
**************************************************************************** */
static hi_s32 read_device_mem(const wifi_dump_mem_info_stru *mem_dump_info, oal_file_stru *fp,
    const firmware_mem_stru *firmware_mem)
{
    hi_s32 ret = 0;
    hi_u32 size = 0;
    hi_u32 remainder = mem_dump_info->size;

    if (firmware_mem == HI_NULL || firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "puc_send_cmd_buff = HI_NULL \n");
        return -OAL_EFAIL;
    }
    while (remainder > 0) {
        memset_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN);

        size = oal_min(remainder, firmware_mem->ul_data_buf_len);

        oam_info_log1(0, 0, "read mem cmd:[%s]\n", (uintptr_t)firmware_mem->puc_send_cmd_buff);
        ret = firmware_send_msg(firmware_mem->puc_send_cmd_buff,
            strlen((const hi_char *)firmware_mem->puc_send_cmd_buff));
        if (ret < 0) {
            oam_warning_log2(0, 0, "wifi mem dump fail, mem_addr is [0x%lx],ret=%d\n", mem_dump_info->mem_addr, ret);
            break;
        }

        ret = recv_device_mem(fp, firmware_mem->puc_data_buf, size);
        if (ret < 0) {
            oam_warning_log2(0, 0, "wifi mem dump fail, mem_addr is [0x%lx],ret=%d\n", mem_dump_info->mem_addr, ret);
            break;
        }

        remainder -= size;
    }

    return ret;
}

static hi_s32 read_mem(hi_u8 *key, const hi_u8 *val, const firmware_mem_stru *firmware_mem)
{
    hi_unref_param(key);
    hi_s32 l_ret;
    hi_size_t size;
    hi_char *flag = HI_NULL;
    oal_file_stru *fp = HI_NULL;
    struct wifi_dump_mem_info_stru read_memory;
    memset_s(&read_memory, sizeof(struct wifi_dump_mem_info_stru), 0, sizeof(struct wifi_dump_mem_info_stru));

    flag = strchr((const hi_char *)val, ',');
    if (flag == HI_NULL) {
        oam_error_log0(0, 0, "RECV LEN ERROR..\n");
        return -OAL_EFAIL;
    }
    if (firmware_mem == HI_NULL || firmware_mem->puc_data_buf == HI_NULL) {
        oam_error_log0(0, 0, "MEM IS HI_NULL \n");
        return -OAL_EFAIL;
    }

    flag++;
    oam_info_log1(0, 0, "recv len [%s]\n", flag);
    while (((flag - (hi_char *)key) < DOWNLOAD_CMD_LEN) && (*flag == COMPART_KEYWORD)) {
        flag++;
    }
    size = oal_simple_strtoul(flag, HI_NULL, 10); /* base 10: DEC */

    fp = open_file_to_readm(HI_NULL);
    if (IS_ERR_OR_NULL(fp)) {
        oam_error_log1(0, 0, "create file error,fp = 0x%p\n", (uintptr_t)fp);
        return -OAL_EFAIL;
    }

    read_memory.mem_addr = oal_simple_strtoul((const hi_char *)val, HI_NULL, 16); /* base 16 */
    read_memory.size = (hi_u32)size;
    l_ret = read_device_mem(&read_memory, fp, firmware_mem);

    oal_file_close(fp);

    return l_ret;
}

/* ****************************************************************************
 功能描述  : 执行number类型的命令
 输入参数  : key  : 命令的关键字
             val: 命令的参数
 输出参数  : 无
 返 回 值  : -1表示失败，0表示成功
**************************************************************************** */
static hi_s32 exec_number_type_cmd(hi_u8 *key, hi_u8 *val, firmware_mem_stru *firmware_mem)
{
    hi_s32 l_ret = -OAL_EFAIL;

    if (key == HI_NULL) {
        oam_error_log0(0, 0, "exec_number_type_cmd key null\n");
        return -OAL_EFAIL;
    }

    if (!memcmp(key, VER_CMD_KEYWORD, strlen(VER_CMD_KEYWORD))) {
        l_ret = check_version(firmware_mem);
        if (l_ret < 0) {
            oam_error_log1(0, 0, "check version FAIL [%d]\n", l_ret);
            return -OAL_EFAIL;
        }
    }

    if (!strcmp((hi_char *)key, WMEM_CMD_KEYWORD)) {
        l_ret = number_type_cmd_send(key, val, firmware_mem);
        if (l_ret < 0) {
            goto ret_err;
        }

        l_ret = recv_expect_result((const hi_u8 *)MSG_FROM_DEV_WRITEM_OK, firmware_mem);
        if (l_ret < 0) {
            oam_error_log0(0, 0, "recv expect result fail!\n");
            return l_ret;
        }
    } else if (!strcmp((hi_char *)key, CONFIG_CMD_KEYWORD)) {
        l_ret = number_type_cmd_send(key, val, firmware_mem);
        if (l_ret < 0) {
            goto ret_err;
        }

        l_ret = recv_expect_result((const hi_u8 *)MSG_FROM_DEV_CONFIG_OK, firmware_mem);
        if (l_ret < 0) {
            oam_print_err("recv expect result fail!\n");
            return l_ret;
        }
    } else if (!strcmp((hi_char *)key, JUMP_CMD_KEYWORD)) {
        g_ul_jump_cmd_result = CMD_JUMP_EXEC_RESULT_SUCC;
        l_ret = number_type_cmd_send(key, val, firmware_mem);
        if (l_ret < 0) {
            goto ret_err;
        }

        /* 100000ms timeout */
        l_ret = recv_expect_result_timeout((const hi_u8 *)MSG_FROM_DEV_JUMP_OK, firmware_mem, READ_MEG_JUMP_TIMEOUT);
        if (l_ret >= 0) {
            return l_ret;
        } else {
            g_ul_jump_cmd_result = CMD_JUMP_EXEC_RESULT_FAIL;
            return l_ret;
        }
    } else if (!strcmp((hi_char *)key, RMEM_CMD_KEYWORD)) {
        l_ret = read_mem(key, val, firmware_mem);
    }
    return l_ret;
ret_err:
    oam_error_log2(0, 0, "send key=[%s],value=[%s] fail\n", (uintptr_t)key, (uintptr_t)val);
    return l_ret;
}


/* ****************************************************************************
 功能描述  : 执行quit类型的命令
 输入参数  : 无
 输出参数  : 无
 返 回 值  : -1表示失败，0表示成功
**************************************************************************** */
static hi_s32 exec_quit_type_cmd(firmware_mem_stru *firmware_mem)
{
    hi_s32 l_ret;
    hi_u32 l_len;

    if (firmware_mem == HI_NULL || firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "puc_send_cmd_buff = HI_NULL \n");
        return -OAL_EFAIL;
    }

    if (memset_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, 0, 8) != EOK) { /* 8: 清8个长度 */
        return -OAL_EFAIL;
    } /* buffer len 8 */

    if (memcpy_s(firmware_mem->puc_send_cmd_buff, strlen(QUIT_CMD_KEYWORD), (hi_u8 *)QUIT_CMD_KEYWORD,
        strlen(QUIT_CMD_KEYWORD)) != EOK) {
        return -OAL_EFAIL;
    }
    l_len = strlen(QUIT_CMD_KEYWORD);

    firmware_mem->puc_send_cmd_buff[l_len] = COMPART_KEYWORD;
    l_len++;

    l_ret = msg_send_and_recv_except(firmware_mem->puc_send_cmd_buff, l_len, (const hi_u8 *)MSG_FROM_DEV_QUIT_OK,
        firmware_mem);

    return l_ret;
}

#ifndef _PRE_HI113X_FS_DISABLE
int g_fw_mode = FIRMWARE_BIN; /* 默认业务bin */
module_param(g_fw_mode, int, 0644);
/* ****************************************************************************
 功能描述  : 执行file类型的命令
 输入参数  : key  : 命令的关键字
             val: 命令的参数
 输出参数  : 无
 返 回 值  : -1表示失败，0表示成功
**************************************************************************** */
static hi_s32 exec_file_type_cmd(hi_u8 *key, hi_u8 *val, firmware_mem_stru *firmware_mem)
{
    hi_unref_param(key);
    unsigned long addr;
    hi_u32 addr_send;
    hi_char *path = HI_NULL;
    hi_s32 ret;
    hi_u32 file_len;
    hi_u32 per_send_len;
    hi_u32 send_count;
    hi_s32 rdlen;
    hi_u32 i;
    hi_u32 offset = 0;
    oal_file_stru *fp = HI_NULL;
    hi_s32 verify_stage = 0;

    if (firmware_mem == HI_NULL || firmware_mem->puc_send_cmd_buff == HI_NULL ||
        firmware_mem->puc_data_buf == HI_NULL) {
        oam_error_log0(0, 0, "exec_file_type_cmd:: mem is HI_NULL");
        return -OAL_EFAIL;
    }

    ret = parse_file_cmd(val, &addr, &path);
    if (ret < 0) {
        oam_error_log0(0, 0, "exec_file_type_cmd:: parse file cmd fail");
        return ret;
    }
    if (g_fw_mode == MFG_FIRMWARE_BIN) {
        path = "/vendor/firmware/hisilicon/hi3881_mfg_fw.bin";
    }
    fp = oal_file_open(path, (OAL_O_RDONLY), 0);
    if (IS_ERR_OR_NULL(fp)) {
        oam_error_log2(0, 0, "exec_file_type_cmd:: filp_open [%s] fail!!, fp=%p", path, fp);
        return -OAL_EFAIL;
    }
    set_under_mfg(g_fw_mode); /* 保存当前firmware模式 */
    /* 获取file文件大小 */
    file_len = oal_file_lseek(fp, 0, OAL_SEEK_END);

    /* 恢复fp->f_pos到文件开头 */
    oal_file_lseek(fp, 0, OAL_SEEK_SET);

    oam_info_log2(0, 0, "exec_file_type_cmd:: file len is [%d],firmware_mem->ul_data_buf_len=%d", file_len,
        firmware_mem->ul_data_buf_len);

    per_send_len = (firmware_mem->ul_data_buf_len > file_len) ? file_len : firmware_mem->ul_data_buf_len;
    if (per_send_len == 0) {
        oam_error_log0(0, 0, "per_send_len == 0");
        oal_file_close(fp);
        return -OAL_EFAIL;
    }
    send_count = (file_len + per_send_len - 1) / per_send_len;
    oam_info_log1(0, 0, "exec_file_type_cmd:: send_count=%d", send_count);

    for (i = 0; i < send_count; i++) {
        rdlen = oal_file_read(fp, firmware_mem->puc_data_buf, per_send_len);
        if (rdlen > 0) {
            oam_info_log2(0, 0, "exec_file_type_cmd:: len of kernel_read is [%d], i=%d", rdlen, i);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
            OAL_FILE_POS(fp) += rdlen;
#endif
        } else {
            oam_error_log2(0, 0, "exec_file_type_cmd:: len of kernel_read is error! ret=[%d], i=%d", rdlen, i);
            oal_file_close(fp);
            return -OAL_EFAIL;
        }

        addr_send = (hi_u32)(addr + offset);
        oam_info_log2(0, 0, "exec_file_type_cmd:: send addr is [0x%x], i=%d", addr_send, i);

        if (offset == 0 && offset + rdlen == file_len) {
            verify_stage = 3; /* 3:start and end. */
        } else if (offset == 0) {
            verify_stage = 0; /* start trans. */
        } else if (offset + rdlen == file_len) {
            verify_stage = 2; /* 2:end trans. */
        } else {
            verify_stage = 1; /* transferring. */
        }

        if (snprintf_s((hi_char *)firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, CMD_BUFF_LEN - 1,
            "%s%c0x%x%c0x%x%c%d%c", FILES_CMD_KEYWORD, COMPART_KEYWORD, addr_send, COMPART_KEYWORD, rdlen,
            COMPART_KEYWORD, verify_stage, COMPART_KEYWORD) == -1) {
            return -OAL_EFAIL;
        }

        /* 发送地址 */
        oam_info_log1(0, 0, "exec_file_type_cmd:: send file addr cmd is [%s]", firmware_mem->puc_send_cmd_buff);
        ret = msg_send_and_recv_except(firmware_mem->puc_send_cmd_buff,
            strlen((hi_char *)firmware_mem->puc_send_cmd_buff), (const hi_u8 *)MSG_FROM_DEV_READY_OK, firmware_mem);
        if (ret < 0) {
            oam_error_log1(0, 0, "exec_file_type_cmd:: SEND [%s] error", (uintptr_t)firmware_mem->puc_send_cmd_buff);
            oal_file_close(fp);
            return -OAL_EFAIL;
        }
        /* Wait at least 5 ms */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        usleep(FILE_CMD_WAIT_TIME_MIN);
#endif
        /* 发送文件内容 */
        ret = msg_send_and_recv_except(firmware_mem->puc_data_buf, rdlen, (const hi_u8 *)MSG_FROM_DEV_FILES_OK,
            firmware_mem);
        if (ret < 0) {
            oam_error_log0(0, 0, "exec_file_type_cmd:: send data fail");
            oal_file_close(fp);
            return -OAL_EFAIL;
        }

        offset += rdlen;
    }
    oal_file_close(fp);
    /* 发送的长度要和文件的长度一致 */
    if (offset != file_len) {
        oam_error_log2(0, 0, "exec_file_type_cmd:: send len[%d] is different with file_len[%d]", offset, file_len);
        return -OAL_EFAIL;
    }

    return HI_SUCCESS;
}
#else
static hi_s32 exec_file_type_cmd(hi_u8 *key, hi_u8 *val, firmware_mem_stru *firmware_mem)
{
    hi_unref_param(key);
    unsigned long addr;
    hi_u32 addr_send;
    hi_char *path = HI_NULL;
    hi_s32 ret;
    hi_u32 file_len;
    hi_u32 per_send_len;
    hi_s32 rdlen;
    hi_u32 offset = 0;
    hi_u32 file_len_count;
    hi_u32 ul_soft_ver;
    hi_s32 verify_stage = 0;

    if (firmware_mem == HI_NULL || firmware_mem->puc_data_buf == HI_NULL ||
        firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "mem is HI_NULL \n");
        return -OAL_EFAIL;
    }

    ul_soft_ver = get_device_soft_version();
    if (ul_soft_ver >= SOFT_VER_BUTT) {
        oam_error_log0(0, 0, "device soft version is invalid!\n");
        return -OAL_EFAIL;
    }

    ret = parse_file_cmd(val, &addr, &path);
    if (ret < 0) {
        oam_error_log0(0, 0, "parse file cmd fail!\n");
        return ret;
    }

    oam_info_log1(0, 0, "download firmware:%s", path);

    file_len = g_st_rw_bin[ul_soft_ver]->len;
    file_len_count = file_len;

    oam_info_log2(0, 0, "file len is [%d],firmware_mem->ulDataBufLen=%d", file_len, firmware_mem->ul_data_buf_len);

    per_send_len = (firmware_mem->ul_data_buf_len > file_len) ? file_len : firmware_mem->ul_data_buf_len;

    while (file_len_count > 0) {
        rdlen = per_send_len < file_len_count ? per_send_len : file_len_count;
        if (memcpy_s(firmware_mem->puc_data_buf, rdlen, g_st_rw_bin[ul_soft_ver]->addr + offset, rdlen) != EOK) {
            return -OAL_EFAIL;
        };

        addr_send = (hi_u32)(addr + offset);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        if (offset == 0 && offset + rdlen == file_len) {
            verify_stage = 3; /* start and end, verify_stage 3. */
        } else if (offset == 0) {
            verify_stage = 0; /* start trans. */
        } else if (offset + rdlen == file_len) {
            verify_stage = 2; /* end trans, verify_stage 2. */
        } else {
            verify_stage = 1; /* transferring. */
        }

        ret = snprintf_s((hi_char *)firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, CMD_BUFF_LEN - 1,
            "%s%c0x%x%c0x%x%c%d%c", FILES_CMD_KEYWORD, COMPART_KEYWORD, addr_send, COMPART_KEYWORD, rdlen,
            COMPART_KEYWORD, verify_stage, COMPART_KEYWORD);
        if (ret < 0) {
            return -OAL_EFAIL;
        }
#endif
        /* 发送地址 */
        oam_info_log1(0, 0, "send file addr cmd is [%s]", firmware_mem->puc_send_cmd_buff);
        ret = msg_send_and_recv_except(firmware_mem->puc_send_cmd_buff,
            strlen((const hi_char *)firmware_mem->puc_send_cmd_buff), (const hi_u8 *)MSG_FROM_DEV_READY_OK,
            firmware_mem);
        if (ret < 0) {
            oam_error_log1(0, 0, "SEND [%s] error\n", firmware_mem->puc_send_cmd_buff);
            return -OAL_EFAIL;
        }
        /* Wait at least 5 ms */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        usleep(FILE_CMD_WAIT_TIME_MIN);
#endif
        /* 发送文件内容 */
        ret = msg_send_and_recv_except(firmware_mem->puc_data_buf, rdlen, (const hi_u8 *)MSG_FROM_DEV_FILES_OK,
            firmware_mem);
        if (ret < 0) {
            oam_error_log0(0, 0, " send data fail");
            return -OAL_EFAIL;
        }

        offset += rdlen;
        file_len_count -= rdlen;
    }

    /* 发送的长度要和文件的长度一致 */
    if (offset != file_len) {
        oam_error_log2(0, 0, "file send len is err! send len is [%d], file len is [%d]", offset, file_len);
        return -OAL_EFAIL;
    }

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 执行firmware download的命令
 输入参数  : cmd_type: 加载命令的类型
             cmd_name: 命令的关键字
             cmd_para: 命令的参数
 输出参数  : 无
 返 回 值  : -1表示失败，非负数表示成功
**************************************************************************** */
static hi_s32 execute_download_cmd(hi_s32 cmd_type, hi_u8 *cmd_name, hi_u8 *cmd_para, firmware_mem_stru *firmware_mem)
{
    hi_s32 l_ret;

    switch (cmd_type) {
        case FILE_TYPE_CMD:
            l_ret = exec_file_type_cmd(cmd_name, cmd_para, firmware_mem);
            break;
        case NUM_TYPE_CMD:
            l_ret = exec_number_type_cmd(cmd_name, cmd_para, firmware_mem);
            break;
        case QUIT_TYPE_CMD:
            l_ret = exec_quit_type_cmd(firmware_mem);
            break;
        default:
            oam_error_log1(0, 0, "command type error[%d]", cmd_type);
            l_ret = -OAL_EFAIL;
            break;
    }

    return l_ret;
}

hi_s32 firmware_write_cfg(hi_u8 *key, hi_u8 *new_val, hi_u8 len)
{
    oal_file_stru    *fp;
    hi_s32           idx;
    hi_s64           l_ret;
    hi_u8            *cfg_patch = (hi_u8 *)WIFI_CFG_C01_PATH;

    /* 查找参数名是否缓存 */
    for (idx = 0; idx < CFG_CMD_NUM_MAX; ++idx) {
        if (strcmp((const hi_char *)key, (const hi_char *)g_cus_cfg_cmd[idx].cmd_name) == 0) {
            break;
        }
    }
    if (idx == CFG_CMD_NUM_MAX) {
        printk("cfg [%s] to write not found\n", key);
        return -OAL_EFAIL;
    }
    /* 校验新值的长度        */
    if (len > g_cus_cfg_cmd[idx].val_len - 3) { /* 3:预留的 ‘;’和“\r\n”的大小 */
        printk("new val [%s] length exceeds old val [%s]\n", new_val, g_cus_cfg_cmd[idx].cmd_para);
        return -OAL_EFAIL;
    }
    fp = oal_file_open((const hi_char *)cfg_patch, (OAL_O_RDWR), 0);
    if (IS_ERR_OR_NULL(fp)) {
        printk("open file %s fail, fp=%p\n", cfg_patch, fp);
        return -OAL_EFAIL;
    }
    /* 定位到目标位置 */
    l_ret = oal_file_lseek(fp, g_cus_cfg_cmd[idx].val_offset, OAL_SEEK_SET);
    hi_u8 *buf = (hi_u8 *)OS_KMALLOC_GFP(g_cus_cfg_cmd[idx].val_len);
    if (buf == HI_NULL) {
        printk("firmware_write_cfg:alloc buf failed\n");
        oal_file_close(fp);
        return -OAL_EFAIL;
    }
    if (memset_s(buf, g_cus_cfg_cmd[idx].val_len, ' ', g_cus_cfg_cmd[idx].val_len) != EOK) {
        return -OAL_EFAIL;
    }

    if (memcpy_s(buf, g_cus_cfg_cmd[idx].val_len, new_val, len + 1) != EOK) {
        oam_warning_log0(0, 0, "firmware_write_cfg:: buf memcpy_s fail.\n");
    }
    if (len < g_cus_cfg_cmd[idx].val_len - 2) { /* 2:回车换行预留 */
        buf[len] = ';';                         /* 重置结束符 */
    }
    buf[g_cus_cfg_cmd[idx].val_len - 2] = '\r'; /* 增加回车换行.2:回退值 */
    buf[g_cus_cfg_cmd[idx].val_len - 1] = '\n';
    l_ret = oal_file_write(fp, (hi_char *)buf, g_cus_cfg_cmd[idx].val_len);
    oal_file_close(fp);

    if (l_ret == g_cus_cfg_cmd[idx].val_len) { /* 返回值为写入字节数表示成功 */
        /* 更新缓存 */
        if (memcpy_s(g_cus_cfg_cmd[idx].cmd_para, g_cus_cfg_cmd[idx].val_len, buf, g_cus_cfg_cmd[idx].val_len) != EOK) {
            oam_warning_log0(0, 0, "firmware_write_cfg:: g_cus_cfg_cmd memcpy_s fail.\n");
        }
        oal_free(buf);
        return HI_SUCCESS;
    }
    oal_free(buf);
    return HI_FAIL;
}

/* ****************************************************************************
 功能描述  : 读取cfg文件的内容，放到驱动动态分配的buffer中
 输入参数  : cfg_patch    : cfg文件的路径
             puc_read_buffer : 保存cfg文件内容的buffer
 输出参数  : 无
 返 回 值  : 0表示成功，-1表示失败
**************************************************************************** */
hi_s32 firmware_read_cfg(const hi_u8 *cfg_patch, hi_u8 *puc_read_buffer)
{
    oal_file_stru *fp = HI_NULL;
    hi_s32 l_ret;

    if ((cfg_patch == HI_NULL) || (puc_read_buffer == HI_NULL)) {
        printk("para is HI_NULL\n");
        return -OAL_EFAIL;
    }

    fp = oal_file_open((const hi_char *)cfg_patch, (OAL_O_RDONLY), 0);
    if (IS_ERR_OR_NULL(fp)) {
        printk("open file %s fail, fp=%p\n", cfg_patch, fp);
        return -OAL_EFAIL;
    }

    memset_s(puc_read_buffer, READ_CFG_BUF_LEN, 0, READ_CFG_BUF_LEN);
    l_ret = oal_file_read(fp, puc_read_buffer, READ_CFG_BUF_LEN);
    *(puc_read_buffer + READ_CFG_BUF_LEN - 1) = '\0';
    oal_file_close(fp);
    fp = HI_NULL;

    return l_ret;
}

hi_u8 firmware_param_check(hi_u8 **begin, hi_u8 **end, hi_u8 **link, hi_u8 *puc_cfg_buffer, hi_s32 *l_ret)
{
    *begin = puc_cfg_buffer;

    /* 注释行 */
    if (puc_cfg_buffer[0] == '@') {
        *l_ret = ERROR_TYPE_CMD;
        return HI_FAIL;
    }

    /* 错误行，或者退出命令行 */
    *link = (hi_u8 *)strchr((const hi_char *)(*begin), '=');
    if (*link == HI_NULL) {
        /* 退出命令行 */
        if (strstr((hi_char *)puc_cfg_buffer, QUIT_CMD_KEYWORD) != HI_NULL) {
            *l_ret = QUIT_TYPE_CMD;
            return HI_SUCCESS;
        }
        *l_ret = ERROR_TYPE_CMD;
        return HI_FAIL;
    }
    /* 错误行，没有结束符 */
    *end = (hi_u8 *)strchr((const hi_char *)(*link), ';');
    if (*end == HI_NULL) {
        *l_ret = ERROR_TYPE_CMD;
        return HI_FAIL;
    }

    return HI_CONTINUE;
}

hi_u8 firmware_parse_cmd_type(hi_s32 *l_cmdlen, hi_u8 **handle, hi_s32 *l_ret)
{
    hi_u8 *handle_temp = HI_NULL;

    /* 判断命令类型 */
    if (!memcmp(*handle, (hi_u8 *)FILE_TYPE_CMD_KEY, strlen((const hi_char *)FILE_TYPE_CMD_KEY))) {
        handle_temp = (hi_u8 *)strstr((const hi_char *)(*handle), (const hi_char *)FILE_TYPE_CMD_KEY);
        if (handle_temp == HI_NULL) {
            oam_error_log1(0, 0, "'ADDR_FILE_'is not handle child string, handle=%s", *handle);
            *l_ret = ERROR_TYPE_CMD;
            return HI_FAIL;
        }
        *handle = handle_temp + strlen(FILE_TYPE_CMD_KEY);
        *l_cmdlen = *l_cmdlen - strlen(FILE_TYPE_CMD_KEY);
        *l_ret = FILE_TYPE_CMD;
    } else if (!memcmp(*handle, (hi_u8 *)NUM_TYPE_CMD_KEY, strlen(NUM_TYPE_CMD_KEY))) {
        handle_temp = (hi_u8 *)strstr((const hi_char *)(*handle), (const hi_char *)NUM_TYPE_CMD_KEY);
        if (handle_temp == HI_NULL) {
            oam_error_log1(0, 0, "'PARA_' is not handle child string, handle=%s", (uintptr_t)*handle);
            *l_ret = ERROR_TYPE_CMD;
            return HI_FAIL;
        }
        *handle = handle_temp + strlen(NUM_TYPE_CMD_KEY);
        *l_cmdlen = *l_cmdlen - strlen(NUM_TYPE_CMD_KEY);
        *l_ret = NUM_TYPE_CMD;
    } else if (!memcmp(*handle, (hi_u8 *)CFG_TYPE_CMD_KEY, strlen(CFG_TYPE_CMD_KEY))) {
        handle_temp = (hi_u8 *)strstr((const hi_char *)(*handle), (const hi_char *)CFG_TYPE_CMD_KEY);
        if (handle_temp == HI_NULL) {
            *l_ret = CFG_TYPE_CMD;
            return HI_SUCCESS;
        }
        *handle = handle_temp + strlen(CFG_TYPE_CMD_KEY);
        *l_cmdlen = *l_cmdlen - strlen(CFG_TYPE_CMD_KEY);
        *l_ret = CFG_TYPE_CMD;
    } else {
        *l_ret = ERROR_TYPE_CMD;
        return HI_FAIL;
    }

    return HI_CONTINUE;
}

/* ****************************************************************************
 功能描述  : 解析cfg文件中的命令
 输入参数  : puc_cfg_buffer: 保存cfg文件内容的buffer
             puc_cmd_name  : 保存解析以后命令关键字的buffer
             puc_cmd_para  : 保存解析以后命令参数的buffer
 输出参数  : 无
 返 回 值  : 返回命令的类型
**************************************************************************** */
static hi_s32 firmware_parse_cmd(hi_u8 *puc_cfg_buffer, hi_u8 *puc_cmd_name, hi_u8 cmd_len, hi_u8 *puc_cmd_para,
    hi_u32 *val_begin)
{
    hi_s32 l_ret;
    hi_s32 l_cmdlen;
    hi_s32 l_paralen;
    hi_u8 *begin = HI_NULL;
    hi_u8 *end = HI_NULL;
    hi_u8 *link = HI_NULL;
    hi_u8 *handle = HI_NULL;

    hi_unref_param(cmd_len);

    if ((puc_cfg_buffer == HI_NULL) || (puc_cmd_name == HI_NULL) || (puc_cmd_para == HI_NULL)) {
        oam_error_log0(0, 0, "para is HI_NULL\n");
        return ERROR_TYPE_CMD;
    }

    if (firmware_param_check(&begin, &end, &link, puc_cfg_buffer, &l_ret) != HI_CONTINUE) {
        return l_ret;
    }

    *val_begin = (uintptr_t)(link + 1);

    l_cmdlen = link - begin;

    /* 删除关键字的两边空格 */
    handle = delete_space((hi_u8 *)begin, &l_cmdlen);
    if (handle == HI_NULL) {
        return ERROR_TYPE_CMD;
    }

    if (firmware_parse_cmd_type(&l_cmdlen, &handle, &l_ret) != HI_CONTINUE) {
        return l_ret;
    }

    if (l_cmdlen > DOWNLOAD_CMD_LEN || l_cmdlen < 0) {
        oam_error_log0(0, 0, "cmd len out of range!\n");
        return ERROR_TYPE_CMD;
    }
    memcpy_s(puc_cmd_name, l_cmdlen, handle, l_cmdlen);

    /* 删除值两边空格 */
    begin = link + 1;
    l_paralen = end - begin;
    if (l_paralen > DOWNLOAD_CMD_PARA_LEN || l_paralen < 0) {
        oam_error_log0(0, 0, "para len out of range!\n");
        return ERROR_TYPE_CMD;
    }

    handle = delete_space((hi_u8 *)begin, &l_paralen);
    if (handle == HI_NULL) {
        return ERROR_TYPE_CMD;
    }
    memcpy_s(puc_cmd_para, l_paralen, handle, l_paralen);

    return l_ret;
}

/* ****************************************************************************
 功能描述  : 解析cfg文件，将解析的结果保存在g_st_cfg_info全局变量中
 输入参数  : puc_cfg_info_buf: 保存了cfg文件内容的buffer
             l_buf_len       : puc_cfg_info_buf的长度
             ul_index        : 保存解析结果的数组索引值
 输出参数  : 无
 返 回 值  : 0表示成功，-1表示失败
**************************************************************************** */
static hi_s32 firmware_parse_cfg(hi_u8 *puc_cfg_info_buf, hi_s32 l_buf_len, hi_u32 ul_index)
{
    hi_u32          cfg_idx = 0;
    hi_s32          i, l_len, cmd_type;
    hi_u8          *flag = HI_NULL;
    hi_u8          *begin = HI_NULL;
    hi_u8          *end = HI_NULL;
    hi_u8           cmd_name[DOWNLOAD_CMD_LEN];
    hi_u8           cmd_para[DOWNLOAD_CMD_PARA_LEN];
    hi_u32          cmd_para_len = 0;
    hi_u32          val_offset = 0;
    if (puc_cfg_info_buf == HI_NULL) {
        oam_error_log0(0, 0, "firmware_parse_cfg:: puc_cfg_info_buf is HI_NULL");
        return -OAL_EFAIL;
    }

    g_st_cfg_info.apst_cmd[ul_index] = (struct cmd_type_st *)malloc_cmd_buf(puc_cfg_info_buf, ul_index);
    if (g_st_cfg_info.apst_cmd[ul_index] == HI_NULL) {
        oam_error_log0(0, 0, "firmware_parse_cfg:: malloc_cmd_buf fail");
        return -OAL_EFAIL;
    }

    /* 解析CMD BUF */
    flag = puc_cfg_info_buf;
    l_len = l_buf_len;
    i = 0;
    while ((i < g_st_cfg_info.al_count[ul_index]) && (flag < &puc_cfg_info_buf[l_len])) {
        /*
         * 获取配置文件中的一行,配置文件必须是unix格式.
         * 配置文件中的某一行含有字符 @ 则认为该行为注释行
         */
        begin = flag;
        end = (hi_u8 *)strchr((const hi_char *)flag, '\n');
        if (end == HI_NULL) { /* 文件的最后一行，没有换行符 */
            end = &puc_cfg_info_buf[l_len];
        } else if (end == begin) { /* 该行只有一个换行符 */
            oam_error_log0(0, 0, "blank line\n");
            flag = end + 1;
            continue;
        }
        *end = '\0';

        memset_s(cmd_name, DOWNLOAD_CMD_LEN, 0, DOWNLOAD_CMD_LEN);
        memset_s(cmd_para, DOWNLOAD_CMD_PARA_LEN, 0, DOWNLOAD_CMD_PARA_LEN);

        cmd_type = firmware_parse_cmd(begin, cmd_name, DOWNLOAD_CMD_LEN, cmd_para, &val_offset);

        if (cmd_type == CFG_TYPE_CMD) { /* 产测配置命令类型，增加到缓存 */
            if (cfg_idx == CFG_CMD_NUM_MAX) {
                oam_error_log1(0, 0, "firmware_parse_cfg:: Cus cfg items exceed limit %d, will omit", CFG_CMD_NUM_MAX);
            }
            memcpy_s(g_cus_cfg_cmd[cfg_idx].cmd_name, DOWNLOAD_CMD_LEN, cmd_name, DOWNLOAD_CMD_LEN);
            memcpy_s(g_cus_cfg_cmd[cfg_idx].cmd_para, DOWNLOAD_CMD_PARA_LEN, cmd_para, DOWNLOAD_CMD_PARA_LEN);
            g_cus_cfg_cmd[cfg_idx].cmd_name[DOWNLOAD_CMD_LEN - 1] = '\0';
            g_cus_cfg_cmd[cfg_idx].cmd_para[DOWNLOAD_CMD_PARA_LEN - 1] = '\0';
            g_cus_cfg_cmd[cfg_idx].val_offset = val_offset - (uintptr_t)puc_cfg_info_buf;
            g_cus_cfg_cmd[cfg_idx].val_len = (hi_u16)((uintptr_t)end - val_offset + 1);
#ifdef CUSTOM_DBG
            oam_info_log0(0, 0, "firmware_parse_cfg:: cmd type=[%d],cmd_name=[%s],cmd_para=[%s], line len %d",
                          cmd_type, cmd_name, cmd_para, g_cus_cfg_cmd[cfg_idx].val_len);
#endif
            ++cfg_idx;
        } else if (cmd_type != ERROR_TYPE_CMD) { /* 正确的命令类型，增加 */
            g_st_cfg_info.apst_cmd[ul_index][i].cmd_type = cmd_type;
            if (memcpy_s(g_st_cfg_info.apst_cmd[ul_index][i].cmd_name, DOWNLOAD_CMD_LEN, cmd_name, DOWNLOAD_CMD_LEN) !=
                EOK ||  memcpy_s(g_st_cfg_info.apst_cmd[ul_index][i].cmd_para, DOWNLOAD_CMD_PARA_LEN, cmd_para,
                DOWNLOAD_CMD_PARA_LEN) != EOK) {
                return -OAL_EFAIL;
            }
            g_st_cfg_info.apst_cmd[ul_index][i].cmd_name[DOWNLOAD_CMD_LEN - 1] = '\0';
            g_st_cfg_info.apst_cmd[ul_index][i].cmd_para[DOWNLOAD_CMD_PARA_LEN - 1] = '\0';

            /* 获取配置版本号 */
            if (!memcmp(g_st_cfg_info.apst_cmd[ul_index][i].cmd_name, VER_CMD_KEYWORD, strlen(VER_CMD_KEYWORD))) {
                cmd_para_len = strlen((const hi_char *)g_st_cfg_info.apst_cmd[ul_index][i].cmd_para);
                if (cmd_para_len <= VERSION_LEN) {
                    if (memcpy_s(g_st_cfg_info.auc_CfgVersion, cmd_para_len,
                        g_st_cfg_info.apst_cmd[ul_index][i].cmd_para, cmd_para_len) != EOK) {
                        oam_warning_log0(0, 0, "firmware_parse_cfg::memcpy_s fail!");
                    }
                } else {
                    oam_error_log1(0, 0, "firmware_parse_cfg:: cmd_para_len = %d over auc_CfgVersion length",
                        cmd_para_len);
                    return -OAL_EFAIL;
                }
                oam_warning_log1(0, 0, "Hi3881 VERSION:: [%s]", (uintptr_t)g_st_cfg_info.auc_CfgVersion);
            }
            i++;
        }
        flag = end + 1;
    }

    /* 根据实际命令个数，修改最终的命令个数 */
    g_st_cfg_info.al_count[ul_index] = i;
    oam_info_log2(0, 0, "firmware_parse_cfg:: effective cmd count: al_count[%d] = %d", ul_index,
        g_st_cfg_info.al_count[ul_index]);

    return HI_SUCCESS;
}


/* ****************************************************************************
 功能描述  : 读取cfg文件并解析，将解析的结果保存在g_st_cfg_info全局变量中
 输入参数  : cfg_patch: cfg文件的路径
             ul_index     : 保存解析结果的数组索引值
 输出参数  : 无
 返 回 值  : 0表示成功，-1表示失败
**************************************************************************** */
static hi_s32 firmware_get_cfg(const hi_u8 *cfg_patch, hi_u32 ul_index)
{
    hi_u8 *read_cfg_buf = HI_NULL;
    hi_s32 l_readlen;
    hi_s32 l_ret;
    hi_u32 ul_soft_ver = 0;

    if (cfg_patch == HI_NULL) {
        oam_warning_log0(0, 0, "firmware_get_cfg:: cfg file path is HI_NULL");
        return -OAL_EFAIL;
    }

    /* cfg文件限定在小于2048,如果cfg文件的大小确实大于2048，可以修改READ_CFG_BUF_LEN的值 */
    read_cfg_buf = OS_KMALLOC_GFP(READ_CFG_BUF_LEN);
    if (read_cfg_buf == HI_NULL) {
        oam_warning_log0(0, 0, "firmware_get_cfg:: kmalloc READ_CFG_BUF fail");
        return -OAL_EFAIL;
    }

    memset_s(read_cfg_buf, READ_CFG_BUF_LEN, 0, READ_CFG_BUF_LEN);

#ifndef _PRE_HI113X_FS_DISABLE
    (hi_void)ul_soft_ver;
    l_readlen = firmware_read_cfg(cfg_patch, read_cfg_buf);
    if (l_readlen < 0) {
        oam_warning_log1(0, 0, "firmware_get_cfg:: firmware_read_cfg failed[%d]", l_readlen);
        oal_free(read_cfg_buf);
        read_cfg_buf = HI_NULL;
        return -OAL_EFAIL;
    } else if (l_readlen > READ_CFG_BUF_LEN - 1) {
        /*
         * 减1是为了确保cfg文件的长度不超过READ_CFG_BUF_LEN，
         * 因为firmware_read_cfg最多只会读取READ_CFG_BUF_LEN长度的内容
         */
        oam_error_log2(0, 0, "firmware_get_cfg:: cfg file [%s] larger than %d", (uintptr_t)cfg_patch, READ_CFG_BUF_LEN);
        oal_free(read_cfg_buf);
        read_cfg_buf = HI_NULL;
        return -OAL_EFAIL;
    }
#else
    ul_soft_ver = get_device_soft_version();
    if (ul_soft_ver >= SOFT_VER_BUTT) {
        oam_error_log1(0, 0, "firmware_get_cfg:: get_device_soft_version failed[%d]", ul_soft_ver);
        oal_free(read_cfg_buf);
        return -OAL_EFAIL;
    }

    l_readlen = g_st_wifi_cfg[ul_soft_ver]->len;
    if (l_readlen > READ_CFG_BUF_LEN) {
        oam_error_log1(0, 0, "firmware_get_cfg:: read_wifi_cfg failed[%d]", l_readlen);
        oal_free(read_cfg_buf);
        read_cfg_buf = HI_NULL;
        return -OAL_EFAIL;
    }

    if (memcpy_s(read_cfg_buf, l_readlen, g_st_wifi_cfg[ul_soft_ver]->addr, l_readlen) != EOK) {
        oal_free(read_cfg_buf);
        return -OAL_EFAIL;
    }
#endif

    l_ret = firmware_parse_cfg(read_cfg_buf, l_readlen, ul_index);
    if (l_ret < 0) {
        oam_error_log1(0, 0, "firmware_get_cfg:: firmware_parse_cfg failed[%d]", l_ret);
    }

    oal_free(read_cfg_buf);
    read_cfg_buf = HI_NULL;

    return l_ret;
}

/* ****************************************************************************
 功能描述  : firmware加载
 输入参数  : ul_index: 有效加载命令数组的索引
 输出参数  : 无
 返 回 值  : 0表示成功，-1表示失败
**************************************************************************** */
hi_s32 firmware_download(hi_u32 ul_index)
{
    hi_s32 l_ret;
    hi_s32 i;
    hi_s32 l_cmd_type;
    hi_u8 *puc_cmd_name = HI_NULL;
    hi_u8 *puc_cmd_para = HI_NULL;
    firmware_mem_stru *firmware_mem = HI_NULL;

    if (ul_index >= CFG_FILE_TOTAL) {
        oam_error_log1(0, 0, "firmware_download:: ul_index [%d] is error", ul_index);
        return -OAL_EFAIL;
    }

    if (g_st_cfg_info.al_count[ul_index] == 0) {
        oam_error_log1(0, 0, "firmware_download:: firmware download cmd count is 0, ul_index = [%d]", ul_index);
        return -OAL_EFAIL;
    }

    firmware_mem = firmware_mem_request();
    if (firmware_mem == HI_NULL) {
        oam_error_log0(0, 0, "firmware_download:: firmware_mem_request fail");
        return -OAL_EFAIL;
    }

    for (i = 0; i < g_st_cfg_info.al_count[ul_index]; i++) {
        l_cmd_type = g_st_cfg_info.apst_cmd[ul_index][i].cmd_type;
        puc_cmd_name = g_st_cfg_info.apst_cmd[ul_index][i].cmd_name;
        puc_cmd_para = g_st_cfg_info.apst_cmd[ul_index][i].cmd_para;
        l_ret = execute_download_cmd(l_cmd_type, puc_cmd_name, puc_cmd_para, firmware_mem);
        if (l_ret < 0) {
            if (puc_cmd_name == HI_NULL) {
                continue;
            }
            if (ul_index == RAM_REG_TEST_CFG && ((!memcmp(puc_cmd_name, JUMP_CMD_KEYWORD, strlen(JUMP_CMD_KEYWORD))) &&
                (g_ul_jump_cmd_result == CMD_JUMP_EXEC_RESULT_FAIL))) {
                /* device mem check 返回失败，继续执行READM命令，将结果读上来 */
                oam_error_log0(0, 0, "firmware_download:: Device Mem Reg check result is fail");
                continue;
            }
            l_ret = -OAL_EFAIL;
            firmware_mem_free(firmware_mem);
            return l_ret;
        }
    }
    l_ret = HI_SUCCESS;
    firmware_mem_free(firmware_mem);
    return l_ret;
}


EXPORT_SYMBOL(firmware_download);

efuse_info_stru *get_efuse_info_handler(hi_void)
{
    return &g_st_efuse_info;
}
EXPORT_SYMBOL(get_efuse_info_handler);

hi_u32 get_device_soft_version(hi_void)
{
    efuse_info_stru *pst_efuse_info;
    hi_u32 ul_soft_ver;
    pst_efuse_info = get_efuse_info_handler();
    if (pst_efuse_info == HI_NULL) {
        printk("***get_device_soft_version***[%d]\n", __LINE__);
        return SOFT_VER_BUTT;
    }

    ul_soft_ver = pst_efuse_info->soft_ver;
    if (ul_soft_ver >= SOFT_VER_BUTT) {
        printk("***get_device_soft_version***[%d]\n", __LINE__);
        return SOFT_VER_BUTT;
    }

    return ul_soft_ver;
}

hi_s32 firmware_read_efuse_info(hi_void)
{
    firmware_mem_stru *firmware_mem = HI_NULL;
    const unsigned long ul_mac_addr = DEVICE_EFUSE_ADDR;

    efuse_info_stru *pst_efuse_info = get_efuse_info_handler();
    if (pst_efuse_info == HI_NULL) {
        oam_error_log0(0, 0, "pst_efuse_info is HI_NULL!\n");
        goto failed;
    }
    if (memset_s(pst_efuse_info, sizeof(efuse_info_stru), 0, sizeof(efuse_info_stru)) != EOK) {
        goto failed;
    }

    firmware_mem = firmware_mem_request();
    if (firmware_mem == HI_NULL) {
        oam_error_log0(0, 0, "firmware_mem_request fail\n");
        goto failed;
    }

    hi_u32 ul_size = DEVICE_EFUSE_LENGTH;
#if (_PRE_FEATURE_USB == _PRE_FEATURE_CHANNEL_TYPE)
    ul_size = hiusb_align_32(ul_size);
#elif (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    ul_size = HISDIO_ALIGN_4_OR_BLK(ul_size);
#endif
    if (ul_size > firmware_mem->ul_data_buf_len) {
        oam_error_log0(0, 0, "device mac length is too long !\n");
        goto failed;
    }

    if (snprintf_s((hi_char *)firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, CMD_BUFF_LEN - 1, "%s%c0x%lx%c%d%c",
        RMEM_CMD_KEYWORD, COMPART_KEYWORD, ul_mac_addr, COMPART_KEYWORD, ul_size, COMPART_KEYWORD) == -1) {
        goto failed;
    }
    oam_info_log1(0, 0, "read mac cmd:[%s]\n", (uintptr_t)firmware_mem->puc_send_cmd_buff);
    hi_s32 l_ret = firmware_send_msg(firmware_mem->puc_send_cmd_buff,
                                     strlen((const hi_char *)firmware_mem->puc_send_cmd_buff));
    if (l_ret < 0) {
        oam_error_log1(0, 0, "read device mac cmd send fail![%d]\n", l_ret);
        goto failed;
    }

    l_ret = firmware_read_msg(firmware_mem->puc_data_buf, ul_size);
    if (l_ret < 0) {
        oam_error_log1(0, 0, "read device mac fail![%d]\n", l_ret);
        goto failed;
    }

    if (memcpy_s(pst_efuse_info, sizeof(efuse_info_stru), firmware_mem->puc_data_buf,
        sizeof(efuse_info_stru)) != EOK) {
        goto failed;
    }

    firmware_mem_free(firmware_mem);
    return HI_SUCCESS;

failed:
    if (firmware_mem != HI_NULL) {
        firmware_mem_free(firmware_mem);
    }
    return -OAL_EFAIL;
}
/* ****************************************************************************
 功能描述  : firmware加载的cfg文件初始化，读取并解析cfg文件，将解析的结果保存在
             g_st_cfg_info全局变量中
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0表示成功，-1表示失败
**************************************************************************** */
hi_u32 plat_firmware_init(void)
{
    hi_s32 l_ret;
    hi_u32 ul_soft_ver;

#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
    if (g_ul_wlan_resume_wifi_init_flag) {
        l_ret = plat_data_recover();
        if (l_ret != HI_SUCCESS) {
            oam_error_log1(0, 0, "plat_firmware_init:: plat_data_recover failed[%d]", l_ret);
            return OAL_EFAIL;
        }
    }
#endif

#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
    if (!g_ul_wlan_resume_wifi_init_flag) {
#endif
        ul_soft_ver = get_device_soft_version();
        if (ul_soft_ver >= SOFT_VER_BUTT) {
            oam_error_log1(0, 0, "plat_firmware_init:: get_device_soft_version failed[%d]", ul_soft_ver);
            return OAL_EFAIL;
        }

        /* 解析cfg文件 */
        l_ret = firmware_get_cfg(g_auc_cfg_path[ul_soft_ver], WIFI_CFG);
        if (l_ret < 0) {
            oam_error_log1(0, 0, "plat_firmware_init:: firware_get_cfg faile[%d]d", l_ret);
            plat_firmware_clear();
            return OAL_EFAIL;
        }

#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
    }
#endif

#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
    l_ret = plat_data_backup();
    if (l_ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "plat_firmware_init:: plat_data_backup failed[%d]", l_ret);
        plat_firmware_clear();
        return OAL_EFAIL;
    }
#endif

    printk("plat_firmware_init SUCCESSFULLY\r\n");
    return HI_SUCCESS;
}

EXPORT_SYMBOL(plat_firmware_init);

/* ****************************************************************************
 功能描述  : 释放firmware_cfg_init时申请的内存
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 总是返回0，表示成功
**************************************************************************** */
hi_s32 plat_firmware_clear(void)
{
    hi_s32 i;

    for (i = 0; i < CFG_FILE_TOTAL; i++) {
        g_st_cfg_info.al_count[i] = 0;
        if (g_st_cfg_info.apst_cmd[i] != HI_NULL) {
            oal_free(g_st_cfg_info.apst_cmd[i]);
            g_st_cfg_info.apst_cmd[i] = HI_NULL;
        }
    }
    return HI_SUCCESS;
}

hi_bool char_2_hex(hi_u8 c, hi_u8 *val)
{
    if ((c >= '0') && (c <= '9')) {
        *val = c - '0';
        return HI_TRUE;
    }
    if ((c >= 'a') && (c <= 'f')) {
        *val = 0x0a + c - 'a';
        return HI_TRUE;
    }
    if ((c >= 'A') && (c <= 'F')) {
        *val = 0x0a + c - 'A';
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_bool parse_mac_addr(const hi_u8 *str, hi_u8 str_len, hi_u8 *mac, hi_u8 size)
{
    hi_u8 i, temp;
    hi_u8 val = 0;
    hi_u8 idx = 0;
    hi_u8 offset = 0;
    hi_bool is_mac_valid = HI_FALSE;
    hi_bool ret;

    for (i = 0; (i < str_len) && (idx < size); ++i) {
        ret = char_2_hex(str[i], &temp);
        if (ret) {
            if (offset == 2) { /* 2:连续有效字符.超过2个有效值 */
                return HI_FALSE;
            }
            val = (offset == 0) ? temp : ((val << 4) + temp); /* 4:位数 */
            if (offset == 1) {
                mac[idx] = val;
                is_mac_valid = (val != 0) ? HI_TRUE : is_mac_valid;
                ++idx;
            }
            ++offset;
        } else if (offset != 2) { /* 2:连续有效字符,少于2个有效值 */
            return HI_FALSE;
        } else if (str[i] == ':') {
            offset = 0;
        } else {
            return HI_FALSE;
        }
    }

    return ((idx == size) && is_mac_valid) ? HI_TRUE : HI_FALSE;
}

hi_bool split(hi_u8 *src, const hi_char *separator, hi_char **dest, hi_u8 num)
{
    char *next = HI_NULL;
    int count = 0;

    if ((src == HI_NULL) || (strlen((const hi_char *)src) == 0) || (separator == HI_NULL) || (strlen(separator) == 0)) {
        return HI_FALSE;
    }
    next = oal_strtok((char *)src, separator);
    while (next != HI_NULL) {
        *(dest++) = next;
        ++count;
        if (count == num) {
            break;
        }
        next = oal_strtok(HI_NULL, separator);
    }
    if (count < num) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

hi_bool split_cmd_paras(const hi_char *cmd_para, hi_u8 size, hi_u32 *data, hi_u8 data_size, hi_bool by_hex)
{
    hi_u8 i;
    hi_char *stop = HI_NULL;
    hi_u8 src[DOWNLOAD_CMD_PARA_LEN + 1] = {0};
    hi_char *revbuf[CMD_SUB_PARA_CNT_MAX] = {0};

    if (data_size > CMD_SUB_PARA_CNT_MAX) {
        return HI_FALSE;
    }

    if (memcpy_s(src, sizeof(src), cmd_para, size) != EOK) {
        oam_error_log0(0, 0, "split_cmd_paras:: memcpy_s failed");
        return HI_FALSE;
    }

    if (!split(src, ",", revbuf, data_size)) {
        return HI_FALSE;
    }
    for (i = 0; i < data_size; ++i) {
        if (by_hex) {
            data[i] = (hi_u32)oal_strtol(revbuf[i], &stop, 16); /* 16:按16进制解析 */
        } else {
            data[i] = (hi_u32)oal_strtol(revbuf[i], &stop, 10); /* 10:按10进制解析 */
        }
    }
    return HI_TRUE;
}

hi_bool get_cfg_idx(const hi_char *cfg_name, hi_u8 *idx)
{
    hi_u8 i;
    /* 查找参数名是否缓存 */
    for (i = 0; i < CFG_CMD_NUM_MAX; ++i) {
        if (strcmp(cfg_name, (const hi_char *)g_cus_cfg_cmd[i].cmd_name) == 0) {
            break;
        }
    }
    *idx = i;
    return HI_TRUE;
}

hi_bool cfg_get_mac(hi_u8 *mac_addr, hi_u8 size)
{
    hi_u8 idx;

    if (!get_cfg_idx((const hi_char *)WIFI_CFG_MAC, &idx)) {
        return HI_FALSE;
    }

    return parse_mac_addr(g_cus_cfg_cmd[idx].cmd_para, strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para), mac_addr,
        size);
}

hi_u32 cfg_dbb(hi_void)
{
    const hi_u8 data_size = 7; /* 7:参数个数 */
    hi_u32 data[data_size];
    hi_u8 idx;
    hi_bool ret;

    memset_s(data, sizeof(data), 0, sizeof(data));
    /* 查找配置索引 */
    if (!get_cfg_idx(WIFI_CFG_DBB_PARAMS, &idx)) {
        return HI_FAIL;
    }
    /* 解析参数 */
    ret = split_cmd_paras((const hi_char *)g_cus_cfg_cmd[idx].cmd_para,
        strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para), data, data_size, HI_TRUE);
    if (ret != HI_FAIL) {
        oam_error_log0(0, 0, "cfg_dbb:: split_cmd_paras failed");
        return ret;
    }

    return wal_cfg_dbb(data, data_size);
}

hi_u32 cfg_country_code(hi_void)
{
    const hi_u8 size = 3;
    hi_char data[size];
    hi_u8 idx;

    /* 查找配置索引 */
    if (!get_cfg_idx(WIFI_CFG_COUNTRY_CODE, &idx)) {
        return HI_FAIL;
    }
    if (strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para) < size - 1) {
        return HI_FAIL;
    }
    /* 拷贝国家码,顺序交叉 */
    data[0] = g_cus_cfg_cmd[idx].cmd_para[1];
    data[1] = g_cus_cfg_cmd[idx].cmd_para[0];
    data[2] = '\0'; /* 下标2 */

    return wal_cfg_country_code(data, size);
}

hi_u32 cfg_tx_pwr_offset(hi_void)
{
    hi_u8 idx;
    const hi_u8 data_size = 13; /* 13:参数个数 */
    hi_u32 data[data_size];
    hi_bool ret;

    memset_s(data, sizeof(data), 0, sizeof(data));
    /* 查找配置索引 */
    if (!get_cfg_idx(WIFI_CFG_CH_TXPWR, &idx)) {
        return HI_FAIL;
    }
    /* 解析参数 */
    ret = split_cmd_paras((const hi_char *)g_cus_cfg_cmd[idx].cmd_para,
        strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para), data, data_size, HI_TRUE);
    if (ret != HI_FAIL) {
        oam_error_log0(0, 0, "cfg_tx_pwr_offset:: split_cmd_paras failed");
        return ret;
    }

    return wal_cfg_fcc_tx_pwr(data, data_size);
}

hi_u32 cfg_freq_comp_val(hi_void)
{
    hi_u8 idx;
    const hi_u8 data_size = 3; /* 3:参数个数 */
    hi_u32 data[data_size];
    hi_bool ret;

    memset_s(data, sizeof(data), 0, sizeof(data));
    /* 查找配置索引 */
    if (!get_cfg_idx(WIFI_CFG_FREQ_COMP, &idx)) {
        return HI_FAIL;
    }
    /* 解析参数 */
    ret = split_cmd_paras((const hi_char *)g_cus_cfg_cmd[idx].cmd_para,
        strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para), data, data_size, HI_FALSE);
    if (ret != HI_FAIL) {
        oam_error_log0(0, 0, "cfg_freq_comp_val:: split_cmd_paras failed");
        return ret;
    }

    return wal_cfg_freq_comp_val(data, data_size);
}

hi_u32 cfg_rssi_ofset(hi_void)
{
    hi_s32 data;
    hi_char *stop = HI_NULL;
    hi_u8 idx;

    /* 查找配置索引 */
    if (!get_cfg_idx(WIFI_CFG_RSSI_OFFSET, &idx)) {
        return HI_FAIL;
    }
    data = oal_strtol((const hi_char *)g_cus_cfg_cmd[idx].cmd_para, &stop, 10); /* 10:按10进制解析 */
    return wal_cfg_rssi_ofset(data);
}

hi_u32 firmware_sync_cfg_paras_to_wal_customize(hi_void)
{
    /* 同步dbb scale功率配置 */
    if (cfg_dbb() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_dbb failed");
        return HI_FAIL;
    }
    /* 同步国家码 */
    if (cfg_country_code() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_country_code failed");
        return HI_FAIL;
    }
    /* 同步FCC功率配置 */
    if (cfg_tx_pwr_offset() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_tx_pwr_offset failed");
        return HI_FAIL;
    }
    /* 同步高温频偏补偿配置 */
    if (cfg_freq_comp_val() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_dbb failed");
        return HI_FAIL;
    }
    /* 同步rssi补偿配置 */
    if (cfg_rssi_ofset() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_rssi_ofset failed");
        return HI_FAIL;
    }

    printk("firmware_sync_cfg_paras_to_wal_customize SUCCESSFULLY\r\n");
    return HI_SUCCESS;
}

EXPORT_SYMBOL(plat_firmware_clear);
EXPORT_SYMBOL(get_device_soft_version);
