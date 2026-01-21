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

import {
  THREAD_COUNT,
  ROUND_COUNT,
  SHARED_INDEX_ROUND,
  SHARED_INDEX_BARRIER
} from '../constants/Config'
import { SharedState } from '../model/SharedState'
import { ThreadManager } from './ThreadManager'
import { Logger } from '../utils/Logger'

export class RoundScheduler {
  private manager: ThreadManager
  private array: Int32Array

  constructor(
    private readonly shared: SharedState
  ) {
    this.manager = new ThreadManager(
      shared.getBuffer()
    )
    this.array = shared.getArray()
  }

  start(): void {
    Logger.info(
      'RoundScheduler',
      'Starting scheduler'
    )

    this.manager.createWorkers()
    this.executeRounds()
  }

  private executeRounds(): void {
    for (let round = 0; round < ROUND_COUNT; round++) {
      Logger.info(
        'RoundScheduler',
        `Starting round ${round}`
      )

      Atomics.store(
        this.array,
        SHARED_INDEX_BARRIER,
        0
      )

      Atomics.store(
        this.array,
        SHARED_INDEX_ROUND,
        round
      )

      Atomics.notify(
        this.array,
        SHARED_INDEX_ROUND,
        THREAD_COUNT
      )

      while (
        Atomics.load(
          this.array,
          SHARED_INDEX_BARRIER
        ) < THREAD_COUNT
      ) {
        // busy wait for barrier completion
      }

      Logger.info(
        'RoundScheduler',
        `Round ${round} completed`
      )
    }

    Logger.info(
      'RoundScheduler',
      `All rounds completed, final value = ${this.shared.getValue()}`
    )
  }
}
