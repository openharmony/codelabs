/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import cryptoFramework from '@ohos.security.cryptoFramework';
import Logger from '../util/Logger';

const TAG: string = '[Crypto_Framework]';
const BASE_16: number = 16;
const SLICE_NUMBER: number = -2;
const CHUNK_SIZE: number = 64 * 1024; // 64KB chunks for stream processing

// 字节流以16进制字符串输出
function uint8ArrayToShowStr(uint8Array: Uint8Array): string {
  let ret: string = Array.prototype.map
    .call(uint8Array, (x) => ('00' + x.toString(BASE_16)).slice(SLICE_NUMBER)).join('');
  return ret;
}

// 16进制字符串转字节流
function fromHexString(hexString: string): Uint8Array {
  let ret: Uint8Array = new Uint8Array(hexString.match(/.{1,2}/g).map(byte => parseInt(byte, BASE_16)));
  return ret;
}

// 字节流转字符串
function arrayBufferToString(buffer: ArrayBuffer): string {
  let ret: string = String.fromCharCode.apply(null, new Uint8Array(buffer));
  return ret;
}

// 可理解的字符串转成字节流
function stringToUint8Array(str: string): Uint8Array {
  let arr = [];
  for (let i = 0, j = str.length; i < j; ++i) {
    arr.push(str.charCodeAt(i));
  }
  let ret: Uint8Array = new Uint8Array(arr);
  return ret;
}

// 新增：将ArrayBuffer转换为十六进制字符串
function arrayBufferToHexString(buffer: ArrayBuffer): string {
  const uint8Array = new Uint8Array(buffer);
  return uint8ArrayToShowStr(uint8Array);
}

// 新增：将十六进制字符串转换为ArrayBuffer
function hexStringToArrayBuffer(hexString: string): ArrayBuffer {
  return fromHexString(hexString).buffer;
}

function genGcmParamsSpec(): cryptoFramework.GcmParamsSpec {
  let arr = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]; // 12 bytes
  let dataIv = new Uint8Array(arr);
  let ivBlob = { data: dataIv };

  arr = [0, 0, 0, 0, 0, 0, 0, 0]; // 8 bytes
  let dataAad = new Uint8Array(arr);
  let aadBlob = { data: dataAad };

  arr = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]; // 16 bytes
  let dataTag = new Uint8Array(arr);
  let tagBlob = { data: dataTag };

  let gcmParamsSpec = {
    iv: ivBlob,
    aad: aadBlob,
    authTag: tagBlob,
    algName: 'GcmParamsSpec'
  };
  return gcmParamsSpec;
}

// 新增：二进制操作结果接口
interface BinaryOperationResult {
  success: boolean;
  data?: ArrayBuffer;
  tag?: ArrayBuffer; // 用于GCM模式
  error?: string;
}

// 新增：流式处理进度回调
interface StreamProgressCallback {
  (bytesProcessed: number, totalBytes: number): void;
}

export class CryptoOperation {
  /**
   * 生成AES密钥（保持不变）
   */
  async generateAesKey(): Promise<string> {
    let symKeyGenerator;
    let encodedKey;
    try {
      symKeyGenerator = cryptoFramework.createSymKeyGenerator('AES256');
    } catch (error) {
      Logger.error(TAG, 'create generator failed');
      return null;
    }

    try {
      let symKey = await symKeyGenerator.generateSymKey();
      encodedKey = symKey.getEncoded();
      let data: Uint8Array = encodedKey.data;
      Logger.info('success, key bytes: ' + data);
      Logger.info('success, key hex:' + uint8ArrayToShowStr(data));
      return uint8ArrayToShowStr(data);
    } catch (error) {
      Logger.error(TAG, 'create symKey failed');
      return null;
    }
  }

  /**
   * 转换AES密钥（保持不变）
   */
  async convertAesKey(aesKeyBlobString: string): Promise<cryptoFramework.SymKey> {
    let symKeyGenerator = cryptoFramework.createSymKeyGenerator('AES256');
    Logger.info(TAG, 'success, read key string' + aesKeyBlobString);
    Logger.info(TAG, 'success, blob key ' + fromHexString(aesKeyBlobString));
    let symKeyBlob = { data: fromHexString(aesKeyBlobString) };
    try {
      let key = await symKeyGenerator.convertKey(symKeyBlob);
      let aesKey: cryptoFramework.SymKey = key;
      return aesKey;
    } catch (error) {
      Logger.error(TAG, `convert aes key failed, ${error.code}, ${error.message}`);
      return null;
    }
  }

