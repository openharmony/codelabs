/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
import { SettingModel } from '../model/SettingModel';
import { BiometricService } from '../services/BiometricService';
import promptAction from '@ohos.promptAction';
import { Logger } from '../common/utils/Logger';

@Component
export struct SettingsPage {
  // 默认给 false，等待 aboutToAppear 读取真实值
  @State isBiometricOn: boolean = false;
  private settingModel = new SettingModel();
  private logger = new Logger('SettingsPage');

  // 页面即将显示时触发
  async aboutToAppear() {
    this.logger.info('🚀 [Page] 进入设置页，开始读取配置...');

    try {
      // 1. 从数据库/首选项读取最新状态
      const savedStatus = await this.settingModel.isBiometricEnabled();
      this.logger.info(`📄 [Page] 读到的状态是: ${savedStatus}`);

      // 2. 更新 UI (这会让开关自动跳到正确位置)
      this.isBiometricOn = savedStatus;

    } catch (err) {
      this.logger.error('❌ [Page] 读取配置失败', err);
    }
  }

  build() {
    NavDestination() {
      Column() {
        List() {
          ListItem() {
            Row() {
              Column() {
                Text($r('app.string.biometric_title'))
                  .fontSize(16)
                  .fontColor('#333333')
                  .fontWeight(FontWeight.Medium)
                Text($r('app.string.biometric_desc'))
                  .fontSize(12)
                  .fontColor('#999999')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)

              // 开关组件
              Toggle({ type: ToggleType.Switch, isOn: this.isBiometricOn })
                .onChange(async (isOn: boolean) => {

                  this.logger.info(`👆 [UI] 开关被点击/改变: ${isOn}`);

                  if (isOn) {
                    // === 用户想开启 ===
                    // 先验证指纹
                    // ✅ 传入当前上下文
                    const success = await BiometricService.getInstance().authenticate(getContext(this));
                    if (success) {
                      this.isBiometricOn = true;
                      // 保存为 true
                      await this.settingModel.setBiometricEnabled(true);
                      promptAction.showToast({ message: $r('app.string.auth_toast_opened') });
                    } else {
                      // 验证失败，把开关拨回去
                      this.isBiometricOn = false;
                      promptAction.showToast({ message: $r('app.string.auth_toast_failed') });
                    }
                  } else {
                    // === 用户想关闭 ===
                    // 直接保存为 false
                    this.isBiometricOn = false;
                    await this.settingModel.setBiometricEnabled(false);
                  }
                })
            }
            .width('100%')
            .padding({
              left: 16,
              right: 16,
              top: 12,
              bottom: 12
            })
            .backgroundColor(Color.White)
            .borderRadius(12)
          }
        }
        .width('100%')
        .margin({ top: 12 })
        .padding({ left: 12, right: 12 })
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F1F3F5')
    }
    .title($r('app.string.settings_title'))
    .backgroundColor('#F1F3F5')
  }
}