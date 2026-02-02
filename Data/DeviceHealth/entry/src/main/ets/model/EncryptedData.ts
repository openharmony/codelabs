/**
 * EncryptedData
 *
 * Unified model for encrypted content and encryption metadata.
 * Used by SecurityService and database layer.
 */
export interface EncryptedData {
  /**
   * Encrypted cipher text (Base64 encoded)
   */
  cipherText: string;

  /**
   * Initialization Vector (Base64 encoded)
   */
  iv: string;

  /**
   * Encryption algorithm identifier
   * e.g. AES_GCM
   */
  algorithm: string;

  /**
   * Key version for key rotation
   */
  keyVersion: number;

  /**
   * Encryption timestamp
   */
  timestamp: number;
}
