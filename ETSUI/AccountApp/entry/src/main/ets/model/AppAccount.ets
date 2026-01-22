/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import relationalStore from '@ohos.data.relationalStore';

/**
 * AppAccount.ets
 * 应用账号实体类
 * 用于存储用户记录的第三方应用账号信息（如微信、QQ的账号密码）
 */
export default class AppAccount {
  private id: number;
  private platformName: string; // 平台名称，如 "微信", "淘宝"
  private accountName: string;  // 账号/用户名
  private password: string;     // 密码
  private ownerId: number;      // 归属人ID (关联 User 表的 id)

  constructor(platformName: string, accountName: string, password: string, ownerId: number) {
    this.id = 0;
    this.platformName = platformName;
    this.accountName = accountName;
    this.password = password;
    this.ownerId = ownerId;
  }

  // --- Getter / Setter ---
  public getId(): number { return this.id; }
  public setId(id: number): void { this.id = id; }

  public getPlatformName(): string { return this.platformName; }
  public getAccountName(): string { return this.accountName; }
  public getPassword(): string { return this.password; }
  public getOwnerId(): number { return this.ownerId; }

  // 转换为数据库存储对象
  public toValuesBucket(): relationalStore.ValuesBucket {
    return {
      'platform_name': this.platformName,
      'account_name': this.accountName,
      'password': this.password, // 实际生产中应加密存储
      'owner_id': this.ownerId
    };
  }
}