  /**
   * AES-GCM加密，支持二进制数据
   */
  async aesGcmEncrypt(globalKey, data: string | ArrayBuffer): Promise<string> {
    let cipherAlgName = 'AES256|GCM|PKCS7';
    let cipher;
    let cipherText: string;
    let globalGcmParams = genGcmParamsSpec();
    let aesEncryptJsonStr = null;

    try {
      cipher = cryptoFramework.createCipher(cipherAlgName);
      Logger.info(TAG, `cipher algName: ${cipher.algName}`);
    } catch (error) {
      Logger.error(TAG, `createCipher failed, ${error.code}, ${error.message}`);
      return aesEncryptJsonStr;
    }

    let mode = cryptoFramework.CryptoMode.ENCRYPT_MODE;
    try {
      await cipher.init(mode, globalKey, globalGcmParams);
    } catch (error) {
      Logger.error(TAG, `init cipher failed, ${error.code}, ${error.message}`);
      return aesEncryptJsonStr;
    }

    // 支持字符串和ArrayBuffer输入
    let plainText;
    if (typeof data === 'string') {
      plainText = { data: stringToUint8Array(data) };
    } else {
      plainText = { data: new Uint8Array(data) };
    }

    Logger.info(TAG, `plain text size: ${plainText.data.length}`);

    try {
      let cipherTextBlob = await cipher.update(plainText);
      let tmpArr: Uint8Array = cipherTextBlob.data;
      cipherText = uint8ArrayToShowStr(tmpArr);
      Logger.info(TAG, `cipher text length: ${cipherText.length}`);
    } catch (error) {
      Logger.error(TAG, `update cipher failed, ${error.code}, ${error.message}`);
      return aesEncryptJsonStr;
    }

    try {
      let authTag = await cipher.doFinal(null);
      let tmoTagArr: Uint8Array = authTag.data;
      let aesEncryptJson = ({ aesGcmTag: uint8ArrayToShowStr(tmoTagArr), encryptedText: cipherText });
      aesEncryptJsonStr = JSON.stringify(aesEncryptJson);
      Logger.info(TAG, `success, authTag blob ${authTag.data}`);
      Logger.info(TAG, `success, authTag blob.length = ${authTag.data.length}`);
      return aesEncryptJsonStr;
    } catch (error) {
      Logger.error(TAG, `doFinal cipher failed, ${error.code}, ${error.message}`);
      return aesEncryptJsonStr;
    }
  }

  /**
   * AES-GCM解密，支持二进制数据
   */
  async aesGcmDecrypt(globalKey, aesEncryptJsonStr: string): Promise<string> {
    let cipherAlgName = 'AES256|GCM|PKCS7';
    let decode;
    let plainTextBlob;
    let plainText: string;
    let aesEncryptJson;

    try {
      aesEncryptJson = JSON.parse(aesEncryptJsonStr);
    } catch (error) {
      Logger.error(TAG, `trans from json string failed, ${error.code}, ${error.message}`);
      return null;
    }

    let authTagStr: string = aesEncryptJson.aesGcmTag;
    let textString: string = aesEncryptJson.encryptedText;
    let globalGcmParams = genGcmParamsSpec();
    globalGcmParams.authTag = { data: fromHexString(authTagStr) };
    Logger.info(TAG, 'success, decrypt authTag string' + authTagStr);
    Logger.info(TAG, 'success, decrypt authTag blob' + globalGcmParams.authTag.data);
    Logger.info(TAG, 'success, decrypt authTag blob.length = ' + globalGcmParams.authTag.data.length);

    try {
      decode = cryptoFramework.createCipher(cipherAlgName);
    } catch (error) {
      Logger.error(TAG, `createCipher failed, ${error.code}, ${error.message}`);
      return null;
    }

    let mode = cryptoFramework.CryptoMode.DECRYPT_MODE;
    try {
      await decode.init(mode, globalKey, globalGcmParams);
    } catch (error) {
      Logger.error(TAG, `init decode failed, ${error.code}, ${error.message}`);
      return null;
    }

    let cipherText = { data: fromHexString(textString) };
    Logger.info(TAG, `success, cipher text length: ${cipherText.data.length}`);

    try {
      plainTextBlob = await decode.update(cipherText);
      let tmpArr: Uint8Array = plainTextBlob.data;
      plainText = arrayBufferToString(tmpArr);
      Logger.info(TAG, `success, plain text length: ${plainText.length}`);
    } catch (error) {
      Logger.error(TAG, `update decode failed, ${error.code}, ${error.message}`);
      return null;
    }

    try {
      let finalOut = await decode.doFinal(null);
    } catch (error) {
      Logger.error(TAG, `doFinal decode failed, ${error.code}, ${error.message}`);
      return null;
    }

    return plainText;
  }

