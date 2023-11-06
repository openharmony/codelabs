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

import NewsList from './NewsList';
import CommonConstants from '../common/constants/CommonConstants';
import { NewsData, NewsDataModel } from '../viewmodel/NewsDataModel';

@Component
export default struct NewsTab {
  @State currentIndex: number = 0;
  @State currentBreakpoint: string = CommonConstants.BREAKPOINT_SM;
  private newsItems: NewsData[] = [];

  @Builder TabBuilder(title: Resource, index: number) {
    Row() {
      Text(title)
        .fontSize(this.currentIndex === index ? $r('app.float.lager_font_size') : $r('app.float.middle_font_size'))
        .fontWeight(this.currentIndex === index ? CommonConstants.FONT_WEIGHT_500 : FontWeight.Normal)
        .fontColor(this.currentIndex === index ? $r('app.color.tab_font_select') : $r('app.color.font_color_gray'))
    }
    .layoutWeight(1)
    .margin({
      right: $r('app.float.news_tab_margin_right'),
      left: (this.currentBreakpoint === CommonConstants.BREAKPOINT_SM && index === 0) ?
        $r('app.float.news_tab_margin_left') : 0
    })
    .height(this.currentIndex === index ? $r('app.float.news_tab_current_height') : $r('app.float.news_tab_height'))
  }

  build() {
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
          lg: CommonConstants.TWELVE_COLUMN
        },
        offset: {
          sm: CommonConstants.ZERO_COLUMN,
          md: CommonConstants.ZERO_COLUMN,
          lg: CommonConstants.ZERO_COLUMN
        }
      }) {
        Tabs() {
          ForEach(CommonConstants.ALL_TITLE, (title: string, index: number) => {
            TabContent() {
              NewsList({ newsItems: NewsDataModel.getNewsByType(this.newsItems, title) })
            }
            .tabBar(this.TabBuilder(NewsDataModel.getTypeByStr(title), index))
          }, (title: string, index: number) => index + JSON.stringify(title))
        }
        .barHeight($r('app.float.news_tab_bar_height'))
        .barWidth(CommonConstants.FULL_COMPONENT)
        .barMode(this.currentBreakpoint === CommonConstants.BREAKPOINT_SM ? BarMode.Scrollable : BarMode.Fixed)
        .onChange((index: number) => {
          this.currentIndex = index;
        })
      }
    }
    .onBreakpointChange((breakpoints) => {
      this.currentBreakpoint = breakpoints;
    })
  }
}