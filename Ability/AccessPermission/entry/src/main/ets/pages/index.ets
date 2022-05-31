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

import RemoteDeviceModel from '../model/RemoteDeviceModel'

// 实例化RemoteDeviceModel
let remoteDeviceModel = new RemoteDeviceModel();
// 设备列表集合
let deviceListGlobal = [];

@Entry
@Component
struct Index {

  @State permissionDescribe: string = '未授权'

  // 未设置权限提示弹框
  dialogHints : CustomDialogController = new CustomDialogController({
    builder: DialogHints({action: this.onAccept}),
    autoCancel: true
  });

  // 设备列表弹框
  dialogController: CustomDialogController = new CustomDialogController({
    builder: DialogDeviceList({action: this.onAccept}),
    autoCancel: true
  })

  onAccept() {
  }

  build() {
    Row() {
      Column() {
        Text('授权状态：' + this.permissionDescribe)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        Button('查询内网设备', { type: ButtonType.Capsule, stateEffect: true })
          .backgroundColor(0x317aff)
          .fontSize(40)
          .height(50)
          .onClick(() => {
            // 判断是否进行过授权操作
            globalThis.getData().then((preferences) => {
              // 获取key：requestPermissionFlag的值，false未进行过操作、true进行过操作
              let promiseGet = preferences.get('requestPermissionFlag', false)
              promiseGet.then((value) => {
                if(value) {
                  // 进行过授权操作，判断是否授权
                   globalThis.checkPermission().then(res => {
                     if (res == globalThis.permissionGranted) {
                       // 有权限，查询内网设备
                       remoteDeviceModel.registerDeviceListCallback(() => {
                         this.callBack()
                       })
                       this.dialogController.open();
                     } else {
                       // 无权限，给出提示
                       this.dialogHints.open()
                     }
                   })
                } else {
                  // 未进行过授权操作，弹出权限申请框
                  globalThis.requestPermission()
                }
              }).catch((err) => {
                console.info("Get the value of startup failed, err: " + err)
              })
            })
          })
        .margin({top: 50})
      }
      .width('100%')
    }
    .height('100%')
  }

  // 查询到设备后回调此函数，放到列表数据中
  callBack() {
    console.info('AccessPermission[IndexPage] registerDeviceListCallback, callback entered');
    var list = [];
    var deviceList;
    if (remoteDeviceModel.discoverList.length > 0) {
      deviceList = remoteDeviceModel.discoverList;
    } else {
      deviceList = remoteDeviceModel.deviceList;
    }
    console.info('AccessPermission[IndexPage] on remote device updated, count=' + deviceList.length);
    for (var i = 0; i < deviceList.length; i++) {
      console.info('AccessPermission[IndexPage] device ' + i + '/' + deviceList.length + ' deviceId='
      + deviceList[i].deviceId + ' deviceName=' + deviceList[i].deviceName );
      list.push({
        name: deviceList[i].deviceName,
        id: deviceList[i].deviceId
      });
    }
    deviceListGlobal = list;
    console.info('AccessPermission[IndexPage] list deviceId' + JSON.stringify(deviceListGlobal));
  }

  aboutToAppear(){
    // 进入应用，查询是否进行过授权操作（包括：禁止操作、允许操作）
    globalThis.getData().then((preferences) => {
      // 获取key：requestPermissionFlag的值，false未进行过操作、true进行过操作
      let promiseGet = preferences.get('requestPermissionFlag', false)
      promiseGet.then((value) => {
        if(value) {
          // 进行过授权操作，判断是否有权限
          globalThis.checkPermission().then(res => {
            if(res != globalThis.permissionGranted) {
              // 未授权
              this.permissionDescribe = '未授权'
            } else {
              // 已授权
              this.permissionDescribe = '已授权'
            }
          })
        } else {
          // 未进行过授权操作，将requestPermissionFlag设置为false
          globalThis.putData(false)
        }
      }).catch((err) => {
        console.info("Get the value of startup failed, err: " + err)
      })
    }).catch((err) => {
      console.info("Get the preferences failed, err: " + err)
    })

    // 授权拒绝、通过时调用，用于判断界面展示是否授权
    globalThis.judgePermission = (() => {
      globalThis.checkPermission().then(res => {
        if(res != globalThis.permissionGranted) {
          this.permissionDescribe = '未授权'
        } else {
          this.permissionDescribe = '已授权'
        }
      })
    })
  }
}

// 未申请权限弹框
@CustomDialog
struct DialogHints {
  controller: CustomDialogController;
  action: () => void;
  build() {
    Column(){
      Text('权限未申请，不能进行请求配对操作！').fontSize(20).width('90%').textAlign(TextAlign.Center).margin({top: 10})
      Text('如需重新申请权限，请在“设置 -> 应用 -> 应用管理 -> 应用名 -> 存储 -> 删除数据、清空缓存”').fontSize(20).width('90%').textAlign(TextAlign.Center).margin({top: 10})
      Divider().width('90%').margin({top: 10})
      Row() {
        Text('确定').fontSize(15).width('50%').textAlign(TextAlign.Center).fontColor('#17A98E')
          .onClick(() => {
            this.controller.close();
          })
      }.margin({top: 20, bottom: 10})
    }.width('100%')
  }
}

// 设备列表弹框
@CustomDialog
struct DialogDeviceList {
  controller: CustomDialogController
  action: () => void;
  build() {
    Column() {
      Text('设备列表').fontSize(20).width('90%').textAlign(TextAlign.Start).margin({top: 10})
      Divider().width('90%').margin({top: 10, bottom: 10})
      Column() {
        if(deviceListGlobal.length == 0) {
          Text('未查询到设备')
            .width('100%')
            .height(100)
            .fontSize(16)
            .textAlign(TextAlign.Center)
            .borderRadius(10)
            .backgroundColor(0xFFFFFF)
        } else {
          List({ space: 20, initialIndex: 0 }) {
            ForEach(deviceListGlobal, (item) => {
              ListItem() {
                Column() {
                  // 设备名称
                  Row() {
                    Text('name: ')
                      .fontSize(20)
                      .textAlign(TextAlign.Center)
                    Text('' + item.name)
                      .fontSize(20)
                      .textAlign(TextAlign.Center)
                  }.width('100%')
                  // 设备id
                  Row() {
                    Text('deviceId: ')
                      .fontSize(20)
                      .textAlign(TextAlign.Center)
                    Text('' + item.id)
                      .fontSize(20)
                      .textAlign(TextAlign.Center)
                      .textOverflow({overflow: TextOverflow.Ellipsis})
                  }.width('100%')
                }
                .justifyContent(FlexAlign.Start)
                .backgroundColor('#E2E2E2')
                .borderRadius(10)
              }
            }, item => item.id)
          }
          .listDirection(Axis.Vertical) // 排列方向
          .divider({ strokeWidth: 2, color: 0xFFFFFF, startMargin: 10, endMargin: 10 }) // 每行之间的分界线
          .edgeEffect(EdgeEffect.None) // 滑动到边缘无效果
          .chainAnimation(false) // 联动特效关闭
          .width('90%').margin({bottom: 10})
        }
      }.flexGrow(1)
      Divider().width('90%').margin({top: 10})
      Row() {
        Text('确定')
          .fontSize(15)
          .width('50%')
          .textAlign(TextAlign.Center)
          .fontColor('#17A98E')
          .onClick(() => {
            this.controller.close();
          })
      }.margin({top: 20, bottom: 10})
    }
  }
}
