/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
// 主功能页（验证通过后进入）
// pages/HomePage.ets
import router from '@ohos.router';

@Entry
@Component
export struct HomePage {
  @State welcomeMessage: string = '欢迎回来！';

  // 跳转到个人中心
  goToProfile() {
    router.pushUrl({
      url: 'pages/ProfilePage'
    });
  }

  // 退出登录
  logout() {
    router.clear();
    router.pushUrl({
      url: 'pages/Index'
    });
  }

  build() {
    Column({ space: 20 }) {
      Text(this.welcomeMessage)
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 50 })

      Text('身份认证成功！')
        .fontSize(18)
        .fontColor('#4CAF50')
        .margin({ bottom: 50 })

      // 这里可以添加应用的具体功能
      // 例如：便签、设置等
      Button('个人中心')
        .width('80%')
        .height(50)
        .backgroundColor('#007DFF')
        .fontColor(Color.White)
        .onClick(() => this.goToProfile())

      Button('退出登录')
        .width('80%')
        .height(50)
        .backgroundColor('#FF3B30')
        .fontColor(Color.White)
        .onClick(() => this.logout())
    }
    .width('100%')
    .height('100%')
  }
}