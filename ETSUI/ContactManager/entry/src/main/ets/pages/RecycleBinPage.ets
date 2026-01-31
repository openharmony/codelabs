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
import promptAction from '@ohos.promptAction';
import { Routes } from '../constants/Routes';
import { DeletedContact } from '../model/DeletedContact';
import { RecycleBinService } from '../service/RecycleBinService';
import { TimeUtils } from '../utils/TimeUtils';

/**
 * 回收站统计信息接口
 */
interface RecycleBinStatistics {
  total: number;    // 总计
  valid: number;    // 可恢复
  expired: number;  // 已过期
}

@Entry
struct RecycleBinPage {
  @State deletedContacts: DeletedContact[] = [];
  @State isLoading: boolean = true;
  @State statistics: RecycleBinStatistics = {
    total: 0,
    valid: 0,
    expired: 0
  };

  async aboutToAppear(): Promise<void> {
    await RecycleBinService.init();
    await this.loadData();
  }

  onPageShow(): void {
    this.loadData();
  }

  async loadData(): Promise<void> {
    this.isLoading = true;
    try {
      // 1. 加载所有已删除的联系人
      this.deletedContacts = await RecycleBinService.getAllDeletedContacts();
      
      // 2. 获取统计信息
      this.statistics = await RecycleBinService.getStatistics();
      
      // 3. 自动清理过期记录
      await RecycleBinService.cleanExpiredContacts();
      
      // 重新加载（清理后）
      this.deletedContacts = await RecycleBinService.getAllDeletedContacts();
      this.statistics = await RecycleBinService.getStatistics();
    } catch (err) {
      console.error('[RecycleBinPage] 加载失败:', err);
      promptAction.showToast({ message: '加载失败' });
    } finally {
      this.isLoading = false;
    }
  }

  /**
   * 恢复联系人
   */
  async handleRestore(backupId: string): Promise<void> {
    try {
      const success = await RecycleBinService.restoreContact(backupId);
      
      if (success) {
        promptAction.showToast({ message: '恢复成功' });
        
        // 触发列表刷新
        PersistentStorage.PersistProp('contactListRefreshFlag', Date.now());
        
        // 重新加载回收站数据
        await this.loadData();
      } else {
        promptAction.showToast({ message: '恢复失败' });
      }
    } catch (err) {
      console.error('[RecycleBinPage] 恢复失败:', err);
      promptAction.showToast({ message: '恢复失败' });
    }
  }

  /**
   * 永久删除
   */
  handlePermanentDelete(backupId: string, contactName: string): void {
    AlertDialog.show({
      title: '永久删除',
      message: `确定要永久删除"${contactName}"吗？此操作不可恢复！`,
      primaryButton: {
        value: '取消',
        action: (): void => {}
      },
      secondaryButton: {
        value: '永久删除',
        fontColor: Color.Red,
        action: async (): Promise<void> => {
          try {
            await RecycleBinService.removeDeletedContact(backupId);
            promptAction.showToast({ message: '已永久删除' });
            await this.loadData();
          } catch (err) {
            console.error('[RecycleBinPage] 永久删除失败:', err);
            promptAction.showToast({ message: '删除失败' });
          }
        }
      }
    });
  }

  /**
   * 清空回收站
   */
  handleClearAll(): void {
    if (this.deletedContacts.length === 0) {
      promptAction.showToast({ message: '回收站已为空' });
      return;
    }

    AlertDialog.show({
      title: '清空回收站',
      message: `确定要清空回收站吗？将永久删除 ${this.deletedContacts.length} 个联系人，此操作不可恢复！`,
      primaryButton: {
        value: '取消',
        action: (): void => {}
      },
      secondaryButton: {
        value: '清空',
        fontColor: Color.Red,
        action: async (): Promise<void> => {
          try {
            await RecycleBinService.clearRecycleBin();
            promptAction.showToast({ message: '已清空回收站' });
            await this.loadData();
          } catch (err) {
            console.error('[RecycleBinPage] 清空失败:', err);
            promptAction.showToast({ message: '清空失败' });
          }
        }
      }
    });
  }

