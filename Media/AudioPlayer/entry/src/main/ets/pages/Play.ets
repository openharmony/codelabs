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

import router from '@ohos.router';
import { MusicItem } from '../common/bean/MusicItem';
import { AvSessionUtil } from '../common/utils/AvSessionUtil';
import { PlayBarView } from '../view/PlayBarView';
import { ProgressView } from '../view/ProgressView';
import { MusicCardView } from '../view/MusicCardView';
import { CommonConstants, PlayMode } from '../common/constants/CommonConstants';
import { CommonUtil } from '../common/utils/CommonUtil';
import Logger from '../common/utils/Logger';

@Entry
@Component
struct Play {
  @StorageLink('musicList') musicList: Array<MusicItem> = [];
  @State musicItem: MusicItem = this.musicList[0];
  @State playValue: number = CommonConstants.START_POSITION; // Progress bar position.
  @State totalValue: number = CommonConstants.TOTAL_VALUE; // Total length of the progress bar.
  @State playMode: number = PlayMode.LIST_LOOP; // Playback mode.
  @State playStateIcon: Resource = $r('app.media.ic_play') // Playback status icon.

  aboutToAppear() {
    globalThis.AudioPlayerController.setMusicList(this.musicList);
    // Obtain the music file to be played.
    this.musicItem = router.getParams()['item'];
    if (CommonUtil.isEmpty(this.musicItem)) {
      Logger.error('play fail,musicItem is empty.');
      return;
    }

    // Access the playback control center.
    AvSessionUtil.initAvSession(this);

    // Setting the media source triggers the dataLoad callback.
    globalThis.AudioPlayerController.setEventCallBack(this);
    globalThis.AudioPlayerController.play(this.musicItem.rawFileDescriptor, CommonConstants.START_POSITION);
  }

  build() {
    Column() {
      Row() {
        Navigator({ target: CommonConstants.PAGE_MUSIC_LIST, type: NavigationType.Back }) {
          Image($r('app.media.ic_back'))
            .objectFit(ImageFit.Contain)
            .width(CommonConstants.PLAY_PAGE.NAVIGATOR_WIDTH)
            .height(CommonConstants.PLAY_PAGE.NAVIGATOR_HEIGHT)
        }
        Text($r('app.string.play_title'))
          .fontSize($r('app.float.play_text'))
          .fontWeight(CommonConstants.FONT_WEIGHT_BOLDER)
          .fontColor($r("app.color.font_color_black"))
          .layoutWeight(CommonConstants.PLAY_PAGE.LAYOUT_WEIGHT)
          .textAlign(TextAlign.Center)
          .width(CommonConstants.FULL_WIDTH)
      }
      .width(CommonConstants.FULL_WIDTH)
      .padding({
        left: CommonConstants.PLAY_PAGE.NAVIGATOR_PADDING_LEFT,
        right: CommonConstants.PLAY_PAGE.NAVIGATOR_PADDING_RIGHT
      })
      .margin({
        top: CommonConstants.PLAY_PAGE.NAVIGATOR_PADDING_TOP,
        bottom: CommonConstants.PLAY_PAGE.NAVIGATOR_PADDING_BOTTOM
      })
      Column() {
        MusicCardView({ musicItem: $musicItem })
        ProgressView({ playValue: $playValue, totalValue: $totalValue })
        PlayBarView({
          musicItem: $musicItem,
          playMode: $playMode,
          playValue: $playValue,
          playStateIcon: $playStateIcon
        })
      }
      .width(CommonConstants.PLAY_PAGE.CONTROL_WIDTH)
      .alignItems(HorizontalAlign.Start)
    }
    .height(CommonConstants.FULL_HEIGHT)
    .width(CommonConstants.FULL_WIDTH)
  }
}