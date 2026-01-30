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
 * 录音数据模型
 */
import { relationalStore } from '@kit.ArkData'

/**
 * 录音初始化参数
 */
export interface RecordingInit {
  id?: string
  scheduleId?: string
  filePath?: string
  fileName?: string
  duration?: number
  fileSize?: number
  format?: string
  transcription?: string
}

/**
 * 录音数据模型
 */
export class Recording {
  id: string = ''
  scheduleId: string = ''     // 关联的日程ID
  filePath: string = ''       // 文件路径
  fileName: string = ''       // 文件名
  duration: number = 0        // 时长（秒）
  fileSize: number = 0        // 文件大小（字节）
  format: string = 'aac'      // 格式 (mp3/aac)
  transcription: string = ''  // 语音转文本结果
  createdAt: number = 0

  constructor(init?: RecordingInit) {
    if (init) {
      if (init.id !== undefined) this.id = init.id
      if (init.scheduleId !== undefined) this.scheduleId = init.scheduleId
      if (init.filePath !== undefined) this.filePath = init.filePath
      if (init.fileName !== undefined) this.fileName = init.fileName
      if (init.duration !== undefined) this.duration = init.duration
      if (init.fileSize !== undefined) this.fileSize = init.fileSize
      if (init.format !== undefined) this.format = init.format
      if (init.transcription !== undefined) this.transcription = init.transcription
    }
    if (!this.id) {
      this.id = generateRecordingUUID()
    }
    if (!this.createdAt) {
      this.createdAt = Date.now()
    }
  }

  /**
   * 获取时长的格式化显示（mm:ss）
   */
  getDurationText(): string {
    const minutes = Math.floor(this.duration / 60)
    const seconds = this.duration % 60
    return `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`
  }

  /**
   * 获取文件大小的格式化显示
   */
  getFileSizeText(): string {
    if (this.fileSize < 1024) {
      return `${this.fileSize} B`
    } else if (this.fileSize < 1024 * 1024) {
      return `${(this.fileSize / 1024).toFixed(1)} KB`
    } else {
      return `${(this.fileSize / 1024 / 1024).toFixed(1)} MB`
    }
  }

  /**
   * 转换为数据库存储对象
   */
  toDbObject(): relationalStore.ValuesBucket {
    const bucket: relationalStore.ValuesBucket = {
      id: this.id,
      scheduleId: this.scheduleId,
      filePath: this.filePath,
      fileName: this.fileName,
      duration: this.duration,
      fileSize: this.fileSize,
      format: this.format,
      transcription: this.transcription,
      createdAt: this.createdAt
    }
    return bucket
  }

  /**
   * 从数据库对象创建 Recording 实例
   */
  static fromDbObject(obj: relationalStore.ValuesBucket): Recording {
    const recording = new Recording()
    recording.id = obj['id'] as string
    recording.scheduleId = obj['scheduleId'] as string
    recording.filePath = obj['filePath'] as string
    recording.fileName = obj['fileName'] as string
    recording.duration = obj['duration'] as number
    recording.fileSize = obj['fileSize'] as number
    recording.format = obj['format'] as string
    recording.transcription = obj['transcription'] as string
    recording.createdAt = obj['createdAt'] as number
    return recording
  }
}

/**
 * 生成 UUID
 */
function generateRecordingUUID(): string {
  const timestamp = Date.now().toString(36)
  const randomPart = Math.random().toString(36).substring(2, 15)
  return `recording-${timestamp}-${randomPart}`
}
