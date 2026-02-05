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
 * EncryptedDB
 *
 * Database access wrapper with transparent encryption support.
 */

import { SecurityService, PlainEventLog, EncryptedEventLogRow } from '../security/SecurityService'

export class EncryptedDB {

  /**
   * Insert event log into database (encrypted)
   */
  static insertEvent(
    dbInsert: (row: EncryptedEventLogRow) => void,
    log: PlainEventLog
  ): void {
    const encryptedRow: EncryptedEventLogRow =
      SecurityService.encryptLogFields(log)

    dbInsert(encryptedRow)
  }

  /**
   * Query event logs from database and decrypt them
   */
  static queryEvents(
    dbQuery: () => EncryptedEventLogRow[]
  ): PlainEventLog[] {
    const rows: EncryptedEventLogRow[] = dbQuery()

    const result: PlainEventLog[] = []
    for (let i = 0; i < rows.length; i++) {
      result.push(SecurityService.decryptLogFields(rows[i]))
    }
    return result
  }

  /**
   * Delete events (pass-through)
   */
  static deleteEvents(
    dbDelete: () => void
  ): void {
    dbDelete()
  }
}
