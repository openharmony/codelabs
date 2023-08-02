/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import common from '@ohos.app.ability.common';
import distributedKVStore from '@ohos.data.distributedKVStore';
import deviceManager from '@ohos.distributedHardware.deviceManager';
import CommonConstants from '../common/constants/CommonConstants';
import Position from '../common/bean/Position';
import KvStoreModel from '../viewmodel/KvStoreModel';
import remoteDeviceModel from '../common/utils/RemoteDeviceUtil';
import DeviceListDialogComponent from '../view/CustomDialogComponent';
import Logger from '../common/utils/Logger';

let storage = LocalStorage.getShared();

@Entry(storage)
@Component
struct Index {
  @StorageLink('deviceList') deviceList: deviceManager.DeviceInfo[] = [];
  @LocalStorageProp('positionList') positionList: Position[] = [];
  @LocalStorageProp('updateCanvas') @Watch('updateCanvas') update: boolean = false;
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private canvasContext: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private kvStoreModel: KvStoreModel = new KvStoreModel();
  private context: common.UIAbilityContext = null;
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

  aboutToAppear() {
    Logger.info('Index', 'aboutToAppear begin');
    this.createKVStore();
  }

  aboutToDisappear() {
    this.kvStoreModel.removeDataChangeListener();
  }

  /**
   * Callback for closing a pop-up window.
   */
  onCancel(): void {
    remoteDeviceModel.stopDeviceDiscovery();
  }

  /**
   * Update canvas.
   */
  updateCanvas(): void {
    this.redraw();
  }

  build() {
    Column() {
      Row() {
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

      Row() {
        Canvas(this.canvasContext)
          .width(CommonConstants.FULL_PERCENT)
          .height(CommonConstants.FULL_PERCENT)
          .backgroundColor($r('app.color.start_window_background'))
          .onReady(() => {
            this.redraw();
          })
      }
      .onTouch((event: TouchEvent) => {
        this.onTouchEvent(event);
      })
      .width(CommonConstants.FULL_PERCENT)
      .layoutWeight(CommonConstants.NUMBER_ONE)
    }
    .height(CommonConstants.FULL_PERCENT)
    .width(CommonConstants.FULL_PERCENT)
  }

  /**
   * Create a distributed key-value database.
   */
  createKVStore(): void {
    this.context = getContext(this) as common.UIAbilityContext;
    this.kvStoreModel.createKvStore(this.context, (data: distributedKVStore.ChangeNotification) => {
      this.positionList = [];
      let entries: distributedKVStore.Entry[] = data.insertEntries.length > 0 ? data.insertEntries : data.updateEntries;
      entries.forEach((entry: distributedKVStore.Entry) => {
        if (CommonConstants.CHANGE_POSITION === entry.key) {
          this.positionList = JSON.parse((entry.value.value) as string);
          this.redraw();
        }
      });
    });
  }

  onPageShow(): void {
    this.redraw();
  }

  /**
   * Redraw the track.
   */
  redraw(): void {
    this.canvasContext.clearRect(0, 0, this.canvasContext.width, this.canvasContext.height);
    this.positionList.forEach((position) => {
      Logger.info('Index', `redraw position=${JSON.stringify(position)}`);
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
    });
  }

  /**
   * Retract the previous stroke.
   */
  goBack(): void {
    if (this.positionList.length === 0) {
      return;
    }

    for (let i: number = this.positionList.length - 1; i >= 0; i--) {
      let position: Position = this.positionList.pop();
      if (position.isFirstPosition) {
        break;
      }
    }
    this.redraw();
    this.kvStoreModel.put(CommonConstants.CHANGE_POSITION, JSON.stringify(this.positionList));
  }

  /**
   * Draw events.
   *
   * @param event Touch event.
   */
  onTouchEvent(event: TouchEvent): void {
    let positionX:number = event.touches[0].x;
    let positionY:number = event.touches[0].y;
    switch (event.type) {
      case TouchType.Down: {
        this.canvasContext.beginPath();
        this.canvasContext.lineWidth = CommonConstants.CANVAS_LINE_WIDTH;
        this.canvasContext.lineJoin = CommonConstants.CANVAS_LINE_JOIN;
        this.canvasContext.moveTo(positionX, positionY);
        this.pushData(true, false, positionX, positionY);
        break;
      }
      case TouchType.Move: {
        this.canvasContext.lineTo(positionX, positionY);
        this.pushData(false, false, positionX, positionY);
        break;
      }
      case TouchType.Up: {
        this.canvasContext.lineTo(positionX, positionY);
        this.canvasContext.stroke();
        this.pushData(false, true, positionX, positionY);
        break;
      }
      default: {
        break;
      }
    }
  }

  /**
   * Add position to position list.
   *
   * @param isFirstPosition Indicates whether the position is the start position of the track.
   * @param isEndPosition Indicates whether the position is the end position of the track.
   * @param positionX Horizontal coordinate information.
   * @param positionY Indicates the vertical coordinate information.
   */
  pushData(isFirstPosition: boolean, isEndPosition, positionX: number, positionY: number): void {
    let position = new Position(isFirstPosition, isEndPosition, positionX, positionY);
    this.positionList.push(position);
    if (position.isEndPosition) {
      this.kvStoreModel.put(CommonConstants.CHANGE_POSITION, JSON.stringify(this.positionList));
    }
  }

  /**
   * Click the transfer button to display nearby devices and open the pop-up window.
   */
  onContinueAbilityClick(): void {
    remoteDeviceModel.startDeviceDiscovery();
    this.dialogController.open();
  }

  /**
   * Select a device from the list in the pop-up window.
   * @param context Ability context.
   * @param device Device information.
   * @param positionList Position list.
   */
  startAbilityContinuation(
    context: common.UIAbilityContext,
    device: deviceManager.DeviceInfo,
    positionList: Position[]
  ): void {
    remoteDeviceModel.authenticateDevice(context, device, positionList);
    remoteDeviceModel.stopDeviceDiscovery();
  }
}