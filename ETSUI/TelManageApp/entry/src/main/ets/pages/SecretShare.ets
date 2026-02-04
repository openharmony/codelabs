/*
 * 模块功能：密令分享页面。
 * 作用：将联系人信息编码为短密令，支持线下分享。
 * 输出：密令展示与说明。
 */

import { router } from '@kit.ArkUI';
import pasteboard from '@ohos.pasteboard';
import { contact } from '@kit.ContactsKit';
import { Context } from '@kit.AbilityKit';
import { ContactData } from '../model/ContactData';
import { SecretCodeUtil } from '../utils/SecretCodeUtil';
import { ValidationUtil } from '../utils/ValidationUtil';

@Entry
@Component
struct SecretShare {
  private contactData: ContactData = new ContactData();

  @State contactName: string = '';
  @State contactPhone: string = '';
  @State contactEmail: string = '';
  @State secretCode: string = '';
  @State formattedCode: string = '';
  @State statusText: string = '准备生成密令';
  @State copyStatusText: string = '';
  @State validationText: string = '';
  @State codeLengthText: string = '';

  /**
   * 生命周期函数：页面显示前准备上下文与数据。
   * 输入：无。
   * 输出：无。
   */
  aboutToAppear(): void {
    this.contactData.setContext(getContext(this) as Context);
    this.loadContactFromParams();
  }

  /**
   * 从路由参数加载联系人并生成密令。
   * 输入：无。
   * 输出：无。
   */
  private loadContactFromParams(): void {
    const params = router.getParams() as Record<string, string>;
    const key = params?.contactKey ?? '';
    if (!key) {
      this.statusText = '缺少联系人参数';
      return;
    }
    this.fetchAndBuildCode(key);
  }

  /**
   * 查询联系人并生成密令。
   * 输入：key 联系人 key。
   * 输出：无。
   */
  private async fetchAndBuildCode(key: string): Promise<void> {
    try {
      const contactItem = await this.contactData.queryContactById(key);
      if (!contactItem) {
        this.statusText = '联系人不存在';
        return;
      }
      this.applyContactSummary(contactItem);
      const code = this.buildSecretCode(contactItem);
      this.secretCode = code;
      this.formattedCode = SecretCodeUtil.formatForDisplay(code);
      this.statusText = '密令已生成';
      this.validationText = this.buildValidationText(contactItem);
      this.codeLengthText = this.buildCodeLengthText(code);
      await this.copyCodeToClipboard(this.secretCode);
    } catch (error) {
      this.statusText = '生成密令失败';
      console.error('SecretShare: 生成密令失败', error);
    }
  }

  /**
   * 提取联系人摘要信息。
   * 输入：contactItem 联系人对象。
   * 输出：无。
   */
  private applyContactSummary(contactItem: contact.Contact): void {
    this.contactName = contactItem.name?.fullName ?? '未知联系人';
    this.contactPhone = contactItem.phoneNumbers?.[0]?.phoneNumber ?? '';
    this.contactEmail = contactItem.emails?.[0]?.email ?? '';
  }

  /**
   * 构建密令字符串。
   * 输入：contactItem 联系人对象。
   * 输出：密令字符串。
   */
  private buildSecretCode(contactItem: contact.Contact): string {
    return SecretCodeUtil.encode({
      name: contactItem.name?.fullName ?? '',
      phone: contactItem.phoneNumbers?.[0]?.phoneNumber ?? '',
      email: contactItem.emails?.[0]?.email ?? ''
    });
  }

  /**
   * 构建字段验证文案。
   * 输入：contactItem 联系人对象。
   * 输出：验证文案。
   */
  private buildValidationText(contactItem: contact.Contact): string {
    const name = contactItem.name?.fullName ?? '';
    const phone = contactItem.phoneNumbers?.[0]?.phoneNumber ?? '';
    const email = contactItem.emails?.[0]?.email ?? '';
    const nameResult = ValidationUtil.validateName(name);
    const phoneResult = ValidationUtil.validatePhone(phone);
    const emailResult = ValidationUtil.validateEmail(email);
    if (nameResult.isValid && phoneResult.isValid && emailResult.isValid) {
      return '字段校验通过';
    }
    return [nameResult, phoneResult, emailResult]
      .filter((item) => !item.isValid)
      .map((item) => item.message)
      .join('，');
  }

