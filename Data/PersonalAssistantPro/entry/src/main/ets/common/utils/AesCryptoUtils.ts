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
// PAP/PersonalAssistantPro/entry/src/main/ets/common/utils/AesCryptoUtils.ts
import cryptoFramework from '@ohos.security.cryptoFramework';
import util from '@ohos.util';
import { Logger } from './Logger';

/**
 * AES-256 加密工具类 (静态工具类版)
 * F6 隐私加密
 */
export class AesCryptoUtils {
  private static logger = new Logger('AesCryptoUtils');
  private static globalKey: cryptoFramework.SymKey | null = null;
  private static readonly ALGO_NAME = 'AES256|CBC|PKCS7';

  private constructor() {}

  public static async initKey(keyHexString?: string): Promise<string> {
    try {
      const symKeyGenerator = cryptoFramework.createSymKeyGenerator('AES256');

      if (keyHexString) {
        const keyData = AesCryptoUtils.hexStringToUint8Array(keyHexString);
        AesCryptoUtils.globalKey = await symKeyGenerator.convertKey({ data: keyData });
        AesCryptoUtils.logger.info('Key loaded successfully from storage.');
        return keyHexString;
      } else {
        AesCryptoUtils.globalKey = await symKeyGenerator.generateSymKey();
        const encodedKey = await AesCryptoUtils.globalKey.getEncoded();
        const newKeyHex = AesCryptoUtils.uint8ArrayToHexString(encodedKey.data);
        AesCryptoUtils.logger.info('New AES-256 key generated.');
        return newKeyHex;
      }
    } catch (error) {
      AesCryptoUtils.logger.error(`initKey failed: ${JSON.stringify(error)}`);
      throw error;
    }
  }

  public static async encrypt(plainText: string): Promise<string> {
    if (!AesCryptoUtils.globalKey) {
      AesCryptoUtils.logger.warn('Key not initialized, auto-generating temp key...');
      await AesCryptoUtils.initKey();
    }

    // ✅ 修复：直接拦截空字符串，避免传给底层导致 401 错误
    if (!plainText || plainText.length === 0) {
      return '';
    }

    try {
      const cipher = cryptoFramework.createCipher(AesCryptoUtils.ALGO_NAME);
      const ivDataBlob = await cryptoFramework.createRandom().generateRandom(16);
      const ivParam: cryptoFramework.IvParamsSpec = { iv: ivDataBlob, algName: 'IvParamsSpec' };

      await cipher.init(cryptoFramework.CryptoMode.ENCRYPT_MODE, AesCryptoUtils.globalKey!, ivParam);

      // 使用 TextEncoder 转换
      const inputBlob: cryptoFramework.DataBlob = { data: new util.TextEncoder().encodeInto(plainText) };
      const outputBlob = await cipher.doFinal(inputBlob);

      const base64Helper = new util.Base64Helper();
      const ivBase64 = base64Helper.encodeToStringSync(ivDataBlob.data);
      const cipherBase64 = base64Helper.encodeToStringSync(outputBlob.data);

      return `${ivBase64}:${cipherBase64}`;
    } catch (error) {
      AesCryptoUtils.logger.error(`Encrypt failed: ${JSON.stringify(error)}`);
      // 发生错误返回原值，保证业务不中断
      return plainText;
    }
  }

  public static async decrypt(cipherFullText: string): Promise<string> {
    if (!AesCryptoUtils.globalKey) {
      await AesCryptoUtils.initKey();
    }

    if (!cipherFullText || !cipherFullText.includes(':')) {
      return cipherFullText;
    }

    try {
      const parts = cipherFullText.split(':');
      const ivBase64 = parts[0];
      const contentBase64 = parts[1];

      const base64Helper = new util.Base64Helper();
      const ivData = base64Helper.decodeSync(ivBase64);
      const cipherData = base64Helper.decodeSync(contentBase64);

      const decoder = cryptoFramework.createCipher(AesCryptoUtils.ALGO_NAME);
      const ivParam: cryptoFramework.IvParamsSpec = { iv: { data: ivData }, algName: 'IvParamsSpec' };

      await decoder.init(cryptoFramework.CryptoMode.DECRYPT_MODE, AesCryptoUtils.globalKey!, ivParam);

      const finalOutput = await decoder.doFinal({ data: cipherData });

      return new util.TextDecoder().decodeWithStream(finalOutput.data);
    } catch (error) {
      // AesCryptoUtils.logger.error(`Decrypt failed: ${JSON.stringify(error)}`);
      return cipherFullText;
    }
  }

  // === 内部工具函数 ===

  private static uint8ArrayToHexString(data: Uint8Array): string {
    let hex = '';
    for (let i = 0; i < data.length; i++) {
      let char = ('00' + data[i].toString(16)).slice(-2);
      hex += char;
    }
    return hex;
  }

  private static hexStringToUint8Array(hex: string): Uint8Array {
    const len = hex.length;
    if (len % 2 !== 0) return new Uint8Array(0);
    const data = new Uint8Array(len / 2);
    for (let i = 0; i < len; i += 2) {
      data[i / 2] = parseInt(hex.substring(i, i + 2), 16);
    }
    return data;
  }
}