  build(): void {
    Navigation() {
      Column() {
        // 统计信息栏
        Row() {
          Column() {
            Text(this.statistics.total.toString())
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333333')
            Text('总计')
              .fontSize(12)
              .fontColor('#999999')
              .margin({ top: 4 })
          }
          .layoutWeight(1)

          Column() {
            Text(this.statistics.valid.toString())
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .fontColor('#4CAF50')
            Text('可恢复')
              .fontSize(12)
              .fontColor('#999999')
              .margin({ top: 4 })
          }
          .layoutWeight(1)

          Column() {
            Text(this.statistics.expired.toString())
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .fontColor('#999999')
            Text('已过期')
              .fontSize(12)
              .fontColor('#999999')
              .margin({ top: 4 })
          }
          .layoutWeight(1)
        }
        .width('100%')
        .padding(16)
        .backgroundColor(Color.White)
        .margin({ bottom: 8 })

        // 列表内容
        if (this.isLoading) {
          Column() {
            LoadingProgress().width(50).height(50)
          }
          .width('100%')
          .layoutWeight(1)
          .justifyContent(FlexAlign.Center)
        } else if (this.deletedContacts.length === 0) {
          Column() {
            Text('回收站为空')
              .fontSize(16)
              .fontColor(Color.Gray)
              .margin({ bottom: 10 })
            Text('已删除的联系人会在这里保留7天')
              .fontSize(14)
              .fontColor('#999999')
          }
          .width('100%')
          .layoutWeight(1)
          .justifyContent(FlexAlign.Center)
        } else {
          List() {
            ForEach(
              this.deletedContacts,
              (item: DeletedContact) => {
                ListItem() {
                  this.DeletedItem(item)
                }
                .width('100%')
              },
              (item: DeletedContact): string => item.backupId
            );
          }
          .width('100%')
          .layoutWeight(1)
          .backgroundColor('#F1F3F5')
          .divider({
            strokeWidth: 1,
            color: '#F0F0F0',
            startMargin: 16,
            endMargin: 16
          })
        }

        // 底部操作栏
        if (!this.isLoading && this.deletedContacts.length > 0) {
          Row() {
            Button('清空回收站')
              .fontColor(Color.Red)
              .backgroundColor('#FFEBEE')
              .width('100%')
              .height(45)
              .onClick(() => this.handleClearAll())
          }
          .width('100%')
          .padding(16)
          .backgroundColor(Color.White)
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F1F3F5')
    }
    .title('回收站')
    .titleMode(NavigationTitleMode.Mini)
    .backgroundColor('#F1F3F5')
  }

  @Builder
  DeletedItem(item: DeletedContact): void {
    Column() {
      Row() {
        // 头像
        Text(item.contactData.name ? item.contactData.name.charAt(0) : '?')
          .width(50)
          .height(50)
          .borderRadius(25)
          .backgroundColor(item.isExpired() ? '#CCCCCC' : '#FF9800')
          .fontColor(Color.White)
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
          .fontSize(20)
          .margin({ right: 12 })

        Column() {
          Text(item.contactData.name || '未知')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .fontColor(item.isExpired() ? '#999999' : '#000000')
          
          Row() {
            Text(item.contactData.phone || '')
              .fontSize(14)
              .fontColor('#999999')
            
            if (!item.isExpired()) {
              Text(` · 剩余 ${item.getRemainingDays()} 天`)
                .fontSize(12)
                .fontColor('#FF9800')
                .margin({ left: 8 })
            } else {
              Text(' · 已过期')
                .fontSize(12)
                .fontColor('#999999')
                .margin({ left: 8 })
            }
          }
          .margin({ top: 4 })

          Text(`删除时间: ${TimeUtils.formatTime(item.deleteTime)}`)
            .fontSize(12)
            .fontColor('#CCCCCC')
            .margin({ top: 2 })
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)
      }
      .width('100%')
      .padding({ left: 16, right: 16, top: 16, bottom: 12 })

      // 操作按钮
      Row() {
        if (!item.isExpired()) {
          Button('恢复')
            .fontSize(14)
            .fontColor('#4CAF50')
            .backgroundColor('#E8F5E9')
            .width('48%')
            .height(36)
            .onClick(() => this.handleRestore(item.backupId))
        }
        
        Button('永久删除')
          .fontSize(14)
          .fontColor(Color.Red)
          .backgroundColor('#FFEBEE')
          .width(item.isExpired() ? '100%' : '48%')
          .height(36)
          .margin({ left: item.isExpired() ? 0 : '4%' })
          .onClick(() => this.handlePermanentDelete(item.backupId, item.contactData.name))
      }
      .width('100%')
      .padding({ left: 16, right: 16, bottom: 16 })
      .justifyContent(FlexAlign.SpaceBetween)
    }
    .width('100%')
    .backgroundColor(Color.White)
  }
}
