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

import { PlayListDialogView } from '../view/PlayListDialogView';
import { MusicItem } from '../viewmodel/MusicItem';
import { PlayBarController } from '../controller/PlayBarController';
import { CommonConstants } from '../common/constants/CommonConstants';
import { PlayBarModel } from '../common/model/PlayBarModel';

@Component
export struct PlayBarView {
  @StorageLink('musicList') musicList: Array<MusicItem> = [];
  private playBarController?: PlayBarController;
  @Consume playBarModel: PlayBarModel;
  dialogController: CustomDialogController = new CustomDialogController({
    builder: PlayListDialogView({
      musicList: $musicList
    }),
    alignment: DialogAlignment.Bottom
  })

  build() {
    Row() {
      Image(this.playBarModel.playModeIcon)
        .width(CommonConstants.MODE_ICON_WIDTH)
        .height(CommonConstants.MODE_ICON_HEIGHT)
        .onClick(() => {
          this.playBarController!.switchPlayMode();
        })
      Blank()
        .layoutWeight(1)
      Image($r('app.media.ic_previous'))
        .width(CommonConstants.PREVIOUS_WIDTH)
        .height(CommonConstants.PREVIOUS_HEIGHT)
        .onClick(() => {
          this.playBarController!.getPreviousMusic(this.musicList, this.playBarModel.playState);
        })
      Blank()
        .layoutWeight(1)
      Image(this.playBarModel.playStateIcon)
        .width(CommonConstants.STATE_ICON_WIDTH)
        .height(CommonConstants.STATE_ICON_HEIGHT)
        .objectFit(ImageFit.Contain)
        .onClick(() => {
          this.playBarController!.switchPlayState(this.playBarModel.playState);
        })
      Blank()
        .layoutWeight(1)
      Image($r('app.media.ic_next'))
        .width(CommonConstants.NEXT_WIDTH)
        .height(CommonConstants.NEXT_HEIGHT)
        .onClick(() => {
          this.playBarController!.getNextMusic(this.musicList);
        })
      Blank()
        .layoutWeight(1)
      Image($r('app.media.ic_play_list'))
        .width(CommonConstants.PLAY_LIST_WIDTH)
        .height(CommonConstants.PLAY_LIST_HEIGHT)
        .onClick(() => {
          this.dialogController.open();
        })
    }
    .margin({ top: CommonConstants.PAGE_MARGIN_TOP })
  }
}