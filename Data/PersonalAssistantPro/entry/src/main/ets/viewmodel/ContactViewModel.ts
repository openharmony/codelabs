/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
// src/main/ets/viewmodel/ContactViewModel.ts
import { Contact, ContactModel } from '../model/ContactModel';
import { Logger } from '../common/utils/Logger';

/**
 * 联系人业务逻辑模型 (ViewModel)
 */
export class ContactViewModel {
  private logger = new Logger('ContactViewModel');

  /**
   * 验证联系人表单数据是否合法
   * @param name 姓名
   * @param phone 电话
   * @returns string | null 返回资源文件的 Key (如 'contact_validate_name_empty')，UI层需自行封装 $r()
   */
  public validateContact(name: string, phone: string): string | null {
    this.logger.info(`Validating contact form: name=${name}, phone=${phone}`);

    // 1. 校验姓名
    if (!name || name.trim().length === 0) {
      this.logger.warn('Validation failed: name is empty');
      // 对应 string.json: contact_validate_name_empty
      return 'contact_validate_name_empty';
    }

    if (name.length > 50) {
      this.logger.warn('Validation failed: name too long');
      // 对应 string.json: contact_validate_name_length
      return 'contact_validate_name_length';
    }

    // 2. 校验电话
    if (!phone || phone.trim().length === 0) {
      this.logger.warn('Validation failed: phone is empty');
      // 对应 string.json: contact_validate_phone_empty
      return 'contact_validate_phone_empty';
    }

    // 简单的数字校验
    const isNum = /^\d+$/.test(phone);
    if (!isNum) {
      this.logger.warn('Validation failed: phone contains non-digits');
      // 对应 string.json: contact_validate_phone_number_only
      return 'contact_validate_phone_number_only';
    }

    if (phone.length < 3 || phone.length > 20) {
      this.logger.warn('Validation failed: phone length invalid');
      // 对应 string.json: contact_validate_phone_length
      return 'contact_validate_phone_length';
    }

    return null; // 验证通过
  }

  /**
   * 格式化联系人显示名称
   */
  public getDisplayName(contact: Contact): string {
    if (contact.relation && contact.relation.trim().length > 0) {
      return `${contact.name} (${contact.relation})`;
    }
    return contact.name;
  }

  /**
   * 异步加载所有联系人并按字母顺序排序
   */
  public async loadContactsSorted(): Promise<Array<Contact>> {
    this.logger.info('Loading contacts and sorting...');
    try {
      const contacts = await ContactModel.queryAll();

      if (!contacts || contacts.length === 0) {
        return [];
      }

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
   * 按首字母分组
   */
  public groupContactsByInitial(contacts: Array<Contact>): Map<string, Array<Contact>> {
    const groups = new Map<string, Array<Contact>>();

    for (const contact of contacts) {
      let initial = '#';
      if (contact.name && contact.name.length > 0) {
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