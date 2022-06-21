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

import { GameElement } from '../model/GameElement'
import prompt from '@system.prompt';
import RemoteDeviceModel from '../model/RemoteDeviceModel';
import featureAbility from '@ohos.ability.featureAbility';
import rpc from "@ohos.rpc";
import commonEvent from '@ohos.commonEvent';

var remoteDeviceModel = new RemoteDeviceModel();
let deviceList = [];
var localDeviceId = "";
var mRemote;

// 发现设备列表
function registerDeviceListCallback() {
  remoteDeviceModel.registerDeviceListCallback(() => {
    localDeviceId = remoteDeviceModel.localDeviceId;
    deviceList.length = 0;
    for (var i = 0; i < remoteDeviceModel.deviceList.length; i++) {
      deviceList.push(remoteDeviceModel.deviceList[i].deviceId);
    }
  });
}

// 拉起远端FA
function startRemoteAbility(deviceId) {
  var params = {
    deviceId: localDeviceId
  }
  var wantValue = {
    bundleName: 'com.huawei.cookbook',
    abilityName: 'com.huawei.cookbook.MainAbility',
    deviceId: deviceId,
    parameters: params
  };
  featureAbility.startAbility({
    want: wantValue
  }).then((data) => {
    console.info('[game] featureAbility.startAbility finished, localDeviceId=' + localDeviceId + '----deviceId:' + deviceId);
    // 拉起远端后，连接远端service
    connectRemoteService(deviceId)
  });
}

// 连接远端Service
async function connectRemoteService(deviceId) {
  // 连接成功的回调
  async function onConnectCallback(element, remote) {
    console.log('[game]connectRemoteService onConnectCallback:' + remote)
    mRemote = remote;
  }
  // Service异常死亡的回调
  function onDisconnectCallback(element) {
    console.log('[game]connectRemoteService onDisconnectCallback')
    prompt.showToast({
      message: "onDisconnectCallback "
    });
  }
  // 连接失败的回调
  function onFailedCallback(code) {
    console.log('[game]connectRemoteService onFailedCallback')
    prompt.showToast({
      message: "onConnectRemoteService onFailed: " + code
    });
  }

  if (remoteDeviceModel.deviceList.length === 0) {
    console.log('[game]connectRemoteService onConnectRemoteService no device found')
    prompt.showToast({
      message: "onConnectRemoteService no device found"
    });
    return;
  }
  await featureAbility.connectAbility(
    {
      'deviceId': deviceId,
      'bundleName': "com.huawei.cookbook",
      'abilityName': "com.huawei.cookbook.ServiceAbility",
    },
    {
      onConnect: onConnectCallback,
      onDisconnect: onDisconnectCallback,
      onFailed: onFailedCallback,
    },
  );
}

// 连接成功后发送消息
async function sendMessageToRemoteService(score) {
  console.log('[game]connectRemoteService sendMessageToRemoteService:')
  if (mRemote == null) {
    console.log('[game]connectRemoteService mRemote == null')
    //    prompt.showToast({
    //      message: "mRemote is null"
    //    });
    return;
  }
  let option = new rpc.MessageOption();
  let data = new rpc.MessageParcel();
  let reply = new rpc.MessageParcel();
  data.writeInt(score);
  await mRemote.sendRequest(1, data, reply, option);
  let msg = reply.readInt();
  console.log('[game]connectRemoteService msg:' + msg)
}

@CustomDialog
export struct DeviceListDialog {
  controller: CustomDialogController

  build() {
    Column() {
      Text("选择设备")
        .fontWeight(FontWeight.Bold)
        .fontSize(20)
        .margin({ top: 20, bottom: 10 })

      List() {
        ForEach(deviceList, item => {
          ListItem() {
            Stack() {
              Text(item)
                .fontSize(12)
                .margin({ top: 10 })
            }
            .onClick(() => {
              startRemoteAbility(item)
              this.controller.close();
            })
            .padding({ left: 30, right: 30 })
          }
        }, item => item.toString())
      }
      .height("30%")
      .align(Alignment.TopStart)

      Row() {
        Button("取消")
          .layoutWeight(1)
          .backgroundColor(Color.White)
          .fontColor(Color.Blue)
          .height(60)
          .onClick(() => {
            this.controller.close();
          })
      }
    }
  }
}

