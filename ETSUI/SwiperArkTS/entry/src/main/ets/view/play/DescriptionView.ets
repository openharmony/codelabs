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

import { CommonConstants } from '../../common/constants/CommonConstant';

/**
 * text style.
 * @param fontSize Font size.
 * @param fonWeight Font weight.
 */
@Extend(Text) function textStyle(fontSize: Resource, fonWeight: number) {
  .fontSize(fontSize)
  .fontWeight(fonWeight)
  .fontColor($r('app.color.start_window_background'))
  .textAlign(TextAlign.Center)
  .margin(CommonConstants.MARGIN_PLAY_PAGE)
}

/**
 * Description component.
 */
@Component
export struct DescriptionView {
  build() {
    Column() {
      Text($r('app.string.movie_description_1'))
        .textStyle($r('app.float.font_size_sort_title'), CommonConstants.FONT_WEIGHT_NORMAL)
      Text($r('app.string.movie_description_2'))
        .textStyle($r('app.float.font_size_photo_name'), CommonConstants.FONT_WEIGHT_LIGHT)
        .opacity($r('app.float.opacity_deep'))
    }
    .height(CommonConstants.HEIGHT_DESCRIPTION)
    .width(CommonConstants.WIDTH_PLAY)
    .alignItems(HorizontalAlign.Start)
    .offset({ y: CommonConstants.OFFSET_DESCRIPTION_Y })
  }
}