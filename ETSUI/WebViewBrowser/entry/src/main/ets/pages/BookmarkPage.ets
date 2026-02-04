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

// Builder函数，用于Navigation路由
@Builder
export function BookmarkPageBuilder() {
  BookmarkPage();
}

@Component
export struct BookmarkPage {
  // 获取全局导航栈
  pathStack: NavPathStack = AppStorage.get('pathStack') as NavPathStack;

  // --- 状态管理 ---
  @State currentParentId: number = 0;
  @State pathHistory: number[] = [];
  @State bookmarkList: BookmarkItem[] = [];

  @State searchKeyword: string = '';
  @State isEditing: boolean = false;
  @State selectedItems: Set<number> = new Set();
  @StorageLink('darkMode') darkMode: boolean = false;
  @StorageLink('bookmarkChanged') storageBookmarkChanged: number = 0;

  // --- 编辑/修改相关状态 ---
  @State showEditDialog: boolean = false;
  @State editingItem: BookmarkItem | null = null;
  @State editTitle: string = '';
  @State editUrl: string = '';

  // --- 弹窗控制器声明 ---
  private moveDialogController: CustomDialogController | null = null;

  // 新建文件夹弹窗控制器
  private createFolderController: CustomDialogController | null = new CustomDialogController({
    builder: FolderInputDialog({
      onConfirm: async (name: string) => {
        await dataManager.addFolder(name, this.currentParentId);
        this.refreshData(); // 立刻刷新
      }
    }),
    autoCancel: true,
    alignment: DialogAlignment.Center,
    customStyle: true
  })

  // 新建书签弹窗控制器
  private createBookmarkController: CustomDialogController | null = new CustomDialogController({
    builder: AddBookmarkDialog({
      onConfirm: async (title: string, url: string) => {
        await dataManager.addBookmark(url, title, this.currentParentId);
        this.refreshData(); // 立刻刷新
      }
    }),
    autoCancel: true,
    alignment: DialogAlignment.Center,
    customStyle: true
  })

  aboutToAppear() {
    this.refreshData();
  }

  // --- 立即刷新---
  refreshData() {
    if (this.searchKeyword.trim()) {
      // 使用展开运算符 [...] 强制改变数组引用，触发视图Diff
      this.bookmarkList = [...dataManager.searchBookmarks(this.searchKeyword)];
    } else {
      this.bookmarkList = [...dataManager.getBookmarksByParent(this.currentParentId)];
    }
    this.storageBookmarkChanged = Date.now(); // 同步全局状态
  }

  handleBack() {
    if (this.pathHistory.length > 0) {
      this.currentParentId = this.pathHistory.pop()!;
      this.refreshData();
    } else {
      this.pathStack.pop();
    }
  }

  // 动态创建并打开移动对话框
  openMoveDialog(ids: number[]) {
    this.moveDialogController = new CustomDialogController({
      builder: MoveToDialog({
        itemIdsToMove: ids,
        onConfirm: () => {
          this.isEditing = false;
          this.selectedItems.clear();
          this.selectedItems = new Set();
          this.refreshData(); // 移动完成后刷新
        }
      }),
      autoCancel: true,
      alignment: DialogAlignment.Bottom,
      customStyle: true
    });
    this.moveDialogController.open();
  }