@Entry
@Component
struct Index {
  private num: number= 0
  private bulletWith: number= 12
  private bulletHeight: number= 24
  private planeSize: number= 120
  private bigEnemySize: number= 140
  private smallEnemySize: number= 80
  private propsSize: number= 120
  @State bulletPosX: number= this.planeSize + this.bulletHeight
  @State bulletPosY: number= 0
  @State planePosX: number= 100
  @State planePosY: number= 0
  @State bullets: GameElement[]= []
  @State enemyPlanes: GameElement[]= []
  @State booms: GameElement[]= []
  @State startX: number= 0
  @State startY: number= 0
  @State screenWidth: number= 0
  @State screenHeight: number= 0
  @State propsPosY: number= 0
  @State propsPosX: number= 0
  @State score: number= 0
  @State deviceList: any[] = []
  @State bombNum: number = 0
  @State getPropsFlag: boolean = true
  @State actionType: number = 0
  @State angle: number = 0
  dialogController: CustomDialogController = new CustomDialogController({
    builder: DeviceListDialog({}),
    autoCancel: true,
    alignment: DialogAlignment.Center
  });

  aboutToAppear() {
    this.grantPermission()

    this.subscribeEvent();
    this.screenWidth = 720
    this.screenHeight = 1200
    this.planePosY = this.screenHeight - 170
    this.bulletPosY = this.screenHeight - 170 - this.bulletHeight
    this.propsPosX = Math.round((Math.random() * (this.screenWidth - this.propsSize)))

    this.startGame()
  }

  grantPermission() {
    console.info(`[game] grantPermission`)
    let context = featureAbility.getContext()
    context.requestPermissionsFromUser(['ohos.permission.DISTRIBUTED_DATASYNC'], 666, function (result) {
      console.info(`[game] grantPermission,requestPermissionsFromUser`)
    })
  }

