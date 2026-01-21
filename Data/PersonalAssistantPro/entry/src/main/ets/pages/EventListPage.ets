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
// entry/src/main/ets/pages/EventListPage.ets

import { Event, EventModel } from '../model/EventModel';
import { Logger } from '../common/utils/Logger';
import { MonthView } from '../view/calendar/MonthView';
import { WeekView } from '../view/calendar/WeekView';
import { CalendarViewModel, TaskStatus } from '../viewmodel/CalendarViewModel';

const logger = new Logger('EventListPage');

@Component
export struct EventListPage {
  @State events: Array<Event> = [];
  @State keyword: string = '';
  // 默认选中今天
  @State selectedTimestamp: number = new Date().getTime();

  @State taskMap: Map<string, TaskStatus> = new Map();

  @State viewMode: string = 'month';

  @Consume('pageStack') pageStack: NavPathStack;

  // 监听全局刷新信号
  @StorageProp('GlobalRefreshSignal') @Watch('onGlobalRefresh') signal: number = 0;

  onGlobalRefresh() {
    logger.info('Received GlobalRefreshSignal, refreshing list...');
    this.refreshAllData();
  }

  // 通知跳转相关状态
  @StorageProp('notificationJumpTarget') @Watch('onNotificationJump') jumpTarget: string = '';
  @StorageProp('notificationJumpId') jumpId: number = -1;

  // 监听列表刷新请求
  @StorageProp('refreshEventList') @Watch('onRefreshChange') refreshSignal: number = 0;

  private calendarViewModel = new CalendarViewModel();

  onPageShow() {
    this.refreshAllData();
  }

  aboutToAppear() {
    // 1. 使用 setTimeout 延迟检查，确保能够捕获冷启动时的通知点击信号
    setTimeout(() => {
      this.checkColdStartJump();
    }, 100);

    // 2. 加载页面数据
    this.refreshAllData();
  }

  // 独立出来的冷启动跳转检查方法
  private checkColdStartJump() {
    // 绕过 @StorageProp，直接查 AppStorage 获取最真实的数据
    const storedTarget = AppStorage.get<string>('notificationJumpTarget');
    const storedId = AppStorage.get<number>('notificationJumpId');

    logger.info(`[ColdStart Check] Target: ${storedTarget}, ID: ${storedId}`);

    if (storedTarget === 'EventDetailPage' && storedId && storedId > 0) {
      logger.info('🚀 DETECTED PENDING JUMP! Triggering now...');

      // 强制同步本地状态，触发 Watch 回调
      this.jumpTarget = storedTarget;
      this.jumpId = storedId;

      // 手动执行跳转逻辑
      this.onNotificationJump();
    } else {
      logger.info('No pending jump found during cold start.');
    }
  }

  // 跳转响应逻辑
  async onNotificationJump() {
    logger.info(`[Jump Logic] Processing jump... Target: ${this.jumpTarget}, ID: ${this.jumpId}`);

    if (this.jumpTarget === 'EventDetailPage') {
      // 1. 消费信号 (清空，防止重复跳转)
      AppStorage.setOrCreate('notificationJumpTarget', '');

      // 2. 检查 ID 并查询数据库
      if (this.jumpId > 0) {
        logger.info(`[Jump Logic] Querying DB for Event ID: ${this.jumpId}`);
        const event = await EventModel.queryById(this.jumpId);

        if (event) {
          logger.info('[Jump Logic] Event found! Pushing to stack.');
          this.pageStack.pushPath({ name: 'EventDetailPage', param: event });
        } else {
          logger.error('[Jump Logic] Failed: Event ID not found in DB.');
        }
      } else {
        logger.error(`[Jump Logic] Failed: Invalid ID ${this.jumpId}`);
      }
    }
  }

  onRefreshChange() {
    this.refreshAllData();
  }

  async refreshAllData() {
    await this.loadMonthTaskDots();
    await this.loadEventsForList();
  }

