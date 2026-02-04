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
 * RoundExecutor.ets
 *
 * 负责：
 * 1. 控制“轮次”执行
 * 2. 确保一轮内所有 worker 完成后再进入下一轮
 */

import { IncrementWorker } from '../workers/IncrementWorker';

export class RoundExecutor {
  private workers: IncrementWorker[] = [];
  private roundCount: number;
  private workerCount: number;
  private sharedValue: number = 0;

  constructor(workerCount: number, roundCount: number) {
    this.workerCount = workerCount;
    this.roundCount = roundCount;

    for (let i = 0; i < workerCount; i++) {
      this.workers.push(new IncrementWorker(i));
    }
  }

  /**
   * 启动所有轮次
   */
  async start(): Promise<number> {
    for (let round = 0; round < this.roundCount; round++) {
      await this.executeOneRound(round);
    }
    return this.sharedValue;
  }

  /**
   * 执行单轮
   * @param roundIndex 当前轮次
   */
  private async executeOneRound(roundIndex: number): Promise<void> {
    const tasks: Promise<number>[] = [];

    for (let i = 0; i < this.workerCount; i++) {
      const worker = this.workers[i];
      tasks.push(worker.run(roundIndex));
    }

    // 等待本轮所有 worker 完成（轮次屏障）
    const results = await Promise.all(tasks);

    // 串行更新共享数据，避免竞态
    results.forEach((value) => {
      this.sharedValue += value;
    });
  }

  /**
   * 获取最终结果
   */
  getResult(): number {
    return this.sharedValue;
  }
}
