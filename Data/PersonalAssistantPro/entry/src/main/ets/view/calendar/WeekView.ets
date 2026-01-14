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
// src/main/ets/view/calendar/WeekView.ets
import { CalendarModel, CalendarViewModel } from '../../viewmodel/CalendarViewModel';
import { DateUtils } from '../../common/utils/DateUtils';
import { DayItemView } from './DayItemView';

/**
 * 周视图组件
 * 仅显示当前周的7天数据，适用于需要节省空间的场景
 */
@Component
export struct WeekView {
  // === 状态定义 ===
  @State currentYear: number = new Date().getFullYear();
  @State currentMonth: number = new Date().getMonth() + 1;
  @State weekData: Array<CalendarModel> = [];
  @Link selectedTimestamp: number;

  private viewModel: CalendarViewModel = new CalendarViewModel();
  private weekTitles: string[] = ['日', '一', '二', '三', '四', '五', '六'];

  /**
   * 组件加载时计算本周数据
   */
  aboutToAppear() {
    this.initWeekData();
  }

  /**
   * 初始化本周数据
   * 逻辑：先获取本周第一天，然后往后推7天
   */
  private initWeekData() {
    const now = new Date();
    // 使用工具类获取本周第一天的时间戳
    const startOfWeekTs = DateUtils.getStartOfWeek(now.getTime());
    const startObj = new Date(startOfWeekTs);

    const result: Array<CalendarModel> = [];

    // 循环构造7天数据
    for (let i = 0; i < 7; i++) {
      // 复制日期对象
      const tempDate = new Date(startObj.getTime() + i * 24 * 60 * 60 * 1000);

      const y = tempDate.getFullYear();
      const m = tempDate.getMonth() + 1;
      const d = tempDate.getDate();

      // 判断是否是今天
      const isToday = DateUtils.isSameDay(tempDate.getTime(), now.getTime());

      result.push(new CalendarModel(y, m, d, true, isToday));
    }

    this.weekData = result;

    // 更新当前显示的年月信息（取第一天的）
    this.currentYear = result[0].year;
    this.currentMonth = result[0].month;
  }

  build() {
    Column() {
      // 1. 顶部显示的年月信息
      Text(`${this.currentYear}年 ${this.currentMonth}月 - 本周`)
        .fontSize(14)
        .fontColor('#666666')
        .width('100%')
        .textAlign(TextAlign.Start)
        .margin({ bottom: 12, left: 4 })

      // 2. 星期标题 (一行)
      GridRow({ columns: 7 }) {
        ForEach(this.weekTitles, (title: string) => {
          GridCol() {
            Text(title)
              .fontSize(10)
              .fontColor('#999999')
              .width('100%')
              .textAlign(TextAlign.Center)
              .margin({ bottom: 6 })
          }
        })
      }
      .width('100%')

      // 3. 日期内容 (一行)
      GridRow({ columns: 7 }) {
        ForEach(this.weekData, (day: CalendarModel) => {
          GridCol() {
            // 复用 DayItemView
            DayItemView({
              dayModel: day,
              selectedTimestamp: this.selectedTimestamp
            })
              .onClick(() => {
                this.selectedTimestamp = day.timestamp;
              })
          }
        })
      }
      .width('100%')
    }
    .width('100%')
    .padding(12)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .margin({ bottom: 12 })
  }
}