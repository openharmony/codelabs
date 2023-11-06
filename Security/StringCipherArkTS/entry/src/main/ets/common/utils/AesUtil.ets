/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import cryptoFramework from '@ohos.security.cryptoFramework';
import CommonConstants from '../constants/CommonConstants';
import DataTransformUtil from '../utils/DataTransformUtil';
import Logger from '../utils/Logger';
import { User } from '../../viewmodel/User';

class AesUtil {
  private globalCipher: cryptoFramework.Cipher = cryptoFramework.createCipher(CommonConstants.GENERATOR_NAME);
  private globalKey: cryptoFramework.SymKey | null = null;

  /**
   * Constructor initialization to generate a key.
   */
  constructor() {
    let symAlgName = CommonConstants.ENCRYPTION_MODE;
    let symKeyGenerator = cryptoFramework.createSymKeyGenerator(symAlgName);
    // Randomly generate a 256-bit symmetric key by using a key generator.
    let keyMaterialBlob = this.genKeyMaterialBlob(CommonConstants.KEY_DATA);
    symKeyGenerator.convertKey(keyMaterialBlob, (err, symKey) => {
      if (err) {
        Logger.error(`Convert symKey failed, ${err.code}, ${err.message}`);
        return;
      }
      this.globalKey = symKey;
      // Generating an encryption/Decryption generator.
      let cipherAlgName = CommonConstants.GENERATOR_NAME;
      try {
        this.globalCipher = cryptoFramework.createCipher(cipherAlgName);
      } catch (error) {
        Logger.error(`createCipher failed, error is ${JSON.stringify(err)}`);
      }
    });
  }

  /**
   * Data of the DataBlob type is generated based on the data group.
   *
   * @param data Data to be encapsulated.
   * @returns DataBlob data
   */
  genKeyMaterialBlob(data: Array<number>): cryptoFramework.DataBlob {
    let keyMaterial = new Uint8Array(data);
    return { data: keyMaterial };
  }

  /**
   * Obtaining parameters required for GCM encryption and decryption.
   *
   * @returns Parameters required for encryption and decryption.
   */
  async genGcmParamsSpec(): Promise<cryptoFramework.GcmParamsSpec> {
    let ivBlob: cryptoFramework.DataBlob = this.genKeyMaterialBlob(CommonConstants.GCM_IV_DATA);
    let aadBlob: cryptoFramework.DataBlob = this.genKeyMaterialBlob(CommonConstants.GCM_AAD_DATA);
    let tagBlob: cryptoFramework.DataBlob = this.genKeyMaterialBlob(CommonConstants.GCM_TAG_DATA);
    let gcmParamsSpec: cryptoFramework.GcmParamsSpec = {
      iv: ivBlob,
      aad: aadBlob,
      authTag: tagBlob,
      algName: `GcmParamsSpec`
    };
    return gcmParamsSpec;
  }

  /**
   * Encrypted
   *
   * @param content Encrypted content.
   * @returns Promise object with ciphertext.
   */
  async encrypt(content: string): Promise<User> {
    // Initialize Encryption Operating Environment: Start Encryption
    let mode = cryptoFramework.CryptoMode.ENCRYPT_MODE;
    let gcmParams = await this.genGcmParamsSpec();
    await this.globalCipher.init(mode, this.globalKey, gcmParams);
    let plainTextBlob: cryptoFramework.DataBlob = {
      data: DataTransformUtil.stringToUint8Array(content)
    };
    let updateOutput: cryptoFramework.DataBlob = await this.globalCipher.update(plainTextBlob);
    if (!updateOutput) {
      return Promise.reject('encrypt updateOutput is null');
    }
    let authTag: cryptoFramework.DataBlob = await this.globalCipher.doFinal(null);
    let encryptContent: string = DataTransformUtil.uint8ArrayToBase64(updateOutput.data);
    let authTagContent: string = DataTransformUtil.uint8ArrayToBase64(authTag.data);
    let user = new User(null, ``, encryptContent, authTagContent);
    return user;
  }

  /**
   * Decryption
   *
   * @param content Decryption content.
   * @param authTag AuthTag content.
   * @returns Promise object with decrypted content.
   */
  async decrypt(content: string, authTag: string): Promise<string> {
    // Initialize the encryption operating environment: Start decryption.
    let mode = cryptoFramework.CryptoMode.DECRYPT_MODE;
    let gcmParams = await this.genGcmParamsSpec();
    let authTagBlob: cryptoFramework.DataBlob = {
      data: DataTransformUtil.base64ToUint8Array(authTag)
    };
    gcmParams.authTag = authTagBlob;
    await this.globalCipher.init(mode, this.globalKey, gcmParams);
    let plainTextBlob: cryptoFramework.DataBlob = {
      data: DataTransformUtil.base64ToUint8Array(content)
    };
    let finalOutput: cryptoFramework.DataBlob = await this.globalCipher.doFinal(plainTextBlob);
    if (!finalOutput) {
      return Promise.reject('decrypt finalOutput is null');
    }
    let decryptContent = DataTransformUtil.uint8ArrayToString(finalOutput.data);
    return decryptContent;
  }
}

export default new AesUtil();