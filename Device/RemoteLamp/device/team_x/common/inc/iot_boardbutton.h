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

typedef void (*ButtonPressedCallback)(char *arg);


/**
 * @brief Initialize a button
 *
 */
int Board_IsButtonPressedF2(void);
int Board_InitButtonF1(ButtonPressedCallback, char *arg);
int Board_InitButtonF2(ButtonPressedCallback, char *arg);


#endif /* IOT_BOARDBUTTON_H_ */
