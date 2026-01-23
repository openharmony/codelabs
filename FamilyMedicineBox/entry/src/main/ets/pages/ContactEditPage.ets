/*
 * Copyright (c) 2026 Beijing Institute of Technology.
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

  @State phone: string = '';
  @State company: string = '';
  @State usageInfo: string = ''; // 瀵瑰簲 position 瀛楁
  @State email: string = '';

  async aboutToAppear() {
    const params = router.getParams() as Record<string, Object>;
    if (params && params['contactId']) {
      this.contactId = params['contactId'] as number;
      let list = await RdbStore.queryAll(1);
      let target = list.find(c => c.id === this.contactId);
      if (target) {
        this.name = target.name;
        this.phone = target.phone;
        this.company = target.company;
        this.usageInfo = target.position;
        this.email = target.email;
      }
    }
  }

  async save() {
    // 鏋勯€犳洿鏂板悗鐨勬暟鎹?    let updated = new Contact(this.name, this.phone);
    updated.id = this.contactId;
    updated.company = this.company;
    updated.position = this.usageInfo;
    updated.email = this.email;
    updated.userId = 1;

    // 鏇存柊鏁版嵁搴?    await RdbStore.update(updated);
    try { promptAction.showToast({ message: '鉁?淇敼淇濆瓨鎴愬姛' }); } catch(e){}
    router.back();
  }

  build() {
    Navigation() {
      Column() {
        List() {
          this.InputItem('鑽搧鍚嶇О', this.name, (val:string) => this.name = val)
          this.InputItem('鏈夋晥鏈熻嚦', this.phone, (val:string) => this.phone = val)
          this.InputItem('鍔熻兘鍒嗙被', this.company, (val:string) => this.company = val)
          this.InputItem('鐢ㄦ硶鐢ㄩ噺', this.usageInfo, (val:string) => this.usageInfo = val)
          this.InputItem('瀛樻斁浣嶇疆', this.email, (val:string) => this.email = val)
        }
        .width('95%').margin({ top: 20 })

        Button('淇濆瓨淇敼')
          .width('90%').height(50).margin({ top: 40 })
          .backgroundColor('#4A90E2').borderRadius(25)
          .onClick(() => this.save())
      }
      .width('100%').height('100%').backgroundColor('#F7F8FA')
    }
    .title('淇敼淇℃伅').titleMode(NavigationTitleMode.Mini)
  }

  @Builder InputItem(label: string, value: string, onChange: (val: string) => void) {
    ListItem() {
      Column() {
        Text(label).fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 8 })
        TextInput({ text: value })
          .height(45).width('100%').backgroundColor(Color.White)
          .borderRadius(8)
          .onChange(onChange)
      }.padding(10)
    }
  }
}




