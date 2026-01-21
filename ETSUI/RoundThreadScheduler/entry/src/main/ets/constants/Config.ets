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