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

import router from '@ohos.router'

@Entry
@Component
struct Index {
  build() {
    // 使用 Stack 容器，组件会从后往前叠放
    Stack({ alignContent: Alignment.Bottom }) {
      Image($r('app.media.launch'))
        .width('100%')
        .height('100%')
        .objectFit(ImageFit.Cover) // 核心：确保图片覆盖全屏不留白

      Row() {
        Button($r('app.string.btn_login'))
          .commonBtn()
          .backgroundColor(Color.Green)
          .fontColor(Color.White)
          .onClick(() => {
            router.replaceUrl({
              url: 'pages/Login'
            })
          })

        Button($r('app.string.btn_register'))
          .commonBtn()
          .backgroundColor(Color.White)
          .fontColor(Color.Grey)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/Register'
            })
          })

      }.width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .padding({ left: 20, right: 20 })
      .margin({ bottom: 60 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#020204')
  }
}

@Extend(Button)
function commonBtn() {
  .type(ButtonType.Normal)
  .fontSize(18)
  .width(120)
  .borderRadius(10)
}