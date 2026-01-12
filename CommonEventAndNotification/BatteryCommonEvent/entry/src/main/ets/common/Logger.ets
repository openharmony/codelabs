/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
