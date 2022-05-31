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

#include "hotspot.h"
#include "common.h"
#include "wifi_hotspot.h"
#include "wifi_logger.h"

using namespace OHOS;
using namespace OHOS::Wifi;

int StartHotspot()
{
    int ret;
    std::unique_ptr<WifiHotspot> wifiHotspotPtr = WifiHotspot::GetInstance(WIFI_HOTSPOT_ABILITY_ID);
    // Wifi热点配置
    const std::string ssid = "SmartDoorAccess";
    const std::string key = "12345678";
    HotspotConfig config;
    config.SetSsid(ssid);
    config.SetPreSharedKey(key);
    config.SetMaxConn(1);
    config.SetBand(BandType::BAND_2GHZ);
    config.SetSecurityType(KeyMgmt::WPA_PSK);
    SAMPLE_INFO("config Ssid=%s, key=%s \r\n", config.GetSsid().c_str(), config.GetPreSharedKey().c_str());
    ret = wifiHotspotPtr->SetHotspotConfig(config);
    if (ret != WIFI_OPT_SUCCESS) {
        SAMPLE_INFO("Set hotspot config error\r\n");
    }
    // 启动wifi热点
    ret = wifiHotspotPtr->EnableHotspot();
    if (ret != WIFI_OPT_SUCCESS) {
        SAMPLE_INFO("Enable hotspot error\r\n");
    }
    return ret;
}