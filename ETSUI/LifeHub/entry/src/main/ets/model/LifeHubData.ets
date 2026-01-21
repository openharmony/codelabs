/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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
/**
 * LifeHub 统一数据管理类
 * 管理健康、旅行、目标、心情、学习5个模块的数据
 */
import { relationalStore } from '@kit.ArkData';
import { Context } from '@kit.AbilityKit';

// ==================== 数据模型定义 ====================

// 健康数据模型
export interface HealthRecord {
  id?: number;
  type: 'weight' | 'blood_pressure' | 'blood_sugar' | 'exercise' | 'sleep'; // 数据类型
  value: number; // 数值
  unit: string; // 单位
  date: string; // 日期 (YYYY-MM-DD)
  time: number; // 时间戳
  note?: string; // 备注
  extra?: string; // 额外数据（JSON字符串，如血压的收缩压/舒张压）
}

// 健康目标模型
export interface HealthGoal {
  id?: number;
  type: string; // 目标类型
  targetValue: number; // 目标值
  currentValue: number; // 当前值
  deadline?: number; // 截止日期
  isActive: boolean; // 是否激活
}

// 旅行行程模型
export interface TravelPlan {
  id?: number;
  title: string; // 行程标题
  destination: string; // 目的地
  startDate: string; // 开始日期
  endDate: string; // 结束日期
  budget: number; // 预算
  actualCost: number; // 实际花费
  status: 'planning' | 'ongoing' | 'completed'; // 状态
  createTime: number; // 创建时间
  description?: string; // 描述
}

// 旅行地点模型
export interface TravelPlace {
  id?: number;
  planId: number; // 关联的行程ID
  name: string; // 地点名称
  type: 'attraction' | 'restaurant' | 'hotel' | 'other'; // 类型
  address?: string; // 地址
  latitude?: number; // 纬度
  longitude?: number; // 经度
  visitDate?: string; // 访问日期
  note?: string; // 备注
}

// 旅行日记模型
export interface TravelDiary {
  id?: number;
  planId: number; // 关联的行程ID
  date: string; // 日期
  title: string; // 标题
  content: string; // 内容
  photos?: string; // 照片路径（JSON数组）
  location?: string; // 位置
  mood?: string; // 心情
  createTime: number; // 创建时间
}

// 目标模型
export interface Goal {
  id?: number;
  title: string; // 目标标题
  description?: string; // 描述
  category: string; // 分类
  priority: 'high' | 'medium' | 'low'; // 优先级
  targetDate?: number; // 目标日期
  progress: number; // 进度 (0-100)
  status: 'not_started' | 'in_progress' | 'completed' | 'cancelled'; // 状态
  createTime: number; // 创建时间
  updateTime: number; // 更新时间
}

// 任务模型
export interface Task {
  id?: number;
  goalId: number; // 关联的目标ID
  title: string; // 任务标题
  description?: string; // 描述
  isCompleted: boolean; // 是否完成
  dueDate?: number; // 截止日期
  createTime: number; // 创建时间
}

// 心情记录模型
export interface MoodRecord {
  id?: number;
  mood: string; // 心情（emoji或文字）
  score: number; // 心情评分 (1-10)
  date: string; // 日期 (YYYY-MM-DD)
  time: number; // 时间戳
  tags?: string; // 标签（JSON数组）
  note?: string; // 备注
}

// 日记模型
export interface Diary {
  id?: number;
  date: string; // 日期
  title?: string; // 标题
  content: string; // 内容
  mood?: string; // 心情
  tags?: string; // 标签（JSON数组）
  photos?: string; // 照片路径（JSON数组）
  location?: string; // 位置
  weather?: string; // 天气
  createTime: number; // 创建时间
  updateTime: number; // 更新时间
}

// 技能模型
export interface Skill {
  id?: number;
  name: string; // 技能名称
  category: string; // 分类
  level: number; // 等级 (1-10)
  targetLevel: number; // 目标等级
  description?: string; // 描述
  createTime: number; // 创建时间
  updateTime: number; // 更新时间
}

