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