  // 加载日历上的任务小圆点
  async loadMonthTaskDots() {
    try {
      const date = new Date(this.selectedTimestamp);
      const year = date.getFullYear();
      const month = date.getMonth() + 1;

      const monthEvents = await EventModel.queryByMonth(year, month);

      this.taskMap = this.calendarViewModel.processEventsToTaskMap(monthEvents);
    } catch (err) {
      logger.error('Load calendar dots failed', JSON.stringify(err));
    }
  }

  // 加载当前选中日期的事件列表
  async loadEventsForList() {
    try {
      // 如果有搜索关键词，优先显示搜索结果
      if (this.keyword && this.keyword.trim().length > 0) {
        this.events = await EventModel.search(this.keyword);
        return;
      }

      // 否则显示当天的日程
      const startOfDay = new Date(this.selectedTimestamp);
      startOfDay.setHours(0, 0, 0, 0);

      const endOfDay = new Date(this.selectedTimestamp);
      endOfDay.setHours(23, 59, 59, 999);

      this.events = await EventModel.queryRange(startOfDay.getTime(), endOfDay.getTime());

    } catch (err) {
      logger.error('Load events list failed', JSON.stringify(err));
      this.events = [];
    }
  }

  // 删除事件
  async deleteEvent(index: number, id: number) {
    const success = await EventModel.deleteById(id);
    if (success) {
      this.events.splice(index, 1);
      this.refreshAllData();
      logger.info(`Deleted event id: ${id}`);
    }
  }

  // 顶部标题栏 + 视图切换按钮
  @Builder
  private CustomHeader(): void {
    Row() {
      Text($r('app.string.event_title'))
        .fontSize(24)
        .fontWeight(FontWeight.Bold)

      Blank()

      // 月视图/周视图切换
      Row() {
        Text('月')
          .fontSize(14)
          .fontColor(this.viewMode === 'month' ? '#FFFFFF' : '#007DFF')
          .backgroundColor(this.viewMode === 'month' ? '#007DFF' : Color.Transparent)
          .padding({ left: 12, right: 12, top: 6, bottom: 6 })
          .borderRadius(14)
          .onClick(() => this.viewMode = 'month')

        Text('周')
          .fontSize(14)
          .fontColor(this.viewMode === 'week' ? '#FFFFFF' : '#007DFF')
          .backgroundColor(this.viewMode === 'week' ? '#007DFF' : Color.Transparent)
          .padding({ left: 12, right: 12, top: 6, bottom: 6 })
          .borderRadius(14)
          .onClick(() => this.viewMode = 'week')
      }
      .backgroundColor('#E5F1FF')
      .borderRadius(16)
      .margin({ right: 16 })

      // 添加按钮
      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('+')
          .fontSize(24)
          .fontColor(Color.Black)
      }
      .width(40)
      .height(40)
      .backgroundColor(Color.Transparent)
      .onClick(() => {
        this.pageStack.pushPath({ name: 'EventEditPage' });
      })
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .alignItems(VerticalAlign.Center)
  }

  // 搜索栏
  @Builder
  private SearchBar(): void {
    Row() {
      Search({ value: this.keyword, placeholder: '搜索日程标题、备注...' })
        .width('100%')
        .height(40)
        .backgroundColor(Color.White)
        .placeholderColor(Color.Gray)
        .placeholderFont({ size: 14 })
        .textFont({ size: 14 })
        .onChange((value: string) => {
          this.keyword = value;
          this.loadEventsForList();
        })
        .onSubmit((value: string) => {
          this.keyword = value;
          this.loadEventsForList();
        })
    }
    .width('100%')
    .padding({ left: 16, right: 16, bottom: 8 })
  }

  // 侧滑删除按钮
  @Builder
  DeleteAction(index: number, id: number) {
    Button() {
      Text($r('app.string.btn_delete'))
        .fontColor(Color.White)
        .fontSize(14)
    }
    .width(80)
    .height('100%')
    .backgroundColor(Color.Red)
    .onClick(() => {
      this.deleteEvent(index, id);
    })
  }

