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

import { CommonConstants } from '../common/CommonConstants';

@Component
export struct EmptyPagePathStack {
  @Consume('appPathStack') appPathStack: NavPathStack;

  build() {
    NavDestination() {
      Column() {
        TopNavigationView({
          title: "详情页",
          onBackClick: () => {
            this.appPathStack.pop();
          }
        })
        Image($r("app.media.realImageGen_travel_0"))
      }
      .justifyContent(FlexAlign.SpaceBetween)
      .alignItems(HorizontalAlign.Center)
      .width(CommonConstants.FULL_PERCENT)
      .height(CommonConstants.FULL_PERCENT)
    }
    .hideTitleBar(true)
  }
}

@Builder
export function EmptyPagePathStackBuilder() {
  EmptyPagePathStack()
}

@Component
struct TopNavigationView {
  title?: ResourceStr;
  onBackClick?: Function;

  build() {
    Row() {
      if (this.onBackClick) {
        Image($r('app.media.ic_back'))
          .width("24vp")
          .height("24vp")
          .margin({ right: "8vp" })
          .onClick(() => this.onBackClick?.())
      }
      Text(this.title)
        .fontSize("18fp")
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Start)
        .layoutWeight(1)
    }
    .alignItems(VerticalAlign.Center)
    .justifyContent(FlexAlign.SpaceBetween)
    .width(CommonConstants.FULL_PERCENT)
    .height("54vp")
    .padding({
      left: "14vp",
      right: "14vp"
    })
  }
}