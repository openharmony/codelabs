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
import { ContactData } from '../model/ContactData';
import { FavoriteManager } from '../model/FavoriteManager';
import { contact } from '@kit.ContactsKit';
import { router } from '@kit.ArkUI';
import { Context } from '@kit.AbilityKit';

@Entry
@Component
struct SelectFavorites {
  @Provide('context') context: Context = getContext(this);
  private contactData: ContactData = new ContactData(this.context);
  private favoriteManager: FavoriteManager = new FavoriteManager(this.context);
  @State allContacts: contact.Contact[] = [];
  @State selectedContactIds: string[] = [];
  @State isLoading: boolean = true;
  @State isSelectAll: boolean = false;
  @State showToast: boolean = false;
  @State toastMessage: string = '';

  aboutToAppear() {
    this.loadContacts();
  }

  async loadContacts() {
    console.log('加载所有联系人');
    this.isLoading = true;
    try {
      this.allContacts = await this.contactData.queryAllContacts();
      const favoriteIds = await this.favoriteManager.getFavoriteContactIds();
      this.selectedContactIds = [...favoriteIds];
      this.isSelectAll = this.allContacts.length > 0 &&
        this.selectedContactIds.length === this.allContacts.length;
      console.log(`加载 ${this.allContacts.length} 个联系人，已收藏 ${favoriteIds.length} 个`);
    } catch (error) {
      console.error('加载联系人失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  // 返回收藏管理页面
  backToFavoriteManagement() {
    router.back();
  }

  // 切换选择状态
  toggleContactSelection(contactId: string) {
    if (!contactId) {
      return;
    }

    const index = this.selectedContactIds.indexOf(contactId);
    if (index === -1) {
      this.selectedContactIds.push(contactId);
    } else {
      this.selectedContactIds.splice(index, 1);
    }

    this.isSelectAll = this.selectedContactIds.length === this.allContacts.length;
  }

  // 全选/取消全选
  toggleSelectAll() {
    if (this.isSelectAll) {
      this.selectedContactIds = [];
    } else {
      this.selectedContactIds = this.allContacts
        .map(contact => contact.key || '')
        .filter(key => key !== '');
    }
    this.isSelectAll = !this.isSelectAll;
  }

  // 显示Toast提示
  showToastMessage(message: string) {
    this.toastMessage = message;
    this.showToast = true;

    // 2秒后自动隐藏
    setTimeout(() => {
      this.showToast = false;
    }, 2000);
  }

  // 保存收藏
  async saveFavorites() {
    try {
      await this.favoriteManager.saveFavoriteContactIds(this.selectedContactIds);
      console.log(`已保存 ${this.selectedContactIds.length} 个收藏联系人`);

      // 显示成功提示
      this.showToastMessage(`已收藏 ${this.selectedContactIds.length} 个联系人`);

      // 延迟返回
      setTimeout(() => {
        router.back();
      }, 500);

    } catch (error) {
      console.error('保存收藏失败:', error);
      this.showToastMessage('保存失败，请重试');
    }
  }

  build() {
    Column() {
      // 顶部标题栏
      Row() {
        Button('< 返回')
          .fontSize(16)
          .margin({ left: 16 })
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => {
            this.backToFavoriteManagement();
          })

        Text('选择收藏联系人')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .layoutWeight(1)
          .textAlign(TextAlign.Center)

        Text(`已选 ${this.selectedContactIds.length}`)
          .fontSize(14)
          .fontColor('#007DFF')
          .margin({ right: 16 })
      }
      .width('100%')
      .height(56)
      .backgroundColor('#f0f0f0')

      // 操作栏
      Row() {
        // 全选按钮
        Row() {
          Checkbox({ name: 'selectAll', group: 'favorites' })
            .select(this.isSelectAll)
            .onChange((checked: boolean) => {
              this.toggleSelectAll();
            })

          Text(this.isSelectAll ? '取消全选' : '全选')
            .fontSize(16)
            .margin({ left: 8 })
        }
        .onClick(() => {
          this.toggleSelectAll();
        })

        // 保存按钮
        Button('保存')
          .fontSize(16)
          .width(80)
          .height(36)
          .backgroundColor('#007DFF')
          .fontColor(Color.White)
          .margin({ left: 20 })
          .enabled(this.selectedContactIds.length > 0)
          .onClick(() => {
            this.saveFavorites();
          })
      }
      .width('90%')
      .margin({ top: 20, bottom: 10 })
      .justifyContent(FlexAlign.SpaceBetween)

      // 联系人列表
      if (this.isLoading) {
        Column() {
          LoadingProgress()
            .width(50)
            .height(50)
            .margin({ top: 100 })
        }
        .width('100%')
        .height('70%')
        .justifyContent(FlexAlign.Center)
      } else if (this.allContacts.length === 0) {
        Column() {
          Text('暂无联系人')
            .fontSize(18)
            .fontColor('#999')
            .margin({ bottom: 10 })

          Text('请先添加联系人')
            .fontSize(14)
            .fontColor('#666')
        }
        .width('100%')
        .height('70%')
        .justifyContent(FlexAlign.Center)
      } else {
        List({ space: 10 }) {
          ForEach(this.allContacts, (item: contact.Contact) => {
            ListItem() {
              // 直接在 ForEach 回调中构建完整的 UI
              Row() {
                // 选择框
                Checkbox({
                  name: item.key || '',
                  group: 'contactSelection'
                })
                  .select(this.selectedContactIds.includes(item.key || ''))
                  .onChange((checked: boolean) => {
                    this.toggleContactSelection(item.key || '');
                  })
                  .margin({ right: 15 })

                // 头像
                Column() {
                  if (item.portrait?.uri) {
                    Image(item.portrait.uri)
                      .width(40)
                      .height(40)
                      .borderRadius(20)
                  } else {
                    Text(item.name?.fullName?.charAt(0) || '?')
                      .fontSize(16)
                      .fontColor(Color.White)
                      .textAlign(TextAlign.Center)
                      .width(40)
                      .height(40)
                      .borderRadius(20)
                      .backgroundColor('#007DFF')
                  }
                }
                .margin({ right: 15 })

                // 联系人信息
                Column() {
                  Text(item.name?.fullName || '未知')
                    .fontSize(16)
                    .fontWeight(FontWeight.Medium)
                    .textAlign(TextAlign.Start)
                    .width('100%')

                  if (item.phoneNumbers && item.phoneNumbers.length > 0) {
                    Text(item.phoneNumbers[0].phoneNumber)
                      .fontSize(12)
                      .fontColor('#666')
                      .textAlign(TextAlign.Start)
                      .width('100%')
                      .margin({ top: 2 })
                  }
                }
                .alignItems(HorizontalAlign.Start)
                .layoutWeight(1)
              }
              .width('90%')
              .padding(15)
              .backgroundColor(Color.White)
              .borderRadius(10)
              .shadow({
                radius: 2,
                color: '#10000000',
                offsetX: 0,
                offsetY: 1
              })
              .margin({ left: '5%', right: '5%' })
              .onClick(() => {
                this.toggleContactSelection(item.key || '');
              })
            }
          }, (item: contact.Contact) => item.key || item.id?.toString() || '')
        }
        .width('100%')
        .layoutWeight(1)
      }

      // Toast提示
      if (this.showToast) {
        Text(this.toastMessage)
          .fontSize(16)
          .fontColor(Color.White)
          .backgroundColor('rgba(0, 0, 0, 0.7)')
          .padding({
            left: 20,
            right: 20,
            top: 10,
            bottom: 10
          })
          .borderRadius(8)
          .position({ x: '50%', y: '80%' })
          .translate({ x: '-50%', y: '-50%' })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }
}