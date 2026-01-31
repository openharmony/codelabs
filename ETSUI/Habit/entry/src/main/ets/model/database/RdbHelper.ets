import relationalStore from '@ohos.data.relationalStore';
import { CommonConstants } from '../../common/constants/CommonConstants';

class RdbHelper {
  private rdbStore: relationalStore.RdbStore | null = null;

  /**
   * 初始化数据库
   */
  initRdb(context: Context): Promise<void> {
    const config: relationalStore.StoreConfig = {
      name: CommonConstants.DATABASE_NAME,
      securityLevel: relationalStore.SecurityLevel.S1
    };

    return new Promise((resolve, reject) => {
      relationalStore.getRdbStore(context, config, async (err, store) => {
        if (err) {
          console.error(`[RdbHelper] 初始化失败: code=${err.code}, message=${err.message}`);
          reject(err);
          return;
        }

        // 创建表
        if (store.version === 0) {
          store.executeSql(CommonConstants.CREATE_TABLE_SQL);
          store.executeSql(CommonConstants.CREATE_RECORD_TABLE_SQL);
          store.executeSql(CommonConstants.CREATE_CATEGORY_TABLE_SQL);

          // 创建成就表
          store.executeSql(CommonConstants.CREATE_ACHIEVEMENT_TABLE_SQL);

          // 初始化数据
          await this.initDefaultCategories(store);
          await this.initDefaultAchievements(store);

          store.version = 1;
        }

        this.rdbStore = store;
        console.info('[RdbHelper] 数据库初始化成功!');
        resolve();
      });
    });
  }

  getRdbStore(): relationalStore.RdbStore | null {
    return this.rdbStore;
  }

  private async initDefaultCategories(store: relationalStore.RdbStore): Promise<void> {
    class InitCategory {
      name: string;
      color: string;
      constructor(name: string, color: string) {
        this.name = name;
        this.color = color;
      }
    }

    const categories: InitCategory[] = [
      new InitCategory('全部', '#007DFF'),
      new InitCategory('学习', '#FF9900'),
      new InitCategory('运动', '#00CC66'),
      new InitCategory('生活', '#FF5555')
    ];

    for (const c of categories) {
      const valueBucket: relationalStore.ValuesBucket = {
        name: c.name,
        color: c.color
      };

      try {
        await store.insert(CommonConstants.CATEGORY_TABLE_NAME, valueBucket);
      } catch (err) {
        console.error(`[RdbHelper] 插入分类失败: ${JSON.stringify(err)}`);
      }
    }
  }

  /**
   * 初始化默认勋章
   */
  private async initDefaultAchievements(store: relationalStore.RdbStore): Promise<void> {
    interface InitAchievement {
      name: string;
      icon: string;
      description: string;
      unlockCondition: string;
    }

    const achievements: InitAchievement[] = [
      // --- 累计里程碑 ---
      { name: '初出茅庐', icon: 'medal_total_3', description: '累计打卡达到 3 天，迈出第一步', unlockCondition: 'total_3' },
      { name: '小试牛刀', icon: 'medal_total_7', description: '累计打卡达到 7 天，渐入佳境', unlockCondition: 'total_7' },
      { name: '初具规模', icon: 'medal_total_15', description: '累计打卡达到 15 天，养成习惯', unlockCondition: 'total_15' },
      { name: '凑满整月', icon: 'medal_total_30', description: '累计打卡达到 30 天，凑满整月', unlockCondition: 'total_30' },
      { name: '坚持不懈', icon: 'medal_total_50', description: '累计打卡达到 50 天，意志坚定', unlockCondition: 'total_50' },
      { name: '百日筑基', icon: 'medal_total_100', description: '累计打卡达到 100 天，坚如磐石', unlockCondition: 'total_100' },
      { name: '登峰造极', icon: 'medal_total_365', description: '累计打卡达到 365 天，年度霸主', unlockCondition: 'total_365' },

      // --- 连胜里程碑 ---
      { name: '习惯达人', icon: 'medal_streak_3', description: '连续打卡达到 3 天，保持势头', unlockCondition: 'streak_3' },
      { name: '自律精英', icon: 'medal_streak_7', description: '连续打卡达到 7 天，势不可挡', unlockCondition: 'streak_7' },
      { name: '不败传说', icon: 'medal_streak_15', description: '连续打卡达到 15 天，独孤求败', unlockCondition: 'streak_15' },
      { name: '超越自我', icon: 'medal_streak_30', description: '连续打卡达到 30 天，你就是神！', unlockCondition: 'streak_30' },

      // 🆕 新增的3个连胜勋章
      { name: '坚韧不拔', icon: 'medal_streak_50', description: '连续打卡达到 50 天，坚韧如钢', unlockCondition: 'streak_50' },
      { name: '百炼成钢', icon: 'medal_streak_100', description: '连续打卡达到 100 天，炉火纯青', unlockCondition: 'streak_100' },
      { name: '永恒传奇', icon: 'medal_streak_365', description: '连续打卡达到 365 天，创造传奇', unlockCondition: 'streak_365' }
    ];

    for (const a of achievements) {
      const valueBucket: relationalStore.ValuesBucket = {
        name: a.name,
        icon: a.icon,
        description: a.description,
        unlockCondition: a.unlockCondition,
        isUnlocked: 0,
        unlockTime: 0
      };
      try {
        await store.insert(CommonConstants.ACHIEVEMENT_TABLE_NAME, valueBucket);
      } catch (err) {
        console.error(`[RdbHelper] 插入成就失败: ${JSON.stringify(err)}`);
      }
    }
  }
}

export default new RdbHelper();