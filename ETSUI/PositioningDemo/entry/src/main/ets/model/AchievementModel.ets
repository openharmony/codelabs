/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// 成就类型枚举
export enum AchievementType {
  FIRST_SPORT = 'first_sport',              // 首次运动
  TOTAL_DISTANCE_1KM = 'total_distance_1km', // 累计距离1公里
  TOTAL_DISTANCE_5KM = 'total_distance_5km', // 累计距离5公里
  TOTAL_DISTANCE_10KM = 'total_distance_10km', // 累计距离10公里
  TOTAL_DISTANCE_50KM = 'total_distance_50km', // 累计距离50公里
  TOTAL_DISTANCE_100KM = 'total_distance_100km', // 累计距离100公里
  CONSECUTIVE_DAYS_3 = 'consecutive_days_3',   // 连续3天
  CONSECUTIVE_DAYS_7 = 'consecutive_days_7',   // 连续7天
  CONSECUTIVE_DAYS_30 = 'consecutive_days_30', // 连续30天
  TOTAL_RECORDS_10 = 'total_records_10',       // 累计10次运动
  TOTAL_RECORDS_50 = 'total_records_50',       // 累计50次运动
  TOTAL_RECORDS_100 = 'total_records_100',     // 累计100次运动
  SINGLE_DISTANCE_5KM = 'single_distance_5km', // 单次5公里
  SINGLE_DISTANCE_10KM = 'single_distance_10km', // 单次10公里
  SINGLE_DURATION_30MIN = 'single_duration_30min', // 单次30分钟
  SINGLE_DURATION_60MIN = 'single_duration_60min', // 单次60分钟
  TOTAL_CALORIES_1000 = 'total_calories_1000', // 累计1000卡路里
  TOTAL_CALORIES_5000 = 'total_calories_5000'  // 累计5000卡路里
}

// 成就数据模型
export class Achievement {
  id: string = '';                    // 成就ID（使用 AchievementType）
  name: string = '';                   // 成就名称
  description: string = '';            // 成就描述
  icon: string = '';                   // 成就图标（emoji）
  type: AchievementType = AchievementType.FIRST_SPORT; // 成就类型
  isUnlocked: boolean = false;         // 是否已解锁
  unlockedTime: number = 0;            // 解锁时间（时间戳）
  progress: number = 0;                // 当前进度（0-100）
  targetValue: number = 0;              // 目标值
  currentValue: number = 0;             // 当前值

  constructor(
    id: string,
    name: string,
    description: string,
    icon: string,
    type: AchievementType,
    targetValue: number
  ) {
    this.id = id;
    this.name = name;
    this.description = description;
    this.icon = icon;
    this.type = type;
    this.targetValue = targetValue;
    this.isUnlocked = false;
    this.unlockedTime = 0;
    this.progress = 0;
    this.currentValue = 0;
  }
}

// 成就配置 - 定义所有成就
export class AchievementConfig {
  static getAllAchievements(): Achievement[] {
    return [
      // 首次运动
      new Achievement(
        AchievementType.FIRST_SPORT,
        '初次体验',
        '完成第一次运动',
        '🎯',
        AchievementType.FIRST_SPORT,
        1
      ),
      
      // 累计距离
      new Achievement(
        AchievementType.TOTAL_DISTANCE_1KM,
        '起步者',
        '累计运动距离达到1公里',
        '🏃',
        AchievementType.TOTAL_DISTANCE_1KM,
        1000
      ),
      new Achievement(
        AchievementType.TOTAL_DISTANCE_5KM,
        '小有成就',
        '累计运动距离达到5公里',
        '🏃‍♂️',
        AchievementType.TOTAL_DISTANCE_5KM,
        5000
      ),
      new Achievement(
        AchievementType.TOTAL_DISTANCE_10KM,
        '坚持不懈',
        '累计运动距离达到10公里',
        '🏃‍♀️',
        AchievementType.TOTAL_DISTANCE_10KM,
        10000
      ),
      new Achievement(
        AchievementType.TOTAL_DISTANCE_50KM,
        '运动达人',
        '累计运动距离达到50公里',
        '🏅',
        AchievementType.TOTAL_DISTANCE_50KM,
        50000
      ),
      new Achievement(
        AchievementType.TOTAL_DISTANCE_100KM,
        '运动大师',
        '累计运动距离达到100公里',
        '🥇',
        AchievementType.TOTAL_DISTANCE_100KM,
        100000
      ),
      
      // 连续天数
      new Achievement(
        AchievementType.CONSECUTIVE_DAYS_3,
        '三天坚持',
        '连续3天运动',
        '🔥',
        AchievementType.CONSECUTIVE_DAYS_3,
        3
      ),
      new Achievement(
        AchievementType.CONSECUTIVE_DAYS_7,
        '一周坚持',
        '连续7天运动',
        '💪',
        AchievementType.CONSECUTIVE_DAYS_7,
        7
      ),
      new Achievement(
        AchievementType.CONSECUTIVE_DAYS_30,
        '月度坚持',
        '连续30天运动',
        '⭐',
        AchievementType.CONSECUTIVE_DAYS_30,
        30
      ),
      
      // 累计次数
      new Achievement(
        AchievementType.TOTAL_RECORDS_10,
        '十次运动',
        '累计完成10次运动',
        '📝',
        AchievementType.TOTAL_RECORDS_10,
        10
      ),
      new Achievement(
        AchievementType.TOTAL_RECORDS_50,
        '五十次运动',
        '累计完成50次运动',
        '📊',
        AchievementType.TOTAL_RECORDS_50,
        50
      ),
      new Achievement(
        AchievementType.TOTAL_RECORDS_100,
        '百次运动',
        '累计完成100次运动',
        '🎖️',
        AchievementType.TOTAL_RECORDS_100,
        100
      ),
      
      // 单次距离
      new Achievement(
        AchievementType.SINGLE_DISTANCE_5KM,
        '单次5公里',
        '单次运动距离达到5公里',
        '🚀',
        AchievementType.SINGLE_DISTANCE_5KM,
        5000
      ),
      new Achievement(
        AchievementType.SINGLE_DISTANCE_10KM,
        '单次10公里',
        '单次运动距离达到10公里',
        '🌟',
        AchievementType.SINGLE_DISTANCE_10KM,
        10000
      ),
      
      // 单次时长
      new Achievement(
        AchievementType.SINGLE_DURATION_30MIN,
        '半小时坚持',
        '单次运动时长达到30分钟',
        '⏰',
        AchievementType.SINGLE_DURATION_30MIN,
        30 * 60 * 1000 // 30分钟（毫秒）
      ),
      new Achievement(
        AchievementType.SINGLE_DURATION_60MIN,
        '一小时坚持',
        '单次运动时长达到60分钟',
        '⏱️',
        AchievementType.SINGLE_DURATION_60MIN,
        60 * 60 * 1000 // 60分钟（毫秒）
      ),
      
      // 累计卡路里
      new Achievement(
        AchievementType.TOTAL_CALORIES_1000,
        '千卡挑战',
        '累计消耗1000卡路里',
        '🔥',
        AchievementType.TOTAL_CALORIES_1000,
        1000
      ),
      new Achievement(
        AchievementType.TOTAL_CALORIES_5000,
        '五千卡挑战',
        '累计消耗5000卡路里',
        '💥',
        AchievementType.TOTAL_CALORIES_5000,
        5000
      )
    ];
  }
}
