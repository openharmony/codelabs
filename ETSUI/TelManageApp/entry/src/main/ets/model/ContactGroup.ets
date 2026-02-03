/**
 * 联系人分组模型与管理器
 * 提供分组创建、管理、自动分类等功能
 */

import { contact } from '@kit.ContactsKit';

/**
 * 分组类型枚举
 */
export enum GroupType {
  CUSTOM = 'CUSTOM',           // 用户自定义分组
  SYSTEM = 'SYSTEM',           // 系统预设分组
  SMART = 'SMART',             // 智能分组（自动分类）
  FAVORITE = 'FAVORITE',       // 收藏分组
  RECENT = 'RECENT',           // 最近联系
  FREQUENT = 'FREQUENT'        // 常用联系人
}

/**
 * 智能分组规则类型
 */
export enum SmartGroupRuleType {
  BY_NAME_PREFIX = 'BY_NAME_PREFIX',       // 按姓氏分组
  BY_PHONE_AREA = 'BY_PHONE_AREA',         // 按电话区号分组
  BY_EMAIL_DOMAIN = 'BY_EMAIL_DOMAIN',     // 按邮箱域名分组
  BY_ORGANIZATION = 'BY_ORGANIZATION',     // 按组织/公司分组
  BY_CREATION_DATE = 'BY_CREATION_DATE',   // 按创建日期分组
  BY_FIRST_LETTER = 'BY_FIRST_LETTER',     // 按首字母分组
  BY_CONTACT_COUNT = 'BY_CONTACT_COUNT',   // 按联系频率分组
  CUSTOM_RULE = 'CUSTOM_RULE'              // 自定义规则
}

/**
 * 分组颜色预设
 */
export const GROUP_COLORS: string[] = [
  '#007DFF', // 蓝色
  '#00C853', // 绿色
  '#FF6D00', // 橙色
  '#D500F9', // 紫色
  '#FF1744', // 红色
  '#00B8D4', // 青色
  '#FFD600', // 黄色
  '#64DD17', // 浅绿
  '#AA00FF', // 深紫
  '#F50057', // 粉色
  '#00BFA5', // 蓝绿
  '#FF9100'  // 深橙
];

/**
 * 分组图标预设
 */
export const GROUP_ICONS: string[] = [
  'people',
  'star',
  'work',
  'home',
  'school',
  'favorite',
  'phone',
  'mail',
  'location',
  'event',
  'business',
  'groups'
];

/**
 * 分组排序方式
 */
export enum GroupSortOrder {
  NAME_ASC = 'NAME_ASC',           // 按名称升序
  NAME_DESC = 'NAME_DESC',         // 按名称降序
  COUNT_ASC = 'COUNT_ASC',         // 按成员数升序
  COUNT_DESC = 'COUNT_DESC',       // 按成员数降序
  CREATED_ASC = 'CREATED_ASC',     // 按创建时间升序
  CREATED_DESC = 'CREATED_DESC',   // 按创建时间降序
  CUSTOM = 'CUSTOM'                // 自定义顺序
}

/**
 * 联系人分组接口
 */
export interface ContactGroup {
  groupId: string;                  // 分组唯一标识
  name: string;                     // 分组名称
  description: string;              // 分组描述
  type: GroupType;                  // 分组类型
  color: string;                    // 分组颜色
  icon: string;                     // 分组图标
  contactKeys: string[];            // 分组内联系人key列表
  createdAt: number;                // 创建时间
  updatedAt: number;                // 更新时间
  sortOrder: number;                // 排序顺序
  isDefault: boolean;               // 是否为默认分组
  isVisible: boolean;               // 是否可见
  smartRule?: SmartGroupRule;       // 智能分组规则（仅智能分组）
}

/**
 * 智能分组规则接口
 */
export interface SmartGroupRule {
  ruleType: SmartGroupRuleType;
  ruleConfig: SmartGroupRuleConfig;
  autoUpdate: boolean;              // 是否自动更新
  lastUpdated: number;              // 最后更新时间
}

/**
 * 日期范围接口
 */
export interface DateRange {
  startDate: number;
  endDate: number;
}

/**
 * 智能分组规则配置
 */
