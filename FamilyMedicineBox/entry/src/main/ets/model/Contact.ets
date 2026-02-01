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

/**
 * 药品 / 联系人 数据模型
 * ⚠ ArkTS 要求：禁止 any / unknown，禁止 ! 断言
 */
export class Contact {
  /** 主键 ID（数据库自增） */
  id: number | undefined = undefined
  /** 药品名称 */
  name: string = ''
  /** 有效期 / 电话（当前项目复用字段） */
  phone: string = ''
  /** 功效 / 公司 */
  company: string | undefined = undefined
  /** 用法用量 / 职位 */
  position: string | undefined = undefined
  /** 存放位置 / 邮箱 */
  email: string | undefined = undefined
  /** 图片路径 */
  image: string | undefined = undefined
  /** 用户 ID */
  userId: number | undefined = undefined

  /**
   * 构造函数
   * @param name 药品名称
   * @param phone 有效期 / 电话
   */
  constructor(name: string, phone: string) {
    this.name = name
    this.phone = phone
  }

  /**
   * 是否是合法对象（防御式校验）
   */
  isValid(): boolean {
    return this.name.length > 0 && this.phone.length > 0
  }

  /**
   * 用于调试输出
   */
  toString(): string {
    return `Contact{id=${this.id}, name=${this.name}, phone=${this.phone}}`
  }
}
