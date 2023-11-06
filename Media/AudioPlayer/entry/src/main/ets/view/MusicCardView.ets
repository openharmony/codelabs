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

import { CommonConstants } from '../common/constants/CommonConstants';
import { PlayBarModel } from '../common/model/PlayBarModel';

@Component
export struct MusicCardView {
  @Consume playBarModel: PlayBarModel;

  build() {
    Column() {
      Image($r('app.media.ic_play_icon'))
        .width(CommonConstants.FULL_WIDTH)
        .height(CommonConstants.IMAGE_HEIGHT)
      Text(this.playBarModel.musicItem!.name)
        .fontSize($r('app.float.music_name'))
        .fontColor($r("app.color.music_card_color"))
        .fontWeight(CommonConstants.FONT_WEIGHT_BOLDER)
        .margin({ top: CommonConstants.NAME_MARGIN_TOP })
      Text(this.playBarModel.musicItem!.singer)
        .fontSize($r('app.float.music_singer'))
        .fontColor($r("app.color.music_card_color"))
        .fontWeight(FontWeight.Normal)
        .margin({ top: CommonConstants.SINGER_MARGIN_TOP })
      Text($r('app.string.lyrics', this.playBarModel.musicItem!.lyrics))
        .fontSize($r('app.float.music_lyrics'))
        .fontColor($r("app.color.music_card_color"))
        .fontWeight(FontWeight.Normal)
        .opacity($r('app.float.lyrics_opacity'))
        .margin({ top: CommonConstants.LYRICS_MARGIN_TOP })
    }
    .alignItems(HorizontalAlign.Start)
  }
}