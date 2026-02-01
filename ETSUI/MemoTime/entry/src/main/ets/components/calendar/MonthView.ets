/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * 月视图组件
 */
import { formatDate, getMonthViewDates, getWeekDayName, isSameDay, isToday } from '../../common/utils/DateUtils'
import { Schedule } from '../../model/Schedule'

@Component
export struct MonthView {
  @Prop @Watch('onMonthChange') year: number = new Date().getFullYear()
  @Prop @Watch('onMonthChange') month: number = new Date().getMonth()
  @Prop @Watch('onDataChange') selectedDate: Date = new Date()
  @Prop @Watch('onDataChange') schedules: Schedule[] = []
  @Prop startFromMonday: boolean = false
  onDateSelect: (date: Date) => void = () => {
  }
  onScheduleClick: (schedule: Schedule) => void = () => {
  }
  @State dates: Date[] = []
  @State weekHeaders: string[] = []
  @State refreshKey: number = 0

  aboutToAppear(): void {
    this.buildWeekHeaders()
    this.buildDates()
  }

  /**
   * 月份变化时重新构建日期
   */
  onMonthChange(): void {
    this.buildDates()
    this.refreshKey++
  }

  onDataChange(): void {
    // 强制刷新视图
    this.refreshKey++
  }

  /**
   * 构建星期表头
   */
  private buildWeekHeaders(): void {
    const days = this.startFromMonday
      ? [1, 2, 3, 4, 5, 6, 0]
      : [0, 1, 2, 3, 4, 5, 6]
    this.weekHeaders = days.map((d: number) => getWeekDayName(d))
  }

  /**
   * 构建日期数组
   */
  private buildDates(): void {
    this.dates = getMonthViewDates(this.year, this.month, this.startFromMonday)
  }

  /**
   * 判断日期是否选中
   */
  private isDateSelected(date: Date): boolean {
    return isSameDay(date, this.selectedDate)
  }

  /**
   * 判断日期是否是当前月
   */
  private isCurrentMonth(date: Date): boolean {
    return date.getMonth() === this.month
  }

  /**
   * 获取日期的日程
   */
  private getSchedulesForDate(date: Date): Schedule[] {
    return this.schedules.filter((s: Schedule) => s.isOnDate(date))
  }

  /**
   * 获取日期文字颜色
   */
  private getDayTextColor(date: Date): ResourceColor {
    if (this.isDateSelected(date)) {
      return '#FFFFFF'
    }
    if (!this.isCurrentMonth(date)) {
      return '#CCCCCC'
    }
    if (isToday(date)) {
      return '#4ECDC4'
    }
    // 周末颜色
    const dayOfWeek = date.getDay()
    if (dayOfWeek === 0 || dayOfWeek === 6) {
      return '#FF6B6B'
    }
    return '#333333'
  }

  /**
   * 获取日期单元格背景色
   */
  private getDayCellBackground(date: Date): ResourceColor {
    if (this.isDateSelected(date)) {
      return '#4ECDC4'
    }
    if (isToday(date)) {
      return '#E8F8F5'
    }
    return Color.Transparent
  }

  @Builder
  WeekHeader() {
    Row() {
      ForEach(this.weekHeaders, (day: string, index: number) => {
        Text(day)
          .fontSize(14)
          .fontWeight(FontWeight.Medium)
          .fontColor(index === 0 || index === 6 ? '#FF6B6B' : '#666666')
          .width('14.28%')
          .textAlign(TextAlign.Center)
          .padding({ top: 8, bottom: 8 })
      }, (day: string, index: number) => `${day}-${index}`)
    }
    .width('100%')
    .backgroundColor('#F8F9FA')
    .borderRadius({ topLeft: 12, topRight: 12 })
  }

  @Builder
  DayCell(date: Date) {
    Column() {
      // 日期数字
      Text(date.getDate().toString())
        .fontSize(16)
        .fontWeight(isToday(date) ? FontWeight.Bold : FontWeight.Normal)
        .fontColor(this.getDayTextColor(date))

      // 日程指示点
      Row({ space: 2 }) {
        ForEach(this.getSchedulesForDate(date).slice(0, 3), (schedule: Schedule) => {
          Circle()
            .width(6)
            .height(6)
            .fill(schedule.color)
        }, (schedule: Schedule) => schedule.id)
      }
      .margin({ top: 4 })
      .height(8)
    }
    .width('14.28%')
    .height(60)
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .backgroundColor(this.getDayCellBackground(date))
    .borderRadius(8)
    .onClick(() => {
      this.onDateSelect(date)
    })
  }

  build() {
    Column() {
      // 星期表头
      this.WeekHeader()

      // 日期网格 - 使用 refreshKey 强制刷新
      Flex({ wrap: FlexWrap.Wrap }) {
        ForEach(this.dates, (date: Date, index: number) => {
          this.DayCell(date)
        }, (date: Date, index: number) => `${formatDate(date)}-${this.refreshKey}`)
      }
      .width('100%')
      .padding(8)
    }
    .width('100%')
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .shadow({
      radius: 8,
      color: 'rgba(0, 0, 0, 0.08)',
      offsetX: 0,
      offsetY: 2
    })
  }
}
