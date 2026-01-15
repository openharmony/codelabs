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
// entry/src/main/ets/view/calendar/WeekView.ets
import { CalendarModel, CalendarViewModel, TaskStatus } from '../../viewmodel/CalendarViewModel';
import { DayItemView } from './DayItemView';

@Component
export struct WeekView {
  //  [修正] @Watch 必须写在变量上，意思为：当 selectedTimestamp 变化时，调用 onSelectedDateChange
  @Link @Watch('onSelectedDateChange') selectedTimestamp: number;

  // 接收忙碌日期 Map
  @Prop taskMap: Map<string, TaskStatus> = new Map();

  // 内部状态：这周的7天数据
  @State weekData: Array<CalendarModel> = [];

  // 回调
  onDateSelected: (timestamp: number) => void = () => {};

  private viewModel: CalendarViewModel = new CalendarViewModel();
  private weekTitles: string[] = ['日', '一', '二', '三', '四', '五', '六'];

  aboutToAppear() {
    this.refreshData();
  }

  //  监听回调方法 (这里不需要再加 @Watch 了)
  onSelectedDateChange() {
    // 只有当选中的日期跳出了当前周的范围时，才需要刷新数据
    this.refreshData();
  }

  refreshData() {
    this.weekData = this.viewModel.getWeekData(this.selectedTimestamp);
  }

  // 切换到上一周
  prevWeek() {
    const date = new Date(this.selectedTimestamp);
    date.setDate(date.getDate() - 7);
    this.selectedTimestamp = date.getTime();
    // 由于是双向绑定，修改 selectedTimestamp 会自动触发 onSelectedDateChange -> refreshData
    this.onDateSelected(this.selectedTimestamp);
  }

  // 切换到下一周
  nextWeek() {
    const date = new Date(this.selectedTimestamp);
    date.setDate(date.getDate() + 7);
    this.selectedTimestamp = date.getTime();
    this.onDateSelected(this.selectedTimestamp);
  }

  build() {
    Column() {
      // 1. 顶部控制栏 (左右切换 + 日期范围)
      Row() {
        Image($r('app.media.ic_public_arrow_left'))
          .width(20).height(20)
          .onClick(() => this.prevWeek())

        Text(this.getWeekRangeTitle())
          .fontSize(14)
          .fontColor(Color.Gray)
          .margin({ left: 10, right: 10 })

        Image($r('app.media.ic_public_arrow_right'))
          .width(20).height(20)
          .onClick(() => this.nextWeek())
      }
      .width('100%')
      .justifyContent(FlexAlign.Center)
      .height(30)
      .margin({ bottom: 8 })

      // 2. 星期标题头 (Mon, Tue, Wed...)
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
      .height(20)
      .margin({ bottom: 4 })

      // 3. 星期数据网格
      Grid() {
        ForEach(this.weekData, (day: CalendarModel) => {
          GridItem() {
            DayItemView({
              dayModel: day,
              selectedTimestamp: this.selectedTimestamp,
              // 传递条状样式
              stripType: this.taskMap.get(`${day.year}-${day.month}-${day.day}`)?.stripType || '',
              // 传递点状样式
              hasPoint: this.taskMap.get(`${day.year}-${day.month}-${day.day}`)?.hasPoint || false
            })
          }
          .onClick(() => {
            this.selectedTimestamp = day.timestamp;
            this.onDateSelected(day.timestamp);
          })
        })
      }
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr')
      .rowsTemplate('1fr')
      .height(40)
    }
    .width('100%')
    .padding(10)
    .backgroundColor(Color.White)
    .borderRadius(16)
  }

  private getWeekRangeTitle(): string {
    if (this.weekData.length === 0) return '';
    const start = this.weekData[0];
    const end = this.weekData[6];
    return `${start.year}年${start.month}月${start.day}日 - ${end.month}月${end.day}日`;
  }
}