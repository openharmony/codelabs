/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSEt-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

  async onPageShow() {
    this.refreshList();
  }

  async refreshList() {
    if (this.searchText === '') {
      this.contacts = await RdbStore.queryAll(1);
    } else {
      this.contacts = await RdbStore.search(this.searchText, 1);
    }
  }

  build() {
    Navigation() {
      Column() {
        Search({ value: this.searchText, placeholder: '馃攳 鎼滅储搴撳瓨鑽搧...' })
          .width('95%').height(40).backgroundColor('#F0F2F5').borderRadius(20)
          .margin({ top: 10, bottom: 10 })
          .onChange((value: string) => {
            this.searchText = value;
            this.refreshList();
          })

        List() {
          if (this.contacts.length === 0) {
            ListItem() {
              Text('鏆傛棤鑽搧鏁版嵁').width('100%').textAlign(TextAlign.Center).margin({top: 50}).fontColor(Color.Gray)
            }
          } else {
            ForEach(this.contacts, (item: Contact) => {
              ListItem() {
                ContactListItem({ contact: item })
              }
              .margin({ left: '4%', right: '4%' })
              .onClick(() => {
                router.pushUrl({
                  url: 'pages/ContactDetailPage',
                  params: { contactId: item.id }
                });
              })
            }, (item: Contact) => JSON.stringify(item))
          }
        }
        .width('100%').layoutWeight(1)
      }
      .width('100%').height('100%').backgroundColor('#F7F8FA')
    }
    .title('搴撳瓨鎬昏').titleMode(NavigationTitleMode.Mini)
  }
}


