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
import { dataManager, BookmarkItem } from '../manager/DataManager';
import { BusinessError } from '@kit.BasicServicesKit';

// Builder函数，用于Navigation路由
@Builder
export function BookmarkPageBuilder() {
  BookmarkPage();
}

@Component
export struct BookmarkPage {
  // 获取全局NavPathStack
  pathStack: NavPathStack = AppStorage.get('pathStack') as NavPathStack;
  // 书签列表
  @State bookmarkList: BookmarkItem[] = [];
  @State groupedBookmarks: Map<string, BookmarkItem[]> = new Map();
  @State searchKeyword: string = '';
  @State isEditing: boolean = false;
  @State selectedItems: Set<number> = new Set();
  @State currentFolder: string = '全部'; // 当前文件夹筛选
  @State showFolderDialog: boolean = false;
  // 编辑书签对话框
  @State showEditDialog: boolean = false;
  @State editingBookmark: BookmarkItem | null = null;
  @State editTitle: string = '';
  @State editFolder: string = '';
  // 深色模式
  @StorageLink('darkMode') darkMode: boolean = false;

  aboutToAppear() {
    this.loadBookmarks();
  }

  // 加载书签
  loadBookmarks() {
    let allBookmarks: BookmarkItem[];

    if (this.searchKeyword.trim()) {
      allBookmarks = dataManager.searchBookmarks(this.searchKeyword);
    } else {
      allBookmarks = dataManager.getBookmarkList();
    }

    // 按文件夹筛选
    if (this.currentFolder !== '全部') {
      allBookmarks = allBookmarks.filter(item =>
      (item.folder || '默认') === this.currentFolder
      );
    }

    this.bookmarkList = allBookmarks;

    // 按文件夹分组
    this.groupedBookmarks = new Map();
    allBookmarks.forEach(item => {
      const folder = item.folder || '默认';
      if (!this.groupedBookmarks.has(folder)) {
        this.groupedBookmarks.set(folder, []);
      }
      this.groupedBookmarks.get(folder)?.push(item);
    });
  }

