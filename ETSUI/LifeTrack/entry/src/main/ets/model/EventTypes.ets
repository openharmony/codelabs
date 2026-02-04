/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// model/EventTypes.ts
export interface ReminderOption {
  label: string;
  minutes: number;
}

export interface RepeatOption {
  label: string;
  value: string;
  type: 'none' | 'daily' | 'weekly' | 'monthly' | 'yearly';
}

export interface RepeatRule {
  type: 'none' | 'daily' | 'weekly' | 'monthly' | 'yearly';
  interval: number;
  endType: 'never' | 'count' | 'date';
  endCount?: number;
  endDate?: number;
  weekdays?: number[];
}

export interface EventInstance {
  id: string;
  originalId: string;
  title: string;
  description: string;
  instanceTime: number;
  completed: boolean;
}