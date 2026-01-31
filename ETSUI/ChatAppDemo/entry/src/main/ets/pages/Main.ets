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

import { ChatList } from '../components/ChatList'
import { Contact } from '../components/Contact'
import { Post } from '../components/Post'
import { Profile } from '../components/Profile'

@Entry
@Component
struct Main {
  @State curIndex: number = 0
  onPageShow() {
    if (this.curIndex === 0) {
      // 触发 ChatList 重新加载数据
      // 你可以在 ChatList 组件上加一个变量来触发刷新，或者使用 AppStorage
      AppStorage.setOrCreate('updateChatList', Date.now());
    }
  }

  @Builder buildTabBar(title: Resource, normalImage: Resource, selectedImage: Resource, index: number) {
    Column() {
      Image(this.curIndex == index ? selectedImage : normalImage)
        .width(28)
      Text(title)
        .fontSize(12)
        .fontColor(this.curIndex == index ? Color.Green : Color.Black)
        .margin({
          top: 2
        })
    }
  }

  build() {
    Tabs() {
      TabContent() {
        ChatList()
      }.tabBar(this.buildTabBar($r("app.string.tab_chat"), $r("app.media.main_0_normal"), $r("app.media.main_0_select"), 0))

      TabContent() {
        Contact()
      }.tabBar(this.buildTabBar($r("app.string.tab_contacts"), $r("app.media.main_1_normal"), $r("app.media.main_1_select"), 1))

      TabContent() {
        Post()
      }.tabBar(this.buildTabBar($r("app.string.tab_moments"), $r("app.media.main_2_normal"), $r("app.media.main_2_select"), 2))

      TabContent() {
        Profile()
      }.tabBar(this.buildTabBar($r("app.string.label_me"), $r("app.media.main_3_normal"), $r("app.media.main_3_select"), 3))

    }.width('100%')
    .height('100%')
    .barPosition(BarPosition.End)
    .backgroundColor($r('app.color.main_background'))
    .onChange((index: number) => {
      this.curIndex = index
    })
  }
}