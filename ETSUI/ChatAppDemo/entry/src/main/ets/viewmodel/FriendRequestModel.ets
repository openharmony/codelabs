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

export interface FriendRequestModel {
  id?: number;
  fromPhone: string; // 申请人手机号
  toPhone: string; // 被申请人手机号
  fromName: string; // 申请人昵称
  message: string; // 验证消息
  status: number; // 0:待处理, 1:已通过, 2:已拒绝
}