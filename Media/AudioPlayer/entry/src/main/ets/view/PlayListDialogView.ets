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
import { MusicItem } from '../common/bean/MusicItem';
import { CommonConstants, PlayMode } from '../common/constants/CommonConstants';

@CustomDialog
export struct PlayListDialogView {
  @Link musicList: Array<MusicItem>;
  @Link musicItem: MusicItem;
  @Link playModeIcon: Resource;
  @Link playMode: number;
  @State playModeDesc: Resource = $r('app.string.sequence');
  controller: CustomDialogController;

  switchPlayModeDesc() {
    if (this.playMode === PlayMode.RANDOM_PLAY) {
      this.playModeDesc = $r('app.string.random');
    } else if (this.playMode === PlayMode.SINGLE_LOOP) {
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
        Image(this.playModeIcon)
          .width(CommonConstants.LIST_DIALOG.IMAGE_WIDTH)
          .height(CommonConstants.LIST_DIALOG.IMAGE_HEIGHT)
          .margin({ right: CommonConstants.LIST_DIALOG.IMAGE_MARGIN_RIGHT })
        Text(this.playModeDesc)
          .fontSize($r("app.float.play_mode_description"))
          .fontWeight(CommonConstants.FONT_WEIGHT_BOLDER)
          .fontFamily($r("app.color.font_color_black"))
          .textAlign(TextAlign.Start)
          .alignSelf(ItemAlign.Start)
      }
      .width(CommonConstants.LIST_DIALOG.ROW_WIDTH)
      .margin({
        top: CommonConstants.LIST_DIALOG.ROW_MARGIN_TOP,
        bottom: CommonConstants.LIST_DIALOG.ROW_MARGIN_BOTTOM
      })
      List({
        space: CommonConstants.LIST_DIALOG.LIST_SPACE,
        initialIndex: CommonConstants.LIST_DIALOG.LIST_INITIAL_INDEX
      }) {
        ForEach(this.musicList, (item: MusicItem) => {
          ListItem() {
            PlayListMusicView({ musicItem: $musicItem, item: item })
          }
        }, item => JSON.stringify(item))
      }
      .listDirection(Axis.Vertical)
      .divider({
        strokeWidth: CommonConstants.LIST_DIALOG.DIVIDER_STROKE_WIDTH,
        color: CommonConstants.DIVIDER_COLOR,
        startMargin: CommonConstants.LIST_DIALOG.DIVIDER_START_MARGIN,
        endMargin: CommonConstants.LIST_DIALOG.END_MARGIN
      })
      .height(CommonConstants.LIST_DIALOG.LIST_HEIGHT)
      .width(CommonConstants.LIST_DIALOG.LIST_WIDTH)
    }
    .width(CommonConstants.FULL_WIDTH)
    .margin({ left: CommonConstants.LIST_DIALOG.PAGE_MARGIN_LEFT })
  }
}