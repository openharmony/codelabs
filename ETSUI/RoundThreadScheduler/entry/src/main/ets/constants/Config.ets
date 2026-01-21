/**
 * Global configuration constants
 * Centralized to avoid magic numbers
 */

export const THREAD_COUNT: number = 10
export const ROUND_COUNT: number = 10

/**
 * Shared memory layout:
 * index 0 -> shared value
 * index 1 -> current round
 * index 2 -> barrier counter
 */
export const SHARED_INDEX_VALUE = 0
export const SHARED_INDEX_ROUND = 1
export const SHARED_INDEX_BARRIER = 2

/**
 * Total number of Int32 elements
 */
export const SHARED_ARRAY_SIZE = 3

/**
 * Worker status flags
 */
export const WORKER_STATUS_INIT = 0
export const WORKER_STATUS_RUNNING = 1
export const WORKER_STATUS_DONE = 2

/**
 * Debug configuration
 */
export const ENABLE_LOG = true