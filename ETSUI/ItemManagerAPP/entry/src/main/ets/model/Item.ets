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