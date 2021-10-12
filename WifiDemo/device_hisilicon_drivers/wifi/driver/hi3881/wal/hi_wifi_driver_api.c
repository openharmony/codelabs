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
#include "hmac_ext_if.h"
#include "wal_customize.h"
#include "wal_11d.h"
#include "wal_hipriv.h"
#include "wal_ioctl.h"
#include "net_adpater.h"
#include "wal_event.h"

#include "mac_pm_driver.h"
#include "oal_chr.h"
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "lwip/netifapi.h"
#include "wal_cfg80211_apt.h"
#include "hi_wifi_api.h"
#include "oal_sdio_host_if.h"
#endif

#include "wal_event_msg.h"
#include "wal_main.h"
#ifdef _PRE_WLAN_FEATURE_ANY
#include "hi_any_api.h"
#endif
#ifdef LOSCFG_APP_MESH
#include "hi_wifi_mesh_api.h"
#endif
#include "hcc_host.h"
#include "plat_pm_wlan.h"
#include "wifi_base.h"
#include "hdf_wifi_product.h"
#include "hi_wifi_driver_wpa_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* ****************************************************************************
  3 函数实现
**************************************************************************** */
#if defined(_PRE_WLAN_FEATURE_HIPRIV) || defined(_PRE_WLAN_FEATURE_SIGMA)
/* ****************************************************************************
 功能描述  : 启动hipriv命令
 输入参数  : [1]argc
             [2]argv[]
 返 回 值  : 无
**************************************************************************** */
hi_void hi_wifi_hipriv(hi_s32 argc, const hi_u8 *argv[])
{
    hi_u32      len;
    hi_u32      total_len;
    hi_s32      index;
    hi_char    *pc_buffer = NULL;
    hi_char    *pc_buffer_index = NULL;

    if (argc == 0 || argc > HI_WIFI_HIPRIV_ARGC_MAX) {
        oam_warning_log0(0, OAM_SF_ANY, "hi_wifi_hipriv: Invalid argc!");
        return;
    }
    if (argv == HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "hi_wifi_hipriv: Invalid argv!");
        return;
    }

    total_len = 0;
    for (index = 0; index < argc; index++) {
        total_len += strlen((hi_char *)argv[index]) + 1;
    }
    if (total_len > WAL_HIPRIV_CMD_MAX_LEN) {
        oam_error_log0(0, OAM_SF_ANY, "hi_wifi_hipriv: cmd too large");
        return;
    }

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    pc_buffer = malloc(total_len * sizeof(hi_char));
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    pc_buffer = oal_memalloc(total_len * sizeof(hi_char));
#endif
    if (pc_buffer == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hi_wifi_hipriv: malloc failed!");
        return;
    }

    pc_buffer_index = pc_buffer;
    for (index = 0; index < argc; index++) {
        len = strlen((hi_char *)argv[index]);
        if (memcpy_s(pc_buffer_index, len, argv[index], len) != EOK) {
            oam_error_log0(0, 0, "{hi_wifi_hipriv::mem safe function err!}");
            continue;
        }
        pc_buffer_index[len] = ' ';
        pc_buffer_index += len + 1;
    }

    pc_buffer[total_len - 1] = '\0';
    if (wal_hipriv_entry(pc_buffer, (hi_u32)total_len) != total_len) {
        oam_warning_log0(0, OAM_SF_ANY, "hipriv failed!");
    }
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    free(pc_buffer);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_free(pc_buffer);
#endif
    pc_buffer = NULL;

    return;
}
#endif

hi_u8 g_wifi_inited_flag = HI_FALSE;
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 初始化WiFi驱动
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_init(uint8_t max_port_count, struct BusDev *bus)
{
    hi_u8 const vap_res_num = max_port_count;
    hi_u8 const user_res_num = 4;

    if ((vap_res_num < 1 || vap_res_num > WIFI_MAX_NUM_VAP) ||
        (user_res_num < 1 || user_res_num > WIFI_MAX_NUM_USER)) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi initialization failed, vap/user num is wrong.");
        goto fail;
    }

    if (g_wifi_inited_flag == HI_TRUE) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi have inited, do not inited again.");
        goto fail;
    }

    if (hi_wifi_plat_init(vap_res_num, user_res_num) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi platform initialize failed.");
        goto fail;
    }

    if (hi_wifi_host_init(bus) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi host initialize failed.");
        goto hi_wifi_host_init_fail;
    }
    g_wifi_inited_flag = HI_TRUE;
    printk("WiFi driver init successfully!\r\n");
    return HI_SUCCESS;

hi_wifi_host_init_fail:
    hi_wifi_plat_exit();
fail:
    return HI_FAIL;
}
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 初始化WiFi驱动
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_init(uint8_t max_port_count, struct BusDev *bus)
{
    hi_u8 const vap_res_num = max_port_count;
    hi_u8 const user_res_num = 4;

    printk("[VERSION]:Hi3881V100R001C00SPC020 2020-08-20 17:10:00\n");
    if (g_wifi_inited_flag == HI_TRUE) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi have inited, do not inited again.");
        goto fail;
    }
    if (oal_register_ioctl() != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "oal_register_ioctl failed!\n");
        goto fail;
    }
    if (hi_wifi_plat_init(vap_res_num, user_res_num) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi platform initialize failed.");
        goto ioctl_clear;
    }

    if (hi_wifi_host_init(bus) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi host initialize failed.");
        goto wifi_host_init_fail;
    }

    g_wifi_inited_flag = HI_TRUE;
    printk("WiFi driver init successfully!\r\n");

    return HI_SUCCESS;
wifi_host_init_fail:
    hi_wifi_plat_exit();
ioctl_clear:
    oal_unregister_ioctl();
fail:
    return -HI_FAIL;
}
#endif

