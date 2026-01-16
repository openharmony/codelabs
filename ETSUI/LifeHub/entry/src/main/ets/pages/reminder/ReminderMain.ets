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
 * 提醒管理页面
 */
import { LifeHubData, Reminder } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Entry
@Component
export struct ReminderMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State reminders: Reminder[] = [];
  @State isLoading: boolean = true;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadReminders();
  }

  async loadReminders() {
    this.isLoading = true;
    try {
      // 从数据库加载提醒
      this.reminders = await this.lifeHubData.getAllReminders();
    } catch (error) {
      console.error('ReminderMain: 加载提醒失败:', error);
      this.reminders = [];
    } finally {
      this.isLoading = false;
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('提醒管理')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
        
        Button('+')
          .fontSize(20)
          .width(40)
          .height(40)
          .borderRadius(20)
          .backgroundColor('#007DFF')
          .fontColor(Color.White)
          .onClick(() => {
            router.pushUrl({ url: 'pages/reminder/ReminderEdit' });
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      // 提醒列表
      if (this.isLoading) {
        Column() {
          LoadingProgress()
            .width(50)
            .height(50)
            .margin({ top: 100 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else if (this.reminders.length === 0) {
        Column() {
          Text('⏰')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('还没有提醒')
            .fontSize(18)
          .fontColor('#666')
          Text('点击右上角"+"添加提醒')
            .fontSize(14)
            .fontColor('#999')
            .margin({ top: 10 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List() {
          ForEach(this.reminders, (reminder: Reminder) => {
            ListItem() {
              this.buildReminderCard(reminder)
            }
          }, (reminder: Reminder) => reminder.id?.toString() || '')
        }
        .width('100%')
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  @Builder
  buildReminderCard(reminder: Reminder) {
    Column() {
      Row() {
        Text(this.getTypeIcon(reminder.type))
          .fontSize(32)
          .width(50)
        
        Column() {
          Text(reminder.title)
            .fontSize(18)
            .fontWeight(FontWeight.Medium)
            .textAlign(TextAlign.Start)
            .width('100%')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Text(reminder.description)
            .fontSize(14)
            .fontColor('#666')
            .textAlign(TextAlign.Start)
            .width('100%')
            .margin({ top: 5 })
            .maxLines(2)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Row() {
            Text(`⏰ ${reminder.time}`)
              .fontSize(12)
              .fontColor('#666')
            
            Text(`📅 ${this.formatDays(reminder.days)}`)
              .fontSize(12)
              .fontColor('#666')
              .margin({ left: 15 })
          }
          .width('100%')
          .margin({ top: 8 })
        }
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 15 })

        Toggle({ type: ToggleType.Switch, isOn: reminder.enabled })
          .selectedColor('#007DFF')
          .onChange((isOn: boolean) => {
            this.toggleReminder(reminder, isOn);
          })
      }
      .width('100%')
    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .margin({ left: 15, right: 15, bottom: 10 })
    .onClick(() => {
      router.pushUrl({
        url: 'pages/reminder/ReminderEdit',
        params: { reminderId: reminder.id }
      });
    })
  }

  getTypeIcon(type: string): string {
    const icons: Record<string, string> = {
      'health': '💚',
      'goal': '🎯',
      'mood': '😊',
      'learning': '📚',
      'diary': '📝',
      'custom': '🔔'
    };
    return icons[type] || '🔔';
  }

  formatDays(days: string): string {
    const dayNames = ['日', '一', '二', '三', '四', '五', '六'];
    const dayNums = days.split(',').map(d => parseInt(d));
    if (dayNums.length === 7) return '每天';
    if (dayNums.length === 5 && dayNums.includes(1) && dayNums.includes(5)) return '工作日';
    if (dayNums.length === 2 && dayNums.includes(0) && dayNums.includes(6)) return '周末';
    return dayNums.map(d => `周${dayNames[d]}`).join(' ');
  }

  async toggleReminder(reminder: Reminder, enabled: boolean) {
    reminder.enabled = enabled;
    reminder.updateTime = Date.now();
    try {
      await this.lifeHubData.updateReminder(reminder);
      await this.loadReminders();
    } catch (error) {
      console.error('ReminderMain: 更新提醒失败:', error);
    }
  }
}
