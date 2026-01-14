/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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
// entry/src/main/ets/pages/MainPage.ets
import { ContactListPage } from './ContactListPage';
import { EventListPage } from './EventListPage';
import { MinePage } from './MinePage';
import { ContactDetailPage } from './ContactDetailPage';
import { Contact } from '../model/ContactModel';
import { EventEditPage } from './EventEditPage';
import { Event } from '../model/EventModel';
// ✅ [新增] 引入详情页
import { EventDetailPage } from './EventDetailPage';

@Entry
@Component
struct MainPage {
  @State currentIndex: number = 0;
  private controller: TabsController = new TabsController();

  @Provide('pageStack') pageStack: NavPathStack = new NavPathStack();

  private readonly COLOR_SELECTED = '#007DFF';
  private readonly COLOR_NORMAL = '#999999';

  @Builder
  TabBuilder(title: string | Resource, targetIndex: number, img: Resource) {
    Column() {
      Image(img)
        .size({ width: 24, height: 24 })
        .objectFit(ImageFit.Contain)
        .fillColor(this.currentIndex === targetIndex ? this.COLOR_SELECTED : this.COLOR_NORMAL)
      Text(title)
        .fontColor(this.currentIndex === targetIndex ? this.COLOR_SELECTED : this.COLOR_NORMAL)
        .fontSize(10)
        .margin({ top: 4 })
        .fontWeight(this.currentIndex === targetIndex ? FontWeight.Medium : FontWeight.Normal)
    }
    .width('100%')
    .height(50)
    .justifyContent(FlexAlign.Center)
    .onClick(() => {
      this.currentIndex = targetIndex;
      this.controller.changeIndex(this.currentIndex);
    })
  }

  @Builder
  PageMap(name: string, param: Object) {
    if (name === 'ContactDetailPage') {
      ContactDetailPage({ contact: param as Contact });
    }
    else if (name === 'EventEditPage') {
      EventEditPage({ eventItem: param as Event });
    }
    // ✅ [新增] 详情页路由配置
    else if (name === 'EventDetailPage') {
      EventDetailPage({ eventItem: param as Event });
    }
  }

  build() {
    Navigation(this.pageStack) {
      Tabs({ barPosition: BarPosition.End, controller: this.controller }) {
        TabContent() {
          EventListPage()
        }
        .tabBar(this.TabBuilder($r('app.string.event_title'), 0, $r('app.media.ic_tab_schedule')))

        TabContent() {
          ContactListPage()
        }
        .tabBar(this.TabBuilder($r('app.string.contact_title'), 1, $r('app.media.ic_tab_contacts')))

        TabContent() {
          MinePage()
        }
        .tabBar(this.TabBuilder($r('app.string.mine_title'), 2, $r('app.media.ic_tab_mine')))
      }
      .scrollable(false)
      .onChange((index: number) => {
        this.currentIndex = index;
      })
      .backgroundColor(Color.White)
    }
    .mode(NavigationMode.Stack)
    .hideTitleBar(true)
    .navDestination(this.PageMap)
  }
}