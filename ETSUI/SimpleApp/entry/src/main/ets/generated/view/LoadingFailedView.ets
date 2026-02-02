/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { CommonConstants } from '../common/CommonConstants'

@Builder
export function LoadingFailedView(handleReload?: () => void) {
  Row() {
    Column() {
      Image($r('app.media.ic_failure'))
        .width("120vp")
        .height("120vp")
      Text("Failed to load the network")
        .fontColor($r('sys.color.mask_secondary'))
        .fontFamily(CommonConstants.HARMONY_HEI_TI_FONT_FAMILY)
        .fontSize("14fp")
        .margin({ top: "8vp" })
      Row() {
        Text("Reload")
          .fontColor("#FF3E50")
          .fontFamily(CommonConstants.HARMONY_HEI_TI_FONT_FAMILY)
          .fontSize("12fp")
          .width(CommonConstants.FULL_PERCENT)
          .textAlign(TextAlign.Center)
      }
      .height("32vp")
      .width("80vp")
      .margin({ top: "8vp" })
      .borderRadius($r('sys.float.corner_radius_level6'))
      .onClick(() => handleReload?.())
    }
  }
  .justifyContent(FlexAlign.Center)
  .layoutWeight(1)
}