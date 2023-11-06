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
import { PlayBarModel } from '../common/model/PlayBarModel';

@Component
export struct PlayListMusicView {
  @Consume playBarModel: PlayBarModel;
  private item: MusicItem = new MusicItem();

  build() {
    Column() {
      Text(this.item.name)
        .fontSize($r('app.float.dialog_name'))
        .fontColor(this.playBarModel!.musicItem!.id === this.item.id ? $r('app.color.font_color_pink') :
          $r('app.color.dialog_name_color'))
        .fontWeight(FontWeight.Normal)
        .lineHeight(CommonConstants.LINE_HEIGHT)
      Row() {
        Image(this.item.isVip ? $r('app.media.ic_vip') : $r('app.media.ic_hd'))
          .width(CommonConstants.LIST_IMAGE_WIDTH)
          .height(CommonConstants.LIST_IMAGE_HEIGHT)
          .margin({ right: CommonConstants.LIST_MARGIN_RIGHT })
        Text(this.item.singer)
          .fontSize($r('app.float.dialog_singer'))
          .fontColor(this.playBarModel!.musicItem!.id === this.item.id ? $r('app.color.font_color_pink') :
            $r('app.color.dialog_singer_color'))
          .fontWeight(FontWeight.Normal)
      }
      .width(CommonConstants.FULL_WIDTH)
      .alignSelf(ItemAlign.Start)
    }
    .alignItems(HorizontalAlign.Start)
    .width(CommonConstants.FULL_WIDTH)
    .margin({
      top: CommonConstants.LIST_MARGIN_TOP,
      bottom: CommonConstants.MUSIC_MARGIN_BOTTOM
    })
  }
}