/**
 * 联系人去重检测器
 * 提供完整的重复联系人检测与合并功能
 */

import { contact } from '@kit.ContactsKit';
import { SimilarityCalculator } from '../utils/SimilarityCalculator';
import {
  DuplicateStrategyType,
  DuplicateStrictLevel,
  DuplicateStrategyConfig,
  DEFAULT_DUPLICATE_STRATEGY_CONFIG,
  MergeConflictResolution
} from './DuplicateStrategy';
import {
  DuplicateGroup,
  DuplicateGroupModel,
  DuplicateGroupStatus,
  SimilarityDetail,
  DuplicatePair,
  MergePreview,
  DuplicateStatistics,
  BatchProcessResult,
  FieldConflict
} from './MergeResult';
import { ContactData } from './ContactData';

/**
 * 辅助函数：复制配置对象
 */
function copyConfig(config: DuplicateStrategyConfig): DuplicateStrategyConfig {
  const copied: DuplicateStrategyConfig = {
    strategyType: config.strategyType,
    strictLevel: config.strictLevel,
    phoneSimThreshold: config.phoneSimThreshold,
    nameSimThreshold: config.nameSimThreshold,
    emailSimThreshold: config.emailSimThreshold,
    overallSimThreshold: config.overallSimThreshold,
    fieldWeights: {
      phoneWeight: config.fieldWeights.phoneWeight,
      nameWeight: config.fieldWeights.nameWeight,
      emailWeight: config.fieldWeights.emailWeight,
      organizationWeight: config.fieldWeights.organizationWeight
    },
    conflictResolution: config.conflictResolution,
    ignorePhoneFormat: config.ignorePhoneFormat,
    ignoreCaseInName: config.ignoreCaseInName,
    ignoreWhitespace: config.ignoreWhitespace,
    enablePinyinMatch: config.enablePinyinMatch
  };
  return copied;
}

/**
 * 去重检测器类
 * 核心的重复联系人检测引擎
 */
export class DuplicateDetector {
  private config: DuplicateStrategyConfig;
  private contactData: ContactData | null = null;

  constructor(config?: DuplicateStrategyConfig) {
    this.config = config ? copyConfig(config) : copyConfig(DEFAULT_DUPLICATE_STRATEGY_CONFIG);
  }

  /**
   * 设置 ContactData 实例
   */
  setContactData(contactData: ContactData): void {
    this.contactData = contactData;
  }

  /**
   * 更新配置
   */
  updateConfig(newConfig: DuplicateStrategyConfig): void {
    this.config = copyConfig(newConfig);
  }

  /**
   * 获取当前配置
   */
  getConfig(): DuplicateStrategyConfig {
    return copyConfig(this.config);
  }

  /**
   * 扫描所有联系人，检测重复
   * @param contacts 联系人列表
   * @returns 重复组列表
   */
  async detectDuplicates(contacts: contact.Contact[]): Promise<DuplicateGroupModel[]> {
    console.log(`DuplicateDetector: 开始扫描 ${contacts.length} 个联系人`);
    const startTime = Date.now();

    const duplicateGroups: DuplicateGroupModel[] = [];
    const processedKeys = new Set<string>();

    // 两两比较
    for (let i = 0; i < contacts.length; i++) {
      const contact1 = contacts[i];
      if (!contact1.key || processedKeys.has(contact1.key)) continue;

      const group = new DuplicateGroupModel([contact1]);

      for (let j = i + 1; j < contacts.length; j++) {
        const contact2 = contacts[j];
        if (!contact2.key || processedKeys.has(contact2.key)) continue;

        const similarity = this.calculateSimilarity(contact1, contact2);

        if (this.isDuplicate(similarity)) {
          group.addContact(contact2, similarity);
          // 存储两两相似度
          const pairKey = `${contact1.key}_${contact2.key}`;
          group.similarities.set(pairKey, similarity);
        }
      }

      // 只有当组内有超过1个联系人时才视为重复组
      if (group.getContactCount() > 1) {
        group.selectPrimaryContact();
        duplicateGroups.push(group);

        // 标记组内所有联系人为已处理
        group.contacts.forEach(c => {
          if (c.key) processedKeys.add(c.key);
        });
      }
    }

    const duration = Date.now() - startTime;
    console.log(`DuplicateDetector: 扫描完成，发现 ${duplicateGroups.length} 组重复，耗时 ${duration}ms`);

    return duplicateGroups;
  }

