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

import { CalendarEvent } from '../model/EventModel';
import { EventInstance } from '../model/EventTypes';
import { DataManager } from './DataManager';
import common from '@ohos.app.ability.common';

// 跳转页面入口函数
@Builder
export function CalendarBuilder() {
  myCalendar();
}

// 日历日期接口
interface CalendarDate {
  date: Date
  day: string
  isCurrentMonth: boolean
  isToday: boolean
  isSelected: boolean
}

class CalendarUtils {
  // 时间戳转时间字符串
  static timestampToTimeString(timestamp: number): string {
    const date = new Date(timestamp);
    let hours = date.getHours();
    const minutes = date.getMinutes();
    const ampm = hours >= 12 ? 'PM' : 'AM';

    hours = hours % 12;
    hours = hours ? hours : 12; // 0点转为12

    const minutesStr = minutes < 10 ? '0' + minutes : minutes.toString();
    return `${hours}:${minutesStr} ${ampm}`;
  }

  // 时间戳转日期对象（只取日期部分）
  static timestampToDate(timestamp: number): Date {
    const date = new Date(timestamp);
    return new Date(date.getFullYear(), date.getMonth(), date.getDate());
  }

  // 时间字符串转分钟数（用于时间比较）
  static timeStringToMinutes(timeStr: string): number {
    const match = timeStr.match(/(\d+):(\d+)\s*(AM|PM)/i);
    if (!match) {
      return 0;
    }

    let hours = parseInt(match[1]);
    const minutes = parseInt(match[2]);
    const ampm = match[3].toUpperCase();

    if (ampm === 'PM' && hours !== 12) {
      hours += 12;
    }
    if (ampm === 'AM' && hours === 12) {
      hours = 0;
    }

    return hours * 60 + minutes;
  }

  // 判断两个时间戳是否在同一天
  static isSameDay(timestamp1: number, timestamp2: number): boolean {
    const date1 = new Date(timestamp1);
    const date2 = new Date(timestamp2);
    return date1.getDate() === date2.getDate() &&
      date1.getMonth() === date2.getMonth() &&
      date1.getFullYear() === date2.getFullYear();
  }

  // 格式化持续时间（如："9:00 AM - 5:00 PM"）
  static formatDuration(startTime: number, endTime: number, allDay: boolean): string {
    if (allDay) {
      return "全天";
    }

    const startStr = CalendarUtils.timestampToTimeString(startTime);
    const endStr = CalendarUtils.timestampToTimeString(endTime);
    return `${startStr} - ${endStr}`;
  }

  // 获取下一个可用时间戳
  static getNextAvailableTime(baseDate: Date, existingEvents: CalendarEvent[]): number {
    const baseTimestamp = baseDate.getTime();
    const dayStart = new Date(baseDate);
    dayStart.setHours(9, 0, 0, 0); // 从上午9点开始

    const existingTimes = existingEvents
      .filter(event => CalendarUtils.isSameDay(event.startTime, baseTimestamp))
      .map(event => {
        const eventDate = new Date(event.startTime);
        return eventDate.getHours() * 60 + eventDate.getMinutes();
      });

    let nextMinutes = 9 * 60; // 9:00 AM

    // 找到下一个30分钟的间隔
    while (existingTimes.includes(nextMinutes)) {
      nextMinutes += 30;
    }

    // 如果超过17:30（下午5:30），则从第二天9:00开始
    if (nextMinutes > 17 * 60 + 30) {
      dayStart.setDate(dayStart.getDate() + 1);
      dayStart.setHours(9, 0, 0, 0);
      nextMinutes = 9 * 60;
    }

    return dayStart.getTime() + nextMinutes * 60 * 1000;
  }
}