// 学习记录模型
export interface LearningRecord {
  id?: number;
  skillId: number; // 关联的技能ID
  date: string; // 日期
  duration: number; // 学习时长（分钟）
  content: string; // 学习内容
  note?: string; // 备注
  createTime: number; // 创建时间
}

// 提醒模型
export interface Reminder {
  id?: number;
  title: string;
  description?: string;
  type: 'health' | 'goal' | 'mood' | 'learning' | 'diary' | 'custom';
  time: string; // HH:mm
  days: string; // 周几，如 "1,2,3,4,5"
  enabled: boolean;
  createTime: number;
  updateTime: number;
}

// 标签模型
export interface Tag {
  id?: number;
  name: string;
  color: string;
  category: 'health' | 'goal' | 'mood' | 'learning' | 'diary' | 'all';
  createTime: number;
}

// ==================== 数据管理类 ====================

// 仪表盘统计数据接口
export interface DashboardStats {
  healthRecordsToday: number;
  activeGoals: number;
  moodToday: MoodRecord | null;
  skillsCount: number;
  travelPlansActive: number;
}

export class LifeHubData {
  private context: Context | null = null;
  private rdbStore: relationalStore.RdbStore | null = null;
  private initPromise: Promise<void> | null = null;
  private readonly DB_NAME = 'lifehub.db';
  // 表名常量
  private readonly TABLE_HEALTH_RECORD = 'health_records';
  private readonly TABLE_HEALTH_GOAL = 'health_goals';
  private readonly TABLE_TRAVEL_PLAN = 'travel_plans';
  private readonly TABLE_TRAVEL_PLACE = 'travel_places';
  private readonly TABLE_TRAVEL_DIARY = 'travel_diaries';
  private readonly TABLE_GOAL = 'goals';
  private readonly TABLE_TASK = 'tasks';
  private readonly TABLE_MOOD_RECORD = 'mood_records';
  private readonly TABLE_DIARY = 'diaries';
  private readonly TABLE_SKILL = 'skills';
  private readonly TABLE_LEARNING_RECORD = 'learning_records';
  private readonly TABLE_REMINDER = 'reminders';
  private readonly TABLE_TAG = 'tags';

  constructor(context?: Context) {
    if (context) {
      this.context = context;
      this.initPromise = this.initDatabase();
    }
  }

  setContext(context: Context): void {
    this.context = context;
    if (!this.initPromise) {
      this.initPromise = this.initDatabase();
    }
  }

  // 确保数据库已初始化
  private async ensureDatabaseReady(): Promise<void> {
    if (this.initPromise) {
      await this.initPromise;
    } else if (this.context) {
      this.initPromise = this.initDatabase();
      await this.initPromise;
    } else {
      throw new Error('Context未设置，无法初始化数据库');
    }
  }

  // 初始化数据库
  private async initDatabase(): Promise<void> {
    if (!this.context) {
      console.error('LifeHubData: Context未设置');
      return;
    }

    try {
      const config: relationalStore.StoreConfig = {
        name: this.DB_NAME,
        securityLevel: relationalStore.SecurityLevel.S1
      };

      const rdbManager = relationalStore.getRdbStore(this.context, config);
      this.rdbStore = await rdbManager;

      // 创建所有表
      await this.createAllTables();
      console.log('LifeHubData: 数据库初始化成功');
    } catch (error) {
      console.error('LifeHubData: 数据库初始化失败:', error);
    }
  }

