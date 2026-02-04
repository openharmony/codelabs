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
 * 年视图组件
 */
import {
  getDaysInMonth, getFirstDayOfMonth, getMonthName, isToday
} from '../../common/utils/DateUtils'
import { Schedule } from '../../model/Schedule'

/**
 * 月份缩略数据
 */
interface MonthMini {
  month: number
  name: string
  days: number[]
  firstDayOffset: number
  hasSchedules: boolean[]
}

@Component
export struct YearView {
  @Prop year: number = new Date().getFullYear()
  @Prop schedules: Schedule[] = []
  onMonthClick: (year: number, month: number) => void = () => {
  }
  @State months: MonthMini[] = []

  aboutToAppear(): void {
    this.buildMonthsData()
  }

  /**
   * 构建12个月的数据
   */
  private buildMonthsData(): void {
    this.months = []
    for (let month = 0; month < 12; month++) {
      const daysInMonth = getDaysInMonth(this.year, month)
      const firstDayOffset = getFirstDayOfMonth(this.year, month).getDay()
      const days: number[] = []
      for (let i = 1; i <= daysInMonth; i++) {
        days.push(i)
      }

      // 检查每天是否有日程
      const hasSchedules: boolean[] = days.map((day: number) => {
        const date = new Date(this.year, month, day)
        return this.schedules.some((s: Schedule) => s.isOnDate(date))
      })

      const monthData: MonthMini = {
        month: month,
        name: getMonthName(month),
        days: days,
        firstDayOffset: firstDayOffset,
        hasSchedules: hasSchedules
      }
      this.months.push(monthData)
    }
  }

  @Builder
  MonthMiniCard(monthData: MonthMini) {
    Column() {
      // 月份标题
      Text(monthData.name)
        .fontSize(14)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ bottom: 8 })

      // 星期表头
      Row() {
        ForEach(['日', '一', '二', '三', '四', '五', '六'], (day: string) => {
          Text(day)
            .fontSize(8)
            .fontColor('#999999')
            .width('14.28%')
            .textAlign(TextAlign.Center)
        }, (day: string) => day)
      }
      .width('100%')
      .margin({ bottom: 4 })

      // 日期网格
      Flex({ wrap: FlexWrap.Wrap }) {
        // 空白占位
        ForEach(this.getEmptySlots(monthData.firstDayOffset), (slot: number) => {
          Text('')
            .width('14.28%')
            .height(14)
        }, (slot: number) => `empty-${slot}`)

        // 日期
        ForEach(monthData.days, (day: number, index: number) => {
          Stack() {
            Text(day.toString())
              .fontSize(9)
              .fontColor(this.getDayColor(monthData.month, day))
              .fontWeight(this.isDayToday(monthData.month, day) ? FontWeight.Bold : FontWeight.Normal)

            // 日程指示点
            if (monthData.hasSchedules[index]) {
              Circle()
                .width(3)
                .height(3)
                .fill('#4ECDC4')
                .position({ x: '50%', y: '100%' })
                .translate({ x: -1.5, y: -2 })
            }
          }
          .width('14.28%')
          .height(14)
        }, (day: number) => `${monthData.month}-${day}`)
      }
      .width('100%')
    }
    .width('100%')
    .padding(12)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .shadow({
      radius: 4,
      color: 'rgba(0, 0, 0, 0.06)',
      offsetX: 0,
      offsetY: 2
    })
    .onClick(() => {
      this.onMonthClick(this.year, monthData.month)
    })
  }

  /**
   * 获取空白槽位数组
   */
  private getEmptySlots(count: number): number[] {
    const result: number[] = []
    for (let i = 0; i < count; i++) {
      result.push(i)
    }
    return result
  }

  /**
   * 判断是否是今天
   */
  private isDayToday(month: number, day: number): boolean {
    const date = new Date(this.year, month, day)
    return isToday(date)
  }

  /**
   * 获取日期颜色
   */
  private getDayColor(month: number, day: number): ResourceColor {
    if (this.isDayToday(month, day)) {
      return '#4ECDC4'
    }
    const date = new Date(this.year, month, day)
    const dayOfWeek = date.getDay()
    if (dayOfWeek === 0 || dayOfWeek === 6) {
      return '#FF6B6B'
    }
    return '#333333'
  }

  build() {
    Column() {
      // 年份标题
      Text(`${this.year}年`)
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')
        .textAlign(TextAlign.Center)
        .padding({ top: 16, bottom: 16 })

      // 月份网格 (4列 x 3行)
      Scroll() {
        Column({ space: 12 }) {
          ForEach([0, 1, 2], (row: number) => {
            Row({ space: 12 }) {
              ForEach([0, 1, 2, 3], (col: number) => {
                Column() {
                  this.MonthMiniCard(this.months[row * 4 + col])
                }
                .layoutWeight(1)
              }, (col: number) => col.toString())
            }
            .width('100%')
          }, (row: number) => row.toString())
        }
        .padding({ left: 16, right: 16, bottom: 16 })
      }
      .layoutWeight(1)
      .scrollable(ScrollDirection.Vertical)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
  }
}