@ComponentV2
struct myCalendar {
  pathStack: NavPathStack = new NavPathStack();
  private context: common.UIAbilityContext =
    this.getUIContext().getHostContext() as common.UIAbilityContext;
  private dataManager: DataManager = DataManager.getInstance(this.context);
  @Local currentDate: Date = new Date()
  @Local selectedDate: Date = new Date()
  @Local events: CalendarEvent[] = []
  @Local eventInstances: EventInstance[] = [] // 新增：存储事件实例
  // 星期标题
  private weekDays: string[] = ['S', 'M', 'T', 'W', 'T', 'F', 'S']
  // 月份名称
  private monthNames: string[] = [
    'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
    'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'
  ]
  // Scroller控制器
  private eventListScroller: Scroller = new Scroller()

  // 获取当前显示的年份
  get currentYear(): number {
    return this.currentDate.getFullYear()
  }

  // 获取当前显示的月份
  get currentMonth(): number {
    return this.currentDate.getMonth()
  }

  // 获取当前显示的月份名称
  get currentMonthName(): string {
    return this.monthNames[this.currentMonth]
  }

  // 构建日期数组
  buildCalendarDates(): CalendarDate[] {
    const dates: CalendarDate[] = []
    const year = this.currentYear
    const month = this.currentMonth

    // 获取当月第一天是星期几（0-6，0=周日）
    const firstDayOfMonth = new Date(year, month, 1).getDay()

    // 获取上个月的最后几天
    const lastMonth = month === 0 ? 11 : month - 1
    const lastMonthYear = month === 0 ? year - 1 : year
    const daysInLastMonth = new Date(lastMonthYear, lastMonth + 1, 0).getDate()

    // 添加上个月的末尾日期
    for (let i = firstDayOfMonth - 1; i >= 0; i--) {
      const date = new Date(lastMonthYear, lastMonth, daysInLastMonth - i)
      dates.push({
        date: date,
        day: (daysInLastMonth - i).toString(),
        isCurrentMonth: false,
        isToday: this.isToday(date),
        isSelected: this.isSameDay(date, this.selectedDate)
      })
    }

    // 添加当前月的日期
    const daysInCurrentMonth = new Date(year, month + 1, 0).getDate()
    for (let i = 1; i <= daysInCurrentMonth; i++) {
      const date = new Date(year, month, i)
      dates.push({
        date: date,
        day: i.toString(),
        isCurrentMonth: true,
        isToday: this.isToday(date),
        isSelected: this.isSameDay(date, this.selectedDate)
      })
    }

    // 添加下个月的开始日期
    const remainingDays = 42 - dates.length
    const nextMonth = month === 11 ? 0 : month + 1
    const nextMonthYear = month === 11 ? year + 1 : year

    for (let i = 1; i <= remainingDays; i++) {
      const date = new Date(nextMonthYear, nextMonth, i)
      dates.push({
        date: date,
        day: i.toString(),
        isCurrentMonth: false,
        isToday: this.isToday(date),
        isSelected: this.isSameDay(date, this.selectedDate)
      })
    }

    return dates
  }

  // 检查是否是今天
  isToday(date: Date): boolean {
    const today = new Date()
    return date.getDate() === today.getDate() &&
      date.getMonth() === today.getMonth() &&
      date.getFullYear() === today.getFullYear()
  }

  // 检查是否是同一天
  isSameDay(date1: Date, date2: Date): boolean {
    return date1.getDate() === date2.getDate() &&
      date1.getMonth() === date2.getMonth() &&
      date1.getFullYear() === date2.getFullYear()
  }

  // 格式化日期为字符串
  formatDate(date: Date): string {
    return `${this.monthNames[date.getMonth()]} ${date.getDate()}, ${date.getFullYear()}`
  }

