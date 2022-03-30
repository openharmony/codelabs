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

#ifndef    __IOT_NETCFG_H__
#define    __IOT_NETCFG_H__

/**
 * @brief start net config.
 *
 * @param ap_name: the ap name
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
int BOARD_NetCfgStartConfig(const char *appName, char *ssid, int sLen, char *pwd, int pLen);

#endif    /* __IOT_NETCFG_H__ */
