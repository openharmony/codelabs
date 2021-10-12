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
#include "net_adpater.h"
#include "hdf_base.h"
#include "hmac_config.h"
#include "oal_util.h"
#include "wal_customize.h"
#include "wal_event_msg.h"
#include "wal_ioctl.h"
#include "wal_main.h"
#include "oal_net.h"
#include "wal_scan.h"
#include "hdf_wlan_utils.h"

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
#include "wal_linux_atcmdsrv.h"
#endif

#ifdef _PRE_WLAN_FEATURE_MESH
#include "dmac_ext_if.h"
#include "hmac_vap.h"
#include "hmac_user.h"
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
#include "wal_cfg80211.h"
#endif
#include "plat_firmware.h"
#include "eapol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern hi_u8 wal_lwip_set_hwaddr(oal_net_device_stru *netDev, uint8_t *addr, uint8_t len);
/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
netif_flow_ctrl_enum g_netif_flow_ctrl = NETIF_FLOW_CTRL_OFF;
wal_dev_addr_stru g_dev_addr = { 0 };
hi_u8 g_efuseMacExist = false;

hi_u8 g_wait_mac_set = 1;
#define netif_is_not_ready() (NETIF_FLOW_CTRL_ON == g_netif_flow_ctrl)

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
/* ****************************************************************************
 功能描述  : 随机化初始mac地址 让单板启动时携带默认mac
 修改历史      :
 1.日    期   : 2019年5月15日
    作    者   : HiSilicon
    修改内容   : 新生成函数
 2.日    期   : 2019年5月29日
    修改内容   :增加从efuse读取mac addr
**************************************************************************** */
hi_void wal_init_dev_addr(hi_void)
{
    hi_u32 wait_count = 10;
    if ((cfg_get_mac(&g_dev_addr.ac_addr[0], ETHER_ADDR_LEN)) &&
        (wal_macaddr_check(&g_dev_addr.ac_addr[0]) == HI_SUCCESS)) { /* 优先从nv读取MAC地址 */
        return;
    } else { /* 未读到正常配置的MAC地址,尝试从efuse读，或生成随机地址 */
        /* 生成随机地址 */
        oal_random_ether_addr(g_dev_addr.ac_addr, WLAN_MAC_ADDR_LEN);
        g_dev_addr.ac_addr[1] = 0x11;
        g_dev_addr.ac_addr[2] = 0x31; /* 2 地址第3位 */
        g_dev_addr.us_status = 0;
        g_efuseMacExist = false;
        /* 尝试下发事件从efuse读取MAC地址 */
        hi_u32 ret = wal_get_efuse_mac_addr();
        if (oal_unlikely(ret != HI_SUCCESS)) {
            oam_warning_log1(0, OAM_SF_ANY, "{wal_get_efuse_mac::return err code [%u]!}", ret);
        }
        while (g_wait_mac_set == 1 && (wait_count--) > 0) {
            msleep(1);
        }
        if (g_wait_mac_set == 1) {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_init_dev_addr::read addr from efuse timeout!}");
        }
    }
}

