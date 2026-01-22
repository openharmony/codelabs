/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
// src/main/ets/pages/ContactDetailPage.ets
import { Contact, ContactModel } from '../model/ContactModel';
import { SettingModel } from '../model/SettingModel';
import { BiometricService } from '../services/BiometricService';
import promptAction from '@ohos.promptAction';
import { emitter } from '@kit.BasicServicesKit';
import { Logger } from '../common/utils/Logger';
// 📝 [import] 确保 ContactViewModel 文件存在于 viewmodel 目录下
import { ContactViewModel } from '../viewmodel/ContactViewModel';

const logger = new Logger('ContactDetailPage');
const EVENT_ID_REFRESH = 1001; // 必须和 ListPage 保持一致

@Component
export struct ContactDetailPage {
  @State contact: Contact = new Contact('', '', '');
  @State isPrivacyOn: boolean = false;
  @State isUnlocked: boolean = false;
  editDialogController: CustomDialogController = new CustomDialogController({
    builder: EditContactDialog({
      name: this.contact.name,
      phone: this.contact.phone,
      relation: this.contact.relation,
      email: this.contact.email,
      onConfirm: (name, phone, relation, email) => {
        this.saveContact(name, phone, relation, email);
      }
    }),
    alignment: DialogAlignment.Bottom,
    autoCancel: true
  });
  private settingModel = new SettingModel();
  // 📝 [修复 2] 显式标注类型，解决 arkts-no-any-unknown 错误
  private contactViewModel: ContactViewModel = new ContactViewModel();

  async aboutToAppear() {
    this.isPrivacyOn = await this.settingModel.isBiometricEnabled();
    if (!this.isPrivacyOn) {
      this.isUnlocked = true;
    }
  }

  async unlock() {
    // ✅ 传入当前上下文 getContext(this)
    const success = await BiometricService.getInstance().authenticate(getContext(this));
    if (success) {
      this.isUnlocked = true;
      promptAction.showToast({ message: $r('app.string.unlock_success') });
    } else {
      promptAction.showToast({ message: $r('app.string.unlock_fail') });
    }
  }

  async saveContact(name: string, phone: string, relation: string, email: string) {
    logger.info(`【保存】尝试保存联系人: ${name}`);

    // 使用 ViewModel 校验输入 (国际化)
    const errorKey = this.contactViewModel.validateContact(name, phone);
    if (errorKey) {
      promptAction.showToast({ message: $r(`app.string.${errorKey}`) });
      return;
    }

    this.contact.name = name;
    this.contact.phone = phone;
    this.contact.relation = relation;
    this.contact.email = email;

    const success = await ContactModel.update(this.contact);

    if (success) {
      logger.info('【保存】数据库更新成功');
      promptAction.showToast({ message: $r('app.string.contact_toast_save_success') });

      // [关键] 发送刷新事件
      logger.info('【事件】发送刷新事件 ID: 1001');
      emitter.emit({ eventId: EVENT_ID_REFRESH });

    } else {
      logger.error('【保存】数据库更新失败');
      promptAction.showToast({ message: $r('app.string.contact_toast_save_fail') });
    }
  }

  handleEditClick() {
    if (this.isPrivacyOn && !this.isUnlocked) {
      promptAction.showToast({ message: $r('app.string.contact_toast_unlock_edit') });
      this.unlock();
    } else {
      this.editDialogController.open();
    }
  }

