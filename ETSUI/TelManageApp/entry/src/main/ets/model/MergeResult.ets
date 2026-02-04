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
/**
 * 联系人合并结果模型
 * 定义重复检测结果、合并记录等数据结构
 */

import { contact } from '@kit.ContactsKit';

/**
 * 重复联系人组状态
 */
export enum DuplicateGroupStatus {
  PENDING = 'PENDING',       // 待处理
  MERGED = 'MERGED',         // 已合并
  IGNORED = 'IGNORED',       // 已忽略
  PARTIAL = 'PARTIAL'        // 部分处理
}

/**
 * 相似度详情接口
 * 记录各字段的相似度计算结果
 */
export interface SimilarityDetail {
  phoneScore: number;          // 电话相似度得分 (0-1)
  nameScore: number;           // 姓名相似度得分 (0-1)
  emailScore: number;          // 邮箱相似度得分 (0-1)
  organizationScore: number;   // 组织相似度得分 (0-1)
  overallScore: number;        // 综合相似度得分 (0-1)
  matchedFields: string[];     // 匹配的字段列表
  conflictFields: string[];    // 冲突的字段列表
}

/**
 * 重复联系人组JSON格式接口
 */
export interface DuplicateGroupJSON {
  groupId: string;
  contactKeys: (string | undefined)[];
  primaryContactKey: string | undefined;
  averageSimilarity: number;
  status: DuplicateGroupStatus;
  createdAt: number;
  processedAt: number | null;
}

/**
 * 合并记录统计信息接口
 */
export interface MergeRecordStatistics {
  totalMerged: number;
  totalDeleted: number;
  recentOperations: number;
}

/**
 * 重复联系人对接口
 * 表示两个可能重复的联系人
 */
export interface DuplicatePair {
  contact1: contact.Contact;
  contact2: contact.Contact;
  similarity: SimilarityDetail;
  detectedAt: number;          // 检测时间戳
  pairId: string;              // 唯一标识
}

/**
 * 重复联系人组接口
 * 表示一组可能重复的联系人（可能超过2个）
 */
export interface DuplicateGroup {
  groupId: string;                         // 组唯一标识
  contacts: contact.Contact[];             // 组内所有联系人
  primaryContact: contact.Contact | null;  // 推荐保留的主联系人
  similarities: Map<string, SimilarityDetail>; // 两两相似度详情
  averageSimilarity: number;               // 平均相似度
  status: DuplicateGroupStatus;            // 组状态
  createdAt: number;                       // 创建时间
  processedAt: number | null;              // 处理时间
}

/**
 * 字段冲突信息接口
 */
export interface FieldConflict {
  fieldName: string;           // 字段名称
  values: string[];            // 各联系人的值
  contactKeys: string[];       // 对应的联系人key
  recommendedValue: string;    // 推荐值
}

/**
 * 合并预览接口
 * 展示合并后的预期结果
 */
export interface MergePreview {
  mergedContact: contact.Contact;    // 合并后的联系人
  contactsToDelete: string[];        // 待删除的联系人key列表
  conflicts: FieldConflict[];        // 冲突字段列表
  warnings: string[];                // 警告信息
}

/**
 * 合并操作记录接口
 */
export interface MergeRecord {
  recordId: string;                  // 记录ID
  timestamp: number;                 // 操作时间
  originalContacts: contact.Contact[]; // 原始联系人列表
  mergedContact: contact.Contact;    // 合并后的联系人
  deletedKeys: string[];             // 已删除的联系人key
  operationType: 'AUTO' | 'MANUAL';  // 操作类型
  canUndo: boolean;                  // 是否可撤销
}

/**
 * 去重统计信息接口
 */
export interface DuplicateStatistics {
  totalContacts: number;             // 总联系人数
  duplicateGroups: number;           // 重复组数量
  duplicateContacts: number;         // 重复联系人数量
  potentialSavings: number;          // 可节省的联系人数
  scanDuration: number;              // 扫描耗时(ms)
  lastScanTime: number;              // 最后扫描时间
  strategyUsed: string;              // 使用的策略
}

