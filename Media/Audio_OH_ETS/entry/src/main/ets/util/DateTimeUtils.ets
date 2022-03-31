/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
 * @file: 日期工具
 */

export default class DateTimeUtil {

  /**
   * 时分秒
   *
   * @return {string} - 返回时分秒
   */
  static getTime() {
    const DATETIME = new Date();
    const HOURS = DATETIME.getHours();
    const MINUTES = DATETIME.getMinutes();
    const SECONDS = DATETIME.getSeconds();
    return this.concatTime(HOURS, MINUTES, SECONDS);
  }

  /**
   * 年月日
   *
   * @return {string} - 返回年月日
   */
  static getDate() {
    const DATETIME = new Date();
    const YEAR = DATETIME.getFullYear();
    const MONTH = DATETIME.getMonth() + 1;
    const DAY = DATETIME.getDate();
    return this.concatDate(YEAR, MONTH, DAY);
  }

  /**
   * 日期不足两位补 0
   *
   * @param {string} value - 数据值
   * @return {string} - 日期不足两位补 0
   */
  static fill(value) {
    return (value > 9 ? '' : '0') + value;
  }

  /**
   * 年月日格式修饰
   *
   * @param {string} year - 年
   * @param {string} month - 月
   * @param {string} date - 日
   * @return {string} - 年月日格式修饰
   */
  static concatDate(year, month, date) {
    return `${year}${month}${date}`;
  }

  /**
   * 时分秒格式修饰
   *
   * @param {string} hours - 时
   * @param {string} minutes - 分
   * @param {string} seconds - 秒
   * @return {string} - 时分秒格式修饰
   */
  static concatTime(hours, minutes, seconds) {
    return `${this.fill(hours)}${this.fill(minutes)}${this.fill(seconds)}`;
  }

  static dateFormat(timestamp?) {
    const t = new Date(timestamp)
    let year = t.getFullYear()
    let month = t.getMonth() + 1
    let day = t.getDate()
    let hours = t.getHours()
    let minutes = t.getMinutes()
    let seconds = t.getSeconds()
    return year + "-" + this.fill(month) + "-" + this.fill(day) + " " + this.fill(hours) + ":" + this.fill(minutes) + ":" + this.fill(seconds);
  }
}