  // 获取选中日期的事件
  getEventsForSelectedDate(): CalendarEvent[] {
    const selectedDateStr = this.selectedDate.toDateString()
    const selectedTimestamp = this.selectedDate.getTime();

    // 1. 获取直接在该日期开始的事件
    const directEvents = this.events.filter(event => {
      const eventDate = CalendarUtils.timestampToDate(event.startTime);
      return eventDate.toDateString() === selectedDateStr;
    });

    // 2. 获取重复事件实例
    const repeatedEvents: CalendarEvent[] = [];

    for (const instance of this.eventInstances) {
      const instanceDate = CalendarUtils.timestampToDate(instance.instanceTime);
      if (instanceDate.toDateString() === selectedDateStr) {
        // 找到对应的原始事件
        const originalEvent = this.events.find(event => event.id === instance.originalId);
        if (originalEvent) {
          // 创建事件副本，使用实例的时间（手动复制所有属性）
          const eventCopy: CalendarEvent = {
            id: instance.id,
            title: originalEvent.title,
            description: originalEvent.description,
            startTime: instance.instanceTime,
            endTime: instance.instanceTime + (originalEvent.endTime - originalEvent.startTime),
            allDay: originalEvent.allDay,
            location: originalEvent.location,
            calendarId: originalEvent.calendarId,
            color: originalEvent.color,
            reminder: originalEvent.reminder,
            repeatRule: '', // 重复规则为空，因为这是具体实例
            completed: instance.completed,
            created: originalEvent.created,
            updated: originalEvent.updated
          };
          repeatedEvents.push(eventCopy);
        }
      }
    }

    // 3. 合并结果，按开始时间排序
    const allEvents = [...directEvents, ...repeatedEvents];
    return allEvents.sort((a, b) => a.startTime - b.startTime);

    // return this.events.filter(event => {
    //   const eventDate = CalendarUtils.timestampToDate(event.startTime);
    //   return eventDate.toDateString() === selectedDateStr;
    // }).sort((a, b) => a.startTime - b.startTime); // 按开始时间排序
  }

  // 初始化示例数据
  aboutToAppear() {
    this.initializeSampleEvents()
    // 从数据库加载事件数据
    this.loadEventsFromDatabase();
  }

  // 从数据库加载事件数据
  private async loadEventsFromDatabase() {
    try {
      // 获取所有事件
      const allEvents = await this.dataManager.getAllEvents();
      this.events = allEvents

      // 获取所有事件实例
      this.eventInstances = [];
      for (const event of allEvents) {
        if (event.repeatRule && event.repeatRule.trim() !== '') {
          const instances = await this.dataManager.getEventInstancesByOriginalId(event.id);
          this.eventInstances.push(...instances);
        }
      }

      console.log(`从数据库加载了 ${this.events.length} 个事件和 ${this.eventInstances.length} 个事件实例`);
    } catch (error) {
      console.error('加载事件数据失败:', error);
    }
  }

  // 初始化示例事件
  initializeSampleEvents() {
    const today = new Date()
    const now = today.getTime();
    const events: CalendarEvent[] = []

    this.events = events
    this.selectedDate = today
  }

  // 切换到上个月
  prevMonth() {
    this.currentDate = new Date(this.currentYear, this.currentMonth - 1, 1)
  }

  // 切换到下个月
  nextMonth() {
    this.currentDate = new Date(this.currentYear, this.currentMonth + 1, 1)
  }

  // 切换到今天
  goToToday() {
    const today = new Date()
    this.currentDate = today
    this.selectedDate = today
  }

  // 检查日期是否有事件
  hasEvents(date: Date): boolean {
    const dateStr = date.toDateString()

    // 检查直接事件
    const hasDirectEvents = this.events.some(event => {
      const eventDate = CalendarUtils.timestampToDate(event.startTime);
      return eventDate.toDateString() === dateStr;
    });

    if (hasDirectEvents) {
      return true;
    }

    // 检查重复事件实例
    return this.eventInstances.some(instance => {
      const instanceDate = CalendarUtils.timestampToDate(instance.instanceTime);
      return instanceDate.toDateString() === dateStr;
    });
  }

