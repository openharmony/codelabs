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

@Entry
@Component
struct Settings {
  build() {
    Column() {
      // --- 自定义导航栏 ---
      Row() {
        Image($r('app.media.back'))
          .width(24)
          .height(24)
          .onClick(() => {
            router.back()
          })

        Text($r('app.string.label_settings'))
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 16 })

        Blank() // 自动撑开中间空间
      }
      .width('100%')
      .height(56)
      .padding({ left: 16, right: 16 })
      .backgroundColor(Color.White)

      // 设置项列表
      List() {
        // 切换语言
        ListItem() {
          this.SettingItem($r('app.string.btn_lang_switch'), () => {
            router.pushUrl({ url: 'pages/LanguageSettings' })
          })
        }

        // 修改密码
        ListItem() {
          this.SettingItem($r('app.string.title_change_pwd'), () => {
            router.pushUrl({ url: 'pages/ChangePassword' })
          })
        }

        // 关于我们
        ListItem() {
          this.SettingItem($r('app.string.label_about_us'), () => {
            // 暂时留空或弹个窗
            router.pushUrl({ url: 'pages/AboutUs' })
          })
        }
      }
      .width('100%')
      .backgroundColor(Color.White)
      .margin({ top: 12 })
      .divider({
        strokeWidth: 0.5,
        color: '#f0f0f0',
        startMargin: 16,
        endMargin: 16
      }) // 分割线
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5') // 设置页面常用的浅灰色背景
  }

  // 封装设置项组件
  @Builder
  SettingItem(title: ResourceStr, action: () => void) {
    Row() {
      Text(title)
        .fontSize(16)
        .fontColor('#181818')

      Blank()

      Image($r('app.media.arrow_right')) // 右侧箭头
        .width(16)
        .height(16)
        .opacity(0.4)
    }
    .width('100%')
    .height(64)
    .padding({ left: 16, right: 16 })
    .onClick(() => {
      action()
    })
  }
}