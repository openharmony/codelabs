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

  async submit() {
    // 1. 鑾峰彇褰撳墠瀛樺偍鐨勭湡瀹炲瘑鐮?    let currentRealPass = await UserPreferences.getPassword();
    let currentAccount = await UserPreferences.getAccount();

    // 2. 鏍￠獙鏃у瘑鐮?    if (this.oldPass !== currentRealPass) {
      try { promptAction.showToast({ message: '鏃у瘑鐮侀敊璇? }); } catch(e){}
      return;
    }

    // 3. 鏍￠獙鏂板瘑鐮?    if (this.newPass === '') {
      try { promptAction.showToast({ message: '鏂板瘑鐮佷笉鑳戒负绌? }); } catch(e){}
      return;
    }
    if (this.newPass !== this.confirmPass) {
      try { promptAction.showToast({ message: '涓ゆ鏂板瘑鐮佽緭鍏ヤ笉涓€鑷? }); } catch(e){}
      return;
    }

    // 4. 銆愭牳蹇冧慨澶嶃€戜繚瀛樻柊瀵嗙爜鍒伴閫夐」
    await UserPreferences.saveAccount(currentAccount, this.newPass);

    try { promptAction.showToast({ message: '瀵嗙爜淇敼鎴愬姛锛岃閲嶆柊鐧诲綍' }); } catch(e){}

    // 5. 閫€鍑虹櫥褰曡烦杞?    router.replaceUrl({ url: 'pages/LoginPage' });
  }

  build() {
    Navigation() {
      Column() {
        List() {
          ListItem() {
            Column() {
              Text('鏃у瘑鐮?).fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 5 })
              TextInput({ placeholder: '璇疯緭鍏ユ棫瀵嗙爜' }).type(InputType.Password)
                .height(45).width('100%').backgroundColor(Color.White).borderRadius(8)
                .onChange((val) => this.oldPass = val)
            }.padding(10)
          }

          ListItem() {
            Column() {
              Text('鏂板瘑鐮?).fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 5 })
              TextInput({ placeholder: '璇疯緭鍏ユ柊瀵嗙爜' }).type(InputType.Password)
                .height(45).width('100%').backgroundColor(Color.White).borderRadius(8)
                .onChange((val) => this.newPass = val)
            }.padding(10)
          }

          ListItem() {
            Column() {
              Text('纭鏂板瘑鐮?).fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 5 })
              TextInput({ placeholder: '璇峰啀娆¤緭鍏ユ柊瀵嗙爜' }).type(InputType.Password)
                .height(45).width('100%').backgroundColor(Color.White).borderRadius(8)
                .onChange((val) => this.confirmPass = val)
            }.padding(10)
          }
        }
        .width('95%').margin({ top: 20 })

        Button('纭淇敼')
          .width('90%').height(50).margin({ top: 40 })
          .backgroundColor('#4A90E2').borderRadius(25)
          .onClick(() => this.submit())
      }
      .width('100%').height('100%').backgroundColor('#F7F8FA')
    }
    .title('淇敼瀵嗙爜').titleMode(NavigationTitleMode.Mini)
  }
}

