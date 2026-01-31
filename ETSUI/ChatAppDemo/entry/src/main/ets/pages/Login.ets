/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { router, Prompt, promptAction } from '@kit.ArkUI';
import { dbManager } from '../utils/DatabaseManager';
import { UserModel } from '../viewmodel/UserModel';

@Entry
@Component
struct Login {
  @State phoneNumber: string = '';
  @State password: string = '';

  // 使用 @StorageLink 绑定全局变量。
  // 一旦此变量被赋值，全应用所有关联页面都会感知到
  @StorageLink('currentUser') currentUser: UserModel | null = null;

  build() {
    Column({ space: 20 }) {
      Text($r('app.string.title_welcome_login'))
        .fontSize(30)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 50, bottom: 30 })

      TextInput({ placeholder: $r('app.string.hint_enter_phone'), text: this.phoneNumber })
        .type(InputType.PhoneNumber)
        .onChange((v) => this.phoneNumber = v)
        .width('90%')

      TextInput({ placeholder: $r('app.string.hint_enter_pwd'), text: this.password })
        .type(InputType.Password)
        .onChange((v) => this.password = v)
        .width('90%')

      Button($r('app.string.btn_login'))
        .width('90%')
        .height(50)
        .onClick(async () => {
          await this.handleLogin();
        })

      Text($r('app.string.btn_go_register'))
        .fontColor(Color.Blue)
        .onClick(() => {
          router.pushUrl({ url: 'pages/Register' });
        })
    }
    .width('100%')
    .height('100%')
  }

  async handleLogin() {
    if (this.phoneNumber === '' || this.password === '') {
      //Prompt.showToast({ message: $r('app.string.toast_fill_complete').toString() });
      promptAction.showToast({
        message: $r('app.string.toast_fill_complete'),
        duration: 2000
      });
      return;
    }

    // 从数据库查询用户
    const user = await dbManager.user?.queryUserByPhone(this.phoneNumber);

    if (user && user.password === this.password) {
      // 登录成功：更新 AppStorage
      // 这行代码执行后，currentUser 就不再是 null，PersistentStorage 会同步将其存入磁盘
      this.currentUser = user;

      // Prompt.showToast({ message: $r('app.string.toast_login_success').toString() });
      promptAction.showToast({
        message: $r('app.string.toast_login_success'),
        duration: 2000
      });
      // 跳转到主页
      setTimeout(() => {
        router.replaceUrl({ url: 'pages/Main' });
      }, 1000);

    } else {
      //Prompt.showToast({ message: $r('app.string.toast_login_fail').toString() });
      promptAction.showToast({
        message: $r('app.string.toast_login_fail'),
        duration: 2000
      });
    }
  }
}