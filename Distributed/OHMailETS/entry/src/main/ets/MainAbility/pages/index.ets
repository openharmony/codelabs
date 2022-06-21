/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import featureAbility from '@ohos.ability.featureAbility';
import prompt from '@system.prompt';
import rpc from "@ohos.rpc";
import commonEvent from '@ohos.commonEvent';
import RemoteDeviceModel from '../../model/RemoteDeviceModel';
//import TextAlign from './css'


var connectedAbility;
var localDeviceId = "";
var mRemote;
var remoteDeviceModel = new RemoteDeviceModel;
let discovereDeviceIdList = [];
let DeviceIdList = [];

// 发现设备列表
function RegisterDeviceListCallback() {
  remoteDeviceModel.registerDeviceListCallback(() => {
    let discoveredDeviceSize = remoteDeviceModel.discoverList.length;
    let deviceSize = remoteDeviceModel.deviceList.length;
    localDeviceId = remoteDeviceModel.localDeviceId;

    discovereDeviceIdList.length = 0;
    for (var i = 0; i < discoveredDeviceSize; i++) {
      discovereDeviceIdList.push(remoteDeviceModel.discoverList[i].deviceId);
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
function onStartRemoteAbility(deviceId, dataList: any[]) {
  AuthDevice(deviceId);
  let numDevices = remoteDeviceModel.deviceList.length;
  if (numDevices === 0) {
    prompt.showToast({
      message: "onStartRemoteAbility no device found"
    });
    return;
  }

  var params = {
    dataList: JSON.stringify(dataList),
    remoteDeviceId: localDeviceId
  }
  var wantValue = {
    bundleName: 'com.example.ohmailets',
    abilityName: 'com.example.ohmailets.MainAbility',
    deviceId: deviceId,
    parameters: params
  };
  featureAbility.startAbility({
    want: wantValue
  }).then((data) => {

    // 拉起远端后，连接远端service
    onConnectRemoteService(deviceId)
    console.info(JSON.stringify(data))
  });
}
// 连接远端Service
async function onConnectRemoteService(deviceId) {
  // 连接成功的回调
  async function onConnectCallback(element, remote) {
    prompt.showToast({
      message: "onConnectRemoteService success" + remote,
      duration: 3000
    });
    mRemote = remote;
  }
  // Service异常死亡的回调
  function onDisconnectCallback(element) {
    console.info(element)
  }
  // 连接失败的回调
  function onFailedCallback(code) {
    prompt.showToast({
      message: "onConnectRemoteService onFailed: " + code
    });
  }

  let numDevices = remoteDeviceModel.deviceList.length;
  if (numDevices === 0) {
    prompt.showToast({
      message: "onConnectRemoteService no device found"
    });
    return;
  }
  connectedAbility = await featureAbility.connectAbility(
    {
      deviceId: deviceId,
      bundleName: "com.example.ohmailets",
      abilityName: "com.example.ohmailets.ServiceAbility",
    },
    {
      onConnect: onConnectCallback,
      onDisconnect: onDisconnectCallback,
      onFailed: onFailedCallback,
    },
  );
}
// 连接成功后发送消息
async function sendMessageToRemoteService(dataList) {
  if (mRemote == null) {
    prompt.showToast({
      message: "mRemote is null"
    });
    return;
  }
  let option = new rpc.MessageOption();
  let data = new rpc.MessageParcel();
  let reply = new rpc.MessageParcel();
  data.writeStringArray(JSON.parse(dataList));
  prompt.showToast({
    message: "sendMessageToRemoteService" + dataList,
    duration: 3000
  });

  await mRemote.sendRequest(1, data, reply, option);
//  let msg = reply.readInt();

}
// 断开连接
async function onDisconnectService() {
  if (connectedAbility == null) {
    return;
  }
  await featureAbility.disconnectAbility(connectedAbility);
  connectedAbility = null;
  prompt.showToast({
    message: "onDisconnectService disconnect done"
  });
}

// 设备列表弹出框
@CustomDialog
struct CustomDialogExample {
  @State editFlag: boolean = false
  @Consume dataList: string[]
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
                  onStartRemoteAbility(item, this.dataList);
                  this.controller.close();
                })
              Radio({ value: item ,group: 'select'})
                .onChange(() => {
                  onStartRemoteAbility(item, this.dataList);
                  this.controller.close();
                }).checked(false)
            }
          }.editable(this.editFlag)
        }, item => item)
      }
    }.width('100%').height(200).backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}

