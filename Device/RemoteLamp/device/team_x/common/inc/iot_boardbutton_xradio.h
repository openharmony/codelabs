/*
 * Copyright (c) 2021 zhangqf1314@163.com .
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

#ifndef IOT_BOARDBUTTON_H_
#define IOT_BOARDBUTTON_H_

typedef enum {
    BUTTON_INVALID = 0xff,
    BUTTON_RELEASE = 0,
    BUTTON_PRESS,
    BUTTON_LONG_PRESS,

    BUTTON_MAX_TYPE
} BUTTON_VALUE_TYPE;

typedef struct {
    int button_id;
    BUTTON_VALUE_TYPE value;
} ButtonEvent;

typedef void (*ButtonPressedCallback)(ButtonEvent *event);

/**
 * @brief Initialize a button
 *
 */
int Board_ButtonInit(ButtonPressedCallback callback);

int Board_IsButtonPressed(void);

#endif /* IOT_BOARDBUTTON_H_ */
