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

    // 2. 绛涢€夊嚭涓存湡/杩囨湡鐨?(閫昏緫鍜岄椤典繚鎸佷竴鑷?
    // 杩欓噷鍋囪鍖呭惈 '2024' 鎴?'2025' 鐨勯兘鏄复鏈燂紝浣犲彲浠ユ牴鎹疄闄呮棩鏈熸瘮杈冮€昏緫淇敼
    this.expiryList = allList.filter(item => {
      return item.phone.includes('2024') || item.phone.includes('2025');
    });
  }

  // 鍚屾牱鍔犱笂宸︽粦鍒犻櫎鍔熻兘锛屾柟渚垮湪杩欓噷鐩存帴鎵旀帀杩囨湡鑽?  @Builder itemEnd(index: number, id: number) {
    Row() {
      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('涓㈠純').fontColor(Color.White).fontSize(14)
      }
      .width(60).height(60).backgroundColor('#FF3B30').margin({ left: 10 })
      .onClick(async () => {
        await RdbStore.delete(id);
        this.expiryList.splice(index, 1); // 鍒锋柊鏈湴鍒楄〃
      })
    }.padding({ left: 10, right: 10 }).justifyContent(FlexAlign.End)
  }

  build() {
    Navigation() {
      Column() {
        // 椤堕儴鎻愮ず鏉?        Row() {
          Text('鈿狅笍 浠ヤ笅鑽搧鍗冲皢杩囨湡鎴栧凡杩囨湡锛岃鍙婃椂澶勭悊')
            .fontSize(14).fontColor('#D32F2F')
        }
        .width('100%').padding(10).backgroundColor('#FFEBEE')

        List() {
          if (this.expiryList.length === 0) {
            ListItem() {
              Column() {
                Text('馃帀').fontSize(50).margin({bottom:10})
                Text('澶浜嗭紝娌℃湁涓存湡鑽搧锛?).fontColor(Color.Gray)
              }.width('100%').height(300).justifyContent(FlexAlign.Center)
            }
          } else {
            ForEach(this.expiryList, (item: Contact, index: number) => {
              ListItem() {
                // 澶嶇敤鍒楄〃椤规牱寮?                ContactListItem({ contact: item })
              }
              .swipeAction({ end: this.itemEnd(index, item.id!) }) // 鏀寔宸︽粦鍒犻櫎
              .margin({ left: '4%', right: '4%', top: 10 })
              .onClick(() => {
                // 鐐瑰嚮涔熻兘杩涜鎯?                router.pushUrl({
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
    .title('涓存湡/杩囨湡棰勮')
    .titleMode(NavigationTitleMode.Mini)
  }
}


