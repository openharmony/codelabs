/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import ButtonClickMethod from '../../../../../entry/src/main/ets/model/ButtonClickMethod';
import StyleConstants from '../../../../../entry/src/main/ets/common/constants/StyleConstants';
import Want from '@ohos.app.ability.Want';
import { GlobalContext } from '../../../../../entry/src/main/ets/common/utils/GlobalContext';
import common from '@ohos.app.ability.common';

/**
 * Get context.
 */
let context = getContext(this) as common.UIAbilityContext;

/**
 * The Second Page of SecondAbility.
 */
@Entry
@Component
struct Second {
  secondAbilityWant?: Want = GlobalContext.getContext().getObject('secondAbilityWant')
  @State src: string = this.secondAbilityWant?.parameters?.src as string ?? '-';
  @State count: number = this.secondAbilityWant?.parameters?.count as number ?? 0;

  build() {
    Column() {
      Image($r('app.media.left'))
        .objectFit(ImageFit.Contain)
        .width(StyleConstants.IMAGE_WIDTH)
        .height(StyleConstants.IMAGE_HEIGHT)
        .margin({
          top: StyleConstants.IMAGE_MARGIN_TOP,
          bottom: StyleConstants.IMAGE_MARGIN_BOTTOM
        })

      Text($r('app.string.second_second_page_name'))
        .fontColor($r('app.color.text_font_color'))
        .fontSize(StyleConstants.FONT_SIZE_BIG)
        .fontWeight(StyleConstants.FONT_WEIGHT)
        .margin({ bottom: StyleConstants.BUTTON_MARGIN_BOTTOM })

      Text(`${this.src}：${this.count}`)
        .fontColor($r('app.color.text_font_color'))
        .fontSize(StyleConstants.FONT_SIZE_SMALL)
        .opacity(StyleConstants.OPACITY)
        .margin({ bottom: StyleConstants.TEXT_MARGIN_BOTTOM })

      Button($r('app.string.terminate_second_for_result_btn_text'), { type: ButtonType.Capsule, stateEffect: true })
        .backgroundColor($r('app.color.button_background_color'))
        .width(StyleConstants.BUTTON_WIDTH)
        .height(StyleConstants.BUTTON_HEIGHT)
        .onClick(() => {
          // Stop SecondAbility itself and return the result.
          ButtonClickMethod.terminateSecondAbilityForResult(context);
        })
    }
    .width(StyleConstants.FULL_PERCENT)
    .height(StyleConstants.FULL_PERCENT)
    .backgroundColor($r('app.color.background_color'))
  }
}