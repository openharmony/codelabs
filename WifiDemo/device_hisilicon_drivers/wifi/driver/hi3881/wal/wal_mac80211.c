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

#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "hi_list.h"
#include "hmac_ext_if.h"
#include "wal_11d.h"
#include "wal_main.h"
#include "wal_ioctl.h"
#include "wal_regdb.h"
#include "wifi_mac80211_ops.h"
#include "wal_cfg80211.h"
#include "wal_wpa_ioctl.h"
#include "net_adpater.h"
#include "hdf_wlan_utils.h"
#include "wifi_module.h"
#include "osal_mem.h"

#define HDF_LOG_TAG Hi3881Driver

static struct MacStorage g_macStorage = { 0 };

#define WIFI_SCAN_EXTRA_IE_LEN_MAX 512

int32_t WalDisconnect(NetDevice *netDev, uint16_t reasonCode)
{
    return wal_cfg80211_disconnect(NULL, netDev, reasonCode);
}

hi_u32 wal_send_scan_abort_msg(oal_net_device_stru *netdev);

static int32_t WifiScanSetSsid(const struct WlanScanRequest *params, oal_cfg80211_scan_request_stru *request)
{
    int32_t count = 0;
    int32_t loop;

    if (params->ssidCount > WPAS_MAX_SCAN_SSIDS) {
        oam_error_log(0, OAM_SF_ANY, "%s:unexpected numSsids!numSsids=%u", __func__, params->ssidCount);
        return HDF_FAILURE;
    }

    if (params->ssidCount == 0) {
        oam_debug_log(0, OAM_SF_ANY, "%s:ssid number is 0!", __func__);
        return HDF_SUCCESS;
    }

    request->ssids = (oal_cfg80211_ssid_stru *)OsalMemCalloc(params->ssidCount * sizeof(oal_cfg80211_ssid_stru));
    if (request->ssids == NULL) {
        oam_error_log(0, OAM_SF_ANY, "%s:oom", __func__);
        return HDF_FAILURE;
    }

    for (loop = 0; loop < params->ssidCount; loop++) {
        if (count >= DRIVER_MAX_SCAN_SSIDS) {
            break;
        }

        if (params->ssids[loop].ssidLen > IEEE80211_MAX_SSID_LEN) {
            continue;
        }

        request->ssids[count].ssid_len = params->ssids[loop].ssidLen;
        if (memcpy_s(request->ssids[count].ssid, OAL_IEEE80211_MAX_SSID_LEN, params->ssids[loop].ssid,
            params->ssids[loop].ssidLen) != EOK) {
            continue;
        }
        count++;
    }
    request->n_ssids = count;

    return HDF_SUCCESS;
}

static int32_t WifiScanSetUserIe(const struct WlanScanRequest *params, oal_cfg80211_scan_request_stru *request)
{
    if (params->extraIEsLen > WIFI_SCAN_EXTRA_IE_LEN_MAX) {
        oam_error_log(0, OAM_SF_ANY, "%s:unexpected extra len!extraIesLen=%d", __func__, params->extraIEsLen);
        return HDF_FAILURE;
    }
    if ((params->extraIEs != NULL) && (params->extraIEsLen != 0)) {
        request->ie = (uint8_t *)OsalMemCalloc(params->extraIEsLen);
        if (request->ie == NULL) {
            oam_error_log(0, OAM_SF_ANY, "%s:oom", __func__);
            goto fail;
        }
        (void)memcpy_s(request->ie, params->extraIEsLen, params->extraIEs, params->extraIEsLen);
        request->ie_len = params->extraIEsLen;
    }

    return HDF_SUCCESS;

fail:
    if (request->ie != NULL) {
        OsalMemFree(request->ie);
        request->ie = NULL;
    }

    return HDF_FAILURE;
}

static oal_ieee80211_channel *GetChannelByFreq(const oal_wiphy_stru *wiphy, uint16_t center_freq)
{
    enum Ieee80211Band band;
    oal_ieee80211_supported_band *currentBand = NULL;
    int32_t loop;
    for (band = (enum Ieee80211Band)0; band < IEEE80211_NUM_BANDS; band++) {
        currentBand = wiphy->bands[band];
        if (currentBand == NULL) {
            continue;
        }
        for (loop = 0; loop < currentBand->n_channels; loop++) {
            if (currentBand->channels[loop].center_freq == center_freq) {
                return &currentBand->channels[loop];
            }
        }
    }
    return NULL;
}

static int32_t WifiScanSetChannel(const oal_wiphy_stru *wiphy, const struct WlanScanRequest *params,
    oal_cfg80211_scan_request_stru *request)
{
    int32_t loop;
    int32_t count = 0;
    enum Ieee80211Band band = IEEE80211_BAND_2GHZ;
    oal_ieee80211_channel *chan = NULL;

    if ((params->freqs == NULL) || (params->freqsCount == 0)) {
        if (wiphy->bands[band] == NULL) {
            oam_error_log(0, OAM_SF_ANY, "%s:invalid band info", __func__);
            return HDF_FAILURE;
        }

        for (loop = 0; loop < (int32_t)wiphy->bands[band]->n_channels; loop++) {
            chan = &wiphy->bands[band]->channels[loop];
            if ((chan->flags & WIFI_CHAN_DISABLED) != 0) {
                continue;
            }
            request->channels[count++] = chan;
        }
    } else {
        for (loop = 0; loop < params->freqsCount; loop++) {
            chan = GetChannelByFreq(wiphy, (uint16_t)(params->freqs[loop]));
            if (chan == NULL) {
                oam_error_log(0, OAM_SF_ANY, "%s:freq not found!freq=%d", __func__, params->freqs[loop]);
                continue;
            }

            request->channels[count++] = chan;
        }
    }

    if (count == 0) {
        oam_error_log(0, OAM_SF_ANY, "%s:invalid freq info", __func__);
        return HDF_FAILURE;
    }
    request->n_channels = count;

    return HDF_SUCCESS;
}