/* ****************************************************************************
 功能描述  : 去初始化WiFi驱动
 返 回 值  : 错误码
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_s32 hi_wifi_deinit(hi_void)
{
    if (g_wifi_inited_flag == HI_FALSE) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi have deinited or have not inited.");
        return HI_FAIL;
    }
    wlan_pm_exit();
    /* WIFI Host Exit */
    hi_wifi_host_exit();
    /* WIFI Plat Exit */
    hi_wifi_plat_exit();
    printk("WiFi driver deinit successfully!\r\n");
    g_wifi_inited_flag = HI_FALSE;
    return HI_SUCCESS;
}
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 hi_wifi_deinit(hi_void)
{
    if (g_wifi_inited_flag == HI_FALSE) {
        oam_error_log0(0, OAM_SF_ANY, "WiFi have deinited or have not inited.");
        return HI_FAIL;
    }
    oal_unregister_ioctl();
    wlan_pm_exit();
    /* WIFI Host Exit */
    hi_wifi_host_exit();
    /* WIFI Plat Exit */
    hi_wifi_plat_exit();
    printk("WiFi driver deinit successfully!\r\n");
    g_wifi_inited_flag = HI_FALSE;
    return HI_SUCCESS;
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 提供产测功率参数定制化接口
 返 回 值  : 错误码
**************************************************************************** */
hi_u32 hi_wifi_set_customize_params(hi_wifi_customize_params *params)
{
    hi_u8 ret;
    const hi_u8 size = 3;
    hi_char data[size];
    if (params == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hi_wifi_set_customize_params: pst_param is NULL! }");
        return HI_ERR_CODE_PTR_NULL;
    }
    ret = wal_customize_init();
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* dbb配置参数 */
    ret = wal_cfg_dbb(params->dbb_params, HI_WIFI_DBB_PARAM_CNT);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* fcc发送功率配置 */
    ret = wal_cfg_fcc_tx_pwr(params->ch_txpwr_offset, HI_WIFI_CH_TX_PWR_PARAM_CNT);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* 高温频偏配置参数 */
    ret = wal_cfg_freq_comp_val(params->freq_comp, HI_WIFI_FREQ_COMP_PARAM_CNT);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* RSSI配置参数 */
    ret = wal_cfg_rssi_ofset(params->rssi_offset);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* 拷贝国家码,顺序交叉 */
    data[0] = params->country_code[1];
    data[1] = params->country_code[0];
    data[2] = '\0'; /* 下标2 */
    ret = wal_cfg_country_code(data, size);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : 设置起始mac地址
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_set_macaddr(const hi_char *mac_addr, hi_u8 mac_len)
{
    hi_char mac_addr_tmp[ETHER_ADDR_LEN] = {0};

    if (mac_addr == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hi_wifi_set_macaddr:: macaddr is NULL!");
        return HI_FAIL;
    }
    if (memcpy_s(mac_addr_tmp, ETHER_ADDR_LEN, mac_addr, mac_len) != EOK) {
        oam_error_log0(0, 0, "hi_wifi_set_macaddr:: memcpy_s failed.");
        return HI_FAIL;
    }
    /* 建议5.5 这里强制类型转换会不会出问题 */
    if (wal_macaddr_check((hi_u8 *)mac_addr_tmp) != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "hi_wifi_set_macaddr:: Mac address invalid!");
        return HI_FAIL;
    }
    if (wal_set_dev_addr(mac_addr_tmp, ETHER_ADDR_LEN) != HI_SUCCESS) {
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取起始mac地址
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_get_macaddr(hi_char *mac_addr, hi_u8 addr_len)
{
    if (mac_addr == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hi_wifi_get_macaddr:: macaddr is NULL!");
        return HI_FAIL;
    }

    if (wal_get_dev_addr((hi_u8 *)mac_addr, addr_len, 2) != HI_SUCCESS) { /* 2: nl80211_iftype */
        oam_error_log0(0, OAM_SF_ANY, "hi_wifi_get_macaddr:: get macaddr failed");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取关联ap的rssi值
 返 回 值  : rssi值
**************************************************************************** */
hi_s32 hi_wifi_sta_get_ap_rssi(hi_void)
{
    hi_s32                          l_ret;
    oal_net_device_stru            *netdev = HI_NULL;
    mac_vap_stru                   *mac_vap = HI_NULL;
    hmac_vap_stru                  *hmac_vap = HI_NULL;
    wal_msg_write_stru             write_msg;
    mac_device_stru                *mac_dev = mac_res_get_dev();
    hi_u8 vap_idx;

    /* 寻找STA */
    for (vap_idx = 0; vap_idx < mac_dev->vap_num; vap_idx++) {
        mac_vap = mac_vap_get_vap_stru(mac_dev->auc_vap_id[vap_idx]);
        if (mac_vap == HI_NULL) {
            continue;
        }
        if (mac_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) {
            break;
        }
    }

    if (mac_vap == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_sta_get_ap_rssi:: mac_vap is NULL");
        return WLAN_RSSI_DUMMY_MARKER;
    }

    netdev = hmac_vap_get_net_device(mac_vap->vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_sta_get_ap_rssi sta device not found.");
        return WLAN_RSSI_DUMMY_MARKER;
    }

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (hmac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hi_wifi_sta_get_ap_rssi::hmac_vap_get_vap_stru, return NULL!}");
        return WLAN_RSSI_DUMMY_MARKER;
    }

    hmac_vap->query_ap_rssi_flag = HI_FALSE;
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_RSSI, sizeof(wlan_rssi_stru));
    l_ret = (hi_s32)wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(wlan_rssi_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(l_ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY,
            "{hi_wifi_sta_get_ap_rssi::wal_send_cfg_event return err code %d!}", l_ret);
        return WLAN_RSSI_DUMMY_MARKER;
    }

    // 使用非wifi目录定义宏函数,误报告警,lin_t e26告警屏蔽
    l_ret =  hi_wait_event_timeout((hmac_vap->query_wait_q),
        (HI_TRUE == hmac_vap->query_ap_rssi_flag), (5 * HZ)); // 5 频率
    if (l_ret <= 0) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "hi_wifi_sta_get_ap_rssi: query timeout.");
        return WLAN_RSSI_DUMMY_MARKER;
    }

    return hmac_vap->ap_rssi;
}

/* ****************************************************************************
 功能描述  : 设置国家码
 输入参数  : *cc 两个大写字符的国家码，数组大小至少3个字节，带字符串结束符
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_set_country(const hi_char *cc, unsigned char cc_len)
{
    oal_net_device_stru *netdev = HI_NULL;

    if (cc == HI_NULL) {
        oam_error_log0(0, 0, "wifi_set_country parameter is NULL.");
        return HI_FAIL;
    }

    if (cc_len < MAC_CONTRY_CODE_LEN) {
        oam_error_log0(0, 0, "wifi_set_country invalid country code length.");
        return HI_FAIL;
    }

    /* 国家码两个字符 */
    if (strlen(cc) != 2) { /* 2 国家码2个字符 */
        oam_error_log0(0, 0, "wifi_set_country invalid country code.");
        return HI_FAIL;
    }

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "wifi_set_country Hisilicon0 device not found.");
        return HI_FAIL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev);
#endif
    if (wal_regdomain_update(netdev, cc, MAC_CONTRY_CODE_LEN) != HI_SUCCESS) {
        oam_error_log0(0, 0, "wifi_set_country regdomain update failed.");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取当前国家码
 输出参数  : [1]cc 两个大写字符的国家码，数组大小至少3个字节
             [2]len 返回字符串长度
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_get_country(hi_char *cc, hi_s32 *len)
{
    oal_net_device_stru *netdev = HI_NULL;

    if ((cc == HI_NULL) || (len == HI_NULL)) {
        oam_error_log0(0, 0, "wifi_get_country parameter is NULL.");
        return HI_FAIL;
    }

    if (*len < MAC_CONTRY_CODE_LEN) {
        oam_error_log0(0, 0, "hi_wifi_get_country invalid country code length.");
        return HI_FAIL;
    }

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "wifi_get_country Hisilicon0 device not found.");
        return HI_FAIL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev);
#endif
    if (wal_hipriv_getcountry(netdev, cc) != HI_SUCCESS) {
        oam_error_log0(0, 0, "wifi_get_country failed.");
        return HI_FAIL;
    }
    *len = (hi_s32)strlen(cc);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 开启/关闭系统低功耗模式并配置预期休眠时间
 输入参数  : [1]enable    使能开关
             [2]sleeptime 预期休眠时间
 返 回 值  : 错误码
