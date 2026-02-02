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

/**
 * 已删除联系人备份模型
 */
export class DeletedContact {
  backupId: string = '';              // 备份ID（唯一标识）
  originalKey: string = '';           // 原系统联系人的key
  contactData: Contact;                // 完整联系人数据备份
  metadata?: ContactMetadata;         // 关联的元数据（如果有）
  deleteTime: number = 0;              // 删除时间戳
  expireTime: number = 0;             // 过期时间（7天后）

  constructor(
    originalKey: string,
    contactData: Contact,
    metadata?: ContactMetadata
  ) {
    this.backupId = `deleted_${originalKey}_${Date.now()}`;
    this.originalKey = originalKey;
    this.contactData = contactData;
    this.metadata = metadata;
    this.deleteTime = Date.now();
    this.expireTime = this.deleteTime + 7 * 24 * 60 * 60 * 1000; // 7天后过期
  }

  /**
   * 是否已过期
   */
  isExpired(): boolean {
    return Date.now() > this.expireTime;
  }

  /**
   * 获取剩余天数
   */
  getRemainingDays(): number {
    const remaining = this.expireTime - Date.now();
    if (remaining <= 0) {
      return 0;
    }
    return Math.ceil(remaining / (24 * 60 * 60 * 1000));
  }

  /**
   * 转换为JSON对象（用于存储）
   */
  toJSON(): Record<string, string | number | object> {
    return {
      backupId: this.backupId,
      originalKey: this.originalKey,
      contactData: {
        id: this.contactData.id,
        key: this.contactData.key,
        name: this.contactData.name,
        phone: this.contactData.phone,
        email: this.contactData.email || '',
        remark: this.contactData.remark || ''
      },
      metadata: this.metadata ? this.metadata.toJSON() : undefined,
      deleteTime: this.deleteTime,
      expireTime: this.expireTime
    };
  }

  /**
   * 从JSON对象创建实例（用于读取）
   */
  static fromJSON(json: Record<string, string | number | object>): DeletedContact {
    const contactData = new Contact(
      (json.contactData as Record<string, string>).name || '',
      (json.contactData as Record<string, string>).phone || ''
    );
    contactData.id = (json.contactData as Record<string, string>).id || '';
    contactData.key = (json.contactData as Record<string, string>).key || '';
    contactData.email = (json.contactData as Record<string, string>).email || '';
    contactData.remark = (json.contactData as Record<string, string>).remark || '';

    const deleted = new DeletedContact(
      json.originalKey as string,
      contactData,
      json.metadata ? ContactMetadata.fromJSON(json.metadata as Record<string, string | number>) : undefined
    );
    
    deleted.backupId = json.backupId as string;
    deleted.deleteTime = json.deleteTime as number;
    deleted.expireTime = json.expireTime as number;
    
    return deleted;
  }
}
