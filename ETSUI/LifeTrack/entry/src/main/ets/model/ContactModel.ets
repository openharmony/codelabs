/*
 * Copyright (c) 2026 BIT-Group23
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

// 联系人数据模型
export interface ContactsItem {
  id: number;           // 主键ID
  name: string;        // 姓名
  phone: string;       // 电话号码
  initial: string;     // 姓名的首字母，用于分组
}

// 插入联系人时使用的类型
export interface InsertContactData {
  name: string;        // 姓名
  phone: string;       // 电话号码
  initial: string;     // 姓名的首字母
}

// 联系人分组模型（用于UI展示）
export interface ContactGroup {
  initial: string;
  contacts: ContactsItem[];
}

// 操作结果类型
export interface OperationResult {
  success: boolean;
  message?: string;
  data?: OperationData;
}

// 操作结果的数据类型定义
export interface OperationData {
  id?: number;                 // 插入时返回的ID
  rowsUpdated?: number;        // 更新行数
  rowsDeleted?: number;        // 删除行数
  successCount?: number;       // 批量操作成功数
  failCount?: number;          // 批量操作失败数
  failMessages?: string[];     // 失败信息
}

// 批量导入的联系人类型
export interface BatchContactData {
  name: string;        // 姓名
  phone: string;       // 电话号码
  initial?: string;    // 姓名的首字母（可选）
}