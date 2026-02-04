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
/*
 * 模块功能：密令导入页面。
 * 作用：解析密令并导入联系人信息。
 * 输出：导入结果提示。
 */

import { router } from '@kit.ArkUI';
import { Context } from '@kit.AbilityKit';
import { ContactData } from '../model/ContactData';
import { SecretCodeUtil, SecretContactFields } from '../utils/SecretCodeUtil';
import { ValidationUtil } from '../utils/ValidationUtil';

@Entry
@Component
struct SecretImport {
  private contactData: ContactData = new ContactData();
  @State inputCode: string = '';
  @State statusText: string = '请输入密令';
  @State parsedFields: SecretContactFields | null = null;
  @State isSaving: boolean = false;
  @State pasteStatusText: string = '';
  @State isEditingFields: boolean = false;
  @State editName: string = '';
  @State editPhone: string = '';
  @State editEmail: string = '';
  @State fieldValidationText: string = '';
  @State inputLengthText: string = '';

  /**
   * 生命周期函数：页面显示前准备上下文。
   * 输入：无。
   * 输出：无。
   */
  aboutToAppear(): void {
    this.contactData.setContext(getContext(this) as Context);
  }

  /**
   * 解析密令。
   * 输入：无。
   * 输出：无。
   */
  parseCode(): void {
    if (!this.inputCode.trim()) {
      this.statusText = '密令不能为空';
      this.parsedFields = null;
      this.fieldValidationText = '';
      return;
    }
    try {
      const normalized = SecretCodeUtil.normalizeCode(this.inputCode.trim());
      this.parsedFields = SecretCodeUtil.decode(normalized);
      this.applyEditableFields(this.parsedFields);
      this.statusText = '解析成功，请确认导入';
      this.fieldValidationText = this.buildValidationText();
      this.inputLengthText = this.buildInputLengthText(normalized);
    } catch (error) {
      this.parsedFields = null;
      this.statusText = '密令解析失败';
      this.fieldValidationText = '';
      console.error('SecretImport: 解析失败', error);
    }
  }

  /**
   * 提示用户粘贴密令。
   * 输入：无。
   * 输出：无。
   */
  private showPasteHint(): void {
    try {
      this.pasteStatusText = '请长按输入框粘贴密令';
    } catch (error) {
      this.pasteStatusText = '提示失败，请手动输入';
      console.error('SecretImport: 提示粘贴失败', error);
    }
  }

  /**
   * 确认导入联系人。
   * 输入：无。
   * 输出：无。
   */
  async confirmImport(): Promise<void> {
    if (!this.parsedFields || this.isSaving) {
      return;
    }
    if (!this.isFieldsValid()) {
      this.statusText = '请修正字段后再导入';
      return;
    }
    this.isSaving = true;
    try {
      const contactItem = this.contactData.createContact(
        this.getFinalFields().name,
        this.getFinalFields().phone,
        this.getFinalFields().email
      );
      await this.contactData.addContact(contactItem);
      this.statusText = '联系人已添加';
      this.goBack();
    } catch (error) {
      this.statusText = '联系人导入失败';
      console.error('SecretImport: 导入失败', error);
    } finally {
      this.isSaving = false;
    }
  }

  /**
   * 返回上一页。
   * 输入：无。
   * 输出：无。
   */
  private goBack(): void {
    router.back();
  }

  /**
   * 应用解析后的字段到可编辑状态。
   * 输入：fields 联系人字段。
   * 输出：无。
   */
  private applyEditableFields(fields: SecretContactFields): void {
    this.editName = fields.name;
    this.editPhone = fields.phone;
    this.editEmail = fields.email;
  }

  /**
   * 构建输入长度提示。
   * 输入：code 密令字符串。
   * 输出：提示文案。
   */
  private buildInputLengthText(code: string): string {
    const normalized = SecretCodeUtil.normalizeCode(code);
    return normalized ? `当前长度 ${normalized.length} 字符` : '';
  }