/**
 * 批量处理结果接口
 */
export interface BatchProcessResult {
  success: boolean;
  processedGroups: number;           // 处理的组数
  mergedContacts: number;            // 合并的联系人数
  deletedContacts: number;           // 删除的联系人数
  failedGroups: number;              // 失败的组数
  errors: string[];                  // 错误信息列表
  duration: number;                  // 处理耗时(ms)
}

/**
 * 重复联系人组类
 * 封装重复组的操作逻辑
 */
export class DuplicateGroupModel implements DuplicateGroup {
  groupId: string;
  contacts: contact.Contact[];
  primaryContact: contact.Contact | null;
  similarities: Map<string, SimilarityDetail>;
  averageSimilarity: number;
  status: DuplicateGroupStatus;
  createdAt: number;
  processedAt: number | null;

  constructor(contacts: contact.Contact[]) {
    this.groupId = this.generateGroupId();
    this.contacts = contacts;
    this.primaryContact = null;
    this.similarities = new Map();
    this.averageSimilarity = 0;
    this.status = DuplicateGroupStatus.PENDING;
    this.createdAt = Date.now();
    this.processedAt = null;
  }

  /**
   * 生成唯一组ID
   */
  private generateGroupId(): string {
    return `DG_${Date.now()}_${Math.random().toString(36).substring(2, 9)}`;
  }

  /**
   * 选择主联系人（信息最完整的）
   */
  selectPrimaryContact(): contact.Contact | null {
    if (this.contacts.length === 0) {
      return null;
    }

    let maxScore = -1;
    let primaryIndex = 0;

    this.contacts.forEach((contact, index) => {
      const score = this.calculateCompletenessScore(contact);
      if (score > maxScore) {
        maxScore = score;
        primaryIndex = index;
      }
    });

    this.primaryContact = this.contacts[primaryIndex];
    return this.primaryContact;
  }

  /**
   * 计算联系人信息完整度得分
   */
  private calculateCompletenessScore(contactItem: contact.Contact): number {
    let score = 0;

    // 姓名完整度
    if (contactItem.name?.fullName) {
      score += 20;
      if (contactItem.name.fullName.length > 2) score += 5;
    }

    // 电话完整度
    if (contactItem.phoneNumbers && contactItem.phoneNumbers.length > 0) {
      score += 25;
      score += Math.min(contactItem.phoneNumbers.length * 5, 15);
    }

    // 邮箱
    if (contactItem.emails && contactItem.emails.length > 0) {
      score += 15;
    }

    // 组织信息
    if (contactItem.organization?.name) {
      score += 10;
    }

    // 头像
    if (contactItem.portrait?.uri) {
      score += 5;
    }

    // 备注
    if (contactItem.note?.noteContent) {
      score += 5;
    }

    return score;
  }

  /**
   * 获取组内联系人数量
   */
  getContactCount(): number {
    return this.contacts.length;
  }

  /**
   * 添加联系人到组
   */
  addContact(contactItem: contact.Contact, similarityToExisting?: SimilarityDetail): void {
    if (!this.contacts.some(c => c.key === contactItem.key)) {
      this.contacts.push(contactItem);

      if (similarityToExisting && contactItem.key) {
        this.similarities.set(contactItem.key, similarityToExisting);
      }

      this.recalculateAverageSimilarity();
    }
  }

  /**
   * 从组中移除联系人
   */
  removeContact(contactKey: string): void {
    this.contacts = this.contacts.filter(c => c.key !== contactKey);
    this.similarities.delete(contactKey);
    this.recalculateAverageSimilarity();
  }

  /**
   * 重新计算平均相似度
   */
  private recalculateAverageSimilarity(): void {
    if (this.similarities.size === 0) {
      this.averageSimilarity = 0;
      return;
    }

    let totalScore = 0;
    this.similarities.forEach((similarity) => {
      totalScore += similarity.overallScore;
    });

    this.averageSimilarity = totalScore / this.similarities.size;
  }

