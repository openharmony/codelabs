// src/main/ets/viewmodel/ContactViewModel.ts
import { Contact, ContactModel } from '../model/ContactModel';
import { Logger } from '../common/utils/Logger';

/**
 * 联系人业务逻辑模型 (ViewModel)
 * 负责处理联系人数据的加载、验证、转换、分组等逻辑
 * 这里的代码量主要来自详细的字段校验和数据处理
 */
export class ContactViewModel {
  private logger = new Logger('ContactViewModel');

  /**
   * 验证联系人表单数据是否合法
   * @param name 姓名
   * @param phone 电话
   * @returns string | null 如果有错误返回错误提示信息，否则返回 null
   */
  public validateContact(name: string, phone: string): string | null {
    this.logger.info(`Validating contact form: name=${name}, phone=${phone}`);

    // 1. 校验姓名
    if (!name || name.trim().length === 0) {
      this.logger.warn('Validation failed: name is empty');
      return '姓名不能为空';
    }

    if (name.length > 50) {
      this.logger.warn('Validation failed: name too long');
      return '姓名长度不能超过50个字符';
    }

    // 2. 校验电话
    if (!phone || phone.trim().length === 0) {
      this.logger.warn('Validation failed: phone is empty');
      return '电话号码不能为空';
    }

    // 简单的数字校验，实际项目可能需要更复杂的正则
    const isNum = /^\d+$/.test(phone);
    if (!isNum) {
      this.logger.warn('Validation failed: phone contains non-digits');
      return '电话号码只能包含数字';
    }

    if (phone.length < 3 || phone.length > 20) {
      this.logger.warn('Validation failed: phone length invalid');
      return '电话号码长度不合理';
    }

    return null; // 验证通过
  }

  /**
   * 格式化联系人显示名称
   * 如果有关系备注，则显示为 "姓名 (关系)"
   */
  public getDisplayName(contact: Contact): string {
    if (contact.relation && contact.relation.trim().length > 0) {
      return `${contact.name} (${contact.relation})`;
    }
    return contact.name;
  }

  /**
   * 异步加载所有联系人并按字母顺序排序
   * (模拟 A-Z 排序逻辑)
   */
  public async loadContactsSorted(): Promise<Array<Contact>> {
    this.logger.info('Loading contacts and sorting...');
    try {
      const contacts = await ContactModel.queryAll();

      if (!contacts || contacts.length === 0) {
        return [];
      }

      // 排序逻辑：按姓名 localeCompare 排序
      contacts.sort((a, b) => {
        const nameA = a.name || '';
        const nameB = b.name || '';
        return nameA.localeCompare(nameB);
      });

      this.logger.info(`Sorted ${contacts.length} contacts.`);
      return contacts;
    } catch (error) {
      this.logger.error('Failed to load sorted contacts');
      return [];
    }
  }

  /**
   * 按首字母分组 (Group By Initial)
   * 用于通讯录的侧边索引栏功能 (模拟)
   * @returns Map<string, Array<Contact>> Key为首字母
   */
  public groupContactsByInitial(contacts: Array<Contact>): Map<string, Array<Contact>> {
    const groups = new Map<string, Array<Contact>>();

    for (const contact of contacts) {
      let initial = '#';
      if (contact.name && contact.name.length > 0) {
        // 简单取第一个字符转大写，实际中文需要转拼音库
        const firstChar = contact.name.charAt(0).toUpperCase();
        if (firstChar >= 'A' && firstChar <= 'Z') {
          initial = firstChar;
        }
      }

      if (!groups.has(initial)) {
        groups.set(initial, []);
      }
      groups.get(initial)?.push(contact);
    }

    return groups;
  }
}