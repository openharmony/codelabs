
import RdbHelper from '../database/RdbHelper';
import Habit from '../../common/bean/Habit';
import { CommonConstants } from '../../common/constants/CommonConstants';
import relationalStore from '@ohos.data.relationalStore';
import { ReminderHelper } from '../ReminderHelper';

class HabitRepository {

  // 1. 检查 INSERT：存的时候带上 categoryId 了吗？
  insert(habit: Habit): Promise<number> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('数据库未初始化');

    const valueBucket: relationalStore.ValuesBucket = {
      name: habit.name,
      color: habit.color,
      targetCount: habit.targetCount,
      categoryId: habit.categoryId,
      // 🆕 新增两列
      reminderTime: habit.reminderTime,
      reminderId: habit.reminderId
    };

    try {
      return store.insert(CommonConstants.HABIT_TABLE_NAME, valueBucket);
    } catch (err) {
      return Promise.reject(err);
    }
  }

  async deleteById(id: number): Promise<number> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB Error');

    try {
      // 🛑 在删除数据库记录前，先查一下这个习惯有没有提醒
      // 如果有，要先调用系统API取消掉
      let predicates = new relationalStore.RdbPredicates(CommonConstants.HABIT_TABLE_NAME);
      predicates.equalTo('id', id);

      const resultSet = await store.query(predicates);
      if (resultSet.goToFirstRow()) {
        let reminderId = resultSet.getLong(resultSet.getColumnIndex('reminderId'));
        // 如果有有效的提醒ID，取消它
        if (reminderId && reminderId > 0) {
          await ReminderHelper.cancelReminder(reminderId);
        }
      }
      resultSet.close();

      // 执行物理删除
      return store.delete(predicates);
    } catch (err) {
      return Promise.reject(err);
    }
  }

  // 2. 检查 QUERY：取的时候读出来了吗？(最容易漏这里！)
  queryAll(): Promise<Array<Habit>> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('数据库未初始化');

    try {
      let predicates = new relationalStore.RdbPredicates(CommonConstants.HABIT_TABLE_NAME);
      return store.query(predicates).then((resultSet) => {
        let habits: Array<Habit> = [];
        while (resultSet.goToNextRow()) {
          let habit = new Habit(
            resultSet.getString(resultSet.getColumnIndex('name')),
            resultSet.getString(resultSet.getColumnIndex('color')),
            resultSet.getLong(resultSet.getColumnIndex('targetCount'))
          );
          habit.id = resultSet.getLong(resultSet.getColumnIndex('id'));

          // 读取 categoryId
          try {
            let catIndex = resultSet.getColumnIndex('categoryId');
            if (catIndex >= 0) habit.categoryId = resultSet.getLong(catIndex);
          } catch (e) {}

          // 🆕 读取 reminderTime 和 reminderId
          try {
            let timeIndex = resultSet.getColumnIndex('reminderTime');
            let idIndex = resultSet.getColumnIndex('reminderId');
            if (timeIndex >= 0) habit.reminderTime = resultSet.getString(timeIndex);
            if (idIndex >= 0) habit.reminderId = resultSet.getLong(idIndex);
          } catch (e) {}

          habits.push(habit);
        }
        resultSet.close();
        return habits;
      });
    } catch (err) {
      return Promise.reject(err);
    }
  }
  cleanAll(): Promise<void> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB Error');

    let predicates = new relationalStore.RdbPredicates(CommonConstants.HABIT_TABLE_NAME);
    return store.delete(predicates).then(() => {
      return;
    });
  }
  /**
   * 🆕 根据 ID 更新习惯信息
   */
  update(habit: Habit): Promise<number> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB Error');

    const valueBucket: relationalStore.ValuesBucket = {
      name: habit.name,
      color: habit.color,
      targetCount: habit.targetCount,
      categoryId: habit.categoryId,
      reminderTime: habit.reminderTime,
      reminderId: habit.reminderId
    };

    try {
      let predicates = new relationalStore.RdbPredicates(CommonConstants.HABIT_TABLE_NAME);
      predicates.equalTo('id', habit.id); // 👈 锁定要修改的那一行
      return store.update(valueBucket, predicates);
    } catch (err) {
      return Promise.reject(err);
    }
  }

  /**
   * 🆕 根据 ID 查询单个习惯 (用于回显)
   */
  queryById(id: number): Promise<Habit | null> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB not initialized');

    try {
      let predicates = new relationalStore.RdbPredicates(CommonConstants.HABIT_TABLE_NAME);
      predicates.equalTo('id', id); // 只查这一个

      return store.query(predicates).then((resultSet) => {
        if (resultSet.goToFirstRow()) {
          let habit = new Habit(
            resultSet.getString(resultSet.getColumnIndex('name')),
            resultSet.getString(resultSet.getColumnIndex('color')),
            resultSet.getLong(resultSet.getColumnIndex('targetCount'))
          );
          habit.id = resultSet.getLong(resultSet.getColumnIndex('id'));

          // 补全所有字段
          try {
            let catIndex = resultSet.getColumnIndex('categoryId');
            if (catIndex >= 0) habit.categoryId = resultSet.getLong(catIndex);

            let timeIndex = resultSet.getColumnIndex('reminderTime');
            let idIndex = resultSet.getColumnIndex('reminderId');
            if (timeIndex >= 0) habit.reminderTime = resultSet.getString(timeIndex);
            if (idIndex >= 0) habit.reminderId = resultSet.getLong(idIndex);
          } catch (e) {}

          resultSet.close();
          return habit;
        }
        resultSet.close();
        return null;
      });
    } catch (err) {
      return Promise.reject(err);
    }
  }
}

export default new HabitRepository();