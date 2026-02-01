/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import router from '@ohos.router'
import promptAction from '@ohos.promptAction'
import { Contact } from '../model/Contact'
import RdbStore from '../utils/RdbStore'
import UserPreferences from '../utils/UserPreferences'

interface DetailParams {
  contactId?: number
}

@Entry
@Component
struct ContactDetailPage {
  @State contact: Contact | undefined = undefined

  async aboutToAppear() {
    const params = router.getParams() as DetailParams
    if (params.contactId === undefined) {
      return
    }

    const userId = await UserPreferences.getUserId()
    const all = await RdbStore.queryAll(userId)
    this.contact = all.find(i => i.id === params.contactId)
  }

  build() {
    Navigation() {
      Column() {
        if (this.contact) {
          Text(this.contact.name).fontSize(20)
          Text(`有效期：${this.contact.phone}`)
          Text(`分类：${this.contact.company}`)
        }
      }.padding(20)
    }
    .title('药品详情')
  }
}
