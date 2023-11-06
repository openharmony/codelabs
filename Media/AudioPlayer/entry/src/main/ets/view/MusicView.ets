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

import { MusicItem } from '../viewmodel/MusicItem';
import { CommonConstants } from '../common/constants/CommonConstants';

@Component
export struct MusicView {
  private item: MusicItem = new MusicItem();

  build() {
    Column() {
      Row() {
        Image(this.item.image)
          .height(CommonConstants.MUSIC_IMAGE_HEIGHT)
          .width(CommonConstants.IMAGE_WIDTH)
        Column() {
          Text(this.item.name)
            .fontSize($r('app.float.musicView_name'))
            .fontColor($r("app.color.font_color_black"))
            .fontWeight(CommonConstants.FONT_WEIGHT_BOLDER)
          Text(this.item.singer)
            .fontSize($r('app.float.musicView_singer'))
            .fontColor($r("app.color.font_color_black"))
            .fontWeight(CommonConstants.FONT_WEIGHT_BOLDER)
            .opacity($r('app.float.singer_opacity'))
        }
        .margin({ left: CommonConstants.ROW_MARGIN_LEFT })
        .alignItems(HorizontalAlign.Start)
      }
      .width(CommonConstants.FULL_WIDTH)
      .alignSelf(ItemAlign.Start)
    }
    .margin({
      left: CommonConstants.PAGE_MARGIN_LEFT,
      top: CommonConstants.MUSIC_MARGIN_TOP,
      right: CommonConstants.PAGE_MARGIN_RIGHT,
      bottom: CommonConstants.PAGE_MARGIN_BUTTON
    })
  }
}