  // 获取事件指示点颜色
  getEventIndicators(date: Date): string[] {
    const dateStr = date.toDateString();
    const indicators: string[] = [];

    // 1. 收集直接事件的指示点
    const directEvents = this.events.filter(event => {
      const eventDate = CalendarUtils.timestampToDate(event.startTime);
      return eventDate.toDateString() === dateStr;
    });

    indicators.push(...directEvents.slice(0, 3).map(event => event.color));

    // 2. 如果直接事件不足3个，添加重复事件的指示点
    if (indicators.length < 3) {
      const repeatedEvents = this.eventInstances.filter(instance => {
        const instanceDate = CalendarUtils.timestampToDate(instance.instanceTime);
        return instanceDate.toDateString() === dateStr;
      }).slice(0, 3 - indicators.length);

      // 获取重复事件的颜色
      for (const instance of repeatedEvents) {
        const originalEvent = this.events.find(event => event.id === instance.originalId);
        if (originalEvent && !indicators.includes(originalEvent.color)) {
          indicators.push(originalEvent.color);
        }
      }
    }

    return indicators.slice(0, 3); // 最多返回3个
  }

  @Builder
  EventItemComponent(event: CalendarEvent) {
    Row({ space: 16 }) {
      // 左侧：颜色标记和复选框
      Column({ space: 4 }) {
        // 复选框
        Column() {
          if (event.completed) {
          }
        }
        .width(20)
        .height(20)
        .border({ width: 2, color: event.completed ? event.color : '#D1D5DB' })
        .borderRadius(4)
        .backgroundColor(event.completed ? event.color : 'transparent')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .onClick(() => {
          this.toggleEventStatus(event.id)
        })
      }
      .width(32)
      .height('100%')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)

      // 事件信息（主要区域）
      Column({ space: 4 }) {
        // 标题和状态
        Row({ space: 8 }) {
          Text(event.title)
            .fontSize(16)
            .fontColor(event.completed ? '#9CA3AF' : '#1F2937')
            .decoration({
              type: event.completed ? TextDecorationType.LineThrough : TextDecorationType.None
            })
            .layoutWeight(1)
        }
        .width('100%')

        // 时间信息
        Text(this.getEventTimeDetails(event))
          .fontSize(12)
          .fontColor('#6B7280')
          .width('100%')

        // 地点
        if (event.location) {
          Row({ space: 4 }) {
            Text(event.location)
              .fontSize(12)
              .fontColor('#6B7280')
              .maxLines(1)
          }
          .width('100%')
        }

        // 描述
        if (event.description) {
          Text(event.description)
            .fontSize(12)
            .fontColor('#9CA3AF')
            .maxLines(2)
            .margin({ top: 2 })
        }
      }
      .layoutWeight(1)
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Start)
      .onClick(() => {
        this.pathStack.replacePathByName('EventEdit', event, false);
      })

