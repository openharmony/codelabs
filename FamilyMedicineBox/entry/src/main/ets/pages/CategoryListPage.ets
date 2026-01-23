/*
 * Copyright (c) 2025 Beijing Institute of Technology.
 *
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

  async aboutToAppear() {
    const params = router.getParams() as Record<string, Object>;
    if (params && params['category']) {
      this.categoryName = params['category'] as string;
      this.loadData();
    }
  }

  async loadData() {
    // 1. 鑾峰彇鎵€鏈?    let all = await RdbStore.queryAll(1);
    // 2. 鏍规嵁鍒嗙被鍚嶇О杩囨护 (瀛楁鏄?company)
    this.filteredList = all.filter(item => item.company === this.categoryName);
  }

  build() {
    Navigation() {
      Column() {
        List() {
          if (this.filteredList.length === 0) {
            ListItem() {
              Column() {
                Text('馃摝').fontSize(50)
                Text(`娌℃湁鎵惧埌 "${this.categoryName}" 绫荤殑鑽搧`).fontColor(Color.Gray).margin({top:10})
              }.width('100%').height(300).justifyContent(FlexAlign.Center)
            }
          } else {
            ForEach(this.filteredList, (item: Contact) => {
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
    .title(this.categoryName).titleMode(NavigationTitleMode.Mini)
  }
}

