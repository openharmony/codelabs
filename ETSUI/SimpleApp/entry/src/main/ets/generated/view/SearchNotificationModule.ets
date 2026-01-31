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

@Component
export struct SearchNotificationModule {
  @State search_placeholder: string = "搜索"
  @Consume('appPathStack') appPathStack: NavPathStack;

  jump(pageName: string = 'EmptyPagePathStack'): void {
    this.appPathStack.pushPathByName(pageName, null);
  }

  build() {
    /**
     * TODO:
     * please modify it according to the actual situation.
     * modify {@link SearchModule#search_placeholder}
     */
    Column() {
      Search({ placeholder: this.search_placeholder })
        .width("92%")
        .enableKeyboardOnFocus(false)
        .backgroundColor('#0C000000')
        .placeholderColor($r('sys.color.font_secondary'))
        .placeholderFont({
          family: "HarmonyHeiTi",
          size: $r('sys.float.Body_L'),
          weight: 100
        })
        .searchIcon({ src: $r('sys.symbol.search_things') })
        .fontColor($r('sys.color.font_primary'))
        .margin({ bottom: '10vp' })
        .onClick(() => {
          this.jump()
        })
    }
    .width('100%')
  }
}