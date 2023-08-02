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
import Want from '@ohos.app.ability.Want';
import prompt from '@ohos.promptAction';
import common from '@ohos.app.ability.common';
import CommonConstants from '../common/constants/CommonConstants';
import { NewsData } from '../common/bean/NewsData';
import RemoteDeviceModel from '../model/RemoteDeviceModel';
import Logger from '../common/utils/Logger';

@Extend(Text) function textStyle() {
  .fontColor($r('app.color.dialog_button_font'))
  .fontSize($r('app.float.normal_font_size'))
  .fontWeight(CommonConstants.FONT_WEIGHT_500)
}

@CustomDialog

export struct DeviceListDialog {
  @StorageLink('discoverDeviceList') discoverDeviceList: deviceManager.DeviceInfo[] = [];
  @StorageLink('trustedDeviceList') trustedDeviceList: deviceManager.DeviceInfo[] = [];
  @Consume newsData: NewsData;
  private selectedDevice: deviceManager.DeviceInfo;
  private confirm: (device: deviceManager.DeviceInfo) => void;
  private isTrusted: boolean;
  private controller: CustomDialogController;
  private context: common.UIAbilityContext;

  aboutToAppear() {
    this.context = getContext(this) as common.UIAbilityContext;
  }

  getDeviceTypeIcon(deviceType): Resource {
    if (deviceType === deviceManager.DeviceType.TABLET) {
      return $r('app.media.ic_pad');
    } else if (deviceType === deviceManager.DeviceType.PHONE) {
      return $r('app.media.ic_phone');
    } else if (deviceType === deviceManager.DeviceType.UNKNOWN_TYPE) {
      return $r('app.media.ic_pc');
    } else {
      return $r('app.media.ic_pc');
    }
  }

