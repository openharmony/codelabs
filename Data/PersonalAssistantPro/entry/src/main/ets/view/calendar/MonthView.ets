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
// entry/src/main/ets/view/calendar/MonthView.ets
import { CalendarModel, CalendarViewModel } from '../../viewmodel/CalendarViewModel';
import { DayItemView } from './DayItemView';

@Component
export struct MonthView {
  @State currentYear: number = new Date().getFullYear();
  @State currentMonth: number = new Date().getMonth() + 1;
  @State calendarData: Array<CalendarModel> = [];
  @State selectedTimestamp: number = 0; // 当前选中的日期

  private viewModel: CalendarViewModel = new CalendarViewModel();
  private weekTitles: string[] = ['日', '一', '二', '三', '四', '五', '六'];

  aboutToAppear() {
    this.refreshData();
    // 默认选中今天
    this.selectedTimestamp = new Date(this.currentYear, this.currentMonth - 1, new Date().getDate()).getTime();
  }

  private refreshData() {
    this.calendarData = this.viewModel.getMonthData(this.currentYear, this.currentMonth);
  }

  public goToNextMonth() {
    const next = this.viewModel.getNextMonth(this.currentYear, this.currentMonth);
    this.currentYear = next.year;
    this.currentMonth = next.month;
    this.refreshData();
  }

  public goToPrevMonth() {
    const prev = this.viewModel.getPrevMonth(this.currentYear, this.currentMonth);
    this.currentYear = prev.year;
    this.currentMonth = prev.month;
    this.refreshData();
  }

  build() {
    Column() {
      // 1. 年月切换标题栏
      Row() {
        Image($r('app.media.ic_back'))
          .width(20).height(20)
          .onClick(() => this.goToPrevMonth())

        Text(`${this.currentYear}年 ${this.currentMonth}月`)
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 16, right: 16 })

        Image($r('app.media.ic_back'))
          .width(20).height(20)
          .rotate({ angle: 180 })
          .onClick(() => this.goToNextMonth())
      }
      .width('100%')
      .justifyContent(FlexAlign.Center)
      .height(50)

      // 2. 星期标题头
      GridRow({ columns: 7 }) {
        ForEach(this.weekTitles, (title: string) => {
          GridCol() {
            Text(title)
              .fontSize(12)
              .fontColor('#999999')
              .width('100%')
              .textAlign(TextAlign.Center)
          }
        })
      }
      .width('100%')
      .height(30)
      .margin({ bottom: 8 })

      // 3. 日期网格
      Grid() {
        ForEach(this.calendarData, (day: CalendarModel) => {
          GridItem() {
            // 这里传参给子组件
            DayItemView({
              dayModel: day,
              selectedTimestamp: this.selectedTimestamp
            })
          }
          .onClick(() => {
            this.selectedTimestamp = day.timestamp;
            // 这里可以触发一个外部回调，通知父组件筛选列表
            console.info(`Selected date: ${day.year}-${day.month}-${day.day}`);
          })
        })
      }
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr')
      .rowsTemplate('1fr 1fr 1fr 1fr 1fr 1fr') // 6行
      .rowsGap(8)
      .height(340) // 固定高度
    }
    .width('100%')
    .padding(16)
    .backgroundColor(Color.White)
    .borderRadius(16)
    .shadow({ radius: 4, color: '#10000000', offsetY: 2 })
  }
}