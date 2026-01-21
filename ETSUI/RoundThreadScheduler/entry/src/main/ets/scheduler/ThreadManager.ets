/**
 * ThreadManager
 *
 * Responsible for creating and managing worker threads.
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
