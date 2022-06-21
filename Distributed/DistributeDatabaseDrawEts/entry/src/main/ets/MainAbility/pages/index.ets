// @ts-nocheck
import featureAbility from '@ohos.ability.featureAbility';
import KvStoreModel from '../model/KvStoreModel';
import RemoteDeviceModel from '../model/RemoteDeviceModel';
var DEVICE_LIST_LOCALHOST = { name: '本机', id: 'localhost' };
const CHANGE_POSITION = 'change_position';

@CustomDialog
struct CustomDialogExample {
  controller: CustomDialogController
  cancel: () => void
  confirm: (deviceId, deviceName) => void
  startAbility: (deviceId, deviceName, positionList) => void
  deviceList:() => void
  positionList:() => void
  private selectedIndex: number = 0
  build() {
    Column() {
      Text('选择设备')
        .fontSize(20)
        .width('100%')
        .textAlign(TextAlign.Center)
        .fontColor(Color.Black)
        .fontWeight(FontWeight.Bold)
      List() {
        ForEach(this.deviceList, (item, index) => {
          ListItem() {
            Row() {
              Text(item.name)
                .fontSize(20)
                .width('90%')
                .fontColor(Color.Black)
              if (this.deviceList.indexOf(item) == this.selectedIndex) {
                Image($r('app.media.checked'))
                  .width('8%')
                  .objectFit(ImageFit.Contain)
              } else {
                Image($r('app.media.uncheck'))
                  .width('8%')
                  .objectFit(ImageFit.Contain)
              }
            }
            .height(55)
            .onClick(() =>{
              this.selectedIndex = index
              this.controller.close();
              this.startAbility(item.id, item.name, this.positionList)
            })
          }
        }, item => item.id)
      }

      Button() {
        Text('取消')
          .fontColor('#0D9FFB')
          .width('90%')
          .textAlign(TextAlign.Center)
          .fontSize(20)
      }
      .type(ButtonType.Capsule)
      .backgroundColor(Color.White)
      .onClick(() => {
        this.controller.close()
      })
    }
    .backgroundColor(Color.White)
    .border({ color: Color.White, radius: 20 })
    .padding(10)
  }
}

