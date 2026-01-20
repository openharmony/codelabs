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
import preferences from '@ohos.data.preferences';
import { promptAction } from '@kit.ArkUI';

@Entry
@Component
struct LoginPage {
  @State password: string = "";
  @State savedPassword: string = "";
  @State action: string = "login"; // login, set, close, modify
  @State title: string = "请输入密码";

  aboutToAppear() {
    const params = router.getParams() as Record<string, Object>;
    if (params) {
      if (params['isSettingMode']) {
        this.action = 'set';
      } else if (params['action']) {
        this.action = params['action'] as string;
      }
    }

    this.updateTitle();
    
    if (this.action !== 'set') {
      this.checkSavedPassword();
    }
  }

  updateTitle() {
    switch (this.action) {
      case 'set':
        this.title = "设置6位数字密码";
        break;
      case 'close':
        this.title = "验证密码以关闭";
        break;
      case 'modify':
        this.title = "验证旧密码";
        break;
      default:
        this.title = "请输入密码";
    }
  }

  async checkSavedPassword() {
    try {
      const context = getContext(this);
      const pref = await preferences.getPreferences(context, 'coin_note_pref');
      this.savedPassword = await pref.get('app_password', '') as string;
      
      // 如果没有设置过密码，且不是设置模式，直接进入主页
      if (!this.savedPassword && this.action !== 'set') {
        this.goHome();
      }
    } catch (e) {
      console.error('Failed to get preferences', e);
      this.goHome();
    }
  }

  goHome() {
    router.replaceUrl({ url: 'pages/Index' });
  }

  inputDigit(digit: string) {
    if (this.password.length < 6) {
      this.password += digit;
      
      if (this.password.length === 6) {
        this.verifyPassword();
      }
    }
  }

  deleteDigit() {
    if (this.password.length > 0) {
      this.password = this.password.substring(0, this.password.length - 1);
    }
  }

  async verifyPassword() {
    if (this.action === 'set') {
      // 设置密码模式：保存密码
      try {
        const context = getContext(this);
        const pref = await preferences.getPreferences(context, 'coin_note_pref');
        await pref.put('app_password', this.password);
        await pref.flush();
        promptAction.showToast({ message: '密码设置成功' });
        router.back();
      } catch (e) {
        console.error('Failed to save password', e);
        promptAction.showToast({ message: '设置失败' });
      }
    } else {
      // 验证模式
      if (this.password === this.savedPassword) {
        if (this.action === 'login') {
          this.goHome();
        } else if (this.action === 'close') {
          // 关闭密码锁
          try {
            const context = getContext(this);
            const pref = await preferences.getPreferences(context, 'coin_note_pref');
            await pref.delete('app_password');
            await pref.flush();
            promptAction.showToast({ message: '密码锁已关闭' });
            router.back();
          } catch (e) {
            console.error('Failed to delete password', e);
            promptAction.showToast({ message: '操作失败' });
          }
        } else if (this.action === 'modify') {
          // 验证旧密码成功，进入设置新密码模式
          this.action = 'set';
          this.password = "";
          this.updateTitle();
        }
      } else {
        promptAction.showToast({ message: '密码错误' });
        this.password = ""; // 清空重输
      }
    }
  }

  build() {
    Column() {
      // 标题
      Text(this.title)
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor("#333333")
        .margin({ top: 80, bottom: 40 })

      // 密码点显示
      Row({ space: 16 }) {
        ForEach([0, 1, 2, 3, 4, 5], (index: number) => {
          Circle({ width: 16, height: 16 })
            .fill(index < this.password.length ? "#4A6CF7" : "#E0E0E0")
        })
      }
      .margin({ bottom: 80 })

      // 数字键盘
      Grid() {
        ForEach(["1", "2", "3", "4", "5", "6", "7", "8", "9", "", "0", "del"], (item: string) => {
          GridItem() {
            if (item === "del") {
              // 删除键
              Column() {
                Text("⌫")
                  .fontSize(24)
                  .fontColor("#333333")
              }
              .width(70)
              .height(70)
              .justifyContent(FlexAlign.Center)
              .onClick(() => this.deleteDigit())
            } else if (item === "") {
              // 空白占位
              Column().width(70).height(70)
            } else {
              // 数字键
              Column() {
                Text(item)
                  .fontSize(28)
                  .fontWeight(FontWeight.Medium)
                  .fontColor("#333333")
              }
              .width(70)
              .height(70)
              .borderRadius(35)
              .backgroundColor("#F5F7FA")
              .justifyContent(FlexAlign.Center)
              .onClick(() => this.inputDigit(item))
            }
          }
        })
      }
      .columnsTemplate("1fr 1fr 1fr")
      .rowsGap(20)
      .width("80%")
    }
    .width("100%")
    .height("100%")
    .backgroundColor(Color.White)
  }
}
