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
import { CourseDataService } from '../service/CourseDataService';
import promptAction from '@ohos.promptAction';

@Entry
@Component
struct LoginPage {
  @State username: string = '';
  @State password: string = '';
  private courseService: CourseDataService = CourseDataService.getInstance();

  async aboutToAppear() {
    this.courseService.init(getContext(this)).catch((err: Error) => {
      console.error('LoginPage init failed:', JSON.stringify(err));
    });
  }

  build() {
    Column() {
      Scroll() {
        Column() {
          // Logo or Header
          Image($r('app.media.startIcon'))
            .width(80)
            .height(80)
            .borderRadius(16)
            .margin({ top: 60, bottom: 24 })
            .shadow({ radius: 10, color: 'rgba(0, 125, 255, 0.2)', offsetY: 5 })

          Text('欢迎回来')
            .fontSize(28)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 8 })

          Text('登录您的账号以继续')
            .fontSize(16)
            .fontColor('#999999')
            .margin({ bottom: 48 })

          // Inputs
          Column({ space: 24 }) {
            // Username Input
            TextInput({ placeholder: '请输入账号' })
              .width('100%')
              .height(56)
              .backgroundColor('#F5F7FA')
              .borderRadius(12)
              .padding({ left: 20, right: 20 })
              .onChange((value: string) => {
                this.username = value;
              })

            // Password Input
            TextInput({ placeholder: '请输入密码' })
              .type(InputType.Password)
              .width('100%')
              .height(56)
              .backgroundColor('#F5F7FA')
              .borderRadius(12)
              .padding({ left: 20, right: 20 })
              .onChange((value: string) => {
                this.password = value;
              })
          }
          .width('100%')
          .margin({ bottom: 40 })

          // Buttons
          Column({ space: 16 }) {
            Button('登录', { type: ButtonType.Normal, stateEffect: true })
              .width('100%')
              .height(56)
              .borderRadius(28)
              .backgroundColor('#007DFF')
              .fontSize(18)
              .fontWeight(FontWeight.Medium)
              .shadow({ radius: 8, color: 'rgba(0, 125, 255, 0.3)', offsetY: 4 })
              .onClick(async () => {
                console.info('Login button clicked');
                if (!this.username || !this.password) {
                  promptAction.showToast({ message: '请输入账号和密码' });
                  return;
                }

                try {
                  const result = await this.courseService.login(this.username, this.password);
                  console.info('Login result:', result);
                  if (result === 'success') {
                    promptAction.showToast({ message: '登录成功' });
                    // Delay slightly to ensure toast is visible and state is stable
                    setTimeout(() => {
                      router.replaceUrl({ url: 'pages/Index' })
                        .then(() => {
                          console.info('Navigated to Index');
                        })
                        .catch((err: Error) => {
                          console.error('Failed to navigate to Index:', JSON.stringify(err));
                          // Fallback to push if replace fails
                          router.pushUrl({ url: 'pages/Index' });
                        });
                    }, 500);
                  } else {
                    promptAction.showToast({ message: result });
                  }
                } catch (e) {
                  console.error('Login failed:', JSON.stringify(e));
                  promptAction.showToast({ message: '登录异常' });
                }
              })

            Row() {
              Text('还没有账号？ ')
                .fontSize(14)
                .fontColor('#999999')
              Text('立即注册')
                .fontSize(14)
                .fontColor('#007DFF')
                .fontWeight(FontWeight.Medium)
                .onClick(() => {
                  console.info('Register link clicked');
                  router.pushUrl({ url: 'pages/RegisterPage' });
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
      .height('100%')
      .scrollBar(BarState.Off)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }
}
