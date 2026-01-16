/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