  build() {
    Column() {
      Row() {
        Text($r('app.string.trusted_device'))
          .fontWeight(CommonConstants.FONT_WEIGHT_500)
          .fontSize($r('app.float.small_font_size'))
          .fontColor($r('app.color.font_color_gray'))
          .lineHeight($r('app.float.choose_device_line_height'))
          .margin({
            top: $r('app.float.choose_device_margin_top'),
            bottom: $r('app.float.dialog_icon_margin_bottom')
          })
          .width(CommonConstants.FULL_COMPONENT)
      }
      .height($r('app.float.choose_device_row_height'))
      .width(CommonConstants.FULL_COMPONENT)
      .padding({
        left: $r('app.float.dialog_padding'),
        right: $r('app.float.dialog_padding')
      })

      List() {
        ForEach(this.trustedDeviceList, (item: deviceManager.DeviceInfo, index: number) => {
          ListItem() {
            Column() {
              Row() {
                Image(this.getDeviceTypeIcon(item.deviceType))
                  .width($r('app.float.dialog_icon_size'))
                  .height($r('app.float.dialog_icon_size'))
                  .margin({ right: $r('app.float.dialog_icon_margin_right') })

                Column() {
                  Row() {
                    Text(item.deviceName)
                      .fontSize($r('app.float.normal_font_size'))
                      .lineHeight($r('app.float.device_list_line_height'))
                      .layoutWeight(CommonConstants.ONE)
                      .fontWeight(CommonConstants.FONT_WEIGHT_500)
                      .textOverflow({ overflow: TextOverflow.Ellipsis })

                    Radio({ value: item.deviceName, group: this.context.applicationInfo.name })
                      .size({
                        width: $r('app.float.dialog_radio_size'),
                        height: $r('app.float.dialog_radio_size')
                      })
                      .margin({ left: $r('app.float.radio_margin_left') })
                      .onChange((isChecked: boolean) => {
                        if (isChecked) {
                          this.selectedDevice = item;
                          this.isTrusted = true;
                        } else {
                          this.selectedDevice = null;
                          this.isTrusted = null;
                        }
                      })
                  }
                  .width(CommonConstants.FULL_COMPONENT)
                  .height($r('app.float.device_list_column_height'))

                  Divider()
                    .opacity(CommonConstants.DIVIDER_OPACITY)
                    .color($r('app.color.list_divider'))
                    .width(CommonConstants.FULL_COMPONENT)
                    .visibility(index === this.trustedDeviceList.length - 1 ? Visibility.None : Visibility.Visible)
                }
                .layoutWeight(CommonConstants.ONE)
              }
              .width(CommonConstants.FULL_COMPONENT)
              .height($r('app.float.device_list_row_height'))
              .justifyContent(FlexAlign.Start)
              .alignItems(VerticalAlign.Center)
            }
            .padding({
              left: $r('app.float.dialog_padding'),
              right: $r('app.float.dialog_padding')
            })
            .width(CommonConstants.FULL_COMPONENT)
          }
        }, item => item.deviceId)
      }

      Row() {
        Text($r('app.string.select_device'))
          .fontWeight(CommonConstants.FONT_WEIGHT_500)
          .fontSize($r('app.float.small_font_size'))
          .fontColor($r('app.color.font_color_gray'))
          .lineHeight($r('app.float.choose_device_line_height'))
          .margin({
            top: $r('app.float.choose_device_margin_top'),
            bottom: $r('app.float.dialog_icon_margin_bottom')
          })
          .width(CommonConstants.FULL_COMPONENT)
      }
      .height($r('app.float.choose_device_row_height'))
      .width(CommonConstants.FULL_COMPONENT)
      .padding({
        left: $r('app.float.dialog_padding'),
        right: $r('app.float.dialog_padding')
      })

      List() {
        ForEach(this.discoverDeviceList, (item: deviceManager.DeviceInfo, index: number) => {
          ListItem() {
            Column() {
              Row() {
                Image(this.getDeviceTypeIcon(item.deviceType))
                  .width($r('app.float.dialog_icon_size'))
                  .height($r('app.float.dialog_icon_size'))
                  .margin({ right: $r('app.float.dialog_icon_margin_right') })

                Column() {
                  Row() {
                    Text(item.deviceName)
                      .fontSize($r('app.float.normal_font_size'))
                      .lineHeight($r('app.float.device_list_line_height'))
                      .layoutWeight(CommonConstants.ONE)
                      .fontWeight(CommonConstants.FONT_WEIGHT_500)
                      .textOverflow({ overflow: TextOverflow.Ellipsis })

                    Radio({ value: item.deviceName, group: this.context.applicationInfo.name })
                      .size({
                        width: $r('app.float.dialog_radio_size'),
                        height: $r('app.float.dialog_radio_size')
                      })
                      .onChange((isChecked: boolean) => {
                        if (isChecked) {
                          this.selectedDevice = item;
                          this.isTrusted = false;
                        } else {
                          this.selectedDevice = null;
                          this.isTrusted = null;
                        }
                      })
                      .margin({ left: $r('app.float.radio_margin_left') })
                  }
                  .layoutWeight(CommonConstants.ONE)
                  .height($r('app.float.device_list_column_height'))

                  Divider()
                    .opacity(CommonConstants.DIVIDER_OPACITY)
                    .color($r('app.color.list_divider'))
                    .width(CommonConstants.FULL_COMPONENT)
                    .visibility(index === this.discoverDeviceList.length - 1 ? Visibility.None : Visibility.Visible)
                }
                .layoutWeight(CommonConstants.ONE)
              }
              .width(CommonConstants.FULL_COMPONENT)
              .height($r('app.float.device_list_row_height'))
              .justifyContent(FlexAlign.Start)
              .alignItems(VerticalAlign.Center)
            }
            .padding({
              left: $r('app.float.dialog_padding'),
              right: $r('app.float.dialog_padding')
            })
            .width(CommonConstants.FULL_COMPONENT)
          }
        }, item => item.deviceId)
      }

      Row() {
        Column() {
          Text($r('app.string.cancel'))
            .textStyle()
            .onClick(() => {
              RemoteDeviceModel.stopDeviceDiscovery();
              this.controller.close();
            })
        }
        .layoutWeight(CommonConstants.ONE)
        .justifyContent(FlexAlign.Center)
        .height($r('app.float.dialog_button_height'))

        Divider()
          .opacity(CommonConstants.DIVIDER_OPACITY)
          .color($r('app.color.list_divider'))
          .vertical(true)
          .height($r('app.float.dialog_button_height'))

        Column() {
          Text($r('app.string.ok'))
            .textStyle()
            .onClick(() => {
              if ((this.selectedDevice === null)) {
                prompt.showToast({
                  message: $r('app.string.please_select_device')
                });
                return;
              }
              if (this.isTrusted === false) {
                // Connect the device.
                this.confirm(this.selectedDevice);
                this.controller.close();
              } else {
                // Start the app.
                startAbilityContinuation(this.selectedDevice.networkId, this.newsData.newsId, this.context);
                this.controller.close();
              }
            })
        }
        .layoutWeight(CommonConstants.ONE)
        .justifyContent(FlexAlign.Center)
        .height($r('app.float.dialog_button_height'))
      }
      .height($r('app.float.dialog_button_row_height'))
      .padding({
        top: $r('app.float.dialog_button_padding_top'),
        bottom: $r('app.float.dialog_button_padding_bottom'),
        left: $r('app.float.dialog_padding'),
        right: $r('app.float.dialog_padding')
      })
      .width(CommonConstants.FULL_COMPONENT)
    }
    .borderRadius($r('app.float.dialog_border_radius'))
    .backgroundColor($r('app.color.device_dialog_background'))
    .width(CommonConstants.FULL_COMPONENT)
  }
}

function startAbilityContinuation(deviceId: string, newsId: string, context: common.UIAbilityContext): void {
  let want: Want = {
    deviceId: deviceId,
    bundleName: context.abilityInfo.bundleName,
    abilityName: CommonConstants.ABILITY_NAME,
    parameters: {
      'url': CommonConstants.NEWS_DETAIL_PAGE,
      'newsId': newsId
    }
  };
  globalThis.context.startAbility(want).catch((err) => {
    Logger.error(`startAbilityContinuation error. Code is ${err.code}, message is ${err.message}`);
    prompt.showToast({
      message: $r('app.string.start_ability_continuation_error')
    });
  })
}