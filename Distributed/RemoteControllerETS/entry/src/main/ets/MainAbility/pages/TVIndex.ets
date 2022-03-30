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

import router from '@system.router';
import { PicData } from '../model/PicData'
import { initializeOnStartup } from '../model/PicDataModel'
import featureAbility from '@ohos.ability.featureAbility';
import prompt from '@system.prompt';
import rpc from "@ohos.rpc";
import commonEvent from '@ohos.commonEvent';
import RemoteDeviceModel from '../model/RemoteDeviceModel.ets';
import ConnectModel from '../model/ConnectModel.ets';

var localDeviceId = "";
var remoteDeviceModel = new RemoteDeviceModel;
var connectModel = new ConnectModel;
let discoveredDeviceIdList = [];
let DeviceIdList = [];

// 发现设备列表
function RegisterDeviceListCallback() {
  remoteDeviceModel.registerDeviceListCallback(() => {
    let discoveredDeviceSize = remoteDeviceModel.discoverList.length;
    let deviceSize = remoteDeviceModel.deviceList.length;
    localDeviceId = remoteDeviceModel.localDeviceId;

    discoveredDeviceIdList.length = 0;
    for (var i = 0; i < discoveredDeviceSize; i++) {
      discoveredDeviceIdList.push(remoteDeviceModel.discoverList[i].deviceId);
    }
    DeviceIdList.length = 0;
    for (var i = 0; i < deviceSize; i++) {
      DeviceIdList.push(remoteDeviceModel.deviceList[i].deviceId);
    }

    if (deviceSize === 0) {
      if (discoveredDeviceSize > 0) {
        prompt.showToast({
          message: "RegisterDeviceListCallback dicovered " + discoveredDeviceSize + " devices, need to authenticate"
        });
      } else {
        prompt.showToast({
          message: "RegisterDeviceListCallback no device discovered"
        });
      }
    }
  });
}
// 设备认证
function AuthDevice(deviceId) {
  if (remoteDeviceModel.deviceList.length >= 1 && remoteDeviceModel.discoverList.length == 0) {

    return;
  }
  if (remoteDeviceModel.discoverList.length > 0) {
    remoteDeviceModel.authDevice(deviceId, () => {
      prompt.showToast({
        message: "AuthDevice device authenticate success"
      });
    });
  } else {
    prompt.showToast({
      message: "AuthDevice no device discovered"
    });
  }
}

// 拉起远端FA
function onStartRemoteAbility(deviceId) {
  AuthDevice(deviceId);
  let numDevices = remoteDeviceModel.deviceList.length;
  if (numDevices === 0) {
    prompt.showToast({
      message: "onStartRemoteAbility no device found"
    });
    return;
  }

  var params = {
    remoteDeviceId: localDeviceId
  }

  var wantValue = {
    bundleName: 'com.example.helloworld0218',
    abilityName: 'com.example.helloworld0218.PhoneAbility',
    deviceId: deviceId,
    parameters: params
  };

  featureAbility.startAbility({
    want: wantValue
  }).then((data) => {
    // 拉起远端后，连接远端service
    connectModel.onConnectRemoteService(deviceId)
  });
}

