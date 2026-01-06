// entry/src/main/ets/common/utils/PermissionUtils.ets
import { abilityAccessCtrl, bundleManager, common, Permissions } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { Logger } from './Logger';

/**
 * 权限请求工具类
 * 封装了 OpenHarmony 的权限检查与请求逻辑
 */
export class PermissionUtils {
  private static logger = new Logger('PermissionUtils');

  /**
   * 检查是否拥有某项权限
   * @param permission 权限名称
   * @returns Promise<boolean>
   */
  public static async checkPermission(permission: Permissions): Promise<boolean> {
    // ❌ Fix: this.logger -> PermissionUtils.logger
    PermissionUtils.logger.info(`Checking permission: ${permission}`);

    let grantStatus: abilityAccessCtrl.GrantStatus;

    try {
      const atManager = abilityAccessCtrl.createAtManager();
      const bundleInfo = await bundleManager.getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
      const appInfo = bundleInfo.appInfo;
      const tokenId = appInfo.accessTokenId;

      grantStatus = await atManager.checkAccessToken(tokenId, permission);

      const isGranted = grantStatus === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED;
      // ❌ Fix: this.logger -> PermissionUtils.logger
      PermissionUtils.logger.info(`Permission ${permission} status: ${isGranted ? 'GRANTED' : 'DENIED'}`);

      return isGranted;
    } catch (error) {
      const err = error as BusinessError;
      // ❌ Fix: this.logger -> PermissionUtils.logger
      PermissionUtils.logger.error(`Check permission failed: ${err.message}`);
      return false;
    }
  }

  /**
   * 请求权限
   * @param context 上下文
   * @param permissions 权限列表
   */
  public static async requestPermissions(context: common.UIAbilityContext, permissions: Array<Permissions>): Promise<boolean> {
    // ❌ Fix: this.logger -> PermissionUtils.logger
    PermissionUtils.logger.info(`Requesting permissions: ${JSON.stringify(permissions)}`);

    try {
      const atManager = abilityAccessCtrl.createAtManager();
      const result = await atManager.requestPermissionsFromUser(context, permissions);

      let allGranted = true;
      result.authResults.forEach((status) => {
        if (status !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
          allGranted = false;
        }
      });

      if (allGranted) {
        // ❌ Fix: this.logger -> PermissionUtils.logger
        PermissionUtils.logger.info('All permissions granted by user.');
      } else {
        // ❌ Fix: this.logger -> PermissionUtils.logger
        PermissionUtils.logger.warn('Some permissions were denied by user.');
      }

      return allGranted;
    } catch (error) {
      const err = error as BusinessError;
      // ❌ Fix: this.logger -> PermissionUtils.logger
      PermissionUtils.logger.error(`Request permissions failed: ${err.message}`);
      return false;
    }
  }
}