static int32_t WifiScanSetRequest(struct NetDevice *netdev, const struct WlanScanRequest *params,
    oal_cfg80211_scan_request_stru *request)
{
    if (netdev == NULL || netdev->ieee80211Ptr == NULL) {
        return HDF_FAILURE;
    }
    request->wiphy = GET_NET_DEV_CFG80211_WIRELESS(netdev)->wiphy;
    request->dev = netdev;
    request->wdev = GET_NET_DEV_CFG80211_WIRELESS(netdev);
    request->n_ssids = params->ssidCount;
    request->prefix_ssid_scan_flag = 0;
    if (WifiScanSetChannel(GET_NET_DEV_CFG80211_WIRELESS(netdev)->wiphy, params, request)) {
        oam_error_log(0, OAM_SF_ANY, "%s:set channel failed!", __func__);
        return HDF_FAILURE;
    }
    if (WifiScanSetSsid(params, request)) {
        oam_error_log(0, OAM_SF_ANY, "%s:set ssid failed!", __func__);
        return HDF_FAILURE;
    }
    if (WifiScanSetUserIe(params, request)) {
        oam_error_log(0, OAM_SF_ANY, "%s:set user ie failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void WifiScanFree(oal_cfg80211_scan_request_stru **request)
{
    if (*request != NULL) {
        if ((*request)->ie != NULL) {
            OsalMemFree((*request)->ie);
            (*request)->ie = NULL;
        }
        if ((*request)->ssids != NULL) {
            OsalMemFree((*request)->ssids);
            (*request)->ssids = NULL;
        }
        OsalMemFree(*request);
        *request = NULL;
    }
}

static int32_t WalStartScan(NetDevice *netdev, struct WlanScanRequest *scanParam)
{
    oal_wiphy_stru *wiphy = oal_wiphy_get();

    oal_cfg80211_scan_request_stru *request =
        (oal_cfg80211_scan_request_stru *)OsalMemCalloc(sizeof(oal_cfg80211_scan_request_stru));
    if (request == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{WalStartScan::oom!}");
        return HDF_FAILURE;
    }
    if (WifiScanSetRequest(netdev, scanParam, request) != HDF_SUCCESS) {
        WifiScanFree(&request);
        return HDF_FAILURE;
    }
    // scan request will be released after scan finish
    hi_u32 ret = wal_cfg80211_scan(wiphy, request);
    if (ret != HI_SUCCESS) {
        WifiScanFree(&request);
    }
    return ret;
}

static oal_ieee80211_channel_stru *WalGetChannel(oal_wiphy_stru *wiphy, int32_t freq)
{
    if (wiphy == NULL) {
        HDF_LOGE("%s: capality is NULL!", __func__);
        return NULL;
    }

    enum Ieee80211Band band;
    oal_ieee80211_supported_band *currentBand = NULL;
    int32_t loop;

    for (band = (enum Ieee80211Band)0; band < IEEE80211_NUM_BANDS; band++) {
        currentBand = wiphy->bands[band];
        if (currentBand == NULL) {
            continue;
        }

        for (loop = 0; loop < currentBand->n_channels; loop++) {
            if (currentBand->channels[loop].center_freq == freq) {
                return &currentBand->channels[loop];
            }
        }
    }

    return NULL;
}

static int32_t WalConnect(NetDevice *netDev, WlanConnectParams *param)
{
    if (netDev == NULL || param == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HDF_FAILURE;
    }
    oal_wiphy_stru *wiphy = oal_wiphy_get();
    oal_cfg80211_connect_params_stru cfg80211_params = { 0 };
    if (param->centerFreq != WLAN_FREQ_NOT_SPECFIED) {
        cfg80211_params.channel = WalGetChannel(wiphy, param->centerFreq);
        if ((cfg80211_params.channel == NULL) || (cfg80211_params.channel->flags & WIFI_CHAN_DISABLED)) {
            HDF_LOGE("%s:illegal channel.flags=%u", __func__,
                (cfg80211_params.channel == NULL) ? 0 : cfg80211_params.channel->flags);
            return HDF_FAILURE;
        }
    }
    cfg80211_params.bssid = param->bssid;
    cfg80211_params.ssid = param->ssid;
    cfg80211_params.ie = param->ie;
    cfg80211_params.ssid_len = param->ssidLen;
    cfg80211_params.ie_len = param->ieLen;
    int ret = memcpy_s(&cfg80211_params.crypto, sizeof(cfg80211_params.crypto), &param->crypto, sizeof(param->crypto));
    if (ret != EOK) {
        HDF_LOGE("%s:Copy crypto info failed!ret=%d", __func__, ret);
        return HDF_FAILURE;
    }
    cfg80211_params.key = param->key;
    cfg80211_params.auth_type = (oal_nl80211_auth_type_enum_uint8)param->authType;
    cfg80211_params.privacy = param->privacy;
    cfg80211_params.key_len = param->keyLen;
    cfg80211_params.key_idx = param->keyIdx;
    cfg80211_params.mfp = (oal_nl80211_mfp_enum_uint8)param->mfp;

    return wal_cfg80211_connect(wiphy, netDev, &cfg80211_params);
}

static int32_t SetupWireLessDev(struct NetDevice *netDev, struct WlanAPConf *apSettings)
{
    if (netDev->ieee80211Ptr == NULL) {
        netDev->ieee80211Ptr = (struct wireless_dev *)OsalMemCalloc(sizeof(struct wireless_dev));
        if (netDev->ieee80211Ptr == NULL) {
            return HDF_ERR_INVALID_PARAM;
        }
    }

    if (GET_NET_DEV_CFG80211_WIRELESS(netDev)->preset_chandef.chan == NULL) {
        GET_NET_DEV_CFG80211_WIRELESS(netDev)->preset_chandef.chan =
            (oal_ieee80211_channel *)OsalMemCalloc(sizeof(oal_ieee80211_channel));
        if (GET_NET_DEV_CFG80211_WIRELESS(netDev)->preset_chandef.chan == NULL) {
            OsalMemFree(netDev->ieee80211Ptr);
            netDev->ieee80211Ptr = NULL;
            return HI_ERR_CODE_PTR_NULL;
        }
    }

    GET_NET_DEV_CFG80211_WIRELESS(netDev)->preset_chandef.width = (oal_nl80211_channel_type)apSettings->width;
    GET_NET_DEV_CFG80211_WIRELESS(netDev)->preset_chandef.center_freq1 = apSettings->centerFreq1;
    GET_NET_DEV_CFG80211_WIRELESS(netDev)->preset_chandef.chan->hw_value = apSettings->channel;
    GET_NET_DEV_CFG80211_WIRELESS(netDev)->preset_chandef.chan->band = IEEE80211_BAND_2GHZ;
    return HI_SUCCESS;
}


int32_t WalSetSsid(NetDevice *netDev, const uint8_t *ssid, uint32_t ssidLen)
{
    mac_cfg_ssid_param_stru ssid_param = { 0 };
    hi_u32 ret;

    ssid_param.ssid_len = ssidLen;
    if (memcpy_s(ssid_param.ac_ssid, WLAN_SSID_MAX_LEN, (hi_s8 *)ssid, ssidLen) != EOK) {
        oam_error_log0(0, 0, "{wal_cfg80211_set_ssid::mem safe function err!}");
        return HI_FAIL;
    }

    ret = wal_cfg80211_start_req(netDev, &ssid_param, sizeof(mac_cfg_ssid_param_stru), WLAN_CFGID_SSID, HI_FALSE);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_ssid::return err code [%u]!}", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_u32 wal_cfg80211_set_channel_info(oal_net_device_stru *netdev);
int32_t WalSetChannel(NetDevice *netDev)
{
    return wal_cfg80211_set_channel_info(netDev);
}

hi_u32 wal_cfg80211_fill_beacon_param(mac_vap_stru *mac_vap, oal_beacon_data_stru *beacon_info,
    mac_beacon_param_stru *beacon_param);

int32_t WalChangeBeacon(NetDevice *netDev, struct WlanBeaconConf *param)
{
    mac_beacon_param_stru beacon_param = { 0 };
    beacon_param.l_interval = param->interval;
    beacon_param.l_dtim_period = param->DTIMPeriod;
    beacon_param.hidden_ssid = param->hiddenSSID;
    mac_vap_stru *mac_vap = oal_net_dev_priv(netDev);

    oam_info_log(0, OAM_SF_ANY, "%s: beacon info, head=%p, headLen=%d, tail=%p, tailLen=%d", __func__, param->headIEs,
        param->headIEsLength, param->tailIEs, param->tailIEsLength);

    oal_beacon_data_stru beacon_data = { 0 };
    beacon_data.head = param->headIEs;
    beacon_data.head_len = param->headIEsLength;
    beacon_data.tail = param->tailIEs;
    beacon_data.tail_len = param->tailIEsLength;

    hi_u32 ret = wal_cfg80211_fill_beacon_param(mac_vap, &beacon_data, &beacon_param);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param failed!ret=%d}\r\n", ret);
        return ret;
    }

    beacon_param.operation_type = MAC_ADD_BEACON;
    return wal_cfg80211_start_req((oal_net_device_stru *)netDev, &beacon_param, sizeof(mac_beacon_param_stru),
        WLAN_CFGID_CFG80211_CONFIG_BEACON, HI_FALSE);
}

int32_t WalSetMeshId(NetDevice *netDev, const char *meshId, uint32_t meshIdLen)
{
#ifdef _PRE_WLAN_FEATURE_MESH
    return wal_cfg80211_set_meshid((oal_net_device_stru *)netDev, meshId, meshIdLen);
#else
    (void)netDev;
    (void)meshId;
    (void)meshIdLen;
    return 0;
#endif
}

hi_u32 wal_start_vap(oal_net_device_stru *netdev);

int32_t WalStartAp(NetDevice *netDev)
{
    return wal_start_vap(netDev);
}

int32_t WalStopAp(NetDevice *netDev)
{
    wal_msg_write_stru write_msg;
    if (netDev == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::pst_netdev is null!}");
        goto fail;
    }
    mac_vap_stru *mac_vap = oal_net_dev_priv(netDev);
    if (mac_vap == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::can't get mac vap from netdevice priv data!}");
        goto fail;
    }

    if ((mac_vap->vap_mode != WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        && (mac_vap->vap_mode != WLAN_VAP_MODE_MESH)
#endif
    ) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_stop_ap::vap is not in ap mode!}");
        goto fail;
    }

    if ((oal_netdevice_flags(netDev) & OAL_IFF_RUNNING) == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::vap is already down!}\r\n");
        return HI_SUCCESS;
    }

    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_DOWN_VAP, sizeof(mac_cfg_start_vap_param_stru));

