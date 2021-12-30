// @ts-nocheck
import featureAbility from '@ohos.ability.featureAbility';
import KvStoreModel from '../model/KvStoreModel';
import RemoteDeviceModel from '../model/RemoteDeviceModel';
var DEVICE_LIST_LOCALHOST = { name: '本机', id: 'localhost' };
const CHANGE_POSITION = 'change_position';
const DEfAULT_PATH_COMMAND = '';

@CustomDialog
struct CustomDialogExample {
  controller: CustomDialogController
  cancel: () => void
  confirm: (deviceId, deviceName) => void
  startAbility: (deviceId, deviceName, positionList) => void
  deviceList:() => void
  positionList:() => void
//  private selectedDevices: any[]= [];
  build() {
    Column() {
      Text('设备列表').width('70%').fontSize(20).margin({ top: 10, bottom: 10 })
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        List({ space: 20, initialIndex: 0 }) {
          ForEach(this.deviceList, (item) => {
            ListItem() {
              Text('' + item.name)
                .width('100%').height(100).fontSize(16)
                .textAlign(TextAlign.Center).borderRadius(10).backgroundColor(0xFFFFFF)
                .onClick((event: ClickEvent) =>{
                  this.controller.close();
                  this.startAbility(item.id, item.name, this.positionList)
//                  this.confirm(item.id,item.name)
                })
            }.editable(true)
          }, item => item.id)
        }
        .listDirection(Axis.Vertical) // 排列方向
        .divider({ strokeWidth: 2, color: 0xFFFFFF, startMargin: 20, endMargin: 20 }) // 每行之间的分界线
        .edgeEffect(EdgeEffect.None) // 滑动到边缘无效果
        .chainAnimation(false) // 联动特效关闭
        .onScrollIndex((firstIndex: number, lastIndex: number) => {
          console.info('first' + firstIndex)
          console.info('last' + lastIndex)
        })
      }.margin({ bottom: 10 })
    }
  }
}


@Entry
@Component
struct Index {
  @State startX: number = 0
  @State startY: number = 0
  @State moveX: number = 0
  @State moveY: number = 0
  @State endX: number = 0
  @State endY: number = 0
  @State pathCommands: string = DEfAULT_PATH_COMMAND
  @State deviceList: any[] = []
  private BUNDLE_NAME: string = "com.huawei.cookbook";
  private kvStoreModel: KvStoreModel = new KvStoreModel()
  private remoteDeviceModel: RemoteDeviceModel = new RemoteDeviceModel()
  @State positionList: any[] = []
  @State initialData: any[] = []
  private isNeedSync: boolean = false
  private intervalID: number = 0

  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExample({ cancel: this.onCancel, confirm: this.onAccept, deviceList: this.deviceList,positionList: this.positionList,startAbility: this.startAbilityContinuation }),
    cancel: this.existApp,
    autoCancel: true,
    deviceList: this.deviceList,
    positionList: this.positionList
  })
  onCancel() {
    console.info('Callback when the first button is clicked')
  }
  onAccept() {
    console.info('Click when confirm')
  }
  existApp() {
    console.info('Click the callback in the blank area')
  }

  build() {
    Column({ space: 1 }) {
      Flex() {
        Image($r('app.media.goback')).width(70).height(70).position({ x: 30, y: 0 })
          .onClick((event: ClickEvent) =>{
            this.goBack()
          })
        Image($r('app.media.ic_hop')).width(70).height(70)
          .align(Alignment.TopEnd)
          .flexGrow(1)
          .position({ x: 375, y: 0 })
          .onClick((event: ClickEvent) =>{
            this.onContinueAbilityClick()
          })
      }.backgroundColor(Color.Grey).width('100%').height('10%')
      Flex() {
        Path().commands(this.pathCommands).strokeWidth(4).fill('none').stroke(Color.Black)
          .width('100%')
          .height('100%')
      }.onTouch((event: TouchEvent) => {
        this.onTouchEvent(event)
      }).width('100%').height('90%')
    }.height('100%').width('100%')
  }
