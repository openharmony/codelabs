/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
    '#FFDAC1', // 马卡龙桃
    '#E2F0CB', // 马卡龙绿
    '#B5EAD7', // 马卡龙薄荷
    '#C7CEEA', // 马卡龙紫
    '#F8B195', // 柔和橙
    '#F67280', // 柔和红
    '#C06C84', // 浆果色
    '#6C5B7B', // 深紫
    '#355C7D', // 深蓝灰
    '#99B898', // 抹茶绿
    '#FECEAB'  // 杏色
  ];

  static getRandomColor(): string {
    const index = Math.floor(Math.random() * ColorConstants.MACARON_COLORS.length);
    return ColorConstants.MACARON_COLORS[index];
  }
}
