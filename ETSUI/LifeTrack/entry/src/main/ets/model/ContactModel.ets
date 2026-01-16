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