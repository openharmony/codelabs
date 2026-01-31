import common from '@ohos.app.ability.common';
import CommonConstants from '../common/constants/CommonConstants'
import deviceManager from '@ohos.distributedDeviceManager';
import Logger from '../common/utils/Logger';
import KvStoreModel from '../viewmodel/KvStoreModel';
import distributedKVStore from '@ohos.data.distributedKVStore';
import DeviceListDialogComponent from '../view/CustomDialogComponent';
import remoteDeviceModel from '../common/utils/RemoteDeviceUtil';

import Position from '../viewmodel/Position';

let storage = LocalStorage.getShared();

@Entry(storage)
@Component
struct Index {

  @StorageLink('deviceList') deviceList: deviceManager.DeviceBasicInfo[] = [];
  @LocalStorageProp('positionList') positionList: Position[] = [];
  @LocalStorageProp('updateCanvas') @Watch('updateCanvas') update: boolean = false;
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private canvasContext: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  //分布式数据库
  private kvStoreModel: KvStoreModel = new KvStoreModel();
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;
  //弹出层
  private dialogController: CustomDialogController = new CustomDialogController({
    builder: DeviceListDialogComponent({
      startAbility: this.startAbilityContinuation,
      deviceList: this.deviceList,
      positionList: this.positionList,
      cancel: this.onCancel
    }),
    alignment: DialogAlignment.Bottom,
    cancel: this.onCancel,
    autoCancel: true,
    offset: {
      dx: $r('app.float.dialog_offset_dx'),
      dy: $r('app.float.dialog_offset_dy')
    }
  });

  //创建分布式数据库
  aboutToAppear() {
    Logger.info('Index', 'aboutToAppear begin');
    this.createKVStore();
  }

  // 应用退出时，分布式键值数据库取消数据改变监听。
  aboutToDisappear() {
    this.kvStoreModel.removeDataChangeListener();
  }

  onCancel(): void {
    remoteDeviceModel.stopDeviceDiscovery();
  }
  updateCanvas(): void {
    this.redraw();
  }



  build() {
    Column() {
      Row() {
        //撤回按钮
        Image($r('app.media.ic_back'))
          .width($r('app.float.ic_back_width'))
          .height($r('app.float.ic_back_height'))
          .margin({ left: CommonConstants.ICON_MARGIN_LEFT })
          .onClick(() => {
            this.goBack();
          })
        Blank()
        Image($r('app.media.ic_hop'))
          .width($r('app.float.ic_hop_width'))
          .height($r('app.float.ic_hop_height'))
          .margin({ right: CommonConstants.ICON_MARGIN_LEFT })
          .onClick(() => {
            this.onContinueAbilityClick();
          })
      }
      .width(CommonConstants.FULL_PERCENT)
      .height(CommonConstants.TITLE_HEIGHT)

      //画布
      Row() {
        Canvas(this.canvasContext)
          .width('100%')
          .height('100%')
          .backgroundColor('#FFFFFF')
          .onReady(() => {
            this.redraw();
          })
      }
      .onTouch((event: TouchEvent) => {
        this.onTouchEvent(event);
      })
      .width('100%')
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
  }

  /**
   * 创建分布式数据库
   *
   */
  createKVStore(): void {
    //创建分布式键值（key - value）数据库
    this.kvStoreModel.createKvStore(this.context, (data: distributedKVStore.ChangeNotification) => {
      // 使用分布式键值数据库内的内容重置位置信息列表
      this.positionList = [];
      let entries: distributedKVStore.Entry[] = data.insertEntries.length > 0 ? data.insertEntries : data.updateEntries;
      entries.forEach((entry: distributedKVStore.Entry) => {
        if (CommonConstants.CHANGE_POSITION === entry.key) {
          this.positionList = JSON.parse((entry.value.value) as string);
          // 位置信息列表更新后，重新绘制
          this.redraw();
        }
      });
    });
  }


  goBack(): void {
    if (this.positionList.length === 0) {
      return;
    }
    //移除位置信息   知道回到起始位置
    for (let i: number = this.positionList.length - 1; i >= 0; i--) {
      let position: Position | undefined = this.positionList.pop();
      if (position !== undefined && position.isFirstPosition) {
        break;
      }
    }
    this.redraw();
    //分布式数据库
    this.kvStoreModel.put(CommonConstants.CHANGE_POSITION, JSON.stringify(this.positionList));
  }

  //打开弹出层，显示附近设备
  onContinueAbilityClick(): void {
    //发现附近设备
    remoteDeviceModel.startDeviceDiscovery();
    // 打开弹出层
    this.dialogController.open();
  }

  //选择一个设备
  startAbilityContinuation(
    context: common.UIAbilityContext,
    device: deviceManager.DeviceBasicInfo,
    positionList: Position[]
  ): void {
    remoteDeviceModel.authenticateDevice(context, device, positionList);
    remoteDeviceModel.stopDeviceDiscovery();
  }

  redraw(): void {
    console.log('huabu')
    //清楚绘制内容
    this.canvasContext.clearRect(0, 0, this.canvasContext.width, this.canvasContext.height);
    //使用当前记录的位置信心重新绘制
    this.positionList.forEach((position) => {
      Logger.info('index', `redraw position=${JSON.stringify(position)}`);
      if (position.isFirstPosition) {
        this.canvasContext.beginPath();
        this.canvasContext.lineWidth = CommonConstants.CANVAS_LINE_WIDTH;
        this.canvasContext.lineJoin = CommonConstants.CANVAS_LINE_JOIN;
        this.canvasContext.moveTo(position.positionX, position.positionY);
      } else {
        this.canvasContext.lineTo(position.positionX, position.positionY);
        if (position.isEndPosition) {
          this.canvasContext.stroke();
        }
      }
    })
  }

  onTouchEvent(event: TouchEvent): void {
    let positionX: number = event.touches[0].x;
    let positionY: number = event.touches[0].y;
    switch (event.type) {
    //按下
      case TouchType.Down: {
        this.canvasContext.beginPath();
        this.canvasContext.lineWidth = CommonConstants.CANVAS_LINE_WIDTH;
        this.canvasContext.lineJoin = CommonConstants.CANVAS_LINE_JOIN;
        this.canvasContext.moveTo(positionX, positionY);
        this.canvasContext.stroke();
        this.pushData(true, false, positionX, positionY);
        break;
      }
    //手指移动
      case TouchType.Move: {
        this.canvasContext.lineTo(positionX, positionY);
        this.pushData(false, false, positionX, positionY);
        this.canvasContext.stroke();
        break;
      }
    //手指抬起   终点位置
      case TouchType.Up: {
        this.canvasContext.lineTo(positionX, positionY);
        this.canvasContext.stroke();

        this.pushData(false, true, positionX, positionY)
        break;
      }
      default:
        break;
    }
  }

  pushData(isFirstPosition: boolean, isEndPosition: boolean, positionX: number, positionY: number): void {
    let position = new Position(isFirstPosition, isEndPosition, positionX, positionY);
    //存位置信息
    this.positionList.push(position);
    //当点为终点时，将数据存入分布式数据库
    if (position.isEndPosition) {
      this.kvStoreModel.put(CommonConstants.CHANGE_POSITION, JSON.stringify(this.positionList));
    }

  }
}