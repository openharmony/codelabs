import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import type { Permissions, PermissionRequestResult } from '@ohos.abilityAccessCtrl';
import type { BusinessError } from '@ohos.base';
import type { Context } from '@kit.AbilityKit';
import { Logger } from '../utils/Logger';

const MICROPHONE_PERMISSION: Permissions = 'ohos.permission.MICROPHONE';
const WRITE_MEDIA_PERMISSION: Permissions = 'ohos.permission.WRITE_MEDIA';
const READ_MEDIA_PERMISSION: Permissions = 'ohos.permission.READ_MEDIA';

export class PermissionService {
  private atManager = abilityAccessCtrl.createAtManager();

  async requestRecordingPermissions(context: Context): Promise<boolean> {
    const permissions: Permissions[] = [MICROPHONE_PERMISSION, WRITE_MEDIA_PERMISSION, READ_MEDIA_PERMISSION];
    return new Promise((resolve) => {
      try {
        this.atManager.requestPermissionsFromUser(
          context,
          permissions,
          (err: BusinessError<void>, result: PermissionRequestResult) => {
            if (err) {
              Logger.error('Permission request failed', String(err));
              resolve(false);
              return;
            }
            const granted = result.authResults.every((value: number) => value === 0);
            if (!granted) {
              Logger.warn('Permission denied', JSON.stringify(result.authResults));
            }
            resolve(granted);
          }
        );
      } catch (err) {
        Logger.error('Permission request failed', String(err));
        resolve(false);
      }
    });
  }
}

export const permissionService = new PermissionService();
