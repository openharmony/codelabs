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

import router from '@ohos.router'
import promptAction from '@ohos.promptAction'
import { Contact } from '../model/Contact'
import RdbStore from '../utils/RdbStore'

/**
 * 页面路由参数类型（显式定义，禁止 any / unknown）
 */
interface ContactEditParams {
  contactId?: number
}

@Entry
@Component
struct ContactEditPage {
  @State contactId: number = 0
  @State name: string = ''
  @State phone: string = ''
  @State company: string = ''
  @State usageInfo: string = ''
  @State email: string = ''

  /**
   * 页面显示时加载原有数据
   */
  async aboutToAppear(): Promise<void> {
    const params: ContactEditParams | undefined =
      router.getParams() as ContactEditParams | undefined

    if (params !== undefined && typeof params.contactId === 'number') {
      this.contactId = params.contactId

      const list: Contact[] = await RdbStore.queryAll(1)
      const target: Contact | undefined = list.find(
        (c: Contact): boolean => c.id === this.contactId
      )

      if (target !== undefined) {
        this.name = target.name
        this.phone = target.phone
        this.company = target.company ?? ''
        this.usageInfo = target.position ?? ''
        this.email = target.email ?? ''
      }
    }
  }

  /**
   * 保存修改内容
   */
  async save(): Promise<void> {
    const updated: Contact = new Contact(this.name, this.phone)
    updated.id = this.contactId
    updated.company = this.company
    updated.position = this.usageInfo
    updated.email = this.email
    updated.userId = 1

    await RdbStore.update(updated)

    try {
      promptAction.showToast({ message: '✓ 修改保存成功' })
    } catch (e) {
      // ArkTS 要求异常捕获，允许留空
    }

    router.back()
  }

  /**
   * 表单输入项构建器
   */
  @Builder
  InputItem(
    label: string,
    value: string,
    onChange: (val: string) => void
  ) {
    ListItem() {
      Column() {
        Text(label)
          .fontSize(14)
          .fontColor('#666666')
          .width('100%')
          .margin({ bottom: 8 })

        TextInput({ text: value })
          .height(45)
          .width('100%')
          .backgroundColor(Color.White)
          .borderRadius(8)
          .onChange(onChange)
      }
      .padding(10)
    }
  }

  build() {
    Navigation() {
      Column() {
        /**
         * 编辑表单
         */
        List() {
          this.InputItem('药品名称', this.name, (val: string) => {
            this.name = val
          })

          this.InputItem('有效期至', this.phone, (val: string) => {
            this.phone = val
          })

          this.InputItem('功能分类', this.company, (val: string) => {
            this.company = val
          })

          this.InputItem('用法用量', this.usageInfo, (val: string) => {
            this.usageInfo = val
          })

          this.InputItem('存放位置', this.email, (val: string) => {
            this.email = val
          })
        }
        .width('95%')
        .margin({ top: 20 })

        /**
         * 保存按钮
         */
        Button('保存修改')
          .width('90%')
          .height(50)
          .margin({ top: 40 })
          .backgroundColor('#4A90E2')
          .borderRadius(25)
          .onClick(() => {
            this.save()
          })
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F7F8FA')
    }
    .title('修改信息')
    .titleMode(NavigationTitleMode.Mini)
  }
}
