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

import { DeviceBean } from '../common/bean/DeviceBean';
import { DeviceListUtil }  from '../common/util/DeviceListUtil';
import { DeviceItem } from './DeviceItem';
import { CommonConstants } from '../common/constants/CommonConstants';

@CustomDialog
export struct DeviceListDialog {
  private deviceListUtil: DeviceListUtil = new DeviceListUtil();
  @State deviceList: Array<DeviceBean> = [];
  controller: CustomDialogController;

  aboutToAppear() {
    this.deviceListUtil.initDmInstance((data) => {
      this.deviceList.push(new DeviceBean(this.deviceListUtil.dealDeviceId(data.device.deviceId),
        data.device.deviceName));
      const map = new Map();
      this.deviceList = this.deviceList.filter(
        (item: DeviceBean) => (!map.has(item.deviceId) && map.set(item.deviceId, 1)));
    });
  }

  build() {
    Column() {
      Text($r('app.string.device_list'))
        .width(CommonConstants.DEVICE_TEXT_WIDTH)
        .fontSize($r('app.float.device_text_size'))
        .textAlign(TextAlign.Start)
        .fontColor(Color.Black)
        .fontWeight(FontWeight.Bold)
        .margin({ top: $r('app.float.device_text_top') })
      Column() {
        if (this.deviceList.length === 0) {
          Text($r('app.string.no_device'))
            .width(CommonConstants.DEVICE_TEXT_WIDTH)
            .fontSize($r('app.float.device_text_size'))
            .fontColor(Color.Black)
            .textAlign(TextAlign.Start)
            .fontWeight(FontWeight.Normal)
        } else {
          Column() {
            List() {
              ForEach(this.deviceList, (item: DeviceBean, index: number) => {
                ListItem() {
                  DeviceItem({item: item, index: index});
                }
              }, item => JSON.stringify(item))
            }
            .width(CommonConstants.FULL_PERCENT)
            .height(this.deviceList.length > 1 ?
              CommonConstants.LIST_MAX_HEIGHT: CommonConstants.LIST_MIN_HEIGHT)
          }
          .width(CommonConstants.COLUMN_WIDTH)
        }
      }
      .width(CommonConstants.FULL_PERCENT)
      .margin({ top: $r('app.float.column_top') })
      Text($r('app.string.dialog_confirm'))
        .fontSize($r('app.float.device_text_size'))
        .fontColor($r('app.color.device_text_color'))
        .margin({
          top: $r('app.float.dialog_text_confirm_top'),
          bottom: $r('app.float.dialog_text_confirm_bottom')
        })
        .onClick(() => {
          this.deviceListUtil.stopDeviceDiscovery();
          this.controller.close();
        })
    }
    .width(CommonConstants.FULL_PERCENT)
    .backgroundColor(Color.White)
    .borderRadius($r('app.float.border_radius'))
  }
}