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

@Entry
@Component
struct RegisterPage {
  @State username: string = '';
  @State password: string = '';
  @State confirmPassword: string = '';
  private courseService: CourseDataService = CourseDataService.getInstance();

  async aboutToAppear() {
    this.courseService.init(getContext(this)).catch((err: Error) => {
      console.error('RegisterPage init failed:', JSON.stringify(err));
    });
  }

  build() {
    Column() {
      // Top Bar
      Row() {
        Image($r('sys.media.ohos_ic_public_arrow_left'))
          .width(24)
          .height(24)
          .onClick(() => {
            console.info('Register back clicked');
            router.back();
          })
      }
      .width('100%')
      .padding({ left: 16, top: 16 })
      .justifyContent(FlexAlign.Start)

      Scroll() {
        Column() {
          // Header
          Text('创建账号')
            .fontSize(28)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .width('100%')
            .margin({ bottom: 8 })
          
          Text('注册以开始使用课程表助手')
            .fontSize(16)
            .fontColor('#999999')
            .width('100%')
            .margin({ bottom: 40 })
          
          // Inputs
          Column({ space: 24 }) {
            // Username
            TextInput({ placeholder: '请输入账号' })
              .width('100%')
              .height(56)
              .backgroundColor('#F5F7FA')
              .borderRadius(12)
              .padding({ left: 20, right: 20 })
              .onChange((value: string) => {
                this.username = value;
              })

            // Password
            TextInput({ placeholder: '设置密码' })
              .type(InputType.Password)
              .width('100%')
              .height(56)
              .backgroundColor('#F5F7FA')
              .borderRadius(12)
              .padding({ left: 20, right: 20 })
              .onChange((value: string) => {
                this.password = value;
              })
            
            // Confirm Password
            TextInput({ placeholder: '再次输入密码' })
              .type(InputType.Password)
              .width('100%')
              .height(56)
              .backgroundColor('#F5F7FA')
              .borderRadius(12)
              .padding({ left: 20, right: 20 })
              .onChange((value: string) => {
                this.confirmPassword = value;
              })
          }
          .width('100%')
          .margin({ bottom: 40 })
          
          // Buttons
          Column({ space: 16 }) {
            Button('注册', { type: ButtonType.Normal, stateEffect: true })
              .width('100%')
              .height(56)
              .borderRadius(28)
              .backgroundColor('#007DFF')
              .fontSize(18)
              .fontWeight(FontWeight.Medium)
              .shadow({ radius: 8, color: 'rgba(0, 125, 255, 0.3)', offsetY: 4 })
              .onClick(async () => {
                console.info('Register submit clicked');
                if (!this.username || !this.password) {
                  promptAction.showToast({ message: '请输入账号和密码' });
                  return;
                }
                
                if (this.password !== this.confirmPassword) {
                  promptAction.showToast({ message: '两次密码输入不一致' });
                  return;
                }
                
                try {
                  const result = await this.courseService.register(this.username, this.password);
                  console.info('Register result:', result);
                  if (result === 'success') {
                    promptAction.showToast({ message: '注册成功' });
                    // 跳转到登录页
                    setTimeout(() => {
                      router.replaceUrl({ url: 'pages/LoginPage' })
                        .then(() => {
                          console.info('Navigated to LoginPage');
                        })
                        .catch((err: Error) => {
                          console.error('Failed to navigate to LoginPage:', JSON.stringify(err));
                          router.back(); // Fallback
                        });
                    }, 500);
                  } else {
                    promptAction.showToast({ message: result });
                  }
                } catch (e) {
                  console.error('Register execution failed:', JSON.stringify(e));
                  promptAction.showToast({ message: '注册异常' });
                }
              })
              
            Row() {
              Text('已有账号？ ')
                .fontSize(14)
                .fontColor('#999999')
              Text('立即登录')
                .fontSize(14)
                .fontColor('#007DFF')
                .fontWeight(FontWeight.Medium)
                .onClick(() => {
                  console.info('Go to login clicked');
                  router.back(); // Go back to Login Page
                })
            }
            .margin({ top: 8 })
          }
          .width('100%')
        }
        .width('100%')
        .padding({ left: 32, right: 32 })
        .constraintSize({ minHeight: '100%' })
        .justifyContent(FlexAlign.Start)
      }
      .width('100%')
      .layoutWeight(1)
      .scrollBar(BarState.Off)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }
}