#ifdef _PRE_WLAN_FEATURE_P2P
    oal_wireless_dev *wdev = netDev->ieee80211Ptr;
    wlan_p2p_mode_enum_uint8 p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(wdev->iftype);
    if (WLAN_P2P_BUTT == p2p_mode) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::wal_wireless_iftype_to_mac_p2p_mode return BUFF}\r\n");
        goto fail;
    }
    oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_stop_ap::en_p2p_mode=%u}\r\n", p2p_mode);

    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->p2p_mode = p2p_mode;
#endif

    ((mac_cfg_start_vap_param_stru *)write_msg.auc_value)->net_dev = netDev;

    hi_u32 ret = wal_send_cfg_event(netDev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_start_vap_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (oal_unlikely(ret != HI_SUCCESS)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_stop_ap::failed to stop ap, error[%u]}", ret);
        goto fail;
    }

    return HI_SUCCESS;

fail:
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return HI_FAIL;
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return -HI_FAIL;
#endif
}

int32_t WalDelStation(NetDevice *netDev, const uint8_t *macAddr)
{
    oal_station_del_parameters_stru params = { 0 };
    params.mac = macAddr;
    return wal_cfg80211_del_station(NULL, netDev, &params);
}

int32_t WalAddKey(struct NetDevice *netdev, uint8_t keyIndex, bool pairwise, const uint8_t *macAddr,
    struct KeyParams *params)
{
    oal_key_params_stru keyParm = { 0 };
    keyParm.key = params->key;
    keyParm.key_len = params->keyLen;
    keyParm.seq_len = params->seqLen;
    keyParm.seq = params->seq;
    keyParm.cipher = params->cipher;
    return wal_cfg80211_add_key(NULL, netdev, keyIndex, pairwise, macAddr, &keyParm);
}

