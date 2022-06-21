/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common.h"
#include "audio.h"
#include "hotspot.h"
#include "tcp.h"

static void SetNetConfig()
{
    FILE *fp;
    std::string cmd = "ifconfig wlan0 192.168.1.2 netmask 255.255.255.0";
    char *sysCommand = cmd.data();
    if ((fp = popen(sysCommand, "r")) == NULL) {
        SAMPLE_INFO("set NetConfig fail\r\n");
    }
    fgets(sysCommand, sizeof(sysCommand), fp);
    SAMPLE_INFO("Set wlan0 Ip : %s", sysCommand);
    pclose(fp);
}

int main(int argc, char **argv)
{
    int ret;
    SetNetConfig();
    ret = StartHotspot();
    if (ret != 0) {
        SAMPLE_INFO("Hotspot start fail");
    }
    ret = TcpServerThread();
    if (ret != 0) {
        SAMPLE_INFO("TcpServer start fail");
    }
    return 0;
}