@Entry
@Component
struct Index {
  private startX: number = 0
  private startY: number = 0
  private moveX: number = 0
  private moveY: number = 0
  private endX: number = 0
  private endY: number = 0
  @State deviceList: any[] = []
  private BUNDLE_NAME: string = "com.huawei.cookbook";
  private kvStoreModel: KvStoreModel = new KvStoreModel()
  private remoteDeviceModel: RemoteDeviceModel = new RemoteDeviceModel()
  private positionList: any[] = []
  private initialData: any[] = []
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)

  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExample({ cancel: this.onCancel, confirm: this.onAccept, deviceList: this.deviceList,positionList: this.positionList,startAbility: this.startAbilityContinuation }),
    cancel: this.existApp,
    autoCancel: true,
    deviceList: this.deviceList,
    positionList: this.positionList
  })
  onCancel() {
    console.log('Callback when the first button is clicked')
  }
  onAccept() {
    console.log('Click when confirm')
  }
  existApp() {
    console.log('Click the callback in the blank area')
  }

  build() {
    Column({ space: 1 }) {
      Row() {
        Image($r('app.media.goback')).width(100).height(100).margin({left:10})
          .onClick(() =>{
            this.goBack()
          })
        Blank()
        Image($r('app.media.ic_hop')).width(100).height(100).margin({right:10})
          .onClick(() =>{
            this.onContinueAbilityClick()
          })
      }.backgroundColor(Color.Grey).width('100%').height('10%')
      Row() {
        Canvas(this.context)
          .width('100%')
          .height('100%')
          .backgroundColor('#FFFFFF')
      }.onTouch((event: TouchEvent) => {
        this.onTouchEvent(event)
      }).width('100%').height('90%')
    }.height('100%').width('100%')
  }
  // 函数在创建自定义组件的新实例后，在执行其build函数之前执行
  async aboutToAppear() {
    this.grantPermission()
    console.log('DrawBoard[IndexPage] aboutToAppear begin');
    this.initialData = []
    let self = this
    await featureAbility.getWant()
      .then((Want) => {
        self.positionList = JSON.parse(Want.parameters.positionList)
        console.log('Operation successful. self.positionList: ' + JSON.stringify(self.positionList.length));
      }).catch((error) => {
        console.error('Operation failed. Cause: ' + JSON.stringify(error));
      })
    console.log('DrawBoard[IndexPage] aboutToAppear positionList length=' + self.positionList.length);
    if (self.positionList.length > 0) {
      self.positionList.forEach((num) => {
        self.initialData.push(num);
      });
      console.log('DrawBoard[IndexPage] aboutToAppear initialData='+JSON.stringify(self.initialData))
      self.initDraw();
    }
    console.log('DrawBoard[IndexPage] setDataChangeListener out setDataChangeListener')
    self.kvStoreModel.setDataChangeListener((data) => {
      console.log('DrawBoard[IndexPage] setDataChangeListener come in')
      self.positionList = [];
      data.updateEntries.forEach((num) => {
        const list = JSON.parse(num.value.value);
        console.log('DrawBoard[IndexPage] setDataChangeListener list=' + JSON.stringify(list))
        if(list.length === 0) {
          console.log('DrawBoard[IndexPage] setDataChangeListener list.length === 0')
        } else{
          list.forEach((num) => {
            self.positionList.push(num);
          })
          console.log('DrawBoard[IndexPage] setDataChangeListener positionList=' + JSON.stringify(self.positionList))
        }
        self.redraw();
      });
    });
  }
  grantPermission() {
    console.log('MusicPlayer[IndexPage] grantPermission')
    let context = featureAbility.getContext()
    context.requestPermissionsFromUser(['ohos.permission.DISTRIBUTED_DATASYNC'], 666, function (result) {
      console.log(`MusicPlayer[IndexPage] grantPermission,requestPermissionsFromUser,result.requestCode=${result.requestCode}`)
    })
  }
  // 初始化画板轨迹
  initDraw() {
    this.initialData.forEach((point)=>{
      if(point.isFirstPosition) {
        this.context.beginPath()
        this.context.lineWidth = 4
        this.context.lineJoin = 'miter'
        this.context.moveTo(point.positionX, point.positionY)
      } else{
        this.context.lineTo(point.positionX, point.positionY)
        if(point.isEndPosition) {
          this.context.stroke()
          console.log('DrawBoard[IndexPage] initDraw context.stroke')
        }
      }
    })
  }
  // 轨迹重绘制
  redraw() {
    console.log('DrawBoard[IndexPage] redraw positionList= ' + JSON.stringify(this.positionList))
    this.context.clearRect(0,0, this.context.width,this.context.height)
    if (this.positionList.length > 0 ) {
      this.positionList.forEach((num) => {
        console.log('DrawBoard[IndexPage] redraw num=')
        console.log('DrawBoard[IndexPage] redraw out isFirstPosition=' + num.isFirstPosition)
        if (num.isFirstPosition) {
          console.log('DrawBoard[IndexPage] redraw isFirstPosition=' + num.isFirstPosition)
          this.context.beginPath()
          this.context.lineWidth = 4
          this.context.lineJoin = 'miter'
          this.context.moveTo(num.positionX, num.positionY)
          console.log('DrawBoard[IndexPage] redraw context.moveTo' + num.positionX+','+ num.positionY)
        } else {
          this.context.lineTo(num.positionX, num.positionY)
          console.log('DrawBoard[IndexPage] redraw context.lineTo' + num.positionX+','+ num.positionY)
          if(num.isEndPosition) {
            this.context.stroke()
            console.log('DrawBoard[IndexPage] redraw context.stroke')
          }
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
          break;
        } else {
          this.positionList.pop();
        }
      }
      this.kvStoreModel.put(CHANGE_POSITION, JSON.stringify(this.positionList));
    }
  }

  onContinueAbilityClick() {
    console.log('DrawBoard[IndexPage] onContinueAbilityClick');
    let self = this;
    this.remoteDeviceModel.registerDeviceListCallback(() => {
      console.log('DrawBoard[IndexPage] registerDeviceListCallback, callback entered');
      var list = [];
      list[0] = DEVICE_LIST_LOCALHOST
      var deviceList = self.remoteDeviceModel.deviceList;
      console.log('DrawBoard[IndexPage] on remote device updated, count=' + deviceList.length);
      for (var i = 0; i < deviceList.length; i++) {
        console.log('DrawBoard[IndexPage] device ' + i + '/' + deviceList.length + ' deviceId='
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
    console.log('DrawBoard[IndexPage] featureAbility.startAbility positionList=' + JSON.stringify(positionList))
    console.log('DrawBoard[IndexPage] featureAbility.startAbility deviceId=' + deviceId
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
      console.log('DrawBoard[IndexPage] featureAbility.startAbility finished, ' + JSON.stringify(data));
    });
    console.log('DrawBoard[IndexPage] featureAbility.startAbility want=' + JSON.stringify(wantValue));
    console.log('DrawBoard[IndexPage] featureAbility.startAbility end');
  }


  onTouchEvent(event: TouchEvent) {
    let position = {};
    switch(event.type){
      case TouchType.Down:
        this.startX = event.touches[0].x
        this.startY = event.touches[0].y
        position.isFirstPosition = true;
        position.positionX = this.startX;
        position.positionY = this.startY;
        position.isEndPosition = false
        this.context.beginPath()
        this.context.lineWidth = 4
        this.context.lineJoin = 'miter'
        this.context.moveTo(this.startX, this.startY)
        this.pushData(position);
        break;
      case TouchType.Move:
        this.moveX = event.touches[0].x
        this.moveY = event.touches[0].y
        position.isFirstPosition = false;
        position.positionX = this.moveX;
        position.positionY = this.moveY;
        position.isEndPosition = false
        this.context.lineTo(this.moveX, this.moveY)
        this.pushData(position);
        break;
      case TouchType.Up:
        this.endX = event.touches[0].x
        this.endY = event.touches[0].y
        position.isFirstPosition = false;
        position.positionX = this.endX;
        position.positionY = this.endY;
        position.isEndPosition = true
        this.context.stroke()
        this.pushData(position);
        break;
      default:
        break;
    }
  }
  pushData(position: any) {
    this.positionList.push(position);
    console.log('DrawBoard[IndexPage] pushData positionList 1 =' + JSON.stringify(this.positionList.length));
    if(position.isEndPosition){
      this.kvStoreModel.put(CHANGE_POSITION, JSON.stringify(this.positionList));
      console.log('DrawBoard[IndexPage] pushData positionList 2 =' + JSON.stringify(this.positionList.length));
    }
  }
}