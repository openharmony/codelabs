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

import { promptAction } from '@kit.ArkUI';
import { dataManager, HistoryItem } from '../manager/DataManager';
import { BusinessError } from '@kit.BasicServicesKit';

// Builder函数，用于Navigation路由
@Builder
export function HistoryPageBuilder() {
  HistoryPage();
}

@Component
export struct HistoryPage {
  // 获取全局NavPathStack
  pathStack: NavPathStack = AppStorage.get('pathStack') as NavPathStack;
  // 历史记录列表
  @State historyList: HistoryItem[] = [];
  @State groupedHistory: Map<string, HistoryItem[]> = new Map();
  @State searchKeyword: string = '';
  @State isEditing: boolean = false;
  @State selectedItems: Set<number> = new Set();
  @State showFilterDialog: boolean = false;
  @State filterOption: string = '全部'; // '全部', '今天', '最近7天', '最近30天'
  // 深色模式
  @StorageLink('darkMode') darkMode: boolean = false;

  aboutToAppear() {
    console.info('=== HistoryPage aboutToAppear ===');
    this.loadHistory();
  }

  // 加载历史记录
  loadHistory() {
    console.info('=== HistoryPage loadHistory 开始 ===');
    let allHistory = dataManager.getHistoryList();

    // 应用时间筛选
    if (this.filterOption !== '全部') {
      const now = Date.now();
      const today = new Date();
      today.setHours(0, 0, 0, 0);
      const todayTime = today.getTime();

      if (this.filterOption === '今天') {
        allHistory = allHistory.filter(item => item.visitTime >= todayTime);
      } else if (this.filterOption === '最近7天') {
        const weekAgo = todayTime - 7 * 24 * 60 * 60 * 1000;
        allHistory = allHistory.filter(item => item.visitTime >= weekAgo);
      } else if (this.filterOption === '最近30天') {
        const monthAgo = todayTime - 30 * 24 * 60 * 60 * 1000;
        allHistory = allHistory.filter(item => item.visitTime >= monthAgo);
      }
    }

    // 应用搜索筛选
    if (this.searchKeyword.trim()) {
      const keyword = this.searchKeyword.toLowerCase();
      allHistory = allHistory.filter(item =>
      item.url.toLowerCase().includes(keyword) ||
      item.title.toLowerCase().includes(keyword)
      );
      // 搜索时不分组
      this.historyList = allHistory;
      this.groupedHistory = new Map([['搜索结果', this.historyList]]);
      console.info(`搜索结果: ${this.historyList.length}条`);
    } else {
      this.historyList = allHistory;
      this.groupedHistory = this.groupByDate(allHistory);
      console.info(`加载历史记录: ${this.historyList.length}条`);
      console.info(`分组数量: ${this.groupedHistory.size}`);
    }
  }

  // 按日期分组
  groupByDate(list: HistoryItem[]): Map<string, HistoryItem[]> {
    const grouped = new Map<string, HistoryItem[]>();
    const now = new Date();
    const today = new Date(now.getFullYear(), now.getMonth(), now.getDate()).getTime();
    const yesterday = today - 24 * 60 * 60 * 1000;
    const weekAgo = today - 7 * 24 * 60 * 60 * 1000;

    list.forEach(item => {
      const itemTime = new Date(item.visitTime);
      const itemDate = new Date(itemTime.getFullYear(), itemTime.getMonth(), itemTime.getDate()).getTime();

      let dateLabel: string;
      if (itemDate === today) {
        dateLabel = '今天';
      } else if (itemDate === yesterday) {
        dateLabel = '昨天';
      } else if (itemDate >= weekAgo) {
        dateLabel = '最近7天';
      } else {
        dateLabel = '更早';
      }

      if (!grouped.has(dateLabel)) {
        grouped.set(dateLabel, []);
      }
      grouped.get(dateLabel)?.push(item);
    });

    return grouped;
  }

  // 格式化时间
  formatTime(timestamp: number): string {
    const date = new Date(timestamp);
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    return `${hours}:${minutes}`;
  }

