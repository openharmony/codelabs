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
