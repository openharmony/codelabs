/**
 * 日程数据模型
 */
import { relationalStore } from '@kit.ArkData'

/**
 * 日程初始化参数
 */
export interface ScheduleInit {
  id?: string
  title?: string
  description?: string
  startTime?: number
  endTime?: number
  isAllDay?: boolean
  location?: string
  tagIds?: string[]
  color?: string
  repeatRule?: string
  reminderIds?: string[]
  recordingIds?: string[]
  externalAppLink?: string
}

/**
 * 日程数据模型
 */
export class Schedule {
  id: string = ''
  title: string = ''
  description: string = ''
  startTime: number = 0
  endTime: number = 0
  isAllDay: boolean = false
  location: string = ''
  tagIds: string[] = []
  color: string = '#4ECDC4'
  repeatRule: string = ''
  reminderIds: string[] = []
  recordingIds: string[] = []
  externalAppLink: string = ''
  createdAt: number = 0
  updatedAt: number = 0
  isDeleted: boolean = false

  constructor(init?: ScheduleInit) {
    if (init) {
      if (init.id !== undefined) this.id = init.id
      if (init.title !== undefined) this.title = init.title
      if (init.description !== undefined) this.description = init.description
      if (init.startTime !== undefined) this.startTime = init.startTime
      if (init.endTime !== undefined) this.endTime = init.endTime
      if (init.isAllDay !== undefined) this.isAllDay = init.isAllDay
      if (init.location !== undefined) this.location = init.location
      if (init.tagIds !== undefined) this.tagIds = init.tagIds
      if (init.color !== undefined) this.color = init.color
      if (init.repeatRule !== undefined) this.repeatRule = init.repeatRule
      if (init.reminderIds !== undefined) this.reminderIds = init.reminderIds
      if (init.recordingIds !== undefined) this.recordingIds = init.recordingIds
      if (init.externalAppLink !== undefined) this.externalAppLink = init.externalAppLink
    }
    if (!this.id) {
      this.id = generateScheduleUUID()
    }
    if (!this.createdAt) {
      this.createdAt = Date.now()
    }
    if (!this.updatedAt) {
      this.updatedAt = Date.now()
    }
  }

  /**
   * 获取日程时长（分钟）
   */
  getDuration(): number {
    return Math.floor((this.endTime - this.startTime) / 60000)
  }

  /**
   * 判断日程是否在指定日期
   */
  isOnDate(date: Date): boolean {
    const scheduleStart = new Date(this.startTime)
    const scheduleEnd = new Date(this.endTime)
    const targetDate = new Date(date.getFullYear(), date.getMonth(), date.getDate())
    const nextDate = new Date(targetDate.getTime() + 24 * 60 * 60 * 1000)

    return scheduleStart < nextDate && scheduleEnd > targetDate
  }

  /**
   * 转换为数据库存储对象
   */
  toDbObject(): relationalStore.ValuesBucket {
    const bucket: relationalStore.ValuesBucket = {
      id: this.id,
      title: this.title,
      description: this.description,
      startTime: this.startTime,
      endTime: this.endTime,
      isAllDay: this.isAllDay ? 1 : 0,
      location: this.location,
      tagIds: JSON.stringify(this.tagIds),
      color: this.color,
      repeatRule: this.repeatRule,
      reminderIds: JSON.stringify(this.reminderIds),
      recordingIds: JSON.stringify(this.recordingIds),
      externalAppLink: this.externalAppLink,
      createdAt: this.createdAt,
      updatedAt: this.updatedAt,
      isDeleted: this.isDeleted ? 1 : 0
    }
    return bucket
  }

  /**
   * 从数据库对象创建 Schedule 实例
   */
  static fromDbObject(obj: relationalStore.ValuesBucket): Schedule {
    const schedule = new Schedule()
    schedule.id = obj['id'] as string
    schedule.title = obj['title'] as string
    schedule.description = (obj['description'] as string) || ''
    schedule.startTime = obj['startTime'] as number
    schedule.endTime = obj['endTime'] as number
    schedule.isAllDay = (obj['isAllDay'] as number) === 1
    schedule.location = (obj['location'] as string) || ''
    
    const tagIdsStr = obj['tagIds'] as string
    if (tagIdsStr) {
      try {
        schedule.tagIds = JSON.parse(tagIdsStr) as string[]
      } catch (e) {
        schedule.tagIds = []
      }
    }
    
    schedule.color = (obj['color'] as string) || '#4ECDC4'
    schedule.repeatRule = (obj['repeatRule'] as string) || ''
    
    const reminderIdsStr = obj['reminderIds'] as string
    if (reminderIdsStr) {
      try {
        schedule.reminderIds = JSON.parse(reminderIdsStr) as string[]
      } catch (e) {
        schedule.reminderIds = []
      }
    }
    
    const recordingIdsStr = obj['recordingIds'] as string
    if (recordingIdsStr) {
      try {
        schedule.recordingIds = JSON.parse(recordingIdsStr) as string[]
      } catch (e) {
        schedule.recordingIds = []
      }
    }
    
    schedule.externalAppLink = (obj['externalAppLink'] as string) || ''
    schedule.createdAt = obj['createdAt'] as number
    schedule.updatedAt = obj['updatedAt'] as number
    schedule.isDeleted = (obj['isDeleted'] as number) === 1
    
    return schedule
  }
}

/**
 * 生成 UUID
 */
function generateScheduleUUID(): string {
  const timestamp = Date.now().toString(36)
  const randomPart = Math.random().toString(36).substring(2, 15)
  return `schedule-${timestamp}-${randomPart}`
}
