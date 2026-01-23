/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// entry/src/main/ets/services/BackupService.ets
import { fileIo as fs } from '@kit.CoreFileKit';
import { Context } from '@kit.AbilityKit';
import { Contact, ContactModel } from '../model/ContactModel';
import { Event, EventModel } from '../model/EventModel';
import { Logger } from '../common/utils/Logger';
import { emitter } from '@kit.BasicServicesKit';
import { CommonConstants } from '../common/constants/CommonConstants';

const logger = new Logger('BackupService');
const BACKUP_FILE_NAME = 'cloud_backup_sim.json';

interface BackupData {
  timestamp: number;
  contacts: Array<Contact>;
  events: Array<Event>;
}

export class BackupService {
  private static instance: BackupService;

  private constructor() {
  }

  public static getInstance(): BackupService {
    if (!BackupService.instance) {
      BackupService.instance = new BackupService();
    }
    return BackupService.instance;
  }

  public async backup(context: Context): Promise<string> {
    logger.info('Starting backup...');

    try {
      const contacts = await ContactModel.queryAll();
      const events = await EventModel.queryUpcoming();

      const data: BackupData = {
        timestamp: Date.now(),
        contacts: contacts,
        events: events
      };

      const jsonString = JSON.stringify(data, null, 2);

      const filesDir = context.filesDir;
      const filePath = `${filesDir}/${BACKUP_FILE_NAME}`;

      const file = fs.openSync(filePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE | fs.OpenMode.TRUNC);
      fs.writeSync(file.fd, jsonString);
      fs.closeSync(file);

      logger.info(`Backup success! File saved at: ${filePath}`);
      return filePath;

    } catch (err) {
      logger.error(`Backup failed: ${JSON.stringify(err)}`);
      //  修复：包装成 Error 对象抛出
      throw new Error(`Backup failed: ${JSON.stringify(err)}`);
    }
  }

  // entry/src/main/ets/services/BackupService.ets

  public async restore(context: Context): Promise<number> {
    logger.info('Starting restore...');

    try {
      const filesDir = context.filesDir;
      const filePath = `${filesDir}/${BACKUP_FILE_NAME}`;

      // 检查文件是否存在
      if (!fs.accessSync(filePath)) {
        const msg = context.resourceManager.getStringSync($r('app.string.mine_restore_file_missing').id);
        throw new Error(msg);
      }

      const content = fs.readTextSync(filePath);
      if (!content) {
        const msg = context.resourceManager.getStringSync($r('app.string.mine_restore_file_empty').id);
        throw new Error(msg);
      }

      const data: BackupData = JSON.parse(content) as BackupData;
      logger.info(`Backup file parsed. Timestamp: ${data.timestamp}`);

      let successCount = 0;

      if (data.contacts && data.contacts.length > 0) {
        for (const c of data.contacts) {
          const newContact = new Contact(c.name, c.phone, c.relation, c.email);
          await ContactModel.insert(newContact);
          successCount++;
        }
      }

      if (data.events && data.events.length > 0) {
        for (const e of data.events) {
          const newEvent = new Event(e.title, e.startTime, e.endTime, e.description, e.isAllDay, e.reminderId);
          await EventModel.insert(newEvent);
          successCount++;
        }
      }

      logger.info(`Restore finished. Restored ${successCount} items.`);
      emitter.emit({ eventId: CommonConstants.EVENT_ID_REFRESH });
      return successCount;

    } catch (err) {

      let errorMsg = 'Unknown error';
      // 显式提取 message 属性
      if (err instanceof Error) {
        errorMsg = err.message;
      } else if (typeof err === 'object' && err !== null) {
        errorMsg = JSON.stringify(err);
      } else {
        errorMsg = String(err);
      }

      logger.error(`Restore failed details: ${errorMsg}`);

      // 抛出具体的错误信息，而不是 stringify 后的对象
      // 注意：不要加 "Restore failed: " 前缀了，直接抛出原始原因，UI 层会处理
      throw new Error(errorMsg);
    }
  }
}