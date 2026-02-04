/*
 * 模块功能：密令编码与解码工具。
 * 作用：将联系人字段压缩并混淆成短密令，支持解析还原。
 * 输出：密令字符串或联系人字段对象。
 */

export interface SecretContactFields {
  name: string;
  phone: string;
  email: string;
}

interface SecretPayload {
  v: number;
  n: string;
  p: string;
  e: string;
}

const SECRET_PREFIX_V1 = 'TM1';
const SECRET_PREFIX_V2 = 'TM2';
const SECRET_KEY = 'TelManageApp-Secret-Key';
const GROUP_SIZE = 4;
const MAX_CODE_LENGTH = 512;

export class SecretCodeUtil {
  /**
   * 生成联系人密令。
   * 输入：fields 联系人字段。
   * 输出：密令字符串。
   */
  static encode(fields: SecretContactFields): string {
    const payloadBytes = this.buildPayloadBytes(fields);
    const mixed = this.xorWithKey(payloadBytes, SECRET_KEY);
    const base64 = this.base64UrlEncode(mixed);
    return `${SECRET_PREFIX_V2}-${base64}`;
  }

  /**
   * 解析联系人密令。
   * 输入：code 密令字符串。
   * 输出：联系人字段对象。
   */
  static decode(code: string): SecretContactFields {
    const normalized = this.normalizeCode(code);
    this.ensureCodeLength(normalized);
    if (normalized.startsWith(`${SECRET_PREFIX_V2}-`)) {
      return this.decodeV2(normalized);
    }
    if (normalized.startsWith(`${SECRET_PREFIX_V1}-`)) {
      return this.decodeV1(normalized);
    }
    throw new Error('密令格式不正确');
  }

  /**
   * 生成用于展示的分组密令。
   * 输入：code 密令字符串。
   * 输出：分组后的密令字符串。
   */
  static formatForDisplay(code: string): string {
    const normalized = this.normalizeCode(code);
    this.ensureCodeLength(normalized);
    const parts = normalized.split('-');
    if (parts.length < 2) {
      return normalized;
    }
    const prefix = parts[0];
    const body = parts.slice(1).join('-');
    return `${prefix}-${this.groupText(body)}`;
  }

  /**
   * 清理密令中的空格与换行。
   * 输入：code 密令字符串。
   * 输出：清理后的密令。
   */
  static normalizeCode(code: string): string {
    return code.replace(/\s+/g, '').trim();
  }

  /**
   * 校验密令长度。
   * 输入：code 密令字符串。
   * 输出：无。
   */
  private static ensureCodeLength(code: string): void {
    if (!code) {
      throw new Error('密令为空');
    }
    if (code.length > MAX_CODE_LENGTH) {
      throw new Error('密令长度异常');
    }
  }

  /**
   * 构建密令载荷（V1）。
   * 输入：fields 联系人字段。
   * 输出：载荷对象。
   */
  private static buildPayload(fields: SecretContactFields): SecretPayload {
    return {
      v: 1,
      n: fields.name.trim(),
      p: fields.phone.trim(),
      e: fields.email.trim()
    };
  }

  /**
   * 校验并规范化载荷。
   * 输入：payload 载荷对象。
   * 输出：联系人字段对象。
   */
  private static normalizePayload(payload: SecretPayload): SecretContactFields {
    if (!payload || payload.v !== 1) {
      throw new Error('密令版本不匹配');
    }
    if (!payload.n || !payload.p) {
      throw new Error('密令信息不完整');
    }
    return {
      name: payload.n,
      phone: payload.p,
      email: payload.e ?? ''
    };
  }

  /**
   * 构建 V2 字节载荷（长度前缀 + UTF-8）。
   * 输入：fields 联系人字段。
   * 输出：字节数组。
   */
  private static buildPayloadBytes(fields: SecretContactFields): number[] {
    const nameBytes = this.encodeUtf8(fields.name.trim());
    const phoneBytes = this.encodeUtf8(fields.phone.trim());
    const emailBytes = this.encodeUtf8(fields.email.trim());
    const checksum = this.calculateChecksum(nameBytes, phoneBytes, emailBytes);
    return [
      2,
      ...this.encodeLength(nameBytes.length),
      ...this.encodeLength(phoneBytes.length),
      ...this.encodeLength(emailBytes.length),
      checksum,
      ...nameBytes,
      ...phoneBytes,
      ...emailBytes
    ];
  }

