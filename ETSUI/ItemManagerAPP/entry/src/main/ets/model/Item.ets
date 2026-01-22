export type ItemUseStatus = "在用" | "闲置" | "已丢弃";

export interface Item {
  id: number;
  name: string;
  category: string;
  buyDate: string;
  price: number;
  location: string;
  note: string;
  useStatus: ItemUseStatus;
  isLent?: boolean;
  lendTo?: string;
  returnDate?: string;
  isPrecious?: boolean;
  maintenanceDate?: string;
  imagePath?: string;
}

// 默认分类列表
export const DEFAULT_CATEGORY_OPTIONS = [
  "数码产品", "衣物", "书籍", "厨具", "家具", "美妆", "食品", "其他"
];