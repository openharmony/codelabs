import { hilog } from '@kit.PerformanceAnalysisKit';

const DOMAIN = 0x0000;
const TAG = 'BatteryMonitor';

export type LogArg = string | number | boolean | null | undefined | Object;

export class Logger {
  static d(msg: string, ...args: LogArg[]): void {
    hilog.debug(DOMAIN, TAG, Logger.fmt(msg, args));
  }

  static i(msg: string, ...args: LogArg[]): void {
    hilog.info(DOMAIN, TAG, Logger.fmt(msg, args));
  }

  static w(msg: string, ...args: LogArg[]): void {
    hilog.warn(DOMAIN, TAG, Logger.fmt(msg, args));
  }

  static e(msg: string, ...args: LogArg[]): void {
    hilog.error(DOMAIN, TAG, Logger.fmt(msg, args));
  }

  private static fmt(msg: string, args: LogArg[]): string {
    if (!args || args.length === 0) {
      return msg;
    }
    const safeArgs: string[] = [];
    for (let i = 0; i < args.length; i++) {
      const a = args[i];
      try {
        if (a === null || a === undefined) {
          safeArgs.push(String(a));
        } else if (typeof a === 'string' || typeof a === 'number' || typeof a === 'boolean') {
          safeArgs.push(String(a));
        } else {
          // Avoid JSON stringify throwing for circular objects
          safeArgs.push(JSON.stringify(a));
        }
      } catch {
        safeArgs.push(String(a));
      }
    }
    return `${msg} ${safeArgs.join(' ')}`;
  }
}
