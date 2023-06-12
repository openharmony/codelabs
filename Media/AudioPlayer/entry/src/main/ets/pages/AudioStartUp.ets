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
import { CommonConstants } from '../common/constants/CommonConstants';

@Entry
@Component
struct AudioStartUp {
  private timeId: number;

  aboutToAppear() {
    // The play list page is automatically displayed 1s later.
    this.timeId = setTimeout(() => {
      router.pushUrl({
        url: CommonConstants.PAGE_MUSIC_LIST,
        params: {}
      }).catch(err => {
        Logger.error('push url fail: ' + err);
      })
    }, CommonConstants.DELAY_TIME)
  }

  onPageHide() {
    router.clear();
    clearTimeout(this.timeId);
  }

  build() {
    Column() {
      Column() {
        Image($r('app.media.icon'))
          .width(CommonConstants.START_UP_PAGE.IMAGE_WIDTH)
          .height(CommonConstants.START_UP_PAGE.IMAGE_HEIGHT)
          .objectFit(ImageFit.Contain)
      }
      .height(CommonConstants.START_UP_PAGE.IMAGE_CONTAINER)
      .width(CommonConstants.FULL_WIDTH)
      .alignItems(HorizontalAlign.Center)
      .justifyContent(FlexAlign.Center)
      Text($r('app.string.app_name'))
        .fontSize($r("app.float.start_title"))
        .textAlign(TextAlign.Center)
        .fontWeight(CommonConstants.FONT_WEIGHT_BOLDER)
        .fontColor($r("app.color.font_color_pink"))
      Text($r('app.string.app_slogan'))
        .fontSize($r("app.float.start_text"))
        .textAlign(TextAlign.Center)
        .fontWeight(FontWeight.Normal)
        .fontColor($r("app.color.font_color_pink"))
    }
    .height(CommonConstants.FULL_HEIGHT)
    .width(CommonConstants.FULL_WIDTH)
  }
}