***************************************************************************** */
hi_s32 hi_wifi_set_pm_switch(hi_u8 enable)
{
    oal_net_device_stru *net_dev = HI_NULL;
    wal_msg_write_stru  write_msg;
    hi_u32              ret;
    hi_u32              pm_cfg;

    net_dev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (net_dev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_set_pm_switch Hisilicon0 device not found.");
        return HI_FAIL;
    }

    /* 缓存低功耗标志 */
    set_under_ps(enable == PM_SWITCH_ON);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(net_dev);
#endif
    if (enable == PM_SWITCH_ON) {
        pm_cfg = PM_SWITCH_ON;
    } else if (enable == PM_SWITCH_OFF) {
        /* 关低功耗 */
        pm_cfg = PM_SWITCH_OFF;
    } else {
        oam_warning_log0(0, OAM_SF_COEX, "{hi_wifi_set_pm_switch::input parameter error!}\r\n");
        return HI_FAIL;
    }

    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_PM_SWITCH, sizeof(hi_u32));
    *((hi_u32 *)(write_msg.auc_value)) = pm_cfg;
    ret = wal_send_cfg_event(net_dev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_hipriv_pm_switch::return err code [%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 设置接口带宽
 输入参数  : [1]ifname
             [2]bw 带宽
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_set_bandwidth(const hi_char *ifname, unsigned char ifname_len, hi_wifi_bw bw)
{
    hi_char ac_bw[WAL_BW_STR_MAX_LEN] = {0};
    hi_char ifname_cpy[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    hi_char *ptr_ifname_cpy = ifname_cpy;

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return HI_FAIL;
    }
    oal_net_device_stru *netdev = oal_get_netdev_by_name(ptr_ifname_cpy);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_set_bandwidth device not found.");
        return HI_FAIL;
    }

    if ((bw > HI_WIFI_BW_LEGACY_20M) || (bw < HI_WIFI_BW_HIEX_5M)) {
        oam_error_log0(0, 0, "hi_wifi_set_bandwidth invalid bw.");
        return HI_FAIL;
    }

    strcpy_s(ac_bw, WAL_BW_STR_MAX_LEN, "20");
    if (bw == HI_WIFI_BW_HIEX_5M) {
        strcpy_s(ac_bw, WAL_BW_STR_MAX_LEN, "5");
    } else if (bw == HI_WIFI_BW_HIEX_10M) {
        strcpy_s(ac_bw, WAL_BW_STR_MAX_LEN, "10");
    }

    if (wal_netdev_stop(netdev) != HI_SUCCESS) {
        oam_error_log0(0, 0, "wal_netdev_stop failed.");
        return HI_FAIL;
    }

    if (wal_hipriv_set_bw(netdev, ac_bw) != HI_SUCCESS) {
        oam_error_log0(0, 0, "wal_hipriv_set_bw failed.");
        return HI_FAIL;
    }

    if (wal_netdev_open(netdev) != HI_SUCCESS) {
        oam_error_log0(0, 0, "wal_netdev_open failed.");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取接口带宽
 输入参数  : [1]ifname
 返 回 值  : hi_wifi_bw 带宽
**************************************************************************** */
hi_wifi_bw hi_wifi_get_bandwidth(const hi_char *ifname, unsigned char ifname_len)
{
    oal_net_device_stru              *netdev  = HI_NULL;
    hal_channel_assemble_enum_uint8  bw_index = WLAN_BAND_ASSEMBLE_20M;
    hi_wifi_bw                       bw       = HI_WIFI_BW_BUTT;
    hi_char ifname_cpy[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    hi_char *ptr_ifname_cpy = ifname_cpy;

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return HI_WIFI_BW_BUTT;
    }

    netdev = oal_get_netdev_by_name(ptr_ifname_cpy);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_get_bandwidth device not found.");
        return HI_WIFI_BW_BUTT;
    }

    if (wal_hipriv_get_bw(netdev, &bw_index) != HI_SUCCESS) {
        oam_error_log0(0, 0, "hi_wifi_get_bandwidth failed.");
        return HI_WIFI_BW_BUTT;
    }

    if (bw_index == WLAN_BAND_ASSEMBLE_5M) {
        bw = HI_WIFI_BW_HIEX_5M;
    } else if (bw_index == WLAN_BAND_ASSEMBLE_10M) {
        bw = HI_WIFI_BW_HIEX_10M;
    } else if (bw_index == WLAN_BAND_ASSEMBLE_20M) {
        bw = HI_WIFI_BW_LEGACY_20M;
    }

    return bw;
}

/* ****************************************************************************
 功能描述  : 发送用户定制的报文
 输入参数  : [1]ifname
             [2]buf  : 报文内容
             [3]len  : 报文长度
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_send_custom_pkt(const hi_char *ifname, const hi_u8 *buf, hi_u32 len)
{
    hi_s32 l_ret;
    wal_msg_write_stru write_msg;
    wlan_custom_pkt_stru *pkt = HI_NULL;

    if (buf == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_send_custom_pkt input param is NULL.");
        return HI_FAIL;
    }

    if ((len < HI_WIFI_CUSTOM_PKT_MIN_LEN) || (len > HI_WIFI_CUSTOM_PKT_MAX_LEN)) {
        oam_error_log0(0, 0, "hi_wifi_send_custom_pkt invalid len.");
        return HI_FAIL;
    }

    oal_net_device_stru *netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_send_custom_pkt device not found.");
        return HI_FAIL;
    }

    /* 复制用户数据 */
    hi_u8 *frame_data = (hi_u8 *)hi_malloc(HI_MOD_ID_WIFI_DRV, len);
    if (frame_data == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_send_custom_pkt: mem alloc err!}");
        return HI_FAIL;
    }

    if (memcpy_s(frame_data, len, (hi_u8 *)buf, len) != EOK) {
        hi_free(HI_MOD_ID_WIFI_DRV, frame_data);
        oam_error_log0(0, 0, "{hi_wifi_any_send::copy user data err!}");
        return HI_FAIL;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_CUSTOM_PKT, sizeof(wlan_custom_pkt_stru));
    pkt = (wlan_custom_pkt_stru *)(write_msg.auc_value);
    pkt->puc_data = frame_data;
    pkt->us_len = len;

    l_ret = (hi_s32)wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(wlan_custom_pkt_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(l_ret != HI_SUCCESS)) {
        hi_free(HI_MOD_ID_WIFI_DRV, frame_data);
        oam_error_log0(0, 0, "hi_wifi_send_custom_pkt failed.");
    }

    return l_ret;
}

