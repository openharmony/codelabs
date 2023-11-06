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

import ButtonClickMethod from '../model/ButtonClickMethod';
import StyleConstants from '../common/constants/StyleConstants';
import CommonConstants from '../common/constants/CommonConstants';
import common from '@ohos.app.ability.common';

/**
 * Get context.
 */
let context = getContext(this) as common.UIAbilityContext;

/**
 * The Index Page of EntryAbility.
 *
 * @param abilityResult The result returned by SecondAbility Second Page.
 */
@Entry
@Component
struct Index {
  @State text: string = '';
  @State bottomMargin: string = StyleConstants.MAIN_INDEX_BUTTON_MARGIN_BOTTOM;

  build() {
    Column() {
      Image($r('app.media.right'))
        .objectFit(ImageFit.Contain)
        .width(StyleConstants.IMAGE_WIDTH)
        .height(StyleConstants.IMAGE_HEIGHT)
        .margin({
          top: StyleConstants.IMAGE_MARGIN_TOP,
          bottom: StyleConstants.IMAGE_MARGIN_BOTTOM
        })

      Text($r('app.string.main_index_page_name'))
        .fontColor($r('app.color.text_font_color'))
        .fontSize(StyleConstants.FONT_SIZE_BIG)
        .fontWeight(StyleConstants.FONT_WEIGHT)
        .margin({ bottom: this.bottomMargin })

      // Conditional rendering: Displays the component when the value of text is not empty.
      if (this.text !== '') {
        Text(this.text)
          .fontColor($r('app.color.text_font_color'))
          .fontSize(StyleConstants.FONT_SIZE_SMALL)
          .opacity(StyleConstants.OPACITY)
          .margin({ bottom: StyleConstants.MAIN_INDEX_TEXT_MARGIN_BOTTOM })
      }

      Button($r('app.string.to_main_second_page_btn_text'),{ type: ButtonType.Capsule, stateEffect: true })
        .backgroundColor($r('app.color.button_background_color'))
        .width(StyleConstants.BUTTON_WIDTH)
        .height(StyleConstants.BUTTON_HEIGHT)
        .margin({ bottom: StyleConstants.BUTTON_MARGIN_BOTTOM })
        .onClick(() => {
          // Navigate to the Second Page of EntryAbility.
          ButtonClickMethod.toEntryAbilitySecond();
          this.text = '';
          this.bottomMargin = StyleConstants.MAIN_INDEX_BUTTON_MARGIN_BOTTOM;
        })

      Button($r('app.string.to_second_index_page_btn_text'), { type: ButtonType.Capsule, stateEffect: true })
        .backgroundColor($r('app.color.button_background_color'))
        .width(StyleConstants.BUTTON_WIDTH)
        .height(StyleConstants.BUTTON_HEIGHT)
        .margin({ bottom: StyleConstants.BUTTON_MARGIN_BOTTOM })
        .onClick(() => {
          // Navigate to the Index Page of SecondAbility.
          ButtonClickMethod.toSecondAbilityIndex(context);
          this.text = '';
          this.bottomMargin = StyleConstants.MAIN_INDEX_BUTTON_MARGIN_BOTTOM;
        })

      Button($r('app.string.to_second_second_page_btn_text'), { type: ButtonType.Capsule, stateEffect: true })
        .backgroundColor($r('app.color.button_background_color'))
        .width(StyleConstants.BUTTON_WIDTH)
        .height(StyleConstants.BUTTON_HEIGHT)
        .onClick(() => {
          this.text = '';
          this.bottomMargin = StyleConstants.MAIN_INDEX_BUTTON_MARGIN_BOTTOM;

          // Navigate to the Second Page of SecondAbility.
          ButtonClickMethod.toSecondAbilitySecond(context, (abilityResult) => {
            // Obtain abilityResult passed on when SecondAbility is destroyed.
            if (abilityResult.resultCode === CommonConstants.RESULT_CODE) {
              let src: string = abilityResult?.want?.parameters?.src as string ?? '-';
              let count: number = abilityResult?.want?.parameters?.count as number ?? 0;
              this.text = `${src}：${count}`;
              this.bottomMargin = StyleConstants.BUTTON_MARGIN_BOTTOM;
            }
          });
        })
    }
    .width(StyleConstants.FULL_PERCENT)
    .height(StyleConstants.FULL_PERCENT)
    .backgroundColor($r('app.color.background_color'))
  }
}