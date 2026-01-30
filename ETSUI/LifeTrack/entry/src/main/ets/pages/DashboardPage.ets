/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// 在Dashboard页面顶部添加导入
import { ContactManager } from './ContactManager';
import { DataManager } from './DataManager';
import { StepManager } from './StepManager';
import { ContactsItem } from '../model/ContactModel';
import { CalendarEvent } from '../model/EventModel';
import { StepRecord } from '../model/StepModel';
import common from '@ohos.app.ability.common';

// 跳转页面入口函数
@Builder
export function DashboardBuilder() {
  Dashboard();
}

@ComponentV2
struct Dashboard {
  pathStack: NavPathStack = new NavPathStack();

  // 联系人状态变量
  @Local contactCount: number = 0;
  @Local contactList: ContactsItem[] = [];
  @Local recentContacts: ContactsItem[] = [];

  // 日程管理相关状态变量
  @Local todayEvents: CalendarEvent[] = [];
  @Local upcomingEvent: CalendarEvent | null = null;
  @Local firstTodayEvent: CalendarEvent | null = null;
  @Local completedEventsCount: number = 0;
  @Local pendingEventsCount: number = 0;
  @Local allEvents: CalendarEvent[] = [];

  // 步数统计相关状态变量
  @Local todaySteps: number = 0;
  @Local stepGoal: number = 8000;
  @Local goalMet: boolean = false;
  @Local weeklyAvgSteps: number = 0;

  private context: common.UIAbilityContext =
    this.getUIContext().getHostContext() as common.UIAbilityContext;
  // 添加ContactManager实例
  private contactManager: ContactManager = ContactManager.getInstance();
  // 添加DataManager实例
  private dataManager: DataManager = DataManager.getInstance(this.context);
  // 添加StepManager实例
  private stepManager: StepManager = StepManager.getInstance(this.context);

  async aboutToAppear() {
    // 加载联系人数据
    await this.loadContactData();
    // 加载日程数据
    await this.loadEventData();
    // 加载步数数据
    await this.loadStepData();
  }

  // 加载联系人数据
  async loadContactData() {
    try {
      // 初始化数据库
      if (this.context) {
        await this.contactManager.initDatabase(this.context);
      } else {
        console.log("联系人数据库初始化失败");
      }

      // 获取联系人列表
      this.contactList = await this.contactManager.getAllContacts();
      this.contactCount = this.contactList.length;

      // 获取最近的联系人（最多5个）
      this.recentContacts = this.contactList.slice(0, Math.min(1, this.contactList.length));
    } catch (error) {
      console.error('加载联系人数据失败:', error);
    }
  }

  // 加载日程数据
  async loadEventData() {
    try {
      // 获取今天的日程
      const todayResult = await this.dataManager.getTodayEvents();
      if (todayResult.success) {
        this.todayEvents = todayResult.data;

        // 找出下一个即将到来的活动（开始时间大于当前时间且未完成）
        const now = Date.now();
        const upcomingEvents = this.todayEvents
          .filter(event => !event.completed && event.startTime > now)
          .sort((a, b) => a.startTime - b.startTime);

        if (upcomingEvents.length > 0) {
          this.upcomingEvent = upcomingEvents[0];
        }

        // 计算今天第一个未完成的活动（即使已经开始）
        const firstTodayUncompleted = this.todayEvents
          .filter(event => !event.completed)
          .sort((a, b) => a.startTime - b.startTime)[0];

        this.firstTodayEvent = firstTodayUncompleted || null;
      }

      // 获取未完成的日程
      const pendingEvents = await this.dataManager.getPendingEvents();
      this.pendingEventsCount = pendingEvents.length;

      // 获取所有日程并计算已完成数量
      this.allEvents = await this.dataManager.getAllEvents();
      this.completedEventsCount = this.allEvents.filter(event => event.completed).length;

    } catch (error) {
      console.error('加载日程数据失败:', error);
    }
  }

  // 加载步数数据
  async loadStepData() {
    try {
      // 获取今天的步数记录
      const today = this.getCurrentDate();
      const stepResult = await this.stepManager.getStepRecordByDate(today);
      
      if (stepResult.success && stepResult.data) {
        const stepRecord = stepResult.data as StepRecord;
        this.todaySteps = stepRecord.steps;
        this.goalMet = stepRecord.steps >= this.stepGoal;
      } else {
        // 如果今天没有记录，保持默认值
        this.todaySteps = 0;
        this.goalMet = false;
      }
      
      // 获取最近7天的平均步数
      const stepStats = await this.stepManager.getStepStats(7);
      this.weeklyAvgSteps = stepStats.avgSteps;
      
    } catch (error) {
      console.error('加载步数数据失败:', error);
    }
  }