  // 获取所有文件夹
  getAllFolders(): string[] {
    const folders = new Set<string>(['全部', '默认']);
    dataManager.getBookmarkList().forEach(item => {
      if (item.folder) {
        folders.add(item.folder);
      }
    });
    return Array.from(folders);
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

          Text('书签')
            .fontSize(18)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .fontWeight(FontWeight.Medium)
            .margin({ left: 16 })

          Blank()

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

          TextInput({ placeholder: '搜索书签', text: this.searchKeyword })
            .layoutWeight(1)
            .height(36)
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .placeholderColor('#8E8E93')
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .onChange((value: string) => {
              this.searchKeyword = value;
              this.loadBookmarks();
            })

          if (this.searchKeyword.length > 0) {
            Image($r('app.media.close'))
              .width(16)
              .height(16)
              .fillColor('#8E8E93')
              .margin({ right: 8 })
              .onClick(() => {
                this.searchKeyword = '';
                this.loadBookmarks();
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

        // 书签列表
        if (this.bookmarkList.length === 0) {
          // 空状态
          Column() {
            Image($r('app.media.bookmark'))
              .width(64)
              .height(64)
              .fillColor('#8E8E93')
              .opacity(0.5)

            Text(this.searchKeyword ? '未找到匹配的书签' : '暂无书签')
              .fontSize(16)
              .fontColor('#8E8E93')
              .margin({ top: 16 })

            Text('在浏览页面时点击书签按钮可添加书签')
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
            ForEach(this.bookmarkList, (item: BookmarkItem) => {
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
                    Image($r('app.media.bookmark_filled'))
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

                    Text(item.url)
                      .fontSize(12)
                      .fontColor('#8E8E93')
                      .maxLines(1)
                      .textOverflow({ overflow: TextOverflow.Ellipsis })
                      .width('100%')
                  }
                  .alignItems(HorizontalAlign.Start)
                  .layoutWeight(1)

                  // 添加日期
                  if (!this.isEditing) {
                    Text(this.formatDate(item.createTime))
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
            }, (item: BookmarkItem) => item.id.toString())
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
        if (this.bookmarkList.length > 0 && !this.isEditing) {
          Button('清空所有书签')
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
    // 编辑书签对话框
    .bindContentCover(this.showEditDialog, this.editBookmarkDialog(), {
      modalTransition: ModalTransition.DEFAULT
    })
  }

  // 编辑书签对话框
  @Builder
  editBookmarkDialog() {
    Column() {
      Column() {
        // 标题栏
        Row() {
          Button('取消')
            .fontSize(16)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .onClick(() => {
              this.showEditDialog = false;
            })

          Text('编辑书签')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .layoutWeight(1)
            .textAlign(TextAlign.Center)

          Button('保存')
            .fontSize(16)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .onClick(() => {
              this.saveBookmarkEdit();
            })
        }
        .width('100%')
        .height(56)
        .padding({ left: 16, right: 16 })

        // 书签名称输入
        Column({ space: 8 }) {
          Text('名称')
            .fontSize(14)
            .fontColor('#8E8E93')
            .width('100%')

          TextInput({ text: this.editTitle })
            .width('100%')
            .height(44)
            .fontSize(16)
            .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
            .borderRadius(8)
            .onChange((value: string) => {
              this.editTitle = value;
            })
        }
        .width('100%')
        .padding(16)

        // 书签URL（只读）
        Column({ space: 8 }) {
          Text('网址')
            .fontSize(14)
            .fontColor('#8E8E93')
            .width('100%')

          Text(this.editingBookmark?.url || '')
            .fontSize(14)
            .fontColor('#8E8E93')
            .width('100%')
            .padding(12)
            .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
            .borderRadius(8)
            .maxLines(2)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
        }
        .width('100%')
        .padding({ left: 16, right: 16 })
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
      this.showEditDialog = false;
    })
  }

  // 根据URL生成图标颜色
  getIconColor(url: string): string {
    const colors = ['#007AFF', '#34C759', '#FF9500', '#FF3B30', '#AF52DE', '#5856D6'];
    let hash = 0;
    for (let i = 0; i < url.length; i++) {
      hash = url.charCodeAt(i) + ((hash << 5) - hash);
    }
    return colors[Math.abs(hash) % colors.length];
  }

  // 打开URL
  openUrl(url: string) {
    // 将URL传递给主页面并返回
    AppStorage.setOrCreate('openUrl', url);
    this.pathStack.pop();
  }

  // 显示单条记录菜单
  showItemMenu(item: BookmarkItem) {
    promptAction.showActionMenu({
      title: item.title,
      buttons: [
        { text: '打开', color: '#007AFF' },
        { text: '编辑', color: '#007AFF' },
        { text: '复制链接', color: '#007AFF' },
        { text: '删除', color: '#FF3B30' }
      ]
    }).then((result) => {
      switch (result.index) {
        case 0:
          this.openUrl(item.url);
          break;
        case 1:
          this.showEditBookmarkDialog(item);
          break;
        case 2:
          // TODO: 复制到剪贴板
          promptAction.openToast({ message: '链接已复制' }).catch((err: BusinessError) => {
            console.error('显示Toast失败:', err);
          });
          break;
        case 3:
          this.deleteItem(item);
          break;
      }
    }).catch(() => {
      // 用户取消
    });
  }

  // 显示编辑书签对话框
  showEditBookmarkDialog(item: BookmarkItem) {
    this.editingBookmark = item;
    this.editTitle = item.title;
    this.showEditDialog = true;
  }

  // 保存书签编辑
  async saveBookmarkEdit() {
    if (this.editingBookmark && this.editTitle.trim()) {
      await dataManager.updateBookmark(this.editingBookmark.id, this.editTitle.trim());
      this.loadBookmarks();
      this.showEditDialog = false;
      promptAction.openToast({ message: '已保存' }).catch((err: BusinessError) => {
        console.error('显示Toast失败:', err);
      });
    }
  }

  // 删除单条记录
  async deleteItem(item: BookmarkItem) {
    await dataManager.deleteBookmark(item.id);
    this.loadBookmarks();
    // 通知主页面更新书签状态
    AppStorage.setOrCreate('bookmarkChanged', Date.now());
    promptAction.openToast({ message: '已删除' }).catch((err: BusinessError) => {
      console.error('显示Toast失败:', err);
    });
  }

  // 删除选中项
  async deleteSelectedItems() {
    for (const id of this.selectedItems) {
      await dataManager.deleteBookmark(id);
    }
    this.selectedItems.clear();
    this.loadBookmarks();
    this.isEditing = false;
    // 通知主页面更新书签状态
    AppStorage.setOrCreate('bookmarkChanged', Date.now());
    promptAction.openToast({ message: '已删除选中书签' }).catch((err: BusinessError) => {
      console.error('显示Toast失败:', err);
    });
  }

  // 全选
  selectAllItems() {
    for (const item of this.bookmarkList) {
      this.selectedItems.add(item.id);
    }
    this.selectedItems = new Set(this.selectedItems);
  }

  // 显示清空确认
  showClearAllConfirm() {
    promptAction.showDialog({
      title: '清空书签',
      message: '确定要清空所有书签吗？此操作无法撤销。',
      buttons: [
        { text: '取消', color: '#007AFF' },
        { text: '清空', color: '#FF3B30' }
      ]
    }).then((result) => {
      if (result.index === 1) {
        this.clearAllBookmarks();
      }
    });
  }

  // 清空所有书签
  async clearAllBookmarks() {
    await dataManager.clearAllBookmarks();
    this.loadBookmarks();
    // 通知主页面更新书签状态
    AppStorage.setOrCreate('bookmarkChanged', Date.now());
    promptAction.openToast({ message: '书签已清空' }).catch((err: BusinessError) => {
      console.error('显示Toast失败:', err);
    });
  }
}
