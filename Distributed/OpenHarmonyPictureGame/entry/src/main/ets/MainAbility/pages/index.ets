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
function onStartRemoteAbility(deviceId, imageIndexForPosition, pictureList: string[]) {
  AuthDevice(deviceId);
  let numDevices = remoteDeviceModel.deviceList.length;
  if (numDevices === 0) {
    prompt.showToast({
      message: "onStartRemoteAbility no device found"
    });
    return;
  }

  var params = {
    imageIndexForPosition: JSON.stringify(imageIndexForPosition),
    pictureList: JSON.stringify(pictureList),
    remoteDeviceId: localDeviceId
  }
  var wantValue = {
    bundleName: 'com.pictureGame.pictureGame',
    abilityName: 'com.example.openharmonypicturegame.MainAbility',
    deviceId: deviceId,
    parameters: params
  };
  featureAbility.startAbility({
    want: wantValue
  }).then((data) => {
    // 拉起远端后，连接远端service
    onConnectRemoteService(deviceId)
  });
}
// 连接远端Service
async function onConnectRemoteService(deviceId) {
  onDisconnectService()
  // 连接成功的回调
  async function onConnectCallback(element, remote) {

    mRemote = remote;
  }
  // Service异常死亡的回调
  function onDisconnectCallback(element) {
  }
  // 连接失败的回调
  function onFailedCallback(code) {
    prompt.showToast({
      message: "onConnectRemoteService onFailed: " + code
    });
  }

  connectedAbility = await featureAbility.connectAbility(
    {
      deviceId: deviceId,
      bundleName: "com.pictureGame.pictureGame",
      abilityName: "com.example.openharmonypicturegame.ServiceAbility",
    },
    {
      onConnect: onConnectCallback,
      onDisconnect: onDisconnectCallback,
      onFailed: onFailedCallback,
    },
  );
}
// 连接成功后发送消息
async function sendMessageToRemoteService(imageIndexForPosition) {
  if (mRemote == null) {
    prompt.showToast({
      message: "mRemote is null"
    });
    return;
  }
  let option = new rpc.MessageOption();
  let data = new rpc.MessageParcel();
  let reply = new rpc.MessageParcel();
  data.writeIntArray(JSON.parse(imageIndexForPosition));
  await mRemote.sendRequest(1, data, reply, option);
  let msg = reply.readInt();

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
  @Consume imageIndexForPosition: number[]
  @Consume pictureList: string[]
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

                  onStartRemoteAbility(item, this.imageIndexForPosition, this.pictureList);
                  this.controller.close();
                })
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
struct PictureGame {
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

  @Provide imageIndexForPosition: number[] = []
  @Provide pictureList: string[]= []
  @Provide clickAble : number = 1

  build() {
    Column() {
      Text("益智拼图游戏").fontSize(40).margin({ top: 30, bottom: 30 })
      PictureGrid()
      Row() {
        Button("重新开始").fontSize(20).margin({ right: 10 }).onClick(() => {
          this.onRandom()
          sendMessageToRemoteService(JSON.stringify(this.imageIndexForPosition))
        })
        Button("亲子协同").fontSize(20).margin({ left: 10 }).onClick(() => {
          RegisterDeviceListCallback();

          this.dialogController.open();
        })
      }.margin(30)

      Image($rawfile('picture.jpg')).width(300).height(300)

    }
    .width('100%')
    .height('100%')
  }

  async aboutToAppear() {
    let self = this;
    self.grantPermission();
    this.onRandom();
    // 当被拉起时，通过want传递的参数同步对端界面UI
    await featureAbility.getWant((error, want) => {
      var status = want.parameters;
      if (want.parameters.pictureList) {
        self.pictureList = JSON.parse(status.pictureList)
        self.imageIndexForPosition = status.imageIndexForPosition;
        // 远端被拉起后，连接对端的service
        if (want.parameters.remoteDeviceId) {
          let remoteDeviceId = want.parameters.remoteDeviceId
          onConnectRemoteService(remoteDeviceId)
        }
      }
    });
  }

  grantPermission(){
    let context = featureAbility.getContext();
    context.requestPermissionsFromUser(['ohos.permission.DISTRIBUTED_DATASYNC'],666,function(result){
      console.info('grantPermission,requestPermissionFromUser,result.requestCode= ${result.requestCode}')
    })
  }

  onRandom() {
    this.setupRandomPosition();
    this.pictureList = []
    this.clickAble = 1
    this.imageIndexForPosition.forEach(value => {
      if (value == 9) {
        this.pictureList.push("--")
      } else {
        this.pictureList.push(`picture_0` + value + `.png`)
      }
    });
  }

