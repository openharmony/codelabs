/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
@Observed
export default class Habit {
  id: number = -1;
  name: string = '';
  icon: string = 'app.media.icon_default';
  color: string = '#FFCC00';
  frequency: string = '1111111';
  currentCount: number = 0;
  targetCount: number = 1;
  createTime: number = new Date().getTime();
  isChecked: boolean = false;
  categoryId: number = 1; // 默认归属 ID=1 (全部)
  reminderTime: string = ''; // 默认为空，表示不提醒
  reminderId: number = -1;

  constructor(name: string, color: string, targetCount: number = 1, categoryId: number = 1) {
    this.name = name;
    this.color = color;
    this.targetCount = targetCount;
    this.categoryId = categoryId; // 🆕 赋值
  }
}