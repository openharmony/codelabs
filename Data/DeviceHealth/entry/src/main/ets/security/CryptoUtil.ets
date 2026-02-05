/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

/**
 * CryptoUtil
 *
 * Project-safe reversible encoding utility.
 * Designed for replaceable encryption strategy.
 */

import { EncryptedData } from '../model/EncryptedData'
import util from '@ohos.util'

const KEY_VERSION: number = 1
const ALGORITHM: string = 'PROJECT_ENCODING'

export class CryptoUtil {

  /**
   * Encode plain text (reversible)
   */
  static encrypt(plainText: string): EncryptedData {
    const encoder = new util.TextEncoder()
    const bytes: Uint8Array = encoder.encode(plainText)

    // Encode bytes to string safely
    const encoded: string = CryptoUtil.bytesToString(bytes)

    return {
      cipherText: encoded,
      iv: 'N/A',
      algorithm: ALGORITHM,
      keyVersion: KEY_VERSION,
      timestamp: Date.now()
    }
  }

  /**
   * Decode encoded text back to plain text
   */
  static decrypt(data: EncryptedData): string {
    const bytes: Uint8Array = CryptoUtil.stringToBytes(data.cipherText)
    const decoder = new util.TextDecoder()
    return decoder.decode(bytes)
  }

  /**
   * Uint8Array -> string
   * Safe reversible encoding
   */
  private static bytesToString(bytes: Uint8Array): string {
    let result = ''
    for (let i = 0; i < bytes.length; i++) {
      result += String.fromCharCode(bytes[i])
    }
    return result
  }

  /**
   * string -> Uint8Array
   */
  private static stringToBytes(str: string): Uint8Array {
    const bytes = new Uint8Array(str.length)
    for (let i = 0; i < str.length; i++) {
      bytes[i] = str.charCodeAt(i)
    }
    return bytes
  }
}
