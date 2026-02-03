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

import router from '@ohos.router';
import RdbStore from '../utils/RdbStore';
import { Contact } from '../model/Contact';
import ContactListItem from './ContactListItem';
import UserPreferences from '../utils/UserPreferences';

@Entry
@Component
struct ContactListPage {
  @State contacts: Array<Contact> = [];

  onPageShow() {
    this.refresh();
  }

  async refresh() {
    let userId = await UserPreferences.getUserId();
    this.contacts = await RdbStore.queryAll(userId);
  }

  build() {
    Column() {
      // 简单的标题
      Text('药品列表')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin(20)

      List() {
        ForEach(this.contacts, (item: Contact) => {
          ListItem() {
            ContactListItem({ contact: item })
              .onClick(() => {
                router.pushUrl({
                  url: 'pages/ContactDetailPage',
                  params: { contactId: item.id }
                });
              })
          }
        }, (item: Contact) => JSON.stringify(item))
      }
      .width('100%')
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F7F8FA')
  }
}