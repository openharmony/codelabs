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
// entry/src/main/ets/common/utils/DateUtils.ets
import { Logger } from './Logger';

/**
 * 日期时间工具类
 * 提供常用的日期格式化、计算、转换等功能
 */
export class DateUtils {
  private static logger = new Logger('DateUtils');

  /**
   * 获取当前时间戳
   * @returns number 当前毫秒级时间戳
   */
  public static now(): number {
    // ❌ Fix: this.logger -> DateUtils.logger
    DateUtils.logger.info('DateUtils.now() called');
    return new Date().getTime();
  }

  /**
   * 格式化时间戳为 YYYY-MM-DD 格式
   * @param timestamp 时间戳
   * @returns string 格式化后的日期字符串
   */
  public static formatDate(timestamp: number): string {
    if (!timestamp || timestamp <= 0) {
      // ❌ Fix: this.logger -> DateUtils.logger
      DateUtils.logger.error('Invalid timestamp provided for formatDate');
      return '';
    }

    const date = new Date(timestamp);
    const year = date.getFullYear();
    const month = (date.getMonth() + 1).toString().padStart(2, '0');
    const day = date.getDate().toString().padStart(2, '0');

    const result = `${year}-${month}-${day}`;
    // ❌ Fix: this.logger -> DateUtils.logger
    DateUtils.logger.info(`Formatted date: ${result}`);
    return result;
  }

  /**
   * 格式化时间戳为 HH:mm 格式
   * @param timestamp 时间戳
   * @returns string 格式化后的时间字符串
   */
  public static formatTime(timestamp: number): string {
    if (!timestamp) {
      return '00:00';
    }
    const date = new Date(timestamp);
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    return `${hours}:${minutes}`;
  }

  /**
   * 格式化为完整时间 YYYY-MM-DD HH:mm:ss
   * @param timestamp
   */
  public static formatFull(timestamp: number): string {
    // ❌ Fix: this.formatDate -> DateUtils.formatDate
    const dateStr = DateUtils.formatDate(timestamp);
    const timeStr = DateUtils.formatTime(timestamp);
    const seconds = new Date(timestamp).getSeconds().toString().padStart(2, '0');
    return `${dateStr} ${timeStr}:${seconds}`;
  }

  /**
   * 获取今天的开始时间戳 (00:00:00)
   */
  public static getStartOfDay(timestamp: number = Date.now()): number {
    const date = new Date(timestamp);
    date.setHours(0, 0, 0, 0);
    return date.getTime();
  }

  /**
   * 获取今天的结束时间戳 (23:59:59)
   */
  public static getEndOfDay(timestamp: number = Date.now()): number {
    const date = new Date(timestamp);
    date.setHours(23, 59, 59, 999);
    return date.getTime();
  }

  /**
   * 判断是否是同一天
   */
  public static isSameDay(ts1: number, ts2: number): boolean {
    const d1 = new Date(ts1);
    const d2 = new Date(ts2);
    return d1.getFullYear() === d2.getFullYear() &&
      d1.getMonth() === d2.getMonth() &&
      d1.getDate() === d2.getDate();
  }

  /**
   * 获取本周第一天
   */
  public static getStartOfWeek(timestamp: number = Date.now()): number {
    const date = new Date(timestamp);
    const day = date.getDay();
    const diff = date.getDate() - day + (day === 0 ? -6 : 1);
    date.setDate(diff);
    date.setHours(0, 0, 0, 0);
    return date.getTime();
  }
}