hi_u32 wal_get_efuse_mac_addr(hi_void)
{
    oal_net_device_stru *netdev;
    wal_msg_write_stru write_msg = { 0 };

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == HI_NULL) {
        oam_error_log0(0, 0, "wal_get_efuse_mac_addr::sta device not fonud.");
        return HI_FAIL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_dev_put(netdev);
#endif

    /* **************************************************************************
                                抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_GET_EFUSE_MAC_ADDR, sizeof(hi_s32));
    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)(&write_msg), HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_get_efuse_mac_addr::return err code [%u]!}\r\n", ret);
    }

    return ret;
}

hi_u32 wal_set_dev_addr_from_efuse(const hi_char *pc_addr, hi_u8 mac_len)
{
    if (pc_addr == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wal_set_dev_addr:: pc_addr is NULL!");
        return HI_FAIL;
    }
    if (wal_macaddr_check((hi_u8 *)pc_addr) != HI_SUCCESS) {
        g_wait_mac_set = 2;
        oam_warning_log0(0, OAM_SF_ANY, "wal_set_dev_addr:: mac from efuse is zero!");
        return HI_FAIL;
    }

    if (memcpy_s(g_dev_addr.ac_addr, ETHER_ADDR_LEN, pc_addr, mac_len) != EOK) {
        oam_error_log0(0, 0, "wal_set_dev_addr:: memcpy_s FAILED");
        return HI_FAIL;
    }

    g_efuseMacExist = true;
    g_wait_mac_set = 0;
    return HI_SUCCESS;
}


hi_u32 wal_set_dev_addr(const hi_char *pc_addr, hi_u8 mac_len)
{
    hi_u32 count = 0;

    if (pc_addr == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wal_set_dev_addr:: pc_addr is NULL!");
        return HI_FAIL;
    }

    count = NetDevGetRegisterCount();

    /* 存在业务vap不能修改mac地址 */
    if (count > 1) {
        oam_error_log0(0, OAM_SF_ANY, "wal_set_dev_addr::vap exist, could not set mac address!");
        return HI_FAIL;
    }

    if (memcpy_s(g_dev_addr.ac_addr, ETHER_ADDR_LEN, pc_addr, mac_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_set_dev_addr::mem safe function err!}");
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* 根据设备类型分配mac地址索引 */
static wal_addr_idx wal_get_dev_addr_idx(nl80211_iftype_uint8 type)
{
    wal_addr_idx addr_idx = WAL_ADDR_IDX_BUTT;

    switch (type) {
        case PROTOCOL_80211_IFTYPE_STATION:
            addr_idx = WAL_ADDR_IDX_STA0;
            break;
        case PROTOCOL_80211_IFTYPE_AP:
        case PROTOCOL_80211_IFTYPE_P2P_CLIENT:
        case PROTOCOL_80211_IFTYPE_P2P_GO:
        case PROTOCOL_80211_IFTYPE_MESH_POINT:
            addr_idx = WAL_ADDR_IDX_AP0;
            break;
        case PROTOCOL_80211_IFTYPE_P2P_DEVICE:
            addr_idx = WAL_ADDR_IDX_STA2;
            break;
        default:
            oam_error_log1(0, OAM_SF_ANY, "wal_get_dev_addr_idx:: dev type [%d] is not supported !", type);
            break;
    }

    return addr_idx;
}
/* 建议5.5误报，在166行对其指向内容进行了修改 */
hi_u32 wal_get_dev_addr(hi_u8 *pc_addr, hi_u8 addr_len, nl80211_iftype_uint8 type) /* 建议5.5误报，166行有元素赋值 */
{
    hi_u16 us_addr[ETHER_ADDR_LEN];
    hi_u32 tmp;
    wal_addr_idx addr_idx;

    if (pc_addr == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wal_get_dev_addr:: pc_addr is NULL!");
        return HI_FAIL;
    }

    addr_idx = wal_get_dev_addr_idx(type);
    if (addr_idx >= WAL_ADDR_IDX_BUTT) {
        return HI_FAIL;
    }

    for (tmp = 0; tmp < ETHER_ADDR_LEN; tmp++) {
        us_addr[tmp] = (hi_u16)g_dev_addr.ac_addr[tmp];
    }

    /* 1.低位自增 2.高位取其进位 3.低位将进位位置0 */
    us_addr[5] += addr_idx;                      /* 5 地址第6位 */
    us_addr[4] += ((us_addr[5] & (0x100)) >> 8); /* 4 地址第5位 5 地址第6位 8 右移8位 */
    us_addr[5] = us_addr[5] & (0xff);            /* 5 地址第6位 */
    /* 最低位运算完成,下面类似 */
    us_addr[3] += ((us_addr[4] & (0x100)) >> 8); /* 3 地址第4位 4 地址第5位 8 右移8位 */
    us_addr[4] = us_addr[4] & (0xff);            /* 4 地址第5位 */
    us_addr[2] += ((us_addr[3] & (0x100)) >> 8); /* 2 地址第3位 3 地址第4位 8 右移8位 */
    us_addr[3] = us_addr[3] & (0xff);            /* 3 地址第4位 */
    us_addr[1] += ((us_addr[2] & (0x100)) >> 8); /* 1 地址第2位 2 地址第3位 8 右移8位 */
    us_addr[2] = us_addr[2] & (0xff);            /* 2 地址第3位 */
    us_addr[0] += ((us_addr[1] & (0x100)) >> 8); /* 8 右移8位 */
    us_addr[1] = us_addr[1] & (0xff);
    if (us_addr[0] > 0xff) {
        us_addr[0] = 0;
    }
    us_addr[0] &= 0xFE;

    for (tmp = 0; tmp < addr_len; tmp++) {
        pc_addr[tmp] = (hi_u8)us_addr[tmp];
    }

    return HI_SUCCESS;
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
ProcessingResult SpecialEtherTypeProcess(const struct NetDevice *netDev, NetBuf *buff)
{
    struct EtherHeader *header = NULL;
    uint16_t etherType;
    const struct Eapol *eapolInstance = NULL;
    int ret;

    if (netDev == NULL || buff == NULL) {
        return PROCESSING_ERROR;
    }

    header = (struct EtherHeader *)NetBufGetAddress(buff, E_DATA_BUF);
    etherType = ntohs(header->etherType);
    if (etherType != ETHER_TYPE_PAE) {
        return PROCESSING_CONTINUE;
    }
    if (netDev->specialProcPriv == NULL) {
        return PROCESSING_ERROR;
    }

    eapolInstance = EapolGetInstance();
    ret = eapolInstance->eapolOp->writeEapolToQueue(netDev, buff);
    if (ret != HDF_SUCCESS) {
        NetBufFree(buff);
    }
    return PROCESSING_COMPLETE;
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 函 数 名  : netif_set_flow_ctrl_status
 功能描述  : LiteOS协议栈流控反压接口
 输入参数  : oal_netif *netif, netif_flow_ctrl_enum_uint8 status
 输出参数  : 无
 返 回 值  : hi_void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年3月22日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
hi_void netif_set_flow_ctrl_status(const struct NetDevice *netDev, netif_flow_ctrl_enum_uint8 status)
{
    if (netDev == HI_NULL) {
        oam_error_log0(0, 0, "netDev parameter NULL.");
        return;
    }

    if (status == NETIF_FLOW_CTRL_ON) {
        g_netif_flow_ctrl = NETIF_FLOW_CTRL_ON;
    } else if (status == NETIF_FLOW_CTRL_OFF) {
        g_netif_flow_ctrl = NETIF_FLOW_CTRL_OFF;
    } else {
        oam_error_log0(0, 0, "netif_set_flow_ctrl_status::status invalid!\r\n");
    }
}

#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
/* ****************************************************************************
 功能描述  : Mesh协议栈通知驱动将某个用户去关联
 输入参数  : oal_lwip_netif *pst_netif, oal_linklayer_addr *pst_mac_addr
 返 回 值  :hi_s32
 修改历史      :
  1.日    期   : 2019年1月26日
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 hwal_lwip_remove_user(oal_lwip_netif *netif, oal_linklayer_addr *mac_addr)
{
    oal_net_device_stru *netdev = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    wal_msg_write_stru write_msg;
    mac_cfg_kick_user_param_stru *kick_user_param = HI_NULL;

    if (netif == HI_NULL) {
        oam_error_log0(0, 0, "hwal_lwip_remove_user parameter NULL.");
        return HI_ERR_CODE_PTR_NULL;
    }

    netdev = (oal_net_device_stru *)netif->state;
    if (oal_unlikely((netdev == HI_NULL) || (mac_addr == HI_NULL))) {
        oam_error_log0(0, OAM_SF_ANY, "{hwal_lwip_remove_user::pst_net_dev or pst_mac_addr null ptr error!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取VAP结构体 */
    mac_vap = (mac_vap_stru *)oal_net_dev_priv(netdev);
    /* 如果VAP结构体不存在，则直接返回 */
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{hwal_lwip_remove_user::pst_vap = OAL_PTR_NULL!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_addr->addrlen != WLAN_MAC_ADDR_LEN) {
        oam_warning_log0(0, OAM_SF_ANY, "{hwal_lwip_remove_user::the mac address len is invalid!}");
        return HI_FAIL;
    }

    if (ether_is_multicast(mac_addr->addr)) {
        oam_warning_log0(0, OAM_SF_ANY, "{hwal_lwip_remove_user::is not unicast mac address!}");
        return HI_FAIL;
    }

    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_KICK_USER, sizeof(mac_cfg_kick_user_param_stru));

    /* 设置配置命令参数 */
    kick_user_param = (mac_cfg_kick_user_param_stru *)(write_msg.auc_value);
    if (memcpy_s(kick_user_param->auc_mac_addr, WLAN_MAC_ADDR_LEN,
        (hi_u8*)(mac_addr->addr), WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hwal_lwip_remove_user::mem safe function err!}");
        return HI_FAIL;
    }
    /* 填写去关联reason code */
    kick_user_param->us_reason_code = MAC_UNSPEC_REASON;

    hi_u32 ret = wal_send_cfg_event(netdev,
                                    WAL_MSG_TYPE_WRITE,
                                    WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_kick_user_param_stru),
                                    (hi_u8 *)&write_msg,
                                    HI_FALSE,
                                    HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hwal_lwip_remove_user::return err code [%u]!}", ret);
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : Mesh协议栈设置beacon/probe rsp中的Hisi-optimization字段
 输入参数  : oal_lwip_netif *pst_netif, hi_u8 uc_prio
 返 回 值  :hi_s32
 修改历史      :
  1.日    期   : 2019年4月24日
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 hwal_lwip_set_beacon_priority(oal_lwip_netif *netif, hi_u8 prio)
{
    hi_u32 ret;
    oal_net_device_stru *netdev = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    wal_msg_write_stru write_msg;

    if (netif == HI_NULL) {
        oam_error_log0(0, 0, "hwal_lwip_set_beacon_priority pst_netif parameter NULL.");
        return HI_ERR_CODE_PTR_NULL;
    }

    netdev = (oal_net_device_stru *)netif->state;
    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hwal_lwip_set_beacon_priority::pst_net_dev null ptr error!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 获取VAP结构体 */
    mac_vap = (mac_vap_stru *)oal_net_dev_priv(netdev);
    /* 如果VAP结构体不存在，则直接返回 */
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{hwal_lwip_set_beacon_priority::pst_vap = OAL_PTR_NULL!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_BEACON_PRIORITY, sizeof(hi_u8));

    /* 设置配置命令参数 */
    *((hi_u8 *)(write_msg.auc_value)) = prio;

    ret = wal_send_cfg_event(netdev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u8),
                             (hi_u8 *)&write_msg,
                             HI_FALSE,
                             HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hwal_lwip_set_beacon_priority::return err code [%u]!}", ret);
        return (hi_s32)ret;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_netdev_set_mnid
 功能描述  : Mesh协议栈设置vap的mnid
 输入参数  : oal_lwip_netif *pst_netif, oal_uniqid_t us_mnid
 返 回 值  :hi_s32
 修改历史      :
  1.日    期   : 2019年4月28日
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 hwal_lwip_set_mnid(oal_lwip_netif *netif, oal_uniqid_t us_mnid)
{
    hi_u32 ret;
    oal_net_device_stru *netdev = HI_NULL;
    mac_vap_stru *mac_vap = HI_NULL;
    wal_msg_write_stru write_msg;

    if (oal_unlikely(netif == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hwal_lwip_set_mnid::pst_netif null ptr error!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    netdev = (oal_net_device_stru *)netif->state;
    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{hwal_lwip_set_mnid::pst_net_dev null ptr error!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    /* 获取VAP结构体 */
    mac_vap = (mac_vap_stru *)oal_net_dev_priv(netdev);
    /* 如果VAP结构体不存在，则直接返回 */
    if (oal_unlikely(mac_vap == HI_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{hwal_lwip_set_mnid::pst_vap = OAL_PTR_NULL!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (mac_vap->vap_mode != WLAN_VAP_MODE_MESH) {
        oam_warning_log0(0, OAM_SF_ANY, "{hwal_lwip_set_mnid::pst_vap is not mesh vap!}");
        return HI_FAIL;
    }

    /* **************************************************************************
                             抛事件到wal层处理
    ************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_SET_MNID, sizeof(hi_u16));

    /* 设置配置命令参数 */
    *((hi_u16 *)(write_msg.auc_value)) = us_mnid;

    ret = wal_send_cfg_event(netdev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_u16),
                             (hi_u8 *)&write_msg,
                             HI_FALSE,
                             HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hwal_lwip_set_mnid::return err code [%u]!}", ret);
        return (hi_s32)ret;
    }
    return HI_SUCCESS;
}
#endif


#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
/* ****************************************************************************
 功能描述  : 驱动上报去关联状态改变到LWIP。
**************************************************************************** */
hi_void wal_report_sta_disassoc_to_lwip(oal_net_device_stru *netdev)
{
    hi_u8 index;
    /* 内部调用函数，参数合法性由调用者保证 */
    if (netdev->lwip_netif->linklayer_event != HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::LL_EVENT_AP_DISCONN}");
        netdev->lwip_netif->linklayer_event(netdev->lwip_netif, LL_EVENT_AP_DISCONN, HI_NULL);
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::linklayer_event callback isn't registed}");
    }

    (hi_void)netifapi_netif_set_link_down(netdev->lwip_netif);
    (hi_void)netifapi_netif_set_addr(netdev->lwip_netif, HI_NULL, HI_NULL, HI_NULL);
    for (index = 0; index < LWIP_IPV6_NUM_ADDRESSES; index++) {
        (hi_void)netifapi_netif_rmv_ip6_address(netdev->lwip_netif, &netdev->lwip_netif->ip6_addr[index]);
    }
    oam_warning_log0(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::report sta disconn succ to lwip!}");
}
#endif

/* ****************************************************************************
 函 数 名  : wal_lwip_set_hwaddr
 功能描述  : 向LWIP协议栈注册的修改mac地址回调函数
 输入参数  : [1]netif
             [2]addr
             [3]len
 输出参数  : 无
 返 回 值  : hi_u32
**************************************************************************** */
/* 1337行将该函数指针赋给了netif->drv_set_hwaddr，如果改变该函数，就需要改变netif结构体，lint_t e818告警屏蔽 */
hi_u8 wal_lwip_set_hwaddr(oal_net_device_stru *netDev, uint8_t *addr, uint8_t len)
{
    oal_net_device_stru *netdev = HI_NULL;

    if (addr == NULL) {
        oam_error_log0(0, 0, "addr is NULL!");
        return (hi_u8)HI_FAIL;
    }

    if ((addr[0] & 0x01) != 0) {
        oam_error_log0(0, 0, "config a multicast mac address, please check!");
        return (hi_u8)HI_FAIL;
    }
    if (len != ETHER_ADDR_LEN) {
        oam_error_log1(0, 0, "config wrong mac address len=%u.", len);
        return (hi_u8)HI_FAIL;
    }

    netdev = netDev;
    if (netdev == NULL) {
        oam_error_log0(0, 0, "netdev is NULL!");
        return (hi_u8)HI_FAIL;
    }

    /* 如果netdev是running状态，则直接返回失败 */
    if ((oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) != 0) {
        oam_error_log0(0, 0, "netdev is running!");
        return (hi_u8)HI_FAIL;
    }

    /* 更新netdev中的mac地址 */
    if (memcpy_s(netdev->macAddr, ETHER_ADDR_LEN, addr, ETHER_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_lwip_set_hwaddr::mem safe function err!}");
        return (hi_u8)HI_FAIL;
    }
    /* 设置netdevice的MAC地址，MAC地址在HMAC层被初始化到MIB中 */
    if (wal_set_mac_to_mib(netdev) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_lwip_set_hwaddr::wal_set_mac_to_mib fail!}");
        return (hi_u8)HI_FAIL;
    }

    return (hi_u8)HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 向LWIP协议栈注册的连接变化回调函数
 输入参数  : struct netif *netif
**************************************************************************** */
/* 1366行将该函数指针赋给了netif->drv_set_hwaddr，如果改变该函数，就需要改变netif结构体，lint_t e818告警屏蔽 */
hi_void wal_lwip_status_callback(oal_net_device_stru *netDev, NetIfStatus status)
{
    oal_net_device_stru *netdev = netDev;
    if (netdev == NULL) {
        oam_error_log0(0, 0, "netdev is NULL!");
        return;
    }
    if (status == NETIF_UP) {
        wal_netdev_open(netdev);
    } else {
        wal_netdev_stop(netdev);
    }
    return;
}
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */

/* ****************************************************************************
 功能描述  : 驱动上报sta关联/去关联AP
 输入参数  : frw_event_mem_stru *pst_event_mem
 返 回 值  :hi_u32
 修改历史      :
  1.日    期   : 2019年7月9日
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_report_sta_assoc_info(frw_event_mem_stru *event_mem)
{
#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
    oal_event_ap_conn_stru ap_conn_info;
#endif

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::pst_event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    hmac_sta_report_assoc_info_stru *sta_asoc_param = (hmac_sta_report_assoc_info_stru *)event->auc_event_data;
    oal_net_device_stru *netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);

    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ANY, "{wal_report_sta_assoc_info::net device null}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (sta_asoc_param->is_assoc == HI_TRUE) {
#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
        if (memcpy_s(ap_conn_info.addr.addr, WLAN_MAC_ADDR_LEN,
            sta_asoc_param->auc_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::mem safe function err!}");
            return HI_FAIL;
        }
        ap_conn_info.addr.addrlen = WLAN_MAC_ADDR_LEN;
        ap_conn_info.rssi = -(sta_asoc_param->rssi);
        ap_conn_info.is_mesh_ap = sta_asoc_param->conn_to_mesh;

        if (netdev->lwip_netif->linklayer_event != HI_NULL) {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::LL_EVENT_AP_CONN}");
            netdev->lwip_netif->linklayer_event(netdev->lwip_netif, LL_EVENT_AP_CONN, (hi_u8 *)&ap_conn_info);
        } else {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::linklayer_event callback isn't registed}");
        }

        (hi_void)netifapi_netif_set_link_up(netdev->lwip_netif);
#ifdef _PRE_WLAN_FEATURE_LWIP_IPV6_AUTOCONFIG
        (hi_void)netifapi_set_ipv6_forwarding(netdev->lwip_netif, HI_FALSE);
        (hi_void)netifapi_set_ra_enable(netdev->lwip_netif, HI_FALSE);
        (hi_void)netifapi_set_ip6_autoconfig_enabled(netdev->lwip_netif);
        (hi_void)netifapi_set_accept_ra(netdev->lwip_netif, HI_TRUE);
#endif
        (hi_void)netifapi_netif_add_ip6_linklocal_address(netdev->lwip_netif, HI_TRUE);
#endif
        oam_warning_log0(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::report sta conn succ to lwip!}");
        oam_warning_log4(0, OAM_SF_ANY, "{wal_report_sta_assoc_info::rssi=%x,is_mesh_ap=%d,mac addr=X:X:X:X:%x:%x}",
            (hi_s32)(-(sta_asoc_param->rssi)), sta_asoc_param->conn_to_mesh,
            sta_asoc_param->auc_mac_addr[4], sta_asoc_param->auc_mac_addr[5]); /* 4 5 地址位数 */
    } else {
#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
        wal_report_sta_disassoc_to_lwip(netdev);
#endif
    }

    return HI_SUCCESS;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/* ****************************************************************************
 函 数 名  : wal_mesh_inform_tx_data_info
 功能描述  : MESH 驱动上报Lwip 单播数据帧的一些信息。
 输入参数  : frw_event_mem_stru *pst_event_mem
 返 回 值  :hi_u32
 修改历史      :
  1.日    期   : 2019年1月26日
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_mesh_inform_tx_data_info(frw_event_mem_stru *event_mem)
{
    frw_event_stru *event = HI_NULL;
    dmac_tx_info_report_stru *tx_info_param = HI_NULL;
    oal_net_device_stru *netdev = HI_NULL;
#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
    oal_event_tx_info_stru tx_info;
#endif

    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_mesh_inform_tx_data_info::pst_event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    event = (frw_event_stru *)event_mem->puc_data;

    tx_info_param = (dmac_tx_info_report_stru *)event->auc_event_data;
    netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ANY,
            "{wal_mesh_inform_tx_data_info::get net device ptr is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
    if (memcpy_s(tx_info.addr.addr, NETIF_MAX_HWADDR_LEN, tx_info_param->auc_da, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_mesh_inform_tx_data_info::mem safe function err!}");
        return HI_FAIL;
    }
    tx_info.addr.addrlen = WLAN_MAC_ADDR_LEN;
    tx_info.retry_count = tx_info_param->tx_count;
    tx_info.status = tx_info_param->mesh_tx_status;
    tx_info.pkt_sz = tx_info_param->us_length;
    tx_info.data_rate = tx_info_param->rate_kbps;
    tx_info.bandwidth = tx_info_param->bw;
    oam_info_log4(0, OAM_SF_ANY,
        "{wal_mesh_inform_tx_data_info::report to mesh stack,retry_count = %d,status = %d,pkt_sz = %d,data_rate = %d}",
        tx_info_param->tx_count, tx_info_param->mesh_tx_status, tx_info_param->us_length, tx_info_param->rate_kbps);
    oam_info_log1(0, OAM_SF_ANY, "{wal_mesh_inform_tx_data_info::report to mesh stack,bandwidth = %d!}",
        tx_info_param->bw);

    if (netdev->lwip_netif->linklayer_event != HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_inform_tx_data_info::LL_EVENT_TX_INFO}");
        netdev->lwip_netif->linklayer_event(netdev->lwip_netif, LL_EVENT_TX_INFO, (hi_u8 *)&tx_info);
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_inform_tx_data_info::linklayer_event callback isn't registed!}");
    }

    oam_info_log0(0, OAM_SF_ANY, "{wal_mesh_inform_tx_data_info::report tx data info!}");
#else
    hi_unref_param(tx_info_param);
#endif
    return HI_SUCCESS;
}

/* wal_mesh_report_assoc_info没有申明，误报lin_t515告警，告警屏蔽 */
hi_u32 wal_mesh_report_assoc_info(const mac_user_assoc_info_stru *assoc_info, oal_net_device_stru *netdev)
{
    hi_unref_param(netdev);

#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
    oal_event_new_peer_stru new_peer;
    if (memcpy_s(new_peer.addr.addr, NETIF_MAX_HWADDR_LEN, assoc_info->auc_user_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::mem safe function err!}");
        return HI_FAIL;
    }
    new_peer.addr.addrlen = WLAN_MAC_ADDR_LEN;
    new_peer.is_mesh_user = ((assoc_info->is_initiative_role << 4) | (assoc_info->is_mesh_user & 0x0F)); /* 4 */
    new_peer.beacon_prio = assoc_info->bcn_prio;
    new_peer.rssi = (hi_s8)(-(assoc_info->conn_rx_rssi));
    new_peer.lqi = 0;

    if (netdev->lwip_netif->linklayer_event != HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::LL_EVENT_NEW_PEER}");
        netdev->lwip_netif->linklayer_event(netdev->lwip_netif, LL_EVENT_NEW_PEER, (hi_s8 *)&new_peer);
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info:linklayer_event callback isn't registed}");
    }
#endif

    if (assoc_info->is_mesh_user == HI_TRUE) {
        oam_warning_log3(0, OAM_SF_ANY,
            "{wal_mesh_report_mesh_user_info:report add mesh peer to lwip,bcn_prio=%d,role=%d,rssi=%d}",
            assoc_info->bcn_prio, assoc_info->is_initiative_role, (hi_s32)(-(assoc_info->conn_rx_rssi)));
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::report add sta to lwip}");
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : MESH 驱动上报某用户关联状态改变。
 输入参数  : frw_event_mem_stru *pst_event_mem
 返 回 值  :hi_u32
 修改历史      :
  1.日    期   : 2019年4月24日
    修改内容   : 新生成函数
**************************************************************************** */
/* g_ast_wal_host_ctx_table数组成员，需要修改结构体frw_event_sub_table_item_stru进而需要修改
   g_ast_dmac_host_crx_table数组的成员，其中dmac_cfg_vap_init_event对变量进行了修改，lint_t e818告警屏蔽 */
hi_u32 wal_mesh_report_mesh_user_info(frw_event_mem_stru *event_mem)
{
    if (oal_unlikely(event_mem == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::pst_event_mem is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    frw_event_stru *event = (frw_event_stru *)event_mem->puc_data;
    mac_user_assoc_info_stru *assoc_info = (mac_user_assoc_info_stru *)event->auc_event_data;
    oal_net_device_stru *netdev = hmac_vap_get_net_device(event->event_hdr.vap_id);
    if (netdev == HI_NULL) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::netdev null}");
        return HI_ERR_CODE_PTR_NULL;
    }

    if (assoc_info->assoc_state == MAC_USER_STATE_DEL) {
#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
        oal_event_del_peer_stru del_peer;
        if (memcpy_s(del_peer.addr.addr, NETIF_MAX_HWADDR_LEN, assoc_info->auc_user_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::mem safe function err!}");
            return HI_FAIL;
        }
        del_peer.addr.addrlen = WLAN_MAC_ADDR_LEN;
        del_peer.is_mesh_user = assoc_info->is_mesh_user;

        if (netdev->lwip_netif->linklayer_event != HI_NULL) {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::LL_EVENT_DEL_PEER}");
            netdev->lwip_netif->linklayer_event(netdev->lwip_netif, LL_EVENT_DEL_PEER, (hi_u8 *)&del_peer);
        } else {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info:linklayer_event callback isn't registed}");
        }
#endif

        if (assoc_info->is_mesh_user == HI_TRUE) {
            oam_warning_log2(0, OAM_SF_ANY,
                "{wal_mesh_report_mesh_user_info:report del mesh peer to lwip,mac addr[%x:%x]}",
                assoc_info->auc_user_addr[4], assoc_info->auc_user_addr[5]); /* 4 5 地址位数 */
        } else {
            oam_warning_log2(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::report del sta to lwip,mac addr[%x:%x]}",
                assoc_info->auc_user_addr[4], assoc_info->auc_user_addr[5]); /* 4 5 地址位数 */
        }

        return HI_SUCCESS;
    } else if (assoc_info->assoc_state == MAC_USER_STATE_ASSOC) {
        hi_u32 ret = wal_mesh_report_assoc_info(assoc_info, netdev);
        return ret;
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::rx unsupport state}");

        return HI_FAIL;
    }
}
#endif /* #ifdef _PRE_WLAN_FEATURE_MESH */

/* ****************************************************************************
 功能描述  : 根据组播ip地址获取组播mac地址
 输入参数  : puc_group_ip指向ip地址的指针; uc_ip_len ip地址长度
 输出参数  : puc_group_mac 存储组播mac地址的指针
 修改历史      :
  1.日    期   : 2019年5月9日
    修改内容   : 新生成函数

**************************************************************************** */
hi_void wal_ip_conver_multi_mac(hi_u8 *puc_group_mac, const hi_u8 *puc_group_ip, hi_u8 ip_len)
{
    if (puc_group_ip == HI_NULL) {
        oam_error_log0(0, 0, "wal_ip_conver_multi_mac::puc_group_ip null!");
        return;
    }

    if (ip_len == OAL_IPV6_ADDR_SIZE) {
        /* ipv6下，组播mac最后4字节由IP地址映射而来 */
        puc_group_mac[0] = 0x33;
        puc_group_mac[1] = 0x33;
        puc_group_ip += 12;                                                               /* 12 取最后4个字节 */
        if (memcpy_s(puc_group_mac + 2, WLAN_MAC_ADDR_LEN - 2, puc_group_ip, 4) != EOK) { /* 2 偏移量 4 复制长度 */
            oam_error_log0(0, OAM_SF_ANY, "{wal_ip_conver_multi_mac::mem safe function err!}");
            return;
        }
    } else {
        /* ipv4下，组播mac最后23bit由IP地址映射而来 */
        puc_group_mac[0] = 0x01;
        puc_group_mac[1] = 0x0;
        puc_group_mac[2] = 0x5e; /* 2 mac第3位 */
        puc_group_ip += 1;
        if (memcpy_s(puc_group_mac + 3, WLAN_MAC_ADDR_LEN - 3, puc_group_ip, 3) != EOK) { /* 3 偏移量 复制长度 */
            oam_error_log0(0, OAM_SF_ANY, "{wal_ip_conver_multi_mac::mem safe function err!}");
            return;
        }
        puc_group_mac[3] &= 0x7f; /* 3 mac第4位 */
    }

    return;
}

hi_u32 wal_netdev_open_send_event(oal_net_device_stru *netdev)
{
    wal_msg_write_stru write_msg;
    wal_msg_stru *rsp_msg = HI_NULL;
    hi_u32 ret;

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_START_VAP, sizeof(mac_cfg_start_vap_param_stru));
    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->net_dev = netdev;

#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype);
    if (p2p_mode == WLAN_P2P_BUTT) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_open::wal_wireless_iftype_to_mac_p2p_mode return BUFF}");
        wal_wake_unlock();
        return HI_FAIL;
    }
    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->p2p_mode = p2p_mode;
