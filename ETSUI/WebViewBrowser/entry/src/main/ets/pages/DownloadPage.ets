/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import { DownloadItem, DownloadManager, downloadManager, DownloadState, DownloadListener } from '../manager/DownloadManager';
import { promptAction } from '@kit.ArkUI';

@Builder
export function DownloadPageBuilder() {
  DownloadPage();
}

@Component
export struct DownloadPage {
  pathStack: NavPathStack = AppStorage.get('pathStack') as NavPathStack;
  @State downloadList: DownloadItem[] = [];
  @StorageLink('darkMode') darkMode: boolean = false;

  // 监听器引用，用于销毁时移除
  private listener: DownloadListener | null = null;

  aboutToAppear() {
    this.refreshList();

    const listener: DownloadListener = {
      onProgress: (item: DownloadItem) => {
        this.updateItem(item);
      },
      onStatusChange: (item: DownloadItem) => {
        this.refreshList();
      },
      onError: (item: DownloadItem, msg: string) => {
        promptAction.showToast({ message: `下载错误: ${msg}` });
        this.refreshList();
      }
    };

    this.listener = listener;

    downloadManager.addListener(this.listener);
  }

  aboutToDisappear() {
    if (this.listener) {
      downloadManager.removeListener(this.listener);
    }
  }

  refreshList() {
    this.downloadList = downloadManager.getDownloadList();
  }

  // 局部更新优化列表渲染
  updateItem(updatedItem: DownloadItem) {
    const index = this.downloadList.findIndex(i => i.id === updatedItem.id);
    if (index !== -1) {
      this.downloadList.splice(index, 1, updatedItem); // 创建新对象触发更新
    }
  }

