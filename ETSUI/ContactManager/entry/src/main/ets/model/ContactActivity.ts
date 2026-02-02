// entry/src/main/ets/model/ContactActivity.ts
/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
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
import { Contact } from './Contact';
import { ContactMetadata } from './ContactMetadata';
import { TimeUtils } from '../utils/TimeUtils'

/**
 * 活动类型
 */
export enum ActivityType {
  VIEW = 'view',    // 查看
  EDIT = 'edit',   // 编辑
  CREATE = 'create' // 创建
}

/**
 * 最近活动联系人数据模型
 */
export class ContactActivity {
  contact: Contact;                    // 联系人信息
  metadata: ContactMetadata;           // 元数据
  activityType: ActivityType;          // 活动类型
  activityTime: number;                // 活动时间戳
  timeCategory: 'today' | 'yesterday' | 'thisWeek' | 'earlier'; // 时间段分类

  constructor(
    contact: Contact,
    metadata: ContactMetadata,
    activityType: ActivityType,
    activityTime: number
  ) {
    this.contact = contact;
    this.metadata = metadata;
    this.activityType = activityType;
    this.activityTime = activityTime;

    // 根据时间戳自动分类时间段
    // const { TimeUtils } = require('../utils/TimeUtils');
    this.timeCategory = TimeUtils.getTimeCategory(activityTime);
  }

  /**
   * 获取活动类型的中文描述
   */
  getActivityTypeText(): string {
    const texts = {
      [ActivityType.VIEW]: '查看',
      [ActivityType.EDIT]: '编辑',
      [ActivityType.CREATE]: '添加'
    };
    return texts[this.activityType];
  }

  /**
   * 获取活动图标颜色
   */
  getActivityColor(): string {
    const colors = {
      [ActivityType.VIEW]: '#999999',   // 灰色
      [ActivityType.EDIT]: '#4CAF50',   // 绿色
      [ActivityType.CREATE]: '#2196F3'  // 蓝色
    };
    return colors[this.activityType];
  }
}