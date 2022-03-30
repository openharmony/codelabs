/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include "cmsis_os2.h"
#include "base64.h"
#include "ohos_types.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/ip4_addr.h"
#include "wifi_hotspot.h"
#include "wifi_device_config.h"
#include "iot_netcfg_nan.h"
#include "network_config_service.h"
#include "iot_demo_def.h"

WifiDeviceConfig g_netCfg = {0};
WifiEvent g_staEventHandler = {0};
struct netif *g_staNetif = NULL;

int g_connectRetryCount = 0;


static NetCfgEventCallback g_netCfgEventCallback = NULL;

#define SET_NET_EVENT(e, d)    ({            \
    if (g_netCfgEventCallback != NULL) {    \
        g_netCfgEventCallback(e, d);        \
    }                                       \
})

const char *g_ssid = "Hi-xxx-Switchs-XXXXX     ";
const char *g_pinCode = "11111111";
const char *g_productId = "1";
const char *g_sn = "01234567890123450123456789012345";

static unsigned int ChannelToFrequency(unsigned int channel)
{
    if (channel <= 0) {
        return 0;
    }

    if (channel == CHANNEL_80211B_ONLY) {
        return FREQ_OF_CHANNEL_80211B_ONLY;
    }

    return (((channel - WIFI_MIN_CHANNEL) * WIFI_FREQ_INTERVAL) + FREQ_OF_CHANNEL_1);
}

static void NetCfgResult(signed char result)
{
    RaiseLog(LOG_LEVEL_INFO, "Network configure done.(result=%d)\n", result);
    UnRegisterWifiEvent(&g_staEventHandler);
    NotifyNetCfgResult(result);
}

static void StaResetAddr(struct netif *ptrLwipNetif)
{
    ip4_addr_t staGW;
    ip4_addr_t staIpaddr;
    ip4_addr_t staNetmask;

    if (ptrLwipNetif == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Null param of netdev\r\n");
        return;
    }

    IP4_ADDR(&staGW, 0, 0, 0, 0);
    IP4_ADDR(&staIpaddr, 0, 0, 0, 0);
    IP4_ADDR(&staNetmask, 0, 0, 0, 0);

    netifapi_netif_set_addr(ptrLwipNetif, &staIpaddr, &staNetmask, &staGW);
}

static int g_state;

static void *WifiConnectTask(const char *arg)
{
    (void)arg;
    if (g_state == WIFI_STATE_AVALIABLE) {
        NetCfgResult(0);
        RaiseLog(LOG_LEVEL_INFO, "WiFi: Connected.\n");
        netifapi_dhcp_start(g_staNetif);
    } else if (g_state == WIFI_STATE_NOT_AVALIABLE) {
        RaiseLog(LOG_LEVEL_INFO, "WiFi: Disconnected retry = %d\n", g_connectRetryCount);
        if (g_connectRetryCount < TEST_CONNECT_RETRY_COUNT) {
            g_connectRetryCount++;
            return NULL;
        }
        NetCfgResult(-1);
        netifapi_dhcp_stop(g_staNetif);
        StaResetAddr(g_staNetif);
    }
    return NULL;
}

static void WifiConnectionChangedHandler(int state, WifiLinkedInfo *info)
{
    (void)info;
    osThreadAttr_t attr;
    attr.name = "WifiConnectTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NETCFG_TASK_STACK_SIZE;
    attr.priority = NETCFG_TASK_PRIO;
    g_state = state;
    if (osThreadNew((osThreadFunc_t)WifiConnectTask, NULL, &attr) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Falied to create WifiConnectTask!\n");
    }
}

static int StaStart(void)
{
    WifiErrorCode error;
    error = EnableWifi();
    if (error == ERROR_WIFI_BUSY) {
        RaiseLog(LOG_LEVEL_ERR, "Sta had already connnected.\n");
        NetCfgResult(0);
    }
    if ((error != ERROR_WIFI_BUSY) && (error != WIFI_SUCCESS)) {
        RaiseLog(LOG_LEVEL_ERR, "EnableWifi failed, error = %d\n", error);
        return -1;
    }

    g_staEventHandler.OnWifiConnectionChanged = WifiConnectionChangedHandler;
    error = RegisterWifiEvent(&g_staEventHandler);
    if (error != WIFI_SUCCESS) {
        RaiseLog(LOG_LEVEL_ERR, "RegisterWifiEvent failed, error = %d\n", error);
        return -1;
    }

    if (IsWifiActive() == 0) {
        RaiseLog(LOG_LEVEL_ERR, "Wifi station is not actived.\n");
        return -1;
    }

    g_staNetif = netif_find("wlan0");
    if (g_staNetif == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Get netif failed\n");
        return -1;
    }

    return 0;
}

