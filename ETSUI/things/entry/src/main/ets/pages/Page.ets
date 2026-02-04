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
import { Setting } from "./Setting"
import { AudioRecordPage } from "./sound"
import {GalleryMainPage} from "./mainPage"


@Entry
@Component
struct Page {
  @State message: string = 'Hello World';

  build() {
    Tabs({ barPosition: BarPosition.End }) {
      TabContent() {
        Row(){
          GalleryMainPage()
        }

      }
      .tabBar('首页')
      .backgroundColor('#FFFFFF')

      // TabContent() {
      //   Text('发现内容')
      //     .fontSize(16)
      //     .fontColor('#333')
      // }
      // .tabBar('视频检视')
      // .backgroundColor('#FFFFFF')

      TabContent() {
        AudioRecordPage()
      }
      .tabBar('录音设置')
      .backgroundColor('#FFFFFF')

      TabContent() {
        Setting()
      }
      .tabBar('视频设置')
      .backgroundColor('#FFFFFF')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}
