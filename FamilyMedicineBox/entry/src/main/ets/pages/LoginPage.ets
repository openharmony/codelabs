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

 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

  async aboutToAppear(): Promise<void> {
    try {
      let context = getContext(this) as common.UIAbilityContext;
      await UserPreferences.init(context);
    } catch(e) {}
  }

  async login(): Promise<void> {
    let realAccount = await UserPreferences.getAccount();
    let realPassword = await UserPreferences.getPassword();

    if (this.account === realAccount && this.password === realPassword) {
      await UserPreferences.putUserId(1);
      try { promptAction.showToast({ message: '鐧诲綍鎴愬姛' }); } catch (e) {}
      // 鐧诲綍鎴愬姛璺宠浆棣栭〉
      router.replaceUrl({ url: 'pages/Index' });
    } else {
      try { promptAction.showToast({ message: '璐﹀彿鎴栧瘑鐮侀敊璇? }); } catch (e) {}
    }
  }

  build() {
    Column() {
      // 椤堕儴鑳屾櫙
      Stack({ alignContent: Alignment.Bottom }) {
        Column().width('100%').height('100%')
          .backgroundColor('#4A90E2')
          .borderRadius({ bottomLeft: 40, bottomRight: 40 })

        Column() {
          Stack({ alignContent: Alignment.Center }) {
            Circle({ width: 80, height: 80 }).fill(Color.White)
            Text('鉁?).fontSize(50).fontWeight(FontWeight.Bold).fontColor('#4A90E2')
          }.margin({ bottom: 15 })

          Text('瀹跺涵灏忚嵂绠卞姪鎵?).fontSize(28).fontWeight(FontWeight.Bold).fontColor(Color.White)
          Text('Family Medicine Box').fontSize(16).fontColor('#E3F2FD').margin({ top: 5 })
        }.margin({ bottom: 50 })
      }.width('100%').height('35%')

      // 杈撳叆妗?      Column() {
        TextInput({ placeholder: '璇疯緭鍏ヨ处鍙? })
          .height(50).backgroundColor('#F5F5F5').borderRadius(25)
          .padding({ left: 20 }).margin({ bottom: 15 })
          .onChange((value: string) => this.account = value)

        TextInput({ placeholder: '璇疯緭鍏ュ瘑鐮? })
          .type(InputType.Password)
          .height(50).backgroundColor('#F5F5F5').borderRadius(25)
          .padding({ left: 20 }).margin({ bottom: 30 })
          .onChange((value: string) => this.password = value)

        Button('瀹夊叏鐧诲綍')
          .width('100%').height(50).backgroundColor('#4A90E2').borderRadius(25).fontSize(18)
          .onClick(() => this.login())

        // 搴曢儴鏂囧瓧閾炬帴
        Row() {
          // 銆愯繖閲屾槸娉ㄥ唽鎸夐挳锛屽繀椤荤‘淇?router.pushUrl 鐨勮矾寰勫湪 main_pages.json 閲屻€?          Text('娉ㄥ唽鏂拌处鍙?)
            .fontSize(14).fontColor('#4A90E2')
            .onClick(() => {
              router.pushUrl({ url: 'pages/RegisterPage' });
            })

          Text(' | ').fontColor('#999999')

          Text('蹇樿瀵嗙爜')
            .fontSize(14).fontColor('#999999')
            .onClick(() => {
              AlertDialog.show({
                title: '鎻愮ず',
                message: '璇疯仈绯荤鐞嗗憳閲嶇疆瀵嗙爜銆俓n鍒濆瀵嗙爜涓猴細123456',
                confirm: { value: '鐭ラ亾浜?, action: () => {} }
              })
            })
        }.margin({ top: 20 })
      }.width('90%').margin({ top: 40 })
    }
    .width('100%').height('100%').backgroundColor(Color.White)
  }
}