// 设备列表弹出框
@CustomDialog
struct CustomDialogExample {
  @State editFlag: boolean = false
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  build() {
    Column() {
      List({ space: 10, initialIndex: 0 }) {
        ForEach(DeviceIdList, (item) => {
          ListItem() {
            Row() {
              Text(item)
                .width('87%')
                .height(50)
                .fontSize(10)
                .textAlign(TextAlign.Center)
                .borderRadius(10)
                .backgroundColor(0xFFFFFF)
                .onClick(() => {
                  onStartRemoteAbility(item);
                  this.controller.close();
                })
            }
          }.editable(this.editFlag)
        }, item => item)
      }
    }.width('100%').height(200).backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}

@Component
struct PicGridItem {
  private picItem: PicData
  build() {
    Column() {
      Image(this.picItem.image)
        .objectFit(ImageFit.Contain)
        .height('85%')
        .width('100%')
        .onClick(() => {
          router.push({
            uri: 'pages/VideoPlay',
            params: { source: this.picItem.video }
          })
        })
      Text(this.picItem.name)
        .fontSize(20)
        .fontColor('#000000')
    }
    .height('100%')
    .width('90%')
  }
}

@Entry
@Component
struct Index {
  // 新建弹框界面
  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExample({ cancel: this.onCancel, confirm: this.onAccept }),
    cancel: this.existApp,
    autoCancel: true
  })

  onCancel() {
  }

  onAccept() {
  }

  existApp() {
  }

  private picItems: PicData[] = initializeOnStartup()
  private letters: string[] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
  private source: string
  @State text: string = ''
  @State choose: number = -1

  build() {
    Flex({ direction: FlexDirection.Column }) {
      TextInput({text: this.text, placeholder: 'Search' })
        .width('80%')
        .height(50)
        .onChange((value: string) => {
          this.text = value
        })
        .margin(5)

      Row({space: 30}) {
        Text('Clear')
          .fontSize(16)
          .backgroundColor('#ABB0BA')
          .width('30%')
          .height(50)
          .textAlign(TextAlign.Center)
          .onClick(() => {
            this.text = ''
          })
          .clip(true)
          .borderRadius(10)

        Text('Backspace')
          .fontSize(16)
          .backgroundColor('#ABB0BA')
          .width('30%')
          .height(50)
          .textAlign(TextAlign.Center)
          .onClick(() => {
            this.text = this.text.substring(0, this.text.length - 1)
          })
          .clip(true)
          .borderRadius(10)

        Text('Controller')
          .fontSize(16)
          .backgroundColor('#ABB0BA')
          .width('30%')
          .height(50)
          .textAlign(TextAlign.Center)
          .onClick(() => {
            RegisterDeviceListCallback();
            this.dialogController.open();
          })
          .clip(true)
          .borderRadius(10)

      }
      .padding({ left:10 })
      .margin(5)

      Grid() {
        ForEach(this.letters, (item) => {
          GridItem() {
            Text(item)
              .fontSize(20)
              .backgroundColor('#FFFFFF')
              .width('100%')
              .height('80%')
              .textAlign(TextAlign.Center)
              .onClick(() => {
                this.text += item
                })
              .clip(true)
              .borderRadius(5)
          }.margin(2)
        }, item => item)

      }
      .rowsTemplate('1fr 1fr 1fr 1fr')
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr')
      .columnsGap(8)
      .rowsGap(8)
      .width('75%')
      .height('25%')
      .margin(5)
      .backgroundColor('#D2D3D8')
      .clip(true)
      .borderRadius(10)

      Grid() {
        ForEach(this.picItems, (item: PicData) => {
          GridItem() {
            PicGridItem({ picItem: item })
          }.backgroundColor(item.id == this.choose ? '#D2D3D8' : Color.White)
        }, (item: PicData) => item.id.toString())
      }
      .rowsTemplate('1fr 1fr 1fr')
      .columnsTemplate('1fr 1fr')
      .columnsGap(5)
      .rowsGap(8)
      .width('90%')
      .height('58%')
      .backgroundColor('#FFFFFF')
      .margin(5)
    }
    .width('98%')
    .backgroundColor('#FFFFFF')
  }

  async aboutToAppear() {
    this.subscribeEvent();
  }

  subscribeEvent() {
    let self = this;
    // 用于保存创建成功的订阅者对象，后续使用其完成订阅及退订的动作
    var subscriber;
    // 订阅者信息
    var subscribeInfo = {
      events: ["publish_change"],
      priority: 100
    };

    // 设置有序公共事件的结果代码回调
    function SetCodeCallBack() {
    }
    // 设置有序公共事件的结果数据回调
    function SetDataCallBack() {
    }
    // 完成本次有序公共事件处理回调
    function FinishCommonEventCallBack() {
    }
    // 订阅公共事件回调
    function SubscribeCallBack(err, data) {
      let msgData = data.data;
      let code = data.code;
      // 设置有序公共事件的结果代码
      subscriber.setCode(code, SetCodeCallBack);
      // 设置有序公共事件的结果数据
      subscriber.setData(msgData, SetDataCallBack);
      // 完成本次有序公共事件处理
      subscriber.finishCommonEvent(FinishCommonEventCallBack)
      // 处理接收到的数据data

      // code = 1时，将手机遥控端search框内数据同步到TV端
      if (code == 1) {
        self.text = data.parameters.dataList;
      }

      // code = 2时，增加选中图片效果
      if (code == 2) {
        // 如果在图片序号范围内就选中图片，否则不更改
        var tmp: number = +data.parameters.dataList;
        if ((self.choose + tmp <= 5) && (self.choose + tmp >= 0)) {
            self.choose += tmp;
        }
      }

      // code = 3时，播放选中图片对应的视频
      if (code == 3) {
        self.picItems.forEach(function (item) {
          if (item.id == self.choose) {
            router.push({
              uri: 'pages/VideoPlay',
              params: { source: item.video }
            })
          }
        })
      }

      // code = 4时，回到TV端默认页面
      if (code == 4) {
        router.push({
          uri: 'pages/TVIndex',
        })
      }

      // code = 5时，关闭程序
      if (code == 5) {
        featureAbility.terminateSelf()
      }

      // code = 6时，搜索图片名称并增加选中特效
      if (code == 6) {
        self.picItems.forEach(function (item) {
          if (item.name == self.text) {
            self.choose = Number(item.id)
          }
        })
      }
      connectModel.onDisconnectService();
    }

    //创建订阅者回调
    function CreateSubscriberCallBack(err, data) {
      subscriber = data;
      //订阅公共事件
      commonEvent.subscribe(subscriber, SubscribeCallBack);
    }

    //创建订阅者
    commonEvent.createSubscriber(subscribeInfo, CreateSubscriberCallBack);
  }
}