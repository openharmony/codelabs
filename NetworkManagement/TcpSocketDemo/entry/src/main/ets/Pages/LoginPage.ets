
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import wifi from '@ohos.wifi';
import router from '@ohos.router';
import prompt from '@system.prompt';
import SocketUtil from '../Util/SocketUtil';

'../../ets/Pages/ChatPage'
@Entry
@Component
struct LoginPage {
  @State userName: string = '请选择用户名'
  @State oppositeIp: string = ''
  @State localIp : string =''

  private socketUtil: SocketUtil = new SocketUtil(wifi.getIpInfo().ipAddress)

  onPageShow(){
    this.localIp = this.socketUtil.resolveIP()
  }

  checkInput() : any{
    if (this.oppositeIp === '') {
      this.showMessage('对端ip不能为空')
      return false
    }
    if (this.localIp === '' || this.localIp === '0.0.0.0') {
      this.showMessage('请检查网络环境')
      return false
    }
    return true
  }

  showMessage(msg: string) {
    prompt.showToast({
      message: msg
    })
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Row() {
        Text('本地ip')
          .fontSize(35)
          .width('25%')
          .fontWeight(FontWeight.Bold)
        Text(this.localIp)
          .fontSize(35)
      }.width('80%')

      Row() {
        Text('对端ip')
          .fontSize(35)
          .width('25%')
          .fontWeight(FontWeight.Bold)
        TextInput({ placeholder: '请输入对端ip' })
          .height(75)
          .fontSize(35)
          .maxLength(30)
          .layoutWeight(1)
          .placeholderFont({ size: 35 })
          .type(InputType.Number)
          .onChange((value: string) => {
            this.oppositeIp = value
          })
      }.height(75)
      .margin({ top: 45 })
      .width('80%')

      Text('确定')
        .height(75)
        .textAlign(TextAlign.Center)
        .fontColor(Color.White)
        .borderRadius(20)
        .fontSize(35)
        .width('80%')
        .margin({ top: 45 })
        .backgroundColor(0x1890ff)
        .onClick(() => {
          if (!this.checkInput()) {
            return
          }
          router.push({
            url: 'Pages/ChatPage',
            params: { localIp: this.localIp, oppositeIp: this.oppositeIp}
          })
        })
    }
    .width('100%')
    .height('100%')
  }
}