hi_s32 hi_wifi_set_plat_ps_mode(hi_u8 sleep_mode)
{
    wal_msg_write_stru          write_msg;
    hi_u32                      ret;

    if (sleep_mode > HI_DEEP_SLEEP) {
        oam_error_log1(0, 0, "hi_wifi_set_plat_ps_mode:: invalid sleep_mode[%hhu]", sleep_mode);
        return HI_FAIL;
    }

    oal_net_device_stru *netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_arp_offload_setting:: device not found.");
        return HI_FAIL;
    }

    /* **************************************************************************
                              抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_STA_HW_PS_MODE, sizeof(hi_u8));
    *(hi_u8 *)(write_msg.auc_value) = sleep_mode;

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u8),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_PWR, "{wal_hipriv_sta_set_hw_ps_mode::return err code [%u]!}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_PROMIS
hi_u8 g_promis_filter = 0;

hi_u8 hwal_get_promis_filter(void)
{
    return g_promis_filter;
}

/* ****************************************************************************
 功能描述  : 设置混杂模式
 输入参数  : [1]net_dev
             [2]mode
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 wifi_promis_set(oal_net_device_stru *netdev, hi_u8 filter_value)
{
    hi_s32 l_ret;
    wal_msg_write_stru write_msg;

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_MONITOR_EN, sizeof(hi_u8));
    *((hi_u8 *)(write_msg.auc_value)) = filter_value; /* 设置配置命令参数 */
    g_promis_filter = filter_value;

    l_ret = (hi_s32)wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u8),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(l_ret != HI_SUCCESS)) {
        oam_error_log1(0, 0, "wifi_promis_set failed,mode=%u.", filter_value);
        return l_ret;
    }

    return l_ret;
}

/* ****************************************************************************
 功能描述  : 使能混杂模式
 输入参数  : [1]ifname
             [2]enable
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_promis_enable(const hi_char *ifname, hi_s32 enable, const hi_wifi_ptype_filter *filter)
{
    hi_s32 l_ret;
    hi_u8  filter_value;
    oal_net_device_stru *netdev = HI_NULL;

    if ((ifname == HI_NULL) || (filter == HI_NULL)) {
        oam_error_log0(0, 0, "hi_wifi_promis_enable parameter NULL.");
        return HI_FAIL;
    }

    filter_value = *((hi_u8 *)((hi_void *)filter)) & 0x0F;
    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_promis_enable device not found.");
        return HI_FAIL;
    }

    if ((enable != HI_FALSE) && (enable != HI_TRUE)) {
        oam_error_log0(0, 0, "hi_wifi_promis_enable invalid parameter.");
        return HI_FAIL;
    }

    if (enable == HI_FALSE) {
        filter_value = 0;
    }
    l_ret = wifi_promis_set(netdev, filter_value);
    return l_ret;
}

/* ****************************************************************************
 功能描述  : 注册混杂模式收包回调函数
 输入参数  : hi_wifi_promis_cb data_cb 收包回调函数
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_promis_set_rx_callback(hi_wifi_promis_cb data_cb)
{
    if (hisi_wlan_register_upload_frame_cb(data_cb) != 0) {
        oam_error_log0(0, 0, "wifi_set_promiscuous_rx_cb failed.");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}
#endif /* _PRE_WLAN_FEATURE_PROMIS */
#endif /* (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */

#ifdef _PRE_WLAN_FEATURE_CSI
/* ****************************************************************************
 功能描述  : 打开CSI数据上报开关
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_csi_start(hi_void)
{
    return wal_csi_switch(HI_SWITCH_ON);
}

/* ****************************************************************************
 功能描述  : 关闭CSI数据上报开关
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_csi_stop(hi_void)
{
    return wal_csi_switch(HI_SWITCH_OFF);
}

/* ****************************************************************************
 功能描述  : 注册CSI数据上报回调函数
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_csi_register_data_recv_func(hi_wifi_csi_data_cb data_cb)
{
    wal_csi_register_data_report_cb(data_cb);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : CSI参数配置
 输入参数  : ifname 接口名字
             hi_wifi_csi_entry 配置参数结构体数组
             report_min_interval CSI数据上报最小间隔
             entry_num 配置参数结构体数组长度
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_csi_set_config(const hi_char *ifname, hi_u32 report_min_interval, const hi_wifi_csi_entry *entry_list,
    hi_s32 entry_num)
{
    if (ifname == HI_NULL || entry_list == HI_NULL || entry_num == 0) {
        oam_error_log0(0, OAM_SF_CSI, "{hi_wifi_csi_set_config::param is error.}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (report_min_interval < OAL_CSI_DATA_REPORT_PERIOD) {
        report_min_interval = OAL_CSI_DATA_REPORT_PERIOD;
        oam_warning_log0(0, OAM_SF_CSI, "{hi_wifi_csi_set_config::csi smaller than 50ms, set to 50ms.}");
    }

    if (entry_num > OAL_CSI_MAX_MAC_NUM) {
        entry_num = OAL_CSI_MAX_MAC_NUM;
        oam_warning_log0(0, OAM_SF_CSI, "{hi_wifi_csi_set_config::num more than 6, ignore the entry num more than 6.}");
    }
    return wal_csi_set_config(ifname, report_min_interval, entry_list, entry_num);
}
#endif

/* ****************************************************************************
 功能描述  : 设置信道
 输入参数  : [1]ifname
             [2]channel
 返 回 值  : 错误码
**************************************************************************** */
hi_s32 hi_wifi_set_channel(const hi_char *ifname, unsigned char ifname_len, hi_s32 channel)
{
    hi_s32 l_ret;
    wal_msg_write_stru     write_msg;
    oal_net_device_stru    *netdev = HI_NULL;
    wal_msg_stru           *rsp_msg = HI_NULL;
    hi_char                ifname_cpy[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    hi_char                 *ptr_ifname_cpy = ifname_cpy;

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return HI_FAIL;
    }

    netdev = oal_get_netdev_by_name(ptr_ifname_cpy);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_set_channel device not found.");
        return HI_FAIL;
    }

    if ((channel > MAC_CHANNEL_FREQ_2_BUTT) || (channel <= 0)) {
        oam_error_log0(0, 0, "hi_wifi_set_channel invalid channel.");
        return HI_FAIL;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_CURRENT_CHANEL, sizeof(hi_s32));
    *((hi_s32 *)(write_msg.auc_value)) = channel;
    l_ret = (hi_s32)wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if ((l_ret != HI_SUCCESS) || (rsp_msg == HI_NULL)) {
        oam_error_log0(0, 0, "hi_wifi_set_channel failed.");
        return l_ret;
    }

    if (HI_SUCCESS != wal_check_and_release_msg_resp(rsp_msg)) {
        oam_error_log0(0, OAM_SF_ANY, "{hi_wifi_set_channel::wal_check_and_release_msg_resp failed.}");
        return HI_FAIL;
    }

    return l_ret;
}