#endif

    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->mgmt_rate_init_flag = HI_TRUE;

    /* 发送消息 */
    ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_start_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open::wal_alloc_cfg_event return err code %u!}", ret);
        wal_wake_unlock();
        return ret;
    }

    /* 处理返回消息 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open::hmac start vap fail,err code[%u]!}", ret);
        wal_wake_unlock();
        return ret;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_netdev_open
 功能描述  : 启用VAP
 输入参数  : pst_net_dev: net_device
 输出参数  : 无
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年12月11日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */

hi_s32 wal_netdev_open(oal_net_device_stru *netdev)
{
    wal_msg_query_stru query_msg;
    wal_msg_stru *rsp_msg = HI_NULL;
    hi_u32 ret;

    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_open::pst_net_dev is null ptr!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open::iftype:%d.!}", GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype);

    wal_wake_lock();

    if (GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_AP
#ifdef _PRE_WLAN_FEATURE_MESH
        || (GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_MESH_POINT)
#endif
    ) {
        query_msg.wid = WLAN_CFGID_SSID;

        /* 发送消息 */
        ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_QUERY, WAL_MSG_WID_LENGTH, (hi_u8 *)&query_msg, HI_TRUE, &rsp_msg);
        if ((ret != HI_SUCCESS) || (rsp_msg == HI_NULL)) {
            oam_warning_log1(0, OAM_SF_ANY, "{_wal_netdev_open::wal_alloc_cfg_event return err code %d!}", ret);
            wal_wake_unlock();
            return ret;
        }

        /* 处理返回消息 */
        wal_msg_rsp_stru *query_rsp_msg = (wal_msg_rsp_stru *)(rsp_msg->auc_msg_data);
        mac_cfg_ssid_param_stru *ssid = (mac_cfg_ssid_param_stru *)(query_rsp_msg->auc_value);
        hi_u8 ssid_len = ssid->ssid_len;

        oal_free(rsp_msg);

        if (ssid_len == 0) {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_open::ssid not set,no need to start vap.!}");
            oal_net_tx_wake_all_queues(); /* 启动发送队列 */
            wal_wake_unlock();
            return HI_SUCCESS;
        }
    }

    /* **************************************************************************
        抛事件到wal层处理
    ************************************************************************** */
    ret = wal_netdev_open_send_event(netdev);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if ((oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) == 0) {
        oal_netdevice_flags(netdev) |= OAL_IFF_RUNNING;
    }

    oal_net_tx_wake_all_queues(); /* 启动发送队列 */
    wal_wake_unlock();

    return HI_SUCCESS;
}

