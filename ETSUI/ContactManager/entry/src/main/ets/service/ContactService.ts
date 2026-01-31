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
import contact from '@ohos.contact'
import common from '@ohos.app.ability.common'
import { BusinessError } from '@ohos.base'
import { Contact as LocalContact } from '../model/Contact'

export class ContactService {
  private static getAbilityContext(): common.UIAbilityContext {
    // @ts-ignore
    return getContext() as common.UIAbilityContext
  }

  /**
   * 【增】添加联系人
   */
  static async addContact(c: LocalContact): Promise<string> {
    const context = ContactService.getAbilityContext()

    const systemData: contact.Contact = {
      name: {
        fullName: c.name || ''
      },
      phoneNumbers: [{
        phoneNumber: c.phone || ''
      }]
    }

    // 可选字段
    if (c.email && c.email.trim()) {
      systemData.emails = [{
        email: c.email.trim()
      }]
    }

    if (c.remark && c.remark.trim()) {
      systemData.note = {
        noteContent: c.remark.trim()
      }
    }

    try {
      await contact.addContact(context, systemData)
      console.info('[ContactService] 添加联系人成功')

      // 添加成功后，可以重新查询获取 key
      const allContacts = await contact.queryContacts(context)
      // 假设通过姓名和电话找到刚添加的联系人
      const newContact = allContacts.find((item: contact.Contact): boolean =>
      item.name?.fullName === c.name &&
        item.phoneNumbers?.[0]?.phoneNumber === c.phone
      )

      return newContact?.key || ''

    } catch (err) {
      const error = err as BusinessError
      console.error(`[ContactService] 添加联系人失败:`, error)
      throw error
    }
  }

  /**
   * 【查】查询联系人
   */
  static async getAllContacts(): Promise<LocalContact[]> {
    const context = ContactService.getAbilityContext()
    const systemContacts = await contact.queryContacts(context)

    if (!systemContacts || systemContacts.length === 0) {
      return []
    }

    return systemContacts.map((item: contact.Contact): LocalContact => {
      console.info('[ContactService] 原始数据:', {
        系统key: item.key,
        key类型: typeof item.key, // 应该是string
        系统id: item.id,
        id类型: typeof item.id     // 应该是number
      })

      const c = new LocalContact(
        item.name?.fullName || '未知',
        item.phoneNumbers?.[0]?.phoneNumber || ''
      )

      // 关键修改：正确映射系统字段
      c.key = item.key || ''      // key是string，系统返回的应该是string
      c.id = item.id ? item.id.toString() : '' // 将number类型的id转为string用于UI
      // 补充：邮箱
      c.email = item.emails?.[0]?.email || ''
      // 补充：备注
      c.remark = item.note?.noteContent || ''
      // 验证类型
      console.info('[ContactService] 处理后的联系人:', {
        key: c.key,
        key类型: typeof c.key,
        id: c.id,
        id类型: typeof c.id
      })
      return c
    })
  }

  /**
   * 【改】更新联系人
   */
  static async updateContact(c: LocalContact): Promise<void> {

    const abilityContext = ContactService.getAbilityContext()

    // 关键：string → number
    const contactIdNum = Number(c.id)
    if (!contactIdNum || Number.isNaN(contactIdNum)) {
      console.error('[updateContact] 非法的联系人 id:', c.id)
      const err: BusinessError = {
        code: 401,
        message: '联系人 id 无效',
        name: 'BusinessError'
      }
      throw err
    }
    // 3. 构建系统期望的数据结构
    // 特别注意：只需要包含要更新的字段和key
    const systemData: contact.Contact = {
      id: contactIdNum,
      key: c.key,  // string类型

      // 更新姓名（必须字段）
      name: {
        fullName: c.name || ''
      },

      // 更新电话（必须字段）
      phoneNumbers: [{
        phoneNumber: c.phone || ''
      }],

      // 可选字段
      emails: c.email && c.email.trim() ? [{
        email: c.email.trim()
      }] : undefined,

      note: c.remark && c.remark.trim() ? {
        noteContent: c.remark.trim()
      } : undefined
    }

    // 4. 验证数据
    console.info('[ContactService] 最终传给系统的数据:', {
      key: systemData.key,
      数据类型: typeof systemData.key,
      完整结构: JSON.stringify(systemData, null, 2)
    })

    try {
      // 5. 调用API
      await contact.updateContact(abilityContext, systemData)
      console.info('[Service] 更新成功')

    } catch (err) {
      const busiErr = err as BusinessError
      console.error(`[Service] 更新失败! 错误码: ${busiErr.code}, 原因: ${busiErr.message}`)

      // 如果是401错误，执行详细诊断
      if (busiErr.code === 401) {
        await this.diagnose401Error(c.key, abilityContext)
      }

      throw busiErr
    }
  }

