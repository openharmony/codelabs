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
import pasteboard from '@ohos.pasteboard';
import promptAction from '@ohos.promptAction';
import { BusinessError } from '@ohos.base';

@Entry
@Component
struct ForgotPasswordPage {
  // 这里配置你的支持邮箱
  private supportEmail: string = 'support@mypassword.app';

  /**
   * 复制邮箱到剪贴板
   */
  handleCopyEmail() {
    try {
      let data = pasteboard.createData(pasteboard.MIMETYPE_TEXT_PLAIN, this.supportEmail);
      let systemPasteboard = pasteboard.getSystemPasteboard();
      systemPasteboard.setData(data).then(() => {
        promptAction.showToast({ message: '邮箱地址已复制' });
      }).catch((err: BusinessError) => {
        promptAction.showToast({ message: '复制失败，请手动记录' });
      });
    } catch (e) {
      console.error(`Pasteboard error: ${e}`);
    }
  }

  build() {
    Column() {
      // 1. 顶部标题栏
      Row() {
        Text('< 返回')
          .fontSize(16).fontColor(Color.Gray)
          .onClick(() => {
            try { router.back(); } catch (e) {}
          })
        Blank()
        Text('找回密码').fontSize(18).fontWeight(FontWeight.Bold)
        Blank()
        Text('      ') // 占位保持居中
      }
      .width('100%').padding(15).backgroundColor(Color.White)

      // 2. 内容区域
      Column({ space: 20 }) {
        // 图标
        Text('📧')
          .fontSize(80)
          .margin({ top: 50, bottom: 20 })

        // 标题
        Text('无法登录？')
          .fontSize(22)
          .fontWeight(FontWeight.Bold)
          .fontColor(0x333333)

        // 说明文案
        Text('如果您忘记了密码，请通过以下邮件联系开发者获取技术支持。')
          .fontSize(15)
          .fontColor(0x666666)
          .lineHeight(24)
          .textAlign(TextAlign.Center)
          .margin({ bottom: 20 })

        // 邮箱显示区域
        Column({ space: 10 }) {
          Text('技术支持邮箱')
            .fontSize(12)
            .fontColor(Color.Gray)

          Text(this.supportEmail)
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor(0x007DFF)
            .onClick(() => this.handleCopyEmail()) // 点击邮箱文字也能复制
        }
        .padding(20)
        .backgroundColor(0xF5F7FA)
        .borderRadius(12)
        .width('100%')
        .alignItems(HorizontalAlign.Center)

        // 复制按钮
        Button('复制邮箱地址')
          .width('80%')
          .height(45)
          .backgroundColor(0x007DFF)
          .borderRadius(25)
          .margin({ top: 20 })
          .onClick(() => this.handleCopyEmail())

      }
      .padding(30)
      .width('100%')
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Center)
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }
}