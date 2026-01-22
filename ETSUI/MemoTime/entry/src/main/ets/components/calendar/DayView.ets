/**
 * 日视图组件
 */
import {
  formatTime,
  formatDateChinese,
  getWeekDayName
} from '../../common/utils/DateUtils'
import { Schedule } from '../../model/Schedule'

@Component
export struct DayView {
  @Prop currentDate: Date = new Date()
  @Prop schedules: Schedule[] = []
  onScheduleClick: (schedule: Schedule) => void = () => {}
  onTimeSlotClick: (date: Date, hour: number) => void = () => {}

  @State hours: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23]

  private scrollerForTimeline: Scroller = new Scroller()

  aboutToAppear(): void {
    // 默认滚动到早上8点
    setTimeout(() => {
      this.scrollerForTimeline.scrollTo({ xOffset: 0, yOffset: 8 * 80 })
    }, 100)
  }

  /**
   * 获取指定小时的日程列表
   */
  private getSchedulesAtHour(hour: number): Schedule[] {
    const date = this.currentDate
    return this.schedules.filter((schedule: Schedule) => {
      const scheduleStart = new Date(schedule.startTime)
      const scheduleEnd = new Date(schedule.endTime)
      const slotStart = new Date(date.getFullYear(), date.getMonth(), date.getDate(), hour, 0, 0)
      const slotEnd = new Date(date.getFullYear(), date.getMonth(), date.getDate(), hour + 1, 0, 0)

      return scheduleStart < slotEnd && scheduleEnd > slotStart && schedule.isOnDate(date)
    })
  }

  @Builder
  DayHeader() {
    Column() {
      Text(formatDateChinese(this.currentDate))
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')

      Text(getWeekDayName(this.currentDate.getDay()))
        .fontSize(14)
        .fontColor('#666666')
        .margin({ top: 4 })
    }
    .width('100%')
    .padding(16)
    .alignItems(HorizontalAlign.Start)
    .backgroundColor('#FFFFFF')
    .border({ width: { bottom: 1 }, color: '#E0E0E0' })
  }

  @Builder
  TimelineRow(hour: number) {
    Row() {
      // 时间标签
      Column() {
        Text(`${String(hour).padStart(2, '0')}:00`)
          .fontSize(12)
          .fontColor('#999999')
      }
      .width(60)
      .height(80)
      .justifyContent(FlexAlign.Start)
      .alignItems(HorizontalAlign.End)
      .padding({ right: 12, top: 4 })

      // 时间槽和日程
      Stack({ alignContent: Alignment.TopStart }) {
        // 时间槽背景
        Column()
          .width('100%')
          .height(80)
          .border({ width: { top: 1 }, color: '#F0F0F0' })
          .onClick(() => {
            this.onTimeSlotClick(this.currentDate, hour)
          })

        // 日程列表
        Column({ space: 4 }) {
          ForEach(this.getSchedulesAtHour(hour), (schedule: Schedule) => {
            this.ScheduleBlock(schedule)
          }, (schedule: Schedule) => schedule.id)
        }
        .width('100%')
        .padding({ left: 4, right: 4, top: 4 })
      }
      .layoutWeight(1)
    }
    .width('100%')
  }

  @Builder
  ScheduleBlock(schedule: Schedule) {
    Row() {
      // 左侧颜色条
      Column()
        .width(4)
        .height('100%')
        .backgroundColor(schedule.color)
        .borderRadius({ topLeft: 4, bottomLeft: 4 })

      // 日程内容
      Column() {
        Row() {
          Text(schedule.title)
            .fontSize(14)
            .fontWeight(FontWeight.Medium)
            .fontColor('#333333')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
            .layoutWeight(1)

          Text(`${formatTime(new Date(schedule.startTime))} - ${formatTime(new Date(schedule.endTime))}`)
            .fontSize(12)
            .fontColor('#666666')
        }
        .width('100%')

        if (schedule.location) {
          Row() {
            Text(schedule.location)
              .fontSize(12)
              .fontColor('#999999')
              .margin({ left: 4 })
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.Ellipsis })
          }
          .margin({ top: 4 })
        }

        if (schedule.description) {
          Text(schedule.description)
            .fontSize(12)
            .fontColor('#666666')
            .margin({ top: 4 })
            .maxLines(2)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
        }
      }
      .layoutWeight(1)
      .padding(8)
    }
    .width('100%')
    .height(60)
    .backgroundColor('#FFFFFF')
    .borderRadius(8)
    .shadow({
      radius: 4,
      color: 'rgba(0, 0, 0, 0.08)',
      offsetX: 0,
      offsetY: 2
    })
    .onClick(() => {
      this.onScheduleClick(schedule)
    })
  }

  @Builder
  TimelineGrid() {
    Scroll(this.scrollerForTimeline) {
      Column() {
        ForEach(this.hours, (hour: number) => {
          this.TimelineRow(hour)
        }, (hour: number) => hour.toString())
      }
    }
    .scrollable(ScrollDirection.Vertical)
    .edgeEffect(EdgeEffect.Spring)
    .layoutWeight(1)
  }

  build() {
    Column() {
      this.DayHeader()
      this.TimelineGrid()
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
    .borderRadius(12)
  }
}
