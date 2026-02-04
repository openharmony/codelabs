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
 * SchedulerConfig.ets
 *
 * 调度器配置定义与校验逻辑
 * 说明：
 * - 所有并发相关参数集中在此处
 * - 不涉及 taskpool
 * - 仅提供只读配置与校验方法
 */

export class SchedulerConfig {
  readonly totalRounds: number;
  readonly workersPerRound: number;

  constructor(totalRounds: number = 10, workersPerRound: number = 10) {
    this.totalRounds = totalRounds;
    this.workersPerRound = workersPerRound;
    this.validate();
  }

  /**
   * 参数合法性校验
   * - 轮次必须为正整数
   * - 每轮 worker 数必须为正整数
   */
  private validate(): void {
    if (!Number.isInteger(this.totalRounds) || this.totalRounds <= 0) {
      throw new Error('totalRounds must be a positive integer');
    }
    if (!Number.isInteger(this.workersPerRound) || this.workersPerRound <= 0) {
      throw new Error('workersPerRound must be a positive integer');
    }
  }
}
