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
