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
/**
 * ExecutionRecord.ets
 *
 * 记录一次完整调度执行的信息
 */

import { RoundResult } from './RoundResult';
import { SchedulerState } from './SchedulerState';

export class ExecutionRecord {
  private readonly startTime: number;
  private endTime: number = 0;
  private state: SchedulerState = SchedulerState.IDLE;
  private roundResults: RoundResult[] = [];

  constructor() {
    this.startTime = Date.now();
  }

  setState(state: SchedulerState): void {
    this.state = state;
  }

  addRoundResult(result: RoundResult): void {
    this.roundResults.push(result);
  }

  finish(): void {
    this.endTime = Date.now();
    this.state = SchedulerState.FINISHED;
  }

  getState(): SchedulerState {
    return this.state;
  }

  getElapsedTimeMs(): number {
    if (this.endTime === 0) {
      return 0;
    }
    return this.endTime - this.startTime;
  }

  getRoundResults(): RoundResult[] {
    return this.roundResults.slice();
  }

  getTotalIncrement(): number {
    let total = 0;
    for (let i = 0; i < this.roundResults.length; i++) {
      total += this.roundResults[i].getIncrementSum();
    }
    return total;
  }
}