export interface SmartGroupRuleConfig {
  // 按姓氏分组配置
  namePrefix?: string[];

  // 按区号分组配置
  phoneAreaCodes?: string[];

  // 按邮箱域名分组配置
  emailDomains?: string[];

  // 按组织分组配置
  organizations?: string[];

  // 按日期分组配置
  dateRange?: DateRange;

  // 按首字母分组配置
  letters?: string[];

  // 自定义规则配置
  customExpression?: string;
  customFields?: string[];
}

/**
 * 分组名称计数接口
 */
export interface GroupNameCount {
  name: string;
  count: number;
}

/**
 * 分组统计信息
 */
export interface GroupStatistics {
  totalGroups: number;
  customGroups: number;
  smartGroups: number;
  totalContactsInGroups: number;
  ungroupedContacts: number;
  averageGroupSize: number;
  largestGroup: GroupNameCount;
  smallestGroup: GroupNameCount;
}

/**
 * 分组操作结果
 */
export interface GroupOperationResult {
  success: boolean;
  message: string;
  affectedGroupId?: string;
  affectedContactCount?: number;
}

/**
 * 联系人分组模型类
 */
export class ContactGroupModel implements ContactGroup {
  groupId: string;
  name: string;
  description: string;
  type: GroupType;
  color: string;
  icon: string;
  contactKeys: string[];
  createdAt: number;
  updatedAt: number;
  sortOrder: number;
  isDefault: boolean;
  isVisible: boolean;
  smartRule?: SmartGroupRule;

  constructor(name: string, type: GroupType = GroupType.CUSTOM) {
    this.groupId = this.generateGroupId();
    this.name = name;
    this.description = '';
    this.type = type;
    this.color = this.getRandomColor();
    this.icon = 'people';
    this.contactKeys = [];
    this.createdAt = Date.now();
    this.updatedAt = Date.now();
    this.sortOrder = 0;
    this.isDefault = false;
    this.isVisible = true;
  }

  /**
   * 生成唯一分组ID
   */
  private generateGroupId(): string {
    return `CG_${Date.now()}_${Math.random().toString(36).substring(2, 9)}`;
  }

  /**
   * 获取随机颜色
   */
  private getRandomColor(): string {
    const index = Math.floor(Math.random() * GROUP_COLORS.length);
    return GROUP_COLORS[index];
  }

  /**
   * 添加联系人到分组
   */
  addContact(contactKey: string): boolean {
    if (!this.contactKeys.includes(contactKey)) {
      this.contactKeys.push(contactKey);
      this.updatedAt = Date.now();
      return true;
    }
    return false;
  }

  /**
   * 批量添加联系人
   */
  addContacts(contactKeys: string[]): number {
    let addedCount = 0;
    contactKeys.forEach(key => {
      if (this.addContact(key)) {
        addedCount++;
      }
    });
    return addedCount;
  }

  /**
   * 从分组移除联系人
   */
  removeContact(contactKey: string): boolean {
    const index = this.contactKeys.indexOf(contactKey);
    if (index > -1) {
      this.contactKeys.splice(index, 1);
      this.updatedAt = Date.now();
      return true;
    }
    return false;
  }

  /**
   * 批量移除联系人
   */
  removeContacts(contactKeys: string[]): number {
    let removedCount = 0;
    contactKeys.forEach(key => {
      if (this.removeContact(key)) {
        removedCount++;
      }
    });
    return removedCount;
  }

  /**
   * 检查联系人是否在分组中
   */
  hasContact(contactKey: string): boolean {
    return this.contactKeys.includes(contactKey);
  }

  /**
   * 获取分组成员数量
   */
  getContactCount(): number {
    return this.contactKeys.length;
  }

  /**
   * 清空分组
   */
  clearContacts(): void {
    this.contactKeys = [];
    this.updatedAt = Date.now();
  }

  /**
   * 更新分组信息
   */
  update(updates: Partial<ContactGroup>): void {
    if (updates.name !== undefined) this.name = updates.name;
    if (updates.description !== undefined) this.description = updates.description;
    if (updates.color !== undefined) this.color = updates.color;
    if (updates.icon !== undefined) this.icon = updates.icon;
    if (updates.sortOrder !== undefined) this.sortOrder = updates.sortOrder;
    if (updates.isVisible !== undefined) this.isVisible = updates.isVisible;
    this.updatedAt = Date.now();
  }

