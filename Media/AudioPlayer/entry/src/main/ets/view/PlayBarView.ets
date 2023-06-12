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
import { MusicItem } from '../common/bean/MusicItem';
import { PlayBarController } from '../controller/PlayBarController';
import { CommonConstants, PlayState } from '../common/constants/CommonConstants';

@Component
export struct PlayBarView {
  @StorageLink('musicList') musicList: Array<MusicItem> = [];
  @Link musicItem: MusicItem;
  @State playModeIcon: Resource = $r('app.media.ic_list_mode'); // Playback mode icon.
  @Link playStateIcon: Resource;
  @Link playMode: number; // Playback mode.
  @Link playValue: number; // Playback duration position.
  playState: number = PlayState.PLAY; // Playback status.
  PlayBarController: PlayBarController = new PlayBarController();
  dialogController: CustomDialogController = new CustomDialogController({
    builder: PlayListDialogView({
      musicList: $musicList,
      musicItem: $musicItem,
      playModeIcon: $playModeIcon,
      playMode: $playMode
    }),
    alignment: DialogAlignment.Bottom
  })

  build() {
    Row() {
      Image(this.playModeIcon)
        .width(CommonConstants.CONTROL_VIEW.MODE_ICON_WIDTH)
        .height(CommonConstants.CONTROL_VIEW.MODE_ICON_HEIGHT)
        .onClick(() => {
          this.PlayBarController.switchPlayMode(this);
        })
      Blank()
        .layoutWeight(1)
      Image($r('app.media.ic_previous'))
        .width(CommonConstants.CONTROL_VIEW.PREVIOUS_WIDTH)
        .height(CommonConstants.CONTROL_VIEW.PREVIOUS_HEIGHT)
        .onClick(() => {
          this.PlayBarController.getPreviousMusic(this);
        })
      Blank()
        .layoutWeight(1)
      Image(this.playStateIcon)
        .width(CommonConstants.CONTROL_VIEW.STATE_ICON_WIDTH)
        .height(CommonConstants.CONTROL_VIEW.STATE_ICON_HEIGHT)
        .objectFit(ImageFit.Contain)
        .onClick(() => {
          this.PlayBarController.switchPlayState(this);
        })
      Blank()
        .layoutWeight(1)
      Image($r('app.media.ic_next'))
        .width(CommonConstants.CONTROL_VIEW.NEXT_WIDTH)
        .height(CommonConstants.CONTROL_VIEW.NEXT_HEIGHT)
        .onClick(() => {
          this.PlayBarController.getNextMusic(this);
        })
      Blank()
        .layoutWeight(1)
      Image($r('app.media.ic_play_list'))
        .width(CommonConstants.CONTROL_VIEW.PLAY_LIST_WIDTH)
        .height(CommonConstants.CONTROL_VIEW.PLAY_LIST_HEIGHT)
        .onClick(() => {
          this.dialogController.open();
        })
    }
    .margin({ top: CommonConstants.CONTROL_VIEW.PAGE_MARGIN_TOP })
  }
}