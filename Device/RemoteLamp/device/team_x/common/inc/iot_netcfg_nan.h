/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef  __IOT_NETCFG_NAN_H__
#define  __IOT_NETCFG_NAN_H__

#define NETCFG_TASK_STACK_SIZE (1024*4)
#define NETCFG_TASK_PRIO 30
#define NETCFG_LED_INTERVAL_TIME_US 300000
#define NETCFG_BIZ_SLEEP_TIME_US    1000000
#define NETCFG_TIME_COUNT 5
#define DEVICE_INFO_NUM 2
#define POWER_NUM (-52)
#define MAX_DATA_LEN 4096
#define FAST_CONNECT_RETRY_NUM 3

#define CHANNEL_80211B_ONLY 14
#define FREQ_OF_CHANNEL_1 2412
#define FREQ_OF_CHANNEL_80211B_ONLY 2484
#define WIFI_MIN_CHANNEL 1
#define WIFI_FREQ_INTERVAL 5
#define TEST_CONNECT_RETRY_COUNT 5

typedef enum {
    NET_EVENT_NULL,
    NET_EVENT_CONFIG,
    NET_EVENT_CONFIG_FAIL, // Connected wifi failed
    NET_EVENT_CONFIG_SUCC, // Connected wifi success
    NET_EVENT_CONNECTTED,  // Wifi connected
    NET_EVENT_DISTCONNECT, // Wifi disconnected
    NET_EVENT_RECV_DATA, // Recv message from FA
    NET_EVENT_SEND_DATA, // Send the message to FA

    NET_EVENT_TYPE_NBR
}NET_EVENT_TYPE;

/**
 * @brief: the network config service callback
 *
 * @param event reference {@link NET_EVENT_TYPE}
 * @param data The data of the event: NET_EVENT_RECV_DATA or NET_EVENT_SEND_DATA
 * @since 1.0
 * @version 1.0
 */
typedef int (*NetCfgEventCallback)(NET_EVENT_TYPE event, void *data);

/**
 * @brief Register the network config task
 *
 * @param nEventCallback The callback of netcfg module
 * @since 1.0
 * @version 1.0
 */
void NetCfgRegister(NetCfgEventCallback nEventCallback);

/**
 * @brief start net config.
 *
 * @param ssid: the buff to store the wifi ssid
 * @param sLen: the ssid buff size
 * @param pwd: the buff to store the wifi password
 * @param pLen: the pwd buff size
 *
 * @since 1.0
 * @version 1.0
 *
 * @return 0 success, -1 failed
 */
int BOARD_NAN_NetCfgStartConfig(const char *apName, char *ssid, int sLen, char *pwd, int pLen);
#endif  // __IOT_NETCFG_NAN_H__
