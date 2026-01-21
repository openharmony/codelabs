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
  SHARED_ARRAY_SIZE,
  SHARED_INDEX_VALUE,
  SHARED_INDEX_ROUND,
  SHARED_INDEX_BARRIER
} from '../constants/Config'

export class SharedState {
  private readonly buffer: SharedArrayBuffer
  private readonly array: Int32Array

  constructor() {
    this.buffer = new SharedArrayBuffer(
      Int32Array.BYTES_PER_ELEMENT * SHARED_ARRAY_SIZE
    )
    this.array = new Int32Array(this.buffer)
    this.initialize()
  }

  /**
   * Initialize all shared values to zero
   */
  private initialize(): void {
    for (let i = 0; i < this.array.length; i++) {
      Atomics.store(this.array, i, 0)
    }
  }

  getBuffer(): SharedArrayBuffer {
    return this.buffer
  }

  getArray(): Int32Array {
    return this.array
  }

  getValue(): number {
    return Atomics.load(this.array, SHARED_INDEX_VALUE)
  }

  incrementValue(): number {
    return Atomics.add(this.array, SHARED_INDEX_VALUE, 1)
  }

  getCurrentRound(): number {
    return Atomics.load(this.array, SHARED_INDEX_ROUND)
  }

  setCurrentRound(round: number): void {
    Atomics.store(this.array, SHARED_INDEX_ROUND, round)
  }

  resetBarrier(): void {
    Atomics.store(this.array, SHARED_INDEX_BARRIER, 0)
  }

  increaseBarrier(): number {
    return Atomics.add(this.array, SHARED_INDEX_BARRIER, 1)
  }

  getBarrierCount(): number {
    return Atomics.load(this.array, SHARED_INDEX_BARRIER)
  }
}
