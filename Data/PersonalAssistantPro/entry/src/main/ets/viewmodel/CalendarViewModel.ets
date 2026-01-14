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
// entry/src/main/ets/viewmodel/CalendarViewModel.ets
import { DateUtils } from '../common/utils/DateUtils';
import { Logger } from '../common/utils/Logger';

/**
 * 日历数据模型
 * 用于计算日历视图所需的日期数据
 */
export class CalendarModel {
  day: number;
  month: number;
  year: number;
  isCurrentMonth: boolean;
  isToday: boolean;
  timestamp: number;

  constructor(year: number, month: number, day: number, isCurrentMonth: boolean, isToday: boolean = false) {
    this.day = day;
    this.month = month;
    this.year = year;
    this.isCurrentMonth = isCurrentMonth;
    this.isToday = isToday;
    this.timestamp = new Date(year, month - 1, day).getTime();
  }
}

/**
 * ✅ 新增：用于返回年月信息的类
 * 解决 "Object literals cannot be used as type declarations" 报错
 */
export class MonthInfo {
  year: number;
  month: number;

  constructor(year: number, month: number) {
    this.year = year;
    this.month = month;
  }
}

/**
 * 日历视图模型
 * 负责日历数据的生成与切换逻辑
 */
export class CalendarViewModel {
  private static logger = new Logger('CalendarViewModel');

  /**
   * 获取指定月份的日历数据（包含上个月结尾和下个月开头，补齐 42 格）
   * @param year 年份
   * @param month 月份 (1-12)
   */
  public getMonthData(year: number, month: number): Array<CalendarModel> {
    CalendarViewModel.logger.info(`Generating calendar data for ${year}-${month}`);

    const result: Array<CalendarModel> = [];
    const now = new Date();
    const currentYear = now.getFullYear();
    const currentMonth = now.getMonth() + 1;
    const currentDay = now.getDate();

    // 1. 获取本月第一天是星期几 (0-6, 0 is Sunday)
    const firstDayOfMonth = new Date(year, month - 1, 1);
    let startDayOfWeek = firstDayOfMonth.getDay();

    // 2. 获取本月总天数
    const daysInMonth = new Date(year, month, 0).getDate();

    // 3. 获取上个月总天数
    const daysInPrevMonth = new Date(year, month - 1, 0).getDate();

    // --- 填充上个月的数据 (前补) ---
    for (let i = 0; i < startDayOfWeek; i++) {
      const day = daysInPrevMonth - startDayOfWeek + i + 1;
      // 上个月的年份月份处理
      let prevMonth = month - 1;
      let prevYear = year;
      if (prevMonth === 0) {
        prevMonth = 12;
        prevYear--;
      }
      result.push(new CalendarModel(prevYear, prevMonth, day, false));
    }

    // --- 填充本月数据 ---
    for (let i = 1; i <= daysInMonth; i++) {
      const isToday = (year === currentYear && month === currentMonth && i === currentDay);
      result.push(new CalendarModel(year, month, i, true, isToday));
    }

    // --- 填充下个月的数据 (后补，凑齐 42 格) ---
    const remainingCells = 42 - result.length;
    for (let i = 1; i <= remainingCells; i++) {
      let nextMonth = month + 1;
      let nextYear = year;
      if (nextMonth === 13) {
        nextMonth = 1;
        nextYear++;
      }
      result.push(new CalendarModel(nextYear, nextMonth, i, false));
    }

    return result;
  }

  /**
   * 获取下一月
   * ✅ 修复：返回值类型改为 MonthInfo
   */
  public getNextMonth(year: number, month: number): MonthInfo {
    let nextMonth = month + 1;
    let nextYear = year;
    if (nextMonth > 12) {
      nextMonth = 1;
      nextYear++;
    }
    // ✅ 修复：返回类实例
    return new MonthInfo(nextYear, nextMonth);
  }

  /**
   * 获取上一月
   * ✅ 修复：返回值类型改为 MonthInfo
   */
  public getPrevMonth(year: number, month: number): MonthInfo {
    let prevMonth = month - 1;
    let prevYear = year;
    if (prevMonth < 1) {
      prevMonth = 12;
      prevYear--;
    }
    // ✅ 修复：返回类实例
    return new MonthInfo(prevYear, prevMonth);
  }
}