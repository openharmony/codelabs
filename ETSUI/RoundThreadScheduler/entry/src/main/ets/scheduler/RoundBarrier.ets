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
  SHARED_INDEX_BARRIER
} from '../constants/Config'

export class RoundBarrier {
  constructor(private readonly shared: Int32Array) {}

  /**
   * Enter the barrier and wait until all workers arrive
   */
  await(): void {
    const previous = Atomics.add(
      this.shared,
      SHARED_INDEX_BARRIER,
      1
    )

    if (previous + 1 === THREAD_COUNT) {
      // Last arriving worker wakes up all others
      Atomics.notify(
        this.shared,
        SHARED_INDEX_BARRIER,
        THREAD_COUNT
      )
    } else {
      // Wait until barrier is released
      Atomics.wait(
        this.shared,
        SHARED_INDEX_BARRIER,
        previous + 1
      )
    }
  }

  /**
   * Reset barrier before next round
   */
  reset(): void {
    Atomics.store(this.shared, SHARED_INDEX_BARRIER, 0)
  }
}