  /**
   * AES-GCM二进制数据加密
   */
  async aesGcmEncryptBinary(globalKey, data: ArrayBuffer): Promise<BinaryOperationResult> {
    let cipherAlgName = 'AES256|GCM|PKCS7';
    let cipher;
    let globalGcmParams = genGcmParamsSpec();

    try {
      cipher = cryptoFramework.createCipher(cipherAlgName);
      Logger.info(TAG, `Binary encryption started, data size: ${data.byteLength} bytes`);
    } catch (error) {
      Logger.error(TAG, `createCipher failed, ${error.code}, ${error.message}`);
      return { success: false, error: `createCipher failed: ${error.message}` };
    }

    let mode = cryptoFramework.CryptoMode.ENCRYPT_MODE;
    try {
      await cipher.init(mode, globalKey, globalGcmParams);
    } catch (error) {
      Logger.error(TAG, `init cipher failed, ${error.code}, ${error.message}`);
      return { success: false, error: `init cipher failed: ${error.message}` };
    }

    try {
      const plainText = { data: new Uint8Array(data) };
      const cipherTextBlob = await cipher.update(plainText);
      const authTag = await cipher.doFinal(null);

      // 返回二进制数据而不是十六进制字符串
      return {
        success: true,
        data: cipherTextBlob.data.buffer,
        tag: authTag.data.buffer
      };
    } catch (error) {
      Logger.error(TAG, `encryption failed, ${error.code}, ${error.message}`);
      return { success: false, error: `encryption failed: ${error.message}` };
    }
  }

  /**
   * AES-GCM二进制数据解密
   */
  async aesGcmDecryptBinary(globalKey, cipherText: ArrayBuffer, tag: ArrayBuffer): Promise<BinaryOperationResult> {
    let cipherAlgName = 'AES256|GCM|PKCS7';
    let decode;
    let globalGcmParams = genGcmParamsSpec();
    globalGcmParams.authTag = { data: new Uint8Array(tag) };

    try {
      decode = cryptoFramework.createCipher(cipherAlgName);
    } catch (error) {
      Logger.error(TAG, `createCipher failed, ${error.code}, ${error.message}`);
      return { success: false, error: `createCipher failed: ${error.message}` };
    }

    let mode = cryptoFramework.CryptoMode.DECRYPT_MODE;
    try {
      await decode.init(mode, globalKey, globalGcmParams);
    } catch (error) {
      Logger.error(TAG, `init decode failed, ${error.code}, ${error.message}`);
      return { success: false, error: `init decode failed: ${error.message}` };
    }

    try {
      const cipherData = { data: new Uint8Array(cipherText) };
      const plainTextBlob = await decode.update(cipherData);
      await decode.doFinal(null);

      return {
        success: true,
        data: plainTextBlob.data.buffer
      };
    } catch (error) {
      Logger.error(TAG, `decryption failed, ${error.code}, ${error.message}`);
      return { success: false, error: `decryption failed: ${error.message}` };
    }
  }

