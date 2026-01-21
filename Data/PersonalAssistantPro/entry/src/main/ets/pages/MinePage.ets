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
import { BiometricService } from '../services/BiometricService';
import { BackupService } from '../services/BackupService';
import { promptAction } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit'; // 引入标准错误类型

@Component
export struct MinePage {
  @State isBiometricOn: boolean = false;
  @State isProcessing: boolean = false;

  private settingModel = new SettingModel();

  async aboutToAppear() {
    this.isBiometricOn = await this.settingModel.isBiometricEnabled();
  }

  // 处理备份
  async handleBackup() {
    if (this.isProcessing) return;
    this.isProcessing = true;
    try {
      const path = await BackupService.getInstance().backup(getContext(this));
      promptAction.showDialog({
        title: '备份成功',
        message: `数据已导出至沙箱:\n${path}`,
        buttons: [{ text: '确定', color: '#007DFF' }]
      });
    } catch (err) {
      //  修复：catch 括号内不写类型，在内部处理
      promptAction.showToast({ message: '备份失败，请查看日志' });
    } finally {
      this.isProcessing = false;
    }
  }

  // 处理恢复
  async handleRestore() {
    if (this.isProcessing) return;

    promptAction.showDialog({
      title: '确认恢复',
      message: '恢复操作将读取本地备份文件并追加到数据库中。',
      buttons: [
        { text: '取消', color: '#666666' },
        { text: '确定恢复', color: '#007DFF' }
      ]
    }).then(async (resp) => {
      if (resp.index === 1) {
        this.isProcessing = true;
        try {
          const count = await BackupService.getInstance().restore(getContext(this));

          AppStorage.setOrCreate('refreshEventList', Date.now());

          promptAction.showToast({ message: `恢复成功！共导入 ${count} 条数据` });
        } catch (err) {
          //  修复：移除类型注解，使用断言获取 message
          let msg = '未知错误';
          // 尝试将其断言为 Error 对象获取 message，或者直接转字符串
          if (err) {
            msg = (err as BusinessError).message ?? JSON.stringify(err);
          }
          promptAction.showToast({ message: `恢复失败: ${msg}` });
        } finally {
          this.isProcessing = false;
        }
      }
    });
  }

  @Builder
  SectionHeader(title: string) {
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
        Text('Admin User')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
      .height(200)
      .justifyContent(FlexAlign.Center)
      .backgroundColor('#E5F1FF')

      // 2. 设置选项列表
      List() {
        // --- 第一组：隐私安全 ---
        // ListItem() {
        //   Column() {
        //     this.SectionHeader('隐私与安全')
        //     Row() {
        //       Column() {
        //         Text('生物识别锁')
        //           .fontSize(16)
        //           .fontColor('#333333')
        //         Text('开启后，查看联系人详情需验证指纹')
        //           .fontSize(12)
        //           .fontColor('#999999')
        //           .margin({ top: 4 })
        //       }
        //       .alignItems(HorizontalAlign.Start)
        //       .layoutWeight(1)
        //
        //       Toggle({ type: ToggleType.Switch, isOn: this.isBiometricOn })
        //         .onChange(async (isOn: boolean) => {
        //           if (isOn) {
        //             const success = await BiometricService.getInstance().authenticate();
        //             if (success) {
        //               this.isBiometricOn = true;
        //               await this.settingModel.setBiometricEnabled(true);
        //               promptAction.showToast({ message: '隐私保护已开启' });
        //             } else {
        //               this.isBiometricOn = false;
        //               promptAction.showToast({ message: '认证失败' });
        //             }
        //           } else {
        //             this.isBiometricOn = false;
        //             await this.settingModel.setBiometricEnabled(false);
        //             promptAction.showToast({ message: '隐私保护已关闭' });
        //           }
        //         })
        //     }
        //     .width('100%')
        //     .padding({ left: 16, right: 16, top: 12, bottom: 12 })
        //     .backgroundColor(Color.White)
        //   }
        // }

        // --- 第二组：数据管理 (F3) ---
        ListItem() {
          Column() {
            this.SectionHeader('数据同步 (模拟)')

            Column() {
              // 备份按钮
              Row() {
                Text('备份数据至本地 (模拟上传)')
                  .fontSize(16)
                  .fontColor('#333333')
                Blank()
                // 箭头占位
                Text('>')
                  .fontSize(16)
                  .fontColor('#999999')
                  .opacity(0.5)
              }
              .width('100%')
              .padding({ left: 16, right: 16, top: 16, bottom: 16 })
              .backgroundColor(Color.White)
              .onClick(() => this.handleBackup())

              Divider().color('#F1F3F5').margin({ left: 16 })

              // 恢复按钮
              Row() {
                Text('从本地恢复数据 (模拟下载)')
                  .fontSize(16)
                  .fontColor('#333333')
                Blank()
                Text('>')
                  .fontSize(16)
                  .fontColor('#999999')
                  .opacity(0.5)
              }
              .width('100%')
              .padding({ left: 16, right: 16, top: 16, bottom: 16 })
              .backgroundColor(Color.White)
              .onClick(() => this.handleRestore())
            }
            .borderRadius(8)
            .clip(true)
          }
        }

        // --- Loading 状态提示 ---
        if (this.isProcessing) {
          ListItem() {
            Row() {
              LoadingProgress().width(20).height(20).margin({ right: 8 })
              Text('处理中...').fontSize(14).fontColor('#999999')
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
  }
}