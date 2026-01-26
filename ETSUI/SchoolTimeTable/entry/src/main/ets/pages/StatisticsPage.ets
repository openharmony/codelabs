import { WeekDay, CourseStatistics, Course } from '../model/CourseModel';
import { CourseDataService } from '../service/CourseDataService';
import { router } from '@kit.ArkUI';

@Entry
@Component
struct StatisticsPage {
  private courseService: CourseDataService = CourseDataService.getInstance();
  @StorageLink('GlobalCourseList') @Watch('updateStatistics') courses: Course[] = [];
  @State statistics: CourseStatistics = {
    totalCourses: 0,
    totalHours: 0,
    coursesByDay: new Map()
  };

  aboutToAppear(): void {
    this.updateStatistics();
  }

  updateStatistics(): void {
    this.statistics = this.courseService.getStatistics();
  }

  build() {
    Column() {
      this.Header()

      Scroll() {
        Column({ space: 20 }) {
          this.TotalStats()
          this.StatsByDay()
          this.CourseList()
        }
        .width('100%')
        .padding({
          left: 16,
          right: 16,
          top: 16,
          bottom: 16
        })
      }
      .layoutWeight(1)
      .scrollBar(BarState.Auto)
      .scrollBarColor('#CCCCCC')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }

  @Builder
  Header() {
    Row() {
      Button() {
        Image($r('sys.media.ohos_ic_public_arrow_left'))
          .width(24)
          .height(24)
          .fillColor('#333333')
      }
      .width(40)
      .height(40)
      .backgroundColor(Color.Transparent)
      .onClick(() => {
        router.back();
      })

      Text('课程统计')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .layoutWeight(1)
        .textAlign(TextAlign.Center)

      Row()
        .width(40)
        .height(40)
    }
    .width('100%')
    .padding({
      left: 16,
      right: 16,
      top: 12,
      bottom: 12
    })
    .backgroundColor('#FFFFFF')
  }

  @Builder
  TotalStats() {
    Column({ space: 16 }) {
      Text('总体统计')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')

      Row({ space: 16 }) {
        this.StatCard('课程总数', this.statistics.totalCourses.toString(), '#007DFF')
        this.StatCard('总学时', this.statistics.totalHours.toFixed(1), '#FF6B6B')
      }
      .width('100%')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  StatCard(title: string, value: string, color: string) {
    Column() {
      Text(value)
        .fontSize(28)
        .fontWeight(FontWeight.Bold)
        .fontColor(color)

      Text(title)
        .fontSize(14)
        .fontColor('#666666')
        .margin({ top: 8 })
    }
    .layoutWeight(1)
    .padding(16)
    .backgroundColor('#F5F5F5')
    .borderRadius(8)
    .alignItems(HorizontalAlign.Center)
  }

  @Builder
  StatsByDay() {
    Column({ space: 16 }) {
      Text('按星期统计')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')

      Column({ space: 12 }) {
        this.DayStatItem('周一', this.statistics.coursesByDay.get(WeekDay.MONDAY) || 0)
        this.DayStatItem('周二', this.statistics.coursesByDay.get(WeekDay.TUESDAY) || 0)
        this.DayStatItem('周三', this.statistics.coursesByDay.get(WeekDay.WEDNESDAY) || 0)
        this.DayStatItem('周四', this.statistics.coursesByDay.get(WeekDay.THURSDAY) || 0)
        this.DayStatItem('周五', this.statistics.coursesByDay.get(WeekDay.FRIDAY) || 0)
        this.DayStatItem('周六', this.statistics.coursesByDay.get(WeekDay.SATURDAY) || 0)
        this.DayStatItem('周日', this.statistics.coursesByDay.get(WeekDay.SUNDAY) || 0)
      }
      .width('100%')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  DayStatItem(dayName: string, count: number) {
    Row() {
      Text(dayName)
        .fontSize(16)
        .fontColor('#333333')
        .width(60)

      Row() {
        ForEach([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], (index: number) => {
          Circle()
            .width(12)
            .height(12)
            .fill(index <= count ? '#007DFF' : '#E0E0E0')
        })
      }
      .layoutWeight(1)
      .justifyContent(FlexAlign.Start)

      Text(`${count}节`)
        .fontSize(14)
        .fontColor('#666666')
        .width(50)
        .textAlign(TextAlign.End)
    }
    .width('100%')
    .padding({ top: 8, bottom: 8 })
  }

  @Builder
  CourseList() {
    Column({ space: 16 }) {
      Text('课程详情')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')

      Column({ space: 12 }) {
        ForEach(this.courses, (course: Course) => {
          Row() {
            Row()
              .width(16)
              .height(16)
              .backgroundColor(course.color)
              .borderRadius(4)
              .margin({ right: 12 })

            Column({ space: 4 }) {
              Text(course.name)
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .fontColor('#333333')

              Row({ space: 8 }) {
                Text(this.courseService.getWeekDayName(course.day))
                  .fontSize(14)
                  .fontColor('#666666')

                Text(`第${course.startSection}-${course.endSection}节`)
                  .fontSize(14)
                  .fontColor('#666666')

              }

              Row({ space: 8 }) {
                Text(course.teacher)
                  .fontSize(14)
                  .fontColor('#999999')

                Text('·')
                  .fontSize(14)
                  .fontColor('#999999')

                Text(course.location)
                  .fontSize(14)
                  .fontColor('#999999')
              }
            }
            .alignItems(HorizontalAlign.Start)
            .layoutWeight(1)
          }
          .width('100%')
          .padding(16)
          .backgroundColor('#FFFFFF')
          .borderRadius(8)
          .border({ width: 1, color: '#E0E0E0' })
        }, (course: Course) => course.id)
      }
      .width('100%')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }
}
