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
 * 数据持久化服务
 */
import { relationalStore } from '@kit.ArkData'
import { Context } from '@kit.AbilityKit'
import { AppConstants } from '../common/constants/Constants'
import { Logger } from '../common/utils/Logger'
import { Schedule } from '../model/Schedule'
import { Reminder } from '../model/Reminder'
import { Recording } from '../model/Recording'
import { Tag, DEFAULT_TAGS } from '../model/Tag'

const TAG = 'DataService'

/**
 * 数据服务类 - 负责本地数据库操作
 */
export class DataService {
  private static instance: DataService | null = null
  private rdbStore: relationalStore.RdbStore | null = null
  private context: Context | null = null

  private constructor() {}

  static getInstance(): DataService {
    if (!DataService.instance) {
      DataService.instance = new DataService()
    }
    return DataService.instance
  }

  /**
   * 初始化数据库
   */
  async initialize(context: Context): Promise<void> {
    this.context = context

    const storeConfig: relationalStore.StoreConfig = {
      name: `${AppConstants.DB_NAME}.db`,
      securityLevel: relationalStore.SecurityLevel.S1
    }

    try {
      this.rdbStore = await relationalStore.getRdbStore(context, storeConfig)
      await this.createTables()
      await this.initializeDefaultTags()
      Logger.info(TAG, 'Database initialized successfully')
    } catch (error) {
      Logger.error(TAG, `Failed to initialize database: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 创建数据表
   */
  private async createTables(): Promise<void> {
    if (!this.rdbStore) return

    // 日程表
    const createScheduleTable = `
      CREATE TABLE IF NOT EXISTS ${AppConstants.TABLE_SCHEDULE} (
        id TEXT PRIMARY KEY,
        title TEXT NOT NULL,
        description TEXT,
        startTime INTEGER NOT NULL,
        endTime INTEGER NOT NULL,
        isAllDay INTEGER DEFAULT 0,
        location TEXT,
        tagIds TEXT,
        color TEXT,
        repeatRule TEXT,
        reminderIds TEXT,
        recordingIds TEXT,
        externalAppLink TEXT,
        createdAt INTEGER,
        updatedAt INTEGER,
        isDeleted INTEGER DEFAULT 0
      )
    `

    // 提醒表
    const createReminderTable = `
      CREATE TABLE IF NOT EXISTS ${AppConstants.TABLE_REMINDER} (
        id TEXT PRIMARY KEY,
        scheduleId TEXT NOT NULL,
        timing INTEGER,
        type TEXT,
        triggerTime INTEGER,
        isTriggered INTEGER DEFAULT 0,
        customSound TEXT,
        vibrationEnabled INTEGER DEFAULT 1,
        createdAt INTEGER
      )
    `

    // 录音表
    const createRecordingTable = `
      CREATE TABLE IF NOT EXISTS ${AppConstants.TABLE_RECORDING} (
        id TEXT PRIMARY KEY,
        scheduleId TEXT,
        filePath TEXT NOT NULL,
        fileName TEXT,
        duration INTEGER,
        fileSize INTEGER,
        format TEXT,
        transcription TEXT,
        createdAt INTEGER
      )
    `

    // 标签表
    const createTagTable = `
      CREATE TABLE IF NOT EXISTS ${AppConstants.TABLE_TAG} (
        id TEXT PRIMARY KEY,
        name TEXT NOT NULL,
        color TEXT,
        icon TEXT,
        scheduleCount INTEGER DEFAULT 0,
        createdAt INTEGER
      )
    `

    try {
      await this.rdbStore.executeSql(createScheduleTable)
      await this.rdbStore.executeSql(createReminderTable)
      await this.rdbStore.executeSql(createRecordingTable)
      await this.rdbStore.executeSql(createTagTable)
      Logger.info(TAG, 'Tables created successfully')
    } catch (error) {
      Logger.error(TAG, `Failed to create tables: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 初始化默认标签
   */
  private async initializeDefaultTags(): Promise<void> {
    if (!this.rdbStore) return

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_TAG)
      const resultSet = await this.rdbStore.query(predicates, ['id'])
      const count = resultSet.rowCount
      resultSet.close()

      if (count === 0) {
        for (const tag of DEFAULT_TAGS) {
          await this.insertTag(tag)
        }
        Logger.info(TAG, 'Default tags initialized')
      }
    } catch (error) {
      Logger.error(TAG, `Failed to initialize default tags: ${JSON.stringify(error)}`)
    }
  }

  // ==================== 日程操作 ====================

  /**
   * 插入日程
   */
  async insertSchedule(schedule: Schedule): Promise<void> {
    if (!this.rdbStore) return

    try {
      const valuesBucket = schedule.toDbObject()
      await this.rdbStore.insert(AppConstants.TABLE_SCHEDULE, valuesBucket)
      Logger.info(TAG, `Schedule inserted: ${schedule.id}`)
    } catch (error) {
      Logger.error(TAG, `Failed to insert schedule: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 更新日程
   */
  async updateSchedule(schedule: Schedule): Promise<void> {
    if (!this.rdbStore) return

    schedule.updatedAt = Date.now()
    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_SCHEDULE)
      predicates.equalTo('id', schedule.id)
      const valuesBucket = schedule.toDbObject()
      await this.rdbStore.update(valuesBucket, predicates)
      Logger.info(TAG, `Schedule updated: ${schedule.id}`)
    } catch (error) {
      Logger.error(TAG, `Failed to update schedule: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 删除日程（软删除）
   */
  async deleteSchedule(id: string): Promise<void> {
    if (!this.rdbStore) return

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_SCHEDULE)
      predicates.equalTo('id', id)
      const valuesBucket: relationalStore.ValuesBucket = {
        isDeleted: 1,
        updatedAt: Date.now()
      }
      await this.rdbStore.update(valuesBucket, predicates)
      Logger.info(TAG, `Schedule deleted: ${id}`)
    } catch (error) {
      Logger.error(TAG, `Failed to delete schedule: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 获取所有日程
   */
  async getAllSchedules(): Promise<Schedule[]> {
    if (!this.rdbStore) return []

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_SCHEDULE)
      predicates.equalTo('isDeleted', 0)
      predicates.orderByAsc('startTime')
      const resultSet = await this.rdbStore.query(predicates)
      const schedules: Schedule[] = []

      while (resultSet.goToNextRow()) {
        const obj: relationalStore.ValuesBucket = {}
        for (const columnName of resultSet.columnNames) {
          const index = resultSet.getColumnIndex(columnName)
          const value = resultSet.getValue(index)
          if (value !== null) {
            obj[columnName] = value as relationalStore.ValueType
          }
        }
        schedules.push(Schedule.fromDbObject(obj))
      }
      resultSet.close()

      return schedules
    } catch (error) {
      Logger.error(TAG, `Failed to get all schedules: ${JSON.stringify(error)}`)
      return []
    }
  }

  /**
   * 根据日期范围获取日程
   */
  async getSchedulesByDateRange(startTime: number, endTime: number): Promise<Schedule[]> {
    if (!this.rdbStore) return []

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_SCHEDULE)
      predicates.equalTo('isDeleted', 0)
      predicates.lessThanOrEqualTo('startTime', endTime)
      predicates.greaterThanOrEqualTo('endTime', startTime)
      predicates.orderByAsc('startTime')
      const resultSet = await this.rdbStore.query(predicates)
      const schedules: Schedule[] = []

      while (resultSet.goToNextRow()) {
        const obj: relationalStore.ValuesBucket = {}
        for (const columnName of resultSet.columnNames) {
          const index = resultSet.getColumnIndex(columnName)
          const value = resultSet.getValue(index)
          if (value !== null) {
            obj[columnName] = value as relationalStore.ValueType
          }
        }
        schedules.push(Schedule.fromDbObject(obj))
      }
      resultSet.close()

      return schedules
    } catch (error) {
      Logger.error(TAG, `Failed to get schedules by date range: ${JSON.stringify(error)}`)
      return []
    }
  }

  /**
   * 根据ID获取日程
   */
  async getScheduleById(id: string): Promise<Schedule | null> {
    if (!this.rdbStore) return null

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_SCHEDULE)
      predicates.equalTo('id', id)
      const resultSet = await this.rdbStore.query(predicates)

      if (resultSet.goToNextRow()) {
        const obj: relationalStore.ValuesBucket = {}
        for (const columnName of resultSet.columnNames) {
          const index = resultSet.getColumnIndex(columnName)
          const value = resultSet.getValue(index)
          if (value !== null) {
            obj[columnName] = value as relationalStore.ValueType
          }
        }
        resultSet.close()
        return Schedule.fromDbObject(obj)
      }
      resultSet.close()
      return null
    } catch (error) {
      Logger.error(TAG, `Failed to get schedule by id: ${JSON.stringify(error)}`)
      return null
    }
  }

  // ==================== 标签操作 ====================

  /**
   * 插入标签
   */
  async insertTag(tag: Tag): Promise<void> {
    if (!this.rdbStore) return

    try {
      const valuesBucket = tag.toDbObject()
      await this.rdbStore.insert(AppConstants.TABLE_TAG, valuesBucket)
    } catch (error) {
      Logger.error(TAG, `Failed to insert tag: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 获取所有标签
   */
  async getAllTags(): Promise<Tag[]> {
    if (!this.rdbStore) return []

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_TAG)
      predicates.orderByAsc('createdAt')
      const resultSet = await this.rdbStore.query(predicates)
      const tags: Tag[] = []

      while (resultSet.goToNextRow()) {
        const obj: relationalStore.ValuesBucket = {}
        for (const columnName of resultSet.columnNames) {
          const index = resultSet.getColumnIndex(columnName)
          const value = resultSet.getValue(index)
          if (value !== null) {
            obj[columnName] = value as relationalStore.ValueType
          }
        }
        tags.push(Tag.fromDbObject(obj))
      }
      resultSet.close()

      return tags
    } catch (error) {
      Logger.error(TAG, `Failed to get all tags: ${JSON.stringify(error)}`)
      return []
    }
  }

  // ==================== 提醒操作 ====================

  /**
   * 插入提醒
   */
  async insertReminder(reminder: Reminder): Promise<void> {
    if (!this.rdbStore) return

    try {
      const valuesBucket = reminder.toDbObject()
      await this.rdbStore.insert(AppConstants.TABLE_REMINDER, valuesBucket)
    } catch (error) {
      Logger.error(TAG, `Failed to insert reminder: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 获取日程的所有提醒
   */
  async getRemindersByScheduleId(scheduleId: string): Promise<Reminder[]> {
    if (!this.rdbStore) return []

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_REMINDER)
      predicates.equalTo('scheduleId', scheduleId)
      const resultSet = await this.rdbStore.query(predicates)
      const reminders: Reminder[] = []

      while (resultSet.goToNextRow()) {
        const obj: relationalStore.ValuesBucket = {}
        for (const columnName of resultSet.columnNames) {
          const index = resultSet.getColumnIndex(columnName)
          const value = resultSet.getValue(index)
          if (value !== null) {
            obj[columnName] = value as relationalStore.ValueType
          }
        }
        reminders.push(Reminder.fromDbObject(obj))
      }
      resultSet.close()

      return reminders
    } catch (error) {
      Logger.error(TAG, `Failed to get reminders: ${JSON.stringify(error)}`)
      return []
    }
  }

  /**
   * 删除日程的所有提醒
   */
  async deleteRemindersByScheduleId(scheduleId: string): Promise<void> {
    if (!this.rdbStore) return

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_REMINDER)
      predicates.equalTo('scheduleId', scheduleId)
      await this.rdbStore.delete(predicates)
      Logger.info(TAG, `Reminders deleted for schedule: ${scheduleId}`)
    } catch (error) {
      Logger.error(TAG, `Failed to delete reminders: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 更新提醒触发状态
   */
  async updateReminderTriggered(reminderId: string): Promise<void> {
    if (!this.rdbStore) return

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_REMINDER)
      predicates.equalTo('id', reminderId)
      const valuesBucket: relationalStore.ValuesBucket = {
        isTriggered: 1
      }
      await this.rdbStore.update(valuesBucket, predicates)
    } catch (error) {
      Logger.error(TAG, `Failed to update reminder: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 获取所有未触发的提醒
   */
  async getPendingReminders(): Promise<Reminder[]> {
    if (!this.rdbStore) return []

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_REMINDER)
      predicates.equalTo('isTriggered', 0)
      predicates.orderByAsc('triggerTime')
      const resultSet = await this.rdbStore.query(predicates)
      const reminders: Reminder[] = []

      while (resultSet.goToNextRow()) {
        const obj: relationalStore.ValuesBucket = {}
        for (const columnName of resultSet.columnNames) {
          const index = resultSet.getColumnIndex(columnName)
          const value = resultSet.getValue(index)
          if (value !== null) {
            obj[columnName] = value as relationalStore.ValueType
          }
        }
        reminders.push(Reminder.fromDbObject(obj))
      }
      resultSet.close()

      return reminders
    } catch (error) {
      Logger.error(TAG, `Failed to get pending reminders: ${JSON.stringify(error)}`)
      return []
    }
  }

  // ==================== 录音操作 ====================

  /**
   * 插入录音
   */
  async insertRecording(recording: Recording): Promise<void> {
    if (!this.rdbStore) return

    try {
      const valuesBucket = recording.toDbObject()
      await this.rdbStore.insert(AppConstants.TABLE_RECORDING, valuesBucket)
    } catch (error) {
      Logger.error(TAG, `Failed to insert recording: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 获取日程的所有录音
   */
  async getRecordingsByScheduleId(scheduleId: string): Promise<Recording[]> {
    if (!this.rdbStore) return []

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_RECORDING)
      predicates.equalTo('scheduleId', scheduleId)
      const resultSet = await this.rdbStore.query(predicates)
      const recordings: Recording[] = []

      while (resultSet.goToNextRow()) {
        const obj: relationalStore.ValuesBucket = {}
        for (const columnName of resultSet.columnNames) {
          const index = resultSet.getColumnIndex(columnName)
          const value = resultSet.getValue(index)
          if (value !== null) {
            obj[columnName] = value as relationalStore.ValueType
          }
        }
        recordings.push(Recording.fromDbObject(obj))
      }
      resultSet.close()

      return recordings
    } catch (error) {
      Logger.error(TAG, `Failed to get recordings: ${JSON.stringify(error)}`)
      return []
    }
  }

  /**
   * 删除录音
   */
  async deleteRecording(id: string): Promise<void> {
    if (!this.rdbStore) return

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_RECORDING)
      predicates.equalTo('id', id)
      await this.rdbStore.delete(predicates)
      Logger.info(TAG, `Recording deleted: ${id}`)
    } catch (error) {
      Logger.error(TAG, `Failed to delete recording: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 更新录音（如更新转录文本）
   */
  async updateRecording(recording: Recording): Promise<void> {
    if (!this.rdbStore) return

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_RECORDING)
      predicates.equalTo('id', recording.id)
      const valuesBucket = recording.toDbObject()
      await this.rdbStore.update(valuesBucket, predicates)
      Logger.info(TAG, `Recording updated: ${recording.id}`)
    } catch (error) {
      Logger.error(TAG, `Failed to update recording: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 删除日程的所有录音
   */
  async deleteRecordingsByScheduleId(scheduleId: string): Promise<void> {
    if (!this.rdbStore) return

    try {
      const predicates = new relationalStore.RdbPredicates(AppConstants.TABLE_RECORDING)
      predicates.equalTo('scheduleId', scheduleId)
      await this.rdbStore.delete(predicates)
      Logger.info(TAG, `Recordings deleted for schedule: ${scheduleId}`)
    } catch (error) {
      Logger.error(TAG, `Failed to delete recordings: ${JSON.stringify(error)}`)
    }
  }
}

export const dataService = DataService.getInstance()