/* ****************************************************************************
 功能描述  : 获取当前信道
 输入参数  : [1]ifname
 返 回 值  : 当前信道号
**************************************************************************** */
hi_s32 hi_wifi_get_channel(const hi_char *ifname, unsigned char ifname_len)
{
    oal_net_device_stru *netdev = HI_NULL;
    hi_u32 l_ret;
    wal_msg_stru *rsp_msg = HI_NULL;
    wal_msg_query_stru query_msg;
    wal_msg_rsp_stru *queue_rsp_msg = HI_NULL;
    hi_s32 channel;
    hi_char ifname_cpy[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    hi_char *ptr_ifname_cpy = ifname_cpy;

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return HI_WIFI_INVALID_CHANNEL;
    }

    netdev = oal_get_netdev_by_name(ptr_ifname_cpy);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_get_channel device not found.");
        return HI_WIFI_INVALID_CHANNEL;
    }

    query_msg.wid = WLAN_CFGID_CURRENT_CHANEL;

    /* 发送消息 */
    l_ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_QUERY, WAL_MSG_WID_LENGTH, (hi_u8 *)&query_msg, HI_TRUE, &rsp_msg);
    if ((l_ret != HI_SUCCESS) || (rsp_msg == HI_NULL)) {
        oam_error_log1(0, OAM_SF_ANY, "{hi_wifi_get_channel::return err code %d!}\r\n", l_ret);
        return HI_WIFI_INVALID_CHANNEL;
    }

    /* 处理返回消息 */
    queue_rsp_msg = (wal_msg_rsp_stru *)(rsp_msg->auc_msg_data);
    channel = *((hi_s32 *)(queue_rsp_msg->auc_value));
    oal_free(rsp_msg);

    return channel;
}

#ifdef _PRE_WLAN_FEATURE_ANY
/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
oal_net_device_stru *g_any_netdev = HI_NULL;

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : hi_wifi_any_init
 功能描述  : ANY初始化，主要是分配内存，方便后续维护对端设备信息。
 输入参数  : ifname是字符串"wlan0"或"wlan1"等，表示用于收发ANY帧的接口名称
 返 回 值  : HI_SUCCESS 上报成功，其它错误码 上报失败

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_s32 hi_wifi_any_init(const hi_char *ifname)
{
    if (ifname == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_any_init:ifname is NULL.}");
        return HI_FAIL;
    }

    /* 一个设备只允许在一个VAP初始化ANY，已经初始化则需要先调用去初始化 */
    if (g_any_netdev != HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_any_init:any has already been initialized, please deinit first.}");
        return HI_FAIL;
    }

    g_any_netdev = oal_get_netdev_by_name(ifname);
    if (g_any_netdev == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_any_init:invalid ifname.}");
        return HI_FAIL;
    }

    return wal_any_global_config(WLAN_CFGID_ANY_INIT, g_any_netdev);
}

