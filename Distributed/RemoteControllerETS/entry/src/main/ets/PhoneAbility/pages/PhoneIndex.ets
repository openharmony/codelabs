// @ts-nocheck
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

import ConnectModel from '../../MainAbility/model/ConnectModel.ets';
import featureAbility from '@ohos.ability.featureAbility';

var connectModel = new ConnectModel;

@Entry
@Component
struct Index {
  async aboutToAppear() {
    await featureAbility.getWant((error, want) => {
      // 远端被拉起后，连接对端的service
      if (want.parameters.remoteDeviceId) {
        let remoteDeviceId = want.parameters.remoteDeviceId
        connectModel.onConnectRemoteService(remoteDeviceId)
      }
    });
  }

  build(){
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      Row() {
        Image($rawfile('TV.png'))
          .width(25)
          .height(25)
        Text('华为智慧屏').fontSize(20).margin(10)
      }

      TextInput({ placeholder: 'Search' })
        .margin(20)
        .onChange((value: string) => {
          if (connectModel.mRemote){
            connectModel.sendMessageToRemoteService(JSON.stringify(value), 1);
          }
        })

      Grid() {
        GridItem() {
          Button({ type: ButtonType.Circle, stateEffect: true }) {
            Image($rawfile('up.png')).width(80).height(80)
          }
          .onClick(() => {
            connectModel.sendMessageToRemoteService(JSON.stringify('-2'), 2);
          })
          .width(80)
          .height(80)
          .backgroundColor('#FFFFFF')
        }
        .columnStart(1)
        .columnEnd(5)

        GridItem() {
          Button({ type: ButtonType.Circle, stateEffect: true }) {
            Image($rawfile('left.png')).width(80).height(80)
          }
          .onClick(() => {
            connectModel.sendMessageToRemoteService(JSON.stringify('-1'), 2);
          })
          .width(80)
          .height(80)
          .backgroundColor('#FFFFFF')
        }

        GridItem() {
          Button({ type: ButtonType.Circle, stateEffect: true }) {
            Image($rawfile('play.png')).width(60).height(60)
          }
          .onClick(() => {
            connectModel.sendMessageToRemoteService(JSON.stringify('1'), 3);
          })
          .width(80)
          .height(80)
          .backgroundColor('#FFFFFF')
        }

        GridItem() {
          Button({ type: ButtonType.Circle, stateEffect: true }) {
            Image($rawfile('right.png')).width(70).height(70)
          }
          .onClick(() => {
            connectModel.sendMessageToRemoteService(JSON.stringify('1'), 2);
          })
          .width(80)
          .height(80)
          .backgroundColor('#FFFFFF')
        }

        GridItem() {
          Button({ type: ButtonType.Circle, stateEffect: true }) {
            Image($rawfile('down.png')).width(70).height(70)
          }
          .onClick(() => {
            connectModel.sendMessageToRemoteService(JSON.stringify('2'), 2);
          })
          .width(80)
          .height(80)
          .backgroundColor('#FFFFFF')
        }
        .columnStart(1)
        .columnEnd(5)
      }
      .rowsTemplate('1fr 1fr 1fr')
      .columnsTemplate('1fr 1fr 1fr')
      .backgroundColor('#FFFFFF')
      .margin(10)
      .clip(new Circle({ width: 325, height: 325 }))
      .width(350)
      .height(350)

      Row({ space:100 }) {
        Button({ type: ButtonType.Circle, stateEffect: true }) {
          Image($rawfile('return.png')).width(40).height(40)
        }
        .onClick(() => {
          connectModel.sendMessageToRemoteService(JSON.stringify('1'), 4);
        })
        .width(100)
        .height(100)
        .backgroundColor('#FFFFFF')

        Button({ type: ButtonType.Circle, stateEffect: true }) {
          Image($rawfile('off.png')).width(40).height(40)
        }
        .onClick(() => {
          connectModel.sendMessageToRemoteService(JSON.stringify('1'), 5);
          featureAbility.terminateSelf()
        })
        .width(100)
        .height(100)
        .backgroundColor('#FFFFFF')

        Button({ type: ButtonType.Circle, stateEffect: true }) {
          Image($rawfile('search.png')).width(40).height(40)
        }
        .onClick(() => {
          connectModel.sendMessageToRemoteService(JSON.stringify('1'), 6);
        })
        .width(100)
        .height(100)
        .backgroundColor('#FFFFFF')
      }
      .padding({ left:100 })
    }
    .backgroundColor('#E3E3E3')
  }
}