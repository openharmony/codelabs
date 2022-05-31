/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

#include <stdint.h>
#include "oled_ssd1306_conf.h"

typedef struct {
    const int8_t fontWidth;
    int8_t fontHeight;
    const int16_t *data;
} FontDef;

#ifdef IOT_INCLUDE_FONT_6X8
extern FontDef g_font68;
#endif
#ifdef IOT_INCLUDE_FONT_7X10
extern FontDef g_font710;
#endif
#ifdef IOT_INCLUDE_FONT_11X18
extern FontDef g_font1118;
#endif
#ifdef IOT_INCLUDE_FONT_16X26
extern FontDef g_font1626;
#endif
#endif