int32_t WalDelKey(struct NetDevice *netdev, uint8_t keyIndex, bool pairwise, const uint8_t *macAddr)
{
    return wal_cfg80211_remove_key(NULL, netdev, keyIndex, pairwise, macAddr);
}

int32_t WalSetDefaultKey(struct NetDevice *netDev, uint8_t keyIndex, bool unicast, bool multicas)
{
    return wal_cfg80211_set_default_key(NULL, netDev, keyIndex, unicast, multicas);
}

int32_t WalSetMacAddr(NetDevice *netDev, uint8_t *mac, uint8_t len)
{
    (void)netDev;
    uint32_t ret;

    ret = wal_macaddr_check(mac);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "mac addr is unavailable!");
        return ret;
    }
    ret = memcpy_s(g_macStorage.mac, ETHER_ADDR_LEN, mac, len);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "memcpy_s failed!");
        return ret;
    }
    g_macStorage.isStorage = true;
    return HI_SUCCESS;
}

static int32_t PreSetMac(NetDevice *netDev, hi_u8 *mac, hi_u8 len)
{
    uint32_t ret;
    Mac80211SetMacParam param;
    wal_msg_write_stru write_msg;
    mac_cfg_staion_id_param_stru *mac_param = HI_NULL;

    ret = NetIfSetMacAddr(netDev, mac, len);
    if (ret != HDF_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "NetIfSetMacAddr failed!");
        return ret;
    }
    if (memcpy_s(param.mac, ETHER_ADDR_LEN, mac, ETHER_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "WalSetMacAddr:: memcpy_s FAILED");
        return HI_FAIL;
    }
    param.p2pMode = false;
    param.resv = 0;
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_STATION_ID, sizeof(mac_cfg_staion_id_param_stru));
    mac_param = (mac_cfg_staion_id_param_stru *)write_msg.auc_value;
    if (memcpy_s(mac_param, sizeof(mac_cfg_staion_id_param_stru), &param, sizeof(mac_cfg_staion_id_param_stru)) !=
        EOK) {
        oam_error_log0(0, OAM_SF_ANY, "WalSetMacParam:: memcpy_s FAILED");
        return HI_FAIL;
    }
    ret = wal_send_cfg_event(netDev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_staion_id_param_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_set_mac_to_mib::return err code [%u]!}\r\n", ret);
        return ret;
    }

    ret = memset_s(g_macStorage.mac, ETHER_ADDR_LEN, 0, len);
    if (ret != HDF_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "memset_s failed!");
        return ret;
    }
    g_macStorage.isStorage = false;
    return HI_SUCCESS;
}