  /**
   * 解析 V2 密令。
   * 输入：code 密令字符串。
   * 输出：联系人字段对象。
   */
  private static decodeV2(code: string): SecretContactFields {
    const payload = code.slice(SECRET_PREFIX_V2.length + 1);
    const bytes = this.base64UrlDecode(payload);
    const raw = this.xorWithKey(bytes, SECRET_KEY);
    return this.parsePayloadBytes(raw);
  }

  /**
   * 解析 V1 密令。
   * 输入：code 密令字符串。
   * 输出：联系人字段对象。
   */
  private static decodeV1(code: string): SecretContactFields {
    const payload = code.slice(SECRET_PREFIX_V1.length + 1);
    const bytes = this.base64UrlDecode(payload);
    const raw = this.xorWithKey(bytes, SECRET_KEY);
    const json = this.decodeUtf8(raw);
    const payloadObj = JSON.parse(json) as SecretPayload;
    return this.normalizePayload(payloadObj);
  }

  /**
   * 解析字节载荷为字段。
   * 输入：bytes 字节数组。
   * 输出：联系人字段对象。
   */
  private static parsePayloadBytes(bytes: number[]): SecretContactFields {
    if (bytes.length < 8 || bytes[0] !== 2) {
      throw new Error('密令版本不匹配');
    }
    const nameLength = this.decodeLength(bytes[1], bytes[2]);
    const phoneLength = this.decodeLength(bytes[3], bytes[4]);
    const emailLength = this.decodeLength(bytes[5], bytes[6]);
    const checksum = bytes[7];
    const offset = 8;
    const totalLength = offset + nameLength + phoneLength + emailLength;
    if (bytes.length < totalLength) {
      throw new Error('密令信息不完整');
    }
    const nameBytes = bytes.slice(offset, offset + nameLength);
    const phoneBytes = bytes.slice(offset + nameLength, offset + nameLength + phoneLength);
    const emailBytes = bytes.slice(offset + nameLength + phoneLength, totalLength);
    if (!this.verifyChecksum(checksum, nameBytes, phoneBytes, emailBytes)) {
      throw new Error('密令校验失败');
    }
    return {
      name: this.decodeUtf8(nameBytes),
      phone: this.decodeUtf8(phoneBytes),
      email: this.decodeUtf8(emailBytes)
    };
  }

  /**
   * 计算字段校验和。
   * 输入：nameBytes/phoneBytes/emailBytes 字节数组。
   * 输出：校验值。
   */
  private static calculateChecksum(nameBytes: number[], phoneBytes: number[], emailBytes: number[]): number {
    const allBytes = [...nameBytes, ...phoneBytes, ...emailBytes];
    return allBytes.reduce((sum, value) => (sum + value) % 256, 0);
  }

  /**
   * 校验字段校验和。
   * 输入：checksum 校验值，nameBytes/phoneBytes/emailBytes 字节数组。
   * 输出：是否匹配。
   */
  private static verifyChecksum(
    checksum: number,
    nameBytes: number[],
    phoneBytes: number[],
    emailBytes: number[]
  ): boolean {
    return this.calculateChecksum(nameBytes, phoneBytes, emailBytes) === checksum;
  }

  /**
   * 按固定长度分组字符串。
   * 输入：text 字符串。
   * 输出：分组字符串。
   */
  private static groupText(text: string): string {
    const parts: string[] = [];
    for (let index = 0; index < text.length; index += GROUP_SIZE) {
      parts.push(text.slice(index, index + GROUP_SIZE));
    }
    return parts.join(' ');
  }

  /**
   * 编码长度为双字节。
   * 输入：length 长度值。
   * 输出：双字节数组。
   */
  private static encodeLength(length: number): number[] {
    const high = (length >> 8) & 0xff;
    const low = length & 0xff;
    return [high, low];
  }

  /**
   * 解码双字节长度。
   * 输入：high 高字节，low 低字节。
   * 输出：长度值。
   */
  private static decodeLength(high: number, low: number): number {
    return ((high & 0xff) << 8) | (low & 0xff);
  }

  /**
   * 字节数组与密钥异或混淆。
   * 输入：bytes 字节数组，key 密钥字符串。
   * 输出：混淆后的字节数组。
   */
  private static xorWithKey(bytes: number[], key: string): number[] {
    const keyBytes = this.encodeUtf8(key);
    return bytes.map((value, index) => value ^ keyBytes[index % keyBytes.length]);
  }

