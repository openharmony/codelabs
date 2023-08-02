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
import CommonConstants from '../common/constants/CommonConstants';
import { NewsData } from '../common/bean/NewsData';
import Logger from '../common/utils/Logger';

@Component
export default struct NewsList {
  private newsItems: NewsData[];

  build() {
    List() {
      ForEach(this.newsItems, (item: NewsData, index: number) => {
        ListItem() {
          GridRow({
            columns: {
              sm: CommonConstants.FOUR_COLUMN,
              md: CommonConstants.EIGHT_COLUMN,
              lg: CommonConstants.TWELVE_COLUMN
            },
            breakpoints: {
              value: [
                CommonConstants.SMALL_DEVICE_TYPE,
                CommonConstants.MIDDLE_DEVICE_TYPE,
                CommonConstants.LARGE_DEVICE_TYPE
              ]
            },
            gutter: { x: $r('app.float.grid_row_gutter') }
          }) {
            GridCol({
              span: {
                sm: CommonConstants.FOUR_COLUMN,
                md: CommonConstants.EIGHT_COLUMN,
                lg: CommonConstants.EIGHT_COLUMN
              },
              offset: {
                sm: CommonConstants.ZERO_COLUMN,
                md: CommonConstants.ZERO_COLUMN,
                lg: CommonConstants.TWO_COLUMN
              }
            }) {
              NewsItem({ newsItem: item, isLast: index === this.newsItems.length - 1 })
            }
          }
        }
      }, (item, index) => index + JSON.stringify(item))
    }
    .height(CommonConstants.FULL_COMPONENT)
  }
}

@Component
struct NewsItem {
  private newsItem: NewsData;
  private isLast: boolean;

  build() {
    Column() {
      Row() {
        Column() {
          Row() {
            Text(this.newsItem.title)
              .fontSize($r('app.float.middle_font_size'))
              .height($r('app.float.news_item_title_height'))
              .maxLines(CommonConstants.NEWS_MAX_LINES)
              .textOverflow({ overflow: TextOverflow.Ellipsis })
              .align(Alignment.Top)
          }
          .alignItems(VerticalAlign.Top)
          .margin({ bottom: $r('app.float.list_title_margin_bottom') })
          .height($r('app.float.news_item_title_height'))
          .width(CommonConstants.FULL_COMPONENT)

          Row() {
            Text(this.newsItem.label)
              .fontSize($r('app.float.small_font_size'))
              .fontColor(this.newsItem.redLabel ? Color.Red : $r('app.color.font_color_gray'))
              .fontWeight(FontWeight.Normal)
              .margin({ right: $r('app.float.news_label_margin_right') })
            Text(this.newsItem.from)
              .fontSize($r('app.float.small_font_size'))
              .fontColor($r('app.color.font_color_gray'))
              .fontWeight(FontWeight.Normal)
          }
          .padding({ bottom: $r('app.float.list_label_padding_bottom') })
          .width(CommonConstants.FULL_COMPONENT)
        }
        .justifyContent(FlexAlign.Start)
        .padding({ right: $r('app.float.news_item_padding_right') })
        .height(CommonConstants.FULL_COMPONENT)
        .layoutWeight(CommonConstants.ONE)

        Image(this.newsItem.imgUrl)
          .height($r('app.float.news_item_image_height'))
          .width($r('app.float.news_item_image_width'))
          .borderRadius($r('app.float.list_image_border_radius'))
      }
      .padding({ top: $r('app.float.list_row_padding_top') })
      .width(CommonConstants.FULL_COMPONENT)
      .alignItems(VerticalAlign.Top)

      Divider()
        .color($r('app.color.detail_divider_color'))
        .opacity(CommonConstants.DIVIDER_OPACITY)
        .visibility(this.isLast ? Visibility.None : Visibility.Visible)
    }
    .height($r('app.float.news_item_column_height'))
    .padding({
      left: $r('app.float.news_list_padding'),
      right: $r('app.float.news_list_padding')
    })
    .width(CommonConstants.FULL_COMPONENT)
    .onClick(() => {
      router.pushUrl({
        url: CommonConstants.NEWS_DETAIL_PAGE,
        params: {
          newsItem: this.newsItem
        }
      }).catch((err) => {
        Logger.error(`router pushUrl failed. Code is ${err.code}, message is ${err.message}`);
      })
    })
  }
}