/* ****************************************************************************
 函 数 名  : hi_wifi_any_deinit
 功能描述  : ANY去初始化，做清理工作，释放内存
 返 回 值  : HI_SUCCESS 上报成功，其它错误码 上报失败

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_s32 hi_wifi_any_deinit(hi_void)
{
    (hi_void)wal_any_global_config(WLAN_CFGID_ANY_DEINIT, g_any_netdev);
    g_any_netdev = HI_NULL;
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hi_wifi_any_add_peer
 功能描述  : 添加指定的点对点对端设备信息
 输入参数  : wal_any_peer_info_stru包含用户下发的对端MAC和信道以及密钥等信息
 返 回 值  : HI_SUCCESS 上报成功，其它错误码 上报失败

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_s32 hi_wifi_any_add_peer(const hi_wifi_any_peer_info *puc_peer_info)
{
    wal_msg_write_stru       write_msg;
    hi_wifi_any_peer_info   *msg_peer_info = HI_NULL;
    hi_s32                   l_ret;
    hi_u8                    auc_mac[ETH_ALEN] = {0};

    if (puc_peer_info == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hi_wifi_any_add_peer:puc_peer_info is NULL.}");
        return HI_FAIL;
    }

    /* 检查对端MAC是否有效，需要为单播地址 */
    if (ether_is_multicast(puc_peer_info->mac) || (memcmp(auc_mac, puc_peer_info->mac, ETH_ALEN) == 0)) {
        oam_error_log0(0, 0, "{hi_wifi_any_add_peer:MAC address should be non-zero unicast address.}");
        return HI_FAIL;
    }

    if (g_any_netdev == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_any_add_peer:g_pst_netdev is NULL, need to initialize ANY.}");
        return HI_FAIL;
    }
    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ANY_ADD_PEER_INFO, sizeof(hi_wifi_any_peer_info));

    /* 设置配置命令参数 */
    msg_peer_info = (hi_wifi_any_peer_info *)(write_msg.auc_value);
    if (memcpy_s(msg_peer_info, sizeof(hi_wifi_any_peer_info), puc_peer_info, sizeof(hi_wifi_any_peer_info)) != EOK) {
        oam_error_log0(0, 0, "{hi_wifi_any_del_peer::mem safe function err!}");
        return HI_FAIL;
    }

    l_ret = (hi_s32)wal_send_cfg_event(g_any_netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_wifi_any_peer_info), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(l_ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hi_wifi_any_add_peer::return err code [%d]!}", l_ret);
        return l_ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hi_wifi_any_del_peer
 功能描述  : 删除指定的点对点对端设备信息
 输入参数  : mac 对端MAC地址数组，只取前六字节
             len 数组长度
 返 回 值  : HI_SUCCESS 上报成功，其它错误码 上报失败

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 hi_wifi_any_del_peer(const hi_u8 *mac, hi_u8 len)
{
    wal_msg_write_stru        write_msg;
    hi_u8                    *mac_addr          = HI_NULL;
    hi_s32                    l_ret;
    hi_u8                     auc_mac[ETH_ALEN] = {0};

    if ((mac == HI_NULL) || (len < ETH_ALEN)) {
        oam_error_log0(0, OAM_SF_ANY, "{hi_wifi_any_del_peer:mac or length is invalid.}");
        return HI_FAIL;
    }

    /* 检查对端MAC是否有效，需要为单播地址 */
    if (ether_is_multicast(mac) || (memcmp(auc_mac, mac, ETH_ALEN) == 0)) {
        oam_error_log0(0, 0, "{hi_wifi_any_del_peer:MAC address should be non-zero unicast address.}");
        return HI_FAIL;
    }

    if (g_any_netdev == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_any_del_peer:g_pst_netdev is NULL, need to initialize ANY.}");
        return HI_FAIL;
    }
    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ANY_DEL_PEER_INFO, ETH_ALEN);

    /* 设置配置命令参数 */
    mac_addr = (hi_u8 *)(write_msg.auc_value);
    if (memcpy_s(mac_addr, ETH_ALEN, mac, ETH_ALEN) != EOK) {
        oam_error_log0(0, 0, "{hi_wifi_any_del_peer::mem safe function err!}");
        return HI_FAIL;
    }

    l_ret = (hi_s32)wal_send_cfg_event(g_any_netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + ETH_ALEN,
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(l_ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hi_wifi_any_del_peer::return err code [%d]!}", l_ret);
        return l_ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hi_wifi_any_send
 功能描述  : 处理用户下发的数据，发送到ANY对端
 输入参数  : mac      对端MAC地址
             mac_len  mac数组长度
             data     用户数据
             data_len 用户数据长度
             seq      用户下发的序列号
 返 回 值  : 发送成功返回HI_SUCCESS，否则返回其他值

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 hi_wifi_any_send(const hi_u8 *mac, hi_u8 mac_len, hi_u8 *data, hi_u16 data_len, hi_u8 seq)
{
    wal_msg_write_stru           write_msg;
    oal_any_peer_param_stru      peer_param;
    hi_u32                       ret;

    if ((mac == HI_NULL) || (mac_len != ETH_ALEN)) {
        oam_error_log0(0, 0, "hi_wifi_any_send: parameter NULL.");
        return HI_FAIL;
    }

    if ((data == HI_NULL) || (data_len == 0) || (data_len > WIFI_ANY_MAX_USER_DATA)) {
        oam_error_log0(0, 0, "hi_wifi_any_send: data length is invalid [1-250].");
        return HI_FAIL;
    }

    if (g_any_netdev == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_any_send: need to initialize ANY first!}");
        return HI_FAIL;
    }

    if (memcpy_s(peer_param.auc_mac, sizeof(oal_any_peer_param_stru), mac, ETH_ALEN) != EOK) {
        oam_error_log0(0, 0, "{hi_wifi_any_send: mem safe function err!}");
        return HI_FAIL;
    }

    peer_param.puc_data = data;
    peer_param.us_len = data_len; /* 用户真实有效的数据长度 */
    peer_param.seq_num = seq;
    peer_param.pad_num = 0; /* 后面的加密功能会自动填充对齐字节，这里不用考虑填充 */
    peer_param.channel = 0;
    peer_param.encrypt = 0;
    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ANY_SEND_PEER_DATA, sizeof(oal_any_peer_param_stru));

    /* 设置配置命令参数 */
    if (memcpy_s(write_msg.auc_value, WAL_MSG_WRITE_MAX_LEN, &peer_param, sizeof(oal_any_peer_param_stru)) != EOK) {
        oam_error_log0(0, 0, "{hi_wifi_any_send::mem safe function err!}");
        return HI_FAIL;
    }

    ret = wal_send_cfg_event(g_any_netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(oal_any_peer_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hi_wifi_any_send::return err code [%d]!}", ret);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hi_wifi_any_set_callback
 功能描述  : 注册ANY设备接口的报文发送和接收回调函数，之前注册过的会被覆盖替换掉
 输入参数  : send_cb从用户传过来的发送回调函数
             recv_cb从用户传过来的接收回调函数
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void hi_wifi_any_set_callback(hi_wifi_any_send_complete_cb send_cb, hi_wifi_any_recv_cb recv_cb)
{
    wal_any_set_callback(send_cb, recv_cb);
    return;
}

/* ****************************************************************************
 函 数 名  : hi_wifi_any_fetch_peer
 功能描述  : 获取指定索引对应的对端信息
 输入参数  : index索引值，从0开始，不超过对端个数，否则获取失败
 输出参数  : hi_wifi_any_peer_info 存储对端信息的结构
 返 回 值  : HI_SUCCESS 上报成功，其它错误码上报失败

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 hi_wifi_any_fetch_peer(hi_u8 index, hi_wifi_any_peer_info *peer)
{
    wal_msg_write_stru       write_msg;
    hi_u32                   ret;
    mac_vap_stru            *mac_vap = HI_NULL;
    hmac_vap_stru           *hmac_vap = HI_NULL;

    if ((index >= HMAC_ANY_MAX_PEER_NUM) || (peer == HI_NULL)) {
        oam_error_log0(0, 0, "{hi_wifi_any_fetch_peer:parameter is invalid.}");
        return HI_FAIL;
    }

    if (g_any_netdev == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_any_fetch_peer:g_pst_netdev is NULL, need to initialize ANY.}");
        return HI_FAIL;
    }

    mac_vap = oal_net_dev_priv(g_any_netdev);
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_error_log0(0, 0, "{hi_wifi_any_fetch_peer:the vap is NULL.}");
        return HI_FAIL;
    }

    /* 填写事件头 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ANY_FETCH_PEER_INFO, 4); /* 4 事件头长度 */
    /* 填写消息体 */
    write_msg.auc_value[0] = index;
    ret = wal_send_cfg_event(g_any_netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + 4, /* 4 长度增4 */
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{hi_wifi_any_fetch_peer::wal_send_cfg_event return err code %d!}\r\n", ret);
        return HI_FAIL;
    }

    hmac_vap = hmac_vap_get_vap_stru(mac_vap->vap_id);
    if (oal_unlikely(hmac_vap == HI_NULL)) {
        oam_error_log0(0, 0, "{hi_wifi_any_fetch_peer:the hmac vap is NULL.}");
        return HI_FAIL;
    }

    ret = wal_any_wait_query_result(hmac_vap, peer);
    if (ret != HI_SUCCESS) {
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : hi_wifi_any_discover_peer
 功能描述  : 发起扫描发现ANY对端设备信息，包括MAC地址，信道和接收到的cookie等信息
 输入参数  : hi_wifi_any_scan_result_cb 扫描完成之后的结果回调处理函数
 返 回 值  : 成功返回0，失败返回-1

 修改历史      :
  1.日    期   : 2019年1月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 hi_wifi_any_discover_peer(hi_wifi_any_scan_result_cb cb)
{
    wal_any_set_scan_callback(cb);

    /* 发起扫描 */
    return wal_any_global_config(WLAN_CFGID_ANY_SCAN, g_any_netdev);
}
#endif

#ifdef _PRE_WLAN_FEATURE_WOW
/* 设置 唤醒包帧体内容 */
hi_u8 hi_wifi_wow_set_pattern(const hi_char *ifname, hi_u32 type, hi_u8 index, hi_char *pattern)
{
    hi_u32 ret;
    wal_msg_write_stru write_msg;
    oal_net_device_stru *netdev = HI_NULL;
    hmac_cfg_wow_pattern_param_stru cfg_wow_param = { 0 };

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_wow_set_pattern:: device not found.");
        return HI_FAIL;
    }

    if (type > HI_WOW_PATTERN_CLR) {
        oam_error_log0(0, 0, "hi_wifi_wow_set_pattern:: type is invalid");
        return HI_FAIL;
    }

    if (type == HI_WOW_PATTERN_ADD) {
        ret = wal_get_add_wow_pattern_param(index, pattern, &cfg_wow_param);
        if (ret != HI_SUCCESS) {
            oam_error_log0(0, OAM_SF_ANY, "{hi_wifi_wow_set_pattern::get add param failed.}");
            return HI_FAIL;
        }
        /* 设置pattern value参数 */
        if (memcpy_s(&((hmac_cfg_wow_pattern_param_stru *)(write_msg.auc_value))->auc_pattern_value[0],
            WAL_HIPRIV_CMD_NAME_MAX_LEN, cfg_wow_param.auc_pattern_value, cfg_wow_param.pattern_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_hipriv_set_wow_pattern::copy pattern value failed.}");
            return HI_FAIL;
        }
    } else if (type == HI_WOW_PATTERN_DEL) {
        if (index >= WOW_NETPATTERN_MAX_NUM) {
            oam_error_log1(0, 0, "hi_wifi_wow_set_pattern:: invalid_index[%d]", index);
            return HI_FAIL;
        }
        cfg_wow_param.us_pattern_option = MAC_WOW_PATTERN_PARAM_OPTION_DEL;
        cfg_wow_param.us_pattern_index = index;
    } else if (type == HI_WOW_PATTERN_CLR) {
        cfg_wow_param.us_pattern_option = MAC_WOW_PATTERN_PARAM_OPTION_CLR;
    } else {
        oam_warning_log0(0, 0, "hi_wifi_wow_set_pattern:: invaliad patter OPTION");
        return HI_FAIL;
    }

    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_WOW_PATTERN, sizeof(hmac_cfg_wow_pattern_param_stru));
    /* 设置pattern option参数 */
    ((hmac_cfg_wow_pattern_param_stru *)(write_msg.auc_value))->us_pattern_option = cfg_wow_param.us_pattern_option;
    /* 设置pattern index参数 */
    ((hmac_cfg_wow_pattern_param_stru *)(write_msg.auc_value))->us_pattern_index = cfg_wow_param.us_pattern_index;
    /* 设置pattern pattern len参数 */
    ((hmac_cfg_wow_pattern_param_stru *)(write_msg.auc_value))->pattern_len = cfg_wow_param.pattern_len;
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hmac_cfg_wow_pattern_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hi_wifi_wow_set_pattern:: return err code[%u]!}", ret);
    }
    return ret;
}

