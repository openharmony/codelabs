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

import { PlayListMusicView } from './PlayListMusicView';
import { MusicItem } from '../viewmodel/MusicItem';
import { CommonConstants, PlayMode } from '../common/constants/CommonConstants';
import { PlayBarModel } from '../common/model/PlayBarModel';

@CustomDialog
export struct PlayListDialogView {
  @Link musicList: Array<MusicItem>;
  @Consume playBarModel: PlayBarModel;
  @State playModeDesc: Resource = $r('app.string.sequence');
  controller: CustomDialogController = new CustomDialogController({ builder: {} });

  switchPlayModeDesc() {
    if (this.playBarModel.playMode === PlayMode.RANDOM_PLAY) {
      this.playModeDesc = $r('app.string.random');
    } else if (this.playBarModel.playMode === PlayMode.SINGLE_LOOP) {
      this.playModeDesc = $r('app.string.single');
    } else {
      this.playModeDesc = $r('app.string.sequence');
    }
  }

  aboutToAppear() {
    this.switchPlayModeDesc();
  }

  build() {
    Column() {
      Row() {
        Image(this.playBarModel.playModeIcon)
          .width(CommonConstants.DIALOG_IMAGE_WIDTH)
          .height(CommonConstants.DIALOG_IMAGE_HEIGHT)
          .margin({ right: CommonConstants.IMAGE_MARGIN_RIGHT })
        Text(this.playModeDesc)
          .fontSize($r("app.float.play_mode_description"))
          .fontWeight(CommonConstants.FONT_WEIGHT_BOLDER)
          .fontFamily($r("app.color.font_color_black"))
          .textAlign(TextAlign.Start)
          .alignSelf(ItemAlign.Start)
      }
      .width(CommonConstants.ROW_WIDTH)
      .margin({
        top: CommonConstants.ROW_MARGIN_TOP,
        bottom: CommonConstants.ROW_MARGIN_BOTTOM
      })

      List({
        space: CommonConstants.LIST_SPACE,
        initialIndex: CommonConstants.LIST_INITIAL_INDEX
      }) {
        ForEach(this.musicList, (item: MusicItem) => {
          ListItem() {
            PlayListMusicView({item: item })
          }
        }, (item: MusicItem) => JSON.stringify(item))
      }
      .listDirection(Axis.Vertical)
      .divider({
        strokeWidth: CommonConstants.DIVIDER_STROKE_WIDTH,
        color: CommonConstants.DIVIDER_COLOR,
        startMargin: CommonConstants.DIVIDER_START_MARGIN,
        endMargin: CommonConstants.END_MARGIN
      })
      .height(CommonConstants.LIST_HEIGHT)
      .width(CommonConstants.LIST_WIDTH)
    }
    .width(CommonConstants.FULL_WIDTH)
    .margin({ left: CommonConstants.DIALOG_MARGIN_LEFT })
  }
}