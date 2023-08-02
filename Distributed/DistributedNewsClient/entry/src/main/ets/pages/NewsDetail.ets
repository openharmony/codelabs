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
import { NewsData } from '../common/bean/NewsData';
import { NewsDataModel } from '../viewmodel/NewsDataModel';
import CommonConstants from '../common/constants/CommonConstants';
import DetailHeadContent from '../view/DetailHeadContent';
import DetailFooter from '../view/DetailFooter';

@Entry
@Component
struct NewsDetail {
  @Provide newsData: NewsData = null;
  private newsItems: NewsData[] = NewsDataModel.getNewsComposition();

  build() {
    Column() {
      Stack({ alignContent: Alignment.Bottom }) {
        DetailHeadContent()
        DetailFooter()
      }
      .height(CommonConstants.FULL_COMPONENT)
      .width(CommonConstants.FULL_COMPONENT)
    }
    .backgroundColor($r('app.color.bg_color_gray'))
    .height(CommonConstants.FULL_COMPONENT)
    .width(CommonConstants.FULL_COMPONENT)
  }

  aboutToAppear() {
    const params: any = router.getParams();
    if (params) {
      this.newsData = params.newsItem;
    } else {
      const want = globalThis.newWant;
      const newsId = want.parameters.newsId;
      this.newsData = this.newsItems.filter(item => (item.newsId === newsId))[0];
    }
  }
}