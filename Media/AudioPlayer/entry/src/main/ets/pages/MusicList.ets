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
import Logger from '../common/utils/Logger';
import { MusicItem } from '../viewmodel/MusicItem';
import { MusicView } from '../view/MusicView';
import { CommonConstants } from '../common/constants/CommonConstants';
import { initializeMusic } from '../viewmodel/MusicViewModel';

@Entry
@Component
struct MusicList {
  @StorageLink('musicList') musicData: Array<MusicItem> = [];

  aboutToAppear() {
    initializeMusic(this.musicData);
    Logger.info('musicData: ' + this.musicData);
  }

  build() {
    Column() {
      Text($r('app.string.hot_music'))
        .fontSize($r('app.float.musicList_title'))
        .width(CommonConstants.TEXT_WIDTH)
        .height(CommonConstants.TEXT_HEIGHT)
        .margin({
          left: CommonConstants.TEXT_MARGIN_LEFT,
          top: CommonConstants.TEXT_MARGIN_TOP,
          right: CommonConstants.TEXT_MARGIN_RIGHT,
          bottom: CommonConstants.TEXT_MARGIN_BOTTOM
        })
      ForEach(this.musicData, (item: MusicItem) => {
        MusicView({ item: item })
          .onClick(() => {
            router.pushUrl({
              url: CommonConstants.PAGE_PLAY,
              params: { 'item': item }
            }).catch((error: Error) => {
              Logger.error(`go to play list fail,error: ${error}`);
            })
          })
      }, (item: MusicItem) => JSON.stringify(item))
    }
  }
}