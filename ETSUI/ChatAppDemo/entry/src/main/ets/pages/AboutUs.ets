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
struct AboutUs {
  build() {
    Column() {
      // 顶部导航栏
      Row() {
        // 返回按钮
        Button() {
          Text($r('app.string.btn_back'))
            .fontSize(16)
            .fontColor('#007AFF')
        }
        .backgroundColor('#FFFFFF')
        .margin({ left: 16 })
        .onClick(() => {
          router.back()
        })

        // 页面标题
        Text($r('app.string.title_about_app'))
          .fontSize(18)
          .fontWeight(700)
          .flexGrow(1)
          .textAlign(TextAlign.Center)

        // 占位元素，保持导航栏平衡
        Text('')
          .width(80)
      }
      .width('100%')
      .height(56)
      .backgroundColor('#FFFFFF')
      .alignItems(VerticalAlign.Center)
      .shadow({ radius: 2, color: '#00000020', offsetY: 2 })

      // 内容区域
      Column() {
        // APP图标
        Image($r('app.media.icon'))
          .width(80)
          .height(80)
          .margin({ bottom: 16 })

        // APP名称
        Text($r('app.string.label_app_name'))
          .fontSize(20)
          .fontWeight(700)
          .margin({ bottom: 8 })

        // 版本信息
        Text($r('app.string.label_version'))
          .fontSize(14)
          .fontColor('#666666')
          .margin({ bottom: 24 })

        // 版权信息
        Text('© 2026 Open Harmony. All rights reserved.')
          .fontSize(12)
          .fontColor('#999999')
      }
      .width('100%')
      .layoutWeight(1)
      .padding(24)
      .alignItems(HorizontalAlign.Center)
      .justifyContent(FlexAlign.Center)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}