hi_u32 wal_netdev_stop_del_vap(const oal_net_device_stru *netdev)
{
    /* wlan0/p2p0 down时 删除VAP */
    if (GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_AP ||
        GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_STATION ||
        GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_P2P_DEVICE
#ifdef _PRE_WLAN_FEATURE_MESH
        || GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype == NL80211_IFTYPE_MESH_POINT
#endif
    ) {
#ifdef _PRE_WLAN_FEATURE_P2P
        /* 用于删除p2p小组 */
        if (oal_net_dev_priv(netdev) == HI_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_stop::pst_mac_vap is null, netdev released.}");
            wal_wake_unlock();
            return HI_SUCCESS;
        }
        mac_device_stru *mac_dev = mac_res_get_dev(); /* 用于删除p2p小组 */
        if (mac_dev != HI_NULL) {
            wal_del_p2p_group(mac_dev);
        }
#endif
        wal_wake_unlock();
        return HI_SUCCESS;
    }

    return HI_CONTINUE;
}

/* ****************************************************************************
 函 数 名  : wal_netdev_stop
 功能描述  : 停用vap
 输入参数  : pst_net_dev: net_device
 输出参数  : 无
 返 回 值  : 错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月13日
    修改内容   : 新生成函数

**************************************************************************** */
hi_s32 wal_netdev_stop(oal_net_device_stru *netdev)
{
    wal_msg_write_stru write_msg;
    wal_msg_stru *rsp_msg = HI_NULL;

    if (oal_unlikely((netdev == HI_NULL) || (GET_NET_DEV_CFG80211_WIRELESS(netdev) == NULL))) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_stop::pst_net_dev/ieee80211_ptr is null ptr!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* stop the netdev's queues */
    oal_net_tx_stop_all_queues(); /* 停止发送队列 */

    oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::iftype:%d.}", GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype);

    /* 如果netdev不是running状态，则直接返回成功 */
    if ((oal_netdevice_flags(netdev) & OAL_IFF_RUNNING) == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_stop::vap is already down!}");
        return HI_SUCCESS;
    }
    wal_wake_lock();
    wal_force_scan_complete(netdev);

    /* **************************************************************************
                           抛事件到wal层处理
    ************************************************************************** */
    /* 填写WID消息 */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DOWN_VAP, sizeof(mac_cfg_down_vap_param_stru));
    ((mac_cfg_down_vap_param_stru *)write_msg.auc_value)->net_dev = netdev;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(GET_NET_DEV_CFG80211_WIRELESS(netdev)->iftype);
    if (p2p_mode == WLAN_P2P_BUTT) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_stop::wal_wireless_iftype_to_mac_p2p_mode return BUFF}");
        wal_wake_unlock();
        return HI_FAIL;
    }
    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->p2p_mode = p2p_mode;
