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
  @State isLoading: boolean = false;
  @State activityGroups: ActivityGroup[] = [];

  // ⭐ 默认选中"最近查看"
  @State selectedTab: 'viewed' | 'edited' | 'created' = 'viewed';
  private loadSeq: number = 0;
  private isFirstLoad: boolean = true;

  async aboutToAppear(): Promise<void> {
    // 初始化服务（只在第一次）
    if (this.isFirstLoad) {
      this.isLoading = true;
      try {
        await ContactService.init();
        await MetadataService.init();
        console.info('[RecentContactsPage] 服务初始化成功');
      } catch (err) {
        console.error('[RecentContactsPage] 服务初始化失败:', err);
      }
      this.isFirstLoad = false;
    }
    
    await this.loadData();
  }

  onPageShow(): void {
    // 只在不是第一次加载时刷新
    if (!this.isFirstLoad) {
      this.loadData();
    }
  }

  private getNavTitle(): string {
    switch (this.selectedTab) {
      case 'viewed': return '最近查看';
      case 'edited': return '最近编辑';
      case 'created': return '最近添加';
    }
  }

  async loadData(): Promise<void> {
    const seq = ++this.loadSeq;
    const tab = this.selectedTab;
    
    console.info('[RecentContactsPage] ===== loadData START =====');
    console.info('[RecentContactsPage] seq:', seq, 'tab:', tab);
    
    this.isLoading = true;
    
    try {
      const allContacts = await ContactService.getAllContacts();
      
      // 检查是否被取消
      if (seq !== this.loadSeq) {
        console.warn('[RecentContactsPage] 请求已过期 (seq)', seq, '当前:', this.loadSeq);
        return;
      }
      
      console.info('[RecentContactsPage] 获取到联系人数量:', allContacts.length);
      
      const contactMap: Map<string, Contact> = new Map();
      allContacts.forEach(c => c.key && contactMap.set(c.key, c));

      let activities: ContactActivity[] = [];
      switch (tab) {
        case 'viewed':
          activities = await MetadataService.getRecentViewedContacts(contactMap);
          console.info('[RecentContactsPage] 最近查看数量:', activities.length);
          break;
        case 'edited':
          activities = await MetadataService.getRecentEditedContacts(contactMap);
          console.info('[RecentContactsPage] 最近编辑数量:', activities.length);
          break;
        case 'created':
          activities = await MetadataService.getRecentCreatedContacts(contactMap);
          console.info('[RecentContactsPage] 最近添加数量:', activities.length);
          break;
      }
      
      // 再次检查
      if (seq !== this.loadSeq || tab !== this.selectedTab) {
        console.warn('[RecentContactsPage] Tab已切换', 'seq:', seq, '当前seq:', this.loadSeq, 'tab:', tab, '当前tab:', this.selectedTab);
        return;
      }

      // 关键：创建新的数组引用，确保触发UI更新
      const newGroups = this.groupByTimeCategory(activities);
      this.activityGroups = [...newGroups];
      
      console.info('[RecentContactsPage] ===== loadData SUCCESS =====');
      console.info('[RecentContactsPage] 分组数量:', this.activityGroups.length, 'Tab:', tab);
    } catch (err) {
      console.error('[RecentContactsPage] ===== loadData ERROR =====');
      console.error('[RecentContactsPage] 加载失败:', err);
      // 即使失败也要清空数组，避免显示旧数据
      if (seq === this.loadSeq) {
        this.activityGroups = [];
      }
    } finally {
      // 只有最新的请求才能关闭 loading
      if (seq === this.loadSeq) {
        this.isLoading = false;
        console.info('[RecentContactsPage] loading 结束, seq:', seq);
      }
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
    // 移除这个判断，允许重复点击刷新
    // if (this.selectedTab === tab) return;
    
    console.info('[RecentContactsPage] ========== Tab切换 ==========');
    console.info('[RecentContactsPage] 从:', this.selectedTab, '到:', tab);
    console.info('[RecentContactsPage] loadSeq:', this.loadSeq);
    
    // 立即更新状态，强制触发 UI 刷新
    this.selectedTab = tab;
    this.activityGroups = [];
    this.isLoading = true;
    
    // 使用 nextTick 确保状态更新后再加载
    setTimeout(() => {
      console.info('[RecentContactsPage] 开始执行 loadData, 当前 Tab:', this.selectedTab);
      this.loadData();
    }, 10);
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
            Text('加载中...').fontSize(14).fontColor(Color.Gray).margin({ top: 10 })
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
