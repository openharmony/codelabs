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
import deviceManager from '@ohos.distributedHardware.deviceManager';
import prompt from '@ohos.promptAction';
import CommonConstants from '../common/constants/CommonConstants';
import Position from '../viewmodel/Position';

@Extend(Text)
function ButtonTextStyle() {
  .fontColor($r('app.color.button_text_color'))
  .fontSize($r('app.float.button_font_size'))
  .fontWeight(CommonConstants.FONT_WEIGHT_500)
}

@CustomDialog
export default struct DeviceListDialogComponent {
  @State selectedIndex: number = CommonConstants.INVALID_INDEX;
  @Link positionList: Position[];
  @Link deviceList: deviceManager.DeviceInfo[];
  controller?: CustomDialogController;
  startAbility: (context: common.UIAbilityContext, device: deviceManager.DeviceInfo, positionList: Position[]) => void
    = () => {};
  cancel: () => void = () => {};

  build() {
    Column() {
      Row() {
        Text($r('app.string.select_device'))
          .fontSize($r('app.float.dialog_title_font_size'))
          .width(CommonConstants.FULL_PERCENT)
          .textAlign(TextAlign.Start)
          .fontColor(Color.Black)
          .fontWeight(FontWeight.Bold)
      }
      .padding({
        left: $r('app.float.dialog_title_padding_left'),
        right: $r('app.float.dialog_title_padding_left'),
        top: $r('app.float.dialog_title_padding_top')
      })
      .height($r('app.float.dialog_title_height'))

      List() {
        ForEach(this.deviceList, (item: deviceManager.DeviceInfo, index: number) => {
          ListItem() {
            Column() {
              Row() {
                Row() {
                  Row() {
                    Image(this.getDeviceTypeIcon(item.deviceType))
                      .opacity($r('app.float.device_icon_opacity'))
                  }
                  .width($r('app.float.device_icon_width'))
                  .aspectRatio(CommonConstants.NUMBER_ONE)

                  Text(item.deviceName)
                    .fontSize($r('app.float.device_name_font_size'))
                    .width(CommonConstants.DEVICE_NAME_WIDTH)
                    .fontColor(Color.Black)
                    .margin({ left: $r('app.float.device_name_margin_left') })
                    .maxLines(CommonConstants.NUMBER_ONE)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })
                    .textAlign(TextAlign.Start)
                }
                .justifyContent(FlexAlign.Start)

                if (index === this.selectedIndex) {
                  Image($r('app.media.ic_checked'))
                    .width(CommonConstants.SELECT_ICON_WIDTH)
                    .objectFit(ImageFit.Contain)
                } else {
                  Image($r('app.media.ic_uncheck'))
                    .width(CommonConstants.SELECT_ICON_WIDTH)
                    .objectFit(ImageFit.Contain)
                    .opacity($r('app.float.icon_uncheck_opacity'))
                }
              }
              .height($r('app.float.device_info_height'))
              .onClick(() => {
                this.selectedIndex = index;
              })
              .padding({
                left: $r('app.float.device_info_padding'),
                right: $r('app.float.device_info_padding')
              })
              .width(CommonConstants.FULL_PERCENT)
              .justifyContent(FlexAlign.SpaceBetween)

              if (index !== this.deviceList.length - 1) {
                Row() {
                  Divider()
                    .width(CommonConstants.FULL_PERCENT)
                    .height($r('app.float.device_info_divider_height'))
                    .opacity($r('app.float.device_info_divider_opacity'))
                }
                .padding({
                  left: $r('app.float.device_info_divider_padding_left'),
                  right: $r('app.float.device_info_divider_padding_right')
                })
                .width(CommonConstants.FULL_PERCENT)
              }
            }
          }
        }, (item: deviceManager.DeviceInfo) => item.deviceId)
      }
      .width(CommonConstants.FULL_PERCENT)

      Row() {
        Column() {
          Text($r('app.string.cancel'))
            .ButtonTextStyle()
        }
        .layoutWeight(CommonConstants.NUMBER_ONE)
        .justifyContent(FlexAlign.Center)
        .height($r('app.float.button_line_height'))
        .onClick(() => {
          if (this.controller !== undefined) {
            this.controller.close();
          }
          this.cancel();
        })

        Divider()
          .vertical(true)
          .height($r('app.float.button_line_divider_height'))
          .color($r('app.color.button_line_divider_color'))
          .width($r('app.float.button_line_divider_width'))

        Column() {
          Text($r('app.string.confirm'))
            .ButtonTextStyle()
        }
        .layoutWeight(CommonConstants.NUMBER_ONE)
        .justifyContent(FlexAlign.Center)
        .height($r('app.float.button_line_height'))
        .onClick(() => {
          if (CommonConstants.INVALID_INDEX === this.selectedIndex) {
            prompt.showToast({
              message: $r('app.string.please_select_device')
            });
          } else {
            if (this.controller !== undefined) {
              this.controller.close();
            }
            this.startAbility(getContext(this) as common.UIAbilityContext, this.deviceList[this.selectedIndex],
              this.positionList);
          }
        })
      }
      .backgroundColor(Color.White)
      .border({
        color: Color.White,
        radius: $r('app.float.button_line_radius')
      })
      .padding($r('app.float.button_line_padding'))
      .margin({ top: $r("app.float.button_line_margin_top") })
    }
  }

  /**
   * Obtains the device type icon.
   *
   * @param deviceType Device type.
   * @returns Icon resource.
   */
  getDeviceTypeIcon(deviceType: deviceManager.DeviceType): Resource {
    if ((deviceType === deviceManager.DeviceType.PHONE) || (deviceType === deviceManager.DeviceType.UNKNOWN_TYPE)) {
      return $r('app.media.ic_public_devices_phone');
    } else if (deviceType === deviceManager.DeviceType.TV) {
      return $r('app.media.ic_smartscreen');
    } else {
      return $r('app.media.ic_smartscreen');
    }
  }
}