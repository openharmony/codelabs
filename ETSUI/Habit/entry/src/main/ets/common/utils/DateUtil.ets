export class DateUtil {

  /**
   * 获取当前毫秒时间戳
   */
  static now(): number {
    return new Date().getTime();
  }

  /**
   * 核心：格式化日期为 YYYY-MM-DD 字符串
   * 修复了补0逻辑，确保是 2023-01-05 而不是 2023-1-5
   */
  static formatDate(date: number | Date): string {
    if (!date) return ''; // 安全保护

    let d = new Date(date);
    let year = d.getFullYear();
    let month = d.getMonth() + 1;
    let day = d.getDate();

    let monthStr = month < 10 ? '0' + month : '' + month;
    let dayStr = day < 10 ? '0' + day : '' + day;

    return `${year}-${monthStr}-${dayStr}`;
  }

  /**
   * 获取友好的时间显示
   */
  static getFriendlyDate(timestamp: number): string {
    let target = new Date(timestamp);
    let now = new Date();

    if (DateUtil.isSameDay(target, now)) {
      return '今天';
    }

    let diff = now.getTime() - timestamp;
    let oneDay = 24 * 60 * 60 * 1000;

    if (diff < oneDay && diff > 0) {
      return '昨天';
    }

    return DateUtil.formatDate(timestamp);
  }

  /**
   * 判断是否是同一天
   */
  static isSameDay(d1: Date, d2: Date): boolean {
    return d1.getFullYear() === d2.getFullYear() &&
      d1.getMonth() === d2.getMonth() &&
      d1.getDate() === d2.getDate();
  }

  /**
   * 获取本周开始的时间戳
   */
  static getStartOfWeek(): number {
    const now = new Date();
    const day = now.getDay() || 7;
    now.setDate(now.getDate() - day + 1);
    now.setHours(0, 0, 0, 0);
    return now.getTime();
  }
}