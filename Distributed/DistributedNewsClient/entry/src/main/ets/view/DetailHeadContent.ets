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
import { NewsData } from '../viewmodel/NewsDataModel';

@Extend(Text) function textStyle() {
  .fontSize($r('app.float.small_font_size'))
  .fontWeight(FontWeight.Normal)
  .fontColor($r('app.color.font_color_gray'))
}

@Component
export default struct DetailHeadContent {
  @Consume newsData: NewsData;

  build() {
    Column() {
      Row() {
        Image($r('app.media.ic_back'))
          .width($r('app.float.back_size'))
          .height($r('app.float.back_size'))
          .margin({
            top: $r('app.float.back_margin'),
            bottom: $r('app.float.back_margin')
          })
          .onClick(() => {
            router.back();
          })
      }
      .height($r('app.float.back_row_height'))
      .width(CommonConstants.FULL_COMPONENT)

      Scroll() {
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
            Column() {
              Text(this.newsData.title)
                .fontSize($r('app.float.news_detail_title_font_size'))
                .fontWeight(CommonConstants.FONT_WEIGHT_500)

              Row() {
                Text($r('app.string.read_count'))
                  .textStyle()
                Text(this.newsData.reads)
                  .textStyle()
                Text($r('app.string.like'))
                  .textStyle()
                  .margin({ left: $r('app.float.like_margin_right') })
                Text(this.newsData.likes)
                  .textStyle()
              }
              .margin({ top: $r('app.float.read_like_margin_top') })
            }
            .width(CommonConstants.FULL_COMPONENT)
            .alignItems(HorizontalAlign.Start)
          }

          GridCol({
            span: {
              sm: CommonConstants.FOUR_COLUMN,
              md: CommonConstants.FOUR_COLUMN,
              lg: CommonConstants.FOUR_COLUMN
            },
            offset: {
              sm: CommonConstants.ZERO_COLUMN,
              md: CommonConstants.TWO_COLUMN,
              lg: CommonConstants.SIX_COLUMN
            }
          }) {
            Image(this.newsData.imgUrl)
              .borderRadius($r('app.float.news_detail_image_border_radius'))
              .margin({
                top: $r('app.float.head_image_margin'),
                bottom: $r('app.float.head_image_margin')
              })
              .objectFit(ImageFit.Fill)
              .height($r('app.float.news_detail_image_height'))
              .width(CommonConstants.FULL_COMPONENT)
          }

          GridCol({
            span: {
              sm: CommonConstants.FOUR_COLUMN,
              md: CommonConstants.EIGHT_COLUMN,
              lg: CommonConstants.EIGHT_COLUMN
            },
            offset: {
              sm: CommonConstants.ZERO_COLUMN,
              md: CommonConstants.TWO_COLUMN,
              lg: CommonConstants.SIX_COLUMN
            }
          }) {
            Text(this.newsData.content)
              .textStyle()
              .lineHeight($r('app.float.news_content_line_height'))
          }
        }
      }
      .padding({
        bottom: $r('app.float.news_detail_padding_bottom')
      })
      .scrollBar(BarState.Off)
    }
    .margin({
      left: $r('app.float.news_detail_margin'),
      right: $r('app.float.news_detail_margin')
    })
    .height(CommonConstants.FULL_COMPONENT)
    .alignItems(HorizontalAlign.Start)
  }
}