  /**
   * 流式AES加密处理（用于大文件）
   */
  async aesGcmEncryptStream(globalKey, dataStream: ArrayBuffer,
    progressCallback?: StreamProgressCallback): Promise<BinaryOperationResult> {
    let cipherAlgName = 'AES256|GCM|PKCS7';
    let cipher;
    let globalGcmParams = genGcmParamsSpec();
    let encryptedChunks: Uint8Array[] = [];

    try {
      cipher = cryptoFramework.createCipher(cipherAlgName);
      await cipher.init(cryptoFramework.CryptoMode.ENCRYPT_MODE, globalKey, globalGcmParams);
    } catch (error) {
      Logger.error(TAG, `Stream cipher init failed, ${error.code}, ${error.message}`);
      return { success: false, error: `cipher init failed: ${error.message}` };
    }

    try {
      const data = new Uint8Array(dataStream);
      let totalProcessed = 0;

      // 分块处理大数据
      for (let offset = 0; offset < data.length; offset += CHUNK_SIZE) {
        const chunk = data.slice(offset, Math.min(offset + CHUNK_SIZE, data.length));
        const plainText = { data: chunk };
        const cipherTextBlob = await cipher.update(plainText);

        encryptedChunks.push(cipherTextBlob.data);
        totalProcessed += chunk.length;

        // 调用进度回调
        if (progressCallback) {
          progressCallback(totalProcessed, data.length);
        }

        Logger.info(TAG, `Processed ${totalProcessed}/${data.length} bytes`);
      }

      // 最终化加密
      const authTag = await cipher.doFinal(null);

      // 合并所有加密块
      const totalLength = encryptedChunks.reduce((sum, chunk) => sum + chunk.length, 0);
      const result = new Uint8Array(totalLength);
      let position = 0;

      for (const chunk of encryptedChunks) {
        result.set(chunk, position);
        position += chunk.length;
      }

      return {
        success: true,
        data: result.buffer,
        tag: authTag.data.buffer
      };
    } catch (error) {
      Logger.error(TAG, `Stream encryption failed, ${error.code}, ${error.message}`);
      return { success: false, error: `stream encryption failed: ${error.message}` };
    }
  }

  /**
   * 流式AES解密处理（用于大文件）
   */
  async aesGcmDecryptStream(globalKey, cipherText: ArrayBuffer, tag: ArrayBuffer,
    progressCallback?: StreamProgressCallback): Promise<BinaryOperationResult> {
    let cipherAlgName = 'AES256|GCM|PKCS7';
    let decode;
    let globalGcmParams = genGcmParamsSpec();
    globalGcmParams.authTag = { data: new Uint8Array(tag) };
    let decryptedChunks: Uint8Array[] = [];

    try {
      decode = cryptoFramework.createCipher(cipherAlgName);
      await decode.init(cryptoFramework.CryptoMode.DECRYPT_MODE, globalKey, globalGcmParams);
    } catch (error) {
      Logger.error(TAG, `Stream decoder init failed, ${error.code}, ${error.message}`);
      return { success: false, error: `decoder init failed: ${error.message}` };
    }

    try {
      const data = new Uint8Array(cipherText);
      let totalProcessed = 0;

      // 分块处理解密
      for (let offset = 0; offset < data.length; offset += CHUNK_SIZE) {
        const chunk = data.slice(offset, Math.min(offset + CHUNK_SIZE, data.length));
        const cipherData = { data: chunk };
        const plainTextBlob = await decode.update(cipherData);

        decryptedChunks.push(plainTextBlob.data);
        totalProcessed += chunk.length;

        // 调用进度回调
        if (progressCallback) {
          progressCallback(totalProcessed, data.length);
        }

        Logger.info(TAG, `Decrypted ${totalProcessed}/${data.length} bytes`);
      }

      await decode.doFinal(null);

      // 合并所有解密块
      const totalLength = decryptedChunks.reduce((sum, chunk) => sum + chunk.length, 0);
      const result = new Uint8Array(totalLength);
      let position = 0;

      for (const chunk of decryptedChunks) {
        result.set(chunk, position);
        position += chunk.length;
      }

      return {
        success: true,
        data: result.buffer
      };
    } catch (error) {
      Logger.error(TAG, `Stream decryption failed, ${error.code}, ${error.message}`);
      return { success: false, error: `stream decryption failed: ${error.message}` };
    }
  }

