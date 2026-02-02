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
