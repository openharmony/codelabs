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


import worker from '@ohos.worker'
import { THREAD_COUNT, ROUND_COUNT } from '../constants/Config'
import { Logger } from '../utils/Logger'

export class ThreadManager {
  private workers: worker.Worker[] = []

  constructor(
    private readonly sharedBuffer: SharedArrayBuffer
  ) {}

  createWorkers(): void {
    Logger.info('ThreadManager', 'Creating worker threads')

    for (let i = 0; i < THREAD_COUNT; i++) {
      const w = new worker.Worker(
        'workers/IncrementWorker.ets',
        {
          workerData: {
            id: i,
            maxRounds: ROUND_COUNT,
            sharedBuffer: this.sharedBuffer
          }
        }
      )

      w.onmessage = (msg) => {
        Logger.info(
          'ThreadManager',
          `Worker ${msg.id} finished`
        )
      }

      this.workers.push(w)
    }
  }

  terminateAll(): void {
    Logger.warn(
      'ThreadManager',
      'Terminating all workers'
    )

    this.workers.forEach(w => {
      w.terminate()
    })
    this.workers = []
  }
}
