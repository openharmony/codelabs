
export class TimeUtil {
  static pad2(n: number): string {
    return n < 10 ? `0${n}` : String(n);
  }

  static formatTime(ms: number): string {
    const d = new Date(ms);
    const yyyy = d.getFullYear();
    const mm = TimeUtil.pad2(d.getMonth() + 1);
    const dd = TimeUtil.pad2(d.getDate());
    const hh = TimeUtil.pad2(d.getHours());
    const mi = TimeUtil.pad2(d.getMinutes());
    const ss = TimeUtil.pad2(d.getSeconds());
    return `${yyyy}-${mm}-${dd} ${hh}:${mi}:${ss}`;
  }

  static daysAgo(ms: number): number {
    const now = Date.now();
    const diff = now - ms;
    return Math.floor(diff / (24 * 3600 * 1000));
  }
}
