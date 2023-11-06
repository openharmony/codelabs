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
import { PlayBarModel } from '../common/model/PlayBarModel';

@Component
export struct ProgressView {
  @Consume playBarModel: PlayBarModel;

  build() {
    Row() {
      Text(CommonUtil.formatDuration(this.playBarModel.playValue))
        .fontColor($r("app.color.music_card_color"))
        .fontSize($r('app.float.dialog_singer'))
        .margin({ right: CommonConstants.PROGRESS_MARGIN_RIGHT })
      Slider({
        value: this.playBarModel.playValue,
        min: CommonConstants.PROGRESS_MIN,
        max: this.playBarModel.totalValue,
        style: SliderStyle.OutSet
      })
        .enabled(false)
        .selectedColor($r("app.color.music_card_color"))
        .layoutWeight(1)
      Text(CommonUtil.formatDuration(this.playBarModel.totalValue))
        .fontColor($r("app.color.music_card_color"))
        .fontSize($r('app.float.dialog_singer'))
        .margin({ left: CommonConstants.PROGRESS_MARGIN_LEFT })
    }
    .margin({ top: CommonConstants.VIEW_MARGIN_TOP })
  }
}