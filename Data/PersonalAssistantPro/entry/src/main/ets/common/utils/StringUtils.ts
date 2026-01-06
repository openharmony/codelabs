/**
 * 字符串处理工具类
 * 提供常见的字符串操作：判空、脱敏、格式化、UUID生成等
 */
export class StringUtils {

  /**
   * 判断字符串是否为空 (null, undefined, '')
   */
  static isEmpty(str: string | null | undefined): boolean {
    return str === null || str === undefined || str.length === 0;
  }

  /**
   * 判断字符串是否不为空
   */
  static isNotEmpty(str: string | null | undefined): boolean {
    return !StringUtils.isEmpty(str);
  }

  /**
   * 判断字符串是否为空白 (包含空格)
   * e.g. "  " -> true
   */
  static isBlank(str: string | null | undefined): boolean {
    if (StringUtils.isEmpty(str)) {
      return true;
    }
    // 替换所有空格后检查长度
    return str!.replace(/\s/g, '').length === 0;
  }

  /**
   * 手机号脱敏
   * 13812345678 -> 138****5678
   */
  static maskPhone(phone: string): string {
    if (StringUtils.isEmpty(phone) || phone.length < 11) {
      return phone;
    }
    return phone.substring(0, 3) + '****' + phone.substring(7);
  }

  /**
   * 姓名脱敏
   * 张三 -> 张*
   * 欧阳锋 -> 欧**
   */
  static maskName(name: string): string {
    if (StringUtils.isEmpty(name)) {
      return '';
    }
    if (name.length === 2) {
      return name.substring(0, 1) + '*';
    }
    if (name.length > 2) {
      return name.substring(0, 1) + '*'.repeat(name.length - 1);
    }
    return name;
  }

  /**
   * 简单生成 UUID (模拟)
   * 格式: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
   * 修改：移除 performance 依赖，仅使用 Date + Math.random，兼容性更好
   */
  static generateUUID(): string {
    let d = new Date().getTime();
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, (c) => {
      const r = (d + Math.random() * 16) % 16 | 0;
      d = Math.floor(d / 16);
      return (c === 'x' ? r : (r & 0x3 | 0x8)).toString(16);
    });
  }

  /**
   * 截取字符串，超出部分用 ... 代替
   */
  static truncate(str: string, maxLength: number): string {
    if (StringUtils.isEmpty(str)) {
      return '';
    }
    if (str.length <= maxLength) {
      return str;
    }
    return str.substring(0, maxLength) + '...';
  }

  /**
   * 移除所有空格
   */
  static removeAllSpaces(str: string): string {
    if (StringUtils.isEmpty(str)) {
      return '';
    }
    return str.replace(/\s+/g, '');
  }

  /**
   * 首字母大写
   */
  static capitalize(str: string): string {
    if (StringUtils.isEmpty(str)) {
      return '';
    }
    return str.charAt(0).toUpperCase() + str.slice(1);
  }
}