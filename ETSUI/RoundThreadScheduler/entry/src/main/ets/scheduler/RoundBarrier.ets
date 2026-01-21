/**
 * RoundBarrier implements a reusable barrier
 * ensuring that all workers complete a round
 * before the next round begins.
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
