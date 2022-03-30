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

#ifndef __IOT_NFC_H__
#define __IOT_NFC_H__


/**
 * @brief Initialize the board NFC
 * @return Returns 0 success while others failed
 */
int BOARD_InitNfc(void);

/**
 * @brief Defines the nfc information
 */
typedef struct {
    const char *deviceID;
    const char *devicePWD;
    const char *wifiSSID;
    const char *wifiPWD;
}NfcInfo;

/**
 * @brief Get the nfc info
 * @param info Indicates the buffer to storage the data get from NFC
 * @return Returns 0 success while others failed
 */
int BOARD_GetNfcInfo(NfcInfo *info);


#endif /* __IOT_NFC_H__ */
