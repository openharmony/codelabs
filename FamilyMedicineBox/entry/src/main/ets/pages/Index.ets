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

class CategoryModel {
  name: string; // 鍒嗙被鍚嶇О
  color: string; // 鑳屾櫙棰滆壊
  icon: string;  // 鏄剧ず鍥炬爣(Emoji)

  constructor(name: string, color: string, icon: string) {
    this.name = name;
    this.color = color;
    this.icon = icon;
  }
}

 * 娣诲姞鑷畾涔夊垎绫荤殑寮圭獥缁勪欢
 * 浣跨敤 @CustomDialog 瑁呴グ鍣?@CustomDialog
struct AddCategoryDialog {
  controller: CustomDialogController;
  // 鍥炶皟鍑芥暟锛屽皢杈撳叆鐨勭粨鏋滀紶鍥炵埗缁勪欢
  confirm: (name: string) => void = () => {};
  @State inputValue: string = '';

  build() {
    Column() {
      Text('娣诲姞鏂板垎绫?)
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ bottom: 20 })

      TextInput({ placeholder: '璇疯緭鍏ュ垎绫诲悕绉?(濡? 鐪艰嵂姘?' })
        .height(50)
        .width('100%')
        .backgroundColor('#F5F5F5')
        .borderRadius(10)
        .onChange((value: string) => {
          this.inputValue = value;
        })

      Row() {
        Button('鍙栨秷')
          .onClick(() => { this.controller.close() })
          .backgroundColor('#F5F5F5')
          .fontColor('#666666')
          .width('40%')

        Button('纭畾')
          .onClick(() => {
            // 杈撳叆闈炵┖鏍￠獙
            if (this.inputValue.trim() === '') {
              try { promptAction.showToast({ message: '鍚嶅瓧涓嶈兘涓虹┖' }); } catch(e){}
              return;
            }
            this.confirm(this.inputValue);
            this.controller.close();
          })
          .backgroundColor('#4A90E2')
          .width('40%')
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceAround)
      .margin({ top: 20 })
    }
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(20)
  }
}


@Entry
@Component
struct Index {
  // 鑽搧鍒楄〃鏁版嵁婧?  @State contacts: Array<Contact> = [];
  // 鎼滅储妗嗘枃鏈?  @State searchText: string = '';
  // 褰撳墠鐧诲綍鐢ㄦ埛ID
  @State currentUserId: number = 0;
  // 涓存湡鑽搧鏁伴噺缁熻
  @State nearExpiryCount: number = 0;

