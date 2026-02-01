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
struct ChangePasswordPage {
  @State oldPass: string = '';
  @State newPass: string = '';
  @State confirmPass: string = '';

  build() {
    Column() {
      // 标题
      Row() {
        Button('<').onClick(() => router.back()).backgroundColor(Color.Transparent).fontColor(Color.Black).fontSize(24)
        Text('修改密码').fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 10 })
      }
      .width('100%').padding(16).margin({ bottom: 20 })

      // 表单
      Column() {
        Text('旧密码').fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
        TextInput({ placeholder: '请输入旧密码' })
          .type(InputType.Password)
          .height(50)
          .backgroundColor('#F5F6FA')
          .borderRadius(8)
          .margin({ bottom: 20 })
          .onChange((val) => this.oldPass = val)

        Text('新密码').fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
        TextInput({ placeholder: '请输入新密码' })
          .type(InputType.Password)
          .height(50)
          .backgroundColor('#F5F6FA')
          .borderRadius(8)
          .margin({ bottom: 20 })
          .onChange((val) => this.newPass = val)

        Text('确认新密码').fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
        TextInput({ placeholder: '请再次输入新密码' })
          .type(InputType.Password)
          .height(50)
          .backgroundColor('#F5F6FA')
          .borderRadius(8)
          .margin({ bottom: 40 })
          .onChange((val) => this.confirmPass = val)

        Button('确认修改')
          .width('100%').height(50)
          .backgroundColor('#4A90E2')
          .onClick(() => {
            // 验证旧密码
            if (this.oldPass !== MockData.currentUser.password) {
              try {
                promptAction.showToast({ message: '旧密码错误' });
              } catch (e) {
              }
              return;
            }
            // 验证新密码
            if (this.newPass.length < 6) {
              try {
                promptAction.showToast({ message: '新密码不能少于6位' });
              } catch (e) {
              }
              return;
            }
            if (this.newPass !== this.confirmPass) {
              try {
                promptAction.showToast({ message: '两次密码输入不一致' });
              } catch (e) {
              }
              return;
            }

            // 修改密码
            MockData.currentUser.password = this.newPass;
            try {
              promptAction.showToast({ message: '密码修改成功' });
            } catch (e) {
            }
            router.back();
          })
      }
      .width('90%')
    }
    .height('100%')
    .backgroundColor(Color.White)
  }
}