  // 获取当前日期字符串 (YYYY-MM-DD)
  getCurrentDate(): string {
    const now = new Date();
    const year = now.getFullYear();
    const month = String(now.getMonth() + 1).padStart(2, '0');
    const day = String(now.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }

  // 格式化时间显示
  formatEventTime(event: CalendarEvent): string {
    const startDate = new Date(event.startTime);
    const endDate = new Date(event.endTime);

    // 判断是否是今天、明天等
    const today = new Date();
    today.setHours(0, 0, 0, 0);

    const startDay = new Date(event.startTime);
    startDay.setHours(0, 0, 0, 0);

    const diffDays = Math.floor((startDay.getTime() - today.getTime()) / (1000 * 60 * 60 * 24));

    let dayText = '';
    if (diffDays === 0) {
      dayText = '今天';
    } else if (diffDays === 1) {
      dayText = '明天';
    } else if (diffDays === 2) {
      dayText = '后天';
    } else {
      dayText = `${startDate.getMonth() + 1}月${startDate.getDate()}日`;
    }

    // 格式化时间
    const formatTime = (date: Date): string => {
      const hours = date.getHours().toString().padStart(2, '0');
      const minutes = date.getMinutes().toString().padStart(2, '0');
      return `${hours}:${minutes}`;
    };

    if (event.allDay) {
      return `${dayText}, 全天`;
    } else {
      return `${dayText}, ${formatTime(startDate)}`;
    }
  }

  build() {
    NavDestination() {
      Column(){
        // Header
        Row(){
          Text('LifeTrack')
            .fontSize(30)
            .fontWeight(FontWeight.Bold)
            .width('100%')
            .fontColor(Color.White)
        }
        .width('100%')
        .height(100)
        .backgroundColor('#1F62A8')
        .margin({bottom: -30})
        .padding({left: 20, right: 20, bottom: 40})
        .expandSafeArea([SafeAreaType.SYSTEM],[SafeAreaEdge.TOP,SafeAreaEdge.BOTTOM])

        Column(){
          // Dashboard
          Column(){
            Row(){
              Text('联系人们')
                .width('100%')
                .fontSize(20)
                .fontWeight(FontWeight.Medium)
            }
            // 分隔线
            Divider()
              .strokeWidth(0.5)
              .color(Color.Gray)
              .margin({ left: 16, right: 16 })
              .margin({ top: 10, bottom: 10 })
            Row(){
              Column(){
                Text(`可联系人数: ${this.contactCount}`)
                  .width('100%')
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#1F62A8')

                // 显示一个示例联系人
                if (this.recentContacts.length > 0) {
                  Column({ space: 4 }) {
                    ForEach(this.recentContacts.slice(0, 3), (contact: ContactsItem, index: number) => {
                      Text(`可联系的人:  ${contact.name} - ${contact.phone}`)
                        .width('100%')
                        .fontSize(14)
                        .fontColor('#666666')
                        .maxLines(1)
                    }, (contact: ContactsItem) => contact.id.toString())
                  }
                  .margin({ top: 8 })
                  .width('100%')
                } else {
                  Text('暂无联系人')
                    .width('100%')
                    .fontSize(14)
                    .fontColor('#999999')
                    .margin({ top: 8 })
                }
              }
              .layoutWeight(1)
            }
          }
          .width('100%')
          .height(120)
          .backgroundColor('#FEFEFE')
          .border({radius: 12})
          .padding(16)
          .shadow({
            radius: 6,
            color: '#4D7CFF',
            offsetX: 0,
            offsetY: 2,
          })

          // Upcoming Events
          Column(){
            Row(){
              Text('即将到来的活动')
                .width('100%')
                .fontSize(20)
                .fontWeight(FontWeight.Medium)
            }
            // 分隔线
            Divider()
              .strokeWidth(0.5)
              .color(Color.Gray)
              .margin({ left: 16, right: 16 })
              .margin({ top: 10, bottom: 10 })
            Row(){
              Column(){
                if (this.upcomingEvent) {
                  Text(`下一项: ${this.upcomingEvent.title}`)
                    .width('100%')
                    .fontSize(16)
                    .fontWeight(FontWeight.Normal)
                    .maxLines(1)
                    .margin({bottom: 8})
                  Text(this.formatEventTime(this.upcomingEvent))
                    .width('100%')
                    .fontSize(14)
                    .fontWeight(FontWeight.Normal)
                    .fontColor('#666666')
                } else if (this.firstTodayEvent) {
                  Text(`下一项: ${this.firstTodayEvent.title}`)
                    .width('100%')
                    .fontSize(16)
                    .fontWeight(FontWeight.Normal)
                    .maxLines(1)
                    .margin({bottom: 8})
                  Text(this.formatEventTime(this.firstTodayEvent))
                    .width('100%')
                    .fontSize(14)
                    .fontWeight(FontWeight.Normal)
                    .fontColor('#666666')
                } else if (this.todayEvents.length > 0) {
                  // 今天有日程但都已完成
                  Text('今日日程已完成')
                    .width('100%')
                    .fontSize(16)
                    .fontWeight(FontWeight.Normal)
                    .fontColor('#999999')
                } else {
                  Text('暂无日程安排')
                    .width('100%')
                    .fontSize(16)
                    .fontWeight(FontWeight.Normal)
                    .fontColor('#999999')
                }
              }
              .layoutWeight(1)
            }
          }
          .width('100%')
          .height(120)
          .backgroundColor('#FEFEFE')
          .border({radius: 12})
          .margin({top: 20})
          .padding(16)
          .shadow({
            radius: 6,
            color: '#4D7CFF',
            offsetX: 0,
            offsetY: 2,
          })

          // Daily Status
          Column(){
            Row(){
              Text('日程状态')
                .width('100%')
                .fontSize(20)
                .fontWeight(FontWeight.Medium)
            }
            // 分隔线
            Divider()
              .strokeWidth(0.5)
              .color(Color.Gray)
              .margin({ left: 16, right: 16 })
              .margin({ top: 10, bottom: 10 })
            Row(){
              Column(){
                Text(`${this.completedEventsCount}项日程完成`)
                  .width('100%')
                  .fontSize(16)
                  .fontWeight(FontWeight.Normal)
              }
              .width('50%')
              Column(){
                Text(`${this.pendingEventsCount}项待办日程`)
                  .width('100%')
                  .fontSize(16)
                  .fontWeight(FontWeight.Normal)
              }
            }
          }
          .width('100%')
          .height(120)
          .backgroundColor('#FEFEFE')
          .border({radius: 12})
          .margin({top: 20})
          .padding(16)
          .shadow({
            radius: 6,
            color: '#4D7CFF',
            offsetX: 0,
            offsetY: 2,
          })

          // 步数统计
          Column(){
            Row(){
              Text('步数统计')
                .width('100%')
                .fontSize(20)
                .fontWeight(FontWeight.Medium)
            }
            // 分隔线
            Divider()
              .strokeWidth(0.5)
              .color(Color.Gray)
              .margin({ left: 16, right: 16 })
              .margin({ top: 10, bottom: 10 })
            Row(){
              Column(){
                Text(`${this.todaySteps.toLocaleString()} 步`)
                  .width('100%')
                  .fontSize(16)
                  .fontWeight(FontWeight.Normal)
                Text(this.goalMet ? '今日目标达成！' : `距目标还差 ${(this.stepGoal - this.todaySteps).toLocaleString()} 步`)
                  .width('100%')
                  .fontSize(12)
                  .fontColor(this.goalMet ? '#34C759' : '#FF9500')
                  .margin({top: 4})
              }
              .width('50%')
              Column(){
                Text(`${this.weeklyAvgSteps.toLocaleString()} 步`)
                  .width('100%')
                  .fontSize(16)
                  .fontWeight(FontWeight.Normal)
                Text('周平均步数')
                  .width('100%')
                  .fontSize(12)
                  .fontColor('#666666')
                  .margin({top: 4})
              }
            }
          }
          .width('100%')
          .height(120)
          .backgroundColor('#FEFEFE')
          .border({radius: 12})
          .margin({top: 20})
          .padding(16)
          .shadow({
            radius: 6,
            color: '#4D7CFF',
            offsetX: 0,
            offsetY: 2,
          })

          // 2 buttons
          Row(){
            Button('联系人管理')
              .backgroundColor('#1F62A8')
              .width('30%')
              .onClick(() => {
                this.pathStack.replacePathByName("Contact", null, false)
              })
            Blank().width('6%')
            Button('日历与日程')
              .backgroundColor('#34C759')
              .width('30%')
              .onClick(() => {
                this.pathStack.replacePathByName("Calendar", null, false)
              })
            Blank().width('6%')
            Button('步数记录')
              .backgroundColor('#FF6B6B')
              .width('30%')
              .onClick(() => {
                this.pathStack.replacePathByName("StepTracker", null, false)
              })
          }
          .width('100%')
          .height(50)
          .margin({top: 15, bottom: 15})

        }
        .width('100%')
        .padding(16)
      }
      .width('100%')
      .height('100%')
      .expandSafeArea([SafeAreaType.SYSTEM],[SafeAreaEdge.TOP,SafeAreaEdge.BOTTOM])
      .backgroundColor('#E3E7EC')
    }
    .hideBackButton(true)
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}