// 入口组件
@Entry
@Component
struct Index {
  private imageList: any[]= []
  @Provide dataList: string[]= ['xiaohua@134.com','xiaoming@134.com','假期温馨提示','2022年新春佳节即将来临，请同学们细读节前相关温馨提示，保持办公场所环境整洁，假期期间注意信息及个人安全，预祝全场景解决方案部全体同学新春快乐，虎虎生威！']

  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExample({ cancel: this.onCancel, confirm: this.onAccept }),
    cancel: this.existApp,
    autoCancel: true
  })

  build() {
    Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.SpaceBetween }) {
      Column() {
        Row() {
          Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
            Text('✕').fontSize(20).fontColor('#000000')
            Button('发送').width(70).fontSize(14).fontColor('#ffffff').backgroundColor('#fc4646')
              .onClick(() => {
                RegisterDeviceListCallback();
                this.dialogController.open();
              })
          }
          .height(50)
          .padding({ top: 10, right: 15, bottom: 10, left: 15 })
        }

        Column() {
          Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
            Text('收件人').width(70).height(30).fontSize(15).fontColor('#969393')
            Text(this.dataList[0]).width('100%').height(30).fontSize(15).fontColor('#000000')
          }
          .padding({ top: 5, right: 15, bottom: 5, left: 15 })

          Text().width('100%').height(1).backgroundColor('#f8f6f6')

          Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
            Text('发件人').width(70).height(30).fontSize(15).fontColor('#969393')
            Text(this.dataList[1]).width('100%').height(30).fontSize(15).fontColor('#000000')
          }
          .padding({ top: 5, right: 15, bottom: 5, left: 15 })

          Text().width('100%').height(1).backgroundColor('#f8f6f6')

          Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
            Text('主题').width(50).height(30).fontSize(15).fontColor('#969393')
            Text(this.dataList[2]).width('100%').height(30).fontSize(15).fontColor('#000000')
          }
          .padding({ top: 5, right: 15, bottom: 5, left: 15 })

          Text().width('100%').height(1).backgroundColor('#f8f6f6')
          TextArea({ placeholder: 'input your word', text: this.dataList[3]}).height('100%').width('100%')
            .onChange((value: string) => {
              this.dataList[3] = value
              if(mRemote){
                sendMessageToRemoteService(JSON.stringify(this.dataList));
              }
//              onDisconnectService();
          })
        }
      }

      Column() {
        Flex({ direction: FlexDirection.Row }) {
          List() {
            ForEach(this.imageList, (item) => {
              ListItem() {
                Image(item).width(50).height(50).objectFit(ImageFit.Contain)
              }.editable(true)
            }, item => item)
          }
          .listDirection(Axis.Horizontal) // 排列方向
          .divider({ strokeWidth: 2, color: 0xFFFFFF, startMargin: 20, endMargin: 20 }) // 每行之间的分界线
        }.width('100%').height(50).backgroundColor('#ccc')

        Text().width('100%').height(1).backgroundColor('#f8f6f6')
        Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceBetween }) {
          Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
            Button({ stateEffect: false }) {
              Image($rawfile('icon_photo.png')).width(20).height(20)
            }.backgroundColor('#ffffff').margin({ right: 20 })
            .onClick(() => {
              RegisterDeviceListCallback();
              this.dialogController.open();
            })

            Button({ stateEffect: false }) {
              Image($rawfile('icon_at.png')).width(20).height(20)
            }.backgroundColor('#ffffff')
          }

          Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.End }) {
            Button({ stateEffect: false }) {
              Image($rawfile('icon_distributed.png')).width(20).height(20)
            }.backgroundColor('#ffffff')
            .onClick(() => {
//              this.getDeviceList()
            })

            Button({ stateEffect: false }) {
              Image($rawfile('icon_timer.png')).width(20).height(20)
            }.backgroundColor('#ffffff').margin({ left: 10, right: 10 })

            Button({ stateEffect: false }) {
              Image($rawfile('icon_enclosure.png')).width(20).height(20)
            }.backgroundColor('#ffffff')
          }
        }.height(50).padding(15)
      }
    }.width('100%').padding({ top: 5, bottom: 15 })
  }
  onCancel() {
  }

  onAccept() {
  }

  existApp() {
  }
  onDataChange(value) {
    console.log(JSON.stringify(value));
    prompt.showToast({
      message: "onDataChange" + JSON.stringify(value)
    });
  }
  async aboutToAppear() {
    this.grantPermission()

    this.subscribeEvent();
    let self = this;
    // 当被拉起时，通过want传递的参数同步对端界面UI
    await featureAbility.getWant((error, want) => {
      var status = want.parameters;
      if (want.parameters.dataList) {
        self.dataList = JSON.parse(status.dataList)
        // 远端被拉起后，连接对端的service
        if (want.parameters.remoteDeviceId) {
          let remoteDeviceId = want.parameters.remoteDeviceId
          onConnectRemoteService(remoteDeviceId)
        }
      }
    });
  }
  grantPermission() {
    console.info(`[game] grantPermission`)
    let context = featureAbility.getContext()
    context.requestPermissionsFromUser(['ohos.permission.DISTRIBUTED_DATASYNC'], 666, function (result) {
      console.info(`[game] grantPermission,requestPermissionsFromUser`)
    })
  }
  subscribeEvent() {
    let self = this;
    // 用于保存创建成功的订阅者对象，后续使用其完成订阅及退订的动作
    var subscriber;
    // 订阅者信息
    var subscribeInfo = {
      events: ["publish_moveImage"],
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
      self.dataList = data.parameters.dataList;
      self.dataList = self.dataList.slice(0, 4)
      prompt.showToast({
        message: "SubscribeCallBack" + data.parameters.dataList[3],
        duration: 3000
      });
      onDisconnectService();
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


