import { SportData } from '../model/SportModel';
import { preferences } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';

export class HistoryService {
  private preferences: preferences.Preferences | null = null;
  private readonly KEY_SPORT_RECORDS = 'sport_records';

  async init(context: common.Context): Promise<void> {
    try {
      this.preferences = await preferences.getPreferences(context, 'sport_history');
      console.log('历史记录服务初始化成功');
    } catch (err) {
      console.error('初始化历史记录服务失败:', err);
    }
  }

  async saveSportRecord(data: SportData): Promise<void> {
    if (!this.preferences) {
      console.error('历史记录服务未初始化');
      return;
    }

    try {
      const records = await this.getSportRecords();
      records.push(data);
      await this.preferences.put(this.KEY_SPORT_RECORDS, JSON.stringify(records));
      await this.preferences.flush();
      console.log('运动记录已保存:', data);
    } catch (err) {
      console.error('保存运动记录失败:', err);
    }
  }

  async getSportRecords(): Promise<SportData[]> {
    if (!this.preferences) {
      console.error('历史记录服务未初始化');
      return [];
    }

    try {
      const recordsJson = await this.preferences.get(this.KEY_SPORT_RECORDS, '[]');
      const records: SportData[] = JSON.parse(recordsJson as string);
      return records;
    } catch (err) {
      console.error('获取运动记录失败:', err);
      return [];
    }
  }

  async clearSportRecords(): Promise<void> {
    if (!this.preferences) {
      console.error('历史记录服务未初始化');
      return;
    }

    try {
      await this.preferences.delete(this.KEY_SPORT_RECORDS);
      await this.preferences.flush();
      console.log('运动记录已清除');
    } catch (err) {
      console.error('清除运动记录失败:', err);
    }
  }
}