  /**
   * 快速扫描（仅检测明显重复）
   */
  async quickScan(contacts: contact.Contact[]): Promise<DuplicateGroupModel[]> {
    const quickConfig = copyConfig(this.config);
    quickConfig.strategyType = DuplicateStrategyType.EXACT_PHONE;
    quickConfig.strictLevel = DuplicateStrictLevel.STRICT;

    const originalConfig = copyConfig(this.config);
    this.config = quickConfig;

    const results = await this.detectDuplicates(contacts);

    this.config = originalConfig;
    return results;
  }

  /**
   * 深度扫描（使用宽松策略）
   */
  async deepScan(contacts: contact.Contact[]): Promise<DuplicateGroupModel[]> {
    const deepConfig = copyConfig(this.config);
    deepConfig.strategyType = DuplicateStrategyType.SMART_DETECT;
    deepConfig.strictLevel = DuplicateStrictLevel.LOOSE;
    deepConfig.overallSimThreshold = 0.5;

    const originalConfig = copyConfig(this.config);
    this.config = deepConfig;

    const results = await this.detectDuplicates(contacts);

    this.config = originalConfig;
    return results;
  }

  /**
   * 计算两个联系人的相似度
   */
  calculateSimilarity(contact1: contact.Contact, contact2: contact.Contact): SimilarityDetail {
    const phoneScore = this.calculatePhoneSimilarity(contact1, contact2);
    const nameScore = this.calculateNameSimilarity(contact1, contact2);
    const emailScore = this.calculateEmailSimilarity(contact1, contact2);
    const organizationScore = this.calculateOrganizationSimilarity(contact1, contact2);

    // 计算加权综合得分
    const weights = this.config.fieldWeights;
    const totalWeight = weights.phoneWeight + weights.nameWeight +
                       weights.emailWeight + weights.organizationWeight;

    const overallScore = totalWeight > 0 ?
      (phoneScore * weights.phoneWeight +
       nameScore * weights.nameWeight +
       emailScore * weights.emailWeight +
       organizationScore * weights.organizationWeight) / totalWeight
      : 0;

    // 识别匹配和冲突字段
    const matchedFields: string[] = [];
    const conflictFields: string[] = [];

    if (phoneScore >= this.config.phoneSimThreshold) {
      matchedFields.push('电话');
    } else if (phoneScore > 0 && phoneScore < 0.5) {
      conflictFields.push('电话');
    }

    if (nameScore >= this.config.nameSimThreshold) {
      matchedFields.push('姓名');
    } else if (nameScore > 0 && nameScore < 0.5) {
      conflictFields.push('姓名');
    }

    if (emailScore >= this.config.emailSimThreshold) {
      matchedFields.push('邮箱');
    } else if (emailScore > 0 && emailScore < 0.5) {
      conflictFields.push('邮箱');
    }

    return {
      phoneScore,
      nameScore,
      emailScore,
      organizationScore,
      overallScore,
      matchedFields,
      conflictFields
    };
  }

  /**
   * 计算电话相似度
   */
  private calculatePhoneSimilarity(contact1: contact.Contact, contact2: contact.Contact): number {
    const phones1 = contact1.phoneNumbers?.map(p => p.phoneNumber || '') || [];
    const phones2 = contact2.phoneNumbers?.map(p => p.phoneNumber || '') || [];

    if (phones1.length === 0 || phones2.length === 0) return 0;

    let maxSimilarity = 0;

    for (const phone1 of phones1) {
      for (const phone2 of phones2) {
        if (!phone1 || !phone2) continue;

        let similarity: number;
        if (this.config.ignorePhoneFormat) {
          similarity = SimilarityCalculator.phoneNumberSimilarity(phone1, phone2);
        } else {
          similarity = phone1 === phone2 ? 1 : SimilarityCalculator.levenshteinSimilarity(phone1, phone2);
        }

        maxSimilarity = Math.max(maxSimilarity, similarity);
      }
    }

    return maxSimilarity;
  }

