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

import router from '@ohos.router';

@Entry
@Component
struct Welcome {
  @State nickname: string = router.getParams().nickname.toString()
  @State message: string = '登录成功'

  build() {
    Row() {
      Column() {
        // 用户昵称文本框
        Text(this.nickname)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        // 登录成功文本框
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
    }
    .height('100%')
    .linearGradient({
      angle: 180,  // 线性渐变的角度
      direction: GradientDirection.Bottom, // 线性渐变的方向
      colors: [['#FCD1D1', 0.2], ['#D3E0DC', 0.5], ['#CCCCFF', 1.0]] // 线性渐变的颜色描述
    })
  }
}