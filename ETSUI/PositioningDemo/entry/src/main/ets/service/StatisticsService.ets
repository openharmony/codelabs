/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { SportData, SportType } from '../model/SportModel';
import { UserStatistics, WeeklyStatistics, DailyRecord } from '../model/UserModel';
import { HistoryService } from './HistoryService';

export class StatisticsService {
  private historyService: HistoryService;

  constructor(historyService: HistoryService) {
    this.historyService = historyService;
  }

  // 计算用户总体统计数据
  async calculateUserStatistics(): Promise<UserStatistics> {
    const statistics = new UserStatistics();
    const records = await this.historyService.getSportRecords();

    console.log('[统计调试] 总记录数:', records.length);

    if (records.length === 0) {
      console.log('[统计调试] 没有运动记录');
      return statistics;
    }

    statistics.totalRecords = records.length;
    statistics.totalDistance = records.reduce((sum, r) => sum + r.distance, 0);
    statistics.totalDuration = records.reduce((sum, r) => sum + r.duration, 0);
    statistics.totalCalories = records.reduce((sum, r) => sum + r.calories, 0);

    console.log('[统计调试] 总次数:', statistics.totalRecords, '总距离:', statistics.totalDistance);
    console.log('[统计调试] 总时长:', statistics.totalDuration, '总卡路里:', statistics.totalCalories);

    if (statistics.totalDuration > 0) {
      const totalDurationInSeconds = statistics.totalDuration / 1000;
      statistics.averageSpeed = statistics.totalDistance / totalDurationInSeconds;
    }

    statistics.maxSpeed = Math.max(...records.map(r => r.averageSpeed || 0));
    statistics.longestDistance = Math.max(...records.map(r => r.distance || 0));
    statistics.longestDuration = Math.max(...records.map(r => r.duration || 0));

    records.forEach(record => {
      switch (record.type) {
        case SportType.RUNNING:
          statistics.runningCount++;
          statistics.runningDistance += record.distance;
          break;
        case SportType.WALKING:
          statistics.walkingCount++;
          statistics.walkingDistance += record.distance;
          break;
        case SportType.CYCLING:
          statistics.cyclingCount++;
          statistics.cyclingDistance += record.distance;
          break;
      }
    });

    console.log('[统计调试] 跑步:', statistics.runningCount, '次', statistics.runningDistance, '米');
    console.log('[统计调试] 行走:', statistics.walkingCount, '次', statistics.walkingDistance, '米');
    console.log('[统计调试] 骑行:', statistics.cyclingCount, '次', statistics.cyclingDistance, '米');

    return statistics;
  }

  // 计算本周统计数据
  async calculateWeeklyStatistics(): Promise<WeeklyStatistics> {
    const weekStats = new WeeklyStatistics();
    const records = await this.historyService.getSportRecords();

    console.log('[周统计调试] 总记录数:', records.length);

    const now = new Date();
    const dayOfWeek = now.getDay();
    const mondayOffset = dayOfWeek === 0 ? -6 : 1 - dayOfWeek;

    const monday = new Date(now);
    monday.setDate(now.getDate() + mondayOffset);
    monday.setHours(0, 0, 0, 0);

    const sunday = new Date(monday);
    sunday.setDate(monday.getDate() + 6);
    sunday.setHours(23, 59, 59, 999);

    weekStats.startDate = monday;
    weekStats.endDate = sunday;

    const year = monday.getFullYear();
    const weekNumber = this.getWeekNumber(monday);
    weekStats.weekLabel = `${year}年第${weekNumber}周`;

    for (let i = 0; i < 7; i++) {
      const day = new Date(monday);
      day.setDate(monday.getDate() + i);

      const dailyRecord = new DailyRecord();
      dailyRecord.date = day;
      dailyRecord.dayLabel = this.getDayLabel(i);
      weekStats.dailyRecords.push(dailyRecord);
    }

    const weekRecords = records.filter(record => {
      const recordTime = record.startTime;
      return recordTime >= monday.getTime() && recordTime <= sunday.getTime();
    });

    console.log('[周统计调试] 本周记录数:', weekRecords.length);

    weekStats.totalRecords = weekRecords.length;
    weekStats.totalDistance = weekRecords.reduce((sum, r) => sum + r.distance, 0);
    weekStats.totalDuration = weekRecords.reduce((sum, r) => sum + r.duration, 0);
    weekStats.totalCalories = weekRecords.reduce((sum, r) => sum + r.calories, 0);

    console.log('[周统计调试] 本周总距离:', weekStats.totalDistance, '总时长:', weekStats.totalDuration);
    console.log('[周统计调试] 本周总卡路里:', weekStats.totalCalories, '总次数:', weekStats.totalRecords);

    weekRecords.forEach(record => {
      const recordDate = new Date(record.startTime);
      const dayIndex = this.getDayIndex(recordDate, monday);

      if (dayIndex >= 0 && dayIndex < 7) {
        weekStats.dailyRecords[dayIndex].records++;
        weekStats.dailyRecords[dayIndex].distance += record.distance;
        weekStats.dailyRecords[dayIndex].duration += record.duration;
        weekStats.dailyRecords[dayIndex].calories += record.calories;
      }
    });

    return weekStats;
  }

