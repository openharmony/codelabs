/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

export enum EventType {
  BATTERY = 'BATTERY',
  NETWORK = 'NETWORK',
  STORAGE = 'STORAGE',
  SCREEN = 'SCREEN',
}

export enum EventLevel {
  INFO = 'INFO',
  WARN = 'WARN',
  ERROR = 'ERROR',
}

export interface EventLog {
  id?: number;
  type: EventType;
  level: EventLevel;
  timestamp: number; // ms
  message: string;
  payload?: Record<string, any>;
  source?: string;
}

export function nowMs(): number {
  return Date.now();
}

export function stablePayloadString(payload?: Record<string, any>): string {
  if (!payload) return '';
  const keys = Object.keys(payload).sort();
  const normalized: Record<string, any> = {};
  for (const k of keys) normalized[k] = payload[k];
  try {
    return JSON.stringify(normalized);
  } catch (_) {
    return '';
  }
}
