/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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
/**
 * 设置页面
 */
import { LifeHubData } from '../../model/LifeHubData';

@Entry
@Component
export struct SettingsMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  @State isDarkMode: boolean = false;
  @State dataCount: number = 0;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadDataCount();
  }

  async loadDataCount() {
    try {
      const healthRecords = await this.lifeHubData.getHealthRecords(undefined, 1000);
      const goals = await this.lifeHubData.getAllGoals();
      const moodRecords = await this.lifeHubData.getMoodRecords(1000);
      const skills = await this.lifeHubData.getAllSkills();
      const travelPlans = await this.lifeHubData.getAllTravelPlans();
      const diaries = await this.lifeHubData.getAllDiaries();

      this.dataCount = healthRecords.length + goals.length + moodRecords.length +
      skills.length + travelPlans.length + diaries.length;
    } catch (error) {
      console.error('SettingsMain: 加载数据统计失败:', error);
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('设置')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)

        Blank()
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          // 外观设置
          Text('外观设置')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .padding({ left: 20, top: 20, bottom: 10 })

          Column() {
            Row() {
              Text('深色模式')
                .fontSize(16)
                .fontColor('#333')

              Blank()

              Toggle({ type: ToggleType.Switch, isOn: this.isDarkMode })
                .selectedColor('#007DFF')
                .onChange((isOn: boolean) => {
                  this.isDarkMode = isOn;
                  // TODO: 实现主题切换
                })
            }
            .width('100%')
            .padding(15)
          }
          .width('100%')
          .backgroundColor(Color.White)
          .margin({ left: 15, right: 15, bottom: 15 })

          // 数据管理
          Text('数据管理')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .padding({ left: 20, top: 20, bottom: 10 })

          Column() {
            Row() {
              Column() {
                Text('数据统计')
                  .fontSize(16)
                  .fontColor('#333')
                Text(`共 ${this.dataCount} 条记录`)
                  .fontSize(14)
                  .fontColor('#666')
                  .margin({ top: 5 })
              }
              .alignItems(HorizontalAlign.Start)

              Blank()
            }
            .width('100%')
            .padding(15)
            .onClick(() => {
              // 显示数据统计详情
            })

            Divider()
              .color('#f0f0f0')
              .margin({ left: 15, right: 15 })

            Row() {
              Text('导出数据')
                .fontSize(16)
                .fontColor('#007DFF')

              Blank()

              Text('>')
                .fontSize(18)
                .fontColor('#999')
            }
            .width('100%')
            .padding(15)
            .onClick(() => {
              this.exportData();
            })

            Divider()
              .color('#f0f0f0')
              .margin({ left: 15, right: 15 })

            Row() {
              Text('清除所有数据')
                .fontSize(16)
                .fontColor('#FF5722')

              Blank()

              Text('>')
                .fontSize(18)
                .fontColor('#999')
            }
            .width('100%')
            .padding(15)
            .onClick(() => {
              this.showClearDataDialog();
            })
          }
          .width('100%')
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ left: 15, right: 15, bottom: 15 })

          // 关于
          Text('关于')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .padding({ left: 20, top: 20, bottom: 10 })

          Column() {
            Row() {
              Text('应用版本')
                .fontSize(16)
                .fontColor('#333')

              Blank()

              Text('1.0.0')
                .fontSize(16)
                .fontColor('#666')
            }
            .width('100%')
            .padding(15)

            Divider()
              .color('#f0f0f0')
              .margin({ left: 15, right: 15 })

            Row() {
              Text('开发者')
                .fontSize(16)
                .fontColor('#333')

              Blank()

              Text('LifeHub Team')
                .fontSize(16)
                .fontColor('#666')
            }
            .width('100%')
            .padding(15)
          }
          .width('100%')
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ left: 15, right: 15, bottom: 20 })
        }
        .width('100%')
      }
      .layoutWeight(1)

      // 清除数据确认对话框
      if (this.showClearDialog) {
        Column() {
          // 半透明背景
          Blank()
            .width('100%')
            .height('100%')
            .backgroundColor('#000000')
            .opacity(0.5)
            .onClick(() => {
              this.showClearDialog = false;
            })

          // 确认对话框内容
          Column() {
            Text('确认清除')
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .margin({ bottom: 10 })

            Text('确定要清除所有数据吗？此操作不可恢复！')
              .fontSize(14)
              .textAlign(TextAlign.Center)
              .margin({ bottom: 20 })

            Row() {
              Button('取消')
                .layoutWeight(1)
                .height(40)
                .backgroundColor('#f0f0f0')
                .fontColor('#333')
                .onClick(() => {
                  this.showClearDialog = false;
                })

              Button('确认清除')
                .layoutWeight(1)
                .height(40)
                .margin({ left: 10 })
                .backgroundColor('#FF5722')
                .fontColor(Color.White)
                .onClick(() => {
                  this.showClearDialog = false;
                  this.clearAllData();
                })
            }
            .width('100%')
            .justifyContent(FlexAlign.SpaceBetween)
          }
          .width('80%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(10)
          .shadow({
            radius: 10,
            color: '#40000000',
            offsetX: 0,
            offsetY: 5
          })
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .position({ x: 0, y: 0 })
        .zIndex(1000)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async exportData() {
    try {
      // 导出数据为文本格式
      let exportText = 'LifeHub 数据导出\n';
      exportText += `导出时间: ${new Date().toLocaleString()}\n\n`;

      const healthRecords = await this.lifeHubData.getHealthRecords(undefined, 1000);
      exportText += `健康记录 (${healthRecords.length}条):\n`;
      for (let i = 0; i < healthRecords.length; i++) {
        const r = healthRecords[i];
        exportText += `  - ${r.type}: ${r.value} ${r.unit} (${r.date})\n`;
      }

      const goals = await this.lifeHubData.getAllGoals();
      exportText += `\n目标 (${goals.length}条):\n`;
      for (let i = 0; i < goals.length; i++) {
        const g = goals[i];
        exportText += `  - ${g.title}: ${g.progress}% (${g.status})\n`;
      }

      const moodRecords = await this.lifeHubData.getMoodRecords(1000);
      exportText += `\n心情记录 (${moodRecords.length}条):\n`;
      for (let i = 0; i < moodRecords.length; i++) {
        const m = moodRecords[i];
        exportText += `  - ${m.mood}: ${m.score}/10 (${m.date})\n`;
      }

      const skills = await this.lifeHubData.getAllSkills();
      exportText += `\n技能 (${skills.length}条):\n`;
      for (let i = 0; i < skills.length; i++) {
        const s = skills[i];
        exportText += `  - ${s.name}: 等级 ${s.level}/${s.targetLevel}\n`;
      }

      // 这里可以添加分享功能
      console.log('导出数据:', exportText);
      // TODO: 实现文件保存或分享功能
    } catch (error) {
      console.error('SettingsMain: 导出数据失败:', error);
    }
  }

  @State showClearDialog: boolean = false;

  showClearDataDialog() {
    this.showClearDialog = true;
  }

  async clearAllData() {
    try {
      // 使用 RDB 直接删除所有表数据
      const context = getContext(this);
      if (!context) {
        return;
      }

      // 注意：这里需要直接操作数据库，因为可能没有删除方法
      // 或者我们可以重新初始化数据库
      console.log('SettingsMain: 清除所有数据功能需要数据库直接操作');
      // TODO: 实现数据库清空功能

      await this.loadDataCount();
    } catch (error) {
      console.error('SettingsMain: 清除数据失败:', error);
    }
  }
}
