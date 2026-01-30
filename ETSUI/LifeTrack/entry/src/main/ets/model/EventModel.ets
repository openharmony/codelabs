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

// Event.ts
export interface CalendarEvent {
  id: string;           // 主键
  title: string;         // 标题
  description: string;  // 描述
  startTime: number;     // 开始时间戳
  endTime: number;       // 结束时间戳
  allDay: boolean;       // 全天事件
  location: string;     // 地点
  calendarId: string;    // 日历分类
  color: string;         // 颜色标记
  reminder: number;      // 提醒时间（分钟）
  repeatRule: string;   // 重复规则
  completed: boolean;   // 完成情况
  created: number;       // 创建时间
  updated: number;       // 更新时间
}
