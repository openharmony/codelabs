/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import router from '@ohos.router';
import PreferenceUtil from '../utils/PreferenceUtil';
import common from '@ohos.app.ability.common';

@Entry
@Component
struct SettingsPage {
  private appVersion: string = 'v1.0.2';
  private buildVersion: string = '20231105_Beta';
  private developerEmail: string = 'support@mypassword.app';

  handleLogout(): void {
    try {
      AlertDialog.show({
        title: '退出登录',
        message: '退出后，下次进入应用需要重新验证身份。',
        primaryButton: { value: '取消', action: () => {} },
        secondaryButton: {
          value: '退出',
          fontColor: Color.Red,
          action: () => {
            let context = getContext(this) as common.UIAbilityContext;
            PreferenceUtil.init(context).then(async () => {
              await PreferenceUtil.clearLoginInfo();
              try {
                router.replaceUrl({ url: 'pages/LoginPage' });
              } catch (e) {
                console.error(`Nav error: ${JSON.stringify(e)}`);
              }
            });
          }
        }
      });
    } catch (e) {
      console.error(`Dialog error: ${JSON.stringify(e)}`);
    }
  }

  @Builder InfoRow(label: string, value: string) {
    Row() {
      Text(label).fontSize(16).fontColor(0x333333)
      Blank()
      Text(value).fontSize(16).fontColor(Color.Gray)
    }
    .width('100%')
    .padding({ top: 15, bottom: 15 })
    .border({ width: { bottom: 1 }, color: 0xF0F0F0 })
  }

  @Builder TextParagraph(title: string, content: string) {
    Column() {
      Text(title)
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor(0x333333)
        .width('100%')
        .margin({ bottom: 8 })

      Text(content)
        .fontSize(14)
        .fontColor(0x666666)
        .lineHeight(22)
        .width('100%')
    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(8)
    .margin({ bottom: 10 })
  }

  build() {
    Column() {
      Row() {
        Text('< 返回')
          .fontSize(16).fontColor(Color.Gray)
          .onClick(() => {
            try { router.back(); } catch (e) {}
          })
        Blank()
        Text('设置与关于').fontSize(18).fontWeight(FontWeight.Bold)
        Blank()
        Text('      ')
      }
      .width('100%').padding(15).backgroundColor(Color.White)

      Scroll() {
        Column() {
          // 头部
          Column() {
            // 【修复】用 Emoji 代替不存在的图片资源，解决 Unknown resource 报错
            Text('🔐')
              .fontSize(60)
              .margin({ bottom: 10 })
            Text('我的密码本')
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .margin({ bottom: 5 })
            Text(`Version ${this.appVersion}`)
              .fontSize(14)
              .fontColor(Color.Gray)
          }
          .width('100%')
          .padding(30)
          .alignItems(HorizontalAlign.Center)

          // 详细信息
          Column() {
            this.InfoRow('构建版本', this.buildVersion)
            this.InfoRow('开发语言', 'ArkTS / HarmonyOS')
            this.InfoRow('数据存储', 'RDB (本地数据库)')
            this.InfoRow('联系我们', this.developerEmail)
          }
          .padding({ left: 15, right: 15 })
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ bottom: 20 })

          // 帮助文档
          Text('使用指南')
            .fontSize(14).fontColor(Color.Gray).margin({ left: 5, bottom: 8 })

          this.TextParagraph(
            '如何保障数据安全？',
            '本应用采用离线优先策略，您的所有账号密码仅存储在当前手机的本地安全区域，绝不会上传至任何云端服务器。'
          )


          this.TextParagraph(
            '免责声明',
            '本应用仅作为密码辅助记忆工具，开发者不对因用户手机丢失、中病毒或Root破解导致的数据泄露承担法律责任。'
          )

          // 底部按钮
          Button('退出登录')
            .width('100%')
            .height(50)
            .backgroundColor(0xFF5252)
            .fontColor(Color.White)
            .borderRadius(25)
            .margin({ top: 20, bottom: 40 })
            .onClick(() => this.handleLogout())

          Text('Copyright © 2023-2024 MySecurity Inc.')
            .fontSize(12)
            .fontColor(Color.Gray)
            .margin({ bottom: 20 })
        }
        .padding(15)
      }
      .layoutWeight(1)
      .align(Alignment.Top)
    }
    .width('100%')
    .height('100%')
    .backgroundColor(0xF0F2F5)
  }
}