#endif

    /* 发送消息 */
    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_down_vap_param_stru), (hi_u8 *)&write_msg, HI_TRUE, &rsp_msg);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::wal_alloc_cfg_event return err code %u!}", ret);
        wal_wake_unlock();
        return ret;
    }

    /* 处理返回消息 */
    ret = wal_check_and_release_msg_resp(rsp_msg);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::hmac stop vap fail!err code [%d]}", ret);
        wal_wake_unlock();
        return ret;
    }

    if (wal_netdev_stop_del_vap(netdev) == HI_SUCCESS) {
        return HI_SUCCESS;
    }

    wal_wake_unlock();
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_netdev_get_stats
 功能描述  : 获取统计信息
**************************************************************************** */
static oal_net_device_stats_stru *wal_netdev_get_stats(oal_net_device_stru *netdev)
{
    oal_net_device_stats_stru *stats = HI_NULL;

    if (netdev == HI_NULL) {
        return HI_NULL;
    }
    stats = &(netdev->stats); // modify by zhangwensheng temp
    stats->rxPackets = 10;
    stats->rxBytes = 10;
    stats->txPackets = 10;
    stats->txBytes = 10;

    return stats;
}

/* ****************************************************************************
 功能描述  : net device的ioctl函数
 输入参数  : net device指针
 返 回 值  : 统计结果指针
**************************************************************************** */

