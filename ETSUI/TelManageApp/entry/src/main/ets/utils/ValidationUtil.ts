/*
 * 模块功能：输入验证工具。
 * 作用：校验姓名、电话、邮箱等字段，并提供标准化结果。
 * 输出：验证结果与提示信息。
 */

export interface FieldValidationResult {
  isValid: boolean;
  message: string;
}

export class ValidationUtil {
  /**
   * 校验姓名是否合法。
   * 输入：name 姓名。
   * 输出：验证结果。
   */
  static validateName(name: string): FieldValidationResult {
    const trimmed = name.trim();
    if (!trimmed) {
      return { isValid: false, message: '姓名不能为空' };
    }
    if (trimmed.length > 30) {
      return { isValid: false, message: '姓名过长' };
    }
    return { isValid: true, message: '姓名可用' };
  }

  /**
   * 校验电话号码是否合法。
   * 输入：phone 电话号码。
   * 输出：验证结果。
   */
  static validatePhone(phone: string): FieldValidationResult {
    const normalized = this.normalizePhone(phone);
    if (!normalized) {
      return { isValid: false, message: '电话不能为空' };
    }
    if (!/^\+?\d{6,20}$/.test(normalized)) {
      return { isValid: false, message: '电话号码格式不正确' };
    }
    return { isValid: true, message: '电话号码可用' };
  }

  /**
   * 校验邮箱是否合法。
   * 输入：email 邮箱。
   * 输出：验证结果。
   */
  static validateEmail(email: string): FieldValidationResult {
    const trimmed = email.trim();
    if (!trimmed) {
      return { isValid: true, message: '邮箱为空可跳过' };
    }
    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(trimmed)) {
      return { isValid: false, message: '邮箱格式不正确' };
    }
    return { isValid: true, message: '邮箱可用' };
  }

  /**
   * 标准化电话号码（去空格/短横线）。
   * 输入：phone 电话号码。
   * 输出：标准化字符串。
   */
  static normalizePhone(phone: string): string {
    return phone.replace(/[\s-]/g, '').trim();
  }
}