/* wow 唤醒/睡眠请求 */
hi_u8 hi_wifi_wow_host_sleep_switch(const hi_char *ifname, hi_u8 en)
{
    wal_msg_write_stru       write_msg;
    oal_net_device_stru     *netdev = HI_NULL;
    hi_u32                   ret;

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_wow_enable_switch:: device not found.");
        return HI_FAIL;
    }

    if ((en != 0) && (en != 1)) { /* 0: disable, 1: enable */
        oam_error_log1(0, 0, "hi_wifi_wow_host_sleep_switch:: invalid enable value[%d]", en);
        return HI_FAIL;
    }

    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_WOW_ACTIVATE_EN, sizeof(hi_s32));
    *((hi_u8 *)(write_msg.auc_value)) = en; /* 设置配置命令参数 */

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hi_wifi_wow_enable_switch::return err code[%u]!}", ret);
        return ret;
    }

    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_HOST_SLEEP_EN, sizeof(hi_s32));
    *((hi_s32 *)(write_msg.auc_value)) = en; /* 设置配置命令参数 */

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hi_wifi_wow_host_sleep_switch::return err code[%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
/* ****************************************************************************
 功能描述  : arp offload 开启/关闭
 输入参数  : [1] en 1-开启，0-关闭
             [2] ip ip地址，注意必须是32bit主机字节序
 返 回 值  : 成功返回0，失败返回-1

 修改历史      :
  1.日    期   : 2019年10月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hi_wifi_arp_offload_setting(const hi_char *ifname, hi_u8 en, hi_u32 ip)
{
    wal_msg_write_stru  write_msg;
    oal_net_device_stru *netdev = HI_NULL;
    hi_u32              ret;
    hi_u16              len;
    mac_ip_addr_config_stru ip_addr_cfg = {0};

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_arp_offload_setting:: device not found.");
        return HI_FAIL;
    }

    if (en >= MAC_IP_OPER_BUTT) {
        oam_error_log0(0, 0, "hi_wifi_arp_offload_setting:: en error.");
        return HI_FAIL;
    }

    ip_addr_cfg.type = MAC_CONFIG_IPV4;
    ip_addr_cfg.oper = en;
    ip_addr_cfg.ip.ipv4 = ip;
    /* 打印IPV4地址[2]和[3] */
    oam_info_log3(0, 0, "{hi_wifi_arp_offload_setting:: oper: %d, ip[xx.xx.%d.%d] succ.}", en,
        *(((hi_char *)&ip_addr_cfg.ip.ipv4) + 2), *(((hi_char *)&ip_addr_cfg.ip.ipv4) + 3)); /* 2/3 偏置 */

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    if (memcpy_s(write_msg.auc_value, sizeof(mac_ip_addr_config_stru), (const hi_void *)&ip_addr_cfg,
        sizeof(mac_ip_addr_config_stru)) != EOK) {
        oam_error_log0(0, 0, "{hi_wifi_arp_offload_setting:: mem safe function err!}");
        return HI_FAIL;
    }
    len = sizeof(mac_ip_addr_config_stru);

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_ARP_OFFLOAD_SETTING, len);
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + len, (hi_u8 *)&write_msg,
        HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_PWR, "{hi_wifi_arp_offload_setting:: wal_send_cfg_event error[%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_DHCP_OFFLOAD
/* ****************************************************************************
 功能描述  : dhcp offload 开启/关闭
 输入参数  : [1] en 1-开启，0-关闭
             [2] ip ip地址，注意必须是32bit主机字节序
 返 回 值  : 成功返回0，失败返回-1

 修改历史      :
  1.日    期   : 2019年10月17日
    作    者   : HiSilicon
    修改内容   : 新生成函数
**************************************************************************** */
hi_u8 hi_wifi_dhcp_offload_setting(const hi_char *ifname, hi_u8 en, hi_u32 ip)
{
    wal_msg_write_stru  write_msg;
    oal_net_device_stru *netdev = HI_NULL;
    hi_u32              ret;
    hi_u16              len;
    mac_ip_addr_config_stru ip_addr_cfg = {0};

    if (ifname == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_dhcp_offload_setting:: ifname is NULL.}");
        return HI_FAIL;
    }

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_dhcp_offload_setting:: device not found.");
        return HI_FAIL;
    }

    if (en >= MAC_IP_OPER_BUTT) {
        oam_error_log0(0, 0, "hi_wifi_dhcp_offload_setting:: en error.");
        return HI_FAIL;
    }

    ip_addr_cfg.type = MAC_CONFIG_IPV4;
    ip_addr_cfg.oper = en;
    ip_addr_cfg.ip.ipv4 = ip;
    /* 打印IPV4地址[2]和[3] */
    oam_info_log3(0, 0, "{hi_wifi_dhcp_offload_setting:: oper: %d, ip[xx.xx.%d.%d] succ.}", en,
        *(((hi_char *)&ip_addr_cfg.ip.ipv4) + 2), *(((hi_char *)&ip_addr_cfg.ip.ipv4) + 3)); /* 2/3 偏置 */

    /* **************************************************************************
         抛事件到wal层处理
    ************************************************************************** */
    if (memcpy_s(write_msg.auc_value, sizeof(mac_ip_addr_config_stru), (const hi_void *)&ip_addr_cfg,
        sizeof(mac_ip_addr_config_stru)) != EOK) {
        oam_error_log0(0, 0, "{hi_wifi_dhcp_offload_setting:: mem safe function err!}");
        return HI_FAIL;
    }
    len = sizeof(mac_ip_addr_config_stru);

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DHCP_OFFLOAD_SETTING, len);
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + len, (hi_u8 *)&write_msg,
        HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_PWR, "{hi_wifi_dhcp_offload_setting:: wal_send_cfg_event error[%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif

