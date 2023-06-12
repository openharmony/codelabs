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

import { CommonUtil } from '../common/utils/CommonUtil';
import { CommonConstants } from '../common/constants/CommonConstants';

@Component
export struct ProgressView {
  @Link playValue: number;
  @Link totalValue: number;

  build() {
    Row() {
      Text(CommonUtil.formatDuration(this.playValue))
        .fontColor($r("app.color.music_card_color"))
        .fontSize($r('app.float.dialog_singer'))
        .margin({ right: CommonConstants.PROGRESS_VIEW.TEXT_MARGIN_RIGHT })
      Slider({
        value: this.playValue,
        min: CommonConstants.PROGRESS_VIEW.PROGRESS_MIN,
        max: this.totalValue,
        style: SliderStyle.OutSet
      })
        .enabled(false)
        .selectedColor($r("app.color.music_card_color"))
        .layoutWeight(1)
      Text(CommonUtil.formatDuration(this.totalValue))
        .fontColor($r("app.color.music_card_color"))
        .fontSize($r('app.float.dialog_singer'))
        .margin({ left: CommonConstants.PROGRESS_VIEW.TEXT_MARGIN_LEFT })
    }
    .margin({ top: CommonConstants.PROGRESS_VIEW.VIEW_MARGIN_TOP })
  }
}