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
 * SecurityService
 *
 * Unified encryption / decryption service for DeviceHealth application.
 */

import { EncryptedData } from '../model/EncryptedData'
import { CryptoUtil } from './CryptoUtil'

/**
 * Plain event log model (before encryption)
 */
export interface PlainEventLog {
  type: string
  level: number
  timestamp: number
  detail?: string
  extra?: string
}

/**
 * Encrypted event log row (stored in database)
 */
export interface EncryptedEventLogRow {
  type: string
  level: number
  timestamp: number

  enc_detail?: string | null
  enc_extra?: string | null

  enc_iv?: string | null
  enc_alg?: string | null
  enc_key_ver?: number | null
}

export class SecurityService {

  /**
   * Encrypt plain text (for user settings, configs, etc.)
   */
  static encryptText(plainText: string): EncryptedData {
    if (!plainText || plainText.length === 0) {
      throw new Error('SecurityService.encryptText: plainText is empty')
    }
    return CryptoUtil.encrypt(plainText)
  }

  /**
   * Decrypt encrypted data back to plain text
   */
  static decryptText(data: EncryptedData): string {
    if (!data) {
      throw new Error('SecurityService.decryptText: encrypted data is null')
    }
    return CryptoUtil.decrypt(data)
  }

  /**
   * Encrypt sensitive fields of event log
   */
  static encryptLogFields(log: PlainEventLog): EncryptedEventLogRow {
    const encryptedDetail: EncryptedData | null = log.detail
      ? CryptoUtil.encrypt(log.detail)
      : null

    const encryptedExtra: EncryptedData | null = log.extra
      ? CryptoUtil.encrypt(log.extra)
      : null

    return {
      type: log.type,
      level: log.level,
      timestamp: log.timestamp,

      enc_detail: encryptedDetail?.cipherText ?? null,
      enc_extra: encryptedExtra?.cipherText ?? null,

      enc_iv: encryptedDetail?.iv ?? null,
      enc_alg: encryptedDetail?.algorithm ?? null,
      enc_key_ver: encryptedDetail?.keyVersion ?? null
    }
  }

  /**
   * Decrypt encrypted database row back to plain event log
   */
  static decryptLogFields(row: EncryptedEventLogRow): PlainEventLog {
    const detail: string | undefined = row.enc_detail
      ? CryptoUtil.decrypt({
        cipherText: row.enc_detail,
        iv: row.enc_iv ?? '',
        algorithm: row.enc_alg ?? '',
        keyVersion: row.enc_key_ver ?? 0,
        timestamp: row.timestamp
      })
      : undefined

    const extra: string | undefined = row.enc_extra
      ? CryptoUtil.decrypt({
        cipherText: row.enc_extra,
        iv: row.enc_iv ?? '',
        algorithm: row.enc_alg ?? '',
        keyVersion: row.enc_key_ver ?? 0,
        timestamp: row.timestamp
      })
      : undefined

    return {
      type: row.type,
      level: row.level,
      timestamp: row.timestamp,
      detail,
      extra
    }
  }
}
