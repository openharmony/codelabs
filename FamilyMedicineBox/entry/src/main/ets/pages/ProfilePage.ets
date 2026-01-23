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

  // 閫€鍑虹櫥褰曢€昏緫
  async logout() {
    // 1. 娓呴櫎鐢ㄦ埛鐧诲綍鎬?(璁句负0)
    await UserPreferences.putUserId(0);

    try { promptAction.showToast({ message: '宸插畨鍏ㄩ€€鍑? }); } catch(e){}

    // 2. 娓呯┖璺敱鏍堝苟璺宠浆鍥炵櫥褰曢〉 (杩欐牱鎸夎繑鍥為敭鍥炰笉鏉?
    router.clear();
    router.replaceUrl({ url: 'pages/LoginPage' });
  }

  build() {
    Navigation() {
      Column() {
        // 1. 椤堕儴鏍囬
        Text('鎴戠殑鐢ㄨ嵂鎻愰啋鍗?)
          .fontSize(20).fontWeight(FontWeight.Bold).margin({ top: 20, bottom: 20 })

        // 2. Canvas 缁樺浘 (淇濈暀浣犵殑鍔犲垎椤?
        Canvas(this.context)
          .width('90%').height(220).backgroundColor('#FFFFFF')
          .borderRadius(12).shadow({ radius: 10, color: 'rgba(0,0,0,0.1)' })
          .onReady(() => {
            this.context.fillStyle = '#E3F2FD';
            this.context.fillRect(0, 0, 350, 220);
            this.context.fillStyle = '#1565C0';
            this.context.font = 'bold 24px sans-serif';
            this.context.fillText('瀹跺涵鐢ㄨ嵂灏忚创澹?, 20, 40);
            this.context.beginPath();
            this.context.moveTo(20, 55);
            this.context.lineTo(330, 55);
            this.context.strokeStyle = '#1565C0';
            this.context.stroke();
            this.context.fillStyle = '#333333';
            this.context.font = '20px sans-serif';
            this.context.fillText('娓╅Θ鎻愮ず锛?, 20, 90);
            this.context.font = '16px sans-serif';
            this.context.fillText('1. 璇峰畾鏈熸竻鐞嗚繃鏈熻嵂鍝?, 20, 130);
            this.context.fillText('2. 鏈嶈嵂鍓嶈鐪嬫竻鐢ㄦ硶鐢ㄩ噺', 20, 160);
            this.context.fillText('3. 鑽搧璇峰瓨鏀惧湪鍎跨鎷夸笉鍒扮殑鍦版柟', 20, 190);
          })

        // 3. 閫€鍑虹櫥褰曟寜閽?(鏂板鍔熻兘)
        Button('閫€鍑虹櫥褰?)
          .width('90%').height(50).margin({ top: 50 })
          .backgroundColor('#FF3B30') // 绾㈣壊
          .borderRadius(25)
          .onClick(() => {
            // 寮瑰嚭纭妗?            AlertDialog.show({
              title: '鎻愮ず',
              message: '纭畾瑕侀€€鍑虹櫥褰曞悧锛?,
              primaryButton: { value: '鍙栨秷', action: () => {} },
              secondaryButton: {
                value: '閫€鍑?,
                fontColor: '#FF3B30',
                action: () => { this.logout(); }
              }
            })
          })
      }
      .width('100%').height('100%').backgroundColor('#F7F8FA')
    }
    .title('鍙戠幇').titleMode(NavigationTitleMode.Mini)
  }
}