  subscribeEvent() {
    let that = this;
    // 用于保存创建成功的订阅者对象，后续使用其完成订阅及退订的动作
    var subscriber;
    // 订阅者信息
    var subscribeInfo = {
      events: ["publish_action"],
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
      that.actionType = data.parameters.actionType;
      that.angle = data.parameters.angle;

      if (that.actionType == 1) {
        that.createBullet()
      }

      if (that.actionType == 2) {
        if (that.bombNum > 0) {
          that.bombNum--
          that.destroyAllEnemy()
        }
      }

      if (that.angle != 0) {
        that.movePlaneByHandle()
      }

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

  startGame() {
    var that = this
    setInterval(function () {
      that.num += 1;

      // 60*16ms创建一个敌机
      if (that.num % 60 == 0) {
        that.createEnemyPlane()
      }

      // 画子弹
      var bulletsTemp: GameElement[] = []
      for (var i = 0; i < that.bullets.length; i++) {
        var bullet = that.bullets[i]
        bullet.positionY -= 8
        // 当子弹移除屏幕外的时候，释放掉
        if (bullet.positionY > 0) {
          bulletsTemp.push(bullet)
        }

      }
      that.bullets = bulletsTemp


      // 移动飞机
      var enemyPlanesTemp: GameElement[] = []
      for (var j = 0; j < that.enemyPlanes.length; j++) {
        var enemyPlane = that.enemyPlanes[j]
        enemyPlane.positionY += 6

        // 当飞机移除屏幕外的时候，释放掉
        if (enemyPlane.positionY < that.screenHeight) {
          enemyPlanesTemp.push(enemyPlane)
        }
      }
      that.enemyPlanes = enemyPlanesTemp

      // 每隔 500*16ms显示降落伞
      if (that.num % 500 == 0) {
        that.getPropsFlag = true
        that.propsPosY = -that.propsSize
        that.propsPosX = Math.round((Math.random() * (that.screenWidth - that.propsSize)))
      }

      // 刷新道具位置
      if (that.propsPosY < that.screenHeight) {
        that.propsPosY += 6
      }

      that.checkCollision()
    }, 16);
  }

  // 收到手柄指令后移动飞机
  movePlaneByHandle() {
    this.planePosX += Math.cos((this.angle + 90) * (Math.PI / 180)) * 10
    this.planePosY -= Math.sin((this.angle + 90) * (Math.PI / 180)) * 10
    if (this.planePosX < 0) {
      this.planePosX = 0
    }

    if (this.planePosY < 0) {
      this.planePosY = 0
    }

    if (this.planePosX > this.screenWidth - this.planeSize) {
      this.planePosX = this.screenWidth - this.planeSize
    }

    if (this.planePosY > this.screenHeight - 170) {
      this.planePosY = this.screenHeight - 170
    }
    this.bulletPosX = this.planePosX + (this.planeSize - this.bulletWith) / 2
    this.bulletPosY = this.planePosY - this.bulletHeight
  }

  // 创建子弹
  createBullet() {
    var that = this
    let bullet = new GameElement();
    bullet.positionX = that.bulletPosX
    bullet.positionY = that.bulletPosY
    bullet.timestamp = new Date().getTime()
    bullet.imgSrc = '/common/images/bullet.png'
    bullet.imgWidth = that.bulletWith
    bullet.imgHeight = that.bulletHeight
    that.bullets.push(bullet)
  }

  // 创建敌机
  createEnemyPlane() {
    var that = this
    let flag = Math.round((Math.random() + 1))
    let enemy = new GameElement()
    enemy.timestamp = new Date().getTime()
    enemy.flag = flag
    if (flag === 1) {
      enemy.positionX = Math.round((Math.random() * (that.screenWidth - that.smallEnemySize)))
      enemy.positionY = -that.smallEnemySize
      enemy.imgSrc = '/common/images/small.png'
      enemy.imgWidth = that.smallEnemySize
      enemy.imgHeight = that.smallEnemySize
    } else {
      enemy.positionX = Math.round((Math.random() * (that.screenWidth - that.bigEnemySize)))
      enemy.positionY = -that.bigEnemySize
      enemy.imgSrc = '/common/images/big.png'
      enemy.imgWidth = that.bigEnemySize
      enemy.imgHeight = that.bigEnemySize
    }
    that.enemyPlanes.push(enemy)
  }

  // 检查是否发生碰撞
  checkCollision() {
    var that = this;

    let myPlane = new GameElement();
    myPlane.positionX = this.planePosX
    myPlane.positionY = this.bulletPosY
    myPlane.imgWidth = this.planeSize
    myPlane.imgHeight = this.planeSize

    let props = new GameElement();
    props.positionX = this.propsPosX
    props.positionY = this.propsPosY
    props.imgWidth = this.propsSize
    props.imgHeight = this.propsSize

    for (var i = 0; i < this.enemyPlanes.length; i++) {
      var enemy = this.enemyPlanes[i];

      for (var j = 0; j < this.bullets.length; j++) {
        var bullet = this.bullets[j];
        var inside = this.isInside(bullet, enemy);

        // 发生碰撞
        if (inside) {
          enemy.imgSrc = '/common/images/boom.png'
          if (enemy.flag == 1) {
            this.score += 50
            sendMessageToRemoteService(that.score)
          } else if (enemy.flag == 2) {
            this.score += 100
            sendMessageToRemoteService(that.score)
          }
          // 清除子弹
          this.enemyPlanes.splice(i, 1);
          i--;
          enemy.flag = 3
          // 清除被子弹打中敌机
          that.bullets.splice(j, 1);
          j--;
        }
      }
    }

    // 飞机和降落伞是否发生碰撞
    var isGetProps = this.isInside(myPlane, props);
    if (isGetProps && this.getPropsFlag) {
      this.getPropsFlag = false
      this.bombNum++
      this.propsPosY = 2000
    }
  }


  // 销毁所有敌机
  destroyAllEnemy() {
    for (var k = 0; k < this.enemyPlanes.length; k++) {
      var destroyEnemy = this.enemyPlanes[k];
      if (destroyEnemy.flag == 1) {
        this.score += 50
        sendMessageToRemoteService(this.score)
      }
      if (destroyEnemy.flag == 2) {
        this.score += 100
        sendMessageToRemoteService(this.score)
      }
    }
    this.enemyPlanes = []
    this.propsPosY = 2000
  }

  // 判断两个元素是否有重叠
  isInside(v1, v2) {
    var v1StartX = v1.positionX;
    var v1EndX = v1.positionX + v1.imgWidth;
    var v1StartY = v1.positionY;
    var v1EndY = v1.positionY + v1.imgHeight;
    var v2StartX = v2.positionX;
    var v2EndX = v2.positionX + v2.imgWidth;
    var v2StartY = v2.positionY;
    var v2EndY = v2.positionY + v2.imgHeight;
    var leftTop = v1StartX > v2StartX && v1StartX < v2EndX && v1StartY > v2StartY && v1StartY < v2EndY;
    var rightTop = v1EndX > v2StartX && v1EndX < v2EndX && v1StartY > v2StartY && v1StartY < v2EndY;
    var leftBottom = v1StartX > v2StartX && v1StartX < v2EndX && v1EndY > v2StartY && v1EndY < v2EndY;
    var rightBottom = v1EndX > v2StartX && v1EndX < v2EndX && v1EndY > v2StartY && v1EndY < v2EndY;
    return leftTop || rightTop || leftBottom || rightBottom;
  }

  onTouchEvent(event: TouchEvent) {
    switch (event.type) {
      case TouchType.Down:
        this.startX = event.touches[0].screenX;
        this.startY = event.touches[0].screenY;
        break;

      case TouchType.Move:

        var moveX = parseInt(((event.touches[0].screenX - this.startX)).toFixed(1))
        var moveY = parseInt(((event.touches[0].screenY - this.startY)).toFixed(1))

        this.planePosX += moveX
        this.planePosY += moveY


        if (this.planePosX < 0) {
          this.planePosX = 0
        }

        if (this.planePosX >= (this.screenWidth - this.planeSize)) {
          this.planePosX = this.screenWidth - this.planeSize
        }

        if (this.planePosY < 0) {
          this.planePosY = 0
        }

        if (this.planePosY >= (this.screenHeight - 170)) {
          this.planePosY = this.screenHeight - 170
        }

        this.bulletPosX = this.planePosX + (this.planeSize - this.bulletWith) / 2
        this.bulletPosY = this.planePosY - this.bulletHeight

        this.startX = event.touches[0].screenX;
        this.startY = event.touches[0].screenY;
        break;

      case TouchType.Up:

        break;

      default:
        break;
    }
  }

  build() {
    Stack() {
      Image('/common/images/bg.png')
        .width('100%')
        .height('100%')

      ForEach(this.bullets, item => {
        Image(item.imgSrc)
          .width(item.imgWidth)
          .height(item.imgHeight)
          .position({ x: item.positionX, y: item.positionY })
      }, item => item.timestamp.toString())

      ForEach(this.enemyPlanes, item => {
        Image(item.imgSrc)
          .width(item.imgWidth)
          .height(item.imgHeight)
          .position({ x: item.positionX, y: item.positionY })
      }, item => item.timestamp.toString())

      Image('/common/images/planeOne.png')
        .width(this.planeSize)
        .height(this.planeSize)
        .position({ x: this.planePosX, y: this.planePosY })
        .onTouch((event: TouchEvent) => {
          this.onTouchEvent(event)
        })

      Image('/common/images/props.png')
        .width(this.propsSize)
        .height(this.propsSize)
        .position({ x: this.propsPosX, y: this.propsPosY })


      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
        Text("得分：" + this.score)
          .fontSize(45)
          .fontColor(Color.White)
          .flexGrow(1)

        Image('/common/images/device.png')
          .width(90)
          .height(90)
          .padding(20)
          .onClick(() => {
            registerDeviceListCallback();
            this.dialogController.open();
          })

        Row() {
          Image('/common/images/bomb.png')
            .width(20)
            .height(60)
          Text('x' + this.bombNum)
            .fontSize(40)
            .fontColor(Color.White)
        }
        .margin({ left: 20 })
        .padding(20)
        .onClick(() => {
          if (this.bombNum > 0) {
            this.bombNum--
            this.destroyAllEnemy()
          }
        })

      }
      .position({ x: 0, y: 0 })
      .width('100%')

    }
    .height('100%')
    .width('100%')
  }
}