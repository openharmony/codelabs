/**
 * 日程卡片组件
 */
import { formatTime, formatDateChinese } from '../../common/utils/DateUtils'
import { Schedule } from '../../model/Schedule'

@Component
export struct ScheduleCard {
  @Prop scheduleData: Schedule = new Schedule()
  @Prop showDate: boolean = false
  onCardClick: (schedule: Schedule) => void = () => {}
  onEditClick: (schedule: Schedule) => void = () => {}
  onDeleteClick: (schedule: Schedule) => void = () => {}

  build() {
    Row() {
      // 左侧颜色条
      Column()
        .width(6)
        .height('100%')
        .backgroundColor(this.scheduleData.color)
        .borderRadius({ topLeft: 12, bottomLeft: 12 })

      // 主内容区
      Column() {
        // 标题行
        Row() {
          Text(this.scheduleData.title)
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .fontColor('#333333')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
            .layoutWeight(1)

          // 时间显示
          if (!this.scheduleData.isAllDay) {
            Text(`${formatTime(new Date(this.scheduleData.startTime))} - ${formatTime(new Date(this.scheduleData.endTime))}`)
              .fontSize(12)
              .fontColor('#666666')
          } else {
            Text('全天')
              .fontSize(12)
              .fontColor('#4ECDC4')
              .fontWeight(FontWeight.Medium)
          }
        }
        .width('100%')

        // 日期显示（可选）
        if (this.showDate) {
          Text(formatDateChinese(new Date(this.scheduleData.startTime)))
            .fontSize(12)
            .fontColor('#999999')
            .margin({ top: 4 })
        }

        // 地点
        if (this.scheduleData.location) {
          Row() {
            Text(this.scheduleData.location)
              .fontSize(13)
              .fontColor('#666666')
              .margin({ left: 4 })
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.Ellipsis })
          }
          .margin({ top: 8 })
        }

        // 描述
        if (this.scheduleData.description) {
          Text(this.scheduleData.description)
            .fontSize(13)
            .fontColor('#888888')
            .margin({ top: 6 })
            .maxLines(2)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
        }

        // 底部信息
        Row() {
          // 标签指示
          if (this.scheduleData.tagIds.length > 0) {
            Row({ space: 4 }) {
              ForEach(this.scheduleData.tagIds.slice(0, 2), (tagId: string) => {
                Text('●')
                  .fontSize(8)
                  .fontColor('#4ECDC4')
              }, (tagId: string) => tagId)
              if (this.scheduleData.tagIds.length > 2) {
                Text(`+${this.scheduleData.tagIds.length - 2}`)
                  .fontSize(10)
                  .fontColor('#999999')
              }
            }
          }

          Blank()

          // 提醒图标
          if (this.scheduleData.reminderIds.length > 0) {
            Image($r('sys.media.ohos_ic_public_clock'))
              .width(14)
              .height(14)
              .fillColor('#999999')
              .margin({ right: 8 })
          }

          // 录音图标
          if (this.scheduleData.recordingIds.length > 0) {
            Image($r('sys.media.ohos_ic_public_voice'))
              .width(14)
              .height(14)
              .fillColor('#999999')
              .margin({ right: 8 })
          }

          // 外部链接图标
          if (this.scheduleData.externalAppLink) {
            Image($r('sys.media.ohos_ic_public_share'))
              .width(14)
              .height(14)
              .fillColor('#999999')
          }
        }
        .width('100%')
        .margin({ top: 8 })
      }
      .layoutWeight(1)
      .padding({ left: 12, right: 12, top: 12, bottom: 12 })
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
    .onClick(() => {
      this.onCardClick(this.scheduleData)
    })
  }
}