  setupRandomPosition() {
    let list1 = [5, 4, 3, 9, 1, 8, 6, 7, 2];
    let list2 = [3, 1, 6, 7, 9, 8, 4, 2, 5];
    let list3 = [4, 8, 3, 5, 2, 7, 9, 1, 6];
    let list4 = [4, 3, 5, 2, 8, 7, 6, 1, 9];
    let lists = [list1, list2, list3, list4];
    this.imageIndexForPosition = lists[Math.floor(Math.random() * 4)];
  }
}
// 九宮格拼图组件
@Component
struct PictureGrid {
  private gridRowTemplate: string = ''
  @Consume imageIndexForPosition: number[]
  @Consume  pictureList: string[]
  private heightValue: number;
  @Consume clickAble : number


  aboutToAppear() {
    this.subscribeEvent();
    var rows = Math.round(this.pictureList.length / 3);
    this.gridRowTemplate = '1fr '.repeat(rows);
    this.heightValue = rows * 101;
  }

  build() {
    Column() {
      Grid() {
        ForEach(this.pictureList.map((item, index) => {
          return { i: index, data: item };
        }), (item, index) => {
          GridItem() {
            Image($rawfile(this.pictureList[item.i]))
              .width(100)
              .height(100)
              .onClick(() => {
                let self = this
                //点击图片更新本地UI
                if(this.clickAble == 1){
                  self.onChange(item.i);
                }

              })

          }
        }, (item: string) => item.toString())


      }
      .rowsTemplate(this.gridRowTemplate)
      .columnsTemplate('1fr 1fr 1fr')
      .columnsGap(1)
      .rowsGap(1)
      .height(this.heightValue)
      .width(303)
    }
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
      self.imageIndexForPosition = data.parameters.imageIndexForPosition;
      self.pictureList = [];
      self.imageIndexForPosition.forEach(value => {
        if (value == 9) {
          self.pictureList.push("--")
        } else {
          self.pictureList.push(`picture_0` + value + `.png`)
        }
      });

      self.onFinish();
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

  onChange(index) {
    let self = this;
    let menu = {
      "1": [2, 4],
      "2": [1, 3, 5],
      "3": [2, 6],
      "4": [1, 5, 7],
      "5": [2, 4, 6, 8],
      "6": [3, 5, 9],
      "7": [4, 8],
      "8": [5, 7, 9],
      "9": [6, 8]
    }
    let click_num = index + 1;

    let no_see_num = self.imageIndexForPosition.indexOf(9) + 1;
    let arr = menu[no_see_num];
    if (arr.length == 2) {
      if (!(arr[0] == click_num || arr[1] == click_num)) {

      } else {
        let temp = self.imageIndexForPosition[no_see_num - 1];
        self.imageIndexForPosition[no_see_num - 1] = self.imageIndexForPosition[click_num - 1];
        self.imageIndexForPosition[click_num - 1] = temp;

        self.pictureList = [];
        self.imageIndexForPosition.forEach(value => {
          if (value == 9) {
            self.pictureList.push("--")
          } else {
            self.pictureList.push(`picture_0` + value + `.png`)

          }
        });
      }
    } else if (arr.length == 3) {
      if (!(arr[0] == click_num || arr[1] == click_num || arr[2] == click_num)) {

      } else {
        let temp = self.imageIndexForPosition[no_see_num - 1];
        self.imageIndexForPosition[no_see_num - 1] = self.imageIndexForPosition[click_num - 1];
        self.imageIndexForPosition[click_num - 1] = temp;

        self.pictureList = [];
        self.imageIndexForPosition.forEach(value => {
          if (value == 9) {
            self.pictureList.push("--")
          } else {
            self.pictureList.push(`picture_0` + value + `.png`)

          }
        });
      }
    } else if (arr.length == 4) {
      if (!(arr[0] == click_num || arr[1] == click_num || arr[2] == click_num || arr[3] == click_num)) {

      } else {
        let temp = self.imageIndexForPosition[no_see_num - 1];
        self.imageIndexForPosition[no_see_num - 1] = self.imageIndexForPosition[click_num - 1];
        self.imageIndexForPosition[click_num - 1] = temp;

        self.pictureList = [];
        self.imageIndexForPosition.forEach(value => {
          if (value == 9) {
            self.pictureList.push("--")
          } else {
            self.pictureList.push(`picture_0` + value + `.png`)

          }
        });
      }
    }
    if(mRemote){
      sendMessageToRemoteService(JSON.stringify(self.imageIndexForPosition));
    }
    self.onFinish();
  }

  onFinish() {
    let finalList = [1, 2, 3, 4, 5, 6, 7, 8, 9];
    if (this.equarList(this.imageIndexForPosition, finalList)) {
      this.pictureList = [];
      this.imageIndexForPosition.forEach(value => {
        this.pictureList.push("picture_0" + value + ".png")
      });
      this.clickAble = 0;
      prompt.showToast({
        message: "success"
      });
      onDisconnectService();
    }
  }

 equarList(c, d) {
    // 判断数组的长度
    if (c.length !== d.length) {
      return false
    } else {
      // 循环遍历数组的值进行比较
      for (let j = 0; j < c.length; j++) {
        if (c[j] !== d[j]) {
          return false
        }
      }
      return true;
    }
  }
}

