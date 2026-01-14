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
import { DebugCase } from './DebugCase';
import { EventModel, Event } from '../model/EventModel';
import { DateUtils } from '../common/utils/DateUtils';

/**
 * 日程算法逻辑测试
 * 场景：验证不同时间段的重叠检测算法、状态判断逻辑
 */
export class EventAlgoTestCase extends DebugCase {
  readonly name = 'Event Algorithm & Logic Test';

  async run(): Promise<void> {
    this.logInfo('=== EventAlgoTestCase START ===');

    const now = Date.now();
    const oneHour = 3600 * 1000;

    // 1. 测试状态判断逻辑 (Past/Current/Future)
    this.logInfo('1. Testing Event Status Logic...');

    const pastEvent = new Event('Past', now - 5 * oneHour, now - 4 * oneHour);
    const currentEvent = new Event('Current', now - oneHour, now + oneHour);
    const futureEvent = new Event('Future', now + 2 * oneHour, now + 3 * oneHour);

    this.checkStatus(pastEvent, '已结束');
    this.checkStatus(currentEvent, '进行中');
    this.checkStatus(futureEvent, '未开始');

    // 2. 测试冲突检测算法 (手动模拟)
    this.logInfo('2. Testing Conflict Detection Algorithm...');
    // 假设基准时间段：10:00 - 12:00
    const baseStart = 1000;
    const baseEnd = 2000;

    // Case A: 08:00 - 09:00 (无冲突)
    this.checkConflict(baseStart, baseEnd, 800, 900, false, 'Before range');

    // Case B: 13:00 - 14:00 (无冲突)
    this.checkConflict(baseStart, baseEnd, 3000, 4000, false, 'After range');

    // Case C: 11:00 - 13:00 (冲突)
    this.checkConflict(baseStart, baseEnd, 1100, 3000, true, 'Overlap End');

    // Case D: 09:00 - 11:00 (冲突)
    this.checkConflict(baseStart, baseEnd, 900, 1100, true, 'Overlap Start');

    // Case E: 10:30 - 11:30 (冲突 - 包含)
    this.checkConflict(baseStart, baseEnd, 1050, 1150, true, 'Contained');

    // 3. 数据库查询排序验证
    this.logInfo('3. Verifying DB Query Sorting...');
    const allEvents = await EventModel.queryUpcoming();
    if (allEvents.length >= 2) {
      const first = allEvents[0];
      const second = allEvents[1];
      if (first.startTime <= second.startTime) {
        this.logInfo(`Sorting Check Passed: ${first.startTime} <= ${second.startTime}`);
      } else {
        this.logInfo(`Sorting Check Failed: List is not sorted by start time.`);
      }
    } else {
      this.logInfo(' Not enough events to check sorting.');
    }

    this.logInfo('=== EventAlgoTestCase END ===');
  }

  // 辅助函数：状态检查
  private checkStatus(event: Event, expectedLabel: string) {
    const now = Date.now();
    let status = '';
    if (now > event.endTime) status = '已结束';
    else if (now >= event.startTime) status = '进行中';
    else status = '未开始';

    if (status === expectedLabel) {
      this.logInfo(`   Status OK: [${event.title}] is ${status}`);
    } else {
      this.logInfo(`   Status Error: [${event.title}] expected ${expectedLabel}, got ${status}`);
    }
  }

  // 辅助函数：冲突检查算法
  private checkConflict(baseStart: number, baseEnd: number, testStart: number, testEnd: number, expected: boolean, tag: string) {
    // 核心算法：Max(start1, start2) < Min(end1, end2)
    const overlapStart = Math.max(baseStart, testStart);
    const overlapEnd = Math.min(baseEnd, testEnd);
    const isConflict = overlapStart < overlapEnd;

    if (isConflict === expected) {
      this.logInfo(`   Algo OK (${tag}): Result=${isConflict}`);
    } else {
      this.logInfo(`   Algo Fail (${tag}): Expected ${expected}, got ${isConflict}`);
    }
  }
}