  /**
   * 转换为JSON
   */
  toJSON(): ContactGroup {
    return {
      groupId: this.groupId,
      name: this.name,
      description: this.description,
      type: this.type,
      color: this.color,
      icon: this.icon,
      contactKeys: this.contactKeys.slice(),
      createdAt: this.createdAt,
      updatedAt: this.updatedAt,
      sortOrder: this.sortOrder,
      isDefault: this.isDefault,
      isVisible: this.isVisible,
      smartRule: this.smartRule
    };
  }

  /**
   * 从JSON创建实例
   */
  static fromJSON(json: ContactGroup): ContactGroupModel {
    const group = new ContactGroupModel(json.name, json.type);
    group.groupId = json.groupId;
    group.description = json.description;
    group.color = json.color;
    group.icon = json.icon;
    group.contactKeys = json.contactKeys.slice();
    group.createdAt = json.createdAt;
    group.updatedAt = json.updatedAt;
    group.sortOrder = json.sortOrder;
    group.isDefault = json.isDefault;
    group.isVisible = json.isVisible;
    group.smartRule = json.smartRule;
    return group;
  }
}

/**
 * 分组管理器类
 * 管理所有分组的增删改查操作
 */
export class GroupManager {
  private groups: Map<string, ContactGroupModel> = new Map();
  private storageKey: string = 'contact_groups';

  constructor() {
    this.initializeDefaultGroups();
  }

  /**
   * 初始化默认分组
   */
  private initializeDefaultGroups(): void {
    // 收藏分组
    const favorites = new ContactGroupModel('收藏', GroupType.FAVORITE);
    favorites.icon = 'star';
    favorites.color = '#FFD600';
    favorites.isDefault = true;
    favorites.sortOrder = -100;
    this.groups.set(favorites.groupId, favorites);

    // 家人分组
    const family = new ContactGroupModel('家人', GroupType.SYSTEM);
    family.icon = 'home';
    family.color = '#FF6D00';
    family.isDefault = true;
    family.sortOrder = -99;
    this.groups.set(family.groupId, family);

    // 朋友分组
    const friends = new ContactGroupModel('朋友', GroupType.SYSTEM);
    friends.icon = 'people';
    friends.color = '#00C853';
    friends.isDefault = true;
    friends.sortOrder = -98;
    this.groups.set(friends.groupId, friends);

    // 同事分组
    const colleagues = new ContactGroupModel('同事', GroupType.SYSTEM);
    colleagues.icon = 'work';
    colleagues.color = '#007DFF';
    colleagues.isDefault = true;
    colleagues.sortOrder = -97;
    this.groups.set(colleagues.groupId, colleagues);
  }

  /**
   * 创建新分组
   */
  createGroup(name: string, type: GroupType = GroupType.CUSTOM): GroupOperationResult {
    // 检查名称是否已存在
    const existingGroup = this.getGroupByName(name);
    if (existingGroup) {
      return {
        success: false,
        message: `分组"${name}"已存在`
      };
    }

    const group = new ContactGroupModel(name, type);
    this.groups.set(group.groupId, group);

    return {
      success: true,
      message: '分组创建成功',
      affectedGroupId: group.groupId
    };
  }

  /**
   * 创建智能分组
   */
  createSmartGroup(name: string, rule: SmartGroupRule): GroupOperationResult {
    const result = this.createGroup(name, GroupType.SMART);
    if (result.success && result.affectedGroupId) {
      const group = this.groups.get(result.affectedGroupId);
      if (group) {
        group.smartRule = rule;
      }
    }
    return result;
  }

  /**
   * 删除分组
   */
  deleteGroup(groupId: string): GroupOperationResult {
    const group = this.groups.get(groupId);
    if (!group) {
      return {
        success: false,
        message: '分组不存在'
      };
    }

    if (group.isDefault) {
      return {
        success: false,
        message: '无法删除默认分组'
      };
    }

    const contactCount = group.getContactCount();
    this.groups.delete(groupId);

    return {
      success: true,
      message: '分组已删除',
      affectedGroupId: groupId,
      affectedContactCount: contactCount
    };
  }

