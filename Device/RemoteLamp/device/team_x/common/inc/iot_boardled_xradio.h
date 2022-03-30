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

#ifndef __IOT_BOARDLED_H__
#define __IOT_BOARDLED_H__

typedef enum {
    LED_FLASH_FAST_PLUGS,
    LED_FLASH_FAST,
    LED_FLASH_NORMAL,
    LED_FLASH_SLOW,
    LED_FLASH_SLOW_PLUGS,

    LED_FLASH_MAX
}LED_FLASH_MODE;

/**
 * @brief use this function to change the flash cycle;
 * @param flashHz indicates the flash frequency
 * @return Returns 0 success while others failed
 */

int LedFlashFrequencySet(LED_FLASH_MODE mode);

/**
 * @brief use this function to kill the flash task
 * @return Returns 0 success while others failed
 */
int LedFlashTaskDeinit(void);

/**
 * @brief Initialize the LED task
 */
void LedFlashTaskStart(void);

#endif /* __IOT_BOARDLED_H__ */