  async aesConvertAndEncrypt(aesKeyBlobString: string, textString: string): Promise<string> {
    let aesEncryptJsonStr = '';
    try {
      let key = await this.convertAesKey(aesKeyBlobString);
      try {
        aesEncryptJsonStr = await this.aesGcmEncrypt(key, textString);
      } catch (error) {
        Logger.error(TAG, `encrypt error, ${error.code}, ${error.message}`);
      }
    } catch (error) {
      Logger.error(TAG, `convert key error, ${error.code}, ${error.message}`);
      return null;
    }
    return aesEncryptJsonStr;
  }

  /**
   * 二进制数据加密
   */
  async aesConvertAndEncryptBinary(aesKeyBlobString: string, data: ArrayBuffer): Promise<BinaryOperationResult> {
    try {
      const key = await this.convertAesKey(aesKeyBlobString);
      return await this.aesGcmEncryptBinary(key, data);
    } catch (error) {
      Logger.error(TAG, `convert key or encrypt error, ${error.code}, ${error.message}`);
      return { success: false, error: `encryption failed: ${error.message}` };
    }
  }

  async aesConvertAndDecrypt(aesKeyBlobString: string, textString: string): Promise<string> {
    let plainText = '';
    try {
      let key = await this.convertAesKey(aesKeyBlobString);
      try {
        plainText = await this.aesGcmDecrypt(key, textString);
      } catch (error) {
        Logger.error(TAG, `encrypt error, ${error.code}, ${error.message}`);
      }
    } catch (error) {
      Logger.error(TAG, `convert key error, ${error.code}, ${error.message}`);
      return null;
    }
    return plainText;
  }

  /**
   * 二进制数据解密
   */
  async aesConvertAndDecryptBinary(aesKeyBlobString: string, cipherText: ArrayBuffer,
    tag: ArrayBuffer): Promise<BinaryOperationResult> {
    try {
      const key = await this.convertAesKey(aesKeyBlobString);
      return await this.aesGcmDecryptBinary(key, cipherText, tag);
    } catch (error) {
      Logger.error(TAG, `convert key or decrypt error, ${error.code}, ${error.message}`);
      return { success: false, error: `decryption failed: ${error.message}` };
    }
  }

  async generateRsaKey(): Promise<string> {
    let rsaKeyGenerator;
    let jsonStr;
    try {
      rsaKeyGenerator = cryptoFramework.createAsyKeyGenerator('RSA3072');
    } catch (error) {
      Logger.error(TAG, 'create generator failed');
      return null;
    }

    try {
      let keyPair = await rsaKeyGenerator.generateKeyPair();
      let encodedPriKey = keyPair.priKey.getEncoded();
      let priKeyData: Uint8Array = encodedPriKey.data;
      let encodedPubKey = keyPair.pubKey.getEncoded();
      let pubKeyData: Uint8Array = encodedPubKey.data;
      let rsaKeyJson = ({ priKey: uint8ArrayToShowStr(priKeyData), pubKey: uint8ArrayToShowStr(pubKeyData) });
      jsonStr = JSON.stringify(rsaKeyJson);
      Logger.info(TAG, 'success, key string: ' + jsonStr.length);
      return jsonStr;
    } catch (error) {
      Logger.error(TAG, 'create symKey failed');
      return null;
    }
  }

  async convertRsaKey(rsaJsonString: string): Promise<cryptoFramework.KeyPair> {
    let rsaKeyGenerator = cryptoFramework.createAsyKeyGenerator('RSA3072');
    Logger.info(TAG, 'success, read key string' + rsaJsonString.length);
    let jsonRsaKeyBlob;
    try {
      jsonRsaKeyBlob = JSON.parse(rsaJsonString);
    } catch (error) {
      Logger.error(TAG, `trans from json string failed, ${error.code}, ${error.message}`);
      return null;
    }
    let priKeyStr: string = jsonRsaKeyBlob.priKey;
    let pubKeyStr: string = jsonRsaKeyBlob.pubKey;
    Logger.info(TAG, 'success, read rsa pri str ' + priKeyStr.length);
    Logger.info(TAG, 'success, read rsa pub str ' + pubKeyStr.length);
    let priKeyBlob = fromHexString(priKeyStr);
    let pubKeyBlob = fromHexString(pubKeyStr);
    Logger.info(TAG, 'success, read rsa pri blob key ' + priKeyBlob.length);
    Logger.info(TAG, 'success, read rsa pub blob key ' + pubKeyBlob.length);
    try {
      let key: cryptoFramework.KeyPair = await rsaKeyGenerator.convertKey({ data: pubKeyBlob }, { data: priKeyBlob });
      return key;
    } catch (error) {
      Logger.error(TAG, `convert rsa key failed, ${error.code}, ${error.message}`);
      return null;
    }
  }