  /**
   * 更新分组
   */
  updateGroup(groupId: string, updates: Partial<ContactGroup>): GroupOperationResult {
    const group = this.groups.get(groupId);
    if (!group) {
      return {
        success: false,
        message: '分组不存在'
      };
    }

    // 检查新名称是否与其他分组冲突
    if (updates.name && updates.name !== group.name) {
      const existingGroup = this.getGroupByName(updates.name);
      if (existingGroup && existingGroup.groupId !== groupId) {
        return {
          success: false,
          message: `分组名称"${updates.name}"已被使用`
        };
      }
    }

    group.update(updates);

    return {
      success: true,
      message: '分组已更新',
      affectedGroupId: groupId
    };
  }

  /**
   * 获取分组
   */
  getGroup(groupId: string): ContactGroupModel | undefined {
    return this.groups.get(groupId);
  }

  /**
   * 按名称获取分组
   */
  getGroupByName(name: string): ContactGroupModel | undefined {
    for (const group of this.groups.values()) {
      if (group.name === name) {
        return group;
      }
    }
    return undefined;
  }

  /**
   * 获取所有分组
   */
  getAllGroups(sortOrder: GroupSortOrder = GroupSortOrder.CUSTOM): ContactGroupModel[] {
    const groups = Array.from(this.groups.values());
    return this.sortGroups(groups, sortOrder);
  }

  /**
   * 获取可见分组
   */
  getVisibleGroups(sortOrder: GroupSortOrder = GroupSortOrder.CUSTOM): ContactGroupModel[] {
    const groups = Array.from(this.groups.values()).filter(g => g.isVisible);
    return this.sortGroups(groups, sortOrder);
  }

  /**
   * 按类型获取分组
   */
  getGroupsByType(type: GroupType): ContactGroupModel[] {
    return Array.from(this.groups.values()).filter(g => g.type === type);
  }

  /**
   * 分组排序
   */
  private sortGroups(groups: ContactGroupModel[], sortOrder: GroupSortOrder): ContactGroupModel[] {
    switch (sortOrder) {
      case GroupSortOrder.NAME_ASC:
        return groups.sort((a, b) => a.name.localeCompare(b.name));
      case GroupSortOrder.NAME_DESC:
        return groups.sort((a, b) => b.name.localeCompare(a.name));
      case GroupSortOrder.COUNT_ASC:
        return groups.sort((a, b) => a.getContactCount() - b.getContactCount());
      case GroupSortOrder.COUNT_DESC:
        return groups.sort((a, b) => b.getContactCount() - a.getContactCount());
      case GroupSortOrder.CREATED_ASC:
        return groups.sort((a, b) => a.createdAt - b.createdAt);
      case GroupSortOrder.CREATED_DESC:
        return groups.sort((a, b) => b.createdAt - a.createdAt);
      case GroupSortOrder.CUSTOM:
      default:
        return groups.sort((a, b) => a.sortOrder - b.sortOrder);
    }
  }

  /**
   * 添加联系人到分组
   */
  addContactToGroup(groupId: string, contactKey: string): GroupOperationResult {
    const group = this.groups.get(groupId);
    if (!group) {
      return {
        success: false,
        message: '分组不存在'
      };
    }

    if (group.addContact(contactKey)) {
      return {
        success: true,
        message: '联系人已添加到分组',
        affectedGroupId: groupId,
        affectedContactCount: 1
      };
    }

    return {
      success: false,
      message: '联系人已在该分组中'
    };
  }

  /**
   * 批量添加联系人到分组
   */
  addContactsToGroup(groupId: string, contactKeys: string[]): GroupOperationResult {
    const group = this.groups.get(groupId);
    if (!group) {
      return {
        success: false,
        message: '分组不存在'
      };
    }

    const addedCount = group.addContacts(contactKeys);

    return {
      success: true,
      message: `已添加 ${addedCount} 个联系人到分组`,
      affectedGroupId: groupId,
      affectedContactCount: addedCount
    };
  }

