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
//  [修复] 引入 TaskStatus 类型
import { CalendarViewModel, TaskStatus } from '../viewmodel/CalendarViewModel';

const logger = new Logger('EventListPage');

@Component
export struct EventListPage {
  @State events: Array<Event> = [];
  @State keyword: string = '';
  // 默认选中今天
  @State selectedTimestamp: number = new Date().getTime();

  //  [修复] 类型改为 Map<string, TaskStatus>
  @State taskMap: Map<string, TaskStatus> = new Map();

  @State viewMode: string = 'month';

  @Consume('pageStack') pageStack: NavPathStack;
  // 同样监听这个强刷新信号
  @StorageProp('GlobalRefreshSignal') @Watch('onGlobalRefresh') signal: number = 0;

  onGlobalRefresh() {
    logger.info('Received GlobalRefreshSignal, refreshing list...');
    this.refreshAllData();
  }
  @StorageProp('notificationJumpTarget') @Watch('onNotificationJump') jumpTarget: string = '';
  @StorageProp('notificationJumpId') jumpId: number = -1;
  @StorageProp('refreshEventList') @Watch('onRefreshChange') refreshSignal: number = 0;

  private calendarViewModel = new CalendarViewModel();
  onPageShow() {
    this.refreshAllData();
  }
  aboutToAppear() {
    // 1. ✅ [核心修复] 使用 setTimeout 延迟检查，确保能够捕获冷启动信号
    // 将检查放到事件队列的末尾，避开 UI 初始化竞争
    setTimeout(() => {
      this.checkColdStartJump();
    }, 100);

    // 2. 加载数据
    this.refreshAllData();
  }

  // ✅ 独立出来的检查方法
  private checkColdStartJump() {
    // 绕过 @StorageProp，直接查 AppStorage (最真实的数据)
    const storedTarget = AppStorage.get<string>('notificationJumpTarget');
    const storedId = AppStorage.get<number>('notificationJumpId');

    logger.info(`[ColdStart Check] Target: ${storedTarget}, ID: ${storedId}`);

    if (storedTarget === 'EventDetailPage' && storedId && storedId > 0) {
      logger.info('🚀 DETECTED PENDING JUMP! Triggering now...');

      // 强制同步本地状态
      this.jumpTarget = storedTarget;
      this.jumpId = storedId;

      // 执行跳转
      this.onNotificationJump();
    } else {
      logger.info('No pending jump found during cold start.');
    }
  }
  // ✅ 跳转响应逻辑
  async onNotificationJump() {
    logger.info(`[Jump Logic] Processing jump... Target: ${this.jumpTarget}, ID: ${this.jumpId}`);

    if (this.jumpTarget === 'EventDetailPage') {
      // 1. 消费信号 (清空)
      AppStorage.setOrCreate('notificationJumpTarget', '');

      // 2. 检查 ID
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

  async loadMonthTaskDots() {
    try {
      const date = new Date(this.selectedTimestamp);
      const year = date.getFullYear();
      const month = date.getMonth() + 1;

      const monthEvents = await EventModel.queryByMonth(year, month);

      //  这里的类型现在匹配了
      this.taskMap = this.calendarViewModel.processEventsToTaskMap(monthEvents);
    } catch (err) {
      logger.error('Load calendar dots failed', JSON.stringify(err));
    }
  }

  async loadEventsForList() {
    try {
      if (this.keyword && this.keyword.trim().length > 0) {
        this.events = await EventModel.search(this.keyword);
        return;
      }

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

  async deleteEvent(index: number, id: number) {
    const success = await EventModel.deleteById(id);
    if (success) {
      this.events.splice(index, 1);
      this.refreshAllData();
      logger.info(`Deleted event id: ${id}`);
    }
  }

  @Builder
  private CustomHeader(): void {
    Row() {
      Text($r('app.string.event_title'))
        .fontSize(24)
        .fontWeight(FontWeight.Bold)

      Blank()

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

  private formatTime(timestamp: number): string {
    const date = new Date(timestamp);
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    return `${hours}:${minutes}`;
  }

  @Builder
  EventItem(item: Event, index: number) {
    ListItem() {
      Row() {
        Column()
          .width(4)
          .height(40)
          .backgroundColor('#007DFF')
          .borderRadius(2)
          .margin({ right: 12 })

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

        if (item.isAllDay) {
          Text('All Day')
            .fontSize(12)
            .fontColor('#007DFF')
            .backgroundColor('#E5F1FF')
            .padding({ left: 8, right: 8, top: 2, bottom: 2 })
            .borderRadius(10)
        }
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
        this.SearchBar()
      }

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