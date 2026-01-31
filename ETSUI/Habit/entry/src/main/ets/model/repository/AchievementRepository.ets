import relationalStore from '@ohos.data.relationalStore';
import { CommonConstants } from '../../common/constants/CommonConstants';
import RdbHelper from '../database/RdbHelper';

export interface Achievement {
  id: number;
  name: string;
  icon: string;
  description: string;
  unlockCondition: string; // 关键字段：用于逻辑判断，如 "total_10", "streak_7"
  isUnlocked: boolean;
  unlockTime: number;      // 0 表示未解锁
}

class AchievementRepository {

  // 查询所有成就，按 ID 排序
  async queryAll(): Promise<Achievement[]> {
    const store = RdbHelper.getRdbStore();
    if (!store) {
      console.error('[AchievementRepo] Store is null');
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(CommonConstants.ACHIEVEMENT_TABLE_NAME);
    predicates.orderByAsc('id');

    const resultSet = await store.query(predicates);
    const achievements: Achievement[] = [];

    while (resultSet.goToNextRow()) {
      achievements.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        name: resultSet.getString(resultSet.getColumnIndex('name')),
        icon: resultSet.getString(resultSet.getColumnIndex('icon')),
        description: resultSet.getString(resultSet.getColumnIndex('description')),
        unlockCondition: resultSet.getString(resultSet.getColumnIndex('unlockCondition')),
        isUnlocked: resultSet.getLong(resultSet.getColumnIndex('isUnlocked')) === 1,
        unlockTime: resultSet.getLong(resultSet.getColumnIndex('unlockTime'))
      });
    }
    resultSet.close();
    return achievements;
  }

  // 根据条件标识查询（用于判断特定成就是否存在）
  async queryByCondition(condition: string): Promise<Achievement | null> {
    const store = RdbHelper.getRdbStore();
    if (!store) return null;

    const predicates = new relationalStore.RdbPredicates(CommonConstants.ACHIEVEMENT_TABLE_NAME);
    predicates.equalTo('unlockCondition', condition);

    const resultSet = await store.query(predicates);
    if (resultSet.rowCount > 0) {
      resultSet.goToFirstRow();
      const achievement: Achievement = {
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        name: resultSet.getString(resultSet.getColumnIndex('name')),
        icon: resultSet.getString(resultSet.getColumnIndex('icon')),
        description: resultSet.getString(resultSet.getColumnIndex('description')),
        unlockCondition: resultSet.getString(resultSet.getColumnIndex('unlockCondition')),
        isUnlocked: resultSet.getLong(resultSet.getColumnIndex('isUnlocked')) === 1,
        unlockTime: resultSet.getLong(resultSet.getColumnIndex('unlockTime'))
      };
      resultSet.close();
      return achievement;
    }
    resultSet.close();
    return null;
  }

  // 解锁指定成就，更新状态和时间戳
  async unlockAchievement(id: number): Promise<void> {
    const store = RdbHelper.getRdbStore();
    if (!store) return;

    const valueBucket: relationalStore.ValuesBucket = {
      isUnlocked: 1,
      unlockTime: new Date().getTime()
    };

    const predicates = new relationalStore.RdbPredicates(CommonConstants.ACHIEVEMENT_TABLE_NAME);
    predicates.equalTo('id', id);

    await store.update(valueBucket, predicates);
    console.info(`[AchievementRepo] 解锁成就 ID: ${id}`);
  }
}

export default new AchievementRepository();