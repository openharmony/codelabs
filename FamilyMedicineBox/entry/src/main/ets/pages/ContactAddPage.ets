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

  @State phone: string = ''; // 瀛樺偍鏃ユ湡瀛楃涓?YYYY-MM-DD
  @State company: string = ''; // 鍒嗙被
  @State email: string = '';   // 浣嶇疆

  // 鐢ㄦ硶鐢ㄩ噺鐩稿叧鐨勭姸鎬?  @State usageFrequency: string = '涓€鏃ヤ笁娆?; // 棰戠巼
  @State isBeforeMeal: boolean = false;      // 楗墠
  @State isAfterMeal: boolean = true;        // 楗悗 (榛樿鍕鹃€?

  // 鏃ユ湡閫夋嫨鍣ㄧ殑鏃堕棿
  private selectedDate: Date = new Date();

  // 鍒嗙被涓嬫媺閫夐」
  private categories: SelectOption[] = [
    { value: '鎰熷啋鍙戠儳' },
    { value: '娑堢値鎶楃敓绱? },
    { value: '鑲犺儍鎶ょ悊' },
    { value: '澶栦激鎬ユ晳' },
    { value: '鎱㈡€х梾鑽? },
    { value: '淇濆仴缁寸敓绱? }
  ];

  // 棰戠巼涓嬫媺閫夐」
  private frequencyOptions: SelectOption[] = [
    { value: '涓€鏃ヤ竴娆? },
    { value: '涓€鏃ヤ袱娆? },
    { value: '涓€鏃ヤ笁娆? },
    { value: '蹇呰鏃舵湇鐢? },
    { value: '姣?灏忔椂涓€娆? }
  ];

  // 鎻愪氦閫昏緫
  async submit() {
    if (this.name === '') {
      promptAction.showToast({ message: '璇疯緭鍏ヨ嵂鍝佸悕绉? });
      return;
    }
    if (this.phone === '') {
      promptAction.showToast({ message: '璇烽€夋嫨杩囨湡鏃ユ湡' });
      return;
    }

    // 1. 鎷兼帴鐢ㄦ硶瀛楃涓?(鍕鹃€?-> 鏂囨湰)
    let usageStr = this.usageFrequency;
    if (this.isBeforeMeal) usageStr += ' (楗墠)';
    if (this.isAfterMeal) usageStr += ' (楗悗)';

    let userId: number = await UserPreferences.getUserId();

    // 2. 鍒涘缓瀵硅薄
    let newContact = new Contact(this.name, this.phone);
    newContact.company = this.company || '鍏朵粬鍒嗙被';
    newContact.position = usageStr; // 鎶婃嫾鎺ュソ鐨勭敤娉曞瓨杩涘幓
    newContact.email = this.email || '鏈垎绫讳綅缃?;
    newContact.userId = userId;

    // 3. 鍐欏叆鏁版嵁搴?(蹇呴』鍔?await 纭繚鍐欏畬鍐嶈繑鍥?
    await RdbStore.insert(newContact);

    promptAction.showToast({ message: '鉁?鑽搧鍏ュ簱鎴愬姛' });

    // 4. 杩斿洖涓婁竴椤?    router.back();
  }

  // 杈呭姪鍑芥暟锛氭牸寮忓寲鏃ユ湡
  formatDate(date: Date): string {
    const y = date.getFullYear();
    const m = date.getMonth() + 1;
    const d = date.getDate();
    return `${y}-${m < 10 ? '0' + m : m}-${d < 10 ? '0' + d : d}`;
  }

  build() {
    Navigation() {
      Column() {
        List() {
          // 1. 鑽搧鍚嶇О (鏂囨湰杈撳叆)
          ListItem() {
            Column() {
              Text('鑽搧鍚嶇О').fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 8 })
              TextInput({ placeholder: '渚嬪锛?99鎰熷啋鐏? })
                .height(45)
                .width('100%')
                .backgroundColor('#F5F5F5')
                .borderRadius(8)
                .onChange((value: string) => this.name = value)
            }.padding(15)
          }

          // 2. 鍔熻兘鍒嗙被 (涓嬫媺鑿滃崟)
          ListItem() {
            Column() {
              Text('鍔熻兘鍒嗙被').fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 8 })
              Select(this.categories)
                .selected(0)
                .value(this.company || '璇烽€夋嫨鍒嗙被')
                .font({ size: 16 })
                .fontColor('#333333')
                .selectedOptionFont({ size: 16 })
                .optionFont({ size: 16 })
                .height(45)
                .width('100%')
                .backgroundColor('#F5F5F5')
                .borderRadius(8)
                .onSelect((index: number, value: string) => {
                  this.company = value;
                })
            }.padding(15)
          }

          // 3. 鏈夋晥鏈?(鏃ユ湡閫夋嫨鍣?- 婊¤冻浣犵殑瑕佹眰1)
          ListItem() {
            Column() {
              Text('鏈夋晥鏈熻嚦').fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 8 })

              // 鐐瑰嚮杩欎釜Row瑙﹀彂鏃ユ湡寮圭獥
              Row() {
                Text(this.phone === '' ? '馃搮 鐐瑰嚮閫夋嫨鏃ユ湡' : this.phone)
                  .fontSize(16)
                  .fontColor(this.phone === '' ? '#999999' : '#333333')
                Blank()
                Text(' > ').fontColor('#CCCCCC')
              }
              .width('100%')
              .height(45)
              .backgroundColor('#F5F5F5')
              .borderRadius(8)
              .padding({ left: 15, right: 15 })
              .onClick(() => {
                DatePickerDialog.show({
                  start: new Date("2020-1-1"),
                  end: new Date("2030-12-31"),
                  selected: this.selectedDate,
                  onAccept: (value: DatePickerResult) => {
                    // 閫氳繃 value.year, value.month, value.day 閲嶆柊鏋勫缓Date瀵硅薄
                    // 娉ㄦ剰锛欰PI9涓?value.month 鏄?0-11
                    const newDate = new Date(value.year!, value.month!, value.day!);
                    this.selectedDate = newDate;
                    this.phone = this.formatDate(newDate); // 鏇存柊鏄剧ず鏂囨湰
                  }
                })
              })
            }.padding(15)
          }

          // 4. 鐢ㄦ硶鐢ㄩ噺 (鍕鹃€?+ 涓嬫媺 - 婊¤冻浣犵殑瑕佹眰1)
          ListItem() {
            Column() {
              Text('鐢ㄦ硶鐢ㄩ噺').fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 8 })

              // 棰戠巼閫夋嫨
              Select(this.frequencyOptions)
                .value(this.usageFrequency)
                .font({ size: 16 })
                .height(40)
                .width('100%')
                .backgroundColor('#F5F5F5')
                .borderRadius(8)
                .margin({ bottom: 10 })
                .onSelect((index: number, value: string) => {
                  this.usageFrequency = value;
                })

              // 鍕鹃€夐キ鍓?楗悗
              Row() {
                // 楗墠 Checkbox
                Row() {
                  Checkbox({ name: 'before', group: 'meal' })
                    .select(this.isBeforeMeal)
                    .onChange((value: boolean) => this.isBeforeMeal = value)
                  Text('楗墠鏈嶇敤').fontSize(16).fontColor('#333333')
                }.margin({ right: 20 })

                // 楗悗 Checkbox
                Row() {
                  Checkbox({ name: 'after', group: 'meal' })
                    .select(this.isAfterMeal)
                    .onChange((value: boolean) => this.isAfterMeal = value)
                  Text('楗悗鏈嶇敤').fontSize(16).fontColor('#333333')
                }
              }
              .width('100%')
            }.padding(15)
          }

          ListItem() {
            Column() {
              Text('瀛樻斁浣嶇疆').fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 8 })
              TextInput({ placeholder: '渚嬪锛氬鍘呯數瑙嗘煖' })
                .height(45)
                .width('100%')
                .backgroundColor('#F5F5F5')
                .borderRadius(8)
                .onChange((value: string) => this.email = value)
            }.padding(15)
          }
        }
        .width('95%')
        .backgroundColor(Color.White)
        .borderRadius(16)
        .margin({ top: 15 })
        .shadow({ radius: 10, color: 'rgba(0,0,0,0.05)' })

        // 5. 搴曢儴澶ф寜閽?        Button('纭鍏ュ簱')
          .width('90%')
          .height(50)
          .margin({ top: 30 })
          .backgroundColor('#4A90E2')
          .borderRadius(25)
          .shadow({ radius: 10, color: '#4A90E2' })
          .onClick(() => {
            this.submit();
          })
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F7F8FA')
    }
    .title('娣诲姞鑽搧')
    .titleMode(NavigationTitleMode.Mini)
  }
}





