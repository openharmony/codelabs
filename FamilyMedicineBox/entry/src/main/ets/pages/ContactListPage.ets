/*
 * Copyright (c) 2026 Beijing Institute of Technology.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE--2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

  onPageShow() {
    this.refresh();
  }

  async refresh() {
    // 绠€鍗曞鐞嗭紝鐩存帴鏌ユ墍鏈?    this.contacts = await RdbStore.queryAll(1);
  }

  build() {
    Column() {
      // 绠€鍗曠殑鏍囬
      Text('鑽搧鍒楄〃')
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
      .height('100%')
    }
    .width('100%')
    .height('100%')
  }
}