  // 格式化日期
  formatDate(timestamp: number): string {
    const date = new Date(timestamp);
    return `${date.getMonth() + 1}月${date.getDate()}日`;
  }

  build() {
    NavDestination() {
      Column() {
        // 自定义标题栏
        Row() {
          // 返回按钮
          Image($r('app.media.back'))
            .width(24)
            .height(24)
            .fillColor(this.darkMode ? '#FFFFFF' : '#000000')
            .margin({ left: 16 })
            .onClick(() => {
              this.pathStack.pop();
            })

          Text('历史记录')
            .fontSize(18)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .fontWeight(FontWeight.Medium)
            .margin({ left: 16 })

          Blank()

          // 筛选按钮
          Button(this.filterOption)
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .margin({ right: 8 })
            .onClick(() => {
              this.showFilterDialog = true;
            })

          // 编辑/完成按钮
          Button(this.isEditing ? '完成' : '编辑')
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .margin({ right: 16 })
            .onClick(() => {
              this.isEditing = !this.isEditing;
              if (!this.isEditing) {
                this.selectedItems.clear();
              }
            })
        }
        .width('100%')
        .height(56)
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
        .alignItems(VerticalAlign.Center)

        // 搜索框
        Row() {
          Image($r('app.media.search'))
            .width(16)
            .height(16)
            .fillColor('#8E8E93')
            .margin({ left: 8 })

          TextInput({ placeholder: '搜索历史记录', text: this.searchKeyword })
            .layoutWeight(1)
            .height(36)
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .placeholderColor('#8E8E93')
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .onChange((value: string) => {
              this.searchKeyword = value;
              this.loadHistory();
            })

          if (this.searchKeyword.length > 0) {
            Image($r('app.media.close'))
              .width(16)
              .height(16)
              .fillColor('#8E8E93')
              .margin({ right: 8 })
              .onClick(() => {
                this.searchKeyword = '';
                this.loadHistory();
              })
          }
        }
        .width('100%')
        .height(36)
        .margin({
          left: 16,
          right: 16,
          top: 8,
          bottom: 8
        })
        .padding({ left: 4, right: 4 })
        .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
        .borderRadius(8)

        // 批量操作栏（编辑模式下显示）
        if (this.isEditing && this.selectedItems.size > 0) {
          Row() {
            Button('删除选中 (' + this.selectedItems.size + ')')
              .fontSize(14)
              .backgroundColor('#FF3B30')
              .fontColor('#FFFFFF')
              .borderRadius(8)
              .onClick(() => {
                this.deleteSelectedItems();
              })

            Blank()

            Button('全选')
              .fontSize(14)
              .backgroundColor(Color.Transparent)
              .fontColor('#007AFF')
              .onClick(() => {
                this.selectAllItems();
              })
          }
          .width('100%')
          .height(44)
          .padding({ left: 16, right: 16 })
          .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
        }

        // 历史记录列表
        if (this.historyList.length === 0) {
          // 空状态
          Column() {
            Image($r('app.media.history'))
              .width(64)
              .height(64)
              .fillColor('#8E8E93')
              .opacity(0.5)

            Text(this.searchKeyword ? '未找到匹配的历史记录' : '暂无浏览历史')
              .fontSize(16)
              .fontColor('#8E8E93')
              .margin({ top: 16 })

            Text('您访问的网页将显示在这里')
              .fontSize(14)
              .fontColor('#8E8E93')
              .margin({ top: 8 })
              .visibility(this.searchKeyword ? Visibility.None : Visibility.Visible)
          }
          .width('100%')
          .layoutWeight(1)
          .justifyContent(FlexAlign.Center)
        } else {
          List() {
            // 按日期分组显示
            ForEach(Array.from(this.groupedHistory.keys()), (dateLabel: string) => {
              ListItemGroup({ header: this.dateHeader(dateLabel) }) {
                ForEach(this.groupedHistory.get(dateLabel) || [], (item: HistoryItem) => {
                  ListItem() {
                    Row() {
                      // 编辑模式下显示选择框
                      if (this.isEditing) {
                        Checkbox()
                          .select(this.selectedItems.has(item.id))
                          .selectedColor('#007AFF')
                          .margin({ right: 12 })
                          .onChange((value: boolean) => {
                            if (value) {
                              this.selectedItems.add(item.id);
                            } else {
                              this.selectedItems.delete(item.id);
                            }
                            // 触发刷新
                            this.selectedItems = new Set(this.selectedItems);
                          })
                      }

                      // 网站图标
                      Column() {
                        Image($r('app.media.web'))
                          .width(20)
                          .height(20)
                          .fillColor('#FFFFFF')
                      }
                      .width(40)
                      .height(40)
                      .borderRadius(8)
                      .backgroundColor(this.getIconColor(item.url))
                      .justifyContent(FlexAlign.Center)
                      .margin({ right: 12 })

                      // 标题和URL
                      Column({ space: 4 }) {
                        Text(item.title || '无标题')
                          .fontSize(16)
                          .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
                          .maxLines(1)
                          .textOverflow({ overflow: TextOverflow.Ellipsis })
                          .width('100%')

                        Row({ space: 8 }) {
                          Text(item.url)
                            .fontSize(12)
                            .fontColor('#8E8E93')
                            .maxLines(1)
                            .textOverflow({ overflow: TextOverflow.Ellipsis })
                            .layoutWeight(1)

                          if (item.visitCount > 1) {
                            Text(`访问${item.visitCount}次`)
                              .fontSize(11)
                              .fontColor('#8E8E93')
                          }
                        }
                        .width('100%')
                      }
                      .alignItems(HorizontalAlign.Start)
                      .layoutWeight(1)

                      // 访问时间
                      if (!this.isEditing) {
                        Text(this.formatTime(item.visitTime))
                          .fontSize(12)
                          .fontColor('#8E8E93')
                          .margin({ left: 8 })
                      }
                    }
                    .width('100%')
                    .padding({
                      left: 16,
                      right: 16,
                      top: 12,
                      bottom: 12
                    })
                    .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
                  }
                  .onClick(() => {
                    if (!this.isEditing) {
                      // 点击打开URL
                      this.openUrl(item.url);
                    }
                  })
                  .gesture(
                    LongPressGesture()
                      .onAction(() => {
                        this.showItemMenu(item);
                      })
                  )
                }, (item: HistoryItem) => item.id.toString())
              }
            }, (dateLabel: string) => dateLabel)
          }
          .width('100%')
          .layoutWeight(1)
          .divider({
            strokeWidth: 0.5,
            color: this.darkMode ? '#38383A' : '#E5E5EA',
            startMargin: this.isEditing ? 80 : 68,
            endMargin: 0
          })
        }

        // 底部清空按钮
        if (this.historyList.length > 0 && !this.isEditing) {
          Button('清空所有历史记录')
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .fontColor('#FF3B30')
            .width('100%')
            .height(50)
            .border({ width: { top: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
            .onClick(() => {
              this.showClearAllConfirm();
            })
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#F2F2F7')
    }
    .hideTitleBar(true)
    .bindContentCover(this.showFilterDialog, this.filterDialog(), {
      modalTransition: ModalTransition.DEFAULT
    })
  }

  // 筛选对话框
  @Builder
  filterDialog() {
    Column() {
      Column({ space: 0 }) {
        // 标题
        Row() {
          Text('时间筛选')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .layoutWeight(1)

          Button('关闭')
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .onClick(() => {
              this.showFilterDialog = false;
            })
        }
        .width('100%')
        .height(56)
        .padding({ left: 16, right: 16 })
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')

        Divider()
          .color(this.darkMode ? '#38383A' : '#E5E5EA')

        // 筛选选项
        Column() {
          this.FilterOptionItem('全部')
          this.FilterOptionItem('今天')
          this.FilterOptionItem('最近7天')
          this.FilterOptionItem('最近30天')
        }
        .width('100%')
      }
      .width('90%')
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
      .borderRadius(12)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('rgba(0, 0, 0, 0.5)')
    .justifyContent(FlexAlign.Center)
    .onClick(() => {
      this.showFilterDialog = false;
    })
  }

  // 筛选选项项
  @Builder
  FilterOptionItem(option: string) {
    Row() {
      Text(option)
        .fontSize(16)
        .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
        .layoutWeight(1)

      if (this.filterOption === option) {
        Image($r('app.media.bookmark_filled'))
          .width(20)
          .height(20)
          .fillColor('#007AFF')
      }
    }
    .width('100%')
    .height(50)
    .padding({ left: 16, right: 16 })
    .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
    .border({ width: { bottom: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
    .onClick(() => {
      this.filterOption = option;
      this.showFilterDialog = false;
      this.loadHistory();
    })
  }

  // 日期分组标题
  @Builder
  dateHeader(label: string) {
    Text(label)
      .fontSize(14)
      .fontColor('#8E8E93')
      .width('100%')
      .height(32)
      .padding({ left: 16 })
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#F2F2F7')
      .textAlign(TextAlign.Start)
  }

  // 根据URL生成图标颜色
  getIconColor(url: string): string {
    const colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#FFA07A', '#98D8C8', '#F7DC6F', '#BB8FCE', '#85C1E9'];
    let hash = 0;
    for (let i = 0; i < url.length; i++) {
      hash = url.charCodeAt(i) + ((hash << 5) - hash);
    }
    return colors[Math.abs(hash) % colors.length];
  }

  // 打开URL
  openUrl(url: string) {
    AppStorage.setOrCreate('openUrl', url);
    this.pathStack.pop();
  }

  // 显示项目菜单
  showItemMenu(item: HistoryItem) {
    AlertDialog.show({
      title: '操作',
      message: item.title,
      primaryButton: {
        value: '删除',
        fontColor: '#FF3B30',
        action: () => {
          this.deleteHistoryItem(item.id);
        }
      },
      secondaryButton: {
        value: '取消',
        action: () => {
        }
      }
    });
  }

  // 删除单个历史记录
  async deleteHistoryItem(id: number) {
    await dataManager.deleteHistory(id);
    this.loadHistory();
    promptAction.openToast({ message: '已删除' }).catch((err: BusinessError) => {
      console.error('显示Toast失败:', err);
    });
  }

  // 删除选中项
  async deleteSelectedItems() {
    if (this.selectedItems.size === 0) {
      return;
    }

    AlertDialog.show({
      title: '确认删除',
      message: `确定要删除选中的 ${this.selectedItems.size} 条历史记录吗？`,
      primaryButton: {
        value: '删除',
        fontColor: '#FF3B30',
        action: async () => {
          await dataManager.deleteHistoryBatch(Array.from(this.selectedItems));
          this.selectedItems.clear();
          this.loadHistory();
          promptAction.openToast({ message: '删除成功' }).catch((err: BusinessError) => {
            console.error('显示Toast失败:', err);
          });
        }
      },
      secondaryButton: {
        value: '取消',
        action: () => {
        }
      }
    });
  }

  // 全选
  selectAllItems() {
    this.historyList.forEach(item => {
      this.selectedItems.add(item.id);
    });
    this.selectedItems = new Set(this.selectedItems);
  }

  // 显示清空确认对话框
  showClearAllConfirm() {
    AlertDialog.show({
      title: '清空历史记录',
      message: '确定要清空所有浏览历史吗？此操作不可恢复。',
      primaryButton: {
        value: '清空',
        fontColor: '#FF3B30',
        action: async () => {
          await dataManager.clearAllHistory();
          this.loadHistory();
          promptAction.openToast({ message: '历史记录已清空' }).catch((err: BusinessError) => {
            console.error('显示Toast失败:', err);
          });
        }
      },
      secondaryButton: {
        value: '取消',
        action: () => {
        }
      }
    });
  }
}
