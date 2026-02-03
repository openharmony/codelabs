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

import router from '@ohos.router';

@Entry
@Component
struct ProfilePage {
  build() {
    Column() {
      // 标题
      Row() {
        Button('<')
          .onClick(() => router.back())
          .backgroundColor(Color.Transparent)
          .fontColor(Color.Black)
          .fontSize(24)
        Text('发现').fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 10 })
      }
      .width('100%').padding(16)

      Text('我的用药提醒卡')
        .fontSize(22)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 20 })

      // 卡片区域 (用蓝色背景模拟截图中的卡片)
      Column() {
        Row() {
          Text('家庭用药小贴士')
            .fontColor('#4A90E2')
            .fontWeight(FontWeight.Bold)
        }
        .width('100%')
        .padding({ bottom: 10 })
        .border({ width: { bottom: 1 }, color: '#4A90E2' })

        Text('温馨提示：\n\n1. 请定期清理过期药品\n\n2. 服药前请看清用法用量\n\n3. 药品请放在儿童拿不到的地方')
          .fontSize(14)
          .fontColor('#333')
          .margin({ top: 20 })
      }
      .width('90%')
      .height(300)
      .backgroundColor('#EBF5FF')
      .borderRadius(12)
      .padding(20)
      .margin({ bottom: 50 })

      // 退出按钮
      Button('退出登录')
        .width('80%')
        .height(50)
        .backgroundColor('#FF5252') // 红色
        .onClick(() => {
          router.replaceUrl({ url: 'pages/LoginPage' }); // 假设有登录页
        })
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }
}