  /**
   * 计算姓名相似度
   */
  private calculateNameSimilarity(contact1: contact.Contact, contact2: contact.Contact): number {
    let name1 = contact1.name?.fullName || '';
    let name2 = contact2.name?.fullName || '';

    if (!name1 || !name2) return 0;

    if (this.config.ignoreCaseInName) {
      name1 = name1.toLowerCase();
      name2 = name2.toLowerCase();
    }

    if (this.config.ignoreWhitespace) {
      name1 = name1.replace(/\s+/g, '');
      name2 = name2.replace(/\s+/g, '');
    }

    // 判断是否为中文名
    if (SimilarityCalculator.isChinese(name1) || SimilarityCalculator.isChinese(name2)) {
      return SimilarityCalculator.chineseNameSimilarity(name1, name2);
    }

    return SimilarityCalculator.jaroWinklerSimilarity(name1, name2);
  }

  /**
   * 计算邮箱相似度
   */
  private calculateEmailSimilarity(contact1: contact.Contact, contact2: contact.Contact): number {
    const emails1 = contact1.emails?.map(e => e.email || '') || [];
    const emails2 = contact2.emails?.map(e => e.email || '') || [];

    if (emails1.length === 0 || emails2.length === 0) return 0;

    let maxSimilarity = 0;

    for (const email1 of emails1) {
      for (const email2 of emails2) {
        if (!email1 || !email2) continue;

        const similarity = SimilarityCalculator.emailSimilarity(email1, email2);
        maxSimilarity = Math.max(maxSimilarity, similarity);
      }
    }

    return maxSimilarity;
  }

  /**
   * 计算组织相似度
   */
  private calculateOrganizationSimilarity(contact1: contact.Contact, contact2: contact.Contact): number {
    const org1 = contact1.organization?.name || '';
    const org2 = contact2.organization?.name || '';

    if (!org1 || !org2) return 0;

    return SimilarityCalculator.jaroWinklerSimilarity(
      org1.toLowerCase(),
      org2.toLowerCase()
    );
  }

  /**
   * 判断是否为重复联系人
   */
  private isDuplicate(similarity: SimilarityDetail): boolean {
    switch (this.config.strategyType) {
      case DuplicateStrategyType.EXACT_PHONE:
        return similarity.phoneScore >= 0.99;

      case DuplicateStrategyType.EXACT_NAME:
        return similarity.nameScore >= 0.99;

      case DuplicateStrategyType.EXACT_EMAIL:
        return similarity.emailScore >= 0.99;

      case DuplicateStrategyType.FUZZY_PHONE:
        return similarity.phoneScore >= this.config.phoneSimThreshold;

      case DuplicateStrategyType.FUZZY_NAME:
        return similarity.nameScore >= this.config.nameSimThreshold;

      case DuplicateStrategyType.PHONE_AND_NAME:
        return similarity.phoneScore >= this.config.phoneSimThreshold &&
               similarity.nameScore >= this.config.nameSimThreshold;

      case DuplicateStrategyType.MULTI_FIELD:
      case DuplicateStrategyType.SMART_DETECT:
      default:
        return similarity.overallScore >= this.config.overallSimThreshold;
    }
  }

  /**
   * 生成合并预览
   */
  generateMergePreview(group: DuplicateGroupModel): MergePreview {
    const contacts = group.contacts;
    if (contacts.length === 0) {
      throw new Error('重复组为空');
    }

    // 选择主联系人
    const primary = group.primaryContact || contacts[0];

    // 合并联系人信息
    const mergedContact = this.mergeContacts(contacts, primary);

    // 收集待删除的联系人key
    const contactsToDelete = contacts
      .filter(c => c.key !== primary.key)
      .map(c => c.key!)
      .filter(key => key !== undefined);

    // 获取冲突字段
    const conflicts = group.getConflictFields();

    // 生成警告
    const warnings: string[] = [];
    if (contacts.length > 5) {
      warnings.push(`将合并 ${contacts.length} 个联系人，请仔细确认`);
    }
    if (conflicts.length > 0) {
      warnings.push(`存在 ${conflicts.length} 个字段冲突，已自动选择推荐值`);
    }

    return {
      mergedContact,
      contactsToDelete,
      conflicts,
      warnings
    };
  }

