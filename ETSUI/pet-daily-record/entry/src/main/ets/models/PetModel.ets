/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * 宠物基础信息模型
 */
export interface Pet {
  id: number; // 数据库自增ID
  name: string; // 宠物名字
  species: string; // 品种（猫、狗等）
  gender: string; // 性别
  birthday: string; // 生日 (YYYY-MM-DD)
  age: number; // 年龄（逻辑计算得出）
  photoPath?: string; // 照片路径（可选）
}

/**
 * 宠物行为记录模型 (对应 RecordTable)
 */
export interface PetRecord {
  id: number;
  petId: number; // 关联的宠物ID
  type: string; // 类型：喂食、洗澡、疫苗等
  date: string; // 记录日期 (YYYY-MM-DD)
  content: string; // 详细描述
}

/**
 * 宠物花销模型 (对应 CostTable)
 */
export interface PetCost {
  id: number;
  petId: number; // 关联的宠物ID
  amount: number; // 金额
  category: string; // 类别：伙食、医疗、玩具等
  date: string; // 发生日期
}