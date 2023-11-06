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

import { NewsData, NewsDataModel } from '../viewmodel/NewsDataModel';
import NewsTab from '../view/NewsTab';
import CommonConstants from '../common/constants/CommonConstants';

@Entry
@Component
struct Index {
  private newsItems: NewsData[] = NewsDataModel.getNewsComposition();

  build() {
    Row() {
      Column() {
        NewsTab({ newsItems: this.newsItems })
      }
      .backgroundColor($r('app.color.bg_color_gray'))
      .width(CommonConstants.FULL_COMPONENT)
      .height(CommonConstants.FULL_COMPONENT)
    }
    .width(CommonConstants.FULL_COMPONENT)
  }
}