static hi_s32 wal_net_device_ioctl(oal_net_device_stru *netdev, oal_ifreq_stru *ifr, hi_s32 cmd)
{
    hi_u32 ret = HI_SUCCESS;

    if ((netdev == HI_NULL) || (ifr == HI_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_net_device_ioctl::pst_dev %p, pst_ifr %p!}", (uintptr_t)netdev,
            (uintptr_t)ifr);
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 1102 wpa_supplicant 通过ioctl 下发命令 */
    if (cmd == WAL_SIOCDEVPRIVATE + 1) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        ret = wal_priv_cmd(netdev, ifr, cmd);
#endif
        return ret;
    } else {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_net_device_ioctl::unrecognised cmd, %d!}", cmd);
        return HI_SUCCESS;
    }
}

/* ****************************************************************************
 函 数 名  : oal_net_device_change_mtu
 功能描述  : net device的change_mtu函数
 输入参数  : net device指针
 输出参数  : 无
 返 回 值  : 统计结果指针
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年12月25日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */

static inline int32_t oal_net_device_change_mtu(oal_net_device_stru *netdev, hi_s32 mtu)
{
    /* 需要优化 */
    netdev->mtu = (hi_u32)mtu;
    return HI_SUCCESS;
}

/* ****************************************************************************
 函 数 名  : wal_netdev_set_mac_addr
 功能描述  : 设置mac地址
 输入参数  : pst_dev: 网络设备
             p_addr : 地址
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年12月24日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */

