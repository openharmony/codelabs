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
import {
  SHARED_INDEX_VALUE,
  SHARED_INDEX_ROUND,
  SHARED_INDEX_BARRIER
} from '../constants/Config'

const data = worker.workerData
const shared = new Int32Array(data.sharedBuffer)
const workerId: number = data.id
const maxRounds: number = data.maxRounds

function log(message: string): void {
  console.info(`[Worker ${workerId}] ${message}`)
}

function run(): void {
  let lastRound = -1

  while (true) {
    const currentRound = Atomics.load(shared, SHARED_INDEX_ROUND)

    if (currentRound === lastRound) {
      Atomics.wait(shared, SHARED_INDEX_ROUND, currentRound)
      continue
    }

    if (currentRound >= maxRounds) {
      break
    }

    lastRound = currentRound

    // Atomic increment
    Atomics.add(shared, SHARED_INDEX_VALUE, 1)
    log(`Round ${currentRound} incremented value`)

    // Barrier arrival
    const count = Atomics.add(shared, SHARED_INDEX_BARRIER, 1)

    if (count + 1 === 10) {
      Atomics.notify(shared, SHARED_INDEX_BARRIER, 10)
    } else {
      Atomics.wait(shared, SHARED_INDEX_BARRIER, count + 1)
    }
  }

  worker.parentPort?.postMessage({
    id: workerId,
    status: 'done'
  })
}

run()
