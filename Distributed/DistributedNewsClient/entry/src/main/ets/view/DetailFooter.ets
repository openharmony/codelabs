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

import deviceManager from '@ohos.distributedHardware.deviceManager';
import { NewsData } from '../common/bean/NewsData';
import CommonConstants from '../common/constants/CommonConstants';
import RemoteDeviceModel from '../model/RemoteDeviceModel';
import { DeviceListDialog } from './DeviceListDialog';

@Extend(Image) function imageStyle() {
  .width($r('app.float.footer_icon_size'))
  .height($r('app.float.footer_icon_size'))
}

@Component
export default struct DetailFooter {
  @StorageLink('discoverDeviceList') discoverDeviceList: deviceManager.DeviceInfo[] = [];
  @StorageLink('trustedDeviceList') trustedDeviceList: deviceManager.DeviceInfo[] = [];
  @Consume newsData: NewsData;
  @Provide currentBreakpoint: string = CommonConstants.BREAKPOINT_SM;
  private dialogController: CustomDialogController;

  build() {
    Column() {
      Divider()
        .color($r('app.color.detail_divider_color'))
        .width(CommonConstants.FULL_COMPONENT)

      GridRow({
        columns: {
          sm: CommonConstants.FOUR_COLUMN,
          md: CommonConstants.EIGHT_COLUMN,
          lg: CommonConstants.TWELVE_COLUMN
        },
        breakpoints: {
          value: [
          CommonConstants.SMALL_DEVICE_TYPE,
          CommonConstants.MIDDLE_DEVICE_TYPE,
          CommonConstants.LARGE_DEVICE_TYPE
          ]
        },
        gutter: { x: $r('app.float.grid_row_gutter') }
      }) {
        GridCol({
          span: {
            sm: CommonConstants.FOUR_COLUMN,
            md: CommonConstants.EIGHT_COLUMN,
            lg: CommonConstants.EIGHT_COLUMN
          },
          offset: {
            sm: CommonConstants.ZERO_COLUMN,
            md: CommonConstants.ZERO_COLUMN,
            lg: CommonConstants.TWO_COLUMN
          }
        }) {
          Row() {
            Row() {
              TextInput({ placeholder: $r('app.string.write_note') })
                .height($r('app.float.text_input_height'))
                .backgroundColor($r('app.color.white'))
                .layoutWeight(1)
                .fontSize($r('app.float.normal_font_size'))
                .placeholderFont({ weight: FontWeight.Normal })
                .placeholderColor($r('app.color.font_color_gray'))
                .enableKeyboardOnFocus(false)

              Image($r('app.media.ic_icon_message'))
                .imageStyle()
                .margin({
                  right: $r('app.float.message_margin_right'),
                  left: $r('app.float.message_margin_left')
                })
            }
            .height($r('app.float.text_input_height'))
            .layoutWeight(1)
            .borderRadius($r('app.float.text_input_border_radius'))
            .backgroundColor($r('app.color.white'))

            Column() {
              Image($r('app.media.ic_icon_good'))
                .imageStyle()
            }
            .width($r('app.float.footer_icon_column_width'))
            .height(CommonConstants.FULL_COMPONENT)
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)

            Column() {
              Image($r('app.media.ic_icon_star'))
                .imageStyle()
            }
            .width($r('app.float.footer_icon_column_width'))
            .height(CommonConstants.FULL_COMPONENT)
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)

            Column() {
              Image($r('app.media.ic_icon_share'))
                .imageStyle()
                .onClick(() => {
                  // Discover devices.
                  RemoteDeviceModel.startDeviceDiscovery();
                  this.dialogController.open();
                })
            }
            .width($r('app.float.footer_icon_column_width'))
            .height(CommonConstants.FULL_COMPONENT)
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)
          }
          .height($r('app.float.footer_height'))
        }
        .margin({
          left: this.currentBreakpoint === CommonConstants.BREAKPOINT_SM ? $r('app.float.footer_margin_sm') :
            $r('app.float.footer_margin_other'),
          right: this.currentBreakpoint === CommonConstants.BREAKPOINT_SM ? $r('app.float.footer_margin_sm') :
            $r('app.float.footer_margin_other')
        })
      }
      .backgroundColor($r('app.color.bg_color_gray'))
      .height($r('app.float.footer_height'))
      .width(CommonConstants.FULL_COMPONENT)
      .onBreakpointChange((breakpoints) => {
        this.currentBreakpoint = breakpoints;
        this.dialogController = new CustomDialogController({
          builder: DeviceListDialog({
            confirm: (device: deviceManager.DeviceInfo) => {
              // Connect the device.
              RemoteDeviceModel.authenticateDevice(device, globalThis.context);
              RemoteDeviceModel.stopDeviceDiscovery();
            }
          }),
          cancel: () => {
            RemoteDeviceModel.stopDeviceDiscovery();
          },
          autoCancel: true,
          gridCount: CommonConstants.FOUR_COLUMN,
          alignment: this.currentBreakpoint === CommonConstants.BREAKPOINT_SM ? DialogAlignment.Bottom :
            DialogAlignment.Center,
          offset: {
            dx: 0,
            dy: this.currentBreakpoint === CommonConstants.BREAKPOINT_SM ? $r('app.float.dialog_offset_y') : 0
          }
        });
      })
    }
  }
}