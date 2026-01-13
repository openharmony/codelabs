import { hilog } from '@kit.PerformanceAnalysisKit';

const DOMAIN: number = 0x0101;
const TAG: string = 'RecorderApp';

export class Logger {
  static info(message: string, data?: string): void {
    if (data) {
      hilog.info(DOMAIN, TAG, '%{public}s %{public}s', message, data);
      return;
    }
    hilog.info(DOMAIN, TAG, '%{public}s', message);
  }

  static warn(message: string, data?: string): void {
    if (data) {
      hilog.warn(DOMAIN, TAG, '%{public}s %{public}s', message, data);
      return;
    }
    hilog.warn(DOMAIN, TAG, '%{public}s', message);
  }

  static error(message: string, data?: string): void {
    if (data) {
      hilog.error(DOMAIN, TAG, '%{public}s %{public}s', message, data);
      return;
    }
    hilog.error(DOMAIN, TAG, '%{public}s', message);
  }
}
