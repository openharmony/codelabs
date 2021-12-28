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
#include "wal_wpa_ioctl.h"
#include "net_adpater.h"
#include "wal_cfg80211.h"
#include "wal_ioctl.h"
#include "wal_event_msg.h"
#include "hmac_ext_if.h"
#include "wal_cfg80211.h"
#include "wal_cfg80211_apt.h"
#include "lwip/netifapi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* ****************************************************************************
 函 数 名  : hisi_hwal_wpa_ioctl
 功能描述  : 驱动对wpa提供统一调用接口
 输入参数  :
 输出参数  : 无
 返 回 值  : static hi_s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */

hi_s32 hisi_hwal_wpa_ioctl(hi_char *pc_ifname, hisi_ioctl_command_stru *cmd)
{
    (void)pc_ifname;
    oam_error_log1(0, 0, "hwal_wpa_ioctl ::The CMD[%d] handlers is NULL", cmd->cmd);

    return -HISI_EFAIL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
