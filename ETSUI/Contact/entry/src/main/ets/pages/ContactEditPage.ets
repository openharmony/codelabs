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
import { BusinessError } from '@ohos.base';
import { Contact } from '../model/Contact';
import { ContactService } from '../service/ContactService';

@Entry
struct ContactEditPage {
  // 状态变量
  @State contactId: string = '';
  @State contactKey: string = '';  // 新增:用于存储系统的 key
  @State name: string = '';
  @State phone: string = '';
  @State email: string = '';
  @State remark: string = '';
  @State isLoading: boolean = true;

  aboutToAppear() {
    const params = router.getParams() as Record<string, string>

    // 情况 1:列表页 / 详情页直接传 contact JSON(首屏快)
    if (params && params['contact']) {
      try {
        const contactObj = JSON.parse(params['contact']) as Contact
        console.info('[ContactEditPage] 使用传参 contact:', contactObj)
        this.contactId = contactObj.id
        this.fillData(contactObj)
        this.isLoading = false
        return
      } catch (e) {
        console.error('[ContactEditPage] 解析 contact 失败', e)
      }
    }

    // 情况 2:详情页只传了 contactId(权威数据)
    if (params && params['contactId']) {
      this.contactId = params['contactId']
      console.info('[ContactEditPage] 使用 contactId:', this.contactId)
      this.loadContactData()
      return
    }

    // 都没有 → 非法进入
    promptAction.showToast({ message: '参数错误' })
    router.back()
  }

  /**
   * 将对象数据填充到状态变量
   */
  fillData(c: Contact) {
    this.contactId = c.id || '';
    this.contactKey = c.key || '';  // 关键:保存系统的 key
    this.name = c.name || '';
    this.phone = c.phone || '';
    this.email = c.email || '';
    this.remark = c.remark || '';

    console.info('[ContactEditPage] 填充数据:', {
      id: this.contactId,
      key: this.contactKey,
      name: this.name
    });
  }

  /**
   * 从 Service 重新获取最新的联系人数据
   */
  async loadContactData() {
    this.isLoading = true;
    try {
      const all = await ContactService.getAllContacts();
      console.info('[ContactEditPage] 所有联系人:', all);
      const found = all.find(c => c.id === this.contactId);
      if (found) {
        console.info('[ContactEditPage] 找到联系人:', found);
        this.fillData(found);
      } else {
        console.error('[ContactEditPage] 未找到联系人:', this.contactId);
        promptAction.showToast({ message: '未找到联系人信息' });
        router.back();
      }
    } catch (err) {
      const error = err as BusinessError;
      console.error(`[ContactEditPage] 加载失败: ${error.code} ${error.message}`);
    } finally {
      this.isLoading = false;
    }
  }

  /**
   * 执行更新操作
   */
  async handleUpdate() {
    console.info('[ContactEditPage] 开始更新,key:', this.contactKey, '类型:', typeof this.contactKey);

    // 先验证key是否正确
    if (!this.contactKey || this.contactKey.trim() === '') {
      promptAction.showToast({ message: '错误:联系人标识丢失' });
      console.error('[ContactEditPage] 更新中断:key为空');
      return;
    }

    // 验证联系人是否存在(使用key)
    try {
      const isExist = await ContactService.isContactExist(this.contactKey);
      if (!isExist) {
        promptAction.showToast({ message: '错误:联系人不存在或已被删除' });
        console.error('[ContactEditPage] 联系人不存在,key:', this.contactKey);
        return;
      }
    } catch (err) {
      console.error('[ContactEditPage] 验证联系人存在时出错:', err);
    }

    // 构建更新数据
    const updatedContact = new Contact(this.name.trim(), this.phone.trim());
    updatedContact.key = this.contactKey;  // 确保key是正确的
    updatedContact.id = this.contactId;    // id只用于UI,不用于API
    updatedContact.email = this.email.trim();
    updatedContact.remark = this.remark.trim();

    console.info('[ContactEditPage] 完整的更新数据:', {
      key: updatedContact.key,
      id: updatedContact.id,
      name: updatedContact.name,
      phone: updatedContact.phone
    });

    try {
      await ContactService.updateContact(updatedContact);

      promptAction.showToast({ message: '修改成功' });

      // 【关键修改】触发全局刷新标志,通知列表页和详情页更新
      AppStorage.Set('contactListRefreshFlag', Date.now());
      console.info('[ContactEditPage] 已触发全局刷新标志');

      setTimeout(() => {
        router.back();
      }, 300);

    } catch (err) {
      const error = err as BusinessError;
      console.error(`[ContactEditPage] 更新失败详情:`, error);

      if (error.code === 201) {
        // 201错误:参数错误
        promptAction.showToast({ message: '修改失败:请检查数据格式是否正确' });
      } else if (error.code === 401) {
        promptAction.showToast({ message: '修改失败:联系人不存在或已被删除' });
      } else {
        promptAction.showToast({ message: `修改失败 (错误码:${error.code})` });
      }
    }
  }

  build() {
    Navigation() {
      if (this.isLoading) {
        Column() {
          LoadingProgress().width(50).height(50)
        }.width('100%').justifyContent(FlexAlign.Center).layoutWeight(1)
      } else {
        Column() {
          List() {
            ListItem() {
              this.EditInput('姓名', this.name, (val) => this.name = val)
            }
            ListItem() {
              this.EditInput('手机', this.phone, (val) => this.phone = val, InputType.PhoneNumber)
            }
            ListItem() {
              this.EditInput('邮箱', this.email, (val) => this.email = val, InputType.Email)
            }
            ListItem() {
              this.EditInput('备注', this.remark, (val) => this.remark = val)
            }
          }
          .backgroundColor(Color.White)
          .margin(16)
          .borderRadius(12)
          .divider({ strokeWidth: 1, color: '#F1F3F5', startMargin: 16, endMargin: 16 })

          Button('保存修改')
            .width('90%')
            .height(44)
            .margin({ top: 20 })
            .onClick(() => this.handleUpdate())
        }
        .width('100%')
        .height('100%')
        .backgroundColor('#F5F5F5')
      }
    }
    .title('编辑联系人')
    .titleMode(NavigationTitleMode.Mini)
  }

  @Builder
  EditInput(label: string, value: string, onValueChange: (val: string) => void, type: InputType = InputType.Normal) {
    Row() {
      Text(label).width(64).fontSize(16).fontColor(Color.Black)
      TextInput({ placeholder: `请输入${label}`, text: value })
        .layoutWeight(1)
        .type(type)
        .backgroundColor(Color.Transparent)
        .onChange(onValueChange)
    }
    .padding({ left: 16, right: 16 })
    .height(56).width('100%')
  }
}