static int WapStaConnect(WifiDeviceConfig *config)
{
    int netId = 0;
    WifiErrorCode error;
    config->securityType = (config->preSharedKey[0] == '\0') ? WIFI_SEC_TYPE_OPEN : WIFI_SEC_TYPE_PSK;
    error = AddDeviceConfig(config, &netId);
    if (error != WIFI_SUCCESS) {
        RaiseLog(LOG_LEVEL_ERR, "AddDeviceConfig add config failed, error=%d\n", error);
        return -1;
    }
    int count = 0;
    while (count < FAST_CONNECT_RETRY_NUM) {
        error = ConnectTo(netId);
        if (error == WIFI_SUCCESS) {
            break;
        }
        RaiseLog(LOG_LEVEL_INFO, "[sample]continue\n");
        count++;
    }

    if (error != WIFI_SUCCESS) {
        RaiseLog(LOG_LEVEL_ERR, "ConnectTo conn failed %d\n", error);
        return -1;
    }

    RaiseLog(LOG_LEVEL_INFO, "WapSta connecting...\n");
    return 0;
}

static void *CfgNetTask(const char *arg)
{
    (void)arg;

    if (StaStart() != 0) {
        SET_NET_EVENT(NET_EVENT_CONFIG_FAIL, NULL);
        return NULL;
    }

    if (WapStaConnect(&g_netCfg) != 0) {
        SET_NET_EVENT(NET_EVENT_CONFIG_FAIL, NULL);
        return NULL;
    }

    SET_NET_EVENT(NET_EVENT_CONNECTTED, NULL);

    return NULL;
}

static int CreateCfgNetTask(void)
{
    osThreadAttr_t attr;
    attr.name = "CfgNetTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NETCFG_TASK_STACK_SIZE;
    attr.priority = NETCFG_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)CfgNetTask, NULL, &attr) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Falied to create NanCfgNetTask!\n");
        return -1;
    }

    return 0;
}

static void DealSsidPwd(const WifiDeviceConfig *config)
{
    if (config == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Input config is illegal.\n");
        return;
    }

    if (memcpy_s(&g_netCfg, sizeof(WifiDeviceConfig), config, sizeof(WifiDeviceConfig)) != 0) {
        RaiseLog(LOG_LEVEL_ERR, "memcpy netCfg failed.\n");
        return;
    }
    RaiseLog(LOG_LEVEL_INFO, "DealSsidPwd\n");
    g_connectRetryCount = 0;
    if (CreateCfgNetTask() != 0) {
        RaiseLog(LOG_LEVEL_ERR, "Create cfgnet task failed.\n");
    }
}

int GetPinCode(unsigned char *pinCode, unsigned int size, unsigned int *len)
{
    if (pinCode == NULL) {
        return -1;
    }
    memset_s(pinCode, size, 0, size);
    if (strncpy_s((char *)pinCode, size, g_pinCode, strlen(g_pinCode)) != 0) {
        RaiseLog(LOG_LEVEL_ERR, "GetPinCode copy pinCode failed\n");
        return -1;
    }
    *len = strlen((char *)pinCode);
    return 0;
}

int FastConnect(const struct WifiInfo *wifiInfo, WifiDeviceConfig *destCfg)
{
    if (memcpy_s(destCfg->ssid, sizeof(destCfg->ssid), wifiInfo->ssid, wifiInfo->ssidLen) != EOK) {
        RaiseLog(LOG_LEVEL_ERR, "FastConnect copy ssid failed\n");
        return -1;
    }
    if (memcpy_s(destCfg->preSharedKey, sizeof(destCfg->preSharedKey), wifiInfo->psk, wifiInfo->pskLen) != EOK) {
        RaiseLog(LOG_LEVEL_ERR, "FastConnect copy pwd failed\n");
        return -1;
    }
    if (memcpy_s(destCfg->bssid, sizeof(destCfg->bssid), wifiInfo->bssid, wifiInfo->bssidLen) != EOK) {
        RaiseLog(LOG_LEVEL_ERR, "FastConnect copy bssid failed\n");
        return -1;
    }
    destCfg->securityType = wifiInfo->authMode;
    destCfg->freq = ChannelToFrequency(wifiInfo->channelNumber);
    destCfg->wapiPskType = WIFI_PSK_TYPE_HEX;
    return 0;
}