  // 创建所有表
  private async createAllTables(): Promise<void> {
    if (!this.rdbStore) {
      return;
    }

    // 健康记录表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_HEALTH_RECORD} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        type TEXT NOT NULL,
        value REAL NOT NULL,
        unit TEXT NOT NULL,
        date TEXT NOT NULL,
        time INTEGER NOT NULL,
        note TEXT,
        extra TEXT
      )
    `);

    // 健康目标表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_HEALTH_GOAL} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        type TEXT NOT NULL,
        targetValue REAL NOT NULL,
        currentValue REAL NOT NULL DEFAULT 0,
        deadline INTEGER,
        isActive INTEGER NOT NULL DEFAULT 1
      )
    `);

    // 旅行行程表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_TRAVEL_PLAN} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT NOT NULL,
        destination TEXT NOT NULL,
        startDate TEXT NOT NULL,
        endDate TEXT NOT NULL,
        budget REAL NOT NULL DEFAULT 0,
        actualCost REAL NOT NULL DEFAULT 0,
        status TEXT NOT NULL,
        createTime INTEGER NOT NULL,
        description TEXT
      )
    `);

    // 旅行地点表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_TRAVEL_PLACE} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        planId INTEGER NOT NULL,
        name TEXT NOT NULL,
        type TEXT NOT NULL,
        address TEXT,
        latitude REAL,
        longitude REAL,
        visitDate TEXT,
        note TEXT,
        FOREIGN KEY (planId) REFERENCES ${this.TABLE_TRAVEL_PLAN}(id) ON DELETE CASCADE
      )
    `);

    // 旅行日记表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_TRAVEL_DIARY} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        planId INTEGER NOT NULL,
        date TEXT NOT NULL,
        title TEXT NOT NULL,
        content TEXT NOT NULL,
        photos TEXT,
        location TEXT,
        mood TEXT,
        createTime INTEGER NOT NULL,
        FOREIGN KEY (planId) REFERENCES ${this.TABLE_TRAVEL_PLAN}(id) ON DELETE CASCADE
      )
    `);

    // 目标表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_GOAL} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT NOT NULL,
        description TEXT,
        category TEXT NOT NULL,
        priority TEXT NOT NULL,
        targetDate INTEGER,
        progress INTEGER NOT NULL DEFAULT 0,
        status TEXT NOT NULL,
        createTime INTEGER NOT NULL,
        updateTime INTEGER NOT NULL
      )
    `);

    // 任务表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_TASK} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        goalId INTEGER NOT NULL,
        title TEXT NOT NULL,
        description TEXT,
        isCompleted INTEGER NOT NULL DEFAULT 0,
        dueDate INTEGER,
        createTime INTEGER NOT NULL,
        FOREIGN KEY (goalId) REFERENCES ${this.TABLE_GOAL}(id) ON DELETE CASCADE
      )
    `);

    // 心情记录表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_MOOD_RECORD} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        mood TEXT NOT NULL,
        score INTEGER NOT NULL,
        date TEXT NOT NULL,
        time INTEGER NOT NULL,
        tags TEXT,
        note TEXT
      )
    `);

    // 日记表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_DIARY} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        date TEXT NOT NULL,
        title TEXT,
        content TEXT NOT NULL,
        mood TEXT,
        tags TEXT,
        photos TEXT,
        location TEXT,
        weather TEXT,
        createTime INTEGER NOT NULL,
        updateTime INTEGER NOT NULL
      )
    `);

    // 技能表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_SKILL} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        category TEXT NOT NULL,
        level INTEGER NOT NULL DEFAULT 1,
        targetLevel INTEGER NOT NULL DEFAULT 5,
        description TEXT,
        createTime INTEGER NOT NULL,
        updateTime INTEGER NOT NULL
      )
    `);

    // 学习记录表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_LEARNING_RECORD} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        skillId INTEGER NOT NULL,
        date TEXT NOT NULL,
        duration INTEGER NOT NULL,
        content TEXT NOT NULL,
        note TEXT,
        createTime INTEGER NOT NULL,
        FOREIGN KEY (skillId) REFERENCES ${this.TABLE_SKILL}(id) ON DELETE CASCADE
      )
    `);

    // 提醒表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_REMINDER} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT NOT NULL,
        description TEXT,
        type TEXT NOT NULL,
        time TEXT NOT NULL,
        days TEXT NOT NULL,
        enabled INTEGER NOT NULL DEFAULT 1,
        createTime INTEGER NOT NULL,
        updateTime INTEGER NOT NULL
      )
    `);

    // 标签表
    await this.rdbStore.executeSql(`
      CREATE TABLE IF NOT EXISTS ${this.TABLE_TAG} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE,
        color TEXT NOT NULL,
        category TEXT NOT NULL,
        createTime INTEGER NOT NULL
      )
    `);

    console.log('LifeHubData: 所有表创建成功');
  }

  // ==================== 工具方法 ====================

  private formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }

  private getTodayString(): string {
    return this.formatDate(new Date());
  }

  // ==================== 健康模块方法 ====================

  async addHealthRecord(record: HealthRecord): Promise<number> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      type: record.type,
      value: record.value,
      unit: record.unit,
      date: record.date || this.getTodayString(),
      time: record.time || Date.now(),
      note: record.note || '',
      extra: record.extra || ''
    };

    return Number(await this.rdbStore.insert(this.TABLE_HEALTH_RECORD, valuesBucket));
  }

  async getHealthRecords(type?: string, days: number = 30): Promise<HealthRecord[]> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_HEALTH_RECORD);
    if (type) {
      predicates.equalTo('type', type);
    }
    predicates.orderByDesc('date');
    predicates.limitAs(days);

    const resultSet = await this.rdbStore.query(predicates);
    const records: HealthRecord[] = [];

    while (resultSet.goToNextRow()) {
      records.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        type: resultSet.getString(resultSet.getColumnIndex('type')) as 'weight' | 'blood_pressure' | 'blood_sugar' | 'exercise' | 'sleep',
        value: resultSet.getDouble(resultSet.getColumnIndex('value')),
        unit: resultSet.getString(resultSet.getColumnIndex('unit')),
        date: resultSet.getString(resultSet.getColumnIndex('date')),
        time: resultSet.getLong(resultSet.getColumnIndex('time')),
        note: resultSet.getString(resultSet.getColumnIndex('note')),
        extra: resultSet.getString(resultSet.getColumnIndex('extra'))
      });
    }

    resultSet.close();
    return records;
  }

  // ==================== 旅行模块方法 ====================

  async addTravelPlan(plan: TravelPlan): Promise<number> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      title: plan.title,
      destination: plan.destination,
      startDate: plan.startDate,
      endDate: plan.endDate,
      budget: plan.budget || 0,
      actualCost: plan.actualCost || 0,
      status: plan.status,
      createTime: plan.createTime || Date.now(),
      description: plan.description || ''
    };

    return Number(await this.rdbStore.insert(this.TABLE_TRAVEL_PLAN, valuesBucket));
  }

  async getAllTravelPlans(): Promise<TravelPlan[]> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_TRAVEL_PLAN);
    predicates.orderByDesc('createTime');

    const resultSet = await this.rdbStore.query(predicates);
    const plans: TravelPlan[] = [];

    while (resultSet.goToNextRow()) {
      plans.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        title: resultSet.getString(resultSet.getColumnIndex('title')),
        destination: resultSet.getString(resultSet.getColumnIndex('destination')),
        startDate: resultSet.getString(resultSet.getColumnIndex('startDate')),
        endDate: resultSet.getString(resultSet.getColumnIndex('endDate')),
        budget: resultSet.getDouble(resultSet.getColumnIndex('budget')),
        actualCost: resultSet.getDouble(resultSet.getColumnIndex('actualCost')),
        status: resultSet.getString(resultSet.getColumnIndex('status')) as 'planning' | 'ongoing' | 'completed',
        createTime: resultSet.getLong(resultSet.getColumnIndex('createTime')),
        description: resultSet.getString(resultSet.getColumnIndex('description'))
      });
    }

    resultSet.close();
    return plans;
  }

  // ==================== 目标模块方法 ====================

  async addGoal(goal: Goal): Promise<number> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      title: goal.title,
      description: goal.description || '',
      category: goal.category,
      priority: goal.priority,
      targetDate: goal.targetDate || null,
      progress: goal.progress || 0,
      status: goal.status,
      createTime: goal.createTime || Date.now(),
      updateTime: goal.updateTime || Date.now()
    };

    return Number(await this.rdbStore.insert(this.TABLE_GOAL, valuesBucket));
  }

  async getAllGoals(): Promise<Goal[]> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_GOAL);
    predicates.orderByDesc('createTime');

    const resultSet = await this.rdbStore.query(predicates);
    const goals: Goal[] = [];

    while (resultSet.goToNextRow()) {
      goals.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        title: resultSet.getString(resultSet.getColumnIndex('title')),
        description: resultSet.getString(resultSet.getColumnIndex('description')),
        category: resultSet.getString(resultSet.getColumnIndex('category')),
        priority: resultSet.getString(resultSet.getColumnIndex('priority')) as 'high' | 'medium' | 'low',
        targetDate: resultSet.getLong(resultSet.getColumnIndex('targetDate')),
        progress: resultSet.getLong(resultSet.getColumnIndex('progress')),
        status: resultSet.getString(resultSet.getColumnIndex('status')) as 'not_started' | 'in_progress' | 'completed' | 'cancelled',
        createTime: resultSet.getLong(resultSet.getColumnIndex('createTime')),
        updateTime: resultSet.getLong(resultSet.getColumnIndex('updateTime'))
      });
    }

    resultSet.close();
    return goals;
  }

  // ==================== 心情模块方法 ====================

  async addMoodRecord(record: MoodRecord): Promise<number> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      mood: record.mood,
      score: record.score,
      date: record.date || this.getTodayString(),
      time: record.time || Date.now(),
      tags: record.tags || '',
      note: record.note || ''
    };

    return Number(await this.rdbStore.insert(this.TABLE_MOOD_RECORD, valuesBucket));
  }

  async getMoodRecords(days: number = 30): Promise<MoodRecord[]> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_MOOD_RECORD);
    predicates.orderByDesc('date');
    predicates.limitAs(days);

    const resultSet = await this.rdbStore.query(predicates);
    const records: MoodRecord[] = [];

    while (resultSet.goToNextRow()) {
      records.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        mood: resultSet.getString(resultSet.getColumnIndex('mood')),
        score: resultSet.getLong(resultSet.getColumnIndex('score')),
        date: resultSet.getString(resultSet.getColumnIndex('date')),
        time: resultSet.getLong(resultSet.getColumnIndex('time')),
        tags: resultSet.getString(resultSet.getColumnIndex('tags')),
        note: resultSet.getString(resultSet.getColumnIndex('note'))
      });
    }

    resultSet.close();
    return records;
  }

  // ==================== 学习模块方法 ====================

  async addSkill(skill: Skill): Promise<number> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      name: skill.name,
      category: skill.category,
      level: skill.level || 1,
      targetLevel: skill.targetLevel || 5,
      description: skill.description || '',
      createTime: skill.createTime || Date.now(),
      updateTime: skill.updateTime || Date.now()
    };

    return Number(await this.rdbStore.insert(this.TABLE_SKILL, valuesBucket));
  }

  async getAllSkills(): Promise<Skill[]> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_SKILL);
    predicates.orderByDesc('updateTime');

    const resultSet = await this.rdbStore.query(predicates);
    const skills: Skill[] = [];

    while (resultSet.goToNextRow()) {
      skills.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        name: resultSet.getString(resultSet.getColumnIndex('name')),
        category: resultSet.getString(resultSet.getColumnIndex('category')),
        level: resultSet.getLong(resultSet.getColumnIndex('level')),
        targetLevel: resultSet.getLong(resultSet.getColumnIndex('targetLevel')),
        description: resultSet.getString(resultSet.getColumnIndex('description')),
        createTime: resultSet.getLong(resultSet.getColumnIndex('createTime')),
        updateTime: resultSet.getLong(resultSet.getColumnIndex('updateTime'))
      });
    }

    resultSet.close();
    return skills;
  }

  // ==================== 日记模块方法 ====================

  async addDiary(diary: Diary): Promise<number> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      date: diary.date || this.getTodayString(),
      title: diary.title || '',
      content: diary.content,
      mood: diary.mood || '',
      tags: diary.tags || '',
      photos: diary.photos || '',
      location: diary.location || '',
      weather: diary.weather || '',
      createTime: diary.createTime || Date.now(),
      updateTime: diary.updateTime || Date.now()
    };

    return Number(await this.rdbStore.insert(this.TABLE_DIARY, valuesBucket));
  }

  async getAllDiaries(): Promise<Diary[]> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_DIARY);
    predicates.orderByDesc('createTime');

    const resultSet = await this.rdbStore.query(predicates);
    const diaries: Diary[] = [];

    while (resultSet.goToNextRow()) {
      diaries.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        date: resultSet.getString(resultSet.getColumnIndex('date')),
        title: resultSet.getString(resultSet.getColumnIndex('title')),
        content: resultSet.getString(resultSet.getColumnIndex('content')),
        mood: resultSet.getString(resultSet.getColumnIndex('mood')),
        tags: resultSet.getString(resultSet.getColumnIndex('tags')),
        photos: resultSet.getString(resultSet.getColumnIndex('photos')),
        location: resultSet.getString(resultSet.getColumnIndex('location')),
        weather: resultSet.getString(resultSet.getColumnIndex('weather')),
        createTime: resultSet.getLong(resultSet.getColumnIndex('createTime')),
        updateTime: resultSet.getLong(resultSet.getColumnIndex('updateTime'))
      });
    }

    resultSet.close();
    return diaries;
  }

  // ==================== 提醒模块方法 ====================

  async addReminder(reminder: Reminder): Promise<number> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      title: reminder.title,
      description: reminder.description || '',
      type: reminder.type,
      time: reminder.time,
      days: reminder.days,
      enabled: reminder.enabled ? 1 : 0,
      createTime: reminder.createTime || Date.now(),
      updateTime: reminder.updateTime || Date.now()
    };

    return Number(await this.rdbStore.insert(this.TABLE_REMINDER, valuesBucket));
  }

  async getAllReminders(): Promise<Reminder[]> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_REMINDER);
    predicates.orderByDesc('createTime');

    const resultSet = await this.rdbStore.query(predicates);
    const reminders: Reminder[] = [];

    while (resultSet.goToNextRow()) {
      const typeStr = resultSet.getString(resultSet.getColumnIndex('type'));
      const type: 'health' | 'goal' | 'mood' | 'learning' | 'diary' | 'custom' =
        (typeStr === 'health' || typeStr === 'goal' || typeStr === 'mood' ||
          typeStr === 'learning' || typeStr === 'diary' || typeStr === 'custom')
          ? typeStr : 'custom';

      reminders.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        title: resultSet.getString(resultSet.getColumnIndex('title')),
        description: resultSet.getString(resultSet.getColumnIndex('description')),
        type: type,
        time: resultSet.getString(resultSet.getColumnIndex('time')),
        days: resultSet.getString(resultSet.getColumnIndex('days')),
        enabled: resultSet.getLong(resultSet.getColumnIndex('enabled')) === 1,
        createTime: resultSet.getLong(resultSet.getColumnIndex('createTime')),
        updateTime: resultSet.getLong(resultSet.getColumnIndex('updateTime'))
      });
    }

    resultSet.close();
    return reminders;
  }

  async updateReminder(reminder: Reminder): Promise<void> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      title: reminder.title,
      description: reminder.description || '',
      type: reminder.type,
      time: reminder.time,
      days: reminder.days,
      enabled: reminder.enabled ? 1 : 0,
      updateTime: Date.now()
    };

    const predicates = new relationalStore.RdbPredicates(this.TABLE_REMINDER);
    predicates.equalTo('id', reminder.id);

    await this.rdbStore.update(valuesBucket, predicates);
  }

  // ==================== 标签模块方法 ====================

  async addTag(tag: Tag): Promise<number> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const valuesBucket: relationalStore.ValuesBucket = {
      name: tag.name,
      color: tag.color,
      category: tag.category,
      createTime: tag.createTime || Date.now()
    };

    return Number(await this.rdbStore.insert(this.TABLE_TAG, valuesBucket));
  }

  async getAllTags(): Promise<Tag[]> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return [];
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_TAG);
    predicates.orderByDesc('createTime');

    const resultSet = await this.rdbStore.query(predicates);
    const tags: Tag[] = [];

    while (resultSet.goToNextRow()) {
      const categoryStr = resultSet.getString(resultSet.getColumnIndex('category'));
      const category: 'health' | 'goal' | 'mood' | 'learning' | 'diary' | 'all' =
        (categoryStr === 'health' || categoryStr === 'goal' || categoryStr === 'mood' ||
          categoryStr === 'learning' || categoryStr === 'diary' || categoryStr === 'all')
          ? categoryStr : 'all';

      tags.push({
        id: resultSet.getLong(resultSet.getColumnIndex('id')),
        name: resultSet.getString(resultSet.getColumnIndex('name')),
        color: resultSet.getString(resultSet.getColumnIndex('color')),
        category: category,
        createTime: resultSet.getLong(resultSet.getColumnIndex('createTime'))
      });
    }

    resultSet.close();
    return tags;
  }

  async deleteTag(tagId: number): Promise<void> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      throw new Error('数据库未初始化');
    }

    const predicates = new relationalStore.RdbPredicates(this.TABLE_TAG);
    predicates.equalTo('id', tagId);

    await this.rdbStore.delete(predicates);
  }

  // ==================== 仪表盘统计方法 ====================

  async getDashboardStats(): Promise<DashboardStats> {
    await this.ensureDatabaseReady();
    if (!this.rdbStore) {
      return {
        healthRecordsToday: 0,
        activeGoals: 0,
        moodToday: null,
        skillsCount: 0,
        travelPlansActive: 0
      };
    }

    const today = this.getTodayString();

    // 今日健康记录数 - 直接查询今日记录
    const healthPredicates = new relationalStore.RdbPredicates(this.TABLE_HEALTH_RECORD);
    healthPredicates.equalTo('date', today);
    const healthResultSet = await this.rdbStore.query(healthPredicates);
    let healthRecordsToday = 0;
    while (healthResultSet.goToNextRow()) {
      healthRecordsToday++;
    }
    healthResultSet.close();

    // 活跃目标数
    const goals = await this.getAllGoals();
    let activeGoals = 0;
    for (let i = 0; i < goals.length; i++) {
      if (goals[i].status === 'in_progress') {
        activeGoals++;
      }
    }

    // 今日心情 - 直接查询今日记录
    const moodPredicates = new relationalStore.RdbPredicates(this.TABLE_MOOD_RECORD);
    moodPredicates.equalTo('date', today);
    moodPredicates.orderByDesc('time');
    const moodResultSet = await this.rdbStore.query(moodPredicates);
    let moodToday: MoodRecord | null = null;
    if (moodResultSet.goToNextRow()) {
      moodToday = {
        id: moodResultSet.getLong(moodResultSet.getColumnIndex('id')),
        mood: moodResultSet.getString(moodResultSet.getColumnIndex('mood')),
        score: moodResultSet.getLong(moodResultSet.getColumnIndex('score')),
        date: moodResultSet.getString(moodResultSet.getColumnIndex('date')),
        time: moodResultSet.getLong(moodResultSet.getColumnIndex('time')),
        tags: moodResultSet.getString(moodResultSet.getColumnIndex('tags')),
        note: moodResultSet.getString(moodResultSet.getColumnIndex('note'))
      };
    }
    moodResultSet.close();

    // 技能数
    const skills = await this.getAllSkills();
    const skillsCount: number = skills.length;

    // 进行中的旅行
    const travelPlans = await this.getAllTravelPlans();
    let travelPlansActive = 0;
    for (let i = 0; i < travelPlans.length; i++) {
      if (travelPlans[i].status === 'ongoing') {
        travelPlansActive++;
      }
    }

    const result: DashboardStats = {
      healthRecordsToday: healthRecordsToday,
      activeGoals: activeGoals,
      moodToday: moodToday,
      skillsCount: skillsCount,
      travelPlansActive: travelPlansActive
    };

    return result;
  }
}
