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
struct FavoritesDisplay {
  @Provide('context') context: Context = getContext(this);
  private contactData: ContactData = new ContactData(this.context);
  private favoriteManager: FavoriteManager = new FavoriteManager(this.context);
  @State favoriteContacts: contact.Contact[] = [];
  @State isLoading: boolean = true;
  @State searchText: string = '';
  @State filteredContacts: contact.Contact[] = [];

  aboutToAppear() {
    this.loadFavoriteContacts();
  }

  onPageShow() {
    this.loadFavoriteContacts();
  }

  async loadFavoriteContacts() {
    console.log('加载收藏联系人');
    this.isLoading = true;
    try {
      const allContacts = await this.contactData.queryAllContacts();
      this.favoriteContacts = await this.favoriteManager.getFavoriteContacts(allContacts);
      this.filteredContacts = [...this.favoriteContacts];
      console.log(`已加载 ${this.favoriteContacts.length} 个收藏联系人`);
    } catch (error) {
      console.error('加载收藏联系人失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  // 搜索收藏联系人
  searchFavorites() {
    if (this.searchText.trim() === '') {
      this.filteredContacts = [...this.favoriteContacts];
      return;
    }

    const searchTerm = this.searchText.toLowerCase();
    this.filteredContacts = this.favoriteContacts.filter(contactItem => {
      const name = contactItem.name?.fullName?.toLowerCase() || '';
      const phone = contactItem.phoneNumbers?.[0]?.phoneNumber?.toLowerCase() || '';
      return name.includes(searchTerm) || phone.includes(searchTerm);
    });
  }

  // 返回主页
  backToHome() {
    router.back();
  }

  // 跳转到收藏管理
  goToFavoriteManagement() {
    router.pushUrl({
      url: 'pages/FavoriteManagement'
    });
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
            this.backToHome();
          })

        Text('收藏联系人')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .layoutWeight(1)
          .textAlign(TextAlign.Center)

        Button('管理')
          .fontSize(14)
          .margin({ right: 16 })
          .backgroundColor('#007DFF')
          .fontColor(Color.White)
          .onClick(() => {
            this.goToFavoriteManagement();
          })
      }
      .width('100%')
      .height(56)
      .backgroundColor('#f0f0f0')

      // 搜索框
      TextInput({ placeholder: '搜索收藏联系人' })
        .width('90%')
        .height(40)
        .margin({ top: 15, bottom: 10 })
        .onChange((value: string) => {
          this.searchText = value;
          this.searchFavorites();
        })

      // 收藏统计
      Row() {
        Text(`收藏联系人 (${this.filteredContacts.length})`)
          .fontSize(16)
          .fontColor('#666')
          .fontWeight(FontWeight.Medium)
      }
      .width('90%')
      .margin({ bottom: 10 })
      .justifyContent(FlexAlign.Start)

      // 收藏联系人列表
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
      } else if (this.filteredContacts.length === 0) {
        Column() {
          Text(this.searchText ? '未找到匹配的收藏联系人' : '暂无收藏联系人')
            .fontSize(18)
            .fontColor('#999')
            .margin({ bottom: 10 })

          if (!this.searchText) {
            Button('去管理收藏')
              .width(140)
              .height(40)
              .margin({ top: 20 })
              .backgroundColor('#007DFF')
              .fontColor(Color.White)
              .onClick(() => {
                this.goToFavoriteManagement();
              })
          }
        }
        .width('100%')
        .height('70%')
        .justifyContent(FlexAlign.Center)
      } else {
        List({ space: 10 }) {
          ForEach(this.filteredContacts, (item: contact.Contact) => {
            ListItem() {
              this.favoriteDisplayItem(item)
            }
          }, (item: contact.Contact) => item.key || item.id?.toString() || '')
        }
        .width('100%')
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  @Builder
  favoriteDisplayItem(contactItem: contact.Contact) {
    Row() {
      // 头像
      Column() {
        if (contactItem.portrait?.uri) {
          Image(contactItem.portrait.uri)
            .width(60)
            .height(60)
            .borderRadius(30)
        } else {
          Text(contactItem.name?.fullName?.charAt(0) || '?')
            .fontSize(24)
            .fontColor(Color.White)
            .textAlign(TextAlign.Center)
            .width(60)
            .height(60)
            .borderRadius(30)
            .backgroundColor('#FFD700')
        }
      }
      .margin({ right: 15 })

      // 联系人信息
      Column() {
        Text(contactItem.name?.fullName || '未知')
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Start)
          .width('100%')

        if (contactItem.phoneNumbers && contactItem.phoneNumbers.length > 0) {
          Text(contactItem.phoneNumbers[0].phoneNumber)
            .fontSize(14)
            .fontColor('#666')
            .textAlign(TextAlign.Start)
            .width('100%')
            .margin({ top: 5 })
        }

        if (contactItem.emails && contactItem.emails.length > 0) {
          Text(contactItem.emails[0].email)
            .fontSize(12)
            .fontColor('#999')
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
  }
}