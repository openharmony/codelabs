// entry/src/main/ets/model/DietModel.ets

// 1. 定义食物类别枚举
export enum FoodCategory {
  STAPLE = '主食',
  MEAT = '肉蛋奶',
  VEGETABLE = '蔬菜',
  FRUIT = '水果',
  SNACK = '零食',
  DRINK = '饮料',
  CUSTOM = '自定义' // 新增自定义类型
}

// 2. 定义餐别枚举
export enum MealTime {
  BREAKFAST = '早餐',
  LUNCH = '午餐',
  DINNER = '晚餐',
  SNACK = '加餐'
}

// 3. 食物对象接口
export interface FoodItem {
  id: number;
  name: string;
  calories: number; // kcal per unit
  protein: number;  // 蛋白质 (g)
  fat: number;      // 脂肪 (g)
  carbs: number;    // 碳水 (g)
  category: FoodCategory;
  unit: string;     // 单位描述，如 '碗', '个', '100g'
}

// 4. 饮食记录接口
export interface DietRecord {
  id: string;
  foodId: number;
  foodName: string;
  mealTime: MealTime;
  portion: number;
  totalCalories: number;
  date: string;
}

// 5. 庞大的本地食物库 (扩充至 ~150 行)
export const FOOD_DATABASE: FoodItem[] = [
  // --- 主食类 (Staple) ---
  { id: 101, name: '米饭(蒸)', calories: 116, protein: 2.6, fat: 0.3, carbs: 25.9, category: FoodCategory.STAPLE, unit: '100g' },
  { id: 102, name: '馒头', calories: 223, protein: 7.0, fat: 1.1, carbs: 47.0, category: FoodCategory.STAPLE, unit: '个(100g)' },
  { id: 103, name: '小米粥', calories: 46, protein: 1.4, fat: 0.7, carbs: 8.4, category: FoodCategory.STAPLE, unit: '碗' },
  { id: 104, name: '全麦面包', calories: 246, protein: 8.5, fat: 3.2, carbs: 46.5, category: FoodCategory.STAPLE, unit: '片' },
  { id: 105, name: '煮玉米', calories: 112, protein: 4.0, fat: 1.2, carbs: 22.8, category: FoodCategory.STAPLE, unit: '根' },
  { id: 106, name: '意大利面', calories: 131, protein: 5.0, fat: 0.5, carbs: 25.0, category: FoodCategory.STAPLE, unit: '盘' },
  { id: 107, name: '红薯(蒸)', calories: 90, protein: 1.6, fat: 0.2, carbs: 20.7, category: FoodCategory.STAPLE, unit: '个' },
  { id: 108, name: '白吐司', calories: 278, protein: 9.0, fat: 3.0, carbs: 55.0, category: FoodCategory.STAPLE, unit: '片' },
  { id: 109, name: '油条', calories: 388, protein: 6.0, fat: 17.0, carbs: 51.0, category: FoodCategory.STAPLE, unit: '根' },
  { id: 110, name: '燕麦片(冲)', calories: 68, protein: 2.4, fat: 1.4, carbs: 12.0, category: FoodCategory.STAPLE, unit: '碗' },
  { id: 111, name: '水饺(猪肉)', calories: 240, protein: 8.0, fat: 10.0, carbs: 25.0, category: FoodCategory.STAPLE, unit: '份(10个)' },
  { id: 112, name: '肉包子', calories: 250, protein: 7.0, fat: 9.0, carbs: 35.0, category: FoodCategory.STAPLE, unit: '个' },
  { id: 113, name: '汉堡包', calories: 450, protein: 20.0, fat: 22.0, carbs: 40.0, category: FoodCategory.STAPLE, unit: '个' },
  { id: 114, name: '披萨', calories: 260, protein: 11.0, fat: 10.0, carbs: 30.0, category: FoodCategory.STAPLE, unit: '片' },
  { id: 115, name: '方便面', calories: 470, protein: 9.0, fat: 20.0, carbs: 60.0, category: FoodCategory.STAPLE, unit: '包' },
  { id: 116, name: '三明治', calories: 300, protein: 12.0, fat: 10.0, carbs: 35.0, category: FoodCategory.STAPLE, unit: '个' },
  { id: 117, name: '寿司', calories: 50, protein: 2.0, fat: 0.5, carbs: 10.0, category: FoodCategory.STAPLE, unit: '个' },
  { id: 118, name: '炒饭', calories: 650, protein: 15.0, fat: 25.0, carbs: 90.0, category: FoodCategory.STAPLE, unit: '盘' },

  // --- 肉蛋奶 (Meat/Dairy) ---
  { id: 201, name: '煮鸡蛋', calories: 144, protein: 13.3, fat: 8.8, carbs: 2.8, category: FoodCategory.MEAT, unit: '个' },
  { id: 202, name: '煎鸡胸肉', calories: 133, protein: 19.4, fat: 5.0, carbs: 2.5, category: FoodCategory.MEAT, unit: '100g' },
  { id: 203, name: '纯牛奶', calories: 65, protein: 3.2, fat: 3.6, carbs: 4.8, category: FoodCategory.MEAT, unit: '盒(250ml)' },
  { id: 204, name: '红烧肉', calories: 470, protein: 10.0, fat: 45.0, carbs: 3.0, category: FoodCategory.MEAT, unit: '份' },
  { id: 205, name: '三文鱼刺身', calories: 146, protein: 20.0, fat: 6.0, carbs: 0.0, category: FoodCategory.MEAT, unit: '100g' },
  { id: 206, name: '瘦牛肉', calories: 106, protein: 20.2, fat: 2.3, carbs: 0.0, category: FoodCategory.MEAT, unit: '100g' },
  { id: 207, name: '荷包蛋(煎)', calories: 196, protein: 14.0, fat: 15.0, carbs: 1.0, category: FoodCategory.MEAT, unit: '个' },
  { id: 208, name: '烤肠', calories: 300, protein: 12.0, fat: 25.0, carbs: 5.0, category: FoodCategory.MEAT, unit: '根' },
  { id: 209, name: '酸奶(无糖)', calories: 72, protein: 3.5, fat: 3.0, carbs: 5.0, category: FoodCategory.MEAT, unit: '杯' },
  { id: 210, name: '豆浆(甜)', calories: 50, protein: 3.0, fat: 1.5, carbs: 6.0, category: FoodCategory.MEAT, unit: '杯' },
  { id: 211, name: '豆腐', calories: 80, protein: 8.0, fat: 4.0, carbs: 2.0, category: FoodCategory.MEAT, unit: '100g' },
  { id: 212, name: '炸鸡腿', calories: 260, protein: 18.0, fat: 15.0, carbs: 10.0, category: FoodCategory.MEAT, unit: '个' },
  { id: 213, name: '酱排骨', calories: 350, protein: 20.0, fat: 25.0, carbs: 8.0, category: FoodCategory.MEAT, unit: '份' },
  { id: 214, name: '虾仁', calories: 85, protein: 20.0, fat: 0.5, carbs: 0.0, category: FoodCategory.MEAT, unit: '100g' },
  { id: 215, name: '羊肉串', calories: 200, protein: 15.0, fat: 15.0, carbs: 0.0, category: FoodCategory.MEAT, unit: '串' },

  // --- 蔬果类 (Vegetable/Fruit) ---
  { id: 301, name: '苹果', calories: 53, protein: 0.4, fat: 0.2, carbs: 13.7, category: FoodCategory.FRUIT, unit: '个' },
  { id: 302, name: '香蕉', calories: 93, protein: 1.4, fat: 0.2, carbs: 22.0, category: FoodCategory.FRUIT, unit: '根' },
  { id: 303, name: '西红柿', calories: 18, protein: 0.9, fat: 0.2, carbs: 3.5, category: FoodCategory.VEGETABLE, unit: '个' },
  { id: 304, name: '黄瓜', calories: 16, protein: 0.8, fat: 0.2, carbs: 2.9, category: FoodCategory.VEGETABLE, unit: '根' },
  { id: 305, name: '西兰花', calories: 36, protein: 4.1, fat: 0.6, carbs: 4.3, category: FoodCategory.VEGETABLE, unit: '盘' },
  { id: 306, name: '生菜', calories: 15, protein: 1.3, fat: 0.2, carbs: 2.0, category: FoodCategory.VEGETABLE, unit: '份' },
  { id: 307, name: '胡萝卜', calories: 39, protein: 1.0, fat: 0.2, carbs: 8.0, category: FoodCategory.VEGETABLE, unit: '根' },
  { id: 308, name: '菠菜', calories: 23, protein: 2.9, fat: 0.4, carbs: 3.6, category: FoodCategory.VEGETABLE, unit: '份' },
  { id: 309, name: '草莓', calories: 32, protein: 0.7, fat: 0.3, carbs: 7.7, category: FoodCategory.FRUIT, unit: '100g' },
  { id: 310, name: '葡萄', calories: 69, protein: 0.7, fat: 0.2, carbs: 18.0, category: FoodCategory.FRUIT, unit: '100g' },
  { id: 311, name: '西瓜', calories: 31, protein: 0.6, fat: 0.1, carbs: 6.0, category: FoodCategory.FRUIT, unit: '片' },
  { id: 312, name: '橙子', calories: 47, protein: 1.0, fat: 0.2, carbs: 11.0, category: FoodCategory.FRUIT, unit: '个' },
  { id: 313, name: '桃子', calories: 42, protein: 0.9, fat: 0.1, carbs: 10.0, category: FoodCategory.FRUIT, unit: '个' },
  { id: 314, name: '梨', calories: 51, protein: 0.4, fat: 0.2, carbs: 13.0, category: FoodCategory.FRUIT, unit: '个' },
  { id: 315, name: '蓝莓', calories: 57, protein: 0.7, fat: 0.3, carbs: 14.0, category: FoodCategory.FRUIT, unit: '盒' },
  { id: 316, name: '芒果', calories: 60, protein: 0.8, fat: 0.4, carbs: 15.0, category: FoodCategory.FRUIT, unit: '个' },

  // --- 饮料 (Drink) ---
  { id: 401, name: '可乐', calories: 140, protein: 0.0, fat: 0.0, carbs: 35.0, category: FoodCategory.DRINK, unit: '罐' },
  { id: 402, name: '零度可乐', calories: 0, protein: 0.0, fat: 0.0, carbs: 0.0, category: FoodCategory.DRINK, unit: '罐' },
  { id: 403, name: '美式咖啡', calories: 5, protein: 0.2, fat: 0.0, carbs: 1.0, category: FoodCategory.DRINK, unit: '杯' },
  { id: 404, name: '拿铁(全脂)', calories: 150, protein: 8.0, fat: 6.0, carbs: 12.0, category: FoodCategory.DRINK, unit: '杯' },
  { id: 405, name: '珍珠奶茶', calories: 450, protein: 2.0, fat: 15.0, carbs: 70.0, category: FoodCategory.DRINK, unit: '杯' },
  { id: 406, name: '橙汁', calories: 110, protein: 2.0, fat: 0.0, carbs: 26.0, category: FoodCategory.DRINK, unit: '杯' },
  { id: 407, name: '啤酒', calories: 150, protein: 1.0, fat: 0.0, carbs: 12.0, category: FoodCategory.DRINK, unit: '罐' },
  { id: 408, name: '红酒', calories: 85, protein: 0.0, fat: 0.0, carbs: 3.0, category: FoodCategory.DRINK, unit: '杯' },
  { id: 409, name: '矿泉水', calories: 0, protein: 0.0, fat: 0.0, carbs: 0.0, category: FoodCategory.DRINK, unit: '瓶' },
  { id: 410, name: '运动饮料', calories: 80, protein: 0.0, fat: 0.0, carbs: 20.0, category: FoodCategory.DRINK, unit: '瓶' },

  // --- 零食 (Snack) ---
  { id: 501, name: '薯片', calories: 540, protein: 7.0, fat: 35.0, carbs: 50.0, category: FoodCategory.SNACK, unit: '包' },
  { id: 502, name: '黑巧克力', calories: 150, protein: 2.0, fat: 10.0, carbs: 15.0, category: FoodCategory.SNACK, unit: '块' },
  { id: 503, name: '冰淇淋', calories: 200, protein: 4.0, fat: 12.0, carbs: 25.0, category: FoodCategory.SNACK, unit: '球' },
  { id: 504, name: '曲奇饼干', calories: 50, protein: 0.5, fat: 2.5, carbs: 6.0, category: FoodCategory.SNACK, unit: '块' },
  { id: 505, name: '坚果(混合)', calories: 180, protein: 6.0, fat: 16.0, carbs: 5.0, category: FoodCategory.SNACK, unit: '把' },
  { id: 506, name: '爆米花', calories: 300, protein: 5.0, fat: 15.0, carbs: 35.0, category: FoodCategory.SNACK, unit: '桶' },
  { id: 507, name: '果冻', calories: 60, protein: 0.0, fat: 0.0, carbs: 15.0, category: FoodCategory.SNACK, unit: '个' },
  { id: 508, name: '辣条', calories: 300, protein: 8.0, fat: 20.0, carbs: 30.0, category: FoodCategory.SNACK, unit: '包' },
  { id: 509, name: '蛋糕', calories: 350, protein: 5.0, fat: 18.0, carbs: 45.0, category: FoodCategory.SNACK, unit: '块' },
  { id: 510, name: '能量棒', calories: 200, protein: 10.0, fat: 8.0, carbs: 25.0, category: FoodCategory.SNACK, unit: '根' },
];

export function getFoodsByCategory(cat: FoodCategory): FoodItem[] {
  return FOOD_DATABASE.filter(item => item.category === cat);
}