  /**
   * RSA签名，支持二进制数据
   */
  async rsaSign(globalKey, data: string | ArrayBuffer): Promise<string> {
    let signer = cryptoFramework.createSign('RSA3072|PKCS1|SHA256');
    let keyPair = globalKey;

    try {
      await signer.init(keyPair.priKey);

      // 支持字符串和二进制数据输入
      let signBlob;
      if (typeof data === 'string') {
        signBlob = stringToUint8Array(data);
      } else {
        signBlob = new Uint8Array(data);
      }

      try {
        let signedBlob = await signer.sign({ data: signBlob });
        let tmpArr: Uint8Array = signedBlob.data;
        Logger.info(TAG, 'success,RSA sign output is' + signedBlob.data.length);
        let rsaSignedBlobString = uint8ArrayToShowStr(tmpArr);
        Logger.info(TAG, 'success,RSA sign string is' + rsaSignedBlobString);
        return rsaSignedBlobString;
      } catch (error1) {
        Logger.error(TAG, `sign text failed, ${error1.code}, ${error1.message}`);
        return null;
      }
    } catch (error) {
      Logger.error(TAG, `sign init failed, ${error.code}, ${error.message}`);
      return null;
    }
  }

  /**
   * RSA验签，支持二进制数据
   */
  async rsaVerify(globalKey, data: string | ArrayBuffer, rsaSignedText: string): Promise<Boolean> {
    let verifyer = cryptoFramework.createVerify('RSA3072|PKCS1|SHA256');
    let keyPair = globalKey;

    // 支持字符串和二进制数据输入
    let signBlob;
    if (typeof data === 'string') {
      signBlob = stringToUint8Array(data);
    } else {
      signBlob = new Uint8Array(data);
    }

    let signedBlob = fromHexString(rsaSignedText);
    Logger.info(TAG, 'success,RSA sign input length ' + signBlob.length);
    Logger.info(TAG, 'success,RSA signed file length ' + signedBlob.length);

    try {
      await verifyer.init(keyPair.pubKey);
      try {
        let result: Boolean = await verifyer.verify({ data: signBlob }, { data: signedBlob });
        if (result === false) {
          Logger.error(TAG, 'RSA Verify result = fail');
        } else {
          Logger.info(TAG, 'success, RSA Verify result = success');
        }
        return result;
      } catch (error) {
        Logger.error(TAG, `verify dofinal failed, ${error.code}, ${error.message}`);
      }
    } catch (err) {
      Logger.error(TAG, `verify init failed, ${err.code}, ${err.message}`);
    }
    return null;
  }

  /**
   * RSA二进制数据签名
   */
  async rsaSignBinary(globalKey, data: ArrayBuffer): Promise<BinaryOperationResult> {
    let signer = cryptoFramework.createSign('RSA3072|PKCS1|SHA256');
    let keyPair = globalKey;

    try {
      await signer.init(keyPair.priKey);
      const signBlob = { data: new Uint8Array(data) };
      const signedBlob = await signer.sign(signBlob);

      return {
        success: true,
        data: signedBlob.data.buffer
      };
    } catch (error) {
      Logger.error(TAG, `RSA sign failed, ${error.code}, ${error.message}`);
      return { success: false, error: `RSA sign failed: ${error.message}` };
    }
  }

  /**
   * RSA二进制数据验签
   */
  async rsaVerifyBinary(globalKey, data: ArrayBuffer,
    signature: ArrayBuffer): Promise<{ success: boolean, valid?: boolean, error?: string }> {
    let verifyer = cryptoFramework.createVerify('RSA3072|PKCS1|SHA256');
    let keyPair = globalKey;

    try {
      await verifyer.init(keyPair.pubKey);
      const signBlob = { data: new Uint8Array(data) };
      const signatureBlob = { data: new Uint8Array(signature) };
      const result: Boolean = await verifyer.verify(signBlob, signatureBlob);

      return {
        success: true,
        valid: result.valueOf()
      };
    } catch (error) {
      Logger.error(TAG, `RSA verify failed, ${error.code}, ${error.message}`);
      return { success: false, error: `RSA verify failed: ${error.message}` };
    }
  }

