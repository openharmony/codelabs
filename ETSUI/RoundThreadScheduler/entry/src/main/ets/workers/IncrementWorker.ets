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
 * IncrementWorker.ets
 *
 * 并发执行单元（Worker）
 * 每个 worker 在一次 round 中执行一次 +1 逻辑
 * -  @Concurrent 普通函数
 * - 不能是 class 方法
 * - 不能返回 Object
 * - 不访问任何共享变量
 */


/**
 * IncrementWorker.ets
 *
 * 单个“工作任务”的执行逻辑
 * 不依赖 taskpool / @Concurrent
 * 作为被调度执行的普通函数
 */

export class IncrementWorker {
  private workerId: number;

  constructor(workerId: number) {
    this.workerId = workerId;
  }

  /**
   * 执行一次累加任务
   * @param roundIndex 当前轮次
   * @returns Promise<number> 返回本次累加的值
   */
  run(roundIndex: number): Promise<number> {
    return new Promise((resolve) => {
      // 使用 setTimeout 模拟异步并发执行
      setTimeout(() => {
        // 模拟计算耗时
        let mockCompute = 0;
        for (let i = 0; i < 10000; i++) {
          mockCompute += i;
        }

        // 每个 worker 每轮只做一次 +1
        resolve(1);
      }, Math.random() * 50);
    });
  }

  getId(): number {
    return this.workerId;
  }
}