#ifdef _PRE_WLAN_FEATURE_BTCOEX
hi_u32 hi_wifi_btcoex_enable(const hi_char *ifname, hi_bool enable, hi_u8 mode, hi_u8 share_ant)
{
    wal_msg_write_stru  write_msg;
    oal_net_device_stru *netdev = HI_NULL;
    hi_u32              ret;
    hi_u16              len;

    if (ifname == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_btcoex_enable:: ifname is NULL.}");
        return HI_FAIL;
    }

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_btcoex_enable:: device not found.");
        return HI_FAIL;
    }

    if (enable == 0) {
        *(hi_u8 *)(write_msg.auc_value) = enable;
        len = sizeof(hi_u8);
    } else if (enable == 1) {
        write_msg.auc_value[0] = enable;
        write_msg.auc_value[1] = mode;
        write_msg.auc_value[2] = share_ant; /* 2: 第3位 */
        len = sizeof(hi_u8) * 3;            /* 个数为3 */
    } else {
        oam_warning_log0(0, OAM_SF_COEX, "{hi_wifi_btcoex_enable::input parameter error!}\r\n");
        return HI_SUCCESS;
    }
    /* **************************************************************************
         抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_BTCOEX_ENABLE, len);
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + len, (hi_u8 *)&write_msg,
        HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_PWR, "{hi_wifi_btcoex_enable:: wal_send_cfg_event error[%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  :
 输入参数  : [1]type 重传设置类型：1、设置数据帧重传 2、设置管理帧重传 3、设置时间重传
           [2]limit 重传限制：重传次数0~15，重传时间0~200
 输出参数  : 无
 返 回 值: 是否成功
**************************************************************************** */
hi_u32 hi_wifi_set_retry_params(const hi_char *ifname, hi_u8 type, hi_u8 limit)
{
    oal_net_device_stru *netdev = HI_NULL;
    mac_cfg_retry_param_stru *set_param = HI_NULL;
    wal_msg_write_stru write_msg;
    hi_u32 ret;

    if (ifname == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_set_retry_params:: ifname is NULL.}");
        return HI_FAIL;
    }

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_set_retry_params:: device not found.");
        return HI_FAIL;
    }

    if (type >= MAC_CFG_RETRY_TYPE_BUTT) {
        oam_error_log0(0, 0, "hi_wifi_set_retry_params:: type is invalid.");
        return HI_FAIL;
    }

    if (type == MAC_CFG_RETRY_DATA || type == MAC_CFG_RETRY_MGMT) {
        if (limit > HI_WIFI_RETRY_MAX_NUM) {
            oam_error_log0(0, 0, "hi_wifi_set_retry_params:: tpye is date or mgmt,limit is invalid.");
            return HI_FAIL;
        }
    }

    if (type == MAC_CFG_RETRY_TIMEOUT) {
        if (limit > HI_WIFI_RETRY_MAX_TIME) {
            oam_error_log0(0, 0, "hi_wifi_set_retry_params:: tpye is timeout,limit is invalid.");
            return HI_FAIL;
        }
    }

    set_param = (mac_cfg_retry_param_stru *)(write_msg.auc_value);
    set_param->type = type;
    set_param->limit = limit;

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_RETRY_LIMIT, sizeof(mac_cfg_retry_param_stru));

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_retry_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_hipriv_set_retry_limit::return err code [%u]!}\r\n", ret);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

hi_u32 hi_wifi_set_cca_threshold(const hi_char *ifname, hi_s8 threshold)
{
    wal_msg_write_stru          write_msg = {0};
    hi_s32                     *param = HI_NULL;
    oal_net_device_stru        *netdev = HI_NULL;
    hi_u32                      ret;

    if (ifname == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_set_cca_threshold:: ifname is NULL.}");
        return HI_FAIL;
    }
    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_set_cca_threshold:: device not found.");
        return HI_FAIL;
    }
    /* 低功耗状态下不允许执行 */
    if (is_under_ps()) {
        oam_warning_log0(0, 0, "under ps mode,can not exec cmd");
        return HI_FAIL;
    }
    /* 解析并设置配置命令参数 */
    param = (hi_s32 *)(write_msg.auc_value);
    *param = threshold;

    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_CCA_TH, sizeof(hi_s32));

    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hi_wifi_set_cca_threshold::return err code [%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_u32 hi_wifi_set_tpc(const char *ifname, unsigned char ifname_len, hi_u32 tpc_value)
{
    oal_net_device_stru  *netdev = HI_NULL;
    hi_u32               mode;
    hi_char              ifname_cpy[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    hi_char              *ptr_ifname_cpy = ifname_cpy;

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return HI_FAIL;
    }
    /* 低功耗状态下不允许执行 */
    if (is_under_ps()) {
        oam_warning_log0(0, 0, "under ps mode,can not exec cmd");
        return HI_FAIL;
    }
    netdev = oal_get_netdev_by_name(ptr_ifname_cpy);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_set_tpc:: device not found.");
        return HI_FAIL;
    }
    if ((tpc_value != 0) && (tpc_value != 1)) {
        oam_error_log0(0, OAM_SF_ANY, "{hi_wifi_set_tpc::tpc_value invalid!}");
        return HI_FAIL;
    }
    mode = (tpc_value == 0) ? 0 : 2; /* 0:禁用tpc模式,2:自适应功率模式 */
    wal_set_tpc_mode(netdev, mode);

    return HI_SUCCESS;
}

/* 从device侧获取算法计算的各种不同业务类型的实际流量
 * 由于3881芯片host、device分离，没有直接从device获取数据的途径，因此采用异步流程
 * 在host发送事件通知device进行处理，随后device通过sdio report机制异步上报host
 */
hi_u32 hi_wifi_get_tx_params(const char *ifname, unsigned char ifname_len)
{
    hi_u32 ret;
    oal_net_device_stru *netdev = HI_NULL;
    wal_msg_write_stru write_msg = { 0 };
    hi_char ifname_cpy[WIFI_IFNAME_MAX_SIZE + 1] = {0};

    (void)ifname_len;
    if (ifname == HI_NULL) {
        oam_error_log0(0, 0, "{hi_wifi_get_tx_params:: input point is NULL}");
        return HI_FAIL;
    }

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        oam_error_log0(0, 0, "{hi_wifi_get_tx_params:: input param is invalid}");
        return HI_FAIL;
    }

    netdev = oal_get_netdevice_by_name(ifname_cpy, sizeof(ifname_cpy));
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "hi_wifi_get_tx_params:: device not found.");
        return HI_FAIL;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_NOTIFY_GET_TX_PARAMS, sizeof(hi_u32));
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hi_wifi_get_tx_params::return err code [%u]!}\r\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void hi_wifi_register_tx_params_callback(hi_wifi_report_tx_params_callback func)
{
    wal_register_tx_params_callback(func);
}


int hi_wifi_register_driver_event_callback(hi_wifi_driver_event_cb event_cb)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (oal_register_ioctl(event_cb) != HI_SUCCESS) {
        return HISI_FAIL;
    }
    return HISI_OK;
#else
    return HISI_OK;
#endif
}

hi_s32 hi_wifi_soft_reset_device(hi_void)
{
    hi_wifi_plat_pm_disable();
    oal_sdio_sleep_dev(oal_get_sdio_default_handler());
    return oal_sdio_send_msg(oal_get_sdio_default_handler(), H2D_MSG_PM_WLAN_OFF);
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
oal_module_init(hi_linux_wifi_init);
oal_module_exit(hi_linux_wifi_deinit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hisilicon Wireless Lan Driver");
MODULE_AUTHOR("Hisilicon Wifi Team");
MODULE_VERSION("V1.0.0_000.20191223");
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
