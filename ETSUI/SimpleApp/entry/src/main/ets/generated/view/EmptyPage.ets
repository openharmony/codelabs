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

import { router } from '@kit.ArkUI';

@Entry
@Component
struct EmptyPage {
  @State content: string = (router.getParams() as Record<string, string>)['text'];

  build() {
    Column() {
      Row() {
        Text('empty page for ')
          .fontSize('25vp')
          .fontColor(Color.Gray)
        Text(this.content)
          .fontSize('25vp')
          .fontColor(Color.Blue)
      }
      .onClick(() => {
        router.back();
      })
    }
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .width('100%')
    .height('100%')
  }
}