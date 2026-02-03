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
 * RoundThreadScheduler.ets
 *
 * 调度总控层：
 * - 对外暴露 start() 方法
 * - 内部使用 RoundExecutor 执行轮次调度
 * - 不涉及 taskpool / @Concurrent
 */

import { RoundExecutor } from './RoundExecutor';

export class RoundThreadScheduler {
  private executor: RoundExecutor | null = null;

  private workerCount: number;
  private roundCount: number;

  private running: boolean = false;
  private finalResult: number = 0;

  constructor(workerCount: number, roundCount: number) {
    this.workerCount = workerCount;
    this.roundCount = roundCount;
  }

  /**
   * 启动调度流程
   */
  async start(): Promise<number> {
    if (this.running) {
      // 防止重复启动
      return this.finalResult;
    }

    this.running = true;

    this.executor = new RoundExecutor(
      this.workerCount,
      this.roundCount
    );

    this.finalResult = await this.executor.start();

    this.running = false;
    return this.finalResult;
  }

  /**
   * 是否正在执行
   */
  isRunning(): boolean {
    return this.running;
  }

  /**
   * 获取最终执行结果
   */
  getResult(): number {
    return this.finalResult;
  }
}

