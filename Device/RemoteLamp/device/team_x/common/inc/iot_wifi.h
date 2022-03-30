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

#ifndef IOT_WIFI_H_
#define IOT_WIFI_H_

/**
 * @brief Initialize the board wifi
 * @return Returns 0 success while others failed
 */
int BOARD_InitWifi(void);

/**
 * @brief Connect to the wifi
 * @param wifiSSID Indicates the ssid of the ap
 * @param wifiPWD Indicates the pwd of the ap
 * @return Returns 0 success while others failed
 */
int BOARD_ConnectWifi(const char *wifiSSID, const char *wifiPWD);

/**
 * @brief Disconnect from the wifi AP
 * @return Returns 0 success while others failed
 */
int BOARD_DisconnectWifi(void);

#endif /* IOT_WIFI_H_ */
