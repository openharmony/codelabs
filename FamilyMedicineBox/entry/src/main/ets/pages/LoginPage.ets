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
import promptAction from '@ohos.promptAction';
import { MockData } from '../model/MockData';

@Entry
@Component
struct LoginPage {
  @State username: string = '';
  @State password: string = '';

  build() {
    Column() {
      Image($r('app.media.startIcon'))
        .width(80).height(80)
        .margin({ top: 80, bottom: 20 })

      Text('家庭小药箱')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333')
        .margin({ bottom: 50 })

      // 提示信息 (满足需求4)
      Text('测试账号: admin   密码: ' + MockData.currentUser.password)
        .fontSize(12)
        .fontColor('#999')
        .backgroundColor('#F0F0F0')
        .padding(8)
        .borderRadius(4)
        .margin({ bottom: 20 })

      TextInput({ placeholder: '请输入账号' })
        .width('80%')
        .height(50)
        .backgroundColor('#F5F6FA')
        .borderRadius(25)
        .margin({ bottom: 16 })
        .onChange((val) => this.username = val)

      TextInput({ placeholder: '请输入密码' })
        .type(InputType.Password)
        .width('80%')
        .height(50)
        .backgroundColor('#F5F6FA')
        .borderRadius(25)
        .margin({ bottom: 40 })
        .onChange((val) => this.password = val)

      Button('登录')
        .width('80%')
        .height(50)
        .backgroundColor('#4A90E2')
        .borderRadius(25)
        .onClick(() => {
          if (this.username === MockData.currentUser.username &&
            this.password === MockData.currentUser.password) {
            try {
              promptAction.showToast({ message: '登录成功' });
            } catch (e) {
            }
            router.replaceUrl({ url: 'pages/Index' });
          } else {
            try {
              promptAction.showToast({ message: '账号或密码错误' });
            } catch (e) {
            }
          }
        })

      Text('注册账号')
        .fontSize(14)
        .fontColor('#4A90E2')
        .margin({ top: 20 })
        .onClick(() => {
          // 跳转注册页逻辑
          router.pushUrl({ url: 'pages/RegisterPage' });
        })
    }
    .width('100%').height('100%')
    .backgroundColor(Color.White)
  }
}