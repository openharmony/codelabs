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
import { CommonConstants } from '../common/constants/CommonConstants';

@Component
export struct DeviceItem {
  private item: DeviceBean;
  private index: number;

  build() {
    Column() {
      Row() {
        Image($r('app.media.ic_device'))
          .width(CommonConstants.DEVICE_IMAGE_WIDTH)
          .aspectRatio(1)
        Row() {
          Column() {
            Text(this.item.deviceName)
              .fontSize($r('app.float.device_text_size'))
              .fontWeight(FontWeight.Bold)
            Text(this.item.deviceId)
              .fontSize($r('app.float.home_device_id_text_size'))
              .fontWeight(FontWeight.Lighter)
          }
          .alignItems(HorizontalAlign.Start)
          .width(CommonConstants.DEVICE_TEXT_WIDTH)
          Column() {
            Radio({
              value: CommonConstants.RADIO_NAME + this.index,
              group: CommonConstants.RADIO_BUTTON_GROUP_NAME
            })
              .width($r('app.float.radio_width'))
              .aspectRatio(1)
          }
          .width(CommonConstants.RADIO_COLUMN_WIDTH)
          .justifyContent(FlexAlign.Start)
        }
        .flexGrow(1)
        .margin({ left: CommonConstants.RADIO_LEFT })
      }
      .width(CommonConstants.FULL_PERCENT)
    }
    .width(CommonConstants.FULL_PERCENT)
    .margin({ bottom: $r('app.float.row_bottom') })
  }
}