int32_t WalSetMode(NetDevice *netDev, enum WlanWorkMode iftype)
{
    int32_t ret;
    uint32_t flags = 0;
    oal_vif_params_stru params = {
        .use_4addr = 0,
        .macaddr = NULL
    };
    oam_error_log1(0, OAM_SF_ANY, "{WalSetMode enter iftype:%d!}", iftype);
    if (iftype == WLAN_WORKMODE_STA || iftype == WLAN_WORKMODE_AP) {
        wal_deinit_drv_wlan_netdev(netDev);
        ret = RenewNetDevice(&netDev);
        if (ret != HDF_SUCCESS) {
            oam_error_log(0, OAM_SF_ANY, "%s:RenewNetDevice failed!ret=%d", __func__, ret);
            return ret;
        }
        ret = wal_init_drv_wlan_netdev(iftype, WAL_PHY_MODE_11N, netDev);
        if (ret != HISI_OK) {
            oam_error_log0(0, OAM_SF_ANY, "wal_init_drv_wlan_netdev failed!");
            return ret;
        }
    }
    if (g_macStorage.isStorage) {
        ret = PreSetMac(netDev, g_macStorage.mac, ETHER_ADDR_LEN);
        if (ret != HI_SUCCESS) {
            oam_error_log0(0, OAM_SF_ANY, "PreSetMac failed!}");
            return HI_FAIL;
        }
        params.macaddr = (hi_u8 *)netDev->macAddr;
    }

    if (iftype == WLAN_WORKMODE_STA)
        return HI_SUCCESS;
    return wal_cfg80211_change_virtual_intf(NULL, netDev, iftype, &flags, &params);
}


int32_t WalAbortScan(NetDevice *netDev)
{
    return wal_force_scan_complete(netDev);
}


extern hi_u8 g_efuseMacExist;
int32_t WalGetDeviceMacAddr(NetDevice *netDev, int32_t type, uint8_t *mac, uint8_t len)
{
    (void)netDev;
    if (mac == NULL || len != ETHER_ADDR_LEN) {
        oam_error_log0(0, OAM_SF_ANY, "{WalGetDeviceMacAddr::input param error!}");
        return HI_FAIL;
    }
    if (!g_efuseMacExist) {
        /* if there is no data in efuse */
        oam_warning_log0(0, OAM_SF_ANY, "wal_get_efuse_mac_addr:: no data in efuse!");
        return HDF_ERR_NOT_SUPPORT;
    }
    if (wal_get_dev_addr(mac, len, type) != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_ANY, "{set_mac_addr_by_type::GetDeviceMacAddr failed!}");
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

int32_t WalGetValidFreqsWithBand(NetDevice *netDev, int32_t band, int32_t *freqs, uint32_t *num)
{
    (void)netDev;
    uint32_t freqIndex = 0;
    uint32_t channelNumber;
    uint32_t freqTmp;
    uint32_t minFreq;
    uint32_t maxFreq;
    const struct ieee80211_regdomain *regdom = NULL;

    regdom = wal_get_cfg_regdb();
    if (regdom == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wal_get_cfg_regdb failed!");
        return HI_FAIL;
    }
    minFreq = regdom->reg_rules[0].freq_range.start_freq_khz / MHZ_TO_KHZ;
    maxFreq = regdom->reg_rules[0].freq_range.end_freq_khz / MHZ_TO_KHZ;
    switch (band) {
        case WLAN_BAND_2G:
            for (channelNumber = 1; channelNumber <= WIFI_24G_CHANNEL_NUMS; channelNumber++) {
                if (channelNumber < WAL_MAX_CHANNEL_2G) {
                    freqTmp = WAL_MIN_FREQ_2G + (channelNumber - 1) * WAL_FREQ_2G_INTERVAL;
                } else if (channelNumber == WAL_MAX_CHANNEL_2G) {
                    freqTmp = WAL_MAX_FREQ_2G;
                }
                if (freqTmp < minFreq || freqTmp > maxFreq) {
                    continue;
                }
                freqs[freqIndex] = freqTmp;
                freqIndex++;
            }
            *num = freqIndex;
            break;
        default:
            oam_error_log0(0, OAM_SF_ANY, "not support this band!");
            return HDF_ERR_NOT_SUPPORT;
    }
    return HI_SUCCESS;
}

int32_t WalSetTxPower(NetDevice *netDev, int32_t power)
{
    int32_t ret;
    wal_msg_write_stru write_msg;
    wal_write_msg_hdr_init(&write_msg, WLAN_CFGID_TX_POWER, sizeof(hi_s32));
    *((hi_s32 *)(write_msg.auc_value)) = power;

    ret = wal_send_cfg_event(netDev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(hi_s32),
        (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, 0, "{WalSetTxPower::return err code %u!}", ret);
        return ret;
    }
    return HI_SUCCESS;
}

int32_t WalGetAssociatedStasCount(NetDevice *netDev, uint32_t *num)
{
    mac_vap_stru *mac_vap = HI_NULL;
    mac_vap = (mac_vap_stru *)oal_net_dev_priv(netDev);
    if (mac_vap == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "WalGetUserNum:: mac_vap is null");
        return HI_FAIL;
    }
    *num = mac_vap->user_nums;
    return HI_SUCCESS;
}

