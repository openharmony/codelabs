// entry/src/main/ets/services/BackupService.ets
import { fileIo as fs } from '@kit.CoreFileKit';
import { Context } from '@kit.AbilityKit';
import { Contact, ContactModel } from '../model/ContactModel';
import { Event, EventModel } from '../model/EventModel';
import { Logger } from '../common/utils/Logger';

const logger = new Logger('BackupService');
const BACKUP_FILE_NAME = 'cloud_backup_sim.json';

interface BackupData {
  timestamp: number;
  contacts: Array<Contact>;
  events: Array<Event>;
}

export class BackupService {
  private static instance: BackupService;

  private constructor() {}

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
      // ✅ 修复：包装成 Error 对象抛出
      throw new Error(`Backup failed: ${JSON.stringify(err)}`);
    }
  }

  public async restore(context: Context): Promise<number> {
    logger.info('Starting restore...');

    try {
      const filesDir = context.filesDir;
      const filePath = `${filesDir}/${BACKUP_FILE_NAME}`;

      if (!fs.accessSync(filePath)) {
        throw new Error('本地没有找到备份文件 (请先执行备份)');
      }

      const content = fs.readTextSync(filePath);
      if (!content) {
        throw new Error('备份文件为空');
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
      return successCount;

    } catch (err) {
      logger.error(`Restore failed: ${JSON.stringify(err)}`);
      // ✅ 修复：ArkTS 禁止 throw unknown，必须包装成 Error 抛出
      throw new Error(`Restore failed: ${JSON.stringify(err)}`);
    }
  }
}