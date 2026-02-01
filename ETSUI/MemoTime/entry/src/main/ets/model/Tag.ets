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
 *
 */

/**
 * 标签数据模型
 */
import { relationalStore } from '@kit.ArkData'

/**
 * 标签初始化参数
 */
export interface TagInit {
  id?: string
  name?: string
  color?: string
  icon?: string
  scheduleCount?: number
}

/**
 * 标签数据模型
 */
export class Tag {
  id: string = ''
  name: string = ''
  color: string = '#4ECDC4'
  icon: string = ''
  scheduleCount: number = 0
  createdAt: number = 0

  constructor(init?: TagInit) {
    if (init) {
      if (init.id !== undefined) {
        this.id = init.id
      }
      if (init.name !== undefined) {
        this.name = init.name
      }
      if (init.color !== undefined) {
        this.color = init.color
      }
      if (init.icon !== undefined) {
        this.icon = init.icon
      }
      if (init.scheduleCount !== undefined) {
        this.scheduleCount = init.scheduleCount
      }
    }
    if (!this.id) {
      this.id = generateTagUUID()
    }
    if (!this.createdAt) {
      this.createdAt = Date.now()
    }
  }

  /**
   * 转换为数据库存储对象
   */
  toDbObject(): relationalStore.ValuesBucket {
    const bucket: relationalStore.ValuesBucket = {
      id: this.id,
      name: this.name,
      color: this.color,
      icon: this.icon,
      scheduleCount: this.scheduleCount,
      createdAt: this.createdAt
    }
    return bucket
  }

  /**
   * 从数据库对象创建 Tag 实例
   */
  static fromDbObject(obj: relationalStore.ValuesBucket): Tag {
    const tag = new Tag()
    tag.id = obj['id'] as string
    tag.name = obj['name'] as string
    tag.color = obj['color'] as string
    tag.icon = obj['icon'] as string
    tag.scheduleCount = obj['scheduleCount'] as number
    tag.createdAt = obj['createdAt'] as number
    return tag
  }
}

/**
 * 生成 UUID
 */
function generateTagUUID(): string {
  const timestamp = Date.now().toString(36)
  const randomPart = Math.random().toString(36).substring(2, 15)
  return `tag-${timestamp}-${randomPart}`
}

/**
 * 创建默认标签
 */
function createDefaultTag(id: string, name: string, color: string): Tag {
  const tag = new Tag()
  tag.id = id
  tag.name = name
  tag.color = color
  return tag
}

/**
 * 默认标签列表
 */
export const DEFAULT_TAGS: Tag[] = [
  createDefaultTag('tag-work', '工作', '#FF6B6B'),
  createDefaultTag('tag-meeting', '会议', '#4ECDC4'),
  createDefaultTag('tag-study', '学习', '#45B7D1'),
  createDefaultTag('tag-sport', '运动', '#96CEB4'),
  createDefaultTag('tag-birthday', '生日', '#FFEAA7'),
  createDefaultTag('tag-life', '生活', '#DDA0DD'),
  createDefaultTag('tag-travel', '旅行', '#FF8C42')
]