int32_t WalGetAssociatedStasInfo(NetDevice *netDev, WifiStaInfo *staInfo, uint32_t num)
{
    uint32_t number = 0;
    mac_vap_stru *mac_vap = HI_NULL;
    hi_list *entry = HI_NULL;
    hi_list *user_list_head = HI_NULL;
    mac_user_stru *user_tmp = HI_NULL;

    mac_vap = (mac_vap_stru *)oal_net_dev_priv(netDev);
    if (mac_vap == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "WalGetAssociatedStas:: mac_vap is null");
        return HI_FAIL;
    }
    user_list_head = &(mac_vap->mac_user_list_head);
    for (entry = user_list_head->next; entry != user_list_head && number < num; number++) {
        user_tmp = hi_list_entry(entry, mac_user_stru, user_dlist);
        if (user_tmp == HI_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "WalGetAssociatedStas:: mac_user is null");
            return HI_FAIL;
        }
        if (memcpy_s(staInfo + number, ETHER_ADDR_LEN, user_tmp->user_mac_addr, ETHER_ADDR_LEN)) {
            oam_error_log0(0, OAM_SF_ANY, "WalGetAssociatedStas:: memcpy_s failed!");
            return HI_FAIL;
        }
        entry = entry->next;
    }
    return HI_SUCCESS;
}

int32_t WalSetCountryCode(NetDevice *netDev, const char *code, uint32_t len)
{
    int32_t ret;
    if (code == NULL || strlen(code) != len) {
        oam_error_log0(0, OAM_SF_ANY, "{WalSetCountryCode::input param error!}");
        return HI_FAIL;
    }
    if (len > (MAC_CONTRY_CODE_LEN - 1)) {
        oam_error_log0(0, OAM_SF_ANY, "{WalSetCountryCode::country code len error!}\r\n");
        return HI_FAIL;
    }
    ret = wal_regdomain_update(netDev, code, MAC_CONTRY_CODE_LEN);
    if (ret != HI_SUCCESS) {
        oam_error_log1(0, OAM_SF_ANY, "{WalSetCountryCode::regdomain_update return err code %u!}\r\n", ret);
        return ret;
    }
    return HI_SUCCESS;
}

int32_t WalSetScanningMacAddress(NetDevice *netDev, unsigned char *mac, uint32_t len)
{
    (void)netDev;
    (void)mac;
    (void)len;
    return HDF_ERR_NOT_SUPPORT;
}

int32_t WalConfigAp(NetDevice *netDev, struct WlanAPConf *apConf)
{
    int32_t ret = SetupWireLessDev(netDev, apConf);
    if (ret != HI_SUCCESS) {
        oam_error_log2(0, OAM_SF_ANY, "{%s::SetupWireLessDev failed!ret=%d!}\r\n", __func__, ret);
        return ret;
    }
    ret = WalSetChannel(netDev);
    if (ret != HI_SUCCESS) {
        oam_error_log2(0, OAM_SF_ANY, "{%s::WalSetChannel failed!ret=%d!}\r\n", __func__, ret);
        return ret;
    }
    ret = WalSetSsid(netDev, apConf->ssidConf.ssid, apConf->ssidConf.ssidLen);
    if (ret != HI_SUCCESS) {
        oam_error_log2(0, OAM_SF_ANY, "{%s::WalSetSsid failed!ret=%d!}\r\n", __func__, ret);
        return ret;
    }
    return HI_SUCCESS;
}

void WalReleaseHwCapability(struct WlanHwCapability *self)
{
    uint8_t i;
    if (self == NULL) {
        return;
    }
    for (i = 0; i < IEEE80211_NUM_BANDS; i++) {
        if (self->bands[i] != NULL) {
            OsalMemFree(self->bands[i]);
            self->bands[i] = NULL;
        }
    }
    if (self->supportedRates != NULL) {
        OsalMemFree(self->supportedRates);
        self->supportedRates = NULL;
    }
    OsalMemFree(self);
}


int32_t WalGetHwCapability(struct NetDevice *netDev, struct WlanHwCapability **capability)
{
    uint8_t loop;
    (void)netDev;
    if (netDev == NULL || GET_NET_DEV_CFG80211_WIRELESS(netDev) == NULL ||
        GET_NET_DEV_CFG80211_WIRELESS(netDev)->wiphy == NULL) {
        return HI_FAIL;
    }
    if (capability == NULL) {
        return HI_FAIL;
    }
    oal_ieee80211_supported_band *band = GET_NET_DEV_CFG80211_WIRELESS(netDev)->wiphy->bands[IEEE80211_BAND_2GHZ];
    struct WlanHwCapability *hwCapability = (struct WlanHwCapability *)OsalMemCalloc(sizeof(struct WlanHwCapability));
    if (hwCapability == NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{%s::oom!}\r\n", __func__);
        return HI_FAIL;
    }
    hwCapability->Release = WalReleaseHwCapability;
    if (hwCapability->bands[IEEE80211_BAND_2GHZ] == NULL) {
        hwCapability->bands[IEEE80211_BAND_2GHZ] =
            OsalMemCalloc(sizeof(struct WlanBand) + (sizeof(struct WlanChannel) * band->n_channels));
        if (hwCapability->bands[IEEE80211_BAND_2GHZ] == NULL) {
            oam_error_log1(0, OAM_SF_ANY, "{%s::oom!}\r\n", __func__);
            WalReleaseHwCapability(hwCapability);
            return HI_FAIL;
        }
    }
    hwCapability->htCapability = band->ht_cap.cap;
    hwCapability->bands[IEEE80211_BAND_2GHZ]->channelCount = band->n_channels;
    for (loop = 0; loop < band->n_channels; loop++) {
        hwCapability->bands[IEEE80211_BAND_2GHZ]->channels[loop].centerFreq = band->channels[loop].center_freq;
        hwCapability->bands[IEEE80211_BAND_2GHZ]->channels[loop].flags = band->channels[loop].flags;
        hwCapability->bands[IEEE80211_BAND_2GHZ]->channels[loop].channelId = band->channels[loop].hw_value;
    }
    hwCapability->supportedRateCount = band->n_bitrates;
    hwCapability->supportedRates = OsalMemCalloc(sizeof(uint16_t) * band->n_bitrates);
    if (hwCapability->supportedRates == NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{%s::oom!}\r\n", __func__);
        WalReleaseHwCapability(hwCapability);
        return HI_FAIL;
    }
    for (loop = 0; loop < band->n_bitrates; loop++) {
        hwCapability->supportedRates[loop] = band->bitrates[loop].bitrate;
    }
    *capability = hwCapability;
    return HI_SUCCESS;
}