  /**
   * 合并联系人
   */
  private mergeContacts(contacts: contact.Contact[], primary: contact.Contact): contact.Contact {
    const merged: contact.Contact = {
      id: primary.id,
      key: primary.key,
      name: { fullName: this.selectBestName(contacts) },
      phoneNumbers: this.mergePhoneNumbers(contacts),
      emails: this.mergeEmails(contacts)
    };

    // 合并组织信息
    const org = this.selectBestOrganization(contacts);
    if (org) {
      merged.organization = org;
    }

    // 保留头像
    if (primary.portrait) {
      merged.portrait = primary.portrait;
    }

    // 合并备注
    const note = this.mergeNotes(contacts);
    if (note) {
      merged.note = { noteContent: note };
    }

    return merged;
  }

  /**
   * 选择最佳姓名
   */
  private selectBestName(contacts: contact.Contact[]): string {
    const names = contacts
      .map(c => c.name?.fullName || '')
      .filter(n => n.length > 0);

    if (names.length === 0) return '';

    switch (this.config.conflictResolution) {
      case MergeConflictResolution.KEEP_FIRST:
        return names[0];
      case MergeConflictResolution.KEEP_LAST:
        return names[names.length - 1];
      case MergeConflictResolution.KEEP_LONGEST:
      default:
        return names.reduce((a, b) => a.length >= b.length ? a : b);
    }
  }

  /**
   * 合并电话号码（去重）
   */
  private mergePhoneNumbers(contacts: contact.Contact[]): contact.PhoneNumber[] {
    const phoneSet = new Map<string, contact.PhoneNumber>();

    contacts.forEach(c => {
      c.phoneNumbers?.forEach(phone => {
        if (phone.phoneNumber) {
          const normalized = phone.phoneNumber.replace(/[^\d]/g, '');
          if (!phoneSet.has(normalized)) {
            phoneSet.set(normalized, phone);
          }
        }
      });
    });

    return Array.from(phoneSet.values());
  }

  /**
   * 合并邮箱（去重）
   */
  private mergeEmails(contacts: contact.Contact[]): contact.Email[] {
    const emailSet = new Map<string, contact.Email>();

    contacts.forEach(c => {
      c.emails?.forEach(email => {
        if (email.email) {
          const normalized = email.email.toLowerCase();
          if (!emailSet.has(normalized)) {
            emailSet.set(normalized, email);
          }
        }
      });
    });

    return Array.from(emailSet.values());
  }

  /**
   * 选择最佳组织信息
   */
  private selectBestOrganization(contacts: contact.Contact[]): contact.Organization | undefined {
    const orgs = contacts
      .map(c => c.organization)
      .filter(o => o && o.name);

    if (orgs.length === 0) return undefined;

    // 选择信息最完整的
    return orgs.reduce((best, current) => {
      const bestScore = (best?.name?.length || 0) + (best?.title?.length || 0);
      const currentScore = (current?.name?.length || 0) + (current?.title?.length || 0);
      return currentScore > bestScore ? current : best;
    });
  }

  /**
   * 合并备注
   */
  private mergeNotes(contacts: contact.Contact[]): string {
    const notes = contacts
      .map(c => c.note?.noteContent || '')
      .filter(n => n.length > 0);

    if (notes.length === 0) return '';
    if (notes.length === 1) return notes[0];

    // 合并所有备注，用换行分隔（去重）
    const uniqueNotes = Array.from(new Set(notes));
    return uniqueNotes.join('\n---\n');
  }

  /**
   * 执行合并操作
   */
  async executeMerge(group: DuplicateGroupModel): Promise<boolean> {
    if (!this.contactData) {
      console.error('DuplicateDetector: ContactData 未设置');
      return false;
    }

    try {
      const preview = this.generateMergePreview(group);

      // 更新主联系人
      await this.contactData.updateContact(preview.mergedContact);

      // 删除其他联系人
      for (const key of preview.contactsToDelete) {
        await this.contactData.deleteContact(key);
      }

      // 标记组为已合并
      group.markAsProcessed(DuplicateGroupStatus.MERGED);

      console.log(`DuplicateDetector: 成功合并 ${preview.contactsToDelete.length + 1} 个联系人`);
      return true;
    } catch (error) {
      console.error('DuplicateDetector: 合并失败', error);
      return false;
    }
  }

