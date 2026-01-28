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
// 1. 定义运动类别枚举
export enum ExerciseCategory {
  AEROBIC = "有氧运动",
  STRENGTH_TRAINING = "力量训练",
  YOGA_FLEXIBILITY = "瑜伽与柔韧性训练",
  BALL_SPORTS = "球类运动",
  OUTDOOR_RECREATION = "户外与休闲运动",
  DAILY_ACTIVITIES = "日常活动与家务",
  CUSTOM = '自定义' // 新增自定义类型
}

// 2. 运动对象接口
export interface ExerciseItem {
  id: number;
  name: string;
  calories: number; // kcal / 15 minutes
  category: ExerciseCategory;
}

// 3. 运动记录接口
export interface ExerciseRecord {
  id: string;
  exerciseId: number;
  exerciseName: string;
  portion: number;
  totalCalories: number;
  date: string;
}

// 4. 庞大的本地运动库
export const EXERCISE_DATABASE: ExerciseItem[] = [
  // 有氧运动类 (id: 100-199)
  { id: 101, name: "慢跑", calories: 150, category: ExerciseCategory.AEROBIC },
  { id: 102, name: "快跑", calories: 210, category: ExerciseCategory.AEROBIC },
  { id: 103, name: "跳绳", calories: 180, category: ExerciseCategory.AEROBIC },
  { id: 104, name: "动感单车", calories: 160, category: ExerciseCategory.AEROBIC },
  { id: 105, name: "椭圆机", calories: 140, category: ExerciseCategory.AEROBIC },
  { id: 106, name: "划船机", calories: 170, category: ExerciseCategory.AEROBIC },
  { id: 107, name: "游泳（慢速）", calories: 130, category: ExerciseCategory.AEROBIC },
  { id: 108, name: "游泳（快速）", calories: 190, category: ExerciseCategory.AEROBIC },
  { id: 109, name: "爬楼梯", calories: 175, category: ExerciseCategory.AEROBIC },
  { id: 110, name: "高强度间歇训练", calories: 220, category: ExerciseCategory.AEROBIC },
  { id: 111, name: "尊巴舞", calories: 145, category: ExerciseCategory.AEROBIC },
  { id: 112, name: "有氧操", calories: 135, category: ExerciseCategory.AEROBIC },
  { id: 113, name: "健走", calories: 110, category: ExerciseCategory.AEROBIC },
  { id: 114, name: "登山", calories: 165, category: ExerciseCategory.AEROBIC },
  { id: 115, name: "拳击训练", calories: 185, category: ExerciseCategory.AEROBIC },

  // 力量训练类 (id: 200-299)
  { id: 201, name: "卧推", calories: 95, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 202, name: "深蹲", calories: 120, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 203, name: "硬拉", calories: 130, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 204, name: "引体向上", calories: 110, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 205, name: "俯卧撑", calories: 105, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 206, name: "哑铃训练", calories: 100, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 207, name: "杠铃训练", calories: 115, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 208, name: "壶铃摇摆", calories: 140, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 209, name: "腿举", calories: 125, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 210, name: "划船动作", calories: 110, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 211, name: "弯举", calories: 85, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 212, name: "臂屈伸", calories: 90, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 213, name: "核心训练", calories: 105, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 214, name: "负重行走", calories: 135, category: ExerciseCategory.STRENGTH_TRAINING },
  { id: 215, name: "综合力量训练", calories: 125, category: ExerciseCategory.STRENGTH_TRAINING },

  // 瑜伽与柔韧性训练 (id: 300-399)
  { id: 301, name: "哈达瑜伽", calories: 70, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 302, name: "流瑜伽", calories: 100, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 303, name: "热瑜伽", calories: 120, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 304, name: "阴瑜伽", calories: 60, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 305, name: "普拉提", calories: 95, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 306, name: "伸展运动", calories: 55, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 307, name: "太极", calories: 75, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 308, name: "芭蕾形体", calories: 110, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 309, name: "呼吸冥想", calories: 40, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 310, name: "体态矫正训练", calories: 85, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 311, name: "平衡训练", calories: 80, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 312, name: "柔韧性训练", calories: 65, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 313, name: "空中瑜伽", calories: 115, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 314, name: "康复瑜伽", calories: 65, category: ExerciseCategory.YOGA_FLEXIBILITY },
  { id: 315, name: "舞韵瑜伽", calories: 105, category: ExerciseCategory.YOGA_FLEXIBILITY },

  // 球类运动 (id: 400-499)
  { id: 401, name: "篮球", calories: 160, category: ExerciseCategory.BALL_SPORTS },
  { id: 402, name: "足球", calories: 170, category: ExerciseCategory.BALL_SPORTS },
  { id: 403, name: "网球", calories: 155, category: ExerciseCategory.BALL_SPORTS },
  { id: 404, name: "羽毛球", calories: 145, category: ExerciseCategory.BALL_SPORTS },
  { id: 405, name: "乒乓球", calories: 125, category: ExerciseCategory.BALL_SPORTS },
  { id: 406, name: "排球", calories: 135, category: ExerciseCategory.BALL_SPORTS },
  { id: 407, name: "壁球", calories: 175, category: ExerciseCategory.BALL_SPORTS },
  { id: 408, name: "高尔夫（步行）", calories: 115, category: ExerciseCategory.BALL_SPORTS },
  { id: 409, name: "棒球", calories: 140, category: ExerciseCategory.BALL_SPORTS },
  { id: 410, name: "手球", calories: 165, category: ExerciseCategory.BALL_SPORTS },
  { id: 411, name: "橄榄球", calories: 190, category: ExerciseCategory.BALL_SPORTS },
  { id: 412, name: "台球", calories: 75, category: ExerciseCategory.BALL_SPORTS },
  { id: 413, name: "保龄球", calories: 95, category: ExerciseCategory.BALL_SPORTS },
  { id: 414, name: "曲棍球", calories: 180, category: ExerciseCategory.BALL_SPORTS },
  { id: 415, name: "沙滩排球", calories: 155, category: ExerciseCategory.BALL_SPORTS },

  // 户外与休闲运动 (id: 500-599)
  { id: 501, name: "自行车通勤", calories: 140, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 502, name: "徒步旅行", calories: 150, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 503, name: "轮滑", calories: 160, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 504, name: "滑板", calories: 135, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 505, name: "滑雪", calories: 180, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 506, name: "冲浪", calories: 130, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 507, name: "攀岩", calories: 190, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 508, name: "皮划艇", calories: 145, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 509, name: "骑马", calories: 120, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 510, name: "射箭", calories: 90, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 511, name: "飞盘", calories: 125, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 512, name: "钓鱼", calories: 85, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 513, name: "园艺", calories: 110, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 514, name: "遛狗", calories: 100, category: ExerciseCategory.OUTDOOR_RECREATION },
  { id: 515, name: "站立桨板", calories: 135, category: ExerciseCategory.OUTDOOR_RECREATION },

  // 日常活动与家务 (id: 600-699)
  { id: 601, name: "打扫卫生", calories: 95, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 602, name: "洗车", calories: 110, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 603, name: "搬运物品", calories: 125, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 604, name: "整理房间", calories: 85, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 605, name: "烹饪准备", calories: 70, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 606, name: "洗衣晾晒", calories: 80, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 607, name: "购物", calories: 90, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 608, name: "带孩子玩耍", calories: 120, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 609, name: "修剪草坪", calories: 140, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 610, name: "铲雪", calories: 160, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 611, name: "爬楼梯代替电梯", calories: 100, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 612, name: "站立办公", calories: 65, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 613, name: "步行通勤", calories: 110, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 614, name: "手工DIY", calories: 75, category: ExerciseCategory.DAILY_ACTIVITIES },
  { id: 615, name: "洗窗户", calories: 105, category: ExerciseCategory.DAILY_ACTIVITIES }
];

export function getExercisesByCategory(cat: ExerciseCategory): ExerciseItem[] {
  return EXERCISE_DATABASE.filter(item => item.category === cat);
}
