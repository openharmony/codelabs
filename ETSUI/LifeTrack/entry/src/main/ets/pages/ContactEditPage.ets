/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import { ContactsItem } from '../model/ContactModel';
import { ContactManager } from './ContactManager';
import common from '@ohos.app.ability.common';

@Builder
export function ContactEditBuilder() {
  ContactEdit();
}

@ComponentV2
struct ContactEdit {
  pathStack: NavPathStack = new NavPathStack();
  @Local contactId: number = -1
  @Local name: string = '';
  @Local phone: string = '';
  private context: common.UIAbilityContext =
    this.getUIContext().getHostContext() as common.UIAbilityContext;
  private contactManager: ContactManager = ContactManager.getInstance();

  // 判断是否是添加模式
  isAddMode(): boolean {
    return this.contactId === -1;
  }

  // 获取页面标题
  getPageTitle(): string {
    return this.isAddMode() ? '添加联系人' : '编辑联系人';
  }

  // 返回上一页
  goBack() {
    this.pathStack.replacePathByName('Contact', null, false);
  }

  // 保存联系人
  async saveContact() {
    if (!this.name.trim()) {
      this.getUIContext().getPromptAction().showToast({ message: '姓名不能为空' });
      return;
    }

    if (!this.phone.trim()) {
      this.getUIContext().getPromptAction().showToast({ message: '电话号码不能为空' });
      return;
    }

    try {
      // 初始化数据库
      await this.contactManager.initDatabase(this.context);

      if (this.isAddMode()) {
        // 添加联系人
        const result = await this.contactManager.addContact(this.name.trim(), this.phone.trim());
        if (result.success) {
          this.getUIContext().getPromptAction().showToast({ message: '联系人添加成功' });
          this.goBack();
        } else {
          this.getUIContext().getPromptAction().showToast({ message: result.message || '添加失败' });
        }
      } else {
        // 编辑联系人
        const contact: ContactsItem = {
          id: this.contactId,
          name: this.name.trim(),
          phone: this.phone.trim(),
          initial: '' // ContactManager会处理
        };

        const result = await this.contactManager.updateContact(contact);
        if (result.success) {
          this.getUIContext().getPromptAction().showToast({ message: '联系人更新成功' });
          this.goBack();
        } else {
          this.getUIContext().getPromptAction().showToast({ message: result.message || '更新失败' });
        }
      }
    } catch (error) {
      console.error('保存联系人失败:', error);
      this.getUIContext().getPromptAction().showToast({ message: '保存失败，请重试' });
    }
  }

  build() {
    NavDestination() {
      Column({ space: 0 }) {
        // 标题栏
        Row({ space: 0 }) {
          // 取消按钮
          Button('取消')
            .fontSize(16)
            .fontColor('#007AFF')
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              this.goBack();
            })

          Text(this.getPageTitle())
            .fontSize(18)
            .fontWeight(FontWeight.Medium)
            .layoutWeight(1)
            .textAlign(TextAlign.Center)

          // 保存按钮
          Button('保存')
            .fontSize(16)
            .fontColor('#007AFF')
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              this.saveContact();
            })
        }
        .padding({
          left: 16,
          right: 16,
          top: 10,
          bottom: 10
        })
        .width('100%')
        .backgroundColor(Color.White)
        .border({ width: { bottom: 1 }, color: '#eeeeee' })

        // 内容区域
        Column({ space: 24 }) {
          // 姓名输入框
          Column({ space: 8 }) {
            Text('姓名')
              .fontSize(16)
              .fontColor('#333333')
              .width('100%')
              .padding({ left: 16 })

            TextInput({ text: this.name, placeholder: '请输入联系人姓名' })
              .placeholderFont({ size: 16 })
              .width('100%')
              .height(50)
              .fontSize(16)
              .backgroundColor(Color.White)
              .padding({ left: 16, right: 16 })
              .onChange((value: string) => {
                this.name = value;
              })
          }
          .width('100%')

          // 电话号码输入框
          Column({ space: 8 }) {
            Text('电话号码')
              .fontSize(16)
              .fontColor('#333333')
              .width('100%')
              .padding({ left: 16 })

            TextInput({ text: this.phone, placeholder: '请输入联系人电话号码' })
              .placeholderFont({ size: 16 })
              .width('100%')
              .height(50)
              .fontSize(16)
              .backgroundColor(Color.White)
              .padding({ left: 16, right: 16 })
              .onChange((value: string) => {
                this.phone = value;
              })
          }
          .width('100%')
        }
        .padding({ top: 24 })
        .layoutWeight(1)
        .backgroundColor('#f5f5f5')
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#f5f5f5')
    }
    .hideBackButton(true)
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
      // 从NavPathStack获取传递的参数
      const params: ContactsItem = context.pathInfo.param as ContactsItem;
      if (params) {
        this.contactId = params.id;
        // 如果是编辑模式，填充数据
        if (this.contactId && this.contactId >= 0) {
          this.name = params.name;
          this.phone = params.phone;
        }
      }
    })
  }
}