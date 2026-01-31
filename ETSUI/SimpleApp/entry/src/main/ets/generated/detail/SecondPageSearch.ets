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
export struct SecondPageSearch {
  @Consume('appPathStack') appPathStack: NavPathStack;
  @State searchValue: string = '';

  jump(): void {
    this.appPathStack.pushPathByName('EmptyPagePathStack', null);
  }

  build() {
    NavDestination() {
      Column() {
        SearchBar({
          searchValue: this.searchValue,
          onBack: () => {
            this.appPathStack.clear();
            return false;
          }
        })
      }
      .backgroundColor($r('sys.color.ohos_id_color_sub_background'))
      .height(CommonConstants.FULL_PERCENT)
      .width(CommonConstants.FULL_PERCENT)
    }
    .hideTitleBar(true)
    .onBackPressed(() => {
      this.appPathStack.clear();
      return true;
    })
  }
}

@Builder
export function SearchPageBuilder() {
  SecondPageSearch()
}

@Component
struct SearchBar {
  @Consume('appPathStack') appPathStack: NavPathStack;
  @Link searchValue: string;
  onBack: () => void = () => {
  };

  build() {
    Row() {
      Image($r('app.media.ic_back'))
        .size({ width: "24vp", height: "24vp" })
        .width("40vp")
        .height("40vp")
        .margin({ right: "8vp" })
        .onClick(() => this.onBack())
      SearchInput({
        searchValue: this.searchValue
      })
    }
    .width(CommonConstants.FULL_PERCENT)
    .padding({
      top: "8vp",
      bottom: "8vp",
      left: "24vp",
      right: "24vp"
    })
  }
}

@Component
struct SearchInput {
  @Link searchValue: string;

  aboutToAppear(): void {
  }

  aboutToDisappear(): void {
  }

  build() {
    Search({
      placeholder: "Search",
      value: this.searchValue
    })
      .enableKeyboardOnFocus(true)
      .textFont({ size: $r('sys.float.Body_L') })
      .placeholderFont({ size: $r('sys.float.Body_L') })
      .layoutWeight(1)
  }
}