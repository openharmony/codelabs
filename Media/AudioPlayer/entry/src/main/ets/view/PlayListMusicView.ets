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

import { MusicItem } from '../common/bean/MusicItem';
import { CommonConstants } from '../common/constants/CommonConstants';

@Component
export struct PlayListMusicView {
  @Link musicItem: MusicItem;
  private item: MusicItem;

  build() {
    Column() {
      Text(this.item.name)
        .fontSize($r('app.float.dialog_name'))
        .fontColor(this.musicItem.id === this.item.id ? $r("app.color.font_color_pink") : $r("app.color.dialog_name_color"))
        .fontWeight(FontWeight.Normal)
        .lineHeight(CommonConstants.LIST_DIALOG_MUSIC.LINE_HEIGHT)
      Row() {
        Image(this.item.isVip ? $r('app.media.ic_vip') : $r('app.media.ic_hd'))
          .width(CommonConstants.LIST_DIALOG_MUSIC.IMAGE_WIDTH)
          .height(CommonConstants.LIST_DIALOG_MUSIC.IMAGE_HEIGHT)
          .margin({ right: CommonConstants.LIST_DIALOG_MUSIC.IMAGE_MARGIN_RIGHT })
        Text(this.item.singer)
          .fontSize($r('app.float.dialog_singer'))
          .fontColor(this.musicItem.id === this.item.id ? $r("app.color.font_color_pink") : $r("app.color.dialog_singer_color"))
          .fontWeight(FontWeight.Normal)
      }
      .width(CommonConstants.FULL_WIDTH)
      .alignSelf(ItemAlign.Start)
    }
    .alignItems(HorizontalAlign.Start)
    .width(CommonConstants.FULL_WIDTH)
    .margin({
      top: CommonConstants.LIST_DIALOG_MUSIC.MUSIC_MARGIN_TOP,
      bottom: CommonConstants.LIST_DIALOG_MUSIC.MUSIC_MARGIN_BOTTOM
    })
  }
}