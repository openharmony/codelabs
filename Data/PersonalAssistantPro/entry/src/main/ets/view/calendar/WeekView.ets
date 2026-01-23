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
  private viewModel: CalendarViewModel = new CalendarViewModel();
  // 📝 [修改] 使用 Resource 数组替代硬编码的字符串数组
  private weekTitles: Resource[] = [
    $r('app.string.week_sun'),
    $r('app.string.week_mon'),
    $r('app.string.week_tue'),
    $r('app.string.week_wed'),
    $r('app.string.week_thu'),
    $r('app.string.week_fri'),
    $r('app.string.week_sat')
  ];

  // 回调
  onDateSelected: (timestamp: number) => void = () => {
  };

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
        // 📝 [修改] 遍历 Resource 数组
        ForEach(this.weekTitles, (title: Resource) => {
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

  // 📝 [修改] 动态获取年月日单位
  private getWeekRangeTitle(): string {
    if (this.weekData.length === 0) {
      return '';
    }
    const start = this.weekData[0];
    const end = this.weekData[6];

    // 获取上下文以解析资源字符串
    const context = getContext(this);
    const y = context.resourceManager.getStringSync($r('app.string.calendar_unit_year').id);
    const m = context.resourceManager.getStringSync($r('app.string.calendar_unit_month').id);
    const d = context.resourceManager.getStringSync($r('app.string.calendar_unit_day').id);

    // 拼接字符串: "2023年5月1日 - 5月7日"
    return `${start.year}${y}${start.month}${m}${start.day}${d} - ${end.month}${m}${end.day}${d}`;
  }
}