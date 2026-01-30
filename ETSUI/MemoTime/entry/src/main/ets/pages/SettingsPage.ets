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
 *
 */

/**
 * 设置页面
 */
import { router } from '@kit.ArkUI'
import { AppConstants } from '../common/constants/Constants'
import { Logger } from '../common/utils/Logger'
import { notificationService } from '../service/NotificationService'
import { promptAction } from '@kit.ArkUI'

const TAG = 'SettingsPage'

@Entry
@Component
struct SettingsPage {
  @State startFromMonday: boolean = false
  @State notificationEnabled: boolean = true
  @State syncEnabled: boolean = false
  @State darkMode: boolean = false
  @State isTestingNotification: boolean = false

  aboutToAppear(): void {
    // 获取通知状态
    this.notificationEnabled = notificationService.isNotificationEnabled()
  }

  /**
   * 测试通知
   */
  async testNotification(): Promise<void> {
    this.isTestingNotification = true
    try {
      const success = await notificationService.sendTestNotification()
      if (success) {
        promptAction.showToast({
          message: '通知发送成功！请检查通知栏',
          duration: 2000
        })
      } else {
        promptAction.showToast({
          message: '通知发送失败，请检查权限设置',
          duration: 2000
        })
      }
    } catch (error) {
      Logger.error(TAG, `Test notification failed: ${JSON.stringify(error)}`)
      promptAction.showToast({
        message: '通知发送失败',
        duration: 2000
      })
    }
    this.isTestingNotification = false
  }

  @Builder
  TopBar() {
    Row() {
      Image($r('sys.media.ohos_ic_public_arrow_left'))
        .width(24)
        .height(24)
        .fillColor('#333333')
        .onClick(() => router.back())

      Text('设置')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ left: 16 })

      Blank()
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .backgroundColor('#FFFFFF')
  }

  @Builder
  SectionHeader(title: string) {
    Text(title)
      .fontSize(13)
      .fontColor('#999999')
      .width('100%')
      .padding({ left: 16, top: 16, bottom: 8 })
  }

  @Builder
  SettingRow(title: string, subtitle: string, showToggle: boolean, isOn: boolean, onToggle: (isOn: boolean) => void) {
    Row() {
      Column() {
        Text(title)
          .fontSize(16)
          .fontColor('#333333')

        if (subtitle) {
          Text(subtitle)
            .fontSize(13)
            .fontColor('#999999')
            .margin({ top: 4 })
        }
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)

      if (showToggle) {
        Toggle({ type: ToggleType.Switch, isOn: isOn })
          .selectedColor('#4ECDC4')
          .onChange(onToggle)
      } else {
        Image($r('sys.media.ohos_ic_public_arrow_right'))
          .width(16)
          .height(16)
          .fillColor('#CCCCCC')
      }
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 14, bottom: 14 })
    .backgroundColor('#FFFFFF')
  }

  @Builder
  SettingsSection() {
    Column() {
      // 日历设置
      this.SectionHeader('日历')

      Column() {
        this.SettingRow('周一作为一周开始', '默认周日开始', true, this.startFromMonday, (isOn: boolean) => {
          this.startFromMonday = isOn
        })
      }
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .margin({ left: 16, right: 16 })

      // 通知设置
      this.SectionHeader('通知')

      Column() {
        this.SettingRow('允许通知', '接收日程提醒通知', true, this.notificationEnabled, (isOn: boolean) => {
          this.notificationEnabled = isOn
        })

        Divider()
          .color('#E0E0E0')
          .margin({ left: 16 })

        // 测试通知按钮
        Row() {
          Text('测试通知')
            .fontSize(16)
            .fontColor('#333333')

          Blank()

          if (this.isTestingNotification) {
            LoadingProgress()
              .width(20)
              .height(20)
              .color('#4ECDC4')
          } else {
            Text('发送')
              .fontSize(14)
              .fontColor('#4ECDC4')
              .fontWeight(FontWeight.Medium)
          }
        }
        .width('100%')
        .padding({ left: 16, right: 16, top: 14, bottom: 14 })
        .onClick(() => {
          if (!this.isTestingNotification) {
            this.testNotification()
          }
        })
      }
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .margin({ left: 16, right: 16 })

      // 同步设置
      this.SectionHeader('同步')

      Column() {
        this.SettingRow('云端同步', '将日程同步到云端（开发中）', true, this.syncEnabled, (isOn: boolean) => {
          this.syncEnabled = isOn
          if (isOn) {
            promptAction.showToast({
              message: '云端同步功能开发中',
              duration: 2000
            })
            this.syncEnabled = false
          }
        })
      }
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .margin({ left: 16, right: 16 })

      // 外观设置
      this.SectionHeader('外观')

      Column() {
        this.SettingRow('深色模式', '跟随系统', true, this.darkMode, (isOn: boolean) => {
          this.darkMode = isOn
        })
      }
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .margin({ left: 16, right: 16 })

      // 关于
      this.SectionHeader('关于')

      Column() {
        Row() {
          Text('版本')
            .fontSize(16)
            .fontColor('#333333')

          Blank()

          Text('1.0.0')
            .fontSize(14)
            .fontColor('#999999')
        }
        .width('100%')
        .padding({ left: 16, right: 16, top: 14, bottom: 14 })

        Divider()
          .color('#E0E0E0')
          .margin({ left: 16 })

        Row() {
          Text('应用名称')
            .fontSize(16)
            .fontColor('#333333')

          Blank()

          Text('MemoTime')
            .fontSize(14)
            .fontColor('#999999')
        }
        .width('100%')
        .padding({ left: 16, right: 16, top: 14, bottom: 14 })
      }
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .margin({ left: 16, right: 16, bottom: 32 })
    }
  }

  build() {
    Column() {
      this.TopBar()

      Scroll() {
        this.SettingsSection()
      }
      .layoutWeight(1)
      .scrollable(ScrollDirection.Vertical)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
  }
}
