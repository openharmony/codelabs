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
import { resolveIP } from '../Utils/SocketUtil'

let localIp = resolveIP(wifi.getIpInfo().ipAddress)
let oppositeIp = ""

@Entry
@Component
struct Login {

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Row() {
        Text('本地IP').fontSize(35).width('25%').fontWeight(FontWeight.Bold)

        Text(localIp)
          .fontSize(35)
      }.width('80%')

      Row() {
        Text('对端IP').fontSize(35).width('25%').fontWeight(FontWeight.Bold)

        TextInput({ placeholder: '请输入对端IP' })
          .height(75)
          .fontSize(35)
          .maxLength(30)
          .layoutWeight(1)
          .placeholderFont({ size: 35 })
          .type(InputType.Number)
          .onChange((value: string) => {
            oppositeIp = value
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
        .backgroundColor('#1890ff')
        .onClick(() => {
          if (oppositeIp === '') {
            prompt.showToast({
              message: '对端ip不能为空'
            })
            return
          }

          if (localIp === '' || localIp === '0.0.0.0') {
            prompt.showToast({
              message: '请检查网络环境'
            })
            return
          }

          router.push({
            url: 'pages/ChatPage',
            params: { localIp: localIp, oppositeIp: oppositeIp}
          })
        })
    }
    .width('100%')
    .height('100%')
  }
}