  /**
   * 获取最终导入字段。
   * 输入：无。
   * 输出：联系人字段。
   */
  private getFinalFields(): SecretContactFields {
    if (!this.isEditingFields) {
      return this.parsedFields!;
    }
    return {
      name: this.editName.trim(),
      phone: ValidationUtil.normalizePhone(this.editPhone),
      email: this.editEmail.trim()
    };
  }

  /**
   * 构建字段验证文案。
   * 输入：无。
   * 输出：验证文案。
   */
  private buildValidationText(): string {
    const fields = this.getFinalFields();
    const nameResult = ValidationUtil.validateName(fields.name);
    const phoneResult = ValidationUtil.validatePhone(fields.phone);
    const emailResult = ValidationUtil.validateEmail(fields.email);
    if (nameResult.isValid && phoneResult.isValid && emailResult.isValid) {
      return '字段校验通过';
    }
    return [nameResult, phoneResult, emailResult]
      .filter((item) =>!item.isValid)
      .map((item) => item.message)
      .join('，');
  }

  /**
   * 判断字段是否有效。
   * 输入：无。
   * 输出：是否有效。
   */
  private isFieldsValid(): boolean {
    return this.buildValidationText() === '字段校验通过';
  }

  /**
   * 切换编辑模式。
   * 输入：无。
   * 输出：无。
   */
  private toggleEditMode(): void {
    this.isEditingFields = !this.isEditingFields;
    if (this.parsedFields) {
      this.applyEditableFields(this.parsedFields);
    }
    this.fieldValidationText = this.buildValidationText();
  }