  /**
   * 从分组移除联系人
   */
  removeContactFromGroup(groupId: string, contactKey: string): GroupOperationResult {
    const group = this.groups.get(groupId);
    if (!group) {
      return {
        success: false,
        message: '分组不存在'
      };
    }

    if (group.removeContact(contactKey)) {
      return {
        success: true,
        message: '联系人已从分组移除',
        affectedGroupId: groupId,
        affectedContactCount: 1
      };
    }

    return {
      success: false,
      message: '联系人不在该分组中'
    };
  }

  /**
   * 获取联系人所属的所有分组
   */
  getGroupsForContact(contactKey: string): ContactGroupModel[] {
    return Array.from(this.groups.values()).filter(g => g.hasContact(contactKey));
  }

  /**
   * 获取未分组的联系人
   */
  getUngroupedContactKeys(allContactKeys: string[]): string[] {
    const groupedKeys = new Set<string>();
    this.groups.forEach(group => {
      group.contactKeys.forEach(key => groupedKeys.add(key));
    });

    return allContactKeys.filter(key => !groupedKeys.has(key));
  }

  /**
   * 获取分组统计信息
   */
  getStatistics(allContactKeys: string[]): GroupStatistics {
    const groups = Array.from(this.groups.values());
    const customGroups = groups.filter(g => g.type === GroupType.CUSTOM).length;
    const smartGroups = groups.filter(g => g.type === GroupType.SMART).length;

    let totalContactsInGroups = 0;
    let largestGroup: GroupNameCount = { name: '', count: 0 };
    let smallestGroup: GroupNameCount = { name: '', count: Number.MAX_VALUE };

    groups.forEach(group => {
      const count = group.getContactCount();
      totalContactsInGroups += count;

      if (count > largestGroup.count) {
        largestGroup = { name: group.name, count: count };
      }
      if (count < smallestGroup.count && count > 0) {
        smallestGroup = { name: group.name, count: count };
      }
    });

    if (smallestGroup.count === Number.MAX_VALUE) {
      smallestGroup = { name: '', count: 0 };
    }

    const ungroupedContacts = this.getUngroupedContactKeys(allContactKeys).length;

    const result: GroupStatistics = {
      totalGroups: groups.length,
      customGroups: customGroups,
      smartGroups: smartGroups,
      totalContactsInGroups: totalContactsInGroups,
      ungroupedContacts: ungroupedContacts,
      averageGroupSize: groups.length > 0 ? totalContactsInGroups / groups.length : 0,
      largestGroup: largestGroup,
      smallestGroup: smallestGroup
    };
    return result;
  }

  /**
   * 合并两个分组
   */
  mergeGroups(sourceGroupId: string, targetGroupId: string): GroupOperationResult {
    const sourceGroup = this.groups.get(sourceGroupId);
    const targetGroup = this.groups.get(targetGroupId);

    if (!sourceGroup || !targetGroup) {
      return {
        success: false,
        message: '源分组或目标分组不存在'
      };
    }

    if (sourceGroup.isDefault) {
      return {
        success: false,
        message: '无法合并默认分组'
      };
    }

    const addedCount = targetGroup.addContacts(sourceGroup.contactKeys);
    this.groups.delete(sourceGroupId);

    return {
      success: true,
      message: `分组合并成功，已移动 ${addedCount} 个联系人`,
      affectedGroupId: targetGroupId,
      affectedContactCount: addedCount
    };
  }

  /**
   * 导出分组数据
   */
  exportGroups(): ContactGroup[] {
    return Array.from(this.groups.values()).map(g => g.toJSON());
  }

  /**
   * 导入分组数据
   */
  importGroups(groupsData: ContactGroup[], mergeExisting: boolean = true): number {
    let importedCount = 0;

    groupsData.forEach(data => {
      if (mergeExisting) {
        const existingGroup = this.getGroupByName(data.name);
        if (existingGroup) {
          existingGroup.addContacts(data.contactKeys);
          importedCount++;
          return;
        }
      }

      const group = ContactGroupModel.fromJSON(data);
      if (!mergeExisting) {
        // 生成新ID避免冲突
        group.groupId = `CG_${Date.now()}_${Math.random().toString(36).substring(2, 9)}`;
      }
      this.groups.set(group.groupId, group);
      importedCount++;
    });

    return importedCount;
  }
}

