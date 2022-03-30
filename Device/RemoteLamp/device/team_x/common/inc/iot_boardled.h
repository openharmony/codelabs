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

/**
 * @brief Initialize the board pwd led
 * @return Returns 0 success while others failed
 */
int BOARD_InitPwmLed(void);

/**
 * @brief Set Pwm led duty cycle
 * @param dutyCycle, the duty cycle to set, not max than 40000
 * @return Returns 0 success while others failed
 */
int BOARD_SetPwdLedDutyCycle(int dutyCycle);


/**
 * @brief Initialize the LED
 * @return Returns 0 success while others failed
 */
int BOARD_InitIoLed(void);

/**
 * @brief Control the led status
 * @param status Indicates the status to set and value should be CN_BOARD_SWICT_ON/OFF
 * @return Returns 0 success while others failed
 */
int BOARD_SetIoLedStatus(int status);


/**
 * @brief use this function to change the flash cycle;
 * @param flashHz indicates the flash frequency
 * @return Returns 0 success while others failed
 */

int LedFlashFrequencySet(float flashHz);

/**
 * @brief use this function to kill the flash task
 * @return Returns 0 success while others failed
 */
int LedFlashTaskDeinit(void);

#endif /* __IOT_BOARDLED_H__ */
