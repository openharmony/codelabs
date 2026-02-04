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
import UserPreferences from '../utils/UserPreferences';
import common from '@ohos.app.ability.common';

@Entry
@Component
struct RegisterPage {
  @State account: string = '';
  @State password: string = '';
  @State confirmPassword: string = '';

  async aboutToAppear() {
    try {
      let context = getContext(this) as common.UIAbilityContext;
      await UserPreferences.init(context);
    } catch (e) {
    }
  }

  async register() {
    if (this.account === '' || this.password === '') {
      promptAction.showToast({ message: '账号或密码不能为空' });
      return;
    }
    if (this.password !== this.confirmPassword) {
      promptAction.showToast({ message: '两次输入的密码不一致' });
      return;
    }

    await UserPreferences.saveAccount(this.account, this.password);
    promptAction.showToast({ message: '注册成功,请登录' });
    router.back();
  }

  build() {
    Column() {
      Text('注册新账号')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 50, bottom: 30 })

      TextInput({ placeholder: '请输入用户名' })
        .height(50)
        .width('90%')
        .backgroundColor('#F5F5F5')
        .borderRadius(25)
        .padding({ left: 20 })
        .margin({ bottom: 15 })
        .onChange((val) => this.account = val)

      TextInput({ placeholder: '请输入密码' })
        .type(InputType.Password)
        .height(50)
        .width('90%')
        .backgroundColor('#F5F5F5')
        .borderRadius(25)
        .padding({ left: 20 })
        .margin({ bottom: 15 })
        .onChange((val) => this.password = val)

      TextInput({ placeholder: '请确认密码' })
        .type(InputType.Password)
        .height(50)
        .width('90%')
        .backgroundColor('#F5F5F5')
        .borderRadius(25)
        .padding({ left: 20 })
        .margin({ bottom: 30 })
        .onChange((val) => this.confirmPassword = val)

      Button('立即注册')
        .width('90%')
        .height(50)
        .backgroundColor('#4A90E2')
        .borderRadius(25)
        .fontSize(18)
        .onClick(() => this.register())

      Text('已有账号?去登录')
        .fontColor('#4A90E2')
        .margin({ top: 20 })
        .onClick(() => router.back())
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }
}
