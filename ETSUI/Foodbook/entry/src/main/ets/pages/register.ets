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
//register
import { Prompt, router } from '@kit.ArkUI';
import prompt from '@system.prompt'; //用于alterAialog警告弹窗

// 定义存储键名（与login页面一致）
const USER_LIST_KEY = 'userList';

export class user_account {
  username: string
  password: string

  constructor(pname: string, pwd: string) {
    this.username = pname
    this.password = pwd
  }
}

@Entry
@Component
struct register {
  @State username: string = "";
  @State password: string = "";
  @State conf_psd: string = "";

  aboutToAppear() {
    // 初始化时从AppStorage加载用户列表，如果没有则初始化为空数组
    if (!AppStorage.has(USER_LIST_KEY)) {
      AppStorage.setOrCreate(USER_LIST_KEY, []);
    }
  }

  build() {
    Column({ space: 1 }) {
      // 顶部应用图标
      Text()
        .width(110)
        .height(110)
        .borderRadius(30)
        .margin({ top: 80 })
        .backgroundImage($r('app.media.app'))
        .backgroundImageSize(ImageSize.FILL)
      Text('注册界面').fontSize(30).fontWeight(FontWeight.Bold)
      Text('注册账号以使用更多服务')

      // 三个输入框及注册按钮///////////////////////
      ///////////////////////////////////////////////////////////////////////
      Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        TextInput({ placeholder: '输入用户名' }).width(300).type(InputType.USER_NAME)
          .onChange((value: string) => {
            this.username = value
          }).maxLength(15)

        TextInput({ placeholder: '输入密码' }).width(300).type(InputType.Password)
          .onChange((value: string) => {
            this.password = value
          }).maxLength(15)

        TextInput({ placeholder: '确认密码' }).width(300).type(InputType.Password)
          .onChange((value: string) => {
            this.conf_psd = value
          }).maxLength(15)

        Button('注册', { type: ButtonType.Capsule, stateEffect: true }).fontSize(20).width(300).height(50)
          .onClick(() => {
            // 验证输入
            if (this.username === "") {
              prompt.showDialog({
                title: '提示',
                message: '用户名不能为空',
                buttons: [
                  {
                    text: '确定',
                    color: '#007DFF'
                  }
                ],
                success: () => {
                  console.log('用户点击了确定');
                }
              });
              return;
            }

            if (this.password === "") {
              prompt.showDialog({
                title: '提示',
                message: '密码不能为空',
                buttons: [
                  {
                    text: '确定',
                    color: '#007DFF'
                  }
                ],
                success: () => {
                  console.log('用户点击了确定');
                }
              });
              return;
            }

            if (this.conf_psd === "") {
              prompt.showDialog({
                title: '提示',
                message: '未确认密码',
                buttons: [
                  {
                    text: '确定',
                    color: '#007DFF'
                  }
                ],
                success: () => {
                  console.log('用户点击了确定');
                }
              });
              return;
            }

            if (this.conf_psd !== this.password) {
              prompt.showDialog({
                title: '提示',
                message: '密码不一致',
                buttons: [
                  {
                    text: '确定',
                    color: '#007DFF'
                  }
                ],
                success: () => {
                  console.log('用户点击了确定');
                }
              });
              return;
            }

            // 获取现有用户列表
            const userList: user_account[] = AppStorage.get(USER_LIST_KEY) || [];

            // 检查用户名是否已存在
            if (userList.some(user => user.username === this.username)) {
              prompt.showDialog({
                title: '提示',
                message: '用户名已存在',
                buttons: [
                  {
                    text: '确定',
                    color: '#007DFF'
                  }
                ],
                success: () => {
                  console.log('用户点击了确定');
                }
              });
              return;
            }

            // 创建新用户
            const newUser = new user_account(this.username, this.password);

            // 添加到用户列表并保存
            userList.push(newUser);
            AppStorage.set(USER_LIST_KEY, userList);

            // 注册成功提示
            Prompt.showToast({
              message: "注册成功",
              duration: 1000
            });

            // 跳转到登录页面，并传递新用户信息
            router.pushUrl({
              url: 'pages/login',
              params: newUser,
            });

          })
        Text('前往登录').fontColor(Color.Blue)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/login'
            });
          })
      }
      .height('40%')
      .width('95%')
      .backgroundColor(Color.White)
      .borderRadius(30)
      .margin({ top: 40 })

      // 添加跳转到登录页面的按钮/////////////////
      //////////////////////////////////////////////////////////////////////


      Text().width('100%').height('7%')
      Text('其他登录方式')
      Row() {
        Image($r('app.media.qqphoto')).width(55).height(55).borderRadius(20)
        Text().height(50).width(20)
        Image($r('app.media.wxphoto')).width(60).height(55).borderRadius(20)
        Text().height(50).width(20)
        Image($r('app.media.douyinphoto')).width(50).height(50).borderRadius(20)
      }
    }
    .height('100%')
    .width('100%')
    .alignItems(HorizontalAlign.Center)
    .backgroundImageSize(ImageSize.FILL)
  }
}