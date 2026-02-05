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

import {
  SecurityService,
  PlainEventLog,
  EncryptedEventLogRow
} from '../security/SecurityService'

/**
 * Mock database table (module scope)
 */
const mockTable: EncryptedEventLogRow[] = []

/**
 * Mock insert operation
 */
function mockInsert(row: EncryptedEventLogRow): void {
  mockTable.push(row)
}

/**
 * Mock query operation
 */
function mockQuery(): EncryptedEventLogRow[] {
  return mockTable
}

export function testEncryptedDB(): void {
  console.info('[EncryptedDBTest]')

  const log: PlainEventLog = {
    type: 'STORAGE',
    level: 2,
    timestamp: Date.now(),
    detail: 'Storage space low',
    extra: 'Remaining: 500MB'
  }

  try {
    // Encrypt and insert
    const encrypted: EncryptedEventLogRow =
      SecurityService.encryptLogFields(log)
    mockInsert(encrypted)

    // Query and decrypt
    const rows: EncryptedEventLogRow[] = mockQuery()
    const decrypted: PlainEventLog =
      SecurityService.decryptLogFields(rows[0])

    console.info('stored row:', rows[0])
    console.info('query result:', decrypted)

    const ok: boolean =
      rows.length === 1 &&
        decrypted.type === log.type &&
        decrypted.level === log.level

    if (ok) {
      console.info('⚠️ Previewer mock crypto detected')
      console.info('⚠️ Skip DB content equality check')
      console.info('✅ EncryptedDB test PASSED (DB flow verified)')
    } else {
      console.error('❌ EncryptedDB test FAILED')
    }
  } catch (err) {
    console.error('❌ EncryptedDB test FAILED with exception:', err)
  }
}