int32_t WalRemainOnChannel(struct NetDevice *netDev, WifiOnChannel *onChannel)
{
    if (netDev == NULL || onChannel == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HI_FAIL;
    }
    oal_wiphy_stru *wiphy = oal_wiphy_get();
    oal_wireless_dev *wdev = GET_NET_DEV_CFG80211_WIRELESS(netDev);
    hi_u64 pullCookie = 0;
    hi_u8 channelIdx;
    oal_ieee80211_channel* wifi2ghzChannels = wal_get_g_wifi_2ghz_channels();

    channelIdx = (hi_u8)oal_ieee80211_frequency_to_channel(onChannel->freq);

    return (hi_s32)wal_cfg80211_remain_on_channel(wiphy, wdev, &(wifi2ghzChannels[channelIdx - 1]),
                                                 (hi_u32)onChannel->duration, &pullCookie);
}

int32_t WalCancelRemainOnChannel(struct NetDevice *netDev)
{
    if (netDev == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HI_FAIL;
    }

    oal_wireless_dev *wdev = GET_NET_DEV_CFG80211_WIRELESS(netDev);
    return (hi_s32)wal_cfg80211_cancel_remain_on_channel(HI_NULL, wdev, (hi_u64)0);
}

int32_t WalProbeReqReport(struct NetDevice *netDev, int32_t report)
{
    (void)report;
    if (netDev == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

int32_t WalAddIf(struct NetDevice *netDev, WifiIfAdd *ifAdd)
{
    if (netDev == NULL || ifAdd == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HI_FAIL;
    }

    int32_t ret = InitNetdev(netDev, (nl80211_iftype_uint8)ifAdd->type);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, 0, "hwal_ioctl_add_if: wal_init_drv_wlan_netdev failed!");
        return -HI_FAIL;
    }
    return HI_SUCCESS;
}

int32_t WalRemoveIf(struct NetDevice *netDev, WifiIfRemove *ifRemove)
{
    if (netDev == NULL || ifRemove == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HI_FAIL;
    }

    struct NetDevice *removeNetdev = NULL;
    removeNetdev = NetDeviceGetInstByName((const char*)(ifRemove->ifname));
    if (removeNetdev == NULL) {
        return -HI_FAIL;
    }
    int32_t ret = wal_deinit_drv_wlan_netdev(removeNetdev);
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, 0, "hwal_ioctl_remove_if: wal_deinit_drv_wlan_netdev failed!");
        return -HI_FAIL;
    }
    NetDeviceDeInit(removeNetdev);
    return HI_SUCCESS;
}

int32_t WalSetApWpsP2pIe(struct NetDevice *netDev, WifiAppIe *appIe)
{
    if (netDev == NULL || appIe == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HI_FAIL;
    }

    if (appIe->ieLen > WLAN_WPS_IE_MAX_SIZE) {
        oam_error_log0(0, 0, "app ie length is too large!");
        return -HI_FAIL;
    }

    return (hi_s32)wal_ioctl_set_wps_p2p_ie(netDev, appIe->ie, appIe->ieLen,
                                            appIe->appIeType);
}

int32_t WalGetDriverFlag(struct NetDevice *netDev, WifiGetDrvFlags **params)
{
    if (netDev == NULL || params == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HI_FAIL;
    }
    oal_wireless_dev *wdev = GET_NET_DEV_CFG80211_WIRELESS(netDev);
    WifiGetDrvFlags *getDrvFlag = (WifiGetDrvFlags *)OsalMemCalloc(sizeof(WifiGetDrvFlags));
    switch (wdev->iftype) {
        case NL80211_IFTYPE_P2P_CLIENT:
             /* fall-through */
        case NL80211_IFTYPE_P2P_GO:
            getDrvFlag->drvFlags = (hi_u64)(HISI_DRIVER_FLAGS_AP);
            break;
        case NL80211_IFTYPE_P2P_DEVICE:
            getDrvFlag->drvFlags = (hi_u64)(HISI_DRIVER_FLAGS_P2P_DEDICATED_INTERFACE |
                                            HISI_DRIVER_FLAGS_P2P_CONCURRENT |
                                            HISI_DRIVER_FLAGS_P2P_CAPABLE);
            break;
        default:
            getDrvFlag->drvFlags = 0;
    }
    *params = getDrvFlag;
    return HI_SUCCESS;
}

