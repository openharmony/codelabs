/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { SearchNotificationModule } from '../view/SearchNotificationModule';
import { MultiFunctionEntryModule } from '../view/MultiFunctionEntryModule';
import { PromotionalBannerModule } from '../view/PromotionalBannerModule';
import { ProductRecommendationModule } from '../view/ProductRecommendationModule';
import { PersonalizedRecommendationModule } from '../view/PersonalizedRecommendationModule';

@Component
export struct HomePage {
  private scroller: Scroller = new Scroller();

  build() {
    Scroll(this.scroller) {
      Column() {
        SearchNotificationModule()
        MultiFunctionEntryModule()
        PromotionalBannerModule()
        ProductRecommendationModule()
        PersonalizedRecommendationModule()
      }
      .width('100%')
    }
    .scrollable(ScrollDirection.Vertical)
    .scrollBar(BarState.Off)
    .width('100%')
    .height('100%')
    .align(Alignment.Top)
    .backgroundColor($r('app.color.page_bg_color'))
  }
}