  /**
   * 构建密令长度文案。
   * 输入：code 密令字符串。
   * 输出：长度文案。
   */
  private buildCodeLengthText(code: string): string {
    const normalized = SecretCodeUtil.normalizeCode(code);
    return `长度 ${normalized.length} 字符`;
  }

  /**
   * 将密令复制到剪贴板。
   * 输入：code 密令字符串。
   * 输出：无。
   */
  private async copyCodeToClipboard(code: string): Promise<void> {
    if (!code) {
      return;
    }
    try {
      const board = pasteboard.getSystemPasteboard();
      const data = pasteboard.createData(pasteboard.MIMETYPE_TEXT_PLAIN, code);
      await board.setData(data);
      this.copyStatusText = '已自动复制到剪贴板';
    } catch (error) {
      this.copyStatusText = '自动复制失败，请手动复制';
      console.error('SecretShare: 复制失败', error);
    }
  }

  /**
   * 手动触发复制。
   * 输入：无。
   * 输出：无。
   */
  private onCopyClick(): void {
    this.copyCodeToClipboard(this.secretCode);
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
   * 构建页面 UI。
   * 输入：无。
   * 输出：无。
   */
  build(): void {
    Column() {
      this.buildHeader();
      this.buildContent();
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

      Text('密令分享')
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
   * 构建内容区域。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildContent(): void {
    Column() {
      this.buildContactCard();
      this.buildSecretCard();
      this.buildHintText();
    }
    .width('100%')
    .alignItems(HorizontalAlign.Center)
    .margin({ top: 16 })
  }

  /**
   * 构建联系人信息卡片。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildContactCard(): void {
    Column() {
      Text(this.contactName)
        .fontSize(18)
        .fontWeight(FontWeight.Medium)
        .margin({ bottom: 6 })

      if (this.contactPhone) {
        Text(this.contactPhone)
          .fontSize(14)
          .fontColor('#666')
      }

      if (this.contactEmail) {
        Text(this.contactEmail)
          .fontSize(12)
          .fontColor('#999')
          .margin({ top: 4 })
      }
    }
    .width('90%')
    .padding(16)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
  }

  /**
   * 构建密令展示卡片。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildSecretCard(): void {
    Column() {
      this.buildSecretTitle();
      this.buildCodeBox();
      this.buildCodeMeta();
      this.buildCopyButton();
      this.buildStatusTexts();
    }
    .width('90%')
    .padding(16)
    .margin({ top: 16 })
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
  }

  /**
   * 构建密令标题。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildSecretTitle(): void {
    Text('密令')
      .fontSize(14)
      .fontColor('#666')
      .margin({ bottom: 8 })
  }

  /**
   * 构建密令显示框。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildCodeBox(): void {
    Text(this.formattedCode || '生成中...')
      .fontSize(13)
      .fontColor('#333')
      .textAlign(TextAlign.Center)
      .width('100%')
      .padding(10)
      .backgroundColor('#f4f4f4')
      .borderRadius(8)
  }

  /**
   * 构建复制按钮。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildCopyButton(): void {
    Button('复制密令')
      .width('100%')
      .height(40)
      .margin({ top: 10 })
      .backgroundColor('#007DFF')
      .fontColor(Color.White)
      .enabled(!!this.secretCode)
      .onClick(() => {
        this.onCopyClick();
      })
  }

  /**
   * 构建密令元信息。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildCodeMeta(): void {
    if (this.codeLengthText) {
      Text(this.codeLengthText)
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 6 })
    }
  }

  /**
   * 构建状态提示文本。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildStatusTexts(): void {
    Text(this.statusText)
      .fontSize(12)
      .fontColor('#666')
      .margin({ top: 10 })

    if (this.copyStatusText) {
      Text(this.copyStatusText)
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 4 })
    }

    if (this.validationText) {
      Text(this.validationText)
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 4 })
    }
  }

  /**
   * 构建提示文本。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildHintText(): void {
    Text('说明：密令已自动复制，可发送给对方在“密令导入”页面使用。')
      .fontSize(12)
      .fontColor('#999')
      .margin({ top: 16 })
      .width('90%')
      .textAlign(TextAlign.Start)
  }
}

