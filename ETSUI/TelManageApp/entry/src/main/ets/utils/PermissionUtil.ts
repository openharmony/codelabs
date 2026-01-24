//联系人权限工具：请求与检查读写权限，返回是否全部授权。

import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';
import common from '@ohos.app.ability.common';

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