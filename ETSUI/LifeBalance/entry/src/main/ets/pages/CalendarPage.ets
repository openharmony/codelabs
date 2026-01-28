/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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
import { CalendarComponent } from './Calendar/Calendar'
import { StatisticsComponent } from './Calendar/statisticsComponent'
import { DailyData } from '../model/information'

@Entry
@Component
export struct CalendarMainPage {
  @State currentIndex: number = 0
  @StorageLink('healthHistory') historyData: Record<string, DailyData> = {}
  private controller: TabsController = new TabsController()

  @Builder
  TabBuilder(title: string, index: number) {
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

