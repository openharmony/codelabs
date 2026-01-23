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
import { Contact } from '../model/Contact';
import { ContactService } from '../service/ContactService';

@Entry
@Component
struct ContactDetailPage {
  @State contact: Contact = new Contact('', '');
  @State isLoading: boolean = true;
  private contactId: string = '';

  aboutToAppear() {
    const params = router.getParams() as Record<string, string>;

    // 【修改】只接收 contactId
    if (params && params['contactId']) {
      this.contactId = params['contactId'];
      this.fetchContactDetail();
    } else if (params && params['contact']) {
      // 兼容旧版本传参方式
      try {
        const passedContact = JSON.parse(params['contact']) as Contact;
        this.contact = passedContact;
        this.contactId = passedContact.id;
        this.fetchContactDetail();
      } catch (e) {
        console.error("解析联系人数据失败");
      }
    }
  }

  onPageShow() {
    // 每次显示页面重新获取,确保从编辑页返回后数据是最新的
    if (this.contactId) {
      this.fetchContactDetail();
    }
  }

  async fetchContactDetail() {
    this.isLoading = true;
    try {
      const allContacts = await ContactService.getAllContacts();
      const found = allContacts.find(item => item.id === this.contactId);

      if (found) {
        // 【关键点】创建新对象赋值,确保响应式更新
        this.contact = {
          id: found.id,
          name: found.name,
          phone: found.phone,
          email: found.email || '',
          remark: found.remark || ''
        } as Contact;
        console.info('[ContactDetailPage] 加载联系人详情成功:', this.contact.name);
      } else {
        console.warn('[ContactDetailPage] 未找到联系人,ID:', this.contactId);
        promptAction.showToast({ message: '联系人不存在' });
        router.back();
      }
    } catch (err) {
      console.error('获取联系人详情失败: ' + JSON.stringify(err));
      promptAction.showToast({ message: '加载失败' });
    } finally {
      this.isLoading = false;
    }
  }

  handleEdit() {
    router.pushUrl({
      url: Routes.CONTACT_EDIT,
      params: { contactId: this.contactId }
    });
  }

  handleDelete() {
    AlertDialog.show({
      title: '删除联系人',
      message: `确定要删除联系人"${this.contact.name}"吗?`,
      primaryButton: {
        value: '取消',
        action: () => {}
      },
      secondaryButton: {
        value: '删除',
        fontColor: Color.Red,
        action: async () => {
          try {
            await ContactService.deleteContact(this.contactId);
            promptAction.showToast({ message: '已成功删除' });

            // 【关键修改】触发全局刷新标志
            AppStorage.Set('contactListRefreshFlag', Date.now());

            router.back();
          } catch (err) {
            console.error('删除失败:', JSON.stringify(err));
            promptAction.showToast({ message: '删除失败,请检查权限' });
          }
        }
      }
    });
  }

  build() {
    Navigation() {
      Column() {
        if (this.isLoading) {
          // 加载中状态
          Column() {
            LoadingProgress()
              .width(50)
              .height(50)
          }
          .width('100%')
          .height('100%')
          .justifyContent(FlexAlign.Center)
        } else {
          // 头部展示区
          Column() {
            Text(this.contact.name ? this.contact.name.charAt(0) : '?')
              .width(80)
              .height(80)
              .borderRadius(40)
              .backgroundColor('#007DFF')
              .fontColor(Color.White)
              .fontSize(32)
              .textAlign(TextAlign.Center)
              .margin({ bottom: 16 })

            Text(this.contact.name)
              .fontSize(24)
              .fontWeight(FontWeight.Bold)
              .margin({ bottom: 8 })

            Text('手机:' + this.contact.phone)
              .fontSize(18)
              .fontColor(Color.Gray)
          }
          .width('100%')
          .padding({ top: 40, bottom: 40 })
          .backgroundColor(Color.White)

          // 详细信息列表
          List() {
            ListItem() {
              this.DetailItem('手机', this.contact.phone)
            }
            ListItem() {
              this.DetailItem('邮箱', this.contact.email || '未填写')
            }
            ListItem() {
              this.DetailItem('备注', this.contact.remark || '无')
            }
          }
          .margin({ top: 12 })
          .backgroundColor(Color.White)
          .divider({ strokeWidth: 1, color: '#F1F3F5', startMargin: 16, endMargin: 16 })

          Blank()

          Row() {
            Button('删除')
              .fontColor(Color.Red)
              .backgroundColor('#FFEBEE')
              .width('46%')
              .height(45)
              .onClick(() => this.handleDelete())

            Button('编辑')
              .width('46%')
              .height(45)
              .onClick(() => this.handleEdit())
          }
          .width('100%')
          .justifyContent(FlexAlign.SpaceBetween)
          .padding(16)
          .backgroundColor(Color.White)
        }
      }
      .height('100%')
      .backgroundColor('#F1F3F5')
    }
    .title("联系人详情")
    .titleMode(NavigationTitleMode.Mini)
  }

  @Builder DetailItem(label: string, value: string) {
    Row() {
      Text(label)
        .width(80)
        .fontSize(16)
        .fontColor(Color.Gray)
      Text(value)
        .layoutWeight(1)
        .fontSize(16)
        .fontColor(Color.Black)
    }
    .width('100%')
    .padding(18)
  }
}