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
import { I18nManager } from '../common/I18nManager';

// 定义语言选项的显式接口
interface LanguageOption {
  label: string; // 显示的名称，如“简体中文”
  value: string; // 逻辑值，如“zh-Hans”
  sub: string; // 副标题，如“Chinese”
}

@Entry
@Component
struct LanguageSettings {
  // 核心：监听全局语言状态
  @StorageLink('currentLanguage') currentLang: string = 'zh-Hans';
  // 使用显式类型数组代替 Array<any>
  private langOptions: LanguageOption[] = [
    {
      label: '简体中文',
      value: 'zh-Hans',
      sub: 'Chinese'
    },
    {
      label: 'English',
      value: 'en-US',
      sub: 'English'
    }
  ];

  build() {
    Column() {
      // --- 自定义导航栏 ---
      Row() {
        Image($r('app.media.back'))
          .width(24)
          .height(24)
          .onClick(() => {
            router.back();
          })

        // 使用之前定义的变量：多语言切换
        Text($r('app.string.btn_lang_switch'))
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 16 })
      }
      .width('100%')
      .height(56)
      .padding({ left: 16 })
      .backgroundColor('#FFFFFF')

      // --- 语言选择列表 ---
      List() {
        ForEach(this.langOptions, (item: LanguageOption) => {
          ListItem() {
            Row() {
              Column() {
                Text(item.label)
                  .fontSize(16)
                  .fontColor(this.currentLang === item.value ? '#007DFF' : '#181818')

                Text(item.sub)
                  .fontSize(12)
                  .fontColor('#999999')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)

              Blank()

              // 选中状态标识
              if (this.currentLang === item.value) {
                Image($r('app.media.ok'))
                  .width(20)
                  .height(20)
                  .fillColor('#007DFF')
              }
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .onClick(() => {
              if (this.currentLang !== item.value) {
                I18nManager.switchLanguage(item.value);
              }
            })
          }
        }, (item: LanguageOption) => item.value)
      }
      .width('100%')
      .divider({
        strokeWidth: 0.5,
        color: '#EEEEEE',
        startMargin: 16,
        endMargin: 16
      })
      .margin({ top: 12 })
      .backgroundColor('#FFFFFF')
      .flexGrow(1)

      // 底部额外说明，增加视觉丰富度
      Text('Language settings will apply to the entire application.')
        .fontSize(12)
        .fontColor('#CCCCCC')
        .margin({ bottom: 20 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}