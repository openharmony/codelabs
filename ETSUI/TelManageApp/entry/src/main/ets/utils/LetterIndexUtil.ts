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
/*
 * 模块功能：联系人首字母索引与分组工具。
 * 作用：为联系人列表生成 A-Z/# 分组与索引映射，支持中文首字母估算。
 * 输出：分组列表与字母索引映射，供滑动条与列表跳转使用。
 */

import { contact } from '@kit.ContactsKit';

export interface ContactGroup {
  letter: string;
  contacts: contact.Contact[];
}

interface LetterBoundary {
  letter: string;
  boundary: string;
}

const CHINESE_BOUNDARIES: Array<LetterBoundary> = [
  { letter: 'A', boundary: '阿' },
  { letter: 'B', boundary: '芭' },
  { letter: 'C', boundary: '擦' },
  { letter: 'D', boundary: '搭' },
  { letter: 'E', boundary: '蛾' },
  { letter: 'F', boundary: '发' },
  { letter: 'G', boundary: '噶' },
  { letter: 'H', boundary: '哈' },
  { letter: 'J', boundary: '击' },
  { letter: 'K', boundary: '喀' },
  { letter: 'L', boundary: '垃' },
  { letter: 'M', boundary: '麻' },
  { letter: 'N', boundary: '拿' },
  { letter: 'O', boundary: '哦' },
  { letter: 'P', boundary: '啪' },
  { letter: 'Q', boundary: '期' },
  { letter: 'R', boundary: '然' },
  { letter: 'S', boundary: '撒' },
  { letter: 'T', boundary: '塌' },
  { letter: 'W', boundary: '挖' },
  { letter: 'X', boundary: '昔' },
  { letter: 'Y', boundary: '压' },
  { letter: 'Z', boundary: '匝' }
];

export class LetterIndexUtil {
  /**
   * 获取联系人显示名称。
   * 输入：contact 联系人对象。
   * 输出：联系人姓名字符串，缺省为 "未知"。
   */
  static getDisplayName(contactItem: contact.Contact): string {
    const name = contactItem.name?.fullName ?? '';
    return name.trim() !== '' ? name : '未知';
  }

  /**
   * 获取联系人首字母分组键（A-Z 或 #）。
   * 输入：name 联系人姓名。
   * 输出：分组键，无法识别时返回 "#".
   */
  static getInitialKey(name: string): string {
    const normalized = name.trim();
    if (normalized === '') {
      return '#';
    }

    const firstChar = normalized.charAt(0);
    const alphaKey = this.getAlphaInitial(firstChar);
    if (alphaKey !== '') {
      return alphaKey;
    }

    return this.getChineseInitial(firstChar);
  }

  /**
   * 按首字母分组联系人。
   * 输入：contacts 联系人数组。
   * 输出：按字母排序的分组数组。
   */
  static groupContacts(contacts: contact.Contact[]): ContactGroup[] {
    const groupedMap = this.buildGroupMap(contacts);
    const sortedKeys = this.sortGroupKeys(Array.from(groupedMap.keys()));
    return sortedKeys.map((key) => ({ letter: key, contacts: groupedMap.get(key) ?? [] }));
  }

  /**
   * 构建字母到列表索引的映射表。
   * 输入：groups 分组数组。
   * 输出：字母->索引映射。
   */
  static buildIndexMap(groups: ContactGroup[]): Map<string, number> {
    const indexMap = new Map<string, number>();
    groups.forEach((group, index) => {
      indexMap.set(group.letter, index);
    });
    return indexMap;
  }

  /**
   * 按姓名对联系人进行排序（升序）。
   * 输入：contacts 联系人数组。
   * 输出：排序后的联系人数组。
   */
  static sortContactsByName(contacts: contact.Contact[]): contact.Contact[] {
    return contacts.slice().sort((left, right) => {
      const leftName = this.getDisplayName(left);
      const rightName = this.getDisplayName(right);
      return leftName.localeCompare(rightName, 'zh');
    });
  }

  /**
   * 构建分组 Map。
   * 输入：contacts 联系人数组。
   * 输出：字母->联系人列表的 Map。
   */
  private static buildGroupMap(contacts: contact.Contact[]): Map<string, contact.Contact[]> {
    const groupedMap = new Map<string, contact.Contact[]>();
    contacts.forEach((item) => {
      const name = this.getDisplayName(item);
      const key = this.getInitialKey(name);
      const bucket = groupedMap.get(key) ?? [];
      bucket.push(item);
      groupedMap.set(key, bucket);
    });

    groupedMap.forEach((bucket, key) => {
      groupedMap.set(key, this.sortContactsByName(bucket));
    });

    return groupedMap;
  }

  /**
   * 对分组键排序，"#" 放末尾。
   * 输入：keys 字母数组。
   * 输出：排序后的字母数组。
   */
  private static sortGroupKeys(keys: string[]): string[] {
    return keys.sort((left, right) => {
      if (left === '#') {
        return 1;
      }
      if (right === '#') {
        return -1;
      }
      return left.localeCompare(right);
    });
  }

  /**
   * 识别英文字母首字母。
   * 输入：char 首字符。
   * 输出：大写字母或空字符串。
   */
  private static getAlphaInitial(char: string): string {
    if (/[a-zA-Z]/.test(char)) {
      return char.toUpperCase();
    }
    if (/[0-9]/.test(char)) {
      return '#';
    }
    return '';
  }

  /**
   * 使用边界字符估算中文首字母。
   * 输入：char 中文字符。
   * 输出：A-Z 或 "#"。
   */
  private static getChineseInitial(char: string): string {
    for (let index = CHINESE_BOUNDARIES.length - 1; index >= 0; index -= 1) {
      const boundary = CHINESE_BOUNDARIES[index];
      if (char.localeCompare(boundary.boundary, 'zh') >= 0) {
        return boundary.letter;
      }
    }
    return '#';
  }
}

