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
 * 周视图组件
 */
import { formatDate, getWeekDates, getWeekDayName, isSameDay, isToday } from '../../common/utils/DateUtils'
import { Schedule } from '../../model/Schedule'

@Component
export struct WeekView {
  @Prop currentDate: Date = new Date()
  @Prop @Watch('onSelectedDateChange') selectedDate: Date = new Date()
  @Prop schedules: Schedule[] = []
  @Prop startFromMonday: boolean = false
  onDateSelect: (date: Date) => void = () => {
  }
  onScheduleClick: (schedule: Schedule) => void = () => {
  }
  onTimeSlotClick: (date: Date, hour: number) => void = () => {
  }
  @State weekDates: Date[] = []
  @State hours: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23]
  private scrollerForTimeline: Scroller = new Scroller()

  aboutToAppear(): void {
    this.buildWeekDates()
    // 默认滚动到早上8点
    setTimeout(() => {
      this.scrollerForTimeline.scrollTo({ xOffset: 0, yOffset: 8 * 60 })
    }, 100)
  }

  onSelectedDateChange(): void {
    this.buildWeekDates()
  }

  /**
   * 构建一周日期
   */
  private buildWeekDates(): void {
    this.weekDates = getWeekDates(this.currentDate, this.startFromMonday)
  }

  /**
   * 获取指定日期和时间的日程
   */
  private getSchedulesAtHour(date: Date, hour: number): Schedule[] {
    return this.schedules.filter((schedule: Schedule) => {
      const scheduleStart = new Date(schedule.startTime)
      const scheduleEnd = new Date(schedule.endTime)
      const slotStart = new Date(date.getFullYear(), date.getMonth(), date.getDate(), hour, 0, 0)
      const slotEnd = new Date(date.getFullYear(), date.getMonth(), date.getDate(), hour + 1, 0, 0)

      return scheduleStart < slotEnd && scheduleEnd > slotStart
    })
  }

  @Builder
  WeekHeader() {
    Row() {
      // 时间列占位
      Text('')
        .width(50)
        .height(60)

      // 日期列
      ForEach(this.weekDates, (date: Date) => {
        Column() {
          Text(getWeekDayName(date.getDay()))
            .fontSize(12)
            .fontColor('#666666')
            .margin({ bottom: 4 })

          Text(date.getDate().toString())
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.getDateHeaderColor(date))
            .backgroundColor(this.getDateHeaderBackground(date))
            .width(36)
            .height(36)
            .borderRadius(18)
            .textAlign(TextAlign.Center)
        }
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .height(60)
        .onClick(() => {
          this.onDateSelect(date)
        })
      }, (date: Date) => formatDate(date))
    }
    .width('100%')
    .backgroundColor('#FFFFFF')
    .border({ width: { bottom: 1 }, color: '#E0E0E0' })
  }

  /**
   * 获取日期表头文字颜色
   */
  private getDateHeaderColor(date: Date): ResourceColor {
    if (isSameDay(date, this.selectedDate)) {
      return '#FFFFFF'
    }
    if (isToday(date)) {
      return '#4ECDC4'
    }
    return '#333333'
  }

  /**
   * 获取日期表头背景色
   */
  private getDateHeaderBackground(date: Date): ResourceColor {
    if (isSameDay(date, this.selectedDate)) {
      return '#4ECDC4'
    }
    return Color.Transparent
  }

  @Builder
  TimeGrid() {
    Scroll(this.scrollerForTimeline) {
      Column() {
        ForEach(this.hours, (hour: number) => {
          Row() {
            // 时间标签
            Text(`${String(hour).padStart(2, '0')}:00`)
              .width(50)
              .fontSize(12)
              .fontColor('#999999')
              .textAlign(TextAlign.End)
              .padding({ right: 8 })

            // 时间槽
            ForEach(this.weekDates, (date: Date) => {
              Stack() {
                // 时间槽背景
                Column()
                  .width('100%')
                  .height('100%')
                  .border({ width: { bottom: 1, right: 1 }, color: '#F0F0F0' })
                  .onClick(() => {
                    this.onTimeSlotClick(date, hour)
                  })

                // 日程块
                Column() {
                  ForEach(this.getSchedulesAtHour(date, hour), (schedule: Schedule) => {
                    this.ScheduleBlock(schedule)
                  }, (schedule: Schedule) => schedule.id)
                }
                .width('100%')
              }
              .layoutWeight(1)
              .height(60)
            }, (date: Date) => formatDate(date))
          }
          .width('100%')
        }, (hour: number) => hour.toString())
      }
    }
    .scrollable(ScrollDirection.Vertical)
    .edgeEffect(EdgeEffect.Spring)
  }

  @Builder
  ScheduleBlock(schedule: Schedule) {
    Column() {
      Text(schedule.title)
        .fontSize(11)
        .fontColor('#FFFFFF')
        .maxLines(1)
        .textOverflow({ overflow: TextOverflow.Ellipsis })

      if (schedule.location) {
        Text(schedule.location)
          .fontSize(9)
          .fontColor('rgba(255, 255, 255, 0.8)')
          .maxLines(1)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
      }
    }
    .width('95%')
    .padding(4)
    .backgroundColor(schedule.color)
    .borderRadius(4)
    .margin(2)
    .onClick(() => {
      this.onScheduleClick(schedule)
    })
  }

  build() {
    Column() {
      this.WeekHeader()
      this.TimeGrid()
    }
    .width('100%')
    .height('100%')
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