static hi_s32 wal_netdev_set_mac_addr(oal_net_device_stru *netdev, void *addr)
{
    oal_sockaddr_stru *mac_addr = HI_NULL;
    wal_msg_write_stru write_msg;

    if (oal_unlikely((netdev == HI_NULL) || (addr == HI_NULL))) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::pst_net_dev or p_addr null ptr error %p, %p!}",
            (uintptr_t)netdev, (uintptr_t)addr);

        return HI_ERR_CODE_PTR_NULL;
    }

    if (oal_netif_running(netdev)) { /* 业务需要,lin_t e506/e774告警屏蔽 */
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::cannot set address; device running!}");

        return HI_FAIL;
    }

    mac_addr = (oal_sockaddr_stru *)addr;

    if (ether_is_multicast(mac_addr->sa_data)) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::can not set group/broadcast addr!}");
        return HI_FAIL;
    }
    wal_wake_lock();
    if (memcpy_s((netdev->macAddr), WLAN_MAC_ADDR_LEN, (mac_addr->sa_data), WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::mem safe function err!}");
        return HI_FAIL;
    }

    /* 1131如果return则无法通过命令配置mac地址到寄存器 */
    wal_wake_unlock();
    /* ****************************************************************************
                     抛事件到wal层处理
    **************************************************************************** */
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_STATION_ID, sizeof(mac_cfg_staion_id_param_stru));
    mac_cfg_staion_id_param_stru *param = (mac_cfg_staion_id_param_stru *)(write_msg.auc_value);

    /* 设置配置命令参数 */
    if (memcpy_s((param->auc_station_id), WLAN_MAC_ADDR_LEN, (mac_addr->sa_data), WLAN_MAC_ADDR_LEN) != EOK) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::write_msg mem safe function err!}");
        return HI_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_P2P
    /* Enter the P2P mode for delivering net_device */
    oal_wireless_dev *wdev = (oal_wireless_dev *)netdev->ieee80211Ptr;
    param->p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(wdev->iftype);
    if (param->p2p_mode == WLAN_P2P_BUTT) {
        oam_warning_log0(0, OAM_SF_ANY, 
            "{wal_netdev_set_mac_addr::wal_wireless_iftype_to_mac_p2p_mode return BUFF}\r\n");
        return HI_FAIL;
    }
#endif

    hi_u32 ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_staion_id_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(0, OAM_SF_ANY, "{hwal_lwip_set_mnid::return err code [%u]!}", ret);
        return (hi_s32)ret;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
  net_device上挂接的net_device_ops函数
**************************************************************************** */
oal_net_device_ops_stru g_wal_net_dev_ops = {
    .getStats = wal_netdev_get_stats,
    .open = wal_netdev_open,
    .stop = wal_netdev_stop,
    .xmit = hmac_bridge_vap_xmit,
    .ioctl = wal_net_device_ioctl,
    .changeMtu = oal_net_device_change_mtu,
    .init = oal_net_device_init,
    .deInit = oal_net_free_netdev,
#if (defined(_PRE_WLAN_FEATURE_FLOWCTL) || defined(_PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL))
    .selectQueue = wal_netdev_select_queue,
#endif

    .setMacAddr = wal_netdev_set_mac_addr,
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    .netifNotify = HI_NULL,
#endif
    .specialEtherTypeProcess = SpecialEtherTypeProcess,
};


/* ****************************************************************************
 功能描述  : 获取g_wal_net_dev_ops结构
**************************************************************************** */
oal_net_device_ops_stru *wal_get_net_dev_ops(hi_void)
{
    return &g_wal_net_dev_ops;
}

