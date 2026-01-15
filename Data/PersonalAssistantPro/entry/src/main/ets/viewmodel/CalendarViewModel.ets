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
//  [修复] 引入 Event 类
import { Event } from '../model/EventModel';

/**
 * 日历数据模型
 * 用于计算日历视图所需的日期数据
 */
export class TaskStatus {
  stripType: string = ''; // 'head' | 'body' | 'tail' | '' (空代表没有跨天条)
  hasPoint: boolean = false; // 是否有单天日程的小圆点
}
@Observed
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
 *  新增：用于返回年月信息的类
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

  public getWeekData(currentTimestamp: number): Array<CalendarModel> {
    const result: Array<CalendarModel> = [];

    // 1. 获取当前选中的日期对象
    const current = new Date(currentTimestamp);

    // 2. 算出这周的第一天 (周日)
    // current.getDay() 返回 0(周日) ~ 6(周六)
    // 如果今天是周三(3)，就要往前推 3 天
    const dayOfWeek = current.getDay();
    const startOfWeek = new Date(current);
    startOfWeek.setDate(current.getDate() - dayOfWeek);

    // 3. 循环生成 7 天的数据
    const today = new Date();
    for (let i = 0; i < 7; i++) {
      const temp = new Date(startOfWeek);
      temp.setDate(startOfWeek.getDate() + i);

      const y = temp.getFullYear();
      const m = temp.getMonth() + 1;
      const d = temp.getDate();

      // 判断是否是今天
      const isToday = (y === today.getFullYear() && m === (today.getMonth() + 1) && d === today.getDate());

      // 生成模型 (isCurrentMonth 在周视图里其实不太重要，设为 true 即可，或者根据实际月份判断)
      result.push(new CalendarModel(y, m, d, true, isToday));
    }

    return result;
  }

  //  此时这里的 Event 类型就能被正确识别了
  public processEventsToTaskDays(events: Event[]): Set<string> {
    const taskSet = new Set<string>();

    events.forEach(event => {
      // 1. 获取日程的开始和结束时间 (清除时分秒，只保留日期)
      const startDate = new Date(event.startTime);
      startDate.setHours(0, 0, 0, 0);

      const endDate = new Date(event.endTime);
      endDate.setHours(0, 0, 0, 0);

      // 2. 循环每一天，加入集合
      // 为了防止死循环，限制最大跨度为 365 天
      let current = new Date(startDate);
      let daysCount = 0;

      while (current <= endDate && daysCount < 365) {
        const dateStr = `${current.getFullYear()}-${current.getMonth() + 1}-${current.getDate()}`;
        taskSet.add(dateStr);

        // 加一天
        current.setDate(current.getDate() + 1);
        daysCount++;
      }
    });

    return taskSet;
  }
  public processEventsToTaskMap(events: Event[]): Map<string, TaskStatus> {
    const taskMap = new Map<string, TaskStatus>();

    // 辅助函数：获取或创建状态对象
    const getStatus = (key: string): TaskStatus => {
      if (!taskMap.has(key)) {
        taskMap.set(key, new TaskStatus());
      }
      return taskMap.get(key)!;
    };

    events.forEach(event => {
      const startDate = new Date(event.startTime);
      startDate.setHours(0, 0, 0, 0);
      const endDate = new Date(event.endTime);
      endDate.setHours(0, 0, 0, 0);

      const startTs = startDate.getTime();
      const endTs = endDate.getTime();

      // === 情况 A：单天日程 -> 标记 hasPoint ===
      if (startTs === endTs) {
        const key = this.formatDateKey(startDate);
        const status = getStatus(key);
        status.hasPoint = true; // ⚠️ 只通过这里打开小圆点，不影响 stripType
        return;
      }

      // === 情况 B：跨天日程 -> 标记 stripType ===
      let current = new Date(startDate);
      let daysCount = 0;

      while (current.getTime() <= endTs && daysCount < 365) {
        const key = this.formatDateKey(current);
        const currentTs = current.getTime();
        const status = getStatus(key);

        let newType = 'body';
        if (currentTs === startTs) newType = 'head';
        else if (currentTs === endTs) newType = 'tail';

        // 冲突逻辑：Body > Head/Tail > Empty
        // 如果已经是 Body，保持 Body；如果是 Head 遇到 Tail，变成 Body
        if (status.stripType === 'body') {
          // keep
        } else if (status.stripType === 'head' && newType === 'tail') {
          status.stripType = 'body';
        } else if (status.stripType === 'tail' && newType === 'head') {
          status.stripType = 'body';
        } else {
          status.stripType = newType;
        }

        current.setDate(current.getDate() + 1);
        daysCount++;
      }
    });

    return taskMap;
  }

  // 辅助方法：生成统一的 Key (yyyy-m-d)
  private formatDateKey(date: Date): string {
    return `${date.getFullYear()}-${date.getMonth() + 1}-${date.getDate()}`;
  }

  /**
   * 获取下一月
   */
  public getNextMonth(year: number, month: number): MonthInfo {
    let nextMonth = month + 1;
    let nextYear = year;
    if (nextMonth > 12) {
      nextMonth = 1;
      nextYear++;
    }
    return new MonthInfo(nextYear, nextMonth);
  }

  /**
   * 获取上一月
   */
  public getPrevMonth(year: number, month: number): MonthInfo {
    let prevMonth = month - 1;
    let prevYear = year;
    if (prevMonth < 1) {
      prevMonth = 12;
      prevYear--;
    }
    return new MonthInfo(prevYear, prevMonth);
  }
}