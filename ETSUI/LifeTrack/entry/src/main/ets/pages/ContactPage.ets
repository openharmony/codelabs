/*
 * Copyright (c) 2026 BIT-Group23
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { ContactsItem } from '../model/ContactModel'
import { ContactManager } from './ContactManager';
import common from '@ohos.app.ability.common'
import call from '@ohos.telephony.call';
import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';
import { PromptAction } from '@ohos.arkui.UIContext';

// 跳转页面入口函数
@Builder
export function ContactBuilder() {
  Contact();
}

@ComponentV2
struct Contact {
  pathStack: NavPathStack = new NavPathStack();

  @Local contactsList: Array<ContactsItem> = []
  @Local searchText: string = ''
  @Local filteredContacts: Array<ContactsItem> = []
  @Local selectedContact: ContactsItem | null = null
  @Local selectedContactId: number = -1 // 添加选中状态的ID

  private context: common.UIAbilityContext =
    this.getUIContext().getHostContext() as common.UIAbilityContext;
  private contactManager: ContactManager = ContactManager.getInstance();
  private prompt : PromptAction = this.getUIContext().getPromptAction()

  async aboutToAppear() {
    // 初始化数据库并加载数据
    await this.initDatabase();
  }

  // 初始化数据库并加载联系人
  async initDatabase() {
    try {
      // 初始化数据库
      await this.contactManager.initDatabase(this.context);

      // 加载联系人
      await this.loadContacts();
    } catch (error) {
      console.error('初始化失败:', error);
    }
  }

  // 加载联系人
  async loadContacts() {
    try {
      this.contactsList = await this.contactManager.getAllContacts();
      this.filteredContacts = [...this.contactsList];
      // 如果有数据，默认选中第一条
      if (this.filteredContacts.length > 0) {
        this.selectedContact = this.filteredContacts[0];
        this.selectedContactId = this.filteredContacts[0].id;
      } else {
        this.selectedContact = null;
        this.selectedContactId = -1;
      }
    } catch (error) {
      console.error('加载联系人失败:', error);
    }
  }

  // 搜索联系人
  async filterContacts() {
    if (this.searchText.trim() === '') {
      this.filteredContacts = [...this.contactsList];
    } else {
      this.filteredContacts = await this.contactManager.searchContacts(this.searchText);
    }

    // 搜索后，如果有数据，默认选中第一条
    if (this.filteredContacts.length > 0) {
      // 如果当前选中的联系人不在搜索结果中，则选中第一条
      const currentSelectedInResults = this.filteredContacts.find(
        contact => contact.id === this.selectedContactId
      );
      if (!currentSelectedInResults) {
        this.selectedContact = this.filteredContacts[0];
        this.selectedContactId = this.filteredContacts[0].id;
      }
    } else {
      this.selectedContact = null;
      this.selectedContactId = -1;
    }
  }

  @Builder
  ContactItem(contact: ContactsItem) {
    Column({ space: 8 }) {
      // 显示字母索引（如果是分组第一个）
      if (this.showInitial(contact, this.filteredContacts)) {
        Text(contact.initial)
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Start)
          .width('100%')
          .padding({ left: 16, top: 10, bottom: 5 })
          .backgroundColor('#f9f9f9')
      }

      Row({ space: 0 }) {
        // 联系人头像
        Column() {
          Text(contact.name.charAt(0))
            .fontSize(20)
            .fontColor(Color.White)
        }
        .width(48)
        .height(48)
        .backgroundColor('#007AFF')
        .borderRadius(24)
        .justifyContent(FlexAlign.Center)
        .margin({ left: 16 })

        // 联系人信息
        Column({ space: 4 }) {
          Text(contact.name)
            .fontSize(18)
            .fontColor(Color.Black)
          Text(contact.phone)
            .fontSize(14)
            .fontColor('#666666')
        }
        .margin({ left: 12 })
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)
      }
      .width('100%')
      .height(72)
      .backgroundColor(this.selectedContactId === contact.id ? '#f0f8ff' : Color.White) // 选中时背景色变化
      .onClick(() => {
        // 点击时选中该联系人
        this.selectedContact = contact;
        this.selectedContactId = contact.id;
      })
    }
  }

  @Builder
  ActionBar() {
    Row({ space: 0 }) {
      // Call 按钮
      Column() {
        Image($r('app.media.phone')) // 电话图标
          .width(24)
          .height(24)
          .margin({ bottom: 4 })
        Text('拨号')
          .fontSize(12)
          .fontColor(this.selectedContact ? '#007AFF' : '#cccccc')  // 没有选中时变灰
      }
      .width('25%')
      .height(60)
      .onClick(() => {
        if (this.selectedContact) {
          this.makeCall()
        }else {
          this.prompt.showToast({ message: '请先选择一个联系人' });
        }
      })

      // Add 按钮
      Column() {
        Image($r('app.media.plus')) // 添加图标
          .width(24)
          .height(24)
          .margin({ bottom: 4 })
        Text('添加')
          .fontSize(12)
          .fontColor('#007AFF')
      }
      .width('25%')
      .height(60)
      .onClick(() => {
        this.addNewContact()
      })

      // Edit 按钮
      Column() {
        Image($r('app.media.pen')) // 编辑图标
          .width(24)
          .height(24)
          .margin({ bottom: 4 })
        Text('编辑')
          .fontSize(12)
          .fontColor(this.selectedContact ? '#007AFF' : '#cccccc') // 没有选中时变灰
      }
      .width('25%')
      .height(60)
      .onClick(() => {
        if (this.selectedContact) {
          this.editContact()
        }else {
          this.prompt.showToast({ message: '请先选择一个联系人' });
        }
      })

      // Delete 按钮
      Column() {
        Image($r('app.media.bin')) // 删除图标
          .width(24)
          .height(24)
          .margin({ bottom: 4 })
        Text('删除')
          .fontSize(12)
          .fontColor(this.selectedContact ? '#FF3B30' : '#ffcccc')  // 没有选中时变灰
      }
      .width('25%')
      .height(60)
      .onClick(() => {
        if (this.selectedContact) {
          this.deleteContact()
        }else {
          this.prompt.showToast({ message: '请先选择一个联系人' });
        }
      })
    }
    .width('100%')
    .backgroundColor('#f9f9f9')
    .border({ width: { top: 1 }, color: '#eeeeee' })
  }

  // 显示字母索引的条件
  showInitial(contact: ContactsItem, list: Array<ContactsItem>): boolean {
    const index = list.findIndex(item => item.id === contact.id)
    if (index === 0) return true
    const prevContact = list[index - 1]
    return prevContact.initial !== contact.initial
  }

  // 添加权限请求函数
  async requestCallPermission(): Promise<boolean> {
    try {
      // 获取上下文
      const context = this.context
      if (!context) {
        console.error('无法获取上下文');
        return false;
      }

      // 创建权限管理器
      const atManager = abilityAccessCtrl.createAtManager();

      // 检查权限
      const permissions: Array<Permissions> = ['ohos.permission.PLACE_CALL'];

      // 请求权限
      const result = await atManager.requestPermissionsFromUser(context, permissions);

      // 检查权限是否被授予
      const grantStatus = result.authResults;
      const permissionGranted = grantStatus[0] === 0; // 0 表示授予权限

      if (permissionGranted) {
        console.info('电话权限已授予');
        return true;
      } else {
        console.warn('电话权限被拒绝');
        return false;
      }
    } catch (error) {
      console.error('请求电话权限失败:', error);
      return false;
    }
  }

  // 拨打电话
  async makeCall() {
    if (!this.selectedContact) {
      this.prompt.showToast({ message: '请先选择一个联系人' });
      return;
    }

    const phoneNumber = this.selectedContact.phone;

    if (!phoneNumber || phoneNumber.trim() === '') {
      this.prompt.showToast({ message: '电话号码无效' });
      return;
    }

    console.log('准备拨打电话:', phoneNumber);

    try {
      // 清理电话号码（移除空格和特殊字符）
      const cleanPhoneNumber = phoneNumber.replace(/[\s\-\(\)]/g, '');

      // 检查并请求权限
      const permissionGranted = await this.requestCallPermission();
      if (!permissionGranted) {
        this.prompt.showToast({ message: '需要电话权限才能拨打电话' });
        return;
      }

      // 拨打电话
      call.makeCall(cleanPhoneNumber, (err) => {
        if (err) {
          console.error('拨打电话失败:', err);
          this.prompt.showToast({ message: '拨打电话失败' });
        } else {
          console.log('拨打电话成功');
          // 可以添加成功的提示，但通常拨号界面会直接打开
        }
      });
    } catch (error) {
      console.error('拨打电话异常:', error);
      this.prompt.showToast({ message: '拨打电话失败' });
    }
  }

  // 添加联系人
  addNewContact() {
    const contact: ContactsItem = {
      id: -1,
      name: '',
      phone: '',
      initial: ''
    }
    console.log('添加联系人')
    this.pathStack.pushPathByName("ContactEdit", contact, false)
  }

  // 编辑联系人
  editContact() {
    if (!this.selectedContact) {
      this.prompt.showToast({ message: '请先选择一个联系人' });
      return;
    }

    console.log('编辑联系人:', this.selectedContact.name)
    this.pathStack.pushPathByName("ContactEdit", this.selectedContact, false)
  }

  // 删除联系人
  async deleteContact() {
    if (!this.selectedContact) {
      this.prompt.showToast({ message: '请先选择一个联系人' });
      return;
    }

    const contact = this.selectedContact;
    console.log('删除联系人:', contact.name)

    // 显示确认对话框
    this.prompt.showDialog({
      title: '删除联系人',
      message: `确定要删除 ${contact.name} 吗？`,
      buttons: [
        { text: '取消', color: '#666666' },
        { text: '删除', color: '#FF3B30' }
      ]
    }).then(async (result) => {
      if (result.index === 1) { // 点击了删除按钮
        const deleteResult = await this.contactManager.deleteContact(contact.id);
        if (deleteResult.success) {
          this.prompt.showToast({ message: '联系人删除成功' });
          // 重新加载数据
          await this.loadContacts();
          this.selectedContact = null;
          this.selectedContactId = -1;
        } else {
          this.prompt.showToast({ message: deleteResult.message || '删除失败' });
        }
      }
    });
  }

  build() {
    NavDestination() {
      Column({ space: 0 }) {
        // 标题栏
        Row({ space: 0 }){
          // 返回按钮
          Button('返回')
            .fontSize(16)
            .fontColor('#007AFF')
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              this.pathStack.pushPathByName("Dashboard", null, false)
            })
          Text('联系人')
            .fontSize(18)
            .fontWeight(FontWeight.Medium)
            .layoutWeight(1)
            .textAlign(TextAlign.Center)
          Blank()
            .width(54)
        }
        .padding({ left: 6, right: 6, top: 10, bottom: 10 })
        .width('100%')
        .backgroundColor(Color.White)

        // 搜索框
        Row({ space: 10 }) {
          Image($r('app.media.search')) // 搜索图标
            .width(20)
            .height(20)
            .margin({ left: 16 })

          TextInput({ placeholder: 'Search contacts' })
            .placeholderFont({ size: 16 })
            .width('80%')
            .height(40)
            .onChange((value: string) => {
              this.searchText = value
              this.filterContacts()
            })
        }
        .width('100%')
        .height(60)
        .backgroundColor('#f5f5f5')
        .borderRadius(8)
        .margin({ left: 16, right: 16, bottom: 20 })

        // 联系人列表
        List({ space: 0 }) {
          ForEach(this.filteredContacts, (contact: ContactsItem, index: number) => {
            ListItem() {
              this.ContactItem(contact)
            }
          }, (contact: ContactsItem) => contact.id.toString())
        }
        .width('100%')
        .layoutWeight(1)
        .divider({ strokeWidth: 1, color: '#eeeeee', startMargin: 16, endMargin: 16 })

        // 底部操作栏
        this.ActionBar()
      }
      .width('100%')
      .height('100%')
      .backgroundColor(Color.White)
    }
    .hideBackButton(true)
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}