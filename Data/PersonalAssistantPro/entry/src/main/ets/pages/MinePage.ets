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
// entry/src/main/ets/pages/MinePage.ets
import { SettingModel } from '../model/SettingModel';
import { BackupService } from '../services/BackupService';
import { promptAction } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { Logger } from '../common/utils/Logger';

const logger = new Logger('MinePage');

@Component
export struct MinePage {
  @State isBiometricOn: boolean = false;
  // 添加处理状态，防止重复点击
  @State isProcessing: boolean = false;
  private settingModel = new SettingModel();

  async aboutToAppear() {
    this.isBiometricOn = await this.settingModel.isBiometricEnabled();
  }

  // 处理备份
  async handleBackup() {
    if (this.isProcessing) {
      return;
    }
    this.isProcessing = true;
    try {
      const path = await BackupService.getInstance().backup(getContext(this));

      // 显示成功弹窗，并传入路径参数
      promptAction.showDialog({
        title: $r('app.string.mine_backup_success_title'),
        message: $r('app.string.mine_backup_success_msg', path),
        buttons: [{ text: $r('app.string.common_confirm'), color: '#007DFF' }]
      });
    } catch (err) {
      logger.error('Backup failed', JSON.stringify(err));
      promptAction.showToast({ message: $r('app.string.mine_backup_fail') });
    } finally {
      this.isProcessing = false;
    }
  }

  // 处理恢复
  async handleRestore() {
    if (this.isProcessing) {
      return;
    }

    promptAction.showDialog({
      title: $r('app.string.mine_restore_confirm_title'),
      message: $r('app.string.mine_restore_confirm_msg'),
      buttons: [
        { text: $r('app.string.common_cancel'), color: '#666666' },
        { text: $r('app.string.mine_restore_btn'), color: '#007DFF' }
      ]
    }).then(async (resp) => {
      if (resp.index === 1) {
        this.isProcessing = true;
        try {
          promptAction.showToast({ message: $r('app.string.common_loading') });

          const count = await BackupService.getInstance().restore(getContext(this));

          AppStorage.setOrCreate('refreshEventList', Date.now());
          AppStorage.setOrCreate('GlobalRefreshSignal', Date.now());

          promptAction.showToast({ message: $r('app.string.mine_restore_success', count) });
        } catch (error) {
          // =========== 🔍 调试日志开始 ===========
          // 将 error 强转为 BusinessError 以便访问属性 (ArkTS 语法要求)
          let err = error as BusinessError;

          logger.error('❌ [Restore Debug] 捕获到异常!');
          logger.error(`❌ [Restore Debug] 类型 (typeof): ${typeof error}`);
          // 显式打印 message，而不是 stringify
          logger.error(`❌ [Restore Debug] 错误信息 (message): ${err.message}`);
          logger.error(`❌ [Restore Debug] 错误代码 (code): ${err.code}`);
          logger.error(`❌ [Restore Debug] 堆栈 (stack): ${err.stack}`);
          // =========== 🔍 调试日志结束 ===========

          // 提取用于显示的错误信息
          let uiMsg = '';
          if (err.message && err.message.length > 0) {
            uiMsg = err.message; // 优先使用 Error 对象的 message
          } else {
            // 如果 message 为空，再尝试转字符串，防止出现 "{}"
            uiMsg = JSON.stringify(error);
            if (uiMsg === '{}') {
              uiMsg = '未知错误 (Unknown Error)';
            }
          }

          promptAction.showToast({
            message: $r('app.string.mine_restore_fail', uiMsg),
            duration: 3000
          });
        } finally {
          this.isProcessing = false;
        }
      }
    });
  }

  // 生物识别开关切换
  async onBiometricChange(isOn: boolean) {
    this.isBiometricOn = isOn;
    await this.settingModel.setBiometricEnabled(isOn);
    if (isOn) {
      promptAction.showToast({ message: $r('app.string.privacy_active_toast') });
    } else {
      promptAction.showToast({ message: $r('app.string.privacy_inactive_toast') });
    }
  }

