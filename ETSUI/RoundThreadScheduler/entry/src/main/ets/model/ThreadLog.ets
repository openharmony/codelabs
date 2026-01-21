/**
 * ThreadLog
 *
 * Simple data structure for UI log rendering
 */

export interface ThreadLogItem {
  timestamp: number
  threadId: number
  round: number
  message: string
}

export class ThreadLog {
  private logs: ThreadLogItem[] = []

  add(
    threadId: number,
    round: number,
    message: string
  ): void {
    this.logs.push({
      timestamp: Date.now(),
      threadId,
      round,
      message
    })
  }

  getAll(): ThreadLogItem[] {
    return this.logs
  }

  clear(): void {
    this.logs.length = 0
  }
}
