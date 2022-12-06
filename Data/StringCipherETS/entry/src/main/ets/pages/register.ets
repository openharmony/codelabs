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

// @ts-nocheck
import prompt from '@ohos.prompt';
import router from '@ohos.router';
import cipher from '@system.cipher';
import { User } from '../model/User';
import { InputItem } from '../pages/inputItem';
import { UserTableApi } from '../model/UserTableApi';

@Entry
@Component
struct Register {
  @State username: string = ''
  @State nickname: string = ''
  @State password: string = ''
  @State confirmPassword: string = ''
  @State @Watch('getValue') inputValue: string = ''
  @State inputType: number = 0
  @State userList: User[] = []
  @State encryptPassword: string = ''
  private userTableApi = new UserTableApi()

  build() {
    Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
      // 用户默认头像
      Image($r('app.media.user'))
        .objectFit(ImageFit.Contain)
        .height('20%')
        .margin({ bottom: '4%' })
      // 用户名输入框
      InputItem({
        placeholderName: '请输入用户名',
        imageRes: $r('app.media.username'),
        inputValue: $inputValue,
        inputType: 0,
        typeFlag: false
      })
      // 昵称输入框
      InputItem({
        placeholderName: '请输入昵称',
        imageRes: $r('app.media.nickname'),
        inputValue: $inputValue,
        inputType: 1,
        typeFlag: false
      })
      // 密码输入框
      InputItem({
        placeholderName: '请输入密码',
        imageRes: $r('app.media.password'),
        inputValue: $inputValue,
        inputType: 2,
        typeFlag: true
      })
      // 确认密码输入框
      InputItem({
        placeholderName: '请输入确认密码',
        imageRes: $r('app.media.password'),
        inputValue: $inputValue,
        inputType: 3,
        typeFlag: true
      })
      // 注册按钮
      Button('注册', { type: ButtonType.Capsule, stateEffect: true })
        .width('50%')
        .height('6%')
        .margin({ top: '2%' })
        .fontSize(30)
        .backgroundColor('#1890FF')
        .onClick(() => {
          // 用户名/昵称/密码/确认密码 数据校验
          if (!this.checkUserData()) {
            return;
          }
          // 注册用户信息
          this.registerUser();
        });
    }
    .width('100%')
    .height('100%')
    .linearGradient({
      angle: 180, // 线性渐变的角度
      direction: GradientDirection.Bottom, // 线性渐变的方向
      colors: [['#AEE1E1', 0.0], ['#D3E0DC', 0.3], ['#FCD1D1', 1.0]] // 线性渐变的颜色描述
    })
  }

  getValue() {
    let returnType = this.inputValue.split(':')[0];
    let value = this.inputValue.split(':')[1];
    if (returnType == 0) {
      this.username = value;
    } else if (returnType == 1) {
      this.nickname = value;
    } else if (returnType == 2) {
      this.password = value;
    } else if (returnType == 3) {
      this.confirmPassword = value;
    }
  }

  // 用户名/昵称/密码/确认密码 数据校验
  checkUserData() {
    // 判断用户名/昵称/密码/确认密码是否为空
    if (this.username == '' || this.nickname == '' || this.password == '' || this.confirmPassword == '') {
      prompt.showToast({
        message: '用户名/昵称/密码/确认密码不能为空',
        duration: 3000,
        bottom: '40%'
      });
      return false;
    }
    // 用户名/昵称为1-14位中文、字母、数字、下划线或者这几项的组合
    let namePattern = /^[\u4E00-\u9FA5A-Za-z0-9_]{1,14}$/;
    if (!namePattern.exec(this.username) || !namePattern.exec(this.nickname)) {
      prompt.showToast({
        message: '用户名/昵称格式不正确，请重新输入！',
        duration: 3000,
        bottom: '40%'
      });
      return false;
    }
    // 密码和确认密码是否输入一致
    if (this.password != this.confirmPassword) {
      prompt.showToast({
        message: '密码和确认密码输入不一致，请重新输入！',
        duration: 3000,
        bottom: '40%'
      });
      return false;
    }
    // 密码为4-14个数字、字母或者!@$#%^&*或者他们的组合。
    let passwordPattern = /^[A-Za-z0-9!@$#%^&*]{4,14}$/;
    if (!passwordPattern.exec(this.password)) {
      prompt.showToast({
        message: '密码格式不正确，请重新输入！',
        duration: 3000,
        bottom: '40%'
      });
      return false;
    }
    return true;
  }

  // 注册用户信息
  async registerUser() {
    // 注册之前先查询该用户名是否已经注册
    await this.userTableApi.queryUserByUsername(this.username).then((ret) => {
      this.userList = ret;
    });
    if (this.userList.length != 0) {
      prompt.showToast({
        message: '用户名已经存在，请重新输入！',
        duration: 3000,
        bottom: '40%'
      });
      return;
    }
    // 将密码用字符串加密API，加密之后，存储加密的密码
    this.aesEncrypt();
  }

  // aes加密
  aesEncrypt() {
    let that = this;
    cipher.aes({
      //加密
      action: 'encrypt',
      //待加密的文本内容
      text: this.password,
      //base64编码后的密钥
      key: 'NDM5Qjk2UjAzMEE0NzVCRjlFMkQwQkVGOFc1NkM1QkQ=',
      transformation: 'AES/CBC/PKCS5Padding',
      ivOffset: 0,
      ivLen: 16,
      success: function(data) {
        that.encryptPassword = data.text;
        that.insertUser();
      },
      fail: function (data, code) {
        console.info("==========aes加密方法执行失败")
        console.info(`### cipher.rsa encrypt fail ### ${code}: ${data}`);
      }
    });
  }

  // 将加密之后的用户信息存入数据库
  insertUser() {
    // 将注册的用户信息存入数据库中
    let user = new User(null, this.username, this.nickname, this.encryptPassword);
    this.userTableApi.insertUserData(user);
    prompt.showToast({
      message: '注册成功，请登录！',
      duration: 3000,
      bottom: '40%'
    });
    // 注册成功之后，跳转到登录页面
    router.push({
      uri: 'pages/index'
    });
  }
}