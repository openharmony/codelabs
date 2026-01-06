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
                  // 为了防止页面初始化时自动触发 onChange 导致逻辑死循环，
                  // 我们在这里做一个简单的判断，或者是依靠用户的真实点击。
                  // 但最稳妥的方式是直接处理。

                  this.logger.info(`👆 [UI] 开关被点击/改变: ${isOn}`);

                  if (isOn) {
                    // === 用户想开启 ===
                    // 先验证指纹
                    const success = await BiometricService.getInstance().authenticate();
                    if (success) {
                      this.isBiometricOn = true;
                      // 保存为 true
                      await this.settingModel.setBiometricEnabled(true);
                      promptAction.showToast({ message: '生物认证已开启' });
                    } else {
                      // 验证失败，把开关拨回去
                      this.isBiometricOn = false;
                      promptAction.showToast({ message: '认证失败' });
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
            .padding({ left: 16, right: 16, top: 12, bottom: 12 })
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