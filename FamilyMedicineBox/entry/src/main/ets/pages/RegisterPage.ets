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

  @State confirmPassword: string = '';

  async aboutToAppear() {
    try {
      let context = getContext(this) as common.UIAbilityContext;
      await UserPreferences.init(context);
    } catch(e) {}
  }

  async register() {
    if (this.account === '' || this.password === '') {
      promptAction.showToast({ message: '璐﹀彿鎴栧瘑鐮佷笉鑳戒负绌? });
      return;
    }
    if (this.password !== this.confirmPassword) {
      promptAction.showToast({ message: '涓ゆ杈撳叆鐨勫瘑鐮佷笉涓€鑷? });
      return;
    }

    // 淇濆瓨鏂拌处鍙峰瘑鐮?    await UserPreferences.saveAccount(this.account, this.password);

    promptAction.showToast({ message: '娉ㄥ唽鎴愬姛锛岃鐧诲綍' });
    router.back(); // 杩斿洖鐧诲綍椤?  }

  build() {
    Column() {
      Text('娉ㄥ唽鏂拌处鍙?).fontSize(24).fontWeight(FontWeight.Bold).margin({ top: 50, bottom: 30 })

      TextInput({ placeholder: '璇疯緭鍏ョ敤鎴峰悕' })
        .height(50).width('90%').backgroundColor('#F5F5F5').borderRadius(25)
        .padding({ left: 20 }).margin({ bottom: 15 })
        .onChange((val) => this.account = val)

      TextInput({ placeholder: '璇疯緭鍏ュ瘑鐮? })
        .type(InputType.Password)
        .height(50).width('90%').backgroundColor('#F5F5F5').borderRadius(25)
        .padding({ left: 20 }).margin({ bottom: 15 })
        .onChange((val) => this.password = val)

      TextInput({ placeholder: '璇风‘璁ゅ瘑鐮? })
        .type(InputType.Password)
        .height(50).width('90%').backgroundColor('#F5F5F5').borderRadius(25)
        .padding({ left: 20 }).margin({ bottom: 30 })
        .onChange((val) => this.confirmPassword = val)

      Button('绔嬪嵆娉ㄥ唽')
        .width('90%').height(50).backgroundColor('#4A90E2').borderRadius(25).fontSize(18)
        .onClick(() => this.register())

      Text('宸叉湁璐﹀彿锛熷幓鐧诲綍')
        .fontColor('#4A90E2').margin({ top: 20 })
        .onClick(() => router.back())
    }
    .width('100%').height('100%').backgroundColor(Color.White)
  }
}




