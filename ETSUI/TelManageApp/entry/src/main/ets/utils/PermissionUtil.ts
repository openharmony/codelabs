/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
//联系人权限工具：请求与检查读写权限，返回是否全部授权。

import { abilityAccessCtrl, common, Permissions } from '@kit.AbilityKit';

export class PermissionUtil {
  /** 请求联系人读写权限。
   * @param context UIAbility 上下文
   * @returns 全部授权返回 true，否则 false
   */
  // 申请联系人读写权限
  static async requestContactPermissions(context: common.UIAbilityContext): Promise<boolean> {
    const permissions: Array<Permissions> = [
      'ohos.permission.READ_CONTACTS',
      'ohos.permission.WRITE_CONTACTS'
    ];

    try {
      // 创建权限管理器并发起请求
      const atManager = abilityAccessCtrl.createAtManager();
      const result = await atManager.requestPermissionsFromUser(context, permissions);
      // 0 表示授权通过
      return result.authResults.every(granted => granted === 0);
    } catch (err) {
      console.error('Permission request failed:', err);
      return false;
    }
  }

  /** 检查一组权限是否已授权。
   * @returns 全部授权返回 true，否则 false
   */

  // 检查权限状态
  static async checkPermissions(context: common.UIAbilityContext, permissions: Array<Permissions>): Promise<boolean> {
    try {
      const atManager = abilityAccessCtrl.createAtManager();
      // 获取应用 tokenID
      const tokenId = context.applicationInfo.accessTokenId;

      for (const permission of permissions) {
        const result = await atManager.checkAccessToken(tokenId, permission);
        if (result !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
          return false;
        }
      }
      return true;
    } catch (err) {
      console.error('Permission check failed:', err);
      return false;
    }
  }
}