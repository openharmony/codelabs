/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
