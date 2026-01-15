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
// File: entry/src/main/ets/pages/ContactDetailPage.ets
import { Contact, ContactModel } from '../model/ContactModel';
import { SettingModel } from '../model/SettingModel';
import { BiometricService } from '../services/BiometricService';
import promptAction from '@ohos.promptAction';

@Component
export struct ContactDetailPage {
  @State contact: Contact = new Contact('', '', '');

  // 🔒 隐私与解锁状态
  @State isPrivacyOn: boolean = false;
  @State isUnlocked: boolean = false;

  private settingModel = new SettingModel();

  //  [新增] 编辑弹窗控制器
  editDialogController: CustomDialogController = new CustomDialogController({
    builder: EditContactDialog({
      // 传入当前数据用于回显
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

  private getAvatarText(): string {
    const name = this.contact.name || '';
    return name.length > 0 ? name.substring(0, 1).toUpperCase() : '?';
  }

  async aboutToAppear() {
    this.isPrivacyOn = await this.settingModel.isBiometricEnabled();
    if (!this.isPrivacyOn) {
      this.isUnlocked = true;
    }
  }

  async unlock() {
    const success = await BiometricService.getInstance().authenticate();
    if (success) {
      this.isUnlocked = true;
      promptAction.showToast({ message: $r('app.string.unlock_success') });
    } else {
      promptAction.showToast({ message: $r('app.string.unlock_fail') });
    }
  }

  //  [新增] 保存修改逻辑
  async saveContact(name: string, phone: string, relation: string, email: string) {
    // 1. 更新内存对象
    this.contact.name = name;
    this.contact.phone = phone;
    this.contact.relation = relation;
    this.contact.email = email;

    // 2. 更新数据库
    const success = await ContactModel.update(this.contact);

    if (success) {
      promptAction.showToast({ message: '修改成功' });
    } else {
      promptAction.showToast({ message: '修改失败' });
    }
  }

  //  [新增] 点击编辑按钮逻辑
  handleEditClick() {
    // 安全策略：如果开启了隐私保护且未解锁，禁止编辑（防止绕过解锁查看号码）
    if (this.isPrivacyOn && !this.isUnlocked) {
      promptAction.showToast({ message: '请先解锁以编辑隐私信息' });
      this.unlock();
    } else {
      this.editDialogController.open();
    }
  }

  build() {
    NavDestination() {
      GridRow({
        columns: { sm: 4, md: 8, lg: 12 },
        gutter: { x: 12, y: 12 }
      }) {
        GridCol({
          span: { sm: 4, md: 6, lg: 8 },
          offset: { sm: 0, md: 1, lg: 2 }
        }) {
          Column() {
            // --- 头部区域 ---
            Column() {
              Stack({ alignContent: Alignment.Center }) {
                Circle({ width: 80, height: 80 }).fill('#E5F1FF')
                Text(this.getAvatarText())
                  .fontSize(30)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#007DFF')
              }
              .margin({ top: 40, bottom: 16 })

              Text(this.contact.name)
                .fontSize(24)
                .fontWeight(FontWeight.Bold)
                .fontColor(Color.Black)
                .margin({ bottom: 8 })

              Text(this.contact.relation)
                .fontSize(14)
                .fontColor(Color.Gray)
                .padding({ left: 12, right: 12, top: 4, bottom: 4 })
                .backgroundColor('#EFEFEF')
                .borderRadius(16)
            }
            .width('100%')
            .alignItems(HorizontalAlign.Center)

            // --- 详情列表区域 ---
            List() {
              // 📞 电话
              ListItem() {
                Row() {
                  Text($r('app.string.contact_detail_phone'))
                    .width(80)
                    .fontColor(Color.Gray)
                    .fontSize(16)

                  if (this.isUnlocked) {
                    Text(this.contact.phone)
                      .fontWeight(FontWeight.Regular)
                      .fontSize(16)
                      .fontColor(Color.Black)
                      .layoutWeight(1)
                      .textAlign(TextAlign.End)
                  } else {
                    Row() {
                      Text(this.contact.phone.replace(/(\d{3})\d{4}(\d{4})/, '$1****$2'))
                        .fontSize(16)
                        .fontColor(Color.Black)
                        .layoutWeight(1)
                        .textAlign(TextAlign.End)
                        .margin({ right: 8 })

                      Button($r('app.string.btn_view'))
                        .type(ButtonType.Capsule)
                        .height(24)
                        .fontSize(12)
                        .backgroundColor('#007DFF')
                        .onClick(() => { this.unlock(); })
                    }
                    .layoutWeight(1)
                    .justifyContent(FlexAlign.End)
                  }
                }
                .width('100%')
                .padding(16)
                .backgroundColor(Color.White)
              }

              // 📧 邮箱
              this.DetailItem(
                $r('app.string.contact_detail_email'),
                this.contact.email || $r('app.string.contact_detail_empty')
              )

              // 🔗 关系
              this.DetailItem(
                $r('app.string.contact_detail_relation'),
                this.contact.relation
              )
            }
            .width('100%')
            .borderRadius(16)
            .margin({ top: 30 })
            .backgroundColor(Color.White)
            .divider({ strokeWidth: 0.5, color: '#F0F0F0', startMargin: 16, endMargin: 16 })
          }
          .width('100%')
          .height('100%')
          .padding({ left: 16, right: 16 })
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F1F3F5')
    }
    .title($r('app.string.contact_detail_title'))
    .backgroundColor('#F1F3F5')
    //  [新增] 右上角菜单
    .menus([
      {
        value: $r('app.string.btn_edit'),
        icon: $r('app.media.ic_edit_box'),
        action: () => {
          this.handleEditClick();
        }
      }
    ])
  }

  @Builder DetailItem(label: Resource, value: string | Resource) {
    ListItem() {
      Row() {
        Text(label)
          .width(80)
          .fontColor(Color.Gray)
          .fontSize(16)

        Text(value)
          .fontWeight(FontWeight.Regular)
          .fontSize(16)
          .fontColor(Color.Black)
          .layoutWeight(1)
          .textAlign(TextAlign.End)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .maxLines(1)
      }
      .width('100%')
      .padding(16)
      .backgroundColor(Color.White)
    }
  }
}

//  [新增] 编辑弹窗组件
@CustomDialog
struct EditContactDialog {
  controller?: CustomDialogController;
  onConfirm: (name: string, phone: string, relation: string, email: string) => void = () => {};

  // 这里的 State 用来接收初始值并支持用户修改
  @State name: string = '';
  @State phone: string = '';
  @State relation: string = '';
  @State email: string = '';

  build() {
    Column() {
      Text('编辑联系人')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 20 })

      TextInput({ text: this.name, placeholder: $r('app.string.input_name') })
        .onChange((v) => { this.name = v; })
        .margin({ bottom: 12 }).width('90%')

      TextInput({ text: this.phone, placeholder: $r('app.string.input_phone') })
        .onChange((v) => { this.phone = v; })
        .margin({ bottom: 12 }).width('90%')

      TextInput({ text: this.email, placeholder: '请输入邮箱' })
        .onChange((v) => { this.email = v; })
        .margin({ bottom: 12 }).width('90%')

      TextInput({ text: this.relation, placeholder: $r('app.string.input_relation') })
        .onChange((v) => { this.relation = v; })
        .margin({ bottom: 20 }).width('90%')

      Row() {
        Button($r('app.string.btn_cancel'))
          .backgroundColor(Color.Transparent)
          .fontColor(Color.Gray)
          .onClick(() => { this.controller?.close(); })

        Button($r('app.string.btn_save')) // 建议添加 string: "保存"
          .onClick(() => {
            this.onConfirm(this.name, this.phone, this.relation, this.email);
            this.controller?.close();
          })
      }
      .justifyContent(FlexAlign.SpaceEvenly).width('100%').margin({ bottom: 20 })
    }
    .backgroundColor(Color.White).borderRadius(24).width('100%')
  }
}