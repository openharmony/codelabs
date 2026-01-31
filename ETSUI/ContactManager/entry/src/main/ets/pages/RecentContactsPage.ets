/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
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
import { Routes } from '../constants/Routes';
import { Contact } from '../model/Contact';
import { ContactService } from '../service/ContactService';
import { MetadataService } from '../service/MetadataService';
import { ContactActivity } from '../model/ContactActivity';
import { TimeUtils } from '../utils/TimeUtils';

interface ActivityGroup {
  category: 'today' | 'yesterday' | 'thisWeek' | 'earlier';
  categoryName: string;
  activities: ContactActivity[];
}

@Entry
struct RecentContactsPage {
  @State isLoading: boolean = true;
  @State activityGroups: ActivityGroup[] = [];

  // ⭐ 默认选中“最近查看”
  @State selectedTab: 'viewed' | 'edited' | 'created' = 'viewed';

  async aboutToAppear(): Promise<void> {
    await this.loadData();
  }

  onPageShow(): void {
    this.loadData();
  }

  private getNavTitle(): string {
    switch (this.selectedTab) {
      case 'viewed': return '最近查看';
      case 'edited': return '最近编辑';
      case 'created': return '最近添加';
    }
  }

  async loadData(): Promise<void> {
    this.isLoading = true;
    try {
      const allContacts = await ContactService.getAllContacts();
      const contactMap: Map<string, Contact> = new Map();
      allContacts.forEach(c => c.key && contactMap.set(c.key, c));

      let activities: ContactActivity[] = [];
      switch (this.selectedTab) {
        case 'viewed':
          activities = await MetadataService.getRecentViewedContacts(contactMap);
          break;
        case 'edited':
          activities = await MetadataService.getRecentEditedContacts(contactMap);
          break;
        case 'created':
          activities = await MetadataService.getRecentCreatedContacts(contactMap);
          break;
      }

      this.activityGroups = this.groupByTimeCategory(activities);
    } catch (err) {
      console.error('[RecentContactsPage] 加载失败:', err);
    } finally {
      this.isLoading = false;
    }
  }

  groupByTimeCategory(activities: ContactActivity[]): ActivityGroup[] {
    const groupMap: Map<'today' | 'yesterday' | 'thisWeek' | 'earlier', ContactActivity[]> = new Map();

    activities.forEach(a => {
      if (!groupMap.has(a.timeCategory)) groupMap.set(a.timeCategory, []);
      groupMap.get(a.timeCategory)?.push(a);
    });

    const order: ('today' | 'yesterday' | 'thisWeek' | 'earlier')[] = [
      'today',
      'yesterday',
      'thisWeek',
      'earlier'
    ];


    return order
      .filter(k => groupMap.has(k))
      .map((k): ActivityGroup => {
        return {
          category: k,
          categoryName: TimeUtils.getTimeCategoryName(k),
          activities: groupMap.get(k) ?? []
        };
      });
  }

  handleTabChange(tab: 'viewed' | 'edited' | 'created'): void {
    if (this.selectedTab === tab) return;
    // 先更新状态，确保 UI 立即响应
    this.selectedTab = tab;
    // 使用 Promise 确保状态更新完成后再加载数据
    Promise.resolve().then(() => {
      this.loadData();
    });
  }

  build(): void {
    Navigation() {
      Column() {

        // ===== 顶部筛选 Tab =====
        Row() {
          this.TabButton('最近查看', 'viewed');
          this.TabButton('最近编辑', 'edited');
          this.TabButton('最近添加', 'created');
        }
        .width('100%')
        .height(48)
        .padding({ left: 16, right: 16 })
        .backgroundColor(Color.White)
        .justifyContent(FlexAlign.SpaceAround)

        // ===== 内容区 =====
        if (this.isLoading) {
          Column() {
            LoadingProgress().width(50).height(50)
          }.width('100%').layoutWeight(1).justifyContent(FlexAlign.Center)

        } else if (this.activityGroups.length === 0) {
          Column() {
            Text('暂无最近活动').fontSize(16).fontColor(Color.Gray)
          }.width('100%').layoutWeight(1).justifyContent(FlexAlign.Center)

        } else {
          List() {
            ForEach(this.activityGroups, (group: ActivityGroup) => {
              ListItemGroup({ header: this.GroupHeader(group.categoryName), space: 0 }) {
                ForEach(group.activities, (activity: ContactActivity) => {
                  ListItem() { this.ActivityItem(activity) }
                  .onClick(() => {
                    router.pushUrl({
                      url: Routes.CONTACT_DETAIL,
                      params: { contactId: activity.contact.id }
                    });
                  });
                }, (a: ContactActivity) => a.contact.key || a.contact.id);
              }
            }, (g: ActivityGroup) => g.category);
          }
          .layoutWeight(1)
          .backgroundColor('#F1F3F5');
        }
      }
    }
    .title(this.getNavTitle())
    .titleMode(NavigationTitleMode.Mini)
    .backgroundColor('#F1F3F5');
  }

  // ⭐ 蓝色高亮完全由 selectedTab 状态控制
  @Builder TabButton(label: string, tabValue: 'viewed' | 'edited' | 'created'): void {
    Text(label)
      .fontSize(14)
      .fontColor(this.selectedTab === tabValue ? '#007DFF' : '#666')
      .fontWeight(this.selectedTab === tabValue ? FontWeight.Bold : FontWeight.Normal)
      .padding({ left: 14, right: 14, top: 8, bottom: 8 })
      .backgroundColor(this.selectedTab === tabValue ? '#E6F2FF' : Color.Transparent)
      .borderRadius(16)
      .onClick(() => this.handleTabChange(tabValue));
  }

  @Builder GroupHeader(title: string): void {
    Row() {
      Text(title).fontSize(14).fontColor('#666').fontWeight(FontWeight.Medium)
    }.height(32).padding({ left: 16 }).backgroundColor('#F1F3F5');
  }

  @Builder ActivityItem(activity: ContactActivity): void {
    Row() {
      Column() {
        Text(activity.getActivityTypeText())
          .fontSize(10).fontColor(Color.White).fontWeight(FontWeight.Bold)
      }
      .width(40).height(40).borderRadius(20)
      .backgroundColor(activity.getActivityColor())
      .justifyContent(FlexAlign.Center)
      .margin({ right: 12 });

      Column() {
        Text(activity.contact.name || '未知').fontSize(16).fontWeight(FontWeight.Medium)
        Row() {
          Text(activity.getActivityTypeText()).fontSize(12).fontColor('#999')
          Text(' · ').fontSize(12).fontColor('#999')
          Text(TimeUtils.formatTime(activity.activityTime)).fontSize(12).fontColor('#999')
        }.margin({ top: 2 })
      }.layoutWeight(1).alignItems(HorizontalAlign.Start);

      Image($r('sys.media.ohos_ic_public_arrow_right'))
        .width(16).height(16).fillColor('#CCC')
    }
    .height(64).padding({ left: 16, right: 16 })
    .backgroundColor(Color.White);
  }
}
