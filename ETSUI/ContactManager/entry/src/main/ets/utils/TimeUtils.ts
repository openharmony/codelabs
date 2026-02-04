/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * 时间工具类
 * 用于格式化时间显示
 */
export class TimeUtils {
  /**
   * 获取今天的开始时间戳（00:00:00）
   */
  static getTodayStart(): number {
    const now = new Date();
    now.setHours(0, 0, 0, 0);
    return now.getTime();
  }

  /**
   * 获取昨天的开始时间戳
   */
  static getYesterdayStart(): number {
    const today = this.getTodayStart();
    return today - 24 * 60 * 60 * 1000;
  }

  /**
   * 获取本周的开始时间戳（周一 00:00:00）
   */
  static getWeekStart(): number {
    const now = new Date();
    const day = now.getDay(); // 0=周日, 1=周一, ...
    const diff = day === 0 ? 6 : day - 1; // 转换为周一为0
    now.setDate(now.getDate() - diff);
    now.setHours(0, 0, 0, 0);
    return now.getTime();
  }

  /**
   * 判断时间戳属于哪个时间段
   * @returns 'today' | 'yesterday' | 'thisWeek' | 'earlier'
   */
  static getTimeCategory(timestamp: number): 'today' | 'yesterday' | 'thisWeek' | 'earlier' {
    const now = Date.now();
    const todayStart = this.getTodayStart();
    const yesterdayStart = this.getYesterdayStart();
    const weekStart = this.getWeekStart();

    if (timestamp >= todayStart) {
      return 'today';
    } else if (timestamp >= yesterdayStart) {
      return 'yesterday';
    } else if (timestamp >= weekStart) {
      return 'thisWeek';
    } else {
      return 'earlier';
    }
  }

  /**
   * 格式化时间显示
   * @param timestamp 时间戳
   * @returns 格式化后的字符串
   */
  static formatTime(timestamp: number): string {
    if (!timestamp || timestamp === 0) {
      return '';
    }

    const category = this.getTimeCategory(timestamp);
    const date = new Date(timestamp);
    const now = new Date();
    const today = new Date(now.getFullYear(), now.getMonth(), now.getDate());
    const targetDate = new Date(date.getFullYear(), date.getMonth(), date.getDate());

    switch (category) {
      case 'today':
        // 今天：显示时间 HH:mm
        const hours = date.getHours().toString().padStart(2, '0');
        const minutes = date.getMinutes().toString().padStart(2, '0');
        return `今天 ${hours}:${minutes}`;

      case 'yesterday':
        return '昨天';

      case 'thisWeek':
        // 本周：显示星期几
        const weekDays = ['周日', '周一', '周二', '周三', '周四', '周五', '周六'];
        return weekDays[date.getDay()];

      case 'earlier':
        // 更早：显示日期 MM-DD 或 YYYY-MM-DD（跨年）
        const year = date.getFullYear();
        const month = (date.getMonth() + 1).toString().padStart(2, '0');
        const day = date.getDate().toString().padStart(2, '0');

        if (year === now.getFullYear()) {
          return `${month}-${day}`;
        } else {
          return `${year}-${month}-${day}`;
        }

      default:
        return '';
    }
  }

  /**
   * 获取时间段的中文名称
   */
  static getTimeCategoryName(category: 'today' | 'yesterday' | 'thisWeek' | 'earlier'): string {
    const names = {
      'today': '今天',
      'yesterday': '昨天',
      'thisWeek': '本周',
      'earlier': '更早'
    };
    return names[category];
  }
}