import prompt from '@system.prompt';
import commonEvent from '@ohos.commonEvent';
import featureAbility from '@ohos.ability.featureAbility';
import rpc from "@ohos.rpc";

var mRemote;
var actionType = 0 // 1:子弹   2：技能
var angle = 0

// 连接远端Service
async function connectRemoteService(deviceId) {
  // 连接成功的回调
  async function onConnectCallback(element, remote) {
    console.log('[handle]connectRemoteService onConnectCallback:' + remote)
    mRemote = remote;
  }
  // Service异常死亡的回调
  function onDisconnectCallback(element) {
    console.log('[handle]connectRemoteService onDisconnectCallback')
    prompt.showToast({
      message: "onDisconnectCallback "
    });
  }
  // 连接失败的回调
  function onFailedCallback(code) {
    console.log('[handle]connectRemoteService onFailedCallback')
    prompt.showToast({
      message: "onConnectRemoteService onFailed: " + code
    });
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
async function sendMessageToRemoteService() {
  console.log('[handle]connectRemoteService sendMessageToRemoteService:')
  if (mRemote == null) {
    console.log('[handle]connectRemoteService mRemote == null')
    prompt.showToast({
      message: "mRemote is null"
    });
    return;
  }
  let option = new rpc.MessageOption();
  let data = new rpc.MessageParcel();
  let reply = new rpc.MessageParcel();
  data.writeInt(actionType);
  data.writeInt(angle);
  await mRemote.sendRequest(1, data, reply, option);
  let msg = reply.readInt();
  console.log('[handle]connectRemoteService msg:' + msg)
}


@Entry
@Component
struct Index {
  private startPosX: number= 80
  private startPosY: number= 80
  private smallR: number= 70
  private QUADRANT_1: number= 1
  private QUADRANT_2: number= 2
  private QUADRANT_3: number= 3
  private QUADRANT_4: number= 4
  @State smallPosX: number= 80
  @State smallPosY: number= 80
  @State startX: number = 0
  @State startY: number = 0
  @State curX: number = 0
  @State curY: number = 0
  @State score: number = 0

  async aboutToAppear() {
    this.subscribeEvent();

    // 当被拉起时，通过want传递的参数同步对端界面UI
    await featureAbility.getWant((error, want) => {
      // 远端被拉起后，连接对端的service
      if (want.parameters.deviceId) {
        let remoteDeviceId = want.parameters.deviceId
        connectRemoteService(remoteDeviceId)
      }
    });
  }

  subscribeEvent() {
    let that = this;
    // 用于保存创建成功的订阅者对象，后续使用其完成订阅及退订的动作
    var subscriber;
    // 订阅者信息
    var subscribeInfo = {
      events: ["publish_score"],
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
      that.score = data.parameters.score;
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

  onTouchEvent(event: TouchEvent) {

    switch (event.type) {
      case TouchType.Down:
        this.startX = event.touches[0].screenX;
        this.startY = event.touches[0].screenY;
        break;

      case TouchType.Move:
        this.curX = event.touches[0].screenX;
        this.curY = event.touches[0].screenY;
        this.getSmallCurrentPos(this.curX - this.smallR - 60, this.curY - this.smallR - 60)
        angle = Math.round(this.calculateAngle());
        console.log("[handle]angle:" + Math.round(this.calculateAngle()));
        sendMessageToRemoteService()
        angle = 0
        break;

      case TouchType.Up:
        this.smallPosX = 80
        this.smallPosY = 80
        break;

      default:
        break;
    }
  }

// 计算手指(即小圆)移动距离大圆圆心的距离
  getDisZ() {
    // 小圆和大圆圆心
    var centerX = this.startPosX + this.smallR + 60
    var centerY = this.startPosY + this.smallR + 60

    return Math.sqrt(Math.abs(this.curX - centerX) * Math.abs(this.curX - centerX)
    + Math.abs(this.curY - centerY) * Math.abs(this.curY - centerY));
  }

  quadrant() {
    if (this.getFlagX() && !this.getFlagY()) {
      return this.QUADRANT_1;
    } else if (!this.getFlagX() && !this.getFlagY()) {
      return this.QUADRANT_2;
    } else if (!this.getFlagX() && this.getFlagY()) {
      return this.QUADRANT_3;
    } else if (this.getFlagX() && this.getFlagY()) {
      return this.QUADRANT_4;
    } else {
      return 0;
    }
  }

  getFlagX() {
    console.log("getFlagX====2:" + (this.curX - this.startPosX - 70 - 60));
    return this.curX - this.startPosX - 70 - 60 > 0 ? true : false;
  }

  getFlagY() {
    console.log("getFlagY====2:" + (this.curY - this.startPosY - 70 - 60));
    return this.curY - this.startPosY - 70 - 60 > 0 ? true : false;
  }

  getSmallCurrentPos(curX, curY) {
    var quadrant = this.quadrant(); // 手指所在象限
    console.log("quadrant4------:" + quadrant)
    if (this.getDisZ() < 150) {
      this.smallPosX = curX;
      this.smallPosY = curY;
    }

  }

// 计算移动距离的绝对值
  getDisAbsX() {
    var disAbsX = Math.abs(this.smallPosX - this.startPosX);
    console.log("[demo4]disAbsX:" + disAbsX);
    if (disAbsX < 10) {
      return 1;
    }
    return disAbsX;
  }

// 计算移动距离的绝对值
  getDisAbsY() {
    var disAbsY = Math.abs(this.smallPosY - this.startPosY);
    console.log("[demo4]disAbsY:" + disAbsY);
    if (disAbsY < 10) {
      return 1;
    }
    return disAbsY;
  }

  calculateAngle() {
    var angle = 0
    var degree = Math.atan(this.getDisAbsY() / this.getDisAbsX()) * 180 / Math.PI
    var quadrant = this.quadrant();

    switch (quadrant) {
      case this.QUADRANT_1:
      // 向右上移动
        angle = degree;
        break;
      case this.QUADRANT_2:
      // 向左上移动
        angle = 180 - degree;
        break;
      case this.QUADRANT_3:
      // 向左下移动
        angle = -180 + degree;
        break;
      case this.QUADRANT_4:
      // 向右下移动
        angle = -degree;
        break;
      default:
        angle = 0;
        break;
    }
    return angle;
  }

  build() {
    Stack() {

      Image('/common/images/bg.png')
        .width('100%')
        .height('100%')

      Text('score:' + this.score)
        .fontColor(Color.White)
        .fontSize(60)
        .height("100%")
        .width("100%")
        .padding({ bottom: 100 })
        .rotate({ x: 0, y: 0, z: 1, angle: 90 })

      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.SpaceBetween }) {
        Stack() {
          Image('/common/images/bigCircle.png')
            .width(300)
            .height(300)

          Image('/common/images/smallCircle.png')
            .width(140)
            .height(140)
            .position({ x: this.smallPosX, y: this.smallPosY }) // 30+75-35
            .onTouch((event: TouchEvent) => {
              this.onTouchEvent(event)
            })
        }

        Image('/common/images/start.png')
          .width(140)
          .height(140)

        Image('/common/images/pause.png')
          .width(140)
          .height(140)

        Row() {
          Image('/common/images/a.png')
            .width(160)
            .height(160)
            .margin({ right: 20, bottom: 80 })
            .onClick(() => {
              actionType = 2
              sendMessageToRemoteService()
              actionType = 0
            })

          Image('/common/images/b.png')
            .width(200)
            .height(200)
        }.alignItems(VerticalAlign.Bottom)
        .onClick(() => {
          actionType = 1
          sendMessageToRemoteService()
          actionType = 0
        })
      }
      .width('100%')
      .height('100%')
      .padding(60)
    }
  }
}