  /**
   * UTF-8 编码字符串为字节数组。
   * 输入：text 字符串。
   * 输出：字节数组。
   */
  private static encodeUtf8(text: string): number[] {
    const bytes: number[] = [];
    for (let index = 0; index < text.length; index += 1) {
      const code = text.charCodeAt(index);
      if (code <= 0x7f) {
        bytes.push(code);
      } else if (code <= 0x7ff) {
        bytes.push(0xc0 | (code >> 6), 0x80 | (code & 0x3f));
      } else {
        bytes.push(0xe0 | (code >> 12), 0x80 | ((code >> 6) & 0x3f), 0x80 | (code & 0x3f));
      }
    }
    return bytes;
  }

  /**
   * UTF-8 解码字节数组为字符串。
   * 输入：bytes 字节数组。
   * 输出：字符串。
   */
  private static decodeUtf8(bytes: number[]): string {
    let result = '';
    for (let index = 0; index < bytes.length; index += 1) {
      const byte = bytes[index];
      if (byte <= 0x7f) {
        result += String.fromCharCode(byte);
      } else if (byte >= 0xc0 && byte <= 0xdf) {
        const next = bytes[index + 1] & 0x3f;
        result += String.fromCharCode(((byte & 0x1f) << 6) | next);
        index += 1;
      } else {
        const next1 = bytes[index + 1] & 0x3f;
        const next2 = bytes[index + 2] & 0x3f;
        result += String.fromCharCode(((byte & 0x0f) << 12) | (next1 << 6) | next2);
        index += 2;
      }
    }
    return result;
  }

  /**
   * Base64URL 编码字节数组。
   * 输入：bytes 字节数组。
   * 输出：Base64URL 字符串。
   */
  private static base64UrlEncode(bytes: number[]): string {
    const base64 = this.base64Encode(bytes);
    return base64.replace(/\+/g, '-').replace(/\//g, '_').replace(/=+$/g, '');
  }

  /**
   * Base64URL 解码为字节数组。
   * 输入：text Base64URL 字符串。
   * 输出：字节数组。
   */
  private static base64UrlDecode(text: string): number[] {
    const padded = this.padBase64(text.replace(/-/g, '+').replace(/_/g, '/'));
    return this.base64Decode(padded);
  }

  /**
   * 为 Base64 字符串补齐 "="。
   * 输入：text Base64 字符串。
   * 输出：补齐后的字符串。
   */
  private static padBase64(text: string): string {
    const mod = text.length % 4;
    if (mod === 2) return `${text}==`;
    if (mod === 3) return `${text}=`;
    return text;
  }

  /**
   * Base64 编码字节数组。
   * 输入：bytes 字节数组。
   * 输出：Base64 字符串。
   */
  private static base64Encode(bytes: number[]): string {
    const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
    let result = '';
    for (let index = 0; index < bytes.length; index += 3) {
      const value = (bytes[index] << 16) | ((bytes[index + 1] ?? 0) << 8) | (bytes[index + 2] ?? 0);
      result += chars[(value >> 18) & 0x3f];
      result += chars[(value >> 12) & 0x3f];
      result += index + 1 < bytes.length ? chars[(value >> 6) & 0x3f] : '=';
      result += index + 2 < bytes.length ? chars[value & 0x3f] : '=';
    }
    return result;
  }

  /**
   * Base64 解码字符串为字节数组。
   * 输入：text Base64 字符串。
   * 输出：字节数组。
   */
  private static base64Decode(text: string): number[] {
    const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
    const cleaned = text.replace(/[^A-Za-z0-9+/=]/g, '');
    const bytes: number[] = [];
    for (let index = 0; index < cleaned.length; index += 4) {
      const c1 = chars.indexOf(cleaned[index]);
      const c2 = chars.indexOf(cleaned[index + 1]);
      const c3 = chars.indexOf(cleaned[index + 2]);
      const c4 = chars.indexOf(cleaned[index + 3]);
      const value = (c1 << 18) | (c2 << 12) | ((c3 & 0x3f) << 6) | (c4 & 0x3f);
      bytes.push((value >> 16) & 0xff);
      if (cleaned[index + 2] !== '=') bytes.push((value >> 8) & 0xff);
      if (cleaned[index + 3] !== '=') bytes.push(value & 0xff);
    }
    return bytes;
  }
}

