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

/**
 * 联系人重要性级别
 */
export enum ImportanceLevel {
  NORMAL = 'normal',       // 普通联系人
  IMPORTANT = 'important', // 重要联系人
  EMERGENCY = 'emergency'  // 紧急联系人
}

/**
 * 联系人元数据模型
 * 用于存储联系人的扩展属性（重要性、活动记录等）
 */
export class ContactMetadata {
  contactKey: string = '';                    // 关联系统联系人的key
  importance: ImportanceLevel = ImportanceLevel.NORMAL; // 重要级别
  lastViewTime: number = 0;                   // 最后查看时间戳
  lastEditTime: number = 0;                   // 最后编辑时间戳
  createTime: number = 0;                     // 创建时间戳
  viewCount: number = 0;                      // 查看次数

  constructor(contactKey: string) {
    this.contactKey = contactKey;
    this.createTime = Date.now();
  }

  /**
   * 是否为重要联系人（包括重要和紧急）
   */
  isImportant(): boolean {
    return this.importance === ImportanceLevel.IMPORTANT || 
           this.importance === ImportanceLevel.EMERGENCY;
  }

  /**
   * 是否为紧急联系人
   */
  isEmergency(): boolean {
    return this.importance === ImportanceLevel.EMERGENCY;
  }

  /**
   * 更新查看记录
   */
  updateViewRecord(): void {
    this.lastViewTime = Date.now();
    this.viewCount++;
  }

  /**
   * 更新编辑记录
   */
  updateEditRecord(): void {
    this.lastEditTime = Date.now();
  }

  /**
   * 转换为JSON对象（用于存储）
   */
  toJSON(): Record<string, string | number> {
    return {
      contactKey: this.contactKey,
      importance: this.importance,
      lastViewTime: this.lastViewTime,
      lastEditTime: this.lastEditTime,
      createTime: this.createTime,
      viewCount: this.viewCount
    };
  }

  /**
   * 从JSON对象创建实例（用于读取）
   */
  static fromJSON(json: Record<string, string | number>): ContactMetadata {
    const metadata = new ContactMetadata(json.contactKey as string);
    metadata.importance = (json.importance as ImportanceLevel) || ImportanceLevel.NORMAL;
    metadata.lastViewTime = (json.lastViewTime as number) || 0;
    metadata.lastEditTime = (json.lastEditTime as number) || 0;
    metadata.createTime = (json.createTime as number) || Date.now();
    metadata.viewCount = (json.viewCount as number) || 0;
    return metadata;
  }
}
