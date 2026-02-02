/**
 * CryptoUtil
 *
 * Low-level cryptographic utility.
 * Real encryption logic will be implemented later.
 */

import { EncryptedData } from '../model/EncryptedData'

export class CryptoUtil {

  /**
   * Encrypt plain text to EncryptedData
   * (Temporary mock implementation)
   */
  static encrypt(plainText: string): EncryptedData {
    return {
      cipherText: plainText,   // mock: no real encryption yet
      iv: '',
      algorithm: 'MOCK',
      keyVersion: 1,
      timestamp: Date.now()
    }
  }

  /**
   * Decrypt EncryptedData back to plain text
   * (Temporary mock implementation)
   */
  static decrypt(data: EncryptedData): string {
    return data.cipherText
  }
}
