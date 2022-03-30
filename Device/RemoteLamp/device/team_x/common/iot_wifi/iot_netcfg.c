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
#include "errno.h"
#include "cmsis_os2.h"
#include "sys/time.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"

#include "cJSON.h"
#include "ohos_types.h"
#include "iot_softap.h"
#include "iot_netcfg.h"
#include "iot_demo_def.h"

#define SSID_MAX_LEN    32
#define PWD_MAX_LEN     32
#define NET_PORT        8686
#define RETRY_TIMES     5
#define CONNECT_TIMEOUT 20
#define CMD_KEY    "cmd"
#define PAR_KEY     "param"
#define SID_KEY     "wifiName"
#define PWD_KEY     "wifiPassword"
#define CMD_CONFIG  0x20
#define REQUEST_STR "{\"cmd\":32,\"code\":200,\"msg\":\"\"}"

static int ParseWifiInfo(const char *psr, char *ssid, int sl, char *pwd, int pl)
{
    cJSON *json;
    cJSON *sub;
    cJSON *obj;
    int cmd;
    char *ps = NULL;
    char *pw = NULL;

    json = cJSON_Parse(psr);
    RaiseLog(LOG_LEVEL_ERR, "parse_json: %s\n", psr);
    if (json == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "parse_json: %s\n", psr);
        return -1;
    }

    sub = cJSON_GetObjectItem(json, CMD_KEY);
    cmd = cJSON_GetNumberValue(sub);
    if (cmd != CMD_CONFIG) {
        RaiseLog(LOG_LEVEL_ERR, "CMD TYPE FAILED! cmd=%d \n", cmd);
        cJSON_Delete(json);
        return -1;
    }

    sub = cJSON_GetObjectItem(json, PAR_KEY);
    if (sub == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "get param obj failed! \n");
        cJSON_Delete(json);
        return -1;
    }

    obj = cJSON_GetObjectItem(sub, SID_KEY);
    ps = cJSON_GetStringValue(obj);
    if (ps == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "get ssid failed! \n");
        cJSON_Delete(json);
        return -1;
    }

    if (strncpy_s(ssid, sl, ps, strlen(ps)) < 0) {
        RaiseLog(LOG_LEVEL_ERR, "strncpy_s failed! \n");
        cJSON_Delete(json);
        return -1;
    }

    obj = cJSON_GetObjectItem(sub, PWD_KEY);
    pw = cJSON_GetStringValue(obj);
    if (pw != NULL) {
        RaiseLog(LOG_LEVEL_INFO, "pw=%s\n", pw);
        if (strncpy_s(pwd, pl, pw, strlen(pw)) < 0) {
            RaiseLog(LOG_LEVEL_ERR, "strncpy_s failed! \n");
            cJSON_Delete(json);
            return -1;
        }
    }

    cJSON_Delete(json);
    return 0;
}

static int NetCfgGetSocketValue(int sockfd, char *ssid, int sLen, char *pwd, int pLen)
{
    int result = -1;

    if (sockfd < 0) {
        RaiseLog(LOG_LEVEL_ERR, "sockfd invalid!\n");
        return -1;
    }

    if (ssid == NULL || sLen < SSID_MAX_LEN || pwd == NULL || pLen < PWD_MAX_LEN) {
        RaiseLog(LOG_LEVEL_ERR, "NULL POINT!\n");
        return -1;
    }

    while (1) {
        int readbytes;
        struct sockaddr_in addr;
        char recvbuf[BUFF_SIZE] = {0};
        socklen_t len = sizeof(addr);

        readbytes = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&addr, &len);
        if (readbytes <= 0) {
            RaiseLog(LOG_LEVEL_ERR, "socket disconnect! \n");
            break;
        }

        if (ParseWifiInfo((const char *)recvbuf, ssid, sLen, pwd, pLen) == 0) {
            sendto(sockfd, REQUEST_STR, strlen(REQUEST_STR), 0, (struct sockaddr *)&addr, &len);
            result = 0;
            break;
        }
    }

    return result;
}

static int NetCfgGetWifiInfo(char *ssid, int sLen, char *pwd, int pLen)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        RaiseLog(LOG_LEVEL_ERR, "socket error! \n");
        return -1;
    }

    memset_s(&servaddr, sizeof(servaddr), 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(NET_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    RaiseLog(LOG_LEVEL_INFO, "listen port %d\n", NET_PORT);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        RaiseLog(LOG_LEVEL_ERR, "bind socket! \n");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 1) < 0) {
        RaiseLog(LOG_LEVEL_ERR,"listen failed! errno = %d \n", errno);
        close(sockfd);
        return -1;
    }

    while (1) {
        int newfd, retval;
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        newfd = accept(sockfd, (struct sockaddr *)&client_addr, &length);
        RaiseLog(LOG_LEVEL_INFO, "new socket connect!\n");
        retval = NetCfgGetSocketValue(newfd, ssid, sLen, pwd, pLen);
        close(newfd);

        if (retval == 0) {
            RaiseLog(LOG_LEVEL_INFO, "config network success! \n");
            break;
        }
    }

    close(sockfd);

    return 0;
}

int BOARD_NetCfgStartConfig(const char *appName, char *ssid, int sLen, char *pwd, int pLen)
{
    int retval = 0;

    if (BOARD_SoftApStart(appName) < 0) {
        RaiseLog(LOG_LEVEL_ERR, "start softap failed! \n");
        return -1;
    }

    if (NetCfgGetWifiInfo(ssid, sLen, pwd, pLen) < 0) {
        RaiseLog(LOG_LEVEL_ERR, "start softap failed! \n");
        retval = -1;
    }

    BOARD_SoftApStop();

    return retval;
}