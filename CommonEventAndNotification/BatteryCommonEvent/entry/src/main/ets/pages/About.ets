/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import { APP_NAME } from '../app';
import { TimeUtil } from '../utils/TimeUtil';
import router from '@ohos.router';

@Entry
@Component
export struct About {
  build() {
    Column({ space: 10 }) {
      Row() {
        Text('About').fontSize(20).fontWeight(FontWeight.Bold).layoutWeight(1);
        Button('Back').onClick(() => { router.back(); });
      }.padding({ left: 16, right: 16, top: 10 });

      Column({ space: 8 }) {
        Text(APP_NAME).fontSize(18).fontWeight(FontWeight.Bold);
        Text('A sample for battery common-event subscription + memory check.')
          .opacity(0.75);
        Text(`Build time: ${TimeUtil.formatTime(Date.now())}`).opacity(0.6);
        Text('Tips: Use Settings page to change record mode.').opacity(0.75);
      }
      .padding(16)
      .backgroundColor(0x08000000)
      .borderRadius(12)
      .margin({ left: 16, right: 16 });
    }
    .height('100%');
  }
}