  // 时间格式化工具
  private formatTime(timestamp: number): string {
    const date = new Date(timestamp);
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    return `${hours}:${minutes}`;
  }

  // 单个日程列表项
  @Builder
  EventItem(item: Event, index: number) {
    ListItem() {
      Row() {
        // 左侧装饰条 (蓝色)
        Column()
          .width(4)
          .height(40)
          .backgroundColor('#007DFF')
          .borderRadius(2)
          .margin({ right: 12 })

        // 中间：标题与时间
        Column() {
          Text(item.title)
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .fontColor(Color.Black)
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Text(item.isAllDay ? $r('app.string.label_all_day') : `${this.formatTime(item.startTime)} - ${this.formatTime(item.endTime)}`)
            .fontSize(14)
            .fontColor(Color.Gray)
            .margin({ top: 4 })
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)

        // 右侧：状态图标区 (提醒 + 全天)
        Row() {
          // 🔔 提醒图标：如果有 reminderId，显示小铃铛
          if (item.reminderId > -1) {
            Image($r('app.media.ic_public_reminder'))
              .width(16)
              .height(16)
              .fillColor('#FF9800') // 设为橙色更醒目，若要灰色可设为 #999999 或删除此行
              .margin({ right: 6 })
          }

          // All Day 标签
          if (item.isAllDay) {
            Text('All Day')
              .fontSize(12)
              .fontColor('#007DFF')
              .backgroundColor('#E5F1FF')
              .padding({ left: 8, right: 8, top: 2, bottom: 2 })
              .borderRadius(10)
          }
        }
        .alignItems(VerticalAlign.Center)
      }
      .width('100%')
      .padding(12)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .margin({ bottom: 10 })
    .onClick(() => {
      this.pageStack.pushPath({ name: 'EventDetailPage', param: item });
    })
    .swipeAction({
      end: this.DeleteAction(index, item.id)
    })
  }

  build() {
    Column() {
      this.CustomHeader()

      // 如果不在搜索模式，显示日历视图
      if (this.keyword.length === 0) {
        Column() {
          if (this.viewMode === 'month') {
            MonthView({
              selectedTimestamp: $selectedTimestamp,
              taskMap: this.taskMap,
              onDateSelected: (ts: number) => {
                this.selectedTimestamp = ts;
                this.loadEventsForList();
              }
            })
          } else {
            WeekView({
              selectedTimestamp: $selectedTimestamp,
              taskMap: this.taskMap,
              onDateSelected: (ts) => {
                this.loadEventsForList();
              }
            })
          }
        }
        .margin({ bottom: 12 })
      } else {
        // 搜索模式显示搜索栏
        this.SearchBar()
      }

      // 日程列表区域
      GridRow({
        columns: { sm: 4, md: 8, lg: 12 },
        gutter: { x: 12, y: 12 }
      }) {
        GridCol({
          span: { sm: 4, md: 6, lg: 8 },
          offset: { sm: 0, md: 1, lg: 2 }
        }) {
          Column() {
            if (this.events.length === 0) {
              // 空状态页
              Column() {
                Image($r('app.media.app_icon'))
                  .width(64)
                  .height(64)
                  .opacity(0.3)
                  .grayscale(1)
                  .margin({ bottom: 10 })

                if (this.keyword.length > 0) {
                  Text('无搜索结果').fontSize(16).fontColor(Color.Gray)
                } else {
                  Text('今天没有安排').fontSize(16).fontColor(Color.Black)
                  Text('去休息一下吧').fontSize(14).fontColor(Color.Gray).margin({ top: 4 })
                }
              }
              .justifyContent(FlexAlign.Center)
              .height('100%')
              .width('100%')
            } else {
              // 列表渲染
              List() {
                ForEach(this.events, (item: Event, index: number) => {
                  this.EventItem(item, index)
                }, (item: Event) => `${item.id}_${item.startTime}`)
              }
              .width('100%')
              .layoutWeight(1)
              .padding({ left: 16, right: 16, top: 10 })
            }
          }
          .width('100%')
          .layoutWeight(1)
        }
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}