  /**
   * 文件哈希计算（用于完整性验证）
   */
  async calculateFileHash(data: ArrayBuffer, algorithm: string = 'SHA256'): Promise<BinaryOperationResult> {
    try {
      // 1. 创建指定算法的消息摘要计算实例
      const hash = cryptoFramework.createMd(algorithm);

      // 2. 传入数据
      // 将ArrayBuffer转换为Uint8Array并封装为DataBlob
      let dataBlob: cryptoFramework.DataBlob = { data: new Uint8Array(data) };
      await hash.update(dataBlob);

      // 3. 最终计算并获取哈希值结果
      const hashBlob = await hash.digest();

      return {
        success: true,
        data: hashBlob.data.buffer // 返回原始二进制数据
      };
    } catch (error) {
      Logger.error(TAG, `Hash calculation failed, ${error.code}, ${error.message}`);
      return {
        success: false,
        error: `Hash calculation failed: ${error.message}`
      };
    }
  }

  /**
   * 数据完整性验证
   */
  async verifyDataIntegrity(data: ArrayBuffer, expectedHash: ArrayBuffer,
    algorithm: string = 'SHA256'): Promise<boolean> {
    try {
      const hashResult = await this.calculateFileHash(data, algorithm);
      if (!hashResult.success) {
        return false;
      }

      const actualHash = new Uint8Array(hashResult.data);
      const expected = new Uint8Array(expectedHash);

      if (actualHash.length !== expected.length) {
        return false;
      }

      for (let i = 0; i < actualHash.length; i++) {
        if (actualHash[i] !== expected[i]) {
          return false;
        }
      }

      return true;
    } catch (error) {
      Logger.error(TAG, `Integrity verification failed, ${error.code}, ${error.message}`);
      return false;
    }
  }

  async rsaConvertAndSign(rsaJsonString: string, textString: string): Promise<string> {
    let rsaSignString;
    try {
      let key = await this.convertRsaKey(rsaJsonString);
      try {
        rsaSignString = await this.rsaSign(key, textString);
      } catch (error) {
        Logger.error(TAG, `sign error, ${error.code}, ${error.message}`);
        return null;
      }
    } catch (error) {
      Logger.error(TAG, `convert rsa key error, ${error.code}, ${error.message}`);
      return null;
    }
    return rsaSignString;
  }

  /**
   * RSA二进制数据签名
   */
  async rsaConvertAndSignBinary(rsaJsonString: string, data: ArrayBuffer): Promise<BinaryOperationResult> {
    try {
      const key = await this.convertRsaKey(rsaJsonString);
      return await this.rsaSignBinary(key, data);
    } catch (error) {
      Logger.error(TAG, `convert key or sign error, ${error.code}, ${error.message}`);
      return { success: false, error: `RSA sign failed: ${error.message}` };
    }
  }

  async rsaConvertAndVerify(rsaJsonString: string, textString: string, rsaSignedText: string): Promise<Boolean> {
    let rsaVerifyRes;
    try {
      let key = await this.convertRsaKey(rsaJsonString);
      try {
        rsaVerifyRes = await this.rsaVerify(key, textString, rsaSignedText);
      } catch (error) {
        Logger.error(TAG, `sign error, ${error.code}, ${error.message}`);
        return null;
      }
    } catch (error) {
      Logger.error(TAG, `convert rsa key error, ${error.code}, ${error.message}`);
      return null;
    }
    return rsaVerifyRes;
  }

  /**
   * RSA二进制数据验签
   */
  async rsaConvertAndVerifyBinary(rsaJsonString: string, data: ArrayBuffer,
    signature: ArrayBuffer): Promise<{ success: boolean, valid?: boolean, error?: string }> {
    try {
      const key = await this.convertRsaKey(rsaJsonString);
      return await this.rsaVerifyBinary(key, data, signature);
    } catch (error) {
      Logger.error(TAG, `convert key or verify error, ${error.code}, ${error.message}`);
      return { success: false, error: `RSA verify failed: ${error.message}` };
    }
  }
}