/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
// src/main/ets/common/ScheduleData.ets
import dataPreferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';

/**
 * 课程信息接口定义
 * 使用接口定义数据结构，保证类型安全
 */
export interface CourseModel {
  id: string;             // 唯一标识符（由坐标生成）
  dayIndex: number;       // 周几 (0-6)
  periodIndex: number;    // 第几节 (0-4)
  courseName: string;     // 课程名称
  classroom: string;      // 教室位置
  teacherName: string;    // 授课教师
  timeRange: string;      // 具体时间段
  color?: string;         // 背景颜色（可选，持久化存储）
  createTime: number;     // 创建时间戳
  updateTime: number;     // 更新时间戳
}

/**
 * 课表配置常量
 */
export class ScheduleConfig {
  // 星期配置
  static readonly WEEK_DAYS: string[] = [
    '周一', '周二', '周三', '周四', '周五', '周六', '周日'
  ];

  // 节次配置
  static readonly PERIODS: string[] = [
    '上午第一节', '上午第二节',
    '下午第一节', '下午第二节',
    '晚上课程'
  ];

  // 默认时间段建议
  static readonly DEFAULT_TIMES: string[] = [
    '08:00 - 09:40',
    '10:00 - 11:40',
    '14:00 - 15:40',
    '16:00 - 17:40',
    '19:00 - 21:30'
  ];
}

/**
 * 颜色工具类
 * 根据课程名称生成哈希颜色，确保同一门课颜色一致
 */
export class CourseColorUtil {
  private static readonly PRESET_COLORS: string[] = [
    '#FF9F89', // 柔和红
    '#89CFF0', // 天空蓝
    '#98FB98', // 帕雷绿
    '#DDA0DD', // 梅子色
    '#F0E68C', // 卡其色
    '#87CEEB', // 天蓝色
    '#FFB6C1', // 浅粉红
    '#20B2AA', // 浅海绿
    '#FFA07A', // 浅鲑红
    '#9370DB'  // 中紫色
  ];

  /**
   * 根据字符串计算哈希值并返回对应颜色
   * @param text 输入文本
   * @returns 十六进制颜色字符串
   */
  static getColorForText(text: string): string {
    if (!text || text.length === 0) {
      return '#E0E0E0'; // 默认灰色
    }
    let hash = 0;
    for (let i = 0; i < text.length; i++) {
      hash = text.charCodeAt(i) + ((hash << 5) - hash);
    }
    const index = Math.abs(hash) % CourseColorUtil.PRESET_COLORS.length;
    return CourseColorUtil.PRESET_COLORS[index];
  }
}

/**
 * 课表数据管理器
 * 负责 Preferences 的初始化、读取、保存和删除
 */
export class ScheduleManager {
  private static readonly PREF_NAME = 'ClassSchedule_V1';
  private static readonly KEY_DATA = 'schedule_list_json';
  private static context: common.UIAbilityContext | null = null;

  /**
   * 初始化管理器
   * @param context 应用上下文
   */
  static init(context: common.UIAbilityContext): void {
    if (!context) {
      console.error('ScheduleManager: Context is null during init.');
      return;
    }
    ScheduleManager.context = context;
  }

  /**
   * 获取所有课程数据
   * @returns Promise<CourseModel[]>
   */
  static async getAllCourses(): Promise<CourseModel[]> {
    if (!ScheduleManager.context) {
      console.error('ScheduleManager: Context not initialized.');
      return [];
    }

    try {
      const pref = await dataPreferences.getPreferences(ScheduleManager.context, ScheduleManager.PREF_NAME);
      const jsonStr = await pref.get(ScheduleManager.KEY_DATA, '[]') as string;
      const courses = JSON.parse(jsonStr) as CourseModel[];
      console.info(`ScheduleManager: Loaded ${courses.length} courses.`);
      return courses;
    } catch (error) {
      console.error(`ScheduleManager: Failed to load courses. Error: ${JSON.stringify(error)}`);
      return [];
    }
  }

  /**
   * 保存或更新课程
   * @param course 课程对象
   * @returns boolean 是否成功
   */
  static async saveCourse(course: CourseModel): Promise<boolean> {
    if (!ScheduleManager.context) return false;

    try {
      // 1. 获取现有列表
      const allCourses = await ScheduleManager.getAllCourses();

      // 2. 查找是否存在（基于坐标）
      const index = allCourses.findIndex(c =>
      c.dayIndex === course.dayIndex && c.periodIndex === course.periodIndex
      );

      // 3. 更新或插入
      if (index !== -1) {
        // 更新现有记录，保留 ID，更新内容和时间
        allCourses[index] = {
          id: allCourses[index].id, // 保持原ID不变
          dayIndex: course.dayIndex,
          periodIndex: course.periodIndex,
          courseName: course.courseName,
          classroom: course.classroom,
          teacherName: course.teacherName,
          timeRange: course.timeRange,
          color: course.color,
          createTime: allCourses[index].createTime, // 保持原创建时间
          updateTime: Date.now()
        };
      } else {
        // 新增记录
        course.createTime = Date.now();
        course.updateTime = Date.now();
        course.id = `${course.dayIndex}_${course.periodIndex}_${Date.now()}`;
        allCourses.push(course);
      }

      // 4. 持久化存储
      const pref = await dataPreferences.getPreferences(ScheduleManager.context, ScheduleManager.PREF_NAME);
      await pref.put(ScheduleManager.KEY_DATA, JSON.stringify(allCourses));
      await pref.flush(); // 强制写入磁盘
      return true;

    } catch (error) {
      console.error(`ScheduleManager: Save failed. Error: ${JSON.stringify(error)}`);
      return false;
    }
  }

  /**
   * 删除指定位置的课程
   * @param dayIndex 周几
   * @param periodIndex 第几节
   */
  static async deleteCourse(dayIndex: number, periodIndex: number): Promise<boolean> {
    if (!ScheduleManager.context) return false;

    try {
      let allCourses = await ScheduleManager.getAllCourses();
      const initialLength = allCourses.length;

      // 过滤掉目标课程
      allCourses = allCourses.filter(c =>
      !(c.dayIndex === dayIndex && c.periodIndex === periodIndex)
      );

      if (allCourses.length === initialLength) {
        console.warn('ScheduleManager: No course found to delete.');
        return true; // 没找到也算删除成功
      }

      const pref = await dataPreferences.getPreferences(ScheduleManager.context, ScheduleManager.PREF_NAME);
      await pref.put(ScheduleManager.KEY_DATA, JSON.stringify(allCourses));
      await pref.flush();
      return true;

    } catch (error) {
      console.error(`ScheduleManager: Delete failed. Error: ${JSON.stringify(error)}`);
      return false;
    }
  }
}