  /**
   * 诊断401错误，针对前期后端系统问题出错使用的判断方法
   */
  static async diagnose401Error(key: string, context: common.Context): Promise<void> {
    console.error('=== 401错误详细诊断 ===')
    console.error('1. 尝试查找key为:', key)
    try {
      // 获取所有联系人
      const allContacts = await contact.queryContacts(context)
      console.error('2. 系统中共有联系人:', allContacts.length)
      // 查找匹配的联系人
      const foundContacts = allContacts.filter((contact: contact.Contact): boolean => contact.key === key)
      console.error('3. 找到匹配的联系人数量:', foundContacts.length)
      if (foundContacts.length > 0) {
        console.error('4. 找到的联系人详情:', foundContacts.map((c: contact.Contact): Record<string, unknown> => ({
          key: c.key,
          id: c.id,
          name: c.name?.fullName
        })))
        // 检查是否有重复key
        const duplicateKeys = allContacts.filter((c: contact.Contact): boolean => c.key === key)
        if (duplicateKeys.length > 1) {
          console.error('5. 警告：发现重复的key！')
        }
      } else {
        console.error('4. 未找到匹配的联系人')
        console.error('5. 系统所有key列表:', allContacts.map((c: contact.Contact): Record<string, unknown> => ({
          key: c.key,
          key类型: typeof c.key,
          name: c.name?.fullName
        })))
      }
    } catch (err) {
      console.error('诊断过程中出错:', err)
    }
  }

  /**
   * 验证联系人是否存在
   */
  static async isContactExist(key: string): Promise<boolean> {
    try {
      const context = ContactService.getAbilityContext()
      const contacts = await contact.queryContacts(context)
      // 精确匹配key（string类型）
      const found = contacts.find((item: contact.Contact): boolean => item.key === key)
      console.info('[isContactExist] 验证结果:', {
        查询的key: key,
        系统联系人数量: contacts.length,
        是否找到: !!found,
        如果找到的key: found?.key,
        如果找到的id: found?.id
      })
      return !!found
    } catch (err) {
      console.error('[isContactExist] 验证失败:', err)
      return false
    }
  }

  /**
   * 【删】删除联系人
   */
  static async deleteContact(key: string): Promise<void> {
    if (!key || key.trim() === '') {
      console.error('[ContactService] 删除失败: key为空')
      return
    }

    const context = ContactService.getAbilityContext()

    try {
      await contact.deleteContact(context, key)
      console.info(`[ContactService] 删除联系人成功, key: ${key}`)
    } catch (err) {
      const error = err as BusinessError
      console.error(`[ContactService] 删除联系人失败:`, error)
      throw error
    }
  }

  /**
   * 测试联系人系统API
   */
  static async testContactSystem(): Promise<void> {
    console.info('=== 开始测试联系人系统 ===')
    try {
      const context = this.getAbilityContext()
      const contacts = await contact.queryContacts(context)

      console.info('[testContactSystem] 系统联系人数量:', contacts.length)

      if (contacts.length === 0) {
        console.warn('[testContactSystem] 系统中没有联系人，无法测试')
        return
      }
      // 显示第一个联系人的信息
      const firstContact = contacts[0]
      console.info('[testContactSystem] 第一个联系人详情:', {
        key: firstContact.key,
        key类型: typeof firstContact.key,
        id: firstContact.id,
        id类型: typeof firstContact.id,
        姓名: firstContact.name?.fullName,
        电话: firstContact.phoneNumbers?.[0]?.phoneNumber
      })
      // 测试更新功能
      const testData: contact.Contact = {
        key: firstContact.key,
        name: {
          fullName: '测试更新_' + Date.now()  // 使用时间戳确保每次不同
        },
        phoneNumbers: [{
          phoneNumber: '13800138000'  // 测试电话号码
        }]
      }
      console.info('[testContactSystem] 测试更新数据:', JSON.stringify(testData, null, 2))
      // 执行更新
      await contact.updateContact(context, testData)
      console.info('[testContactSystem] ✅ 测试更新成功')

      // 再次查询验证更新结果
      const updatedContacts = await contact.queryContacts(context)
      const updatedContact = updatedContacts.find((c: contact.Contact): boolean => c.key === firstContact.key)

      console.info('[testContactSystem] 更新后验证:', {
        更新前姓名: firstContact.name?.fullName,
        更新后姓名: updatedContact?.name?.fullName,
        是否一致: firstContact.name?.fullName !== updatedContact?.name?.fullName
      })

    } catch (err) {
      const error = err as BusinessError
      console.error('[testContactSystem] ❌ 测试失败:', {
        错误码: error.code,
        错误信息: error.message,
        完整错误: error
      })
    }
    console.info('=== 联系人系统测试结束 ===')
  }
}
