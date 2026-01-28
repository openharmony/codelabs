import { CalendarComponent } from './Calendar/Calendar'
import { StatisticsComponent } from './Calendar/statisticsComponent'
import { DailyData } from '../model/information'

@Entry
@Component
export struct CalendarMainPage {
  @State currentIndex: number = 0
  private controller: TabsController = new TabsController()

  @StorageLink('healthHistory') historyData: Record<string, DailyData> = {}

  @Builder TabBuilder(title: string, index: number) {
    Column() {
      Text(title)
        .fontColor(this.currentIndex === index ? '#007DFF' : '#666')
        .fontSize(16)
        .fontWeight(this.currentIndex === index ? FontWeight.Bold : FontWeight.Normal)

      Rect()
        .width(this.currentIndex === index ? 30 : 0)
        .height(3)
        .fill('#007DFF')
        .margin({ top: 6 })
        .borderRadius(2)
        .animation({ duration: 200, curve: Curve.EaseInOut })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }

  build() {
    Column() {
      Tabs({ barPosition: BarPosition.Start, controller: this.controller }) {
        TabContent() {
          CalendarComponent()
        }.tabBar(this.TabBuilder('历史日历', 0))

        TabContent() {
          StatisticsComponent()
        }.tabBar(this.TabBuilder('数据统计', 1))
      }
      .barHeight(56)
      .barMode(BarMode.Fixed)
      .onChange((index: number) => {
        this.currentIndex = index
      })
      .vertical(false)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}

