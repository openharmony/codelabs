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
import { InputItem } from '../pages/inputItem'
import { UserTableApi } from '../model/UserTableApi'

@Entry
@Component
struct Index {
  @State username: string = ''
  @State password: string = ''
  @State decryptPassword: string = ''
  @State @Watch('getValue') inputValue: string = ''
  @State inputType: number = 0
  @State userList: User[] = []
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
      // 密码输入框
      InputItem({
        placeholderName: '请输入密码',
        imageRes: $r('app.media.password'),
        inputValue: $inputValue,
        inputType: 1,
        typeFlag: true
      })
      // 登录按钮
      Button('登录', { type: ButtonType.Capsule, stateEffect: true })
        .width('50%')
        .height('6%')
        .margin({ top: '2%' })
        .fontSize(30)
        .backgroundColor('#1890FF')
        .onClick(() => {
          // 用户名和密码数据校验
          if (!this.checkUserData()) {
            return;
          }
          // 处理用户输入的信息
          this.dealUserData();
        });
      // 跳转到注册页面的文本框
      Text("注册账号")
        .fontColor('#317AFF')
        .fontSize(30)
        .margin({ top: '3%' })
        .textAlign(TextAlign.Center)
        .onClick(() => {
          router.push({
            uri: 'pages/register'
          });
        });
    }
    .width('100%')
    .height('100%')
    .radialGradient({
      center: [50, 50], // 径向渐变的中心点
      radius: '100%', // 径向渐变的半径
      colors: [['#AEE1E1', 0.0], ['#D3E0DC', 0.5], ['#FCD1D1', 1.0]] // 径向渐变的颜色描述
    })
  }

  // 通过返回值判断输入框中输入的是用户名还是密码信息
  getValue() {
    let returnType = this.inputValue.split(':')[0];
    let value = this.inputValue.split(':')[1];
    if(returnType == 0) {
      this.username = value;
    } else if (returnType == 1) {
      this.password = value;
    }
  }

  // 用户名和密码数据校验
  checkUserData() {
    // 判断账号或密码是否为空
    if (this.username == '' || this.password == '') {
      prompt.showToast({
        message: '用户名/密码输入不能为空！',
        duration: 3000,
        bottom: '40%'
      });
      return false;
    }
    // 用户名为1-14位中文、字母、数字、下划线或者这几项的组合
    let namePattern = /^[\u4E00-\u9FA5A-Za-z0-9_]{1,14}$/;
    if (!namePattern.exec(this.username)) {
      prompt.showToast({
        message: '用户名格式不正确，请重新输入！',
        duration: 3000,
        bottom: '40%'
      });
      return false;
    }
    // 密码为4-14个数字、字母或者!@$#%^&*或者他们的组合
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

  // 处理用户输入的信息
  async dealUserData() {
    await this.userTableApi.queryUserByUsername(this.username).then((ret) => {
      this.userList = ret;
    });
    if (this.userList.length === 0) {
      prompt.showToast({
        message: '用户名不存在，请先注册！',
        duration: 3000,
        bottom: '40%'
      });
      return;
    }
    // 将密码用字符串解密API解密
    this.aesDecrypt();
  }

  // aes解密
  aesDecrypt() {
    let that = this;
    cipher.aes({
      //解密：
      action: 'decrypt',
      //待解密的内容，是base64编码后的一段二进制值
      text: this.userList[0].password,
      key: 'NDM5Qjk2UjAzMEE0NzVCRjlFMkQwQkVGOFc1NkM1QkQ=',
      transformation: 'AES/CBC/PKCS5Padding',
      ivOffset: 0,
      ivLen: 16,
      success: function(data) {
        that.decryptPassword = data.text;
        that.login();
      },
      fail: function (data, code) {
        console.info("==========aes解密方法执行失败")
        console.info(`### cipher.rsa encrypt fail ### ${code}: ${data}`);
      },
    });
  }

  // 判断解密之后的密码和用户输入的密码是否一致，一致就登录成功
  login() {
    let user = this.userList[0];
    if (this.decryptPassword == this.password) {
      // 跳转到登录成功页面
      router.push({
        uri: 'pages/welcome',
        params: { 'nickname': user.nickname }
      });
    } else {
      prompt.showToast({
        message: '密码输入错误！',
        duration: 3000,
        bottom: '40%'
      });
    }
  }
}