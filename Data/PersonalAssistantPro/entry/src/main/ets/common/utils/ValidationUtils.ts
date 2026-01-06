/**
 * 正则校验工具类
 * 用于表单验证：手机号、邮箱、身份证、密码强度等
 */
export class ValidationUtils {

  // 手机号正则 (中国大陆)
  private static readonly PHONE_REGEX = /^1[3-9]\d{9}$/;

  // 邮箱正则
  private static readonly EMAIL_REGEX = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;

  // 身份证正则 (简单版18位)
  private static readonly ID_CARD_REGEX = /^[1-9]\d{5}(18|19|20)\d{2}(0[1-9]|1[0-2])(0[1-9]|[1-2]\d|3[0-1])\d{3}[\dXx]$/;

  // 数字正则
  private static readonly NUMBER_REGEX = /^\d+$/;

  /**
   * 校验手机号
   */
  static isPhone(phone: string): boolean {
    if (!phone) return false;
    return this.PHONE_REGEX.test(phone);
  }

  /**
   * 校验邮箱
   */
  static isEmail(email: string): boolean {
    if (!email) return false;
    return this.EMAIL_REGEX.test(email);
  }

  /**
   * 校验身份证
   */
  static isIdCard(idCard: string): boolean {
    if (!idCard) return false;
    return this.ID_CARD_REGEX.test(idCard);
  }

  /**
   * 是否纯数字
   */
  static isNumber(val: string): boolean {
    if (!val) return false;
    return this.NUMBER_REGEX.test(val);
  }

  /**
   * 校验密码强度
   * 规则：长度至少6位，必须包含字母和数字
   */
  static isStrongPassword(password: string): boolean {
    if (!password || password.length < 6) {
      return false;
    }
    const hasLetter = /[a-zA-Z]/.test(password);
    const hasDigit = /\d/.test(password);
    return hasLetter && hasDigit;
  }

  /**
   * 校验字符串长度范围
   */
  static isLengthInRange(str: string, min: number, max: number): boolean {
    if (!str) return false;
    return str.length >= min && str.length <= max;
  }

  /**
   * 校验是否包含特殊字符
   */
  static hasSpecialChar(str: string): boolean {
    const regex = /[`~!@#$%^&*()_\-+=<>?:"{},.\/;'[\]]/im;
    return regex.test(str);
  }
}