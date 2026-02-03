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

export class MedicineItem {
  id: number;
  name: string;
  category: string;
  expiryDate: string;
  quantity: string;
  usage: string;
  location: string;
  isExpired: boolean;
  daysRemaining: number;
  description: string; // 新增描述字段

  constructor(id: number, name: string, category: string, expiryDate: string, quantity: string, usage: string,
    location: string, description: string = '') {
    this.id = id;
    this.name = name;
    this.category = category;
    this.expiryDate = expiryDate;
    this.quantity = quantity;
    this.usage = usage;
    this.location = location;
    this.description = description;

    // 计算过期逻辑
    const today = new Date();
    const expiry = new Date(expiryDate);
    const diffTime = expiry.getTime() - today.getTime();
    this.daysRemaining = Math.ceil(diffTime / (1000 * 60 * 60 * 24));
    this.isExpired = this.daysRemaining < 0;
  }
}

// 简单的用户类
class User {
  username: string;
  password: string;

  constructor(u: string, p: string) {
    this.username = u;
    this.password = p;
  }
}

export class MockData {
  // 1. 模拟药品数据
  static medicineList: MedicineItem[] = [
    new MedicineItem(1, '999感冒灵', '感冒发烧', '2025-12-01', '2盒', '一日三次', '客厅电视柜', '用于感冒引起的头痛'),
    new MedicineItem(2, '阿莫西林', '消炎抗生素', '2024-11-15', '1盒', '一日两次', '卧室抽屉', '处方药，谨遵医嘱'),
    new MedicineItem(3, '布洛芬缓释胶囊', '止痛退烧', '2026-05-20', '1瓶', '疼痛时服', '药箱上层', '退烧止疼专用'),
    new MedicineItem(4, '健胃消食片', '肠胃护理', '2028-01-22', '3盒', '一日三次', '厨房柜子', '助消化')
  ];
  // 2. 模拟分类列表 (动态的)
  static categories: string[] = [
    '感冒发烧', '消炎抗生素', '肠胃护理', '外伤急救', '慢性病药', '保健维生素'
  ];
  // 3. 模拟用户 (用于登录和改密)
  static currentUser: User = new User('admin', '123456');

  // ================= 药品操作 =================

  static getAllMedicines(): MedicineItem[] {
    return MockData.medicineList;
  }

  static getExpiredOrNearMedicines(): MedicineItem[] {
    return MockData.medicineList.filter(item => item.daysRemaining <= 180 || item.isExpired);
  }

  static getMedicinesByCategory(category: string): MedicineItem[] {
    return MockData.medicineList.filter(item => item.category === category);
  }

  static getMedicineById(id: number): MedicineItem | undefined {
    return MockData.medicineList.find(item => item.id === id);
  }

  static addMedicine(name: string, category: string, date: string, usage: string, location: string) {
    // 生成唯一ID
    const newId = MockData.medicineList.length > 0 ? Math.max(...MockData.medicineList.map(m => m.id)) + 1 : 1;
    MockData.medicineList.push(new MedicineItem(newId, name, category, date, '1盒', usage, location));
  }

  static updateMedicine(id: number, name: string, category: string, date: string, usage: string, location: string) {
    const index = MockData.medicineList.findIndex(item => item.id === id);
    if (index !== -1) {
      // 更新数据
      MockData.medicineList[index] =
        new MedicineItem(id, name, category, date, MockData.medicineList[index].quantity, usage, location);
    }
  }

  static deleteMedicine(id: number) {
    MockData.medicineList = MockData.medicineList.filter(item => item.id !== id);
  }

  // ================= 分类操作 =================

  static addCategory(name: string) {
    if (!MockData.categories.includes(name)) {
      MockData.categories.push(name);
    }
  }

  static getCategories(): string[] {
    return MockData.categories;
  }
}