  // 计算最近7天的运动趋势
  async calculateRecentTrend(days: number = 7): Promise<DailyRecord[]> {
    const records = await this.historyService.getSportRecords();
    const trendData: DailyRecord[] = [];

    const now = new Date();
    for (let i = days - 1; i >= 0; i--) {
      const date = new Date(now);
      date.setDate(now.getDate() - i);
      date.setHours(0, 0, 0, 0);

      const nextDay = new Date(date);
      nextDay.setDate(date.getDate() + 1);

      const dailyRecord = new DailyRecord();
      dailyRecord.date = date;
      dailyRecord.dayLabel = this.formatDateShort(date);

      const dayRecords = records.filter(record => {
        const recordTime = record.startTime;
        return recordTime >= date.getTime() && recordTime < nextDay.getTime();
      });

      dailyRecord.records = dayRecords.length;
      dailyRecord.distance = dayRecords.reduce((sum, r) => sum + r.distance, 0);
      dailyRecord.duration = dayRecords.reduce((sum, r) => sum + r.duration, 0);
      dailyRecord.calories = dayRecords.reduce((sum, r) => sum + r.calories, 0);

      trendData.push(dailyRecord);
    }

    console.log(`计算最近${days}天趋势完成:`, trendData);
    return trendData;
  }

  // 计算运动频率（每周平均运动几次）
  async calculateWeeklyFrequency(): Promise<number> {
    const records = await this.historyService.getSportRecords();
    if (records.length === 0) {
      return 0;
    }

    const sortedRecords = records.sort((a, b) => a.startTime - b.startTime);
    const firstTime = sortedRecords[0].startTime;
    const lastTime = sortedRecords[sortedRecords.length - 1].startTime;

    const timeSpanInDays = (lastTime - firstTime) / (1000 * 60 * 60 * 24);
    const timeSpanInWeeks = Math.max(timeSpanInDays / 7, 1);

    const weeklyFrequency = records.length / timeSpanInWeeks;
    console.log('计算运动频率:', weeklyFrequency, '次/周');
    return weeklyFrequency;
  }

  // 计算运动活跃度（最近30天的运动天数）
  async calculateActivityDays(days: number = 30): Promise<number> {
    const records = await this.historyService.getSportRecords();
    const now = new Date();
    const startDate = new Date(now);
    startDate.setDate(now.getDate() - days);
    startDate.setHours(0, 0, 0, 0);

    const recentRecords = records.filter(record => record.startTime >= startDate.getTime());

    const activityDates = new Set<string>();
    recentRecords.forEach(record => {
      const date = new Date(record.startTime);
      const dateKey = `${date.getFullYear()}-${date.getMonth()}-${date.getDate()}`;
      activityDates.add(dateKey);
    });

    const activityDays = activityDates.size;
    console.log(`最近${days}天运动了${activityDays}天`);
    return activityDays;
  }

  // 计算距离排名（返回前N次距离最长的运动记录）
  async getTopDistanceRecords(count: number = 5): Promise<SportData[]> {
    const records = await this.historyService.getSportRecords();
    const sorted = records.sort((a, b) => b.distance - a.distance);
    return sorted.slice(0, count);
  }

  private getWeekNumber(date: Date): number {
    const firstDayOfYear = new Date(date.getFullYear(), 0, 1);
    const pastDaysOfYear = (date.getTime() - firstDayOfYear.getTime()) / (1000 * 60 * 60 * 24);
    return Math.ceil((pastDaysOfYear + firstDayOfYear.getDay() + 1) / 7);
  }

  private getDayLabel(dayIndex: number): string {
    const labels = ['周一', '周二', '周三', '周四', '周五', '周六', '周日'];
    return labels[dayIndex] || '';
  }

  private getDayIndex(date: Date, monday: Date): number {
    const diffTime = date.getTime() - monday.getTime();
    const diffDays = Math.floor(diffTime / (1000 * 60 * 60 * 24));
    return diffDays;
  }

  private formatDateShort(date: Date): string {
    const month = date.getMonth() + 1;
    const day = date.getDate();
    return `${month}/${day}`;
  }
}
