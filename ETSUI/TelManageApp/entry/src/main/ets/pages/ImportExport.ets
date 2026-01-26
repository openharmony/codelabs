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
import { contact } from '@kit.ContactsKit';
import { router } from '@kit.ArkUI';
import { Context, common } from '@kit.AbilityKit';
import { fileIo } from '@kit.CoreFileKit';

// 定义导出数据接口
interface ExportContactData {
  name: string;
  phones: string[];
  emails: string[];
  company: string;
  address: string;
  notes: string;
}

@Entry
@Component
struct ImportExport {
  @Provide('context') context: Context = getContext(this);
  private contactData: ContactData = new ContactData(this.context);

  // 导出状态
  @State isExporting: boolean = false;
  @State exportMessage: string = '';
  @State exportSuccess: boolean = false;

  // 导出选项
  @State exportFormat: 'json' | 'csv' = 'json';

  // 返回主页
  backToHome() {
    router.back();
  }

  // 获取导出文件路径
  getExportFilePath(fileName: string): string {
    try {
      // 直接使用类型断言为 UIAbilityContext
      const context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;
      const dir: string = context.filesDir;
      return dir + '/' + fileName;
    } catch {
      // 捕获错误但不使用错误对象，避免类型问题
      console.error('获取文件路径失败');
      // 使用硬编码的默认路径
      return '/data/storage/el2/base/files/' + fileName;
    }
  }

  // 生成导出文件名
  generateExportFileName(): string {
    const now: Date = new Date();
    const timestamp: string = `${now.getFullYear()}${(now.getMonth() + 1).toString().padStart(2, '0')}${now.getDate().toString().padStart(2, '0')}_${now.getHours().toString().padStart(2, '0')}${now.getMinutes().toString().padStart(2, '0')}`;

    return `contacts_${timestamp}.${this.exportFormat}`;
  }

  // JSON格式导出
  async exportToJson(contacts: contact.Contact[]): Promise<void> {
    const fileName: string = this.generateExportFileName();
    const filePath: string = this.getExportFilePath(fileName);

    console.log('导出文件路径:', filePath);

    // 转换数据 - 使用明确的类型
    const exportData: ExportContactData[] = contacts.map((contactItem: contact.Contact): ExportContactData => {
      // 处理 note 字段：如果是 Note 类型，转换为字符串
      let notesString: string = '';
      if (contactItem.note) {
        if (typeof contactItem.note === 'string') {
          notesString = contactItem.note;
        } else if (contactItem.note && typeof contactItem.note === 'object') {
          // 假设 Note 类型有 content 或其他属性
          notesString = JSON.stringify(contactItem.note);
        }
      }

      return {
        name: contactItem.name?.fullName || '未知',
        phones: contactItem.phoneNumbers?.map((phone: contact.PhoneNumber): string => phone.phoneNumber) || [],
        emails: contactItem.emails?.map((email: contact.Email): string => email.email) || [],
        company: contactItem.organization?.[0]?.name || '',
        address: contactItem.postalAddresses?.[0]?.street || '',
        notes: notesString
      };
    });

    const jsonString: string = JSON.stringify(exportData, null, 2);

    try {
      const file: fileIo.File = await fileIo.open(filePath, fileIo.OpenMode.READ_WRITE | fileIo.OpenMode.CREATE);
      await fileIo.write(file.fd, jsonString);
      await fileIo.close(file.fd);

      console.log('JSON导出成功');
    } catch {
      console.error('JSON导出失败');
      throw new Error('JSON导出失败');
    }
  }

  // CSV格式导出
  async exportToCsv(contacts: contact.Contact[]): Promise<void> {
    const fileName: string = this.generateExportFileName();
    const filePath: string = this.getExportFilePath(fileName);

    console.log('导出文件路径:', filePath);

    // CSV头部
    let csvContent: string = '姓名,电话,邮箱,公司,地址,备注\n';

    // CSV数据行
    for (const contactItem of contacts) {
      // 处理 note 字段
      let notesString: string = '';
      if (contactItem.note) {
        if (typeof contactItem.note === 'string') {
          notesString = contactItem.note;
        } else if (contactItem.note && typeof contactItem.note === 'object') {
          notesString = JSON.stringify(contactItem.note);
        }
      }

      const row: string[] = [
        `"${contactItem.name?.fullName || '未知'}"`,
        `"${contactItem.phoneNumbers?.map((phone: contact.PhoneNumber): string => phone.phoneNumber).join('; ') || ''}"`,
        `"${contactItem.emails?.map((email: contact.Email): string => email.email).join('; ') || ''}"`,
        `"${contactItem.organization?.[0]?.name || ''}"`,
        `"${contactItem.postalAddresses?.[0]?.street || ''}"`,
        `"${notesString}"`
      ];
      csvContent += row.join(',') + '\n';
    }

    try {
      const file: fileIo.File = await fileIo.open(filePath, fileIo.OpenMode.READ_WRITE | fileIo.OpenMode.CREATE);
      await fileIo.write(file.fd, csvContent);
      await fileIo.close(file.fd);

      console.log('CSV导出成功');
    } catch {
      console.error('CSV导出失败');
      throw new Error('CSV导出失败');
    }
  }

