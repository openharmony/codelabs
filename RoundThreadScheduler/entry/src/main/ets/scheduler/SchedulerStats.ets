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
 * SchedulerStats.ets
 *
 * 调度执行过程中的统计信息
 * 用于 UI 展示 / 教学说明
 */

export class SchedulerStats {
  private roundResults: number[] = [];
  private startTime: number = 0;
  private endTime: number = 0;

  onStart(): void {
    this.roundResults = [];
    this.startTime = Date.now();
    this.endTime = 0;
  }

  recordRoundResult(roundIndex: number, incrementSum: number): void {
    this.roundResults[roundIndex] = incrementSum;
  }

  onFinish(): void {
    this.endTime = Date.now();
  }

  getTotalIncrement(): number {
    let sum = 0;
    for (let i = 0; i < this.roundResults.length; i++) {
      sum += this.roundResults[i];
    }
    return sum;
  }

  getElapsedTimeMs(): number {
    if (this.endTime === 0) {
      return 0;
    }
    return this.endTime - this.startTime;
  }

  getRoundResults(): number[] {
    return this.roundResults.slice();
  }
}
