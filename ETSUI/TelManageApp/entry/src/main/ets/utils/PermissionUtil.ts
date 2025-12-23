<<<<<<< HEAD
=======
/** 联系人权限工具：请求与检查读写权限，返回是否全部授权。*/
>>>>>>> d22acc0b635404fda9e57f547c003077af4e3bb9
import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';
import common from '@ohos.app.ability.common';

export class PermissionUtil {
<<<<<<< HEAD
=======
  /** 请求联系人读写权限。
   * @param context UIAbility 上下文
   * @returns 全部授权返回 true，否则 false
   */
>>>>>>> d22acc0b635404fda9e57f547c003077af4e3bb9
  // 申请联系人读写权限
  static async requestContactPermissions(context: common.UIAbilityContext): Promise<boolean> {
    const permissions: Array<Permissions> = [
      'ohos.permission.READ_CONTACTS',
      'ohos.permission.WRITE_CONTACTS'
    ];

    try {
<<<<<<< HEAD
      const atManager = abilityAccessCtrl.createAtManager();
      const result = await atManager.requestPermissionsFromUser(context, permissions);
=======
      // 创建权限管理器并发起请求
      const atManager = abilityAccessCtrl.createAtManager();
      const result = await atManager.requestPermissionsFromUser(context, permissions);
      // 0 表示授权通过
>>>>>>> d22acc0b635404fda9e57f547c003077af4e3bb9
      return result.authResult.every(granted => granted === 0);
    } catch (err) {
      console.error('Permission request failed:', err);
      return false;
    }
  }

<<<<<<< HEAD
=======
  /** 检查一组权限是否已授权。
   * @returns 全部授权返回 true，否则 false
   */
>>>>>>> d22acc0b635404fda9e57f547c003077af4e3bb9
  // 检查权限状态
  static async checkPermissions(context: common.UIAbilityContext, permissions: Array<Permissions>): Promise<boolean> {
    try {
      const atManager = abilityAccessCtrl.createAtManager();
      for (const permission of permissions) {
        const result = await atManager.checkAccessToken(context, permission);
<<<<<<< HEAD
        if (result !== 0) { // 0表示有权限
=======
        if (result !== 0) { // 非 0 表示未授权
>>>>>>> d22acc0b635404fda9e57f547c003077af4e3bb9
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