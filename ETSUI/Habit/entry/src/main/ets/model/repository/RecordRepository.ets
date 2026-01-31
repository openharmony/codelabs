import RdbHelper from '../database/RdbHelper';
import { CommonConstants } from '../../common/constants/CommonConstants';
import relationalStore from '@ohos.data.relationalStore';
import { DateUtil } from '../../common/utils/DateUtil';

/**
 * 统计数据返回类
 */
export class StreakStats {
  currentStreak: number = 0;
  maxStreak: number = 0;
  totalCount: number = 0;
}

/**
 * 记录详情接口
 */
export interface RecordDetail {
  id: number;
  mood: number;
  note: string;
  createTime: number;
}

class RecordRepository {

  // =================================================================
  // 1. 基础增删改查
  // =================================================================

  // 新增记录 (支持心情和心得)
  createRecord(habitId: number, mood: number = 1, note: string = ''): Promise<number> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB not initialized');

    const valueBucket: relationalStore.ValuesBucket = {
      habitId: habitId,
      createTime: new Date().getTime(),
      mood: mood,
      note: note
    };

    return store.insert(CommonConstants.RECORD_TABLE_NAME, valueBucket);
  }

  // 🟢 [修复] 插入模拟数据 (补全字段，防止数据库报错)
  insertMockData(habitId: number, timestamp: number): Promise<number> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB not initialized');

    const valueBucket: relationalStore.ValuesBucket = {
      habitId: habitId,
      createTime: timestamp,
      mood: 3, // 默认心情不错
      note: '模拟打卡数据' // 默认备注
    };
    return store.insert(CommonConstants.RECORD_TABLE_NAME, valueBucket);
  }

  // 删除指定习惯的所有记录
  deleteByHabitId(habitId: number): Promise<number> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB Error');
    let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
    predicates.equalTo('habitId', habitId);
    return store.delete(predicates);
  }

  // 清空所有数据
  cleanAll(): Promise<void> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB Error');
    let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
    return store.delete(predicates).then(() => { return; });
  }

  // =================================================================
  // 2. 首页 (Index) 专用
  // =================================================================

  async isCheckedInToday(habitId: number): Promise<boolean> {
    const store = RdbHelper.getRdbStore();
    if (!store) return false;
    const now = new Date();
    now.setHours(0, 0, 0, 0);
    const todayStart = now.getTime();
    let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
    predicates.equalTo('habitId', habitId);
    predicates.greaterThanOrEqualTo('createTime', todayStart);
    const resultSet = await store.query(predicates);
    const count = resultSet.rowCount;
    resultSet.close();
    return count > 0;
  }

  async getTodayCompletedNums(): Promise<Map<number, number>> {
    const store = RdbHelper.getRdbStore();
    if (!store) return new Map();
    const now = new Date();
    now.setHours(0, 0, 0, 0);
    const todayStart = now.getTime();
    let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
    predicates.greaterThanOrEqualTo('createTime', todayStart);
    const resultSet = await store.query(predicates);
    let countMap = new Map<number, number>();
    while (resultSet.goToNextRow()) {
      let habitId = resultSet.getLong(resultSet.getColumnIndex('habitId'));
      if (countMap.has(habitId)) {
        countMap.set(habitId, countMap.get(habitId)! + 1);
      } else {
        countMap.set(habitId, 1);
      }
    }
    resultSet.close();
    return countMap;
  }

  async deleteLatestRecord(habitId: number): Promise<void> {
    const store = RdbHelper.getRdbStore();
    if (!store) return;
    const now = new Date();
    now.setHours(0, 0, 0, 0);
    const todayStart = now.getTime();
    let sql = `SELECT id FROM record WHERE habitId = ? AND createTime >= ? ORDER BY createTime DESC LIMIT 1`;
    let resultSet = await store.querySql(sql, [habitId, todayStart]);
    if (resultSet.goToNextRow()) {
      let recordId = resultSet.getLong(resultSet.getColumnIndex('id'));
      let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
      predicates.equalTo('id', recordId);
      await store.delete(predicates);
    }
    resultSet.close();
  }

  async getTodayCompletedIds(): Promise<number[]> {
    const store = RdbHelper.getRdbStore();
    if (!store) return [];
    const now = new Date();
    now.setHours(0, 0, 0, 0);
    const todayStart = now.getTime();
    let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
    predicates.greaterThanOrEqualTo('createTime', todayStart);
    const resultSet = await store.query(predicates);
    let ids: number[] = [];
    while (resultSet.goToNextRow()) {
      ids.push(resultSet.getLong(resultSet.getColumnIndex('habitId')));
    }
    resultSet.close();
    return ids;
  }

  // =================================================================
  // 📊 统计页 (StatsPage) 专用 (🟢 重点修复区)
  // =================================================================

  // 🟢 [修复] 获取所有记录日期，增加日志，防止静默失败
  async getAllRecordDates(): Promise<string[]> {
    const store = RdbHelper.getRdbStore();
    if (!store) {
      console.error('[RecordRepo] DB store is null');
      return [];
    }

    try {
      let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
      // 🟢 关键：按时间排序，确保连续性计算正确
      predicates.orderByAsc('createTime');

      const resultSet = await store.query(predicates);
      // 🟢 打印日志：看看到底查到多少条
      console.info(`[RecordRepo] getAllRecordDates 查到记录数: ${resultSet.rowCount}`);

      let dates: string[] = [];
      while (resultSet.goToNextRow()) {
        let timestamp = resultSet.getLong(resultSet.getColumnIndex('createTime'));
        dates.push(DateUtil.formatDate(timestamp));
      }
      resultSet.close();
      return dates;
    } catch (err) {
      // 🟢 打印报错信息，方便排查
      console.error('[RecordRepo] getAllRecordDates Error: ' + JSON.stringify(err));
      return [];
    }
  }

  async getAllRecordIds(): Promise<number[]> {
    const store = RdbHelper.getRdbStore();
    if (!store) return [];
    try {
      let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
      const resultSet = await store.query(predicates);
      let ids: number[] = [];
      while (resultSet.goToNextRow()) {
        ids.push(resultSet.getLong(resultSet.getColumnIndex('habitId')));
      }
      resultSet.close();
      return ids;
    } catch (err) {
      return [];
    }
  }

  // =================================================================
  // 📅 详情页 (DetailPage) 专用
  // =================================================================

  async getRecordTimestamps(habitId: number): Promise<number[]> {
    const store = RdbHelper.getRdbStore();
    if (!store) return [];
    try {
      let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
      predicates.equalTo('habitId', habitId);
      predicates.orderByDesc('createTime');
      const resultSet = await store.query(predicates);
      let times: number[] = [];
      while (resultSet.goToNextRow()) {
        let timestamp = resultSet.getLong(resultSet.getColumnIndex('createTime'));
        times.push(timestamp);
      }
      resultSet.close();
      return times;
    } catch (err) {
      return [];
    }
  }

  async getRecordByDate(habitId: number, dateStr: string): Promise<RecordDetail | null> {
    const store = RdbHelper.getRdbStore();
    if (!store) return null;
    const start = new Date(dateStr + 'T00:00:00').getTime();
    const end = new Date(dateStr + 'T23:59:59').getTime();
    let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
    predicates.equalTo('habitId', habitId);
    predicates.greaterThanOrEqualTo('createTime', start);
    predicates.lessThanOrEqualTo('createTime', end);
    predicates.orderByDesc('createTime');
    const resultSet = await store.query(predicates);
    if (resultSet.goToNextRow()) {
      const detail: RecordDetail = {
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        mood: resultSet.getLong(resultSet.getColumnIndex('mood')),
        note: resultSet.getString(resultSet.getColumnIndex('note')),
        createTime: resultSet.getLong(resultSet.getColumnIndex('createTime'))
      };
      resultSet.close();
      return detail;
    }
    resultSet.close();
    return null;
  }

  async getRecordDateStrings(habitId: number): Promise<string[]> {
    const store = RdbHelper.getRdbStore();
    if (!store) return [];
    try {
      let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
      predicates.equalTo('habitId', habitId);
      const resultSet = await store.query(predicates);
      let dates: string[] = [];
      while (resultSet.goToNextRow()) {
        let timestamp = resultSet.getLong(resultSet.getColumnIndex('createTime'));
        dates.push(DateUtil.formatDate(timestamp));
      }
      resultSet.close();
      return dates;
    } catch (err) {
      return [];
    }
  }

  async getStreakStats(habitId: number): Promise<StreakStats> {
    const store = RdbHelper.getRdbStore();
    let stats = new StreakStats();
    if (!store) return stats;
    let predicates = new relationalStore.RdbPredicates(CommonConstants.RECORD_TABLE_NAME);
    predicates.equalTo('habitId', habitId);
    predicates.orderByDesc('createTime');
    const resultSet = await store.query(predicates);
    let uniqueDates: string[] = [];
    while (resultSet.goToNextRow()) {
      let ts = resultSet.getLong(resultSet.getColumnIndex('createTime'));
      let dateStr = DateUtil.formatDate(ts);
      if (!uniqueDates.includes(dateStr)) {
        uniqueDates.push(dateStr);
      }
    }
    resultSet.close();
    stats.totalCount = uniqueDates.length;
    if (stats.totalCount === 0) return stats;
    const todayStr = DateUtil.formatDate(new Date().getTime());
    const yesterday = new Date();
    yesterday.setDate(yesterday.getDate() - 1);
    const yesterdayStr = DateUtil.formatDate(yesterday.getTime());
    let startIndex = -1;
    if (uniqueDates.includes(todayStr)) {
      startIndex = uniqueDates.indexOf(todayStr);
    } else if (uniqueDates.includes(yesterdayStr)) {
      startIndex = uniqueDates.indexOf(yesterdayStr);
    }
    let currentStreak = 0;
    if (startIndex !== -1) {
      currentStreak = 1;
      for (let i = startIndex; i < uniqueDates.length - 1; i++) {
        if (this.checkIsConsecutive(uniqueDates[i], uniqueDates[i+1])) {
          currentStreak++;
        } else {
          break;
        }
      }
    }
    let maxStreak = 0;
    let tempStreak = 1;
    for (let i = 0; i < uniqueDates.length - 1; i++) {
      if (this.checkIsConsecutive(uniqueDates[i], uniqueDates[i+1])) {
        tempStreak++;
      } else {
        maxStreak = Math.max(maxStreak, tempStreak);
        tempStreak = 1;
      }
    }
    maxStreak = Math.max(maxStreak, tempStreak);
    if (stats.totalCount > 0 && maxStreak === 0) maxStreak = 1;
    stats.currentStreak = currentStreak;
    stats.maxStreak = maxStreak;
    return stats;
  }

  private checkIsConsecutive(dateStr1: string, dateStr2: string): boolean {
    const d1 = new Date(dateStr1);
    const d2 = new Date(dateStr2);
    const diffTime = Math.abs(d1.getTime() - d2.getTime());
    const oneDayTime = 1000 * 60 * 60 * 24;
    const diffDays = Math.round(diffTime / oneDayTime);
    return diffDays === 1;
  }
}

export default new RecordRepository();