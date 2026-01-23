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

  onPageShow() {
    this.loadData();
  }

  async loadData() {
    const params = router.getParams() as Record<string, Object>;
    if (params && params['contactId']) {
      this.contactId = params['contactId'] as number;
      let list = await RdbStore.queryAll(1);
      let target = list.find(c => c.id === this.contactId);
      if (target) {
        this.contact = target;
      }
    }
  }

  async deleteContact() {
    await RdbStore.delete(this.contactId);
    try { promptAction.showToast({ message: '馃棏锔?宸插垹闄? }); } catch(e){}
    router.back();
  }

  build() {
    Navigation() {
      Column() {
        // 1. 椤堕儴澶у崱鐗?        Column() {
          Stack({ alignContent: Alignment.Center }) {
            Circle({ width: 90, height: 90 }).fill('#E3F2FD')
            Text(this.contact.name ? this.contact.name.substring(0, 1) : '?')
              .fontSize(40)
              .fontWeight(FontWeight.Bold)
              .fontColor('#4A90E2')
          }
          .margin({ bottom: 15 })

          Text(this.contact.name)
            .fontSize(26)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 10 })

          Text(this.contact.company || '閫氱敤鍒嗙被')
            .fontSize(14)
            .fontColor(Color.White)
            .backgroundColor('#FF9500')
            .padding({ left: 12, right: 12, top: 4, bottom: 4 })
            .borderRadius(20)
        }
        .width('100%')
        .padding({ top: 40, bottom: 30 })
        .backgroundColor(Color.White)
        .borderRadius({ bottomLeft: 30, bottomRight: 30 })
        .shadow({ radius: 10, color: 'rgba(0,0,0,0.05)' })

        // 2. 璇︽儏鍒楄〃
        List() {
          // --- 鏈夋晥鏈?---
          ListItem() {
            Row() {
              // 銆愪慨澶嶃€戝垹闄や簡杩欓噷鐨?Image 寮曠敤锛岄槻姝㈡姤閿?              Text('馃搮 鏈夋晥鏈?).width(100).fontColor('#666666')

              Text(this.contact.phone ? this.contact.phone : '鏈缃棩鏈?)
                .layoutWeight(1)
                .textAlign(TextAlign.End)
                .fontWeight(FontWeight.Medium)
                .fontColor(this.contact.phone ?
                  (this.contact.phone.includes('2024') || this.contact.phone.includes('2025') ? '#FF3B30' : '#333333')
                  : '#CCCCCC')
            }
            .width('100%').padding(20).backgroundColor(Color.White).borderRadius(12).margin({ bottom: 10 })
          }

          // --- 鐢ㄦ硶鐢ㄩ噺 ---
          ListItem() {
            Row() {
              Text('馃拪 鐢ㄦ硶鐢ㄩ噺').width(100).fontColor('#666666')

              Text(this.contact.position ? this.contact.position : '鏆傛棤鐢ㄦ硶璇存槑')
                .layoutWeight(1)
                .textAlign(TextAlign.End)
                .fontColor(this.contact.position ? '#333333' : '#CCCCCC')
            }
            .width('100%').padding(20).backgroundColor(Color.White).borderRadius(12).margin({ bottom: 10 })
          }

          // --- 瀛樻斁浣嶇疆 ---
          ListItem() {
            Row() {
              Text('馃搷 瀛樻斁浣嶇疆').width(100).fontColor('#666666')

              Text(this.contact.email ? this.contact.email : '鏈缃綅缃?)
                .layoutWeight(1)
                .textAlign(TextAlign.End)
                .fontColor(this.contact.email ? '#333333' : '#CCCCCC')
            }
            .width('100%').padding(20).backgroundColor(Color.White).borderRadius(12).margin({ bottom: 10 })
          }
        }
        .width('95%')
        .layoutWeight(1)
        .margin({ top: 20 })

        // 3. 搴曢儴鎸夐挳
        Row() {
          Button('鉁忥笍 淇敼淇℃伅')
            .backgroundColor('#4A90E2').width('45%').height(50).borderRadius(25)
            .onClick(() => {
              router.pushUrl({
                url: 'pages/ContactEditPage',
                params: { contactId: this.contact.id }
              });
            })

          Button('馃棏锔?鍒犻櫎鑽搧')
            .backgroundColor('#FF3B30').width('45%').height(50).borderRadius(25)
            .onClick(() => {
              AlertDialog.show({
                title: '纭鍒犻櫎', message: '纭畾瑕佸垹闄ゅ悧锛?,
                primaryButton: { value: '鍙栨秷', action: () => {} },
                secondaryButton: { value: '鍒犻櫎', fontColor: 'red', action: () => this.deleteContact() }
              })
            })
        }
        .width('100%').justifyContent(FlexAlign.SpaceAround).margin({ bottom: 20 })
      }
      .width('100%').height('100%').backgroundColor('#F7F8FA')
    }
    .title('鑽搧璇︽儏').titleMode(NavigationTitleMode.Mini)
  }
}

