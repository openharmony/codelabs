import { testCryptoUtil } from './CryptoUtilTest'
import { testSecurityService } from './SecurityServiceTest'
import { testEncryptedDB } from './EncryptedDBTest'

export function runAllTests(): void {
  console.info('====== DeviceHealth Security Module Tests ======')

  testCryptoUtil()
  testSecurityService()
  testEncryptedDB()

  console.info('====== All tests finished ======')
}
