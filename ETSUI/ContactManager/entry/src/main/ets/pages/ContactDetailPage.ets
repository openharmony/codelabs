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
import router from '@ohos.router'
import promptAction from '@ohos.promptAction'
import { Routes } from '../constants/Routes'
import { Contact } from '../model/Contact'
import { ContactService } from '../service/ContactService'
import { MetadataService } from '../service/MetadataService'
import { ImportanceLevel } from '../model/ContactMetadata'
import { RecycleBinService } from '../service/RecycleBinService'

interface ContactDetailParams {
  contactId?: string
  contact?: string
}

@Entry
struct ContactDetailPage {
  @State contact: Contact = new Contact('', '')
  @State isLoading: boolean = true
  @State importance: ImportanceLevel = ImportanceLevel.NORMAL
  private contactId: string = ''

  aboutToAppear(): void {
    const params = router.getParams() as ContactDetailParams

    if (params?.contactId) {
      this.contactId = params.contactId
      this.fetchContactDetail()
    } else if (params?.contact) {
      try {
        const passedContact = JSON.parse(params.contact) as Contact
        this.contact = passedContact
        this.contactId = passedContact.id
        this.fetchContactDetail()
      } catch {
        console.error('解析联系人数据失败')
      }
    }
  }

  onPageShow(): void {
    if (this.contactId) {
      this.fetchContactDetail()
    }
  }

  async fetchContactDetail(): Promise<void> {
    this.isLoading = true
    try {
      const allContacts = await ContactService.getAllContacts()
      const found = allContacts.find((item: Contact): boolean => item.id === this.contactId)

      if (found) {
        const newContact = new Contact(found.name, found.phone)
        newContact.id = found.id
        newContact.email = found.email ?? ''
        newContact.remark = found.remark ?? ''
        newContact.key = found.key
        this.contact = newContact

        const metadata = await MetadataService.getMetadata(this.contact.key)
        this.importance = metadata.importance

        // 记录查看操作
        await MetadataService.recordView(this.contact.key)
      } else {
        promptAction.showToast({ message: '联系人不存在' })
        router.back()
      }
    } catch {
      promptAction.showToast({ message: '加载失败' })
    } finally {
      this.isLoading = false
    }
  }

  private getImportanceText(level: ImportanceLevel): string {
    if (level === ImportanceLevel.EMERGENCY) return '已标记为紧急联系人'
    if (level === ImportanceLevel.IMPORTANT) return '已标记为重要联系人'
    return '已取消标记'
  }

  async handleToggleImportance(): Promise<void> {
    try {
      const newImportance = await MetadataService.toggleImportance(this.contact.key)
      this.importance = newImportance
      promptAction.showToast({ message: this.getImportanceText(newImportance) })
      PersistentStorage.PersistProp('contactListRefreshFlag', Date.now())
    } catch {
      promptAction.showToast({ message: '操作失败' })
    }
  }

  handleEdit(): void {
    router.pushUrl({
      url: Routes.CONTACT_EDIT,
      params: { contactId: this.contactId }
    })
  }

  handleDelete(): void {
    AlertDialog.show({
      title: '删除联系人',
      message: `确定要删除联系人"${this.contact.name}"吗？\n删除后可在回收站中恢复（7天内有效）`,
      primaryButton: {
        value: '取消',
        action: (): void => {}
      },
      secondaryButton: {
        value: '删除',
        fontColor: Color.Red,
        action: async (): Promise<void> => {
          try {
            // 1. 获取元数据（如果有）
            const metadata = await MetadataService.getMetadata(this.contact.key)
            
            // 2. 备份到回收站（软删除）
            await RecycleBinService.addDeletedContact(this.contact, metadata)
            
            // 3. 从系统删除（硬删除）
            await ContactService.deleteContact(this.contact.key)
            
            // 4. 删除元数据
            await MetadataService.deleteMetadata(this.contact.key)
            
            promptAction.showToast({ message: '已移入回收站，7天内可恢复' })
            
            // 5. 触发全局刷新
            PersistentStorage.PersistProp('contactListRefreshFlag', Date.now())
            
            router.back()
          } catch (err) {
            console.error('[ContactDetailPage] 删除失败:', err)
            promptAction.showToast({ message: '删除失败，请检查权限' })
          }
        }
      }
    })
  }

  build(): void {
    Navigation() {
      Column() {
        if (this.isLoading) {
          Column() {
            LoadingProgress().width(50).height(50)
          }
          .width('100%')
          .height('100%')
          .justifyContent(FlexAlign.Center)
        } else {
          Column() {
            /** 头像区域（根据重要程度变色） */
            Text(this.contact.name ? this.contact.name.charAt(0) : '?')
              .width(80)
              .height(80)
              .borderRadius(40)
              .backgroundColor(
                this.importance === ImportanceLevel.EMERGENCY ? '#FF4444' :
                  this.importance === ImportanceLevel.IMPORTANT ? '#FF9800' :
                    '#007DFF'
              )
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
              .margin({ bottom: 16 })

            /** ⭐ 标记按钮 */
            Button({ type: ButtonType.Normal }) {
              Text(
                this.importance === ImportanceLevel.EMERGENCY ? '设为紧急联系人' :
                  this.importance === ImportanceLevel.IMPORTANT ? '设为重要联系人' :
                    '标记为重要联系人'
              )
                .fontSize(14)
                .fontColor(
                  this.importance === ImportanceLevel.EMERGENCY ? '#FF4444' :
                    this.importance === ImportanceLevel.IMPORTANT ? '#FF9800' :
                      '#666666'
                )
            }
            .height(36)
            .padding({ left: 16, right: 16 })
            .backgroundColor('#F5F5F5')
            .borderRadius(18)
            .onClick(() => this.handleToggleImportance())
          }
          .width('100%')
          .padding({ top: 40, bottom: 40 })
          .backgroundColor(Color.White)

          List() {
            ListItem() { this.DetailItem('手机', this.contact.phone) }
            ListItem() { this.DetailItem('邮箱', this.contact.email || '未填写') }
            ListItem() { this.DetailItem('备注', this.contact.remark || '无') }
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
    .title('联系人详情')
    .titleMode(NavigationTitleMode.Mini)
  }

  @Builder
  DetailItem(label: string, value: string): void {
    Row() {
      Text(label).width(80).fontSize(16).fontColor(Color.Gray)
      Text(value).layoutWeight(1).fontSize(16).fontColor(Color.Black)
    }
    .width('100%')
    .padding(18)
  }
}
