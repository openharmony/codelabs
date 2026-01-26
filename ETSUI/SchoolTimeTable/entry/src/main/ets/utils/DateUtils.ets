export class DateUtils {
  static getCurrentWeekDay(): number {
    const date = new Date();
    let day = date.getDay();
    if (day === 0) {
      return 7;
    }
    return day;
  }

  static getCurrentWeekNumber(startDate: string): number {
    const start = new Date(startDate);
    const now = new Date();
    const diffTime = now.getTime() - start.getTime();
    const diffDays = Math.floor(diffTime / (1000 * 60 * 60 * 24));
    const weekNumber = Math.floor(diffDays / 7) + 1;
    return weekNumber > 0 ? weekNumber : 1;
  }

  // 修改方法签名和逻辑
  static isCurrentWeek(currentWeek: number, startWeek: number, endWeek: number): boolean {
    // 只要当前周在开始和结束周之间即可，不再判断单双
    return currentWeek >= startWeek && currentWeek <= endWeek;
  }

  static getCurrentDateString(): string {
    const date = new Date();
    const year = date.getFullYear();
    const month = date.getMonth() + 1;
    const day = date.getDate();
    return `${year}-${month.toString().padStart(2, '0')}-${day.toString().padStart(2, '0')}`;
  }

  static formatDate(dateStr: string): string {
    const date = new Date(dateStr);
    const year = date.getFullYear();
    const month = date.getMonth() + 1;
    const day = date.getDate();
    return `${year}年${month}月${day}日`;
  }
}