  /**
   * 构建页面 UI。
   * 输入：无。
   * 输出：无。
   */
  build(): void {
    Column() {
      this.buildHeader();
      this.buildInputCard();
      this.buildPreviewCard();
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  /**
   * 构建标题栏。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildHeader(): void {
    Row() {
      Button('< 返回')
        .fontSize(16)
        .margin({ left: 16 })
        .backgroundColor(Color.Transparent)
        .fontColor('#007DFF')
        .onClick(() => {
          this.goBack();
        })

      Text('密令导入')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .layoutWeight(1)
        .textAlign(TextAlign.Center)
    }
    .width('100%')
    .height(56)
    .backgroundColor('#f0f0f0')
  }

  /**
   * 构建输入卡片。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildInputCard(): void {
    Column() {
      this.buildInputTitle();
      this.buildInputArea();
      this.buildInputActions();
      this.buildInputStatus();
    }
    .width('90%')
    .padding(16)
    .margin({ top: 16 })
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({
      radius: 2,
      color: '#10000000',
      offsetX: 0,
      offsetY: 1
    })
  }

  /**
   * 构建输入标题。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildInputTitle(): void {
    Text('密令输入')
      .fontSize(14)
      .fontColor('#666')
      .margin({ bottom: 8 })
  }

  /**
   * 构建输入区域。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildInputArea(): void {
    TextArea({ placeholder: '粘贴或输入密令' })
      .height(120)
      .width('100%')
      .backgroundColor('#f4f4f4')
      .borderRadius(8)
      .padding(10)
      .onChange((value: string) => {
        this.inputCode = value.trim();
        this.inputLengthText = this.buildInputLengthText(this.inputCode);
      })
  }

  /**
   * 构建输入动作按钮。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildInputActions(): void {
    Row() {
      Button('粘贴提示')
        .layoutWeight(1)
        .height(42)
        .backgroundColor('#e0e0e0')
        .fontColor('#333')
        .onClick(() => {
          this.showPasteHint();
        })

      Button('解析密令')
        .layoutWeight(1)
        .height(42)
        .margin({ left: 10 })
        .backgroundColor('#007DFF')
        .fontColor(Color.White)
        .onClick(() => {
          this.parseCode();
        })
    }
    .width('100%')
    .margin({ top: 12 })
  }

  /**
   * 构建输入状态提示。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildInputStatus(): void {
    Text(this.statusText)
      .fontSize(12)
      .fontColor('#666')
      .margin({ top: 8 })

    if (this.pasteStatusText) {
      Text(this.pasteStatusText)
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 4 })
    }

    if (this.inputLengthText) {
      Text(this.inputLengthText)
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 4 })
    }

    if (this.fieldValidationText) {
      Text(this.fieldValidationText)
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 4 })
    }
  }

  /**
   * 构建解析预览卡片。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildPreviewCard(): void {
    Column() {
      this.buildPreviewTitle();
      this.buildPreviewFields();
      this.buildPreviewActions();
    }
    .width('90%')
    .padding(16)
    .margin({ top: 16 })
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({
      radius: 2,
      color: '#10000000',
      offsetX: 0,
      offsetY: 1
    })
  }

  /**
   * 构建预览标题。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildPreviewTitle(): void {
    Text('预览信息')
      .fontSize(14)
      .fontColor('#666')
      .margin({ bottom: 8 })
  }

  /**
   * 构建预览字段区域。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildPreviewFields(): void {
    if (this.parsedFields) {
      this.buildEditableFields();
    } else {
      Text('暂无解析结果')
        .fontSize(14)
        .fontColor('#999')
        .padding(8)
    }
  }

  /**
   * 构建预览操作区。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildPreviewActions(): void {
    if (this.parsedFields) {
      Button(this.isEditingFields ? '完成编辑' : '修改字段')
        .width('100%')
        .height(40)
        .margin({ top: 10 })
        .backgroundColor('#e0e0e0')
        .fontColor('#333')
        .onClick(() => {
          this.toggleEditMode();
        })
    }

    Button(this.isSaving ? '导入中...' : '确认导入')
      .width('100%')
      .height(44)
      .margin({ top: 12 })
      .backgroundColor('#34C759')
      .fontColor(Color.White)
      .enabled(!!this.parsedFields && !this.isSaving)
      .onClick(() => {
        this.confirmImport();
      })
  }

  /**
   * 构建字段行。
   * 输入：label 标题，value 值。
   * 输出：无。
   */
  @Builder
  private buildFieldRow(label: string, value: string): void {
    Row() {
      Text(label)
        .fontSize(14)
        .fontColor('#666')
        .width(60)

      Text(value)
        .fontSize(16)
        .fontColor('#333')
        .layoutWeight(1)
    }
    .width('100%')
    .margin({ bottom: 8 })
  }

  /**
   * 构建可编辑字段区域。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildEditableFields(): void {
    if (this.isEditingFields) {
      this.buildEditableField('姓名', this.editName, (value: string) => {
        this.editName = value;
      });
      this.buildEditableField('电话', this.editPhone, (value: string) => {
        this.editPhone = value;
      });
      this.buildEditableField('邮箱', this.editEmail, (value: string) => {
        this.editEmail = value;
      });
    } else {
      this.buildFieldRow('姓名', this.parsedFields!.name);
      this.buildFieldRow('电话', this.parsedFields!.phone);
      this.buildFieldRow('邮箱', this.parsedFields!.email || '无');
    }
  }

  /**
   * 构建可编辑字段输入。
   * 输入：label 标题，value 值，onChange 变更回调。
   * 输出：无。
   */
  @Builder
  private buildEditableField(label: string, value: string, onChange: (value: string) => void): void {
    Column() {
      Text(label)
        .fontSize(12)
        .fontColor('#666')
        .margin({ bottom: 4 })

      TextInput({ text: value, placeholder: `请输入${label}` })
        .width('100%')
        .height(38)
        .backgroundColor('#f4f4f4')
        .borderRadius(6)
        .padding({ left: 10, right: 10 })
        .onChange((text: string) => {
          onChange(text);
          this.fieldValidationText = this.buildValidationText();
        })
    }
    .width('100%')
    .margin({ bottom: 10 })
  }
}