/**
 * 智能分组生成器
 * 根据规则自动生成分组
 */
export class SmartGroupGenerator {
  /**
   * 按姓氏自动分组
   */
  static groupByLastName(contacts: contact.Contact[]): Map<string, string[]> {
    const groups = new Map<string, string[]>();

    contacts.forEach(contactItem => {
      const fullName = contactItem.name?.fullName || '';
      if (fullName.length === 0) {
        const ungrouped = groups.get('未知姓氏') || [];
        if (contactItem.key) ungrouped.push(contactItem.key);
        groups.set('未知姓氏', ungrouped);
        return;
      }

      // 提取姓氏（假设第一个字符是姓）
      const lastName = fullName.charAt(0);
      const existing = groups.get(lastName) || [];
      if (contactItem.key) existing.push(contactItem.key);
      groups.set(lastName, existing);
    });

    return groups;
  }

  /**
   * 按首字母自动分组
   */
  static groupByFirstLetter(contacts: contact.Contact[]): Map<string, string[]> {
    const groups = new Map<string, string[]>();

    contacts.forEach(contactItem => {
      const fullName = contactItem.name?.fullName || '';
      let letter = '#';

      if (fullName.length > 0) {
        const firstChar = fullName.charAt(0).toUpperCase();
        if (/[A-Z]/.test(firstChar)) {
          letter = firstChar;
        } else if (/[\u4e00-\u9fa5]/.test(firstChar)) {
          // 中文字符，使用原字符
          letter = firstChar;
        }
      }

      const existing = groups.get(letter) || [];
      if (contactItem.key) existing.push(contactItem.key);
      groups.set(letter, existing);
    });

    return groups;
  }

  /**
   * 按邮箱域名自动分组
   */
  static groupByEmailDomain(contacts: contact.Contact[]): Map<string, string[]> {
    const groups = new Map<string, string[]>();

    contacts.forEach(contactItem => {
      const email = contactItem.emails?.[0]?.email || '';
      let domain = '无邮箱';

      if (email.includes('@')) {
        domain = email.split('@')[1];
      }

      const existing = groups.get(domain) || [];
      if (contactItem.key) existing.push(contactItem.key);
      groups.set(domain, existing);
    });

    return groups;
  }

  /**
   * 按电话区号自动分组
   */
  static groupByPhoneAreaCode(contacts: contact.Contact[]): Map<string, string[]> {
    const groups = new Map<string, string[]>();

    const areaCodeMap: Map<string, string> = new Map([
      ['010', '北京'],
      ['021', '上海'],
      ['020', '广州'],
      ['0755', '深圳'],
      ['0571', '杭州'],
      ['025', '南京'],
      ['028', '成都'],
      ['027', '武汉'],
      ['029', '西安'],
      ['022', '天津']
    ]);

    contacts.forEach(contactItem => {
      const phone = contactItem.phoneNumbers?.[0]?.phoneNumber || '';
      const normalized = phone.replace(/[^\d]/g, '');
      let area = '其他地区';

      // 检查手机号
      if (normalized.length === 11 && normalized.startsWith('1')) {
        area = '手机号码';
      } else {
        // 检查固话区号
        areaCodeMap.forEach((name: string, code: string) => {
          if (normalized.startsWith(code)) {
            area = name;
          }
        });
      }

      const existing = groups.get(area) || [];
      if (contactItem.key) existing.push(contactItem.key);
      groups.set(area, existing);
    });

    return groups;
  }

  /**
   * 按组织/公司自动分组
   */
  static groupByOrganization(contacts: contact.Contact[]): Map<string, string[]> {
    const groups = new Map<string, string[]>();

    contacts.forEach(contactItem => {
      const org = contactItem.organization?.name || '无组织信息';

      const existing = groups.get(org) || [];
      if (contactItem.key) existing.push(contactItem.key);
      groups.set(org, existing);
    });

    return groups;
  }
}