  // 修改选中的单项
  renameSelectedItem() {
    if (this.selectedItems.size !== 1) return;
    const selectedId = Array.from(this.selectedItems)[0];
    const item = this.bookmarkList.find(i => i.id === selectedId);
    if (item) {
      this.editingItem = item;
      this.editTitle = item.title;
      this.editUrl = item.url || '';
      this.showEditDialog = true;
    }
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
              this.handleBack();
            })

          Text(this.currentParentId === 0 ? '书签' : '返回上级')
            .fontSize(18)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .fontWeight(FontWeight.Medium)
            .margin({ left: 16 })

          Blank()

          if (!this.isEditing) {
            Button('新建文件夹')
              .fontSize(12)
              .backgroundColor(Color.Transparent)
              .fontColor('#007AFF')
              .onClick(() => this.createFolderController?.open())
            Button('新建书签')
              .fontSize(12)
              .backgroundColor(Color.Transparent)
              .fontColor('#007AFF')
              .onClick(() => this.createBookmarkController?.open())
          }

          Button(this.isEditing ? '完成' : '编辑')
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .margin({ right: 16, left: 8 })
            .onClick(() => {
              this.isEditing = !this.isEditing;
              if (!this.isEditing) {
                this.selectedItems.clear();
                this.selectedItems = new Set();
              }
            })
        }
        .width('100%')
        .height(56)
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')

        // 搜索框
        Search({ value: this.searchKeyword, placeholder: '搜索书签...' })
          .height(44)
          .margin(12)
          .onChange((value: string) => {
            this.searchKeyword = value;
            this.refreshData();
          })

        // 批量操作栏（编辑模式下显示）
        if (this.isEditing) {
          Row({ space: 12 }) {
            Button('删除选中 (' + this.selectedItems.size + ')')
              .fontSize(14)
              .backgroundColor('#FF3B30')
              .fontColor('#FFFFFF')
              .borderRadius(8)
              .enabled(this.selectedItems.size > 0)
              .onClick(() => {
                this.deleteSelectedItems();
              })

            Button('修改')
              .fontSize(12)
              .backgroundColor('#007AFF')
              .borderRadius(8)
              .enabled(this.selectedItems.size === 1)
              .onClick(() => {
                this.renameSelectedItem()
              })

            Button('移动到')
              .fontSize(12)
              .backgroundColor('#007AFF')
              .borderRadius(8)
              .enabled(this.selectedItems.size > 0)
              .onClick(() => {
                this.openMoveDialog(Array.from(this.selectedItems));
              })

            Blank()

            Button(this.isAllSelected() ? '取消全选' : '全选')
              .fontSize(12)
              .backgroundColor(Color.Transparent)
              .fontColor('#007AFF')
              .onClick(() => {
                this.toggleSelectAll();
              })
          }
          .width('100%')
          .padding({ left: 16, right: 16, bottom: 8 })
          .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
        }

        // 书签列表
        if (this.bookmarkList.length === 0) {
          this.emptyState();
        } else {
          List() {
            ForEach(this.bookmarkList, (item: BookmarkItem) => {
              ListItem() {
                this.bookmarkRow(item);
              }
              .onClick(() => {
                if (this.isEditing) {
                  this.toggleSelection(item.id);
                  return;
                }
                if (item.isFolder) {
                  this.pathHistory.push(this.currentParentId);
                  this.currentParentId = item.id;
                  this.refreshData();
                } else if (item.url) {
                  this.openUrl(item.url);
                }
              })
              .gesture(LongPressGesture()
                .onAction(() => this.showItemMenu(item)))
            }, (item: BookmarkItem) => item.id.toString() + item.title) // Key包含title确保修改刷新
          }
          .layoutWeight(1)
          .divider({ strokeWidth: 0.5, color: this.darkMode ? '#38383A' : '#E5E5EA', startMargin: 70 })
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#F2F2F7')
    }
    .hideTitleBar(true)
    .bindContentCover(this.showEditDialog, this.editDialogBuilder())
  }

  @Builder
  bookmarkRow(item: BookmarkItem) {
    Row() {
      if (this.isEditing) {
        Checkbox({ name: item.id.toString() })
          .select(this.selectedItems.has(item.id))
          .onChange((v: boolean) => {
            // 增加判断条件，只有当视觉状态与数据状态不一致时（即手动点击）才触发逻辑，防止渲染死循环
            if (v !== this.selectedItems.has(item.id)) {
              this.toggleSelection(item.id);
            }
          })
          .margin({ right: 10 })
      }

      Column() {
        Image($r('app.media.bookmark_filled'))
          .width(22)
          .height(22)
          .fillColor('#FFFFFF')
      }
      .width(44)
      .height(44)
      .borderRadius(8)
      .backgroundColor(item.isFolder ? '#FFCC00' : this.getIconColor(item.url || ''))
      .justifyContent(FlexAlign.Center).margin({ right: 12 })

      Column({ space: 4 }) {
        Text(item.title)
          .fontSize(16)
          .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
          .maxLines(1).textOverflow({ overflow: TextOverflow.Ellipsis })
        if (!item.isFolder) {
          Text(item.url)
            .fontSize(12)
            .fontColor('#8E8E93')
            .maxLines(1).textOverflow({ overflow: TextOverflow.Ellipsis })
        }
      }.layoutWeight(1).alignItems(HorizontalAlign.Start)

      if (item.isFolder && !this.isEditing) {
        Image($r('app.media.forward'))
          .width(12)
          .height(12)
          .fillColor('#8E8E93')
      }
    }
    .padding(16)
    .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
  }

  @Builder
  emptyState() {
    Column() {
      Image($r('app.media.bookmark'))
        .width(64)
        .opacity(0.3)
      Text('此目录暂无内容')
        .fontColor('#8E8E93')
        .margin({ top: 10 })
    }
    .width('100%')
    .layoutWeight(1)
    .justifyContent(FlexAlign.Center)
  }

  showItemMenu(item: BookmarkItem) {
    promptAction.showActionMenu({
      title: item.title,
      buttons: [
        { text: '修改/编辑', color: '#007AFF' },
        { text: '移动', color: '#007AFF' },
        { text: '删除', color: '#FF3B30' }
      ]
    }).then((res) => {
      if (res.index === 0) {
        this.editingItem = item;
        this.editTitle = item.title;
        this.editUrl = item.url || '';
        this.showEditDialog = true;
      } else if (res.index === 1) {
        this.openMoveDialog([item.id]);
      } else if (res.index === 2) {
        this.deleteItem(item);
      }
    })
  }

  @Builder
  editDialogBuilder() {
    Column() {
      Column({ space: 15 }) {
        // 标题栏
        Row() {
          Text('取消')
            .fontSize(16)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .onClick(() => {
              this.showEditDialog = false;
            })
          Text(this.editingItem?.isFolder ? '修改' : '编辑')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .layoutWeight(1)
            .textAlign(TextAlign.Center)
          Text('保存')
            .fontSize(16)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .onClick(() => {
              this.saveEdit();
            })
        }
        .width('100%')
        .padding(20)
        // 书签名称输入
        TextInput({ text: this.editTitle, placeholder: '名称' })
          .width('90%')
          .onChange(v => this.editTitle = v)

        if (this.editingItem && !this.editingItem.isFolder) {
          TextInput({ text: this.editUrl, placeholder: 'URL' })
            .width('90%')
            .onChange(v => this.editUrl = v)
        }
      }
      .width('90%')
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
      .borderRadius(15)
      .padding({ bottom: 20 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
  }

  async saveEdit() {
    if (this.editingItem && this.editTitle.trim()) {
      await dataManager.updateBookmark(
        this.editingItem.id,
        this.editTitle,
        this.editUrl
      );
      this.showEditDialog = false;
      this.refreshData();
    }
  }
  // --- 判断当前可见列表是否已全部选中 ---
  isAllSelected(): boolean {
    if (this.bookmarkList.length === 0) return false;
    // 检查可见列表中的每一个 ID 是否都在 selectedItems 集合中
    return this.bookmarkList.every(item => this.selectedItems.has(item.id));
  }

  toggleSelection(id: number) {
    if (this.selectedItems.has(id)) {
      this.selectedItems.delete(id);
    } else {
      this.selectedItems.add(id);
    }
    this.selectedItems = new Set(this.selectedItems); // 强制刷新Set
  }

  toggleSelectAll() {
    if (this.isAllSelected()){
      this.bookmarkList.forEach(item => this.selectedItems.delete(item.id));
    } else {
      this.bookmarkList.forEach(item => this.selectedItems.add(item.id));
    }
    this.selectedItems = new Set(this.selectedItems);
  }

  async deleteSelectedItems() {
    for (const id of this.selectedItems) {
      await dataManager.deleteBookmark(id);
    }
    this.selectedItems.clear();
    this.selectedItems = new Set();
    this.isEditing = false;
    this.refreshData();
  }

  async deleteItem(item: BookmarkItem) {
    await dataManager.deleteBookmark(item.id);
    this.refreshData();
  }

  openUrl(url: string) {
    AppStorage.setOrCreate('openUrl', url);
    this.pathStack.pop();
  }

  getIconColor(url: string): string {
    const colors = ['#007AFF', '#34C759', '#FF9500', '#AF52DE'];
    return colors[url.length % colors.length];
  }
}

// --- 弹窗组件集 ---

@CustomDialog
struct MoveToDialog {
  controller: CustomDialogController
  itemIdsToMove: number[] = []
  onConfirm: () => void = () => {}
  @State currentId: number = 0
  @State folderList: BookmarkItem[] = []
  @State navStack: number[] = []

  aboutToAppear() { this.refresh(); }
  refresh() {
    this.folderList = dataManager.getBookmarksByParent(this.currentId)
      .filter(i => i.isFolder && !this.itemIdsToMove.includes(i.id));
  }

  build() {
    Column() {
      Text('移动到...')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .margin(20)
      List() {
        if (this.currentId === 0) {
          ListItem() {
            Row() {
              Image($r('app.media.home'))
                .width(20)
                .margin({ right: 10 })
              Text('书签根目录')
                .fontSize(16)
            }
            .width('100%')
            .padding(15)
            .backgroundColor('#F2F2F7')
          }
        }
        ForEach(this.folderList, (f: BookmarkItem) => {
          ListItem() {
            Row() {
              Image($r('app.media.bookmark_filled'))
                .width(20)
                .fillColor('#FFCC00')
                .margin({ right: 10 })
              Text(f.title)
                .fontSize(16)
              Blank()
              Image($r('app.media.forward'))
                .width(12)
                .opacity(0.5)
            }
            .width('100%')
            .padding(15)
            .onClick(() => {
              this.navStack.push(this.currentId);
              this.currentId = f.id;
              this.refresh();
            })
          }
        })
      }.layoutWeight(1)
      Row({ space: 12 }) {
        if (this.navStack.length > 0) {
          Button('上级')
            .onClick(() => { this.currentId = this.navStack.pop()!; this.refresh(); })
            .backgroundColor('#E5E5EA')
            .fontColor('#000000')
        }
        Blank()
        Button('取消')
          .onClick(() => this.controller.close())
        Button('移动此处').onClick(async () => {
          for (const id of this.itemIdsToMove){
            await dataManager.moveNode(id, this.currentId);
          }
          this.onConfirm();
          this.controller.close();
        })
          .backgroundColor('#007AFF')
      }
      .width('100%')
      .padding(16)
    }
    .width('100%')
    .height('60%')
    .backgroundColor(Color.White)
    .borderRadius({ topLeft: 20, topRight: 20 })
  }
}

@CustomDialog
struct FolderInputDialog {
  controller: CustomDialogController
  @State folderName: string = ''
  onConfirm: (name: string) => void = () => {}
  build() {
    Column({ space: 15 }) {
      Text('新建文件夹')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 10 })
      TextInput({ placeholder: '文件夹名称', text: this.folderName })
        .width('90%')
        .onChange(v => this.folderName = v)
      Row({ space: 20 }) {
        Button('取消')
          .onClick(() => this.controller.close())
          .fontColor('#666666')
        Button('确定')
          .onClick(() => {
            if (this.folderName.trim()) {
              this.onConfirm(this.folderName);
              this.controller.close();
            }})
          .fontColor('#007AFF')
      }
      .margin({ bottom: 10 })
    }
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(16)
  }
}

@CustomDialog
struct AddBookmarkDialog {
  controller: CustomDialogController
  @State title: string = ''
  @State url: string = ''
  onConfirm: (title: string, url: string) => void = () => {}
  build() {
    Column({ space: 15 }) {
      Text('添加书签')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 10 })
      TextInput({ placeholder: '标题', text: this.title })
        .width('90%')
        .onChange(v => this.title = v)
      TextInput({ placeholder: 'URL (https://...)', text: this.url })
        .width('90%')
        .onChange(v => this.url = v)
      Row({ space: 20 }) {
        Button('取消')
          .onClick(() => this.controller.close())
          .fontColor('#666666')
        Button('创建')
          .onClick(() => {
            if (this.title.trim() && this.url.trim()) {
              this.onConfirm(this.title, this.url);
              this.controller.close();
            } })
          .fontColor('#007AFF')
      }
      .margin({ bottom: 10 })
    }
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(16)
  }
}