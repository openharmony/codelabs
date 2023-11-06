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
import { MusicItem } from '../viewmodel/MusicItem';
import { AvSessionUtil } from '../common/utils/AvSessionUtil';
import { PlayBarView } from '../view/PlayBarView';
import { ProgressView } from '../view/ProgressView';
import { MusicCardView } from '../view/MusicCardView';
import { CommonConstants, PlayMode } from '../common/constants/CommonConstants';
import Logger from '../common/utils/Logger';
import { GlobalContext } from '../common/utils/GlobalContext';
import { AudioPlayerController } from '../controller/AudioPlayerControllerl';
import { PlayBarModel } from '../common/model/PlayBarModel';
import { PlayBarController } from '../controller/PlayBarController';

@Entry
@Component
struct Play {
  @StorageLink('musicList') musicList: Array<MusicItem> = [];
  @Provide playBarModel: PlayBarModel = new PlayBarModel();
  playBarController: PlayBarController = new PlayBarController(this.playBarModel);

  aboutToAppear() {
    let playController = GlobalContext.getContext().getObject('audioPlayerController') as AudioPlayerController;
    playController.setMusicList(this.musicList);
    // Obtain the music file to be played.
    let params = router.getParams() as Record<string, Object>;
    this.playBarModel.musicItem = params.item as MusicItem;
    if (this.playBarModel.musicItem === undefined) {
      Logger.error('play fail,musicItem is empty.');
      return;
    }

    // Access the playback control center.
    AvSessionUtil.initAvSession(this.playBarModel);

    // Setting the media source triggers the dataLoad callback.
    playController.playBarModel = this.playBarModel;
    playController.setEventCallBack();
    playController.play(this.playBarModel.musicItem.rawFileDescriptor, CommonConstants.START_POSITION);
  }

  build() {
    Column() {
      Row() {
        Navigator({ target: CommonConstants.PAGE_MUSIC_LIST, type: NavigationType.Back }) {
          Image($r('app.media.ic_back'))
            .objectFit(ImageFit.Contain)
            .width(CommonConstants.NAVIGATOR_WIDTH)
            .height(CommonConstants.NAVIGATOR_HEIGHT)
        }

        Text($r('app.string.play_title'))
          .fontSize($r('app.float.play_text'))
          .fontWeight(CommonConstants.FONT_WEIGHT_BOLDER)
          .fontColor($r("app.color.font_color_black"))
          .layoutWeight(CommonConstants.LAYOUT_WEIGHT)
          .textAlign(TextAlign.Center)
          .width(CommonConstants.FULL_WIDTH)
      }
      .width(CommonConstants.FULL_WIDTH)
      .padding({
        left: CommonConstants.NAVIGATOR_PADDING_LEFT,
        right: CommonConstants.NAVIGATOR_PADDING_RIGHT
      })
      .margin({
        top: CommonConstants.NAVIGATOR_PADDING_TOP,
        bottom: CommonConstants.NAVIGATOR_PADDING_BOTTOM
      })

      Column() {
        MusicCardView()
        ProgressView()
        PlayBarView({ playBarController: this.playBarController })
      }
      .width(CommonConstants.CONTROL_WIDTH)
      .alignItems(HorizontalAlign.Start)
    }
    .height(CommonConstants.FULL_HEIGHT)
    .width(CommonConstants.FULL_WIDTH)
  }
}