  /**
   * 批量执行合并
   */
  async executeBatchMerge(groups: DuplicateGroupModel[]): Promise<BatchProcessResult> {
    const startTime = Date.now();
    let processedGroups = 0;
    let mergedContacts = 0;
    let deletedContacts = 0;
    let failedGroups = 0;
    const errors: string[] = [];

    for (const group of groups) {
      if (group.status !== DuplicateGroupStatus.PENDING) continue;

      try {
        const preview = this.generateMergePreview(group);
        const success = await this.executeMerge(group);

        if (success) {
          processedGroups++;
          mergedContacts += group.getContactCount();
          deletedContacts += preview.contactsToDelete.length;
        } else {
          failedGroups++;
          errors.push(`分组 ${group.groupId} 合并失败`);
        }
      } catch (error) {
        failedGroups++;
        errors.push(`分组 ${group.groupId}: ${(error as Error).message}`);
      }
    }

    return {
      success: failedGroups === 0,
      processedGroups,
      mergedContacts,
      deletedContacts,
      failedGroups,
      errors,
      duration: Date.now() - startTime
    };
  }

  /**
   * 忽略重复组
   */
  ignoreGroup(group: DuplicateGroupModel): void {
    group.markAsProcessed(DuplicateGroupStatus.IGNORED);
  }

  /**
   * 获取扫描统计信息
   */
  getStatistics(contacts: contact.Contact[], groups: DuplicateGroupModel[]): DuplicateStatistics {
    const duplicateContacts = groups.reduce((sum, g) => sum + g.getContactCount(), 0);

    return {
      totalContacts: contacts.length,
      duplicateGroups: groups.length,
      duplicateContacts,
      potentialSavings: duplicateContacts - groups.length,
      scanDuration: 0,
      lastScanTime: Date.now(),
      strategyUsed: this.config.strategyType
    };
  }
}

/**
 * 去重服务单例
 * 提供全局访问的去重功能
 */
export class DuplicateService {
  private static instance: DuplicateService | null = null;
  private detector: DuplicateDetector;
  private cachedGroups: DuplicateGroupModel[] = [];
  private lastScanTime: number = 0;

  private constructor() {
    this.detector = new DuplicateDetector();
  }

  /**
   * 获取单例实例
   */
  static getInstance(): DuplicateService {
    if (!DuplicateService.instance) {
      DuplicateService.instance = new DuplicateService();
    }
    return DuplicateService.instance;
  }

  /**
   * 设置 ContactData
   */
  setContactData(contactData: ContactData): void {
    this.detector.setContactData(contactData);
  }

  /**
   * 更新配置
   */
  updateConfig(config: DuplicateStrategyConfig): void {
    this.detector.updateConfig(config);
  }

  /**
   * 执行扫描
   */
  async scan(contacts: contact.Contact[], forceRefresh: boolean = false): Promise<DuplicateGroupModel[]> {
    // 如果有缓存且未超时（5分钟），直接返回
    const cacheTimeout = 5 * 60 * 1000;
    if (!forceRefresh && this.cachedGroups.length > 0 &&
        Date.now() - this.lastScanTime < cacheTimeout) {
      return this.cachedGroups;
    }

    this.cachedGroups = await this.detector.detectDuplicates(contacts);
    this.lastScanTime = Date.now();

    return this.cachedGroups;
  }

  /**
   * 获取缓存的重复组
   */
  getCachedGroups(): DuplicateGroupModel[] {
    return this.cachedGroups;
  }

  /**
   * 合并指定组
   */
  async mergeGroup(group: DuplicateGroupModel): Promise<boolean> {
    const result = await this.detector.executeMerge(group);
    if (result) {
      // 从缓存中移除已处理的组
      this.cachedGroups = this.cachedGroups.filter(g => g.groupId !== group.groupId);
    }
    return result;
  }

  /**
   * 忽略指定组
   */
  ignoreGroup(group: DuplicateGroupModel): void {
    this.detector.ignoreGroup(group);
    this.cachedGroups = this.cachedGroups.filter(g => g.groupId !== group.groupId);
  }

  /**
   * 批量合并所有
   */
  async mergeAll(): Promise<BatchProcessResult> {
    const result = await this.detector.executeBatchMerge(this.cachedGroups);
    this.cachedGroups = this.cachedGroups.filter(g => g.status === DuplicateGroupStatus.PENDING);
    return result;
  }

  /**
   * 清除缓存
   */
  clearCache(): void {
    this.cachedGroups = [];
    this.lastScanTime = 0;
  }

  /**
   * 获取检测器
   */
  getDetector(): DuplicateDetector {
    return this.detector;
  }
}