  // 主要导出函数
  async exportContacts(): Promise<void> {
    console.log('开始导出联系人');

    this.isExporting = true;
    this.exportMessage = '正在导出联系人...';
    this.exportSuccess = false;

    try {
      // 1. 获取所有联系人
      const contacts: contact.Contact[] = await this.contactData.queryAllContacts();

      if (contacts.length === 0) {
        this.exportMessage = '没有找到联系人';
        this.isExporting = false;
        return;
      }

      console.log(`找到 ${contacts.length} 个联系人`);

      // 2. 根据选择的格式导出
      if (this.exportFormat === 'json') {
        await this.exportToJson(contacts);
      } else {
        await this.exportToCsv(contacts);
      }

      // 3. 导出成功
      this.exportSuccess = true;
      this.exportMessage = `导出完成！共导出 ${contacts.length} 个联系人`;

    } catch {
      console.error('导出失败');
      this.exportSuccess = false;
      this.exportMessage = '导出失败，请重试';
    } finally {
      this.isExporting = false;
    }
  }

  build() {
    Column() {
      // 顶部标题栏
      Row() {
        // 返回按钮
        Button('< 返回')
          .fontSize(16)
          .margin({ left: 16 })
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => {
            this.backToHome();
          })

        Text('联系人导出')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .layoutWeight(1)
          .textAlign(TextAlign.Center)
      }
      .width('100%')
      .height(56)
      .backgroundColor('#f0f0f0')

      // 主内容
      Column() {
        // 格式选择
        Text('选择导出格式：')
          .fontSize(18)
          .margin({ top: 30, bottom: 20 })
          .width('90%')

        Row() {
          Button('JSON格式')
            .fontSize(16)
            .width('45%')
            .height(45)
            .backgroundColor(this.exportFormat === 'json' ? '#007DFF' : '#e0e0e0')
            .fontColor(this.exportFormat === 'json' ? Color.White : '#333')
            .onClick(() => {
              this.exportFormat = 'json';
            })

          Button('CSV格式')
            .fontSize(16)
            .width('45%')
            .height(45)
            .margin({ left: 10 })
            .backgroundColor(this.exportFormat === 'csv' ? '#007DFF' : '#e0e0e0')
            .fontColor(this.exportFormat === 'csv' ? Color.White : '#333')
            .onClick(() => {
              this.exportFormat = 'csv';
            })
        }
        .width('90%')
        .margin({ bottom: 40 })

        // 状态显示
        if (this.isExporting) {
          Column() {
            LoadingProgress()
              .width(40)
              .height(40)
              .margin({ bottom: 15 })

            Text(this.exportMessage)
              .fontSize(16)
              .fontColor('#666')
          }
          .width('100%')
          .margin({ top: 20 })

        } else if (this.exportMessage) {
          Column() {
            Text(this.exportMessage)
              .fontSize(16)
              .fontColor(this.exportSuccess ? '#34C759' : '#ff3b30')
              .textAlign(TextAlign.Center)

            if (this.exportSuccess) {
              Button('再次导出')
                .width(120)
                .height(40)
                .margin({ top: 20 })
                .backgroundColor('#e0e0e0')
                .fontColor('#333')
                .onClick(() => {
                  this.exportMessage = '';
                })
            } else {
              Button('重试')
                .width(120)
                .height(40)
                .margin({ top: 20 })
                .backgroundColor('#ff3b30')
                .fontColor(Color.White)
                .onClick(() => {
                  this.exportMessage = '';
                })
            }
          }
          .width('100%')
          .margin({ top: 20 })
        }

        // 导出按钮
        if (!this.isExporting && !this.exportMessage) {
          Button('导出联系人')
            .width('90%')
            .height(50)
            .margin({ top: 40 })
            .backgroundColor('#007DFF')
            .fontColor(Color.White)
            .fontSize(18)
            .fontWeight(FontWeight.Medium)
            .onClick(() => {
              this.exportContacts();
            })
        }

        // 说明文字
        Text('说明：\n1. JSON格式适合程序读取\n2. CSV格式适合用Excel打开\n3. 文件保存在应用目录中')
          .fontSize(14)
          .fontColor('#666')
          .margin({ top: 60 })
          .width('90%')
          .textAlign(TextAlign.Start)

      }
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.Start)
      .alignItems(HorizontalAlign.Center)

    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }
}