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
