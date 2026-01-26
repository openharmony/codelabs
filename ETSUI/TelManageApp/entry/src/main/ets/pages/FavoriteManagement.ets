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
struct FavoriteManagement {
  @Provide('context') context: Context = getContext(this);
  private contactData: ContactData = new ContactData(this.context);
  private favoriteManager: FavoriteManager = new FavoriteManager(this.context);

  @State favoriteContacts: contact.Contact[] = [];
  @State isLoading: boolean = true;

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
      console.log(`已加载 ${this.favoriteContacts.length} 个收藏联系人`);
    } catch (error) {
      console.error('加载收藏联系人失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  // 返回主页
  backToHome() {
    router.back();
  }

  // 跳转到选择收藏页面
  goToSelectFavorites() {
    console.log('跳转到选择收藏页面');
    router.pushUrl({
      url: 'pages/SelectFavorites'
    });
  }

  // 移除收藏
  async removeFavorite(contactId: string) {
    if (!contactId) return;

    try {
      await this.favoriteManager.removeFavoriteContact(contactId);
      console.log('已移除收藏联系人:', contactId);
      await this.loadFavoriteContacts();
    } catch (error) {
      console.error('移除收藏失败:', error);
    }
  }

  // 显示移除确认对话框
  showRemoveConfirmDialog(contactItem: contact.Contact) {
    if (!contactItem.key) return;

    const contactName = contactItem.name?.fullName || '未知联系人';

    AlertDialog.show({
      title: '移除收藏',
      message: `确定要移除"${contactName}"的收藏吗？`,
      autoCancel: true,
      alignment: DialogAlignment.Center,
      primaryButton: {
        value: '取消',
        action: () => {
          console.log('用户取消移除操作');
        }
      },
      secondaryButton: {
        value: '移除',
        fontColor: '#ff3b30',
        action: () => {
          console.log('用户确认移除收藏');
          this.removeFavorite(contactItem.key!);
        }
      }
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

        Text('收藏管理')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .layoutWeight(1)
          .textAlign(TextAlign.Center)

        Button('选择收藏')
          .fontSize(14)
          .margin({ right: 16 })
          .backgroundColor('#007DFF')
          .fontColor(Color.White)
          .onClick(() => {
            this.goToSelectFavorites();
          })
      }
      .width('100%')
      .height(56)
      .backgroundColor('#f0f0f0')

      // 收藏联系人统计
      Row() {
        Text(`已收藏 ${this.favoriteContacts.length} 个联系人`)
          .fontSize(16)
          .fontColor('#666')
      }
      .width('90%')
      .margin({ top: 20, bottom: 10 })
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
      } else if (this.favoriteContacts.length === 0) {
        Column() {
          Text('暂无收藏联系人')
            .fontSize(18)
            .fontColor('#999')
            .margin({ bottom: 10 })

          Text('点击右上角"选择收藏"添加联系人')
            .fontSize(14)
            .fontColor('#666')

          Button('去选择')
            .width(120)
            .height(40)
            .margin({ top: 20 })
            .backgroundColor('#007DFF')
            .fontColor(Color.White)
            .onClick(() => {
              this.goToSelectFavorites();
            })
        }
        .width('100%')
        .height('70%')
        .justifyContent(FlexAlign.Center)
      } else {
        List({ space: 10 }) {
          ForEach(this.favoriteContacts, (item: contact.Contact) => {
            ListItem() {
              this.favoriteContactItem(item)
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
  favoriteContactItem(contactItem: contact.Contact) {
    Row() {
      // 头像
      Column() {
        if (contactItem.portrait?.uri) {
          Image(contactItem.portrait.uri)
            .width(50)
            .height(50)
            .borderRadius(25)
        } else {
          Text(contactItem.name?.fullName?.charAt(0) || '?')
            .fontSize(20)
            .fontColor(Color.White)
            .textAlign(TextAlign.Center)
            .width(50)
            .height(50)
            .borderRadius(25)
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
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)

      // 操作按钮
      Button('移除')
        .fontSize(12)
        .width(60)
        .height(30)
        .backgroundColor('#ff3b30')
        .fontColor(Color.White)
        .onClick(() => {
          this.showRemoveConfirmDialog(contactItem);
        })
    }
    .width('90%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(10)
    .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
    .margin({ left: '5%', right: '5%' })
  }
}