  // 榛樿鐨勭郴缁熷唴缃垎绫?  private defaultCategories: Array<CategoryModel> = [
    new CategoryModel('鎰熷啋鍙戠儳', '#FF9500', '馃尅锔?),
    new CategoryModel('娑堢値鎶楃敓绱?, '#4A90E2', '馃拪'),
    new CategoryModel('鑲犺儍鎶ょ悊', '#5AC8FA', '馃ィ'),
    new CategoryModel('澶栦激鎬ユ晳', '#FF3B30', '馃┕'),
    new CategoryModel('鎱㈡€х梾鑽?, '#5856D6', '馃挀'),
    new CategoryModel('淇濆仴缁寸敓绱?, '#34C759', '馃')
  ];

  // 瀹為檯娓叉煋鐨勫垎绫诲垪琛?(鍖呭惈鐢ㄦ埛鑷畾涔夌殑)
  @State categories: Array<CategoryModel> = [];

  // 鍒濆鍖栧脊绐楁帶鍒跺櫒
  dialogController: CustomDialogController = new CustomDialogController({
    builder: AddCategoryDialog({
      confirm: (name: string) => this.addCategory(name)
    }),
    alignment: DialogAlignment.Center,
    autoCancel: true
  });

  
  async aboutToAppear(): Promise<void> {
    this.currentUserId = await UserPreferences.getUserId();
    // 灏濊瘯鍒濆鍖?RDB 鏁版嵁搴撹繛鎺?    try {
      let context = getContext(this) as common.UIAbilityContext;
      await RdbStore.getRdbStore(context);
    } catch (e) {
      console.error('RdbStore init failed: ' + JSON.stringify(e));
    }

    // 骞惰鍔犺浇鍒嗙被鍜屽垪琛ㄦ暟鎹?    await this.initCategories();
    this.refreshList();
  }

  
  onPageShow() {
    this.refreshList();
  }

  
  async initCategories(): Promise<void> {
    let savedStr = await UserPreferences.getCategories();
    if (savedStr && savedStr !== '') {
      // 瑙ｆ瀽 JSON 瀛楃涓蹭负瀵硅薄鏁扮粍
      let savedList = JSON.parse(savedStr) as Array<CategoryModel>;
      // 閲嶆柊瀹炰緥鍖栧璞?      this.categories = savedList.map((item: CategoryModel) => new CategoryModel(item.name, item.color, item.icon));
    } else {
      this.categories = [...this.defaultCategories];
    }
  }

  
  async addCategory(name: string): Promise<void> {
    // 闅忔満鍒嗛厤棰滆壊鍜屽浘鏍囷紙绠€鍖栭€昏緫锛?    let newCat = new CategoryModel(name, '#9C27B0', '鉁?);
    this.categories.push(newCat);

    // 鎸佷箙鍖栦繚瀛樺埌棣栭€夐」
    await UserPreferences.saveCategories(JSON.stringify(this.categories));

    try { promptAction.showToast({ message: '鍒嗙被娣诲姞鎴愬姛' }); } catch(e){}
  }

  
  async refreshList(): Promise<void> {
    let list: Array<Contact> = [];
    // 1. 鍒ゆ柇鏄惁涓烘悳绱㈡ā寮?    if (this.searchText === '') {
      list = await RdbStore.queryAll(this.currentUserId);
    } else {
      list = await RdbStore.search(this.searchText, this.currentUserId);
    }
    this.contacts = list;

    // 2. 鏅鸿兘璁＄畻涓存湡鏁伴噺 (绛涢€夊寘鍚?2024 鎴?2025 骞翠唤鐨勬棩鏈?
    this.nearExpiryCount = list.filter(item => item.phone.includes('2024') || item.phone.includes('2025')).length;
  }

  
  build() {
    Navigation() {
      Column() {
        // --- 椤堕儴鎼滅储鏍?---
        Search({ value: this.searchText, placeholder: '馃攳 鎼滆嵂鍝?/ 鏌ュ姛鏁?..' })
          .width('92%')
          .height(40)
          .backgroundColor('#F0F2F5')
          .borderRadius(20)
          .margin({ top: 10, bottom: 10 })
          .padding({ left: 15 })
          .onClick(() => {
            // 鐐瑰嚮鐩存帴璺宠浆鍒板叏閲忓垪琛ㄩ〉
            router.pushUrl({ url: 'pages/AllMedicinesPage' });
          })

        Scroll() {
          Column() {
            // --- 缁熻澶у崱鐗囧尯鍩?---
            Column() {
              // 鍗＄墖澶撮儴
              Row() {
                Column() {
                  Text('瀹跺涵鑽 路 瀹堟姢鍋ュ悍')
                    .fontSize(18)
                    .fontWeight(FontWeight.Bold)
                    .fontColor(Color.White)
                    .margin({bottom:4})
                  Text('瀹氭湡娓呯悊杩囨湡鑽紝瀹夊叏甯稿')
                    .fontSize(12)
                    .fontColor('rgba(255,255,255,0.8)')
                }.alignItems(HorizontalAlign.Start)

                // 瑁呴グ鍥炬爣
                Stack({ alignContent: Alignment.Center }) {
                  Circle({ width: 40, height: 40 }).fill(Color.White)
                  Text('鉁?).fontSize(24).fontColor('#4A90E2').fontWeight(FontWeight.Bold)
                }
              }
              .width('100%')
              .justifyContent(FlexAlign.SpaceBetween)
              .padding(20)

              // 鍗＄墖鏁版嵁缁熻鍖?              Row() {
                // 宸︿晶锛氫复鏈熼璀﹀叆鍙?                Column() {
                  Text(this.nearExpiryCount.toString())
                    .fontSize(32)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#FFD600')
                  Text('涓存湡/杩囨湡 >')
                    .fontSize(12)
                    .fontColor(Color.White)
                    .opacity(0.8)
                }
                .onClick(() => router.pushUrl({ url: 'pages/ExpiryWarningPage' }))

                // 涓棿鍒嗗壊绾?                Divider()
                  .vertical(true)
                  .height(30)
                  .color('rgba(255,255,255,0.3)')

                // 鍙充晶锛氬簱瀛樻€昏鍏ュ彛
                Column() {
                  Text(this.contacts.length.toString())
                    .fontSize(32)
                    .fontWeight(FontWeight.Bold)
                    .fontColor(Color.White)
                  Text('搴撳瓨鎬绘暟 >')
                    .fontSize(12)
                    .fontColor(Color.White)
                    .opacity(0.8)
                }
                .onClick(() => router.pushUrl({ url: 'pages/AllMedicinesPage' }))
              }
              .width('100%')
              .justifyContent(FlexAlign.SpaceEvenly)
              .margin({top: 10})
            }
            .width('92%')
            .height(160)
            .borderRadius(20)
            .linearGradient({ angle: 135, colors: [[ '#4A90E2', 0.0 ], [ '#8E2DE2', 1.0 ]] }) // 娓愬彉鑳屾櫙
            .margin({ bottom: 20 })
            .shadow({ radius: 10, color: 'rgba(74, 144, 226, 0.3)' })

            // --- 鍒嗙被鍖哄煙 ---
            Row() {
              Text('鎸夊垎绫绘煡鐪?)
                .fontSize(18)
                .fontWeight(FontWeight.Bold)
                .layoutWeight(1)

              // 娣诲姞鍒嗙被鎸夐挳
              Row() {
                Text('鉃?娣诲姞鍒嗙被').fontSize(14).fontColor('#4A90E2')
              }
              .padding({top: 5, bottom: 5, left: 10, right: 10})
              .backgroundColor('#E3F2FD')
              .borderRadius(15)
              .onClick(() => {
                this.dialogController.open();
              })
            }
            .width('92%')
            .margin({ bottom: 15 })
            .alignItems(VerticalAlign.Center)

            // 鍒嗙被缃戞牸甯冨眬
            Grid() {
              ForEach(this.categories, (item: CategoryModel) => {
                GridItem() {
                  Column() {
                    Stack({ alignContent: Alignment.Center }) {
                      Circle({ width: 50, height: 50 }).fill(item.color).opacity(0.1)
                      Text(item.icon).fontSize(24)
                    }
                    .margin({ bottom: 8 })

                    Text(item.name)
                      .fontSize(14)
                      .fontColor('#333333')
                      .fontWeight(FontWeight.Medium)
                  }
                  .width('100%')
                  .height(100)
                  .backgroundColor(Color.White)
                  .borderRadius(12)
                  .justifyContent(FlexAlign.Center)
                  .onClick(() => {
                    // 璺宠浆鍒板垎绫昏鎯呴〉
                    router.pushUrl({
                      url: 'pages/CategoryListPage',
                      params: { category: item.name }
                    });
                  })
                }
              })
            }
            .columnsTemplate('1fr 1fr') // 涓ゅ垪甯冨眬
            .rowsGap(15)
            .columnsGap(15)
            .width('92%')
            .height(350)
          }
          .width('100%')
        }
        .layoutWeight(1)

        // --- 搴曢儴瀵艰埅鏍?(Dock) ---
        Row() {
          // 鍙戠幇椤靛叆鍙?          Column() {
            Text('馃挕').fontSize(24)
            Text('鍙戠幇').fontSize(12).fontColor('#666666')
          }
          .onClick(() => router.pushUrl({ url: 'pages/ProfilePage' }))

          // 蹇€熸坊鍔犳寜閽?(鎮诞)
          Button('+')
            .width(60)
            .height(60)
            .fontSize(32)
            .backgroundColor('#4A90E2')
            .onClick(() => router.pushUrl({ url: 'pages/ContactAddPage' }))
            .offset({ y: -25 })
            .shadow({ radius: 15, color: '#4A90E2' })

          // 璁剧疆椤靛叆鍙?(澶嶇敤淇敼瀵嗙爜椤?
          Column() {
            Text('鈿欙笍').fontSize(24)
            Text('璁剧疆').fontSize(12).fontColor('#666666')
          }
          .onClick(() => router.pushUrl({ url: 'pages/ChangePasswordPage' }))
        }
        .width('100%')
        .height(60)
        .backgroundColor(Color.White)
        .justifyContent(FlexAlign.SpaceAround)
        .alignItems(VerticalAlign.Center)
        .shadow({ radius: 20, color: 'rgba(0,0,0,0.1)' })
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F7F8FA')
    }
    .hideTitleBar(true)
  }
}