int ParseNetCfgData(const struct WifiInfo *wifiInfo, const unsigned char *vendorData, unsigned int len)
{
    RaiseLog(LOG_LEVEL_INFO, "ParseWifiData vendorData len:%d\n", len);
    if (wifiInfo == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "wifiInfo is NULL\n");
        return -1;
    }

    WifiDeviceConfig netConfig;
    memset_s(&netConfig, sizeof(netConfig), 0, sizeof(netConfig));
    FastConnect(wifiInfo, &netConfig);

    if (vendorData != NULL) {
        /* process vendorData */
    }

    DealSsidPwd(&netConfig);
    return 0;
}

int SendRawEncodeData(const unsigned char *data, size_t len)
{
    size_t writeLen = 0;
    int ret = mbedtls_base64_encode(NULL, 0, &writeLen, (const unsigned char *)data, len);
    if (ret != 0) {
        RaiseLog(LOG_LEVEL_ERR, "SendRawEncodeData base64 encode fial\n");
        return -1;
    }
    size_t encodeBufLen = writeLen;
    if (writeLen > MAX_DATA_LEN) {
        RaiseLog(LOG_LEVEL_ERR, "SendRawEncodeData dataLen overSize\n");
        return -1;
    }
    char *buf = malloc(writeLen + 1);
    if (buf == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Malloc failed\r\n");
        return -1;
    }
    (void)memset_s(buf, writeLen + 1, 0, writeLen + 1);
    if (mbedtls_base64_encode((unsigned char *)buf, encodeBufLen, &writeLen, (const unsigned char *)data, len) != 0) {
        RaiseLog(LOG_LEVEL_ERR, "SendRawEncodeData base64 encode failed\r\n");
        free(buf);
        buf = NULL;
        return -1;
    }

    LOG_I("SendRawEncodeData encode buf = %s\n", buf);
    SendRawData((const char*)buf);
    if (buf != NULL) {
        free(buf);
    }

    return 0;
}

void NotifyNetCfgStatus(enum NetCfgStatus status)
{
    (void)status;
    return;
}

static int RecvRawData(const char *svcId, unsigned int mode, const char *data)
{
    (void)svcId;
    (void)mode;

    RaiseLog(LOG_LEVEL_DEBUG, "data : %s \n\n", data);
    SET_NET_EVENT(NET_EVENT_RECV_DATA, (void *)data);
    return 0;
}

static void *NetCfgTask(void *arg)
{
    (void)arg;
    int ret;
    ret = SetSafeDistancePower(POWER_NUM); // Make sure the device discovery distance about 30 cm
    if (ret != 0) {
        RaiseLog(LOG_LEVEL_ERR, "Set saft distance power failed\n");
        return NULL;
    }

    struct SoftAPParam config = {0};
    memset_s(&config, sizeof(struct SoftAPParam), 0, sizeof(struct SoftAPParam));
    strncpy_s(config.ssid, sizeof(config.ssid), g_ssid, strlen(g_ssid));
    config.authType = WIFI_SECURITY_OPEN;
    ret = SetSoftAPParameter(&config);
    if (ret != 0) {
        RaiseLog(LOG_LEVEL_ERR, "Set softAP parameters failed\n");
        return NULL;
    }

    NetCfgCallback hook;
    memset_s(&hook, sizeof(NetCfgCallback), 0, sizeof(NetCfgCallback));
    hook.GetPinCode = GetPinCode;
    hook.ParseNetCfgData = ParseNetCfgData;
    hook.RecvRawData = RecvRawData;
    hook.NotifyNetCfgStatus = NotifyNetCfgStatus;
    ret = RegNetCfgCallback(&hook);
    if (ret != 0) {
        RaiseLog(LOG_LEVEL_ERR, "Register config callback failed\n");
        return NULL;
    }

    struct DevInfo devInfo[DEVICE_INFO_NUM];
    memset_s(&devInfo, sizeof(devInfo), 0, sizeof(devInfo));
    devInfo[0].key = "productId";
    devInfo[1].key = "sn";
    devInfo[0].value = g_productId;
    devInfo[1].value = g_sn;
    ret = StartNetCfg(devInfo, DEVICE_INFO_NUM, NETCFG_SOFTAP_NAN);
    if (ret != 0) {
        RaiseLog(LOG_LEVEL_ERR, "Start config wifi fail.\n");
        return NULL;
    }

    return NULL;
}

void NetCfgRegister(NetCfgEventCallback nEventCallback)
{
    RaiseLog(LOG_LEVEL_INFO, "NetCfgRegister enter.\n");
    osThreadAttr_t attr;

    g_netCfgEventCallback = nEventCallback;

    attr.name = "NetCfgTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NETCFG_TASK_STACK_SIZE;
    attr.priority = NETCFG_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)NetCfgTask, NULL, &attr) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Falied to create NetCfgTask!\n");
    }
}
