/**
 * SharedState encapsulates SharedArrayBuffer access.
 * All atomic operations must go through this abstraction.
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