  /**
   * 标记为已处理
   */
  markAsProcessed(newStatus: DuplicateGroupStatus): void {
    this.status = newStatus;
    this.processedAt = Date.now();
  }

  /**
   * 获取所有冲突字段
   */
  getConflictFields(): FieldConflict[] {
    const conflicts: FieldConflict[] = [];

    // 检查姓名冲突
    const names = this.contacts
      .map(c => c.name?.fullName || '')
      .filter(n => n !== '');
    if (new Set(names).size > 1) {
      conflicts.push({
        fieldName: '姓名',
        values: names,
        contactKeys: this.contacts.map(c => c.key || ''),
        recommendedValue: this.getRecommendedValue(names)
      });
    }

    // 检查电话冲突
    const phones = this.contacts
      .flatMap(c => c.phoneNumbers?.map(p => p.phoneNumber) || [])
      .filter(p => p !== undefined && p !== '');
    if (new Set(phones).size > 1) {
      conflicts.push({
        fieldName: '电话号码',
        values: phones as string[],
        contactKeys: this.contacts.map(c => c.key || ''),
        recommendedValue: '合并所有号码'
      });
    }

    // 检查邮箱冲突
    const emails = this.contacts
      .flatMap(c => c.emails?.map(e => e.email) || [])
      .filter(e => e !== undefined && e !== '');
    if (new Set(emails).size > 1) {
      conflicts.push({
        fieldName: '邮箱',
        values: emails as string[],
        contactKeys: this.contacts.map(c => c.key || ''),
        recommendedValue: '合并所有邮箱'
      });
    }

    return conflicts;
  }

  /**
   * 获取推荐值（最长的非空值）
   */
  private getRecommendedValue(values: string[]): string {
    return values.reduce((longest, current) =>
      current.length > longest.length ? current : longest, '');
  }

  /**
   * 转换为JSON格式（用于持久化）
   */
  toJSON(): DuplicateGroupJSON {
    const result: DuplicateGroupJSON = {
      groupId: this.groupId,
      contactKeys: this.contacts.map(c => c.key),
      primaryContactKey: this.primaryContact?.key,
      averageSimilarity: this.averageSimilarity,
      status: this.status,
      createdAt: this.createdAt,
      processedAt: this.processedAt
    };
    return result;
  }
}

/**
 * 合并记录管理器
 * 用于追踪和撤销合并操作
 */
export class MergeRecordManager {
  private records: MergeRecord[] = [];
  private maxRecords: number = 100;

  /**
   * 添加合并记录
   */
  addRecord(record: MergeRecord): void {
    this.records.unshift(record);
    if (this.records.length > this.maxRecords) {
      this.records = this.records.slice(0, this.maxRecords);
    }
  }

  /**
   * 获取所有记录
   */
  getAllRecords(): MergeRecord[] {
    return this.records.slice();
  }

  /**
   * 获取可撤销的记录
   */
  getUndoableRecords(): MergeRecord[] {
    return this.records.filter(r => r.canUndo);
  }

  /**
   * 根据ID获取记录
   */
  getRecordById(recordId: string): MergeRecord | undefined {
    return this.records.find(r => r.recordId === recordId);
  }

  /**
   * 标记记录为不可撤销
   */
  markAsNonUndoable(recordId: string): void {
    const record = this.records.find(r => r.recordId === recordId);
    if (record) {
      record.canUndo = false;
    }
  }

  /**
   * 清空所有记录
   */
  clearRecords(): void {
    this.records = [];
  }

  /**
   * 获取统计信息
   */
  getStatistics(): MergeRecordStatistics {
    const oneDayAgo = Date.now() - 24 * 60 * 60 * 1000;
    const result: MergeRecordStatistics = {
      totalMerged: this.records.length,
      totalDeleted: this.records.reduce((sum, r) => sum + r.deletedKeys.length, 0),
      recentOperations: this.records.filter(r => r.timestamp > oneDayAgo).length
    };
    return result;
  }
}