  build() {
    NavDestination() {
      Column() {
        // 头部
        Column() {
          Stack({ alignContent: Alignment.Center }) {
            Circle({ width: 80, height: 80 }).fill('#E5F1FF')
            Text(this.getAvatarText()).fontSize(30).fontWeight(FontWeight.Bold).fontColor('#007DFF')
          }
          .margin({ top: 40, bottom: 16 })

          Text(this.contact.name).fontSize(24).fontWeight(FontWeight.Bold).margin({ bottom: 8 })
          Text(this.contact.relation)
            .fontSize(14)
            .fontColor(Color.Gray)
            .padding(6)
            .backgroundColor('#EFEFEF')
            .borderRadius(16)
        }
        .width('100%').alignItems(HorizontalAlign.Center)

        // 列表
        List() {
          ListItem() {
            Row() {
              Text($r('app.string.contact_detail_phone')).width(80).fontColor(Color.Gray).fontSize(16)
              if (this.isUnlocked) {
                Text(this.contact.phone).fontSize(16).layoutWeight(1).textAlign(TextAlign.End)
              } else {
                Row() {
                  Text(this.contact.phone.replace(/(\d{3})\d{4}(\d{4})/, '$1****$2'))
                    .fontSize(16).layoutWeight(1).textAlign(TextAlign.End).margin({ right: 8 })
                  Button($r('app.string.btn_view')).type(ButtonType.Capsule).height(24).fontSize(12).onClick(() => {
                    this.unlock();
                  })
                }
                .layoutWeight(1).justifyContent(FlexAlign.End)
              }
            }
            .width('100%').padding(16).backgroundColor(Color.White)
          }

          this.DetailItem($r('app.string.contact_label_email'),
            this.contact.email || $r('app.string.contact_value_empty'))
          this.DetailItem($r('app.string.contact_label_relation'), this.contact.relation)
        }
        .width('100%').borderRadius(16).margin({ top: 30 }).backgroundColor(Color.White)
      }
      .width('100%').height('100%').padding({ left: 16, right: 16 })
    }
    .title($r('app.string.contact_detail_title'))
    .backgroundColor('#F1F3F5')
    .menus([{
      value: $r('app.string.btn_edit'),
      icon: $r('app.media.ic_edit_box'),
      action: () => {
        this.handleEditClick();
      }
    }])
  }

  @Builder
  DetailItem(label: string | Resource, value: string | Resource) {
    ListItem() {
      Row() {
        Text(label).width(80).fontColor(Color.Gray).fontSize(16)
        Text(value).fontSize(16).fontColor(Color.Black).layoutWeight(1).textAlign(TextAlign.End)
      }
      .width('100%').padding(16).backgroundColor(Color.White)
    }
  }

  private getAvatarText(): string {
    const name = this.contact.name || '';
    return name.length > 0 ? name.substring(0, 1).toUpperCase() : '?';
  }
}

@CustomDialog
struct EditContactDialog {
  controller?: CustomDialogController;
  @State name: string = '';
  @State phone: string = '';
  @State relation: string = '';
  @State email: string = '';

  onConfirm: (name: string, phone: string, relation: string, email: string) => void = () => {
  };

  build() {
    Column() {
      // 这里的 "编辑联系人" 可以复用 contact_btn_edit 或者 common_edit
      Text($r('app.string.contact_btn_edit')).fontSize(20).fontWeight(FontWeight.Bold).margin({ top: 20, bottom: 20 })
      TextInput({ text: this.name, placeholder: $r('app.string.contact_label_name') }).onChange((v) => {
        this.name = v;
      }).margin({ bottom: 12 }).width('90%')
      TextInput({ text: this.phone, placeholder: $r('app.string.contact_label_phone') }).onChange((v) => {
        this.phone = v;
      }).margin({ bottom: 12 }).width('90%')
      TextInput({ text: this.email, placeholder: $r('app.string.contact_label_email') }).onChange((v) => {
        this.email = v;
      }).margin({ bottom: 12 }).width('90%')
      TextInput({ text: this.relation, placeholder: $r('app.string.contact_label_relation') }).onChange((v) => {
        this.relation = v;
      }).margin({ bottom: 20 }).width('90%')
      Row() {
        Button($r('app.string.common_cancel')).backgroundColor(Color.Transparent).fontColor(Color.Gray).onClick(() => {
          this.controller?.close();
        })
        Button($r('app.string.common_save')).onClick(() => {
          this.onConfirm(this.name, this.phone, this.relation, this.email);
          this.controller?.close();
        })
      }
      .justifyContent(FlexAlign.SpaceEvenly).width('100%').margin({ bottom: 20 })
    }
    .backgroundColor(Color.White).borderRadius(24).width('100%')
  }
}