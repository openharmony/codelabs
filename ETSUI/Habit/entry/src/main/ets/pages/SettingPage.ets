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
import promptAction from '@ohos.promptAction';
import HabitRepository from '../model/repository/HabitRepository';
import RecordRepository from '../model/repository/RecordRepository';
import CategoryRepository from '../model/repository/CategoryRepository';

@Entry
@Component
struct SettingPage {
  @State isDarkMode: boolean = false; // 模拟开关状态

  build() {
    Column() {
      // 1. 顶部导航
      Row() {
        Button({ type: ButtonType.Circle, stateEffect: true }) {
          Text('<').fontSize(26).fontWeight(FontWeight.Bold).fontColor('#333')
        }
        .width(40).height(40).backgroundColor('#FFF')
        .onClick(() => router.back())

        Text("系统设置")
          .fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 15 })
      }
      .width('100%').padding(15).backgroundColor('#F1F3F5')

      // 2. 设置列表
      List({ space: 10 }) {

        // --- 分组 1: 个人信息 ---
        ListItem() {
          this.SettingItem("开发者", "李孙鹏")
        }

        ListItem() {
          this.SettingItem("学号", "3220251393")
        }

        ListItem() {
          this.SettingItem("版本号", "v1.0.0 Beta")
        }


        // --- 分组 3: 数据管理 (危险区) ---
        ListItem() {
          Row() {
            Text("重置所有数据")
              .fontSize(16).fontColor('#FF4040') // 红色警示
            Blank()
            Text('>')
              .fontSize(16).fontColor('#CCC')
          }
          .width('100%')
          .padding(15)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .onClick(() => {
            this.showResetDialog();
          })
        }
      }
      .width('100%')
      .layoutWeight(1)
      .padding(15)
    }
    .height('100%')
    .backgroundColor('#F1F3F5')
  }

  // 封装通用的列表项样式
  @Builder
  SettingItem(title: string, value: string) {
    Row() {
      Text(title).fontSize(16).fontColor('#333')
      Blank()
      Text(value).fontSize(14).fontColor('#999')
    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(12)
  }

  // 显示确认弹窗
  showResetDialog() {
    AlertDialog.show({
      title: '⚠️ 警告',
      message: '确定要清空所有习惯和打卡记录吗？此操作无法撤销！',
      primaryButton: {
        value: '取消',
        action: () => {
        }
      },
      secondaryButton: {
        value: '确认清空',
        fontColor: Color.Red,
        action: () => {
          this.handleResetData();
        }
      }
    })
  }

  // 执行清空逻辑
  async handleResetData() {
    try {
      await RecordRepository.cleanAll();
      await HabitRepository.cleanAll();
      await CategoryRepository.cleanAll();

      promptAction.showToast({ message: '数据已全部重置！' });

      setTimeout(() => {
        router.back();
      }, 500);
    } catch (err) {
      console.error('重置失败: ' + JSON.stringify(err));
    }
  }
}