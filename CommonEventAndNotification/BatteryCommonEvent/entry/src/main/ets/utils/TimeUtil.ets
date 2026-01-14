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

  static formatForFile(ms: number): string {
    const d = new Date(ms);
    const yyyy = d.getFullYear();
    const mm = TimeUtil.pad2(d.getMonth() + 1);
    const dd = TimeUtil.pad2(d.getDate());
    const hh = TimeUtil.pad2(d.getHours());
    const mi = TimeUtil.pad2(d.getMinutes());
    const ss = TimeUtil.pad2(d.getSeconds());
    return `${yyyy}-${mm}-${dd}_${hh}-${mi}-${ss}`;
  }

  static daysAgo(ms: number): number {
    const now = Date.now();
    const diff = now - ms;
    return Math.floor(diff / (24 * 3600 * 1000));
  }
}
