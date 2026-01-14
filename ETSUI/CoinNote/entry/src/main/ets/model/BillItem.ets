export interface BillItem {
  id: number;
  type: string;
  amount: number;
  date: string; // YYYY/M/D
  payType: string;
  tags: string[];
  isReimbursable: boolean;
  isReimbursementReceived?: boolean; // 是否已报销
  isAA: boolean; // 是否AA
  isAaReceived?: boolean; // AA款项是否已收回
  originalAmount?: number; // 原始金额（用于撤销操作）
  aaReceivedAmount?: number; // 已收到的AA金额
  note: string;
  direction: 'expense' | 'income';
}