  @Builder
  SectionHeader(title: ResourceStr) {
    Text(title)
      .fontSize(14)
      .fontColor('#666666')
      .margin({ left: 16, top: 20, bottom: 8 })
      .width('100%')
  }

  build() {
    Column() {
      // 1. 顶部个人信息区域
      Column() {
        Image($r('app.media.app_icon'))
          .width(80)
          .height(80)
          .margin({ bottom: 10 })
          .borderRadius(16)
          // 简单的灰度处理模拟头像
          .grayscale(0)
        Text('Personal Assistant')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 4 })
        Text('Pro Version')
          .fontSize(14)
          .fontColor('#666666')
      }
      .width('100%')
      .height(220)
      .justifyContent(FlexAlign.Center)
      .backgroundColor('#E5F1FF')

      // 2. 设置选项列表
      List() {
        // --- 第一组：设置 ---
        // ListItem() {
        //   Column() {
        //     this.SectionHeader($r('app.string.settings_title'))
        //
        //     Column() {
        //       // 隐私锁开关
        //       Row() {
        //         Row() {
        //           Image($r('app.media.ic_public_lock')) // 确保你有这个图标，或者换一个存在的
        //             .width(24).height(24).margin({ right: 12 }).fillColor('#007DFF')
        //           Text($r('app.string.privacy_lock_title')).fontSize(16).fontColor('#333333')
        //         }
        //
        //         Toggle({ type: ToggleType.Switch, isOn: this.isBiometricOn })
        //           .onChange((isOn) => this.onBiometricChange(isOn))
        //       }
        //       .width('100%')
        //       .justifyContent(FlexAlign.SpaceBetween)
        //       .padding({ left: 16, right: 16, top: 12, bottom: 12 })
        //       .backgroundColor(Color.White)
        //     }
        //     .borderRadius(12)
        //     .clip(true)
        //     .backgroundColor(Color.White)
        //     .margin({ left: 12, right: 12 })
        //   }
        // }

        // --- 第二组：数据管理 ---
        ListItem() {
          Column() {
            this.SectionHeader($r('app.string.mine_section_sync'))

            Column() {
              // 备份按钮
              Row() {
                Row() {
                  Image($r('app.media.ic_public_save')) // 假设有 save 图标，或者用 ic_public_folder
                    .width(24).height(24).margin({ right: 12 }).fillColor('#007DFF')
                  Text($r('app.string.mine_item_backup')).fontSize(16).fontColor('#333333')
                }

                Image($r('app.media.ic_public_arrow_right'))
                  .width(16).height(16).fillColor('#CCCCCC')
              }
              .width('100%')
              .justifyContent(FlexAlign.SpaceBetween)
              .padding(16)
              .backgroundColor(Color.White)
              .onClick(() => this.handleBackup())

              Divider().color('#F1F3F5').margin({ left: 52 })

              // 恢复按钮
              Row() {
                Row() {
                  Image($r('app.media.ic_public_refresh')) // 假设有 refresh 图标
                    .width(24).height(24).margin({ right: 12 }).fillColor('#007DFF')
                  Text($r('app.string.mine_item_restore')).fontSize(16).fontColor('#333333')
                }

                Image($r('app.media.ic_public_arrow_right'))
                  .width(16).height(16).fillColor('#CCCCCC')
              }
              .width('100%')
              .justifyContent(FlexAlign.SpaceBetween)
              .padding(16)
              .backgroundColor(Color.White)
              .onClick(() => this.handleRestore())
            }
            .borderRadius(12)
            .clip(true)
            .backgroundColor(Color.White)
            .margin({ left: 12, right: 12 })
          }
        }

        // --- Loading 状态提示 ---
        if (this.isProcessing) {
          ListItem() {
            Row() {
              LoadingProgress().width(24).height(24).margin({ right: 8 }).color('#007DFF')
              Text($r('app.string.common_loading')).fontSize(14).fontColor('#999999')
            }
            .width('100%')
            .justifyContent(FlexAlign.Center)
            .padding({ top: 20 })
          }
        }
      }
      .width('100%')
      .layoutWeight(1)
      .backgroundColor('#F1F3F5')
      .padding({ bottom: 20 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}