int32_t InitMsgHdr (wal_msg_write_stru *writeMsg, WifiActionData *actionData)
{
    hi_unref_param(writeMsg);

    if(actionData->data[0] == MAC_ACTION_CATEGORY_SELF_PROTECTED) {
#ifdef _PRE_WLAN_FEATURE_MESH
        wal_write_msg_hdr_init(writeMsg, WLAN_CFGID_SEND_MESH_ACTION, sizeof(WifiActionData));
#else
        return HI_FAIL;
#endif
    } else if (actionData->data[0] == MAC_ACTION_CATEGORY_PUBLIC) {
#ifdef _PRE_WLAN_FEATURE_P2P
        wal_write_msg_hdr_init(writeMsg, WLAN_CFGID_SEND_P2P_ACTION, sizeof(WifiActionData));
#endif
    } else {
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

int32_t WalSendAction(struct NetDevice *netDev, WifiActionData *actionData)
{
    wal_msg_write_stru write_msg;

    if (netDev == NULL || actionData == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HI_FAIL;
    }
    if (InitMsgHdr(&write_msg, actionData) == HI_FAIL) {
        return HI_FAIL;
    }

    mac_action_data_stru *actionParam = (mac_action_data_stru *)(write_msg.auc_value);

    if (memcpy_s(actionParam->dst, WLAN_MAC_ADDR_LEN, actionData->dst, WLAN_MAC_ADDR_LEN) ||
        memcpy_s(actionParam->src, WLAN_MAC_ADDR_LEN, actionData->src, WLAN_MAC_ADDR_LEN) ||
        memcpy_s(actionParam->bssid, WLAN_MAC_ADDR_LEN, actionData->bssid, WLAN_MAC_ADDR_LEN)) {
        oam_error_log0(0, 0, "hwal_ioctol_send_action :oal_copy_from_user_new dst,src or bssid ailed ");
        return HI_FAIL;    
    }

    actionParam->data = HI_NULL;
    if (actionData->dataLen > 0 && actionData->dataLen <= MAX_ACTION_DATA_LEN) {
        actionParam->data = (hi_u8 *)OsalMemCalloc(actionData->dataLen * sizeof(hi_u8));
        if (oal_unlikely(actionParam->data == HI_NULL)) {
            oam_error_log0(0, OAM_SF_CFG, "{hwal_ioctol_send_action ::puc_data alloc mem return null ptr!}");
            return HI_FAIL;  
        }
        if (memcpy_s(actionParam->data, actionData->dataLen, actionData->data, actionData->dataLen)) {
            oam_error_log0(0, 0, "hwal_ioctl_send_action :oal_copy_from_user_new action_data failed. ");
            return HI_FAIL;
        }
    }
    actionParam->data_len = actionData->dataLen;

    oam_warning_log4(0, 0, "hwal_ioctl_send_action send action frame(mac addr = %02X:XX:%02X:XX:%02X:%02X)",
        actionParam->dst[0], actionParam->dst[2],   /* 0 2 */
        actionParam->dst[4], actionParam->dst[5]);  /* 4 5 */

    hi_s32 ret = wal_send_cfg_event(netDev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_action_data_stru), (hi_u8 *)&write_msg, HI_FALSE, HI_NULL);
    if (ret != HI_SUCCESS) {
        oam_warning_log1(0,0, "{hwal_ioctl_send_action ::send action frame to driver failed[%d].}", ret);
        OsalMemFree(actionParam->data);
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

int32_t WalGetIftype(struct NetDevice *netDev, uint8_t *iftype)
{
    iftype = &(GET_NET_DEV_CFG80211_WIRELESS(netDev)->iftype);
    return HI_SUCCESS;
}

static struct HdfMac80211BaseOps g_baseOps = {
    .SetMode = WalSetMode,
    .AddKey = WalAddKey,
    .DelKey = WalDelKey,
    .SetDefaultKey = WalSetDefaultKey,
    .GetDeviceMacAddr = WalGetDeviceMacAddr,
    .SetMacAddr = WalSetMacAddr,
    .SetTxPower = WalSetTxPower,
    .GetValidFreqsWithBand = WalGetValidFreqsWithBand,
    .GetHwCapability = WalGetHwCapability,
    .RemainOnChannel = WalRemainOnChannel,
    .CancelRemainOnChannel = WalCancelRemainOnChannel,
    .ProbeReqReport = WalProbeReqReport,
    .AddIf = WalAddIf,
    .RemoveIf = WalRemoveIf,
    .SetApWpsP2pIe = WalSetApWpsP2pIe,
    .GetDriverFlag = WalGetDriverFlag,
    .SendAction = WalSendAction,
    .GetIftype = WalGetIftype,
};

static struct HdfMac80211STAOps g_staOps = {
    .Connect =
    .Disconnect =
    .StartScan = WalStartScan,
    .AbortScan = WalAbortScan,
    .SetScanningMacAddress = WalSetScanningMacAddress,
};

static struct HdfMac80211APOps g_apOps = {
    .ConfigAp = WalConfigAp,
    .StartAp = WalStartAp,
    .StopAp = WalStopAp,
    .ConfigBeacon = WalChangeBeacon,
    .DelStation = WalDelStation,
    .SetCountryCode = WalSetCountryCode,
    .GetAssociatedStasCount = WalGetAssociatedStasCount,
    .GetAssociatedStasInfo = WalGetAssociatedStasInfo
};

hi_void HiMac80211Init(struct HdfChipDriver *chipDriver)
{
    if (chipDriver == NULL) {
        oam_error_log(0, OAM_SF_ANY, "%s:input is NULL!", __func__);
        return;
    }
    chipDriver->ops = &g_baseOps;
    chipDriver->staOps = &g_staOps;
    chipDriver->apOps = &g_apOps;
}
