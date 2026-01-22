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
import { CalendarModel, CalendarViewModel, TaskStatus } from '../../viewmodel/CalendarViewModel';
import { DayItemView } from './DayItemView';

@Component
export struct MonthView {
  @State currentYear: number = new Date().getFullYear();
  @State currentMonth: number = new Date().getMonth() + 1;
  @State calendarData: Array<CalendarModel> = [];
  //  [修改] 改为 @Link 以便和父组件双向同步 (选中日期)
  @Link selectedTimestamp: number;
  //  [新增] 接收父组件传来的“忙碌日期”集合
  @Prop taskDaysSet: Set<string> = new Set();
  @Prop taskMap: Map<string, TaskStatus> = new Map();
  private viewModel: CalendarViewModel = new CalendarViewModel();
  private weekTitles: ResourceStr[] = [
    $r('app.string.week_sun'),
    $r('app.string.week_mon'),
    $r('app.string.week_tue'),
    $r('app.string.week_wed'),
    $r('app.string.week_thu'),
    $r('app.string.week_fri'),
    $r('app.string.week_sat')
  ];

  //  [新增] 点击日期的回调函数
  onDateSelected: (timestamp: number) => void = () => {
  };

  aboutToAppear() {
    this.refreshData();
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
        Image($r('app.media.ic_public_arrow_left'))
          .width(20).height(20)
          .onClick(() => this.goToPrevMonth())

        // ✅ 使用 Span 组合: "2025" + "年" + " " + "1" + "月"
        Text() {
          Span(`${this.currentYear}`)
          Span($r('app.string.calendar_unit_year'))
          Span(` ${this.currentMonth}`)
          Span($r('app.string.calendar_unit_month'))
        }
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .margin({ left: 16, right: 16 })

        Image($r('app.media.ic_public_arrow_right'))
          .width(20).height(20)
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
            DayItemView({
              dayModel: day,
              selectedTimestamp: this.selectedTimestamp,

              //  [修改] 传递 stripType (如果 undefined 则传空串)
              stripType: this.taskMap.get(`${day.year}-${day.month}-${day.day}`)?.stripType || '',

              //  [修改] 传递 hasPoint (如果 undefined 则传 false)
              hasPoint: this.taskMap.get(`${day.year}-${day.month}-${day.day}`)?.hasPoint || false
            })
          }
          .onClick(() => {
            // 更新选中状态
            this.selectedTimestamp = day.timestamp;
            // 触发回调
            this.onDateSelected(day.timestamp);
          })
        })
      }
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr')
      .rowsTemplate('1fr 1fr 1fr 1fr 1fr 1fr') // 6行
      .rowsGap(8)
      .height(300)
    }
    .width('100%')
    .padding(16)
    .backgroundColor(Color.White)
    .borderRadius(16)
  }

  private refreshData() {
    this.calendarData = this.viewModel.getMonthData(this.currentYear, this.currentMonth);
  }
}