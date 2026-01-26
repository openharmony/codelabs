/* 
  * Copyright (c) 2026 Huawei Device Co., Ltd. 
  * Licensed under the Apache License, Version 2.0 (the "License"); 
  * you may not use this file except in compliance with the License. 
  * You may obtain a copy of the License at 
  * 
  *     `http://www.apache.org/licenses/LICENSE-2.0`  
  * 
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
  * See the License for the specific language governing permissions and 
  * limitations under the License. 
*/

import { router, promptAction } from '@kit.ArkUI';
import { RdbUtil } from '../utils/RdbUtil';
import { CryptoUtil } from '../utils/CryptoUtil';
import { User } from '../model/DataModels';

@Entry
@Component
// 登录注册页面
struct LoginPage {
  @State isRegister: boolean = false;
  @State username: string = '';
  @State password: string = '';
  @State phone: string = '';

  build() {
    Column() {
      Text(this.isRegister ? '注册账号' : '登录')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 50, bottom: 30 })

      Column({ space: 15 }) {
        TextInput({ placeholder: '请输入用户名', text: this.username })
          .width('80%')
          .height(50)
          .onChange((value: string) => {
            this.username = value;
          })

        TextInput({ placeholder: '请输入密码', text: this.password })
          .type(InputType.Password)
          .width('80%')
          .height(50)
          .onChange((value: string) => {
            this.password = value;
          })

    if (this.isRegister) {
          TextInput({ placeholder: '请输入手机号', text: this.phone })
            .type(InputType.PhoneNumber)
            .width('80%')
            .height(50)
            .onChange((value: string) => {
              this.phone = value;
            })
        }
      }
      .width('100%')
      .alignItems(HorizontalAlign.Center)

      Button(this.isRegister ? '立即注册' : '登录')
        .width('80%')
        .height(50)
        .margin({ top: 30 })
        .onClick(() => {
          this.handleAction();
        })

      Text(this.isRegister ? '已有账号？去登录' : '没有账号？去注册')
        .fontSize(14)
        .fontColor('#1698CE')
        .margin({ top: 20 })
        .onClick(() => {
          this.isRegister = !this.isRegister;
          this.username = '';
          this.password = '';
          this.phone = '';
        })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }

  // 处理登录或注册按钮点击
  async handleAction() {
    if (this.username === '' || this.password === '') {
      promptAction.showToast({ message: '用户名和密码不能为空' });
      return;
    }

    if (this.isRegister) {
      const existUser = await RdbUtil.getUser(this.username);
      if (existUser) {
        promptAction.showToast({ message: '用户名已存在' });
        return;
      }

      const encryptedPwd = await CryptoUtil.encryptAES(this.password);

      const newUser = new User(0, this.username, encryptedPwd, this.phone, new Date().getTime());
      const id = await RdbUtil.addUser(newUser);
      
      if (id >= 0) {
        promptAction.showToast({ message: '注册成功，请登录' });
        this.isRegister = false;
      } else {
        promptAction.showToast({ message: '注册失败' });
      }
    } else {
      const user = await RdbUtil.getUser(this.username);
      if (!user) {
        promptAction.showToast({ message: '用户不存在' });
        return;
      }

      const inputEncrypted = await CryptoUtil.encryptAES(this.password);

      if (inputEncrypted === user.password || this.password === user.password) {
        promptAction.showToast({ message: '登录成功' });
        
        AppStorage.setOrCreate('currentUser', user);
        await RdbUtil.setSessionUserId(user.id);
        router.back();
      } else {
        promptAction.showToast({ message: '密码错误' });
      }
    }
  }
}