hi_s32 wal_init_netdev(nl80211_iftype_uint8 type, oal_net_device_stru *netdev)
{
    hi_u8 ac_addr[ETHER_ADDR_LEN] = {0};

    /* 对netdevice进行赋值 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    netdev->wireless_handlers = wal_get_g_iw_handler_def();
#endif
    netdev->netDeviceIf = wal_get_net_dev_ops();

    if (wal_get_dev_addr(ac_addr, ETHER_ADDR_LEN, type) != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_init_wlan_netdev::wal_get_dev_addr failed!}");
        return HI_FAIL;
    }

    if (memcpy_s(netdev->macAddr, ETHER_ADDR_LEN, ac_addr, ETHER_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_init_wlan_netdev::mem safe function err!}");
        return HI_FAIL;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && \
    !defined(_PRE_HDF_LINUX)
    netdev->ethtool_ops = &g_wal_ethtool_ops;
#endif

    oal_netdevice_watchdog_timeo(netdev) = 5; /* 固定设置为 5 */

    oal_wireless_dev *wdev = (oal_wireless_dev *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, sizeof(oal_wireless_dev));
    if (oal_unlikely(wdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{alloc mem, pst_wdev is null ptr!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    if (memset_s(wdev, sizeof(oal_wireless_dev), 0, sizeof(oal_wireless_dev)) != EOK) {
        oal_mem_free(wdev);
        return HI_FAIL;
    }

    netdev->ieee80211Ptr = wdev;
    oal_netdevice_specical_proc_priv(netdev) = NULL;

    wdev->netdev = netdev;
    wdev->iftype = type;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && !defined(_PRE_HDF_LINUX)
    wdev->wiphy = mac_res_get_dev()->wiphy;
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    wdev->wiphy = oal_wiphy_get();
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
    if (type == NL80211_IFTYPE_P2P_DEVICE) {
        mac_wiphy_priv_stru *wiphy_priv = oal_wiphy_priv(wdev->wiphy);
        wiphy_priv->mac_device->p2p_info.pst_p2p_net_device = netdev;
    }
#endif

    oal_netdevice_flags(netdev) &= ~OAL_IFF_RUNNING; /* 将net device的flag设为down */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* HCC层用 */
    oal_netdevice_headroom(netdev) = 64; /* 固定设置为 64 */
    oal_netdevice_tailroom(netdev) = 32; /* 固定设置为 32 */
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || defined(_PRE_HDF_LINUX)
    CreateEapolData(netdev);
#endif

    return HI_SUCCESS;
}

hi_s32 wal_init_netif(nl80211_iftype_uint8 type, oal_net_device_stru *netdev)
{
    (void)type;
    /* 注册net_device */
    hi_u32 ret = NetDeviceAdd(netdev);
    if (oal_unlikely(ret != 0)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_init_netif::NetDeviceAdd return error code %d}", ret);
        oal_net_unregister_netdev(netdev);
        return HI_FAIL;
    }
    if (wal_init_wlan_vap(netdev) != HI_SUCCESS) {
        oal_net_unregister_netdev(netdev);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 初始化wlan设备
 输入参数  : [1]type 设备类型
             [2]mode 模式
             [3]NetDevice
 输出参数  : [1]ifname 设备名
             [2]len 设备名长度
 返 回 值  : 错误码
**************************************************************************** */
/* 建议5.5误检，在2024行作为strncpy_s函数的第一个参数传入 */
hi_s32 wal_init_drv_wlan_netdev(nl80211_iftype_uint8 type, wal_phy_mode mode, oal_net_device_stru *netdev)
{
    hi_char *ac_mode_str = NULL;
    hi_s32 ret;
    if (oal_unlikely(netdev == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{netdev is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    do {
        ret = wal_init_netdev(type, netdev);
        if (ret != HI_SUCCESS) {
            break;
        }

        ret = wal_init_netif(type, netdev);
        if (ret != HI_SUCCESS) {
            break;
        }
        ac_mode_str = "11bgn";
        if (mode == WAL_PHY_MODE_11G) {
            ac_mode_str = "11bg";
        } else if (mode == WAL_PHY_MODE_11B) {
            ac_mode_str = "11b";
        }

        ret = wal_ioctl_set_mode(netdev, ac_mode_str);
    } while (false);

    if (ret != HI_SUCCESS) {
        wal_deinit_wlan_vap(netdev);
        oal_net_unregister_netdev(netdev);
        oal_net_clear_netdev(netdev);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

int32_t GetIfName(nl80211_iftype_uint8 type, char *ifName, uint32_t len)
{
    if (ifName == NULL || len == 0) {
        HDF_LOGE("%s:para is null!", __func__);
        return HI_FAIL;
    }
    switch (type) {
        case NL80211_IFTYPE_P2P_DEVICE:
            if (snprintf_s(ifName, len, len - 1, "p2p%d", 0) < 0) {
                HDF_LOGE("%s:format ifName failed!", __func__);
                return HI_FAIL;
            }
            break;
        case NL80211_IFTYPE_P2P_CLIENT:
            /*  fall-through */
        case NL80211_IFTYPE_P2P_GO:
            if (snprintf_s(ifName, len, len - 1, "p2p-p2p0-%d", 0) < 0) {
                HDF_LOGE("%s:format ifName failed!", __func__);
                return HI_FAIL;
            }
            break;
        default:
            HDF_LOGE("%s:GetIfName::not supported dev type!", __func__);
            return HI_FAIL;
    }
    return HI_SUCCESS;
}

hi_s32 InitNetdev(struct NetDevice *netDevice, nl80211_iftype_uint8 type)
{
    if (netDevice == NULL) {
        HDF_LOGE("%s:para is null!", __func__);
        return HI_FAIL;
    }
    struct NetDevice *netdev = NULL;
    char ifName[WIFI_IFNAME_MAX_SIZE] = {0};
    struct HdfWifiNetDeviceData *data = NULL;
    hi_s32 ret;

    if (GetIfName(type, ifName, WIFI_IFNAME_MAX_SIZE) != HI_SUCCESS) {
        HDF_LOGE("%s:get ifName failed!", __func__);
        return HI_FAIL;
    }
#ifdef _PRE_HDF_LINUX
    netdev = NetDeviceInit(ifName, strlen(ifName), WIFI_LINK, FULL_OS);
#else
    netdev = NetDeviceInit(ifName, strlen(ifName), WIFI_LINK, LITE_OS);
#endif
    if (netdev == NULL) {
        HDF_LOGE("%s:netdev is null!", __func__);
        return HI_FAIL;
    }
    data = GetPlatformData(netDevice);
    if (data == NULL) {
        HDF_LOGE("%s:netdevice data null!", __func__);
        return HI_FAIL;
    }
    netdev->classDriverName = netDevice->classDriverName;
    netdev->classDriverPriv = data;
    ret = wal_init_drv_wlan_netdev(type, WAL_PHY_MODE_11N, netdev);
    if (ret != HI_SUCCESS) {
        oam_error_log2(0, OAM_SF_ANY, "InitP2pNetdev %s failed. return:%d\n", netdev->name, ret);
    }

    return ret;
}

hi_s32 DeinitNetdev(nl80211_iftype_uint8 type)
{
    struct NetDevice *netDevice = NULL;
    hi_s32 ret;
    char *ifName = (char *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, WIFI_IFNAME_MAX_SIZE);
    if (oal_unlikely(ifName == HI_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{alloc mem, pst_ifName is null ptr!}");
        return HI_ERR_CODE_PTR_NULL;
    }
    if (GetIfName(type, ifName, WIFI_IFNAME_MAX_SIZE) != HI_SUCCESS) {
        HDF_LOGE("%s:get ifName failed!", __func__);
        oal_mem_free(ifName);
        return HI_FAIL;
    }
    netDevice = NetDeviceGetInstByName(ifName);

    ret = wal_deinit_drv_wlan_netdev(netDevice);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:wal_deinit_drv_wlan_netdev failed!", __func__);
        oal_mem_free(ifName);
        return ret;
    }
    ret = NetDeviceDeInit(netDevice);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:NetDeviceDeInit failed!", __func__);
        oal_mem_free(ifName);
        return ret;
    }
    if (ifName != HI_NULL) {
        oal_mem_free(ifName);
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : 去初始化wlan设备
 输入参数  : *ifname 设备名
 返 回 值：错误码
**************************************************************************** */
hi_s32 wal_deinit_drv_wlan_netdev(oal_net_device_stru *netdev)
{
    oal_wireless_dev *wdev = HI_NULL;
    hi_u32 ret;

    if (netdev == HI_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_deinit_wlan_netdev::netdev is null!}");
        return HI_ERR_CODE_PTR_NULL;
    }

    /* 先通知lwip解注册 */
    oal_net_unregister_netdev(netdev);

    ret = wal_deinit_wlan_vap(netdev);
    if (ret != HI_SUCCESS) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_deinit_wlan_netdev::deinit vap failed!}");
        return HI_FAIL;
    }
    wdev = (struct wireless_dev *)oal_netdevice_wdev(netdev);
    if (wdev != HI_NULL) {
        oal_mem_free(wdev);
    }
    oal_net_clear_netdev(netdev);

    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 判断netdev是否处于busy
**************************************************************************** */
hi_u8 wal_dev_is_running(hi_void)
{
    return NetDeviceIsAnyInstRunning();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