      // 右侧操作按钮
      Column({ space: 4 }) {
        // 更多操作按钮
        Button('更多')
          .width(60)
          .height(28)
          .fontSize(12)
          .fontColor('#3C82F6')
          .backgroundColor('#DBEAFE')
          .borderRadius(6)
          .margin({ bottom: 10 })
          .onClick(() => {
            this.pathStack.replacePathByName('EventEdit', event, false);
          })

        // 删除按钮
        Button('删除')
          .width(60)
          .height(28)
          .fontSize(12)
          .fontColor('#EF4444')
          .backgroundColor('#FEE2E2')
          .borderRadius(6)
          .onClick(() => {
            this.deleteEvent(event.id)
          })
      }
      .width(80)
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.End)
    }
    .width('100%')
    .padding({
      top: 16,
      bottom: 16,
      left: 12,
      right: 12
    })
    .border({ width: { bottom: 1 }, color: '#F3F4F6' })
    .backgroundColor('#FFFFFF')
    .borderRadius(8)
    .margin({ bottom: 8 })
    .alignItems(VerticalAlign.Center)
  }

  // 切换事件状态
  async toggleEventStatus(eventId: string) {
    try {
      // 判断是主事件还是实例
      const isInstance = eventId.includes('_'); // 实例ID格式：originalId_timestamp_index

      if (isInstance) {
        // 处理实例
        const instance = this.eventInstances.find(inst => inst.id === eventId);
        if (!instance) {
          console.warn('未找到要更新的事件实例:', eventId);
          return;
        }

        const newCompleted = !instance.completed;

        // 更新实例完成状态
        const success = await this.dataManager.updateEventInstanceCompletion(eventId, newCompleted);

        if (success) {
          // 更新本地实例列表（手动复制）
          this.eventInstances = this.eventInstances.map(inst => {
            if (inst.id === eventId) {
              // 手动创建新对象
              return {
                id: inst.id,
                originalId: inst.originalId,
                title: inst.title,
                description: inst.description,
                instanceTime: inst.instanceTime,
                completed: newCompleted
              };
            }
            return inst;
          });
        } else {
          console.error('更新事件实例状态失败');
        }
      } else {
        // 处理主事件（原有逻辑）
        const eventToUpdate = this.events.find(event => event.id === eventId);
        if (!eventToUpdate) {
          console.warn('未找到要更新的事件:', eventId);
          return;
        }

        const newCompleted = !eventToUpdate.completed;

        // 调用DataManager更新数据库中的事件状态
        const result = await this.dataManager.toggleEventCompletion(eventId, newCompleted);

        if (result.success) {
          // 更新本地事件列表（手动复制）
          this.events = this.events.map(event => {
            if (event.id === eventId) {
              // 手动创建新对象
              return {
                id: event.id,
                title: event.title,
                description: event.description,
                startTime: event.startTime,
                endTime: event.endTime,
                allDay: event.allDay,
                location: event.location,
                calendarId: event.calendarId,
                color: event.color,
                reminder: event.reminder,
                repeatRule: event.repeatRule,
                completed: newCompleted,
                created: event.created,
                updated: Date.now() // 更新修改时间
              };
            }
            return event;
          });
        } else {
          console.error('更新事件状态失败:', result.error);
        }
      }
    } catch (error) {
      console.error('切换事件状态时发生错误:', error);
    }
  }

  // 删除事件
  async deleteEvent(eventId: string) {
    try {
      // 判断是主事件还是实例
      const isInstance = eventId.includes('_'); // 实例ID格式：originalId_timestamp_index

      if (isInstance) {
        // 删除实例
        const success = await this.dataManager.deleteEventInstance(eventId);

        if (success) {
          // 从本地列表删除实例
          this.eventInstances = this.eventInstances.filter(inst => inst.id !== eventId);
          console.log('事件实例删除成功');
        } else {
          console.error('删除事件实例失败');
        }
      } else {
        // 删除主事件（原有逻辑）
        console.log('正在删除事件:', eventId);

        // 调用DataManager删除数据库中的事件
        const result = await this.dataManager.deleteEvent(eventId);

        if (result.success) {
          console.log('事件删除成功');

          // 从本地列表删除主事件
          this.events = this.events.filter(event => event.id !== eventId);

          // 同时删除相关的所有实例
          this.eventInstances = this.eventInstances.filter(inst => inst.originalId !== eventId);
        } else {
          console.error('删除事件失败:', result.error);
        }
      }
    } catch (error) {
      console.error('删除事件时发生错误:', error);
    }
  }

  // 显示添加事件对话框
  showAddEventDialog() {
    // 获取下一个可用时间
    const nextStartTime = CalendarUtils.getNextAvailableTime(
      this.selectedDate,
      this.events
    );

    // 默认持续1小时
    const nextEndTime = nextStartTime + 60 * 60 * 1000;

    const newEvent: CalendarEvent = {
      id: '0',
      title: '新事件',
      description: '',
      startTime: nextStartTime,
      endTime: nextEndTime,
      allDay: false,
      location: '',
      calendarId: 'default',
      color: '#3B82F6', // 默认蓝色
      reminder: 0,
      repeatRule: '',
      completed: false,
      created: Date.now(),
      updated: Date.now()
    };

    this.events = [...this.events, newEvent];

    setTimeout(() => {
      this.eventListScroller.scrollEdge(Edge.Bottom);
      setTimeout(() => {
        this.pathStack.replacePathByName('EventEdit', newEvent, false);
      }, 300); // 给滚动一点时间
    }, 100);
  }

  getEventDisplayTime(event: CalendarEvent): string {
    if (event.allDay) {
      return "全天";
    }
    return CalendarUtils.formatDuration(event.startTime, event.endTime, event.allDay);
  }

  getEventTimeDetails(event: CalendarEvent): string {
    const startDate = new Date(event.startTime);
    const endDate = new Date(event.endTime);

    if (event.allDay) {
      return "全天事件";
    }

    const startStr = CalendarUtils.timestampToTimeString(event.startTime);
    const endStr = CalendarUtils.timestampToTimeString(event.endTime);

    // 如果跨天，显示日期
    if (startDate.getDate() !== endDate.getDate()) {
      return `${startStr} - ${endDate.getMonth() + 1}/${endDate.getDate()} ${endStr}`;
    }

    return `${startStr} - ${endStr}`;
  }

  build() {
    NavDestination() {
      Column({ space: 0 }) {
        // 日历标题部分
        Row({ space: 0 }) {
          // 返回按钮
          Button('返回')
            .fontSize(16)
            .fontColor('#007AFF')
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              // 返回逻辑
              this.pathStack.pushPathByName("Dashboard", null, false)
            })
          Text('日历')
            .fontSize(18)
            .fontWeight(FontWeight.Medium)
            .layoutWeight(1)
            .textAlign(TextAlign.Center)
          Blank()
            .width(54)
        }
        .padding({
          left: 6,
          right: 6,
          top: 10,
          bottom: 10
        })
        .width('100%')
        .backgroundColor(Color.White)

        Column({ space: 5 }) {
          // 月份导航
          Row({ space: 5 }) {
            // 上个月按钮
            Button('<', { type: ButtonType.Normal })
              .width(40)
              .height(40)
              .fontSize(20)
              .fontColor('#3B82F6')
              .backgroundColor('#EFF6FF')
              .borderRadius(5)
              .onClick(() => {
                this.prevMonth()
              })

            // 当前年月显示
            Column() {
              Text(`${this.currentMonthName} ${this.currentYear}`)
                .fontSize(20)
                .fontWeight(FontWeight.Medium)
                .fontColor('#1F2937')
            }
            .layoutWeight(1)
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)

            // 下个月按钮
            Button('>', { type: ButtonType.Normal })
              .width(40)
              .height(40)
              .fontSize(20)
              .fontColor('#3B82F6')
              .backgroundColor('#EFF6FF')
              .borderRadius(5)
              .onClick(() => {
                this.nextMonth()
              })

            // 今天按钮
            Button('今天', { type: ButtonType.Normal })
              .width(80)
              .height(40)
              .fontSize(14)
              .fontColor('#FFFFFF')
              .backgroundColor('#3B82F6')
              .borderRadius(8)
              .margin({ left: 8 })
              .onClick(() => {
                this.goToToday()
              })
          }
          .width('100%')
          .padding({ left: 16, right: 16 })
          .margin({ bottom: 8 })
        }
        .width('100%')
        .alignItems(HorizontalAlign.Center)


        // 星期标题行
        Row() {
          ForEach(this.weekDays, (day: string) => {
            Text(day)
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .fontColor('#6B7280')
              .width('14.28%')
              .textAlign(TextAlign.Center)
              .padding({ top: 12, bottom: 12 })
          })
        }
        .width('100%')
        .padding({ left: 16, right: 16 })
        .border({ width: { bottom: 1 }, color: '#E5E7EB' })
        .margin({ top: 8 })

        // 日期网格
        Grid() {
          ForEach(this.buildCalendarDates(), (calendarDate: CalendarDate, index: number) => {
            GridItem() {
              Column() {
                if (calendarDate.day !== '') {
                  // 日期显示
                  Text(calendarDate.day)
                    .fontSize(16)
                    .fontWeight(calendarDate.isToday ? FontWeight.Bold : FontWeight.Normal)
                    .fontColor(calendarDate.isCurrentMonth ?
                      (calendarDate.isSelected || calendarDate.isToday ? '#FFFFFF' : '#1F2937') :
                      (calendarDate.isSelected || calendarDate.isToday ? '#FFFFFF' : '#9CA3AF'))
                    .margin({ top: 5 })

                  // 事件指示点
                  if (this.hasEvents(calendarDate.date)) {
                    Row({ space: 1 }) {
                      ForEach(this.getEventIndicators(calendarDate.date), (color: string) => {
                        Column()
                          .width(4)
                          .height(4)
                          .backgroundColor(color)
                          .borderRadius(2)
                      })
                    }
                    .margin({ top: 2 })
                  }
                }
              }
              .width('100%')
              .height(45)
              .backgroundColor(calendarDate.isSelected ? '#3B82F6' :
                calendarDate.isToday ? '#DBEAFE' : 'transparent')
              .borderRadius(calendarDate.isSelected || calendarDate.isToday ? 10 : 0)
              .justifyContent(FlexAlign.Start)
              .alignItems(HorizontalAlign.Center)
            }
            .width('100%')
            .onClick(() => {
              this.selectedDate = calendarDate.date
            })
          })
        }
        .height(225)
        .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr')
        .rowsTemplate('1fr 1fr 1fr 1fr 1fr')
        .columnsGap(0)
        .rowsGap(0)
        .padding({ left: 16, right: 16 })
        .margin({ top: 5, bottom: 5 })

        // 分隔线
        Divider()
          .strokeWidth(0.5)
          .color('#E5E7EB')
          .margin({ left: 24, right: 24 })

        // 事件列表标题
        Text(`${this.formatDate(this.selectedDate)} (${this.getEventsForSelectedDate().length}个事件)`)
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#1F2937')
          .margin({ top: 20, left: 24, right: 24 })
          .alignSelf(ItemAlign.Start)

        Scroll(this.eventListScroller) {
          // 事件列表
          Column() {
            if (this.getEventsForSelectedDate().length > 0) {
              ForEach(this.getEventsForSelectedDate(), (event: CalendarEvent) => {
                this.EventItemComponent(event)
              })
            } else {
              Column()
                .width('100%')
                .height(100)
                .justifyContent(FlexAlign.Center)
                .alignItems(HorizontalAlign.Center)
                .margin({ top: 20 })

              Text('今天没有事件')
                .fontSize(16)
                .fontColor('#9CA3AF')
            }
          }
          .width('100%')
          .padding({ left: 24, right: 24 })
        }
        .width('100%')
        // 计算事件列表区域的高度：总高度减去日历部分的高度
        .layoutWeight(1) // 使用layoutWeight占据剩余空间
        .scrollable(ScrollDirection.Vertical)
        .scrollBar(BarState.Off) // 可选：隐藏滚动条
        .margin({ top: 20 })

        // 添加事件按钮
        Button('+ 添加事件', { type: ButtonType.Normal })
          .width(200)
          .height(48)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#FFFFFF')
          .backgroundColor('#3B82F6')
          .borderRadius(8)
          .margin({ top: 30, bottom: 20 })
          .onClick(() => {
            this.showAddEventDialog()
          })
      }
      .width('100%')
      .backgroundColor('#FFFFFF')
    }
    .hideBackButton(true)
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}