  build() {
    NavDestination() {
      Column() {
        // 标题栏
        Row() {
          Image($r('app.media.back'))
            .width(24)
            .height(24)
            .fillColor(this.darkMode ? '#FFFFFF' : '#000000')
            .margin({ left: 16 })
            .onClick(() => {
              this.pathStack.pop();
            })

          Text('下载管理')
            .fontSize(18)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .fontWeight(FontWeight.Medium)
            .margin({ left: 16 })

          Blank()

          // 清空历史按钮
          if (this.downloadList.length > 0) {
            Text('清空')
              .fontSize(16)
              .fontColor('#FF3B30')
              .margin({ right: 16 })
              .onClick(() => {
                this.showClearConfirm();
              })
          }
        }
        .width('100%')
        .height(56)
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
        .alignItems(VerticalAlign.Center)

        // 列表区域
        if (this.downloadList.length === 0) {
          this.EmptyView()
        } else {
          List({ space: 12 }) {
            ForEach(this.downloadList, (item: DownloadItem) => {
              ListItem() {
                this.DownloadItemCard(item)
              }
            }, (item: DownloadItem) => item.id + '_' + item.state) // Key包含状态以确保重绘
          }
          .width('100%')
          .layoutWeight(1)
          .padding(16)
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor(this.darkMode ? '#000000' : '#F2F2F7')
    }
    .hideTitleBar(true)
  }

  @Builder
  EmptyView() {
    Column({ space: 16 }) {
      Image($r('app.media.web'))
        .width(64)
        .height(64)
        .fillColor('#8E8E93')
        .opacity(0.5)

      Text('暂无下载任务')
        .fontSize(16)
        .fontColor('#8E8E93')
    }
    .width('100%')
    .layoutWeight(1)
    .justifyContent(FlexAlign.Center)
  }

  @Builder
  DownloadItemCard(item: DownloadItem) {
    Column({ space: 8 }) {
      // 头部：文件名和状态
      Row() {
        Image($r('app.media.web')) // 文件图标占位
          .width(32)
          .height(32)
          .fillColor('#007AFF')
          .margin({ right: 12 })

        Column({ space: 4 }) {
          Text(item.fileName)
            .fontSize(16)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
            .width('100%')

          Row({ space: 6 }) {
            Text(this.getStatusText(item))
              .fontSize(12)
              .fontColor(this.getStatusColor(item.state))

            if(item.isSavedToTarget) {
              Text('已保存')
                .fontSize(10)
                .fontColor("#FFFFFF")
                .backgroundColor("324123")
                .padding({left: 4, right: 4, top: 1, bottom: 1})
                .borderRadius(4)
            }
          }
        }
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)

        // 操作按钮
        this.ControlButtons(item)
      }
      .width('100%')
      .onClick(() => {
        if(item.state == DownloadState.COMPLETED) {
          this.openFile(item)
        }
      })

      // 进度条 (仅在下载中或暂停时显示)
      if (item.state === DownloadState.DOWNLOADING || item.state === DownloadState.PAUSED) {
        Column({ space: 4 }) {
          Progress({ value: item.progress, total: 100 })
            .color('#007AFF')
            .backgroundColor(this.darkMode ? '#48484A' : '#E5E5EA')
            .height(4)

          Row() {
            Text(DownloadManager.formatFileSize(item.receivedSize) + ' / ' + DownloadManager.formatFileSize(item.totalSize))
              .fontSize(10)
              .fontColor('#8E8E93')

            Blank()

            if (item.speed && item.state === DownloadState.DOWNLOADING) {
              Text(DownloadManager.formatFileSize(item.speed) + '/s')
                .fontSize(10)
                .fontColor('#8E8E93')
            }
          }
          .width('100%')
        }
      }
    }
    .width('100%')
    .padding(16)
    .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
    .borderRadius(12)
    .shadow({ radius: 4, color: '#00000010', offsetY: 1 })
  }

  async openFile(item: DownloadItem) {
    const result = await downloadManager.openDownloadItem(item);

    if(!result) {
      promptAction.showToast({message: "Could't Open!!!"});
    }
  }

  @Builder
  ControlButtons(item: DownloadItem) {
    if(item.state === DownloadState.COMPLETED) {
      Button('打开')
        .fontSize(12)
        .height(28)
        .backgroundColor('#007AFF')
        .onClick(() => {
          this.openFile(item);
        })
    }

    Row({ space: 8 }) {
      if (item.state === DownloadState.DOWNLOADING) {
        Button('暂停')
          .fontSize(12)
          .height(28)
          .backgroundColor('#FF9500')
          .onClick(() => {
            downloadManager.operateTask(item.id, 'pause');
          })
      } else if (item.state === DownloadState.PAUSED || item.state === DownloadState.FAILED) {
        Button('继续')
          .fontSize(12)
          .height(28)
          .backgroundColor('#34C759')
          .onClick(() => {
            downloadManager.operateTask(item.id, 'resume');
          })
      }

      // 删除/取消按钮
      Button(item.state === DownloadState.COMPLETED ? '删除' : '取消')
        .fontSize(12)
        .height(28)
        .backgroundColor('#FF3B30')
        .onClick(() => {
          downloadManager.operateTask(item.id, 'cancel');
          this.refreshList(); // 强制刷新
        })
    }
  }

  showClearConfirm() {
    AlertDialog.show({
      title: '清空记录',
      message: '确定要清空所有下载记录吗？',
      primaryButton: {
        value: '仅清空记录',
        action: () => {
          downloadManager.clearHistory(false);
          this.refreshList();
        }
      },
      secondaryButton: {
        value: '同时删除文件',
        fontColor: '#FF3B30',
        action: () => {
          downloadManager.clearHistory(true);
          this.refreshList();
        }
      }
    });
  }

  getStatusText(item: DownloadItem): string {
    switch (item.state) {
      case DownloadState.QUEUED: return '等待中...';
      case DownloadState.DOWNLOADING: return '下载中';
      case DownloadState.PAUSED: return '已暂停';
      case DownloadState.COMPLETED: return `已完成 • ${DownloadManager.formatFileSize(item.totalSize)}`;
      case DownloadState.FAILED: return '下载失败';
      case DownloadState.CANCELED: return '已取消';
      default: return '未知状态';
    }
  }

  getStatusColor(state: DownloadState): string {
    switch (state) {
      case DownloadState.DOWNLOADING: return '#007AFF';
      case DownloadState.COMPLETED: return '#34C759';
      case DownloadState.FAILED: return '#FF3B30';
      case DownloadState.PAUSED: return '#FF9500';
      default: return '#8E8E93';
    }
  }
}