// 函数在创建自定义组件的新实例后，在执行其build函数之前执行
  async aboutToAppear() {
     console.info('DrawBoard[IndexPage] aboutToAppear begin');
     this.initialData = []
     let self = this
     await featureAbility.getWant()
       .then((Want) => {
         self.positionList = JSON.parse(Want.parameters.positionList)
         console.info('Operation successful. self.positionList: ' + JSON.stringify(self.positionList.length));
       }).catch((error) => {
       console.error('Operation failed. Cause: ' + JSON.stringify(error));
     })
     /*await this.kvStoreModel.get(CHANGE_POSITION,(data)=>{
       console.info('DrawBoard[IndexPage] aboutToAppear data ='+data)
       self.positionList = JSON.parse(data)
     })*/
     console.info('DrawBoard[IndexPage] aboutToAppear positionList length=' + this.positionList.length);

     if (this.positionList.length > 0) {
      this.positionList.forEach((num) => {
        this.initialData.push(num);
      });
      this.initDraw();
    }

    this.kvStoreModel.setDataChangeListener((data) => {
      self.positionList = [];
      data.updateEntries.forEach((num) => {
        const list = JSON.parse(num.value.value);
        console.info('DrawBoard[IndexPage] setDataChangeListener list=' + JSON.stringify(list))
        if(list.length === 0) {
          console.info('DrawBoard[IndexPage] setDataChangeListener list.length === 0')
          self.pathCommands = DEfAULT_PATH_COMMAND
        } else{
          list.forEach((num) => {
            self.positionList.push(num);
          })
          console.info('DrawBoard[IndexPage] setDataChangeListener positionList=' + JSON.stringify(this.positionList))
        }
        setTimeout(function() {
          self.redraw();
        }, 10);
      });
    });
  }

  // 初始化画板轨迹
  initDraw() {
    const self = this;
    self.pathCommands = ''
    this.intervalID = setInterval(function() {
      if (self.initialData[0].isFirstPosition) {
        self.pathCommands += ' M' + self.initialData[0].positionX + ' ' + self.initialData[0].positionY
        console.info('DrawBoard[IndexPage] initDraw pathCommands=' + self.pathCommands)

      } else {
        self.pathCommands += ' L' + self.initialData[0].positionX + ' ' +  self.initialData[0].positionY
        console.info('DrawBoard[IndexPage] initDraw pathCommands=' + self.pathCommands)
      }
      self.initialData.shift();

      if (self.initialData.length < 1) {
        clearInterval(self.intervalID);
        self.intervalID = 0;
      }
    }, 10);
  }
  // 轨迹重绘制
  redraw() {
    console.info('DrawBoard[IndexPage] redraw positionList= ' + JSON.stringify(this.positionList))
    this.pathCommands = DEfAULT_PATH_COMMAND
    if (this.positionList.length > 0 ) {
      this.positionList.forEach((num) => {
        console.info('DrawBoard[IndexPage] redraw num=' + JSON.stringify(num))
        if (num.isFirstPosition) {
          this.pathCommands += ' M' + num.positionX + ' ' + num.positionY
          console.info('DrawBoard[IndexPage] redraw pathCommands=' + this.pathCommands)
        } else {
          this.pathCommands += ' L' + num.positionX + ' ' + num.positionY
          console.info('DrawBoard[IndexPage] redraw pathCommands=' + this.pathCommands)
        }
      });
    }
  }

  // 撤回上一笔绘制
  goBack() {
    if (this.positionList.length > 0) {
      for (let i = this.positionList.length - 1; i > -1; i--) {
        if (this.positionList[i].isFirstPosition) {
          this.positionList.pop();
          this.redraw();
          break;
        } else {
          this.positionList.pop();
        }
      }
      this.kvStoreModel.put(CHANGE_POSITION, JSON.stringify(this.positionList));
    }
  }

   onContinueAbilityClick() {
      console.info('DrawBoard[IndexPage] onContinueAbilityClick');
      let self = this;
      this.remoteDeviceModel.registerDeviceListCallback(() => {
          console.info('DrawBoard[IndexPage] registerDeviceListCallback, callback entered');
          var list = [];
          list[0] = DEVICE_LIST_LOCALHOST
          var deviceList = self.remoteDeviceModel.deviceList;
          console.info('DrawBoard[IndexPage] on remote device updated, count=' + deviceList.length);
          for (var i = 0; i < deviceList.length; i++) {
              console.info('DrawBoard[IndexPage] device ' + i + '/' + deviceList.length + ' deviceId='
              + deviceList[i].deviceId + ' deviceName=' + deviceList[i].deviceName + ' deviceType='
              + deviceList[i].deviceType);
              list[i + 1] = {
                  name: deviceList[i].deviceName,
                  id: deviceList[i].deviceId,
              };
          }
          self.deviceList = list;
          self.dialogController.open()
      });
  }

   startAbilityContinuation(deviceId: string, deviceName: string,positionList: any[] ) {
        var params = {
          positionList: JSON.stringify(positionList)
        }
        console.info('DrawBoard[IndexPage] featureAbility.startAbility positionList=' + JSON.stringify(positionList))
        console.info('DrawBoard[IndexPage] featureAbility.startAbility deviceId=' + deviceId
        + ' deviceName=' + deviceName);
        var wantValue = {
            bundleName: 'com.huawei.cookbook',
            abilityName: 'com.huawei.distributedatabasedrawetsopenh.MainAbility',
            deviceId: deviceId,
            parameters: params
        };

        featureAbility.startAbility({
            want: wantValue
        }).then((data) => {
            console.info('DrawBoard[IndexPage] featureAbility.startAbility finished, ' + JSON.stringify(data));
        });
        console.info('DrawBoard[IndexPage] featureAbility.startAbility want=' + JSON.stringify(wantValue));
        console.info('DrawBoard[IndexPage] featureAbility.startAbility end');
    }


  onTouchEvent(event: TouchEvent) {
    let position = {};
    switch(event.type){
      case TouchType.Down:
        this.startX = event.touches[0].x
        this.startY = event.touches[0].y
        this.pathCommands += ' M' + this.startX + ' ' + this.startY
        position.isFirstPosition = true;
        position.positionX = this.startX;
        position.positionY = this.startY;
        this.pushData(position);
        break;
      case TouchType.Move:
        this.moveX = event.touches[0].x
        this.moveY = event.touches[0].y
        this.pathCommands += ' L' + this.moveX + ' ' + this.moveY
        position.isFirstPosition = false;
        position.positionX = this.moveX;
        position.positionY = this.moveY;
        this.pushData(position);
        break;
      case TouchType.Up:
        this.endX = event.touches[0].x
        this.endY = event.touches[0].y
        position.isFirstPosition = false;
        position.positionX = this.moveX;
        position.positionY = this.moveY;
        this.pushData(position);
        break;
      default:
        break
    }
  }
    pushData(position: any) {
      this.isNeedSync = true;
      this.positionList.push(position);
      console.info('DrawBoard[IndexPage] pushData positionList 1 =' + JSON.stringify(this.positionList.length));
      let self = this;
      if (this.intervalID === 0) {
        this.intervalID = setInterval(function () {
          if (self.isNeedSync) {
            self.kvStoreModel.put(CHANGE_POSITION, JSON.stringify(self.positionList));
            console.info('DrawBoard[IndexPage] pushData positionList 2 =' + JSON.stringify(this.positionList.length));
            // self.positionList = [];
            self.isNeedSync = false;
          }
        }, 100);
    }
  }
}